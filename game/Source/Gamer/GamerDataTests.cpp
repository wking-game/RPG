#include "GamerDataLoader.h"

#include "HAL/FileManager.h"
#include "Misc/AutomationTest.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"

#if WITH_DEV_AUTOMATION_TESTS

// Run with: tools/test_data.ps1  (or console: Automation RunTests Gamer.Data)

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FGamerDataShippedIsValidTest, "Gamer.Data.ShippedDataIsValid",
	EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::EngineFilter)

bool FGamerDataShippedIsValidTest::RunTest(const FString& Parameters)
{
	FGamerDataSet Data;
	FGamerDataReport Report;
	GamerData::LoadFromDirectory(GamerData::DefaultDirectory(), Data, Report);

	for (const FString& Error : Report.Errors)
	{
		AddError(Error);
	}
	for (const FString& Warning : Report.Warnings)
	{
		AddInfo(Warning);
	}

	TestTrue(TEXT("items loaded"), Data.Items.Num() > 0);
	TestTrue(TEXT("resource nodes loaded"), Data.ResourceNodes.Num() > 0);
	TestTrue(TEXT("recipes loaded"), Data.Recipes.Num() > 0);
	TestTrue(TEXT("buildables loaded"), Data.Buildables.Num() > 0);
	TestTrue(TEXT("item_log exists"), Data.Items.Contains(TEXT("item_log")));
	TestTrue(TEXT("checksum set"), Data.Checksum != 0);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FGamerDataValidatorFindsErrorsTest, "Gamer.Data.ValidatorFindsErrors",
	EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::EngineFilter)

bool FGamerDataValidatorFindsErrorsTest::RunTest(const FString& Parameters)
{
	// Deliberately broken data: every mistake below must be reported.
	const FString Dir = FPaths::Combine(FPaths::ProjectSavedDir(), TEXT("Automation"), TEXT("GamerDataBroken"));
	IFileManager::Get().DeleteDirectory(*Dir, false, true);
	IFileManager::Get().MakeDirectory(*Dir, true);

	FFileHelper::SaveStringToFile(TEXT(R"({ "schema": 1, "rows": [
		{ "id": "item_a", "name": "A", "category": "resource", "tier": 1, "max_stack": 10, "weight_kg": 1 },
		{ "id": "item_a", "name": "A again", "category": "resource", "tier": 1, "max_stack": 10, "weight_kg": 1 },
		{ "id": "item_b", "name": "B", "category": "resource", "tier": 9, "max_stack": 10, "wieght_kg": 1 },
		{ "id": "Item-C", "name": "C", "category": "resource", "tier": 1, "max_stack": 10, "weight_kg": 1 },
		{ "id": "tool_saw_t1", "name": "Saw", "category": "tool", "tier": 1, "max_stack": 1, "weight_kg": 1,
		  "tool": { "type": "saw", "durability": 10, "work_speed": 1.0 } }
	] })"), *FPaths::Combine(Dir, TEXT("items.json")));

	FFileHelper::SaveStringToFile(TEXT(R"({ "schema": 1, "rows": [
		{ "id": "res_x", "name": "X", "work_seconds": 1, "drops": [ { "item": "item_missing", "min": 1, "max": 2 } ] },
		{ "id": "res_y", "name": "Y", "work_seconds": 1, "drops": [ { "item": "item_a", "min": 5, "max": 2 } ],
		  "required_tool": { "type": "axe", "min_tier": 1 } }
	] })"), *FPaths::Combine(Dir, TEXT("resource_nodes.json")));

	FFileHelper::SaveStringToFile(TEXT(R"({ "schema": 1, "rows": [
		{ "id": "recipe_z", "station": "building_missing", "inputs": [ { "item": "item_a", "count": 0 } ],
		  "outputs": [ { "item": "item_a", "count": 1 } ], "work_seconds": 1 }
	] })"), *FPaths::Combine(Dir, TEXT("recipes.json")));

	FFileHelper::SaveStringToFile(TEXT(R"({ "schema": 1, "rows": [
		{ "id": "building_w", "name": "W", "category": "structure", "cost": [ { "item": "item_a", "count": 1 } ], "build_seconds": 1 }
	] })"), *FPaths::Combine(Dir, TEXT("buildables.json")));

	FGamerDataSet Data;
	FGamerDataReport Report;
	const bool bOk = GamerData::LoadFromDirectory(Dir, Data, Report, false);
	TestFalse(TEXT("loading broken data reports failure"), bOk);

	auto ExpectError = [&](const TCHAR* Needle, const TCHAR* What)
	{
		const bool bFound = Report.Errors.ContainsByPredicate([Needle](const FString& E) { return E.Contains(Needle); });
		TestTrue(FString::Printf(TEXT("reports: %s"), What), bFound);
	};
	ExpectError(TEXT("ID doppelt vergeben"), TEXT("duplicate id"));
	ExpectError(TEXT("tier muss zwischen"), TEXT("tier out of range"));
	ExpectError(TEXT("unbekanntes Feld \"wieght_kg\""), TEXT("typo in field name"));
	ExpectError(TEXT("snake_case"), TEXT("bad id format"));
	ExpectError(TEXT("\"item_missing\" gibt es nicht"), TEXT("unknown item reference"));
	ExpectError(TEXT("min <= max"), TEXT("min greater than max"));
	ExpectError(TEXT("Werkzeugtyp \"axe\""), TEXT("unknown tool type"));
	ExpectError(TEXT("Station \"building_missing\""), TEXT("unknown station"));
	ExpectError(TEXT("\"count\" muss eine ganze Zahl ab 1"), TEXT("count 0"));
	ExpectError(TEXT("Pflichtfeld \"snap\""), TEXT("structure without snap"));
	ExpectError(TEXT("(tool_saw_t1): nicht erreichbar ab Spielstart"), TEXT("item that can never be obtained"));

	for (const FString& Error : Report.Errors)
	{
		AddInfo(FString::Printf(TEXT("(expected) %s"), *Error));
	}
	IFileManager::Get().DeleteDirectory(*Dir, false, true);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FGamerDataDetectsToolDeadlockTest, "Gamer.Data.DetectsToolDeadlock",
	EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::EngineFilter)

