#include "GamerInventoryComponent.h"

#include "Gamer.h"
#include "GamerDataSubsystem.h"
#include "Net/UnrealNetwork.h"

UGamerInventoryComponent::UGamerInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
}

void UGamerInventoryComponent::BeginPlay()
{
	Super::BeginPlay();
	if (GetOwner() && GetOwner()->HasAuthority())
	{
		Slots.SetNum(FMath::Max(1, SlotCount));
	}
}

void UGamerInventoryComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION(UGamerInventoryComponent, Slots, COND_OwnerOnly);
}

void UGamerInventoryComponent::OnRep_Slots()
{
	OnInventoryChanged.Broadcast();
}

const FGamerItemDef* UGamerInventoryComponent::FindDef(FName Item) const
{
	const UGamerDataSubsystem* Data = UGamerDataSubsystem::Get(this);
	return Data ? Data->FindItem(Item) : nullptr;
}

int32 UGamerInventoryComponent::MaxStackOf(FName Item) const
{
	const FGamerItemDef* Def = FindDef(Item);
	return Def ? FMath::Max(1, Def->MaxStack) : 1;
}

int32 UGamerInventoryComponent::AddItem(FName Item, int32 Count)
{
	if (!GetOwner() || !GetOwner()->HasAuthority() || Item.IsNone() || Count <= 0)
	{
		return Count;
	}
	if (!FindDef(Item))
	{
		UE_LOG(LogGamer, Error, TEXT("Inventory: unknown item %s"), *Item.ToString());
		return Count;
	}

	const int32 MaxStack = MaxStackOf(Item);
	int32 Remaining = Count;

	// Fill existing stacks first, then empty slots.
	for (FGamerInventorySlot& Slot : Slots)
	{
		if (Remaining > 0 && Slot.Item == Item && Slot.Count < MaxStack)
		{
			const int32 Moved = FMath::Min(Remaining, MaxStack - Slot.Count);
			Slot.Count += Moved;
			Remaining -= Moved;
		}
	}
	for (FGamerInventorySlot& Slot : Slots)
	{
		if (Remaining > 0 && Slot.IsEmpty())
		{
			const int32 Moved = FMath::Min(Remaining, MaxStack);
			Slot.Item = Item;
			Slot.Count = Moved;
			Remaining -= Moved;
		}
	}

	if (Remaining != Count)
	{
		OnInventoryChanged.Broadcast();
	}
	return Remaining;
}

bool UGamerInventoryComponent::RemoveItems(const TArray<FGamerItemAmount>& Amounts)
{
	if (!GetOwner() || !GetOwner()->HasAuthority() || !HasAll(Amounts))
	{
		return false;
	}
	for (const FGamerItemAmount& Amount : Amounts)
	{
		int32 Remaining = Amount.Count;
		// Take from the last slots first so the front of the inventory stays stable.
		for (int32 Index = Slots.Num() - 1; Index >= 0 && Remaining > 0; --Index)
		{
			FGamerInventorySlot& Slot = Slots[Index];
			if (Slot.Item == Amount.Item)
			{
				const int32 Taken = FMath::Min(Remaining, Slot.Count);
				Slot.Count -= Taken;
				Remaining -= Taken;
				if (Slot.Count <= 0)
				{
					Slot = FGamerInventorySlot();
				}
			}
		}
	}
	OnInventoryChanged.Broadcast();
	return true;
}

int32 UGamerInventoryComponent::CountOf(FName Item) const
{
	int32 Total = 0;
	for (const FGamerInventorySlot& Slot : Slots)
	{
		if (Slot.Item == Item)
		{
			Total += Slot.Count;
		}
	}
	return Total;
}

bool UGamerInventoryComponent::HasAll(const TArray<FGamerItemAmount>& Amounts) const
{
	for (const FGamerItemAmount& Amount : Amounts)
	{
		if (CountOf(Amount.Item) < Amount.Count)
		{
			return false;
		}
	}
	return true;
}

int32 UGamerInventoryComponent::SpaceFor(FName Item) const
{
	const int32 MaxStack = MaxStackOf(Item);
	int32 Space = 0;
	for (const FGamerInventorySlot& Slot : Slots)
	{
		if (Slot.IsEmpty())
		{
			Space += MaxStack;
		}
		else if (Slot.Item == Item)
		{
			Space += FMath::Max(0, MaxStack - Slot.Count);
		}
	}
	return Space;
}

const FGamerItemDef* UGamerInventoryComponent::FindBestTool(FName ToolType, int32 MinTier) const
{
	const FGamerItemDef* Best = nullptr;
	for (const FGamerInventorySlot& Slot : Slots)
	{
		if (Slot.IsEmpty())
		{
			continue;
		}
		const FGamerItemDef* Def = FindDef(Slot.Item);
		if (Def && Def->Category == EGamerItemCategory::Tool && Def->Tool.Type == ToolType && Def->Tier >= MinTier)
		{
			if (!Best || Def->Tier > Best->Tier || (Def->Tier == Best->Tier && Def->Tool.WorkSpeed > Best->Tool.WorkSpeed))
			{
				Best = Def;
			}
		}
	}
	return Best;
}

float UGamerInventoryComponent::GetTotalWeightKg() const
{
	float Total = 0.f;
	for (const FGamerInventorySlot& Slot : Slots)
	{
		if (!Slot.IsEmpty())
		{
			if (const FGamerItemDef* Def = FindDef(Slot.Item))
			{
				Total += Def->WeightKg * Slot.Count;
			}
		}
	}
	return Total;
}

FString UGamerInventoryComponent::Describe() const
{
	TMap<FName, int32> Totals;
	for (const FGamerInventorySlot& Slot : Slots)
	{
		if (!Slot.IsEmpty())
		{
			Totals.FindOrAdd(Slot.Item) += Slot.Count;
		}
	}
	if (Totals.Num() == 0)
	{
		return TEXT("(leer)");
	}
	Totals.KeySort(FNameLexicalLess());
	TArray<FString> Parts;
	for (const TPair<FName, int32>& Pair : Totals)
	{
		Parts.Add(FString::Printf(TEXT("%s x%d"), *Pair.Key.ToString(), Pair.Value));
	}
	return FString::Join(Parts, TEXT(", "));
}
