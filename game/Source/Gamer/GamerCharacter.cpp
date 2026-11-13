#include "GamerCharacter.h"

#include "Gamer.h"
#include "GamerBuildComponent.h"
#include "GamerBuilding.h"
#include "GamerDataSubsystem.h"
#include "GamerInventoryComponent.h"
#include "GamerKeyBindings.h"
#include "GamerPlayerController.h"
#include "GamerResourceNode.h"
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
		Input->BindAction(InteractAction, ETriggerEvent::Started, this, &AGamerCharacter::Interact);
		Input->BindAction(CraftMenuAction, ETriggerEvent::Started, this, &AGamerCharacter::ToggleCraftMenu);
		Input->BindAction(NumberAction, ETriggerEvent::Started, this, &AGamerCharacter::NumberKey);
		Input->BindAction(StrikeAction, ETriggerEvent::Triggered, this, &AGamerCharacter::Strike);
		Input->BindAction(BuildMenuAction, ETriggerEvent::Started, this, &AGamerCharacter::OpenBuildMenu);
		Input->BindAction(RotateAction, ETriggerEvent::Started, this, &AGamerCharacter::RotatePiece);
		Input->BindAction(DemolishAction, ETriggerEvent::Started, this, &AGamerCharacter::Demolish);
		Input->BindAction(VillageMenuAction, ETriggerEvent::Started, this, &AGamerCharacter::OpenVillageMenu);
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
	FocusNode.Reset();
	FocusBuilding.Reset();
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
	if (AGamerResourceNode* Node = FocusNode.Get())
	{
		Work->ServerHarvest(Node);
	}
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
	bCraftMenuOpen = !bCraftMenuOpen;
	if (bCraftMenuOpen)
	{
		RefreshHandRecipes();
	}
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

	// With the hammer: an unfinished plan under the crosshair is hammered first, otherwise the chosen piece is placed.
	if (Inventory->IsHoldingTool(TEXT("hammer"), 1))
	{
		AGamerBuilding* Building = FocusBuilding.Get();
		if (Building && !Building->IsFinished())
		{
			Build->ServerHammer(Building);
			return;
		}
		if (Build->IsPlacing())
		{
			Build->TryPlace();
			return;
		}
	}
	Work->ServerStrike(FocusNode.Get());
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
