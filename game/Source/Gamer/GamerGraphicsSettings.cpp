#include "GamerGraphicsSettings.h"

#include "Gamer.h"
#include "GamerMenuGameMode.h"
#include "Engine/Engine.h"
#include "GameFramework/GameUserSettings.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Scalability.h"

extern ENGINE_API float GAverageFPS;

namespace
{
	UGameUserSettings* Settings()
	{
		return GEngine ? GEngine->GetGameUserSettings() : nullptr;
	}

	/** The quality level if all groups (except the render resolution) are the same, else -1 (mixed). */
	int32 QualityLevel(const UGameUserSettings& User)
	{
		const Scalability::FQualityLevels& Q = User.ScalabilityQuality;
		const int32 Groups[] = { Q.AntiAliasingQuality, Q.ShadowQuality, Q.GlobalIlluminationQuality, Q.ReflectionQuality, Q.PostProcessQuality,
			Q.TextureQuality, Q.EffectsQuality, Q.FoliageQuality, Q.ShadingQuality, Q.LandscapeQuality };
		for (const int32 Group : Groups)
		{
			if (Group != Q.ViewDistanceQuality)
			{
				return -1;
			}
		}
		return Q.ViewDistanceQuality;
	}

	FString QualityName(int32 Level)
	{
		switch (Level)
		{
		case 0: return TEXT("Niedrig");
		case 1: return TEXT("Mittel");
		case 2: return TEXT("Hoch");
		case 3: return TEXT("Episch");
		case 4: return TEXT("Kino");
		default: return TEXT("Gemischt");
		}
	}

	/** The engine's resolution presets (BaseScalability.ini [ResolutionQuality]); 0 = the engine's automatic default. */
	struct FScalePreset
	{
		float Value;
		const TCHAR* Name;
	};
	const FScalePreset ScalePresets[] = {
		{ 0.f, TEXT("Automatisch") },
		{ 50.f, TEXT("Leistung (50 %)") },
		{ 58.328f, TEXT("Ausgewogen (58 %)") },
		{ 66.662f, TEXT("Qualität (67 %)") },
		{ 100.f, TEXT("Voll (100 %)") },
	};

	int32 ScaleIndex(float Value)
	{
		for (int32 Index = 0; Index < UE_ARRAY_COUNT(ScalePresets); ++Index)
		{
			if (FMath::Abs(ScalePresets[Index].Value - Value) < 0.5f)
			{
				return Index;
			}
		}
		return INDEX_NONE;
	}

	const float FrameLimits[] = { 0.f, 30.f, 60.f, 90.f, 120.f, 144.f };
	const float HudScales[] = { 0.8f, 1.0f, 1.2f, 1.4f, 1.6f };

	/** Index of the nearest value in a list. */
	template <int32 N>
	int32 NearestIndex(const float (&Values)[N], float Value)
	{
		int32 Best = 0;
		for (int32 Index = 1; Index < N; ++Index)
		{
			if (FMath::Abs(Values[Index] - Value) < FMath::Abs(Values[Best] - Value))
			{
				Best = Index;
			}
		}
		return Best;
	}

	FString SizeText(const FIntPoint& Size)
	{
		return FString::Printf(TEXT("%d × %d"), Size.X, Size.Y);
	}

	/** Window sizes that fit on this screen, smallest first. */
	TArray<FIntPoint> WindowSizes()
	{
		TArray<FIntPoint> Sizes;
		UKismetSystemLibrary::GetConvenientWindowedResolutions(Sizes);
		if (Sizes.Num() == 0)
		{
			Sizes = { FIntPoint(1280, 720), FIntPoint(1600, 900), FIntPoint(1920, 1080) };
		}
		Sizes.Sort([](const FIntPoint& A, const FIntPoint& B) { return A.X * A.Y < B.X * B.Y; });
		return Sizes;
	}

	void ApplyAndSave(UGameUserSettings& User)
	{
		User.ConfirmVideoMode(); // only safe modes are offered, so no "keep these settings?" countdown
		User.ApplySettings(false);
		User.SaveSettings();
		UE_LOG(LogGamer, Log, TEXT("Graphics: %s"), *GamerGraphics::Describe());
	}
}

