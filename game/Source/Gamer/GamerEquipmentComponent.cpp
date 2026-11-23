#include "GamerEquipmentComponent.h"

#include "Gamer.h"
#include "GamerDataSubsystem.h"
#include "GamerHealthComponent.h"
#include "GamerInventoryComponent.h"
#include "Net/UnrealNetwork.h"

UGamerEquipmentComponent::UGamerEquipmentComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
	Worn.SetNum(PlaceCount);
}

void UGamerEquipmentComponent::BeginPlay()
{
	Super::BeginPlay();
	Worn.SetNum(PlaceCount);
	Rebuild();
}

void UGamerEquipmentComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UGamerEquipmentComponent, Worn);
}

FName UGamerEquipmentComponent::SlotOfPlace(int32 Place)
{
	static const FName Slots[PlaceCount] = { TEXT("helmet"), TEXT("shoulder"), TEXT("chest"), TEXT("hands"), TEXT("legs"), TEXT("feet"),
		TEXT("cloak"), TEXT("ring"), TEXT("ring") };
	return (Place >= 0 && Place < PlaceCount) ? Slots[Place] : NAME_None;
}

TArray<int32> UGamerEquipmentComponent::PlacesOfSlot(FName Slot)
{
	TArray<int32> Places;
	for (int32 Place = 0; Place < PlaceCount; ++Place)
	{
		if (SlotOfPlace(Place) == Slot)
		{
			Places.Add(Place);
		}
	}
	return Places;
}

const FGamerItemDef* UGamerEquipmentComponent::GetWornDef(int32 Place) const
{
	const FName Item = GetWornAt(Place);
	const UGamerDataSubsystem* Data = UGamerDataSubsystem::Get(this);
	return (Item.IsNone() || !Data) ? nullptr : Data->FindItem(Item);
}

void UGamerEquipmentComponent::Rebuild()
{
	TArray<const FGamerItemDef*> Defs;
	for (int32 Place = 0; Place < Worn.Num(); ++Place)
	{
		Defs.Add(GetWornDef(Place));
	}
	Totals = GamerEquipment::Summarize(Defs);
	OnEquipmentChanged.Broadcast();
}

void UGamerEquipmentComponent::OnRep_Worn()
{
	Rebuild();
}

bool UGamerEquipmentComponent::CanWork() const
{
	const UGamerHealthComponent* Health = GetOwner() ? GetOwner()->FindComponentByClass<UGamerHealthComponent>() : nullptr;
	return GetOwner() && GetOwner()->HasAuthority() && !(Health && Health->IsIncapacitated());
}

bool UGamerEquipmentComponent::EquipFromInventory(FName Item, int32 PreferredPlace, FString& OutWhy)
{
	UGamerInventoryComponent* Inventory = GetOwner() ? GetOwner()->FindComponentByClass<UGamerInventoryComponent>() : nullptr;
	const UGamerDataSubsystem* Data = UGamerDataSubsystem::Get(this);
	const FGamerItemDef* Def = Data ? Data->FindItem(Item) : nullptr;
	if (!CanWork() || !Inventory || !Def)
	{
		OutWhy = TEXT("Jetzt nicht möglich.");
		return false;
	}
	if (Def->Category != EGamerItemCategory::Armor)
	{
		OutWhy = FString::Printf(TEXT("%s kann man nicht anlegen."), *Def->Name);
		return false;
	}
	if (Inventory->CountOf(Item) < 1)
	{
		OutWhy = FString::Printf(TEXT("%s ist nicht im Inventar."), *Def->Name);
		return false;
	}
	const TArray<int32> Candidates = PlacesOfSlot(Def->Armor.Slot);
	if (Candidates.Num() == 0)
	{
		OutWhy = TEXT("Dafür gibt es keinen Platz.");
		return false;
	}
	// Place: the wanted one if it fits, else the first empty one, else the first.
	int32 Place = Candidates[0];
	if (Candidates.Contains(PreferredPlace))
	{
		Place = PreferredPlace;
	}
	else
	{
		for (const int32 Candidate : Candidates)
		{
			if (GetWornAt(Candidate).IsNone()) { Place = Candidate; break; }
		}
	}

	const FName Old = GetWornAt(Place);
	if (!Inventory->RemoveItems({ FGamerItemAmount{ Item, 1 } }))
	{
		OutWhy = TEXT("Jetzt nicht möglich.");
		return false;
	}
	if (!Old.IsNone() && Inventory->AddItem(Old, 1) > 0)
	{
		Inventory->AddItem(Item, 1);   // put it back, nothing changed
		OutWhy = TEXT("Das Inventar ist voll, das alte Stück hat keinen Platz.");
		return false;
	}
	Worn[Place] = Item;
	Rebuild();
	if (GetOwner()) { GetOwner()->ForceNetUpdate(); }
	UE_LOG(LogGamer, Log, TEXT("Equipment: %s put on (place %d, %s)"), *Item.ToString(), Place, *Def->Armor.Slot.ToString());
	return true;
}

