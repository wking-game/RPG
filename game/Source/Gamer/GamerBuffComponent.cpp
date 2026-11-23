#include "GamerBuffComponent.h"

#include "Gamer.h"
#include "GamerCharacter.h"
#include "GamerDataSubsystem.h"
#include "GamerInventoryComponent.h"
#include "GamerWorkComponent.h"
#include "GameFramework/GameStateBase.h"
#include "Misc/CommandLine.h"
#include "Misc/Parse.h"
#include "Net/UnrealNetwork.h"
#include "TimerManager.h"

UGamerBuffComponent::UGamerBuffComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
}

void UGamerBuffComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION(UGamerBuffComponent, Meals, COND_OwnerOnly);
}

void UGamerBuffComponent::BeginPlay()
{
	Super::BeginPlay();
#if !UE_BUILD_SHIPPING
	float Seconds = 0.f;
	if (FParse::Value(FCommandLine::Get(), TEXT("GamerBuffMinuteSeconds="), Seconds) && Seconds > 0.f)
	{
		MinuteSeconds = Seconds; // tests: let buffs run out quickly (same value on server and clients)
	}
#endif
	if (GetOwner() && GetOwner()->HasAuthority())
	{
		GetWorld()->GetTimerManager().SetTimer(ExpiryTimer, this, &UGamerBuffComponent::TickExpiry, 1.f, true);
	}
}

double UGamerBuffComponent::ServerNow() const
{
	const AGameStateBase* GameState = GetWorld() ? GetWorld()->GetGameState() : nullptr;
	return GameState ? GameState->GetServerWorldTimeSeconds() : (GetWorld() ? GetWorld()->GetTimeSeconds() : 0.0);
}

FString UGamerBuffComponent::DescribeEffect(const FGamerBuffEffect& Effect)
{
	const FString Value = FString::SanitizeFloat(FMath::RoundToFloat(Effect.Value * 100.f) / 100.f, 0).Replace(TEXT("."), TEXT(","));
	switch (Effect.Stat)
	{
	case EGamerBuffStat::Health:       return FString::Printf(TEXT("+%s Gesundheit"), *Value);
	case EGamerBuffStat::Regen:        return FString::Printf(TEXT("+%s Regeneration/s"), *Value);
	case EGamerBuffStat::Carry:        return FString::Printf(TEXT("+%s kg Traglast"), *Value);
	case EGamerBuffStat::ColdResist:   return FString::Printf(TEXT("+%s %% Kälteresistenz"), *Value);
	case EGamerBuffStat::PoisonResist: return FString::Printf(TEXT("+%s %% Giftresistenz"), *Value);
	case EGamerBuffStat::Damage:       return FString::Printf(TEXT("+%s %% Schaden"), *Value);
	case EGamerBuffStat::StaminaMax:   return FString::Printf(TEXT("+%s Ausdauer"), *Value);
	case EGamerBuffStat::StaminaRegen: return FString::Printf(TEXT("+%s Ausdauer-Erholung/s"), *Value);
	}
	return Value;
}

void UGamerBuffComponent::ServerEat_Implementation(FName Item)
{
	const UGamerDataSubsystem* Data = UGamerDataSubsystem::Get(this);
	const FGamerItemDef* Def = Data ? Data->FindItem(Item) : nullptr;
	const AGamerCharacter* Character = Cast<AGamerCharacter>(GetOwner());
	UGamerInventoryComponent* Inventory = Character ? Character->GetInventory() : nullptr;
	UGamerWorkComponent* Work = Character ? Character->GetWork() : nullptr;
	if (!Def || !Inventory)
	{
		return;
	}
	auto Tell = [Work](const FString& Text)
	{
		if (Work)
		{
			Work->ClientMessage(Text);
		}
	};
	if (!Def->Food.IsMeal())
	{
		return Tell(FString::Printf(TEXT("%s kann man nicht als Mahlzeit essen."), *Def->Name));
	}
	// Raw food (snack) only takes a free place or the place of another snack; it never pushes out a cooked meal.
	const bool bAlreadyActive = Meals.ContainsByPredicate([Item](const FGamerActiveMeal& M) { return M.Item == Item; });
	int32 SnackToReplace = INDEX_NONE;
	if (!bAlreadyActive && Def->Food.bSnack && Meals.Num() >= FMath::Max(1, MaxMeals))
	{
		for (int32 Index = 0; Index < Meals.Num(); ++Index)
		{
			const FGamerItemDef* Other = Data->FindItem(Meals[Index].Item);
			if (Other && Other->Food.bSnack && (SnackToReplace == INDEX_NONE || Meals[Index].EndTime < Meals[SnackToReplace].EndTime))
			{
				SnackToReplace = Index;
			}
		}
		if (SnackToReplace == INDEX_NONE)
		{
			return Tell(FString::Printf(TEXT("Du bist satt: %s würde keine deiner %d gekochten Mahlzeiten verdrängen."), *Def->Name, Meals.Num()));
		}
	}
	FGamerItemAmount One;
	One.Item = Item;
	One.Count = 1;
	if (Inventory->CountOf(Item) < 1 || !Inventory->RemoveItems({ One }))
	{
		return Tell(FString::Printf(TEXT("Du hast kein %s."), *Def->Name));
	}
	if (AGamerCharacter* Eater = Cast<AGamerCharacter>(GetOwner()))
	{
		Eater->MulticastAction(EGamerAnimAction::Consume, true);
	}

	const double End = ServerNow() + Def->Food.Minutes * MinuteSeconds;
	FString Replaced;
	if (FGamerActiveMeal* Same = Meals.FindByPredicate([Item](const FGamerActiveMeal& M) { return M.Item == Item; }))
	{
		Same->EndTime = End; // same dish again: only the time is refreshed, nothing stacks
	}
	else
	{
		if (Meals.Num() >= FMath::Max(1, MaxMeals))
		{
			int32 Shortest = 0;
			for (int32 Index = 1; Index < Meals.Num(); ++Index)
			{
				Shortest = Meals[Index].EndTime < Meals[Shortest].EndTime ? Index : Shortest;
			}
			if (SnackToReplace != INDEX_NONE)
			{
				Shortest = SnackToReplace;
			}
			const FGamerItemDef* Old = Data->FindItem(Meals[Shortest].Item);
			Replaced = Old ? Old->Name : Meals[Shortest].Item.ToString();
			Meals.RemoveAt(Shortest);
		}
		FGamerActiveMeal& Meal = Meals.AddDefaulted_GetRef();
		Meal.Item = Item;
		Meal.EndTime = End;
	}

	TArray<FString> Effects;
	for (const FGamerBuffEffect& Effect : Def->Food.Buffs)
	{
		Effects.Add(DescribeEffect(Effect));
	}
	UE_LOG(LogGamer, Log, TEXT("Meal: %s ate %s (%s, %.0f min)%s - active: %s"), *GetOwner()->GetName(), *Item.ToString(),
		*FString::Join(Effects, TEXT(", ")), Def->Food.Minutes, Replaced.IsEmpty() ? TEXT("") : *(TEXT(", replaced ") + Replaced), *Describe());
	Tell(FString::Printf(TEXT("%s gegessen: %s für %.0f min%s"), *Def->Name, *FString::Join(Effects, TEXT(", ")), Def->Food.Minutes,
		Replaced.IsEmpty() ? TEXT("") : *FString::Printf(TEXT(" (ersetzt %s)"), *Replaced)));
	GetOwner()->ForceNetUpdate();
	OnMealsChanged.Broadcast();
}

