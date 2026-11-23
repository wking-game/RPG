#include "GamerCreatureData.h"
#include "GamerDataLoader.h"

#include "HAL/FileManager.h"
#include "Misc/AutomationTest.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"

#if WITH_DEV_AUTOMATION_TESTS

// Run with: tools/test_data.ps1 -Filter Gamer.Creatures

namespace
{
	/** Loads the shipped item tables and creature tables. Errors go to the test. */
	bool LoadShipped(FAutomationTestBase& Test, FGamerDataSet& Data, FGamerCreatureDataSet& Set)
	{
		FGamerDataReport Report;
		GamerData::LoadFromDirectory(GamerData::DefaultDirectory(), Data, Report);
		GamerData::LoadCreatures(GamerData::DefaultDirectory(), Data, Set, Report, true);
		for (const FString& Error : Report.Errors)
		{
			Test.AddError(Error);
		}
		for (const FString& Warning : Report.Warnings)
		{
			Test.AddInfo(Warning);
		}
		return !Report.HasErrors();
	}
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FGamerCreaturesShippedIsValidTest, "Gamer.Creatures.ShippedDataIsValid",
	EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::EngineFilter)

bool FGamerCreaturesShippedIsValidTest::RunTest(const FString& Parameters)
{
	FGamerDataSet Data;
	FGamerCreatureDataSet Set;
	TestTrue(TEXT("all tables load without errors"), LoadShipped(*this, Data, Set));

	int32 Creatures = 0, Animals = 0;
	for (const TPair<FName, FGamerCreatureDef>& Pair : Set.Creatures)
	{
		(Pair.Value.bAnimal ? Animals : Creatures)++;
		TestTrue(FString::Printf(TEXT("%s has rank stats"), *Pair.Key.ToString()), Pair.Value.Ranks.Num() > 0);
		TestTrue(FString::Printf(TEXT("%s can appear somewhere"), *Pair.Key.ToString()), Set.ReachableRanks(Pair.Key).Num() > 0);
		TestTrue(FString::Printf(TEXT("%s has loot"), *Pair.Key.ToString()), Set.Loot.Contains(Pair.Key));
	}
	TestEqual(TEXT("48 creatures"), Creatures, 48);
	TestEqual(TEXT("12 animals"), Animals, 12);
	TestEqual(TEXT("6 biomes"), Set.Biomes.Num(), 6);
	TestEqual(TEXT("one loot table each"), Set.Loot.Num(), 60);
	TestTrue(TEXT("checksum set"), Set.Checksum != 0);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FGamerCreaturesRulesTest, "Gamer.Creatures.HomeAndRankRules",
	EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::EngineFilter)

