// Character page of the start menu (choose origin, name, appearance). Part of AGamerPlayerController,
// kept in its own file so the menu code in GamerPlayerController.cpp stays small.

#include "GamerPlayerController.h"

#include "Gamer.h"
#include "GamerDataSubsystem.h"
#include "GamerMenuGameMode.h"
#include "GamerPlayerState.h"
#include "InputCoreTypes.h"

void AGamerPlayerController::OpenCharacterPage(EGamerMenuPage Next)
{
	CharacterNext = Next;
	MenuPage = EGamerMenuPage::Character;
	MenuNote.Reset();

	// First visit (or data changed): start with the first origin and a suggested name.
	const UGamerDataSubsystem* Data = UGamerDataSubsystem::Get(this);
	if (!Data || Data->GetCharacters().IsEmpty())
	{
		MenuNote = TEXT("Charakterdaten fehlen (characters.json). Weiter mit Standardfigur.");
		return;
	}
	const FGamerCharacterDataSet& Characters = Data->GetCharacters();
	FGamerCharacterProfile& Profile = UGamerCharacterSettings::Get()->Profile;
	if (!Characters.FindOrigin(Profile.Origin))
	{
		const FString Name = Characters.Names.Num() > 0 ? Characters.Names[FMath::RandRange(0, Characters.Names.Num() - 1)] : FString();
		Profile = Characters.MakeProfile(Characters.Origins[0].Id, Profile.Name.IsEmpty() ? Name : Profile.Name);
	}
	else
	{
		Characters.Sanitize(Profile);
	}
}

void AGamerPlayerController::HandleCharacterButton(const FString& Id)
{
	const UGamerDataSubsystem* Data = UGamerDataSubsystem::Get(this);
	UGamerCharacterSettings* Settings = UGamerCharacterSettings::Get();
	FGamerCharacterProfile& Profile = Settings->Profile;
	if (!Data || Data->GetCharacters().IsEmpty())
	{
		if (Id == TEXT("char:next"))
		{
			MenuPage = CharacterNext;
		}
		return;
	}
	const FGamerCharacterDataSet& Characters = Data->GetCharacters();
	MenuNote.Reset();

	if (Id.StartsWith(TEXT("char:origin:")))
	{
		// New origin: its values, equipment and suggested look; the name stays.
		const FName Origin(*Id.Mid(12));
		if (Characters.FindOrigin(Origin))
		{
			Profile = Characters.MakeProfile(Origin, Profile.Name);
		}
	}
	else if (Id == TEXT("char:randomname") && Characters.Names.Num() > 0)
	{
		FString Name = Profile.Name;
		for (int32 Try = 0; Try < 8 && Name == Profile.Name; ++Try)
		{
			Name = Characters.Names[FMath::RandRange(0, Characters.Names.Num() - 1)];
		}
		Profile.Name = Name;
	}
	else if (Id.StartsWith(TEXT("char:name:")))
	{
		Profile.Name = Id.Mid(10); // menu test scripts
		Characters.Sanitize(Profile);
	}
	else if (Id.StartsWith(TEXT("char:look:")))
	{
		// Next option of that slot (wraps around).
		const FGamerLookSlotDef* Slot = Characters.FindLookSlot(FName(*Id.Mid(10)));
		if (Slot && Slot->Options.Num() > 0)
		{
			const FName Current = Profile.GetLook(Slot->Id);
			const int32 Index = Slot->Options.IndexOfByPredicate([Current](const FGamerLookOption& O) { return O.Id == Current; });
			Profile.SetLook(Slot->Id, Slot->Options[(Index + 1) % Slot->Options.Num()].Id);
		}
	}
	else if (Id == TEXT("char:lookreset"))
	{
		Profile = Characters.MakeProfile(Profile.Origin, Profile.Name);
		MenuNote = TEXT("Aussehen auf den Vorschlag der Herkunft zurückgesetzt.");
	}
	else if (Id == TEXT("char:edit"))
	{
		MenuPage = EGamerMenuPage::CharacterLook;
	}
	else if (Id == TEXT("char:lookdone"))
	{
		MenuPage = EGamerMenuPage::Character;
	}
	else if (Id == TEXT("char:next"))
	{
		Profile.Name.TrimStartAndEndInline();
		if (Profile.Name.IsEmpty())
		{
			MenuNote = TEXT("Bitte einen Namen eingeben (oder „Zufallsname“).");
			return;
		}
		Characters.Sanitize(Profile);
		Settings->bHasCharacter = true;
		Settings->Save();
		UE_LOG(LogGamer, Log, TEXT("Character saved: %s"), *Profile.ToLogString());
		MenuPage = CharacterNext;
		if (MenuPage == EGamerMenuPage::Join)
		{
			JoinAddress = UGamerMenuSettings::Get()->JoinAddress;
		}
	}
}

bool AGamerPlayerController::TypeIntoCharacterName(const FKey& Key)
{
	FString& Name = UGamerCharacterSettings::Get()->Profile.Name;
	const UGamerDataSubsystem* Data = UGamerDataSubsystem::Get(this);
	const int32 MaxLength = Data ? Data->GetCharacters().NameMaxLength : 16;
	if (Key == EKeys::BackSpace)
	{
		Name.LeftChopInline(1);
		return true;
	}
	if (Key == EKeys::Enter)
	{
		HandleCharacterButton(TEXT("char:next"));
		return true;
	}
	TCHAR Char = 0;
	const FString KeyName = Key.GetFName().ToString();
	if (KeyName.Len() == 1 && FChar::IsAlpha(KeyName[0]))
	{
		// Capital letter at the start and after a space or with Shift held.
		const bool bShift = IsInputKeyDown(EKeys::LeftShift) || IsInputKeyDown(EKeys::RightShift);
		const bool bWordStart = Name.IsEmpty() || Name.EndsWith(TEXT(" ")) || Name.EndsWith(TEXT("-"));
		Char = (bShift || bWordStart) ? FChar::ToUpper(KeyName[0]) : FChar::ToLower(KeyName[0]);
	}
	else if (Key == EKeys::SpaceBar)
	{
		Char = TEXT(' ');
	}
	else if (Key == EKeys::Hyphen || Key == EKeys::Subtract)
	{
		Char = TEXT('-');
	}
	if (Char == 0)
	{
		return false;
	}
	if (Name.Len() < MaxLength && !(Char == TEXT(' ') && (Name.IsEmpty() || Name.EndsWith(TEXT(" ")))))
	{
		Name.AppendChar(Char);
	}
	return true;
}
