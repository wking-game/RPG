#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GamerAnimTypes.h"
#include "GamerCharacter.generated.h"

class AGamerBuilding;
class AGamerResourceNode;
class UGamerBuildComponent;
class UCameraComponent;
class UGamerInventoryComponent;
class UMaterialInstanceDynamic;
class UGamerScanComponent;
class UGamerListenComponent;
class UGamerHealthComponent;
class UGamerEquipmentComponent;
class UGamerProgressComponent;
class UGamerStaminaComponent;
class UGamerWorkComponent;
class UInputAction;
class UInputMappingContext;
class USpringArmComponent;
class UStaticMeshComponent;
struct FInputActionValue;

/**
 * Co-op player character (third person).
 * Movement runs through UCharacterMovementComponent: the server is authoritative,
 * the owning client predicts and the server corrects. Other players see a smoothed copy.
 * Placeholder look: a cylinder body plus a small cube that shows the facing direction.
 */
UCLASS(Config = Game)
class GAMER_API AGamerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AGamerCharacter();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	virtual void NotifyControllerChanged() override;
	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	/** Sprinting (held key, no stamina: architecture rule 5). Local player sets it at once, the server confirms. */
	void SetSprinting(bool bOn);
	bool IsSprinting() const { return bSprinting; }

	/** Animation inputs (sprint, work loop, last one-shot action); speed and falling come from the movement. */
	void FillAnimInputs(FGamerAnimInputs& Out) const;

	/** Local only: play a one-shot work animation on this machine (prediction for the acting player). */
	void PlayActionLocally(EGamerAnimAction Kind);

	/** Local player: walk up to a hand-gatherable node or ground item, then grab it (E key path; tests). */
	void StartGrab(AActor* Target);

	/** Local player: turn the body towards what it works on (the movement component replicates the rotation). */
	void FaceTowards(const AActor* Target);
	/** Work clip for crafting at the station of this recipe (workbench, camp fire, other). */
	EGamerAnimAction StationAnim(FName RecipeId) const;

	/** Server: play a one-shot work animation on all machines. bPredicted: the owner already played it. */
	void MulticastAction(EGamerAnimAction Kind, bool bPredicted);

	/** Pile of things close together (hand-pickable resources and ground items): candidates around the aimed thing, and the chosen one. */
	const TArray<TWeakObjectPtr<AActor>>& GetPile() const { return Pile; }
	int32 GetPilePick() const { return PilePick; }
	/** Key "switch in pile": the next candidate becomes the target of the interact key. */
	void CycleFocus();

	/** True while the silhouette outline (post-process material) is set up; then the HUD draws no rectangle. */
	bool HasOutline() const { return OutlineMaterial != nullptr; }
	/** The camera's contour material (the scan drives its magic parameters). */
	UMaterialInstanceDynamic* GetOutlineMaterial() const { return OutlineMaterial; }

	/** Mouse wheel: next (+1) or previous (-1) hotbar slot, wrapping around. */
	void CycleHotbar(int32 Direction);

	/**
	 * Local player: the action of the strike key, as a real click does it (also used by tests).
	 * Takes the matching tool from the hotbar into the hand first (best axe at a tree, pickaxe at a rock, hammer at a plan).
	 */
	void PerformStrike();

	/** Test helper: walk straight forward for the given time. Only acts on the locally controlled character. */
	void StartTestWalk(float Seconds);

	UGamerInventoryComponent* GetInventory() const { return Inventory; }
	UGamerWorkComponent* GetWork() const { return Work; }
	UGamerBuildComponent* GetBuild() const { return Build; }
	UGamerScanComponent* GetScan() const { return Scan; }
	UGamerListenComponent* GetListen() const { return Listen; }
	UGamerHealthComponent* GetHealth() const { return Health; }
	UGamerEquipmentComponent* GetEquipment() const { return Equipment; }
	UGamerProgressComponent* GetProgress() const { return Progress; }
	UGamerStaminaComponent* GetStamina() const { return Stamina; }
	/** Sets the walk speed from sprint state and stamina (the stamina component calls it when it runs empty or recovers). */
	void ApplyMoveSpeed();
	virtual void OnJumped_Implementation() override;

	/** Server: damages by the height fallen (the health component). */
	virtual void Landed(const FHitResult& Hit) override;
	class UGamerBuffComponent* GetBuffs() const { return Buffs; }

	/** Camera position and direction through the crosshair (for aiming at resources and building spots). */
	void GetAimView(FVector& OutLocation, FVector& OutDirection) const;
	float GetCameraDistance() const { return CameraDistance; }

	/** Local player only: the building piece under the crosshair within reach (for hammering, tearing down, HUD). */
	AGamerBuilding* GetFocusBuilding() const { return FocusBuilding.Get(); }

	/** Local player only: the resource node in front of the camera and within reach (for the HUD hint). */
	AGamerResourceNode* GetFocusNode() const { return FocusNode.Get(); }
	class AGamerGroundItem* GetFocusGroundItem() const { return FocusGroundItem.Get(); }

	/** Local player only: the crafting menu (recipes by hand plus those of stations nearby). */
	bool IsCraftMenuOpen() const { return bCraftMenuOpen; }
	/** Equipment window open: number keys 1-9 put on / take off the piece of that place. */
	bool IsGearMenuOpen() const { return bGearMenuOpen; }
	/** Tests: open or close the equipment window without the key. */
	void SetGearMenuOpen(bool bOpen) { bGearMenuOpen = bOpen; if (bOpen) { CloseCraftMenu(); } }
	const TArray<FName>& GetHandRecipes() const { return HandRecipes; }

	/** Station window: E at a finished station opens the crafting menu with only that station's recipes (null = normal menu). */
	AGamerBuilding* GetStationWindow() const { return StationWindow.Get(); }
	void OpenStationWindow(AGamerBuilding* Station);
	/** Starts a recipe (server checks it); at a station the figure first turns to it. Used by keys and tests. */
	void RequestCraft(FName RecipeId);
	bool HasStationRecipes(const AGamerBuilding* Station) const;
	void CloseCraftMenu();

	/** Local player only: world time of the last swing (for the HUD). */
	double GetLastStrikeTime() const { return LastStrikeTime; }

	/** Local player: (re)map all keys from UGamerKeyBindings (after a change in the menu). */
	void ApplyKeyBindings();

	/** Local player: switch movement, looking and actions off while the menu is open. */
	void SetGameplayInputEnabled(bool bEnabled);

	/** Current keyboard/mouse mapping as text, for logs and tests ("IA_Jump=SpaceBar ..."). */
	FString DescribeKeyBindings() const;

