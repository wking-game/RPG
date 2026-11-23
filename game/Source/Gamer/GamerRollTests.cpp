#include "GamerCreatureRoll.h"
#include "GamerDataLoader.h"

#include "Misc/AutomationTest.h"

#if WITH_DEV_AUTOMATION_TESTS

// Run with: tools/test_data.ps1 -Filter Gamer.Loot   and   -Filter Gamer.Spawn
// Statistical tests use a fixed seed and 100 000 rolls, so they give the same result on every run.

namespace
{
	constexpr int32 Rolls = 100000;

	FGamerCombatData LoadCombat(FAutomationTestBase& Test)
	{
		FGamerCombatData Data;
		FGamerDataReport Report;
		GamerData::LoadCombat(GamerData::DefaultDirectory(), Data, Report, true);
		for (const FString& Error : Report.Errors) { Test.AddError(Error); }
		return Data;
	}

	FGamerLootEntry Line(const TCHAR* Item, int32 Min, int32 Max, float Chance, int32 MinRank = 0, bool bScale = false)
	{
		FGamerLootEntry Entry;
		Entry.Item = Item;
		Entry.Min = Min;
		Entry.Max = Max;
		Entry.Chance = Chance;
		Entry.MinRank = MinRank;
		Entry.bScaleCount = bScale;
		return Entry;
	}

	/** Loot with known numbers: A 1-2 at 0.9 from rank 2; B 1 at 0.5 from rank 2 growing with the rank; C biome bonus 1 at 0.5 in biome_x. */
	FGamerCreatureLoot TestLoot()
	{
		FGamerCreatureLoot Loot;
		Loot.Id = TEXT("creature_test");
		Loot.Base.Add(Line(TEXT("item_a"), 1, 2, 0.9f, 2));
		Loot.Rank.Add(Line(TEXT("item_b"), 1, 1, 0.5f, 2, true));
		FGamerLootEntry Bonus = Line(TEXT("item_c"), 1, 1, 0.5f);
		Bonus.Biomes.Add(TEXT("biome_x"));
		Loot.Biome.Add(Bonus);
		return Loot;
	}

	/** Average number of pieces of an item over many loot rolls. */
	double MeanCount(const FGamerCreatureLoot& Loot, FName Item, int32 Rank, int32 ZoneRank, FName Biome, int32 Leader, float Yield,
		const FGamerCombatData& Combat, int32 Seed)
	{
		FRandomStream Stream(Seed);
		double Sum = 0.0;
		for (int32 I = 0; I < Rolls; ++I)
		{
			for (const FGamerRolledItem& Rolled : GamerCreature::RollLoot(Loot, Rank, ZoneRank, Biome, Leader, Yield, Combat, Stream))
			{
				if (Rolled.Item == Item) { Sum += Rolled.Count; }
			}
		}
		return Sum / Rolls;
	}

	/** Two biomes and a creature that exists from rank 2, for the spawn tests. */
	FGamerCreatureDataSet TestSet(float Chance, int32 MaxRank, bool bFixed)
	{
		FGamerCreatureDataSet Set;
		FGamerBiomeDef Biome;
		Biome.Id = TEXT("biome_x");
		Biome.Rank = 3;
		Biome.PlaceRank = 4;
		Set.Biomes.Add(Biome);
		FGamerCreatureDef Def;
		Def.Id = TEXT("creature_test");
		Def.MinRank = 2;
		for (int32 Rank = 2; Rank <= 6; ++Rank)
		{
			Def.Ranks.Add({ Rank, 100.f * Rank, 10.f * Rank, 5.f * Rank });
		}
		Set.Creatures.Add(Def.Id, Def);
		FGamerSpawnRule Rule;
		Rule.Biome = Biome.Id;
		Rule.Creature = Def.Id;
		Rule.ScaleChance = Chance;
		Rule.MaxRank = MaxRank;
		Rule.bScaleToZone = !bFixed;
		Rule.FixedRank = bFixed ? 2 : 0;
		Set.Spawns.Add(Rule);
		return Set;
	}
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FGamerLootDeterministicTest, "Gamer.Loot.Deterministic",
	EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::EngineFilter)

