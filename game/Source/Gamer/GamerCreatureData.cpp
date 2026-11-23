#include "GamerCreatureData.h"

#include "GamerDataLoader.h"
#include "GamerJsonTable.h"

// Validation messages are German on purpose: they are read by whoever edits the JSON files.

namespace
{
	using GamerJson::FRow;
	using GamerJson::FRowSource;

	bool ParseTemperament(const FString& Text, EGamerTemperament& Out)
	{
		if (Text == TEXT("passive")) { Out = EGamerTemperament::Passive; return true; }
		if (Text == TEXT("defensive")) { Out = EGamerTemperament::Defensive; return true; }
		if (Text == TEXT("territorial")) { Out = EGamerTemperament::Territorial; return true; }
		if (Text == TEXT("hostile")) { Out = EGamerTemperament::Hostile; return true; }
		if (Text == TEXT("ambush")) { Out = EGamerTemperament::Ambush; return true; }
		return false;
	}

	bool CheckRank(FRow& Row, const TCHAR* Field, int32 Value)
	{
		if (Value < 1 || Value > GamerData::MaxTier)
		{
			Row.Error(FString::Printf(TEXT("%s muss zwischen 1 und %d liegen"), Field, GamerData::MaxTier));
			return false;
		}
		return true;
	}

	/** Reads "id", checks prefix and format, and adds the ID to the row context. */
	bool ReadId(FRow& Row, const TCHAR* Prefix, FName& OutId)
	{
		FString Id;
		if (!Row.GetString(TEXT("id"), Id, true))
		{
			return false;
		}
		Row.SetContext(FString::Printf(TEXT("%s (%s)"), *Row.GetContext(), *Id));
		if (!GamerJson::IsSnakeCase(Id) || !Id.StartsWith(Prefix))
		{
			Row.Error(FString::Printf(TEXT("id muss snake_case sein und mit %s beginnen"), Prefix));
			return false;
		}
		OutId = FName(*Id);
		return true;
	}

	void ParseBiomes(const TSharedPtr<FJsonObject>& Root, FGamerCreatureDataSet& Out, FGamerDataReport& Report)
	{
		const TArray<TSharedPtr<FJsonValue>>* List = nullptr;
		if (!Root.IsValid() || !Root->TryGetArrayField(TEXT("biomes"), List) || !List || List->Num() == 0)
		{
			Report.Errors.Add(FString::Printf(TEXT("%s: Liste \"biomes\" fehlt oder ist leer"), GamerData::SpawnsFile));
			return;
		}
		for (int32 Index = 0; Index < List->Num(); ++Index)
		{
			const TSharedPtr<FJsonValue>& Value = (*List)[Index];
			if (!Value.IsValid() || Value->Type != EJson::Object)
			{
				Report.Errors.Add(FString::Printf(TEXT("%s, biomes[%d]: ist kein Objekt { ... }"), GamerData::SpawnsFile, Index + 1));
				continue;
			}
			FRow Row(Value->AsObject(), FString::Printf(TEXT("%s, biomes[%d]"), GamerData::SpawnsFile, Index + 1), Report);
			Row.CheckKnown({ TEXT("id"), TEXT("name"), TEXT("rank"), TEXT("place_rank"), TEXT("places") });
			FGamerBiomeDef Def;
			if (!ReadId(Row, TEXT("biome_"), Def.Id))
			{
				continue;
			}
			Row.GetString(TEXT("name"), Def.Name, true);
			Row.GetString(TEXT("places"), Def.Places);
			if (Row.GetInt(TEXT("rank"), Def.Rank, true))
			{
				CheckRank(Row, TEXT("rank"), Def.Rank);
			}
			if (Row.GetInt(TEXT("place_rank"), Def.PlaceRank, true) && CheckRank(Row, TEXT("place_rank"), Def.PlaceRank) && Def.PlaceRank < Def.Rank)
			{
				Row.Error(TEXT("place_rank darf nicht kleiner als rank sein"));
			}
			if (Out.FindBiome(Def.Id))
			{
				Row.Error(TEXT("Biom-ID kommt doppelt vor"));
				continue;
			}
			Out.Biomes.Add(MoveTemp(Def));
		}
	}

