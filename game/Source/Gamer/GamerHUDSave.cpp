// Start menu page "Welt laden" (save system). Part of AGamerHUD, in its own file like GamerHUDCharacter.cpp.

#include "GamerHUD.h"

#include "GamerPlayerController.h"
#include "GamerSaveSubsystem.h"
#include "Engine/Canvas.h"

namespace
{
	const FLinearColor SaveWhite(0.97f, 0.97f, 0.94f);
	const FLinearColor SaveDim(0.82f, 0.82f, 0.80f);
	const FLinearColor SaveHint(1.00f, 0.88f, 0.45f);

	/** "15.11.2026 14:32" in local time (files keep UTC). */
	FString LocalTimeText(const FDateTime& Utc)
	{
		if (Utc.GetTicks() <= 0)
		{
			return TEXT("?");
		}
		const FDateTime Local = Utc + (FDateTime::Now() - FDateTime::UtcNow());
		return Local.ToString(TEXT("%d.%m.%Y %H:%M"));
	}
}

void AGamerHUD::DrawLoadWorldMenu(const AGamerPlayerController& PC)
{
	const float Width = Canvas->ClipX;
	const float Height = Canvas->ClipY;
	const float S = Ui;
	const float ButtonH = LineHeight(S) * 1.5f;
	DrawRect(FLinearColor(0.06f, 0.07f, 0.08f, 1.f), 0.f, 0.f, Width, Height);

	float Y = Height * 0.10f;
	DrawCentered(TEXT("Welt laden"), Y, SaveWhite, 1.4f);
	Y += LineHeight(S * 1.4f) * 1.3f;

	const TArray<FGamerWorldSaveInfo>& Worlds = PC.GetSavedWorlds();
	if (Worlds.Num() == 0)
	{
		DrawCentered(TEXT("Noch keine gespeicherte Welt. Über „Welt erstellen“ entsteht die erste."), Y, SaveDim, 0.85f);
		Y += LineHeight(S * 0.85f) * 2.f;
	}
	else
	{
		DrawCentered(TEXT("Welt anklicken, um sie als Gastgeber zu starten. Mitspieler treten wie gewohnt bei."), Y, SaveDim, 0.7f);
		Y += LineHeight(S * 0.7f) * 1.8f;
		const int32 Shown = FMath::Min(Worlds.Num(), PC.GetSavedWorldsShown());
		float ButtonW = TextWidth(TEXT("MMMMMMMMMMMMMMMMMMMMMMMM"), S);
		for (int32 Index = 0; Index < Shown; ++Index)
		{
			ButtonW = FMath::Max(ButtonW, TextWidth(Worlds[Index].Name, S) + 40.f * S);
		}
		ButtonW = FMath::Min(ButtonW, Width * 0.8f);
		const float X = (Width - ButtonW) * 0.5f;
		for (int32 Index = 0; Index < Shown; ++Index)
		{
			const FGamerWorldSaveInfo& Info = Worlds[Index];
			DrawButton(FName(*(TEXT("load:") + Info.WorldId)), Info.Name, X, Y, ButtonW, ButtonH, S);
			Y += ButtonH + 2.f * S;
			const FString Line = FString::Printf(TEXT("Gespeichert %s · Spielzeit %s"), *LocalTimeText(Info.Saved), *Info.DescribePlayTime());
			DrawShadowText(Line, X, Y, SaveDim, S * 0.7f);
			Y += LineHeight(S * 0.7f) * 1.05f;
			if (!Info.OptionsText.IsEmpty())
			{
				// Wrapped: with mode and difficulty (2026-11-16) the line is wider than small windows.
				for (const FString& OptionsLine : WrapText(Info.OptionsText, Width - X - 16.f * S, S * 0.6f))
				{
					DrawShadowText(OptionsLine, X, Y, SaveDim, S * 0.6f);
					Y += LineHeight(S * 0.6f) * 1.05f;
				}
			}
			Y += 8.f * S;
		}
		if (Worlds.Num() > Shown)
		{
			DrawCentered(FString::Printf(TEXT("… und %d ältere"), Worlds.Num() - Shown), Y, SaveDim, 0.7f);
			Y += LineHeight(S * 0.7f) * 1.5f;
		}
	}
	const FString BackText = TEXT("Zurück");
	const float BackW = TextWidth(BackText, S) + 60.f * S;
	DrawButton(TEXT("back"), BackText, (Width - BackW) * 0.5f, Y, BackW, ButtonH, S);
	Y += ButtonH + 9.f * S;
	if (!PC.GetMenuNote().IsEmpty())
	{
		DrawCentered(PC.GetMenuNote(), Y, SaveHint, 0.8f);
	}
}
