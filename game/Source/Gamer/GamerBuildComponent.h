#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GamerBuildSnapping.h"
#include "GamerBuildComponent.generated.h"

class AGamerBuilding;
class UGamerInventoryComponent;
class UGamerWorkComponent;
struct FGamerBuildableDef;

/**
 * Building for one player. Local player: chosen piece, rotation and the see-through preview that follows
 * the aim. Server: places pieces, hammers plans and tears pieces down, following the world options
 * (build flow, placement, demolish; AGamerGameState). Clients only ask (Server RPCs); the server checks
 * tool in hand, reach, snapping, material and inventory space.
 */
UCLASS(ClassGroup = (Gamer), Config = Game)
class GAMER_API UGamerBuildComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UGamerBuildComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	// --- Local player ---

	/** Piece chosen in the build menu (NAME_None = only hammering). */
	void SelectBuildable(FName BuildableId);
	FName GetSelected() const { return Selected; }
	void Rotate() { ++RotationSteps; }

	/** True while a piece is chosen and the hammer (or the piece's tool) is in hand. */
	bool IsPlacing() const;

	/** Where the preview currently is and whether it may be placed there. */
	const FGamerPlacement& GetPlacement() const { return Placement; }

	/** Asks the server to place the chosen piece at the preview. Returns false (with a message) if not possible. */
	bool TryPlace();

	/** Same placement logic as the preview, for an explicit aim point (tests). */
	FGamerPlacement ComputePlacement(FName BuildableId, const FVector& AimPoint, bool bAimHit, float ViewYaw,
		const FVector& AimNormal = FVector::UpVector) const;

	float GetBuildReach() const { return BuildReach; }

	// --- Server ---

	UFUNCTION(Server, Reliable)
	void ServerPlace(FName BuildableId, FTransform Transform);

	/** One hammer strike on an unfinished plan (cooldown as for all strikes). */
	UFUNCTION(Server, Reliable)
	void ServerHammer(AGamerBuilding* Building);

	UFUNCTION(Server, Reliable)
	void ServerDemolish(AGamerBuilding* Building);

protected:
	/** How far from the player pieces may be placed (cm). */
	UPROPERTY(Config, EditDefaultsOnly, Category = "Build")
	float BuildReach = 1000.f;

	/** How far from the player a piece may be hammered or torn down (cm, to its pivot). */
	UPROPERTY(Config, EditDefaultsOnly, Category = "Build")
	float WorkReach = 600.f;

private:
	void UpdatePreview();
	void HidePreview();
	bool HasToolFor(const FGamerBuildableDef& Def) const;
	void Message(const FString& Text) const;
	UGamerInventoryComponent* GetInventory() const;
	UGamerWorkComponent* GetWork() const;

	FName Selected;
	int32 RotationSteps = 0;
	FGamerPlacement Placement;
	TWeakObjectPtr<AGamerBuilding> Preview;
	double LastHammerTime = -1000.0;
};
