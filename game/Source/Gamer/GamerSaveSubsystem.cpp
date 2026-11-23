#include "GamerSaveSubsystem.h"

#include "Gamer.h"
#include "GamerBuilding.h"
#include "GamerDataSubsystem.h"
#include "GamerGameState.h"
#include "GamerGrave.h"
#include "GamerGroundItem.h"
#include "GamerInventoryComponent.h"
#include "GamerResourceNode.h"
#include "GamerSettlement.h"
#include "GamerVillager.h"
#include "Dom/JsonObject.h"
#include "Dom/JsonValue.h"
#include "EngineUtils.h"
#include "Engine/World.h"
#include "GameFramework/GameModeBase.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/PlayerState.h"
#include "HAL/FileManager.h"
#include "Kismet/GameplayStatics.h"
#include "Misc/CommandLine.h"
#include "Misc/CoreDelegates.h"
#include "Misc/FileHelper.h"
#include "Misc/Guid.h"
#include "Misc/Parse.h"
#include "Misc/Paths.h"
#include "Net/UnrealNetwork.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"
#include "Serialization/JsonWriter.h"
#include "TimerManager.h"

// ---------------------------------------------------------------------------------------------------------------
// Files and the world list
// ---------------------------------------------------------------------------------------------------------------

FString FGamerWorldSaveInfo::DescribePlayTime() const
{
	const int32 Minutes = FMath::FloorToInt(PlaySeconds / 60.0);
	return Minutes >= 60 ? FString::Printf(TEXT("%d h %02d min"), Minutes / 60, Minutes % 60) : FString::Printf(TEXT("%d min"), Minutes);
}

FString GamerSave::WorldsDirectory()
{
	return FPaths::Combine(FPaths::ProjectSavedDir(), TEXT("SaveGames"), TEXT("Worlds"));
}

FString GamerSave::WorldDirectory(const FString& WorldId)
{
	return FPaths::Combine(WorldsDirectory(), WorldId);
}

FString GamerSave::WorldFile(const FString& WorldId)
{
	return FPaths::Combine(WorldDirectory(WorldId), TEXT("world.json"));
}

bool GamerSave::IsValidWorldId(const FString& WorldId)
{
	if (WorldId.IsEmpty() || WorldId.Len() > 64)
	{
		return false;
	}
	for (const TCHAR Char : WorldId)
	{
		if (!FChar::IsAlnum(Char) || Char > 127)
		{
			if (Char != TEXT('_') && Char != TEXT('-'))
			{
				return false;
			}
		}
	}
	return true;
}

FString GamerSave::MakeWorldId(const FString& Name)
{
	FString Id;
	for (const TCHAR Char : Name.ToLower())
	{
		switch (Char)
		{
		case TEXT('ä'): Id += TEXT("ae"); continue;
		case TEXT('ö'): Id += TEXT("oe"); continue;
		case TEXT('ü'): Id += TEXT("ue"); continue;
		case TEXT('ß'): Id += TEXT("ss"); continue;
		default: break;
		}
		if (Char < 128 && FChar::IsAlnum(Char))
		{
			Id.AppendChar(Char);
		}
		else if (!Id.IsEmpty() && !Id.EndsWith(TEXT("_")))
		{
			Id.AppendChar(TEXT('_'));
		}
	}
	while (Id.EndsWith(TEXT("_")))
	{
		Id.LeftChopInline(1);
	}
	Id = Id.Left(32);
	if (Id.IsEmpty())
	{
		Id = TEXT("welt");
	}
	FString Unique = Id;
	for (int32 Number = 2; IFileManager::Get().DirectoryExists(*WorldDirectory(Unique)); ++Number)
	{
		Unique = FString::Printf(TEXT("%s_%d"), *Id, Number);
	}
	return Unique;
}

static FDateTime ParseTime(const FString& Text)
{
	FDateTime Time;
	return FDateTime::ParseIso8601(*Text, Time) ? Time : FDateTime(0);
}

static bool ReadJsonFile(const FString& Path, TSharedPtr<FJsonObject>& Out, FString* OutError)
{
	FString Text;
	if (!FFileHelper::LoadFileToString(Text, *Path))
	{
		if (OutError)
		{
			*OutError = FString::Printf(TEXT("cannot read %s"), *Path);
		}
		return false;
	}
	const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Text);
	if (!FJsonSerializer::Deserialize(Reader, Out) || !Out.IsValid())
	{
		if (OutError)
		{
			*OutError = FString::Printf(TEXT("broken JSON in %s: %s"), *Path, *Reader->GetErrorMessage());
		}
		Out.Reset();
		return false;
	}
	return true;
}

TArray<FGamerWorldSaveInfo> GamerSave::ListWorlds()
{
	TArray<FGamerWorldSaveInfo> Worlds;
	TArray<FString> Folders;
	IFileManager::Get().FindFiles(Folders, *FPaths::Combine(WorldsDirectory(), TEXT("*")), false, true);
	for (const FString& Folder : Folders)
	{
		if (!IsValidWorldId(Folder))
		{
			continue;
		}
		TSharedPtr<FJsonObject> Meta;
		if (!ReadJsonFile(FPaths::Combine(WorldDirectory(Folder), TEXT("meta.json")), Meta, nullptr))
		{
			// No meta file (e.g. deleted by hand): take the block from the world file itself.
			TSharedPtr<FJsonObject> Root;
			if (!ReadJsonFile(WorldFile(Folder), Root, nullptr))
			{
				continue;
			}
			Meta = FGamerSaveRecord(Root).GetRecord(TEXT("meta")).Json;
		}
		const FGamerSaveRecord Record(Meta);
		FGamerWorldSaveInfo Info;
		Info.WorldId = Folder;
		Info.Name = Record.GetString(TEXT("name"), Folder);
		Info.Created = ParseTime(Record.GetString(TEXT("created")));
		Info.Saved = ParseTime(Record.GetString(TEXT("saved")));
		Info.PlaySeconds = Record.GetNumber(TEXT("play_seconds"));
		Info.Schema = Record.GetInt(TEXT("schema"));
		Info.OptionsText = Record.GetString(TEXT("options_text"));
		Worlds.Add(Info);
	}
	Worlds.Sort([](const FGamerWorldSaveInfo& A, const FGamerWorldSaveInfo& B) { return A.Saved > B.Saved; });
	return Worlds;
}

TSharedPtr<FJsonObject> GamerSave::ReadWorldFile(const FString& WorldId, FString* OutError)
{
	TSharedPtr<FJsonObject> Root;
	FString Error;
	if (ReadJsonFile(WorldFile(WorldId), Root, &Error))
	{
		return Root;
	}
	// A crash while writing can only damage the temporary file, but a damaged world.json is also covered by the backup.
	const FString Backup = FPaths::Combine(WorldDirectory(WorldId), TEXT("world.bak.json"));
	if (IFileManager::Get().FileExists(*Backup) && ReadJsonFile(Backup, Root, nullptr))
	{
		UE_LOG(LogGamer, Warning, TEXT("Save: %s - using the backup copy"), *Error);
		return Root;
	}
	if (OutError)
	{
		*OutError = Error;
	}
	return nullptr;
}

FString GamerSave::ToJsonText(const TSharedRef<FJsonObject>& Object)
{
	FString Text;
	const TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&Text);
	FJsonSerializer::Serialize(Object, Writer);
	return Text;
}

