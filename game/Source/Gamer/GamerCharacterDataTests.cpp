#include "GamerCharacterData.h"
#include "GamerDataLoader.h"

#include "HAL/FileManager.h"
#include "Misc/AutomationTest.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"

#if WITH_DEV_AUTOMATION_TESTS

// Run with: tools/test_data.ps1  (filter Gamer.Data)

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FGamerCharacterDataShippedTest, "Gamer.Data.CharactersAreValid",
	EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::EngineFilter)

bool FGamerCharacterDataShippedTest::RunTest(const FString& Parameters)
{
	FGamerDataSet Base;
	FGamerDataReport BaseReport;
	GamerData::LoadFromDirectory(GamerData::DefaultDirectory(), Base, BaseReport, false);

	FGamerCharacterDataSet Characters;
	FGamerDataReport Report;
	GamerData::LoadCharacters(GamerData::DefaultDirectory(), Base, Characters, Report);
	for (const FString& Error : Report.Errors)
	{
		AddError(Error);
	}
	for (const FString& Warning : Report.Warnings)
	{
		AddInfo(Warning);
	}

	TestEqual(TEXT("five origins"), Characters.Origins.Num(), 5);
	TestEqual(TEXT("five stats"), Characters.Stats.Num(), 5);
	TestTrue(TEXT("appearance slots"), Characters.LookSlots.Num() > 0);
	for (const FGamerOriginDef& Origin : Characters.Origins)
	{
		TestTrue(*FString::Printf(TEXT("%s has start items"), *Origin.Id.ToString()), Origin.StartItems.Num() > 0);
	}

	// Sanitize: a profile from an unknown origin with a bad name and bad look becomes a valid one.
	FGamerCharacterProfile Bad;
	Bad.Origin = TEXT("origin_does_not_exist");
	Bad.Name = TEXT("  <script>Ragnarrrrrrrrrrrrrrrrrrrrr  ");
	Bad.Appearance.Add({ TEXT("look_body"), TEXT("body_dragon") });
	TestFalse(TEXT("bad profile reported"), Characters.Sanitize(Bad));
	TestNotNull(TEXT("origin fixed"), Characters.FindOrigin(Bad.Origin));
	TestTrue(TEXT("name shortened"), Bad.Name.Len() <= Characters.NameMaxLength && !Bad.Name.Contains(TEXT("<")));
	TestEqual(TEXT("every slot filled"), Bad.Appearance.Num(), Characters.LookSlots.Num());
	for (const FGamerLookChoice& Choice : Bad.Appearance)
	{
		const FGamerLookSlotDef* Slot = Characters.FindLookSlot(Choice.Slot);
		TestTrue(*FString::Printf(TEXT("%s valid"), *Choice.Slot.ToString()), Slot && Slot->HasOption(Choice.Option));
	}

	// A fresh profile is already valid.
	if (Characters.Origins.Num() > 0)
	{
		FGamerCharacterProfile Good = Characters.MakeProfile(Characters.Origins.Last().Id, TEXT("Sigrun"));
		TestTrue(TEXT("made profile is valid"), Characters.Sanitize(Good));
		TestEqual(TEXT("name kept"), Good.Name, FString(TEXT("Sigrun")));
	}
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FGamerCharacterValidatorTest, "Gamer.Data.CharacterValidatorFindsErrors",
	EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::EngineFilter)

bool FGamerCharacterValidatorTest::RunTest(const FString& Parameters)
{
	const FString Dir = FPaths::Combine(FPaths::ProjectSavedDir(), TEXT("Automation"), TEXT("GamerCharactersBroken"));
	IFileManager::Get().DeleteDirectory(*Dir, false, true);
	IFileManager::Get().MakeDirectory(*Dir, true);

	FGamerDataSet Base;
	FGamerItemDef Bow;
	Bow.Id = TEXT("item_bow");
	Bow.MaxStack = 1;
	Base.Items.Add(Bow.Id, Bow);

	FFileHelper::SaveStringToFile(TEXT(R"({ "schema": 1,
		"rules": { "stat_sum": 10, "stat_min": 1, "stat_max": 6, "name_max_length": 8, "own_branch_cost_factor": 0.5 },
		"stats": [ { "id": "stat_a", "name": "A", "short": "A", "affects": "x" }, { "id": "stat_b", "name": "B", "short": "B", "affects": "x" } ],
		"abilities": [ { "id": "ability_x", "name": "X", "description": "x", "branch": "origin_one", "cost": 1 },
		               { "id": "ability_y", "name": "Y", "description": "y", "branch": "origin_nowhere", "cost": 1 } ],
		"appearance": [ { "id": "look_body", "name": "Body", "options": [ { "id": "body_a", "name": "a" }, { "id": "body_a", "name": "dup" } ] } ],
		"names": [ "Averyverylongname" ],
		"origins": [
			{ "id": "origin_one", "name": "One", "role": "r", "description": "d", "stats": { "stat_a": 5, "stat_b": 4 },
			  "start_items": [ { "item": "item_bow", "count": 2 } ], "start_ability": "ability_x" },
			{ "id": "origin_two", "name": "Two", "role": "r", "description": "d", "stats": { "stat_a": 9, "stat_c": 1 },
			  "start_items": [ { "item": "item_missing", "count": 1 } ], "start_ability": "ability_x",
			  "appearance": { "look_body": "body_z" }, "colour": "red" }
		] })"), *FPaths::Combine(Dir, GamerData::CharactersFile));

	FGamerCharacterDataSet Characters;
	FGamerDataReport Report;
	TestFalse(TEXT("broken data fails"), GamerData::LoadCharacters(Dir, Base, Characters, Report));
	auto Has = [&Report](const TCHAR* Part)
	{
		return Report.Errors.ContainsByPredicate([Part](const FString& E) { return E.Contains(Part); });
	};
	TestTrue(TEXT("sum checked"), Has(TEXT("Summe der Werte ist 9, muss 10 sein")));
	TestTrue(TEXT("stat limit checked"), Has(TEXT("\"stat_a\" muss zwischen 1 und 6 liegen")));
	TestTrue(TEXT("unknown stat"), Has(TEXT("Wert \"stat_c\" gibt es nicht")));
	TestTrue(TEXT("missing stat"), Has(TEXT("Pflichtfeld \"stat_b\" fehlt")));
	TestTrue(TEXT("stack too big"), Has(TEXT("passen nicht in einen Platz")));
	TestTrue(TEXT("missing item"), Has(TEXT("\"item_missing\" gibt es nicht")));
	TestTrue(TEXT("foreign start ability"), Has(TEXT("gehört nicht zum eigenen Zweig")));
	TestTrue(TEXT("unknown branch"), Has(TEXT("branch \"origin_nowhere\" ist keine Herkunft")));
	TestTrue(TEXT("duplicate option"), Has(TEXT("Option \"body_a\" doppelt")));
	TestTrue(TEXT("unknown look option"), Has(TEXT("\"body_z\" ist keine Option")));
	TestTrue(TEXT("typo field"), Has(TEXT("unbekanntes Feld \"colour\"")));
	TestTrue(TEXT("long name"), Has(TEXT("länger als name_max_length")));

	FGamerDataReport MissingReport;
	FGamerCharacterDataSet Empty;
	TestFalse(TEXT("missing file is an error when required"), GamerData::LoadCharacters(Dir / TEXT("nothing"), Base, Empty, MissingReport));
	return true;
}

#endif
