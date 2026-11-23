#include "GamerCombatMath.h"

#include "GamerDataLoader.h"
#include "GamerJsonTable.h"

// Validation messages are German on purpose: they are read by whoever edits combat.json.

float GamerCombat::ArmorDamageReduction(float Armor, const FGamerCombatData& Data)
{
	const float Safe = FMath::Max(0.f, Armor);
	const float K = FMath::Max(1.f, Data.ArmorK);
	return FMath::Clamp(Safe / (Safe + K), 0.f, Data.ArmorMaxReduction);
}

float GamerCombat::ApplyArmor(float RawDamage, float Armor, const FGamerCombatData& Data)
{
	return FMath::Max(Data.MinDamage, RawDamage * (1.f - ArmorDamageReduction(Armor, Data)));
}

float GamerCombat::ClampResistance(float TotalResistance, const FGamerCombatData& Data)
{
	return FMath::Clamp(TotalResistance, Data.ResistMin, Data.ResistMax);
}

float GamerCombat::ApplyResistance(float RawEffect, float TotalResistance, const FGamerCombatData& Data)
{
	return RawEffect * (1.f - ClampResistance(TotalResistance, Data));
}

float GamerCombat::PlayerPoiseDamageTaken(float EnemyPoiseDamage, float AttackPoiseMult, float PlayerPoiseResist, const FGamerCombatData& Data)
{
	const float Resist = FMath::Max(0.f, PlayerPoiseResist);
	const float K = FMath::Max(1.f, Data.PoiseKPlayer);
	return FMath::Max(0.f, EnemyPoiseDamage * AttackPoiseMult * (K / (Resist + K)));
}

float GamerCombat::EnemyPoiseDamageTaken(float WeaponPoiseDamage, float AttackPoiseMult, bool bWeakPoint, const FGamerCombatData& Data)
{
	return FMath::Max(0.f, WeaponPoiseDamage * AttackPoiseMult * (bWeakPoint ? Data.WeakPointPoiseMult : 1.f));
}

float GamerCombat::ZoneArmor(EGamerHitZone Zone, const TMap<FName, float>& SlotArmor, const FGamerCombatData& Data)
{
	const FGamerZoneDef& Def = Data.GetZone(Zone);
	float ArmorSum = 0.f;
	float ShareSum = 0.f;
	for (const FName Slot : Def.Slots)
	{
		const float* Share = Data.SlotShare.Find(Slot);
		ShareSum += Share ? *Share : 0.f;
		if (const float* Armor = SlotArmor.Find(Slot))
		{
			ArmorSum += FMath::Max(0.f, *Armor);
		}
	}
	return ShareSum > 0.f ? ArmorSum / ShareSum : 0.f;
}

float GamerCombat::ZoneDamageFactor(EGamerHitZone Zone, const FGamerCombatData& Data)
{
	return Data.GetZone(Zone).DamageFactor;
}

float GamerCombat::PlayerMaxHp(int32 Toughness, float MealBonus, const FGamerCombatData& Data)
{
	return FMath::Max(1.f, Data.PlayerBaseHp + Data.PlayerHpPerToughness * static_cast<float>(Toughness - Data.ToughnessNeutral) + MealBonus);
}

float GamerCombat::CreatureHp(float BaseHp, float Roll, float LeaderFactor, const FGamerCombatData& Data)
{
	return FMath::Max(1.f, BaseHp * Data.CreatureHpScale * Roll * LeaderFactor);
}

bool FGamerPoiseMeter::AddDamage(float Amount, float Max, bool bBoss, const FGamerCombatData& Data)
{
	SecondsSinceHit = 0.f;
	if (IsStaggered() || Amount <= 0.f)
	{
		return false; // already down: the bar stays empty during the stagger window
	}
	Current += Amount;
	if (Current >= Max)
	{
		Current = 0.f;
		StaggerLeft = bBoss ? Data.BossStaggerSeconds : Data.StaggerSeconds;
		return true;
	}
	return false;
}

void FGamerPoiseMeter::Tick(float DeltaSeconds, float Max, const FGamerCombatData& Data)
{
	if (StaggerLeft > 0.f)
	{
		StaggerLeft = FMath::Max(0.f, StaggerLeft - DeltaSeconds);
	}
	SecondsSinceHit += DeltaSeconds;
	if (SecondsSinceHit > Data.DecayDelaySeconds && Current > 0.f)
	{
		Current = FMath::Max(0.f, Current - Max * Data.DecayFractionPerSecond * DeltaSeconds);
	}
}

namespace
{
	using GamerJson::FRow;