bool GamerSave::WriteWorldFile(const FString& WorldId, const TSharedRef<FJsonObject>& Root, FString* OutError)
{
	if (!IsValidWorldId(WorldId))
	{
		if (OutError)
		{
			*OutError = FString::Printf(TEXT("invalid world id \"%s\""), *WorldId);
		}
		return false;
	}
	const FString Dir = WorldDirectory(WorldId);
	IFileManager::Get().MakeDirectory(*Dir, true);
	const FString Final = WorldFile(WorldId);
	const FString Temp = FPaths::Combine(Dir, TEXT("world.tmp"));
	const FString Backup = FPaths::Combine(Dir, TEXT("world.bak.json"));
	if (!FFileHelper::SaveStringToFile(ToJsonText(Root), *Temp, FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM))
	{
		if (OutError)
		{
			*OutError = FString::Printf(TEXT("cannot write %s"), *Temp);
		}
		return false;
	}
	if (IFileManager::Get().FileExists(*Final))
	{
		IFileManager::Get().Move(*Backup, *Final, true, true);
	}
	if (!IFileManager::Get().Move(*Final, *Temp, true, true))
	{
		if (OutError)
		{
			*OutError = FString::Printf(TEXT("cannot move %s to %s"), *Temp, *Final);
		}
		return false;
	}
	const FGamerSaveRecord Meta = FGamerSaveRecord(Root).GetRecord(TEXT("meta"));
	FFileHelper::SaveStringToFile(ToJsonText(Meta.Json.ToSharedRef()), *FPaths::Combine(Dir, TEXT("meta.json")),
		FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM);
	return true;
}

bool GamerSave::MigrateWorld(const TSharedRef<FJsonObject>& Root, TArray<FString>& OutNotes)
{
	FGamerSaveRecord Record(Root);
	const int32 Schema = Record.GetInt(TEXT("schema"), 0);
	if (Schema <= 0 || !Root->HasField(TEXT("world")))
	{
		OutNotes.Add(TEXT("no schema/world block: not a world save"));
		return false;
	}
	if (Schema > SchemaVersion)
	{
		OutNotes.Add(FString::Printf(TEXT("save is from a newer game version (schema %d, this game knows %d): loading what is known"), Schema, SchemaVersion));
		return true;
	}
	// Future migrations go here, one step per version:
	// if (Schema < 2) { ... rename/convert fields ...; Schema = 2; OutNotes.Add(TEXT("1 -> 2: ...")); }
	Record.SetInt(TEXT("schema"), FMath::Max(Schema, SchemaVersion));
	return true;
}

bool GamerSave::IsKnownItem(const UObject* WorldContext, FName Item)
{
	const UGamerDataSubsystem* Data = UGamerDataSubsystem::Get(WorldContext);
	return !Item.IsNone() && Data && Data->FindItem(Item);
}

void GamerSave::FilterKnownItems(const UObject* WorldContext, TArray<FGamerItemAmount>& Items, const TCHAR* Where)
{
	for (int32 Index = Items.Num() - 1; Index >= 0; --Index)
	{
		if (!IsKnownItem(WorldContext, Items[Index].Item))
		{
			UE_LOG(LogGamer, Warning, TEXT("Save: unknown item \"%s\" x%d in %s dropped (no longer in the data)"), *Items[Index].Item.ToString(), Items[Index].Count, Where);
			Items.RemoveAt(Index);
		}
	}
}

static FString SafeFileId(const FString& PlayerId)
{
	FString Id;
	for (const TCHAR Char : PlayerId.Left(64))
	{
		if (Char < 128 && (FChar::IsAlnum(Char) || Char == TEXT('_') || Char == TEXT('-')))
		{
			Id.AppendChar(Char);
		}
	}
	return Id.IsEmpty() ? FString(TEXT("player")) : Id;
}

static TSharedPtr<FJsonObject> ParseObject(const FString& Text)
{
	TSharedPtr<FJsonObject> Object;
	if (!Text.IsEmpty())
	{
		FJsonSerializer::Deserialize(TJsonReaderFactory<>::Create(Text), Object);
	}
	return Object;
}

FString GamerSave::CharacterFile(const FString& PlayerId)
{
	return FPaths::Combine(FPaths::ProjectSavedDir(), TEXT("SaveGames"), TEXT("Characters"), SafeFileId(PlayerId) + TEXT(".json"));
}

FString GamerSave::ReadCarriedInventory(const FString& PlayerId)
{
	TSharedPtr<FJsonObject> Root;
	if (!ReadJsonFile(CharacterFile(PlayerId), Root, nullptr))
	{
		return FString();
	}
	const FGamerSaveRecord Record(Root);
	return Record.Has(TEXT("inventory")) ? ToJsonText(Record.GetRecord(TEXT("inventory")).Json.ToSharedRef()) : FString();
}

bool GamerSave::WriteCarriedInventory(const FString& PlayerId, const FGamerSaveRecord& Inventory)
{
	const FString Path = CharacterFile(PlayerId);
	TSharedPtr<FJsonObject> Root;
	ReadJsonFile(Path, Root, nullptr); // keep whatever else the file holds
	FGamerSaveRecord Record(Root);
	Record.SetInt(TEXT("schema"), SchemaVersion);
	Record.SetString(TEXT("saved"), FDateTime::UtcNow().ToIso8601());
	Record.SetRecord(TEXT("inventory"), Inventory);
	const FString Temp = Path + TEXT(".tmp");
	return FFileHelper::SaveStringToFile(ToJsonText(Record.Json.ToSharedRef()), *Temp, FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM)
		&& IFileManager::Get().Move(*Path, *Temp, true, true);
}

// --- Comparing two world blocks (round-trip checks) ---

static void CompareJson(const FString& Path, const TSharedPtr<FJsonValue>& A, const TSharedPtr<FJsonValue>& B, double Tolerance, TArray<FString>& Out);

/** Sort key of an entry in a world list: id + position (order of actors in the world is not stable). */
static FString EntryKey(const TSharedPtr<FJsonValue>& Value)
{
	const TSharedPtr<FJsonObject>* Object = nullptr;
	if (!Value.IsValid() || !Value->TryGetObject(Object) || !Object)
	{
		return FString();
	}
	const FGamerSaveRecord Record(*Object);
	const FVector Location = Record.GetRecord(TEXT("transform")).GetVector(TEXT("loc"));
	FString Id = Record.GetString(TEXT("id"));
	if (Id.IsEmpty())
	{
		Id = Record.GetString(TEXT("item")) + Record.GetString(TEXT("name"));
	}
	return FString::Printf(TEXT("%s@%d,%d,%d"), *Id, FMath::RoundToInt(Location.X), FMath::RoundToInt(Location.Y), FMath::RoundToInt(Location.Z));
}

