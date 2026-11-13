#pragma once

#include "CoreMinimal.h"
#include "GamerDataLoader.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "GamerDataSubsystem.generated.h"

/**
 * Holds the game data tables (Content/Data/*.json) for the running game.
 * Loaded once at start on the server and on every client, so everybody uses the same numbers.
 * Network code sends only IDs and counts; the rows themselves are looked up here.
 */
UCLASS()
class GAMER_API UGamerDataSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	/** Reads all tables again (development; console: gamer.ReloadData). Keeps the old data if the new files have errors. */
	bool Reload();

	const FGamerDataSet& GetData() const { return Data; }
	uint32 GetChecksum() const { return Data.Checksum; }
	bool HasLoadErrors() const { return bHasLoadErrors; }

	const FGamerItemDef* FindItem(FName Id) const { return Data.Items.Find(Id); }
	const FGamerResourceNodeDef* FindResourceNode(FName Id) const { return Data.ResourceNodes.Find(Id); }
	const FGamerRecipeDef* FindRecipe(FName Id) const { return Data.Recipes.Find(Id); }
	const FGamerBuildableDef* FindBuildable(FName Id) const { return Data.Buildables.Find(Id); }
	const FGamerJobDef* FindJob(FName Id) const { return Data.Jobs.Find(Id); }
	const FGamerNeedDef* FindNeed(FName Id) const { return Data.Needs.Find(Id); }

	/** Convenience: the subsystem of the game instance that owns WorldContextObject. */
	static UGamerDataSubsystem* Get(const UObject* WorldContextObject);

private:
	FGamerDataSet Data;
	bool bHasLoadErrors = false;
};
