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
#include "GamerListenComponent.h"
#include "GamerScanComponent.h"
#include "GamerWorkComponent.h"
#include "GamerAnimInstance.h"
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
	if (PendingGrab.IsValid() && IsLocallyControlled())
	{
		TickGrab(DeltaSeconds);
	}

	if (TestWalkRemaining > 0.f && IsLocallyControlled())
	{
		TestWalkRemaining -= DeltaSeconds;
		AddMovementInput(GetActorForwardVector(), 1.f);
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
	if (AGamerResourceNode* Node = FocusNode.Get())
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
	GetCharacterMovement()->MaxWalkSpeed = bSprinting ? SprintSpeed : WalkSpeed;
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
	const FName Key(*FString::Printf(TEXT("%s|%s"), *BodyLook.ToString(), *Cloth.ToString()));
	if (Key == AppliedBodyLook)
	{
		return;
	}
	AppliedBodyLook = Key;
	USkeletalMesh* Wanted = BodyLook == TEXT("body_slim") && BodySlim ? BodySlim.Get() : BodyStrong.Get();
	if (Wanted && GetMesh()->GetSkeletalMeshAsset() != Wanted)
	{
		GetMesh()->SetSkeletalMesh(Wanted);
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
		: State.Kind == EGamerWorkKind::Craft ? (State.bLowOrStation ? EGamerAnimAction::CraftStation : EGamerAnimAction::Craft)
		: EGamerAnimAction::None;
	Out.Pulse = LocalAction;
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
