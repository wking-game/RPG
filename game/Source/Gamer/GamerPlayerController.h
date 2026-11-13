#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GamerPlayerController.generated.h"

/** Which menu page is open (local player only; in a world the game keeps running). */
enum class EGamerMenuPage : uint8
{
	None,
	Main,         // Esc in a world: Weiter, Steuerung, Hauptmenue, Spiel beenden
	Controls,
	Build,
	Start,        // start menu: Welt erstellen, Beitreten, Steuerung, Beenden
	CreateWorld,  // world options, then host the world (listen server)
	Join,         // type the host's address, then connect
	Village,      // village management: villagers, jobs, needs (key V)
	Storage,      // settlement storage: take and put (E at a storage building)
	Graphics      // graphics settings (GamerGraphicsSettings.h), from the Esc menu and the start menu
};

/**
 * Player controller. Gameplay input lives on the character; this class holds the menus and test helpers.
 * In a world: Esc menu (Weiter, Steuerung with key rebinding, Hauptmenue, Spiel beenden) and the build menu.
 * In the start menu (map Entry, AGamerMenuGameMode): create a world with world options (this player becomes
 * the host, listen server) or join a host by address. The HUD draws the menus, this class handles their input.
 * Test helpers for automated multiplayer tests (tools/mp_test.ps1): see RunTestScript.
 */
UCLASS()
class GAMER_API AGamerPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;
	virtual void PlayerTick(float DeltaTime) override;
	virtual bool InputKey(const FInputKeyEventArgs& Params) override;

	/** Console: walk forward for N seconds. Example: GamerTestWalk 3 */
	UFUNCTION(Exec)
	void GamerTestWalk(float Seconds = 3.f);

	EGamerMenuPage GetMenuPage() const { return MenuPage; }
	bool IsMenuOpen() const { return MenuPage != EGamerMenuPage::None; }

	/** Controls page: the action waiting for its new key, or NAME_None. */
	FName GetListeningAction() const { return ListeningAction; }

	/** Short feedback line on the menu ("Getauscht mit ..."). */
	const FString& GetMenuNote() const { return MenuNote; }

	void OpenMenu(EGamerMenuPage Page = EGamerMenuPage::Main);
	void CloseMenu();

	/**
	 * A menu button was clicked. Ids from the HUD: resume, controls, quit, back, reset, bind:<action>, build:<id>|none,
	 * leave (to the start menu), create, joinpage, opt:flow|place|demolish (cycle a world option), start, connect.
	 */
	void PressMenuButton(FName ButtonId);

	/** Server: storage of the settlement (the player must stand near a finished storage building). */
	UFUNCTION(Server, Reliable)
	void ServerStorageTake(FName Item, int32 Count);

	UFUNCTION(Server, Reliable)
	void ServerStorageDeposit(FName Item, int32 Count);

	/** Everything from the inventory except tools (or with tools, for tests). */
	UFUNCTION(Server, Reliable)
	void ServerStorageDepositAll(bool bIncludeTools);

	/** Village management: the player sets a villager's job. */
	UFUNCTION(Server, Reliable)
	void ServerSetVillagerJob(class AGamerVillager* Villager, FName JobId);

	/** Development builds only: a villager appears next to the player (tests). */
	UFUNCTION(Server, Reliable)
	void ServerDebugSpawnVillager();

	/** Villagers in a stable order (by name) for the village management and tests. */
	TArray<class AGamerVillager*> GetVillagersSorted() const;

	/** Job ids in menu order (fallback job first), for cycling a villager's job. */
	TArray<FName> GetJobOrder() const;

	/** True in the start menu (no world, no character). */
	bool IsInStartMenu() const { return bInStartMenu; }

	/** Join page: the address being typed. */
	const FString& GetJoinAddress() const { return JoinAddress; }

	/**
	 * Bird's-eye view (key B, rebindable; B or Esc goes back): a camera high above the settlement, only for this
	 * player (nothing is replicated). Move keys pan, mouse wheel zooms, right mouse button held turns, left click
	 * selects a villager, building or resource node. The character stands still, the game keeps running. The camera
	 * stays within BirdMaxRange of the own character, so it is no free scouting camera in co-op.
	 */
	bool IsInBirdView() const { return bBirdView; }
	void SetBirdView(bool bOn);

	/** What was clicked in the bird's-eye view, or null. */
	AActor* GetBirdSelection() const { return BirdSelection.Get(); }

	/** Info lines about the selection (title first), shown in the bird's-eye view and logged by tests. */
	TArray<FString> DescribeBirdSelection() const;