static void CompareJson(const FString& Path, const TSharedPtr<FJsonValue>& A, const TSharedPtr<FJsonValue>& B, double Tolerance, TArray<FString>& Out)
{
	if (Out.Num() > 40)
	{
		return;
	}
	if (!A.IsValid() || !B.IsValid())
	{
		if (A.IsValid() != B.IsValid())
		{
			Out.Add(FString::Printf(TEXT("%s: only in %s"), *Path, A.IsValid() ? TEXT("first") : TEXT("second")));
		}
		return;
	}
	if (A->Type != B->Type)
	{
		Out.Add(FString::Printf(TEXT("%s: type differs"), *Path));
		return;
	}
	switch (A->Type)
	{
	case EJson::Number:
	{
		const double X = A->AsNumber();
		const double Y = B->AsNumber();
		// Remaining times ("..._s") and the season phase move while the game runs; everything else must match.
		const double Allowed = Path.EndsWith(TEXT("_s")) ? Tolerance
			: Path.EndsWith(TEXT("season_phase")) ? 0.01 + Tolerance / 3600.0
			: 0.01 + FMath::Abs(X) * 1e-5;
		if (FMath::Abs(X - Y) > Allowed)
		{
			Out.Add(FString::Printf(TEXT("%s: %g vs %g"), *Path, X, Y));
		}
		return;
	}
	case EJson::String:
		if (A->AsString() != B->AsString())
		{
			Out.Add(FString::Printf(TEXT("%s: \"%s\" vs \"%s\""), *Path, *A->AsString(), *B->AsString()));
		}
		return;
	case EJson::Boolean:
		if (A->AsBool() != B->AsBool())
		{
			Out.Add(FString::Printf(TEXT("%s: %d vs %d"), *Path, A->AsBool(), B->AsBool()));
		}
		return;
	case EJson::Object:
	{
		const TSharedPtr<FJsonObject> OA = A->AsObject();
		const TSharedPtr<FJsonObject> OB = B->AsObject();
		TSet<FString> Keys;
		for (const auto& Pair : OA->Values) { Keys.Add(FString(Pair.Key)); }
		for (const auto& Pair : OB->Values) { Keys.Add(FString(Pair.Key)); }
		for (const FString& Key : Keys)
		{
			CompareJson(Path + TEXT(".") + Key, OA->TryGetField(Key), OB->TryGetField(Key), Tolerance, Out);
		}
		return;
	}
	case EJson::Array:
	{
		TArray<TSharedPtr<FJsonValue>> LA = A->AsArray();
		TArray<TSharedPtr<FJsonValue>> LB = B->AsArray();
		if (LA.Num() != LB.Num())
		{
			Out.Add(FString::Printf(TEXT("%s: %d vs %d entries"), *Path, LA.Num(), LB.Num()));
			return;
		}
		const bool bKeyed = LA.Num() > 0 && !EntryKey(LA[0]).IsEmpty();
		if (bKeyed)
		{
			auto ByKey = [](const TSharedPtr<FJsonValue>& X, const TSharedPtr<FJsonValue>& Y) { return EntryKey(X) < EntryKey(Y); };
			LA.StableSort(ByKey);
			LB.StableSort(ByKey);
		}
		for (int32 Index = 0; Index < LA.Num(); ++Index)
		{
			const FString Label = bKeyed ? EntryKey(LA[Index]) : FString::FromInt(Index);
			CompareJson(FString::Printf(TEXT("%s[%s]"), *Path, *Label), LA[Index], LB[Index], Tolerance, Out);
		}
		return;
	}
	default:
		return;
	}
}

TArray<FString> GamerSave::CompareWorldBlocks(const FGamerSaveRecord& A, const FGamerSaveRecord& B, double Tolerance)
{
	TArray<FString> Out;
	CompareJson(TEXT("world"), MakeShared<FJsonValueObject>(A.Json), MakeShared<FJsonValueObject>(B.Json), Tolerance, Out);
	return Out;
}

// ---------------------------------------------------------------------------------------------------------------
// Player identity and the per-player component
// ---------------------------------------------------------------------------------------------------------------

FString UGamerPlayerIdentity::GetLocalPlayerId()
{
	FString FromCommandLine;
	if (FParse::Value(FCommandLine::Get(), TEXT("GamerPlayerId="), FromCommandLine) && !FromCommandLine.IsEmpty())
	{
		return FromCommandLine;
	}
	UGamerPlayerIdentity* Identity = GetMutableDefault<UGamerPlayerIdentity>();
	if (Identity->PlayerId.IsEmpty())
	{
		Identity->PlayerId = FGuid::NewGuid().ToString(EGuidFormats::Digits).ToLower();
		Identity->SaveConfig();
		UE_LOG(LogGamer, Log, TEXT("Save: new local player id created"));
	}
	return Identity->PlayerId;
}

UGamerSaveClientComponent::UGamerSaveClientComponent()
{
	SetIsReplicatedByDefault(true);
}

void UGamerSaveClientComponent::BeginPlay()
{
	Super::BeginPlay();
	TrySendIdentity();
}

void UGamerSaveClientComponent::TrySendIdentity()
{
	const APlayerController* Controller = Cast<APlayerController>(GetOwner());
	if (bSent || !Controller)
	{
		return;
	}
	if (!Controller->IsLocalController())
	{
		// Server copy of a remote player's controller: the client sends. Clients see only their own controller.
		if (Controller->GetNetMode() == NM_Client && ++Attempts < 20)
		{
			GetWorld()->GetTimerManager().SetTimer(RetryTimer, this, &UGamerSaveClientComponent::TrySendIdentity, 0.25f, false);
		}
		return;
	}
	bSent = true;
	const FString Id = UGamerPlayerIdentity::GetLocalPlayerId();
	const FString Carried = GamerSave::ReadCarriedInventory(Id);
	UE_LOG(LogGamer, Log, TEXT("Save: sending player id %s (character file inventory: %s)"), *Id, Carried.IsEmpty() ? TEXT("none") : TEXT("yes"));
	ServerIdentify(Id, Carried);
}

void UGamerSaveClientComponent::ServerIdentify_Implementation(const FString& PlayerId, const FString& CarriedInventory)
{
	if (UGamerSaveSubsystem* Saves = UGamerSaveSubsystem::Get(this))
	{
		Saves->HandleIdentify(Cast<APlayerController>(GetOwner()), PlayerId, CarriedInventory.Left(64 * 1024));
	}
}

void UGamerSaveClientComponent::ClientKeepCarriedInventory_Implementation(const FString& AppliedInventory)
{
	ExpectedInventory = AppliedInventory;
	CarryStart = FPlatformTime::Seconds();
	bCarrySynced = false;
	LastWritten.Reset();
	UE_LOG(LogGamer, Log, TEXT("Save: this world's inventory travels with the character (%s)"), *GamerSave::CharacterFile(UGamerPlayerIdentity::GetLocalPlayerId()));
	GetWorld()->GetTimerManager().SetTimer(CarryTimer, this, &UGamerSaveClientComponent::WriteCarriedInventory, 1.f, true);
}

