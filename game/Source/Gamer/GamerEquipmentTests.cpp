#include "GamerEquipmentMath.h"
#include "GamerDataLoader.h"
#include "GamerProgression.h"

#include "HAL/FileManager.h"
#include "Misc/AutomationTest.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"

#if WITH_DEV_AUTOMATION_TESTS

// Run with: tools/test_data.ps1 -Filter Gamer.Equipment

namespace
{
	FGamerItemDef MakeArmor(const TCHAR* Id, const TCHAR* Slot, float Armor, float Poise, float WeightKg)
	{
		FGamerItemDef Def;
		Def.Id = Id;
		Def.Category = EGamerItemCategory::Armor;
		Def.WeightKg = WeightKg;
		Def.Armor.Slot = Slot;
		Def.Armor.Armor = Armor;
		Def.Armor.PoiseResist = Poise;
		return Def;
	}

	FGamerCombatData LoadCombatData(FAutomationTestBase& Test)
	{
		FGamerCombatData Data;
		FGamerDataReport Report;
		GamerData::LoadCombat(GamerData::DefaultDirectory(), Data, Report, true);
		for (const FString& Error : Report.Errors) { Test.AddError(Error); }
		return Data;
	}
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FGamerEquipmentTotalsTest, "Gamer.Equipment.Totals",
	EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::EngineFilter)

