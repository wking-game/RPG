#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GamerDataTypes.h"
#include "GamerGrave.generated.h"

class UStaticMeshComponent;
struct FGamerSaveRecord;

/**
 * A grave (world option "Tod" = grave, decision 2026-11-16): holds the inventory a player lost when dying and can be
 * opened with the interact key. The owner can open it at once, teammates only after a protection time (health.json
 * "grave.others_after_s"); the items go to whoever opens it, what does not fit stays in the grave. Server-authoritative;
 * the items are not replicated (only the count and the owner's name for the HUD). Saved with the world
 * (UGamerSaveSubsystem calls WriteSaveState / ReadSaveState). Placeholder look: a low mound with a stake.
 */
UCLASS()
class GAMER_API AGamerGrave : public AActor
{
	GENERATED_BODY()

public:
	AGamerGrave();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	/** Server: spawns a grave with these items. OwnerKey = the player's save id (or session id), OwnerName for the HUD. */
	static AGamerGrave* Create(UWorld* World, const FVector& Location, const FString& OwnerKey, const FString& OwnerName, const TArray<FGamerItemAmount>& Items);

	const FString& GetOwnerKey() const { return OwnerKey; }
	const FString& GetOwnerName() const { return OwnerName; }
	int32 GetItemCount() const { return ItemCount; }
	const TArray<FGamerItemAmount>& GetItems() const { return Items; }
	/** Seconds since the death (server clock; also correct on clients). */
	float GetAgeSeconds() const;

	/** Server: moves as much as fits into the inventory of the player; returns how many pieces moved. Destroys the grave when empty. */
	int32 CollectInto(class UGamerInventoryComponent* Inventory);

	void WriteSaveState(FGamerSaveRecord& Out) const;
	void ReadSaveState(const FGamerSaveRecord& In);

protected:
	UPROPERTY(VisibleAnywhere, Category = "Visual")
	TObjectPtr<UStaticMeshComponent> Mound;

	UPROPERTY(VisibleAnywhere, Category = "Visual")
	TObjectPtr<UStaticMeshComponent> Stake;

	UPROPERTY(Replicated)
	FString OwnerName;

	UPROPERTY(Replicated)
	int32 ItemCount = 0;

private:
	void RefreshCount();

	/** Who the grave belongs to (replicated so the HUD can tell who may open it). */
	UPROPERTY(Replicated)
	FString OwnerKey;

	/** Server only. */
	TArray<FGamerItemAmount> Items;
	/** Server clock (GameState) when the player died; replicated so every machine can show the protection time. */
	UPROPERTY(Replicated)
	double DeathTime = 0.0;
	double ServerNow() const;
};
