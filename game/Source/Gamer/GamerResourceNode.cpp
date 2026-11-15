#include "GamerResourceNode.h"

#include "Gamer.h"
#include "GamerDataSubsystem.h"
#include "GamerGameState.h"
#include "GamerGroundItem.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/CollisionProfile.h"
#include "Engine/StaticMesh.h"
#include "HAL/IConsoleManager.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Math/RandomStream.h"
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
		else if (Id == TEXT("res_bee_nest"))
		{
			// Hollow stump with a honeycomb lump on top (placeholder, Taverne-Session).
			L.BodyMesh = TEXT("Cylinder");
			L.BodyScale = FVector(0.5f, 0.5f, 0.9f);
			L.BodyOffset = FVector(0, 0, 45);
			L.BodyColor = Bark;
			L.TopMesh = TEXT("Sphere");
			L.TopScale = FVector(0.35f, 0.35f, 0.45f);
			L.TopOffset = FVector(0, 0, 105);
			L.TopColor = FLinearColor(0.80f, 0.55f, 0.10f);
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
	DOREPLIFETIME(AGamerResourceNode, GrowStage);
	DOREPLIFETIME(AGamerResourceNode, WorkProgress);
	DOREPLIFETIME(AGamerResourceNode, FruitStage);
	DOREPLIFETIME(AGamerResourceNode, FruitLeft);
}

FString AGamerResourceNode::GetDisplayName() const
{
	const FGamerResourceNodeDef* Def = GetDef();
	const FString Name = Def ? Def->Name : NodeId.ToString();
	if (bPlantedGrowing)
	{
		return Name + (GrowStage <= 1 ? TEXT(" (Setzling)") : TEXT(" (Jungbaum)"));
	}
	switch (FruitStage)
	{
	case EGamerFruitStage::Bloom: return Name + TEXT(" (blüht)");
	case EGamerFruitStage::Unripe: return Name + TEXT(" (Früchte noch unreif)");
	case EGamerFruitStage::Ripe: return FruitLeft > 0 ? FString::Printf(TEXT("%s (%d reif)"), *Name, FruitLeft) : Name;
	default: return Name;
	}
}

TArray<FGamerItemAmount> AGamerResourceNode::Harvest()
{
	TArray<FGamerItemAmount> Out;
	const FGamerResourceNodeDef* Def = GetDef();
	if (!HasAuthority() || !Def || bDepleted)
	{
		return Out;
	}
	if (FruitStage != EGamerFruitStage::None)
	{
		const int32 Count = Def->Season.Pick > 0 ? FMath::Min(Def->Season.Pick, FruitLeft) : FruitLeft;
		FruitLeft -= Count;
		if (Count > 0 && Def->Drops.Num() > 0)
		{
			FGamerItemAmount& Entry = Out.AddDefaulted_GetRef();
			Entry.Item = Def->Drops[0].Item;
			Entry.Count = Count;
		}
		WorkProgress = 0.f;
		Worker.Reset();
		if (FruitLeft <= 0)
		{
			bDepleted = true;
			ApplyLook();
		}
		ForceNetUpdate();
		UE_LOG(LogGamer, Log, TEXT("Node %s (%s) picked: %d fruit, %d left"), *GetName(), *NodeId.ToString(), Count, FruitLeft);
		return Out;
	}
	for (const FGamerDrop& Drop : Def->Drops)
	{
		const int32 Count = FMath::RandRange(Drop.Min, Drop.Max);
		if (Count > 0)
		{
			FGamerItemAmount& Entry = Out.AddDefaulted_GetRef();
			Entry.Item = Drop.Item;
			Entry.Count = Count;
		}
	}
	Deplete();
	return Out;
}