void UGamerSaveClientComponent::WriteCarriedInventory()
{
	const APlayerController* Controller = Cast<APlayerController>(GetOwner());
	const APawn* Pawn = Controller ? Controller->GetPawn() : nullptr;
	const UGamerInventoryComponent* Inventory = Pawn ? Pawn->FindComponentByClass<UGamerInventoryComponent>() : nullptr;
	if (!Inventory)
	{
		return;
	}
	FGamerSaveRecord Record;
	Inventory->WriteSaveState(Record);
	const FString Text = GamerSave::ToJsonText(Record.Json.ToSharedRef());
	if (!bCarrySynced)
	{
		// The new inventory replicates separately from this message: never write the pawn's old (empty) state over the file.
		const bool bSame = !ExpectedInventory.IsEmpty() && GamerSave::CompareWorldBlocks(Record, FGamerSaveRecord(ParseObject(ExpectedInventory)), 0.0).Num() == 0;
		if (!bSame && FPlatformTime::Seconds() - CarryStart < 10.0)
		{
			return;
		}
		bCarrySynced = true;
	}
	if (Text != LastWritten && GamerSave::WriteCarriedInventory(UGamerPlayerIdentity::GetLocalPlayerId(), Record))
	{
		LastWritten = Text;
		UE_LOG(LogGamer, Log, TEXT("Save: character file inventory updated: %s"), *Inventory->Describe());
	}
}

void UGamerSaveClientComponent::ServerRequestSave_Implementation()
{
	const APlayerController* Controller = Cast<APlayerController>(GetOwner());
	UGamerSaveSubsystem* Saves = UGamerSaveSubsystem::Get(this);
	if (!Controller || !Saves || !Saves->IsActive())
	{
		return;
	}
	// Listen server: only the host. Dedicated server: any player (there is no host), at most every 10 s.
	static double LastRequest = -1000.0;
	const bool bAllowed = Controller->IsLocalController() || (GetNetMode() == NM_DedicatedServer && FPlatformTime::Seconds() - LastRequest > 10.0);
	if (!bAllowed)
	{
		UE_LOG(LogGamer, Log, TEXT("Save: request from a guest ignored (only the host saves)"));
		return;
	}
	LastRequest = FPlatformTime::Seconds();
	Saves->SaveNow(TEXT("menu"));
}

// ---------------------------------------------------------------------------------------------------------------
// Subsystem
// ---------------------------------------------------------------------------------------------------------------

bool UGamerSaveSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	const UWorld* World = Cast<UWorld>(Outer);
	return World && World->IsGameWorld() && Super::ShouldCreateSubsystem(Outer);
}

UGamerSaveSubsystem* UGamerSaveSubsystem::Get(const UObject* WorldContext)
{
	const UWorld* World = WorldContext ? WorldContext->GetWorld() : nullptr;
	return World ? World->GetSubsystem<UGamerSaveSubsystem>() : nullptr;
}

void UGamerSaveSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	PostLoginHandle = FGameModeEvents::GameModePostLoginEvent.AddUObject(this, &UGamerSaveSubsystem::HandlePostLogin);
	LogoutHandle = FGameModeEvents::GameModeLogoutEvent.AddUObject(this, &UGamerSaveSubsystem::HandleLogout);
	TearDownHandle = FWorldDelegates::OnWorldBeginTearDown.AddUObject(this, &UGamerSaveSubsystem::HandleWorldTearDown);
	PreExitHandle = FCoreDelegates::OnPreExit.AddUObject(this, &UGamerSaveSubsystem::HandlePreExit);
}

void UGamerSaveSubsystem::Deinitialize()
{
	FGameModeEvents::GameModePostLoginEvent.Remove(PostLoginHandle);
	FGameModeEvents::GameModeLogoutEvent.Remove(LogoutHandle);
	FWorldDelegates::OnWorldBeginTearDown.Remove(TearDownHandle);
	FCoreDelegates::OnPreExit.Remove(PreExitHandle);
	Super::Deinitialize();
}

void UGamerSaveSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);
}

bool UGamerSaveSubsystem::LoadOrCreate()
{
	UWorld* World = GetWorld();
	if (!World || World->GetNetMode() == NM_Client)
	{
		return false;
	}
	const AGameModeBase* GameMode = World->GetAuthGameMode();
	const FString Options = GameMode ? GameMode->OptionsString : FString();
	WorldId = UGameplayStatics::ParseOption(Options, TEXT("SaveWorld"));
	FString FromCommandLine;
	if (WorldId.IsEmpty() && FParse::Value(FCommandLine::Get(), TEXT("GamerWorld="), FromCommandLine))
	{
		WorldId = FromCommandLine;
	}
	if (WorldId.IsEmpty())
	{
		UE_LOG(LogGamer, Log, TEXT("Save: off (no ?SaveWorld= / -GamerWorld=); this world is not saved"));
		return false;
	}
	if (!GamerSave::IsValidWorldId(WorldId))
	{
		UE_LOG(LogGamer, Error, TEXT("Save: invalid world id \"%s\" (letters, digits, _ and - only); this world is not saved"), *WorldId);
		WorldId.Reset();
		return false;
	}
	bActive = true;
	SessionStart = FPlatformTime::Seconds();
	WorldName = UGameplayStatics::ParseOption(Options, TEXT("WorldName")).Replace(TEXT("+"), TEXT(" "));
	FString NameFromCommandLine;
	if (WorldName.IsEmpty() && FParse::Value(FCommandLine::Get(), TEXT("GamerWorldName="), NameFromCommandLine))
	{
		WorldName = NameFromCommandLine;
	}

	FString Error;
	const bool bFileExists = IFileManager::Get().FileExists(*GamerSave::WorldFile(WorldId));
	TSharedPtr<FJsonObject> Root = GamerSave::ReadWorldFile(WorldId, &Error);
	TArray<FString> Notes;
	if (Root.IsValid() && !GamerSave::MigrateWorld(Root.ToSharedRef(), Notes))
	{
		Error = FString::Join(Notes, TEXT("; "));
		Root.Reset();
	}
	if (!Root.IsValid())
	{
		if (bFileExists)
		{
			// Never overwrite a save we could not read: keep it aside for repair and start fresh.
			const FString Keep = FPaths::Combine(GamerSave::WorldDirectory(WorldId), FString::Printf(TEXT("world.unreadable-%s.json"), *FDateTime::Now().ToString(TEXT("%Y%m%d-%H%M%S"))));
			IFileManager::Get().Copy(*Keep, *GamerSave::WorldFile(WorldId));
			UE_LOG(LogGamer, Error, TEXT("Save: world \"%s\" could not be loaded (%s); kept as %s, starting a new world"), *WorldId, *Error, *Keep);
		}
		Created = FDateTime::UtcNow();
		if (WorldName.IsEmpty())
		{
			WorldName = WorldId;
		}
		UE_LOG(LogGamer, Log, TEXT("Save: new world \"%s\" (id %s)"), *WorldName, *WorldId);
		// First save once the game mode has set up the start area (same frame would miss it).
		FTimerHandle FirstSave;
		World->GetTimerManager().SetTimer(FirstSave, FTimerDelegate::CreateWeakLambda(this, [this]() { SaveNow(TEXT("new world")); }), 1.f, false);
	}
	else
	{
		for (const FString& Note : Notes)
		{
			UE_LOG(LogGamer, Warning, TEXT("Save: %s"), *Note);
		}
		const FGamerSaveRecord Save(Root);
		const FGamerSaveRecord Meta = Save.GetRecord(TEXT("meta"));
		if (WorldName.IsEmpty())
		{
			WorldName = Meta.GetString(TEXT("name"), WorldId);
		}
		Created = ParseTime(Meta.GetString(TEXT("created")));
		PlaySecondsBefore = Meta.GetNumber(TEXT("play_seconds"));
		int32 Skipped = 0;
		const FGamerSaveRecord WorldBlock = Save.GetRecord(TEXT("world"));
		RestoreWorld(WorldBlock, Skipped);
		for (const FGamerSaveRecord& Player : Save.GetRecords(TEXT("players")))
		{
			const FString Id = Player.GetString(TEXT("id"));
			if (!Id.IsEmpty())
			{
				PlayerRecords.Add(Id, Player);
			}
		}
		bLoaded = true;
		UE_LOG(LogGamer, Log, TEXT("Save: loaded world \"%s\" (id %s, schema %d, played %.0f s, %d players known, %d entries skipped)"),
			*WorldName, *WorldId, Save.GetInt(TEXT("schema")), PlaySecondsBefore, PlayerRecords.Num(), Skipped);

		// Round-trip check for tests: collecting right after loading must give the same world again.
		if (FParse::Param(FCommandLine::Get(), TEXT("GamerSaveVerify")))
		{
			const TArray<FString> Differences = GamerSave::CompareWorldBlocks(WorldBlock, CollectWorld(), 1.0);
			for (const FString& Difference : Differences)
			{
				UE_LOG(LogGamer, Warning, TEXT("Save verify: %s"), *Difference);
			}
			UE_LOG(LogGamer, Log, TEXT("Save verify: %s (%d differences)"), Differences.Num() == 0 ? TEXT("PASS") : TEXT("FAIL"), Differences.Num());
		}
	}

	float AutosaveSeconds = AutosaveMinutes * 60.f;
	FParse::Value(FCommandLine::Get(), TEXT("GamerAutosaveSeconds="), AutosaveSeconds);
	if (AutosaveSeconds > 0.f)
	{
		World->GetTimerManager().SetTimer(AutosaveTimer, this, &UGamerSaveSubsystem::Autosave, FMath::Max(5.f, AutosaveSeconds), true);
	}
	World->GetTimerManager().SetTimer(RestoreTimer, this, &UGamerSaveSubsystem::TickRestore, 0.25f, true);
	// Test: end the server after N seconds through the normal exit path (checks the save on shutdown).
	if (FParse::Value(FCommandLine::Get(), TEXT("GamerSaveQuitAfter="), QuitAfterSeconds) && QuitAfterSeconds > 0.f)
	{
		FTimerHandle QuitTimer;
		World->GetTimerManager().SetTimer(QuitTimer, FTimerDelegate::CreateWeakLambda(this, []()
		{
			UE_LOG(LogGamer, Log, TEXT("Save: -GamerSaveQuitAfter reached, quitting"));
			FPlatformMisc::RequestExit(false, TEXT("GamerSaveQuitAfter"));
		}), QuitAfterSeconds, false);
	}
	// Players who joined before the save system was active (listen host: joins before StartPlay).
	for (FConstPlayerControllerIterator It = World->GetPlayerControllerIterator(); It; ++It)
	{
		if (APlayerController* Controller = It->Get(); Controller && !ControllerIds.Contains(Controller) && !AwaitingId.Contains(Controller))
		{
			HandlePostLogin(nullptr, Controller);
		}
	}
	return bLoaded;
}

