#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GamerDataTypes.h"
#include "GamerSaveable.h"
#include "GamerBuilding.generated.h"

class UStaticMeshComponent;
class UMaterialInstanceDynamic;

/** How a building actor is shown. */
enum class EGamerBuildingLook : uint8
{
	Finished,
	Plan,          // placed, not finished yet: see-through, players walk through it
	PreviewValid,  // local placement preview (client only, never replicated)
	PreviewInvalid
};

/**
 * A placed building piece or work station (data: buildables.json via BuildableId).
 * The server spawns it; position, id and build progress replicate to everyone.
 * Unfinished plans are see-through and do not block players; finished pieces block.
 * The same class is used client-side (not replicated) as the placement preview.
 * Work stations without a model yet get a placeholder made of engine shapes.
 */
UCLASS()
class GAMER_API AGamerBuilding : public AActor, public IGamerSaveable
{
	GENERATED_BODY()

public:
	// IGamerSaveable (save system, implemented in GamerSaveImpl.cpp)
	virtual void WriteSaveState(FGamerSaveRecord& Out) const override;
	virtual void ReadSaveState(const FGamerSaveRecord& In) override;

public:
	AGamerBuilding();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void BeginPlay() override;

	/** Drawn fields: how many units a field of this scale has (area / unit_m2, at least 1). Other pieces: 1. */
	static int32 UnitsFor(const FGamerBuildableDef& Def, const FVector& Scale);
	/** Cost of a piece at this scale (drawn fields pay per unit). */
	static TArray<FGamerItemAmount> CostFor(const FGamerBuildableDef& Def, const FVector& Scale);
	/** Drawn fields: clamps a requested scale (1 = 100 cm per side) to the allowed size. Other pieces: 1. */
	static FVector ClampScale(const FGamerBuildableDef& Def, const FVector& Scale);

	/** Upgrade level (1 = as built). Decision 2026-11-14: work buildings can be upgraded for more work places. */
	int32 GetLevel() const { return Level; }
	/** Work places at the current level (0 = no limit). */
	int32 GetWorkers() const;
	/** Next upgrade stage, or null at the top level. */
	const FGamerUpgradeDef* GetNextUpgrade() const;
	/** Server: one level up (cost already paid by the caller). */
	void Upgrade();

	/** Actor class for a placed piece: farm buildings (fields, composter, rain catcher) have their own. */
	static UClass* ClassFor(const UObject* WorldContext, FName BuildableId);

	/** Server (before FinishSpawning): which piece, and how far it is built (1 = finished). */
	void InitBuilding(FName InBuildableId, float InProgress);

	/** Client: turn this (non-replicated) actor into the placement preview for a piece. */
	void InitPreview(FName InBuildableId);
	void SetPreviewValid(bool bValid);

	FName GetBuildableId() const { return BuildableId; }
	const FGamerBuildableDef* GetDef() const;
	bool IsFinished() const { return Progress >= 0.999f; }
	float GetProgress() const { return Progress; }
	bool IsPreview() const { return bIsPreview; }

	/** Server: add build progress (0..1). Returns true when the piece became finished. */
	bool AddProgress(float Amount);

	/** Material already put in (build flow "material on build"; for the other flows the full cost). */
	const TArray<FGamerItemAmount>& GetSpent() const { return Spent; }
	void SetSpent(const TArray<FGamerItemAmount>& InSpent);

	/** Number of strikes already done (build flow "material on build"). */
	int32 GetStrikesDone() const { return StrikesDone; }
	void SetStrikesDone(int32 Value) { StrikesDone = Value; }

protected:
	UPROPERTY(VisibleAnywhere, Category = "Visual")
	TObjectPtr<UStaticMeshComponent> Mesh;

	UPROPERTY(ReplicatedUsing = OnRep_State)
	FName BuildableId;

	UPROPERTY(ReplicatedUsing = OnRep_State)
	float Progress = 0.f;

	UPROPERTY(Replicated)
	TArray<FGamerItemAmount> Spent;

	UPROPERTY(Replicated)
	int32 Level = 1;

	UFUNCTION()
	void OnRep_State();

private:
	void ApplyLook();
	EGamerBuildingLook CurrentLook() const;

	UPROPERTY(Transient)
	TObjectPtr<UMaterialInstanceDynamic> PlanMaterial;

	bool bIsPreview = false;
	bool bPreviewValid = true;
	int32 StrikesDone = 0;
	FName AppliedId;
	bool bAppliedFinished = false;
	bool bAppliedOnce = false;
};
