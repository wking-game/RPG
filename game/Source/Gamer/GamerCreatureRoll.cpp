#include "GamerCreatureRoll.h"

namespace
{
	void AddRolled(TArray<FGamerRolledItem>& Out, FName Item, int32 Count, bool bFromBiome)
	{
		if (Item.IsNone() || Count <= 0)
		{
			return;
		}
		for (FGamerRolledItem& Existing : Out)
		{
			if (Existing.Item == Item && Existing.bFromBiome == bFromBiome)
			{
				Existing.Count += Count;
				return;
			}
		}
		Out.Add({ Item, Count, bFromBiome });
	}

	/** One pass over a base or rank list. */
	void RollList(const TArray<FGamerLootEntry>& Entries, int32 EffectiveRank, int32 CountMult, float YieldFactor, const FGamerCombatData& Combat,
		FRandomStream& Stream, TArray<FGamerRolledItem>& Out)
	{
		for (const FGamerLootEntry& Entry : Entries)
		{
			if (EffectiveRank < Entry.MinRank)
			{
				continue; // rank lock
			}
			if (Stream.FRand() >= Entry.Chance)
			{
				continue;
			}
			int32 Count = Stream.RandRange(Entry.Min, Entry.Max);
			if (Entry.bScaleCount)
			{
				Count += FMath::Clamp(EffectiveRank - Entry.MinRank, 0, Combat.LootCountBonusCap);
			}
			Count = FMath::Max(1, FMath::RoundToInt(static_cast<float>(Count * CountMult) * YieldFactor));
			AddRolled(Out, Entry.Resolve(EffectiveRank), Count, false);
		}
	}
}

float GamerCreature::BiomeLootDownscale(int32 RankDifference, const FGamerCombatData& Combat)
{
	if (RankDifference <= 0) { return 1.f; }
	if (RankDifference == 1) { return Combat.LootDownscale1; }
	if (RankDifference == 2) { return Combat.LootDownscale2; }
	return Combat.LootDownscale3;
}

const FGamerSpawnRule* GamerCreature::PickSpawnRule(const FGamerCreatureDataSet& Set, FName Biome, bool bAtPlace, FRandomStream& Stream)
{
	float Total = 0.f;
	for (const FGamerSpawnRule& Rule : Set.Spawns)
	{
		if (Rule.Biome == Biome && (!Rule.bOnlyPlaces || bAtPlace) && Set.Creatures.Contains(Rule.Creature))
		{
			Total += Rule.Weight;
		}
	}
	if (Total <= 0.f)
	{
		return nullptr;
	}
	float Pick = Stream.FRand() * Total;
	const FGamerSpawnRule* Last = nullptr;
	for (const FGamerSpawnRule& Rule : Set.Spawns)
	{
		if (Rule.Biome == Biome && (!Rule.bOnlyPlaces || bAtPlace) && Set.Creatures.Contains(Rule.Creature))
		{
			Last = &Rule;
			if (Pick < Rule.Weight)
			{
				return &Rule;
			}
			Pick -= Rule.Weight;
		}
	}
	return Last; // rounding at the upper end
}

int32 GamerCreature::RollRank(const FGamerCreatureDef& Def, const FGamerBiomeDef& Biome, const FGamerSpawnRule& Rule, bool bAtPlace, FRandomStream& Stream)
{
	if (!Rule.bScaleToZone)
	{
		return Rule.FixedRank;
	}
	const int32 Zone = bAtPlace ? Biome.PlaceRank : Biome.Rank;
	const int32 Top = Rule.MaxRank > 0 ? FMath::Min(Zone, Rule.MaxRank) : Zone;
	if (Rule.ScaleChance >= 1.f || Stream.FRand() < Rule.ScaleChance)
	{
		return Top;
	}
	return FMath::Max(Def.MinRank, Top - 1);
}

int32 GamerCreature::RollLeader(const FGamerCombatData& Combat, int32 Rank, bool bAtPlace, FRandomStream& Stream)
{
	const float U = Stream.FRand();
	float Accumulated = 0.f;
	for (int32 Index = Combat.Leaders.Num() - 1; Index >= 0; --Index)
	{
		const FGamerLeaderDef& Leader = Combat.Leaders[Index];
		if (Rank < Leader.MinRank)
		{
			continue;
		}
		Accumulated += bAtPlace ? Leader.PlaceChance : Leader.Chance;
		if (U < Accumulated)
		{
			return Index;
		}
	}
	return -1;
}

