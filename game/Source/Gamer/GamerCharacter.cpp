#include "GamerCharacter.h"

#include "Gamer.h"
#include "GamerBuildComponent.h"
#include "GamerBuffComponent.h"
#include "GamerBuilding.h"
#include "GamerDataSubsystem.h"
#include "GamerInventoryComponent.h"
#include "GamerKeyBindings.h"
#include "GamerPlayerController.h"
#include "GamerFarmBuilding.h"
#include "GamerGroundItem.h"
#include "GamerResourceNode.h"
#include "GamerScanComponent.h"
#include "GamerWorkComponent.h"
#include "EngineUtils.h"
#include "Engine/World.h"
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
#include "Net/UnrealNetwork.h"
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

	Inventory = CreateDefaultSubobject<UGamerInventoryComponent>(TEXT("Inventory"));
	Work = CreateDefaultSubobject<UGamerWorkComponent>(TEXT("Work"));
	Build = CreateDefaultSubobject<UGamerBuildComponent>(TEXT("Build"));
	Scan = CreateDefaultSubobject<UGamerScanComponent>(TEXT("Scan"));
	Buffs = CreateDefaultSubobject<UGamerBuffComponent>(TEXT("Buffs"));

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
	ApplyMoveSpeed();
	GetCharacterMovement()->JumpZVelocity = JumpVelocity;
	GetCharacterMovement()->GravityScale = GravityScale;
	CameraBoom->TargetArmLength = CameraDistance;
	CameraBoom->SocketOffset = CameraShoulderOffset;

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

	if (IsLocallyControlled())
	{
		UpdateFocus();
		// Holding the interact key for half a second picks up everything within 3 m (the first press already did one).
		if (bInteractDown && !bHoldFired && GetWorld()->GetTimeSeconds() - InteractDownTime >= 0.5 && !bCraftMenuOpen)
		{
			bHoldFired = true;
			Work->ServerPickUpAll(GetPickAnchor(), false);
		}
		// The station window closes when the station is gone or too far away.
		if (bCraftMenuOpen && (StationWindow.IsValid() || StationWindow.IsStale()))
		{
			const AGamerBuilding* Station = StationWindow.Get();
			if (!Station || FVector::Dist(Station->GetActorLocation(), GetActorLocation()) > Work->GetStationReach() + 100.f)
			{
				CloseCraftMenu();
			}
		}
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

	auto MakeAction = [this](const TCHAR* Name, EInputActionValueType Type)
	{
		UInputAction* Action = NewObject<UInputAction>(this, Name);
		Action->ValueType = Type;
		return Action;
	};
	MoveAction = MakeAction(TEXT("IA_Move"), EInputActionValueType::Axis2D);
	LookAction = MakeAction(TEXT("IA_Look"), EInputActionValueType::Axis2D);
	JumpAction = MakeAction(TEXT("IA_Jump"), EInputActionValueType::Boolean);
	InteractAction = MakeAction(TEXT("IA_Interact"), EInputActionValueType::Boolean);
	StrikeAction = MakeAction(TEXT("IA_Strike"), EInputActionValueType::Boolean);
	CraftMenuAction = MakeAction(TEXT("IA_CraftMenu"), EInputActionValueType::Boolean);
	// Number keys: one action, the number arrives as the value (hotbar slot, or recipe while crafting).
	NumberAction = MakeAction(TEXT("IA_Number"), EInputActionValueType::Axis1D);
	BuildMenuAction = MakeAction(TEXT("IA_BuildMenu"), EInputActionValueType::Boolean);
	RotateAction = MakeAction(TEXT("IA_Rotate"), EInputActionValueType::Boolean);
	DemolishAction = MakeAction(TEXT("IA_Demolish"), EInputActionValueType::Boolean);
	VillageMenuAction = MakeAction(TEXT("IA_VillageMenu"), EInputActionValueType::Boolean);
	SprintAction = MakeAction(TEXT("IA_Sprint"), EInputActionValueType::Boolean);
	FocusNextAction = MakeAction(TEXT("IA_FocusNext"), EInputActionValueType::Boolean);
	ScanAction = MakeAction(TEXT("IA_Scan"), EInputActionValueType::Boolean);
	HotbarNextAction = MakeAction(TEXT("IA_HotbarNext"), EInputActionValueType::Boolean);
	HotbarPrevAction = MakeAction(TEXT("IA_HotbarPrev"), EInputActionValueType::Boolean);

	ApplyKeyBindings();
}