	bool ReadZone(FRow& Zones, const TCHAR* Name, FGamerZoneDef& Out)
	{
		const TSharedPtr<FJsonObject> Object = Zones.GetSubObject(Name, true);
		if (!Object.IsValid())
		{
			return false;
		}
		FRow Row(Object, FString::Printf(TEXT("%s, zones.%s"), GamerData::CombatFile, Name), Zones.GetReport());
		Row.CheckKnown({ TEXT("slots"), TEXT("damage_factor") });
		Row.GetFloat(TEXT("damage_factor"), Out.DamageFactor, true);
		if (Out.DamageFactor <= 0.f)
		{
			Row.Error(TEXT("damage_factor muss größer als 0 sein"));
		}
		if (const TArray<TSharedPtr<FJsonValue>>* Slots = Row.GetArray(TEXT("slots"), true))
		{
			for (const TSharedPtr<FJsonValue>& Slot : *Slots)
			{
				FString Id;
				if (Slot.IsValid() && Slot->TryGetString(Id))
				{
					Out.Slots.Add(FName(*Id));
				}
				else
				{
					Row.Error(TEXT("slots darf nur Texte enthalten"));
				}
			}
			if (Out.Slots.Num() == 0)
			{
				Row.Error(TEXT("slots darf nicht leer sein"));
			}
		}
		return true;
	}
}

