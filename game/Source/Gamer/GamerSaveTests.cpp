#include "GamerSaveSubsystem.h"

#include "Dom/JsonObject.h"
#include "Engine/Engine.h"
#include "Engine/GameInstance.h"
#include "Engine/World.h"
#include "HAL/FileManager.h"
#include "Misc/AutomationTest.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"

#if WITH_DEV_AUTOMATION_TESTS

// Run with: tools/test_data.ps1 -Filter Gamer   (or console: Automation RunTests Gamer.Save)

namespace
{
	TSharedPtr<FJsonObject> ParseJson(const FString& Text)
	{
		TSharedPtr<FJsonObject> Object;
		const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Text);
		FJsonSerializer::Deserialize(Reader, Object);
		return Object;
	}

	/** A world block with every kind of saved object, as the game would write it. */
	const TCHAR* TestWorld = TEXT(R"({
		"settlement": { "transform": { "loc": [0, 0, 100], "rot": [0, 0, 0] },
			"storage": [ { "item": "item_log", "count": 12 }, { "item": "item_berries", "count": 5, "fresh": 0.5 } ],
			"arrival": 0.25, "name_counter": 3 },
		"nodes": [
			{ "transform": { "loc": [500, 0, 0], "rot": [0, 45, 0] }, "id": "res_tree_pine", "depleted": true, "grow_stage": 1,
			  "regrow_s": 600, "grow_next_s": 120, "grow_step_s": 150 },
			{ "transform": { "loc": [800, 200, 0], "rot": [0, 10, 0] }, "id": "res_rock_small", "depleted": false, "grow_stage": 0, "work": 0.5 },
			{ "transform": { "loc": [900, -300, 0], "rot": [0, 0, 0] }, "id": "res_tree_apple", "depleted": false, "grow_stage": 0,
			  "fruit_stage": 4, "fruit_left": 7, "fruit_rolled": true, "fruit_fall_start": 0, "fruit_fallen": 0 }
		],
		"buildings": [
			{ "sid": 0, "transform": { "loc": [0, 600, 0], "rot": [0, 90, 0] }, "id": "building_bed", "progress": 1,
			  "spent": [ { "item": "item_fiber", "count": 6 } ], "level": 1 },
			{ "sid": 1, "transform": { "loc": [300, 600, 0], "rot": [0, 0, 0] }, "id": "building_wall_wood", "progress": 0.375,
			  "spent": [ { "item": "item_plank", "count": 4 } ], "level": 1, "strikes": 3 },
			{ "sid": 2, "transform": { "loc": [-800, 0, 0], "rot": [0, 0, 0], "scale": [4, 3, 1] }, "id": "building_field", "progress": 1,
			  "spent": [ { "item": "item_stick", "count": 4 }, { "item": "item_stone", "count": 2 } ], "level": 1,
			  "seed": "item_seed_carrot", "growth": 0.4, "water": 0.75, "fertilized": true, "stored": 0, "amount": 0 },
			{ "sid": 3, "transform": { "loc": [-800, 600, 0], "rot": [0, 0, 0] }, "id": "building_woodcutter", "progress": 1,
			  "spent": [], "level": 2 }
		],
		"ground_items": [
			{ "transform": { "loc": [100, 100, 0], "rot": [0, 0, 0] }, "item": "item_apple_bruised", "count": 1, "fresh": 0.6 }
		],
		"villagers": [
			{ "transform": { "loc": [0, 300, 100], "rot": [0, 0, 0] }, "name": "Astrid", "job": "job_woodcutter", "hunger": 0.7, "sleep": 0.4,
			  "mood": 0.6, "shelter": true, "tool": "tool_axe_t1", "carry": [ { "item": "item_log", "count": 2 } ], "hunger_empty_days": 0.25, "bed": 0 }
		]
	})");
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FGamerSaveRecordTest, "Gamer.Save.RecordRoundTrip",
	EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::EngineFilter)

