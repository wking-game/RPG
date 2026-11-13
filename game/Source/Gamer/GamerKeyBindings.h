#pragma once

#include "CoreMinimal.h"
#include "InputCoreTypes.h"
#include "UObject/Object.h"
#include "GamerKeyBindings.generated.h"

/** Ids of the rebindable actions (stable, also used in the saved settings). */
namespace GamerInputActions
{
	inline const FName MoveForward(TEXT("move_forward"));
	inline const FName MoveBack(TEXT("move_back"));
	inline const FName MoveLeft(TEXT("move_left"));
	inline const FName MoveRight(TEXT("move_right"));
	inline const FName Jump(TEXT("jump"));
	inline const FName Strike(TEXT("strike"));
	inline const FName Interact(TEXT("interact"));
	inline const FName CraftMenu(TEXT("craft_menu"));
	inline const FName BuildMenu(TEXT("build_menu"));
	inline const FName Rotate(TEXT("rotate"));
	inline const FName Demolish(TEXT("demolish"));
	inline const FName VillageMenu(TEXT("village_menu"));
	inline const FName BirdView(TEXT("bird_view"));

	/** hotbar_1 .. hotbar_8 */
	GAMER_API FName Hotbar(int32 Number);
}

/** One keyboard/mouse action the player can rebind. Gamepad buttons stay fixed for now. */
struct FGamerBindableAction
{
	FName Id;
	FString Label;
	FKey DefaultKey;
};

USTRUCT()
struct FGamerKeyOverride
{
	GENERATED_BODY()

	UPROPERTY()
	FName Action;

	UPROPERTY()
	FKey Key;
};

/**
 * The player's keyboard and mouse bindings (decision 2026-11-13: all keys can be rebound in the
 * menu under "Steuerung"). Defaults live in code; only changes are saved, per user, in
 * Saved/Config/<Platform>/GameUserSettings.ini, never in the project's Config folder.
 * Escape always opens the menu and cannot be rebound.
 */
UCLASS(Config = GameUserSettings)
class GAMER_API UGamerKeyBindings : public UObject
{
	GENERATED_BODY()

public:
	static UGamerKeyBindings* Get() { return GetMutableDefault<UGamerKeyBindings>(); }

	/** All rebindable actions in menu order. */
	static const TArray<FGamerBindableAction>& GetActions();

	/** Current key of an action (the saved change, or the default). */
	FKey GetKey(FName Action) const;

	/**
	 * Binds Key to Action and saves. If another action had that key, it gets Action's old key
	 * (swap, so no action is ever left without a key). Returns the swapped action or NAME_None.
	 */
	FName SetKey(FName Action, const FKey& Key);

	void ResetToDefaults();

	/** Short German name for a key ("Linksklick", "Leertaste", "E"). */
	static FString KeyName(const FKey& Key);

	/** Short German name of the key bound to an action. */
	static FString ActionKeyName(FName Action) { return KeyName(Get()->GetKey(Action)); }

	static FString ActionLabel(FName Action);

	/** False for keys that cannot be bound (Escape, gamepad, analog axes). */
	static bool CanBind(const FKey& Key);

private:
	void SetOverride(FName Action, const FKey& Key);

	UPROPERTY(Config)
	TArray<FGamerKeyOverride> Overrides;
};
