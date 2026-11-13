#include "GamerSettlement.h"

#include "Gamer.h"
#include "GamerBuilding.h"
#include "GamerCharacter.h"
#include "GamerDataSubsystem.h"
#include "GamerVillager.h"
#include "GamerWorkComponent.h"
#include "EngineUtils.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "HAL/IConsoleManager.h"
#include "Misc/CommandLine.h"
#include "Misc/Parse.h"
#include "Net/UnrealNetwork.h"

namespace
{
	TAutoConsoleVariable<float> CVarVillagerTimeScale(
		TEXT("gamer.VillagerTimeScale"), 1.f,
		TEXT("Speeds up villager needs and arrivals (tests). 1 = normal (one game day = DayLengthSeconds)."));

	const FName StorageId(TEXT("building_storage"));
	const FName BedId(TEXT("building_bed"));

	// Old Norse style names (decision 2026-11-13: names now, traits from Phase 2). Mixed, not bound to a body type.
	const TCHAR* const Names[] = {
		TEXT("Astrid"), TEXT("Bjarni"), TEXT("Sigrun"), TEXT("Halvard"), TEXT("Ingrid"), TEXT("Ketil"), TEXT("Ragna"), TEXT("Torvald"),
		TEXT("Gudrun"), TEXT("Einar"), TEXT("Sölvi"), TEXT("Freydis"), TEXT("Ulf"), TEXT("Hild"), TEXT("Orm"), TEXT("Tora"),
	};
}

AGamerSettlement::AGamerSettlement()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickInterval = 0.5f;
	bReplicates = true;
	bAlwaysRelevant = true;
	SetReplicatingMovement(false);
	SetRootComponent(CreateDefaultSubobject<USceneComponent>(TEXT("Root")));
}

void AGamerSettlement::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AGamerSettlement, Storage);
	DOREPLIFETIME(AGamerSettlement, Capacity);
	DOREPLIFETIME(AGamerSettlement, FreeBeds);
	DOREPLIFETIME(AGamerSettlement, ArrivalProgress);
}

void AGamerSettlement::BeginPlay()
{
	Super::BeginPlay();
	float Scale = 0.f;
	if (HasAuthority() && FParse::Value(FCommandLine::Get(), TEXT("GamerVillagerTimeScale="), Scale) && Scale > 0.f)
	{
		CVarVillagerTimeScale->Set(Scale, ECVF_SetByCommandline);
		UE_LOG(LogGamer, Log, TEXT("Villager time scale %.1f"), Scale);
	}
}

AGamerSettlement* AGamerSettlement::Get(const UObject* WorldContext)
{
	const UWorld* World = WorldContext ? WorldContext->GetWorld() : nullptr;
	if (!World)
	{
		return nullptr;
	}
	for (TActorIterator<AGamerSettlement> It(const_cast<UWorld*>(World)); It; ++It)
	{
		return *It;
	}
	return nullptr;
}

float AGamerSettlement::ToGameDays(float RealSeconds) const
{
	return RealSeconds * FMath::Max(0.f, CVarVillagerTimeScale.GetValueOnGameThread()) / FMath::Max(1.f, DayLengthSeconds);
}

// --- Storage ------------------------------------------------------------------------------------

int32 AGamerSettlement::CountInStorage(FName Item) const
{
	const FGamerItemAmount* Found = Storage.FindByPredicate([Item](const FGamerItemAmount& A) { return A.Item == Item; });
	return Found ? Found->Count : 0;
}

bool AGamerSettlement::HasInStorage(const TArray<FGamerItemAmount>& Amounts) const
{
	for (const FGamerItemAmount& Amount : Amounts)
	{
		if (CountInStorage(Amount.Item) < Amount.Count)
		{
			return false;
		}
	}
	return true;
}

int32 AGamerSettlement::GetStoredTotal() const
{
	int32 Total = 0;
	for (const FGamerItemAmount& Amount : Storage)
	{
		Total += Amount.Count;
	}
	return Total;
}

AGamerBuilding* AGamerSettlement::FindNearestStorageBuilding(const FVector& From) const
{
	AGamerBuilding* Best = nullptr;
	float BestDistance = TNumericLimits<float>::Max();
	for (TActorIterator<AGamerBuilding> It(GetWorld()); It; ++It)
	{
		if (!It->IsPreview() && It->IsFinished() && It->GetBuildableId() == StorageId)
		{
			const float Distance = FVector::Dist(It->GetActorLocation(), From);
			if (Distance < BestDistance)
			{
				BestDistance = Distance;
				Best = *It;
			}
		}
	}
	return Best;
}

