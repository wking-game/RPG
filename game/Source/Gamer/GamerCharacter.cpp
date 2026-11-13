#include "GamerCharacter.h"

#include "Gamer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "Engine/StaticMesh.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/PlayerState.h"
#include "GameFramework/SpringArmComponent.h"
#include "HAL/IConsoleManager.h"
#include "InputAction.h"
#include "InputActionValue.h"
#include "InputMappingContext.h"
#include "InputModifiers.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Misc/CommandLine.h"
#include "Misc/Parse.h"
#include "UObject/ConstructorHelpers.h"

namespace
{
	TAutoConsoleVariable<float> CVarLogPositions(
		TEXT("gamer.LogPositions"),
		0.f,
		TEXT("If > 0, every character writes its position to the log every N seconds (for multiplayer tests). ")
		TEXT("Can also be set with -GamerLogPositions=N on the command line."));

	// Placeholder colours so players can tell each other apart, picked by PlayerId.
	const FLinearColor PlayerColors[] = {
		FLinearColor(0.80f, 0.20f, 0.15f), FLinearColor(0.15f, 0.45f, 0.85f),
		FLinearColor(0.20f, 0.65f, 0.25f), FLinearColor(0.90f, 0.70f, 0.10f),
		FLinearColor(0.55f, 0.25f, 0.75f), FLinearColor(0.10f, 0.70f, 0.70f),
		FLinearColor(0.90f, 0.45f, 0.10f), FLinearColor(0.85f, 0.85f, 0.85f),
	};
}

AGamerCharacter::AGamerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.f);

	// The body turns towards the movement direction; the camera follows the controller.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	UCharacterMovementComponent* Movement = GetCharacterMovement();
	Movement->bOrientRotationToMovement = true;
	Movement->RotationRate = FRotator(0.f, 500.f, 0.f);
	Movement->AirControl = 0.35f;
	Movement->BrakingDecelerationWalking = 2000.f;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->bUsePawnControlRotation = true;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	static ConstructorHelpers::FObjectFinder<UStaticMesh> CylinderMesh(TEXT("/Engine/BasicShapes/Cylinder.Cylinder"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(TEXT("/Engine/BasicShapes/Cube.Cube"));

	// Engine cylinder: 100 x 100 x 100 units, centred. Scaled to the capsule (84 wide, 192 high).
	BodyMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BodyMesh"));
	BodyMesh->SetupAttachment(GetCapsuleComponent());
	BodyMesh->SetRelativeScale3D(FVector(0.84f, 0.84f, 1.92f));
	BodyMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	if (CylinderMesh.Succeeded())
	{
		BodyMesh->SetStaticMesh(CylinderMesh.Object);
	}

	FacingMarker = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FacingMarker"));
	FacingMarker->SetupAttachment(GetCapsuleComponent());
	FacingMarker->SetRelativeLocation(FVector(45.f, 0.f, 60.f));
	FacingMarker->SetRelativeScale3D(FVector(0.3f, 0.2f, 0.2f));
	FacingMarker->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	if (CubeMesh.Succeeded())
	{
		FacingMarker->SetStaticMesh(CubeMesh.Object);
	}
}

void AGamerCharacter::BeginPlay()
{
	Super::BeginPlay();

	// Config values are loaded after the constructor, so apply them here (same on server and clients).
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	GetCharacterMovement()->JumpZVelocity = JumpVelocity;
	CameraBoom->TargetArmLength = CameraDistance;

	float LogInterval = 0.f;
	if (FParse::Value(FCommandLine::Get(), TEXT("GamerLogPositions="), LogInterval))
	{
		CVarLogPositions->Set(LogInterval, ECVF_SetByCommandline);
	}

	ApplyPlayerColor();
}

void AGamerCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (TestWalkRemaining > 0.f && IsLocallyControlled())
	{
		TestWalkRemaining -= DeltaSeconds;
		AddMovementInput(GetActorForwardVector(), 1.f);
	}

	const float LogInterval = CVarLogPositions.GetValueOnGameThread();
	if (LogInterval > 0.f)
	{
		PositionLogTimer += DeltaSeconds;
		if (PositionLogTimer >= LogInterval)
		{
			PositionLogTimer = 0.f;
			const APlayerState* PS = GetPlayerState();
			UE_LOG(LogGamer, Log, TEXT("Pos player=%d role=%s local=%d loc=%s"),
				PS ? PS->GetPlayerId() : -1,
				*UEnum::GetValueAsString(GetLocalRole()),
				IsLocallyControlled() ? 1 : 0,
				*GetActorLocation().ToCompactString());
		}
	}
}

