#include "GamerGrave.h"

#include "Gamer.h"
#include "GamerInventoryComponent.h"
#include "GameFramework/GameStateBase.h"
#include "GamerSaveable.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Engine/World.h"
#include "Net/UnrealNetwork.h"
#include "UObject/ConstructorHelpers.h"

AGamerGrave::AGamerGrave()
{
	bReplicates = true;
	PrimaryActorTick.bCanEverTick = false;
	SetNetCullDistanceSquared(FMath::Square(8000.f));

	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(TEXT("/Engine/BasicShapes/Cube.Cube"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> CylinderMesh(TEXT("/Engine/BasicShapes/Cylinder.Cylinder"));

	Mound = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mound"));
	SetRootComponent(Mound);
	Mound->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Mound->SetRelativeScale3D(FVector(0.6f, 1.7f, 0.22f));   // a low mound, 60 x 170 cm
	if (CubeMesh.Succeeded())
	{
		Mound->SetStaticMesh(CubeMesh.Object);
	}
	Stake = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Stake"));
	Stake->SetupAttachment(Mound);
	Stake->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Stake->SetRelativeLocation(FVector(-30.f, 0.f, 220.f));
	Stake->SetRelativeScale3D(FVector(0.12f / 0.6f, 0.12f / 1.7f, 0.9f / 0.22f));   // a 90 cm stake at the head end
	if (CylinderMesh.Succeeded())
	{
		Stake->SetStaticMesh(CylinderMesh.Object);
	}
}

void AGamerGrave::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AGamerGrave, OwnerName);
	DOREPLIFETIME(AGamerGrave, OwnerKey);
	DOREPLIFETIME(AGamerGrave, ItemCount);
	DOREPLIFETIME(AGamerGrave, DeathTime);
}

double AGamerGrave::ServerNow() const
{
	const AGameStateBase* State = GetWorld() ? GetWorld()->GetGameState() : nullptr;
	return State ? State->GetServerWorldTimeSeconds() : (GetWorld() ? GetWorld()->GetTimeSeconds() : 0.0);
}

AGamerGrave* AGamerGrave::Create(UWorld* World, const FVector& Location, const FString& InOwnerKey, const FString& InOwnerName, const TArray<FGamerItemAmount>& InItems)
{
	if (!World)
	{
		return nullptr;
	}
	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	AGamerGrave* Grave = World->SpawnActor<AGamerGrave>(Location, FRotator::ZeroRotator, Params);
	if (Grave)
	{
		Grave->OwnerKey = InOwnerKey;
		Grave->OwnerName = InOwnerName;
		Grave->Items = InItems;
		Grave->DeathTime = Grave->ServerNow();
		Grave->RefreshCount();
		UE_LOG(LogGamer, Log, TEXT("Grave: %s at %s with %d pieces"), *InOwnerName, *Location.ToCompactString(), Grave->ItemCount);
	}
	return Grave;
}

float AGamerGrave::GetAgeSeconds() const
{
	return static_cast<float>(ServerNow() - DeathTime);
}

void AGamerGrave::RefreshCount()
{
	ItemCount = 0;
	for (const FGamerItemAmount& Amount : Items)
	{
		ItemCount += FMath::Max(0, Amount.Count);
	}
	ForceNetUpdate();
}

int32 AGamerGrave::CollectInto(UGamerInventoryComponent* Inventory)
{
	if (!HasAuthority() || !Inventory)
	{
		return 0;
	}
	int32 Moved = 0;
	for (FGamerItemAmount& Amount : Items)
	{
		if (Amount.Count <= 0)
		{
			continue;
		}
		const int32 NotFitting = Inventory->AddItem(Amount.Item, Amount.Count, Amount.Freshness);
		Moved += Amount.Count - NotFitting;
		Amount.Count = NotFitting;
	}
	Items.RemoveAll([](const FGamerItemAmount& A) { return A.Count <= 0; });
	RefreshCount();
	if (Items.Num() == 0)
	{
		Destroy();
	}
	return Moved;
}

void AGamerGrave::WriteSaveState(FGamerSaveRecord& Out) const
{
	Out.SetString(TEXT("owner_key"), OwnerKey);
	Out.SetString(TEXT("owner_name"), OwnerName);
	Out.SetItems(TEXT("items"), Items);
	Out.SetNumber(TEXT("age_s"), GetAgeSeconds());   // the protection time keeps running across save and load
}

void AGamerGrave::ReadSaveState(const FGamerSaveRecord& In)
{
	OwnerKey = In.GetString(TEXT("owner_key"));
	OwnerName = In.GetString(TEXT("owner_name"));
	Items = In.GetItems(TEXT("items"));
	DeathTime = ServerNow() - In.GetFloat(TEXT("age_s"), 0.f);
	RefreshCount();
}
