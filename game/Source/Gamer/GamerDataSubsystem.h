#pragma once

#include "CoreMinimal.h"
#include "GamerCharacterData.h"
#include "GamerCombatMath.h"
#include "GamerCreatureData.h"
#include "GamerProgression.h"
#include "GamerDifficulty.h"
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
	/** CRC over all data tables incl. creatures, combat and progression: equal on every machine with the same data. */
	uint32 GetChecksum() const;
	bool HasLoadErrors() const { return bHasLoadErrors; }

	const FGamerItemDef* FindItem(FName Id) const { return Data.Items.Find(Id); }
	const FGamerResourceNodeDef* FindResourceNode(FName Id) const { return Data.ResourceNodes.Find(Id); }
	const FGamerRecipeDef* FindRecipe(FName Id) const { return Data.Recipes.Find(Id); }
	const FGamerBuildableDef* FindBuildable(FName Id) const { return Data.Buildables.Find(Id); }
	const FGamerJobDef* FindJob(FName Id) const { return Data.Jobs.Find(Id); }
	const FGamerNeedDef* FindNeed(FName Id) const { return Data.Needs.Find(Id); }

	/** Character creation data (characters.json): origins, values, abilities, appearance options. */
	const FGamerCharacterDataSet& GetCharacters() const { return Characters; }

	/** Creatures, animals, spawn rules and loot (creatures.json, animals.json, spawns.json, creature_loot.json). Empty if the data set has none. */
	const FGamerCreatureDataSet& GetCreatures() const { return Creatures; }

	/** Combat constants and formulas data (combat.json). Defaults if the file is missing. */
	const FGamerCombatData& GetCombat() const { return Combat; }

	/** Player level 1-50, experience and equipment values per tier (progression.json). */
	const FGamerProgressionData& GetProgression() const { return Progression; }

	/** Difficulty presets (difficulty.json), sorted easy .. hardcore. */
	const TArray<FGamerDifficultyPreset>& GetDifficulties() const { return Difficulties; }

	/** Convenience: the subsystem of the game instance that owns WorldContextObject. */
	static UGamerDataSubsystem* Get(const UObject* WorldContextObject);

private:
	FGamerDataSet Data;
	FGamerCharacterDataSet Characters;
	FGamerCreatureDataSet Creatures;
	FGamerCombatData Combat;
	FGamerProgressionData Progression;
	TArray<FGamerDifficultyPreset> Difficulties;
	bool bHasLoadErrors = false;
};
