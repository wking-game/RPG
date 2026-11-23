#include "GamerInventoryComponent.h"
#include "GamerGroundItem.h"

#include "Gamer.h"
#include "GamerDataSubsystem.h"
#include "GamerSpoilage.h"
#include "GamerWorkComponent.h"
#include "Misc/ConfigCacheIni.h"
#include "Net/UnrealNetwork.h"
#include "TimerManager.h"

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
		GetWorld()->GetTimerManager().SetTimer(SpoilTimer, this, &UGamerInventoryComponent::TickSpoilage,
			FMath::Max(0.2f, UGamerSpoilage::Get().TickSeconds), true);
	}
}

void UGamerInventoryComponent::TickSpoilage()
{
	const UGamerDataSubsystem* Data = UGamerDataSubsystem::Get(this);
	if (!Data)
	{
		return;
	}
	const float Seconds = UGamerSpoilage::Get().TickSeconds;
	bool bChanged = false;
	for (FGamerInventorySlot& Slot : Slots)
	{
		const FGamerItemDef* Def = Slot.IsEmpty() ? nullptr : Data->FindItem(Slot.Item);
		if (!Def || !Def->Spoils())
		{
			continue;
		}
		const float Before = Slot.Freshness;
		Slot.Freshness = FMath::Max(0.f, Slot.Freshness - UGamerSpoilage::Loss(*Def, Seconds, EGamerSpoilPlace::Carried));
		bChanged |= Slot.Freshness != Before;
		if (Slot.Freshness <= 0.f)
		{
			const FGamerItemDef* Into = Data->FindItem(Def->SpoilsInto);
			UE_LOG(LogGamer, Log, TEXT("Spoiled in inventory of %s: %s x%d -> %s"), *GetNameSafe(GetOwner()), *Def->Id.ToString(), Slot.Count,
				*Def->SpoilsInto.ToString());
			ClientSpoilNotice(FString::Printf(TEXT("Verdorben: %s."), *Def->Name));
			const int32 Count = Slot.Count;
			Slot = FGamerInventorySlot();
			if (Into)
			{
				Slot.Item = Into->Id;
				Slot.Count = FMath::Min(Count, FMath::Max(1, Into->MaxStack));
			}
		}
		else if (UGamerSpoilage::CrossedWarning(Before, Slot.Freshness))
		{
			UE_LOG(LogGamer, Log, TEXT("Spoil warning for %s: %s x%d at %d %%"), *GetNameSafe(GetOwner()), *Def->Id.ToString(), Slot.Count,
				UGamerSpoilage::Percent(Slot.Freshness));
			ClientSpoilNotice(FString::Printf(TEXT("Bald verdorben: %s (noch %d %%)."), *Def->Name, UGamerSpoilage::Percent(Slot.Freshness)));
		}
	}
	if (bChanged)
	{
		OnInventoryChanged.Broadcast();
	}
}

void UGamerInventoryComponent::ClientSpoilNotice_Implementation(const FString& Text)
{
	// Beginner tip once per player (decision 2026-11-14), remembered in the local user settings.
	static const TCHAR* const Section = TEXT("/Script/Gamer.GamerTips");
	bool bTipShown = false;
	GConfig->GetBool(Section, TEXT("bSpoilTipShown"), bTipShown, GGameUserSettingsIni);
	FString Message = Text;
	if (!bTipShown)
	{
		Message += TEXT("  Tipp: Verderbliches trocken und geschützt lagern – im Lager hält es viel länger.");
		GConfig->SetBool(Section, TEXT("bSpoilTipShown"), true, GGameUserSettingsIni);
		GConfig->Flush(false, GGameUserSettingsIni);
		UE_LOG(LogGamer, Log, TEXT("Spoil tip shown (once)"));
	}
	if (UGamerWorkComponent* Work = GetOwner() ? GetOwner()->FindComponentByClass<UGamerWorkComponent>() : nullptr)
	{
		Work->ShowMessage(Message);
	}
}

void UGamerInventoryComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION(UGamerInventoryComponent, Slots, COND_OwnerOnly);
	DOREPLIFETIME(UGamerInventoryComponent, SelectedSlot);
}

const FGamerItemDef* UGamerInventoryComponent::GetSelectedItemDef() const
{
	if (!Slots.IsValidIndex(SelectedSlot) || SelectedSlot >= GetHotbarSize() || Slots[SelectedSlot].IsEmpty())
	{
		return nullptr;
	}
	return FindDef(Slots[SelectedSlot].Item);
}

bool UGamerInventoryComponent::IsHoldingTool(FName ToolType, int32 MinTier) const
{
	const FGamerItemDef* Held = GetSelectedItemDef();
	return Held && Held->Category == EGamerItemCategory::Tool && Held->Tool.Type == ToolType && Held->Tier >= MinTier;
}

int32 UGamerInventoryComponent::FindToolInHotbar(FName ToolType, int32 MinTier) const
{
	int32 BestIndex = INDEX_NONE;
	const FGamerItemDef* Best = nullptr;
	for (int32 Index = 0; Index < GetHotbarSize(); ++Index)
	{
		const FGamerItemDef* Def = Slots[Index].IsEmpty() ? nullptr : FindDef(Slots[Index].Item);
		if (Def && Def->Category == EGamerItemCategory::Tool && Def->Tool.Type == ToolType && Def->Tier >= MinTier
			&& (!Best || Def->Tier > Best->Tier))
		{
			Best = Def;
			BestIndex = Index;
		}
	}
	return BestIndex;
}

void UGamerInventoryComponent::SelectSlot(int32 Index)
{
	if (Index < 0 || Index >= FMath::Max(1, FMath::Min(HotbarSize, SlotCount)))
	{
		return;
	}
	SelectedSlot = Index; // shown at once; the server's value replicates back
	if (GetOwner() && !GetOwner()->HasAuthority())
	{
		ServerSelectSlot(Index);
	}
}

void UGamerInventoryComponent::ServerSelectSlot_Implementation(int32 Index)
{
	if (Index >= 0 && Index < GetHotbarSize())
	{
		SelectedSlot = Index;
	}
}

void UGamerInventoryComponent::ServerSwapSlots_Implementation(int32 From, int32 To)
{
	if (!Slots.IsValidIndex(From) || !Slots.IsValidIndex(To) || From == To || Slots[From].IsEmpty())
	{
		return;
	}
	FGamerInventorySlot& A = Slots[From];
	FGamerInventorySlot& B = Slots[To];
	if (!B.IsEmpty() && B.Item == A.Item && B.Count < MaxStackOf(B.Item))
	{
		const int32 Moved = FMath::Min(A.Count, MaxStackOf(B.Item) - B.Count);
		B.Freshness = UGamerSpoilage::Merge(B.Count, B.Freshness, Moved, A.Freshness);
		B.Count += Moved;
		A.Count -= Moved;
		if (A.Count <= 0) { A = FGamerInventorySlot(); }
	}
	else
	{
		Swap(A, B);
	}
	UE_LOG(LogGamer, Log, TEXT("Inventory: moved slot %d to %d"), From + 1, To + 1);
	OnInventoryChanged.Broadcast();
}