void AGamerResourceNode::UpdateSeason()
{
	const FGamerResourceNodeDef* Def = GetDef();
	const AGamerGameState* State = GetWorld() ? GetWorld()->GetGameState<AGamerGameState>() : nullptr;
	if (!Def || !Def->Season.IsSet() || !State || Def->Drops.Num() == 0)
	{
		return;
	}
	const FGamerSeasonProps& S = Def->Season;
	const float Phase = State->GetSeasonPhase();
	const EGamerFruitStage Stage = (Phase < S.Bloom || Phase >= S.Bare) ? EGamerFruitStage::Bare
		: Phase < S.Unripe ? EGamerFruitStage::Bloom
		: Phase < S.Ripe ? EGamerFruitStage::Unripe : EGamerFruitStage::Ripe;
	const EGamerFruitStage OldStage = FruitStage;
	const bool bOldDepleted = bDepleted;
	const int32 OldFruit = FruitLeft;

	// A new year without passing "bare" (only when the season is set backwards, e.g. tests or loading): start over.
	if (bFruitRolled && Stage != EGamerFruitStage::Bare && OldStage != EGamerFruitStage::Bare && static_cast<uint8>(Stage) < static_cast<uint8>(OldStage))
	{
		bFruitRolled = false;
		FruitLeft = 0;
	}
	if (Stage == EGamerFruitStage::Bare || Stage == EGamerFruitStage::Bloom)
	{
		// End of the year: what still hangs falls now (nothing stays on the bare tree in winter).
		if (Stage == EGamerFruitStage::Bare && OldStage == EGamerFruitStage::Ripe && S.Falls())
		{
			for (int32 Index = 0; Index < FMath::Min(FruitLeft, 30); ++Index)
			{
				DropFruit();
			}
		}
		FruitLeft = 0;
		bFruitRolled = false;
		FruitAtFallStart = 0;
		FruitFallen = 0;
	}
	else
	{
		if (!bFruitRolled)
		{
			const FGamerDrop& Fruit = Def->Drops[0];
			FruitLeft = FMath::RandRange(Fruit.Min, Fruit.Max);
			bFruitRolled = true;
			FruitAtFallStart = 0;
			FruitFallen = 0;
		}
		if (Stage == EGamerFruitStage::Ripe && S.Falls() && Phase >= S.Fall)
		{
			// Unpicked fruit falls bit by bit until "bare".
			if (FruitAtFallStart == 0 && FruitFallen == 0)
			{
				FruitAtFallStart = FruitLeft;
			}
			const float Progress = FMath::Clamp((Phase - S.Fall) / FMath::Max(0.001f, S.Bare - S.Fall), 0.f, 1.f);
			const int32 Due = FMath::FloorToInt(FruitAtFallStart * Progress) - FruitFallen;
			const int32 Now = FMath::Min3(Due, FruitLeft, 4);
			for (int32 Index = 0; Index < Now; ++Index)
			{
				DropFruit();
				--FruitLeft;
				++FruitFallen;
			}
		}
	}
	FruitStage = Stage;
	bDepleted = !(Stage == EGamerFruitStage::Ripe && FruitLeft > 0);
	if (bDepleted)
	{
		WorkProgress = 0.f;
	}
	if (OldStage != FruitStage || bOldDepleted != bDepleted || OldFruit != FruitLeft)
	{
		if (OldStage != FruitStage)
		{
			UE_LOG(LogGamer, Log, TEXT("Node %s (%s): season stage %d -> %d at phase %.2f, fruit %d"), *GetName(), *NodeId.ToString(),
				static_cast<int32>(OldStage), static_cast<int32>(FruitStage), Phase, FruitLeft);
		}
		ApplyLook();
		ForceNetUpdate();
	}
}

void AGamerResourceNode::DropFruit()
{
	const FGamerResourceNodeDef* Def = GetDef();
	if (!Def || Def->Drops.Num() == 0)
	{
		return;
	}
	const FGamerSeasonProps& S = Def->Season;
	const float Height = FMath::FRandRange(S.DropHeightMin, S.DropHeightMax);
	const bool bBruised = S.BruiseHeight > 0.f && Height > S.BruiseHeight && !S.BruisedItem.IsNone();
	const FName Item = bBruised ? S.BruisedItem : Def->Drops[0].Item;

	// Somewhere under the crown.
	float Radius = 150.f;
	if (Body && Body->GetStaticMesh())
	{
		Radius = FMath::Clamp(FMath::Min(Body->Bounds.BoxExtent.X, Body->Bounds.BoxExtent.Y) * 0.75f, 60.f, 450.f);
	}
	const float Angle = FMath::FRandRange(0.f, 2.f * PI);
	const float Distance = Radius * FMath::Sqrt(FMath::FRandRange(0.15f, 1.f));
	const FVector Probe = GetActorLocation() + FVector(FMath::Cos(Angle) * Distance, FMath::Sin(Angle) * Distance, 0.f);
	FHitResult Hit;
	FCollisionQueryParams Params(SCENE_QUERY_STAT(GamerFruitFall), false, this);
	FVector Ground = Probe;
	if (GetWorld()->LineTraceSingleByChannel(Hit, Probe + FVector(0.f, 0.f, Height + 300.f), Probe - FVector(0.f, 0.f, 1000.f), ECC_WorldStatic, Params))
	{
		Ground = Hit.ImpactPoint;
	}
	AGamerGroundItem::SpawnFallen(GetWorld(), Item, 1, 1.f, Ground, Height);
	UE_LOG(LogGamer, Log, TEXT("Fruit fell from %s (%s): %s from %.0f cm%s"), *GetName(), *NodeId.ToString(), *Item.ToString(), Height,
		bBruised ? TEXT(" - bruised") : TEXT(""));
}