float GamerGraphics::GetHudScale()
{
	return FMath::Clamp(UGamerMenuSettings::Get()->HudScale, 0.5f, 2.f);
}

float GamerGraphics::GetAverageFps()
{
	return GAverageFPS;
}

TArray<FGamerGraphicsRow> GamerGraphics::GetRows()
{
	TArray<FGamerGraphicsRow> Rows;
	const UGameUserSettings* User = Settings();
	if (!User)
	{
		return Rows;
	}
	const EWindowMode::Type Mode = User->GetFullscreenMode();
	const bool bWindowed = Mode == EWindowMode::Windowed;
	const int32 Scale = ScaleIndex(User->ScalabilityQuality.ResolutionQuality);
	const float FrameLimit = User->GetFrameRateLimit();

	Rows.Add({ TEXT("gfx:quality"), TEXT("Qualität"), QualityName(QualityLevel(*User)),
		TEXT("Schatten, Licht, Sichtweite, Effekte. Niedriger = mehr Bilder pro Sekunde.") });
	Rows.Add({ TEXT("gfx:mode"), TEXT("Anzeige"),
		bWindowed ? FString(TEXT("Fenster")) : Mode == EWindowMode::WindowedFullscreen ? FString(TEXT("Vollbild (randlos)")) : FString(TEXT("Vollbild (exklusiv)")),
		TEXT("Randloser Vollbild nutzt die Auflösung des Bildschirms.") });
	Rows.Add({ TEXT("gfx:size"), TEXT("Fenstergröße"),
		bWindowed ? SizeText(User->GetScreenResolution()) : FString::Printf(TEXT("Bildschirm (%s)"), *SizeText(User->GetDesktopResolution())),
		bWindowed ? TEXT("Anklicken für die nächste Größe.") : TEXT("Nur im Fenster einstellbar."), bWindowed });
	Rows.Add({ TEXT("gfx:scale"), TEXT("Render-Auflösung"),
		Scale != INDEX_NONE ? FString(ScalePresets[Scale].Name) : FString::Printf(TEXT("%.0f %%"), User->ScalabilityQuality.ResolutionQuality),
		TEXT("Weniger = schneller, etwas weicher (wird hochgerechnet). Automatisch = Vorgabe der Engine.") });
	Rows.Add({ TEXT("gfx:fps"), TEXT("Bildrate begrenzen"),
		FrameLimit <= 0.f ? FString(TEXT("Unbegrenzt")) : FString::Printf(TEXT("%.0f Bilder/s"), FrameLimit),
		TEXT("Eine Grenze spart Strom und hält die Grafikkarte kühler.") });
	Rows.Add({ TEXT("gfx:vsync"), TEXT("V-Sync"), User->IsVSyncEnabled() ? TEXT("An") : TEXT("Aus"),
		TEXT("Verhindert Bildrisse, kann die Eingabe leicht verzögern.") });
	Rows.Add({ TEXT("gfx:hud"), TEXT("Schriftgröße"), FString::Printf(TEXT("%.0f %%"), GetHudScale() * 100.f),
		TEXT("Größe aller Texte der Anzeige und der Menüs.") });
	return Rows;
}

