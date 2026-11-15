#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Subsystems/WorldSubsystem.h"
#include "GamerSaveable.h"
#include "GamerSaveSubsystem.generated.h"

class AController;
class AGameModeBase;
class APlayerController;

/** One saved world as listed in the start menu ("Welt laden"). From meta.json, no need to read the whole world. */
struct GAMER_API FGamerWorldSaveInfo
{
	FString WorldId;
	FString Name;
	FDateTime Created;
	FDateTime Saved;
	double PlaySeconds = 0.0;
	int32 Schema = 0;
	/** "Bauplan + Hammer · ..." of the saved world options. */
	FString OptionsText;

	/** "12 h 05 min" / "7 min". */
	FString DescribePlayTime() const;
};

/** File locations and the world list (usable everywhere, also in the start menu where no game world runs). */
namespace GamerSave
{
	/** Current save format. Older files are migrated on load (MigrateWorld), newer ones load best-effort. */
	constexpr int32 SchemaVersion = 1;

	/** Saved/SaveGames/Worlds */
	GAMER_API FString WorldsDirectory();
	GAMER_API FString WorldDirectory(const FString& WorldId);
	GAMER_API FString WorldFile(const FString& WorldId);
	/** Only letters, digits, '_' and '-' (ids become folder names). */
	GAMER_API bool IsValidWorldId(const FString& WorldId);
	/** Folder-safe id from a display name, unique among the existing worlds ("Mein Dorf" -> "mein_dorf", "mein_dorf_2"). */
	GAMER_API FString MakeWorldId(const FString& Name);
	/** All saved worlds, newest save first. */
	GAMER_API TArray<FGamerWorldSaveInfo> ListWorlds();
	/** Reads the whole save file (null if missing or broken; falls back to the backup copy). */
	GAMER_API TSharedPtr<FJsonObject> ReadWorldFile(const FString& WorldId, FString* OutError = nullptr);
	/** Writes world.json via a temporary file, keeping the previous file as world.bak.json, plus meta.json. */
	GAMER_API bool WriteWorldFile(const FString& WorldId, const TSharedRef<FJsonObject>& Root, FString* OutError = nullptr);
	/** Brings an older file to SchemaVersion. Returns false if it cannot be read at all. */
	GAMER_API bool MigrateWorld(const TSharedRef<FJsonObject>& Root, TArray<FString>& OutNotes);
	/** Pretty JSON text of a record (tests, verification files). */
	GAMER_API FString ToJsonText(const TSharedRef<FJsonObject>& Object);
	/** Compares two world blocks, ignoring volatile fields (timers within Tolerance seconds). Empty = equal. */
	GAMER_API TArray<FString> CompareWorldBlocks(const FGamerSaveRecord& A, const FGamerSaveRecord& B, double Tolerance);
	/** Item known in the data tables of the running game (unknown ids are dropped on load with a warning). */
	GAMER_API bool IsKnownItem(const UObject* WorldContext, FName Item);
	/**
	 * The player's character file (world option "inventory travels with the character", 2026-11-15), kept on the
	 * player's own machine: Saved/SaveGames/Characters/<player id>.json. Holds the carried inventory for now; the
	 * character profile can move here later (agreed with the character session).
	 */
	GAMER_API FString CharacterFile(const FString& PlayerId);
	/** "inventory" block of the character file as JSON text, empty if there is none. */
	GAMER_API FString ReadCarriedInventory(const FString& PlayerId);
	GAMER_API bool WriteCarriedInventory(const FString& PlayerId, const FGamerSaveRecord& Inventory);
	/** Removes entries with unknown item ids; logs and counts them. */
	GAMER_API void FilterKnownItems(const UObject* WorldContext, TArray<FGamerItemAmount>& Items, const TCHAR* Where);
}

/**
 * Stable per-user player id for re-joining a world (proposal 2026-11-15): a random GUID created once and kept in
 * Saved/Config/<Platform>/GameUserSettings.ini. Tests (several game instances on one PC share that file) pass
 * -GamerPlayerId=<name>. Later replaced by the online id (Steam) once there is an online subsystem.
 */
UCLASS(Config = GameUserSettings)
class GAMER_API UGamerPlayerIdentity : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(Config)
	FString PlayerId;

	/** The id of this game instance (command line first, then the saved GUID, created on first use). */
	static FString GetLocalPlayerId();
};

/**
 * Added by the save subsystem to every player controller on the server (replicated, owner only).
 * The owning client sends its player id once (ServerIdentify); the server restores that player from the save.
 */
UCLASS(ClassGroup = (Gamer))
class GAMER_API UGamerSaveClientComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UGamerSaveClientComponent();
	virtual void BeginPlay() override;

	/** CarriedInventory: the inventory block of the local character file (JSON, may be empty); used in worlds whose
	 *  inventory travels with the character, ignored otherwise. */
	UFUNCTION(Server, Reliable)
	void ServerIdentify(const FString& PlayerId, const FString& CarriedInventory);

	/** Server -> owning client: this world's inventory travels with the character; keep the character file up to date. */
	UFUNCTION(Client, Reliable)
	void ClientKeepCarriedInventory(const FString& AppliedInventory);

	/** Owning client or listen-server host: request a save of the world (Esc menu "Speichern"). Only the host may. */
	UFUNCTION(Server, Reliable)
	void ServerRequestSave();