int32 AGamerSettlement::AddToStorage(FName Item, int32 Count)
{
	if (!HasAuthority() || Item.IsNone() || Count <= 0)
	{
		return Count;
	}
	const int32 Fits = FMath::Clamp(Capacity - GetStoredTotal(), 0, Count);
	if (Fits > 0)
	{
		FGamerItemAmount* Found = Storage.FindByPredicate([Item](const FGamerItemAmount& A) { return A.Item == Item; });
		if (Found)
		{
			Found->Count += Fits;
		}
		else
		{
			FGamerItemAmount& Added = Storage.AddDefaulted_GetRef();
			Added.Item = Item;
			Added.Count = Fits;
		}
		ForceNetUpdate();
	}
	return Count - Fits;
}

int32 AGamerSettlement::TakeFromStorage(FName Item, int32 Count)
{
	if (!HasAuthority() || Count <= 0)
	{
		return 0;
	}
	const int32 Index = Storage.IndexOfByPredicate([Item](const FGamerItemAmount& A) { return A.Item == Item; });
	if (Index == INDEX_NONE)
	{
		return 0;
	}
	const int32 Taken = FMath::Min(Count, Storage[Index].Count);
	Storage[Index].Count -= Taken;
	if (Storage[Index].Count <= 0)
	{
		Storage.RemoveAt(Index);
	}
	ForceNetUpdate();
	return Taken;
}

bool AGamerSettlement::TakeAllFromStorage(const TArray<FGamerItemAmount>& Amounts)
{
	if (!HasAuthority() || !HasInStorage(Amounts))
	{
		return false;
	}
	for (const FGamerItemAmount& Amount : Amounts)
	{
		TakeFromStorage(Amount.Item, Amount.Count);
	}
	return true;
}

FName AGamerSettlement::FindToolInStorage(FName ToolType) const
{
	const UGamerDataSubsystem* Data = UGamerDataSubsystem::Get(this);
	FName Best;
	int32 BestTier = -1;
	for (const FGamerItemAmount& Amount : Storage)
	{
		const FGamerItemDef* Def = Data ? Data->FindItem(Amount.Item) : nullptr;
		if (Def && Amount.Count > 0 && Def->Category == EGamerItemCategory::Tool && Def->Tool.Type == ToolType && Def->Tier > BestTier)
		{
			Best = Amount.Item;
			BestTier = Def->Tier;
		}
	}
	return Best;
}

// --- Villagers ----------------------------------------------------------------------------------

bool AGamerSettlement::IsReservedByOther(const AActor* Target, const AGamerVillager* Villager) const
{
	const TWeakObjectPtr<AGamerVillager>* Holder = Reservations.Find(const_cast<AActor*>(Target));
	return Holder && Holder->IsValid() && Holder->Get() != Villager;
}

void AGamerSettlement::Reserve(AActor* Target, AGamerVillager* Villager)
{
	if (Target)
	{
		Reservations.Add(Target, Villager);
	}
}

void AGamerSettlement::ReleaseAll(AGamerVillager* Villager)
{
	for (auto It = Reservations.CreateIterator(); It; ++It)
	{
		if (!It->Value.IsValid() || It->Value.Get() == Villager || !It->Key.IsValid())
		{
			It.RemoveCurrent();
		}
	}
}

FString AGamerSettlement::NextName()
{
	return Names[NameCounter++ % UE_ARRAY_COUNT(Names)];
}

AGamerVillager* AGamerSettlement::SpawnVillager(const FVector& Location)
{
	if (!HasAuthority())
	{
		return nullptr;
	}
	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	AGamerVillager* Villager = GetWorld()->SpawnActor<AGamerVillager>(AGamerVillager::StaticClass(), Location + FVector(0.f, 0.f, 100.f), FRotator::ZeroRotator, Params);
	if (!Villager)
	{
		return nullptr;
	}
	// A free bed, if any.
	AGamerBuilding* FreeBed = nullptr;
	for (TActorIterator<AGamerBuilding> It(GetWorld()); It && !FreeBed; ++It)
	{
		if (!It->IsPreview() && It->IsFinished() && It->GetBuildableId() == BedId)
		{
			bool bTaken = false;
			for (TActorIterator<AGamerVillager> V(GetWorld()); V; ++V)
			{
				bTaken |= *V != Villager && V->GetBed() == *It;
			}
			FreeBed = bTaken ? nullptr : *It;
		}
	}
	const FString Name = NextName();
	Villager->InitVillager(Name, FreeBed);
	UE_LOG(LogGamer, Log, TEXT("Villager arrived: %s at %s bed=%s"), *Name, *Villager->GetActorLocation().ToCompactString(),
		FreeBed ? *FreeBed->GetName() : TEXT("none"));
	Broadcast(FString::Printf(TEXT("Neu in der Siedlung: %s"), *Name));
	return Villager;
}

