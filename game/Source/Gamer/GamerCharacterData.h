#pragma once

#include "CoreMinimal.h"
#include "GamerDataTypes.h"
#include "GamerCharacterData.generated.h"

struct FGamerDataSet;
struct FGamerDataReport;

// Character creation data (Content/Data/characters.json): values, origins, abilities, appearance options.
// Loaded next to the other tables by UGamerDataSubsystem; the server looks origins up when a player joins.

/** One of the five values (Stärke, Geschick, Zähigkeit, Handwerk, Wille). */
USTRUCT(BlueprintType)
struct FGamerStatDef
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Character")
	FName Id;

	UPROPERTY(BlueprintReadOnly, Category = "Character")
	FString Name;

	/** Short label for tight spaces ("STÄ"). */
	UPROPERTY(BlueprintReadOnly, Category = "Character")
	FString Short;

	/** What it will affect (shown in the menu; the effects come with the matching systems). */
	UPROPERTY(BlueprintReadOnly, Category = "Character")
	FString Affects;
};

/** A learnable ability. Anyone can learn every ability; the own origin's branch is cheaper (decision 2026-11-14). */
USTRUCT(BlueprintType)
struct FGamerAbilityDef
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Character")
	FName Id;

	UPROPERTY(BlueprintReadOnly, Category = "Character")
	FString Name;

	UPROPERTY(BlueprintReadOnly, Category = "Character")
	FString Description;

	/** Origin whose branch it belongs to (cheaper for that origin). */
	UPROPERTY(BlueprintReadOnly, Category = "Character")
	FName Branch;

	/** Ability points to learn it (before the own-branch factor). */
	UPROPERTY(BlueprintReadOnly, Category = "Character")
	int32 Cost = 1;
};

/** A value of one stat (replicable list entry). */
USTRUCT(BlueprintType)
struct FGamerStatValue
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Character")
	FName Stat;

	UPROPERTY(BlueprintReadOnly, Category = "Character")
	int32 Value = 0;
};

/** A chosen appearance option for one slot (replicable list entry). */
USTRUCT(BlueprintType)
struct FGamerLookChoice
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Character")
	FName Slot;

	UPROPERTY(BlueprintReadOnly, Category = "Character")
	FName Option;
};

/** An origin (class): start values, start equipment, start ability, suggested look. */
USTRUCT(BlueprintType)
struct FGamerOriginDef
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Character")
	FName Id;

	UPROPERTY(BlueprintReadOnly, Category = "Character")
	FString Name;

	UPROPERTY(BlueprintReadOnly, Category = "Character")
	FString Description;

	/** Strength in co-op ("Erkunden, Jagd"). */
	UPROPERTY(BlueprintReadOnly, Category = "Character")
	FString Role;

	/** One entry per stat, in the order of the stat list. */
	UPROPERTY(BlueprintReadOnly, Category = "Character")
	TArray<FGamerStatValue> Stats;

	UPROPERTY(BlueprintReadOnly, Category = "Character")
	TArray<FGamerItemAmount> StartItems;

	UPROPERTY(BlueprintReadOnly, Category = "Character")
	FName StartAbility;

	/** Suggested look; slots not listed use the slot's first option. */
	UPROPERTY(BlueprintReadOnly, Category = "Character")
	TArray<FGamerLookChoice> Appearance;

	int32 GetStat(FName Stat) const
	{
		const FGamerStatValue* Found = Stats.FindByPredicate([Stat](const FGamerStatValue& V) { return V.Stat == Stat; });
		return Found ? Found->Value : 0;
	}
};

USTRUCT(BlueprintType)
struct FGamerLookOption
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Character")
	FName Id;

	UPROPERTY(BlueprintReadOnly, Category = "Character")
	FString Name;
};

