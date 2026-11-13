#pragma once

#include "CoreMinimal.h"
#include "NavigationSystem.h"
#include "GamerNavigationSystem.generated.h"

/**
 * Navigation for the villagers (pathfinding around buildings, trees and rocks, through door openings).
 *
 * The maps have no NavMeshBoundsVolume, so in game worlds this class adds one large fixed navigable area itself
 * (NavigableHalfSize around the world origin, NavigableMinZ..NavigableMaxZ in height). The height is fixed on
 * purpose: bounds taken from the actors at load time would be as flat as the floor, and walls above it would be
 * missing from the mesh. The mesh is still only built around navigation invokers (the villagers) and rebuilt
 * while the game runs (DefaultEngine.ini: bGenerateNavigationOnlyAroundNavigationInvokers, RuntimeGeneration=Dynamic).
 *
 * Maps saved with their own navigation class (the engine template maps) ignore NavigationSystemClassName;
 * AGamerGameMode swaps in this class for those (UseForWorld). Server only: clients have no navigation system.
 */
UCLASS()
class GAMER_API UGamerNavigationSystem : public UNavigationSystemV1
{
	GENERATED_BODY()

public:
	/** Replaces the world's navigation system with this class, before the engine sets it up for play. */
	static void UseForWorld(UWorld& World);

	/** Half size (cm) of the navigable square around the world origin. */
	UPROPERTY(Config, EditAnywhere, Category = "Gamer")
	float NavigableHalfSize = 50000.f;

	/** Lowest and highest point (cm) the navigation mesh may cover. */
	UPROPERTY(Config, EditAnywhere, Category = "Gamer")
	float NavigableMinZ = -5000.f;

	UPROPERTY(Config, EditAnywhere, Category = "Gamer")
	float NavigableMaxZ = 10000.f;

protected:
	virtual bool IsThereAnywhereToBuildNavigation() const override;
	virtual void GatherNavigationBounds() override;

private:
	/** Only in game worlds (also PIE); editor maps keep the normal bounds volumes. */
	bool UsesGameBounds() const;
};
