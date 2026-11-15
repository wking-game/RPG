#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GamerDataTypes.h"
#include "GamerAnimTypes.h"
#include "GamerSaveable.h"
#include "GamerVillager.generated.h"

class AGamerBuilding;
class AGamerResourceNode;
class AGamerSettlement;
class UNavigationInvokerComponent;
class UStaticMeshComponent;
class UTextRenderComponent;

/**
 * A villager. All decisions run on the server (architecture rule 7); clients only see the replicated
 * name, job, current activity, needs and movement.
 *
 * Decisions (needs instead of a clock, priorities with a fallback job), checked about once a second:
 *  1. hungry and food in the storage -> eat     2. tired -> sleep (own bed, else on the spot, slower)
 *  3. carrying something -> bring it to the storage
 *  4. own job (chosen by the player in the village management): harvest near the station / cook at the
 *     station / hammer building plans; the tool comes from the storage
 *  5. own job not possible (no station, no tool, nothing to do) -> fallback job (data: "fallback": true)
 *  6. otherwise wait near the storage
 * Movement: paths from the navigation mesh (UGamerNavigationSystem, built only around the villagers), so they walk
 * around buildings, trees and rocks and through door openings. Without a path (mesh not built yet) they walk
 * straight with side-steps; a target that stays out of reach is given up.
 * Placeholder look: cylinder in the job colour, name and activity above the head.
 */
UCLASS(Config = Game)
class GAMER_API AGamerVillager : public ACharacter, public IGamerSaveable
{
	GENERATED_BODY()

public:
	// IGamerSaveable (save system, implemented in GamerSaveImpl.cpp)
	virtual void WriteSaveState(FGamerSaveRecord& Out) const override;
	virtual void ReadSaveState(const FGamerSaveRecord& In) override;

public:
	AGamerVillager();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	// --- Shown everywhere (replicated) ---

	const FString& GetVillagerName() const { return VillagerName; }
	FName GetJobId() const { return JobId; }
	const FString& GetActivity() const { return Activity; }
	float GetHunger() const { return Hunger; }
	float GetSleep() const { return Sleep; }
	/** Mood 0..1 (decision 2026-11-14: a derived value, not a fourth need). Follows hunger, sleep and shelter, plus a bonus from tavern meals. */
	float GetMood() const { return Mood; }
	bool HasShelter() const { return bShelter; }
	AGamerBuilding* GetBed() const { return Bed; }
	FName GetHeldTool() const { return HeldTool; }

	// --- Server ---

	void InitVillager(const FString& InName, AGamerBuilding* InBed);
	/** The player's choice in the village management (any job id from jobs.json). */
	/** Server: assign a job. Refused (false, OutWhyNot) when all its work places are taken (decision 2026-11-14). */
	bool SetJob(FName InJobId, FString* OutWhyNot = nullptr);
	void SetShelter(bool bInShelter);

	/** One-line state for logs and tests. */
	FString Describe() const;

protected:
	UPROPERTY(VisibleAnywhere, Category = "Visual")
	TObjectPtr<UStaticMeshComponent> Body;

	UPROPERTY(VisibleAnywhere, Category = "Visual")
	TObjectPtr<UTextRenderComponent> Label;

	/** The navigation mesh is built only around these (server). */
	UPROPERTY(VisibleAnywhere, Category = "Navigation")
	TObjectPtr<UNavigationInvokerComponent> NavInvoker;

	UPROPERTY(ReplicatedUsing = OnRep_Look)
	FString VillagerName;

	UPROPERTY(ReplicatedUsing = OnRep_Look)
	FName JobId;

	UPROPERTY(ReplicatedUsing = OnRep_Look)
	FString Activity;

	UPROPERTY(Replicated)
	float Hunger = 1.f;

	UPROPERTY(Replicated)
	float Sleep = 1.f;

	UPROPERTY(Replicated)
	float Mood = 0.5f;

	UPROPERTY(Replicated)
	bool bShelter = false;

	UPROPERTY(Replicated)
	TObjectPtr<AGamerBuilding> Bed;

	UPROPERTY(ReplicatedUsing = OnRep_Look)
	FName HeldTool;

	/** Work animation loop while working at the target (server decides, everyone plays it). */
	UPROPERTY(Replicated)
	EGamerAnimAction AnimWork = EGamerAnimAction::None;

	/** Walking speed (cm/s). */
	UPROPERTY(Config, EditDefaultsOnly, Category = "Villager")
	float WalkSpeed = 320.f;

	/** A task running longer than this (seconds) is given up, except sleeping and waiting. */
	UPROPERTY(Config, EditDefaultsOnly, Category = "Villager")
	float MaxTaskSeconds = 45.f;

	/** How much a villager carries before going to the storage. */
	UPROPERTY(Config, EditDefaultsOnly, Category = "Villager")
	int32 CarryLimit = 10;

	UFUNCTION()
	void OnRep_Look();

public:
	/** Animation inputs (work loop); speed and falling come from the movement. */
	void FillAnimInputs(FGamerAnimInputs& Out) const { Out.Loop = AnimWork; }

protected:
	/** Interim figures on the UE5 skeleton (Manny/Quinn), picked by name. */
	UPROPERTY()
	TObjectPtr<class USkeletalMesh> BodyA;