	void ParseCreatures(const TArray<FRowSource>& Rows, bool bAnimals, FGamerCreatureDataSet& Out, FGamerDataReport& Report)
	{
		for (const FRowSource& Source : Rows)
		{
			FRow Row(Source.Object, Source.Context, Report);
			Row.CheckKnown({ TEXT("id"), TEXT("name"), TEXT("gemini_name"), TEXT("role"), TEXT("kind"), TEXT("temperament"), TEXT("pack"),
				TEXT("min_rank"), TEXT("home_biome"), TEXT("wave"), TEXT("ranks") });

			FGamerCreatureDef Def;
			Def.bAnimal = bAnimals;
			if (!ReadId(Row, bAnimals ? TEXT("animal_") : TEXT("creature_"), Def.Id))
			{
				continue;
			}
			Row.GetString(TEXT("name"), Def.Name, true);
			Row.GetString(TEXT("gemini_name"), Def.GeminiName);
			Row.GetString(TEXT("role"), Def.Role);
			Row.GetBool(TEXT("pack"), Def.bPack);

			FString Kind;
			if (Row.GetString(TEXT("kind"), Kind, true) && Kind != (bAnimals ? TEXT("animal") : TEXT("creature")))
			{
				Row.Error(FString::Printf(TEXT("kind muss \"%s\" sein"), bAnimals ? TEXT("animal") : TEXT("creature")));
			}
			FString Temperament;
			if (Row.GetString(TEXT("temperament"), Temperament, true) && !ParseTemperament(Temperament, Def.Temperament))
			{
				Row.Error(FString::Printf(TEXT("temperament \"%s\" unbekannt (erlaubt: passive, defensive, territorial, hostile, ambush)"), *Temperament));
			}
			if (Row.GetInt(TEXT("min_rank"), Def.MinRank, true))
			{
				CheckRank(Row, TEXT("min_rank"), Def.MinRank);
			}
			FString Home;
			if (Row.GetString(TEXT("home_biome"), Home, true))
			{
				Def.HomeBiome = FName(*Home);
				if (!Out.FindBiome(Def.HomeBiome))
				{
					Row.Error(FString::Printf(TEXT("home_biome \"%s\" steht nicht in der Biom-Liste (spawns.json)"), *Home));
				}
			}
			if (Row.GetInt(TEXT("wave"), Def.Wave, true) && (Def.Wave < 0 || Def.Wave > 5))
			{
				Row.Error(TEXT("wave muss zwischen 0 und 5 liegen"));
			}

			if (const TArray<TSharedPtr<FJsonValue>>* Ranks = Row.GetArray(TEXT("ranks"), true))
			{
				if (Ranks->Num() == 0)
				{
					Row.Error(TEXT("ranks darf nicht leer sein"));
				}
				for (int32 Index = 0; Index < Ranks->Num(); ++Index)
				{
					const TSharedPtr<FJsonValue>& Value = (*Ranks)[Index];
					if (!Value.IsValid() || Value->Type != EJson::Object)
					{
						Row.Error(FString::Printf(TEXT("ranks[%d]: muss ein Objekt { ... } sein"), Index + 1));
						continue;
					}
					FRow RankRow(Value->AsObject(), FString::Printf(TEXT("%s, ranks[%d]"), *Row.GetContext(), Index + 1), Report);
					RankRow.CheckKnown({ TEXT("rank"), TEXT("hp"), TEXT("poise"), TEXT("dmg") });
					FGamerRankStats Stats;
					RankRow.GetInt(TEXT("rank"), Stats.Rank, true);
					CheckRank(RankRow, TEXT("rank"), Stats.Rank);
					RankRow.GetFloat(TEXT("hp"), Stats.Hp, true);
					RankRow.GetFloat(TEXT("poise"), Stats.Poise, true);
					RankRow.GetFloat(TEXT("dmg"), Stats.Damage, true);
					if (Stats.Hp <= 0.f)
					{
						RankRow.Error(TEXT("hp muss größer als 0 sein"));
					}
					if (Stats.Poise < 0.f || Stats.Damage < 0.f)
					{
						RankRow.Error(TEXT("poise und dmg dürfen nicht negativ sein"));
					}
					if (Def.FindRank(Stats.Rank))
					{
						RankRow.Error(TEXT("Rang kommt doppelt vor"));
						continue;
					}
					Def.Ranks.Add(Stats);
				}
				Def.Ranks.Sort([](const FGamerRankStats& A, const FGamerRankStats& B) { return A.Rank < B.Rank; });
				if (Def.Ranks.Num() > 0 && Def.Ranks[0].Rank != Def.MinRank)
				{
					Row.Warning(FString::Printf(TEXT("niedrigster Rang in ranks (%d) ist nicht min_rank (%d)"), Def.Ranks[0].Rank, Def.MinRank));
				}
			}

			if (Out.Creatures.Contains(Def.Id))
			{
				Row.Error(TEXT("ID kommt doppelt vor (auch zwischen creatures.json und animals.json)"));
				continue;
			}
			Out.Creatures.Add(Def.Id, MoveTemp(Def));
		}
	}

