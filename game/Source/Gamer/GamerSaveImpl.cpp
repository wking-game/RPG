// Save/load of the existing game classes (IGamerSaveable), kept in one file of the save system so the other
// systems' files only gain the interface declarations. Field keys are stable: never rename or reuse one.
// Remaining times end in "_s" (seconds; the round-trip check allows them to move a little).

#include "GamerSaveSubsystem.h"

#include "Gamer.h"
#include "GamerBuffComponent.h"
#include "GamerBuilding.h"
#include "GamerDataSubsystem.h"
#include "GamerFarmBuilding.h"
#include "GamerGameState.h"
#include "GamerGroundItem.h"
#include "GamerInventoryComponent.h"
#include "GamerResourceNode.h"
#include "GamerSettlement.h"
#include "GamerTavern.h"
#include "GamerVillager.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "TimerManager.h"

namespace
{
	float Remaining(const UWorld* World, const FTimerHandle& Handle)
	{
		return World && World->GetTimerManager().IsTimerActive(Handle) ? World->GetTimerManager().GetTimerRemaining(Handle) : -1.f;
	}
}

// --- Game state: world options and season -------------------------------------------------------------------

void AGamerGameState::WriteSaveState(FGamerSaveRecord& Out) const
{
	Out.SetString(TEXT("options"), WorldOptions.ToUrlOptions());
	Out.SetNumber(TEXT("season_phase"), GetSeasonPhase());
}

void AGamerGameState::ReadSaveState(const FGamerSaveRecord& In)
{
	if (In.Has(TEXT("options")))
	{
		SetWorldOptions(FGamerWorldOptions::FromUrlOptions(In.GetString(TEXT("options"))));
		UE_LOG(LogGamer, Log, TEXT("World options from the save: %s"), *WorldOptions.ToLogString());
	}
	if (In.Has(TEXT("season_phase")))
	{
		SetSeasonPhase(In.GetFloat(TEXT("season_phase"), StartSeasonPhase));
	}
}

// --- Settlement: storage and arrivals ---------------------------------------------------------------------------

void AGamerSettlement::WriteSaveState(FGamerSaveRecord& Out) const
{
	Out.SetItems(TEXT("storage"), Storage);
	Out.SetNumber(TEXT("arrival"), ArrivalProgress);
	Out.SetInt(TEXT("name_counter"), NameCounter);
}

void AGamerSettlement::ReadSaveState(const FGamerSaveRecord& In)
{
	Storage = In.GetItems(TEXT("storage"));
	GamerSave::FilterKnownItems(this, Storage, TEXT("storage"));
	ArrivalProgress = FMath::Clamp(In.GetFloat(TEXT("arrival")), 0.f, 1.f);
	NameCounter = FMath::Max(0, In.GetInt(TEXT("name_counter")));
	BuildingCheckTimer = 0.f; // recount storage buildings and beds on the next tick
	ForceNetUpdate();
}

// --- Resource nodes: depleted, regrow, growing stage, fruit cycle ------------------------------------------------

void AGamerResourceNode::WriteSaveState(FGamerSaveRecord& Out) const
{
	Out.SetName(TEXT("id"), NodeId);
	Out.SetBool(TEXT("depleted"), bDepleted);
	Out.SetInt(TEXT("grow_stage"), GrowStage);
	if (WorkProgress > 0.f)
	{
		Out.SetNumber(TEXT("work"), WorkProgress);
	}
	const float Regrow = Remaining(GetWorld(), RegrowTimer);
	if (Regrow >= 0.f)
	{
		Out.SetNumber(TEXT("regrow_s"), Regrow);
	}
	const float GrowNext = Remaining(GetWorld(), GrowTimer);
	if (GrowNext >= 0.f)
	{
		Out.SetNumber(TEXT("grow_next_s"), GrowNext);
		Out.SetNumber(TEXT("grow_step_s"), GrowStepSeconds);
	}
	if (bPlantedGrowing)
	{
		Out.SetBool(TEXT("planted"), true);
		Out.SetNumber(TEXT("planted_days"), PlantedGrowDays);
	}
	if (FruitStage != EGamerFruitStage::None)
	{
		Out.SetInt(TEXT("fruit_stage"), static_cast<int32>(FruitStage));
		Out.SetInt(TEXT("fruit_left"), FruitLeft);
		Out.SetBool(TEXT("fruit_rolled"), bFruitRolled);
		Out.SetInt(TEXT("fruit_fall_start"), FruitAtFallStart);
		Out.SetInt(TEXT("fruit_fallen"), FruitFallen);
	}
}