	UPROPERTY()
	TObjectPtr<class USkeletalMesh> BodyB;

private:
	enum class ETask : uint8
	{
		None,
		Eat,
		Sleep,
		Deliver,
		FetchTool,
		ReturnTool,
		Harvest,
		FetchIngredients,
		Craft,
		/** Farmer: take seeds, water, fertilizer or spoiled food from the storage for a farm building. */
		FetchFarmInput,
		/** Farmer: work at a field, composter or rain catcher (FarmAction). */
		FarmWork,
		FetchMaterial,
		Build,
		/** Innkeeper: take dishes and drinks from the storage (ServeItem) ... */
		ServeFetch,
		/** ... and put them into the tavern stock. */
		Serve,
		Wait
	};

	void ServerTick(float DeltaSeconds);
	void UpdateNeeds(float GameDays);
	void Think();
	/** Picks the next step of a job (fetch the tool only if there is work for it). bIsFallback: keep a held tool. */
	bool PlanJob(const FGamerJobDef& Job, bool bIsFallback, FString& OutWhyNot);
	/** Interrupt the current task when hunger or sleep became urgent. */
	bool NeedIsUrgent() const;
	void RunTask(float DeltaSeconds);
	void FinishTask();
	void Leave(const FString& Reason);

	/** Walks along a navigation path (straight with side-steps if there is none); true when within Accept (2D) of Goal. */
	bool MoveTo(const FVector& Goal, float Accept, float DeltaSeconds);
	/** Asks the navigation mesh for a path (partial paths allowed: targets like trees stand on no walkable ground). */
	bool FindPath(const FVector& Destination);

	void SetActivity(const FString& Text);
	float WorkSpeed() const;
	const FGamerJobDef* CurrentJobDef() const;
	const FGamerJobDef* FallbackJobDef() const;
	const FGamerNeedDef* NeedOfKind(EGamerNeedKind Kind) const;
	AGamerSettlement* GetSettlement() const;
	AActor* FindStation(FName StationId) const;
	AGamerResourceNode* FindNode(const FGamerJobDef& Job, const FVector& Center, int32 ToolTier) const;
	AGamerBuilding* FindPlan(const FVector& Center, float Radius, TArray<FGamerItemAmount>& OutMissing) const;
	void AddCarry(FName Item, int32 Count);
	int32 CarryCount() const;
	void ApplyLook();

	/** Cook: the place a recipe is made at - the job's own station, or a cooking place of the recipe near it (tavern). */
	AActor* CraftStationFor(const struct FGamerRecipeDef& Recipe, const FGamerJobDef& Job, AActor* JobStation) const;
	/** Nearest finished tavern whose stock holds something from the hunger list. */
	class AGamerTavern* FindTavernWithFood() const;
	/** Innkeeper (EGamerJobKind::Serve): bring dishes and drinks from the storage into the tavern. */
	bool PlanServe(const FGamerJobDef& Job, AActor* Station, AActor* Storage, FString& OutWhyNot);
	/** Meal in the tavern: mood gain. The mood value itself comes from the Gameplay session; until then only logged. */
	void OnTavernMeal(float MoodGain);
	/** Server: a one-off mood bonus (tavern meal, later festivals ...); fades over a few game days. */
	void AddMood(float Delta);

	/** Farmer job (EGamerJobKind::Farm): pick the next field work around the farmstead. */
	bool PlanFarm(const FGamerJobDef& Job, const FVector& Center, AActor* Storage, FString& OutWhyNot);
	void RunFarmWork(float DeltaSeconds);

	ETask Task = ETask::None;
	/** Farmer: what to do at FarmTarget and what is carried for it (not part of Carry, it is used up there). */
	uint8 FarmAction = 0;
	TWeakObjectPtr<class AGamerFarmBuilding> FarmTarget;
	FName FarmItem;
	int32 FarmCount = 0;
	int32 FarmWanted = 0;
	TWeakObjectPtr<AActor> Target;
	FVector Goal = FVector::ZeroVector;
	float TaskTimer = 0.f;
	float StrikeTimer = 0.f;
	float ThinkTimer = 0.f;
	FName TaskRecipe;
	TWeakObjectPtr<AGamerBuilding> BuildPlan;
	/** Innkeeper: what is carried to which tavern (not part of Carry). */
	TWeakObjectPtr<AActor> ServeTavern;
	FName ServeItem;
	int32 ServeCount = 0;
	int32 ServeWanted = 0;
	float ServeFreshness = 1.f;
	/** Sum of mood from tavern meals not yet handed to the mood need (see OnTavernMeal). */
	float PendingMoodGain = 0.f;
	TArray<FGamerItemAmount> Carry;
	bool bDeliverNow = false;
	float ShelterTimer = 0.f;
	/** Seconds the current task runs; a task taking far too long is given up (safety net against getting stuck). */
	float TaskAge = 0.f;
	float HungerEmptyDays = 0.f;
	bool bWorkingOnFallback = false;

	FVector LastPosition = FVector::ZeroVector;
	float StuckTimer = 0.f;
	float SidestepTimer = 0.f;
	float SidestepSign = 1.f;
	int32 StuckCount = 0;
	TMap<TWeakObjectPtr<AActor>, double> Unreachable;

	TArray<FVector> PathPoints;
	int32 PathIndex = 0;
	FVector PathGoal = FVector::ZeroVector;
	float RepathTimer = 0.f;
	/** Paths found / walks without a path, for logs and tests. */
	int32 PathsFound = 0;
	int32 PathsMissing = 0;
};
