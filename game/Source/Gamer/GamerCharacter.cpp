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
#include "GamerGrave.h"
#include "GamerHealthComponent.h"
#include "GamerEquipmentComponent.h"
#include "GamerProgressComponent.h"
#include "GamerStaminaComponent.h"
#include "GamerListenComponent.h"
#include "GamerScanComponent.h"
#include "GamerWorkComponent.h"
#include "GamerAnimInstance.h"
#include "GamerFigures.h"
#include "GamerPlayerState.h"
#include "Engine/SkeletalMesh.h"
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
#include "Components/PrimitiveComponent.h"
#include "Misc/PackageName.h"
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

	// Interim figure (Manny/Quinn): cloth colour of the character editor as tint. Values (sRGB) from the style
	// design/stil_norden_gezeitenhall.md, table "Kleidungsfarben"; FLinearColor(FColor) converts to linear.
	FLinearColor ClothTint(FName Option)
	{
		static const TMap<FName, FLinearColor> Colors = {
			{ TEXT("cloth_natural"), FLinearColor(FColor::FromHex(TEXT("A99B80"))) }, { TEXT("cloth_grey"), FLinearColor(FColor::FromHex(TEXT("8A8C86"))) },
			{ TEXT("cloth_brown"), FLinearColor(FColor::FromHex(TEXT("716957"))) },   { TEXT("cloth_ochre"), FLinearColor(FColor::FromHex(TEXT("A9844A"))) },
			{ TEXT("cloth_red"), FLinearColor(FColor::FromHex(TEXT("904C3E"))) },     { TEXT("cloth_blue"), FLinearColor(FColor::FromHex(TEXT("4E6475"))) },
			{ TEXT("cloth_green"), FLinearColor(FColor::FromHex(TEXT("56654F"))) },
		};
		const FLinearColor* Found = Colors.Find(Option);
		return Found ? *Found : Colors[TEXT("cloth_natural")];
	}

	EGamerAnimAction ActionForTool(FName ToolType)
	{
		return ToolType == TEXT("axe") ? EGamerAnimAction::Chop
			: ToolType == TEXT("pickaxe") ? EGamerAnimAction::Mine
			: ToolType == TEXT("hammer") ? EGamerAnimAction::Hammer
			: EGamerAnimAction::Gather;
	}
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
	Listen = CreateDefaultSubobject<UGamerListenComponent>(TEXT("Listen"));
	Health = CreateDefaultSubobject<UGamerHealthComponent>(TEXT("Health"));
	Equipment = CreateDefaultSubobject<UGamerEquipmentComponent>(TEXT("Equipment"));
	Progress = CreateDefaultSubobject<UGamerProgressComponent>(TEXT("Progress"));
	Stamina = CreateDefaultSubobject<UGamerStaminaComponent>(TEXT("Stamina"));
	Buffs = CreateDefaultSubobject<UGamerBuffComponent>(TEXT("Buffs"));

	// Figure on the UE5 skeleton: Epic Manny/Quinn for now, MetaHuman bodies later (same skeleton, same animation).
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> MannyMesh(TEXT("/Game/Characters/Mannequins/Meshes/SKM_Manny_Simple.SKM_Manny_Simple"));
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> QuinnMesh(TEXT("/Game/Characters/Mannequins/Meshes/SKM_Quinn_Simple.SKM_Quinn_Simple"));
	BodyStrong = MannyMesh.Object;
	BodySlim = QuinnMesh.Object;
	USkeletalMeshComponent* Body = GetMesh();
	Body->SetSkeletalMesh(BodyStrong);
	Body->SetRelativeLocationAndRotation(FVector(0.f, 0.f, -96.f), FRotator(0.f, -90.f, 0.f));
	Body->SetAnimInstanceClass(UGamerAnimInstance::StaticClass());
	Body->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	// A dedicated server never renders: no pose work there (gameplay does not read bones).
	Body->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::OnlyTickPoseWhenRendered;

	FaceMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FaceMesh"));
	FaceMesh->SetupAttachment(Body);
	FaceMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	FaceMesh->SetLeaderPoseComponent(Body);
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
	ApplyBodyLook(); // cheap when unchanged; the profile can replicate after the player state
	if (!PendingCraftRecipe.IsNone() && IsLocallyControlled())
	{
		TickCraftApproach(DeltaSeconds);
	}
	if (PendingGrab.IsValid() && IsLocallyControlled())
	{
		TickGrab(DeltaSeconds);
	}

	if (TestWalkRemaining > 0.f && IsLocallyControlled())
	{
		TestWalkRemaining -= DeltaSeconds;
		AddMovementInput(GetActorForwardVector(), 1.f);
	}

	if (HasAuthority())
	{
		// Highest point of the fall, so Landed() knows how far it was.
		const float Z = GetActorLocation().Z;
		FallApexZ = GetCharacterMovement()->IsFalling() ? FMath::Max(FallApexZ, Z) : Z;
	}

	if (IsLocallyControlled())
	{
		UpdateFocus();
		UpdateOutline();
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
	GearMenuAction = MakeAction(TEXT("IA_GearMenu"), EInputActionValueType::Boolean);
	InventoryMenuAction = MakeAction(TEXT("IA_InventoryMenu"), EInputActionValueType::Boolean);
	// Number keys: one action, the number arrives as the value (hotbar slot, or recipe while crafting).
	NumberAction = MakeAction(TEXT("IA_Number"), EInputActionValueType::Axis1D);
	BuildMenuAction = MakeAction(TEXT("IA_BuildMenu"), EInputActionValueType::Boolean);
	RotateAction = MakeAction(TEXT("IA_Rotate"), EInputActionValueType::Boolean);
	DemolishAction = MakeAction(TEXT("IA_Demolish"), EInputActionValueType::Boolean);
	VillageMenuAction = MakeAction(TEXT("IA_VillageMenu"), EInputActionValueType::Boolean);
	SprintAction = MakeAction(TEXT("IA_Sprint"), EInputActionValueType::Boolean);
	FocusNextAction = MakeAction(TEXT("IA_FocusNext"), EInputActionValueType::Boolean);
	ScanAction = MakeAction(TEXT("IA_Scan"), EInputActionValueType::Boolean);
	ListenAction = MakeAction(TEXT("IA_Listen"), EInputActionValueType::Boolean);
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
	MappingContext->MapKey(GearMenuAction, Keys->GetKey(GamerInputActions::GearMenu));
	MappingContext->MapKey(InventoryMenuAction, Keys->GetKey(GamerInputActions::InventoryMenu));
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
	MappingContext->MapKey(ListenAction, Keys->GetKey(GamerInputActions::Listen));
	MappingContext->MapKey(ListenAction, EKeys::Gamepad_LeftShoulder);
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
		Input->BindAction(GearMenuAction, ETriggerEvent::Started, this, &AGamerCharacter::ToggleGearMenu);
		Input->BindAction(InventoryMenuAction, ETriggerEvent::Started, this, &AGamerCharacter::OpenInventoryWindow);
		Input->BindAction(NumberAction, ETriggerEvent::Started, this, &AGamerCharacter::NumberKey);
		Input->BindAction(StrikeAction, ETriggerEvent::Triggered, this, &AGamerCharacter::Strike);
		Input->BindAction(BuildMenuAction, ETriggerEvent::Started, this, &AGamerCharacter::OpenBuildMenu);
		Input->BindAction(RotateAction, ETriggerEvent::Started, this, &AGamerCharacter::RotatePiece);
		Input->BindAction(DemolishAction, ETriggerEvent::Started, this, &AGamerCharacter::Demolish);
		Input->BindAction(VillageMenuAction, ETriggerEvent::Started, this, &AGamerCharacter::OpenVillageMenu);
		Input->BindAction(FocusNextAction, ETriggerEvent::Started, this, &AGamerCharacter::CycleFocus);
		Input->BindAction(ScanAction, ETriggerEvent::Started, this, &AGamerCharacter::DoScan);
		Input->BindAction(ListenAction, ETriggerEvent::Started, this, &AGamerCharacter::DoListen);
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
	if (Health && Health->IsIncapacitated())
	{
		return;
	}
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
	// Next to a knocked-down teammate: holding the interact key revives them (the server counts the hold, HUD shows the bar).
	if (Health && !Health->IsIncapacitated())
	{
		if (AActor* Downed = Health->FindDownedNearby())
		{
			bReviveHeld = true;
			Health->ServerSetReviving(Downed, true);
			return;
		}
	}
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
	if (bReviveHeld)
	{
		bReviveHeld = false;
		if (Health)
		{
			Health->ServerSetReviving(nullptr, false);
		}
		return;
	}
	bInteractDown = false;
}