void UGamerInventoryComponent::ServerDropSlot_Implementation(int32 Index, int32 Count)
{
	if (!Slots.IsValidIndex(Index) || Slots[Index].IsEmpty() || !GetOwner())
	{
		return;
	}
	FGamerInventorySlot& Slot = Slots[Index];
	const int32 Amount = FMath::Clamp(Count, 1, Slot.Count);
	const FName Item = Slot.Item;
	const float Freshness = Slot.Freshness;
	Slot.Count -= Amount;
	if (Slot.Count <= 0) { Slot = FGamerInventorySlot(); }
	const FVector Ground = GetOwner()->GetActorLocation() + GetOwner()->GetActorForwardVector() * 130.f - FVector(0.f, 0.f, 88.f);
	AGamerGroundItem::SpawnFallen(GetWorld(), Item, Amount, Freshness, Ground, 40.f);
	UE_LOG(LogGamer, Log, TEXT("Inventory: dropped %s x%d from slot %d"), *Item.ToString(), Amount, Index + 1);
	OnInventoryChanged.Broadcast();
}

bool UGamerInventoryComponent::SelectBestToolInHotbar(FName ToolType, int32 MinTier)
{
	const int32 Best = FindToolInHotbar(ToolType, MinTier);
	if (Best != INDEX_NONE && Best != SelectedSlot)
	{
		SelectedSlot = Best;
	}
	return IsHoldingTool(ToolType, MinTier);
}

TArray<FGamerItemAmount> UGamerInventoryComponent::TakeEverything()
{
	TArray<FGamerItemAmount> Result;
	for (FGamerInventorySlot& Slot : Slots)
	{
		if (!Slot.IsEmpty())
		{
			FGamerItemAmount Amount;
			Amount.Item = Slot.Item;
			Amount.Count = Slot.Count;
			Amount.Freshness = Slot.Freshness;
			Result.Add(Amount);
		}
		Slot = FGamerInventorySlot();
	}
	OnInventoryChanged.Broadcast();
	return Result;
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

int32 UGamerInventoryComponent::AddItem(FName Item, int32 Count, float Freshness)
{
	Freshness = FMath::Clamp(Freshness, 0.f, 1.f);
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
			Slot.Freshness = UGamerSpoilage::Merge(Slot.Count, Slot.Freshness, Moved, Freshness);
			Slot.Count += Moved;
			Remaining -= Moved;
		}
	}

	// Tools and food go to the hotbar first, building materials to the backpack first,
	// so the hotbar keeps room for the things you hold in your hand.
	const FGamerItemDef* Def = FindDef(Item);
	const bool bPreferHotbar = Def->Category == EGamerItemCategory::Tool || Def->Category == EGamerItemCategory::Weapon
		|| Def->Category == EGamerItemCategory::Food;
	const int32 Hotbar = GetHotbarSize();
	TArray<int32> Order;
	for (int32 Index = bPreferHotbar ? 0 : Hotbar; Index < Slots.Num(); ++Index)
	{
		Order.Add(Index);
	}
	if (!bPreferHotbar)
	{
		for (int32 Index = 0; Index < Hotbar; ++Index)
		{
			Order.Add(Index);
		}
	}
	for (const int32 Index : Order)
	{
		FGamerInventorySlot& Slot = Slots[Index];
		if (Remaining > 0 && Slot.IsEmpty())
		{
			const int32 Moved = FMath::Min(Remaining, MaxStack);
			Slot.Item = Item;
			Slot.Count = Moved;
			Slot.Freshness = Freshness;
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

float UGamerInventoryComponent::FreshnessOf(FName Item) const
{
	int32 Total = 0;
	float Sum = 0.f;
	for (const FGamerInventorySlot& Slot : Slots)
	{
		if (Slot.Item == Item && Slot.Count > 0)
		{
			Total += Slot.Count;
			Sum += Slot.Count * Slot.Freshness;
		}
	}
	return Total > 0 ? Sum / Total : 1.f;
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
		const float Fresh = FreshnessOf(Pair.Key);
		Parts.Add(Fresh < 0.995f ? FString::Printf(TEXT("%s x%d (%d %%)"), *Pair.Key.ToString(), Pair.Value, UGamerSpoilage::Percent(Fresh))
			: FString::Printf(TEXT("%s x%d"), *Pair.Key.ToString(), Pair.Value));
	}
	return FString::Join(Parts, TEXT(", "));
}