bool AGamerResourceNode::AddWork(float Amount)
{
	if (!HasAuthority() || bDepleted)
	{
		return false;
	}
	WorkProgress = FMath::Min(1.f, WorkProgress + FMath::Max(0.f, Amount));
	ForceNetUpdate();
	return WorkProgress >= 0.999f;
}

void AGamerResourceNode::BeginPlay()
{
	Super::BeginPlay();
	if (HasAuthority())
	{
		const FGamerResourceNodeDef* Def = GetDef();
		if (bPlantedGrowing && Def)
		{
			// Sapling first, the other growing models share the time, full at the end (then the season starts).
			const float Seconds = FMath::Max(1.f, PlantedGrowDays * DayLengthSeconds * CVarRegrowScale.GetValueOnGameThread());
			bDepleted = true;
			GrowStage = Def->MeshGrowing.Num() > 0 ? 1 : 0;
			const int32 Stages = Def->MeshGrowing.Num();
			GrowStepSeconds = Seconds / FMath::Max(1, Stages);
			if (Stages > 1)
			{
				GetWorldTimerManager().SetTimer(GrowTimer, this, &AGamerResourceNode::Grow, GrowStepSeconds, false);
			}
			GetWorldTimerManager().SetTimer(RegrowTimer, this, &AGamerResourceNode::Regrow, Seconds, false);
			UE_LOG(LogGamer, Log, TEXT("Node %s (%s) planted: grows %.1f s through %d stages"), *GetName(), *NodeId.ToString(), Seconds, Stages);
		}
		else if (Def && Def->Season.IsSet())
		{
			StartSeason();
		}
	}
	ApplyLook();
}

void AGamerResourceNode::SetPlanted(float GrowDays)
{
	bPlantedGrowing = true;
	PlantedGrowDays = FMath::Max(0.01f, GrowDays);
}

void AGamerResourceNode::StartSeason()
{
	FruitStage = EGamerFruitStage::Bare;
	UpdateSeason();
	GetWorldTimerManager().SetTimer(SeasonTimer, this, &AGamerResourceNode::UpdateSeason, 1.f, true, FMath::FRandRange(0.2f, 1.f));
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
	if (FruitStage != EGamerFruitStage::None)
	{
		FruitLeft = 0; // season nodes come back with the next year, not by a timer
		bDepleted = true;
		ApplyLook();
		ForceNetUpdate();
		return;
	}
	bDepleted = true;
	GrowStage = 0;
	WorkProgress = 0.f;
	Worker.Reset();
	ApplyLook();
	ForceNetUpdate();

	const FGamerResourceNodeDef* Def = GetDef();
	if (Def && Def->RegrowDays > 0.f)
	{
		const float Seconds = FMath::Max(0.1f, Def->RegrowDays * DayLengthSeconds * CVarRegrowScale.GetValueOnGameThread());
		GetWorldTimerManager().SetTimer(RegrowTimer, this, &AGamerResourceNode::Regrow, Seconds, false);
		// Growing stages share the second half of the time: stump first, then sapling, young tree, full.
		const int32 Stages = Def->MeshGrowing.Num();
		if (Stages > 0)
		{
			GrowStepSeconds = Seconds * 0.5f / Stages;
			GetWorldTimerManager().SetTimer(GrowTimer, this, &AGamerResourceNode::Grow, Seconds * 0.5f, false);
		}
	}
}

void AGamerResourceNode::Grow()
{
	const FGamerResourceNodeDef* Def = GetDef();
	if (!bDepleted || !Def || GrowStage >= Def->MeshGrowing.Num())
	{
		return;
	}
	++GrowStage;
	ApplyLook();
	ForceNetUpdate();
	UE_LOG(LogGamer, Log, TEXT("Node %s (%s) grows: stage %d of %d"), *GetName(), *NodeId.ToString(), GrowStage, Def->MeshGrowing.Num());
	if (GrowStage < Def->MeshGrowing.Num())
	{
		GetWorldTimerManager().SetTimer(GrowTimer, this, &AGamerResourceNode::Grow, GrowStepSeconds, false);
	}
}

