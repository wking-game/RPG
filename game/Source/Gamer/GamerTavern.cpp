#include "GamerTavern.h"

#include "GamerDataSubsystem.h"
#include "GamerSpoilage.h"
#include "EngineUtils.h"
#include "Net/UnrealNetwork.h"

void AGamerTavern::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AGamerTavern, Stock);
}

int32 AGamerTavern::CountStock(FName Item) const
{
	int32 Sum = 0;
	for (const FGamerItemAmount& Amount : Stock)
	{
		Sum += Amount.Item == Item ? Amount.Count : 0;
	}
	return Sum;
}

int32 AGamerTavern::TotalStock() const
{
	int32 Sum = 0;
	for (const FGamerItemAmount& Amount : Stock)
	{
		Sum += Amount.Count;
	}
	return Sum;
}

int32 AGamerTavern::FreeSpace() const
{
	const FGamerBuildableDef* Def = GetDef();
	return Def ? FMath::Max(0, Def->Tavern.Stock - TotalStock()) : 0;
}

int32 AGamerTavern::AddStock(FName Item, int32 Count, float Freshness)
{
	const int32 Taken = FMath::Min(Count, FreeSpace());
	if (!HasAuthority() || Taken <= 0 || Item.IsNone())
	{
		return 0;
	}
	if (FGamerItemAmount* Existing = Stock.FindByPredicate([Item](const FGamerItemAmount& A) { return A.Item == Item; }))
	{
		Existing->Freshness = UGamerSpoilage::Merge(Existing->Count, Existing->Freshness, Taken, Freshness);
		Existing->Count += Taken;
	}
	else
	{
		FGamerItemAmount& Added = Stock.AddDefaulted_GetRef();
		Added.Item = Item;
		Added.Count = Taken;
		Added.Freshness = Freshness;
	}
	ForceNetUpdate();
	return Taken;
}

int32 AGamerTavern::TakeStock(FName Item, int32 Count)
{
	const int32 Index = Stock.IndexOfByPredicate([Item](const FGamerItemAmount& A) { return A.Item == Item; });
	if (!HasAuthority() || Index == INDEX_NONE || Count <= 0)
	{
		return 0;
	}
	const int32 Taken = FMath::Min(Count, Stock[Index].Count);
	Stock[Index].Count -= Taken;
	if (Stock[Index].Count <= 0)
	{
		Stock.RemoveAt(Index);
	}
	ForceNetUpdate();
	return Taken;
}

FString AGamerTavern::DescribeStock() const
{
	TArray<FString> Parts;
	for (const FGamerItemAmount& Amount : Stock)
	{
		Parts.Add(FString::Printf(TEXT("%s x%d"), *Amount.Item.ToString(), Amount.Count));
	}
	return Parts.Num() > 0 ? FString::Join(Parts, TEXT(", ")) : TEXT("empty");
}

AGamerBuilding* AGamerTavern::FindNear(const UWorld* World, FName BuildableId, int32 Level, const FVector& Location, float Radius)
{
	AGamerBuilding* Best = nullptr;
	float BestDistance = Radius;
	for (TActorIterator<AGamerBuilding> It(World); World && It; ++It)
	{
		if (It->IsPreview() || !It->IsFinished() || It->GetBuildableId() != BuildableId || It->GetLevel() < Level)
		{
			continue;
		}
		const float Distance = FVector::Dist2D(It->GetActorLocation(), Location);
		if (Distance <= BestDistance)
		{
			BestDistance = Distance;
			Best = *It;
		}
	}
	return Best;
}

FString AGamerTavern::CheckRequirement(const UWorld* World, const FGamerBuildableDef& Def, const FVector& Location)
{
	const FGamerBuildRequirement& Req = Def.Requires;
	if (!Req.IsSet() || FindNear(World, Req.Building, Req.Level, Location, Req.RadiusCm))
	{
		return FString();
	}
	const UGamerDataSubsystem* Data = World ? UGamerDataSubsystem::Get(World) : nullptr;
	const FGamerBuildableDef* Needed = Data ? Data->FindBuildable(Req.Building) : nullptr;
	const FString Name = Needed ? Needed->Name : Req.Building.ToString();
	return Req.Level > 1
		? FString::Printf(TEXT("Braucht eine fertige %s (Stufe %d) im Umkreis von %.0f m"), *Name, Req.Level, Req.RadiusCm / 100.f)
		: FString::Printf(TEXT("Braucht eine fertige %s im Umkreis von %.0f m"), *Name, Req.RadiusCm / 100.f);
}