bool FGamerEquipmentTotalsTest::RunTest(const FString& Parameters)
{
	const FGamerCombatData Combat = LoadCombatData(*this);

	// A full set (shares .2 .1 .4 .1 .15 .05 of the set value 100) gives 100 in every zone.
	const FGamerItemDef Helmet = MakeArmor(TEXT("armor_h"), TEXT("helmet"), 20.f, 2.f, 1.f);
	const FGamerItemDef Shoulder = MakeArmor(TEXT("armor_s"), TEXT("shoulder"), 10.f, 1.f, 1.f);
	const FGamerItemDef Chest = MakeArmor(TEXT("armor_c"), TEXT("chest"), 40.f, 4.f, 3.f);
	const FGamerItemDef Hands = MakeArmor(TEXT("armor_g"), TEXT("hands"), 10.f, 1.f, 0.5f);
	const FGamerItemDef Legs = MakeArmor(TEXT("armor_l"), TEXT("legs"), 15.f, 1.f, 1.5f);
	const FGamerItemDef Feet = MakeArmor(TEXT("armor_f"), TEXT("feet"), 5.f, 1.f, 0.5f);
	FGamerItemDef Cloak = MakeArmor(TEXT("armor_k"), TEXT("cloak"), 0.f, 0.f, 1.f);
	Cloak.Armor.Resist.Add(TEXT("cold"), 0.05f);
	FGamerItemDef Ring1 = MakeArmor(TEXT("armor_r1"), TEXT("ring"), 0.f, 0.f, 0.1f);
	Ring1.Armor.Resist.Add(TEXT("cold"), 0.03f);
	Ring1.Armor.Resist.Add(TEXT("poison"), 0.02f);
	FGamerItemDef Ring2 = MakeArmor(TEXT("armor_r2"), TEXT("ring"), 0.f, 0.f, 0.1f);
	Ring2.Armor.Resist.Add(TEXT("cold"), 0.04f);
	const FGamerItemDef Sword; // not armor: ignored

	TArray<const FGamerItemDef*> Worn = { &Helmet, &Shoulder, &Chest, &Hands, &Legs, &Feet, &Cloak, &Ring1, &Ring2, &Sword, nullptr };
	const FGamerEquipmentTotals Totals = GamerEquipment::Summarize(Worn);

	TestEqual(TEXT("nine pieces counted"), Totals.Pieces, 9);
	TestEqual(TEXT("poise resist adds up"), Totals.PoiseResist, 10.f);
	TestEqual(TEXT("weight adds up"), Totals.WeightKg, 8.7f, 0.001f);
	TestEqual(TEXT("cold from cloak and both rings"), Totals.GetResist(TEXT("cold")), 0.12f, 0.0001f);
	TestEqual(TEXT("poison from one ring"), Totals.GetResist(TEXT("poison")), 0.02f, 0.0001f);
	TestEqual(TEXT("no heat"), Totals.GetResist(TEXT("heat")), 0.f);
	TestFalse(TEXT("cloak and rings give no slot armor"), Totals.SlotArmor.Contains(TEXT("cloak")) || Totals.SlotArmor.Contains(TEXT("ring")));

	for (const EGamerHitZone Zone : { EGamerHitZone::Head, EGamerHitZone::Body, EGamerHitZone::Limbs })
	{
		TestEqual(TEXT("full set gives the set value in every zone"), GamerEquipment::ZoneArmor(Totals, Zone, Combat), 100.f, 0.01f);
	}

	// No helmet: the head is unprotected, the body stays covered.
	TArray<const FGamerItemDef*> NoHelmet = { &Shoulder, &Chest, &Hands, &Legs, &Feet };
	const FGamerEquipmentTotals Partial = GamerEquipment::Summarize(NoHelmet);
	TestEqual(TEXT("no helmet: head armor 0"), GamerEquipment::ZoneArmor(Partial, EGamerHitZone::Head, Combat), 0.f);
	TestEqual(TEXT("no helmet: body armor stays"), GamerEquipment::ZoneArmor(Partial, EGamerHitZone::Body, Combat), 100.f, 0.01f);

	// Nothing worn: raw damage times the zone factor.
	const FGamerEquipmentTotals None = GamerEquipment::Summarize({});
	TestEqual(TEXT("naked head takes 1.5x"), GamerEquipment::HitDamage(10.f, EGamerHitZone::Head, None, Combat), 15.f, 0.001f);
	TestEqual(TEXT("naked limbs take 0.7x"), GamerEquipment::HitDamage(10.f, EGamerHitZone::Limbs, None, Combat), 7.f, 0.001f);

	// Armor 100 of K 160 reduces by 100/260; the head hit is raw * 1.5 first.
	const float Expected = 10.f * 1.5f * (1.f - 100.f / 260.f);
	TestEqual(TEXT("armored head"), GamerEquipment::HitDamage(10.f, EGamerHitZone::Head, Totals, Combat), Expected, 0.01f);
	TestTrue(TEXT("armor never raises damage"), GamerEquipment::HitDamage(10.f, EGamerHitZone::Body, Totals, Combat) < 10.f);
	TestTrue(TEXT("minimum damage"), GamerEquipment::HitDamage(0.1f, EGamerHitZone::Body, Totals, Combat) >= Combat.MinDamage);

	// Hazards: equipment 12 % cold + buff 10 % = 22 % off; clamped at 60 %.
	TestEqual(TEXT("cold hazard"), GamerEquipment::HazardEffect(10.f, TEXT("cold"), Totals, 0.10f, Combat), 7.8f, 0.001f);
	TestEqual(TEXT("clamped at 60 %"), GamerEquipment::HazardEffect(10.f, TEXT("cold"), Totals, 0.9f, Combat), 4.f, 0.001f);

	TestEqual(TEXT("ring capacity"), GamerEquipment::SlotCapacity(TEXT("ring")), 2);
	TestEqual(TEXT("helmet capacity"), GamerEquipment::SlotCapacity(TEXT("helmet")), 1);
	TestEqual(TEXT("unknown slot"), GamerEquipment::SlotCapacity(TEXT("tail")), 0);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FGamerEquipmentShippedTest, "Gamer.Equipment.ShippedArmorFitsTiers",
	EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::EngineFilter)