double UGamerSaveSubsystem::GetPlaySeconds() const
{
	return PlaySecondsBefore + (bActive ? FPlatformTime::Seconds() - SessionStart : 0.0);
}

void UGamerSaveSubsystem::Autosave()
{
	SaveNow(TEXT("autosave"));
}

bool UGamerSaveSubsystem::SaveNow(const FString& Reason)
{
	if (!bActive || !GetWorld())
	{
		return false;
	}
	const double Start = FPlatformTime::Seconds();
	const TSharedRef<FJsonObject> Root = BuildSave();
	FString Error;
	if (!GamerSave::WriteWorldFile(WorldId, Root, &Error))
	{
		UE_LOG(LogGamer, Error, TEXT("Save: FAILED (%s): %s"), *Reason, *Error);
		if (AGamerSettlement* Settlement = AGamerSettlement::Get(this))
		{
			Settlement->Broadcast(TEXT("Speichern fehlgeschlagen!"));
		}
		return false;
	}
	const FGamerSaveRecord WorldBlock = FGamerSaveRecord(Root).GetRecord(TEXT("world"));
	UE_LOG(LogGamer, Log, TEXT("Save: world \"%s\" saved (%s) in %.0f ms: %d nodes, %d buildings, %d ground items, %d villagers, %d players -> %s"),
		*WorldName, *Reason, (FPlatformTime::Seconds() - Start) * 1000.0,
		WorldBlock.GetRecords(TEXT("nodes")).Num(), WorldBlock.GetRecords(TEXT("buildings")).Num(), WorldBlock.GetRecords(TEXT("ground_items")).Num(),
		WorldBlock.GetRecords(TEXT("villagers")).Num(), FGamerSaveRecord(Root).GetRecords(TEXT("players")).Num(),
		*FPaths::ConvertRelativePathToFull(GamerSave::WorldFile(WorldId)));
	if (Reason == TEXT("menu") || Reason == TEXT("autosave"))
	{
		if (AGamerSettlement* Settlement = AGamerSettlement::Get(this))
		{
			Settlement->Broadcast(Reason == TEXT("menu") ? TEXT("Welt gespeichert.") : TEXT("Automatisch gespeichert."));
		}
	}
	return true;
}

TSharedRef<FJsonObject> UGamerSaveSubsystem::BuildSave() const
{
	FGamerSaveRecord Root;
	Root.SetInt(TEXT("schema"), GamerSave::SchemaVersion);

	FGamerSaveRecord Meta;
	Meta.SetString(TEXT("world_id"), WorldId);
	Meta.SetString(TEXT("name"), WorldName);
	Meta.SetString(TEXT("created"), (Created.GetTicks() > 0 ? Created : FDateTime::UtcNow()).ToIso8601());
	Meta.SetString(TEXT("saved"), FDateTime::UtcNow().ToIso8601());
	Meta.SetNumber(TEXT("play_seconds"), FMath::RoundToDouble(GetPlaySeconds()));
	Meta.SetInt(TEXT("schema"), GamerSave::SchemaVersion);
	Meta.SetString(TEXT("options_text"), AGamerGameState::Get(this).Describe());
	if (const UGamerDataSubsystem* Data = UGamerDataSubsystem::Get(this))
	{
		Meta.SetString(TEXT("data_checksum"), FString::Printf(TEXT("%08X"), Data->GetChecksum()));
	}
	Root.SetRecord(TEXT("meta"), Meta);
	Root.SetRecord(TEXT("world"), CollectWorld());

	// Players: everyone known, with the connected ones refreshed.
	TMap<FString, FGamerSaveRecord> Players = PlayerRecords;
	for (const auto& Pair : ControllerIds)
	{
		if (Pair.Key.IsValid() && Resolved.Contains(Pair.Key) && Pair.Key->GetPawn())
		{
			FGamerSaveRecord Record = CollectPlayer(Pair.Key.Get());
			Record.SetString(TEXT("id"), Pair.Value);
			Players.Add(Pair.Value, Record);
		}
	}
	TArray<FGamerSaveRecord> PlayerList;
	Players.GenerateValueArray(PlayerList);
	Root.SetRecords(TEXT("players"), PlayerList);
	return Root.Json.ToSharedRef();
}