bool UGamerEquipmentComponent::Unequip(int32 Place, FString& OutWhy)
{
	UGamerInventoryComponent* Inventory = GetOwner() ? GetOwner()->FindComponentByClass<UGamerInventoryComponent>() : nullptr;
	const FName Item = GetWornAt(Place);
	if (!CanWork() || !Inventory || Item.IsNone())
	{
		OutWhy = TEXT("Jetzt nicht möglich.");
		return false;
	}
	if (Inventory->AddItem(Item, 1) > 0)
	{
		OutWhy = TEXT("Das Inventar ist voll.");
		return false;
	}
	Worn[Place] = NAME_None;
	Rebuild();
	if (GetOwner()) { GetOwner()->ForceNetUpdate(); }
	UE_LOG(LogGamer, Log, TEXT("Equipment: %s taken off (place %d)"), *Item.ToString(), Place);
	return true;
}

FName UGamerEquipmentComponent::FindCandidateFor(int32 Place) const
{
	const UGamerInventoryComponent* Inventory = GetOwner() ? GetOwner()->FindComponentByClass<UGamerInventoryComponent>() : nullptr;
	const UGamerDataSubsystem* Data = UGamerDataSubsystem::Get(this);
	if (!Inventory || !Data)
	{
		return NAME_None;
	}
	const FName Slot = SlotOfPlace(Place);
	FName Best = NAME_None;
	const FGamerItemDef* BestDef = nullptr;
	for (const FGamerInventorySlot& Entry : Inventory->GetSlots())
	{
		const FGamerItemDef* Def = Entry.IsEmpty() ? nullptr : Data->FindItem(Entry.Item);
		if (Def && Def->Category == EGamerItemCategory::Armor && Def->Armor.Slot == Slot
			&& (!BestDef || Def->Tier > BestDef->Tier || (Def->Tier == BestDef->Tier && Def->Armor.Armor > BestDef->Armor.Armor)))
		{
			Best = Entry.Item;
			BestDef = Def;
		}
	}
	return Best;
}

TArray<FGamerItemAmount> UGamerEquipmentComponent::TakeEverything()
{
	TArray<FGamerItemAmount> Out;
	for (FName& Item : Worn)
	{
		if (!Item.IsNone())
		{
			Out.Add(FGamerItemAmount{ Item, 1 });
			Item = NAME_None;
		}
	}
	if (Out.Num() > 0)
	{
		Rebuild();
		if (GetOwner()) { GetOwner()->ForceNetUpdate(); }
	}
	return Out;
}

void UGamerEquipmentComponent::ServerEquip_Implementation(FName Item, int32 PreferredPlace)
{
	FString Why;
	if (!EquipFromInventory(Item, PreferredPlace, Why))
	{
		ClientNotice(Why);
	}
}

void UGamerEquipmentComponent::ServerUnequip_Implementation(int32 Place)
{
	FString Why;
	if (!Unequip(Place, Why))
	{
		ClientNotice(Why);
	}
}

void UGamerEquipmentComponent::ClientNotice_Implementation(const FString& Text)
{
	LastNotice = Text;
	LastNoticeTime = GetWorld() ? GetWorld()->GetTimeSeconds() : 0.0;
}

FString UGamerEquipmentComponent::Describe() const
{
	FString Out;
	for (int32 Place = 0; Place < Worn.Num(); ++Place)
	{
		if (!Worn[Place].IsNone())
		{
			Out += FString::Printf(TEXT("%s%s=%s"), Out.IsEmpty() ? TEXT("") : TEXT(", "), *SlotOfPlace(Place).ToString(), *Worn[Place].ToString());
		}
	}
	return Out.IsEmpty() ? TEXT("nothing worn") : Out;
}

void UGamerEquipmentComponent::WriteSaveState(FGamerSaveRecord& Out) const
{
	Out.SetNames(TEXT("worn"), Worn);
}

void UGamerEquipmentComponent::ReadSaveState(const FGamerSaveRecord& In)
{
	Worn.Reset();
	Worn.SetNum(PlaceCount);
	const UGamerDataSubsystem* Data = UGamerDataSubsystem::Get(this);
	const TArray<FName> Saved = In.GetNames(TEXT("worn"));
	for (int32 Place = 0; Place < PlaceCount && Place < Saved.Num(); ++Place)
	{
		const FName Item = Saved[Place];
		if (Item.IsNone())
		{
			continue;
		}
		const FGamerItemDef* Def = Data ? Data->FindItem(Item) : nullptr;
		if (!Def || Def->Category != EGamerItemCategory::Armor || Def->Armor.Slot != SlotOfPlace(Place))
		{
			UE_LOG(LogGamer, Warning, TEXT("Save: worn piece \"%s\" at place %d is no longer valid, dropped"), *Item.ToString(), Place);
			continue;
		}
		Worn[Place] = Item;
	}
	Rebuild();
	if (GetOwner()) { GetOwner()->ForceNetUpdate(); }
}
