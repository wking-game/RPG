#include "GamerResourceNode.h"

#include "Gamer.h"
#include "GamerDataSubsystem.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/CollisionProfile.h"
#include "Engine/StaticMesh.h"
#include "HAL/IConsoleManager.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Net/UnrealNetwork.h"
#include "TimerManager.h"
#include "UObject/ConstructorHelpers.h"

namespace
{
	TAutoConsoleVariable<float> CVarRegrowScale(
		TEXT("gamer.RegrowScale"),
		1.f,
		TEXT("Multiplies regrow times of resource nodes (tests: 0.01 = 100 times faster)."));

	// Engine basic shapes are 100 units in size, centred on the origin.
	struct FPlaceholderLook
	{
		const TCHAR* BodyMesh = TEXT("Sphere");
		FVector BodyScale = FVector(1.f);
		FVector BodyOffset = FVector::ZeroVector;
		FRotator BodyRotation = FRotator::ZeroRotator;
		FLinearColor BodyColor = FLinearColor::Gray;
		const TCHAR* TopMesh = nullptr;
		FVector TopScale = FVector(1.f);
		FVector TopOffset = FVector::ZeroVector;
		FLinearColor TopColor = FLinearColor::Green;
		/** Fruit trees keep their crown when picked; only its colour changes. */
		bool bKeepTopWhenDepleted = false;
		FLinearColor DepletedTopColor = FLinearColor::Green;
		bool bBlocksPlayers = false;
		/** When depleted: hide everything (true) or keep the body as stump/bare bush (false). */
		bool bHideWhenDepleted = false;
		FVector DepletedBodyScale = FVector::ZeroVector; // zero = unchanged
	};