void AGamerResourceNode::ReadSaveState(const FGamerSaveRecord& In)
{
	// BeginPlay already ran (fresh node: full, or a season node that rolled new fruit). Replace that with the saved state.
	FTimerManager& Timers = GetWorldTimerManager();
	Timers.ClearTimer(RegrowTimer);
	Timers.ClearTimer(GrowTimer);

	bDepleted = In.GetBool(TEXT("depleted"));
	GrowStage = static_cast<uint8>(FMath::Clamp(In.GetInt(TEXT("grow_stage")), 0, 255));
	WorkProgress = FMath::Clamp(In.GetFloat(TEXT("work")), 0.f, 1.f);
	bPlantedGrowing = In.GetBool(TEXT("planted"));
	PlantedGrowDays = In.GetFloat(TEXT("planted_days"), PlantedGrowDays);
	GrowStepSeconds = In.GetFloat(TEXT("grow_step_s"), GrowStepSeconds);

	const float Regrow = In.GetFloat(TEXT("regrow_s"), -1.f);
	if (Regrow >= 0.f)
	{
		Timers.SetTimer(RegrowTimer, this, &AGamerResourceNode::Regrow, FMath::Max(0.05f, Regrow), false);
	}
	const float GrowNext = In.GetFloat(TEXT("grow_next_s"), -1.f);
	if (GrowNext >= 0.f)
	{
		Timers.SetTimer(GrowTimer, this, &AGamerResourceNode::Grow, FMath::Max(0.05f, GrowNext), false);
	}
	if (In.Has(TEXT("fruit_stage")))
	{
		FruitStage = static_cast<EGamerFruitStage>(FMath::Clamp(In.GetInt(TEXT("fruit_stage")), 0, static_cast<int32>(EGamerFruitStage::Ripe)));
		FruitLeft = FMath::Max(0, In.GetInt(TEXT("fruit_left")));
		bFruitRolled = In.GetBool(TEXT("fruit_rolled"));
		FruitAtFallStart = FMath::Max(0, In.GetInt(TEXT("fruit_fall_start")));
		FruitFallen = FMath::Max(0, In.GetInt(TEXT("fruit_fallen")));
		if (bPlantedGrowing)
		{
			Timers.ClearTimer(SeasonTimer); // the season starts when the planted tree is full grown (Regrow)
		}
	}
	ApplyLook();
	ForceNetUpdate();
}

// --- Buildings, farm buildings, tavern ---------------------------------------------------------------------------

void AGamerBuilding::WriteSaveState(FGamerSaveRecord& Out) const
{
	Out.SetName(TEXT("id"), BuildableId);
	Out.SetNumber(TEXT("progress"), Progress);
	Out.SetItems(TEXT("spent"), Spent);
	Out.SetInt(TEXT("level"), Level);
	if (StrikesDone > 0)
	{
		Out.SetInt(TEXT("strikes"), StrikesDone);
	}
}

void AGamerBuilding::ReadSaveState(const FGamerSaveRecord& In)
{
	// Id and progress are set by the save subsystem before spawning (InitBuilding).
	TArray<FGamerItemAmount> InSpent = In.GetItems(TEXT("spent"));
	GamerSave::FilterKnownItems(this, InSpent, *BuildableId.ToString());
	Spent = InSpent;
	const FGamerBuildableDef* Def = GetDef();
	Level = FMath::Clamp(In.GetInt(TEXT("level"), 1), 1, Def ? Def->MaxLevel() : 1);
	StrikesDone = FMath::Max(0, In.GetInt(TEXT("strikes")));
	ApplyLook();
	ForceNetUpdate();
}

void AGamerFarmBuilding::WriteSaveState(FGamerSaveRecord& Out) const
{
	Super::WriteSaveState(Out);
	if (!Seed.IsNone())
	{
		Out.SetName(TEXT("seed"), Seed);
		Out.SetNumber(TEXT("growth"), Growth);
	}
	Out.SetNumber(TEXT("water"), Water);
	Out.SetBool(TEXT("fertilized"), bFertilized);
	Out.SetInt(TEXT("stored"), Stored);
	Out.SetNumber(TEXT("amount"), Amount);
}

void AGamerFarmBuilding::ReadSaveState(const FGamerSaveRecord& In)
{
	Super::ReadSaveState(In);
	const FName InSeed = In.GetName(TEXT("seed"));
	if (!InSeed.IsNone() && !GamerSave::IsKnownItem(this, InSeed))
	{
		UE_LOG(LogGamer, Warning, TEXT("Save: seed \"%s\" no longer in the data, field emptied"), *InSeed.ToString());
	}
	Seed = GamerSave::IsKnownItem(this, InSeed) ? InSeed : NAME_None;
	Growth = Seed.IsNone() ? 0.f : FMath::Clamp(In.GetFloat(TEXT("growth")), 0.f, 1.f);
	Water = FMath::Clamp(In.GetFloat(TEXT("water")), 0.f, 1.f);
	bFertilized = In.GetBool(TEXT("fertilized"));
	Stored = FMath::Max(0, In.GetInt(TEXT("stored")));
	Amount = FMath::Max(0.f, In.GetFloat(TEXT("amount")));
	UpdateCrops();
	ForceNetUpdate();
}

