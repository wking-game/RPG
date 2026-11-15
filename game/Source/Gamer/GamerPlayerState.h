#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "GamerCharacterData.h"
#include "GamerPlayerState.generated.h"

/**
 * The local character (per player, decision 2026-11-14), saved in Saved/Config/<Platform>/GameUserSettings.ini
 * until there is a save system. Edited in the start menu page "Charakter", sent to the server on joining.
 */
UCLASS(Config = GameUserSettings)
class GAMER_API UGamerCharacterSettings : public UObject
{
	GENERATED_BODY()

public:
	static UGamerCharacterSettings* Get() { return GetMutableDefault<UGamerCharacterSettings>(); }

	/** False until the player confirmed a character once ("Weiter" on the character page). */
	UPROPERTY(Config)
	bool bHasCharacter = false;

	UPROPERTY(Config)
	FGamerCharacterProfile Profile;

	void Save() { SaveConfig(); }
};

/**
 * Per-player state that every machine sees: name, origin, values, appearance choice, known abilities.
 * Set only by the server: the owning client sends its saved profile (ServerSetCharacter), the server checks it
 * against characters.json, looks up values, start ability and start equipment itself and puts the equipment
 * into the inventory once. Effects of the values come with the matching systems (combat, crafting, magic).
 */
UCLASS()
class GAMER_API AGamerPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	AGamerPlayerState();

	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	/** Owning client -> server: the character this player wants to play. Ignored once a character is set. */
	UFUNCTION(Server, Reliable)
	void ServerSetCharacter(const FGamerCharacterProfile& InProfile);

	bool HasCharacter() const { return bHasCharacter; }
	const FGamerCharacterProfile& GetProfile() const { return Profile; }
	const TArray<FGamerStatValue>& GetStats() const { return Stats; }
	const TArray<FName>& GetAbilities() const { return Abilities; }
	int32 GetStat(FName Stat) const;

	/** "Ragnar, Kriegerin/Krieger · STÄ 8 GES 5 ..." (HUD, logs). */
	FString Describe() const;

private:
	/** Owning client: sends the saved profile once the player state belongs to the local controller. */
	void TrySendLocalProfile();
	/** Server: puts the origin's start equipment into the pawn's inventory (once per player). */
	void GiveStartItems();

	UFUNCTION()
	void HandlePawnSet(APlayerState* Player, APawn* NewPawn, APawn* OldPawn);

	UPROPERTY(Replicated)
	FGamerCharacterProfile Profile;

	UPROPERTY(Replicated)
	TArray<FGamerStatValue> Stats;

	/** Learned abilities (at the start only the origin's start ability). */
	UPROPERTY(Replicated)
	TArray<FName> Abilities;

	UPROPERTY(Replicated)
	bool bHasCharacter = false;

	bool bStartItemsGiven = false;
	bool bProfileSent = false;
	FTimerHandle SendTimer;
};