bool GamerData::LoadCombat(const FString& Directory, FGamerCombatData& Out, FGamerDataReport& Report, bool bRequired)
{
	Out = FGamerCombatData();
	const int32 ErrorsBefore = Report.Errors.Num();
	TSharedPtr<FJsonObject> Root;
	uint32 Crc = 0;
	if (!GamerJson::ReadObjectFile(Directory, CombatFile, Root, Crc, Report, !bRequired))
	{
		return Report.Errors.Num() == ErrorsBefore; // missing optional file: defaults
	}
	Out.Checksum = Crc;
	Out.bLoaded = true;

	FRow Top(Root, CombatFile, Report);
	Top.CheckKnown({ TEXT("schema"), TEXT("armor"), TEXT("resist"), TEXT("poise"), TEXT("slots"), TEXT("zones"), TEXT("attacks"),
		TEXT("weak_point_poise_mult"), TEXT("spread"), TEXT("leaders"), TEXT("loot"), TEXT("creature_hp_scale"), TEXT("player_hp") });

	if (const TSharedPtr<FJsonObject> Armor = Top.GetSubObject(TEXT("armor"), true))
	{
		FRow Row(Armor, FString::Printf(TEXT("%s, armor"), CombatFile), Report);
		Row.CheckKnown({ TEXT("k"), TEXT("max_reduction"), TEXT("min_damage") });
		Row.GetFloat(TEXT("k"), Out.ArmorK, true);
		Row.GetFloat(TEXT("max_reduction"), Out.ArmorMaxReduction, true);
		Row.GetFloat(TEXT("min_damage"), Out.MinDamage, true);
		if (Out.ArmorK <= 0.f) { Row.Error(TEXT("k muss größer als 0 sein")); }
		if (Out.ArmorMaxReduction <= 0.f || Out.ArmorMaxReduction >= 1.f) { Row.Error(TEXT("max_reduction muss zwischen 0 und 1 liegen (ohne die Grenzen)")); }
		if (Out.MinDamage < 0.f) { Row.Error(TEXT("min_damage darf nicht negativ sein")); }
	}
	if (const TSharedPtr<FJsonObject> Resist = Top.GetSubObject(TEXT("resist"), true))
	{
		FRow Row(Resist, FString::Printf(TEXT("%s, resist"), CombatFile), Report);
		Row.CheckKnown({ TEXT("min"), TEXT("max") });
		Row.GetFloat(TEXT("min"), Out.ResistMin, true);
		Row.GetFloat(TEXT("max"), Out.ResistMax, true);
		if (Out.ResistMin > 0.f || Out.ResistMax >= 1.f || Out.ResistMax <= Out.ResistMin)
		{
			Row.Error(TEXT("min muss höchstens 0 sein, max kleiner als 1 und größer als min"));
		}
	}
	if (const TSharedPtr<FJsonObject> Poise = Top.GetSubObject(TEXT("poise"), true))
	{
		FRow Row(Poise, FString::Printf(TEXT("%s, poise"), CombatFile), Report);
		Row.CheckKnown({ TEXT("k_player"), TEXT("stagger_s"), TEXT("boss_stagger_s"), TEXT("stagger_damage_mult"), TEXT("decay_delay_s"),
			TEXT("decay_fraction_per_s"), TEXT("player_max"), TEXT("player_stagger_s"), TEXT("player_regen_delay_s"), TEXT("player_regen_per_s") });
		Row.GetFloat(TEXT("k_player"), Out.PoiseKPlayer, true);
		Row.GetFloat(TEXT("stagger_s"), Out.StaggerSeconds, true);
		Row.GetFloat(TEXT("boss_stagger_s"), Out.BossStaggerSeconds, true);
		Row.GetFloat(TEXT("stagger_damage_mult"), Out.StaggerDamageMult, true);
		Row.GetFloat(TEXT("decay_delay_s"), Out.DecayDelaySeconds, true);
		Row.GetFloat(TEXT("decay_fraction_per_s"), Out.DecayFractionPerSecond, true);
		Row.GetFloat(TEXT("player_max"), Out.PlayerPoiseMax, true);
		Row.GetFloat(TEXT("player_stagger_s"), Out.PlayerStaggerSeconds, true);
		Row.GetFloat(TEXT("player_regen_delay_s"), Out.PlayerRegenDelaySeconds, true);
		Row.GetFloat(TEXT("player_regen_per_s"), Out.PlayerRegenPerSecond, true);
		if (Out.PoiseKPlayer <= 0.f || Out.StaggerSeconds <= 0.f || Out.BossStaggerSeconds <= 0.f || Out.PlayerPoiseMax <= 0.f)
		{
			Row.Error(TEXT("k_player, stagger_s, boss_stagger_s und player_max müssen größer als 0 sein"));
		}
		if (Out.StaggerDamageMult < 1.f)
		{
			Row.Error(TEXT("stagger_damage_mult darf nicht kleiner als 1 sein"));
		}
		if (Out.DecayFractionPerSecond < 0.f || Out.DecayFractionPerSecond > 1.f)
		{
			Row.Error(TEXT("decay_fraction_per_s muss zwischen 0 und 1 liegen"));
		}
	}
	Top.GetFloat(TEXT("weak_point_poise_mult"), Out.WeakPointPoiseMult, true);
	if (Out.WeakPointPoiseMult < 1.f)
	{
		Top.Error(TEXT("weak_point_poise_mult darf nicht kleiner als 1 sein"));
	}

	// Slots: shares sum to 1; every slot belongs to exactly one zone.
	if (const TSharedPtr<FJsonObject> Slots = Top.GetSubObject(TEXT("slots"), true))
	{
		float Sum = 0.f;
		for (const TPair<FString, TSharedPtr<FJsonValue>>& Pair : Slots->Values)
		{
			if (Pair.Key.StartsWith(TEXT("_")))
			{
				continue;
			}
			double Share = 0.0;
			if (!Pair.Value.IsValid() || !Pair.Value->TryGetNumber(Share) || Share <= 0.0)
			{
				Report.Errors.Add(FString::Printf(TEXT("%s, slots.%s: muss eine Zahl größer als 0 sein"), CombatFile, *Pair.Key));
				continue;
			}
			Out.SlotShare.Add(FName(*Pair.Key), static_cast<float>(Share));
			Sum += static_cast<float>(Share);
		}
		if (!FMath::IsNearlyEqual(Sum, 1.f, 0.001f))
		{
			Report.Errors.Add(FString::Printf(TEXT("%s, slots: die Anteile ergeben %s statt 1"), CombatFile, *FString::SanitizeFloat(Sum)));
		}
	}
	if (const TSharedPtr<FJsonObject> Zones = Top.GetSubObject(TEXT("zones"), true))
	{
		FRow Row(Zones, FString::Printf(TEXT("%s, zones"), CombatFile), Report);
		Row.CheckKnown({ TEXT("head"), TEXT("body"), TEXT("limbs") });
		ReadZone(Row, TEXT("head"), Out.Zones[static_cast<int32>(EGamerHitZone::Head)]);
		ReadZone(Row, TEXT("body"), Out.Zones[static_cast<int32>(EGamerHitZone::Body)]);
		ReadZone(Row, TEXT("limbs"), Out.Zones[static_cast<int32>(EGamerHitZone::Limbs)]);
		TMap<FName, int32> Uses;
		for (const FGamerZoneDef& Zone : Out.Zones)
		{
			for (const FName Slot : Zone.Slots)
			{
				Uses.FindOrAdd(Slot)++;
			}
		}
		for (const TPair<FName, float>& Pair : Out.SlotShare)
		{
			if (!Uses.Contains(Pair.Key))
			{
				Report.Errors.Add(FString::Printf(TEXT("%s: Slot %s gehört zu keiner Zone"), CombatFile, *Pair.Key.ToString()));
			}
		}
		for (const TPair<FName, int32>& Pair : Uses)
		{
			if (!Out.SlotShare.Contains(Pair.Key))
			{
				Report.Errors.Add(FString::Printf(TEXT("%s: Zone nennt den unbekannten Slot %s (slots)"), CombatFile, *Pair.Key.ToString()));
			}
			if (Pair.Value > 1)
			{
				Report.Errors.Add(FString::Printf(TEXT("%s: Slot %s steht in mehreren Zonen"), CombatFile, *Pair.Key.ToString()));
			}
		}
	}

	if (const TArray<TSharedPtr<FJsonValue>>* Attacks = Top.GetArray(TEXT("attacks"), true))
	{
		for (int32 Index = 0; Index < Attacks->Num(); ++Index)
		{
			const TSharedPtr<FJsonValue>& Value = (*Attacks)[Index];
			if (!Value.IsValid() || Value->Type != EJson::Object)
			{
				Report.Errors.Add(FString::Printf(TEXT("%s, attacks[%d]: muss ein Objekt { ... } sein"), CombatFile, Index + 1));
				continue;
			}
			FRow Row(Value->AsObject(), FString::Printf(TEXT("%s, attacks[%d]"), CombatFile, Index + 1), Report);
			Row.CheckKnown({ TEXT("id"), TEXT("damage"), TEXT("poise") });
			FGamerAttackDef Attack;
			FString Id;
			if (Row.GetString(TEXT("id"), Id, true))
			{
				Attack.Id = FName(*Id);
				if (!GamerJson::IsSnakeCase(Id)) { Row.Error(TEXT("id muss snake_case sein")); }
				if (Out.FindAttack(Attack.Id)) { Row.Error(TEXT("id kommt doppelt vor")); continue; }
			}
			Row.GetFloat(TEXT("damage"), Attack.Damage, true);
			Row.GetFloat(TEXT("poise"), Attack.Poise, true);
			if (Attack.Damage < 0.f || Attack.Poise < 0.f)
			{
				Row.Error(TEXT("damage und poise dürfen nicht negativ sein"));
			}
			Out.Attacks.Add(Attack);
		}
	}

	if (const TSharedPtr<FJsonObject> Spread = Top.GetSubObject(TEXT("spread"), true))
	{
		FRow Row(Spread, FString::Printf(TEXT("%s, spread"), CombatFile), Report);
		Row.CheckKnown({ TEXT("min"), TEXT("max"), TEXT("size_follow") });
		Row.GetFloat(TEXT("min"), Out.SpreadMin, true);
		Row.GetFloat(TEXT("max"), Out.SpreadMax, true);
		Row.GetFloat(TEXT("size_follow"), Out.SizeFollow, true);
		if (Out.SpreadMin <= 0.f || Out.SpreadMax < Out.SpreadMin)
		{
			Row.Error(TEXT("min muss größer als 0 sein und max darf nicht kleiner als min sein"));
		}
		if (Out.SizeFollow < 0.f || Out.SizeFollow > 1.f)
		{
			Row.Error(TEXT("size_follow muss zwischen 0 und 1 liegen"));
		}
	}

	if (const TArray<TSharedPtr<FJsonValue>>* Leaders = Top.GetArray(TEXT("leaders"), true))
	{
		float Previous = 0.f;
		for (int32 Index = 0; Index < Leaders->Num(); ++Index)
		{
			const TSharedPtr<FJsonValue>& Value = (*Leaders)[Index];
			if (!Value.IsValid() || Value->Type != EJson::Object)
			{
				Report.Errors.Add(FString::Printf(TEXT("%s, leaders[%d]: muss ein Objekt { ... } sein"), CombatFile, Index + 1));
				continue;
			}
			FRow Row(Value->AsObject(), FString::Printf(TEXT("%s, leaders[%d]"), CombatFile, Index + 1), Report);
			Row.CheckKnown({ TEXT("id"), TEXT("min_rank"), TEXT("chance"), TEXT("place_chance"), TEXT("hp"), TEXT("poise"), TEXT("dmg"), TEXT("size"),
				TEXT("extra_base_rolls"), TEXT("extra_rank_rolls"), TEXT("rank_roll_mult") });
			FGamerLeaderDef Leader;
			FString Id;
			if (Row.GetString(TEXT("id"), Id, true)) { Leader.Id = FName(*Id); }
			Row.GetInt(TEXT("min_rank"), Leader.MinRank, true);
			Row.GetFloat(TEXT("chance"), Leader.Chance, true);
			Row.GetFloat(TEXT("place_chance"), Leader.PlaceChance, true);
			Row.GetFloat(TEXT("hp"), Leader.Hp, true);
			Row.GetFloat(TEXT("poise"), Leader.Poise, true);
			Row.GetFloat(TEXT("dmg"), Leader.Dmg, true);
			Row.GetFloat(TEXT("size"), Leader.Size, true);
			Row.GetInt(TEXT("extra_base_rolls"), Leader.ExtraBaseRolls, true);
			Row.GetInt(TEXT("extra_rank_rolls"), Leader.ExtraRankRolls, true);
			Row.GetInt(TEXT("rank_roll_mult"), Leader.RankRollMult, true);
			if (Leader.MinRank < 1 || Leader.MinRank > GamerData::MaxTier) { Row.Error(TEXT("min_rank muss zwischen 1 und 6 liegen")); }
			if (Leader.Chance < 0.f || Leader.Chance > 1.f || Leader.PlaceChance < 0.f || Leader.PlaceChance > 1.f)
			{
				Row.Error(TEXT("chance und place_chance müssen zwischen 0 und 1 liegen"));
			}
			if (Leader.Hp < 1.f || Leader.Poise < 1.f || Leader.Dmg < 1.f || Leader.Size < 1.f)
			{
				Row.Error(TEXT("hp, poise, dmg und size dürfen nicht kleiner als 1 sein (Anführer sind nie schwächer)"));
			}
			if (Leader.Hp < Previous)
			{
				Row.Error(TEXT("die Stufen müssen nach hp aufsteigend sortiert sein"));
			}
			if (Leader.ExtraBaseRolls < 0 || Leader.ExtraRankRolls < 0 || Leader.RankRollMult < 1)
			{
				Row.Error(TEXT("extra_base_rolls und extra_rank_rolls dürfen nicht negativ sein, rank_roll_mult mindestens 1"));
			}
			Previous = Leader.Hp;
			Out.Leaders.Add(Leader);
		}
	}

	if (const TSharedPtr<FJsonObject> Loot = Top.GetSubObject(TEXT("loot"), true))
	{
		FRow Row(Loot, FString::Printf(TEXT("%s, loot"), CombatFile), Report);
		Row.CheckKnown({ TEXT("count_bonus_cap"), TEXT("downscale") });
		Row.GetInt(TEXT("count_bonus_cap"), Out.LootCountBonusCap, true);
		if (Out.LootCountBonusCap < 0) { Row.Error(TEXT("count_bonus_cap darf nicht negativ sein")); }
		if (const TSharedPtr<FJsonObject> Down = Row.GetSubObject(TEXT("downscale"), true))
		{
			FRow DownRow(Down, FString::Printf(TEXT("%s, loot.downscale"), CombatFile), Report);
			DownRow.CheckKnown({ TEXT("delta1"), TEXT("delta2"), TEXT("delta3") });
			DownRow.GetFloat(TEXT("delta1"), Out.LootDownscale1, true);
			DownRow.GetFloat(TEXT("delta2"), Out.LootDownscale2, true);
			DownRow.GetFloat(TEXT("delta3"), Out.LootDownscale3, true);
			if (Out.LootDownscale1 > 1.f || Out.LootDownscale2 > Out.LootDownscale1 || Out.LootDownscale3 > Out.LootDownscale2 || Out.LootDownscale3 <= 0.f)
			{
				DownRow.Error(TEXT("die Dämpfung muss mit dem Abstand sinken: 1 >= delta1 >= delta2 >= delta3 > 0"));
			}
		}
	}

	Top.GetFloat(TEXT("creature_hp_scale"), Out.CreatureHpScale, true);
	if (Out.CreatureHpScale <= 0.f)
	{
		Top.Error(TEXT("creature_hp_scale muss größer als 0 sein"));
	}
	if (const TSharedPtr<FJsonObject> Hp = Top.GetSubObject(TEXT("player_hp"), true))
	{
		FRow Row(Hp, FString::Printf(TEXT("%s, player_hp"), CombatFile), Report);
		Row.CheckKnown({ TEXT("base"), TEXT("per_toughness"), TEXT("toughness_neutral") });
		Row.GetFloat(TEXT("base"), Out.PlayerBaseHp, true);
		Row.GetFloat(TEXT("per_toughness"), Out.PlayerHpPerToughness, true);
		Row.GetInt(TEXT("toughness_neutral"), Out.ToughnessNeutral, true);
		if (Out.PlayerBaseHp <= 0.f) { Row.Error(TEXT("base muss größer als 0 sein")); }
	}

	return Report.Errors.Num() == ErrorsBefore;
}