bool FGamerLootDeterministicTest::RunTest(const FString& Parameters)
{
	const FGamerCombatData Combat = LoadCombat(*this);
	const FGamerCreatureLoot Loot = TestLoot();
	FRandomStream A(4711), B(4711), C(4712);
	bool bSame = true, bDifferent = false;
	for (int32 I = 0; I < 200; ++I)
	{
		const TArray<FGamerRolledItem> X = GamerCreature::RollLoot(Loot, 4, 4, TEXT("biome_x"), -1, 1.f, Combat, A);
		const TArray<FGamerRolledItem> Y = GamerCreature::RollLoot(Loot, 4, 4, TEXT("biome_x"), -1, 1.f, Combat, B);
		const TArray<FGamerRolledItem> Z = GamerCreature::RollLoot(Loot, 4, 4, TEXT("biome_x"), -1, 1.f, Combat, C);
		bSame &= X.Num() == Y.Num();
		for (int32 K = 0; K < FMath::Min(X.Num(), Y.Num()); ++K)
		{
			bSame &= X[K].Item == Y[K].Item && X[K].Count == Y[K].Count;
		}
		bDifferent |= X.Num() != Z.Num() || (X.Num() > 0 && Z.Num() > 0 && X[0].Count != Z[0].Count);
	}
	TestTrue(TEXT("same seed gives the same loot"), bSame);
	TestTrue(TEXT("another seed gives other loot"), bDifferent);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FGamerLootStatisticsTest, "Gamer.Loot.Statistics",
	EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::EngineFilter)

bool FGamerLootStatisticsTest::RunTest(const FString& Parameters)
{
	const FGamerCombatData Combat = LoadCombat(*this);
	const FGamerCreatureLoot Loot = TestLoot();
	const FName X = TEXT("biome_x");
	// A: 1-2 pieces at 0.9 -> mean 0.9 * 1.5 = 1.35
	TestEqual(TEXT("A mean"), MeanCount(Loot, TEXT("item_a"), 2, 2, X, -1, 1.f, Combat, 1), 1.35, 0.027);
	// B at its own rank: 1 piece at 0.5; rank lock: below rank 2 nothing at all
	TestEqual(TEXT("B mean at the lock rank"), MeanCount(Loot, TEXT("item_b"), 2, 2, X, -1, 1.f, Combat, 2), 0.5, 0.01);
	TestEqual(TEXT("A below its rank lock never drops"), MeanCount(Loot, TEXT("item_a"), 1, 1, X, -1, 1.f, Combat, 3), 0.0, 0.0);
	// Count bonus +1 per rank, capped at +2: rank 3 gives 2, rank 4 and rank 6 give 3
	TestEqual(TEXT("B at rank 3: 2 pieces"), MeanCount(Loot, TEXT("item_b"), 3, 3, X, -1, 1.f, Combat, 4), 1.0, 0.02);
	TestEqual(TEXT("B at rank 4: 3 pieces"), MeanCount(Loot, TEXT("item_b"), 4, 4, X, -1, 1.f, Combat, 5), 1.5, 0.03);
	TestEqual(TEXT("B at rank 6 stays capped at 3 pieces"), MeanCount(Loot, TEXT("item_b"), 6, 6, X, -1, 1.f, Combat, 6), 1.5, 0.03);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FGamerLootDampeningTest, "Gamer.Loot.Dampening",
	EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::EngineFilter)

bool FGamerLootDampeningTest::RunTest(const FString& Parameters)
{
	const FGamerCombatData Combat = LoadCombat(*this);
	const FGamerCreatureLoot Loot = TestLoot();
	const FName X = TEXT("biome_x");
	TestEqual(TEXT("difference 0: 1"), GamerCreature::BiomeLootDownscale(0, Combat), 1.f, 0.0001f);
	TestEqual(TEXT("difference 1: 0.85"), GamerCreature::BiomeLootDownscale(1, Combat), 0.85f, 0.0001f);
	TestEqual(TEXT("difference 2: 0.70"), GamerCreature::BiomeLootDownscale(2, Combat), 0.70f, 0.0001f);
	TestEqual(TEXT("difference 3: 0.55"), GamerCreature::BiomeLootDownscale(3, Combat), 0.55f, 0.0001f);
	TestEqual(TEXT("difference 5: 0.55"), GamerCreature::BiomeLootDownscale(5, Combat), 0.55f, 0.0001f);

	// Biome bonus loot is dampened for stragglers ...
	TestEqual(TEXT("C at difference 0"), MeanCount(Loot, TEXT("item_c"), 2, 2, X, -1, 1.f, Combat, 7), 0.5, 0.01);
	TestEqual(TEXT("C at difference 1"), MeanCount(Loot, TEXT("item_c"), 2, 3, X, -1, 1.f, Combat, 8), 0.425, 0.01);
	TestEqual(TEXT("C at difference 3"), MeanCount(Loot, TEXT("item_c"), 2, 5, X, -1, 1.f, Combat, 9), 0.275, 0.01);
	// ... base and rank loot are not.
	TestEqual(TEXT("A is not dampened at difference 3"), MeanCount(Loot, TEXT("item_a"), 2, 5, X, -1, 1.f, Combat, 10), 1.35, 0.027);
	TestEqual(TEXT("B is not dampened at difference 3"), MeanCount(Loot, TEXT("item_b"), 2, 5, X, -1, 1.f, Combat, 11), 0.5, 0.01);
	// Another biome: no bonus loot at all.
	TestEqual(TEXT("no bonus loot in another biome"), MeanCount(Loot, TEXT("item_c"), 2, 2, TEXT("biome_y"), -1, 1.f, Combat, 12), 0.0, 0.0);
	TestEqual(TEXT("no biome given: no bonus loot"), MeanCount(Loot, TEXT("item_c"), 2, 2, NAME_None, -1, 1.f, Combat, 13), 0.0, 0.0);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FGamerLootModifiersTest, "Gamer.Loot.YieldAndLeader",
	EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::EngineFilter)

