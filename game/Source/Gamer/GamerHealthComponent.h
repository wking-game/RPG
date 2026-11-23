#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GamerSaveable.h"
#include "UObject/Interface.h"
#include "GamerHealthComponent.generated.h"

class AGamerGrave;

/**
 * One portion of damage (decision 2026-11-16): who or what caused it, how much, what kind. Animals, falling, cold and later
 * the combat system all use this, so there is one door for damage. Types are plain names ("fall", "animal", "cold", "debug" ...).
 */
struct FGamerDamageInfo
{
	float Amount = 0.f;
	FName Type;
	TWeakObjectPtr<AActor> Source;
	/** Hit zone for the armor (0 head, 1 body, 2 limbs, like EGamerHitZone). -1 = no zone: armor does not apply (fall, cold, ...). */
	int32 Zone = -1;
	/** Resistance kind (heat, cold, storm, poison) that lowers the damage; none = no resistance applies. */
	FName Resist;
};

UINTERFACE(MinimalAPI)
class UGamerDamageable : public UInterface
{
	GENERATED_BODY()
};

/** Anything that can take damage from the general damage door (the player now, animals and villagers later). */
class GAMER_API IGamerDamageable
{
	GENERATED_BODY()

public:
	/** Server only. */
	virtual void ReceiveGamerDamage(const FGamerDamageInfo& Info) = 0;
};

/**
 * Hit points of a player (decisions 2026-11-16 "Spielertod" and "Heilung"). Server-authoritative; the hit points are
 * replicated to the owner, the dead state to everybody; saved per player (IGamerSaveable, key "health"). Values in
 * `Content/Data/health.json`. Max hit points = base + the "Gesundheit" buff of the meals (UGamerBuffComponent).
 * Healing is slow and only while fed (a meal is active); the "Regeneration" buff adds to it. Dying follows the world option
 * "Tod": none (lose nothing), grave (the inventory lies in a grave at the place of death), hardcore (everything lost).
 * Getting up again: at the bed the player built last (a straw bed), else at the start point. A dead player blocks nobody
 * (invisible, no collision). Villagers get no hit points yet (the interface IGamerDamageable is the hook).
 */
UCLASS(ClassGroup = (Gamer), meta = (BlueprintSpawnableComponent))
class GAMER_API UGamerHealthComponent : public UActorComponent, public IGamerSaveable, public IGamerDamageable
{
	GENERATED_BODY()

public:
	UGamerHealthComponent();

	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// IGamerSaveable
	virtual FString GetSaveKey() const override { return TEXT("health"); }
	virtual void WriteSaveState(FGamerSaveRecord& Out) const override;
	virtual void ReadSaveState(const FGamerSaveRecord& In) override;

	// IGamerDamageable
	virtual void ReceiveGamerDamage(const FGamerDamageInfo& Info) override { TakeDamage(Info); }

	float GetHP() const { return HP; }
	float GetMaxHP() const;
	float GetFraction() const { return GetMaxHP() > 0.f ? FMath::Clamp(HP / GetMaxHP(), 0.f, 1.f) : 0.f; }
	bool IsDead() const { return bDead; }
	/** True while the player stands at the bed it respawns at (a finished bed near the saved spawn point). */
	bool IsNearOwnBed() const;
	/** Knocked down (co-op): 15 s, teammates can revive. */
	bool IsDowned() const { return bDowned; }
	/** Dead or downed: cannot act. */
	bool IsIncapacitated() const { return bDead || bDowned; }
	float GetDownedLeft() const { return DownedLeft; }
	/** The reviver's progress 0..1 while holding E at a downed teammate. */
	float GetReviveProgress() const { return ReviveProgress; }
	float GetReviveHoldSeconds() const { return ReviveHoldSeconds; }
	/** The nearest downed teammate within reach (local, for the key and the HUD), or null. */
	AActor* FindDownedNearby() const;
	/** True if this grave belongs to this player (for the grave marker; clients go by the figure's name). */
	bool IsMyGrave(const AGamerGrave* Grave) const;
	/** Seconds until "Aufstehen" works after dying (client estimate from the replicated state). */
	float GetRespawnWait() const;
	/** True if fed, i.e. at least one meal is active (the regeneration condition). */
	bool IsFed() const;

	// --- Server ---

	/** The general damage door. Ignored while dead. */
	void TakeDamage(const FGamerDamageInfo& Info);
	/** Called by the character when it lands: damages by the height fallen (health.json "fall"). */
	void NotifyLanded(float FallHeightCm);
	/** Records where a straw bed was built (the next respawn point). */
	void SetSpawnPoint(const FVector& Location);
	/** Called when this player built something: a straw bed becomes the respawn point. */
	void NoteBuilt(FName BuildableId, const FVector& Location);

