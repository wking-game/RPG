#include "GamerNavigationSystem.h"

#include "Gamer.h"
#include "AI/Navigation/NavigationBounds.h"
#include "Engine/World.h"
#include "GameFramework/WorldSettings.h"

void UGamerNavigationSystem::UseForWorld(UWorld& World)
{
	if (Cast<UGamerNavigationSystem>(World.GetNavigationSystem()) || World.GetNetMode() == NM_Client)
	{
		return;
	}
	AWorldSettings* WorldSettings = World.GetWorldSettings();
	if (!WorldSettings)
	{
		return;
	}
	UNavigationSystemModuleConfig* Config = NewObject<UNavigationSystemModuleConfig>(WorldSettings);
	Config->NavigationSystemClass = FSoftClassPath(UGamerNavigationSystem::StaticClass());
	WorldSettings->SetNavigationSystemConfigOverride(Config);
	FNavigationSystem::AddNavigationSystemToWorld(World, FNavigationSystemRunMode::InvalidMode, Config,
		/*bInitializeForWorld=*/false, /*bOverridePreviousNavSys=*/true);
	UE_LOG(LogGamer, Log, TEXT("Navigation: %s"), *GetNameSafe(World.GetNavigationSystem() ? World.GetNavigationSystem()->GetClass() : nullptr));
}

bool UGamerNavigationSystem::UsesGameBounds() const
{
	const UWorld* World = GetWorld();
	return World && World->IsGameWorld();
}

bool UGamerNavigationSystem::IsThereAnywhereToBuildNavigation() const
{
	return UsesGameBounds() || Super::IsThereAnywhereToBuildNavigation();
}

void UGamerNavigationSystem::GatherNavigationBounds()
{
	Super::GatherNavigationBounds();
	if (!UsesGameBounds())
	{
		return;
	}
	FNavigationBounds Bounds;
	Bounds.UniqueID = GetUniqueID();
	Bounds.AreaBox = FBox(FVector(-NavigableHalfSize, -NavigableHalfSize, NavigableMinZ), FVector(NavigableHalfSize, NavigableHalfSize, NavigableMaxZ));
	Bounds.Level = GetWorld()->PersistentLevel;
	AddNavigationBounds(Bounds);
	UE_LOG(LogGamer, Log, TEXT("Navigation bounds: %s (mesh only around villagers)"), *Bounds.AreaBox.ToString());
}
