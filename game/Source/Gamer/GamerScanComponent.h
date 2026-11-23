#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameFramework/Actor.h"
#include "GamerScanComponent.generated.h"

/** One marker after a look-around. */
struct FGamerScanMark
{
	FVector Location = FVector::ZeroVector;
	int32 Category = 0;
	/** When the wave reaches it (it appears) and when it fades. */
	double RevealTime = 0.0;
	double ExpireTime = 0.0;
};

/** One look-around category (scan.json): name, colour, short symbol. */
struct FGamerScanCategory
{
	FName Id;
	FString Name;
	FLinearColor Color = FLinearColor::White;
	FString Symbol;
};

/**
 * Look around (key Z; decision 2026-11-15: "Umschauen" and "Lauschen" are two separate senses). Shows only material:
 * food, wood, stone, plants and lying things, through walls, for a while. No levels, no progress, no magic (magic is
 * listened for, see UGamerListenComponent). The wave is a sine curve running outwards. Purely local: it only reads
 * what is replicated anyway. Values (radius 40 m, spread 5 s, visible 20 s, cooldown, wave shape, categories) come from
 * `Content/Data/scan.json`; the defaults here are used when the file is missing.
 */
UCLASS(ClassGroup = (Gamer), meta = (BlueprintSpawnableComponent))
class GAMER_API UGamerScanComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UGamerScanComponent();

	/** Local player: look around now, or say how long the cooldown still lasts. Returns true if it ran. */
	bool TryScan();

	const TArray<FGamerScanMark>& GetMarks() const { return Marks; }
	const TArray<FGamerScanCategory>& GetCategories() const { return Categories; }
	int32 GetCount(int32 Category) const { return Counts.IsValidIndex(Category) ? Counts[Category] : 0; }

	/** Seconds until the next look-around is possible (0 = ready). */
	float GetCooldownLeft() const;
	/** True while any marker is still visible or the wave is running. */
	bool IsShowing() const;
	double GetMarkFadeSeconds() const { return 1.5; }

	/** The wave: where it started, how long it takes to reach the radius, how far it is, its sine shape. */
	FVector GetCenter() const { return Center; }
	float GetRadiusCm() const { return RadiusCm; }
	float GetSpreadSeconds() const { return SpreadSeconds; }
	float GetWaveAmplitudeCm() const { return WaveAmplitudeCm; }
	int32 GetWaveCount() const { return WaveCount; }
	float GetRippleSpeed() const { return RippleSpeed; }
	/** Seconds since the last look-around started (large when none is running). */
	float GetAgeSeconds() const;
	/** Tests only: freeze the wave at this age (seconds); a negative value ends it. */
	void SetDebugAge(float Seconds) { DebugAge = Seconds; }

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/** Test helper: category counts as text ("food=3 wood=2 ..."). */
	FString DescribeCounts() const;

private:
	void LoadSettings();
	/** Adds the markers within the radius of the current centre; with bNewOnly only things not yet marked (the wave moves with the player). */
	void CollectMarks(bool bNewOnly, double RevealBase);
	int32 CategoryOfNode(const struct FGamerResourceNodeDef& Def) const;
	int32 CategoryIndex(FName Id) const;

	bool bLoaded = false;
	float RadiusCm = 4000.f;
	float SpreadSeconds = 5.f;
	float VisibleSeconds = 20.f;
	float CooldownSeconds = 12.f;
	float WaveAmplitudeCm = 110.f;
	int32 WaveCount = 7;
	float RippleSpeed = 1.4f;
	TArray<FGamerScanCategory> Categories;
	TArray<FGamerScanMark> Marks;
	TArray<int32> Counts;
	double LastScanTime = -1000.0;
	double ShowUntil = 0.0;
	FVector Center = FVector::ZeroVector;
	TSet<TWeakObjectPtr<const AActor>> Marked;
	double LastCollectTime = 0.0;
	float DebugAge = -1.f;
};
