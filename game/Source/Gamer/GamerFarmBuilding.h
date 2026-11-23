#pragma once

#include "CoreMinimal.h"
#include "GamerBuilding.h"
#include "GamerSaveable.h"
#include "GamerFarmBuilding.generated.h"

class AGamerCharacter;
class UGamerInventoryComponent;
class UInstancedStaticMeshComponent;
struct FGamerFarmProps;

/** What a field needs next (for players' hints and the farmer job). */
enum class EGamerFieldNeed : uint8
{
	None,
	Harvest,
	Sow,
	Water,
	Fertilize
};

/**
 * Farm building (buildables.json category "farm", decision 2026-11-14 "Alles jetzt"):
 * - Field (bed): drawn by the player as a rectangle (actor scale X/Y = size in metres, see GetSizeCm).
 *   Its size counts in "units" (farm.unit_m2): seeds, water, fertilizer and harvest are per unit.
 *   Sow a seed, water it, fertilize it once; it grows while watered and the season allows, then harvest.
 *   Never dies (no time pressure, rule 6): without water it just stops growing.
 * - Composter: spoiled food goes in, fertilizer comes out over time.
 * - Water (primitive rain catcher): fills up over time, gives water for drinking and watering.
 * Everything runs on the server; the state replicates to everyone. Players: one interact key does the next useful
 * thing (Use). Villagers with the farmer job use the same actions (Sow, AddWater, Fertilize, HarvestCrop, TakeOutput).
 * Tests: gamer.FarmTimeScale / -GamerFarmTimeScale=N on the server speeds up growing, composting and filling.
 */
UCLASS()
class GAMER_API AGamerFarmBuilding : public AGamerBuilding
{
	GENERATED_BODY()

public:
	// IGamerSaveable (save system, implemented in GamerSaveImpl.cpp)
	virtual void WriteSaveState(FGamerSaveRecord& Out) const override;
	virtual void ReadSaveState(const FGamerSaveRecord& In) override;

public:
	AGamerFarmBuilding();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void BeginPlay() override;

	/** Server: the interact key on this building. Returns the feedback for the player. */
	FString Use(AGamerCharacter* User);

	/** What the interact key would do now ("säen", "ernten", "Wasser nehmen"), for the hint under the crosshair. */
	FString GetUseHint(const UGamerInventoryComponent* Inventory) const;

	/** Current state in one line ("Möhren: 40 %, Wasser 20 %, gedüngt"). */
	FString GetStatusText() const;

	const FGamerFarmProps* GetFarm() const;
	EGamerFarmKind GetKind() const;

	// --- Field size ---

	/** Side lengths of a field in cm (from the actor scale: 1 = 100 cm). */
	FVector2D GetSizeCm() const;
	/** Seeds, water, fertilizer and harvest count per unit (area / unit_m2, at least 1). */
	int32 GetUnits() const;

	// --- Actions (server), shared by players and the farmer job ---

	/** Field: what should happen next. */
	EGamerFieldNeed GetFieldNeed() const;
	/** Field: water items needed to fill it up. */
	int32 WaterItemsNeeded() const;
	/** Field: start growing Seed (the caller has taken GetUnits() seeds). */
	bool Sow(FName SeedItem);
	/** Field: Count water items poured in. */
	void AddWater(int32 Count);
	/** Field: fertilized (the caller has taken GetUnits() fertilizer). */
	bool Fertilize();
	/** Field: harvest a ripe field (empty afterwards). */
	TArray<FGamerItemAmount> HarvestCrop();
	/** Composter / rain catcher: finished output pieces. */
	int32 GetReadyOutput() const;
	/** Composter / rain catcher: take up to Max finished pieces. */
	int32 TakeOutput(int32 Max);
	/** Composter: room for more input. */
	int32 GetInputSpace() const;
	void PutInput(int32 Count);

	// State (read-only outside, for HUD and tests)
	FName GetSeed() const { return Seed; }
	float GetGrowth() const { return Growth; }
	float GetWater() const { return Water; }
	bool IsFertilized() const { return bFertilized; }
	bool IsRipe() const { return !Seed.IsNone() && Growth >= 1.f; }
	int32 GetStored() const { return Stored; }
	float GetAmount() const { return Amount; }

	/** Server: who is working on it (farmer reservations). */
	TWeakObjectPtr<AActor> Worker;

protected:
	/** Field: what grows there (seed item id), None = empty. */
	UPROPERTY(ReplicatedUsing = OnRep_Farm)
	FName Seed;

	/** Field: 0 = just sown .. 1 = ripe. */
	UPROPERTY(ReplicatedUsing = OnRep_Farm)
	float Growth = 0.f;

	/** Field: 0 = dry .. 1 = fully watered. */
	UPROPERTY(Replicated)
	float Water = 0.f;

	UPROPERTY(Replicated)
	bool bFertilized = false;

	/** Composter: pieces put in. */
	UPROPERTY(Replicated)
	int32 Stored = 0;

	/** Composter: progress to the next output piece (0..1) plus finished pieces (whole part).
	 *  Water: water pieces collected (fractional). */
	UPROPERTY(Replicated)
	float Amount = 0.f;

	UPROPERTY(VisibleAnywhere, Category = "Visual")
	TObjectPtr<UInstancedStaticMeshComponent> Crops;

	/** Field: the 1 m soil tile from the data, laid out over the drawn size (instead of stretching one mesh). */
	UPROPERTY(VisibleAnywhere, Category = "Visual")
	TObjectPtr<UInstancedStaticMeshComponent> SoilTiles;

	UFUNCTION()
	void OnRep_Farm();

private:
	void TickFarm();
	/** Tree kind: spawn the growing node in place of this piece and remove the piece. */
	void PlantTree();
	void UpdateCrops();
	/** Field, machines that render: soil tiles once the field is finished (the stretched mesh keeps the collision). */
	void UpdateSoil();
	FTimerHandle SoilTimer;
	bool bSoilShown = false;
	float ToGameDays(float Seconds) const;
	/** Gives Count of Item to the user; what does not fit is dropped next to the building. Returns the given count. */
	int32 Give(AGamerCharacter* User, FName Item, int32 Count) const;

	FString UseField(AGamerCharacter* User, const FGamerFarmProps& Farm);
	FString UseComposter(AGamerCharacter* User, const FGamerFarmProps& Farm);
	FString UseWater(AGamerCharacter* User, const FGamerFarmProps& Farm);

	FTimerHandle FarmTimer;
	int32 AppliedStage = -2;
	FName AppliedSeed;
	FVector AppliedScale = FVector::ZeroVector;
};