void AGamerCharacter::ApplyKeyBindings()
{
	if (!MappingContext)
	{
		return;
	}
	MappingContext->UnmapAll();
	const UGamerKeyBindings* Keys = UGamerKeyBindings::Get();

	// Move keys onto one 2D axis: X = right, Y = forward.
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
	MapMoveKey(Keys->GetKey(GamerInputActions::MoveForward), true, false);
	MapMoveKey(Keys->GetKey(GamerInputActions::MoveBack), true, true);
	MapMoveKey(Keys->GetKey(GamerInputActions::MoveRight), false, false);
	MapMoveKey(Keys->GetKey(GamerInputActions::MoveLeft), false, true);
	MappingContext->MapKey(MoveAction, EKeys::Gamepad_Left2D);

	// Mouse/stick up should look up: invert Y only. Not rebindable.
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

	// Keyboard/mouse keys from the player's bindings; gamepad buttons fixed.
	MappingContext->MapKey(JumpAction, Keys->GetKey(GamerInputActions::Jump));
	MappingContext->MapKey(JumpAction, EKeys::Gamepad_FaceButton_Bottom);
	MappingContext->MapKey(InteractAction, Keys->GetKey(GamerInputActions::Interact));
	MappingContext->MapKey(InteractAction, EKeys::Gamepad_FaceButton_Left);
	MappingContext->MapKey(StrikeAction, Keys->GetKey(GamerInputActions::Strike));
	MappingContext->MapKey(StrikeAction, EKeys::Gamepad_RightTrigger);
	MappingContext->MapKey(CraftMenuAction, Keys->GetKey(GamerInputActions::CraftMenu));
	MappingContext->MapKey(CraftMenuAction, EKeys::Gamepad_FaceButton_Top);
	MappingContext->MapKey(BuildMenuAction, Keys->GetKey(GamerInputActions::BuildMenu));
	MappingContext->MapKey(BuildMenuAction, EKeys::Gamepad_LeftTrigger);
	MappingContext->MapKey(RotateAction, Keys->GetKey(GamerInputActions::Rotate));
	MappingContext->MapKey(RotateAction, EKeys::Gamepad_RightShoulder);
	MappingContext->MapKey(DemolishAction, Keys->GetKey(GamerInputActions::Demolish));
	MappingContext->MapKey(DemolishAction, EKeys::Gamepad_FaceButton_Right);
	MappingContext->MapKey(VillageMenuAction, Keys->GetKey(GamerInputActions::VillageMenu));
	MappingContext->MapKey(VillageMenuAction, EKeys::Gamepad_Special_Left);
	MappingContext->MapKey(ScanAction, Keys->GetKey(GamerInputActions::Scan));
	MappingContext->MapKey(ScanAction, EKeys::Gamepad_DPad_Down);
	MappingContext->MapKey(FocusNextAction, Keys->GetKey(GamerInputActions::FocusNext));
	MappingContext->MapKey(FocusNextAction, EKeys::Gamepad_DPad_Up);
	MappingContext->MapKey(SprintAction, Keys->GetKey(GamerInputActions::Sprint));
	MappingContext->MapKey(SprintAction, EKeys::Gamepad_LeftThumbstick);
	MappingContext->MapKey(HotbarNextAction, Keys->GetKey(GamerInputActions::HotbarNext));
	MappingContext->MapKey(HotbarNextAction, EKeys::Gamepad_DPad_Right);
	MappingContext->MapKey(HotbarPrevAction, Keys->GetKey(GamerInputActions::HotbarPrev));
	MappingContext->MapKey(HotbarPrevAction, EKeys::Gamepad_DPad_Left);

	auto MapNumber = [this](const FKey& Key, int32 Number)
	{
		FEnhancedActionKeyMapping& Mapping = MappingContext->MapKey(NumberAction, Key);
		UInputModifierScalar* Scalar = NewObject<UInputModifierScalar>(MappingContext);
		Scalar->Scalar = FVector(Number, 1.f, 1.f);
		Mapping.Modifiers.Add(Scalar);
	};
	for (int32 Number = 1; Number <= 8; ++Number)
	{
		MapNumber(Keys->GetKey(GamerInputActions::Hotbar(Number)), Number);
	}
	MapNumber(EKeys::Nine, 9); // 9th recipe only, not rebindable

	if (const APlayerController* PC = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
		{
			Subsystem->RequestRebuildControlMappings();
		}
	}
}

