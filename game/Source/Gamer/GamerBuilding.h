#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GamerDataTypes.h"
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
class GAMER_API AGamerBuilding : public AActor
{
	GENERATED_BODY()

public:
	AGamerBuilding();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void BeginPlay() override;

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
