#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GamerScanComponent.generated.h"

/** One marker after a scan. */
struct FGamerScanMark
{
	FVector Location = FVector::ZeroVector;
	int32 Category = 0;
	double ExpireTime = 0.0;
};

/** One scan category (data/scan.json): name, colour, short symbol. */
struct FGamerScanCategory
{
	FName Id;
	FString Name;
	FLinearColor Color = FLinearColor::White;
	FString Symbol;
};

/**
 * Environment scan (decision 2026-11-15): one key marks nearby food, wood, stone, plants and lying items for a few seconds,
 * visible through walls, then a cooldown (no stamina). Purely local: it only reads what is replicated anyway, so nothing
 * is sent over the network. Values (radius, duration, cooldown, categories, origin bonus such as the hunter's 60 m) come
 * from `Content/Data/scan.json`; the defaults below are used when the file is missing.
 * Not yet: animals and enemies (no such actors yet), origin bonus (no origins yet).
 */
UCLASS(ClassGroup = (Gamer), meta = (BlueprintSpawnableComponent))
class GAMER_API UGamerScanComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UGamerScanComponent();

	/** Local player: scan now, or say how long the cooldown still lasts. Returns true if a scan ran. */
	bool TryScan();

	const TArray<FGamerScanMark>& GetMarks() const { return Marks; }
	const TArray<FGamerScanCategory>& GetCategories() const { return Categories; }
	int32 GetCount(int32 Category) const { return Counts.IsValidIndex(Category) ? Counts[Category] : 0; }

	/** Seconds until the next scan is possible (0 = ready). */
	float GetCooldownLeft() const;
	bool IsShowing() const;
	double GetMarkFadeSeconds() const { return 1.5; }

	/** Test helper: category counts as text ("food=3 wood=2 ..."). */
	FString DescribeCounts() const;

private:
	void LoadSettings();
	int32 CategoryOfNode(const struct FGamerResourceNodeDef& Def) const;
	int32 CategoryIndex(FName Id) const;

	bool bLoaded = false;
	float RadiusCm = 4000.f;
	float DurationSeconds = 8.f;
	float CooldownSeconds = 12.f;
	TArray<FGamerScanCategory> Categories;
	TArray<FGamerScanMark> Marks;
	TArray<int32> Counts;
	double LastScanTime = -1000.0;
};