bool FGamerCreaturesRulesTest::RunTest(const FString& Parameters)
{
	FGamerDataSet Data;
	FGamerCreatureDataSet Set;
	LoadShipped(*this, Data, Set);

	// A creature never appears before its own wave (its home biome has the lowest biome rank it appears in).
	for (const FGamerSpawnRule& Rule : Set.Spawns)
	{
		const FGamerCreatureDef* Def = Set.Creatures.Find(Rule.Creature);
		const FGamerBiomeDef* Biome = Set.FindBiome(Rule.Biome);
		const FGamerBiomeDef* Home = Def ? Set.FindBiome(Def->HomeBiome) : nullptr;
		if (!Def || !Biome || !Home)
		{
			AddError(FString::Printf(TEXT("spawn rule %s in %s points to nothing"), *Rule.Creature.ToString(), *Rule.Biome.ToString()));
			continue;
		}
		TestTrue(FString::Printf(TEXT("%s: not before its home biome"), *Rule.Creature.ToString()), Biome->Rank >= Home->Rank);
		for (const int32 Rank : Set.ReachableRanks(Rule.Creature, Rule.Biome))
		{
			TestTrue(FString::Printf(TEXT("%s in %s: rank %d >= min rank"), *Rule.Creature.ToString(), *Rule.Biome.ToString(), Rank), Rank >= Def->MinRank);
			TestTrue(FString::Printf(TEXT("%s in %s: stats for rank %d"), *Rule.Creature.ToString(), *Rule.Biome.ToString(), Rank), Def->FindRank(Rank) != nullptr);
		}
	}

	// Dragon sinew only from rank 5 (higher grades only exist from then on).
	for (const TPair<FName, FGamerCreatureLoot>& Pair : Set.Loot)
	{
		TArray<const FGamerLootEntry*> All;
		for (const FGamerLootEntry& E : Pair.Value.Base) { All.Add(&E); }
		for (const FGamerLootEntry& E : Pair.Value.Rank) { All.Add(&E); }
		for (const FGamerLootEntry& E : Pair.Value.Biome) { All.Add(&E); }
		for (const FGamerLootEntry* Entry : All)
		{
			if (Entry->ItemFamily == TEXT("item_dragon_sinew"))
			{
				for (const int32 Rank : Set.ReachableRanks(Pair.Key))
				{
					if (Rank >= Entry->MinRank)
					{
						TestTrue(FString::Printf(TEXT("%s: dragon sinew only from rank 5 (rank %d)"), *Pair.Key.ToString(), Rank), Rank >= 5);
					}
				}
			}
		}
	}

	// The data keeps the decision "no leaders above rank 6": ranks are 1..6.
	for (const TPair<FName, FGamerCreatureDef>& Pair : Set.Creatures)
	{
		for (const FGamerRankStats& Stats : Pair.Value.Ranks)
		{
			TestTrue(FString::Printf(TEXT("%s: rank %d within 1..6"), *Pair.Key.ToString(), Stats.Rank), Stats.Rank >= 1 && Stats.Rank <= GamerData::MaxTier);
		}
	}
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FGamerCreaturesLootItemsTest, "Gamer.Creatures.LootItemsExist",
	EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::EngineFilter)

bool FGamerCreaturesLootItemsTest::RunTest(const FString& Parameters)
{
	FGamerDataSet Data;
	FGamerCreatureDataSet Set;
	LoadShipped(*this, Data, Set);

	int32 Checked = 0;
	for (const TPair<FName, FGamerCreatureLoot>& Pair : Set.Loot)
	{
		auto Check = [&](const TArray<FGamerLootEntry>& Entries, bool bBiome)
		{
			for (const FGamerLootEntry& Entry : Entries)
			{
				TSet<int32> Ranks;
				if (bBiome)
				{
					for (const FName Biome : Entry.Biomes) { Ranks.Append(Set.ReachableRanks(Pair.Key, Biome)); }
				}
				else
				{
					for (const int32 Rank : Set.ReachableRanks(Pair.Key))
					{
						if (Rank >= Entry.MinRank) { Ranks.Add(Rank); }
					}
				}
				TestTrue(FString::Printf(TEXT("%s: a loot line can roll at some rank"), *Pair.Key.ToString()), Ranks.Num() > 0);
				for (const int32 Rank : Ranks)
				{
					const FName Item = Entry.Resolve(Rank);
					TestTrue(FString::Printf(TEXT("%s drops %s: item exists"), *Pair.Key.ToString(), *Item.ToString()), Data.Items.Contains(Item));
					++Checked;
				}
			}
		};
		Check(Pair.Value.Base, false);
		Check(Pair.Value.Rank, false);
		Check(Pair.Value.Biome, true);
	}
	TestTrue(TEXT("many loot lines were checked"), Checked > 300);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FGamerCreaturesValidatorTest, "Gamer.Creatures.ValidatorFindsErrors",
	EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::EngineFilter)

