// HUD part of hit points and death (decisions 2026-11-16 "Spielertod" and "Heilung"): the hit point bar, a short flash when hurt,
// the hint at a grave and the death screen. Own file so GamerHUD.cpp stays small. Style stage A: palette colours; Rostrot only as
// a surface (bar, flash), never as a text colour (reading rules of 2026-11-13).
#include "GamerHUD.h"

#include "GamerCharacter.h"
#include "GamerGameState.h"
#include "GamerGrave.h"
#include "GamerHealthComponent.h"
#include "GamerStaminaComponent.h"
#include "GamerKeyBindings.h"
#include "Engine/Canvas.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "GameFramework/PlayerController.h"

namespace
{
	FLinearColor HexColor(uint32 Rgb, float Alpha = 1.f)
	{
		auto Channel = [](uint32 Value) { return FMath::Pow(static_cast<float>(Value) / 255.f, 2.2f); };
		return FLinearColor(Channel((Rgb >> 16) & 0xFF), Channel((Rgb >> 8) & 0xFF), Channel(Rgb & 0xFF), Alpha);
	}
}

void AGamerHUD::DrawHealth(const AGamerCharacter* Character)
{
	const UGamerHealthComponent* Health = Character ? Character->GetHealth() : nullptr;
	if (!Health || !Canvas)
	{
		return;
	}
	const float Width = Canvas->ClipX;
	const float Height = Canvas->ClipY;
	const float Margin = 10.f * Ui;
	const double Now = GetWorld()->GetTimeSeconds();
	const FLinearColor TextWhiteColor(0.97f, 0.97f, 0.94f);
	const FLinearColor TextDimColor(0.82f, 0.82f, 0.80f);

	// --- Death screen ---
	if (Health->IsDead())
	{
		DrawRect(FLinearColor(0.02f, 0.02f, 0.03f, 0.78f), 0.f, 0.f, Width, Height);
		const FString Title = TEXT("Du bist gefallen");
		DrawShadowText(Title, (Width - TextWidth(Title, Ui * 2.0f)) * 0.5f, Height * 0.30f, TextWhiteColor, Ui * 2.0f);
		const EGamerDeathMode Mode = AGamerGameState::Get(this).DeathMode;
		const FString Info = Mode == EGamerDeathMode::None ? TEXT("Du verlierst nichts und stehst gleich wieder auf.")
			: Mode == EGamerDeathMode::Hardcore ? TEXT("Alles, was du bei dir hattest, ist verloren.")
			: TEXT("Dein Inventar liegt in einem Grab, wo du gefallen bist. Hol es dir ab.");
		DrawShadowText(Info, (Width - TextWidth(Info, Ui * 0.95f)) * 0.5f, Height * 0.30f + LineHeight(Ui * 2.0f) * 1.2f, TextDimColor, Ui * 0.95f);

		// The "Aufstehen" button (the interact key; mouse buttons would need the menu cursor, kept simple here).
		const float Wait = Health->GetRespawnWait();
		const FString Label = Wait > 0.05f ? FString::Printf(TEXT("Aufstehen (in %.0f s)"), FMath::CeilToFloat(Wait))
			: FString::Printf(TEXT("Aufstehen: %s"), *UGamerKeyBindings::ActionKeyName(GamerInputActions::Interact));
		const float ButtonW = TextWidth(Label, Ui * 1.1f) + 60.f * Ui;
		const float ButtonH = LineHeight(Ui * 1.1f) * 1.5f;
		const float ButtonX = (Width - ButtonW) * 0.5f;
		const float ButtonY = Height * 0.55f;
		DrawRect(Wait > 0.05f ? HexColor(0x2A302D, 0.92f) : HexColor(0x8A5A24, 0.95f), ButtonX, ButtonY, ButtonW, ButtonH);
		DrawRect(HexColor(0xA99B80, 0.75f), ButtonX, ButtonY, ButtonW, FMath::Max(1.f, Ui));
		DrawRect(HexColor(0xA99B80, 0.75f), ButtonX, ButtonY + ButtonH - FMath::Max(1.f, Ui), ButtonW, FMath::Max(1.f, Ui));
		DrawShadowText(Label, ButtonX + (ButtonW - TextWidth(Label, Ui * 1.1f)) * 0.5f, ButtonY + (ButtonH - LineHeight(Ui * 1.1f)) * 0.5f, TextWhiteColor, Ui * 1.1f);
		return;
	}

	// --- Downed (co-op): grey screen with a turquoise edge and the countdown; teammates can revive ---
	if (Health->IsDowned())
	{
		DrawRect(FLinearColor(0.25f, 0.25f, 0.26f, 0.55f), 0.f, 0.f, Width, Height);
		const int32 Bands = 10;
		const float Depth = Height * 0.08f;
		for (int32 Band = 0; Band < Bands; ++Band)
		{
			const float Step = Depth / Bands;
			const float Offset = Band * Step;
			const FLinearColor Color = HexColor(0x2FA8A0, 0.55f * FMath::Square(1.f - static_cast<float>(Band) / Bands));
			DrawRect(Color, 0.f, Offset, Width, Step);
			DrawRect(Color, 0.f, Height - Offset - Step, Width, Step);
			DrawRect(Color, Offset, 0.f, Step, Height);
			DrawRect(Color, Width - Offset - Step, 0.f, Step, Height);
		}
		const FString Title = TEXT("Du bist niedergeschlagen");
		DrawShadowText(Title, (Width - TextWidth(Title, Ui * 1.8f)) * 0.5f, Height * 0.30f, TextWhiteColor, Ui * 1.8f);
		const FString Info = FString::Printf(TEXT("Ein Mitspieler kann dich wiederbeleben - noch %.0f s"), FMath::CeilToFloat(FMath::Max(0.f, Health->GetDownedLeft())));
		DrawShadowText(Info, (Width - TextWidth(Info, Ui)) * 0.5f, Height * 0.30f + LineHeight(Ui * 1.8f) * 1.2f, TextDimColor, Ui);
		const float BarW2 = 220.f * Ui;
		const float BarX2 = (Width - BarW2) * 0.5f;
		const float BarY2 = Height * 0.30f + LineHeight(Ui * 1.8f) * 1.2f + LineHeight(Ui) * 1.6f;
		DrawRect(FLinearColor(0.f, 0.f, 0.f, 0.6f), BarX2 - 2.f, BarY2 - 2.f, BarW2 + 4.f, 10.f * Ui + 4.f);
		DrawRect(HexColor(0x2FA8A0, 0.9f), BarX2, BarY2, BarW2 * FMath::Clamp(Health->GetDownedLeft() / 15.f, 0.f, 1.f), 10.f * Ui);
		return;
	}

	// --- Reviving a teammate: progress bar at the crosshair; marker over downed teammates ---
	{
		APlayerController* PC = GetOwningPlayerController();
		for (TActorIterator<APawn> It(GetWorld()); It; ++It)
		{
			const UGamerHealthComponent* Other = It->FindComponentByClass<UGamerHealthComponent>();
			if (!Other || Other == Health || !Other->IsDowned())
			{
				continue;
			}
			FVector2D Screen;
			if (PC && PC->ProjectWorldLocationToScreen(It->GetActorLocation() + FVector(0.f, 0.f, 110.f), Screen, true))
			{
				const FString Mark = FString::Printf(TEXT("Niedergeschlagen %.0f s"), FMath::CeilToFloat(FMath::Max(0.f, Other->GetDownedLeft())));
				DrawShadowText(Mark, Screen.X - TextWidth(Mark, Ui * 0.8f) * 0.5f, Screen.Y, FLinearColor(0.55f, 0.9f, 0.88f), Ui * 0.8f);
			}
		}
		if (const AActor* Downed = Health->FindDownedNearby())
		{
			const float Progress = Health->GetReviveProgress();
			const FString Hint = Progress > 0.f ? FString(TEXT("Wiederbeleben ...")) : FString::Printf(TEXT("%s halten: Mitspieler wiederbeleben"), *UGamerKeyBindings::ActionKeyName(GamerInputActions::Interact));
			DrawShadowText(Hint, (Width - TextWidth(Hint, Ui)) * 0.5f, Height * 0.5f + LineHeight(Ui) * 3.4f, FLinearColor(0.55f, 0.9f, 0.88f), Ui);
			const float W = 160.f * Ui;
			const float X = (Width - W) * 0.5f;
			const float Y = Height * 0.5f + LineHeight(Ui) * 4.6f;
			DrawRect(FLinearColor(0.f, 0.f, 0.f, 0.6f), X - 2.f, Y - 2.f, W + 4.f, 10.f * Ui + 4.f);
			DrawRect(FLinearColor(0.25f, 0.25f, 0.25f, 0.9f), X, Y, W, 10.f * Ui);
			DrawRect(HexColor(0x2FA8A0, 0.95f), X, Y, W * Progress, 10.f * Ui);
		}
	}

	// --- Grave marker for the owner (name and distance, also when far away) ---
	for (TActorIterator<AGamerGrave> It(GetWorld()); It; ++It)
	{
		if (!Health->IsMyGrave(*It))
		{
			continue;
		}
		APlayerController* PC = GetOwningPlayerController();
		FVector2D Screen;
		const float Meters = FVector::Dist(It->GetActorLocation(), Character->GetActorLocation()) / 100.f;
		if (PC && PC->ProjectWorldLocationToScreen(It->GetActorLocation() + FVector(0.f, 0.f, 160.f), Screen, true)
			&& Screen.X > 0 && Screen.Y > 0 && Screen.X < Width && Screen.Y < Height)
		{
			const FString Mark = FString::Printf(TEXT("Mein Grab  %.0f m"), Meters);
			DrawShadowText(Mark, Screen.X - TextWidth(Mark, Ui * 0.85f) * 0.5f, Screen.Y, FLinearColor(0.92f, 0.85f, 0.66f), Ui * 0.85f);
		}
	}

	// --- Hit point bar (bottom left, below the "Satt" display) ---
	const float Box = 18.f * Ui;
	const float BarW = 90.f * Ui;
	const float BarY = Height - 80.f * Ui;
	const float Fraction = Health->GetFraction();
	const bool bLow = Fraction < 0.3f;
	const float Pulse = bLow ? 0.75f + 0.25f * FMath::Sin(static_cast<float>(Now) * 6.f) : 1.f;
	DrawRect(FLinearColor(0.f, 0.f, 0.f, 0.7f), Margin - 1.f, BarY - 1.f, Box + 2.f, Box + 2.f);
	DrawRect(HexColor(0x904C3E, Pulse), Margin, BarY, Box, Box);   // Rostrot as a surface
	DrawShadowText(TEXT("L"), Margin + Box * 0.28f, BarY + Box * 0.05f, FLinearColor(0.97f, 0.93f, 0.88f), Ui * 0.7f);
	const float BarX = Margin + Box + 8.f * Ui;
	DrawRect(FLinearColor(0.f, 0.f, 0.f, 0.7f), BarX - 2.f, BarY + Box * 0.2f - 2.f, BarW + 4.f, Box * 0.6f + 4.f);
	DrawRect(FLinearColor(0.25f, 0.25f, 0.25f, 0.9f), BarX, BarY + Box * 0.2f, BarW, Box * 0.6f);
	DrawRect(HexColor(0xB8604B, Pulse), BarX, BarY + Box * 0.2f, BarW * Fraction, Box * 0.6f);
	const FString Numbers = FString::Printf(TEXT("%d / %d"), FMath::CeilToInt(Health->GetHP()), FMath::RoundToInt(Health->GetMaxHP()));
	DrawShadowText(Numbers, BarX + BarW + 6.f * Ui, BarY, TextWhiteColor, Ui * 0.75f);

	// --- Stamina bar next to the hit points, exhaustion as a thin bar below; the dark end of the bar is what exhaustion took away ---
	if (const UGamerStaminaComponent* StaminaComponent = Character->GetStamina())
	{
		const float SX = BarX + BarW + 64.f * Ui;
		const float BaseMax = FMath::Max(1.f, StaminaComponent->GetBaseMax());
		const float Max = StaminaComponent->GetMaxStamina();
		const float SW = 90.f * Ui;
		const float SY = BarY + Box * 0.2f;
		const FLinearColor Surface = StaminaComponent->IsEmpty() ? HexColor(0x8C7A4A, 0.75f) : HexColor(0xC8A85A, 1.f);   // ocker; dull when empty
		DrawRect(FLinearColor(0.f, 0.f, 0.f, 0.7f), SX - 2.f, SY - 2.f, SW + 4.f, Box * 0.6f + 4.f);
		DrawRect(FLinearColor(0.25f, 0.25f, 0.25f, 0.9f), SX, SY, SW, Box * 0.6f);
		DrawRect(Surface, SX, SY, SW * FMath::Clamp(StaminaComponent->GetStamina() / BaseMax, 0.f, 1.f), Box * 0.6f);
		DrawRect(FLinearColor(0.f, 0.f, 0.f, 0.75f), SX + SW * (Max / BaseMax), SY, SW * (1.f - Max / BaseMax), Box * 0.6f);
		DrawShadowText(TEXT("A"), SX - 14.f * Ui, BarY + Box * 0.05f, TextDimColor, Ui * 0.7f);
		const float Tired = StaminaComponent->GetExhaustion();
		DrawRect(FLinearColor(0.f, 0.f, 0.f, 0.6f), SX - 1.f, SY + Box * 0.6f + 5.f, SW + 2.f, 5.f * Ui + 2.f);
		DrawRect(HexColor(0x6B7F8F, 0.95f), SX, SY + Box * 0.6f + 6.f, SW * Tired, 5.f * Ui);
		if (Health->IsNearOwnBed())
		{
			const FString Hint = FString::Printf(TEXT("%s: %s"), *UGamerKeyBindings::ActionKeyName(GamerInputActions::Interact),
				StaminaComponent->CanSleepNow() ? TEXT("schlafen") : TEXT("Bett (nicht müde genug zum Schlafen)"));
			DrawShadowText(Hint, (Width - TextWidth(Hint, Ui)) * 0.5f, Height * 0.5f + LineHeight(Ui) * 2.4f, FLinearColor(0.92f, 0.85f, 0.66f), Ui);
		}
	}

	// --- Flash when hurt (a rostrot glow at the screen edge, fades in about a third of a second) ---
	static float LastHP = -1.f;
	static double FlashUntil = 0.0;
	if (LastHP >= 0.f && Health->GetHP() < LastHP - 0.2f)
	{
		FlashUntil = Now + 0.35;
	}
	LastHP = Health->GetHP();
	if (Now < FlashUntil)
	{
		const float Alpha = static_cast<float>((FlashUntil - Now) / 0.35) * 0.35f;
		const int32 Bands = 8;
		const float Depth = Height * 0.10f;
		for (int32 Band = 0; Band < Bands; ++Band)
		{
			const float Step = Depth / Bands;
			const float Offset = Band * Step;
			const FLinearColor Color = HexColor(0x904C3E, Alpha * FMath::Square(1.f - static_cast<float>(Band) / Bands));
			DrawRect(Color, 0.f, Offset, Width, Step);
			DrawRect(Color, 0.f, Height - Offset - Step, Width, Step);
			DrawRect(Color, Offset, 0.f, Step, Height);
			DrawRect(Color, Width - Offset - Step, 0.f, Step, Height);
		}
	}

	// --- Hint at a grave within reach ---
	if (const AGamerGrave* Grave = Health->FindNearbyGrave())
	{
		const FString Line = Health->MayOpenGrave(Grave)
			? FString::Printf(TEXT("%s: Grab von %s öffnen (%d Teile)"), *UGamerKeyBindings::ActionKeyName(GamerInputActions::Interact), *Grave->GetOwnerName(), Grave->GetItemCount())
			: FString::Printf(TEXT("Grab von %s – noch geschützt"), *Grave->GetOwnerName());
		DrawShadowText(Line, (Width - TextWidth(Line, Ui)) * 0.5f, Height * 0.5f + LineHeight(Ui) * 2.4f, FLinearColor(0.92f, 0.85f, 0.66f), Ui);
	}
}
