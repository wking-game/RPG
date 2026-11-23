#include "GamerProgression.h"
#include "GamerDataLoader.h"

#include "HAL/FileManager.h"
#include "Misc/AutomationTest.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"

#if WITH_DEV_AUTOMATION_TESTS

// Run with: tools/test_data.ps1 -Filter Gamer.Progression   and   -Filter Gamer.Recipes

namespace
{
	FGamerProgressionData LoadShippedProgression(FAutomationTestBase& Test)
	{
		FGamerProgressionData Data;
		FGamerDataReport Report;
		GamerData::LoadProgression(GamerData::DefaultDirectory(), Data, Report, true);
		for (const FString& Error : Report.Errors) { Test.AddError(Error); }
		return Data;
	}
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FGamerProgressionShippedTest, "Gamer.Progression.ShippedDataIsValid",
	EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::EngineFilter)

bool FGamerProgressionShippedTest::RunTest(const FString& Parameters)
{
	const FGamerProgressionData Data = LoadShippedProgression(*this);
	TestTrue(TEXT("loaded"), Data.bLoaded);
	TestEqual(TEXT("max level"), Data.MaxLevel, 50);
	TestEqual(TEXT("six tiers"), Data.Tiers.Num(), 6);
	TestEqual(TEXT("49 level-ups"), Data.XpToNext.Num(), 49);
	TestTrue(TEXT("checksum set"), Data.Checksum != 0);
	// About 66 hours at 460 experience per tier-hour: 122 360 in total (rounded to 5 per level).
	const int64 Total = Data.TotalXpForLevel(50);
	TestTrue(FString::Printf(TEXT("total experience %lld close to 122 360"), Total), Total > 121000 && Total < 123500);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FGamerProgressionCurveTest, "Gamer.Progression.Curve",
	EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::EngineFilter)

bool FGamerProgressionCurveTest::RunTest(const FString& Parameters)
{
	const FGamerProgressionData Data = LoadShippedProgression(*this);

	// The need per level rises strictly, also across tier borders.
	for (int32 Level = 2; Level < Data.MaxLevel; ++Level)
	{
		TestTrue(FString::Printf(TEXT("need rises at level %d"), Level), Data.XpToNextLevel(Level) > Data.XpToNextLevel(Level - 1));
	}
	TestEqual(TEXT("nothing to reach after the maximum level"), Data.XpToNextLevel(50), 0);

	// Level <-> experience round trip, and one point less is one level less.
	for (int32 Level = 1; Level <= Data.MaxLevel; ++Level)
	{
		const int64 Needed = Data.TotalXpForLevel(Level);
		TestEqual(FString::Printf(TEXT("level %d reached with its experience"), Level), Data.LevelForXp(Needed), Level);
		if (Level >= 2)
		{
			TestEqual(FString::Printf(TEXT("one point short of level %d"), Level), Data.LevelForXp(Needed - 1), Level - 1);
		}
	}
	TestEqual(TEXT("no experience: level 1"), Data.LevelForXp(0), 1);
	TestEqual(TEXT("more than needed stays at the maximum"), Data.LevelForXp(Data.TotalXpForLevel(50) * 10), 50);

	// Tiers cover the levels without gaps: T1 1-8, T2 9-16, T3 17-25, T4 26-34, T5 35-42, T6 43-50.
	const int32 Borders[][3] = { { 1, 1, 1 }, { 8, 1, 1 }, { 9, 2, 2 }, { 16, 2, 2 }, { 17, 3, 3 }, { 25, 3, 3 }, { 26, 4, 4 }, { 34, 4, 4 }, { 35, 5, 5 }, { 42, 5, 5 }, { 43, 6, 6 }, { 50, 6, 6 } };
	for (const int32(&Border)[3] : Borders)
	{
		const FGamerTierDef* Tier = Data.TierForLevel(Border[0]);
		TestTrue(FString::Printf(TEXT("level %d has a tier"), Border[0]), Tier != nullptr);
		if (Tier)
		{
			TestEqual(FString::Printf(TEXT("level %d is in T%d"), Border[0], Border[1]), Tier->Tier, Border[1]);
		}
	}
	TestNull(TEXT("level 0 has no tier"), Data.TierForLevel(0));
	TestNull(TEXT("level 51 has no tier"), Data.TierForLevel(51));

	// Pacing: the first level comes within the first hour, and the total fits the planned 66 hours (460 per tier-hour).
	TestTrue(TEXT("first level-up well within the first hour"), Data.XpToNextLevel(1) < Data.TypicalHourXpPerTier);
	double Hours = 0.0;
	for (const FGamerTierDef& Tier : Data.Tiers) { Hours += Tier.Hours; }
	TestEqual(TEXT("planned hours"), Hours, 66.0, 0.001);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FGamerProgressionPointsTest, "Gamer.Progression.SkillPointsAndSources",
	EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::EngineFilter)