protected:
	UPROPERTY(VisibleAnywhere, Category = "Gameplay")
	TObjectPtr<UGamerInventoryComponent> Inventory;

	UPROPERTY(VisibleAnywhere, Category = "Gameplay")
	TObjectPtr<UGamerWorkComponent> Work;

	UPROPERTY(VisibleAnywhere, Category = "Gameplay")
	TObjectPtr<UGamerBuildComponent> Build;

	UPROPERTY(VisibleAnywhere, Category = "Gameplay")
	TObjectPtr<UGamerScanComponent> Scan;

	UPROPERTY(VisibleAnywhere, Category = "Gameplay")
	TObjectPtr<UGamerListenComponent> Listen;

	UPROPERTY(VisibleAnywhere, Category = "Gameplay")
	TObjectPtr<UGamerHealthComponent> Health;

	UPROPERTY(VisibleAnywhere, Category = "Gameplay")
	TObjectPtr<UGamerEquipmentComponent> Equipment;

	UPROPERTY(VisibleAnywhere, Category = "Gameplay")
	TObjectPtr<UGamerProgressComponent> Progress;

	UPROPERTY()
	TObjectPtr<UGamerStaminaComponent> Stamina;

	/** Highest point of the current fall (server), for the fall damage. */
	float FallApexZ = 0.f;

	/** Meal buffs (Taverne-Session, decision 2026-11-14). */
	UPROPERTY(VisibleAnywhere, Category = "Gameplay")
	TObjectPtr<class UGamerBuffComponent> Buffs;

	UPROPERTY(VisibleAnywhere, Category = "Camera")
	TObjectPtr<USpringArmComponent> CameraBoom;

	UPROPERTY(VisibleAnywhere, Category = "Camera")
	TObjectPtr<UCameraComponent> FollowCamera;

	/** Interim figures on the UE5 skeleton (Epic Manny/Quinn) until the MetaHuman bodies exist. */
	UPROPERTY()
	TObjectPtr<class USkeletalMesh> BodyStrong;

	UPROPERTY()
	TObjectPtr<class USkeletalMesh> BodySlim;

	/** MetaHuman head (follows the body pose); empty while the interim figures are used. */
	UPROPERTY(VisibleAnywhere, Category = "Visual")
	TObjectPtr<class USkeletalMeshComponent> FaceMesh;

	/** Hair, beard, eyebrows of the MetaHuman figure (groom components, created on demand). */
	UPROPERTY(Transient)
	TArray<TObjectPtr<class USceneComponent>> GroomComponents;

	/** Own Norse clothing on the MetaHuman body (GamerFigures::ApplyClothing, created on demand). */
	UPROPERTY(Transient)
	TObjectPtr<class USkeletalMeshComponent> ClothMesh;

	/** Cloak layer over the clothing (GamerFigures::ApplyClothing "Umhang"). */
	UPROPERTY(Transient)
	TObjectPtr<class USkeletalMeshComponent> CloakMesh;

	// Tunable values. Read from [/Script/Gamer.GamerCharacter] in DefaultGame.ini;
	// the numbers here are only fallbacks. Server and clients must use the same values.
	UPROPERTY(Config, EditDefaultsOnly, Category = "Movement")
	float WalkSpeed = 450.f;

	// Jump: a higher start speed together with stronger gravity gives a short, snappy jump
	// (about 1.25 m high, 0.7 s in the air). 500 / 1.0 felt floaty in the first play test.
	UPROPERTY(Config, EditDefaultsOnly, Category = "Movement")
	float JumpVelocity = 700.f;

	/** Running speed while the sprint key is held (no stamina). */
	UPROPERTY(Config, EditDefaultsOnly, Category = "Movement")
	float SprintSpeed = 750.f;

	/** Gathering by hand / picking up: walk up to this distance (cm, horizontal, to the target's origin) before grabbing. */
	UPROPERTY(Config, EditDefaultsOnly, Category = "Movement")
	float GrabDistance = 70.f;

	/** Give up walking up after this long (blocked by the bush itself, a slope ...) and grab from where we stand. */
	UPROPERTY(Config, EditDefaultsOnly, Category = "Movement")
	float GrabApproachSeconds = 1.5f;

	/** Crafting at a station: walk up until this close to the station's outline (cm, horizontal), then work. */
	UPROPERTY(Config, EditDefaultsOnly, Category = "Movement")
	float StationWorkDistance = 55.f;

	/** Give up walking to the station after this long and work from where we stand (the server allows its reach). */
	UPROPERTY(Config, EditDefaultsOnly, Category = "Movement")
	float StationApproachSeconds = 3.f;

	UPROPERTY(Config, EditDefaultsOnly, Category = "Movement")
	float GravityScale = 2.f;

	UPROPERTY(Config, EditDefaultsOnly, Category = "Camera")
	float CameraDistance = 400.f;

	/** Over-the-shoulder offset (right, up) so the own body does not hide the spot under the crosshair. */
	UPROPERTY(Config, EditDefaultsOnly, Category = "Camera")
	FVector CameraShoulderOffset = FVector(0.f, 60.f, 60.f);

	UPROPERTY(Config, EditDefaultsOnly, Category = "Camera")
	float LookSensitivity = 1.f;

