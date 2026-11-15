#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GamerSaveable.h"
#include "GamerListenComponent.generated.h"

/** One level of the magic sense (listen.json "levels"). */
struct FGamerListenLevel
{
	int32 Level = 1;
	FString Name;
	float MinEP = 0.f;
	float RangeCm = 1000.f;
	float WaveSeconds = 5.f;
	float VisibleSeconds = 15.f;
	float CooldownSeconds = 15.f;
	/** How clearly the silhouette shows (0..1). */
	float Strength = 0.3f;
	/** "flicker", "direction_coarse", "name_amount", "hidden_mark", "rare" (each level also shows what the lower ones show). */
	FString Shows;
};

/** A magical thing found by listening. */
struct FGamerListenFind
{
	TWeakObjectPtr<AActor> Actor;
	FVector Location = FVector::ZeroVector;
	float Distance = 0.f;
	/** World time when the wave reached it (0 = not yet). */
	double RevealTime = 0.0;
	FString Kind;
	FString Coarse;
	FString Name;
	int32 Amount = 1;
	bool bHidden = false;
	bool bRare = false;
};

/**
 * Listening for magic, the Gezeitenhall (decision 2026-11-15: "Umschauen und Lauschen getrennt", "Magie-Gespür: Stufen und Boni").
 * Passive: a magical thing within range makes the screen edge shimmer turquoise, stronger the nearer (hot/cold) and calls
 * the hum hook (no sounds exist yet). Active (own key, the figure stands still): a slow turquoise sine ring runs out, the
 * finds stay visible as silhouettes; moving breaks it off. The level (1..5) rises with the experience (EP) of this player
 * (server counts, replicated to the owner, saved); the seer starts on level 2; the amulet and special times add a level
 * (at most 5); a village seer adds +25 % EP. Co-op: shared finds give teammates 50 % EP, listening together lifts everybody
 * to at least the best level minus 1 with +25 % range. Same code for solo and co-op (rule 2); group rules just do not apply
 * with one player. All values: `Content/Data/listen.json`. Magical things are actors with the tag "GamerMagic" (none exist yet).
 */
UCLASS(ClassGroup = (Gamer), meta = (BlueprintSpawnableComponent))
class GAMER_API UGamerListenComponent : public UActorComponent, public IGamerSaveable
{
	GENERATED_BODY()

public:
	UGamerListenComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// IGamerSaveable: only the experience of the player is saved.
	virtual FString GetSaveKey() const override { return TEXT("listen"); }
	virtual void WriteSaveState(FGamerSaveRecord& Out) const override;
	virtual void ReadSaveState(const FGamerSaveRecord& In) override;

	// --- Experience and level ---

	float GetEP() const { return EP; }
	/** Server: adds raw experience (no multipliers). */
	void AddEP(float Amount);
	/**
	 * Server: a magical event of this player (new find, seaglass harvest, stone circle, tide event; values in listen.json).
	 * Applies the world's "Gespür-Tempo" and the village seer bonus; teammates within the share radius get a part of it.
	 * TargetId is the id of the find: experience counts ONCE per find and player (the server remembers which finds a player
	 * already counted, saved with the player), so one cannot farm a single target. Without an id (events that are not a find) it always counts.
	 * The magical objects will call this; tests use the debug RPC.
	 */
	void AwardSource(FName Source, const FVector& Where, FName TargetId = NAME_None);
	/** Tests only: lets a client ask the server for a source award at its own place. */
	UFUNCTION(Server, Reliable)
	void ServerDebugAward(FName Source, FName TargetId);
	/** Server: true the first time this player counts this find, false if it was counted before (then nothing is awarded). */
	bool TryCountTarget(FName TargetId);

	/** Own level 1..5 (experience, origin start level, bonuses). OutReason explains it for logs and tests. */
	int32 GetLevel(FString* OutReason = nullptr) const;
	/** The level in use now: the own level, or higher while a group listen lifts it. */
	int32 GetEffectiveLevel() const;
	const FGamerListenLevel* GetLevelDef(int32 Level) const;
	/** Range of the effective level, with the group bonus. */
	float GetRangeCm() const;

	// --- Active listening ---

