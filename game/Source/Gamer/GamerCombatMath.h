#pragma once

#include "CoreMinimal.h"

struct FGamerDataReport;

/**
 * Combat numbers and formulas (docs/21_kreaturen_level_kampf.md, Abschnitte 4 bis 6).
 * Constants live in combat.json. All functions are pure (no UObject, no randomness) so tests can check them exactly.
 */

enum class EGamerHitZone : uint8
{
	Head,
	Body,
	Limbs
};

/** Which equipment slots protect a zone, and how much more damage a hit there does. */
struct FGamerZoneDef
{
	TArray<FName> Slots;
	float DamageFactor = 1.f;
};

struct FGamerAttackDef
{
	FName Id;
	float Damage = 1.f;
	float Poise = 1.f;
};

/** Leader tiers instead of captain ranks above 6 (docs/21, Abschnitt 6.4). */
struct FGamerLeaderDef
{
	FName Id;
	int32 MinRank = 2;
	/** Share of the spawns (everywhere / at dangerous places). */
	float Chance = 0.f;
	float PlaceChance = 0.f;
	float Hp = 1.f;
	float Poise = 1.f;
	float Dmg = 1.f;
	float Size = 1.f;
	/** Extra rolls of the base and rank loot lists; rank loot counts are multiplied by RankRollMult. */
	int32 ExtraBaseRolls = 0;
	int32 ExtraRankRolls = 0;
	int32 RankRollMult = 1;
};

struct GAMER_API FGamerCombatData
{
	// armor: reduction = min(MaxReduction, armor / (armor + K)); damage never below MinDamage
	float ArmorK = 160.f;
	float ArmorMaxReduction = 0.75f;
	float MinDamage = 1.f;

	// resistances: clamped to [ResistMin, ResistMax]
	float ResistMin = -0.25f;
	float ResistMax = 0.60f;

	// poise
	float PoiseKPlayer = 90.f;
	float StaggerSeconds = 2.5f;
	float BossStaggerSeconds = 3.f;
	float StaggerDamageMult = 1.5f;
	float DecayDelaySeconds = 3.f;
	float DecayFractionPerSecond = 0.25f;
	float PlayerPoiseMax = 100.f;
	float PlayerStaggerSeconds = 0.8f;
	float PlayerRegenDelaySeconds = 2.f;
	float PlayerRegenPerSecond = 30.f;
	float WeakPointPoiseMult = 1.5f;

	/** Share of the set value each slot carries (sums to 1). */
	TMap<FName, float> SlotShare;
	/** Indexed by EGamerHitZone. */
	FGamerZoneDef Zones[3];
	TArray<FGamerAttackDef> Attacks;

	// spawn roll: one roll for hp, poise and damage; the size follows (size_follow = share of the deviation)
	float SpreadMin = 0.85f;
	float SpreadMax = 1.15f;
	float SizeFollow = 0.5f;
	TArray<FGamerLeaderDef> Leaders;

	// loot
	int32 LootCountBonusCap = 2;
	float LootDownscale1 = 0.85f;
	float LootDownscale2 = 0.70f;
	float LootDownscale3 = 0.55f;

	/** Scales the hit points of all creatures at once (tuning after the first fight test). */
	float CreatureHpScale = 1.f;

	// player hit points: base + per_toughness * (toughness - neutral) + meal bonus
	float PlayerBaseHp = 100.f;
	float PlayerHpPerToughness = 5.f;
	int32 ToughnessNeutral = 5;

	uint32 Checksum = 0;
	bool bLoaded = false;

	const FGamerAttackDef* FindAttack(FName Id) const
	{
		return Attacks.FindByPredicate([Id](const FGamerAttackDef& A) { return A.Id == Id; });
	}
	const FGamerZoneDef& GetZone(EGamerHitZone Zone) const { return Zones[static_cast<int32>(Zone)]; }
};

namespace GamerData
{
	inline const TCHAR* const CombatFile = TEXT("combat.json");

	/** Loads and checks combat.json. Missing file: defaults stay and that is fine unless bRequired. Returns true without errors. */
	GAMER_API bool LoadCombat(const FString& Directory, FGamerCombatData& Out, FGamerDataReport& Report, bool bRequired = false);
}

namespace GamerCombat
{
	/** armor / (armor + K), capped. Negative armor counts as 0. */
	GAMER_API float ArmorDamageReduction(float Armor, const FGamerCombatData& Data);

	/** Damage after armor: max(MinDamage, raw * (1 - reduction)). */
	GAMER_API float ApplyArmor(float RawDamage, float Armor, const FGamerCombatData& Data);

	/** Resistance clamped to the allowed range. */
	GAMER_API float ClampResistance(float TotalResistance, const FGamerCombatData& Data);

	/** Hazard effect after resistance: raw * (1 - clamped resistance). */
	GAMER_API float ApplyResistance(float RawEffect, float TotalResistance, const FGamerCombatData& Data);

	/** Poise damage a creature does to the player: base * attack mult * K / (resist + K). */
	GAMER_API float PlayerPoiseDamageTaken(float EnemyPoiseDamage, float AttackPoiseMult, float PlayerPoiseResist, const FGamerCombatData& Data);

	/** Poise damage the player does to a creature: weapon * attack mult * (weak point mult if hit on a weak point). */
	GAMER_API float EnemyPoiseDamageTaken(float WeaponPoiseDamage, float AttackPoiseMult, bool bWeakPoint, const FGamerCombatData& Data);

	/**
	 * Armor value of one zone from the armor of the single slots: sum of the slot values divided by the sum of the shares of the zone's
	 * slots. A full set gives the set value in every zone; a missing slot lowers it (no helmet = head without protection).
	 */
	GAMER_API float ZoneArmor(EGamerHitZone Zone, const TMap<FName, float>& SlotArmor, const FGamerCombatData& Data);

	/** Damage factor of the hit zone (head more, limbs less). */
	GAMER_API float ZoneDamageFactor(EGamerHitZone Zone, const FGamerCombatData& Data);

	/** Hit points of a player: base + per_toughness * (toughness - neutral) + meal bonus; never below 1. */
	GAMER_API float PlayerMaxHp(int32 Toughness, float MealBonus, const FGamerCombatData& Data);

	/** Hit points of a creature after the spawn roll and leader tier: base * hp_scale * roll * leader. */
	GAMER_API float CreatureHp(float BaseHp, float Roll, float LeaderFactor, const FGamerCombatData& Data);
}

/** Poise bar of one creature (or the player): fills with hits, breaks into a stagger, empties, recovers slowly. */
struct GAMER_API FGamerPoiseMeter
{
	float Current = 0.f;
	float SecondsSinceHit = 0.f;
	float StaggerLeft = 0.f;

	bool IsStaggered() const { return StaggerLeft > 0.f; }

	/** Hits during a stagger do more damage. */
	float DamageMultiplier(const FGamerCombatData& Data) const { return IsStaggered() ? Data.StaggerDamageMult : 1.f; }

	/** Adds poise damage. Returns true if this hit broke the poise (bar reset to 0, stagger starts). */
	bool AddDamage(float Amount, float Max, bool bBoss, const FGamerCombatData& Data);

	/** Time passes: the stagger runs out, and after the delay the bar empties by a fraction of Max per second. */
	void Tick(float DeltaSeconds, float Max, const FGamerCombatData& Data);
};
