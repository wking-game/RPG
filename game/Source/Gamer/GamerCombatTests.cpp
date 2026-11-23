#include "GamerCombatMath.h"
#include "GamerDataLoader.h"

#include "HAL/FileManager.h"
#include "Misc/AutomationTest.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"

#if WITH_DEV_AUTOMATION_TESTS

// Run with: tools/test_data.ps1 -Filter Gamer.Combat

namespace
{
	FGamerCombatData LoadShippedCombat(FAutomationTestBase& Test)
	{
		FGamerCombatData Data;
		FGamerDataReport Report;
		GamerData::LoadCombat(GamerData::DefaultDirectory(), Data, Report, true);
		for (const FString& Error : Report.Errors)
		{
			Test.AddError(Error);
		}
		return Data;
	}

	constexpr float Tol = 0.001f;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FGamerCombatShippedTest, "Gamer.Combat.ShippedDataIsValid",
	EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::EngineFilter)

bool FGamerCombatShippedTest::RunTest(const FString& Parameters)
{
	const FGamerCombatData Data = LoadShippedCombat(*this);
	TestTrue(TEXT("loaded"), Data.bLoaded);
	TestEqual(TEXT("armor K"), Data.ArmorK, 160.f);
	TestEqual(TEXT("armor cap"), Data.ArmorMaxReduction, 0.75f);
	TestEqual(TEXT("three leader tiers"), Data.Leaders.Num(), 3);
	TestEqual(TEXT("six slots"), Data.SlotShare.Num(), 6);
	float Sum = 0.f;
	for (const TPair<FName, float>& Pair : Data.SlotShare) { Sum += Pair.Value; }
	TestTrue(TEXT("slot shares sum to 1"), FMath::IsNearlyEqual(Sum, 1.f, Tol));
	TestEqual(TEXT("creature hp scale starts at 1"), Data.CreatureHpScale, 1.f);
	TestTrue(TEXT("light attack exists"), Data.FindAttack(TEXT("light")) != nullptr);
	TestTrue(TEXT("checksum set"), Data.Checksum != 0);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FGamerCombatArmorTest, "Gamer.Combat.Armor",
	EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::EngineFilter)

bool FGamerCombatArmorTest::RunTest(const FString& Parameters)
{
	const FGamerCombatData Data = LoadShippedCombat(*this);
	TestEqual(TEXT("armor 240 reduces by 60 %"), GamerCombat::ArmorDamageReduction(240.f, Data), 0.6f, Tol);
	TestEqual(TEXT("armor 160 reduces by 50 %"), GamerCombat::ArmorDamageReduction(160.f, Data), 0.5f, Tol);
	TestEqual(TEXT("reduction is capped at 75 %"), GamerCombat::ArmorDamageReduction(100000.f, Data), 0.75f, Tol);
	TestEqual(TEXT("negative armor counts as 0"), GamerCombat::ArmorDamageReduction(-50.f, Data), 0.f, Tol);
	TestEqual(TEXT("80.5 raw damage against armor 240"), GamerCombat::ApplyArmor(80.5f, 240.f, Data), 32.2f, Tol);
	TestEqual(TEXT("damage never below the minimum"), GamerCombat::ApplyArmor(0.5f, 100.f, Data), 1.f, Tol);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FGamerCombatResistTest, "Gamer.Combat.Resist",
	EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::EngineFilter)

bool FGamerCombatResistTest::RunTest(const FString& Parameters)
{
	const FGamerCombatData Data = LoadShippedCombat(*this);
	TestEqual(TEXT("resistance capped at 60 %"), GamerCombat::ClampResistance(0.9f, Data), 0.6f, Tol);
	TestEqual(TEXT("resistance floor -25 %"), GamerCombat::ClampResistance(-0.5f, Data), -0.25f, Tol);
	TestEqual(TEXT("45 % resistance leaves 55 %"), GamerCombat::ApplyResistance(100.f, 0.45f, Data), 55.f, Tol);
	TestEqual(TEXT("-25 % resistance adds 25 %"), GamerCombat::ApplyResistance(100.f, -0.25f, Data), 125.f, Tol);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FGamerCombatPoiseTest, "Gamer.Combat.Poise",
	EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::EngineFilter)

