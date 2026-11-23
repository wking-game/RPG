#pragma once

#include "CoreMinimal.h"
#include "GamerCombatMath.h"
#include "GamerCreatureData.h"

/**
 * Dice for creatures: which creature appears where, at which rank, how strong, and what it drops
 * (docs/21_kreaturen_level_kampf.md, Abschnitte 6 und 10). Pure functions on a random stream, so tests can repeat every roll.
 * The server rolls (architecture rule 1); clients only see the result.
 */

struct FGamerRolledItem
{
	FName Item;
	int32 Count = 0;
	bool bFromBiome = false;
};

/** Result of one spawn roll. LeaderIndex indexes FGamerCombatData::Leaders, -1 = normal. */
struct FGamerSpawnRoll
{
	FName Creature;
	int32 Rank = 0;
	int32 LeaderIndex = -1;
	/** The one roll (85-115 %) that scales hit points, poise and damage together. */
	float Roll = 1.f;
	float Hp = 0.f;
	float Poise = 0.f;
	float Damage = 0.f;
	/** Size factor: follows the roll and the leader tier. */
	float Size = 1.f;
};

namespace GamerCreature
{
	/** Picks one spawn rule of the biome by weight (rules only for dangerous places count only there). Null if none fits. */
	GAMER_API const FGamerSpawnRule* PickSpawnRule(const FGamerCreatureDataSet& Set, FName Biome, bool bAtPlace, FRandomStream& Stream);

	/** Rank of one exemplar: fixed rank, or the zone rank (capped); the share that does not reach it is one rank lower (at least MinRank). */
	GAMER_API int32 RollRank(const FGamerCreatureDef& Def, const FGamerBiomeDef& Biome, const FGamerSpawnRule& Rule, bool bAtPlace, FRandomStream& Stream);

	/** Index of the leader tier (FGamerCombatData::Leaders) or -1. Only tiers whose min_rank is reached count; the chances are disjoint. */
	GAMER_API int32 RollLeader(const FGamerCombatData& Combat, int32 Rank, bool bAtPlace, FRandomStream& Stream);

	/** Full spawn: rule, rank, spread roll, leader tier, final values. False if the biome has no rule that fits. */
	GAMER_API bool RollSpawn(const FGamerCreatureDataSet& Set, const FGamerCombatData& Combat, FName Biome, bool bAtPlace, FRandomStream& Stream, FGamerSpawnRoll& Out);

	/**
	 * Loot of one creature. EffectiveRank = rank of the exemplar, ZoneRank = rank of the place it spawned at (the difference dampens
	 * biome bonus loot only). YieldFactor comes from the difficulty. LeaderIndex adds rolls (-1 = none).
	 */
	GAMER_API TArray<FGamerRolledItem> RollLoot(const FGamerCreatureLoot& Loot, int32 EffectiveRank, int32 ZoneRank, FName BiomeId,
		int32 LeaderIndex, float YieldFactor, const FGamerCombatData& Combat, FRandomStream& Stream);

	/** Chance factor of biome bonus loot for a straggler: 1 / 0.85 / 0.70 / 0.55 for a rank difference of 0 / 1 / 2 / 3+. */
	GAMER_API float BiomeLootDownscale(int32 RankDifference, const FGamerCombatData& Combat);
}