void UGamerBuffComponent::TickExpiry()
{
	const double Now = ServerNow();
	const int32 Removed = Meals.RemoveAll([Now](const FGamerActiveMeal& M) { return M.EndTime <= Now; });
	if (Removed > 0)
	{
		UE_LOG(LogGamer, Log, TEXT("Meal: %s - %d meal buff(s) ran out, active: %s"), *GetOwner()->GetName(), Removed, *Describe());
		GetOwner()->ForceNetUpdate();
		OnMealsChanged.Broadcast();
	}
}

void UGamerBuffComponent::OnRep_Meals()
{
	OnMealsChanged.Broadcast();
}

float UGamerBuffComponent::GetRemainingSeconds(int32 Index) const
{
	return Meals.IsValidIndex(Index) ? FMath::Max(0.f, static_cast<float>(Meals[Index].EndTime - ServerNow())) : 0.f;
}

float UGamerBuffComponent::GetBonus(EGamerBuffStat Stat) const
{
	const UGamerDataSubsystem* Data = UGamerDataSubsystem::Get(this);
	float Sum = 0.f;
	for (const FGamerActiveMeal& Meal : Meals)
	{
		if (const FGamerItemDef* Def = Data ? Data->FindItem(Meal.Item) : nullptr)
		{
			for (const FGamerBuffEffect& Effect : Def->Food.Buffs)
			{
				Sum += Effect.Stat == Stat ? Effect.Value : 0.f;
			}
		}
	}
	return Sum;
}

FString UGamerBuffComponent::Describe() const
{
	TArray<FString> Parts;
	for (int32 Index = 0; Index < Meals.Num(); ++Index)
	{
		const int32 Seconds = FMath::CeilToInt(GetRemainingSeconds(Index));
		Parts.Add(FString::Printf(TEXT("%s %d:%02d"), *Meals[Index].Item.ToString(), Seconds / 60, Seconds % 60));
	}
	return Parts.Num() > 0 ? FString::Join(Parts, TEXT(", ")) : TEXT("none");
}

bool UGamerBuffComponent::NeedsCooking(const UObject* WorldContext, const FGamerItemDef& Item)
{
	const UGamerDataSubsystem* Data = UGamerDataSubsystem::Get(WorldContext);
	if (!Data || Item.Category != EGamerItemCategory::Food || Item.Food.IsMeal())
	{
		return false;
	}
	for (const TPair<FName, FGamerRecipeDef>& Pair : Data->GetData().Recipes)
	{
		if (!Pair.Value.Station.IsNone() && Pair.Value.Inputs.ContainsByPredicate([&Item](const FGamerItemAmount& In) { return In.Item == Item.Id; }))
		{
			return true;
		}
	}
	return false;
}

FString UGamerBuffComponent::DescribeFood(const FGamerItemDef& Item)
{
	if (!Item.Food.IsMeal())
	{
		return FString();
	}
	TArray<FString> Effects;
	for (const FGamerBuffEffect& Effect : Item.Food.Buffs)
	{
		Effects.Add(DescribeEffect(Effect));
	}
	return FString::Printf(TEXT("%s, %.0f min"), *FString::Join(Effects, TEXT(", ")), Item.Food.Minutes);
}