bool FGamerSaveRecordTest::RunTest(const FString& Parameters)
{
	FGamerSaveRecord Record;
	Record.SetNumber(TEXT("number"), 1.25);
	Record.SetInt(TEXT("int"), 42);
	Record.SetBool(TEXT("bool"), true);
	Record.SetString(TEXT("text"), TEXT("Grüße, Åsa"));
	Record.SetName(TEXT("name"), TEXT("item_log"));
	Record.SetVector(TEXT("vector"), FVector(1.5, -2.0, 300.0));
	Record.SetTransform(TEXT("transform"), FTransform(FRotator(0, 90, 0), FVector(10, 20, 30), FVector(4, 3, 1)));
	Record.SetItems(TEXT("items"), { { TEXT("item_log"), 3, 1.f }, { TEXT("item_apple"), 2, 0.5f }, { NAME_None, 5, 1.f }, { TEXT("item_stone"), 0, 1.f } });
	FGamerSaveRecord Child;
	Child.SetInt(TEXT("x"), 7);
	Record.SetRecords(TEXT("children"), { Child, Child });

	// Through text and back, like a file.
	const FGamerSaveRecord Read(ParseJson(GamerSave::ToJsonText(Record.Json.ToSharedRef())));
	TestEqual(TEXT("number"), Read.GetNumber(TEXT("number")), 1.25);
	TestEqual(TEXT("int"), Read.GetInt(TEXT("int")), 42);
	TestTrue(TEXT("bool"), Read.GetBool(TEXT("bool")));
	TestEqual(TEXT("text with umlauts"), Read.GetString(TEXT("text")), FString(TEXT("Grüße, Åsa")));
	TestEqual(TEXT("name"), Read.GetName(TEXT("name")), FName(TEXT("item_log")));
	TestTrue(TEXT("vector"), Read.GetVector(TEXT("vector")).Equals(FVector(1.5, -2.0, 300.0)));
	const FTransform Transform = Read.GetTransform(TEXT("transform"));
	TestTrue(TEXT("transform location"), Transform.GetLocation().Equals(FVector(10, 20, 30)));
	TestTrue(TEXT("transform scale"), Transform.GetScale3D().Equals(FVector(4, 3, 1)));
	TestEqual(TEXT("transform yaw"), static_cast<int32>(FMath::RoundToInt(Transform.Rotator().Yaw)), 90);
	const TArray<FGamerItemAmount> Items = Read.GetItems(TEXT("items"));
	TestEqual(TEXT("empty item entries are not written"), Items.Num(), 2);
	if (Items.Num() == 2)
	{
		TestEqual(TEXT("item 2 freshness"), Items[1].Freshness, 0.5f);
		TestEqual(TEXT("item 1 freshness defaults to 1"), Items[0].Freshness, 1.f);
	}
	TestEqual(TEXT("children"), Read.GetRecords(TEXT("children")).Num(), 2);

	// Missing and wrongly typed fields give the default (old saves in new code).
	TestEqual(TEXT("missing number"), Read.GetNumber(TEXT("nope"), 5.0), 5.0);
	TestEqual(TEXT("string read as number"), Read.GetNumber(TEXT("text"), 3.0), 3.0);
	TestEqual(TEXT("number read as bool"), Read.GetBool(TEXT("number"), false), false);
	TestEqual(TEXT("missing record is empty"), Read.GetRecord(TEXT("nope")).Json->Values.Num(), 0);
	TestTrue(TEXT("missing transform default"), Read.GetTransform(TEXT("nope")).Equals(FTransform::Identity));
	const FGamerSaveRecord Broken(ParseJson(TEXT(R"({ "vector": [1, "x", 3], "items": [ 5, { "count": 2 }, { "item": "item_log", "count": "many" } ] })")));
	TestTrue(TEXT("broken vector -> default"), Broken.GetVector(TEXT("vector"), FVector(9)).Equals(FVector(9)));
	TestEqual(TEXT("broken item entries skipped"), Broken.GetItems(TEXT("items")).Num(), 0);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FGamerSaveFileTest, "Gamer.Save.FilesBackupAndList",
	EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::EngineFilter)