void AGamerCharacter::SetGameplayInputEnabled(bool bEnabled)
{
	const APlayerController* PC = Cast<APlayerController>(Controller);
	UEnhancedInputLocalPlayerSubsystem* Subsystem = PC ? ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()) : nullptr;
	if (!Subsystem || !MappingContext)
	{
		return;
	}
	if (bEnabled)
	{
		Subsystem->AddMappingContext(MappingContext, 0);
	}
	else
	{
		Subsystem->RemoveMappingContext(MappingContext);
		SetSprinting(false); // the key release would not arrive any more
	}
}

FString AGamerCharacter::DescribeKeyBindings() const
{
	TArray<FString> Parts;
	if (MappingContext)
	{
		for (const FEnhancedActionKeyMapping& Mapping : MappingContext->GetMappings())
		{
			if (Mapping.Action && !Mapping.Key.IsGamepadKey())
			{
				Parts.Add(FString::Printf(TEXT("%s=%s"), *Mapping.Action->GetName(), *Mapping.Key.ToString()));
			}
		}
	}
	return FString::Join(Parts, TEXT(" "));
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
		Input->BindAction(InteractAction, ETriggerEvent::Started, this, &AGamerCharacter::InteractPressed);
		Input->BindAction(InteractAction, ETriggerEvent::Completed, this, &AGamerCharacter::InteractReleased);
		Input->BindAction(CraftMenuAction, ETriggerEvent::Started, this, &AGamerCharacter::ToggleCraftMenu);
		Input->BindAction(NumberAction, ETriggerEvent::Started, this, &AGamerCharacter::NumberKey);
		Input->BindAction(StrikeAction, ETriggerEvent::Triggered, this, &AGamerCharacter::Strike);
		Input->BindAction(BuildMenuAction, ETriggerEvent::Started, this, &AGamerCharacter::OpenBuildMenu);
		Input->BindAction(RotateAction, ETriggerEvent::Started, this, &AGamerCharacter::RotatePiece);
		Input->BindAction(DemolishAction, ETriggerEvent::Started, this, &AGamerCharacter::Demolish);
		Input->BindAction(VillageMenuAction, ETriggerEvent::Started, this, &AGamerCharacter::OpenVillageMenu);
		Input->BindAction(FocusNextAction, ETriggerEvent::Started, this, &AGamerCharacter::CycleFocus);
		Input->BindAction(ScanAction, ETriggerEvent::Started, this, &AGamerCharacter::DoScan);
		Input->BindAction(SprintAction, ETriggerEvent::Started, this, &AGamerCharacter::SprintStart);
		Input->BindAction(SprintAction, ETriggerEvent::Completed, this, &AGamerCharacter::SprintStop);
		Input->BindAction(HotbarNextAction, ETriggerEvent::Started, this, &AGamerCharacter::HotbarNext);
		Input->BindAction(HotbarPrevAction, ETriggerEvent::Started, this, &AGamerCharacter::HotbarPrev);
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

void AGamerCharacter::GetAimView(FVector& OutLocation, FVector& OutDirection) const
{
	OutLocation = FollowCamera ? FollowCamera->GetComponentLocation() : GetActorLocation();
	OutDirection = FollowCamera ? FollowCamera->GetForwardVector() : GetActorForwardVector();
}

void AGamerCharacter::UpdateFocus()
{
	UpdateFocusRaw();
	BuildPile();
}

AActor* AGamerCharacter::GetPickAnchor() const
{
	if (FocusGroundItem.IsValid())
	{
		return FocusGroundItem.Get();
	}
	return FocusNode.Get();
}

void AGamerCharacter::InteractPressed()
{
	bInteractDown = true;
	bHoldFired = false;
	InteractDownTime = GetWorld()->GetTimeSeconds();
	// Shift (or its rebound key) + interact: everything of the aimed type nearby, at once.
	const APlayerController* PC = Cast<APlayerController>(Controller);
	if (PC && PC->IsInputKeyDown(UGamerKeyBindings::Get()->GetKey(GamerInputActions::PickSame)) && GetPickAnchor())
	{
		bHoldFired = true;
		Work->ServerPickUpAll(GetPickAnchor(), true);
		return;
	}
	Interact();
}

void AGamerCharacter::InteractReleased()
{
	bInteractDown = false;
}

void AGamerCharacter::DoScan()
{
	Scan->TryScan();
}

void AGamerCharacter::CycleFocus()
{
	if (Pile.Num() > 1)
	{
		PilePick = (PilePick + 1) % Pile.Num();
		UE_LOG(LogGamer, Log, TEXT("Pile: pick %d of %d (%s)"), PilePick + 1, Pile.Num(), *GetNameSafe(Pile[PilePick].Get()));
	}
}

void AGamerCharacter::BuildPile()
{
	// Leader = what the aim found; candidates = hand-pickable things (no tool needed) and ground items close to it.
	AActor* Leader = FocusGroundItem.IsValid() ? static_cast<AActor*>(FocusGroundItem.Get()) : nullptr;
	if (!Leader)
	{
		const AGamerResourceNode* Node = FocusNode.Get();
		const FGamerResourceNodeDef* Def = Node ? Node->GetDef() : nullptr;
		if (Def && !Def->RequiredTool.IsRequired())
		{
			Leader = FocusNode.Get();
		}
	}
	if (!Leader)
	{
		Pile.Reset();
		PileLeader.Reset();
		PilePick = 0;
		return;
	}
	if (PileLeader.Get() != Leader)
	{
		PileLeader = Leader;
		PilePick = 0;
	}
	const float PileRadius = 150.f;
	const float Reach = Work ? Work->GetMaxReach() : 300.f;
	TArray<TPair<float, AActor*>> Found;
	auto Consider = [&](AActor* Actor)
	{
		const float ToLeader = FVector::Dist2D(Actor->GetActorLocation(), Leader->GetActorLocation());
		if (Actor == Leader || (ToLeader <= PileRadius && FVector::Dist2D(Actor->GetActorLocation(), GetActorLocation()) <= Reach))
		{
			Found.Emplace(Actor == Leader ? -1.f : ToLeader, Actor);
		}
	};
	for (TActorIterator<AGamerGroundItem> It(GetWorld()); It; ++It)
	{
		Consider(*It);
	}
	for (TActorIterator<AGamerResourceNode> It(GetWorld()); It; ++It)
	{
		const FGamerResourceNodeDef* Def = It->GetDef();
		if (!It->IsDepleted() && Def && !Def->RequiredTool.IsRequired())
		{
			Consider(*It);
		}
	}
	Found.Sort([](const TPair<float, AActor*>& A, const TPair<float, AActor*>& B) { return A.Key < B.Key; });
	Pile.Reset();
	for (int32 Index = 0; Index < FMath::Min(6, Found.Num()); ++Index)
	{
		Pile.Add(Found[Index].Value);
	}
	if (Pile.Num() <= 1)
	{
		PilePick = 0;
		return;
	}
	PilePick = FMath::Clamp(PilePick, 0, Pile.Num() - 1);
	// The chosen candidate becomes the target of the interact key.
	AActor* Picked = Pile[PilePick].Get();
	FocusNode = Cast<AGamerResourceNode>(Picked);
	FocusGroundItem = Cast<AGamerGroundItem>(Picked);
}

void AGamerCharacter::UpdateFocusRaw()
{
	FocusNode.Reset();
	FocusBuilding.Reset();
	FocusGroundItem.Reset();
	if (!Work || !FollowCamera)
	{
		return;
	}
	const float Reach = Work->GetMaxReach();
	const FVector CameraLocation = FollowCamera->GetComponentLocation();
	const FVector CameraForward = FollowCamera->GetForwardVector();

	// 1) What the camera looks at.
	FHitResult Hit;
	FCollisionQueryParams Params(SCENE_QUERY_STAT(GamerFocus), false, this);
	const FVector End = CameraLocation + CameraForward * (CameraDistance + Reach + 200.f);
	if (GetWorld()->LineTraceSingleByChannel(Hit, CameraLocation, End, ECC_Visibility, Params))
	{
		// Fallen fruit and other things on the ground (pick up with the interact key).
		AGamerGroundItem* Ground = Cast<AGamerGroundItem>(Hit.GetActor());
		if (Ground && FVector::Dist2D(Ground->GetActorLocation(), GetActorLocation()) <= Reach)
		{
			FocusGroundItem = Ground;
			return;
		}
		AGamerResourceNode* Node = Cast<AGamerResourceNode>(Hit.GetActor());
		if (Node && !Node->IsDepleted() && FVector::Dist2D(Node->GetActorLocation(), GetActorLocation()) <= Reach)
		{
			FocusNode = Node;
			return;
		}
		// Building pieces are big: measure to the point aimed at, not to the pivot.
		AGamerBuilding* Building = Cast<AGamerBuilding>(Hit.GetActor());
		if (Building && !Building->IsPreview() && FVector::Dist(Hit.ImpactPoint, GetActorLocation()) <= Reach + 100.f)
		{
			FocusBuilding = Building;
			return;
		}
	}

	// 2) Small things on the ground are easy to miss with the camera: nearest node in front within reach.
	const FVector Forward2D = FVector(CameraForward.X, CameraForward.Y, 0.f).GetSafeNormal();
	float BestDistance = Reach;
	for (TActorIterator<AGamerResourceNode> It(GetWorld()); It; ++It)
	{
		AGamerResourceNode* Node = *It;
		if (Node->IsDepleted())
		{
			continue;
		}
		const FVector ToNode = FVector(Node->GetActorLocation() - GetActorLocation()) * FVector(1.f, 1.f, 0.f);
		const float Distance = ToNode.Size();
		if (Distance <= BestDistance && FVector::DotProduct(ToNode.GetSafeNormal(), Forward2D) > 0.5f)
		{
			BestDistance = Distance;
			FocusNode = Node;
		}
	}
	// Ground items are tiny: the nearest one in front wins when it is closer than the nearest node.
	for (TActorIterator<AGamerGroundItem> It(GetWorld()); It; ++It)
	{
		const FVector ToItem = FVector(It->GetActorLocation() - GetActorLocation()) * FVector(1.f, 1.f, 0.f);
		const float Distance = ToItem.Size();
		if (Distance <= FMath::Min(BestDistance, Reach * 0.6f) && FVector::DotProduct(ToItem.GetSafeNormal(), Forward2D) > 0.5f)
		{
			BestDistance = Distance;
			FocusGroundItem = *It;
			FocusNode.Reset();
		}
	}
}

void AGamerCharacter::Interact()
{
	if (Work->IsBusy())
	{
		return; // already gathering or crafting; the progress bar shows it
	}
	// The settlement storage: open it (every finished storage building is an access point to the same storage).
	if (const AGamerBuilding* Building = FocusBuilding.Get(); Building && Building->IsFinished() && Building->GetBuildableId() == TEXT("building_storage"))
	{
		if (AGamerPlayerController* PC = Cast<AGamerPlayerController>(Controller))
		{
			PC->OpenMenu(EGamerMenuPage::Storage);
		}
		return;
	}
	// Work buildings with upgrade stages: the interact key upgrades them (decision 2026-11-14).
	if (AGamerBuilding* Building = FocusBuilding.Get(); Building && Building->IsFinished() && Building->GetNextUpgrade()
		&& !Cast<AGamerFarmBuilding>(Building))
	{
		Build->ServerUpgrade(Building);
		return;
	}
	// Fields, composter, rain catcher: the building decides what the key does (sow, water, harvest, take ...).
	if (AGamerFarmBuilding* Farm = Cast<AGamerFarmBuilding>(FocusBuilding.Get()); Farm && Farm->IsFinished())
	{
		Work->ServerUseFarm(Farm);
		return;
	}
	if (AGamerGroundItem* Ground = FocusGroundItem.Get())
	{
		Work->ServerPickUp(Ground);
		return;
	}
	// A finished station with recipes of its own: E opens (or closes) its station window.
	if (AGamerBuilding* Station = FocusBuilding.Get(); Station && Station->IsFinished() && !FocusNode.IsValid())
	{
		if (StationWindow.Get() == Station)
		{
			CloseCraftMenu();
			return;
		}
		if (HasStationRecipes(Station))
		{
			OpenStationWindow(Station);
			return;
		}
	}
	if (AGamerResourceNode* Node = FocusNode.Get())
	{
		Work->ServerHarvest(Node);
	}
}

bool AGamerCharacter::HasStationRecipes(const AGamerBuilding* Station) const
{
	const UGamerDataSubsystem* Data = UGamerDataSubsystem::Get(this);
	if (!Station || !Data)
	{
		return false;
	}
	for (const TPair<FName, FGamerRecipeDef>& Pair : Data->GetData().Recipes)
	{
		if (!Pair.Value.bDeprecated && Pair.Value.Station == Station->GetBuildableId())
		{
			return true;
		}
	}
	return false;
}

void AGamerCharacter::OpenStationWindow(AGamerBuilding* Station)
{
	if (!Station || !Station->IsFinished())
	{
		return;
	}
	StationWindow = Station;
	bCraftMenuOpen = true;
	HandRecipes.Reset();
	if (const UGamerDataSubsystem* Data = UGamerDataSubsystem::Get(this))
	{
		for (const TPair<FName, FGamerRecipeDef>& Pair : Data->GetData().Recipes)
		{
			if (!Pair.Value.bDeprecated && Pair.Value.Station == Station->GetBuildableId())
			{
				HandRecipes.Add(Pair.Key);
			}
		}
	}
	HandRecipes.Sort(FNameLexicalLess());
	UE_LOG(LogGamer, Log, TEXT("Station window opened: %s with %d recipes"), *Station->GetBuildableId().ToString(), HandRecipes.Num());
}

void AGamerCharacter::CloseCraftMenu()
{
	bCraftMenuOpen = false;
	StationWindow.Reset();
}

void AGamerCharacter::RefreshHandRecipes()
{
	// Hand recipes first (sorted), then those of finished stations nearby (work bench, camp fire).
	HandRecipes.Reset();
	TArray<FName> StationRecipes;
	if (const UGamerDataSubsystem* Data = UGamerDataSubsystem::Get(this))
	{
		for (const TPair<FName, FGamerRecipeDef>& Pair : Data->GetData().Recipes)
		{
			if (Pair.Value.bDeprecated)
			{
				continue;
			}
			if (Pair.Value.Station.IsNone())
			{
				HandRecipes.Add(Pair.Key);
			}
			else if (Work->FindStationNearby(Pair.Value.Station))
			{
				StationRecipes.Add(Pair.Key);
			}
		}
	}
	HandRecipes.Sort(FNameLexicalLess());
	StationRecipes.Sort(FNameLexicalLess());
	HandRecipes.Append(StationRecipes);
}

void AGamerCharacter::ToggleCraftMenu()
{
	if (bCraftMenuOpen)
	{
		CloseCraftMenu();
		return;
	}
	StationWindow.Reset();
	bCraftMenuOpen = true;
	RefreshHandRecipes();
}

void AGamerCharacter::NumberKey(const FInputActionValue& Value)
{
	const int32 Index = FMath::RoundToInt(Value.Get<float>()) - 1;
	if (bCraftMenuOpen)
	{
		if (HandRecipes.IsValidIndex(Index))
		{
			Work->ServerCraft(HandRecipes[Index]);
		}
		return;
	}
	Inventory->SelectSlot(Index);
}

void AGamerCharacter::Strike()
{
	// Held button fires every frame: one action per interval. The server checks the timing again.
	const double Now = GetWorld()->GetTimeSeconds();
	if (Now - LastStrikeTime < Work->GetStrikeInterval())
	{
		return;
	}
	LastStrikeTime = Now;
	PerformStrike();
}

bool AGamerCharacter::TakeToolInHand(FName ToolType, int32 MinTier)
{
	const int32 Best = Inventory->FindToolInHotbar(ToolType, MinTier);
	if (Best != INDEX_NONE && Best != Inventory->GetSelectedSlot())
	{
		// Same actor channel as the strike RPC that follows, so the server sees the new slot first.
		Inventory->SelectSlot(Best);
		UE_LOG(LogGamer, Log, TEXT("Auto tool: %s from hotbar slot %d"), *ToolType.ToString(), Best + 1);
	}
	return Best != INDEX_NONE;
}

void AGamerCharacter::PerformStrike()
{
	// A dish or drink in hand: eat it (meal buffs, Taverne-Session; architecture rule 5).
	if (const FGamerItemDef* Held = Inventory->GetSelectedItemDef(); Held && Held->Food.IsMeal())
	{
		Buffs->ServerEat(Held->Id);
		return;
	}
	// An unfinished plan under the crosshair: take the hammer and hammer it.
	AGamerBuilding* Building = FocusBuilding.Get();
	if (Building && !Building->IsFinished() && TakeToolInHand(TEXT("hammer"), 1))
	{
		Build->ServerHammer(Building);
		return;
	}
	// Placing a chosen piece (hammer in hand). After the click the piece leaves the hand,
	// unless the "build several" key is held (decision 2026-11-14).
	if (Inventory->IsHoldingTool(TEXT("hammer"), 1) && Build->IsPlacing())
	{
		bool bKeep = false;
		if (const APlayerController* PC = Cast<APlayerController>(Controller))
		{
			bKeep = PC->IsInputKeyDown(UGamerKeyBindings::Get()->GetKey(GamerInputActions::BuildMulti));
		}
		if (Build->TryPlace())
		{
			Build->AfterPlaced(bKeep);
		}
		return;
	}
	// A tree, rock or stump: take the best matching tool from the hotbar (the server does the same as a fallback).
	if (const AGamerResourceNode* Node = FocusNode.Get())
	{
		if (const FGamerResourceNodeDef* Def = Node->GetDef(); Def && Def->RequiredTool.IsRequired())
		{
			TakeToolInHand(Def->RequiredTool.Type, Def->RequiredTool.MinTier);
		}
	}
	Work->ServerStrike(FocusNode.Get());
}

void AGamerCharacter::CycleHotbar(int32 Direction)
{
	const int32 Size = Inventory->GetHotbarSize();
	if (Size <= 0)
	{
		return;
	}
	const int32 Next = ((Inventory->GetSelectedSlot() + Direction) % Size + Size) % Size;
	Inventory->SelectSlot(Next);
	UE_LOG(LogGamer, Log, TEXT("Hotbar: slot %d selected (wheel %+d)"), Next + 1, Direction);
}

void AGamerCharacter::SetSprinting(bool bOn)
{
	if (bSprinting == bOn)
	{
		return;
	}
	bSprinting = bOn;
	ApplyMoveSpeed();
	if (!HasAuthority())
	{
		ServerSetSprinting(bOn);
	}
}

void AGamerCharacter::ServerSetSprinting_Implementation(bool bOn)
{
	bSprinting = bOn;
	ApplyMoveSpeed();
	UE_LOG(LogGamer, Log, TEXT("Sprint %s: speed %.0f"), bOn ? TEXT("on") : TEXT("off"), GetCharacterMovement()->MaxWalkSpeed);
}

void AGamerCharacter::OnRep_Sprinting()
{
	ApplyMoveSpeed();
}

void AGamerCharacter::ApplyMoveSpeed()
{
	GetCharacterMovement()->MaxWalkSpeed = bSprinting ? SprintSpeed : WalkSpeed;
}

void AGamerCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	// The owner sets it itself; the others need it for the right speed of their smoothed copy.
	DOREPLIFETIME_CONDITION(AGamerCharacter, bSprinting, COND_SkipOwner);
}

