#include "GamerEquipmentMath.h"
#include "GamerDataLoader.h"

int32 GamerEquipment::SlotCapacity(FName Slot)
{
	if (Slot == TEXT("ring"))
	{
		return 2;
	}
	return GamerData::ArmorSlots().Contains(Slot) ? 1 : 0;
}

FGamerEquipmentTotals GamerEquipment::Summarize(const TArray<const FGamerItemDef*>& Worn)
{
	FGamerEquipmentTotals Totals;
	for (const FGamerItemDef* Item : Worn)
	{
		if (!Item || Item->Category != EGamerItemCategory::Armor)
		{
			continue;
		}
		++Totals.Pieces;
		Totals.WeightKg += Item->WeightKg;
		Totals.PoiseResist += Item->Armor.PoiseResist;
		if (Item->Armor.Armor > 0.f)
		{
			Totals.SlotArmor.FindOrAdd(Item->Armor.Slot) += Item->Armor.Armor;
		}
		for (const TPair<FName, float>& Pair : Item->Armor.Resist)
		{
			Totals.Resist.FindOrAdd(Pair.Key) += Pair.Value;
		}
	}
	return Totals;
}

float GamerEquipment::ZoneArmor(const FGamerEquipmentTotals& Totals, EGamerHitZone Zone, const FGamerCombatData& Data)
{
	return GamerCombat::ZoneArmor(Zone, Totals.SlotArmor, Data);
}

float GamerEquipment::HitDamage(float RawDamage, EGamerHitZone Zone, const FGamerEquipmentTotals& Totals, const FGamerCombatData& Data)
{
	const float Scaled = RawDamage * GamerCombat::ZoneDamageFactor(Zone, Data);
	return GamerCombat::ApplyArmor(Scaled, ZoneArmor(Totals, Zone, Data), Data);
}

float GamerEquipment::HazardEffect(float RawEffect, FName Kind, const FGamerEquipmentTotals& Totals, float BuffResist, const FGamerCombatData& Data)
{
	return GamerCombat::ApplyResistance(RawEffect, Totals.GetResist(Kind) + BuffResist, Data);
}