bool FGamerProgressionPointsTest::RunTest(const FString& Parameters)
{
	const FGamerProgressionData Data = LoadShippedProgression(*this);
	TestEqual(TEXT("level 1: no points yet"), Data.SkillPointsForLevel(1), 0);
	TestEqual(TEXT("level 4: three points"), Data.SkillPointsForLevel(4), 3);
	TestEqual(TEXT("level 5: four plus one bonus"), Data.SkillPointsForLevel(5), 5);
	TestEqual(TEXT("level 10: nine plus two bonuses"), Data.SkillPointsForLevel(10), 11);
	TestEqual(TEXT("level 50: 59 points"), Data.SkillPointsForLevel(50), 59);
	TestEqual(TEXT("levels above the maximum do not add"), Data.SkillPointsForLevel(80), 59);

	TestEqual(TEXT("kill xp rank 3"), Data.KillXp(3, -1), 30.f, 0.001f);
	TestEqual(TEXT("kill xp rank 3, experienced"), Data.KillXp(3, 0), 45.f, 0.001f);
	TestEqual(TEXT("kill xp rank 3, veteran"), Data.KillXp(3, 1), 60.f, 0.001f);
	TestEqual(TEXT("kill xp rank 3, leader"), Data.KillXp(3, 2), 90.f, 0.001f);

	TestEqual(TEXT("same tier: full experience"), Data.UnderLevelFactor(0), 1.f, 0.001f);
	TestEqual(TEXT("one tier below: 75 %"), Data.UnderLevelFactor(1), 0.75f, 0.001f);
	TestEqual(TEXT("two tiers below: 25 %"), Data.UnderLevelFactor(2), 0.25f, 0.001f);
	TestEqual(TEXT("three tiers below: 10 %"), Data.UnderLevelFactor(3), 0.10f, 0.001f);
	TestEqual(TEXT("far below stays at 10 %"), Data.UnderLevelFactor(9), 0.10f, 0.001f);
	TestEqual(TEXT("higher tier than the player: full"), Data.UnderLevelFactor(-2), 1.f, 0.001f);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FGamerProgressionValidatorTest, "Gamer.Progression.ValidatorFindsErrors",
	EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::EngineFilter)

