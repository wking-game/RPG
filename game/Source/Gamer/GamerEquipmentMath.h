#pragma once

#include "CoreMinimal.h"
#include "GamerCombatMath.h"
#include "GamerDataTypes.h"

/**
 * Values of the worn armor (docs/21 5.1/5.2, docs/25). Pure functions on item definitions, no UObject, so tests can check them
 * exactly. The equipment component keeps what is worn; this adds it up and applies it to damage.
 */

/** What all worn pieces give together. */
struct GAMER_API FGamerEquipmentTotals
{
	/** Armor per slot (helmet, shoulder, chest, hands, legs, feet); rings and cloaks give none to the zones. */
	TMap<FName, float> SlotArmor;
	float PoiseResist = 0.f;
	/** Sum of the resistances per kind (heat, cold, storm, poison), before buffs and clamping. */
	TMap<FName, float> Resist;
	int32 Pieces = 0;
	float WeightKg = 0.f;

	float GetResist(FName Kind) const
	{
		const float* Found = Resist.Find(Kind);
		return Found ? *Found : 0.f;
	}
};

namespace GamerEquipment
{
	/** How many pieces fit one slot (2 for rings, otherwise 1; 0 for a slot name that does not exist). */
	GAMER_API int32 SlotCapacity(FName Slot);

	/** Adds up worn pieces (null entries and pieces that are not armor are ignored). Two pieces in one slot add up; the component prevents that. */
	GAMER_API FGamerEquipmentTotals Summarize(const TArray<const FGamerItemDef*>& Worn);

	/** Armor of a hit zone from the totals (see GamerCombat::ZoneArmor). */
	GAMER_API float ZoneArmor(const FGamerEquipmentTotals& Totals, EGamerHitZone Zone, const FGamerCombatData& Data);

	/** Damage of one hit on a zone: raw * zone factor, reduced by the armor of that zone (never below the minimum damage). */
	GAMER_API float HitDamage(float RawDamage, EGamerHitZone Zone, const FGamerEquipmentTotals& Totals, const FGamerCombatData& Data);

	/** Effect of a hazard of one kind after resistance: equipment + buff resistance, clamped (see GamerCombat::ApplyResistance). */
	GAMER_API float HazardEffect(float RawEffect, FName Kind, const FGamerEquipmentTotals& Totals, float BuffResist, const FGamerCombatData& Data);
}