bool FGamerDataDetectsToolDeadlockTest::RunTest(const FString& Parameters)
{
	// The mistake from 2026-11-13: the axe recipe needs a log, but logs only come from trees that need an axe.
	const FString Dir = FPaths::Combine(FPaths::ProjectSavedDir(), TEXT("Automation"), TEXT("GamerDataDeadlock"));
	IFileManager::Get().DeleteDirectory(*Dir, false, true);
	IFileManager::Get().MakeDirectory(*Dir, true);

	FFileHelper::SaveStringToFile(TEXT(R"({ "schema": 1, "rows": [
		{ "id": "item_log", "name": "Log", "category": "resource", "tier": 1, "max_stack": 10, "weight_kg": 1 },
		{ "id": "item_stone", "name": "Stone", "category": "resource", "tier": 1, "max_stack": 10, "weight_kg": 1 },
		{ "id": "tool_axe_t1", "name": "Axe", "category": "tool", "tier": 1, "max_stack": 1, "weight_kg": 1,
		  "tool": { "type": "axe", "durability": 10, "work_speed": 1.0 } }
	] })"), *FPaths::Combine(Dir, TEXT("items.json")));
	FFileHelper::SaveStringToFile(TEXT(R"({ "schema": 1, "rows": [
		{ "id": "res_tree", "name": "Tree", "work_seconds": 1, "required_tool": { "type": "axe", "min_tier": 1 },
		  "drops": [ { "item": "item_log", "min": 1, "max": 1 } ] },
		{ "id": "res_loose_stone", "name": "Stones", "work_seconds": 1, "drops": [ { "item": "item_stone", "min": 1, "max": 1 } ] }
	] })"), *FPaths::Combine(Dir, TEXT("resource_nodes.json")));
	FFileHelper::SaveStringToFile(TEXT(R"({ "schema": 1, "rows": [
		{ "id": "recipe_axe_t1", "inputs": [ { "item": "item_log", "count": 1 }, { "item": "item_stone", "count": 2 } ],
		  "outputs": [ { "item": "tool_axe_t1", "count": 1 } ], "work_seconds": 1 }
	] })"), *FPaths::Combine(Dir, TEXT("recipes.json")));
	FFileHelper::SaveStringToFile(TEXT(R"({ "schema": 1, "rows": [ ] })"), *FPaths::Combine(Dir, TEXT("buildables.json")));

	FGamerDataSet Data;
	FGamerDataReport Report;
	const bool bOk = GamerData::LoadFromDirectory(Dir, Data, Report, false);
	TestFalse(TEXT("deadlocked data reports failure"), bOk);

	auto Has = [&](const TCHAR* Needle)
	{
		return Report.Errors.ContainsByPredicate([Needle](const FString& E) { return E.Contains(Needle); });
	};
	TestTrue(TEXT("axe reported unreachable"), Has(TEXT("(tool_axe_t1): nicht erreichbar")));
	TestTrue(TEXT("log reported unreachable"), Has(TEXT("(item_log): nicht erreichbar")));
	TestTrue(TEXT("tree reported unreachable"), Has(TEXT("(res_tree): nicht erreichbar")));
	TestTrue(TEXT("recipe reported unreachable"), Has(TEXT("(recipe_axe_t1): nicht erreichbar")));
	TestFalse(TEXT("loose stones are reachable"), Has(TEXT("(res_loose_stone): nicht erreichbar")));

	for (const FString& Error : Report.Errors)
	{
		AddInfo(FString::Printf(TEXT("(expected) %s"), *Error));
	}
	IFileManager::Get().DeleteDirectory(*Dir, false, true);
	return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
