#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GamerDataTypes.h"
#include "GamerSaveable.h"
#include "GamerBuffComponent.generated.h"

/** One meal the player has eaten and whose buffs are active. The buff values come from the item's "food" block. */
USTRUCT(BlueprintType)
struct FGamerActiveMeal
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Buffs")
	FName Item;

	/** Server world time (GameState->GetServerWorldTimeSeconds) when the buffs end. */
	UPROPERTY(BlueprintReadOnly, Category = "Buffs")
	double EndTime = 0.0;
};

/**
 * Meal buffs of a player (decision 2026-11-14, architecture rule 5: food is a buff system, never a stamina lock;
 * no food = no buffs, no penalty). Server-authoritative: the client asks to eat (ServerEat), the server checks
 * the inventory, takes one piece and sets the buff; the list replicates to the owning player only.
 * At most MaxMeals at once; the same dish again only refreshes its time; a new dish when full replaces the
 * one with the least time left. Health, regeneration, damage, carry weight and cold do not exist yet:
 * other systems read GetBonus() once they do.
 */
UCLASS(ClassGroup = (Gamer), Config = Game)
class GAMER_API UGamerBuffComponent : public UActorComponent, public IGamerSaveable
{
	GENERATED_BODY()

public:
	// IGamerSaveable (save system, implemented in GamerSaveImpl.cpp)
	virtual FString GetSaveKey() const override { return TEXT("buffs"); }
	virtual void WriteSaveState(FGamerSaveRecord& Out) const override;
	virtual void ReadSaveState(const FGamerSaveRecord& In) override;

public:
	UGamerBuffComponent();

	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	/** Eat one piece of a dish or drink from the inventory (needs a "food" block in items.json). */
	UFUNCTION(Server, Reliable)
	void ServerEat(FName Item);

	/** Active meals (owner and server). "Satt" in the HUD = at least one. */
	const TArray<FGamerActiveMeal>& GetActiveMeals() const { return Meals; }

	/** Seconds left for an active meal (0 when over). Works on the owning client (server time). */
	float GetRemainingSeconds(int32 Index) const;

	/** Sum of all active buffs of one kind (points, kg or percent, see EGamerBuffStat). */
	float GetBonus(EGamerBuffStat Stat) const;

	int32 GetMaxMeals() const { return MaxMeals; }

	/** Seconds per buff minute (60; tests shorten it with -GamerBuffMinuteSeconds=N, not in Shipping). */
	float GetMinuteSeconds() const { return MinuteSeconds; }

	/** Food that cannot be eaten as it is but goes into a cooking recipe (raw elderberries, potatoes, grain). */
	static bool NeedsCooking(const UObject* WorldContext, const FGamerItemDef& Item);

	/** One line for the hand hint: "+3 Gesundheit, 3 min" (empty if not edible). */
	static FString DescribeFood(const FGamerItemDef& Item);

	/** Text for a stat value ("+10 Gesundheit", "+15 % Kälteresistenz"). */
	static FString DescribeEffect(const FGamerBuffEffect& Effect);

	/** "item_meal 9:58, ..." for logs and tests. */
	FString Describe() const;

	/** Fires on the server after a change and on the owning client after replication. */
	DECLARE_MULTICAST_DELEGATE(FOnMealsChanged);
	FOnMealsChanged OnMealsChanged;

protected:
	/** Meals active at the same time (proposal 2026-11-14: 3). */
	UPROPERTY(Config, EditDefaultsOnly, Category = "Buffs")
	int32 MaxMeals = 3;

	UPROPERTY(Config, EditDefaultsOnly, Category = "Buffs")
	float MinuteSeconds = 60.f;

	UPROPERTY(ReplicatedUsing = OnRep_Meals)
	TArray<FGamerActiveMeal> Meals;

	UFUNCTION()
	void OnRep_Meals();

private:
	/** Server: drop meals whose time is over. */
	void TickExpiry();
	double ServerNow() const;

	FTimerHandle ExpiryTimer;
};
