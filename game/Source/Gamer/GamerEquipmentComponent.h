#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GamerEquipmentMath.h"
#include "GamerSaveable.h"
#include "GamerEquipmentComponent.generated.h"

/**
 * What a player wears (docs/25, decision 2026-11-22): 9 places - helmet, shoulder, chest, hands, legs, feet, cloak and two rings.
 * Weapons and shields stay in the hotbar. Only the server changes it (RPCs from the owning client); the list is replicated to
 * everybody (later other players see the gear on the body). Saved per player (key "equipment"). The values of the worn pieces
 * are added up in `GetTotals()` (pure math in GamerEquipmentMath) and used by UGamerHealthComponent for hits and hazards.
 * With the world option "Tod" (grave/hardcore) the worn pieces go with the inventory.
 */
UCLASS(ClassGroup = (Gamer), meta = (BlueprintSpawnableComponent))
class GAMER_API UGamerEquipmentComponent : public UActorComponent, public IGamerSaveable
{
	GENERATED_BODY()

public:
	UGamerEquipmentComponent();

	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// IGamerSaveable
	virtual FString GetSaveKey() const override { return TEXT("equipment"); }
	virtual void WriteSaveState(FGamerSaveRecord& Out) const override;
	virtual void ReadSaveState(const FGamerSaveRecord& In) override;

	/** Number of places: helmet, shoulder, chest, hands, legs, feet, cloak, ring, ring. */
	static constexpr int32 PlaceCount = 9;
	/** Slot name of a place ("helmet" ... "cloak", "ring" for both ring places). */
	static FName SlotOfPlace(int32 Place);
	/** Places a slot can use (one, or two for rings). */
	static TArray<int32> PlacesOfSlot(FName Slot);

	/** Item worn at a place, or NAME_None. */
	FName GetWornAt(int32 Place) const { return Worn.IsValidIndex(Place) ? Worn[Place] : NAME_None; }
	const FGamerItemDef* GetWornDef(int32 Place) const;
	/** Sum of all worn pieces (cached, rebuilt on every change). */
	const FGamerEquipmentTotals& GetTotals() const { return Totals; }

	// --- Server ---

	/** Puts the item from the inventory on (a worn piece in the same place goes back into the inventory). False and a reason if it does not work. */
	bool EquipFromInventory(FName Item, int32 PreferredPlace, FString& OutWhy);
	/** Takes the piece off into the inventory. */
	bool Unequip(int32 Place, FString& OutWhy);
	/** Best piece in the inventory for this place (highest tier, then armor), or NAME_None. Local, works on the owning client. */
	FName FindCandidateFor(int32 Place) const;
	/** Takes everything off and returns it (death with a lost or grave inventory). */
	TArray<FGamerItemAmount> TakeEverything();

	/** Owning client: put on / take off. The server answers with a message when it does not work. */
	UFUNCTION(Server, Reliable)
	void ServerEquip(FName Item, int32 PreferredPlace);
	UFUNCTION(Server, Reliable)
	void ServerUnequip(int32 Place);
	UFUNCTION(Client, Reliable)
	void ClientNotice(const FString& Text);

	FString Describe() const;

	/** Fires on the server after a change and on clients after replication. */
	DECLARE_MULTICAST_DELEGATE(FOnEquipmentChanged);
	FOnEquipmentChanged OnEquipmentChanged;

	/** Last message of the server for the owner (HUD shows it for a few seconds). */
	const FString& GetLastNotice() const { return LastNotice; }
	double GetLastNoticeTime() const { return LastNoticeTime; }

private:
	UFUNCTION()
	void OnRep_Worn();
	void Rebuild();
	bool CanWork() const;

	UPROPERTY(ReplicatedUsing = OnRep_Worn)
	TArray<FName> Worn;

	FGamerEquipmentTotals Totals;
	FString LastNotice;
	double LastNoticeTime = -100.0;
};
