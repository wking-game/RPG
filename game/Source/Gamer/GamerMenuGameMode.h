#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "GamerGameState.h"
#include "GamerMenuGameMode.generated.h"

/**
 * Game mode of the start menu (map /Engine/Maps/Entry, set in DefaultEngine.ini via GameModeMapPrefixes).
 * No pawn: the player controller opens the start menu (create a world, join, controls, quit).
 */
UCLASS()
class GAMER_API AGamerMenuGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AGamerMenuGameMode();

	/** No character in the start menu. */
	virtual void RestartPlayer(AController* NewPlayer) override {}
};

/**
 * What the menus remember between starts (per user, Saved/Config/<Platform>/GameUserSettings.ini):
 * the world options last used for "Welt erstellen", the last address for "Beitreten" and the text size
 * (page "Grafik"; the other graphics settings are the engine's UGameUserSettings).
 */
UCLASS(Config = GameUserSettings)
class GAMER_API UGamerMenuSettings : public UObject
{
	GENERATED_BODY()

public:
	static UGamerMenuSettings* Get() { return GetMutableDefault<UGamerMenuSettings>(); }

	UPROPERTY(Config)
	FGamerWorldOptions WorldOptions;

	UPROPERTY(Config)
	FString JoinAddress = TEXT("127.0.0.1");

	/** Text size of the HUD and menus (1 = normal), on top of the automatic size from the window height. */
	UPROPERTY(Config)
	float HudScale = 1.f;

	void Save() { SaveConfig(); }
};

/** Maps used by the start menu (fixed until there is a world generator and save games). */
namespace GamerMaps
{
	inline const TCHAR* const StartMenu = TEXT("/Engine/Maps/Entry");
	inline const TCHAR* const World = TEXT("/Engine/Maps/Templates/Template_Default");
}