void AGamerTavern::WriteSaveState(FGamerSaveRecord& Out) const
{
	Super::WriteSaveState(Out);
	Out.SetItems(TEXT("stock"), Stock);
}

void AGamerTavern::ReadSaveState(const FGamerSaveRecord& In)
{
	Super::ReadSaveState(In);
	Stock = In.GetItems(TEXT("stock"));
	GamerSave::FilterKnownItems(this, Stock, TEXT("tavern stock"));
	ForceNetUpdate();
}

// --- Items on the ground ------------------------------------------------------------------------------------------

void AGamerGroundItem::WriteSaveState(FGamerSaveRecord& Out) const
{
	Out.SetName(TEXT("item"), Item);
	Out.SetInt(TEXT("count"), Count);
	Out.SetNumber(TEXT("fresh"), Freshness);
	if (RotSeconds > 0.f)
	{
		Out.SetNumber(TEXT("rot_s"), RotSeconds);
	}
}

void AGamerGroundItem::ReadSaveState(const FGamerSaveRecord& In)
{
	// Item, count and freshness are passed to SpawnFallen; only the rotting time is left.
	RotSeconds = FMath::Max(0.f, In.GetFloat(TEXT("rot_s")));
	FallHeight = 0.f;
}

// --- Villagers ------------------------------------------------------------------------------------------------------

void AGamerVillager::WriteSaveState(FGamerSaveRecord& Out) const
{
	Out.SetString(TEXT("name"), VillagerName);
	Out.SetName(TEXT("job"), JobId);
	Out.SetNumber(TEXT("hunger"), Hunger);
	Out.SetNumber(TEXT("sleep"), Sleep);
	Out.SetNumber(TEXT("mood"), Mood);
	Out.SetBool(TEXT("shelter"), bShelter);
	Out.SetName(TEXT("tool"), HeldTool);
	// Everything the villager carries, including what is on the way to a field or the tavern (the task itself is
	// not saved: after loading the villager decides anew, and the goods go back to the storage).
	TArray<FGamerItemAmount> All = Carry;
	if (!FarmItem.IsNone() && FarmCount > 0)
	{
		All.Add({ FarmItem, FarmCount, 1.f });
	}
	if (!ServeItem.IsNone() && ServeCount > 0)
	{
		All.Add({ ServeItem, ServeCount, ServeFreshness });
	}
	Out.SetItems(TEXT("carry"), All);
	if (HungerEmptyDays > 0.f)
	{
		Out.SetNumber(TEXT("hunger_empty_days"), HungerEmptyDays);
	}
	if (PendingMoodGain != 0.f)
	{
		Out.SetNumber(TEXT("pending_mood"), PendingMoodGain); // fading tavern bonus on top of the mood
	}
}

void AGamerVillager::ReadSaveState(const FGamerSaveRecord& In)
{
	// Name and bed come through InitVillager. The job is taken as saved (work places were checked when it was set).
	const FName InJob = In.GetName(TEXT("job"));
	const UGamerDataSubsystem* Data = UGamerDataSubsystem::Get(this);
	if (Data && Data->FindJob(InJob))
	{
		JobId = InJob;
	}
	else if (!InJob.IsNone())
	{
		UE_LOG(LogGamer, Warning, TEXT("Save: job \"%s\" of %s no longer in the data, fallback job"), *InJob.ToString(), *VillagerName);
	}
	Hunger = FMath::Clamp(In.GetFloat(TEXT("hunger"), 1.f), 0.f, 1.f);
	Sleep = FMath::Clamp(In.GetFloat(TEXT("sleep"), 1.f), 0.f, 1.f);
	Mood = FMath::Clamp(In.GetFloat(TEXT("mood"), Mood), 0.f, 1.f);
	bShelter = In.GetBool(TEXT("shelter"));
	const FName InTool = In.GetName(TEXT("tool"));
	HeldTool = GamerSave::IsKnownItem(this, InTool) ? InTool : NAME_None;
	Carry = In.GetItems(TEXT("carry"));
	GamerSave::FilterKnownItems(this, Carry, *VillagerName);
	bDeliverNow = Carry.Num() > 0;
	HungerEmptyDays = FMath::Max(0.f, In.GetFloat(TEXT("hunger_empty_days")));
	PendingMoodGain = In.GetFloat(TEXT("pending_mood"));
	Task = ETask::None;
	ThinkTimer = 0.f;
	SetActivity(TEXT("wacht auf"));
	ApplyLook();
	ForceNetUpdate();
}

