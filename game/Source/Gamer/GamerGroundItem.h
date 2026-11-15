#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GamerSaveable.h"
#include "GamerGroundItem.generated.h"

class UStaticMeshComponent;
struct FGamerItemDef;

/**
 * An item lying on the ground (fallen fruit, decision 2026-11-14). The server spawns it, lets it spoil
 * (faster than carried) and removes it when picked up; spoiled fruit turns into spoiled food and rots away later.
 * Picked up with the interact key. Model: the item's "world_mesh", else a small placeholder ball.
 * Clients play a short fall from the replicated height, so the drop is visible without moving the actor.
 */
UCLASS()
class GAMER_API AGamerGroundItem : public AActor, public IGamerSaveable
{
	GENERATED_BODY()

public:
	// IGamerSaveable (save system, implemented in GamerSaveImpl.cpp)
	virtual void WriteSaveState(FGamerSaveRecord& Out) const override;
	virtual void ReadSaveState(const FGamerSaveRecord& In) override;

public:
	AGamerGroundItem();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	/** Server: spawn an item at Ground that fell from FallHeight cm. */
	static AGamerGroundItem* SpawnFallen(UWorld* World, FName Item, int32 Count, float Freshness, const FVector& Ground, float FallHeight);

	FName GetItem() const { return Item; }
	int32 GetCount() const { return Count; }
	float GetFreshness() const { return HasAuthority() ? Freshness : FreshPercent / 100.f; }
	const FGamerItemDef* GetItemDef() const;

	/** "Fallobst mit Druckstellen (60 %)". */
	FString GetDisplayName() const;

	/** Server: hand everything to the taker; destroys the actor. Returns false if it was already taken. */
	bool TakeAll(FName& OutItem, int32& OutCount, float& OutFreshness);

protected:
	UPROPERTY(VisibleAnywhere, Category = "Visual")
	TObjectPtr<USceneComponent> Root;

	UPROPERTY(VisibleAnywhere, Category = "Visual")
	TObjectPtr<UStaticMeshComponent> Mesh;

	UPROPERTY(ReplicatedUsing = OnRep_Item)
	FName Item;

	UPROPERTY(Replicated)
	int32 Count = 1;

	/** Server value; clients see FreshPercent (changes rarely, so little traffic). */
	float Freshness = 1.f;

	UPROPERTY(Replicated)
	uint8 FreshPercent = 100;

	UPROPERTY(Replicated)
	float FallHeight = 0.f;

	/** Seconds a spoiled item stays on the ground before it rots away (half a game day). */
	UPROPERTY(EditDefaultsOnly, Category = "Ground item")
	float RotAwaySeconds = 900.f;

	UFUNCTION()
	void OnRep_Item();

private:
	void ApplyLook();
	/** Server: spoil, turn into spoiled food, rot away. */
	void TickSpoilage();

	FTimerHandle SpoilTimer;
	float RotSeconds = 0.f;
	bool bTaken = false;
	/** Client fall animation: seconds since BeginPlay. */
	float FallTime = 0.f;
	FName AppliedItem;
};
