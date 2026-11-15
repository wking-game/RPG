#include "GamerKeyBindings.h"

#include "Gamer.h"

FName GamerInputActions::Hotbar(int32 Number)
{
	return FName(*FString::Printf(TEXT("hotbar_%d"), Number));
}

const TArray<FGamerBindableAction>& UGamerKeyBindings::GetActions()
{
	static const TArray<FGamerBindableAction> Actions = []
	{
		TArray<FGamerBindableAction> List = {
			{ GamerInputActions::MoveForward, TEXT("Vorwärts"), EKeys::W },
			{ GamerInputActions::MoveBack, TEXT("Rückwärts"), EKeys::S },
			{ GamerInputActions::MoveLeft, TEXT("Links"), EKeys::A },
			{ GamerInputActions::MoveRight, TEXT("Rechts"), EKeys::D },
			{ GamerInputActions::Jump, TEXT("Springen"), EKeys::SpaceBar },
			{ GamerInputActions::Sprint, TEXT("Sprinten (halten)"), EKeys::LeftShift },
			{ GamerInputActions::Strike, TEXT("Schlagen"), EKeys::LeftMouseButton },
			{ GamerInputActions::Interact, TEXT("Sammeln"), EKeys::E },
			{ GamerInputActions::FocusNext, TEXT("Im Haufen wechseln"), EKeys::F },
			{ GamerInputActions::PickSame, TEXT("Alles gleicher Art (mit Sammeln)"), EKeys::LeftShift },
			{ GamerInputActions::CraftMenu, TEXT("Herstellen-Menü"), EKeys::Tab },
			{ GamerInputActions::BuildMenu, TEXT("Baumenü (mit Hammer)"), EKeys::RightMouseButton },
			{ GamerInputActions::Rotate, TEXT("Bauteil drehen"), EKeys::R },
			{ GamerInputActions::BuildMulti, TEXT("Mehrfach bauen (halten)"), EKeys::LeftShift },
			{ GamerInputActions::Demolish, TEXT("Abreißen"), EKeys::X },
			{ GamerInputActions::Scan, TEXT("Umschauen (Material)"), EKeys::Z },
			{ GamerInputActions::Listen, TEXT("Lauschen (Magie)"), EKeys::Q },
			{ GamerInputActions::VillageMenu, TEXT("Dorfverwaltung"), EKeys::V },
			{ GamerInputActions::BirdView, TEXT("Vogelperspektive"), EKeys::B },
		};
		const FKey NumberKeys[] = { EKeys::One, EKeys::Two, EKeys::Three, EKeys::Four,
			EKeys::Five, EKeys::Six, EKeys::Seven, EKeys::Eight };
		for (int32 Index = 0; Index < UE_ARRAY_COUNT(NumberKeys); ++Index)
		{
			List.Add({ GamerInputActions::Hotbar(Index + 1), FString::Printf(TEXT("Schnellleiste %d"), Index + 1), NumberKeys[Index] });
		}
		List.Add({ GamerInputActions::HotbarNext, TEXT("Schnellleiste weiter"), EKeys::MouseScrollDown });
		List.Add({ GamerInputActions::HotbarPrev, TEXT("Schnellleiste zurück"), EKeys::MouseScrollUp });
		return List;
	}();
	return Actions;
}

FKey UGamerKeyBindings::GetKey(FName Action) const
{
	for (const FGamerKeyOverride& Override : Overrides)
	{
		if (Override.Action == Action && Override.Key.IsValid())
		{
			return Override.Key;
		}
	}
	for (const FGamerBindableAction& Def : GetActions())
	{
		if (Def.Id == Action)
		{
			return Def.DefaultKey;
		}
	}
	return EKeys::Invalid;
}

FString UGamerKeyBindings::ActionLabel(FName Action)
{
	for (const FGamerBindableAction& Def : GetActions())
	{
		if (Def.Id == Action)
		{
			return Def.Label;
		}
	}
	return Action.ToString();
}

bool UGamerKeyBindings::CanBind(const FKey& Key)
{
	return Key.IsValid() && Key != EKeys::Escape && !Key.IsGamepadKey() && !Key.IsAnalog() && !Key.IsTouch();
}

