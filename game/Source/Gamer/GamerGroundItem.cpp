#include "GamerGroundItem.h"

#include "Gamer.h"
#include "GamerDataSubsystem.h"
#include "GamerSpoilage.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/CollisionProfile.h"
#include "Engine/StaticMesh.h"
#include "Engine/World.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Net/UnrealNetwork.h"
#include "TimerManager.h"

AGamerGroundItem::AGamerGroundItem()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false; // only while the client plays the fall
	bReplicates = true;
	SetReplicatingMovement(false);
	SetNetUpdateFrequency(1.f);
	SetNetCullDistanceSquared(FMath::Square(8000.f)); // small things: only relevant within 80 m

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(Root);
	// Players walk over it; the focus trace (Visibility) finds it.
	Mesh->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	Mesh->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	Mesh->SetGenerateOverlapEvents(false);
}

void AGamerGroundItem::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AGamerGroundItem, Item);
	DOREPLIFETIME(AGamerGroundItem, Count);
	DOREPLIFETIME(AGamerGroundItem, FreshPercent);
	DOREPLIFETIME(AGamerGroundItem, FallHeight);
}

AGamerGroundItem* AGamerGroundItem::SpawnFallen(UWorld* World, FName InItem, int32 InCount, float InFreshness, const FVector& Ground, float InFallHeight)
{
	if (!World || InItem.IsNone() || InCount <= 0)
	{
		return nullptr;
	}
	const FTransform Transform(FRotator(0.f, FMath::FRandRange(0.f, 360.f), 0.f), Ground);
	AGamerGroundItem* Spawned = World->SpawnActorDeferred<AGamerGroundItem>(AGamerGroundItem::StaticClass(), Transform, nullptr, nullptr,
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
	if (Spawned)
	{
		Spawned->Item = InItem;
		Spawned->Count = InCount;
		Spawned->Freshness = FMath::Clamp(InFreshness, 0.f, 1.f);
		Spawned->FreshPercent = static_cast<uint8>(UGamerSpoilage::Percent(Spawned->Freshness));
		Spawned->FallHeight = FMath::Max(0.f, InFallHeight);
		Spawned->FinishSpawning(Transform);
	}
	return Spawned;
}

void AGamerGroundItem::BeginPlay()
{
	Super::BeginPlay();
	ApplyLook();
	if (HasAuthority())
	{
		GetWorldTimerManager().SetTimer(SpoilTimer, this, &AGamerGroundItem::TickSpoilage, FMath::Max(0.2f, UGamerSpoilage::Get().TickSeconds), true);
	}
	// Short fall on machines that render (purely visual; the actor itself never moves).
	if (GetNetMode() != NM_DedicatedServer && FallHeight > 10.f && GetGameTimeSinceCreation() < 1.f)
	{
		FallTime = 0.f;
		SetActorTickEnabled(true);
		Mesh->SetRelativeLocation(FVector(0.f, 0.f, FallHeight));
	}
}

void AGamerGroundItem::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	// Free fall: h = g t^2 / 2 with g = 981 cm/s^2.
	FallTime += DeltaSeconds;
	const float Left = FMath::Max(0.f, FallHeight - 0.5f * 981.f * FallTime * FallTime);
	Mesh->SetRelativeLocation(FVector(0.f, 0.f, Left));
	if (Left <= 0.f)
	{
		SetActorTickEnabled(false);
	}
}

const FGamerItemDef* AGamerGroundItem::GetItemDef() const
{
	const UGamerDataSubsystem* Data = UGamerDataSubsystem::Get(this);
	return Data ? Data->FindItem(Item) : nullptr;
}

FString AGamerGroundItem::GetDisplayName() const
{
	const FGamerItemDef* Def = GetItemDef();
	const FString Name = Def ? Def->Name : Item.ToString();
	const FString Amount = Count > 1 ? FString::Printf(TEXT("%d "), Count) : FString();
	if (Def && Def->Spoils())
	{
		return FString::Printf(TEXT("%s%s (%d %%)"), *Amount, *Name, UGamerSpoilage::Percent(GetFreshness()));
	}
	return Amount + Name;
}

bool AGamerGroundItem::TakeAll(FName& OutItem, int32& OutCount, float& OutFreshness)
{
	if (!HasAuthority() || bTaken)
	{
		return false;
	}
	bTaken = true;
	OutItem = Item;
	OutCount = Count;
	OutFreshness = Freshness;
	Destroy();
	return true;
}

void AGamerGroundItem::TickSpoilage()
{
	const FGamerItemDef* Def = GetItemDef();
	if (!Def || bTaken)
	{
		return;
	}
	const float Seconds = UGamerSpoilage::Get().TickSeconds;
	if (!Def->Spoils())
	{
		// Spoiled food (or anything that does not spoil further) rots away after a while.
		RotSeconds += Seconds;
		if (RotSeconds >= RotAwaySeconds)
		{
			UE_LOG(LogGamer, Log, TEXT("Ground item %s (%s) rotted away"), *GetName(), *Item.ToString());
			Destroy();
		}
		return;
	}
	Freshness = FMath::Max(0.f, Freshness - UGamerSpoilage::Loss(*Def, Seconds, EGamerSpoilPlace::Ground));
	if (Freshness <= 0.f)
	{
		UE_LOG(LogGamer, Log, TEXT("Ground item %s spoiled: %s -> %s"), *GetName(), *Item.ToString(), *Def->SpoilsInto.ToString());
		Item = Def->SpoilsInto;
		Freshness = 1.f;
		RotSeconds = 0.f;
		ApplyLook();
	}
	const uint8 Percent = static_cast<uint8>(UGamerSpoilage::Percent(Freshness));
	if (Percent != FreshPercent)
	{
		FreshPercent = Percent;
		ForceNetUpdate();
	}
}

void AGamerGroundItem::OnRep_Item()
{
	ApplyLook();
}

void AGamerGroundItem::ApplyLook()
{
	if (Item.IsNone() || Item == AppliedItem)
	{
		return;
	}
	AppliedItem = Item;
	const FGamerItemDef* Def = GetItemDef();
	UStaticMesh* Model = Def && !Def->WorldMesh.IsNull() ? Cast<UStaticMesh>(Def->WorldMesh.TryLoad()) : nullptr;
	if (Model)
	{
		Mesh->SetStaticMesh(Model);
		Mesh->SetRelativeScale3D(FVector::OneVector);
		Mesh->EmptyOverrideMaterials();
	}
	else
	{
		// Placeholder: a small ball, brown when spoiled, otherwise yellow-green (no red: readability rule).
		Mesh->SetStaticMesh(LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Sphere.Sphere")));
		Mesh->SetRelativeScale3D(FVector(0.08f));
		const bool bSpoiled = Def && !Def->Spoils() && Def->Category == EGamerItemCategory::Resource;
		if (UMaterialInstanceDynamic* Material = Mesh->CreateAndSetMaterialInstanceDynamic(0))
		{
			Material->SetVectorParameterValue(TEXT("Color"), bSpoiled ? FLinearColor(0.25f, 0.17f, 0.08f) : FLinearColor(0.75f, 0.65f, 0.15f));
		}
	}
	Mesh->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
}
