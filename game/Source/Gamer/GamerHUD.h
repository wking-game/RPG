#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "GamerHUD.generated.h"

class UFont;

/** One line of HUD text (not reflected, only used while drawing). */
struct FGamerHudLine
{
	FString Text;
	FLinearColor Color;
	float Size = 1.f;
};

/**
 * Simple placeholder HUD drawn on the canvas (no UMG yet): inventory, gathering hint,
 * work progress, feedback messages and the start crafting menu.
 * Text size follows the window height (at least 1.4x the engine font); every text has a shadow
 * and sits on a dark panel so it stays readable on any ground. Extra factor: cvar gamer.HudScale.
 * Will be replaced by the designed UI (ui_hud, ui_inventory, ui_crafting) later.
 */
UCLASS()
class GAMER_API AGamerHUD : public AHUD
{
	GENERATED_BODY()

public:
	virtual void DrawHUD() override;

	/** Menu button under a screen position (from the last drawn frame), or NAME_None. */
	FName FindMenuButtonAt(FVector2D Position) const;

	/** The actor a menu button belongs to (e.g. the villager of a job button), or null. */
	AActor* FindButtonTarget(FName ButtonId) const;

private:
	void DrawMenu(const class AGamerPlayerController& PC);
	void DrawBuildMenu(const class AGamerPlayerController& PC);
	void DrawStartMenu(const class AGamerPlayerController& PC);
	void DrawVillageMenu(const class AGamerPlayerController& PC);
	void DrawStorageMenu(const class AGamerPlayerController& PC);
	/** Graphics page: one row per setting (click the value for the next one), measured frame rate, buttons. */
	void DrawGraphicsMenu(const class AGamerPlayerController& PC);
	/** Bird's-eye view: name tags over villagers, help line, info panel of the selection. */
	void DrawBirdView(const class AGamerPlayerController& PC);
	void DrawBar(float X, float Y, float W, float H, float Value);
	void DrawButton(FName Id, const FString& Text, float X, float Y, float W, float H, float Scale, bool bActive = false);
	bool IsMouseOver(float X, float Y, float W, float H) const;

	/** Clickable areas of the menu, rebuilt every frame. */
	TArray<TPair<FName, FBox2D>> MenuButtons;
	TMap<FName, TWeakObjectPtr<AActor>> ButtonTargets;
	FVector2D MousePosition = FVector2D(-1.f, -1.f);

	UFont* HudFont() const;
	float TextWidth(const FString& Text, float Scale);
	float LineHeight(float Scale);
	void DrawShadowText(const FString& Text, float X, float Y, const FLinearColor& Color, float Scale);
	void DrawPanel(float X, float Y, float W, float H);
	/** Draws the lines top-down from (X, Y) on a panel; returns the used size without padding. */
	FVector2D DrawLineBlock(const TArray<FGamerHudLine>& Lines, float X, float Y);
	void DrawCentered(const FString& Text, float Y, const FLinearColor& Color, float Size = 1.f);

	/** Size factor for this frame (window height x gamer.HudScale). */
	float Ui = 1.f;
};
