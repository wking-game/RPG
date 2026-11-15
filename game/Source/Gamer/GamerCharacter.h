#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GamerCharacter.generated.h"

class AGamerBuilding;
class AGamerResourceNode;
class UGamerBuildComponent;
class UCameraComponent;
class UGamerInventoryComponent;
class UGamerScanComponent;
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

	/** Pile of things close together (hand-pickable resources and ground items): candidates around the aimed thing, and the chosen one. */
	const TArray<TWeakObjectPtr<AActor>>& GetPile() const { return Pile; }
	int32 GetPilePick() const { return PilePick; }
	/** Key "switch in pile": the next candidate becomes the target of the interact key. */
	void CycleFocus();

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
	const TArray<FName>& GetHandRecipes() const { return HandRecipes; }

	/** Station window: E at a finished station opens the crafting menu with only that station's recipes (null = normal menu). */
	AGamerBuilding* GetStationWindow() const { return StationWindow.Get(); }
	void OpenStationWindow(AGamerBuilding* Station);
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

	/** Meal buffs (Taverne-Session, decision 2026-11-14). */
	UPROPERTY(VisibleAnywhere, Category = "Gameplay")
	TObjectPtr<class UGamerBuffComponent> Buffs;

	UPROPERTY(VisibleAnywhere, Category = "Camera")
	TObjectPtr<USpringArmComponent> CameraBoom;

	UPROPERTY(VisibleAnywhere, Category = "Camera")
	TObjectPtr<UCameraComponent> FollowCamera;

	UPROPERTY(VisibleAnywhere, Category = "Visual")
	TObjectPtr<UStaticMeshComponent> BodyMesh;

	UPROPERTY(VisibleAnywhere, Category = "Visual")
	TObjectPtr<UStaticMeshComponent> FacingMarker;

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
	void InteractPressed();
	void InteractReleased();
	/** What the pick-up-all key combination acts on: the aimed thing of the pile. */
	AActor* GetPickAnchor() const;
	bool bInteractDown = false;
	bool bHoldFired = false;
	double InteractDownTime = 0.0;
	void SprintStart() { SetSprinting(true); }
	void SprintStop() { SetSprinting(false); }
	void HotbarNext() { CycleHotbar(1); }
	void HotbarPrev() { CycleHotbar(-1); }
	void ApplyMoveSpeed();
	/** Selects the best hotbar tool of that type if it is not already in hand. True if one is (now) in hand. */
	bool TakeToolInHand(FName ToolType, int32 MinTier);

	UFUNCTION(Server, Reliable)
	void ServerSetSprinting(bool bOn);

	UFUNCTION()
	void OnRep_Sprinting();

	UPROPERTY(ReplicatedUsing = OnRep_Sprinting)
	bool bSprinting = false;

	UPROPERTY(Transient)
	TObjectPtr<UInputAction> FocusNextAction;

	UPROPERTY(Transient)
	TObjectPtr<UInputAction> ScanAction;

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
	/** Number keys: pick a recipe while the crafting menu is open, otherwise select a hotbar slot. */
	void NumberKey(const FInputActionValue& Value);
	void UpdateFocus();
	void UpdateFocusRaw();
	void BuildPile();
	void RefreshHandRecipes();

	UPROPERTY(Transient)
	TObjectPtr<UInputAction> InteractAction;

	UPROPERTY(Transient)
	TObjectPtr<UInputAction> CraftMenuAction;

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