void AGamerResourceNode::Regrow()
{
	GetWorldTimerManager().ClearTimer(GrowTimer);
	bDepleted = false;
	GrowStage = 0;
	WorkProgress = 0.f;
	ApplyLook();
	ForceNetUpdate();
	UE_LOG(LogGamer, Log, TEXT("Node %s (%s) has grown back%s"), *GetName(), *NodeId.ToString(), bPlantedGrowing ? TEXT(" (planted tree is full grown)") : TEXT(""));
	if (bPlantedGrowing)
	{
		bPlantedGrowing = false;
		const FGamerResourceNodeDef* Def = GetDef();
		if (Def && Def->Season.IsSet())
		{
			StartSeason(); // from now on it follows the year like the wild trees
		}
	}
}

void AGamerResourceNode::ApplyModel(const FGamerResourceNodeDef& Def)
{
	const FPlaceholderLook Look = LookFor(NodeId.ToString()); // only for the collision rule (big objects block players)
	FSoftObjectPath Path = !bDepleted ? Def.MeshFull
		: (GrowStage > 0 && Def.MeshGrowing.IsValidIndex(GrowStage - 1)) ? Def.MeshGrowing[GrowStage - 1] : Def.MeshDepleted;
	// Season nodes: blossoms, unripe fruit, ripe fruit or the bare plant (without its own model: the picked one).
	if (FruitStage == EGamerFruitStage::Bloom && !Def.MeshBloom.IsNull())
	{
		Path = Def.MeshBloom;
	}
	else if (FruitStage == EGamerFruitStage::Unripe && !Def.MeshUnripe.IsNull())
	{
		Path = Def.MeshUnripe;
	}
	if (Path.IsNull() && FruitStage != EGamerFruitStage::None)
	{
		Path = Def.MeshDepleted.IsNull() ? Def.MeshFull : Def.MeshDepleted;
	}
	UStaticMesh* Mesh = Path.IsNull() ? nullptr : Cast<UStaticMesh>(Path.TryLoad());
	if (!Path.IsNull() && !Mesh)
	{
		UE_LOG(LogGamer, Warning, TEXT("Node %s: model %s not found"), *NodeId.ToString(), *Path.ToString());
	}
	if (Body->GetStaticMesh() != Mesh)
	{
		Body->SetStaticMesh(Mesh);
	}
	// Natural variety: yaw and size from the position, so server and every client get the same.
	const FVector Location = GetActorLocation();
	FRandomStream Random(static_cast<int32>(HashCombine(
		GetTypeHash(FIntPoint(FMath::RoundToInt(Location.X), FMath::RoundToInt(Location.Y))), GetTypeHash(NodeId))));
	Body->SetRelativeLocation(FVector::ZeroVector);
	Body->SetRelativeRotation(FRotator(0.f, Random.FRandRange(0.f, 360.f), 0.f));
	Body->SetRelativeScale3D(FVector(Random.FRandRange(0.85f, 1.15f)));

	// Interaction traces use the Visibility channel; only big objects block players.
	Body->SetCollisionProfileName(Look.bBlocksPlayers ? UCollisionProfile::BlockAllDynamic_ProfileName : TEXT("OverlapAllDynamic"));
	Body->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	Body->SetVisibility(Mesh != nullptr);
	Body->SetCollisionEnabled(Mesh ? ECollisionEnabled::QueryAndPhysics : ECollisionEnabled::NoCollision);
	Top->SetVisibility(false);
}

void AGamerResourceNode::ApplyLook()
{
	if (NodeId.IsNone())
	{
		return;
	}
	if (const FGamerResourceNodeDef* Def = GetDef(); Def && !Def->MeshFull.IsNull())
	{
		ApplyModel(*Def);
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
		FLinearColor TopColor = bDepleted && Look.bKeepTopWhenDepleted ? Look.DepletedTopColor : Look.TopColor;
		if (FruitStage == EGamerFruitStage::Bloom) { TopColor = FLinearColor(0.85f, 0.82f, 0.80f); }
		else if (FruitStage == EGamerFruitStage::Unripe) { TopColor = FLinearColor(0.30f, 0.50f, 0.12f); }
		SetColor(Top, TopColor);
		Top->SetRelativeLocation(Look.TopOffset);
		Top->SetRelativeScale3D(Look.TopScale);
		Top->SetVisibility(!bDepleted || Look.bKeepTopWhenDepleted);
	}
	else
	{
		Top->SetVisibility(false);
	}
}