bool FGamerSaveFileTest::RunTest(const FString& Parameters)
{
	const FString Id = TEXT("automation_save_test");
	IFileManager::Get().DeleteDirectory(*GamerSave::WorldDirectory(Id), false, true);

	TestTrue(TEXT("valid id"), GamerSave::IsValidWorldId(Id));
	TestFalse(TEXT("path in id rejected"), GamerSave::IsValidWorldId(TEXT("../evil")));
	TestFalse(TEXT("empty id rejected"), GamerSave::IsValidWorldId(TEXT("")));
	TestEqual(TEXT("id from name"), GamerSave::MakeWorldId(TEXT("Mein Dorf am Fjörd!")), FString(TEXT("mein_dorf_am_fjoerd")));
	TestEqual(TEXT("id from symbols only"), GamerSave::MakeWorldId(TEXT("???")), FString(TEXT("welt")));

	auto MakeSave = [&Id](const FString& Name, double PlaySeconds)
	{
		FGamerSaveRecord Root;
		Root.SetInt(TEXT("schema"), GamerSave::SchemaVersion);
		FGamerSaveRecord Meta;
		Meta.SetString(TEXT("world_id"), Id);
		Meta.SetString(TEXT("name"), Name);
		Meta.SetString(TEXT("saved"), FDateTime::UtcNow().ToIso8601());
		Meta.SetNumber(TEXT("play_seconds"), PlaySeconds);
		Root.SetRecord(TEXT("meta"), Meta);
		Root.SetRecord(TEXT("world"), FGamerSaveRecord(ParseJson(TestWorld)));
		return Root;
	};

	FString Error;
	TestTrue(TEXT("first write"), GamerSave::WriteWorldFile(Id, MakeSave(TEXT("Erste"), 60).Json.ToSharedRef(), &Error));
	TestTrue(TEXT("second write"), GamerSave::WriteWorldFile(Id, MakeSave(TEXT("Zweite"), 3725).Json.ToSharedRef(), &Error));
	TestTrue(TEXT("backup kept"), IFileManager::Get().FileExists(*FPaths::Combine(GamerSave::WorldDirectory(Id), TEXT("world.bak.json"))));
	TestFalse(TEXT("no temp file left"), IFileManager::Get().FileExists(*FPaths::Combine(GamerSave::WorldDirectory(Id), TEXT("world.tmp"))));
	TestEqual(TEXT("unique id when the folder exists"), GamerSave::MakeWorldId(Id), Id + TEXT("_2"));

	TSharedPtr<FJsonObject> Root = GamerSave::ReadWorldFile(Id, &Error);
	TestTrue(TEXT("read back"), Root.IsValid());
	if (Root.IsValid())
	{
		TestEqual(TEXT("newest name"), FGamerSaveRecord(Root).GetRecord(TEXT("meta")).GetString(TEXT("name")), FString(TEXT("Zweite")));
		TestEqual(TEXT("world unchanged on disk"), GamerSave::CompareWorldBlocks(FGamerSaveRecord(Root).GetRecord(TEXT("world")), FGamerSaveRecord(ParseJson(TestWorld)), 0.0).Num(), 0);
	}

	bool bListed = false;
	for (const FGamerWorldSaveInfo& Info : GamerSave::ListWorlds())
	{
		if (Info.WorldId == Id)
		{
			bListed = true;
			TestEqual(TEXT("listed name"), Info.Name, FString(TEXT("Zweite")));
			TestEqual(TEXT("play time text"), Info.DescribePlayTime(), FString(TEXT("1 h 02 min")));
		}
	}
	TestTrue(TEXT("world in the list"), bListed);

	// A damaged world.json (e.g. disk full while copying by hand): the backup is used.
	FFileHelper::SaveStringToFile(TEXT("{ \"schema\": 1, \"world\": "), *GamerSave::WorldFile(Id));
	AddExpectedMessage(TEXT("broken JSON"), ELogVerbosity::Warning, EAutomationExpectedMessageFlags::Contains, 0);
	Root = GamerSave::ReadWorldFile(Id, &Error);
	TestTrue(TEXT("backup used for a broken file"), Root.IsValid() && FGamerSaveRecord(Root).GetRecord(TEXT("meta")).GetString(TEXT("name")) == TEXT("Erste"));

	IFileManager::Get().DeleteDirectory(*GamerSave::WorldDirectory(Id), false, true);
	TestFalse(TEXT("missing world -> null"), GamerSave::ReadWorldFile(Id, &Error).IsValid());
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FGamerSaveMigrationTest, "Gamer.Save.SchemaAndCompare",
	EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::EngineFilter)

bool FGamerSaveMigrationTest::RunTest(const FString& Parameters)
{
	TArray<FString> Notes;
	TestFalse(TEXT("no schema = not a save"), GamerSave::MigrateWorld(ParseJson(TEXT(R"({ "world": {} })")).ToSharedRef(), Notes));
	Notes.Reset();
	TestTrue(TEXT("current schema loads"), GamerSave::MigrateWorld(ParseJson(TEXT(R"({ "schema": 1, "world": {} })")).ToSharedRef(), Notes));
	TestEqual(TEXT("current schema: no notes"), Notes.Num(), 0);
	Notes.Reset();
	TestTrue(TEXT("newer schema loads best-effort"), GamerSave::MigrateWorld(ParseJson(TEXT(R"({ "schema": 99, "world": {} })")).ToSharedRef(), Notes));
	TestEqual(TEXT("newer schema: one note"), Notes.Num(), 1);

	// Comparison: order of entries does not matter, remaining times may move, real changes are found.
	const FGamerSaveRecord A(ParseJson(TEXT(R"({ "nodes": [
		{ "id": "res_a", "transform": { "loc": [0, 0, 0] }, "regrow_s": 100 },
		{ "id": "res_b", "transform": { "loc": [10, 0, 0] }, "depleted": true } ] })")));
	const FGamerSaveRecord B(ParseJson(TEXT(R"({ "nodes": [
		{ "id": "res_b", "transform": { "loc": [10, 0, 0] }, "depleted": true },
		{ "id": "res_a", "transform": { "loc": [0, 0, 0] }, "regrow_s": 99.5 } ] })")));
	const FGamerSaveRecord C(ParseJson(TEXT(R"({ "nodes": [
		{ "id": "res_b", "transform": { "loc": [10, 0, 0] }, "depleted": false },
		{ "id": "res_a", "transform": { "loc": [0, 0, 0] }, "regrow_s": 100 } ] })")));
	TestEqual(TEXT("same world, other order, timer moved"), GamerSave::CompareWorldBlocks(A, B, 1.0).Num(), 0);
	TestEqual(TEXT("changed flag found"), GamerSave::CompareWorldBlocks(A, C, 1.0).Num(), 1);
	TestEqual(TEXT("timer beyond tolerance found"), GamerSave::CompareWorldBlocks(A, B, 0.1).Num(), 1);
	return true;
}

