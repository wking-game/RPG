#pragma once

#include "CoreMinimal.h"
#include "GamerBuilding.h"
#include "GamerSaveable.h"
#include "GamerTavern.generated.h"

/**
 * The tavern (decision 2026-11-14/15, docs/12_taverne_gerichte.md): work places for cook and innkeeper,
 * upgrade levels unlock cooking places around it (buildables.json "requires"). It keeps a small stock of
 * dishes and drinks the innkeeper brings from the storage; villagers eat there (mood bonus) when it has food.
 * Server changes the stock; it replicates to everyone (shown in tests and later in the station window).
 */
UCLASS()
class GAMER_API AGamerTavern : public AGamerBuilding
{
	GENERATED_BODY()

public:
	// IGamerSaveable (save system, implemented in GamerSaveImpl.cpp)
	virtual void WriteSaveState(FGamerSaveRecord& Out) const override;
	virtual void ReadSaveState(const FGamerSaveRecord& In) override;

public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	const TArray<FGamerItemAmount>& GetStock() const { return Stock; }
	int32 CountStock(FName Item) const;
	int32 TotalStock() const;
	/** Pieces that still fit (data: tavern.stock). */
	int32 FreeSpace() const;

	/** Server: put pieces in (as many as fit), returns how many were taken. */
	int32 AddStock(FName Item, int32 Count, float Freshness = 1.f);
	/** Server: take up to Count pieces out, returns how many. */
	int32 TakeStock(FName Item, int32 Count);

	/** "item_meal x3, item_drink_beer x2" for logs and tests. */
	FString DescribeStock() const;

	/**
	 * Placement rule "requires": empty if Def may stand at Location, else the reason
	 * ("Braucht eine Taverne (Stufe 2) im Umkreis von 15 m"). Used by the preview and the server.
	 */
	static FString CheckRequirement(const UWorld* World, const FGamerBuildableDef& Def, const FVector& Location);

	/** Nearest finished building of that id with at least Level within Radius of Location, or null. */
	static AGamerBuilding* FindNear(const UWorld* World, FName BuildableId, int32 Level, const FVector& Location, float Radius);

protected:
	UPROPERTY(Replicated)
	TArray<FGamerItemAmount> Stock;
};
