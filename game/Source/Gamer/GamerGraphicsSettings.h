#pragma once

#include "CoreMinimal.h"

/** One row of the graphics page: clicking the value button switches to the next value. */
struct FGamerGraphicsRow
{
	FName Id;          // gfx:quality, gfx:mode, gfx:size, gfx:scale, gfx:fps, gfx:vsync, gfx:hud
	FString Label;
	FString Value;
	FString Explain;
	bool bEnabled = true;
};

/**
 * Graphics settings (menu page "Grafik", in the Esc menu and the start menu): quality, display mode, window size,
 * render resolution, frame rate limit, V-Sync, text size. Built on the engine's UGameUserSettings (saved per user
 * in Saved/Config/<Platform>/GameUserSettings.ini and loaded by the engine at start); the text size is saved in
 * UGamerMenuSettings. Every change is applied and saved at once.
 *
 * Exclusive fullscreen is not offered: a mode the monitor does not take can leave a black screen. Borderless
 * fullscreen always uses the desktop resolution; the render resolution row lowers the cost instead.
 * Quality and render resolution are kept apart (the engine's quality levels would also change the resolution).
 */
namespace GamerGraphics
{
	GAMER_API TArray<FGamerGraphicsRow> GetRows();

	/** Next value of a row (Id from GetRows), applied and saved. False for an unknown or disabled row. */
	GAMER_API bool Cycle(FName Id);

	/** Our defaults: quality high, automatic render resolution, no frame limit, V-Sync on, text 100 %. The display mode stays. */
	GAMER_API void ResetToDefaults();

	/** The engine's hardware benchmark picks the quality levels (the render resolution stays). */
	GAMER_API void AutoDetect();

	/** One line for logs and tests. */
	GAMER_API FString Describe();

	/** Measured frames per second (engine average), 0 on a server. */
	GAMER_API float GetAverageFps();

	/** Text size factor for the HUD (1 = normal). */
	GAMER_API float GetHudScale();
}
