#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GamerDataTypes.h"
#include "GamerInventoryComponent.generated.h"

struct FGamerItemDef;

USTRUCT(BlueprintType)
struct FGamerInventorySlot
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Inventory")
	FName Item;

	UPROPERTY(BlueprintReadOnly, Category = "Inventory")
	int32 Count = 0;

	bool IsEmpty() const { return Item.IsNone() || Count <= 0; }
};

/**
 * Player inventory. Only the server changes it; the owning client gets a replicated copy
 * (other players never see it). Stack sizes and weights come from the data tables.
 * Not yet: tool durability per item, carry-weight limit (later steps of Phase 1/2).
 */
UCLASS(ClassGroup = (Gamer), meta = (BlueprintSpawnableComponent), Config = Game)
class GAMER_API UGamerInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UGamerInventoryComponent();

	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// --- Server only (changes) ---

	/** Adds as much as fits. Returns the amount that did not fit. */
	int32 AddItem(FName Item, int32 Count);

	/** Removes all listed amounts, or nothing if one of them is missing. */
	bool RemoveItems(const TArray<FGamerItemAmount>& Amounts);

	// --- Queries (server and owning client) ---

	int32 CountOf(FName Item) const;
	bool HasAll(const TArray<FGamerItemAmount>& Amounts) const;

	/** How many of Item would still fit. */
	int32 SpaceFor(FName Item) const;

	/** Best tool of the given type with at least MinTier, or nullptr. */
	const FGamerItemDef* FindBestTool(FName ToolType, int32 MinTier) const;

	float GetTotalWeightKg() const;
	const TArray<FGamerInventorySlot>& GetSlots() const { return Slots; }

	/** Short text for logs and tests, e.g. "item_stone x3, tool_axe_t1 x1". */
	FString Describe() const;

	/** Fires on the server after a change and on the owning client after replication. */
	DECLARE_MULTICAST_DELEGATE(FOnInventoryChanged);
	FOnInventoryChanged OnInventoryChanged;

protected:
	/** Number of slots. From [/Script/Gamer.GamerInventoryComponent] in DefaultGame.ini; fallback here. */
	UPROPERTY(Config, EditDefaultsOnly, Category = "Inventory")
	int32 SlotCount = 24;

	UPROPERTY(ReplicatedUsing = OnRep_Slots)
	TArray<FGamerInventorySlot> Slots;

	UFUNCTION()
	void OnRep_Slots();

private:
	int32 MaxStackOf(FName Item) const;
	const FGamerItemDef* FindDef(FName Item) const;
};
