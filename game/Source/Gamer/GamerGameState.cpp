#include "GamerGameState.h"

#include "Gamer.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "Materials/MaterialParameterCollection.h"
#include "Materials/MaterialParameterCollectionInstance.h"
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

	const FString Inventory = Read(TEXT("Inventory"));
	if (Inventory == TEXT("world")) { Result.bCarryInventory = false; }
	else if (Inventory == TEXT("character")) { Result.bCarryInventory = true; }
	else if (!Inventory.IsEmpty()) { UE_LOG(LogGamer, Warning, TEXT("World option Inventory=%s unknown (world, character)"), *Inventory); }

	const FString Season = Read(TEXT("SeasonMinutes"));
	if (!Season.IsEmpty())
	{
		const float Minutes = FCString::Atof(*Season);
		if (Minutes >= 1.f && Minutes <= 24.f * 60.f) { Result.SeasonMinutes = Minutes; }
		else { UE_LOG(LogGamer, Warning, TEXT("World option SeasonMinutes=%s out of range (1..1440)"), *Season); }
	}
	return Result;
}

FString FGamerWorldOptions::Describe() const
{
	const TCHAR* Flow = BuildFlow == EGamerBuildFlow::Blueprint ? TEXT("Bauplan + Hammer")
		: BuildFlow == EGamerBuildFlow::Instant ? TEXT("sofort fertig") : TEXT("Material beim Bauen");
	const TCHAR* Place = Placement == EGamerPlacement::FreeSnap ? TEXT("frei mit Einrasten") : TEXT("festes Raster");
	const TCHAR* Tear = Demolish == EGamerDemolish::FullRefund ? TEXT("Abreißen: volles Material")
		: Demolish == EGamerDemolish::HalfRefund ? TEXT("Abreißen: halbes Material") : TEXT("kein Abreißen");
	return FString::Printf(TEXT("%s · %s · %s · Jahreszeit %g Min. · Inventar %s"), Flow, Place, Tear, SeasonMinutes,
		bCarryInventory ? TEXT("mit dem Charakter") : TEXT("pro Welt"));
}

FString FGamerWorldOptions::ToLogString() const
{
	const TCHAR* Flow = BuildFlow == EGamerBuildFlow::Blueprint ? TEXT("blueprint")
		: BuildFlow == EGamerBuildFlow::Instant ? TEXT("instant") : TEXT("material");
	const TCHAR* Place = Placement == EGamerPlacement::FreeSnap ? TEXT("free") : TEXT("grid");
	const TCHAR* Tear = Demolish == EGamerDemolish::FullRefund ? TEXT("full")
		: Demolish == EGamerDemolish::HalfRefund ? TEXT("half") : TEXT("off");
	return FString::Printf(TEXT("BuildFlow=%s Placement=%s Demolish=%s SeasonMinutes=%g Inventory=%s"), Flow, Place, Tear, SeasonMinutes,
		bCarryInventory ? TEXT("character") : TEXT("world"));
}

FString FGamerWorldOptions::ToUrlOptions() const
{
	return TEXT("?") + ToLogString().Replace(TEXT(" "), TEXT("?"));
}

AGamerGameState::AGamerGameState()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickInterval = 0.2f; // the season moves slowly; 5 updates per second are plenty
	WorldParameters = TSoftObjectPtr<UMaterialParameterCollection>(FSoftObjectPath(TEXT("/Game/Gamer/Materials/MPC_GamerWorld.MPC_GamerWorld")));
}

void AGamerGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AGamerGameState, WorldOptions);
	DOREPLIFETIME(AGamerGameState, SeasonAnchorPhase);
	DOREPLIFETIME(AGamerGameState, SeasonAnchorTime);
}

void AGamerGameState::BeginPlay()
{
	Super::BeginPlay();
	if (HasAuthority())
	{
		SetSeasonPhase(StartSeasonPhase);
	}
	PushSeasonToMaterials();
}

void AGamerGameState::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	PushSeasonToMaterials();

	// One log line per eighth of a year (start/middle of each season), for tests and tracking.
	const float Quarter = FMath::FloorToFloat(GetSeasonPhase() * 2.f) / 2.f;
	if (HasAuthority() && Quarter != LoggedSeasonQuarter)
	{
		LoggedSeasonQuarter = Quarter;
		UE_LOG(LogGamer, Log, TEXT("Season: %s (phase %.2f)"), *DescribeSeason(GetSeasonPhase()), GetSeasonPhase());
	}
}

float AGamerGameState::GetSeasonPhase() const
{
	const double Length = FMath::Max(60.0, static_cast<double>(WorldOptions.SeasonMinutes) * 60.0);
	const double Phase = SeasonAnchorPhase + (GetServerWorldTimeSeconds() - SeasonAnchorTime) / Length;
	const double Wrapped = FMath::Fmod(Phase, 4.0);
	return static_cast<float>(Wrapped < 0.0 ? Wrapped + 4.0 : Wrapped);
}

void AGamerGameState::SetSeasonPhase(float Phase)
{
	if (!HasAuthority())
	{
		return;
	}
	SeasonAnchorPhase = FMath::Fmod(FMath::Max(0.f, Phase), 4.f);
	SeasonAnchorTime = GetServerWorldTimeSeconds();
	LoggedSeasonQuarter = -1.f;
	ForceNetUpdate();
	PushSeasonToMaterials();
	UE_LOG(LogGamer, Log, TEXT("Season set: %s (phase %.2f, %g min per season)"), *DescribeSeason(SeasonAnchorPhase), SeasonAnchorPhase, WorldOptions.SeasonMinutes);
}

FString AGamerGameState::DescribeSeason(float Phase)
{
	static const TCHAR* const Names[] = { TEXT("Frühling"), TEXT("Sommer"), TEXT("Herbst"), TEXT("Winter") };
	const float Wrapped = FMath::Fmod(FMath::Max(0.f, Phase), 4.f);
	const int32 Index = FMath::Clamp(FMath::FloorToInt(Wrapped), 0, 3);
	const float Within = Wrapped - Index;
	const TCHAR* Part = Within < 1.f / 3.f ? TEXT("Anfang") : Within < 2.f / 3.f ? TEXT("Mitte") : TEXT("Ende");
	return FString::Printf(TEXT("%s, %s"), Names[Index], Part);
}

void AGamerGameState::PushSeasonToMaterials()
{
	UWorld* World = GetWorld();
	if (!World || World->GetNetMode() == NM_DedicatedServer)
	{
		return; // nothing is rendered there
	}
	UMaterialParameterCollection* Collection = WorldParameters.LoadSynchronous();
	if (!Collection)
	{
		if (!bWarnedMissingParameters)
		{
			bWarnedMissingParameters = true;
			UE_LOG(LogGamer, Warning, TEXT("Season: %s missing, vegetation keeps its default colours"), *WorldParameters.ToString());
		}
		return;
	}
	if (UMaterialParameterCollectionInstance* Instance = World->GetParameterCollectionInstance(Collection))
	{
		Instance->SetScalarParameterValue(TEXT("SeasonPhase"), GetSeasonPhase());
	}
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
