#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GamerDataTypes.h"
#include "GamerSettlement.generated.h"

class AGamerBuilding;
class AGamerVillager;

/**
 * The settlement of a world (one for the vertical slice). Lives on the server, replicates to everyone.
 * - One storage per settlement: every finished storage building ("Lager") is an access point to the same
 *   contents; capacity grows with the number of storage buildings.
 * - Villagers: every free bed draws a new villager after a while (proposal 2026-11-13); beds are assigned.
 * - Time: villager needs run in game days (DayLengthSeconds, 30 min); for tests faster with the cvar
 *   gamer.VillagerTimeScale or -GamerVillagerTimeScale=N on the server command line.
 * The villagers' own decisions are in AGamerVillager (server only, needs instead of a clock).
 */
UCLASS(Config = Game)
class GAMER_API AGamerSettlement : public AActor
{
	GENERATED_BODY()

public:
	AGamerSettlement();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	/** The settlement of the world WorldContext lives in (null before it is spawned/replicated). */
	static AGamerSettlement* Get(const UObject* WorldContext);

	// --- Storage (queries everywhere, changes on the server) ---

	const TArray<FGamerItemAmount>& GetStorage() const { return Storage; }
	int32 CountInStorage(FName Item) const;
	bool HasInStorage(const TArray<FGamerItemAmount>& Amounts) const;
	int32 GetStoredTotal() const;
	int32 GetCapacity() const { return Capacity; }
	AGamerBuilding* FindNearestStorageBuilding(const FVector& From) const;

	/** Server: returns how many did not fit. */
	int32 AddToStorage(FName Item, int32 Count);
	/** Server: returns how many were taken (0..Count). */
	int32 TakeFromStorage(FName Item, int32 Count);
	/** Server: all or nothing. */
	bool TakeAllFromStorage(const TArray<FGamerItemAmount>& Amounts);
	/** Server: first item of this tool type (best tier) in the storage, or NAME_None. */
	FName FindToolInStorage(FName ToolType) const;

	// --- Villagers ---

	/** Real seconds to game days, including the test time scale. */
	float ToGameDays(float RealSeconds) const;
	float GetDayLengthSeconds() const { return DayLengthSeconds; }

	int32 GetFreeBeds() const { return FreeBeds; }
	/** 0..1 towards the next villager (while a bed is free). */
	float GetArrivalProgress() const { return ArrivalProgress; }

	/** Server: new villager near Location (arrival or tests). Gets a free bed if there is one. */
	AGamerVillager* SpawnVillager(const FVector& Location);

	/** Server: work target reservations, so two villagers do not fell the same tree. */
	bool IsReservedByOther(const AActor* Target, const AGamerVillager* Villager) const;
	void Reserve(AActor* Target, AGamerVillager* Villager);
	void ReleaseAll(AGamerVillager* Villager);

	/** Server: tell every player (top of the screen, like other messages). */
	void Broadcast(const FString& Text);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastNews(const FString& Text);

	/** One-line summary for logs and tests. */
	FString Describe() const;

protected:
	UPROPERTY(Replicated)
	TArray<FGamerItemAmount> Storage;

	UPROPERTY(Replicated)
	int32 Capacity = 0;

	UPROPERTY(Replicated)
	int32 FreeBeds = 0;

	UPROPERTY(Replicated)
	float ArrivalProgress = 0.f;

	/** Items per finished storage building. */
	UPROPERTY(Config, EditDefaultsOnly, Category = "Settlement")
	int32 CapacityPerStorage = 500;

	/** Game days until a free bed draws a villager (0.1 day = 3 min). */
	UPROPERTY(Config, EditDefaultsOnly, Category = "Settlement")
	float ArrivalGameDays = 0.1f;

	/** Length of a game day in seconds (as for regrowing resources). */
	UPROPERTY(Config, EditDefaultsOnly, Category = "Settlement")
	float DayLengthSeconds = 1800.f;

private:
	void UpdateBuildings();
	FString NextName();

	TMap<TWeakObjectPtr<AActor>, TWeakObjectPtr<AGamerVillager>> Reservations;
	float BuildingCheckTimer = 0.f;
	int32 NameCounter = 0;
};
