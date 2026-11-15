// Save system parts of the player controller: "Welt laden" page, new world names, "Speichern" in the Esc menu.
// Kept in its own file like GamerPlayerControllerCharacter.cpp.

#include "GamerPlayerController.h"

#include "Gamer.h"
#include "GamerMenuGameMode.h"
#include "GamerSaveSubsystem.h"
#include "GamerSessionSubsystem.h"
#include "Kismet/GameplayStatics.h"

void AGamerPlayerController::OpenLoadWorldPage()
{
	SavedWorlds = GamerSave::ListWorlds();
	MenuPage = EGamerMenuPage::LoadWorld;
	MenuNote.Reset();
}

FString AGamerPlayerController::MakeNewWorldName()
{
	// Proposal 2026-11-15: worlds are named automatically ("Welt 3"); renaming can come later with a text field.
	// Drawn every frame on "Welt erstellen": read the folder at most every 2 seconds.
	static FString Cached;
	static double CachedAt = -100.0;
	if (!Cached.IsEmpty() && FPlatformTime::Seconds() - CachedAt < 2.0)
	{
		return Cached;
	}
	CachedAt = FPlatformTime::Seconds();
	TSet<FString> Taken;
	for (const FGamerWorldSaveInfo& Info : GamerSave::ListWorlds())
	{
		Taken.Add(Info.Name);
	}
	for (int32 Number = 1; ; ++Number)
	{
		const FString Name = FString::Printf(TEXT("Welt %d"), Number);
		if (!Taken.Contains(Name))
		{
			Cached = Name;
			return Name;
		}
	}
}

bool AGamerPlayerController::HandleSaveButton(const FString& Id)
{
	if (Id == TEXT("loadpage"))
	{
		OpenLoadWorldPage();
		return true;
	}
	if (Id.StartsWith(TEXT("load:")))
	{
		const FString WorldId = Id.Mid(5);
		const FGamerWorldSaveInfo* Info = SavedWorlds.FindByPredicate([&WorldId](const FGamerWorldSaveInfo& W) { return W.WorldId == WorldId; });
		if (!Info || !GamerSave::IsValidWorldId(WorldId))
		{
			MenuNote = TEXT("Diese Welt gibt es nicht mehr.");
			OpenLoadWorldPage();
			return true;
		}
		HostSavedWorld(WorldId, FString(), nullptr);
		return true;
	}
	if (Id == TEXT("save"))
	{
		if (UGamerSaveClientComponent* Saves = FindComponentByClass<UGamerSaveClientComponent>())
		{
			Saves->ServerRequestSave();
			MenuNote = HasAuthority() ? TEXT("Gespeichert.") : TEXT("Speichern angefragt (nur der Gastgeber speichert).");
		}
		else
		{
			MenuNote = TEXT("Diese Welt wird nicht gespeichert (ohne Spielstand gestartet).");
		}
		return true;
	}
	return false;
}

void AGamerPlayerController::HostSavedWorld(const FString& WorldId, const FString& NewName, const FGamerWorldOptions* NewOptions)
{
	// The host is a listen server, like "Welt erstellen" before; the world id makes the server load and save it.
	if (UGamerSessionSubsystem* Session = UGamerSessionSubsystem::Get(this))
	{
		Session->ClearError();
	}
	FString Options = TEXT("listen?game=/Script/Gamer.GamerGameMode?SaveWorld=") + WorldId;
	if (NewOptions)
	{
		Options += NewOptions->ToUrlOptions(); // a loaded world keeps its own options from the save
	}
	if (!NewName.IsEmpty())
	{
		Options += TEXT("?WorldName=") + NewName.Replace(TEXT(" "), TEXT("+")).Replace(TEXT("?"), TEXT("")).Replace(TEXT("="), TEXT(""));
	}
	UE_LOG(LogGamer, Log, TEXT("Hosting world %s?%s"), GamerMaps::World, *Options);
	MenuNote = NewOptions ? TEXT("Welt wird erstellt …") : TEXT("Welt wird geladen …");
	UGameplayStatics::OpenLevel(this, FName(GamerMaps::World), true, Options);
}
