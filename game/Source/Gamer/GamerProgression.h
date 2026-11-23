#pragma once

#include "CoreMinimal.h"

struct FGamerDataReport;

/**
 * Player level 1-50, experience and equipment values per tier (decisions 2026-11-22, docs/21_kreaturen_level_kampf.md,
 * Abschnitte 2 und 3). Data in progression.json. All functions are pure so tests can check them exactly.
 * Nothing here is used in the game yet: levels, experience and the skill tree come with the fight (step 2).
 */

/** One tier (T1..T6): level range, the biome it belongs to and the equipment values that fit it. */
struct FGamerTierDef
{
	int32 Tier = 1;
	FName Biome;
	int32 LevelMin = 1;
	int32 LevelMax = 1;
	/** Planned play time in this tier (solo), the basis of the experience curve. */
	float Hours = 0.f;
	/** Ranges {min, max} for a standard one-handed weapon and a full medium armor set. */
	int32 WeaponDamage[2] = { 0, 0 };
	int32 WeaponPoise[2] = { 0, 0 };
	int32 Armor[2] = { 0, 0 };
	int32 PoiseResist[2] = { 0, 0 };
	/** Highest resistance per type that gear can reach in this tier. */
	float ResistCap = 0.f;
};

struct GAMER_API FGamerProgressionData
{
	int32 MaxLevel = 50;
	TArray<FGamerTierDef> Tiers;
	/** Experience from level i+1 to i+2 (index 0 = level 1 to 2). MaxLevel - 1 entries. */
	TArray<int32> XpToNext;

	int32 SkillPointsPerLevel = 1;
	int32 SkillPointBonusEvery = 5;
	int32 SkillPointBonus = 1;

	// experience sources (per tier of the source unless noted)
	float KillPerRank = 10.f;
	TArray<float> LeaderMultipliers;
	float BossFirstWinPerTier = 300.f;
	float QuestStagePerTier = 50.f;
	float DiscoveryPerTier = 25.f;
	float FirstCraftPerTier = 20.f;
	float RepeatCraftPerTier = 1.f;
	float BuildPiecePerTier = 2.f;
	float GatherPerTier = 0.5f;

	// co-op: players within the radius share; share of combat, quest and discovery vs. work (craft, build, gather)
	float CoopRadiusCm = 5000.f;
	float CoopShareCombat = 1.f;
	float CoopShareWork = 0.25f;

	/** Experience factor for opponents below the own tier: 0, 1, 2, 3+ tiers below. */
	TArray<float> UnderLevelFactors;

	/** Experience per tier-hour of a "typical hour" (the mix is documented in progression.json). */
	float TypicalHourXpPerTier = 0.f;

	uint32 Checksum = 0;
	bool bLoaded = false;

	/** Experience needed to reach Level from level 1 (0 for level 1). Levels above the maximum count as the maximum. */
	int64 TotalXpForLevel(int32 Level) const;

	/** Level for a total experience (1 .. MaxLevel). */
	int32 LevelForXp(int64 TotalXp) const;

	/** Experience from this level to the next (0 at the maximum level). */
	int32 XpToNextLevel(int32 Level) const;

	/** The tier a level belongs to, or null. */
	const FGamerTierDef* TierForLevel(int32 Level) const;

	/** Skill points earned in total by reaching this level (1 per level, a bonus every 5th level: 59 at level 50). */
	int32 SkillPointsForLevel(int32 Level) const;

	/** Factor for killing something TiersBelow tiers below the own tier (0 = same tier or higher = 1.0). */
	float UnderLevelFactor(int32 TiersBelow) const;

	/** Experience for a kill: rank * KillPerRank * leader multiplier (LeaderIndex -1 = normal). */
	float KillXp(int32 Rank, int32 LeaderIndex) const;

	/** Experience for making a recipe: the first time 20 x tier (plus 1 x tier for each further piece), afterwards 1 x tier per piece. */
	float CraftXp(int32 Tier, int32 Count, bool bFirstTime) const;

	/** Experience for gathered pieces: 0.5 x tier each. */
	float GatherXp(int32 Tier, int32 Count) const;

	/** Experience for a finished building piece: 2 x tier. */
	float BuildXp(int32 Tier) const;
};

namespace GamerData
{
	inline const TCHAR* const ProgressionFile = TEXT("progression.json");

	/** Loads and checks progression.json. Missing file: empty set and that is fine unless bRequired. Returns true without errors. */
	GAMER_API bool LoadProgression(const FString& Directory, FGamerProgressionData& Out, FGamerDataReport& Report, bool bRequired = false);
}
