#include "GamerDataLoader.h"

#include "HAL/FileManager.h"
#include "Misc/AutomationTest.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"

#if WITH_DEV_AUTOMATION_TESTS

// Meal buffs, tavern and cooking places (Taverne-Session 2026-11-15). Run with tools/test_data.ps1.

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FGamerMealDataTest, "Gamer.Data.MealsAndTavern",
	EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::EngineFilter)

bool FGamerMealDataTest::RunTest(const FString& Parameters)
{
	// 1. Shipped data: every dish has buffs, higher tiers last longer, cooking places need the tavern.
	{
		FGamerDataSet Data;
		FGamerDataReport Report;
		GamerData::LoadFromDirectory(GamerData::DefaultDirectory(), Data, Report);
		TestEqual(TEXT("shipped data has no errors"), Report.Errors.Num(), 0);
		const FGamerItemDef* Meal = Data.Items.Find(TEXT("item_meal"));
		const FGamerItemDef* Cake = Data.Items.Find(TEXT("item_dish_honey_cake"));
		TestTrue(TEXT("item_meal is a meal"), Meal && Meal->Food.IsMeal());
		TestTrue(TEXT("honey cake is a meal"), Cake && Cake->Food.IsMeal());
		if (Meal && Cake)
		{
			TestTrue(TEXT("T6 lasts longer than T1"), Cake->Food.Minutes > Meal->Food.Minutes);
		}
		const FGamerItemDef* Berries = Data.Items.Find(TEXT("item_berries"));
		TestTrue(TEXT("raw berries are a snack (edible, weak)"), Berries && Berries->Food.IsMeal() && Berries->Food.bSnack);
		const FGamerItemDef* Elder = Data.Items.Find(TEXT("item_elderberry"));
		TestTrue(TEXT("raw elderberries are not edible (poisonous)"), Elder && !Elder->Food.IsMeal());
		const FGamerBuildableDef* Oven = Data.Buildables.Find(TEXT("building_clay_oven"));
		TestTrue(TEXT("clay oven needs tavern level 2"), Oven && Oven->Requires.Building == TEXT("building_tavern") && Oven->Requires.Level == 2);
		const FGamerBuildableDef* Tavern = Data.Buildables.Find(TEXT("building_tavern"));
		TestTrue(TEXT("tavern has a stock and 3 levels"), Tavern && Tavern->Tavern.IsTavern() && Tavern->MaxLevel() == 3);
		for (const TPair<FName, FGamerItemDef>& Pair : Data.Items)
		{
			if (Pair.Value.Food.IsMeal() && !Pair.Value.Food.bSnack) // raw snacks: nuts and honey keep
			{
				TestTrue(FString::Printf(TEXT("%s spoils (cooked food spoils too)"), *Pair.Key.ToString()), Pair.Value.SpoilDays > 0.f);
			}
		}
	}

	// 2. Broken data: the validator reports each mistake.
	const FString Dir = FPaths::Combine(FPaths::ProjectSavedDir(), TEXT("Automation"), TEXT("GamerMealBroken"));
	IFileManager::Get().DeleteDirectory(*Dir, false, true);
	IFileManager::Get().MakeDirectory(*Dir, true);
	FFileHelper::SaveStringToFile(TEXT(R"({ "schema": 1, "rows": [
		{ "id": "item_a", "name": "A", "category": "resource", "tier": 1, "max_stack": 10, "weight_kg": 1,
		  "food": { "minutes": 10, "buffs": [ { "stat": "health", "value": 5 } ] } },
		{ "id": "item_b", "name": "B", "category": "food", "tier": 1, "max_stack": 10, "weight_kg": 1,
		  "food": { "minutes": 10, "buffs": [ { "stat": "stamina", "value": 5 }, { "stat": "damage", "value": -3 } ] } }
	] })"), *FPaths::Combine(Dir, TEXT("items.json")));
	FFileHelper::SaveStringToFile(TEXT(R"({ "schema": 1, "rows": [
		{ "id": "res_a", "name": "A", "work_seconds": 1, "drops": [ { "item": "item_a", "min": 1, "max": 1 }, { "item": "item_b", "min": 1, "max": 1 } ] }
	] })"), *FPaths::Combine(Dir, TEXT("resource_nodes.json")));
	FFileHelper::SaveStringToFile(TEXT(R"({ "schema": 1, "rows": [ ] })"), *FPaths::Combine(Dir, TEXT("recipes.json")));
	FFileHelper::SaveStringToFile(TEXT(R"({ "schema": 1, "rows": [
		{ "id": "building_inn", "name": "Inn", "category": "workstation", "cost": [ { "item": "item_a", "count": 1 } ], "build_seconds": 1,
		  "workers": 1, "tavern": { "stock": 4 } },
		{ "id": "building_oven", "name": "Oven", "category": "workstation", "cost": [ { "item": "item_a", "count": 1 } ], "build_seconds": 1,
		  "requires": { "building": "building_inn", "level": 2 } },
		{ "id": "building_pot", "name": "Pot", "category": "workstation", "cost": [ { "item": "item_a", "count": 1 } ], "build_seconds": 1,
		  "requires": { "building": "building_nowhere" } }
	] })"), *FPaths::Combine(Dir, TEXT("buildables.json")));

	FGamerDataSet Data;
	FGamerDataReport Report;
	TestFalse(TEXT("broken meal data reports failure"), GamerData::LoadFromDirectory(Dir, Data, Report, false));
	auto ExpectError = [&](const TCHAR* Needle, const TCHAR* What)
	{
		TestTrue(FString::Printf(TEXT("reports: %s"), What), Report.Errors.ContainsByPredicate([Needle](const FString& E) { return E.Contains(Needle); }));
	};
	ExpectError(TEXT("\"food\" nur bei category \"food\""), TEXT("food block on a resource"));
	ExpectError(TEXT("stat \"stamina\" unbekannt"), TEXT("stat outside the buff catalogue (rule 5)"));
	ExpectError(TEXT("value muss größer als 0"), TEXT("negative buff"));
	ExpectError(TEXT("hat nur 1 Stufen"), TEXT("requires a level the building does not have"));
	ExpectError(TEXT("\"building_nowhere\" gibt es nicht"), TEXT("requires an unknown building"));
	for (const FString& Error : Report.Errors)
	{
		AddInfo(FString::Printf(TEXT("(expected) %s"), *Error));
	}
	IFileManager::Get().DeleteDirectory(*Dir, false, true);
	return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