	void ParseSpawns(const TArray<FRowSource>& Rows, FGamerCreatureDataSet& Out, FGamerDataReport& Report)
	{
		for (const FRowSource& Source : Rows)
		{
			FRow Row(Source.Object, Source.Context, Report);
			Row.CheckKnown({ TEXT("biome"), TEXT("creature"), TEXT("weight"), TEXT("only_places"), TEXT("scale_to_zone"), TEXT("scale_chance"),
				TEXT("max_rank"), TEXT("fixed_rank") });
			FGamerSpawnRule Rule;
			FString Text;
			if (Row.GetString(TEXT("biome"), Text, true))
			{
				Rule.Biome = FName(*Text);
				Row.SetContext(FString::Printf(TEXT("%s (%s"), *Row.GetContext(), *Text));
			}
			if (Row.GetString(TEXT("creature"), Text, true))
			{
				Rule.Creature = FName(*Text);
				Row.SetContext(FString::Printf(TEXT("%s, %s)"), *Row.GetContext(), *Text));
			}
			if (!Out.FindBiome(Rule.Biome))
			{
				Row.Error(TEXT("Biom steht nicht in der Biom-Liste"));
			}
			if (!Out.Creatures.Contains(Rule.Creature))
			{
				Row.Error(TEXT("Kreatur oder Tier unbekannt (creatures.json, animals.json)"));
			}
			if (Row.GetFloat(TEXT("weight"), Rule.Weight, true) && Rule.Weight <= 0.f)
			{
				Row.Error(TEXT("weight muss größer als 0 sein"));
			}
			Row.GetBool(TEXT("only_places"), Rule.bOnlyPlaces, true);
			Row.GetBool(TEXT("scale_to_zone"), Rule.bScaleToZone, true);
			Row.GetFloat(TEXT("scale_chance"), Rule.ScaleChance, true);
			Row.GetInt(TEXT("max_rank"), Rule.MaxRank, true);
			Row.GetInt(TEXT("fixed_rank"), Rule.FixedRank, true);
			if (Rule.bScaleToZone && (Rule.ScaleChance <= 0.f || Rule.ScaleChance > 1.f))
			{
				Row.Error(TEXT("scale_chance muss bei scale_to_zone größer als 0 und höchstens 1 sein"));
			}
			if (!Rule.bScaleToZone && (Rule.FixedRank < 1 || Rule.FixedRank > GamerData::MaxTier))
			{
				Row.Error(TEXT("fixed_rank muss ohne scale_to_zone zwischen 1 und 6 liegen"));
			}
			if (Rule.MaxRank < 0 || Rule.MaxRank > GamerData::MaxTier)
			{
				Row.Error(TEXT("max_rank muss zwischen 0 und 6 liegen (0 = keine Obergrenze)"));
			}
			Out.Spawns.Add(Rule);
		}
	}

