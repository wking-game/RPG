#pragma once

#include "CoreMinimal.h"

struct FGamerDataSet;
struct FGamerDataReport;

/**
 * Creatures, animals, spawn rules and loot (docs/21_kreaturen_level_kampf.md, docs/24_schritt1_daten_und_logik.md).
 * Plain C++ (no UObject), like FGamerDataSet, so tests can load it without a running game.
 * Tables: creatures.json, animals.json, spawns.json (with the list of biomes), creature_loot.json.
 */

enum class EGamerTemperament : uint8
{
	Passive,
	Defensive,
	Territorial,
	Hostile,
	Ambush
};

/** Base values of one rank (before the 85-115 % roll and the leader tiers). */
struct FGamerRankStats
{
	int32 Rank = 0;
	float Hp = 0.f;
	float Poise = 0.f;
	float Damage = 0.f;
};

/** One creature (kind "creature") or animal (kind "animal"). */
struct FGamerCreatureDef
{
	FName Id;
	/** Display name (German). */
	FString Name;
	/** Name in the Gemini document (for orientation only). */
	FString GeminiName;
	FString Role;
	bool bAnimal = false;
	EGamerTemperament Temperament = EGamerTemperament::Hostile;
	bool bPack = false;
	int32 MinRank = 1;
	FName HomeBiome;
	/** Model wave: 0 = core (Fjordwiesen), 1..5 = later biomes. */
	int32 Wave = 0;
	/** Sorted by rank. */
	TArray<FGamerRankStats> Ranks;

	const FGamerRankStats* FindRank(int32 Rank) const
	{
		return Ranks.FindByPredicate([Rank](const FGamerRankStats& S) { return S.Rank == Rank; });
	}
};

struct FGamerBiomeDef
{
	FName Id;
	FString Name;
	/** Danger rank of the biome (= tier T1..T6). */
	int32 Rank = 1;
	/** Rank at dangerous places (Rank + 1, at most 6). */
	int32 PlaceRank = 1;
	FString Places;
};

/** Where and how a creature appears in a biome. */
struct FGamerSpawnRule
{
	FName Biome;
	FName Creature;
	float Weight = 1.f;
	/** Only at dangerous places (rank = PlaceRank). */
	bool bOnlyPlaces = false;
	/** true: grows with the zone rank (ScaleChance = share that reaches it, the rest one rank lower). false: stays on FixedRank. */
	bool bScaleToZone = true;
	float ScaleChance = 1.f;
	/** 0 = no cap. */
	int32 MaxRank = 0;
	int32 FixedRank = 0;
};

/** One loot line. Either Item or ItemFamily is set (family = part per tier: "item_sinew" gives item_sinew_t<rank>). */
struct FGamerLootEntry
{
	FName Item;
	FName ItemFamily;
	int32 Min = 1;
	int32 Max = 1;
	float Chance = 1.f;
	/** Base and rank lines: lowest rank that rolls this line. 0 for biome lines. */
	int32 MinRank = 0;
	/** +1 piece per rank above MinRank (capped, see combat.json). */
	bool bScaleCount = false;
	/** Biome lines only. */
	TArray<FName> Biomes;

	/** The item this line gives at this rank (family lines add _t<rank>). */
	FName Resolve(int32 EffectiveRank) const
	{
		return ItemFamily.IsNone() ? Item : FName(*FString::Printf(TEXT("%s_t%d"), *ItemFamily.ToString(), EffectiveRank));
	}
};

struct FGamerCreatureLoot
{
	FName Id;
	TArray<FGamerLootEntry> Base;
	TArray<FGamerLootEntry> Rank;
	TArray<FGamerLootEntry> Biome;
};

struct GAMER_API FGamerCreatureDataSet
{
	/** Creatures and animals together, keyed by ID. */
	TMap<FName, FGamerCreatureDef> Creatures;
	TArray<FGamerBiomeDef> Biomes;
	TArray<FGamerSpawnRule> Spawns;
	TMap<FName, FGamerCreatureLoot> Loot;
	/** CRC32 over the four files (line endings ignored). */
	uint32 Checksum = 0;

	bool IsEmpty() const { return Creatures.Num() == 0; }
	const FGamerBiomeDef* FindBiome(FName Id) const
	{
		return Biomes.FindByPredicate([Id](const FGamerBiomeDef& B) { return B.Id == Id; });
	}

	/**
	 * Ranks this creature can have at all (or in one biome if BiomeId is set), from the spawn rules: grows with the zone, the
	 * share that does not reach the top rank one rank lower (at least MinRank), or the fixed rank.
	 */
	TSet<int32> ReachableRanks(FName CreatureId, FName BiomeId = NAME_None) const;
};

namespace GamerData
{
	inline const TCHAR* const CreaturesFile = TEXT("creatures.json");
	inline const TCHAR* const AnimalsFile = TEXT("animals.json");
	inline const TCHAR* const SpawnsFile = TEXT("spawns.json");
	inline const TCHAR* const CreatureLootFile = TEXT("creature_loot.json");

	/**
	 * Loads and checks creatures.json, animals.json, spawns.json and creature_loot.json against the item tables in Base.
	 * If creatures.json does not exist the set stays empty and that is fine (bRequired = false), e.g. test data without creatures.
	 * Returns true if there were no errors.
	 */
	GAMER_API bool LoadCreatures(const FString& Directory, const FGamerDataSet& Base, FGamerCreatureDataSet& Out, FGamerDataReport& Report,
		bool bRequired = false);

	/**
	 * Light read of creature_loot.json for the item reachability check: every item that creatures or animals can drop
	 * (family lines add item_<family>_t1..t6). Needs no other table. Missing file = nothing added.
	 */
	GAMER_API void ReadCreatureLootItems(const FString& Directory, TSet<FName>& OutItems);
}