void AGamerCharacter::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);
	if (HasAuthority() && Health)
	{
		Health->NotifyLanded(FallApexZ - GetActorLocation().Z);
		FallApexZ = GetActorLocation().Z;
	}
}

void AGamerCharacter::DoScan()
{
	Scan->TryScan();
}

void AGamerCharacter::DoListen()
{
	Listen->TryListen();
}

void AGamerCharacter::SetOutlined(AActor* Actor, bool bOn)
{
	if (!Actor)
	{
		return;
	}
	TArray<UPrimitiveComponent*> Primitives;
	Actor->GetComponents<UPrimitiveComponent>(Primitives);
	for (UPrimitiveComponent* Primitive : Primitives)
	{
		if (bOn)
		{
			Primitive->SetCustomDepthStencilValue(1);
		}
		Primitive->SetRenderCustomDepth(bOn);
	}
}

void AGamerCharacter::SetupOutline()
{
	bOutlineSetupTried = true;
	// The stencil needs r.CustomDepth 3; set at run time instead of in DefaultEngine.ini (core file).
	if (IConsoleVariable* CustomDepth = IConsoleManager::Get().FindConsoleVariable(TEXT("r.CustomDepth")))
	{
		if (CustomDepth->GetInt() < 3)
		{
			CustomDepth->Set(3, ECVF_SetByConsole);
		}
	}
	const FString Package = TEXT("/Game/Gamer/Materials/M_Outline");
	if (!FPackageName::DoesPackageExist(Package))
	{
		UE_LOG(LogGamer, Warning, TEXT("Outline: %s missing (run tools/unreal/build_outline_material.py), the HUD rectangle is used"), *Package);
		return;
	}
	UMaterialInterface* Base = LoadObject<UMaterialInterface>(nullptr, *(Package + TEXT(".M_Outline")));
	if (!Base || !FollowCamera)
	{
		return;
	}
	OutlineMaterial = UMaterialInstanceDynamic::Create(Base, this);
	FollowCamera->PostProcessSettings.AddBlendable(OutlineMaterial, 1.f);
	FollowCamera->PostProcessBlendWeight = 1.f;
	UE_LOG(LogGamer, Log, TEXT("Outline: post-process material set up"));
}