// --- Player components --------------------------------------------------------------------------------------------

void UGamerInventoryComponent::WriteSaveState(FGamerSaveRecord& Out) const
{
	TArray<FGamerSaveRecord> Records;
	for (int32 Index = 0; Index < Slots.Num(); ++Index)
	{
		const FGamerInventorySlot& Slot = Slots[Index];
		if (Slot.IsEmpty())
		{
			continue;
		}
		FGamerSaveRecord Record;
		Record.SetInt(TEXT("slot"), Index);
		Record.SetName(TEXT("item"), Slot.Item);
		Record.SetInt(TEXT("count"), Slot.Count);
		if (Slot.Freshness < 0.9999f)
		{
			Record.SetNumber(TEXT("fresh"), Slot.Freshness);
		}
		Records.Add(Record);
	}
	Out.SetRecords(TEXT("slots"), Records);
	Out.SetInt(TEXT("selected"), SelectedSlot);
}

void UGamerInventoryComponent::ReadSaveState(const FGamerSaveRecord& In)
{
	Slots.Reset();
	Slots.SetNum(FMath::Max(1, SlotCount));
	TArray<FGamerSaveRecord> Overflow;
	for (const FGamerSaveRecord& Record : In.GetRecords(TEXT("slots")))
	{
		const FName Item = Record.GetName(TEXT("item"));
		const int32 Count = Record.GetInt(TEXT("count"));
		if (Count <= 0 || !GamerSave::IsKnownItem(this, Item))
		{
			if (!Item.IsNone())
			{
				UE_LOG(LogGamer, Warning, TEXT("Save: unknown item \"%s\" x%d in an inventory dropped"), *Item.ToString(), Count);
			}
			continue;
		}
		const int32 Index = Record.GetInt(TEXT("slot"), -1);
		if (Slots.IsValidIndex(Index) && Slots[Index].IsEmpty())
		{
			Slots[Index].Item = Item;
			Slots[Index].Count = FMath::Min(Count, MaxStackOf(Item));
			Slots[Index].Freshness = FMath::Clamp(Record.GetFloat(TEXT("fresh"), 1.f), 0.f, 1.f);
			if (Count > Slots[Index].Count)
			{
				FGamerSaveRecord Rest = Record;
				Overflow.Add(Rest);
			}
		}
		else
		{
			Overflow.Add(Record); // fewer slots than before (config change): put it where it fits
		}
	}
	for (const FGamerSaveRecord& Record : Overflow)
	{
		const FName Item = Record.GetName(TEXT("item"));
		const int32 Index = Record.GetInt(TEXT("slot"), -1);
		const int32 Already = Slots.IsValidIndex(Index) && Slots[Index].Item == Item ? Slots[Index].Count : 0;
		const int32 Left = AddItem(Item, Record.GetInt(TEXT("count")) - Already, Record.GetFloat(TEXT("fresh"), 1.f));
		if (Left > 0)
		{
			UE_LOG(LogGamer, Warning, TEXT("Save: %d x %s did not fit into the inventory any more"), Left, *Item.ToString());
		}
	}
	SelectedSlot = FMath::Clamp(In.GetInt(TEXT("selected")), 0, FMath::Max(0, GetHotbarSize() - 1));
	OnInventoryChanged.Broadcast();
	if (AActor* Owner = GetOwner())
	{
		Owner->ForceNetUpdate();
	}
}

void UGamerBuffComponent::WriteSaveState(FGamerSaveRecord& Out) const
{
	TArray<FGamerSaveRecord> Records;
	const double Now = ServerNow();
	for (const FGamerActiveMeal& Meal : Meals)
	{
		FGamerSaveRecord Record;
		Record.SetName(TEXT("item"), Meal.Item);
		Record.SetNumber(TEXT("left_s"), FMath::Max(0.0, Meal.EndTime - Now));
		Records.Add(Record);
	}
	Out.SetRecords(TEXT("meals"), Records);
}

void UGamerBuffComponent::ReadSaveState(const FGamerSaveRecord& In)
{
	Meals.Reset();
	const double Now = ServerNow();
	for (const FGamerSaveRecord& Record : In.GetRecords(TEXT("meals")))
	{
		const FName Item = Record.GetName(TEXT("item"));
		const double Left = Record.GetNumber(TEXT("left_s"));
		if (Left > 0.0 && GamerSave::IsKnownItem(this, Item) && Meals.Num() < MaxMeals)
		{
			FGamerActiveMeal Meal;
			Meal.Item = Item;
			Meal.EndTime = Now + Left;
			Meals.Add(Meal);
		}
	}
	OnMealsChanged.Broadcast();
	if (AActor* Owner = GetOwner())
	{
		Owner->ForceNetUpdate();
	}
}