	/** Cross checks that need creatures, biomes and spawn rules together. */
	void CheckSpawns(const FGamerCreatureDataSet& Set, FGamerDataReport& Report)
	{
		TSet<FName> Spawning;
		for (const FGamerSpawnRule& Rule : Set.Spawns)
		{
			const FGamerCreatureDef* Def = Set.Creatures.Find(Rule.Creature);
			const FGamerBiomeDef* Biome = Set.FindBiome(Rule.Biome);
			if (!Def || !Biome)
			{
				continue; // reported when the rule was read
			}
			Spawning.Add(Rule.Creature);
			const FString Where = FString::Printf(TEXT("%s (%s in %s)"), GamerData::SpawnsFile, *Rule.Creature.ToString(), *Rule.Biome.ToString());

			// A creature never appears before its own wave: its home biome has the lowest rank it appears in.
			if (const FGamerBiomeDef* Home = Set.FindBiome(Def->HomeBiome))
			{
				if (Biome->Rank < Home->Rank)
				{
					Report.Errors.Add(FString::Printf(TEXT("%s: kommt vor seinem Heimat-Biom %s vor (Biom-Rang %d, Heimat-Rang %d)"),
						*Where, *Def->HomeBiome.ToString(), Biome->Rank, Home->Rank));
				}
			}
			const TSet<int32> Ranks = Set.ReachableRanks(Rule.Creature, Rule.Biome);
			for (const int32 Rank : Ranks)
			{
				if (Rank < Def->MinRank)
				{
					Report.Errors.Add(FString::Printf(TEXT("%s: Rang %d liegt unter min_rank %d"), *Where, Rank, Def->MinRank));
				}
				if (!Def->FindRank(Rank))
				{
					Report.Errors.Add(FString::Printf(TEXT("%s: keine Werte für Rang %d in %s"), *Where, Rank, Def->bAnimal ? GamerData::AnimalsFile : GamerData::CreaturesFile));
				}
			}
			if (Rule.bScaleToZone && Rule.MaxRank > 0 && Rule.MaxRank < Def->MinRank)
			{
				Report.Errors.Add(FString::Printf(TEXT("%s: max_rank %d liegt unter min_rank %d"), *Where, Rule.MaxRank, Def->MinRank));
			}
		}
		for (const TPair<FName, FGamerCreatureDef>& Pair : Set.Creatures)
		{
			if (!Spawning.Contains(Pair.Key))
			{
				Report.Errors.Add(FString::Printf(TEXT("%s: %s kommt in keinem Biom vor (spawns.json)"),
					Pair.Value.bAnimal ? GamerData::AnimalsFile : GamerData::CreaturesFile, *Pair.Key.ToString()));
			}
		}
	}