/** One appearance slot of the editor (body, face, hair ...). Option IDs are stored; models come later (MetaHuman). */
USTRUCT(BlueprintType)
struct FGamerLookSlotDef
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Character")
	FName Id;

	UPROPERTY(BlueprintReadOnly, Category = "Character")
	FString Name;

	UPROPERTY(BlueprintReadOnly, Category = "Character")
	TArray<FGamerLookOption> Options;

	bool HasOption(FName Option) const
	{
		return Options.ContainsByPredicate([Option](const FGamerLookOption& O) { return O.Id == Option; });
	}
};

/**
 * What a player made in the character screen. Saved locally per player (UGamerCharacterSettings) and sent to the
 * server on joining; the server checks it and looks up values and equipment itself (the client never sends values).
 */
USTRUCT(BlueprintType)
struct FGamerCharacterProfile
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Character")
	FString Name;

	UPROPERTY(BlueprintReadOnly, Category = "Character")
	FName Origin;

	UPROPERTY(BlueprintReadOnly, Category = "Character")
	TArray<FGamerLookChoice> Appearance;

	FName GetLook(FName Slot) const
	{
		const FGamerLookChoice* Found = Appearance.FindByPredicate([Slot](const FGamerLookChoice& C) { return C.Slot == Slot; });
		return Found ? Found->Option : NAME_None;
	}
	void SetLook(FName Slot, FName Option);

	/** "Ragnar (origin_warrior) look_body=body_strong ..." for logs. */
	FString ToLogString() const;
};

/** All character data, in file order (the menu shows it in this order). */
struct GAMER_API FGamerCharacterDataSet
{
	int32 StatSum = 25;
	int32 StatMin = 2;
	int32 StatMax = 8;
	int32 NameMaxLength = 16;
	float OwnBranchCostFactor = 0.5f;

	TArray<FGamerStatDef> Stats;
	TArray<FGamerAbilityDef> Abilities;
	TArray<FGamerOriginDef> Origins;
	TArray<FGamerLookSlotDef> LookSlots;
	TArray<FString> Names;

	uint32 Checksum = 0;

	bool IsEmpty() const { return Origins.Num() == 0; }
	const FGamerOriginDef* FindOrigin(FName Id) const { return Origins.FindByPredicate([Id](const FGamerOriginDef& D) { return D.Id == Id; }); }
	const FGamerAbilityDef* FindAbility(FName Id) const { return Abilities.FindByPredicate([Id](const FGamerAbilityDef& D) { return D.Id == Id; }); }
	const FGamerStatDef* FindStat(FName Id) const { return Stats.FindByPredicate([Id](const FGamerStatDef& D) { return D.Id == Id; }); }
	const FGamerLookSlotDef* FindLookSlot(FName Id) const { return LookSlots.FindByPredicate([Id](const FGamerLookSlotDef& D) { return D.Id == Id; }); }

	/** Ability point cost for a character of the given origin (own branch cheaper, at least 1). */
	int32 AbilityCostFor(const FGamerAbilityDef& Ability, FName Origin) const;

	/**
	 * Makes a profile valid: unknown origin -> first origin, name trimmed/shortened (empty -> first name of the list),
	 * every slot filled with a known option (missing or unknown -> origin suggestion, else the slot's first option).
	 * Returns false if something had to be changed (the server logs that).
	 */
	bool Sanitize(FGamerCharacterProfile& Profile) const;

	/** A new profile for the origin with its suggested look and the given name. */
	FGamerCharacterProfile MakeProfile(FName Origin, const FString& Name) const;

	void Reset() { *this = FGamerCharacterDataSet(); }
};

namespace GamerData
{
	inline const TCHAR* const CharactersFile = TEXT("characters.json");

	/**
	 * Loads and checks characters.json in Directory. Items of start equipment are checked against Base.
	 * If the file is missing: error when bRequired, else Out stays empty. Returns true if there were no errors.
	 */
	/** Item IDs any origin starts with (characters.json), without checks. Used by the item reachability check. */
	GAMER_API void ReadStartItems(const FString& Directory, TSet<FName>& OutItems);

	GAMER_API bool LoadCharacters(const FString& Directory, const FGamerDataSet& Base, FGamerCharacterDataSet& Out, FGamerDataReport& Report,
		bool bRequired = true);
}
