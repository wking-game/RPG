#include "GamerGameMode.h"

#include "Gamer.h"
#include "GamerCharacter.h"
#include "GamerPlayerController.h"
#include "Engine/World.h"

AGamerGameMode::AGamerGameMode()
{
	DefaultPawnClass = AGamerCharacter::StaticClass();
	PlayerControllerClass = AGamerPlayerController::StaticClass();
}

APawn* AGamerGameMode::SpawnDefaultPawnAtTransform_Implementation(AController* NewPlayer, const FTransform& SpawnTransform)
{
	// Several players may share one start point: line them up and let the engine nudge them free of overlaps.
	const int32 PlayerIndex = FMath::Max(0, GetNumPlayers() - 1);
	FTransform Transform = SpawnTransform;
	Transform.AddToTranslation(Transform.GetRotation().GetRightVector() * SpawnSpacing * PlayerIndex);

	FActorSpawnParameters Params;
	Params.Instigator = GetInstigator();
	Params.ObjectFlags |= RF_Transient;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	UClass* PawnClass = GetDefaultPawnClassForController(NewPlayer);
	APawn* Pawn = GetWorld()->SpawnActor<APawn>(PawnClass, Transform, Params);
	UE_LOG(LogGamer, Log, TEXT("Spawned pawn for player %d at %s"), PlayerIndex, Pawn ? *Pawn->GetActorLocation().ToCompactString() : TEXT("(failed)"));
	return Pawn;
}