private:
	void TrySendIdentity();
	/** Owning client: write the inventory into the character file when it changed (every 2 s). */
	void WriteCarriedInventory();
	FString LastWritten;
	/** What the server put into the inventory; writing starts once the replicated inventory shows it (or after 10 s). */
	FString ExpectedInventory;
	double CarryStart = 0.0;
	bool bCarrySynced = false;
	FTimerHandle CarryTimer;
	bool bSent = false;
	int32 Attempts = 0;
	FTimerHandle RetryTimer;
};

/**
 * Save system (decision request 2026-11-15, docs/01_entscheidungen.md "Speichersystem"). Server only.
 *
 * - Active only when the world has a save id: map URL ?SaveWorld=<id> (start menu "Welt erstellen"/"Welt laden")
 *   or -GamerWorld=<id> on the command line (dedicated server). Without an id nothing is saved (tests and old
 *   start paths stay as they were). ?WorldName= gives the display name of a new world.
 * - Loads in AGamerGameMode::StartPlay (LoadOrCreate): respawns resource nodes, buildings, ground items, villagers,
 *   restores game state and settlement. A new world just gets its first save.
 * - Saves: every AutosaveMinutes, when the host saves in the Esc menu, when a player leaves (their record only, in
 *   memory), when the world ends (host to main menu, quit, dedicated server shutdown).
 * - Players: matched by a stable id (UGamerPlayerIdentity); on re-join they get position, inventory, hotbar and
 *   buffs back. Unknown players start fresh (start equipment from their character).
 */
UCLASS(Config = Game)
class GAMER_API UGamerSaveSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	virtual void OnWorldBeginPlay(UWorld& InWorld) override;

	static UGamerSaveSubsystem* Get(const UObject* WorldContext);

	/** Server, from AGamerGameMode::StartPlay. Returns true if a saved world was loaded (then no start area). */
	bool LoadOrCreate();

	/** True when this world is saved (has a save id) and runs on the server. */
	bool IsActive() const { return bActive; }
	const FString& GetWorldId() const { return WorldId; }
	const FString& GetWorldName() const { return WorldName; }
	bool WasLoaded() const { return bLoaded; }

	/** Server: write the world now. Reason goes to the log ("autosave", "menu", "shutdown"). */
	bool SaveNow(const FString& Reason);

	/** Server: whether this player id has a record in the loaded world (then no new start equipment). */
	bool IsKnownPlayer(const FString& PlayerId) const;
	/** Player id assigned to a controller (empty until it identified itself). */
	FString GetPlayerId(const AController* Controller) const;
	/** True once a controller's player is restored or known to be new (start equipment may be given then). */
	bool IsPlayerResolved(const AController* Controller) const;

	/** Server: called by UGamerSaveClientComponent. */
	void HandleIdentify(APlayerController* Controller, const FString& RequestedId, const FString& CarriedInventory = FString());
	/** Server: whether the player gets the inventory from a save (world record or carried character inventory), so no start equipment. */
	bool HasSavedInventory(const AController* Controller) const;

	/** Builds the whole save (world + players) as JSON. Exposed for tests. */
	TSharedRef<FJsonObject> BuildSave() const;
	/** World block only (no meta, no players): what the round-trip checks compare. */
	FGamerSaveRecord CollectWorld() const;
	/** Restores a world block into this world (removes existing nodes, buildings, ground items, villagers first). */
	void RestoreWorld(const FGamerSaveRecord& World, int32& OutSkipped);

	/** Player record of a controller's pawn and player state (position, inventory, buffs ...). */
	FGamerSaveRecord CollectPlayer(const AController* Controller) const;
	void RestorePlayer(AController* Controller, const FGamerSaveRecord& Record) const;

	/** Played seconds of this world including earlier sessions. */
	double GetPlaySeconds() const;

	/** Minutes between autosaves (proposal 2026-11-15: 5). 0 = off. -GamerAutosaveSeconds=N overrides for tests. */
	UPROPERTY(Config)
	float AutosaveMinutes = 5.f;

private:
	void HandlePostLogin(AGameModeBase* GameMode, APlayerController* NewPlayer);
	void HandleLogout(AGameModeBase* GameMode, AController* Exiting);
	void HandleWorldTearDown(UWorld* InWorld);
	void HandlePreExit();
	void TickRestore();
	void Autosave();

	bool bActive = false;
	bool bLoaded = false;
	bool bShutdownSaved = false;
	FString WorldId;
	FString WorldName;
	FDateTime Created;
	double PlaySecondsBefore = 0.0;
	double SessionStart = 0.0;

	/** Records of all players this world knows (loaded, plus everyone who left or was saved since). */
	TMap<FString, FGamerSaveRecord> PlayerRecords;
	/** Connected controllers and their player ids. */
	TMap<TWeakObjectPtr<AController>, FString> ControllerIds;
	/** Identified, waiting for a pawn to restore into. */
	TArray<TWeakObjectPtr<AController>> PendingRestore;
	/** Inventory the player brought in the character file (worlds with bCarryInventory). */
	TMap<TWeakObjectPtr<AController>, FGamerSaveRecord> CarriedInventories;
	/** Controllers whose player is restored or new. */
	TSet<TWeakObjectPtr<AController>> Resolved;
	/** Joined but no id yet: time of joining (falls back to the online id after a while). */
	TMap<TWeakObjectPtr<APlayerController>, double> AwaitingId;

	FTimerHandle AutosaveTimer;
	FTimerHandle RestoreTimer;
	FDelegateHandle PostLoginHandle;
	FDelegateHandle LogoutHandle;
	FDelegateHandle TearDownHandle;
	FDelegateHandle PreExitHandle;
	float QuitAfterSeconds = 0.f;
};