bool FGamerLootModifiersTest::RunTest(const FString& Parameters)
{
	const FGamerCombatData Combat = LoadCombat(*this);
	const FGamerCreatureLoot Loot = TestLoot();
	const FName X = TEXT("biome_x");
	// Yield 1.5: counts 1 -> 2 and 2 -> 3 (rounded), so mean A = 0.9 * (0.5 * 2 + 0.5 * 3) = 2.25
	TestEqual(TEXT("yield 1.5"), MeanCount(Loot, TEXT("item_a"), 2, 2, X, -1, 1.5f, Combat, 14), 2.25, 0.05);
	// Yield 0.7: counts never drop to 0 once rolled: 1 -> 1 and 2 -> 1, mean A = 0.9
	TestEqual(TEXT("yield 0.7 keeps at least 1"), MeanCount(Loot, TEXT("item_a"), 2, 2, X, -1, 0.7f, Combat, 15), 0.9, 0.02);

	// Leader tiers: the shipped data has +1 base roll for every tier (experienced = index 0), rank loot doubled for the top tier.
	TestTrue(TEXT("three leader tiers"), Combat.Leaders.Num() == 3);
	if (Combat.Leaders.Num() == 3)
	{
		TestEqual(TEXT("experienced: two base rolls double A"), MeanCount(Loot, TEXT("item_a"), 2, 2, X, 0, 1.f, Combat, 16), 2.7, 0.05);
		TestEqual(TEXT("experienced: rank loot not doubled"), MeanCount(Loot, TEXT("item_b"), 2, 2, X, 0, 1.f, Combat, 17), 0.5, 0.02);
		TestEqual(TEXT("leader: two rank rolls, each doubled: B x4"), MeanCount(Loot, TEXT("item_b"), 2, 2, X, 2, 1.f, Combat, 18), 2.0, 0.04);
	}
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FGamerSpawnRankTest, "Gamer.Spawn.RankAndSpread",
	EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::EngineFilter)

