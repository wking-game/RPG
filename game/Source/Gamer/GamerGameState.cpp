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
	else if (Demolish == TEXT("partial")) { Result.Demolish = EGamerDemolish::PartialRefund; }
	else if (Demolish == TEXT("off")) { Result.Demolish = EGamerDemolish::Disabled; }
	else if (!Demolish.IsEmpty()) { UE_LOG(LogGamer, Warning, TEXT("World option Demolish=%s unknown (full, partial, half, off)"), *Demolish); }

	const FString Inventory = Read(TEXT("Inventory"));
	if (Inventory == TEXT("world")) { Result.bCarryInventory = false; }
	else if (Inventory == TEXT("character")) { Result.bCarryInventory = true; }
	else if (!Inventory.IsEmpty()) { UE_LOG(LogGamer, Warning, TEXT("World option Inventory=%s unknown (world, character)"), *Inventory); }

	const FString Tempo = Read(TEXT("Tempo"));
	if (Tempo == TEXT("slow")) { Result.GespuerTempo = EGamerGespuerTempo::Slow; }
	else if (Tempo == TEXT("normal")) { Result.GespuerTempo = EGamerGespuerTempo::Normal; }
	else if (Tempo == TEXT("fast")) { Result.GespuerTempo = EGamerGespuerTempo::Fast; }
	else if (!Tempo.IsEmpty()) { UE_LOG(LogGamer, Warning, TEXT("World option Tempo=%s unknown (slow, normal, fast)"), *Tempo); }

	const FString Death = Read(TEXT("Death"));
	if (Death == TEXT("none")) { Result.DeathMode = EGamerDeathMode::None; }
	else if (Death == TEXT("grave")) { Result.DeathMode = EGamerDeathMode::Grave; }
	else if (Death == TEXT("hardcore")) { Result.DeathMode = EGamerDeathMode::Hardcore; }
	else if (!Death.IsEmpty()) { UE_LOG(LogGamer, Warning, TEXT("World option Death=%s unknown (none, grave, hardcore)"), *Death); }

	// Mode, difficulty and factors (2026-11-16).
	const FString Mode = Read(TEXT("Mode"));
	if (Mode == TEXT("saga")) { Result.PlayMode = EGamerPlayMode::Saga; }
	else if (Mode == TEXT("free")) { Result.PlayMode = EGamerPlayMode::Free; }
	else if (!Mode.IsEmpty()) { UE_LOG(LogGamer, Warning, TEXT("World option Mode=%s unknown (saga, free)"), *Mode); }

	const FString Level = Read(TEXT("Difficulty"));
	if (Level == TEXT("easy")) { Result.Difficulty = EGamerDifficulty::Easy; }
	else if (Level == TEXT("medium")) { Result.Difficulty = EGamerDifficulty::Medium; }
	else if (Level == TEXT("hard")) { Result.Difficulty = EGamerDifficulty::Hard; }
	else if (Level == TEXT("hardcore")) { Result.Difficulty = EGamerDifficulty::Hardcore; }
	else if (Level == TEXT("custom")) { Result.Difficulty = EGamerDifficulty::Custom; }
	else if (!Level.IsEmpty()) { UE_LOG(LogGamer, Warning, TEXT("World option Difficulty=%s unknown (easy, medium, hard, hardcore, custom)"), *Level); }

	auto ReadFactor = [&Read](const TCHAR* Key, float& Out)
	{
		const FString Text = Read(Key);
		if (Text.IsEmpty()) { return; }
		const float Value = FCString::Atof(*Text);
		if (Value >= 0.1f && Value <= 5.f) { Out = Value; }
		else { UE_LOG(LogGamer, Warning, TEXT("World option %s=%s out of range (0.1..5)"), Key, *Text); }
	};
	ReadFactor(TEXT("NeedsRate"), Result.NeedsRate);
	ReadFactor(TEXT("Damage"), Result.DamageTaken);
	ReadFactor(TEXT("Yield"), Result.YieldFactor);

	auto ReadSwitch = [&Read](const TCHAR* Key, bool& Out)
	{
		const FString Text = Read(Key);
		if (Text == TEXT("on")) { Out = true; }
		else if (Text == TEXT("off")) { Out = false; }
		else if (!Text.IsEmpty()) { UE_LOG(LogGamer, Warning, TEXT("World option %s=%s unknown (on, off)"), Key, *Text); }
	};
	ReadSwitch(TEXT("Hordes"), Result.bHordes);
	ReadSwitch(TEXT("Immortal"), Result.bVillagersImmortal);
	ReadSwitch(TEXT("FreeBuild"), Result.bFreeBuild);
	// "Sagenwelt" was removed on 2026-11-22 (saga bosses are standard). Old saves still contain ?Sagenwelt=...; keys that
	// are not read here are simply ignored.
	ReadSwitch(TEXT("FriendlyFire"), Result.bFriendlyFire);

	const FString Players = Read(TEXT("MaxPlayers"));
	if (!Players.IsEmpty())
	{
		const int32 Count = FCString::Atoi(*Players);
		if (Count >= 1 && Count <= 8) { Result.MaxPlayers = Count; }
		else { UE_LOG(LogGamer, Warning, TEXT("World option MaxPlayers=%s out of range (1..8)"), *Players); }
	}

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
		: Demolish == EGamerDemolish::HalfRefund ? TEXT("Abreißen: halbes Material")
		: Demolish == EGamerDemolish::PartialRefund ? TEXT("Abreißen: etwa 80 % Material") : TEXT("kein Abreißen");
	const TCHAR* Gespuer = GespuerTempo == EGamerGespuerTempo::Slow ? TEXT("langsam") : GespuerTempo == EGamerGespuerTempo::Fast ? TEXT("schnell") : TEXT("normal");
	const TCHAR* DeathText = DeathMode == EGamerDeathMode::None ? TEXT("nichts verlieren") : DeathMode == EGamerDeathMode::Hardcore ? TEXT("alles verlieren") : TEXT("Grab");
	static const TCHAR* const Levels[] = { TEXT("Einfach"), TEXT("Mittel"), TEXT("Schwer"), TEXT("Hardcore"), TEXT("Benutzerdefiniert") };
	return FString::Printf(TEXT("%s · %s · %s · %s · %s · Jahreszeit %g Min. · Inventar %s · Gespür %s · Tod: %s"),
		PlayMode == EGamerPlayMode::Saga ? TEXT("Saga") : TEXT("Freies Spiel"), Levels[FMath::Min<int32>(static_cast<int32>(Difficulty), 4)],
		Flow, Place, Tear, SeasonMinutes, bCarryInventory ? TEXT("mit dem Charakter") : TEXT("pro Welt"), Gespuer, DeathText);
}