bool FGamerCombatPoiseTest::RunTest(const FString& Parameters)
{
	const FGamerCombatData Data = LoadShippedCombat(*this);
	// Player side: 150 poise resist dampens by 62.5 %.
	TestEqual(TEXT("150 poise resist leaves 37.5 %"), GamerCombat::PlayerPoiseDamageTaken(100.f, 1.f, 150.f, Data), 37.5f, Tol);
	TestEqual(TEXT("no poise resist leaves everything"), GamerCombat::PlayerPoiseDamageTaken(100.f, 1.f, 0.f, Data), 100.f, Tol);
	// Creature side: weapon 50, heavy hit (x1.6), weak point (x1.5) = 120.
	TestEqual(TEXT("heavy hit on a weak point"), GamerCombat::EnemyPoiseDamageTaken(50.f, 1.6f, true, Data), 120.f, Tol);
	TestEqual(TEXT("normal hit"), GamerCombat::EnemyPoiseDamageTaken(50.f, 1.f, false, Data), 50.f, Tol);

	// The bar fills, breaks into a stagger, stays empty while staggered, then recovers.
	FGamerPoiseMeter Meter;
	TestFalse(TEXT("first hit does not break"), Meter.AddDamage(60.f, 100.f, false, Data));
	TestEqual(TEXT("bar filled to 60"), Meter.Current, 60.f, Tol);
	TestTrue(TEXT("second hit breaks"), Meter.AddDamage(50.f, 100.f, false, Data));
	TestEqual(TEXT("bar reset to 0 after the break"), Meter.Current, 0.f, Tol);
	TestEqual(TEXT("stagger lasts 2.5 s"), Meter.StaggerLeft, 2.5f, Tol);
	TestEqual(TEXT("hits during the stagger do x1.5"), Meter.DamageMultiplier(Data), 1.5f, Tol);
	TestFalse(TEXT("no second break during the stagger"), Meter.AddDamage(500.f, 100.f, false, Data));
	TestEqual(TEXT("bar stays 0 during the stagger"), Meter.Current, 0.f, Tol);
	Meter.Tick(2.6f, 100.f, Data);
	TestFalse(TEXT("stagger is over"), Meter.IsStaggered());
	TestEqual(TEXT("damage multiplier back to 1"), Meter.DamageMultiplier(Data), 1.f, Tol);

	// Decay: nothing for 3 s without hits, then 25 % of the maximum per second.
	FGamerPoiseMeter Decay;
	Decay.AddDamage(50.f, 100.f, false, Data);
	Decay.Tick(3.0f, 100.f, Data);
	TestEqual(TEXT("no decay in the first 3 s"), Decay.Current, 50.f, Tol);
	Decay.Tick(1.0f, 100.f, Data);
	TestEqual(TEXT("then 25 per second"), Decay.Current, 25.f, Tol);
	Decay.Tick(10.f, 100.f, Data);
	TestEqual(TEXT("never below 0"), Decay.Current, 0.f, Tol);

	FGamerPoiseMeter Boss;
	Boss.AddDamage(200.f, 100.f, true, Data);
	TestEqual(TEXT("bosses stagger for 3 s"), Boss.StaggerLeft, 3.f, Tol);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FGamerCombatZonesTest, "Gamer.Combat.Zones",
	EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::EngineFilter)