bool FGamerSpawnRankTest::RunTest(const FString& Parameters)
{
	const FGamerCombatData Combat = LoadCombat(*this);
	const FName Biome = TEXT("biome_x"); // rank 3, dangerous places rank 4

	// "grows with the zone (85 %)": 85 % at the zone rank, the rest one rank lower.
	{
		const FGamerCreatureDataSet Set = TestSet(0.85f, 0, false);
		FRandomStream Stream(21);
		int32 Top = 0, Lower = 0, Other = 0;
		double RollSum = 0.0;
		bool bScaled = true, bSize = true, bRange = true;
		for (int32 I = 0; I < Rolls; ++I)
		{
			FGamerSpawnRoll Roll;
			if (!GamerCreature::RollSpawn(Set, Combat, Biome, true, Stream, Roll)) { AddError(TEXT("spawn failed")); break; }
			(Roll.Rank == 4 ? Top : Roll.Rank == 3 ? Lower : Other)++;
			RollSum += Roll.Roll;
			bRange &= Roll.Roll >= Combat.SpreadMin && Roll.Roll <= Combat.SpreadMax;
			// One roll for everything: hit points, poise and damage scale by the same factor (leader factor aside).
			const FGamerLeaderDef* Leader = Combat.Leaders.IsValidIndex(Roll.LeaderIndex) ? &Combat.Leaders[Roll.LeaderIndex] : nullptr;
			const float BaseHp = 100.f * Roll.Rank, BasePoise = 10.f * Roll.Rank, BaseDmg = 5.f * Roll.Rank;
			bScaled &= FMath::IsNearlyEqual(Roll.Hp, BaseHp * Roll.Roll * Combat.CreatureHpScale * (Leader ? Leader->Hp : 1.f), 0.01f);
			bScaled &= FMath::IsNearlyEqual(Roll.Poise, BasePoise * Roll.Roll * (Leader ? Leader->Poise : 1.f), 0.01f);
			bScaled &= FMath::IsNearlyEqual(Roll.Damage, BaseDmg * Roll.Roll * (Leader ? Leader->Dmg : 1.f), 0.01f);
			bSize &= FMath::IsNearlyEqual(Roll.Size, (1.f + (Roll.Roll - 1.f) * Combat.SizeFollow) * (Leader ? Leader->Size : 1.f), 0.0001f);
		}
		TestEqual(TEXT("share at the zone rank"), static_cast<double>(Top) / Rolls, 0.85, 0.01);
		TestEqual(TEXT("share one rank lower"), static_cast<double>(Lower) / Rolls, 0.15, 0.01);
		TestEqual(TEXT("no other ranks"), Other, 0);
		TestEqual(TEXT("the mean roll is 1"), RollSum / Rolls, 1.0, 0.005);
		TestTrue(TEXT("roll stays in the spread range"), bRange);
		TestTrue(TEXT("hp, poise and damage scale by the same roll"), bScaled);
		TestTrue(TEXT("size follows the roll"), bSize);
	}

	// Cap: "up to rank 3" at a place of rank 4 never goes above 3.
	{
		const FGamerCreatureDataSet Set = TestSet(1.0f, 3, false);
		FRandomStream Stream(22);
		int32 Max = 0;
		for (int32 I = 0; I < 5000; ++I)
		{
			FGamerSpawnRoll Roll;
			if (GamerCreature::RollSpawn(Set, Combat, Biome, true, Stream, Roll)) { Max = FMath::Max(Max, Roll.Rank); }
		}
		TestEqual(TEXT("rank cap"), Max, 3);
	}
	// Fixed rank (straggler): always rank 2 whatever the zone.
	{
		const FGamerCreatureDataSet Set = TestSet(0.f, 0, true);
		FRandomStream Stream(23);
		bool bAlways2 = true;
		for (int32 I = 0; I < 2000; ++I)
		{
			FGamerSpawnRoll Roll;
			bAlways2 &= GamerCreature::RollSpawn(Set, Combat, Biome, true, Stream, Roll) && Roll.Rank == 2;
		}
		TestTrue(TEXT("fixed rank"), bAlways2);
	}
	// The rank never goes below the minimum rank of the creature.
	{
		const FGamerCreatureDataSet Set = TestSet(0.5f, 0, false);
		FRandomStream Stream(24);
		int32 Min = 99;
		for (int32 I = 0; I < 5000; ++I)
		{
			FGamerSpawnRoll Roll;
			if (GamerCreature::RollSpawn(Set, Combat, Biome, false, Stream, Roll)) { Min = FMath::Min(Min, Roll.Rank); }
		}
		TestEqual(TEXT("minimum rank holds (zone rank 3, one lower would be 2)"), Min, 2);
	}
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FGamerSpawnWeightsTest, "Gamer.Spawn.WeightsAndPlaces",
	EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::EngineFilter)