// --- World ---

FGamerSaveRecord UGamerSaveSubsystem::CollectWorld() const
{
	UWorld* World = GetWorld();
	FGamerSaveRecord Out;
	if (!World)
	{
		return Out;
	}
	if (const AGamerGameState* State = World->GetGameState<AGamerGameState>())
	{
		FGamerSaveRecord Record;
		State->WriteSaveState(Record);
		Out.SetRecord(TEXT("game_state"), Record);
	}
	if (const AGamerSettlement* Settlement = AGamerSettlement::Get(World))
	{
		FGamerSaveRecord Record;
		Record.SetTransform(TEXT("transform"), Settlement->GetActorTransform());
		Settlement->WriteSaveState(Record);
		Out.SetRecord(TEXT("settlement"), Record);
	}

	TArray<FGamerSaveRecord> Nodes;
	for (TActorIterator<AGamerResourceNode> It(World); It; ++It)
	{
		if (!IsValid(*It) || It->IsActorBeingDestroyed())
		{
			continue;
		}
		FGamerSaveRecord Record;
		Record.SetTransform(TEXT("transform"), It->GetActorTransform());
		It->WriteSaveState(Record);
		Nodes.Add(Record);
	}
	Out.SetRecords(TEXT("nodes"), Nodes);

	// Buildings get a save index, so villagers can name their bed.
	TMap<const AGamerBuilding*, int32> BuildingIndex;
	TArray<FGamerSaveRecord> Buildings;
	for (TActorIterator<AGamerBuilding> It(World); It; ++It)
	{
		if (!IsValid(*It) || It->IsPreview() || It->IsActorBeingDestroyed() || It->GetBuildableId().IsNone())
		{
			continue;
		}
		FGamerSaveRecord Record;
		Record.SetInt(TEXT("sid"), Buildings.Num());
		Record.SetTransform(TEXT("transform"), It->GetActorTransform());
		It->WriteSaveState(Record);
		BuildingIndex.Add(*It, Buildings.Num());
		Buildings.Add(Record);
	}
	Out.SetRecords(TEXT("buildings"), Buildings);

	TArray<FGamerSaveRecord> GroundItems;
	for (TActorIterator<AGamerGroundItem> It(World); It; ++It)
	{
		if (!IsValid(*It) || It->IsActorBeingDestroyed() || It->GetCount() <= 0)
		{
			continue;
		}
		FGamerSaveRecord Record;
		Record.SetTransform(TEXT("transform"), It->GetActorTransform());
		It->WriteSaveState(Record);
		GroundItems.Add(Record);
	}
	Out.SetRecords(TEXT("ground_items"), GroundItems);

	TArray<FGamerSaveRecord> Graves;
	for (TActorIterator<AGamerGrave> It(World); It; ++It)
	{
		if (!IsValid(*It) || It->IsActorBeingDestroyed() || It->GetItemCount() <= 0)
		{
			continue;
		}
		FGamerSaveRecord Record;
		Record.SetTransform(TEXT("transform"), It->GetActorTransform());
		It->WriteSaveState(Record);
		Graves.Add(Record);
	}
	Out.SetRecords(TEXT("graves"), Graves);

	TArray<FGamerSaveRecord> Villagers;
	for (TActorIterator<AGamerVillager> It(World); It; ++It)
	{
		if (!IsValid(*It) || It->IsActorBeingDestroyed())
		{
			continue;
		}
		FGamerSaveRecord Record;
		Record.SetTransform(TEXT("transform"), It->GetActorTransform());
		It->WriteSaveState(Record);
		const int32* Bed = It->GetBed() ? BuildingIndex.Find(It->GetBed()) : nullptr;
		Record.SetInt(TEXT("bed"), Bed ? *Bed : -1);
		Villagers.Add(Record);
	}
	Out.SetRecords(TEXT("villagers"), Villagers);
	return Out;
}