bool FGamerCreaturesValidatorTest::RunTest(const FString& Parameters)
{
	// Deliberately broken data: every mistake below must be reported (checks the checker).
	const FString Dir = FPaths::Combine(FPaths::ProjectSavedDir(), TEXT("Automation"), TEXT("GamerCreaturesBroken"));
	IFileManager::Get().DeleteDirectory(*Dir, false, true);
	IFileManager::Get().MakeDirectory(*Dir, true);

	FFileHelper::SaveStringToFile(TEXT(R"({ "schema": 1, "rows": [
		{ "id": "creature_ok", "name": "Ok", "kind": "creature", "temperament": "hostile", "min_rank": 2, "home_biome": "biome_x", "wave": 1,
		  "ranks": [ { "rank": 2, "hp": 100, "poise": 10, "dmg": 5 }, { "rank": 3, "hp": 150, "poise": 12, "dmg": 6 } ] },
		{ "id": "monster_bad", "name": "Bad", "kind": "creature", "temperament": "hostile", "min_rank": 2, "home_biome": "biome_x", "wave": 1,
		  "ranks": [ { "rank": 2, "hp": 100, "poise": 10, "dmg": 5 } ] },
		{ "id": "creature_nohp", "name": "NoHp", "kind": "creature", "temperament": "hostile", "min_rank": 2, "home_biome": "biome_x", "wave": 1,
		  "ranks": [ { "rank": 2, "hp": 0, "poise": 10, "dmg": 5 } ] },
		{ "id": "creature_late", "name": "Late", "kind": "creature", "temperament": "hostile", "min_rank": 2, "home_biome": "biome_x", "wave": 1,
		  "ranks": [ { "rank": 2, "hp": 100, "poise": 10, "dmg": 5 } ] },
		{ "id": "creature_norank", "name": "NoRank", "kind": "creature", "temperament": "hostile", "min_rank": 2, "home_biome": "biome_x", "wave": 1,
		  "ranks": [ { "rank": 2, "hp": 100, "poise": 10, "dmg": 5 } ] },
		{ "id": "creature_lonely", "name": "Lonely", "kind": "creature", "temperament": "wild", "min_rank": 2, "home_biome": "biome_x", "wave": 1,
		  "ranks": [ { "rank": 2, "hp": 100, "poise": 10, "dmg": 5 } ] },
		{ "id": "creature_ok", "name": "Again", "kind": "creature", "temperament": "hostile", "min_rank": 2, "home_biome": "biome_x", "wave": 1,
		  "ranks": [ { "rank": 2, "hp": 100, "poise": 10, "dmg": 5 } ] }
	] })"), *FPaths::Combine(Dir, GamerData::CreaturesFile));

	FFileHelper::SaveStringToFile(TEXT(R"({ "schema": 1, "rows": [] })"), *FPaths::Combine(Dir, GamerData::AnimalsFile));

	FFileHelper::SaveStringToFile(TEXT(R"({ "schema": 1,
		"biomes": [ { "id": "biome_x", "name": "X", "rank": 2, "place_rank": 3, "places": "-" },
		            { "id": "biome_y", "name": "Y", "rank": 1, "place_rank": 2, "places": "-" } ],
		"rows": [
		{ "biome": "biome_x", "creature": "creature_ok", "weight": 1.0, "only_places": false, "scale_to_zone": true, "scale_chance": 1.0, "max_rank": 0, "fixed_rank": 0 },
		{ "biome": "biome_x", "creature": "creature_ok", "weight": 1.0, "only_places": false, "scale_to_zone": false, "scale_chance": 0.0, "max_rank": 1, "fixed_rank": 1 },
		{ "biome": "biome_x", "creature": "creature_norank", "weight": 1.0, "only_places": false, "scale_to_zone": true, "scale_chance": 1.0, "max_rank": 0, "fixed_rank": 0 },
		{ "biome": "biome_x", "creature": "creature_nohp", "weight": 1.0, "only_places": false, "scale_to_zone": true, "scale_chance": 0.5, "max_rank": 0, "fixed_rank": 0 },
		{ "biome": "biome_y", "creature": "creature_late", "weight": 1.0, "only_places": false, "scale_to_zone": true, "scale_chance": 1.0, "max_rank": 0, "fixed_rank": 0 },
		{ "biome": "biome_y", "creature": "creature_ghost", "weight": 1.0, "only_places": false, "scale_to_zone": true, "scale_chance": 1.0, "max_rank": 0, "fixed_rank": 0 }
	] })"), *FPaths::Combine(Dir, GamerData::SpawnsFile));

	FFileHelper::SaveStringToFile(TEXT(R"({ "schema": 1, "rows": [
		{ "id": "creature_ok",
		  "base": [ { "item": "item_unknown", "min": 1, "max": 1, "chance": 0.5, "min_rank": 2, "scale_count": false } ],
		  "rank": [ { "item_family": "item_fam", "min": 1, "max": 2, "chance": 0.5, "min_rank": 2, "scale_count": true } ],
		  "biome": [ { "item": "item_a", "min": 1, "max": 1, "chance": 0.5, "biomes": [ "biome_y" ] } ] },
		{ "id": "creature_nohp",
		  "base": [ { "item": "item_a", "item_family": "item_fam", "min": 1, "max": 1, "chance": 0.5, "min_rank": 2, "scale_count": false } ],
		  "rank": [], "biome": [] },
		{ "id": "creature_ghost", "base": [], "rank": [], "biome": [] }
	] })"), *FPaths::Combine(Dir, GamerData::CreatureLootFile));

	FGamerDataSet Base;
	Base.Items.Add(TEXT("item_a"), FGamerItemDef());
	Base.Items.Add(TEXT("item_fam_t2"), FGamerItemDef());

	FGamerCreatureDataSet Set;
	FGamerDataReport Report;
	const bool bOk = GamerData::LoadCreatures(Dir, Base, Set, Report, true);
	TestFalse(TEXT("loading broken data reports failure"), bOk);

	auto ExpectError = [&](const TCHAR* Needle, const TCHAR* What)
	{
		const bool bFound = Report.Errors.ContainsByPredicate([Needle](const FString& E) { return E.Contains(Needle); });
		TestTrue(FString::Printf(TEXT("reports: %s"), What), bFound);
	};
	ExpectError(TEXT("id muss snake_case sein und mit creature_ beginnen"), TEXT("wrong id prefix"));
	ExpectError(TEXT("hp muss größer als 0 sein"), TEXT("hp 0"));
	ExpectError(TEXT("ID kommt doppelt vor"), TEXT("duplicate id"));
	ExpectError(TEXT("temperament \"wild\" unbekannt"), TEXT("unknown temperament"));
	ExpectError(TEXT("kommt vor seinem Heimat-Biom"), TEXT("before its own wave"));
	ExpectError(TEXT("Rang 1 liegt unter min_rank 2"), TEXT("rank below min rank"));
	ExpectError(TEXT("keine Werte für Rang 3"), TEXT("rank without stats"));
	ExpectError(TEXT("kommt in keinem Biom vor"), TEXT("creature that never spawns"));
	ExpectError(TEXT("Kreatur oder Tier unbekannt"), TEXT("spawn or loot for an unknown creature"));
	ExpectError(TEXT("Gegenstand item_unknown unbekannt"), TEXT("unknown loot item"));
	ExpectError(TEXT("Gegenstand item_fam_t3 fehlt"), TEXT("missing tier of a loot family"));
	ExpectError(TEXT("diese Biom-Beute würde nie fallen"), TEXT("biome loot where the creature does not spawn"));
	ExpectError(TEXT("genau eines von \"item\" und \"item_family\""), TEXT("item and item_family both set"));
	ExpectError(TEXT("keine Beute für creature_lonely"), TEXT("creature without loot"));

	for (const FString& Error : Report.Errors)
	{
		AddInfo(FString::Printf(TEXT("(expected) %s"), *Error));
	}
	IFileManager::Get().DeleteDirectory(*Dir, false, true);
	return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