void AGamerSettlement::Broadcast(const FString& Text)
{
	MulticastNews(Text);
}

void AGamerSettlement::MulticastNews_Implementation(const FString& Text)
{
	// Every local player sees it as a normal message.
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		const APlayerController* PC = It->Get();
		const AGamerCharacter* Character = PC && PC->IsLocalController() ? Cast<AGamerCharacter>(PC->GetPawn()) : nullptr;
		if (Character && Character->GetWork())
		{
			Character->GetWork()->ShowMessage(Text);
		}
	}
}

void AGamerSettlement::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	if (!HasAuthority())
	{
		return;
	}
	BuildingCheckTimer -= DeltaSeconds;
	if (BuildingCheckTimer <= 0.f)
	{
		BuildingCheckTimer = 2.f;
		UpdateBuildings();
	}

	// Arrivals: a free bed draws a villager after ArrivalGameDays (the progress waits while no bed is free).
	if (FreeBeds > 0)
	{
		ArrivalProgress += ToGameDays(DeltaSeconds) / FMath::Max(0.001f, ArrivalGameDays);
		if (ArrivalProgress >= 1.f)
		{
			ArrivalProgress = 0.f;
			AGamerBuilding* Anchor = FindNearestStorageBuilding(GetActorLocation());
			FVector Spot = GetActorLocation();
			for (TActorIterator<AGamerBuilding> It(GetWorld()); It; ++It)
			{
				if (!It->IsPreview() && It->IsFinished() && It->GetBuildableId() == BedId)
				{
					Spot = It->GetActorLocation();
					break;
				}
			}
			// Walk in from a little outside the settlement.
			const FVector From = (Anchor ? Anchor->GetActorLocation() : Spot) + FVector(1500.f, 0.f, 0.f);
			SpawnVillager(FVector(From.X, From.Y, Spot.Z));
			UpdateBuildings();
		}
	}
	else
	{
		ArrivalProgress = 0.f;
	}
}

void AGamerSettlement::UpdateBuildings()
{
	int32 Storages = 0;
	int32 Beds = 0;
	for (TActorIterator<AGamerBuilding> It(GetWorld()); It; ++It)
	{
		if (It->IsPreview() || !It->IsFinished())
		{
			continue;
		}
		Storages += It->GetBuildableId() == StorageId ? 1 : 0;
		Beds += It->GetBuildableId() == BedId ? 1 : 0;
	}
	int32 Assigned = 0;
	for (TActorIterator<AGamerVillager> It(GetWorld()); It; ++It)
	{
		Assigned += It->GetBed() ? 1 : 0;
	}
	const int32 NewCapacity = Storages * CapacityPerStorage;
	const int32 NewFreeBeds = FMath::Max(0, Beds - Assigned);
	if (NewCapacity != Capacity || NewFreeBeds != FreeBeds)
	{
		Capacity = NewCapacity;
		FreeBeds = NewFreeBeds;
		ForceNetUpdate();
	}
}

FString AGamerSettlement::Describe() const
{
	TArray<FString> Parts;
	for (const FGamerItemAmount& Amount : Storage)
	{
		Parts.Add(FString::Printf(TEXT("%s x%d"), *Amount.Item.ToString(), Amount.Count));
	}
	int32 Villagers = 0;
	for (TActorIterator<AGamerVillager> It(GetWorld()); It; ++It)
	{
		++Villagers;
	}
	return FString::Printf(TEXT("storage %d/%d [%s], villagers %d, free beds %d, arrival %.0f%%"), GetStoredTotal(), Capacity,
		*FString::Join(Parts, TEXT(", ")), Villagers, FreeBeds, ArrivalProgress * 100.f);
}
