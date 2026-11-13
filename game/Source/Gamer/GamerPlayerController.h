#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GamerPlayerController.generated.h"

/**
 * Player controller. Gameplay input lives on the character; this class only holds
 * test helpers for automated multiplayer tests (tools/mp_test.ps1).
 */
UCLASS()
class GAMER_API AGamerPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;
	virtual void PlayerTick(float DeltaTime) override;

	/** Console: walk forward for N seconds. Example: GamerTestWalk 3 */
	UFUNCTION(Exec)
	void GamerTestWalk(float Seconds = 3.f);

private:
	/** From -GamerTestWalk=N and -GamerTestWalkDelay=N: walk once the pawn exists and the delay is over. */
	float PendingWalkSeconds = 0.f;
	float PendingWalkDelay = 0.f;
};
