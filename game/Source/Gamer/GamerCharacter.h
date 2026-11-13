#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GamerCharacter.generated.h"

class UCameraComponent;
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

protected:
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

	UPROPERTY(Config, EditDefaultsOnly, Category = "Movement")
	float JumpVelocity = 500.f;

	UPROPERTY(Config, EditDefaultsOnly, Category = "Camera")
	float CameraDistance = 400.f;

	UPROPERTY(Config, EditDefaultsOnly, Category = "Camera")
	float LookSensitivity = 1.f;

private:
	void BuildInput();
	void ApplyPlayerColor();
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);

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
