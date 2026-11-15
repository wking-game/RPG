#include "GamerSpoilage.h"

#include "Gamer.h"
#include "GamerDataTypes.h"
#include "HAL/IConsoleManager.h"
#include "Misc/CommandLine.h"
#include "Misc/Parse.h"

namespace
{
	TAutoConsoleVariable<float> CVarSpoilScale(
		TEXT("gamer.SpoilScale"),
		1.f,
		TEXT("Speeds up spoiling (tests: 100 = a hundred times faster). 1 = normal."));
}

float UGamerSpoilage::Loss(const FGamerItemDef& Def, float Seconds, EGamerSpoilPlace Place)
{
	if (!Def.Spoils() || Seconds <= 0.f)
	{
		return 0.f;
	}
	const UGamerSpoilage& S = Get();
	const float Factor = Place == EGamerSpoilPlace::Storage ? S.StorageFactor : Place == EGamerSpoilPlace::Ground ? S.GroundFactor : S.CarriedFactor;
	// Decision 2026-11-14: higher tiers keep longer (T1 = data value, every tier above adds TierBonus).
	const float TierFactor = 1.f + S.TierBonus * FMath::Max(0, Def.Tier - 1);
	const float Lifetime = FMath::Max(1.f, Def.SpoilDays * TierFactor * S.DayLengthSeconds);
	return Seconds * Factor * FMath::Max(0.f, CVarSpoilScale.GetValueOnGameThread()) / Lifetime;
}

float UGamerSpoilage::Merge(int32 CountA, float FreshA, int32 CountB, float FreshB)
{
	const int32 Total = FMath::Max(0, CountA) + FMath::Max(0, CountB);
	if (Total <= 0)
	{
		return 1.f;
	}
	return FMath::Clamp((FMath::Max(0, CountA) * FreshA + FMath::Max(0, CountB) * FreshB) / Total, 0.f, 1.f);
}

void UGamerSpoilage::ApplyCommandLine()
{
	float Scale = 0.f;
	if (FParse::Value(FCommandLine::Get(), TEXT("GamerSpoilScale="), Scale) && Scale > 0.f)
	{
		CVarSpoilScale->Set(Scale, ECVF_SetByCommandline);
		UE_LOG(LogGamer, Log, TEXT("Spoiling: time scale %g (command line)"), Scale);
	}
}
