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
	LogReport(Report);
	LogSummary(TEXT("loaded"), Data, Report);
}

bool UGamerDataSubsystem::Reload()
{
	FGamerDataSet NewData;
	FGamerDataReport Report;
	const bool bOk = GamerData::LoadFromDirectory(GamerData::DefaultDirectory(), NewData, Report);
	LogReport(Report);
	if (!bOk)
	{
		UE_LOG(LogGamer, Error, TEXT("Data reload failed, keeping the previous data (checksum %08X)."), Data.Checksum);
		return false;
	}
	Data = MoveTemp(NewData);
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