FString FGamerWorldOptions::ToLogString() const
{
	const TCHAR* Flow = BuildFlow == EGamerBuildFlow::Blueprint ? TEXT("blueprint")
		: BuildFlow == EGamerBuildFlow::Instant ? TEXT("instant") : TEXT("material");
	const TCHAR* Place = Placement == EGamerPlacement::FreeSnap ? TEXT("free") : TEXT("grid");
	const TCHAR* Tear = Demolish == EGamerDemolish::FullRefund ? TEXT("full")
		: Demolish == EGamerDemolish::HalfRefund ? TEXT("half") : Demolish == EGamerDemolish::PartialRefund ? TEXT("partial") : TEXT("off");
	const TCHAR* Tempo = GespuerTempo == EGamerGespuerTempo::Slow ? TEXT("slow") : GespuerTempo == EGamerGespuerTempo::Fast ? TEXT("fast") : TEXT("normal");
	const TCHAR* DeathKey = DeathMode == EGamerDeathMode::None ? TEXT("none") : DeathMode == EGamerDeathMode::Hardcore ? TEXT("hardcore") : TEXT("grave");
	static const TCHAR* const Levels[] = { TEXT("easy"), TEXT("medium"), TEXT("hard"), TEXT("hardcore"), TEXT("custom") };
	auto OnOff = [](bool b) { return b ? TEXT("on") : TEXT("off"); };
	return FString::Printf(TEXT("BuildFlow=%s Placement=%s Demolish=%s SeasonMinutes=%g Inventory=%s Tempo=%s Death=%s")
		TEXT(" Mode=%s Difficulty=%s NeedsRate=%g Damage=%g Yield=%g Hordes=%s Immortal=%s FreeBuild=%s FriendlyFire=%s MaxPlayers=%d"),
		Flow, Place, Tear, SeasonMinutes, bCarryInventory ? TEXT("character") : TEXT("world"), Tempo, DeathKey,
		PlayMode == EGamerPlayMode::Saga ? TEXT("saga") : TEXT("free"), Levels[FMath::Min<int32>(static_cast<int32>(Difficulty), 4)],
		NeedsRate, DamageTaken, YieldFactor, OnOff(bHordes), OnOff(bVillagersImmortal), OnOff(bFreeBuild), OnOff(bFriendlyFire), MaxPlayers);
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