bool GamerGraphics::Cycle(FName Id)
{
	UGameUserSettings* User = Settings();
	if (!User)
	{
		return false;
	}
	const FString Which = Id.ToString();
	if (Which == TEXT("gfx:quality"))
	{
		const int32 Level = QualityLevel(*User);
		const float Resolution = User->ScalabilityQuality.ResolutionQuality;
		User->SetOverallScalabilityLevel(Level < 0 ? 2 : (Level + 1) % 4);
		User->SetResolutionScaleValueEx(Resolution); // the quality level must not change the render resolution
	}
	else if (Which == TEXT("gfx:mode"))
	{
		if (User->GetFullscreenMode() == EWindowMode::Windowed)
		{
			User->SetFullscreenMode(EWindowMode::WindowedFullscreen);
			User->SetScreenResolution(User->GetDesktopResolution());
		}
		else
		{
			// Back to a window that fits on the screen (the largest convenient size below the desktop).
			const FIntPoint Desktop = User->GetDesktopResolution();
			FIntPoint Size = WindowSizes()[0];
			for (const FIntPoint& Candidate : WindowSizes())
			{
				if (Candidate.X < Desktop.X && Candidate.Y < Desktop.Y)
				{
					Size = Candidate;
				}
			}
			User->SetFullscreenMode(EWindowMode::Windowed);
			User->SetScreenResolution(Size);
		}
	}
	else if (Which == TEXT("gfx:size"))
	{
		if (User->GetFullscreenMode() != EWindowMode::Windowed)
		{
			return false;
		}
		const TArray<FIntPoint> Sizes = WindowSizes();
		const FIntPoint Current = User->GetScreenResolution();
		int32 Next = 0;
		for (int32 Index = 0; Index < Sizes.Num(); ++Index)
		{
			if (Sizes[Index].X * Sizes[Index].Y > Current.X * Current.Y)
			{
				Next = Index;
				break;
			}
		}
		User->SetScreenResolution(Sizes[Next]); // after the largest size it starts again at the smallest
	}
	else if (Which == TEXT("gfx:scale"))
	{
		const int32 Index = ScaleIndex(User->ScalabilityQuality.ResolutionQuality);
		User->SetResolutionScaleValueEx(ScalePresets[(Index + 1) % UE_ARRAY_COUNT(ScalePresets)].Value);
	}
	else if (Which == TEXT("gfx:fps"))
	{
		const int32 Index = NearestIndex(FrameLimits, User->GetFrameRateLimit());
		User->SetFrameRateLimit(FrameLimits[(Index + 1) % UE_ARRAY_COUNT(FrameLimits)]);
	}
	else if (Which == TEXT("gfx:vsync"))
	{
		User->SetVSyncEnabled(!User->IsVSyncEnabled());
	}
	else if (Which == TEXT("gfx:hud"))
	{
		UGamerMenuSettings* Menu = UGamerMenuSettings::Get();
		Menu->HudScale = HudScales[(NearestIndex(HudScales, Menu->HudScale) + 1) % UE_ARRAY_COUNT(HudScales)];
		Menu->Save();
		UE_LOG(LogGamer, Log, TEXT("Graphics: %s"), *Describe());
		return true;
	}
	else
	{
		return false;
	}
	ApplyAndSave(*User);
	return true;
}

void GamerGraphics::ResetToDefaults()
{
	UGameUserSettings* User = Settings();
	if (!User)
	{
		return;
	}
	User->SetOverallScalabilityLevel(2);
	User->SetResolutionScaleValueEx(0.f);
	User->SetFrameRateLimit(0.f);
	User->SetVSyncEnabled(true);
	UGamerMenuSettings::Get()->HudScale = 1.f;
	UGamerMenuSettings::Get()->Save();
	ApplyAndSave(*User);
}

void GamerGraphics::AutoDetect()
{
	UGameUserSettings* User = Settings();
	if (!User)
	{
		return;
	}
	const float Resolution = User->ScalabilityQuality.ResolutionQuality;
	User->RunHardwareBenchmark();
	User->ApplyHardwareBenchmarkResults();
	User->SetResolutionScaleValueEx(Resolution);
	UE_LOG(LogGamer, Log, TEXT("Graphics benchmark: CPU %.0f, GPU %.0f"), User->GetLastCPUBenchmarkResult(), User->GetLastGPUBenchmarkResult());
	ApplyAndSave(*User);
}

FString GamerGraphics::Describe()
{
	const UGameUserSettings* User = Settings();
	if (!User)
	{
		return TEXT("(no settings)");
	}
	const FIntPoint Size = User->GetScreenResolution();
	return FString::Printf(TEXT("quality=%d (%s) mode=%d size=%dx%d scale=%.1f fps_limit=%.0f vsync=%d hud=%.2f avg_fps=%.0f"),
		QualityLevel(*User), *QualityName(QualityLevel(*User)), static_cast<int32>(User->GetFullscreenMode()), Size.X, Size.Y,
		User->ScalabilityQuality.ResolutionQuality, User->GetFrameRateLimit(), User->IsVSyncEnabled() ? 1 : 0, GetHudScale(), GetAverageFps());
}
