#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GamerCharacter.generated.h"

class AGamerBuilding;
class AGamerResourceNode;
class UGamerBuildComponent;
class UCameraComponent;
class UGamerInventoryComponent;
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

	/** Test helper: walk straight forward for the given time. Only acts on the locally controlled character. */
	void StartTestWalk(float Seconds);

	UGamerInventoryComponent* GetInventory() const { return Inventory; }
	UGamerWorkComponent* GetWork() const { return Work; }
	UGamerBuildComponent* GetBuild() const { return Build; }

	/** Camera position and direction through the crosshair (for aiming at resources and building spots). */
	void GetAimView(FVector& OutLocation, FVector& OutDirection) const;
	float GetCameraDistance() const { return CameraDistance; }

	/** Local player only: the building piece under the crosshair within reach (for hammering, tearing down, HUD). */
	AGamerBuilding* GetFocusBuilding() const { return FocusBuilding.Get(); }

	/** Local player only: the resource node in front of the camera and within reach (for the HUD hint). */
	AGamerResourceNode* GetFocusNode() const { return FocusNode.Get(); }

	/** Local player only: the crafting menu (recipes by hand plus those of stations nearby). */
	bool IsCraftMenuOpen() const { return bCraftMenuOpen; }
	const TArray<FName>& GetHandRecipes() const { return HandRecipes; }

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
	void OpenBuildMenu();
	void RotatePiece();
	void Demolish();
	void OpenVillageMenu();
	void ToggleCraftMenu();
	/** Number keys: pick a recipe while the crafting menu is open, otherwise select a hotbar slot. */
	void NumberKey(const FInputActionValue& Value);
	void UpdateFocus();
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
	TWeakObjectPtr<AGamerBuilding> FocusBuilding;
	double LastStrikeTime = -1000.0;
	bool bCraftMenuOpen = false;
	TArray<FName> HandRecipes;

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
