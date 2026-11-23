#include "GamerDataSubsystem.h"

#include "Gamer.h"
#include "Engine/Engine.h"
#include "Engine/GameInstance.h"
#include "Engine/World.h"
#include "HAL/IConsoleManager.h"

namespace
{
	void LogReport(const FGamerDataReport& Report)
	{
		for (const FString& Error : Report.Errors)
		{
			UE_LOG(LogGamer, Error, TEXT("Data: %s"), *Error);
		}
		for (const FString& Warning : Report.Warnings)
		{
			UE_LOG(LogGamer, Warning, TEXT("Data: %s"), *Warning);
		}
	}

	void LogSummary(const TCHAR* What, const FGamerDataSet& Data, const FGamerDataReport& Report)
	{
		UE_LOG(LogGamer, Log, TEXT("Data %s: %d items, %d resource nodes, %d recipes, %d buildables, %d jobs, %d needs, checksum %08X, %d errors, %d warnings"),
			What, Data.Items.Num(), Data.ResourceNodes.Num(), Data.Recipes.Num(), Data.Buildables.Num(), Data.Jobs.Num(), Data.Needs.Num(),
			Data.Checksum, Report.Errors.Num(), Report.Warnings.Num());
	}

	FAutoConsoleCommand ReloadDataCommand(
		TEXT("gamer.ReloadData"),
		TEXT("Reads Content/Data/*.json again (only on this machine; for development)."),
		FConsoleCommandDelegate::CreateLambda([]()
		{
			if (!GEngine)
			{
				return;
			}
			for (const FWorldContext& Context : GEngine->GetWorldContexts())
			{
				if (UGameInstance* GameInstance = Context.OwningGameInstance)
				{
					if (UGamerDataSubsystem* Subsystem = GameInstance->GetSubsystem<UGamerDataSubsystem>())
					{
						Subsystem->Reload();
					}
				}
			}
		}));
}

void UGamerDataSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	FGamerDataReport Report;
	bHasLoadErrors = !GamerData::LoadFromDirectory(GamerData::DefaultDirectory(), Data, Report);
	bHasLoadErrors |= !GamerData::LoadCharacters(GamerData::DefaultDirectory(), Data, Characters, Report);
	bHasLoadErrors |= !GamerData::LoadDifficulty(GamerData::DefaultDirectory(), Difficulties, Report);
	bHasLoadErrors |= !GamerData::LoadCreatures(GamerData::DefaultDirectory(), Data, Creatures, Report);
	bHasLoadErrors |= !GamerData::LoadCombat(GamerData::DefaultDirectory(), Combat, Report);
	bHasLoadErrors |= !GamerData::LoadProgression(GamerData::DefaultDirectory(), Progression, Report);
	LogReport(Report);
	LogSummary(TEXT("loaded"), Data, Report);
	UE_LOG(LogGamer, Log, TEXT("Creature data: %d creatures and animals, %d spawn rules, %d loot tables, %d biomes"),
		Creatures.Creatures.Num(), Creatures.Spawns.Num(), Creatures.Loot.Num(), Creatures.Biomes.Num());
	UE_LOG(LogGamer, Log, TEXT("Character data: %d origins, %d stats, %d abilities, %d appearance slots"),
		Characters.Origins.Num(), Characters.Stats.Num(), Characters.Abilities.Num(), Characters.LookSlots.Num());
}

uint32 UGamerDataSubsystem::GetChecksum() const
{
	return HashCombineFast(HashCombineFast(HashCombineFast(Data.Checksum, Creatures.Checksum), Combat.Checksum), Progression.Checksum);
}

bool UGamerDataSubsystem::Reload()
{
	FGamerDataSet NewData;
	FGamerDataReport Report;
	FGamerCharacterDataSet NewCharacters;
	bool bOk = GamerData::LoadFromDirectory(GamerData::DefaultDirectory(), NewData, Report);
	bOk &= GamerData::LoadCharacters(GamerData::DefaultDirectory(), NewData, NewCharacters, Report);
	TArray<FGamerDifficultyPreset> NewDifficulties;
	bOk &= GamerData::LoadDifficulty(GamerData::DefaultDirectory(), NewDifficulties, Report);
	FGamerCreatureDataSet NewCreatures;
	bOk &= GamerData::LoadCreatures(GamerData::DefaultDirectory(), NewData, NewCreatures, Report);
	FGamerCombatData NewCombat;
	bOk &= GamerData::LoadCombat(GamerData::DefaultDirectory(), NewCombat, Report);
	FGamerProgressionData NewProgression;
	bOk &= GamerData::LoadProgression(GamerData::DefaultDirectory(), NewProgression, Report);
	LogReport(Report);
	if (!bOk)
	{
		UE_LOG(LogGamer, Error, TEXT("Data reload failed, keeping the previous data (checksum %08X)."), Data.Checksum);
		return false;
	}
	Data = MoveTemp(NewData);
	Characters = MoveTemp(NewCharacters);
	Creatures = MoveTemp(NewCreatures);
	Combat = MoveTemp(NewCombat);
	Progression = MoveTemp(NewProgression);
	Difficulties = MoveTemp(NewDifficulties);
	bHasLoadErrors = false;
	LogSummary(TEXT("reloaded"), Data, Report);
	return true;
}

UGamerDataSubsystem* UGamerDataSubsystem::Get(const UObject* WorldContextObject)
{
	const UWorld* World = GEngine ? GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::ReturnNull) : nullptr;
	const UGameInstance* GameInstance = World ? World->GetGameInstance() : nullptr;
	return GameInstance ? GameInstance->GetSubsystem<UGamerDataSubsystem>() : nullptr;
}
