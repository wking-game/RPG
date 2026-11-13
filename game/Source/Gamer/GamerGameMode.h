#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "GamerGameMode.generated.h"

/**
 * Game rules for the co-op world. Exists only on the server.
 * Use it via ?game=/Script/Gamer.GamerGameMode in the map URL (or later as the project default).
 */
UCLASS()
class GAMER_API AGamerGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AGamerGameMode();

	virtual APawn* SpawnDefaultPawnAtTransform_Implementation(AController* NewPlayer, const FTransform& SpawnTransform) override;
	virtual void StartPlay() override;

protected:
	/** Players sharing one start point are placed side by side, this far apart (Unreal units = cm). */
	UPROPERTY(Config, EditDefaultsOnly, Category = "Spawning")
	float SpawnSpacing = 150.f;

	/**
	 * Temporary until world generation exists (Phase 4): if the map has no resource nodes,
	 * scatter a start area with every resource type around the player start.
	 */
	UPROPERTY(Config, EditDefaultsOnly, Category = "Start area")
	bool bSpawnStartArea = true;

	UPROPERTY(Config, EditDefaultsOnly, Category = "Start area")
	int32 StartAreaSeed = 4711;

private:
	void SpawnStartArea();
};