void AGamerCharacter::UpdateOutline()
{
	if (!bOutlineSetupTried)
	{
		SetupOutline();
	}
	if (!OutlineMaterial)
	{
		return;
	}
	AActor* Target = FocusNode.Get() ? static_cast<AActor*>(FocusNode.Get())
		: FocusGroundItem.Get() ? static_cast<AActor*>(FocusGroundItem.Get())
		: static_cast<AActor*>(FocusBuilding.Get());
	if (Target != OutlineTarget.Get())
	{
		SetOutlined(OutlineTarget.Get(), false);
		OutlineTarget = Target;
	}
	SetOutlined(Target, true);   // also after a mesh change of the target (growth stages)
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
		if (Def && !Def->RequiredTool.IsRequired() && !Node->IsDepleted())
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
		if (Node && (!Node->IsDepleted() || Node->ShowsWhenDepleted()) && FVector::Dist2D(Node->GetActorLocation(), GetActorLocation()) <= Reach)
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
	AGamerResourceNode* Seasonal = nullptr;   // fruit tree out of season: only if nothing can be taken nearby
	float SeasonalDistance = Reach;
	for (TActorIterator<AGamerResourceNode> It(GetWorld()); It; ++It)
	{
		AGamerResourceNode* Node = *It;
		const bool bInfoOnly = Node->IsDepleted();
		if (bInfoOnly && !Node->ShowsWhenDepleted())
		{
			continue;
		}
		const FVector ToNode = FVector(Node->GetActorLocation() - GetActorLocation()) * FVector(1.f, 1.f, 0.f);
		const float Distance = ToNode.Size();
		if (FVector::DotProduct(ToNode.GetSafeNormal(), Forward2D) <= 0.5f)
		{
			continue;
		}
		if (bInfoOnly)
		{
			if (Distance <= SeasonalDistance)
			{
				SeasonalDistance = Distance;
				Seasonal = Node;
			}
		}
		else if (Distance <= BestDistance)
		{
			BestDistance = Distance;
			FocusNode = Node;
		}
	}
	if (!FocusNode.IsValid() && Seasonal)
	{
		FocusNode = Seasonal;
		BestDistance = SeasonalDistance;
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
	// Dead: the interact key gets up again (after the short delay; the HUD shows "Aufstehen").
	if (Health && Health->IsDead())
	{
		Health->ServerRequestRespawn();
		return;
	}
	if (Health && Health->IsDowned())
	{
		return;   // knocked down: only waiting for help
	}
	if (Work->IsBusy())
	{
		return; // already gathering or crafting; the progress bar shows it
	}
	// A grave next to us and nothing else aimed at: open it (the owner at once, teammates after the protection time).
	if (Health && !FocusNode.IsValid() && !FocusGroundItem.IsValid() && !FocusBuilding.IsValid())
	{
		if (AGamerGrave* Grave = Health->FindNearbyGrave())
		{
			Health->ServerCollectGrave(Grave);
			return;
		}
	}
	// The own bed and nothing else aimed at: sleep (brings the exhaustion down).
	{
		// The aimed bed, or (a bed is low and easy to miss with the crosshair) the nearest finished bed right in front of the player.
		AGamerBuilding* Bed = FocusBuilding.Get();
		if (Bed && !(Bed->IsFinished() && Bed->GetBuildableId() == TEXT("building_bed"))) { Bed = nullptr; }
		if (!Bed && Health && !FocusGroundItem.IsValid() && !FocusBuilding.IsValid())
		{
			float Best = 200.f;
			for (TActorIterator<AGamerBuilding> It(GetWorld()); It; ++It)
			{
				const FVector ToBed = It->GetActorLocation() - GetActorLocation();
				const float Distance = FVector(ToBed.X, ToBed.Y, 0.f).Size();
				if (It->IsFinished() && It->GetBuildableId() == TEXT("building_bed") && Distance < Best
					&& FVector::DotProduct(FVector(ToBed.X, ToBed.Y, 0.f).GetSafeNormal(), GetActorForwardVector()) > 0.3f)
				{
					Best = Distance;
					Bed = *It;
				}
			}
		}
		if (Health && Stamina && Bed)
		{
			Stamina->ServerSleepAt(Bed);
			return;
		}
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
		StartGrab(Ground); // walk up, then ServerPickUp
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
	// Nothing to gather in front of you: E eats the food in hand, like the left click (player test 2026-11-22).
	if (const AGamerResourceNode* Node = FocusNode.Get(); !Node || Node->IsDepleted())
	{
		if (const FGamerItemDef* Held = Inventory->GetSelectedItemDef(); Held && Held->Food.IsMeal())
		{
			PlayActionLocally(EGamerAnimAction::Consume);
			Buffs->ServerEat(Held->Id);
			return;
		}
		else if (Held && UGamerBuffComponent::NeedsCooking(this, *Held))
		{
			Work->ShowMessage(FString::Printf(TEXT("%s kann man so nicht essen – erst kochen."), *Held->Name));
			return;
		}
	}
	if (AGamerResourceNode* Node = FocusNode.Get(); Node && !Node->IsDepleted())
	{
		const FGamerResourceNodeDef* Def = Node->GetDef();
		if (Def && !Def->RequiredTool.IsRequired())
		{
			StartGrab(Node); // by hand: walk up, then ServerHarvest
		}
		else
		{
			Work->ServerHarvest(Node);
		}
	}
}

void AGamerCharacter::StartGrab(AActor* Target)
{
	PendingGrab = Target;
	PendingGrabTime = 0.f;
	PendingGrabBestDistance = TNumericLimits<float>::Max();
	TickGrab(0.f); // close enough already: grab at once
}

void AGamerCharacter::TickGrab(float DeltaSeconds)
{
	AActor* Target = PendingGrab.Get();
	if (!Target)
	{
		PendingGrab.Reset();
		return;
	}
	PendingGrabTime += DeltaSeconds;
	const FVector Delta = Target->GetActorLocation() - GetActorLocation();
	const float Distance = Delta.Size2D();
	// Done when close, out of time, or no longer getting closer (the bush itself blocks the way).
	const bool bStuck = PendingGrabTime > 0.3f && Distance > PendingGrabBestDistance - 1.f;
	PendingGrabBestDistance = FMath::Min(PendingGrabBestDistance, Distance);
	if (Distance <= GrabDistance || PendingGrabTime >= GrabApproachSeconds || bStuck)
	{
		FinishGrab();
		return;
	}
	AddMovementInput(FVector(Delta.X, Delta.Y, 0.f).GetSafeNormal(), 1.f);
}

void AGamerCharacter::FinishGrab()
{
	AActor* Target = PendingGrab.Get();
	PendingGrab.Reset();
	if (!Target)
	{
		return;
	}
	FaceTowards(Target);
	if (AGamerGroundItem* Ground = Cast<AGamerGroundItem>(Target))
	{
		PlayActionLocally(EGamerAnimAction::GatherLow); // the server's copy skips this player
		Work->ServerPickUp(Ground);
	}
	else if (AGamerResourceNode* Node = Cast<AGamerResourceNode>(Target))
	{
		Work->ServerHarvest(Node); // work loop (crouch) comes from the replicated work state
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

void AGamerCharacter::OpenInventoryWindow()
{
	if (Health && Health->IsIncapacitated())
	{
		return;
	}
	if (AGamerPlayerController* PC = Cast<AGamerPlayerController>(Controller))
	{
		bGearMenuOpen = false;
		CloseCraftMenu();
		PC->OpenMenu(EGamerMenuPage::Inventory);   // a menu page with the mouse cursor; the key (or Esc) closes it again
	}
}

void AGamerCharacter::ToggleGearMenu()
{
	bGearMenuOpen = !bGearMenuOpen;
	if (bGearMenuOpen)
	{
		CloseCraftMenu();
	}
}

void AGamerCharacter::ToggleCraftMenu()
{
	bGearMenuOpen = false;
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
	if (bGearMenuOpen)
	{
		// Keys 1-7 = helmet ... cloak: take off what is worn, else put on the best fitting piece from the inventory.
		// Key 8 = both rings: put a ring on while a ring place is free, else take the last worn ring off.
		if (Equipment && Index >= 0 && Index < UGamerEquipmentComponent::PlaceCount - 1)
		{
			if (Index < 7)
			{
				if (!Equipment->GetWornAt(Index).IsNone())
				{
					Equipment->ServerUnequip(Index);
				}
				else if (const FName Candidate = Equipment->FindCandidateFor(Index); !Candidate.IsNone())
				{
					Equipment->ServerEquip(Candidate, Index);
				}
			}
			else
			{
				const FName Candidate = Equipment->FindCandidateFor(7);
				const bool bFree = Equipment->GetWornAt(7).IsNone() || Equipment->GetWornAt(8).IsNone();
				if (!Candidate.IsNone() && bFree)
				{
					Equipment->ServerEquip(Candidate, INDEX_NONE);
				}
				else if (!Equipment->GetWornAt(8).IsNone())
				{
					Equipment->ServerUnequip(8);
				}
				else if (!Equipment->GetWornAt(7).IsNone())
				{
					Equipment->ServerUnequip(7);
				}
			}
		}
		return;
	}
	if (bCraftMenuOpen)
	{
		if (HandRecipes.IsValidIndex(Index))
		{
			RequestCraft(HandRecipes[Index]);
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
	if (Health && Health->IsIncapacitated())
	{
		return;
	}
	// A dish, drink or raw food in hand: eat it (meal buffs, Taverne-Session; architecture rule 5) -
	// unless the crosshair is on work for a tool (tree, rock, plan): then the tool is taken as before.
	const AGamerResourceNode* WorkNode = FocusNode.Get();
	const bool bToolWork = (WorkNode && WorkNode->GetDef() && WorkNode->GetDef()->RequiredTool.IsRequired())
		|| (FocusBuilding.IsValid() && !FocusBuilding->IsFinished());
	if (const FGamerItemDef* Held = Inventory->GetSelectedItemDef(); !bToolWork && Held && UGamerBuffComponent::NeedsCooking(this, *Held))
	{
		// Food that cannot be eaten as it is (raw elderberries, potatoes, grain): say so instead of a swing into the air.
		Work->ShowMessage(FString::Printf(TEXT("%s kann man so nicht essen – erst kochen."), *Held->Name));
		return;
	}
	if (const FGamerItemDef* Held = Inventory->GetSelectedItemDef(); !bToolWork && Held && Held->Food.IsMeal())
	{
		PlayActionLocally(EGamerAnimAction::Consume);
		Buffs->ServerEat(Held->Id);
		return;
	}
	// An unfinished plan under the crosshair: take the hammer and hammer it.
	AGamerBuilding* Building = FocusBuilding.Get();
	if (Building && !Building->IsFinished() && TakeToolInHand(TEXT("hammer"), 1))
	{
		FaceTowards(Building);
		PlayActionLocally(EGamerAnimAction::Hammer);
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
			if (!Work->IsBusy())
			{
				FaceTowards(Node);
				PlayActionLocally(ActionForTool(Def->RequiredTool.Type));
			}
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
	// Empty stamina only slows down (architecture rule 5), it never blocks.
	GetCharacterMovement()->MaxWalkSpeed = (bSprinting ? SprintSpeed : WalkSpeed) * (Stamina ? Stamina->GetSpeedFactor() : 1.f);
}

void AGamerCharacter::OnJumped_Implementation()
{
	Super::OnJumped_Implementation();
	if (HasAuthority() && Stamina)
	{
		Stamina->NoteJump();
	}
}

void AGamerCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	// The owner sets it itself; the others need it for the right speed of their smoothed copy.
	DOREPLIFETIME_CONDITION(AGamerCharacter, bSprinting, COND_SkipOwner);
	DOREPLIFETIME(AGamerCharacter, ActionPulse);
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
	AppliedBodyLook = NAME_None; // apply again with the (new) player state
	ApplyBodyLook();
}

void AGamerCharacter::ApplyBodyLook()
{
	const AGamerPlayerState* PS = GetPlayerState<AGamerPlayerState>();
	if (!PS || GetNetMode() == NM_DedicatedServer)
	{
		return;
	}
	const FGamerCharacterProfile& Profile = PS->GetProfile();
	const FName BodyLook = Profile.GetLook(TEXT("look_body"));
	const FName Cloth = Profile.GetLook(TEXT("look_cloth_color"));
	const FName Hair = Profile.GetLook(TEXT("look_hair"));
	const FName Beard = Profile.GetLook(TEXT("look_beard"));
	const FName HairColour = Profile.GetLook(TEXT("look_hair_color"));
	const FName Key(*FString::Printf(TEXT("%s|%s|%s|%s|%s"), *BodyLook.ToString(), *Cloth.ToString(), *Hair.ToString(),
		*Beard.ToString(), *HairColour.ToString()));
	if (Key == AppliedBodyLook)
	{
		return;
	}
	AppliedBodyLook = Key;
	// MetaHuman figure if assembled and linked (Content/Data/figures.json), else the interim Manny/Quinn.
	const bool bSlim = BodyLook == TEXT("body_slim");
	GamerFigures::FFigure Figure;
	// Medium assembly first (hair cards that move: docs/01_entscheidungen.md "Haare"), then Low (static helmet hair),
	// then High (its long hair did not render, 21.11.).
	const bool bMetaHuman = GamerFigures::Find(bSlim ? TEXT("slim_mid") : TEXT("strong_mid"), Figure)
		|| GamerFigures::Find(bSlim ? TEXT("slim_low") : TEXT("strong_low"), Figure)
		|| GamerFigures::Find(bSlim ? TEXT("slim") : TEXT("strong"), Figure);
	USkeletalMesh* Wanted = bMetaHuman ? Figure.Body : (bSlim && BodySlim ? BodySlim.Get() : BodyStrong.Get());
	if (Wanted && GetMesh()->GetSkeletalMeshAsset() != Wanted)
	{
		GetMesh()->SetSkeletalMesh(Wanted);
	}
	if (Figure.GroomLOD == 3) // hair kits are Medium assemblies: only for the Medium player figure
	{
		GamerFigures::ApplyHairChoice(Figure, bSlim, Hair, Beard);
	}
	FaceMesh->SetSkeletalMesh(bMetaHuman ? Figure.Face : nullptr);
	FaceMesh->SetLeaderPoseComponent(GetMesh());
	GamerFigures::ApplyGrooms(FaceMesh, GroomComponents, Figure);
	GamerFigures::SetHairColour(GroomComponents, HairColour);
	// Own Norse clothing (tunic) in the chosen cloth colour, red-brown trim; only on the MetaHuman figure.
	GamerFigures::ApplyClothing(GetMesh(), ClothMesh, bMetaHuman, bSlim, TEXT("Tunika"), ClothTint(Cloth),
		GamerFigures::ClothPalette(Cloth == TEXT("cloth_red") ? 3 : 4));
	// Travel cloak in Aschebraun over every outfit (players are on the road; villagers only sometimes).
	GamerFigures::ApplyClothing(GetMesh(), CloakMesh, bMetaHuman, bSlim, TEXT("Umhang"), GamerFigures::ClothPalette(2),
		GamerFigures::ClothPalette(4));
	if (bMetaHuman)
	{
		return; // the tint below is for the interim figure
	}
	// Mannequin material: "Paint Tint" colours the painted panels (the clothing stand-in).
	const FLinearColor Tint = ClothTint(Cloth);
	for (int32 i = 0; i < GetMesh()->GetNumMaterials(); ++i)
	{
		if (UMaterialInstanceDynamic* Material = GetMesh()->CreateAndSetMaterialInstanceDynamic(i))
		{
			Material->SetVectorParameterValue(TEXT("Paint Tint"), Tint);
		}
	}
}

void AGamerCharacter::FillAnimInputs(FGamerAnimInputs& Out) const
{
	Out.bSprinting = bSprinting;
	const FGamerWorkState& State = Work->GetWork();
	// bLowOrStation (Gameplay-Session): Harvest = target lies low (crouch); Craft = at a station.
	// Crafting the first tools by hand (no station) kneels and hammers.
	Out.Loop = State.Kind == EGamerWorkKind::Harvest ? (State.bLowOrStation ? EGamerAnimAction::GatherLow : EGamerAnimAction::Gather)
		: State.Kind == EGamerWorkKind::Craft ? (State.bLowOrStation ? StationAnim(State.Subject) : EGamerAnimAction::Craft)
		: EGamerAnimAction::None;
	Out.Pulse = LocalAction;
}

void AGamerCharacter::RequestCraft(FName RecipeId)
{
	// At a station: walk up to it and work facing it (it can be up to the station reach away).
	const UGamerDataSubsystem* Data = UGamerDataSubsystem::Get(this);
	const FGamerRecipeDef* Recipe = Data ? Data->FindRecipe(RecipeId) : nullptr;
	const AActor* Station = nullptr;
	if (Recipe && !Recipe->Station.IsNone())
	{
		const AGamerBuilding* Open = StationWindow.Get();
		Station = Open && Open->GetBuildableId() == Recipe->Station ? Open : Work->FindStationNearby(Recipe->Station);
	}
	if (!Station)
	{
		Work->ServerCraft(RecipeId); // by hand: right here
		return;
	}
	// Walk up to the station first (TickCraftApproach), then turn to it and work.
	PendingCraftStation = Station;
	PendingCraftRecipe = RecipeId;
	PendingCraftTime = 0.f;
	PendingCraftBestDistance = TNumericLimits<float>::Max();
	TickCraftApproach(0.f); // close enough already: start at once
}

void AGamerCharacter::TickCraftApproach(float DeltaSeconds)
{
	const AActor* Station = PendingCraftStation.Get();
	const FName Recipe = PendingCraftRecipe;
	if (!Station)
	{
		PendingCraftRecipe = NAME_None;
		return;
	}
	PendingCraftTime += DeltaSeconds;
	// Distance to the station's outline (a workbench is long: its centre can be a metre away while standing at it).
	const FBox Box = Station->GetComponentsBoundingBox(false);
	const FVector Here = GetActorLocation();
	const FVector Closest(FMath::Clamp(Here.X, Box.Min.X, Box.Max.X), FMath::Clamp(Here.Y, Box.Min.Y, Box.Max.Y), Here.Z);
	const float Distance = Box.IsValid ? FVector::Dist2D(Here, Closest) : FVector::Dist2D(Here, Station->GetActorLocation());
	const bool bStuck = PendingCraftTime > 0.4f && Distance > PendingCraftBestDistance - 1.f;
	PendingCraftBestDistance = FMath::Min(PendingCraftBestDistance, Distance);
	if (Distance <= StationWorkDistance || PendingCraftTime >= StationApproachSeconds || bStuck)
	{
		PendingCraftStation.Reset();
		PendingCraftRecipe = NAME_None;
		FaceTowards(Station);
		Work->ServerCraft(Recipe);
		return;
	}
	const FVector Delta = (Box.IsValid ? Closest : Station->GetActorLocation()) - Here;
	AddMovementInput(FVector(Delta.X, Delta.Y, 0.f).GetSafeNormal(), 1.f);
}

EGamerAnimAction AGamerCharacter::StationAnim(FName RecipeId) const
{
	// Work clip per station (the recipe of the running work replicates as its subject).
	const UGamerDataSubsystem* Data = UGamerDataSubsystem::Get(this);
	const FGamerRecipeDef* Recipe = Data ? Data->FindRecipe(RecipeId) : nullptr;
	const FName Station = Recipe ? Recipe->Station : NAME_None;
	if (Station == TEXT("building_workbench"))
	{
		return EGamerAnimAction::CraftBench;
	}
	if (Station == TEXT("building_campfire"))
	{
		return EGamerAnimAction::CraftLow;
	}
	return EGamerAnimAction::CraftStation;
}

void AGamerCharacter::FaceTowards(const AActor* Target)
{
	if (!Target || !IsLocallyControlled())
	{
		return;
	}
	const FVector Delta = Target->GetActorLocation() - GetActorLocation();
	if (Delta.SizeSquared2D() > 1.f)
	{
		SetActorRotation(FRotator(0.f, Delta.Rotation().Yaw, 0.f));
	}
}

void AGamerCharacter::PlayActionLocally(EGamerAnimAction Kind)
{
	LocalAction.Kind = Kind;
	++LocalAction.Count;
}

void AGamerCharacter::MulticastAction(EGamerAnimAction Kind, bool bPredicted)
{
	if (!HasAuthority())
	{
		return;
	}
	ActionPulse.Kind = Kind;
	ActionPulse.bPredicted = bPredicted;
	++ActionPulse.Count;
	if (!bPredicted || !IsLocallyControlled())
	{
		PlayActionLocally(Kind); // the listen server shows it too
	}
}

void AGamerCharacter::OnRep_ActionPulse()
{
	const bool bPlay = !(ActionPulse.bPredicted && IsLocallyControlled());
	UE_LOG(LogGamer, Log, TEXT("Anim action %d #%d of %s: %s"), static_cast<int32>(ActionPulse.Kind), ActionPulse.Count,
		*GetName(), bPlay ? TEXT("played") : TEXT("already played locally"));
	if (bPlay)
	{
		PlayActionLocally(ActionPulse.Kind);
	}
}
