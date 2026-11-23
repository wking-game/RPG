#include "GamerProgression.h"

#include "GamerDataLoader.h"
#include "GamerJsonTable.h"

// Validation messages are German on purpose: they are read by whoever edits progression.json.

int64 FGamerProgressionData::TotalXpForLevel(int32 Level) const
{
	int64 Sum = 0;
	const int32 Target = FMath::Clamp(Level, 1, FMath::Max(1, MaxLevel));
	for (int32 Index = 0; Index < Target - 1 && Index < XpToNext.Num(); ++Index)
	{
		Sum += XpToNext[Index];
	}
	return Sum;
}

int32 FGamerProgressionData::LevelForXp(int64 TotalXp) const
{
	int32 Level = 1;
	int64 Needed = 0;
	for (int32 Index = 0; Index < XpToNext.Num() && Level < MaxLevel; ++Index)
	{
		Needed += XpToNext[Index];
		if (TotalXp < Needed)
		{
			break;
		}
		Level = Index + 2;
	}
	return Level;
}

int32 FGamerProgressionData::XpToNextLevel(int32 Level) const
{
	return (Level >= 1 && Level < MaxLevel && XpToNext.IsValidIndex(Level - 1)) ? XpToNext[Level - 1] : 0;
}

const FGamerTierDef* FGamerProgressionData::TierForLevel(int32 Level) const
{
	return Tiers.FindByPredicate([Level](const FGamerTierDef& T) { return Level >= T.LevelMin && Level <= T.LevelMax; });
}

int32 FGamerProgressionData::SkillPointsForLevel(int32 Level) const
{
	const int32 Reached = FMath::Clamp(Level, 1, FMath::Max(1, MaxLevel));
	const int32 Bonuses = SkillPointBonusEvery > 0 ? Reached / SkillPointBonusEvery : 0;
	return (Reached - 1) * SkillPointsPerLevel + Bonuses * SkillPointBonus;
}

float FGamerProgressionData::UnderLevelFactor(int32 TiersBelow) const
{
	if (UnderLevelFactors.Num() == 0 || TiersBelow <= 0)
	{
		return 1.f;
	}
	return UnderLevelFactors[FMath::Min(TiersBelow, UnderLevelFactors.Num() - 1)];
}

float FGamerProgressionData::KillXp(int32 Rank, int32 LeaderIndex) const
{
	const float Multiplier = LeaderMultipliers.IsValidIndex(LeaderIndex) ? LeaderMultipliers[LeaderIndex] : 1.f;
	return static_cast<float>(FMath::Max(0, Rank)) * KillPerRank * Multiplier;
}

float FGamerProgressionData::CraftXp(int32 Tier, int32 Count, bool bFirstTime) const
{
	const float T = static_cast<float>(FMath::Max(1, Tier));
	const int32 Pieces = FMath::Max(0, Count);
	if (Pieces == 0)
	{
		return 0.f;
	}
	return bFirstTime ? T * (FirstCraftPerTier + RepeatCraftPerTier * static_cast<float>(Pieces - 1)) : T * RepeatCraftPerTier * static_cast<float>(Pieces);
}

float FGamerProgressionData::GatherXp(int32 Tier, int32 Count) const
{
	return static_cast<float>(FMath::Max(1, Tier)) * GatherPerTier * static_cast<float>(FMath::Max(0, Count));
}

float FGamerProgressionData::BuildXp(int32 Tier) const
{
	return static_cast<float>(FMath::Max(1, Tier)) * BuildPiecePerTier;
}

namespace
{
	using GamerJson::FRow;