void UGamerKeyBindings::SetOverride(FName Action, const FKey& Key)
{
	Overrides.RemoveAll([Action](const FGamerKeyOverride& Override) { return Override.Action == Action; });
	for (const FGamerBindableAction& Def : GetActions())
	{
		if (Def.Id == Action && Def.DefaultKey != Key)
		{
			FGamerKeyOverride& Override = Overrides.AddDefaulted_GetRef();
			Override.Action = Action;
			Override.Key = Key;
		}
	}
}

FName UGamerKeyBindings::SetKey(FName Action, const FKey& Key)
{
	if (!CanBind(Key))
	{
		return NAME_None;
	}
	const FKey OldKey = GetKey(Action);
	FName Swapped = NAME_None;
	// Sprint and "build several" share Shift by default on purpose (different situations): never swapped against each other.
	auto MayShare = [](FName A, FName B)
	{
		auto IsShiftLike = [](FName Id) { return Id == GamerInputActions::Sprint || Id == GamerInputActions::BuildMulti || Id == GamerInputActions::PickSame; };
		return IsShiftLike(A) && IsShiftLike(B);
	};
	for (const FGamerBindableAction& Def : GetActions())
	{
		if (Def.Id != Action && GetKey(Def.Id) == Key && !MayShare(Action, Def.Id))
		{
			SetOverride(Def.Id, OldKey);
			Swapped = Def.Id;
		}
	}
	SetOverride(Action, Key);
	SaveConfig();
	UE_LOG(LogGamer, Log, TEXT("Key binding: %s = %s%s"), *Action.ToString(), *Key.ToString(),
		Swapped.IsNone() ? TEXT("") : *FString::Printf(TEXT(" (swapped: %s = %s)"), *Swapped.ToString(), *OldKey.ToString()));
	return Swapped;
}

void UGamerKeyBindings::ResetToDefaults()
{
	Overrides.Reset();
	SaveConfig();
	UE_LOG(LogGamer, Log, TEXT("Key bindings reset to defaults"));
}

FString UGamerKeyBindings::KeyName(const FKey& Key)
{
	static const TMap<FKey, FString> Names = {
		{ EKeys::LeftMouseButton, TEXT("Linksklick") },
		{ EKeys::RightMouseButton, TEXT("Rechtsklick") },
		{ EKeys::MiddleMouseButton, TEXT("Mausrad-Klick") },
		{ EKeys::ThumbMouseButton, TEXT("Maustaste 4") },
		{ EKeys::ThumbMouseButton2, TEXT("Maustaste 5") },
		{ EKeys::MouseScrollUp, TEXT("Mausrad hoch") },
		{ EKeys::MouseScrollDown, TEXT("Mausrad runter") },
		{ EKeys::SpaceBar, TEXT("Leertaste") },
		{ EKeys::LeftShift, TEXT("Umschalt") },
		{ EKeys::RightShift, TEXT("Umschalt rechts") },
		{ EKeys::LeftControl, TEXT("Strg") },
		{ EKeys::RightControl, TEXT("Strg rechts") },
		{ EKeys::LeftAlt, TEXT("Alt") },
		{ EKeys::RightAlt, TEXT("Alt Gr") },
		{ EKeys::Enter, TEXT("Eingabe") },
		{ EKeys::BackSpace, TEXT("Rücktaste") },
		{ EKeys::Tab, TEXT("Tab") },
		{ EKeys::CapsLock, TEXT("Feststell") },
		{ EKeys::Escape, TEXT("Esc") },
		{ EKeys::Delete, TEXT("Entf") },
		{ EKeys::Insert, TEXT("Einfg") },
		{ EKeys::Home, TEXT("Pos1") },
		{ EKeys::End, TEXT("Ende") },
		{ EKeys::PageUp, TEXT("Bild hoch") },
		{ EKeys::PageDown, TEXT("Bild runter") },
		{ EKeys::Up, TEXT("Pfeil hoch") },
		{ EKeys::Down, TEXT("Pfeil runter") },
		{ EKeys::Left, TEXT("Pfeil links") },
		{ EKeys::Right, TEXT("Pfeil rechts") },
	};
	if (const FString* Name = Names.Find(Key))
	{
		return *Name;
	}
	return Key.IsValid() ? Key.GetDisplayName(false).ToString() : TEXT("–");
}
