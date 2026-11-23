#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GamerSaveable.h"
#include "GamerStaminaComponent.generated.h"

/**
 * Stamina (short term) and exhaustion (long term) of one player (decision 2026-11-16 "Ausdauer").
 * Never blocks anything (architecture rule 5): empty stamina only makes the player slower and weaker. Exhaustion lowers the
 * maximum stamina and the work speed and rises over the game day; sleeping at the own bed brings it down.
 * Server authoritative; stamina and exhaustion replicate to the owner; saved per player (key "stamina").
 * Values in Content/Data/stamina.json; buffs "stamina_max" / "stamina_regen" (food, later equipment) change the numbers.
 */
UCLASS(ClassGroup = (Gamer), meta = (BlueprintSpawnableComponent))
class GAMER_API UGamerStaminaComponent : public UActorComponent, public IGamerSaveable
{
	GENERATED_BODY()

public:
	UGamerStaminaComponent();

	virtual FString GetSaveKey() const override { return TEXT("stamina"); }
	virtual void WriteSaveState(FGamerSaveRecord& Out) const override;
	virtual void ReadSaveState(const FGamerSaveRecord& In) override;

	float GetStamina() const { return Stamina; }
	/** Maximum without the exhaustion penalty (what the bar's full length means). */
	float GetBaseMax() const;
	/** Current maximum (exhaustion lowers it). */
	float GetMaxStamina() const;
	float GetExhaustion() const { return Exhaustion; }   // 0..1
	bool IsEmpty() const { return bEmpty; }
	/** Movement speed factor (1 normally, less when empty). */
	float GetSpeedFactor() const;
	/** Factor on the strength of strikes (empty stamina). */
	float GetStrikeFactor() const;
	/** Factor on the speed of gathering and crafting by hand (exhaustion). */
	float GetWorkSpeedFactor() const;

	// Server side: things that cost stamina.
	void NoteJump();
	void NoteStrike();

	/** Sleep at the own bed (owning client asks; the server checks the bed and the cool-down). */
	UFUNCTION(Server, Reliable)
	void ServerSleep();
	/** E at a finished bed (any bed in reach): it becomes the respawn point and, if tired, you sleep. */
	UFUNCTION(Server, Reliable)
	void ServerSleepAt(class AGamerBuilding* Bed);
	bool CanSleepNow() const;

	/** Tests only. Sets stamina (0..max) and exhaustion (0..1); a negative value leaves it. */
	UFUNCTION(Server, Reliable)
	void ServerDebugSet(float NewStamina, float NewExhaustion);
	/** Tests only: how fast exhaustion rises (time-lapse), 1 = normal. */
	UFUNCTION(Server, Reliable)
	void ServerDebugTimeLapse(float Factor);

	FString DescribeState() const;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
	void LoadSettings();
	void Spend(float Amount);
	float BuffBonus(uint8 Stat) const;
	bool IsFed() const;

	bool bLoaded = false;
	float BaseMax = 100.f;
	float RegenPerSecond = 14.f;
	float RegenDelay = 1.2f;
	float NotFedRegenFactor = 0.7f;
	float SprintCostPerSecond = 14.f;
	float JumpCost = 8.f;
	float StrikeCost = 5.f;
	float EmptySpeedFactor = 0.65f;
	float EmptyStrikeFactor = 0.7f;
	float RecoverFraction = 0.15f;
	float RisePerGameDay = 0.7f;
	float MaxPenalty = 0.4f;
	float WorkSpeedPenalty = 0.3f;
	float SleepRecovery = 1.f;
	float SleepMinExhaustion = 0.1f;
	float SleepCooldown = 60.f;
	float GameDaySeconds = 1800.f;

	UPROPERTY(Replicated)
	float Stamina = 100.f;

	UPROPERTY(Replicated)
	float Exhaustion = 0.f;

	bool bEmpty = false;
	bool bInitialised = false;
	double LastSpendTime = -100.0;
	double LastSleepTime = -1000.0;
	float TimeLapse = 1.f;
};