	FPlaceholderLook LookFor(const FString& Id)
	{
		const FLinearColor Bark(0.30f, 0.20f, 0.12f), Leaves(0.15f, 0.40f, 0.12f), DarkNeedles(0.08f, 0.28f, 0.12f);
		FPlaceholderLook L;
		if (Id.StartsWith(TEXT("res_tree")))
		{
			L.BodyMesh = TEXT("Cylinder");
			L.BodyScale = FVector(0.35f, 0.35f, 4.0f);
			L.BodyOffset = FVector(0, 0, 200);
			L.BodyColor = Id.Contains(TEXT("birch")) ? FLinearColor(0.85f, 0.85f, 0.80f) : Bark;
			L.TopMesh = Id.Contains(TEXT("pine")) ? TEXT("Cone") : TEXT("Sphere");
			L.TopScale = Id.Contains(TEXT("pine")) ? FVector(2.2f, 2.2f, 4.0f) : FVector(2.6f, 2.6f, 2.2f);
			L.TopOffset = Id.Contains(TEXT("pine")) ? FVector(0, 0, 450) : FVector(0, 0, 430);
			L.TopColor = Id.Contains(TEXT("pine")) ? DarkNeedles : Leaves;
			if (Id.Contains(TEXT("apple")))
			{
				L.BodyScale = FVector(0.3f, 0.3f, 2.4f);
				L.BodyOffset = FVector(0, 0, 120);
				L.TopScale = FVector(2.4f, 2.4f, 1.8f);
				L.TopOffset = FVector(0, 0, 280);
				L.TopColor = FLinearColor(0.60f, 0.50f, 0.12f); // crown full of yellow-red fruit (placeholder colour)
				L.bKeepTopWhenDepleted = true;
				L.DepletedTopColor = Leaves;
			}
			L.bBlocksPlayers = true;
			L.DepletedBodyScale = Id.Contains(TEXT("apple")) ? FVector::ZeroVector : FVector(0.35f, 0.35f, 0.4f);
		}
		else if (Id.StartsWith(TEXT("res_rock")))
		{
			const float Size = Id.Contains(TEXT("large")) ? 2.4f : Id.Contains(TEXT("medium")) ? 1.5f : 0.9f;
			L.BodyScale = FVector(Size, Size * 0.9f, Size * 0.6f);
			L.BodyOffset = FVector(0, 0, Size * 20.f);
			L.BodyColor = FLinearColor(0.35f, 0.35f, 0.33f);
			L.bBlocksPlayers = true;
			L.DepletedBodyScale = FVector(Size * 0.5f, Size * 0.45f, Size * 0.2f);
		}
		else if (Id == TEXT("res_berry_bush") || Id == TEXT("res_hazel_bush"))
		{
			const bool bHazel = Id.Contains(TEXT("hazel"));
			L.BodyScale = bHazel ? FVector(1.6f, 1.6f, 2.0f) : FVector(1.1f, 1.1f, 0.9f);
			L.BodyOffset = FVector(0, 0, bHazel ? 100 : 45);
			L.BodyColor = Leaves;
			L.TopMesh = TEXT("Sphere");
			L.TopScale = FVector(0.35f);
			L.TopOffset = FVector(bHazel ? 70 : 50, 0, bHazel ? 140 : 70);
			L.TopColor = bHazel ? FLinearColor(0.55f, 0.40f, 0.20f) : FLinearColor(0.60f, 0.05f, 0.10f);
		}
		else if (Id == TEXT("res_mushroom_patch"))
		{
			L.BodyMesh = TEXT("Cylinder");
			L.BodyScale = FVector(0.12f, 0.12f, 0.15f);
			L.BodyOffset = FVector(0, 0, 7);
			L.BodyColor = FLinearColor(0.85f, 0.80f, 0.70f);
			L.TopMesh = TEXT("Sphere");
			L.TopScale = FVector(0.3f, 0.3f, 0.12f);
			L.TopOffset = FVector(0, 0, 16);
			L.TopColor = FLinearColor(0.45f, 0.28f, 0.12f);
			L.bHideWhenDepleted = true;
		}
		else if (Id == TEXT("res_loose_stone"))
		{
			L.BodyScale = FVector(0.35f, 0.3f, 0.2f);
			L.BodyOffset = FVector(0, 0, 8);
			L.BodyColor = FLinearColor(0.45f, 0.45f, 0.42f);
			L.bHideWhenDepleted = true;
		}
		else if (Id == TEXT("res_fallen_branch"))
		{
			L.BodyMesh = TEXT("Cylinder");
			L.BodyScale = FVector(0.07f, 0.07f, 1.2f);
			L.BodyOffset = FVector(0, 0, 5);
			L.BodyRotation = FRotator(90.f, 0.f, 0.f);
			L.BodyColor = Bark;
			L.bHideWhenDepleted = true;
		}
		else if (Id == TEXT("res_fiber_plant"))
		{
			L.BodyMesh = TEXT("Cone");
			L.BodyScale = FVector(0.35f, 0.35f, 0.8f);
			L.BodyOffset = FVector(0, 0, 40);
			L.BodyColor = FLinearColor(0.20f, 0.45f, 0.15f);
			L.DepletedBodyScale = FVector(0.3f, 0.3f, 0.15f);
		}
		return L;
	}

	void SetColor(UStaticMeshComponent* Mesh, const FLinearColor& Color)
	{
		if (Mesh && Mesh->GetStaticMesh())
		{
			if (UMaterialInstanceDynamic* Material = Mesh->CreateAndSetMaterialInstanceDynamic(0))
			{
				Material->SetVectorParameterValue(TEXT("Color"), Color);
			}
		}
	}
}

AGamerResourceNode::AGamerResourceNode()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
	SetNetUpdateFrequency(2.f);

	USceneComponent* Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = Root;

	Body = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Body"));
	Body->SetupAttachment(Root);
	Top = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Top"));
	Top->SetupAttachment(Root);
	Top->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AGamerResourceNode::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AGamerResourceNode, NodeId);
	DOREPLIFETIME(AGamerResourceNode, bDepleted);
}

void AGamerResourceNode::BeginPlay()
{
	Super::BeginPlay();
	ApplyLook();
}

void AGamerResourceNode::SetNodeId(FName InNodeId)
{
	NodeId = InNodeId;
	ApplyLook();
}