bool FGamerEquipmentShippedTest::RunTest(const FString& Parameters)
{
	// Every shipped armor piece: valid slot, and per tier a full set of the best pieces stays inside docs/21 2.2 (progression.json).
	FGamerDataSet Data;
	FGamerDataReport Report;
	GamerData::LoadFromDirectory(GamerData::DefaultDirectory(), Data, Report);
	for (const FString& Error : Report.Errors) { AddError(Error); }
	FGamerProgressionData Progression;
	FGamerDataReport ProgressionReport;
	GamerData::LoadProgression(GamerData::DefaultDirectory(), Progression, ProgressionReport, true);
	const FGamerCombatData Combat = LoadCombatData(*this);

	int32 ArmorCount = 0;
	for (const TPair<FName, FGamerItemDef>& Pair : Data.Items)
	{
		if (Pair.Value.Category == EGamerItemCategory::Armor)
		{
			++ArmorCount;
			TestTrue(FString::Printf(TEXT("%s has a known slot"), *Pair.Key.ToString()), GamerEquipment::SlotCapacity(Pair.Value.Armor.Slot) > 0);
			TestTrue(FString::Printf(TEXT("%s: id starts with armor_"), *Pair.Key.ToString()), Pair.Key.ToString().StartsWith(TEXT("armor_")));
		}
	}
	TestTrue(TEXT("shipped armor exists"), ArmorCount >= 8);

	for (const FGamerTierDef& Tier : Progression.Tiers)
	{
		// Best piece per slot (rings: the best two) of this tier.
		TMap<FName, TArray<const FGamerItemDef*>> BySlot;
		for (const TPair<FName, FGamerItemDef>& Pair : Data.Items)
		{
			const FGamerItemDef& Item = Pair.Value;
			if (Item.Category == EGamerItemCategory::Armor && Item.Tier == Tier.Tier)
			{
				BySlot.FindOrAdd(Item.Armor.Slot).Add(&Item);
			}
		}
		if (BySlot.Num() == 0) { continue; }

		TArray<const FGamerItemDef*> Best;
		for (TPair<FName, TArray<const FGamerItemDef*>>& Slot : BySlot)
		{
			Slot.Value.Sort([](const FGamerItemDef& A, const FGamerItemDef& B) { return A.Armor.Armor > B.Armor.Armor; });
			for (int32 i = 0; i < FMath::Min(Slot.Value.Num(), GamerEquipment::SlotCapacity(Slot.Key)); ++i) { Best.Add(Slot.Value[i]); }
		}
		const FGamerEquipmentTotals Totals = GamerEquipment::Summarize(Best);
		const float SetValue = GamerEquipment::ZoneArmor(Totals, EGamerHitZone::Body, Combat);
		const bool bFullSet = BySlot.Contains(TEXT("helmet")) && BySlot.Contains(TEXT("chest")) && BySlot.Contains(TEXT("shoulder"))
			&& BySlot.Contains(TEXT("hands")) && BySlot.Contains(TEXT("legs")) && BySlot.Contains(TEXT("feet"));
		if (bFullSet)
		{
			TestTrue(FString::Printf(TEXT("T%d set armor %.1f within %d..%d"), Tier.Tier, SetValue, Tier.Armor[0], Tier.Armor[1]),
				SetValue >= Tier.Armor[0] && SetValue <= Tier.Armor[1]);
		}
		TestTrue(FString::Printf(TEXT("T%d poise resist %.1f at most %d"), Tier.Tier, Totals.PoiseResist, Tier.PoiseResist[1]),
			Totals.PoiseResist <= Tier.PoiseResist[1]);
		for (const FName Kind : GamerData::ResistKinds())
		{
			TestTrue(FString::Printf(TEXT("T%d %s resistance %.2f at most %.2f"), Tier.Tier, *Kind.ToString(), Totals.GetResist(Kind), Tier.ResistCap),
				Totals.GetResist(Kind) <= Tier.ResistCap + 0.0001f);
		}
	}
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FGamerEquipmentValidatorTest, "Gamer.Equipment.ValidatorFindsErrors",
	EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::EngineFilter)

