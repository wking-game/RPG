#include "GamerGameState.h"

#include "Gamer.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

FGamerWorldOptions FGamerWorldOptions::FromUrlOptions(const FString& Options)
{
	FGamerWorldOptions Result;
	auto Read = [&Options](const TCHAR* Key) { return UGameplayStatics::ParseOption(Options, Key).ToLower(); };

	const FString Flow = Read(TEXT("BuildFlow"));
	if (Flow == TEXT("blueprint")) { Result.BuildFlow = EGamerBuildFlow::Blueprint; }
	else if (Flow == TEXT("instant")) { Result.BuildFlow = EGamerBuildFlow::Instant; }
	else if (Flow == TEXT("material")) { Result.BuildFlow = EGamerBuildFlow::MaterialOnBuild; }
	else if (!Flow.IsEmpty()) { UE_LOG(LogGamer, Warning, TEXT("World option BuildFlow=%s unknown (blueprint, instant, material)"), *Flow); }

	const FString Placement = Read(TEXT("Placement"));
	if (Placement == TEXT("free")) { Result.Placement = EGamerPlacement::FreeSnap; }
	else if (Placement == TEXT("grid")) { Result.Placement = EGamerPlacement::WorldGrid; }
	else if (!Placement.IsEmpty()) { UE_LOG(LogGamer, Warning, TEXT("World option Placement=%s unknown (free, grid)"), *Placement); }

	const FString Demolish = Read(TEXT("Demolish"));
	if (Demolish == TEXT("full")) { Result.Demolish = EGamerDemolish::FullRefund; }
	else if (Demolish == TEXT("half")) { Result.Demolish = EGamerDemolish::HalfRefund; }
	else if (Demolish == TEXT("off")) { Result.Demolish = EGamerDemolish::Disabled; }
	else if (!Demolish.IsEmpty()) { UE_LOG(LogGamer, Warning, TEXT("World option Demolish=%s unknown (full, half, off)"), *Demolish); }
	return Result;
}

FString FGamerWorldOptions::Describe() const
{
	const TCHAR* Flow = BuildFlow == EGamerBuildFlow::Blueprint ? TEXT("Bauplan + Hammer")
		: BuildFlow == EGamerBuildFlow::Instant ? TEXT("sofort fertig") : TEXT("Material beim Bauen");
	const TCHAR* Place = Placement == EGamerPlacement::FreeSnap ? TEXT("frei mit Einrasten") : TEXT("festes Raster");
	const TCHAR* Tear = Demolish == EGamerDemolish::FullRefund ? TEXT("Abreißen: volles Material")
		: Demolish == EGamerDemolish::HalfRefund ? TEXT("Abreißen: halbes Material") : TEXT("kein Abreißen");
	return FString::Printf(TEXT("%s · %s · %s"), Flow, Place, Tear);
}

FString FGamerWorldOptions::ToLogString() const
{
	const TCHAR* Flow = BuildFlow == EGamerBuildFlow::Blueprint ? TEXT("blueprint")
		: BuildFlow == EGamerBuildFlow::Instant ? TEXT("instant") : TEXT("material");
	const TCHAR* Place = Placement == EGamerPlacement::FreeSnap ? TEXT("free") : TEXT("grid");
	const TCHAR* Tear = Demolish == EGamerDemolish::FullRefund ? TEXT("full")
		: Demolish == EGamerDemolish::HalfRefund ? TEXT("half") : TEXT("off");
	return FString::Printf(TEXT("BuildFlow=%s Placement=%s Demolish=%s"), Flow, Place, Tear);
}

FString FGamerWorldOptions::ToUrlOptions() const
{
	return TEXT("?") + ToLogString().Replace(TEXT(" "), TEXT("?"));
}

void AGamerGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AGamerGameState, WorldOptions);
}

void AGamerGameState::SetWorldOptions(const FGamerWorldOptions& InOptions)
{
	WorldOptions = InOptions;
	ForceNetUpdate();
}

const FGamerWorldOptions& AGamerGameState::Get(const UObject* WorldContext)
{
	static const FGamerWorldOptions Defaults;
	const UWorld* World = WorldContext ? WorldContext->GetWorld() : nullptr;
	const AGamerGameState* State = World ? World->GetGameState<AGamerGameState>() : nullptr;
	return State ? State->WorldOptions : Defaults;
}