void UGamerSaveSubsystem::RestoreWorld(const FGamerSaveRecord& Block, int32& OutSkipped)
{
	UWorld* World = GetWorld();
	const UGamerDataSubsystem* Data = UGamerDataSubsystem::Get(World);
	if (!World || !Data)
	{
		UE_LOG(LogGamer, Error, TEXT("Save: cannot restore (no world or no game data)"));
		return;
	}
	OutSkipped = 0;

	// Start from an empty world (maps may bring their own nodes; tests restore twice).
	for (TActorIterator<AGamerGrave> It(World); It; ++It) { It->Destroy(); }
	for (TActorIterator<AGamerVillager> It(World); It; ++It) { It->Destroy(); }
	for (TActorIterator<AGamerGroundItem> It(World); It; ++It) { It->Destroy(); }
	for (TActorIterator<AGamerBuilding> It(World); It; ++It) { if (!It->IsPreview()) { It->Destroy(); } }
	for (TActorIterator<AGamerResourceNode> It(World); It; ++It) { It->Destroy(); }

	if (AGamerGameState* State = World->GetGameState<AGamerGameState>())
	{
		State->ReadSaveState(Block.GetRecord(TEXT("game_state")));
	}

	const FGamerSaveRecord SettlementBlock = Block.GetRecord(TEXT("settlement"));
	AGamerSettlement* Settlement = AGamerSettlement::Get(World);
	if (!Settlement && SettlementBlock.Has(TEXT("transform")))
	{
		const FTransform Transform = SettlementBlock.GetTransform(TEXT("transform"));
		Settlement = World->SpawnActor<AGamerSettlement>(AGamerSettlement::StaticClass(), Transform);
	}
	if (Settlement)
	{
		Settlement->ReadSaveState(SettlementBlock);
	}

	int32 Nodes = 0;
	for (const FGamerSaveRecord& Record : Block.GetRecords(TEXT("nodes")))
	{
		const FName Id = Record.GetName(TEXT("id"));
		if (!Data->FindResourceNode(Id))
		{
			UE_LOG(LogGamer, Warning, TEXT("Save: resource node \"%s\" no longer in the data, skipped"), *Id.ToString());
			++OutSkipped;
			continue;
		}
		const FTransform Transform = Record.GetTransform(TEXT("transform"));
		AGamerResourceNode* Node = World->SpawnActorDeferred<AGamerResourceNode>(AGamerResourceNode::StaticClass(), Transform, nullptr, nullptr,
			ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
		if (!Node)
		{
			++OutSkipped;
			continue;
		}
		Node->SetNodeId(Id);
		Node->FinishSpawning(Transform);
		Node->ReadSaveState(Record);
		++Nodes;
	}

	TMap<int32, AGamerBuilding*> BySid;
	int32 Buildings = 0;
	for (const FGamerSaveRecord& Record : Block.GetRecords(TEXT("buildings")))
	{
		const FName Id = Record.GetName(TEXT("id"));
		if (!Data->FindBuildable(Id))
		{
			UE_LOG(LogGamer, Warning, TEXT("Save: building \"%s\" no longer in the data, skipped"), *Id.ToString());
			++OutSkipped;
			continue;
		}
		const FTransform Transform = Record.GetTransform(TEXT("transform"));
		AGamerBuilding* Building = World->SpawnActorDeferred<AGamerBuilding>(AGamerBuilding::ClassFor(World, Id), Transform, nullptr, nullptr,
			ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
		if (!Building)
		{
			++OutSkipped;
			continue;
		}
		Building->InitBuilding(Id, Record.GetFloat(TEXT("progress"), 1.f));
		Building->FinishSpawning(Transform);
		if (!IsValid(Building) || Building->IsActorBeingDestroyed())
		{
			continue; // a planted tree turns into a resource node in BeginPlay
		}
		Building->ReadSaveState(Record);
		BySid.Add(Record.GetInt(TEXT("sid"), -1), Building);
		++Buildings;
	}

	// Fresh season nodes run their season once in BeginPlay; in autumn that drops fruit before ReadSaveState sets the
	// saved fruit count. Those drops are not part of the save: remove them, the saved ground items follow.
	for (TActorIterator<AGamerGroundItem> It(World); It; ++It) { It->Destroy(); }
	int32 GroundItems = 0;
	for (const FGamerSaveRecord& Record : Block.GetRecords(TEXT("ground_items")))
	{
		const FName Item = Record.GetName(TEXT("item"));
		if (!GamerSave::IsKnownItem(World, Item))
		{
			UE_LOG(LogGamer, Warning, TEXT("Save: ground item \"%s\" no longer in the data, skipped"), *Item.ToString());
			++OutSkipped;
			continue;
		}
		const FTransform Transform = Record.GetTransform(TEXT("transform"));
		if (AGamerGroundItem* Ground = AGamerGroundItem::SpawnFallen(World, Item, FMath::Max(1, Record.GetInt(TEXT("count"), 1)),
			Record.GetFloat(TEXT("fresh"), 1.f), Transform.GetLocation(), 0.f))
		{
			Ground->SetActorRotation(Transform.Rotator());
			Ground->ReadSaveState(Record);
			++GroundItems;
		}
	}

	int32 Graves = 0;
	for (const FGamerSaveRecord& Record : Block.GetRecords(TEXT("graves")))
	{
		const FTransform Transform = Record.GetTransform(TEXT("transform"));
		TArray<FGamerItemAmount> Items = Record.GetItems(TEXT("items"));
		GamerSave::FilterKnownItems(World, Items, TEXT("grave"));
		if (AGamerGrave* Grave = AGamerGrave::Create(World, Transform.GetLocation(), Record.GetString(TEXT("owner_key")), Record.GetString(TEXT("owner_name")), Items))
		{
			Grave->SetActorRotation(Transform.Rotator());
			Grave->ReadSaveState(Record);
			++Graves;
		}
	}

	int32 Villagers = 0;
	for (const FGamerSaveRecord& Record : Block.GetRecords(TEXT("villagers")))
	{
		const FTransform Transform = Record.GetTransform(TEXT("transform"));
		FActorSpawnParameters Params;
		Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
		AGamerVillager* Villager = World->SpawnActor<AGamerVillager>(AGamerVillager::StaticClass(), Transform.GetLocation(), Transform.Rotator(), Params);
		if (!Villager)
		{
			++OutSkipped;
			continue;
		}
		AGamerBuilding* const* Bed = BySid.Find(Record.GetInt(TEXT("bed"), -1));
		Villager->InitVillager(Record.GetString(TEXT("name"), TEXT("Bewohner")), Bed ? *Bed : nullptr);
		Villager->ReadSaveState(Record);
		++Villagers;
	}
	UE_LOG(LogGamer, Log, TEXT("Save: restored %d nodes, %d buildings, %d ground items, %d graves, %d villagers (%d skipped)"), Nodes, Buildings, GroundItems, Graves, Villagers, OutSkipped);
}

// --- Players ---

static void ForEachSaveable(const AController* Controller, TFunctionRef<void(IGamerSaveable&, const FString&)> Visit)
{
	TArray<const AActor*> Owners;
	if (Controller)
	{
		Owners.Add(Controller->GetPawn());
		Owners.Add(Controller->PlayerState);
	}
	TSet<FString> Seen;
	for (const AActor* Owner : Owners)
	{
		if (!Owner)
		{
			continue;
		}
		for (UActorComponent* Component : Owner->GetComponents())
		{
			if (IGamerSaveable* Saveable = Cast<IGamerSaveable>(Component))
			{
				const FString Key = Saveable->GetSaveKey();
				if (!Key.IsEmpty() && !Seen.Contains(Key))
				{
					Seen.Add(Key);
					Visit(*Saveable, Key);
				}
			}
		}
	}
}

FGamerSaveRecord UGamerSaveSubsystem::CollectPlayer(const AController* Controller) const
{
	FGamerSaveRecord Out;
	if (!Controller)
	{
		return Out;
	}
	if (Controller->PlayerState)
	{
		Out.SetString(TEXT("name"), Controller->PlayerState->GetPlayerName());
	}
	if (const APawn* Pawn = Controller->GetPawn())
	{
		Out.SetVector(TEXT("location"), Pawn->GetActorLocation());
		Out.SetNumber(TEXT("yaw"), Controller->GetControlRotation().Yaw);
	}
	FGamerSaveRecord Components;
	ForEachSaveable(Controller, [&Components](IGamerSaveable& Saveable, const FString& Key)
	{
		FGamerSaveRecord Record;
		Saveable.WriteSaveState(Record);
		Components.SetRecord(Key, Record);
	});
	Out.SetRecord(TEXT("components"), Components);
	return Out;
}

void UGamerSaveSubsystem::RestorePlayer(AController* Controller, const FGamerSaveRecord& Record) const
{
	APawn* Pawn = Controller ? Controller->GetPawn() : nullptr;
	if (!Pawn)
	{
		return;
	}
	if (Record.Has(TEXT("location")))
	{
		const FVector Location = Record.GetVector(TEXT("location"));
		const FRotator Rotation(0.f, Record.GetFloat(TEXT("yaw")), 0.f);
		Pawn->TeleportTo(Location, Rotation, false, false);
		Controller->SetControlRotation(Rotation);
		if (APlayerController* Player = Cast<APlayerController>(Controller))
		{
			Player->ClientSetRotation(Rotation, true);
		}
	}
	const FGamerSaveRecord Components = Record.GetRecord(TEXT("components"));
	ForEachSaveable(Controller, [&Components](IGamerSaveable& Saveable, const FString& Key)
	{
		if (Components.Has(Key))
		{
			Saveable.ReadSaveState(Components.GetRecord(Key));
		}
	});
	UE_LOG(LogGamer, Log, TEXT("Save: player %s restored at %s"), *GetPlayerId(Controller), *Pawn->GetActorLocation().ToCompactString());
}

bool UGamerSaveSubsystem::IsKnownPlayer(const FString& PlayerId) const
{
	return PlayerRecords.Contains(PlayerId);
}

FString UGamerSaveSubsystem::GetPlayerId(const AController* Controller) const
{
	const FString* Id = ControllerIds.Find(const_cast<AController*>(Controller));
	return Id ? *Id : FString();
}

bool UGamerSaveSubsystem::HasSavedInventory(const AController* Controller) const
{
	return IsKnownPlayer(GetPlayerId(Controller)) || CarriedInventories.Contains(const_cast<AController*>(Controller));
}

bool UGamerSaveSubsystem::IsPlayerResolved(const AController* Controller) const
{
	return !bActive || Resolved.Contains(const_cast<AController*>(Controller));
}

void UGamerSaveSubsystem::HandlePostLogin(AGameModeBase* GameMode, APlayerController* NewPlayer)
{
	if (!bActive || !NewPlayer || NewPlayer->GetWorld() != GetWorld())
	{
		return;
	}
	AwaitingId.Add(NewPlayer, FPlatformTime::Seconds());
	if (!NewPlayer->FindComponentByClass<UGamerSaveClientComponent>())
	{
		UGamerSaveClientComponent* Component = NewObject<UGamerSaveClientComponent>(NewPlayer, TEXT("GamerSaveClient"));
		Component->SetIsReplicated(true);
		Component->RegisterComponent(); // BeginPlay on the host sends at once; remote clients get it replicated
	}
}

void UGamerSaveSubsystem::HandleIdentify(APlayerController* Controller, const FString& RequestedId, const FString& CarriedInventory)
{
	if (!bActive || !Controller || ControllerIds.Contains(Controller))
	{
		return;
	}
	// Folder/JSON-safe and short; never trust the client's text blindly.
	FString Id;
	for (const TCHAR Char : RequestedId.Left(64))
	{
		if (Char < 128 && (FChar::IsAlnum(Char) || Char == TEXT('_') || Char == TEXT('-')))
		{
			Id.AppendChar(Char);
		}
	}
	if (Id.IsEmpty())
	{
		Id = TEXT("player");
	}
	// Two connected players with the same id (several game instances on one PC sharing their settings): number them.
	FString Unique = Id;
	for (int32 Number = 2; ; ++Number)
	{
		bool bTaken = false;
		for (const auto& Pair : ControllerIds)
		{
			bTaken |= Pair.Key.IsValid() && Pair.Value == Unique;
		}
		if (!bTaken)
		{
			break;
		}
		Unique = FString::Printf(TEXT("%s-%d"), *Id, Number);
	}
	if (Unique != Id)
	{
		UE_LOG(LogGamer, Warning, TEXT("Save: player id %s is already playing; this player gets %s"), *Id, *Unique);
	}
	AwaitingId.Remove(Controller);
	ControllerIds.Add(Controller, Unique);
	if (AGamerGameState::Get(this).bCarryInventory)
	{
		if (const TSharedPtr<FJsonObject> Parsed = ParseObject(CarriedInventory))
		{
			CarriedInventories.Add(Controller, FGamerSaveRecord(Parsed));
			UE_LOG(LogGamer, Log, TEXT("Save: player %s brings the inventory of their character"), *Unique);
		}
	}
	if (PlayerRecords.Contains(Unique) || CarriedInventories.Contains(Controller))
	{
		PendingRestore.AddUnique(Controller);
		UE_LOG(LogGamer, Log, TEXT("Save: player %s is known in this world, restoring"), *Unique);
	}
	else
	{
		Resolved.Add(Controller);
		UE_LOG(LogGamer, Log, TEXT("Save: player %s is new in this world"), *Unique);
		if (AGamerGameState::Get(this).bCarryInventory)
		{
			if (UGamerSaveClientComponent* Client = Controller->FindComponentByClass<UGamerSaveClientComponent>())
			{
				Client->ClientKeepCarriedInventory(FString()); // start equipment comes next; from then on the client keeps the file
			}
		}
	}
}

void UGamerSaveSubsystem::TickRestore()
{
	const double Now = FPlatformTime::Seconds();
	// No answer from a client (old build, component not replicated): fall back to its online id.
	for (auto It = AwaitingId.CreateIterator(); It; ++It)
	{
		if (!It.Key().IsValid())
		{
			It.RemoveCurrent();
			continue;
		}
		if (Now - It.Value() > 10.0)
		{
			APlayerController* Controller = It.Key().Get();
			const FUniqueNetIdRepl& NetId = Controller->PlayerState ? Controller->PlayerState->GetUniqueId() : FUniqueNetIdRepl();
			const FString Fallback = NetId.IsValid() ? NetId->ToString() : FString::Printf(TEXT("player_%s"), Controller->PlayerState ? *Controller->PlayerState->GetPlayerName() : TEXT("unknown"));
			UE_LOG(LogGamer, Warning, TEXT("Save: player sent no id within 10 s, using fallback %s"), *Fallback);
			It.RemoveCurrent();
			HandleIdentify(Controller, Fallback);
			return; // HandleIdentify changed the maps; continue next tick
		}
	}
	for (int32 Index = PendingRestore.Num() - 1; Index >= 0; --Index)
	{
		AController* Controller = PendingRestore[Index].Get();
		if (!Controller)
		{
			PendingRestore.RemoveAt(Index);
			continue;
		}
		if (!Controller->GetPawn())
		{
			continue;
		}
		const FString Id = GetPlayerId(Controller);
		if (const FGamerSaveRecord* Record = PlayerRecords.Find(Id))
		{
			RestorePlayer(Controller, *Record);
		}
		if (AGamerGameState::Get(this).bCarryInventory)
		{
			// The character's own inventory wins over what this world remembers.
			UGamerInventoryComponent* Inventory = Controller->GetPawn()->FindComponentByClass<UGamerInventoryComponent>();
			const FGamerSaveRecord* Carried = CarriedInventories.Find(Controller);
			if (Carried && Inventory)
			{
				Inventory->ReadSaveState(*Carried);
				UE_LOG(LogGamer, Log, TEXT("Save: player %s got the character's inventory: %s"), *Id, *Inventory->Describe());
			}
			UGamerSaveClientComponent* Client = Controller->FindComponentByClass<UGamerSaveClientComponent>();
			if (Inventory && Client)
			{
				FGamerSaveRecord Applied;
				Inventory->WriteSaveState(Applied);
				Client->ClientKeepCarriedInventory(GamerSave::ToJsonText(Applied.Json.ToSharedRef()));
			}
		}
		Resolved.Add(Controller);
		PendingRestore.RemoveAt(Index);
	}
}

void UGamerSaveSubsystem::HandleLogout(AGameModeBase* GameMode, AController* Exiting)
{
	if (!bActive || !Exiting || Exiting->GetWorld() != GetWorld())
	{
		return;
	}
	const FString Id = GetPlayerId(Exiting);
	if (!Id.IsEmpty() && Resolved.Contains(Exiting) && Exiting->GetPawn())
	{
		FGamerSaveRecord Record = CollectPlayer(Exiting);
		Record.SetString(TEXT("id"), Id);
		PlayerRecords.Add(Id, Record);
		UE_LOG(LogGamer, Log, TEXT("Save: player %s left, record kept"), *Id);
	}
	ControllerIds.Remove(Exiting);
	CarriedInventories.Remove(Exiting);
	Resolved.Remove(Exiting);
	PendingRestore.Remove(Exiting);
	AwaitingId.Remove(Cast<APlayerController>(Exiting));
}

void UGamerSaveSubsystem::HandleWorldTearDown(UWorld* InWorld)
{
	if (InWorld == GetWorld() && bActive && !bShutdownSaved)
	{
		bShutdownSaved = true;
		SaveNow(TEXT("world end"));
	}
}

void UGamerSaveSubsystem::HandlePreExit()
{
	if (bActive && !bShutdownSaved && GetWorld())
	{
		bShutdownSaved = true;
		SaveNow(TEXT("shutdown"));
	}
}