bool FGamerEquipmentValidatorTest::RunTest(const FString& Parameters)
{
	const FString Dir = FPaths::Combine(FPaths::ProjectSavedDir(), TEXT("Automation"), TEXT("GamerArmorBroken"));
	IFileManager::Get().DeleteDirectory(*Dir, false, true);
	IFileManager::Get().MakeDirectory(*Dir, true);
	FFileHelper::SaveStringToFile(TEXT(R"({ "schema": 1, "rows": [
		{ "id": "armor_ok", "name": "Ok", "category": "armor", "tier": 1, "max_stack": 1, "weight_kg": 1,
		  "armor": { "slot": "helmet", "armor": 5, "resist": { "cold": 0.05 } } },
		{ "id": "armor_tail", "name": "Tail", "category": "armor", "tier": 1, "max_stack": 1, "weight_kg": 1,
		  "armor": { "slot": "tail", "armor": 5 } },
		{ "id": "armor_heat", "name": "Heat", "category": "armor", "tier": 1, "max_stack": 1, "weight_kg": 1,
		  "armor": { "slot": "chest", "armor": 5, "resist": { "fire": 0.1 } } },
		{ "id": "armor_huge", "name": "Huge", "category": "armor", "tier": 1, "max_stack": 1, "weight_kg": 1,
		  "armor": { "slot": "chest", "armor": 5, "resist": { "cold": 0.9 } } },
		{ "id": "armor_neg", "name": "Neg", "category": "armor", "tier": 1, "max_stack": 1, "weight_kg": 1,
		  "armor": { "slot": "legs", "armor": -3 } },
		{ "id": "armor_typo", "name": "Typo", "category": "armor", "tier": 1, "max_stack": 1, "weight_kg": 1,
		  "armor": { "slot": "legs", "armour": 3 } },
		{ "id": "armor_nocat", "name": "NoCat", "category": "resource", "tier": 1, "max_stack": 1, "weight_kg": 1 },
		{ "id": "item_wrong", "name": "Wrong", "category": "resource", "tier": 1, "max_stack": 1, "weight_kg": 1,
		  "armor": { "slot": "feet", "armor": 1 } },
		{ "id": "armor_noblock", "name": "NoBlock", "category": "armor", "tier": 1, "max_stack": 1, "weight_kg": 1 }
	] })"), *FPaths::Combine(Dir, TEXT("items.json")));
	for (const TCHAR* Name : { TEXT("resource_nodes.json"), TEXT("recipes.json"), TEXT("buildables.json") })
	{
		FFileHelper::SaveStringToFile(TEXT(R"({ "schema": 1, "rows": [] })"), *FPaths::Combine(Dir, Name));
	}

	FGamerDataSet Data;
	FGamerDataReport Report;
	TestFalse(TEXT("broken armor reports failure"), GamerData::LoadFromDirectory(Dir, Data, Report, false));
	auto ExpectError = [&](const TCHAR* Needle, const TCHAR* What)
	{
		TestTrue(FString::Printf(TEXT("reports: %s"), What), Report.Errors.ContainsByPredicate([Needle](const FString& E) { return E.Contains(Needle); }));
	};
	ExpectError(TEXT("slot \"tail\" unbekannt"), TEXT("unknown slot"));
	ExpectError(TEXT("resist \"fire\" unbekannt"), TEXT("unknown resistance kind"));
	ExpectError(TEXT("resist.cold muss eine Zahl zwischen"), TEXT("resistance out of range"));
	ExpectError(TEXT("armor darf nicht negativ sein"), TEXT("negative armor"));
	ExpectError(TEXT("unbekanntes Feld \"armour\""), TEXT("typo in armor block"));
	ExpectError(TEXT("Rüstung braucht ID armor_"), TEXT("armor id and category must match"));
	ExpectError(TEXT("\"armor\" nur bei category"), TEXT("armor block on a non-armor item"));
	TestTrue(TEXT("the valid piece is read"), Data.Items.Contains(TEXT("armor_ok")));
	if (const FGamerItemDef* Ok = Data.Items.Find(TEXT("armor_ok")))
	{
		TestEqual(TEXT("slot read"), Ok->Armor.Slot, FName(TEXT("helmet")));
		TestEqual(TEXT("cold read"), Ok->Armor.Resist.FindRef(TEXT("cold")), 0.05f, 0.0001f);
	}
	return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