private:
	void BuildInput();
	void ApplyPlayerColor();
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void Interact();
	void Strike();
	void DoScan();
	void DoListen();
	void InteractPressed();
	void InteractReleased();
	/** What the pick-up-all key combination acts on: the aimed thing of the pile. */
	AActor* GetPickAnchor() const;
	bool bInteractDown = false;
	bool bReviveHeld = false;
	bool bHoldFired = false;
	double InteractDownTime = 0.0;
	void SprintStart() { SetSprinting(true); }
	void SprintStop() { SetSprinting(false); }
	void HotbarNext() { CycleHotbar(1); }
	void HotbarPrev() { CycleHotbar(-1); }
	
	/** Selects the best hotbar tool of that type if it is not already in hand. True if one is (now) in hand. */
	bool TakeToolInHand(FName ToolType, int32 MinTier);

	UFUNCTION(Server, Reliable)
	void ServerSetSprinting(bool bOn);

	UFUNCTION()
	void OnRep_Sprinting();

	UPROPERTY(ReplicatedUsing = OnRep_Sprinting)
	bool bSprinting = false;

	/** Last one-shot work action (server -> everyone); LocalAction is what this machine's animation plays. */
	UPROPERTY(ReplicatedUsing = OnRep_ActionPulse)
	FGamerActionPulse ActionPulse;

	UFUNCTION()
	void OnRep_ActionPulse();

	FGamerActionPulse LocalAction;

	/** Local player: walking up to a hand-gatherable node or ground item before grabbing it. */
	TWeakObjectPtr<AActor> PendingGrab;
	float PendingGrabTime = 0.f;
	float PendingGrabBestDistance = 0.f;
	void TickGrab(float DeltaSeconds);
	void FinishGrab();

	/** Local player: walking up to a station before crafting there (RequestCraft). */
	TWeakObjectPtr<const AActor> PendingCraftStation;
	FName PendingCraftRecipe;
	float PendingCraftTime = 0.f;
	float PendingCraftBestDistance = 0.f;
	void TickCraftApproach(float DeltaSeconds);
	FName AppliedBodyLook;
	void ApplyBodyLook();

	UPROPERTY(Transient)
	TObjectPtr<UInputAction> FocusNextAction;

	UPROPERTY(Transient)
	TObjectPtr<UInputAction> ScanAction;

	UPROPERTY(Transient)
	TObjectPtr<UInputAction> ListenAction;

	UPROPERTY(Transient)
	TObjectPtr<UInputAction> SprintAction;

	UPROPERTY(Transient)
	TObjectPtr<UInputAction> HotbarNextAction;

	UPROPERTY(Transient)
	TObjectPtr<UInputAction> HotbarPrevAction;
	void OpenBuildMenu();
	void RotatePiece();
	void Demolish();
	void OpenVillageMenu();
	void ToggleCraftMenu();
	void ToggleGearMenu();
	void OpenInventoryWindow();
	/** Number keys: pick a recipe while the crafting menu is open, otherwise select a hotbar slot. */
	void NumberKey(const FInputActionValue& Value);
	void UpdateFocus();
	void UpdateFocusRaw();
	void BuildPile();
	/** Local player: the aimed target gets the custom stencil, the camera's post-process material draws its contour. */
	void UpdateOutline();
	void SetupOutline();
	void SetOutlined(AActor* Actor, bool bOn);
	bool bOutlineSetupTried = false;
	TWeakObjectPtr<AActor> OutlineTarget;

	UPROPERTY(Transient)
	TObjectPtr<UMaterialInstanceDynamic> OutlineMaterial;
	void RefreshHandRecipes();

	UPROPERTY(Transient)
	TObjectPtr<UInputAction> InteractAction;

	UPROPERTY(Transient)
	TObjectPtr<UInputAction> CraftMenuAction;

	UPROPERTY(Transient)
	TObjectPtr<UInputAction> GearMenuAction;

	UPROPERTY()
	TObjectPtr<UInputAction> InventoryMenuAction;

	UPROPERTY(Transient)
	TObjectPtr<UInputAction> NumberAction;

	UPROPERTY(Transient)
	TObjectPtr<UInputAction> StrikeAction;

	UPROPERTY(Transient)
	TObjectPtr<UInputAction> BuildMenuAction;

	UPROPERTY(Transient)
	TObjectPtr<UInputAction> RotateAction;

	UPROPERTY(Transient)
	TObjectPtr<UInputAction> DemolishAction;

	UPROPERTY(Transient)
	TObjectPtr<UInputAction> VillageMenuAction;

	TWeakObjectPtr<AGamerResourceNode> FocusNode;
	TWeakObjectPtr<class AGamerGroundItem> FocusGroundItem;
	TWeakObjectPtr<AGamerBuilding> FocusBuilding;
	double LastStrikeTime = -1000.0;
	bool bCraftMenuOpen = false;
	bool bGearMenuOpen = false;
	TArray<FName> HandRecipes;
	TWeakObjectPtr<AGamerBuilding> StationWindow;
	TArray<TWeakObjectPtr<AActor>> Pile;
	TWeakObjectPtr<AActor> PileLeader;
	int32 PilePick = 0;

	UPROPERTY(Transient)
	TObjectPtr<UInputMappingContext> MappingContext;

	UPROPERTY(Transient)
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(Transient)
	TObjectPtr<UInputAction> LookAction;

	UPROPERTY(Transient)
	TObjectPtr<UInputAction> JumpAction;

	float TestWalkRemaining = 0.f;
	float PositionLogTimer = 0.f;
};
