// "Welt erstellen" pages of the start menu (decision 2026-11-16): page 1 game mode and difficulty, page 2
// "Erweiterte Einstellungen" with every world option by area. Part of AGamerHUD, kept in its own file.

#include "GamerHUD.h"

#include "GamerDataSubsystem.h"
#include "GamerDifficulty.h"
#include "GamerMenuGameMode.h"
#include "GamerPlayerController.h"
#include "Engine/Canvas.h"

namespace
{
	const FLinearColor WorldWhite(0.97f, 0.97f, 0.94f);
	const FLinearColor WorldDim(0.82f, 0.82f, 0.80f);
	const FLinearColor WorldOff(0.55f, 0.55f, 0.53f);
	const FLinearColor WorldHint(1.00f, 0.88f, 0.45f);

	FString FactorText(float Value)
	{
		return TEXT("×") + FString::SanitizeFloat(Value, 1).Replace(TEXT("."), TEXT(","));
	}
	FString OnOff(bool bOn) { return bOn ? TEXT("an") : TEXT("aus"); }
}

void AGamerHUD::DrawWorldMenu(const AGamerPlayerController& PC)
{
	const float Width = Canvas->ClipX;
	const float Height = Canvas->ClipY;
	DrawRect(FLinearColor(0.06f, 0.07f, 0.08f, 1.f), 0.f, 0.f, Width, Height);

	const FGamerWorldOptions& Options = UGamerMenuSettings::Get()->WorldOptions;
	const UGamerDataSubsystem* Data = UGamerDataSubsystem::Get(this);
	const TArray<FGamerDifficultyPreset> NoPresets;
	const TArray<FGamerDifficultyPreset>& Presets = Data ? Data->GetDifficulties() : NoPresets;
	const FString LevelName = GamerData::DifficultyName(Presets, Options.Difficulty);

	if (PC.GetMenuPage() == EGamerMenuPage::CreateWorld)
	{
		// Page 1: game mode, difficulty, start.
		const float S = Ui;
		const float ButtonH = LineHeight(S) * 1.5f;
		const float BlockW = FMath::Min(Width * 0.86f, TextWidth(TEXT("M"), S) * 46.f);
		const float X = (Width - BlockW) * 0.5f;
		const float LabelW = FMath::Max(TextWidth(TEXT("Schwierigkeit"), S), TextWidth(TEXT("Spielmodus"), S)) + 24.f * S;
		const float ValueW = FMath::Max(TextWidth(TEXT("Benutzerdefiniert"), S), TextWidth(TEXT("Freies Spiel"), S)) + 60.f * S;
		auto Wrapped = [&](const FString& Text, float& Y, const FLinearColor& Color, float Scale)
		{
			for (const FString& Line : WrapText(Text, BlockW, S * Scale))
			{
				DrawShadowText(Line, X, Y, Color, S * Scale);
				Y += LineHeight(S * Scale);
			}
		};

		float Y = Height * 0.08f;
		DrawCentered(TEXT("Welt erstellen"), Y, WorldWhite, 1.4f);
		Y += LineHeight(S * 1.4f) * 1.5f;

		const float TextH = LineHeight(S) / 1.12f;
		DrawShadowText(TEXT("Spielmodus"), X, Y + (ButtonH - TextH) * 0.5f, WorldWhite, S);
		DrawButton(TEXT("opt:mode"), Options.PlayMode == EGamerPlayMode::Saga ? TEXT("Saga") : TEXT("Freies Spiel"), X + LabelW, Y, ValueW, ButtonH, S);
		Y += ButtonH + 4.f * S;
		Wrapped(Options.PlayMode == EGamerPlayMode::Saga
			? TEXT("Die Hauptgeschichte ist der rote Faden. (Die Saga-Inhalte gibt es noch nicht; die Wahl wird mit der Welt gespeichert.)")
			: TEXT("Ohne Hauptgeschichte: Nebenaufgaben, Magie und Erkunden bleiben, alle Optionen frei kombinierbar."), Y, WorldDim, 0.7f);
		Y += 10.f * S;

		DrawShadowText(TEXT("Schwierigkeit"), X, Y + (ButtonH - TextH) * 0.5f, WorldWhite, S);
		DrawButton(TEXT("opt:difficulty"), LevelName, X + LabelW, Y, ValueW, ButtonH, S);
		Y += ButtonH + 4.f * S;
		if (const FGamerDifficultyPreset* Preset = GamerData::FindDifficulty(Presets, Options.Difficulty))
		{
			Wrapped(Preset->Description, Y, WorldDim, 0.7f);
			Wrapped(Preset->Summary(), Y, WorldDim, 0.62f);
		}
		else
		{
			Wrapped(TEXT("Eigene Zusammenstellung aus den Erweiterten Einstellungen."), Y, WorldDim, 0.7f);
		}
		Wrapped(TEXT("Die Schwierigkeit lässt sich nach dem Erstellen nicht mehr ändern."), Y, WorldHint, 0.62f);
		Y += 12.f * S;

		DrawCentered(TEXT("Du bist Gastgeber. Mitspieler treten über deine IP-Adresse bei (Port 7777)."), Y, WorldDim, 0.65f);
		Y += LineHeight(S * 0.65f) * 1.3f;
		DrawCentered(FString::Printf(TEXT("Wird gespeichert als „%s“ (später unter „Welt laden“)."), *AGamerPlayerController::MakeNewWorldName()), Y, WorldDim, 0.65f);
		Y += LineHeight(S * 0.65f) * 1.8f;

		const FString StartText = TEXT("Welt starten");
		const FString AdvancedText = TEXT("Erweiterte Einstellungen");
		const FString BackText = TEXT("Zurück");
		const float StartW = TextWidth(StartText, S) + 50.f * S;
		const float AdvancedW = TextWidth(AdvancedText, S) + 40.f * S;
		const float BackW = TextWidth(BackText, S) + 50.f * S;
		float ButtonX = (Width - (StartW + AdvancedW + BackW + 40.f * S)) * 0.5f;
		DrawButton(TEXT("start"), StartText, ButtonX, Y, StartW, ButtonH, S, true);
		ButtonX += StartW + 20.f * S;
		DrawButton(TEXT("opt:advanced"), AdvancedText, ButtonX, Y, AdvancedW, ButtonH, S);
		ButtonX += AdvancedW + 20.f * S;
		DrawButton(TEXT("back"), BackText, ButtonX, Y, BackW, ButtonH, S);
		Y += ButtonH + 10.f * S;
		if (!PC.GetMenuNote().IsEmpty())
		{
			DrawCentered(PC.GetMenuNote(), Y, WorldHint, 0.8f);
		}
		return;
	}

	// Page 2: every option, two columns of areas. Button = click for the next value; no button = "kommt später".
	struct FRow
	{
		FString Label;
		FString Value;
		FName Id; // NAME_None = not built yet
	};
	struct FArea
	{
		FString Title;
		TArray<FRow> Rows;
	};
	const TCHAR* Flow = Options.BuildFlow == EGamerBuildFlow::Blueprint ? TEXT("Bauplan + Hammer")
		: Options.BuildFlow == EGamerBuildFlow::Instant ? TEXT("Sofort fertig") : TEXT("Material beim Bauen");
	const TCHAR* Tear = Options.Demolish == EGamerDemolish::FullRefund ? TEXT("Volles Material")
		: Options.Demolish == EGamerDemolish::HalfRefund ? TEXT("Halbes Material")
		: Options.Demolish == EGamerDemolish::PartialRefund ? TEXT("Etwa 80 % Material") : TEXT("Aus");
	const TCHAR* Death = Options.DeathMode == EGamerDeathMode::None ? TEXT("Nichts verlieren")
		: Options.DeathMode == EGamerDeathMode::Hardcore ? TEXT("Alles verlieren") : TEXT("Grab mit Inventar");
	const TCHAR* Tempo = Options.GespuerTempo == EGamerGespuerTempo::Slow ? TEXT("Langsam (×0,6)")
		: Options.GespuerTempo == EGamerGespuerTempo::Fast ? TEXT("Schnell (×1,6)") : TEXT("Normal (×1)");
	const TArray<FArea> Columns[2] = {
		{
			{ TEXT("Bauen"), {
				{ TEXT("Bauablauf"), Flow, TEXT("opt:flow") },
				{ TEXT("Platzierung"), Options.Placement == EGamerPlacement::FreeSnap ? TEXT("Frei mit Einrasten") : TEXT("Festes Raster"), TEXT("opt:place") },
				{ TEXT("Abreißen"), Tear, TEXT("opt:demolish") },
				{ TEXT("Freies Bauen"), OnOff(Options.bFreeBuild), NAME_None } } },
			{ TEXT("Überleben"), {
				{ TEXT("Tod"), Death, TEXT("opt:death") },
				{ TEXT("Inventar"), Options.bCarryInventory ? TEXT("Mit dem Charakter") : TEXT("Pro Welt"), TEXT("opt:inventory") },
				{ TEXT("Hunger/Durst/Erschöpfung"), FactorText(Options.NeedsRate), TEXT("opt:needs") },
				{ TEXT("Erlittener Schaden"), FactorText(Options.DamageTaken), TEXT("opt:damage") },
				{ TEXT("Ertrag beim Abbauen"), FactorText(Options.YieldFactor), TEXT("opt:yield") } } },
		},
		{
			{ TEXT("Welt"), {
				{ TEXT("Jahreszeit dauert"), FString::Printf(TEXT("%g Min."), Options.SeasonMinutes), TEXT("opt:season") },
				{ TEXT("Gespür-Tempo"), Tempo, TEXT("opt:gespuer") },
				{ TEXT("Horden"), OnOff(Options.bHordes), NAME_None } } },
			{ TEXT("Dorf"), {
				{ TEXT("Bewohner unsterblich"), Options.bVillagersImmortal ? TEXT("ja") : TEXT("nein"), NAME_None } } },
			{ TEXT("Koop"), {
				{ TEXT("Friendly Fire"), OnOff(Options.bFriendlyFire), NAME_None },
				{ TEXT("Höchstzahl Spieler"), FString::FromInt(Options.MaxPlayers), TEXT("opt:players") } } },
		},
	};

	auto Layout = [&](float S, bool bDraw) -> float
	{
		const float RowH = LineHeight(S * 0.8f) * 1.25f;
		const float ColGap = 50.f * S;
		float LabelW = 0.f, ValueW = TextWidth(TEXT("Material beim Bauen"), S * 0.8f);
		for (const TArray<FArea>& Column : Columns)
		{
			for (const FArea& Area : Column)
			{
				for (const FRow& Row : Area.Rows)
				{
					LabelW = FMath::Max(LabelW, TextWidth(Row.Label, S * 0.8f));
					ValueW = FMath::Max(ValueW, TextWidth(Row.Value + TEXT("  (kommt später)"), S * 0.8f));
				}
			}
		}
		ValueW += 24.f * S;
		const float ColW = LabelW + 16.f * S + ValueW;
		const float TotalW = 2.f * ColW + ColGap;
		if (!bDraw && TotalW > Width * 0.96f)
		{
			return Height * TotalW / (Width * 0.96f) + 1.f; // too wide: let the caller shrink by the width ratio
		}
		const float Left = (Width - TotalW) * 0.5f;

		float Y = Height * 0.04f;
		if (bDraw)
		{
			DrawCentered(TEXT("Erweiterte Einstellungen"), Y, WorldWhite, S / Ui * 1.3f);
		}
		Y += LineHeight(S * 1.3f) * 1.3f;
		const FString Help = FString::Printf(TEXT("Schwierigkeit: %s. Wert anklicken für den nächsten; weicht etwas von der Stufe ab, gilt „Benutzerdefiniert“."), *LevelName);
		if (bDraw)
		{
			DrawCentered(Help, Y, WorldDim, S / Ui * 0.62f);
		}
		Y += LineHeight(S * 0.62f) * 1.7f;

		float Bottom = Y;
		for (int32 Col = 0; Col < 2; ++Col)
		{
			const float X = Left + Col * (ColW + ColGap);
			float CY = Y;
			for (const FArea& Area : Columns[Col])
			{
				if (bDraw)
				{
					DrawShadowText(Area.Title, X, CY, WorldHint, S * 0.85f);
				}
				CY += LineHeight(S * 0.85f) * 1.1f;
				for (const FRow& Row : Area.Rows)
				{
					if (bDraw)
					{
						const float TextH = LineHeight(S * 0.8f) / 1.12f;
						DrawShadowText(Row.Label, X, CY + (RowH - TextH) * 0.5f, Row.Id.IsNone() ? WorldOff : WorldWhite, S * 0.8f);
						const float VX = X + LabelW + 16.f * S;
						if (Row.Id.IsNone())
						{
							DrawShadowText(Row.Value + TEXT("  (kommt später)"), VX + 12.f * S, CY + (RowH - TextH) * 0.5f, WorldOff, S * 0.8f);
						}
						else
						{
							DrawButton(Row.Id, Row.Value, VX, CY, ValueW, RowH, S * 0.8f);
						}
					}
					CY += RowH + 4.f * S;
				}
				CY += 8.f * S;
			}
			Bottom = FMath::Max(Bottom, CY);
		}
		Y = Bottom + 6.f * S;
		const float ButtonH = LineHeight(S) * 1.4f;
		const FString DoneText = TEXT("Fertig");
		const float DoneW = TextWidth(DoneText, S) + 60.f * S;
		if (bDraw)
		{
			DrawButton(TEXT("opt:advdone"), DoneText, (Width - DoneW) * 0.5f, Y, DoneW, ButtonH, S, true);
		}
		Y += ButtonH + 8.f * S;
		if (bDraw && !PC.GetMenuNote().IsEmpty())
		{
			DrawCentered(PC.GetMenuNote(), Y, WorldHint, S / Ui * 0.75f);
		}
		return Y + LineHeight(S * 0.75f) + Height * 0.02f;
	};

	// Shrink until it fits (width and height).
	float S = Ui;
	for (int32 Pass = 0; Pass < 4; ++Pass)
	{
		const float Needed = Layout(S, false);
		if (Needed <= Height)
		{
			break;
		}
		S *= FMath::Max(0.5f, Height / Needed * 0.98f);
	}
	Layout(S, true);
}