/**
 * The real game classes: restore a world block into a game world, collect it again, compare.
 * Uses a standalone game instance (own world and game data), no map, no network. Actors do not run BeginPlay
 * there, which matches loading closely enough: ReadSaveState sets everything (incl. timers) itself.
 */
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FGamerSaveWorldRoundTripTest, "Gamer.Save.WorldRoundTrip",
	EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::EngineFilter)

bool FGamerSaveWorldRoundTripTest::RunTest(const FString& Parameters)
{
	UGameInstance* GameInstance = NewObject<UGameInstance>(GEngine);
	GameInstance->AddToRoot();
	GameInstance->InitializeStandalone(TEXT("GamerSaveTestWorld"));
	UWorld* World = GameInstance->GetWorld();
	UGamerSaveSubsystem* Saves = World ? World->GetSubsystem<UGamerSaveSubsystem>() : nullptr;
	TestNotNull(TEXT("game world"), World);
	TestNotNull(TEXT("save subsystem in the game world"), Saves);
	if (Saves)
	{
		const FGamerSaveRecord Original(ParseJson(TestWorld));
		int32 Skipped = 0;
		Saves->RestoreWorld(Original, Skipped);
		TestEqual(TEXT("nothing skipped"), Skipped, 0);
		const FGamerSaveRecord First = Saves->CollectWorld();
		for (const FString& Difference : GamerSave::CompareWorldBlocks(Original, First, 1.0))
		{
			AddError(TEXT("restore -> collect: ") + Difference);
		}

		// Through a file and once more (save, load, save gives the same file).
		Saves->RestoreWorld(FGamerSaveRecord(ParseJson(GamerSave::ToJsonText(First.Json.ToSharedRef()))), Skipped);
		const FGamerSaveRecord Second = Saves->CollectWorld();
		for (const FString& Difference : GamerSave::CompareWorldBlocks(First, Second, 1.0))
		{
			AddError(TEXT("second round: ") + Difference);
		}
		TestEqual(TEXT("villager count"), Second.GetRecords(TEXT("villagers")).Num(), 1);
		TestEqual(TEXT("villager keeps the bed"), Second.GetRecords(TEXT("villagers")).Num() == 1 ? Second.GetRecords(TEXT("villagers"))[0].GetInt(TEXT("bed")) : -2,
			Second.GetRecords(TEXT("buildings")).IndexOfByPredicate([](const FGamerSaveRecord& R) { return R.GetString(TEXT("id")) == TEXT("building_bed"); }));

		// Ids that the data no longer knows are dropped, everything else still loads.
		FGamerSaveRecord Changed(ParseJson(TestWorld));
		TArray<FGamerSaveRecord> Nodes = Changed.GetRecords(TEXT("nodes"));
		Nodes[0].SetName(TEXT("id"), TEXT("res_removed_in_new_version"));
		Changed.SetRecords(TEXT("nodes"), Nodes);
		FGamerSaveRecord Settlement = Changed.GetRecord(TEXT("settlement"));
		Settlement.SetItems(TEXT("storage"), { { TEXT("item_log"), 12, 1.f }, { TEXT("item_removed"), 3, 1.f } });
		Changed.SetRecord(TEXT("settlement"), Settlement);
		AddExpectedMessage(TEXT("no longer in the data"), ELogVerbosity::Warning, EAutomationExpectedMessageFlags::Contains, 0);
		Saves->RestoreWorld(Changed, Skipped);
		TestEqual(TEXT("unknown node skipped"), Skipped, 1);
		const FGamerSaveRecord Third = Saves->CollectWorld();
		TestEqual(TEXT("other nodes loaded"), Third.GetRecords(TEXT("nodes")).Num(), 2);
		TestEqual(TEXT("unknown item dropped from storage"), Third.GetRecord(TEXT("settlement")).GetItems(TEXT("storage")).Num(), 1);
	}
	if (World)
	{
		GEngine->DestroyWorldContext(World);
		World->DestroyWorld(false);
	}
	GameInstance->RemoveFromRoot();
	return true;
}

#endif