	/** The owning client asks to get up again (works after the minimum delay). */
	UFUNCTION(Server, Reliable)
	void ServerRequestRespawn();
	/** The owning client holds / lets go of E at a downed teammate (server counts the hold). */
	UFUNCTION(Server, Reliable)
	void ServerSetReviving(AActor* TargetPawn, bool bOn);
	/** The owning client opens the grave next to it. */
	UFUNCTION(Server, Reliable)
	void ServerCollectGrave(AGamerGrave* Grave);

	/** The grave close enough to open (local, for the hint and the key), or null. */
	AGamerGrave* FindNearbyGrave() const;
	/** Whether this player may open that grave now (the owner always; others after the protection time). */
	bool MayOpenGrave(const AGamerGrave* Grave) const;
	/** Stable id of the player for graves: the save id, else the session id. */
	FString GetPlayerKey() const;

	// --- Tests only ---

	UFUNCTION(Server, Reliable)
	void ServerDebugDamage(float Amount, FName Type);
	/** Tests only: a hit on a zone (0 head, 1 body, 2 limbs; -1 none) with an optional resistance kind. */
	UFUNCTION(Server, Reliable)
	void ServerDebugHit(float Amount, int32 Zone, FName ResistKind);
	UFUNCTION(Server, Reliable)
	void ServerDebugFall(float HeightCm);
	UFUNCTION(Server, Reliable)
	void ServerDebugRespawnNow();
	/** Tests only: gives or takes the "Heilende Hände" ability (there is no learning system yet). */
	UFUNCTION(Server, Reliable)
	void ServerDebugHealingHands(bool bOn);
	/** Tests only: makes the daily limits (last breath, healing hands) available again. */
	UFUNCTION(Server, Reliable)
	void ServerDebugNewDay();
	FString DescribeState() const;

private:
	void LoadSettings();
	/** Damage after worn armor (zone) and resistance (kind); unchanged without zone and kind. */
	float ReduceDamage(const FGamerDamageInfo& Info) const;
	void Die(const FGamerDamageInfo& Info);
	void Respawn();
	void GoDown(const FGamerDamageInfo& Info);
	void Revive(float HpFraction, const TCHAR* Why);
	bool TryLastBreath();
	bool AnyOtherAlivePlayer() const;
	int32 DayNow() const;
	bool HasHealingHands() const;

	bool bLoaded = false;
	float BaseHP = 100.f;
	float RegenPerSecond = 0.5f;
	bool bRegenNeedsFed = true;
	float FallMinHeightCm = 400.f;
	float FallDamagePer100 = 12.f;
	float MinRespawnDelay = 2.5f;
	float RespawnHealFraction = 1.f;
	float GraveOthersAfter = 120.f;
	float GraveReachCm = 300.f;
	FName BedBuilding = TEXT("building_bed");
	float BedCheckCm = 300.f;
	bool bDownedEnabled = true;
	float DownedDuration = 15.f;
	float ReviveHoldSeconds = 3.f;
	float ReviveHpFraction = 0.3f;
	float ReviveReachCm = 250.f;
	TArray<FName> LastBreathItems;
	bool bLastBreathHotbarOnly = true;
	float LastBreathHpFraction = 0.25f;
	FName HealingHandsAbility = TEXT("ability_healing_hands");
	float GameDaySeconds = 1800.f;

	/** Hit points: owner only (the others do not need them). */
	UPROPERTY(Replicated)
	float HP = 100.f;

	UPROPERTY(Replicated)
	bool bDead = false;

	/** Knocked down (co-op, 15 s): replicated to everybody so teammates see who to help. */
	UPROPERTY(Replicated)
	bool bDowned = false;

	UPROPERTY(Replicated)
	float DownedLeft = 0.f;

	/** Progress of the revive this player is doing (owner only). */
	UPROPERTY(Replicated)
	float ReviveProgress = 0.f;

	// Server bookkeeping
	TWeakObjectPtr<AActor> ReviveTarget;
	int32 LastBreathDay = -1;
	int32 HealingHandsDay = -1;
	bool bDebugHealingHands = false;

	/** Server world time of the death, replicated as seconds ago would be unstable; the HUD only needs "dead since a while". */
	UPROPERTY(Replicated)
	float DeadSeconds = 0.f;

	FVector SpawnPoint = FVector::ZeroVector;
	bool bHasSpawnPoint = false;
	FVector StartPoint = FVector::ZeroVector;
	bool bHasStartPoint = false;
	double DeathTime = 0.0;
	float FallApex = 0.f;
	/** A new player starts at full hit points once the character (and with it the toughness) is known; a loaded save keeps its own value. */
	bool bHpInitialized = false;
};