private:
	/** Cursor and gameplay input for the current state: menu or bird's-eye view = cursor, otherwise the character. */
	void ApplyInputModeForState();
	void UpdateBirdView(float DeltaTime);
	void ApplyBirdCamera();
	/** Villager near the screen position (by distance on screen), else the building or resource node under it. */
	AActor* FindBirdTargetAt(const FVector2D& ScreenPosition) const;
	void SelectBirdTarget(AActor* Target);

	bool bBirdView = false;
	UPROPERTY(Transient)
	TObjectPtr<class ACameraActor> BirdCamera;
	FVector BirdFocus = FVector::ZeroVector;
	float BirdYaw = 0.f;
	float BirdPitch = -55.f;
	float BirdDistance = 2600.f;
	bool bBirdRotating = false;
	FVector2D BirdLastMouse = FVector2D::ZeroVector;
	TWeakObjectPtr<AActor> BirdSelection;

	void ApplyKeyBindingsToPawn();
	class AGamerBuilding* StorageInReach() const;
	void HostWorld();
	void JoinWorld();
	void LeaveWorld();
	/** Join page: a typed character for the address (digits, letters, '.', ':', '-'); false if the key is not one. */
	bool TypeIntoAddress(const FKey& Key);
	void ParseScript(const TCHAR* CommandLineKey);
	void RunMenuScript(float DeltaTime);
	void TakeTestShot();

	bool bInStartMenu = false;
	FString JoinAddress;

	EGamerMenuPage MenuPage = EGamerMenuPage::None;
	FName ListeningAction;
	FString MenuNote;

	/** From -GamerTestWalk=N and -GamerTestWalkDelay=N: walk once the pawn exists and the delay is over. */
	float PendingWalkSeconds = 0.f;
	float PendingWalkDelay = 0.f;

	/**
	 * From -GamerTestScript="harvest:res_fallen_branch*2;craft:recipe_axe_t1;wait:2;harvest:res_tree_pine":
	 * steps run one after another on the owning client and go through the normal Server RPCs
	 * (harvest steps first ask the server to move the player next to the nearest matching node).
	 * harvest on a tree/rock selects the matching tool in the hotbar and strikes until it is done.
	 * Also: select:N (hotbar slot 1..8), handharvest:res_x (interact key on any node, refusal test),
	 * bind:<action>=<Key> (rebind like the menu, then log the mapping), resetkeys, keys (log the mapping),
	 * menu:main|controls|village|storage|build|close, button:<menu button id>, press:<Key> (simulated key press), shot (screenshot),
	 * place:<buildable>@x,y[,z] (aim point: in the frame of the nearest foundation, or ahead/right/up of the player
	 * if there is none; the hammer is taken into the hand), hammer[:<seconds per plan>] (finish all plans nearby), demolish:<buildable>,
	 * goto:<buildable> (stand next to it), view:<buildable>:<cm> (stand back and look at it), buildings (log all pieces), give:<item>:<count> (development builds only).
	 * In the start menu the steps come from -GamerMenuScript (once per process): wait, button, press, address:<text>, shot.
	 * Settlement: deposit:<item>:<count>, depositall (with tools), take:<item>:<count>, spawnvillager (development builds),
	 * setjob:<n>:<job_id> (n = 1.. in name order), villagers, settlement (log the state as this player sees it).
	 * Bird's-eye view: bird:on|off, birdfocus:<target>, birdzoom:<cm>, birdturn:<yaw>,<pitch>, birdselect:<target> (clicks on the
	 * target's screen position, like the mouse), birdinfo (log the selection). <target> = villager:<n> or a buildable id.
	 */
	void RunTestScript(float DeltaTime);
	TArray<FString> TestSteps;
	int32 TestStepIndex = 0;
	float TestPause = 0.f;

	TWeakObjectPtr<class AActor> StrikeTarget;
	float StrikeTimeLeft = 0.f;
	float StrikeCooldown = 0.f;
	bool bHammerAll = false;
	float HammerTimeout = 60.f;
	void StartHammerNext();
};
