// "Welt erstellen" (decision 2026-11-16): page 1 = game mode and difficulty, page 2 = advanced settings (all world
// options by area). Part of AGamerPlayerController, kept in its own file. Options are remembered in UGamerMenuSettings
// and sent to the server in the map URL (HostWorld); the server enforces the preset values from its own data.

#include "GamerPlayerController.h"

#include "Gamer.h"
#include "GamerDataSubsystem.h"
#include "GamerDifficulty.h"
#include "GamerMenuGameMode.h"

namespace
{
	/** Next value of a fixed list (wraps; a value not in the list goes to the first one bigger than it). */
	float NextStep(float Current, std::initializer_list<float> Steps)
	{
		for (const float Step : Steps)
		{
			if (Step > Current + 0.001f)
			{
				return Step;
			}
		}
		return *Steps.begin();
	}
}

void AGamerPlayerController::HandleWorldOptionButton(const FString& Id)
{
	UGamerMenuSettings* Settings = UGamerMenuSettings::Get();
	FGamerWorldOptions& Options = Settings->WorldOptions;
	const UGamerDataSubsystem* Data = UGamerDataSubsystem::Get(this);
	const TArray<FGamerDifficultyPreset> NoPresets;
	const TArray<FGamerDifficultyPreset>& Presets = Data ? Data->GetDifficulties() : NoPresets;
	const FString Which = Id.Mid(4);
	MenuNote.Reset();

	// Page changes.
	if (Which == TEXT("advanced"))
	{
		MenuPage = EGamerMenuPage::WorldAdvanced;
		return;
	}
	if (Which == TEXT("advdone"))
	{
		MenuPage = EGamerMenuPage::CreateWorld;
		return;
	}

	// Page 1: mode and difficulty.
	if (Which == TEXT("mode"))
	{
		Options.PlayMode = Options.PlayMode == EGamerPlayMode::Saga ? EGamerPlayMode::Free : EGamerPlayMode::Saga;
	}
	else if (Which == TEXT("difficulty") || Which.StartsWith(TEXT("difficulty=")))
	{
		// Click: next preset (from "Benutzerdefiniert" back to the first). Tests: opt:difficulty=hard.
		EGamerDifficulty Next = EGamerDifficulty::Easy;
		if (Which.StartsWith(TEXT("difficulty=")))
		{
			const FString Level = Which.Mid(11);
			Next = Level == TEXT("medium") ? EGamerDifficulty::Medium : Level == TEXT("hard") ? EGamerDifficulty::Hard
				: Level == TEXT("hardcore") ? EGamerDifficulty::Hardcore : EGamerDifficulty::Easy;
		}
		else if (Options.Difficulty < EGamerDifficulty::Hardcore)
		{
			Next = static_cast<EGamerDifficulty>(static_cast<uint8>(Options.Difficulty) + 1);
		}
		if (const FGamerDifficultyPreset* Preset = GamerData::FindDifficulty(Presets, Next))
		{
			Preset->ApplyTo(Options);
		}
		else
		{
			MenuNote = TEXT("Schwierigkeitsdaten fehlen (difficulty.json).");
		}
	}
	// Page 2: every world option. Options without effect yet ("kommt später") have no button and cannot be changed here.
	else if (Which == TEXT("flow"))
	{
		Options.BuildFlow = static_cast<EGamerBuildFlow>((static_cast<uint8>(Options.BuildFlow) + 1) % 3);
	}
	else if (Which == TEXT("place"))
	{
		Options.Placement = static_cast<EGamerPlacement>((static_cast<uint8>(Options.Placement) + 1) % 2);
	}
	else if (Which == TEXT("demolish"))
	{
		// Order in the menu: full -> about 80 % -> half -> off -> full (the enum keeps its saved values, see EGamerDemolish)
		static const EGamerDemolish Order[] = { EGamerDemolish::FullRefund, EGamerDemolish::PartialRefund, EGamerDemolish::HalfRefund, EGamerDemolish::Disabled };
		int32 Index = 0;
		for (int32 i = 0; i < UE_ARRAY_COUNT(Order); ++i) { if (Order[i] == Options.Demolish) { Index = i; } }
		Options.Demolish = Order[(Index + 1) % UE_ARRAY_COUNT(Order)];
	}
	else if (Which == TEXT("inventory"))
	{
		Options.bCarryInventory = !Options.bCarryInventory;
	}
	else if (Which == TEXT("death"))
	{
		Options.DeathMode = static_cast<EGamerDeathMode>((static_cast<uint8>(Options.DeathMode) + 1) % 3);
	}
	else if (Which == TEXT("gespuer"))
	{
		Options.GespuerTempo = static_cast<EGamerGespuerTempo>((static_cast<uint8>(Options.GespuerTempo) + 1) % 3);
	}
	else if (Which == TEXT("needs"))
	{
		Options.NeedsRate = NextStep(Options.NeedsRate, { 0.5f, 0.7f, 1.f, 1.3f, 1.6f, 2.f });
	}
	else if (Which == TEXT("damage"))
	{
		Options.DamageTaken = NextStep(Options.DamageTaken, { 0.6f, 1.f, 1.4f, 1.8f, 2.5f });
	}
	else if (Which == TEXT("yield"))
	{
		Options.YieldFactor = NextStep(Options.YieldFactor, { 0.5f, 0.7f, 0.8f, 1.f, 1.5f, 2.f });
	}
	else if (Which == TEXT("season"))
	{
		Options.SeasonMinutes = NextStep(Options.SeasonMinutes, { 15.f, 30.f, 60.f, 90.f, 120.f });
	}
	else if (Which == TEXT("players"))
	{
		Options.MaxPlayers = Options.MaxPlayers >= 8 ? 1 : Options.MaxPlayers + 1;
	}
	else
	{
		return; // unknown button
	}

	// A change on page 2 that leaves the preset makes the world "Benutzerdefiniert"; back on a preset's values = that preset.
	if (Which != TEXT("mode") && !Which.StartsWith(TEXT("difficulty")))
	{
		const FGamerDifficultyPreset* Current = GamerData::FindDifficulty(Presets, Options.Difficulty);
		if (!Current || !Current->Matches(Options))
		{
			const FGamerDifficultyPreset* Matching = Presets.FindByPredicate([&Options](const FGamerDifficultyPreset& P) { return P.Matches(Options); });
			Options.Difficulty = Matching ? Matching->Level : EGamerDifficulty::Custom;
		}
	}
	Settings->Save();
	UE_LOG(LogGamer, Log, TEXT("Start menu world options: %s"), *Options.ToLogString());
}