bool GamerCreature::RollSpawn(const FGamerCreatureDataSet& Set, const FGamerCombatData& Combat, FName Biome, bool bAtPlace, FRandomStream& Stream, FGamerSpawnRoll& Out)
{
	const FGamerSpawnRule* Rule = PickSpawnRule(Set, Biome, bAtPlace, Stream);
	const FGamerBiomeDef* BiomeDef = Set.FindBiome(Biome);
	const FGamerCreatureDef* Def = Rule ? Set.Creatures.Find(Rule->Creature) : nullptr;
	if (!Rule || !BiomeDef || !Def)
	{
		return false;
	}
	Out = FGamerSpawnRoll();
	Out.Creature = Def->Id;
	Out.Rank = RollRank(*Def, *BiomeDef, *Rule, bAtPlace, Stream);
	Out.Roll = Stream.FRandRange(Combat.SpreadMin, Combat.SpreadMax);
	Out.LeaderIndex = RollLeader(Combat, Out.Rank, bAtPlace, Stream);

	const FGamerRankStats* Stats = Def->FindRank(Out.Rank);
	if (!Stats)
	{
		return false; // the data check reports this; do not spawn something without values
	}
	const FGamerLeaderDef* Leader = Combat.Leaders.IsValidIndex(Out.LeaderIndex) ? &Combat.Leaders[Out.LeaderIndex] : nullptr;
	Out.Hp = GamerCombat::CreatureHp(Stats->Hp, Out.Roll, Leader ? Leader->Hp : 1.f, Combat);
	Out.Poise = Stats->Poise * Out.Roll * (Leader ? Leader->Poise : 1.f);
	Out.Damage = Stats->Damage * Out.Roll * (Leader ? Leader->Dmg : 1.f);
	Out.Size = (1.f + (Out.Roll - 1.f) * Combat.SizeFollow) * (Leader ? Leader->Size : 1.f);
	return true;
}

TArray<FGamerRolledItem> GamerCreature::RollLoot(const FGamerCreatureLoot& Loot, int32 EffectiveRank, int32 ZoneRank, FName BiomeId,
	int32 LeaderIndex, float YieldFactor, const FGamerCombatData& Combat, FRandomStream& Stream)
{
	TArray<FGamerRolledItem> Result;
	const FGamerLeaderDef* Leader = Combat.Leaders.IsValidIndex(LeaderIndex) ? &Combat.Leaders[LeaderIndex] : nullptr;
	const int32 BasePasses = 1 + (Leader ? Leader->ExtraBaseRolls : 0);
	const int32 RankPasses = 1 + (Leader ? Leader->ExtraRankRolls : 0);
	const int32 RankMult = Leader ? FMath::Max(1, Leader->RankRollMult) : 1;

	for (int32 Pass = 0; Pass < BasePasses; ++Pass)
	{
		RollList(Loot.Base, EffectiveRank, 1, YieldFactor, Combat, Stream, Result);
	}
	for (int32 Pass = 0; Pass < RankPasses; ++Pass)
	{
		RollList(Loot.Rank, EffectiveRank, RankMult, YieldFactor, Combat, Stream, Result);
	}

	// Biome bonus loot: only in the matching biome; the chance drops for stragglers (below the rank of the place).
	if (!BiomeId.IsNone())
	{
		const float Downscale = BiomeLootDownscale(ZoneRank - EffectiveRank, Combat);
		for (const FGamerLootEntry& Entry : Loot.Biome)
		{
			if (!Entry.Biomes.Contains(BiomeId))
			{
				continue;
			}
			if (Stream.FRand() >= Entry.Chance * Downscale)
			{
				continue;
			}
			const int32 Count = FMath::Max(1, FMath::RoundToInt(static_cast<float>(Stream.RandRange(Entry.Min, Entry.Max)) * YieldFactor));
			AddRolled(Result, Entry.Resolve(EffectiveRank), Count, true);
		}
	}
	return Result;
}
