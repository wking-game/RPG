#pragma once

#include "CoreMinimal.h"
#include "GamerDataTypes.h"

/** All game data tables, keyed by ID. Plain C++ so tests can load it without a running game. */
struct GAMER_API FGamerDataSet
{
	TMap<FName, FGamerItemDef> Items;
	TMap<FName, FGamerResourceNodeDef> ResourceNodes;
	TMap<FName, FGamerRecipeDef> Recipes;
	TMap<FName, FGamerBuildableDef> Buildables;
	TMap<FName, FGamerJobDef> Jobs;
	TMap<FName, FGamerNeedDef> Needs;

	/** CRC32 over the file contents (line endings ignored). Equal on every machine with the same data. */
	uint32 Checksum = 0;

	void Reset();
};

/** Problems found while loading. Errors = data is wrong; warnings = worth a look (e.g. model not imported yet). */
struct GAMER_API FGamerDataReport
{
	TArray<FString> Errors;
	TArray<FString> Warnings;

	bool HasErrors() const { return Errors.Num() > 0; }
};

namespace GamerData
{
	/** Value of "schema" this code understands. */
	inline constexpr int32 SchemaVersion = 1;

	/** Highest tier (T6 Relikt). */
	inline constexpr int32 MaxTier = 6;

	/** Data folder of the project: <Project>/Content/Data. */
	GAMER_API FString DefaultDirectory();

	/**
	 * Loads and checks all tables in Directory (items, resource_nodes, recipes, buildables; jobs and needs if present).
	 * Fills Out as far as possible even if there are errors. Returns true if there were no errors.
	 * bCheckAssetPaths: warn when a model/icon path points to an asset that does not exist.
	 */
	GAMER_API bool LoadFromDirectory(const FString& Directory, FGamerDataSet& Out, FGamerDataReport& Report, bool bCheckAssetPaths = true);
}