	/** Local player: start listening, or say why not (cooldown, moving). */
	bool TryListen();
	bool IsListening() const;
	bool IsShowing() const;
	float GetCooldownLeft() const;
	float GetAgeSeconds() const;
	float GetWaveSeconds() const;
	float GetWaveRadiusCm() const;
	FVector GetCenter() const { return Center; }
	const TArray<FGamerListenFind>& GetFinds() const { return Finds; }
	/** Seconds a find stays visible after the wave reached it (level, group bonus). */
	float GetVisibleSeconds() const;
	/** Whether this level lets the player mark finds for everybody (level 4 up); prepared, there are no finds to mark yet. */
	bool CanMark() const;

	// --- Passive signal ---

	/** 0..1, how close the nearest magical thing in range is (1 = right here). */
	float GetProximity() const { return Proximity; }
	/** Strongest alpha of the edge shimmer (listen.json "passive"). */
	float GetMaxEdgeAlpha() const { return MaxEdgeAlpha; }
	/** Audio hook for the hum (there are no sounds yet): called with the intensity 0..1 while a magical thing is in range. */
	DECLARE_MULTICAST_DELEGATE_OneParam(FOnMagicHum, float);
	FOnMagicHum OnMagicHum;

	// --- Group listening (co-op) ---

	/** Server side of "listen together": works out the lift for everybody who listened within the window and tells them. */
	UFUNCTION(Server, Reliable)
	void ServerStartListen();
	UFUNCTION(Client, Reliable)
	void ClientListenResult(int32 EffectiveLevel, float InRangeMul);

	// --- Tests only ---

	void SetDebugAge(float Seconds) { DebugAge = Seconds; }
	void SetDebugAmulet(bool bOn) { bDebugAmulet = bOn; }
	void SetDebugNoOrigin(bool bOn) { bDebugNoOrigin = bOn; }
	/** A fake magical thing (a sphere tagged like real ones will be). Local only. */
	void AddDebugTarget(const FVector& Location, const FString& Kind, const FString& Name, bool bHidden, bool bRare);
	FString DescribeState() const;

private:
	void LoadSettings();
	bool EvaluateCondition(const FString& Condition) const;
	float EPMultiplier() const;
	bool IsEligible(const AActor* Actor, int32 Level) const;
	void BuildFinds();
	void UpdateFinds();
	void UpdatePassive();
	void UpdateOutline();
	void ClearOutline();
	void EndListening();
	static void SetMagicStencil(AActor* Actor, bool bOn);

	bool bLoaded = false;
	TArray<FGamerListenLevel> Levels;
	float WaveSpeedMs = 2.f;
	TMap<FName, int32> StartLevelByOrigin;
	TMap<FName, int32> BonusItems;
	TArray<FString> DConditions;
	int32 DLevels = 1;
	float VillageSeerBonus = 0.25f;
	TArray<FString> VillageSeerConditions;
	TMap<FName, float> Sources;
	TMap<FName, float> TempoFactors;
	float FindShare = 0.5f;
	float FindRadiusCm = 3000.f;
	float GroupWindowSeconds = 10.f;
	float GroupDistanceCm = 1500.f;
	float GroupRangeMul = 1.25f;
	int32 GroupLevelMinus = 1;
	float PassiveInterval = 0.15f;
	float MaxEdgeAlpha = 0.55f;

	/** Experience: counted on the server, replicated to the owner, saved. */
	UPROPERTY(Replicated)
	float EP = 0.f;

	/** Finds this player already got experience for (server, saved). */
	TArray<FName> CountedTargets;

	// Active listening (local)
	bool bListening = false;
	double StartTime = -1000.0;
	double EndTime = -1000.0;
	double ShowUntil = 0.0;
	double CooldownEnd = 0.0;
	FVector Center = FVector::ZeroVector;
	TArray<FGamerListenFind> Finds;
	TArray<TWeakObjectPtr<AActor>> Outlined;
	int32 FrozenLevel = 1;
	// Group lift (set by the server's answer)
	int32 GroupLevel = 0;
	float RangeMul = 1.f;
	// Server side bookkeeping for group listening
	double ServerListenTime = -1000.0;
	// Passive
	float Proximity = 0.f;
	double NextPassive = 0.0;

	TArray<TWeakObjectPtr<AActor>> DebugActors;
	float DebugAge = -1.f;
	bool bDebugAmulet = false;
	bool bDebugNoOrigin = false;
};