const FGamerResourceNodeDef* AGamerResourceNode::GetDef() const
{
	const UGamerDataSubsystem* Data = UGamerDataSubsystem::Get(this);
	return Data ? Data->FindResourceNode(NodeId) : nullptr;
}

void AGamerResourceNode::OnRep_NodeId()
{
	ApplyLook();
}

void AGamerResourceNode::OnRep_Depleted()
{
	ApplyLook();
	UE_LOG(LogGamer, Log, TEXT("Node %s (%s) is now %s (replicated)"), *GetName(), *NodeId.ToString(), bDepleted ? TEXT("depleted") : TEXT("full"));
}

void AGamerResourceNode::Deplete()
{
	if (!HasAuthority() || bDepleted)
	{
		return;
	}
	bDepleted = true;
	Worker.Reset();
	ApplyLook();
	ForceNetUpdate();

	const FGamerResourceNodeDef* Def = GetDef();
	if (Def && Def->RegrowDays > 0.f)
	{
		const float Seconds = FMath::Max(0.1f, Def->RegrowDays * DayLengthSeconds * CVarRegrowScale.GetValueOnGameThread());
		GetWorldTimerManager().SetTimer(RegrowTimer, this, &AGamerResourceNode::Regrow, Seconds, false);
	}
}

void AGamerResourceNode::Regrow()
{
	bDepleted = false;
	ApplyLook();
	ForceNetUpdate();
	UE_LOG(LogGamer, Log, TEXT("Node %s (%s) has grown back"), *GetName(), *NodeId.ToString());
}

void AGamerResourceNode::ApplyLook()
{
	if (NodeId.IsNone())
	{
		return;
	}
	const FPlaceholderLook Look = LookFor(NodeId.ToString());

	UStaticMesh* BodyMesh = LoadObject<UStaticMesh>(nullptr, *FString::Printf(TEXT("/Engine/BasicShapes/%s.%s"), Look.BodyMesh, Look.BodyMesh));
	if (Body->GetStaticMesh() != BodyMesh)
	{
		Body->SetStaticMesh(BodyMesh);
		SetColor(Body, Look.BodyColor);
	}
	Body->SetRelativeLocation(Look.BodyOffset);
	Body->SetRelativeRotation(Look.BodyRotation);
	Body->SetRelativeScale3D(bDepleted && !Look.DepletedBodyScale.IsZero() ? Look.DepletedBodyScale : Look.BodyScale);
	if (bDepleted && !Look.DepletedBodyScale.IsZero())
	{
		Body->SetRelativeLocation(FVector(Look.BodyOffset.X, Look.BodyOffset.Y, Look.DepletedBodyScale.Z * 50.f));
	}

	// Interaction traces use the Visibility channel; only big objects block players.
	Body->SetCollisionProfileName(Look.bBlocksPlayers ? UCollisionProfile::BlockAllDynamic_ProfileName : TEXT("OverlapAllDynamic"));
	Body->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

	const bool bHideAll = bDepleted && Look.bHideWhenDepleted;
	Body->SetVisibility(!bHideAll);
	Body->SetCollisionEnabled(bHideAll ? ECollisionEnabled::NoCollision : ECollisionEnabled::QueryAndPhysics);

	if (Look.TopMesh)
	{
		UStaticMesh* TopMesh = LoadObject<UStaticMesh>(nullptr, *FString::Printf(TEXT("/Engine/BasicShapes/%s.%s"), Look.TopMesh, Look.TopMesh));
		if (Top->GetStaticMesh() != TopMesh)
		{
			Top->SetStaticMesh(TopMesh);
		}
		SetColor(Top, bDepleted && Look.bKeepTopWhenDepleted ? Look.DepletedTopColor : Look.TopColor);
		Top->SetRelativeLocation(Look.TopOffset);
		Top->SetRelativeScale3D(Look.TopScale);
		Top->SetVisibility(!bDepleted || Look.bKeepTopWhenDepleted);
	}
	else
	{
		Top->SetVisibility(false);
	}
}