	void ParseLootList(FRow& Owner, const TCHAR* Field, bool bBiome, TArray<FGamerLootEntry>& Out)
	{
		const TArray<TSharedPtr<FJsonValue>>* List = Owner.GetArray(Field, true);
		if (!List)
		{
			return;
		}
		for (int32 Index = 0; Index < List->Num(); ++Index)
		{
			const TSharedPtr<FJsonValue>& Value = (*List)[Index];
			if (!Value.IsValid() || Value->Type != EJson::Object)
			{
				Owner.Error(FString::Printf(TEXT("%s[%d]: muss ein Objekt { ... } sein"), Field, Index + 1));
				continue;
			}
			FRow Row(Value->AsObject(), FString::Printf(TEXT("%s, %s[%d]"), *Owner.GetContext(), Field, Index + 1), Owner.GetReport());
			Row.CheckKnown({ TEXT("item"), TEXT("item_family"), TEXT("min"), TEXT("max"), TEXT("chance"), TEXT("min_rank"), TEXT("scale_count"), TEXT("biomes") });

			FGamerLootEntry Entry;
			FString Item, Family;
			const bool bHasItem = Row.GetString(TEXT("item"), Item);
			const bool bHasFamily = Row.GetString(TEXT("item_family"), Family);
			if (bHasItem == bHasFamily)
			{
				Row.Error(TEXT("genau eines von \"item\" und \"item_family\" muss gesetzt sein"));
			}
			Entry.Item = FName(*Item);
			Entry.ItemFamily = FName(*Family);
			Row.GetInt(TEXT("min"), Entry.Min, true);
			Row.GetInt(TEXT("max"), Entry.Max, true);
			if (Entry.Min < 1 || Entry.Max < Entry.Min)
			{
				Row.Error(TEXT("min muss mindestens 1 sein und max darf nicht kleiner als min sein"));
			}
			if (Row.GetFloat(TEXT("chance"), Entry.Chance, true) && (Entry.Chance <= 0.f || Entry.Chance > 1.f))
			{
				Row.Error(TEXT("chance muss größer als 0 und höchstens 1 sein"));
			}
			if (bBiome)
			{
				if (Row.Has(TEXT("min_rank")) || Row.Has(TEXT("scale_count")))
				{
					Row.Error(TEXT("Biom-Zeilen haben kein min_rank und kein scale_count"));
				}
				if (const TArray<TSharedPtr<FJsonValue>>* Biomes = Row.GetArray(TEXT("biomes"), true))
				{
					for (const TSharedPtr<FJsonValue>& Biome : *Biomes)
					{
						FString BiomeId;
						if (Biome.IsValid() && Biome->TryGetString(BiomeId))
						{
							Entry.Biomes.Add(FName(*BiomeId));
						}
						else
						{
							Row.Error(TEXT("biomes darf nur Texte enthalten"));
						}
					}
					if (Entry.Biomes.Num() == 0)
					{
						Row.Error(TEXT("biomes darf nicht leer sein"));
					}
				}
			}
			else
			{
				if (Row.Has(TEXT("biomes")))
				{
					Row.Error(TEXT("\"biomes\" nur in Biom-Zeilen erlaubt"));
				}
				if (Row.GetInt(TEXT("min_rank"), Entry.MinRank, true))
				{
					CheckRank(Row, TEXT("min_rank"), Entry.MinRank);
				}
				Row.GetBool(TEXT("scale_count"), Entry.bScaleCount, true);
			}
			Out.Add(MoveTemp(Entry));
		}
	}

	/** Items must exist for every rank the line can roll at. */
	void CheckLootEntry(const FGamerLootEntry& Entry, const FGamerCreatureDef& Def, const FGamerCreatureDataSet& Set, const FGamerDataSet& Base,
		bool bBiome, const FString& Where, FGamerDataReport& Report)
	{
		TSet<int32> Ranks;
		if (bBiome)
		{
			for (const FName Biome : Entry.Biomes)
			{
				if (!Set.FindBiome(Biome))
				{
					Report.Errors.Add(FString::Printf(TEXT("%s: Biom %s unbekannt"), *Where, *Biome.ToString()));
					continue;
				}
				const TSet<int32> InBiome = Set.ReachableRanks(Def.Id, Biome);
				if (InBiome.Num() == 0)
				{
					Report.Errors.Add(FString::Printf(TEXT("%s: %s kommt in %s nicht vor, diese Biom-Beute würde nie fallen"), *Where, *Def.Id.ToString(), *Biome.ToString()));
				}
				Ranks.Append(InBiome);
			}
		}
		else
		{
			for (const int32 Rank : Set.ReachableRanks(Def.Id))
			{
				if (Rank >= Entry.MinRank)
				{
					Ranks.Add(Rank);
				}
			}
			if (Ranks.Num() == 0)
			{
				Report.Errors.Add(FString::Printf(TEXT("%s: ab Rang %d wird nie erreicht (%s)"), *Where, Entry.MinRank, *Def.Id.ToString()));
			}
		}
		if (Entry.ItemFamily.IsNone())
		{
			if (!Base.Items.Contains(Entry.Item))
			{
				Report.Errors.Add(FString::Printf(TEXT("%s: Gegenstand %s unbekannt (items.json, items_creature.json)"), *Where, *Entry.Item.ToString()));
			}
			return;
		}
		for (const int32 Rank : Ranks)
		{
			const FName Item = Entry.Resolve(Rank);
			if (!Base.Items.Contains(Item))
			{
				Report.Errors.Add(FString::Printf(TEXT("%s: Gegenstand %s fehlt (Stufe %d von %s)"), *Where, *Item.ToString(), Rank, *Entry.ItemFamily.ToString()));
			}
		}
	}

