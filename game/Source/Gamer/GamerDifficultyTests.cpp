#include "GamerDifficulty.h"
#include "GamerDataLoader.h"

#include "HAL/FileManager.h"
#include "Misc/AutomationTest.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"

#if WITH_DEV_AUTOMATION_TESTS

// Run with: tools/test_data.ps1  (filter Gamer.Data)

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FGamerDifficultyShippedTest, "Gamer.Data.DifficultyPresets",
	EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::EngineFilter)

bool FGamerDifficultyShippedTest::RunTest(const FString& Parameters)
{
	TArray<FGamerDifficultyPreset> Presets;
	FGamerDataReport Report;
	GamerData::LoadDifficulty(GamerData::DefaultDirectory(), Presets, Report);
	for (const FString& Error : Report.Errors)
	{
		AddError(Error);
	}
	TestEqual(TEXT("four presets"), Presets.Num(), 4);

	// "Mittel" must equal the defaults of a new world, so an untouched world is "Mittel", not "Benutzerdefiniert".
	const FGamerDifficultyPreset* Medium = GamerData::FindDifficulty(Presets, EGamerDifficulty::Medium);
	TestTrue(TEXT("medium = defaults"), Medium && Medium->Matches(FGamerWorldOptions()));

	// Every preset survives the trip through the map URL (that is how the host sends them to the server).
	for (const FGamerDifficultyPreset& Preset : Presets)
	{
		FGamerWorldOptions Options;
		Options.PlayMode = EGamerPlayMode::Free;
		Options.MaxPlayers = 3;
		Preset.ApplyTo(Options);
		const FGamerWorldOptions Back = FGamerWorldOptions::FromUrlOptions(Options.ToUrlOptions());
		TestTrue(*FString::Printf(TEXT("%s survives the URL"), *Preset.Id.ToString()), Preset.Matches(Back) && Back.Difficulty == Preset.Level);
		TestTrue(TEXT("mode and players survive the URL"), Back.PlayMode == EGamerPlayMode::Free && Back.MaxPlayers == 3);
	}

	// Server side: a preset level with wrong values is corrected; "custom" keeps its values.
	if (const FGamerDifficultyPreset* Hard = GamerData::FindDifficulty(Presets, EGamerDifficulty::Hard))
	{
		FGamerWorldOptions Tampered;
		Hard->ApplyTo(Tampered);
		Tampered.DamageTaken = 0.1f;
		TestTrue(TEXT("tampered preset corrected"), GamerData::EnforceDifficulty(Presets, Tampered));
		TestEqual(TEXT("damage from the data"), Tampered.DamageTaken, Hard->DamageTaken);

		FGamerWorldOptions Custom = Tampered;
		Custom.Difficulty = EGamerDifficulty::Custom;
		Custom.DamageTaken = 0.1f;
		TestFalse(TEXT("custom not touched"), GamerData::EnforceDifficulty(Presets, Custom));
		TestEqual(TEXT("custom keeps its value"), Custom.DamageTaken, 0.1f);
	}
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FGamerDifficultyValidatorTest, "Gamer.Data.DifficultyValidatorFindsErrors",
	EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::EngineFilter)

