#include "GamerGameMode.h"

#include "Gamer.h"
#include "GamerCharacter.h"
#include "GamerDataSubsystem.h"
#include "GamerGameState.h"
#include "GamerHUD.h"
#include "GamerNavigationSystem.h"
#include "GamerPlayerController.h"
#include "GamerResourceNode.h"
#include "GamerSettlement.h"
#include "EngineUtils.h"
#include "Engine/World.h"
#include "GameFramework/PlayerStart.h"
#include "Math/RandomStream.h"

AGamerGameMode::AGamerGameMode()
{
	DefaultPawnClass = AGamerCharacter::StaticClass();
	PlayerControllerClass = AGamerPlayerController::StaticClass();
	HUDClass = AGamerHUD::StaticClass();
	GameStateClass = AGamerGameState::StaticClass();
}

void AGamerGameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage); // sets OptionsString
	ApplyWorldOptions(true);
	// Villager pathfinding: the engine sets up navigation for play after InitGame, so the swap happens in time.
	if (UWorld* World = GetWorld())
	{
		UGamerNavigationSystem::UseForWorld(*World);
	}
}

void AGamerGameMode::InitGameState()
{
	Super::InitGameState();
	ApplyWorldOptions(false);
}

void AGamerGameMode::ApplyWorldOptions(bool bFromInitGame)
{
	// World options from the map URL until the start menu exists (decision 2026-11-13).
	// The game state may be spawned before or after InitGame: InitGame always wins, InitGameState only fills in.
	AGamerGameState* State = GetGameState<AGamerGameState>();
	if (!State || (bWorldOptionsApplied && !bFromInitGame))
	{
		return;
	}
	State->SetWorldOptions(FGamerWorldOptions::FromUrlOptions(OptionsString));
	bWorldOptionsApplied = true;
	UE_LOG(LogGamer, Log, TEXT("World options: %s"), *State->GetWorldOptions().ToLogString());
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

void AGamerGameMode::StartPlay()
{
	Super::StartPlay();

	bool bMapHasNodes = false;
	for (TActorIterator<AGamerResourceNode> It(GetWorld()); It; ++It)
	{
		bMapHasNodes = true;
		break;
	}
	if (bSpawnStartArea && !bMapHasNodes)
	{
		SpawnStartArea();
	}

	// One settlement per world for the vertical slice (storage, villagers).
	if (!AGamerSettlement::Get(this))
	{
		FVector Center = FVector::ZeroVector;
		for (TActorIterator<APlayerStart> It(GetWorld()); It; ++It)
		{
			Center = It->GetActorLocation();
			break;
		}
		GetWorld()->SpawnActor<AGamerSettlement>(AGamerSettlement::StaticClass(), Center, FRotator::ZeroRotator);
		UE_LOG(LogGamer, Log, TEXT("Settlement created at %s"), *Center.ToCompactString());
	}
}

void AGamerGameMode::SpawnStartArea()
{
	const UGamerDataSubsystem* Data = UGamerDataSubsystem::Get(this);
	if (!Data)
	{
		return;
	}

	// Placeholder layout for the vertical slice: hand resources close by, trees and rocks further out.
	// Replaced by the seed-based world generation later (docs: design register chapter 9).
	struct FEntry
	{
		const TCHAR* Id;
		int32 Count;
		float MinRadius;
		float MaxRadius;
	};
	static const FEntry Layout[] = {
		{ TEXT("res_loose_stone"), 10, 300.f, 1400.f },
		{ TEXT("res_fallen_branch"), 10, 300.f, 1400.f },
		{ TEXT("res_fiber_plant"), 8, 300.f, 1400.f },
		{ TEXT("res_berry_bush"), 4, 600.f, 1800.f },
		{ TEXT("res_hazel_bush"), 3, 700.f, 2000.f },
		{ TEXT("res_mushroom_patch"), 5, 600.f, 2000.f },
		{ TEXT("res_tree_apple"), 3, 900.f, 2200.f },
		{ TEXT("res_tree_pine"), 6, 1000.f, 2400.f },
		{ TEXT("res_tree_birch"), 5, 1000.f, 2400.f },
		{ TEXT("res_rock_small"), 4, 800.f, 2300.f },
		{ TEXT("res_rock_medium"), 3, 1000.f, 2400.f },
		{ TEXT("res_rock_large"), 2, 1200.f, 2400.f },
	};

	FVector Center = FVector::ZeroVector;
	for (TActorIterator<APlayerStart> It(GetWorld()); It; ++It)
	{
		Center = It->GetActorLocation();
		break;
	}

	FRandomStream Random(StartAreaSeed);
	int32 Spawned = 0;
	int32 Skipped = 0;
	for (const FEntry& Entry : Layout)
	{
		if (!Data->FindResourceNode(Entry.Id))
		{
			continue;
		}
		for (int32 Index = 0; Index < Entry.Count; ++Index)
		{
			const float Angle = Random.FRandRange(0.f, 2.f * PI);
			const float Radius = Random.FRandRange(Entry.MinRadius, Entry.MaxRadius);
			const FVector Probe = Center + FVector(FMath::Cos(Angle) * Radius, FMath::Sin(Angle) * Radius, 0.f);

			FHitResult Hit;
			FCollisionQueryParams Params(SCENE_QUERY_STAT(GamerStartArea), false);
			if (!GetWorld()->LineTraceSingleByChannel(Hit, Probe + FVector(0, 0, 2000.f), Probe - FVector(0, 0, 4000.f), ECC_WorldStatic, Params))
			{
				++Skipped; // no ground there (edge of the test map)
				continue;
			}
			const FTransform Transform(FRotator(0.f, Random.FRandRange(0.f, 360.f), 0.f), Hit.ImpactPoint);
			AGamerResourceNode* Node = GetWorld()->SpawnActorDeferred<AGamerResourceNode>(AGamerResourceNode::StaticClass(), Transform);
			if (Node)
			{
				Node->SetNodeId(FName(Entry.Id));
				Node->FinishSpawning(Transform);
				++Spawned;
			}
		}
	}
	UE_LOG(LogGamer, Log, TEXT("Start area: %d resource nodes spawned around %s (%d skipped, no ground)"), Spawned, *Center.ToCompactString(), Skipped);
}