bool FGamerSpawnWeightsTest::RunTest(const FString& Parameters)
{
	FGamerCreatureDataSet Set;
	FGamerBiomeDef Biome;
	Biome.Id = TEXT("biome_x");
	Biome.Rank = 2;
	Biome.PlaceRank = 3;
	Set.Biomes.Add(Biome);
	for (const TCHAR* Name : { TEXT("creature_a"), TEXT("creature_b"), TEXT("creature_c") })
	{
		FGamerCreatureDef Def;
		Def.Id = Name;
		Def.MinRank = 1;
		Set.Creatures.Add(Def.Id, Def);
	}
	auto AddRule = [&](const TCHAR* Creature, float Weight, bool bOnlyPlaces)
	{
		FGamerSpawnRule Rule;
		Rule.Biome = Biome.Id;
		Rule.Creature = Creature;
		Rule.Weight = Weight;
		Rule.bOnlyPlaces = bOnlyPlaces;
		Set.Spawns.Add(Rule);
	};
	AddRule(TEXT("creature_a"), 1.f, false);
	AddRule(TEXT("creature_b"), 3.f, false);
	AddRule(TEXT("creature_c"), 4.f, true); // dangerous places only

	FRandomStream Stream(31);
	int32 A = 0, B = 0, C = 0;
	for (int32 I = 0; I < Rolls; ++I)
	{
		const FGamerSpawnRule* Rule = GamerCreature::PickSpawnRule(Set, Biome.Id, false, Stream);
		if (!Rule) { AddError(TEXT("no rule picked")); break; }
		(Rule->Creature == TEXT("creature_a") ? A : Rule->Creature == TEXT("creature_b") ? B : C)++;
	}
	TestEqual(TEXT("outside places: share of A is 1/4"), static_cast<double>(A) / Rolls, 0.25, 0.01);
	TestEqual(TEXT("outside places: share of B is 3/4"), static_cast<double>(B) / Rolls, 0.75, 0.01);
	TestEqual(TEXT("outside places: C never appears"), C, 0);

	A = B = C = 0;
	for (int32 I = 0; I < Rolls; ++I)
	{
		const FGamerSpawnRule* Rule = GamerCreature::PickSpawnRule(Set, Biome.Id, true, Stream);
		if (!Rule) { AddError(TEXT("no rule picked")); break; }
		(Rule->Creature == TEXT("creature_a") ? A : Rule->Creature == TEXT("creature_b") ? B : C)++;
	}
	TestEqual(TEXT("at places: share of C is 4/8"), static_cast<double>(C) / Rolls, 0.5, 0.01);
	TestEqual(TEXT("at places: share of B is 3/8"), static_cast<double>(B) / Rolls, 0.375, 0.01);
	TestNull(TEXT("unknown biome: no rule"), GamerCreature::PickSpawnRule(Set, TEXT("biome_none"), false, Stream));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FGamerSpawnLeaderTest, "Gamer.Spawn.Leader",
	EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::EngineFilter)

bool FGamerSpawnLeaderTest::RunTest(const FString& Parameters)
{
	const FGamerCombatData Combat = LoadCombat(*this);
	TestEqual(TEXT("three leader tiers in the data"), Combat.Leaders.Num(), 3);
	if (Combat.Leaders.Num() != 3) { return true; }

	auto Share = [&](int32 Rank, bool bAtPlace, int32 Seed, int32 Index)
	{
		FRandomStream Stream(Seed);
		int32 Count = 0;
		for (int32 I = 0; I < Rolls; ++I)
		{
			if (GamerCreature::RollLeader(Combat, Rank, bAtPlace, Stream) == Index) { ++Count; }
		}
		return static_cast<double>(Count) / Rolls;
	};
	// Rank 3: only "experienced" (from rank 2) can appear: 5 % everywhere, 10 % at places.
	TestEqual(TEXT("rank 3 everywhere: experienced 5 %"), Share(3, false, 41, 0), 0.05, 0.005);
	TestEqual(TEXT("rank 3 at places: experienced 10 %"), Share(3, true, 42, 0), 0.10, 0.007);
	TestEqual(TEXT("rank 3: no veteran"), Share(3, true, 43, 1), 0.0, 0.0);
	TestEqual(TEXT("rank 3: no leader"), Share(3, true, 44, 2), 0.0, 0.0);
	// Rank 4: veteran possible, 3 % everywhere.
	TestEqual(TEXT("rank 4: veteran 3 %"), Share(4, false, 45, 1), 0.03, 0.004);
	// Rank 6 at places: leader 4 %, veteran 6 %, experienced 10 % (disjoint, 20 % together).
	TestEqual(TEXT("rank 6 at places: leader 4 %"), Share(6, true, 46, 2), 0.04, 0.005);
	TestEqual(TEXT("rank 6 at places: veteran 6 %"), Share(6, true, 47, 1), 0.06, 0.006);
	TestEqual(TEXT("rank 6 at places: experienced 10 %"), Share(6, true, 48, 0), 0.10, 0.007);
	// Rank 1: never a leader.
	TestEqual(TEXT("rank 1: no leader at all"), Share(1, true, 49, 0), 0.0, 0.0);
	return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
