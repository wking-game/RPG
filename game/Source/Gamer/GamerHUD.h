#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "GamerHUD.generated.h"

/**
 * Simple placeholder HUD drawn on the canvas (no UMG yet): inventory, gathering hint,
 * work progress, feedback messages and the start crafting menu.
 * Will be replaced by the designed UI (ui_hud, ui_inventory, ui_crafting) later.
 */
UCLASS()
class GAMER_API AGamerHUD : public AHUD
{
	GENERATED_BODY()

public:
	virtual void DrawHUD() override;

private:
	void DrawTextLine(const FString& Text, float X, float& Y, const FLinearColor& Color, float Scale = 1.f);
	void DrawCentered(const FString& Text, float Y, const FLinearColor& Color);
};