	void ParseLoot(const TArray<FRowSource>& Rows, const FGamerDataSet& Base, FGamerCreatureDataSet& Out, FGamerDataReport& Report)
	{
		for (const FRowSource& Source : Rows)
		{
			FRow Row(Source.Object, Source.Context, Report);
			Row.CheckKnown({ TEXT("id"), TEXT("base"), TEXT("rank"), TEXT("biome") });
			FString Id;
			if (!Row.GetString(TEXT("id"), Id, true))
			{
				continue;
			}
			Row.SetContext(FString::Printf(TEXT("%s (%s)"), *Row.GetContext(), *Id));
			FGamerCreatureLoot Loot;
			Loot.Id = FName(*Id);
			const FGamerCreatureDef* Def = Out.Creatures.Find(Loot.Id);
			if (!Def)
			{
				Row.Error(TEXT("Kreatur oder Tier unbekannt (creatures.json, animals.json)"));
				continue;
			}
			ParseLootList(Row, TEXT("base"), false, Loot.Base);
			ParseLootList(Row, TEXT("rank"), false, Loot.Rank);
			ParseLootList(Row, TEXT("biome"), true, Loot.Biome);
			if (Out.Loot.Contains(Loot.Id))
			{
				Row.Error(TEXT("Beute für diese ID steht doppelt in der Datei"));
				continue;
			}
			if (Loot.Base.Num() == 0)
			{
				Row.Warning(TEXT("keine Grund-Beute (base)"));
			}
			for (const FGamerLootEntry& E : Loot.Base) { CheckLootEntry(E, *Def, Out, Base, false, Row.GetContext() + TEXT(", base"), Report); }
			for (const FGamerLootEntry& E : Loot.Rank) { CheckLootEntry(E, *Def, Out, Base, false, Row.GetContext() + TEXT(", rank"), Report); }
			for (const FGamerLootEntry& E : Loot.Biome) { CheckLootEntry(E, *Def, Out, Base, true, Row.GetContext() + TEXT(", biome"), Report); }
			Out.Loot.Add(Loot.Id, MoveTemp(Loot));
		}
		for (const TPair<FName, FGamerCreatureDef>& Pair : Out.Creatures)
		{
			if (!Out.Loot.Contains(Pair.Key))
			{
				Report.Errors.Add(FString::Printf(TEXT("%s: keine Beute für %s"), GamerData::CreatureLootFile, *Pair.Key.ToString()));
			}
		}
	}

	void AddRanksOfRule(const FGamerCreatureDef& Def, const FGamerBiomeDef& Biome, const FGamerSpawnRule& Rule, TSet<int32>& Out)
	{
		if (!Rule.bScaleToZone)
		{
			Out.Add(Rule.FixedRank);
			return;
		}
		TArray<int32, TInlineAllocator<2>> Zones;
		if (!Rule.bOnlyPlaces)
		{
			Zones.Add(Biome.Rank);
		}
		Zones.Add(Biome.PlaceRank);
		for (const int32 Zone : Zones)
		{
			const int32 Top = Rule.MaxRank > 0 ? FMath::Min(Zone, Rule.MaxRank) : Zone;
			Out.Add(Top);
			if (Rule.ScaleChance < 1.f)
			{
				Out.Add(FMath::Max(Def.MinRank, Top - 1));
			}
		}
	}
}

TSet<int32> FGamerCreatureDataSet::ReachableRanks(FName CreatureId, FName BiomeId) const
{
	TSet<int32> Result;
	const FGamerCreatureDef* Def = Creatures.Find(CreatureId);
	if (!Def)
	{
		return Result;
	}
	for (const FGamerSpawnRule& Rule : Spawns)
	{
		if (Rule.Creature != CreatureId || (!BiomeId.IsNone() && Rule.Biome != BiomeId))
		{
			continue;
		}
		if (const FGamerBiomeDef* Biome = FindBiome(Rule.Biome))
		{
			AddRanksOfRule(*Def, *Biome, Rule, Result);
		}
	}
	return Result;
}

