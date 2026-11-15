#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "GamerDataTypes.h"
#include "GamerSaveable.generated.h"

class FJsonObject;

/**
 * One block of saved state (save system, docs/01_entscheidungen.md "Speichersystem").
 * A thin wrapper around a JSON object with typed helpers. Reading never fails: a missing or wrongly typed field
 * returns the given default, so old saves load into new code (new fields get their defaults) and new saves
 * load into old code (unknown fields are ignored). Item ids are kept as text; the loader decides what to do with
 * ids the data tables no longer know (dropped with a warning, see UGamerSaveSubsystem).
 */
struct GAMER_API FGamerSaveRecord
{
	FGamerSaveRecord();
	explicit FGamerSaveRecord(TSharedPtr<FJsonObject> InJson);

	TSharedPtr<FJsonObject> Json;

	bool Has(const FString& Key) const;

	void SetNumber(const FString& Key, double Value);
	void SetInt(const FString& Key, int32 Value) { SetNumber(Key, Value); }
	void SetBool(const FString& Key, bool Value);
	void SetString(const FString& Key, const FString& Value);
	void SetName(const FString& Key, FName Value);
	void SetVector(const FString& Key, const FVector& Value);
	void SetRotator(const FString& Key, const FRotator& Value);
	void SetTransform(const FString& Key, const FTransform& Value);
	/** [{"item": "item_stone", "count": 3, "fresh": 0.8}, ...] ("fresh" only when below 1). */
	void SetItems(const FString& Key, const TArray<FGamerItemAmount>& Items);
	void SetNames(const FString& Key, const TArray<FName>& Names);
	void SetRecord(const FString& Key, const FGamerSaveRecord& Child);
	void SetRecords(const FString& Key, const TArray<FGamerSaveRecord>& Children);

	double GetNumber(const FString& Key, double Default = 0.0) const;
	float GetFloat(const FString& Key, float Default = 0.f) const { return static_cast<float>(GetNumber(Key, Default)); }
	int32 GetInt(const FString& Key, int32 Default = 0) const;
	bool GetBool(const FString& Key, bool Default = false) const;
	FString GetString(const FString& Key, const FString& Default = FString()) const;
	FName GetName(const FString& Key, FName Default = NAME_None) const;
	FVector GetVector(const FString& Key, const FVector& Default = FVector::ZeroVector) const;
	FRotator GetRotator(const FString& Key, const FRotator& Default = FRotator::ZeroRotator) const;
	FTransform GetTransform(const FString& Key, const FTransform& Default = FTransform::Identity) const;
	/** Entries with an empty id or count <= 0 are skipped. Ids are not checked here (see FilterKnownItems). */
	TArray<FGamerItemAmount> GetItems(const FString& Key) const;
	TArray<FName> GetNames(const FString& Key) const;
	/** Missing child = empty record (all reads return defaults). */
	FGamerSaveRecord GetRecord(const FString& Key) const;
	TArray<FGamerSaveRecord> GetRecords(const FString& Key) const;
};

UINTERFACE(MinimalAPI)
class UGamerSaveable : public UInterface
{
	GENERATED_BODY()
};

/**
 * Everything whose state must survive saving and loading implements this (actors and components).
 * Only the server saves and loads (rule 1); clients get the loaded state through normal replication.
 *
 * Who is collected:
 *  - World actors of the known kinds (resource nodes, buildings incl. farm buildings and tavern, ground items,
 *    villagers) are found and respawned by UGamerSaveSubsystem, which then calls LoadSaveState on the fresh actor
 *    (after BeginPlay, so the implementation must re-arm its own timers from the saved remaining time).
 *  - Singletons (game state, settlement) are saved as one block under GetSaveKey().
 *  - Components on a player's pawn or player state (inventory, buffs, ...) are saved per player under their
 *    GetSaveKey(), so a new system only needs to implement this interface on its component; nothing else changes.
 *
 * Rules: write only what cannot be rebuilt (no derived values, no references to other actors except through
 * the helpers of the save subsystem), keep keys snake_case and stable, never reuse a key for something else.
 * Times are stored as remaining real seconds (or game days), never as absolute world time.
 */
class GAMER_API IGamerSaveable
{
	GENERATED_BODY()

public:
	/** Stable block name, e.g. "inventory", "buffs", "settlement". Must be unique on one owner. */
	virtual FString GetSaveKey() const { return FString(); }
	/** Server: write the state. */
	virtual void WriteSaveState(FGamerSaveRecord& Out) const {}
	/** Server: restore the state. Missing fields keep their defaults. */
	virtual void ReadSaveState(const FGamerSaveRecord& In) {}
};
