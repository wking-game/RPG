#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GamerDataTypes.h"
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
class GAMER_API AGamerVillager : public ACharacter
{
	GENERATED_BODY()

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
	bool HasShelter() const { return bShelter; }
	AGamerBuilding* GetBed() const { return Bed; }
	FName GetHeldTool() const { return HeldTool; }

	// --- Server ---

	void InitVillager(const FString& InName, AGamerBuilding* InBed);
	/** The player's choice in the village management (any job id from jobs.json). */
	void SetJob(FName InJobId);
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
	bool bShelter = false;

	UPROPERTY(Replicated)
	TObjectPtr<AGamerBuilding> Bed;

	UPROPERTY(ReplicatedUsing = OnRep_Look)
	FName HeldTool;

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
		FetchMaterial,
		Build,
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

	ETask Task = ETask::None;
	TWeakObjectPtr<AActor> Target;
	FVector Goal = FVector::ZeroVector;
	float TaskTimer = 0.f;
	float StrikeTimer = 0.f;
	float ThinkTimer = 0.f;
	FName TaskRecipe;
	TWeakObjectPtr<AGamerBuilding> BuildPlan;
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