void AGamerCharacter::BuildInput()
{
	if (MappingContext)
	{
		return;
	}

	// Input objects are built in code instead of editor assets (project rule: C++ for systems).
	MappingContext = NewObject<UInputMappingContext>(this, TEXT("IMC_GamerDefault"));

	MoveAction = NewObject<UInputAction>(this, TEXT("IA_Move"));
	MoveAction->ValueType = EInputActionValueType::Axis2D;
	LookAction = NewObject<UInputAction>(this, TEXT("IA_Look"));
	LookAction->ValueType = EInputActionValueType::Axis2D;
	JumpAction = NewObject<UInputAction>(this, TEXT("IA_Jump"));
	JumpAction->ValueType = EInputActionValueType::Boolean;

	// WASD onto one 2D axis: X = right, Y = forward.
	auto MapMoveKey = [this](const FKey& Key, bool bToForwardAxis, bool bNegate)
	{
		FEnhancedActionKeyMapping& Mapping = MappingContext->MapKey(MoveAction, Key);
		if (bToForwardAxis)
		{
			Mapping.Modifiers.Add(NewObject<UInputModifierSwizzleAxis>(MappingContext));
		}
		if (bNegate)
		{
			Mapping.Modifiers.Add(NewObject<UInputModifierNegate>(MappingContext));
		}
	};
	MapMoveKey(EKeys::W, true, false);
	MapMoveKey(EKeys::S, true, true);
	MapMoveKey(EKeys::D, false, false);
	MapMoveKey(EKeys::A, false, true);
	MappingContext->MapKey(MoveAction, EKeys::Gamepad_Left2D);

	// Mouse/stick up should look up: invert Y only.
	auto MapLookKey = [this](const FKey& Key)
	{
		FEnhancedActionKeyMapping& Mapping = MappingContext->MapKey(LookAction, Key);
		UInputModifierNegate* InvertY = NewObject<UInputModifierNegate>(MappingContext);
		InvertY->bX = false;
		InvertY->bY = true;
		InvertY->bZ = false;
		Mapping.Modifiers.Add(InvertY);
	};
	MapLookKey(EKeys::Mouse2D);
	MapLookKey(EKeys::Gamepad_Right2D);

	MappingContext->MapKey(JumpAction, EKeys::SpaceBar);
	MappingContext->MapKey(JumpAction, EKeys::Gamepad_FaceButton_Bottom);
}

void AGamerCharacter::NotifyControllerChanged()
{
	Super::NotifyControllerChanged();

	// Only the owning client has a local player; on the server this does nothing for remote players.
	if (const APlayerController* PC = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
		{
			BuildInput();
			Subsystem->AddMappingContext(MappingContext, 0);
		}
	}
}

void AGamerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	BuildInput();
	if (UEnhancedInputComponent* Input = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		Input->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		Input->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);
		Input->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AGamerCharacter::Move);
		Input->BindAction(LookAction, ETriggerEvent::Triggered, this, &AGamerCharacter::Look);
	}
	else
	{
		UE_LOG(LogGamer, Error, TEXT("Enhanced Input component missing - check DefaultInputComponentClass in DefaultInput.ini."));
	}
}

void AGamerCharacter::Move(const FInputActionValue& Value)
{
	const FVector2D Axis = Value.Get<FVector2D>();
	if (!Controller)
	{
		return;
	}
	const FRotator YawRotation(0.f, Controller->GetControlRotation().Yaw, 0.f);
	const FRotationMatrix YawMatrix(YawRotation);
	AddMovementInput(YawMatrix.GetUnitAxis(EAxis::X), Axis.Y);
	AddMovementInput(YawMatrix.GetUnitAxis(EAxis::Y), Axis.X);
}

void AGamerCharacter::Look(const FInputActionValue& Value)
{
	const FVector2D Axis = Value.Get<FVector2D>();
	AddControllerYawInput(Axis.X * LookSensitivity);
	AddControllerPitchInput(Axis.Y * LookSensitivity);
}

void AGamerCharacter::StartTestWalk(float Seconds)
{
	TestWalkRemaining = FMath::Max(0.f, Seconds);
	UE_LOG(LogGamer, Log, TEXT("Test walk started for %.1f s"), TestWalkRemaining);
}

void AGamerCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	ApplyPlayerColor();
}

void AGamerCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	ApplyPlayerColor();
}

void AGamerCharacter::ApplyPlayerColor()
{
	const APlayerState* PS = GetPlayerState();
	if (!PS || !BodyMesh || !BodyMesh->GetStaticMesh())
	{
		return;
	}
	const FLinearColor Color = PlayerColors[FMath::Abs(PS->GetPlayerId()) % UE_ARRAY_COUNT(PlayerColors)];
	if (UMaterialInstanceDynamic* Material = BodyMesh->CreateAndSetMaterialInstanceDynamic(0))
	{
		// "Color" is the parameter of the engine's BasicShapeMaterial.
		Material->SetVectorParameterValue(TEXT("Color"), Color);
	}
}