bool FGamerCombatZonesTest::RunTest(const FString& Parameters)
{
	const FGamerCombatData Data = LoadShippedCombat(*this);
	// A full set of value 200: every slot carries its share, every zone gets the full value.
	TMap<FName, float> Full;
	for (const TPair<FName, float>& Pair : Data.SlotShare) { Full.Add(Pair.Key, Pair.Value * 200.f); }
	TestEqual(TEXT("head with a full set"), GamerCombat::ZoneArmor(EGamerHitZone::Head, Full, Data), 200.f, Tol);
	TestEqual(TEXT("body with a full set"), GamerCombat::ZoneArmor(EGamerHitZone::Body, Full, Data), 200.f, Tol);
	TestEqual(TEXT("limbs with a full set"), GamerCombat::ZoneArmor(EGamerHitZone::Limbs, Full, Data), 200.f, Tol);

	TMap<FName, float> NoHelmet = Full;
	NoHelmet.Remove(TEXT("helmet"));
	TestEqual(TEXT("no helmet: head unprotected"), GamerCombat::ZoneArmor(EGamerHitZone::Head, NoHelmet, Data), 0.f, Tol);
	TestEqual(TEXT("no helmet: body unchanged"), GamerCombat::ZoneArmor(EGamerHitZone::Body, NoHelmet, Data), 200.f, Tol);

	TMap<FName, float> NoHands = Full;
	NoHands.Remove(TEXT("hands"));
	TestEqual(TEXT("no gloves: limbs at 2/3"), GamerCombat::ZoneArmor(EGamerHitZone::Limbs, NoHands, Data), 133.333f, 0.01f);
	TestEqual(TEXT("nothing worn: no armor"), GamerCombat::ZoneArmor(EGamerHitZone::Body, TMap<FName, float>(), Data), 0.f, Tol);

	TestEqual(TEXT("head damage factor"), GamerCombat::ZoneDamageFactor(EGamerHitZone::Head, Data), 1.5f, Tol);
	TestEqual(TEXT("body damage factor"), GamerCombat::ZoneDamageFactor(EGamerHitZone::Body, Data), 1.0f, Tol);
	TestEqual(TEXT("limbs damage factor"), GamerCombat::ZoneDamageFactor(EGamerHitZone::Limbs, Data), 0.7f, Tol);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FGamerCombatPlayerHpTest, "Gamer.Combat.PlayerHp",
	EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::EngineFilter)

bool FGamerCombatPlayerHpTest::RunTest(const FString& Parameters)
{
	const FGamerCombatData Data = LoadShippedCombat(*this);
	TestEqual(TEXT("toughness 3"), GamerCombat::PlayerMaxHp(3, 0.f, Data), 90.f, Tol);
	TestEqual(TEXT("toughness 5"), GamerCombat::PlayerMaxHp(5, 0.f, Data), 100.f, Tol);
	TestEqual(TEXT("toughness 8"), GamerCombat::PlayerMaxHp(8, 0.f, Data), 115.f, Tol);
	TestEqual(TEXT("meal bonus adds"), GamerCombat::PlayerMaxHp(5, 10.f, Data), 110.f, Tol);
	TestEqual(TEXT("never below 1"), GamerCombat::PlayerMaxHp(-100, 0.f, Data), 1.f, Tol);
	TestEqual(TEXT("creature hp uses the scale, the roll and the leader factor"), GamerCombat::CreatureHp(1000.f, 1.1f, 1.25f, Data), 1375.f, Tol);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FGamerCombatValidatorTest, "Gamer.Combat.ValidatorFindsErrors",
	EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::EngineFilter)

