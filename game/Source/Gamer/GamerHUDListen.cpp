// HUD part of listening for magic (decision 2026-11-15: "Umschauen und Lauschen getrennt"). Kept in its own file so the big
// GamerHUD.cpp is touched as little as possible. The silhouettes of the finds are drawn by the contour material (stencil 2),
// see UGamerListenComponent; here: the passive edge shimmer, the slow turquoise sine ring and the labels.
#include "GamerHUD.h"

#include "GamerCharacter.h"
#include "GamerKeyBindings.h"
#include "GamerListenComponent.h"
#include "Engine/Canvas.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"

namespace
{
	// Gezeitenlicht #A9E9DC (sRGB -> linear, simple gamma), the colour of the magic in the style guide.
	FLinearColor TideLight(float Alpha)
	{
		return FLinearColor(0.405f, 0.82f, 0.723f, Alpha);
	}
}

void AGamerHUD::DrawListen(const AGamerCharacter* Character)
{
	const UGamerListenComponent* Listen = Character ? Character->GetListen() : nullptr;
	if (!Listen || !Canvas)
	{
		return;
	}
	const float Width = Canvas->ClipX;
	const float Height = Canvas->ClipY;
	const double Now = GetWorld()->GetTimeSeconds();
	APlayerController* PC = GetOwningPlayerController();

	// Passive: the screen edge shimmers turquoise, stronger the nearer a magical thing is (hot/cold).
	if (const float Proximity = Listen->GetProximity(); Proximity > 0.01f)
	{
		const float Pulse = 0.75f + 0.25f * FMath::Sin(static_cast<float>(Now) * (2.f + 5.f * Proximity));
		const float Alpha = FMath::Pow(Proximity, 0.7f) * Listen->GetMaxEdgeAlpha() * Pulse;
		const int32 Bands = 10;
		const float Depth = Height * (0.05f + 0.12f * Proximity);   // closer = the glow reaches further in
		for (int32 Band = 0; Band < Bands; ++Band)
		{
			const float T = static_cast<float>(Band) / Bands;
			const FLinearColor Color = TideLight(Alpha * FMath::Pow(1.f - T, 1.6f));
			const float Step = Depth / Bands;
			const float Offset = Band * Step;
			DrawRect(Color, 0.f, Offset, Width, Step);                    // top
			DrawRect(Color, 0.f, Height - Offset - Step, Width, Step);    // bottom
			DrawRect(Color, Offset, 0.f, Step, Height);                   // left
			DrawRect(Color, Width - Offset - Step, 0.f, Step, Height);    // right
		}
	}

	if (!Listen->IsShowing() || !PC)
	{
		// Cooldown hint (small, only right after listening)
		if (const float Left = Listen->GetCooldownLeft(); Left > 0.f)
		{
			const FString Line = FString::Printf(TEXT("Lauschen %.0f s"), FMath::CeilToFloat(Left));
			DrawShadowText(Line, Width - 10.f * Ui - TextWidth(Line, Ui * 0.75f), Height * 0.20f - LineHeight(Ui * 0.75f) * 1.4f, FLinearColor(0.82f, 0.82f, 0.80f), Ui * 0.75f);
		}
		return;
	}

	// Active: a slow turquoise sine ring, open with three notches (the style guide's sign), gathering - spreading - fading.
	const float Age = Listen->GetAgeSeconds();
	const float WaveSeconds = Listen->GetWaveSeconds();
	const float WaveRadius = Listen->GetWaveRadiusCm();
	const float Fade = Listen->IsListening() ? 1.f : FMath::Clamp(1.f - (Age - WaveSeconds) / 2.f, 0.f, 1.f);
	if (WaveRadius > 60.f && (Listen->IsListening() || Fade > 0.f))
	{
		for (int32 Trail = 0; Trail < 5; ++Trail)
		{
			const float R = WaveRadius - Trail * 130.f;
			if (R <= 60.f)
			{
				continue;
			}
			const float Strength = FMath::Square(1.f - Trail / 5.f);
			const int32 Steps = 220;
			const float Dot = FMath::Max(3.f, (Trail == 0 ? 5.f : 4.f) * Ui);
			for (int32 Step = 0; Step < Steps; ++Step)
			{
				const float Fraction = static_cast<float>(Step) / Steps;
				if (FMath::Frac(Fraction * 3.f + 0.02f) < 0.08f)
				{
					continue;   // the three notches
				}
				const float Angle = 2.f * PI * Fraction + Age * 0.15f;
				const float Swing = 70.f * FMath::Sin(9.f * Angle - Age * 1.2f - Trail * 0.6f);   // slow sine
				const FVector Point = Listen->GetCenter() + FVector(FMath::Cos(Angle) * (R + Swing), FMath::Sin(Angle) * (R + Swing), -90.f);
				FVector2D ScreenPoint;
				if (PC->ProjectWorldLocationToScreen(Point, ScreenPoint) && ScreenPoint.X >= 0 && ScreenPoint.Y >= 0 && ScreenPoint.X <= Width && ScreenPoint.Y <= Height)
				{
					DrawRect(TideLight(0.40f * Strength * Fade), static_cast<float>(ScreenPoint.X) - Dot * 0.5f, static_cast<float>(ScreenPoint.Y) - Dot * 0.5f, Dot, Dot);
				}
			}
		}
	}

	// The finds: label by level (kind / direction from level 2, name and amount from level 3), arrows at the edge when off screen.
	const int32 Level = Listen->GetEffectiveLevel();
	const float Visible = Listen->GetVisibleSeconds();
	for (const FGamerListenFind& Find : Listen->GetFinds())
	{
		if (Find.RevealTime <= 0.0 || Level < 2)
		{
			continue;   // not reached yet; at level 1 only the silhouette shows, no label
		}
		const float Shown = static_cast<float>(Now - Find.RevealTime);
		if (Shown > Visible)
		{
			continue;   // faded
		}
		const float Alpha = FMath::Clamp((Visible - Shown) / 1.5f, 0.f, 1.f) * FMath::Clamp(Shown / 0.4f, 0.f, 1.f);
		FString Label = Find.Coarse.IsEmpty() ? Find.Kind : Find.Coarse;
		if (Level >= 3)
		{
			Label = Find.Name.IsEmpty() ? Label : Find.Name;
			if (Find.Amount > 1)
			{
				Label += FString::Printf(TEXT(" ×%d"), Find.Amount);
			}
		}
		if (Find.bHidden) { Label += TEXT(" (verborgen)"); }
		if (Find.bRare) { Label += TEXT(" (selten)"); }
		FVector2D Screen;
		const bool bOnScreen = PC->ProjectWorldLocationToScreen(Find.Location + FVector(0.f, 0.f, 120.f), Screen) && Screen.X >= 0 && Screen.Y >= 0 && Screen.X <= Width && Screen.Y <= Height;
		FLinearColor LabelColor = TideLight(Alpha);
		if (bOnScreen)
		{
			DrawShadowText(Label, static_cast<float>(Screen.X) - TextWidth(Label, Ui * 0.7f) * 0.5f, static_cast<float>(Screen.Y) - 18.f * Ui, LabelColor, Ui * 0.7f);
		}
		else
		{
			// Direction (level 2 up): a small turquoise block at the left or right screen edge, on the side where the find lies.
			const FRotator View = PC->PlayerCameraManager->GetCameraRotation();
			const float Right = FVector::DotProduct(Find.Location - PC->PlayerCameraManager->GetCameraLocation(), FRotationMatrix(View).GetUnitAxis(EAxis::Y));
			const float Side = Right >= 0.f ? 1.f : -1.f;
			const FVector2D Target(Width * 0.5f + Side * (Width * 0.5f - 40.f * Ui), Height * 0.5f);
			DrawRect(TideLight(Alpha), static_cast<float>(Target.X) - 8.f * Ui, static_cast<float>(Target.Y) - 8.f * Ui, 16.f * Ui, 16.f * Ui);
			DrawShadowText(Label, static_cast<float>(Target.X) - (Side > 0.f ? TextWidth(Label, Ui * 0.7f) + 14.f * Ui : -14.f * Ui), static_cast<float>(Target.Y) - 6.f * Ui, LabelColor, Ui * 0.7f);
		}
	}

	// Short status line at the right edge while listening.
	if (Listen->IsListening())
	{
		const FString Line = FString::Printf(TEXT("Lauschen · Stufe %d · %s"), Level, *UGamerKeyBindings::ActionKeyName(GamerInputActions::Listen));
		DrawShadowText(Line, Width - 10.f * Ui - TextWidth(Line, Ui * 0.8f), Height * 0.20f - LineHeight(Ui * 0.8f) * 1.4f, TideLight(1.f), Ui * 0.8f);
	}
}