bool FGamerProgressionValidatorTest::RunTest(const FString& Parameters)
{
	const FString Dir = FPaths::Combine(FPaths::ProjectSavedDir(), TEXT("Automation"), TEXT("GamerProgressionBroken"));
	IFileManager::Get().DeleteDirectory(*Dir, false, true);
	IFileManager::Get().MakeDirectory(*Dir, true);

	FFileHelper::SaveStringToFile(TEXT(R"({ "schema": 1, "max_level": 10,
		"typical_hour": { "xp_per_tier": 100 },
		"tiers": [
			{ "tier": 1, "biome": "biome_a", "level_min": 1, "level_max": 5, "hours": 1, "weapon_damage": [10, 20], "weapon_poise": [5, 8],
			  "armor": [10, 20], "poise_resist": [10, 20], "resist_cap": 0.1 },
			{ "tier": 2, "biome": "biome_b", "level_min": 7, "level_max": 10, "hours": 1, "weapon_damage": [15, 30], "weapon_poise": [9, 12],
			  "armor": [25, 30], "poise_resist": [25, 30], "resist_cap": 0.05 } ],
		"xp_to_next": [10, 20, 20, 30, 40, 50, 60, 70, 80],
		"skill_points": { "per_level": 1, "bonus_every": 0, "bonus": 1 },
		"xp_sources": { "kill_per_rank": 10, "leader_multipliers": [1.5, 1.2], "boss_first_win_per_tier": 300, "quest_stage_per_tier": 50,
		                "discovery_per_tier": 25, "first_craft_per_tier": 20, "repeat_craft_per_tier": 1, "build_piece_per_tier": 2, "gather_per_tier": 0.5 },
		"coop": { "radius_cm": 0, "share_combat": 1, "share_work": 0.25 },
		"under_level_factor": [0.9, 0.5] })"), *FPaths::Combine(Dir, GamerData::ProgressionFile));

	FGamerProgressionData Data;
	FGamerDataReport Report;
	const bool bOk = GamerData::LoadProgression(Dir, Data, Report, true);
	TestFalse(TEXT("loading broken data reports failure"), bOk);

	auto ExpectError = [&](const TCHAR* Needle, const TCHAR* What)
	{
		const bool bFound = Report.Errors.ContainsByPredicate([Needle](const FString& E) { return E.Contains(Needle); });
		TestTrue(FString::Printf(TEXT("reports: %s"), What), bFound);
	};
	ExpectError(TEXT("tiers muss genau 6 Stufen enthalten"), TEXT("wrong number of tiers"));
	ExpectError(TEXT("level_min ist 7, erwartet 6"), TEXT("gap between level ranges"));
	ExpectError(TEXT("überlappen die vorige Stufe"), TEXT("equipment values overlap"));
	ExpectError(TEXT("resist_cap muss gegenüber der vorigen Stufe steigen"), TEXT("resist cap does not rise"));
	ExpectError(TEXT("xp_to_next steigt nicht streng bei Level 3"), TEXT("experience curve not rising"));
	ExpectError(TEXT("passt nicht zu Stunden"), TEXT("tier sum does not fit the hours"));
	ExpectError(TEXT("bonus_every mindestens 1"), TEXT("bonus every 0"));
	ExpectError(TEXT("leader_multipliers müssen Zahlen sein, die ab 1 aufsteigen"), TEXT("leader multipliers falling"));
	ExpectError(TEXT("radius_cm muss größer als 0 sein"), TEXT("co-op radius 0"));
	ExpectError(TEXT("under_level_factor: Zahlen größer als 0, die mit 1,0 beginnen"), TEXT("under-level factors not starting with 1"));

	for (const FString& Error : Report.Errors)
	{
		AddInfo(FString::Printf(TEXT("(expected) %s"), *Error));
	}
	IFileManager::Get().DeleteDirectory(*Dir, false, true);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FGamerRecipesMinLevelTest, "Gamer.Recipes.MinLevel",
	EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::EngineFilter)