bool FGamerCombatValidatorTest::RunTest(const FString& Parameters)
{
	const FString Dir = FPaths::Combine(FPaths::ProjectSavedDir(), TEXT("Automation"), TEXT("GamerCombatBroken"));
	IFileManager::Get().DeleteDirectory(*Dir, false, true);
	IFileManager::Get().MakeDirectory(*Dir, true);

	FFileHelper::SaveStringToFile(TEXT(R"({ "schema": 1,
		"armor": { "k": 0, "max_reduction": 1.5, "min_damage": 1 },
		"resist": { "min": -0.25, "max": 0.6 },
		"poise": { "k_player": 90, "stagger_s": 2.5, "boss_stagger_s": 3, "stagger_damage_mult": 0.5, "decay_delay_s": 3,
		           "decay_fraction_per_s": 0.25, "player_max": 100, "player_stagger_s": 0.8, "player_regen_delay_s": 2, "player_regen_per_s": 30 },
		"slots": { "helmet": 0.5, "chest": 0.2, "legs": 0.2 },
		"zones": { "head": { "slots": [ "helmet", "visor" ], "damage_factor": 1.5 },
		           "body": { "slots": [ "chest", "legs" ], "damage_factor": 1.0 },
		           "limbs": { "slots": [ "legs" ], "damage_factor": 0.7 } },
		"attacks": [ { "id": "light", "damage": 1, "poise": 1 }, { "id": "light", "damage": 1, "poise": 1 } ],
		"weak_point_poise_mult": 1.5,
		"spread": { "min": 1.2, "max": 1.1, "size_follow": 0.5 },
		"leaders": [ { "id": "a", "min_rank": 2, "chance": 0.05, "place_chance": 0.1, "hp": 1.3, "poise": 1.1, "dmg": 1.1, "size": 1.05,
		               "extra_base_rolls": 1, "extra_rank_rolls": 0, "rank_roll_mult": 1 },
		             { "id": "b", "min_rank": 4, "chance": 0.03, "place_chance": 0.06, "hp": 0.9, "poise": 1.1, "dmg": 1.1, "size": 1.05,
		               "extra_base_rolls": 1, "extra_rank_rolls": 1, "rank_roll_mult": 1 } ],
		"loot": { "count_bonus_cap": 2, "downscale": { "delta1": 0.5, "delta2": 0.7, "delta3": 0.55 } },
		"creature_hp_scale": 0,
		"player_hp": { "base": 100, "per_toughness": 5, "toughness_neutral": 5 } })"), *FPaths::Combine(Dir, GamerData::CombatFile));

	FGamerCombatData Data;
	FGamerDataReport Report;
	const bool bOk = GamerData::LoadCombat(Dir, Data, Report, true);
	TestFalse(TEXT("loading broken data reports failure"), bOk);

	auto ExpectError = [&](const TCHAR* Needle, const TCHAR* What)
	{
		const bool bFound = Report.Errors.ContainsByPredicate([Needle](const FString& E) { return E.Contains(Needle); });
		TestTrue(FString::Printf(TEXT("reports: %s"), What), bFound);
	};
	ExpectError(TEXT("k muss größer als 0 sein"), TEXT("armor K 0"));
	ExpectError(TEXT("max_reduction muss zwischen 0 und 1"), TEXT("armor cap above 1"));
	ExpectError(TEXT("stagger_damage_mult darf nicht kleiner als 1 sein"), TEXT("stagger multiplier below 1"));
	ExpectError(TEXT("die Anteile ergeben"), TEXT("slot shares not 1"));
	ExpectError(TEXT("Zone nennt den unbekannten Slot visor"), TEXT("unknown slot in a zone"));
	ExpectError(TEXT("Slot legs steht in mehreren Zonen"), TEXT("slot in two zones"));
	ExpectError(TEXT("id kommt doppelt vor"), TEXT("duplicate attack id"));
	ExpectError(TEXT("max darf nicht kleiner als min sein"), TEXT("spread max below min"));
	ExpectError(TEXT("Anführer sind nie schwächer"), TEXT("leader weaker than a normal creature"));
	ExpectError(TEXT("die Stufen müssen nach hp aufsteigend sortiert sein"), TEXT("leaders not sorted"));
	ExpectError(TEXT("die Dämpfung muss mit dem Abstand sinken"), TEXT("loot downscale not decreasing"));
	ExpectError(TEXT("creature_hp_scale muss größer als 0 sein"), TEXT("hp scale 0"));

	for (const FString& Error : Report.Errors)
	{
		AddInfo(FString::Printf(TEXT("(expected) %s"), *Error));
	}
	IFileManager::Get().DeleteDirectory(*Dir, false, true);
	return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