	bool ReadPair(FRow& Row, const TCHAR* Field, int32 (&Out)[2])
	{
		const TArray<TSharedPtr<FJsonValue>>* List = Row.GetArray(Field, true);
		if (!List)
		{
			return false;
		}
		if (List->Num() != 2)
		{
			Row.Error(FString::Printf(TEXT("\"%s\" muss genau zwei Zahlen [min, max] enthalten"), Field));
			return false;
		}
		for (int32 Index = 0; Index < 2; ++Index)
		{
			double Number = 0.0;
			if (!(*List)[Index].IsValid() || !(*List)[Index]->TryGetNumber(Number))
			{
				Row.Error(FString::Printf(TEXT("\"%s\" darf nur Zahlen enthalten"), Field));
				return false;
			}
			Out[Index] = static_cast<int32>(Number);
		}
		if (Out[0] < 0 || Out[1] < Out[0])
		{
			Row.Error(FString::Printf(TEXT("\"%s\": min darf nicht negativ sein und max nicht kleiner als min"), Field));
			return false;
		}
		return true;
	}
}

bool GamerData::LoadProgression(const FString& Directory, FGamerProgressionData& Out, FGamerDataReport& Report, bool bRequired)
{
	Out = FGamerProgressionData();
	const int32 ErrorsBefore = Report.Errors.Num();
	TSharedPtr<FJsonObject> Root;
	uint32 Crc = 0;
	if (!GamerJson::ReadObjectFile(Directory, ProgressionFile, Root, Crc, Report, !bRequired))
	{
		return Report.Errors.Num() == ErrorsBefore;
	}
	Out.Checksum = Crc;
	Out.bLoaded = true;

	FRow Top(Root, ProgressionFile, Report);
	Top.CheckKnown({ TEXT("schema"), TEXT("max_level"), TEXT("tiers"), TEXT("xp_to_next"), TEXT("skill_points"), TEXT("xp_sources"), TEXT("coop"),
		TEXT("under_level_factor"), TEXT("typical_hour") });

	Top.GetInt(TEXT("max_level"), Out.MaxLevel, true);
	if (Out.MaxLevel < 2 || Out.MaxLevel > GamerData::MaxPlayerLevel)
	{
		Top.Error(FString::Printf(TEXT("max_level muss zwischen 2 und %d liegen"), GamerData::MaxPlayerLevel));
		Out.MaxLevel = FMath::Clamp(Out.MaxLevel, 2, GamerData::MaxPlayerLevel);
	}

	if (const TSharedPtr<FJsonObject> Hour = Top.GetSubObject(TEXT("typical_hour"), true))
	{
		FRow Row(Hour, FString::Printf(TEXT("%s, typical_hour"), ProgressionFile), Report);
		Row.GetFloat(TEXT("xp_per_tier"), Out.TypicalHourXpPerTier, true);
		if (Out.TypicalHourXpPerTier <= 0.f)
		{
			Row.Error(TEXT("xp_per_tier muss größer als 0 sein"));
		}
	}

	if (const TArray<TSharedPtr<FJsonValue>>* Tiers = Top.GetArray(TEXT("tiers"), true))
	{
		for (int32 Index = 0; Index < Tiers->Num(); ++Index)
		{
			const TSharedPtr<FJsonValue>& Value = (*Tiers)[Index];
			if (!Value.IsValid() || Value->Type != EJson::Object)
			{
				Report.Errors.Add(FString::Printf(TEXT("%s, tiers[%d]: muss ein Objekt { ... } sein"), ProgressionFile, Index + 1));
				continue;
			}
			FRow Row(Value->AsObject(), FString::Printf(TEXT("%s, tiers[%d]"), ProgressionFile, Index + 1), Report);
			Row.CheckKnown({ TEXT("tier"), TEXT("biome"), TEXT("level_min"), TEXT("level_max"), TEXT("hours"), TEXT("weapon_damage"),
				TEXT("weapon_poise"), TEXT("armor"), TEXT("poise_resist"), TEXT("resist_cap") });
			FGamerTierDef Tier;
			Row.GetInt(TEXT("tier"), Tier.Tier, true);
			FString Biome;
			if (Row.GetString(TEXT("biome"), Biome, true))
			{
				Tier.Biome = FName(*Biome);
				if (!Biome.StartsWith(TEXT("biome_"))) { Row.Error(TEXT("biome muss mit biome_ beginnen")); }
			}
			Row.GetInt(TEXT("level_min"), Tier.LevelMin, true);
			Row.GetInt(TEXT("level_max"), Tier.LevelMax, true);
			Row.GetFloat(TEXT("hours"), Tier.Hours, true);
			Row.GetFloat(TEXT("resist_cap"), Tier.ResistCap, true);
			ReadPair(Row, TEXT("weapon_damage"), Tier.WeaponDamage);
			ReadPair(Row, TEXT("weapon_poise"), Tier.WeaponPoise);
			ReadPair(Row, TEXT("armor"), Tier.Armor);
			ReadPair(Row, TEXT("poise_resist"), Tier.PoiseResist);
			if (Tier.Hours <= 0.f) { Row.Error(TEXT("hours muss größer als 0 sein")); }
			if (Tier.ResistCap < 0.f || Tier.ResistCap > 1.f) { Row.Error(TEXT("resist_cap muss zwischen 0 und 1 liegen")); }
			Out.Tiers.Add(Tier);
		}

		// Tiers 1..6 in order; levels without gaps from 1 to MaxLevel; equipment values rise from tier to tier without overlap.
		if (Out.Tiers.Num() != GamerData::MaxTier)
		{
			Top.Error(FString::Printf(TEXT("tiers muss genau %d Stufen enthalten (T1 bis T6)"), GamerData::MaxTier));
		}
		for (int32 Index = 0; Index < Out.Tiers.Num(); ++Index)
		{
			const FGamerTierDef& Tier = Out.Tiers[Index];
			const FString Where = FString::Printf(TEXT("%s, tiers[%d] (T%d)"), ProgressionFile, Index + 1, Tier.Tier);
			if (Tier.Tier != Index + 1)
			{
				Report.Errors.Add(Where + TEXT(": tier muss der Reihe nach 1, 2, 3 … sein"));
			}
			if (Tier.LevelMin > Tier.LevelMax)
			{
				Report.Errors.Add(Where + TEXT(": level_min ist größer als level_max"));
			}
			const int32 ExpectedMin = Index == 0 ? 1 : Out.Tiers[Index - 1].LevelMax + 1;
			if (Tier.LevelMin != ExpectedMin)
			{
				Report.Errors.Add(FString::Printf(TEXT("%s: level_min ist %d, erwartet %d (Lücke oder Überlappung der Level-Bereiche)"), *Where, Tier.LevelMin, ExpectedMin));
			}
			if (Index == Out.Tiers.Num() - 1 && Tier.LevelMax != Out.MaxLevel)
			{
				Report.Errors.Add(FString::Printf(TEXT("%s: level_max ist %d, muss max_level (%d) sein"), *Where, Tier.LevelMax, Out.MaxLevel));
			}
			if (Index > 0)
			{
				const FGamerTierDef& Previous = Out.Tiers[Index - 1];
				if (Tier.WeaponDamage[0] <= Previous.WeaponDamage[1] || Tier.WeaponPoise[0] <= Previous.WeaponPoise[1]
					|| Tier.Armor[0] <= Previous.Armor[1] || Tier.PoiseResist[0] <= Previous.PoiseResist[1])
				{
					Report.Errors.Add(Where + TEXT(": Waffen-, Rüstungs- oder Haltungswerte überlappen die vorige Stufe (sie müssen ansteigen)"));
				}
				if (Tier.ResistCap <= Previous.ResistCap)
				{
					Report.Errors.Add(Where + TEXT(": resist_cap muss gegenüber der vorigen Stufe steigen"));
				}
			}
		}
	}

	if (const TArray<TSharedPtr<FJsonValue>>* Xp = Top.GetArray(TEXT("xp_to_next"), true))
	{
		for (const TSharedPtr<FJsonValue>& Value : *Xp)
		{
			double Number = 0.0;
			if (!Value.IsValid() || !Value->TryGetNumber(Number) || Number <= 0.0 || FMath::FloorToDouble(Number) != Number)
			{
				Top.Error(TEXT("xp_to_next darf nur ganze Zahlen größer als 0 enthalten"));
				break;
			}
			Out.XpToNext.Add(static_cast<int32>(Number));
		}
		if (Out.XpToNext.Num() != Out.MaxLevel - 1)
		{
			Top.Error(FString::Printf(TEXT("xp_to_next braucht %d Einträge (max_level - 1), gefunden %d"), Out.MaxLevel - 1, Out.XpToNext.Num()));
		}
		for (int32 Index = 1; Index < Out.XpToNext.Num(); ++Index)
		{
			if (Out.XpToNext[Index] <= Out.XpToNext[Index - 1])
			{
				Top.Error(FString::Printf(TEXT("xp_to_next steigt nicht streng bei Level %d (%d nach %d)"), Index + 1, Out.XpToNext[Index - 1], Out.XpToNext[Index]));
				break;
			}
		}
		// The sum of each tier must fit hours * experience per hour * tier (rounding to 5 is allowed).
		if (Out.TypicalHourXpPerTier > 0.f && Out.XpToNext.Num() == Out.MaxLevel - 1)
		{
			for (const FGamerTierDef& Tier : Out.Tiers)
			{
				int64 Sum = 0;
				for (int32 Level = Tier.LevelMin; Level <= FMath::Min(Tier.LevelMax, Out.MaxLevel - 1); ++Level)
				{
					Sum += Out.XpToNext[Level - 1];
				}
				const double Wanted = static_cast<double>(Tier.Hours) * Out.TypicalHourXpPerTier * Tier.Tier;
				if (Wanted > 0.0 && FMath::Abs(static_cast<double>(Sum) - Wanted) / Wanted > 0.01)
				{
					Top.Error(FString::Printf(TEXT("T%d: die Erfahrung der Level (%lld) passt nicht zu Stunden × Erfahrung je Stunde × Stufe (%.0f)"), Tier.Tier, Sum, Wanted));
				}
			}
		}
	}

	if (const TSharedPtr<FJsonObject> Points = Top.GetSubObject(TEXT("skill_points"), true))
	{
		FRow Row(Points, FString::Printf(TEXT("%s, skill_points"), ProgressionFile), Report);
		Row.CheckKnown({ TEXT("per_level"), TEXT("bonus_every"), TEXT("bonus") });
		Row.GetInt(TEXT("per_level"), Out.SkillPointsPerLevel, true);
		Row.GetInt(TEXT("bonus_every"), Out.SkillPointBonusEvery, true);
		Row.GetInt(TEXT("bonus"), Out.SkillPointBonus, true);
		if (Out.SkillPointsPerLevel < 0 || Out.SkillPointBonus < 0 || Out.SkillPointBonusEvery < 1)
		{
			Row.Error(TEXT("per_level und bonus dürfen nicht negativ sein, bonus_every mindestens 1"));
		}
	}

	if (const TSharedPtr<FJsonObject> Sources = Top.GetSubObject(TEXT("xp_sources"), true))
	{
		FRow Row(Sources, FString::Printf(TEXT("%s, xp_sources"), ProgressionFile), Report);
		Row.CheckKnown({ TEXT("kill_per_rank"), TEXT("leader_multipliers"), TEXT("boss_first_win_per_tier"), TEXT("quest_stage_per_tier"),
			TEXT("discovery_per_tier"), TEXT("first_craft_per_tier"), TEXT("repeat_craft_per_tier"), TEXT("build_piece_per_tier"), TEXT("gather_per_tier") });
		Row.GetFloat(TEXT("kill_per_rank"), Out.KillPerRank, true);
		Row.GetFloat(TEXT("boss_first_win_per_tier"), Out.BossFirstWinPerTier, true);
		Row.GetFloat(TEXT("quest_stage_per_tier"), Out.QuestStagePerTier, true);
		Row.GetFloat(TEXT("discovery_per_tier"), Out.DiscoveryPerTier, true);
		Row.GetFloat(TEXT("first_craft_per_tier"), Out.FirstCraftPerTier, true);
		Row.GetFloat(TEXT("repeat_craft_per_tier"), Out.RepeatCraftPerTier, true);
		Row.GetFloat(TEXT("build_piece_per_tier"), Out.BuildPiecePerTier, true);
		Row.GetFloat(TEXT("gather_per_tier"), Out.GatherPerTier, true);
		if (const TArray<TSharedPtr<FJsonValue>>* Mult = Row.GetArray(TEXT("leader_multipliers"), true))
		{
			float Previous = 1.f;
			for (const TSharedPtr<FJsonValue>& Value : *Mult)
			{
				double Number = 0.0;
				if (!Value.IsValid() || !Value->TryGetNumber(Number) || Number < Previous)
				{
					Row.Error(TEXT("leader_multipliers müssen Zahlen sein, die ab 1 aufsteigen"));
					break;
				}
				Previous = static_cast<float>(Number);
				Out.LeaderMultipliers.Add(static_cast<float>(Number));
			}
		}
		if (Out.KillPerRank <= 0.f || Out.BossFirstWinPerTier < 0.f || Out.QuestStagePerTier < 0.f || Out.DiscoveryPerTier < 0.f
			|| Out.FirstCraftPerTier < 0.f || Out.RepeatCraftPerTier < 0.f || Out.BuildPiecePerTier < 0.f || Out.GatherPerTier < 0.f)
		{
			Row.Error(TEXT("kill_per_rank muss größer als 0 sein, alle anderen Quellen dürfen nicht negativ sein"));
		}
	}

	if (const TSharedPtr<FJsonObject> Coop = Top.GetSubObject(TEXT("coop"), true))
	{
		FRow Row(Coop, FString::Printf(TEXT("%s, coop"), ProgressionFile), Report);
		Row.CheckKnown({ TEXT("radius_cm"), TEXT("share_combat"), TEXT("share_work") });
		Row.GetFloat(TEXT("radius_cm"), Out.CoopRadiusCm, true);
		Row.GetFloat(TEXT("share_combat"), Out.CoopShareCombat, true);
		Row.GetFloat(TEXT("share_work"), Out.CoopShareWork, true);
		if (Out.CoopRadiusCm <= 0.f || Out.CoopShareCombat < 0.f || Out.CoopShareCombat > 1.f || Out.CoopShareWork < 0.f || Out.CoopShareWork > 1.f)
		{
			Row.Error(TEXT("radius_cm muss größer als 0 sein, die Anteile müssen zwischen 0 und 1 liegen"));
		}
	}

	if (const TArray<TSharedPtr<FJsonValue>>* Factors = Top.GetArray(TEXT("under_level_factor"), true))
	{
		float Previous = 1.f;
		for (int32 Index = 0; Index < Factors->Num(); ++Index)
		{
			double Number = 0.0;
			if (!(*Factors)[Index].IsValid() || !(*Factors)[Index]->TryGetNumber(Number) || Number <= 0.0 || Number > 1.0 || Number > Previous
				|| (Index == 0 && Number != 1.0))
			{
				Top.Error(TEXT("under_level_factor: Zahlen größer als 0, die mit 1,0 beginnen und nicht steigen"));
				break;
			}
			Previous = static_cast<float>(Number);
			Out.UnderLevelFactors.Add(static_cast<float>(Number));
		}
		if (Out.UnderLevelFactors.Num() == 0)
		{
			Top.Error(TEXT("under_level_factor darf nicht leer sein"));
		}
	}

	return Report.Errors.Num() == ErrorsBefore;
}