bool FGamerDifficultyValidatorTest::RunTest(const FString& Parameters)
{
	const FString Dir = FPaths::Combine(FPaths::ProjectSavedDir(), TEXT("Automation"), TEXT("GamerDifficultyBroken"));
	IFileManager::Get().DeleteDirectory(*Dir, false, true);
	IFileManager::Get().MakeDirectory(*Dir, true);
	FFileHelper::SaveStringToFile(TEXT(R"({ "schema": 1, "presets": [
		{ "id": "difficulty_easy", "level": "easy", "name": "E", "death": "maybe", "needs_rate": 0.7, "damage_taken": 9, "yield": 1.5,
		  "hordes": false, "villagers_immortal": true, "demolish": "full", "gespuer_tempo": "fast", "colour": "red" },
		{ "id": "difficulty_easy2", "level": "easy", "name": "E2", "death": "none", "needs_rate": 1, "damage_taken": 1, "yield": 1,
		  "hordes": false, "villagers_immortal": false, "demolish": "full", "gespuer_tempo": "normal" },
		{ "id": "difficulty_medium", "level": "medium", "name": "M", "death": "grave", "needs_rate": 1, "damage_taken": 1,
		  "hordes": "no", "villagers_immortal": false, "demolish": "full", "gespuer_tempo": "normal" }
	] })"), *FPaths::Combine(Dir, GamerData::DifficultyFile));

	TArray<FGamerDifficultyPreset> Presets;
	FGamerDataReport Report;
	TestFalse(TEXT("broken data fails"), GamerData::LoadDifficulty(Dir, Presets, Report));
	auto Has = [&Report](const TCHAR* Part)
	{
		return Report.Errors.ContainsByPredicate([Part](const FString& E) { return E.Contains(Part); });
	};
	TestTrue(TEXT("unknown death"), Has(TEXT("death \"maybe\" unbekannt")));
	TestTrue(TEXT("factor range"), Has(TEXT("damage_taken muss zwischen")));
	TestTrue(TEXT("typo field"), Has(TEXT("unbekanntes Feld \"colour\"")));
	TestTrue(TEXT("level twice"), Has(TEXT("level doppelt")));
	TestTrue(TEXT("missing yield"), Has(TEXT("\"yield\" fehlt")));
	TestTrue(TEXT("bool type"), Has(TEXT("\"hordes\" fehlt oder ist nicht true/false")));
	TestTrue(TEXT("missing levels"), Has(TEXT("Stufe \"hard\" fehlt")) && Has(TEXT("Stufe \"hardcore\" fehlt")));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FGamerWorldOptionsOldSavesTest, "Gamer.Data.WorldOptionsOldSaves",
	EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::EngineFilter)

bool FGamerWorldOptionsOldSavesTest::RunTest(const FString& Parameters)
{
	// The world option "Sagenwelt" was removed on 2026-11-22. Saves written before still contain ?Sagenwelt=...:
	// it is ignored and every other value loads (string as in Saved/SaveGames/Worlds/<id>/world.json, "options").
	const FGamerWorldOptions FromSave = FGamerWorldOptions::FromUrlOptions(TEXT(
		"?BuildFlow=instant?Placement=free?Demolish=half?SeasonMinutes=60?Inventory=world?Tempo=normal?Death=grave?Mode=saga"
		"?Difficulty=hard?NeedsRate=1.3?Damage=1.4?Yield=0.8?Hordes=on?Immortal=off?FreeBuild=off?Sagenwelt=on?FriendlyFire=on?MaxPlayers=2"));
	TestTrue(TEXT("old save: values around Sagenwelt kept"), FromSave.BuildFlow == EGamerBuildFlow::Instant
		&& FromSave.Difficulty == EGamerDifficulty::Hard && FromSave.bHordes && !FromSave.bFreeBuild && FromSave.bFriendlyFire && FromSave.MaxPlayers == 2);
	TestFalse(TEXT("new saves do not write Sagenwelt"), FromSave.ToUrlOptions().Contains(TEXT("Sagenwelt")));

	// The menu settings of older builds (GameUserSettings.ini, UGamerMenuSettings::WorldOptions) still name bSagaWorld:
	// the unknown member is skipped, the members after it are read.
	FGamerWorldOptions FromIni;
	const TCHAR* const OldIni = TEXT("(BuildFlow=Instant,Placement=FreeSnap,Demolish=HalfRefund,SeasonMinutes=60.000000,bCarryInventory=False,")
		TEXT("GespuerTempo=Normal,DeathMode=Grave,PlayMode=Saga,Difficulty=Hard,NeedsRate=1.300000,DamageTaken=1.400000,YieldFactor=0.800000,")
		TEXT("bHordes=True,bVillagersImmortal=False,bFreeBuild=False,bSagaWorld=True,bFriendlyFire=True,MaxPlayers=2)");
	const TCHAR* const End = FGamerWorldOptions::StaticStruct()->ImportText(OldIni, &FromIni, nullptr, PPF_None, nullptr, FString(TEXT("FGamerWorldOptions")));
	TestNotNull(TEXT("old menu settings parse"), End);
	TestTrue(TEXT("old menu settings: values around bSagaWorld kept"), FromIni.BuildFlow == EGamerBuildFlow::Instant
		&& FromIni.Difficulty == EGamerDifficulty::Hard && FromIni.bHordes && FromIni.bFriendlyFire && FromIni.MaxPlayers == 2);
	return true;
}

#endif