bool GamerData::LoadCreatures(const FString& Directory, const FGamerDataSet& Base, FGamerCreatureDataSet& Out, FGamerDataReport& Report, bool bRequired)
{
	Out = FGamerCreatureDataSet();
	if (!bRequired && !FPaths::FileExists(FPaths::Combine(Directory, CreaturesFile)))
	{
		return true; // data set without creatures (e.g. test data)
	}
	const int32 ErrorsBefore = Report.Errors.Num();

	uint32 Crc = 0;
	TSharedPtr<FJsonObject> CreaturesRoot, AnimalsRoot, SpawnsRoot, LootRoot;
	TArray<FRowSource> CreatureRows, AnimalRows, SpawnRows, LootRows;
	GamerJson::ReadTable(Directory, CreaturesFile, CreaturesRoot, CreatureRows, Crc, Report);
	GamerJson::ReadTable(Directory, AnimalsFile, AnimalsRoot, AnimalRows, Crc, Report);
	GamerJson::ReadTable(Directory, SpawnsFile, SpawnsRoot, SpawnRows, Crc, Report);
	GamerJson::ReadTable(Directory, CreatureLootFile, LootRoot, LootRows, Crc, Report);
	Out.Checksum = Crc;

	ParseBiomes(SpawnsRoot, Out, Report);
	ParseCreatures(CreatureRows, false, Out, Report);
	ParseCreatures(AnimalRows, true, Out, Report);
	ParseSpawns(SpawnRows, Out, Report);
	CheckSpawns(Out, Report);
	ParseLoot(LootRows, Base, Out, Report);

	return Report.Errors.Num() == ErrorsBefore;
}

void GamerData::ReadCreatureLootItems(const FString& Directory, TSet<FName>& OutItems)
{
	FString Text;
	if (!FFileHelper::LoadFileToString(Text, *FPaths::Combine(Directory, CreatureLootFile)))
	{
		return;
	}
	TSharedPtr<FJsonObject> Root;
	if (!FJsonSerializer::Deserialize(TJsonReaderFactory<>::Create(Text), Root) || !Root.IsValid())
	{
		return;
	}
	const TArray<TSharedPtr<FJsonValue>>* Rows = nullptr;
	if (!Root->TryGetArrayField(TEXT("rows"), Rows) || !Rows)
	{
		return;
	}
	for (const TSharedPtr<FJsonValue>& RowValue : *Rows)
	{
		const TSharedPtr<FJsonObject> Row = (RowValue.IsValid() && RowValue->Type == EJson::Object) ? RowValue->AsObject() : nullptr;
		if (!Row.IsValid())
		{
			continue;
		}
		for (const TCHAR* Section : { TEXT("base"), TEXT("rank"), TEXT("biome") })
		{
			const TArray<TSharedPtr<FJsonValue>>* List = nullptr;
			if (!Row->TryGetArrayField(Section, List) || !List)
			{
				continue;
			}
			for (const TSharedPtr<FJsonValue>& EntryValue : *List)
			{
				const TSharedPtr<FJsonObject> Entry = (EntryValue.IsValid() && EntryValue->Type == EJson::Object) ? EntryValue->AsObject() : nullptr;
				FString Item, Family;
				if (!Entry.IsValid())
				{
					continue;
				}
				if (Entry->TryGetStringField(TEXT("item"), Item) && !Item.IsEmpty())
				{
					OutItems.Add(FName(*Item));
				}
				else if (Entry->TryGetStringField(TEXT("item_family"), Family) && !Family.IsEmpty())
				{
					for (int32 Tier = 1; Tier <= GamerData::MaxTier; ++Tier)
					{
						OutItems.Add(FName(*FString::Printf(TEXT("%s_t%d"), *Family, Tier)));
					}
				}
			}
		}
	}
}
