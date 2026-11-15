#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "GamerSpoilage.generated.h"

struct FGamerItemDef;

/** Where a perishable item is kept; scales how fast it spoils. */
enum class EGamerSpoilPlace : uint8
{
	/** Player inventory, villager load ("nicht trocken gelagert"). */
	Carried,
	/** Settlement storage (under a roof). */
	Storage,
	/** Lying on the ground (fallen fruit). */
	Ground
};

/**
 * Spoiling rules (decision 2026-11-14: spoiling in phase 1, warning at 30 %, one beginner tip).
 * Durability per item comes from items.json ("spoil_days", game days while carried); the place factors
 * below scale it. Proposals by Claude, marked in docs/10_nahrung_feldbau.md:
 *   storage 0.25 (four times as long), ground 2.0 (half as long), warning at 30 % left.
 * Tests: console variable gamer.SpoilScale (or -GamerSpoilScale=N on the server) speeds everything up.
 */
UCLASS(Config = Game)
class GAMER_API UGamerSpoilage : public UObject
{
	GENERATED_BODY()

public:
	static const UGamerSpoilage& Get() { return *GetDefault<UGamerSpoilage>(); }

	/** Freshness lost (0..1) by an item of this kind kept Seconds (real time) at Place. 0 if it does not spoil. */
	static float Loss(const FGamerItemDef& Def, float Seconds, EGamerSpoilPlace Place);

	/** Freshness of a merged stack (weighted by count). */
	static float Merge(int32 CountA, float FreshA, int32 CountB, float FreshB);

	/** True when the freshness went from above the warning line to at/below it. */
	static bool CrossedWarning(float Before, float After) { return Before > Get().WarnAt && After <= Get().WarnAt; }

	/** Text "noch 30 %" style percentage of freshness. */
	static int32 Percent(float Freshness) { return FMath::Clamp(FMath::RoundToInt(Freshness * 100.f), 0, 100); }

	/** Reads -GamerSpoilScale=N (server command line, tests). */
	static void ApplyCommandLine();

	/** Length of a game day in seconds (same as resources and villagers: 30 min). */
	UPROPERTY(Config)
	float DayLengthSeconds = 1800.f;

	UPROPERTY(Config)
	float CarriedFactor = 1.f;

	UPROPERTY(Config)
	float StorageFactor = 0.25f;

	UPROPERTY(Config)
	float GroundFactor = 2.f;

	/** Extra durability per tier above T1 (0.5: T2 lasts 1.5 times, T3 twice as long). Proposal by Claude. */
	UPROPERTY(Config)
	float TierBonus = 0.5f;

	/** Warn when this much freshness is left. */
	UPROPERTY(Config)
	float WarnAt = 0.3f;

	/** Seconds between two spoiling updates on the server. */
	UPROPERTY(Config)
	float TickSeconds = 2.f;
};