bool FGamerRecipesMinLevelTest::RunTest(const FString& Parameters)
{
	// Shipped recipes have no level requirement yet.
	{
		FGamerDataSet Data;
		FGamerDataReport Report;
		GamerData::LoadFromDirectory(GamerData::DefaultDirectory(), Data, Report);
		for (const TPair<FName, FGamerRecipeDef>& Pair : Data.Recipes)
		{
			TestEqual(FString::Printf(TEXT("%s: no level requirement yet"), *Pair.Key.ToString()), Pair.Value.MinLevel, 0);
		}
	}

	const FString Dir = FPaths::Combine(FPaths::ProjectSavedDir(), TEXT("Automation"), TEXT("GamerRecipesMinLevel"));
	IFileManager::Get().DeleteDirectory(*Dir, false, true);
	IFileManager::Get().MakeDirectory(*Dir, true);
	FFileHelper::SaveStringToFile(TEXT(R"({ "schema": 1, "rows": [
		{ "id": "item_a", "name": "A", "category": "resource", "tier": 1, "max_stack": 10, "weight_kg": 1 } ] })"), *FPaths::Combine(Dir, TEXT("items.json")));
	FFileHelper::SaveStringToFile(TEXT(R"({ "schema": 1, "rows": [] })"), *FPaths::Combine(Dir, TEXT("resource_nodes.json")));
	FFileHelper::SaveStringToFile(TEXT(R"({ "schema": 1, "rows": [] })"), *FPaths::Combine(Dir, TEXT("buildables.json")));
	FFileHelper::SaveStringToFile(TEXT(R"({ "schema": 1, "rows": [
		{ "id": "recipe_ok", "inputs": [ { "item": "item_a", "count": 1 } ], "outputs": [ { "item": "item_a", "count": 2 } ], "work_seconds": 1, "min_level": 5 },
		{ "id": "recipe_bad", "inputs": [ { "item": "item_a", "count": 1 } ], "outputs": [ { "item": "item_a", "count": 2 } ], "work_seconds": 1, "min_level": 99 },
		{ "id": "recipe_none", "inputs": [ { "item": "item_a", "count": 1 } ], "outputs": [ { "item": "item_a", "count": 2 } ], "work_seconds": 1 } ] })"),
		*FPaths::Combine(Dir, TEXT("recipes.json")));

	FGamerDataSet Data;
	FGamerDataReport Report;
	GamerData::LoadFromDirectory(Dir, Data, Report, false);
	const FGamerRecipeDef* Ok = Data.Recipes.Find(TEXT("recipe_ok"));
	const FGamerRecipeDef* None = Data.Recipes.Find(TEXT("recipe_none"));
	TestTrue(TEXT("recipe with min_level loaded"), Ok != nullptr);
	TestTrue(TEXT("recipe without min_level loaded"), None != nullptr);
	if (Ok) { TestEqual(TEXT("min_level 5 read"), Ok->MinLevel, 5); }
	if (None) { TestEqual(TEXT("no min_level means 0"), None->MinLevel, 0); }
	TestTrue(TEXT("min_level 99 is reported"), Report.Errors.ContainsByPredicate([](const FString& E) { return E.Contains(TEXT("min_level muss zwischen 0 und 50 liegen")); }));
	IFileManager::Get().DeleteDirectory(*Dir, false, true);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FGamerProgressionWorkXpTest, "Gamer.Progression.WorkXp",
	EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::EngineFilter)

bool FGamerProgressionWorkXpTest::RunTest(const FString& Parameters)
{
	const FGamerProgressionData Data = LoadShippedProgression(*this);
	// docs/21 section 3: first craft 20 x tier (each further piece in the same batch 1 x tier), repeat 1 x tier per piece.
	TestEqual(TEXT("first craft, tier 1"), Data.CraftXp(1, 1, true), 20.f, 0.001f);
	TestEqual(TEXT("first craft, tier 3"), Data.CraftXp(3, 1, true), 60.f, 0.001f);
	TestEqual(TEXT("first craft of 4 pieces, tier 2"), Data.CraftXp(2, 4, true), 2.f * (20.f + 3.f), 0.001f);
	TestEqual(TEXT("repeat craft of 5, tier 2"), Data.CraftXp(2, 5, false), 10.f, 0.001f);
	TestEqual(TEXT("nothing made"), Data.CraftXp(2, 0, true), 0.f);
	TestEqual(TEXT("gather 10 pieces, tier 4"), Data.GatherXp(4, 10), 20.f, 0.001f);
	TestEqual(TEXT("build piece, tier 5"), Data.BuildXp(5), 10.f, 0.001f);
	TestEqual(TEXT("tier below 1 counts as 1"), Data.GatherXp(0, 2), 1.f, 0.001f);
	// The first levels are reachable by simple work: one hour of crafting, building and gathering must not skip more than a few levels.
	TestTrue(TEXT("a first craft is a noticeable step toward level 2"), Data.CraftXp(1, 1, true) / static_cast<float>(Data.XpToNextLevel(1)) > 0.1f);
	return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