void AGamerCharacter::OpenBuildMenu()
{
	if (!Inventory->IsHoldingTool(TEXT("hammer"), 1))
	{
		const int32 Slot = Inventory->FindToolInHotbar(TEXT("hammer"), 1);
		Work->ShowMessage(Slot != INDEX_NONE
			? FString::Printf(TEXT("Zum Bauen den Hammer in die Hand nehmen: %s"), *UGamerKeyBindings::ActionKeyName(GamerInputActions::Hotbar(Slot + 1)))
			: FString(TEXT("Zum Bauen brauchst du einen Hammer (Herstellen-Menü).")));
		return;
	}
	if (AGamerPlayerController* PC = Cast<AGamerPlayerController>(Controller))
	{
		PC->OpenMenu(EGamerMenuPage::Build);
	}
}

void AGamerCharacter::RotatePiece()
{
	Build->Rotate();
}

void AGamerCharacter::OpenVillageMenu()
{
	if (AGamerPlayerController* PC = Cast<AGamerPlayerController>(Controller))
	{
		PC->OpenMenu(EGamerMenuPage::Village);
	}
}

void AGamerCharacter::Demolish()
{
	AGamerBuilding* Building = FocusBuilding.Get();
	if (!Building)
	{
		return;
	}
	if (!Inventory->IsHoldingTool(TEXT("hammer"), 1))
	{
		Work->ShowMessage(TEXT("Zum Abreißen den Hammer in die Hand nehmen."));
		return;
	}
	Build->ServerDemolish(Building);
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
