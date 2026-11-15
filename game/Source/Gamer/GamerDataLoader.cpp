#include "GamerDataLoader.h"

#include "GamerCharacterData.h"
#include "Dom/JsonObject.h"
#include "Dom/JsonValue.h"
#include "Misc/Crc.h"
#include "Misc/FileHelper.h"
#include "Misc/PackageName.h"
#include "Misc/Paths.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"

// Validation messages are German on purpose: they are read by whoever edits the data files.

namespace
{
	const TCHAR* const ItemsFile = TEXT("items.json");
	const TCHAR* const NodesFile = TEXT("resource_nodes.json");
	const TCHAR* const RecipesFile = TEXT("recipes.json");
	const TCHAR* const BuildablesFile = TEXT("buildables.json");
	const TCHAR* const JobsFile = TEXT("jobs.json");
	const TCHAR* const NeedsFile = TEXT("needs.json");

	bool IsSnakeCase(const FString& S)
	{
		if (S.IsEmpty() || !FChar::IsLower(S[0]))
		{
			return false;
		}
		for (const TCHAR C : S)
		{
			if (!(FChar::IsLower(C) || FChar::IsDigit(C) || C == TEXT('_')))
			{
				return false;
			}
		}
		return true;
	}

	/** Reads fields of one JSON object and records problems with a readable location ("items.json, Eintrag 3 (item_log)"). */
	class FRowReader
	{
	public:
		FRowReader(const TSharedPtr<FJsonObject>& InObject, FString InContext, FGamerDataReport& InReport)
			: Object(InObject), Context(MoveTemp(InContext)), Report(InReport)
		{
		}

		const FString& GetContext() const { return Context; }
		void SetContext(FString InContext) { Context = MoveTemp(InContext); }
		FGamerDataReport& GetReport() const { return Report; }

		void Error(const FString& Message) { Report.Errors.Add(FString::Printf(TEXT("%s: %s"), *Context, *Message)); }
		void Warning(const FString& Message) { Report.Warnings.Add(FString::Printf(TEXT("%s: %s"), *Context, *Message)); }

		/** Unknown fields are errors: a typo like "regrow_dayz" would otherwise be ignored silently. */
		void CheckKnownFields(std::initializer_list<const TCHAR*> Allowed)
		{
			for (const TPair<FString, TSharedPtr<FJsonValue>>& Pair : Object->Values)
			{
				bool bKnown = false;
				for (const TCHAR* Name : Allowed)
				{
					if (Pair.Key.Equals(Name, ESearchCase::CaseSensitive))
					{
						bKnown = true;
						break;
					}
				}
				if (!bKnown)
				{
					Error(FString::Printf(TEXT("unbekanntes Feld \"%s\" (Tippfehler?)"), *Pair.Key));
				}
			}
		}

		bool GetString(const TCHAR* Field, FString& Out, bool bRequired)
		{
			const TSharedPtr<FJsonValue> Value = Find(Field, bRequired);
			if (!Value)
			{
				return false;
			}
			if (Value->Type != EJson::String)
			{
				Error(FString::Printf(TEXT("\"%s\" muss Text sein"), Field));
				return false;
			}
			Out = Value->AsString();
			if (bRequired && Out.IsEmpty())
			{
				Error(FString::Printf(TEXT("\"%s\" ist leer"), Field));
				return false;
			}
			return true;
		}

		bool GetNumber(const TCHAR* Field, double& Out, bool bRequired)
		{
			const TSharedPtr<FJsonValue> Value = Find(Field, bRequired);
			if (!Value)
			{
				return false;
			}
			if (Value->Type != EJson::Number)
			{
				Error(FString::Printf(TEXT("\"%s\" muss eine Zahl sein"), Field));
				return false;
			}
			Out = Value->AsNumber();
			return true;
		}

		bool GetInt(const TCHAR* Field, int32& Out, bool bRequired)
		{
			double Number = 0.0;
			if (!GetNumber(Field, Number, bRequired))
			{
				return false;
			}
			if (Number != FMath::RoundToDouble(Number) || FMath::Abs(Number) > 1.0e9)
			{
				Error(FString::Printf(TEXT("\"%s\" muss eine ganze Zahl sein"), Field));
				return false;
			}
			Out = static_cast<int32>(Number);
			return true;
		}

		bool GetBool(const TCHAR* Field, bool& Out)
		{
			const TSharedPtr<FJsonValue> Value = Find(Field, false);
			if (!Value)
			{
				return false;
			}
			if (Value->Type != EJson::Boolean)
			{
				Error(FString::Printf(TEXT("\"%s\" muss true oder false sein"), Field));
				return false;
			}
			Out = Value->AsBool();
			return true;
		}

		TSharedPtr<FJsonObject> GetObject(const TCHAR* Field, bool bRequired)
		{
			const TSharedPtr<FJsonValue> Value = Find(Field, bRequired);
			if (!Value)
			{
				return nullptr;
			}
			if (Value->Type != EJson::Object)
			{
				Error(FString::Printf(TEXT("\"%s\" muss ein Objekt { ... } sein"), Field));
				return nullptr;
			}
			return Value->AsObject();
		}

		const TArray<TSharedPtr<FJsonValue>>* GetArray(const TCHAR* Field, bool bRequired)
		{
			const TSharedPtr<FJsonValue> Value = Find(Field, bRequired);
			if (!Value)
			{
				return nullptr;
			}
			if (Value->Type != EJson::Array)
			{
				Error(FString::Printf(TEXT("\"%s\" muss eine Liste [ ... ] sein"), Field));
				return nullptr;
			}
			return &Value->AsArray();
		}

		bool Has(const TCHAR* Field) const { return Object->HasField(Field); }

	private:
		TSharedPtr<FJsonValue> Find(const TCHAR* Field, bool bRequired)
		{
			const TSharedPtr<FJsonValue> Value = Object->TryGetField(Field);
			if (!Value.IsValid() || Value->Type == EJson::Null)
			{
				if (bRequired)
				{
					Error(FString::Printf(TEXT("Pflichtfeld \"%s\" fehlt"), Field));
				}
				return nullptr;
			}
			return Value;
		}

		TSharedPtr<FJsonObject> Object;
		FString Context;
		FGamerDataReport& Report;
	};

	/** One parsed row plus where it came from. */
	struct FRow
	{
		TSharedPtr<FJsonObject> Object;
		FString Context;
	};

	/** Reads a file, checks "schema" and "rows", and returns the row objects. */
	bool ReadTable(const FString& Directory, const TCHAR* FileName, TArray<FRow>& OutRows, uint32& InOutCrc, FGamerDataReport& Report,
		bool bOptional = false)
	{
		const FString Path = FPaths::Combine(Directory, FileName);
		FString Text;
		if (bOptional && !FPaths::FileExists(Path))
		{
			return false; // table not used by this data set (e.g. test data without villagers)
		}
		if (!FFileHelper::LoadFileToString(Text, *Path))
		{
			Report.Errors.Add(FString::Printf(TEXT("%s: Datei fehlt oder ist nicht lesbar (%s)"), FileName, *Path));
			return false;
		}

		// Checksum over the content without carriage returns, so CRLF/LF checkouts give the same value.
		FString Normalized = Text.Replace(TEXT("\r"), TEXT(""));
		const FTCHARToUTF8 Utf8(*Normalized);
		InOutCrc = FCrc::MemCrc32(Utf8.Get(), Utf8.Length(), InOutCrc);

		TSharedPtr<FJsonObject> Root;
		const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Text);
		if (!FJsonSerializer::Deserialize(Reader, Root) || !Root.IsValid())
		{
			Report.Errors.Add(FString::Printf(TEXT("%s: kein gültiges JSON (%s)"), FileName, *Reader->GetErrorMessage()));
			return false;
		}

		double Schema = 0.0;
		if (!Root->TryGetNumberField(TEXT("schema"), Schema) || static_cast<int32>(Schema) != GamerData::SchemaVersion)
		{
			Report.Errors.Add(FString::Printf(TEXT("%s: \"schema\" fehlt oder ist nicht %d"), FileName, GamerData::SchemaVersion));
			return false;
		}

		const TArray<TSharedPtr<FJsonValue>>* Rows = nullptr;
		if (!Root->TryGetArrayField(TEXT("rows"), Rows) || !Rows)
		{
			Report.Errors.Add(FString::Printf(TEXT("%s: Liste \"rows\" fehlt"), FileName));
			return false;
		}

		for (int32 Index = 0; Index < Rows->Num(); ++Index)
		{
			const TSharedPtr<FJsonValue>& Value = (*Rows)[Index];
			FString Context = FString::Printf(TEXT("%s, Eintrag %d"), FileName, Index + 1);
			if (!Value.IsValid() || Value->Type != EJson::Object)
			{
				Report.Errors.Add(FString::Printf(TEXT("%s: ist kein Objekt { ... }"), *Context));
				continue;
			}
			OutRows.Add({ Value->AsObject(), MoveTemp(Context) });
		}
		return true;
	}

	/** Reads "id", checks format and prefix, and adds the ID to the reader context. */
	bool ReadId(FRowReader& Row, std::initializer_list<const TCHAR*> Prefixes, FName& OutId)
	{
		FString Id;
		if (!Row.GetString(TEXT("id"), Id, true))
		{
			return false;
		}
		Row.SetContext(FString::Printf(TEXT("%s (%s)"), *Row.GetContext(), *Id));
		if (!IsSnakeCase(Id))
		{
			Row.Error(TEXT("ID muss snake_case sein (Kleinbuchstaben, Ziffern, Unterstrich)"));
			return false;
		}
		bool bPrefixOk = false;
		FString Expected;
		for (const TCHAR* Prefix : Prefixes)
		{
			bPrefixOk |= Id.StartsWith(Prefix, ESearchCase::CaseSensitive);
			Expected += Expected.IsEmpty() ? FString(Prefix) : FString::Printf(TEXT(" oder %s"), Prefix);
		}
		if (!bPrefixOk)
		{
			Row.Error(FString::Printf(TEXT("ID muss mit %s beginnen"), *Expected));
			return false;
		}
		OutId = FName(*Id);
		return true;
	}

	void ReadAssetPath(FRowReader& Row, const TCHAR* Field, FSoftObjectPath& Out, bool bCheckAssetPaths)
	{
		FString Path;
		if (!Row.GetString(Field, Path, false) || Path.IsEmpty())
		{
			return;
		}
		if (!Path.StartsWith(TEXT("/Game/")) && !Path.StartsWith(TEXT("/Engine/")))
		{
			Row.Error(FString::Printf(TEXT("\"%s\" muss ein Unreal-Pfad sein, der mit /Game/ oder /Engine/ beginnt"), Field));
			return;
		}
		Out = FSoftObjectPath(Path);
		if (bCheckAssetPaths && !FPackageName::DoesPackageExist(Out.GetLongPackageName()))
		{
			Row.Warning(FString::Printf(TEXT("\"%s\": Asset %s nicht gefunden (noch nicht importiert?)"), Field, *Path));
		}
	}

	void ReadToolRequirement(FRowReader& Row, const TCHAR* Field, FGamerToolRequirement& Out)
	{
		const TSharedPtr<FJsonObject> Object = Row.GetObject(Field, false);
		if (!Object)
		{
			return;
		}
		FRowReader Sub(Object, FString::Printf(TEXT("%s, %s"), *Row.GetContext(), Field), Row.GetReport());
		Sub.CheckKnownFields({ TEXT("type"), TEXT("min_tier") });
		FString Type;
		if (Sub.GetString(TEXT("type"), Type, true))
		{
			if (!IsSnakeCase(Type))
			{
				Sub.Error(FString::Printf(TEXT("type \"%s\" muss snake_case sein"), *Type));
			}
			Out.Type = FName(*Type);
		}
		if (Sub.GetInt(TEXT("min_tier"), Out.MinTier, true) && (Out.MinTier < 1 || Out.MinTier > GamerData::MaxTier))
		{
			Sub.Error(FString::Printf(TEXT("min_tier muss zwischen 1 und %d liegen"), GamerData::MaxTier));
		}
	}

	/** Reads a list of { "item": ..., "count": ... }. */
	void ReadAmounts(FRowReader& Row, const TCHAR* Field, TArray<FGamerItemAmount>& Out, bool bRequired)
	{
		const TArray<TSharedPtr<FJsonValue>>* List = Row.GetArray(Field, bRequired);
		if (!List)
		{
			return;
		}
		if (bRequired && List->Num() == 0)
		{
			Row.Error(FString::Printf(TEXT("\"%s\" ist leer"), Field));
		}
		for (int32 Index = 0; Index < List->Num(); ++Index)
		{
			const TSharedPtr<FJsonValue>& Value = (*List)[Index];
			if (!Value.IsValid() || Value->Type != EJson::Object)
			{
				Row.Error(FString::Printf(TEXT("%s[%d] muss ein Objekt { \"item\", \"count\" } sein"), Field, Index + 1));
				continue;
			}
			const TSharedPtr<FJsonObject> Entry = Value->AsObject();
			FGamerItemAmount Amount;
			FString Item;
			double Count = 0.0;
			for (const TPair<FString, TSharedPtr<FJsonValue>>& Pair : Entry->Values)
			{
				if (Pair.Key != TEXT("item") && Pair.Key != TEXT("count"))
				{
					Row.Error(FString::Printf(TEXT("%s[%d]: unbekanntes Feld \"%s\""), Field, Index + 1, *Pair.Key));
				}
			}
			if (!Entry->TryGetStringField(TEXT("item"), Item) || Item.IsEmpty())
			{
				Row.Error(FString::Printf(TEXT("%s[%d]: \"item\" fehlt"), Field, Index + 1));
				continue;
			}
			if (!Entry->TryGetNumberField(TEXT("count"), Count) || Count != FMath::RoundToDouble(Count) || Count < 1.0)
			{
				Row.Error(FString::Printf(TEXT("%s[%d]: \"count\" muss eine ganze Zahl ab 1 sein"), Field, Index + 1));
				continue;
			}
			Amount.Item = FName(*Item);
			Amount.Count = static_cast<int32>(Count);
			Out.Add(Amount);
		}
	}

	/** Reads a list of { "item", "min", "max" } (resource drops, field harvests). */
	void ReadDrops(FRowReader& Row, const TCHAR* Field, TArray<FGamerDrop>& Out, bool bRequired)
	{
		const TArray<TSharedPtr<FJsonValue>>* Drops = Row.GetArray(Field, bRequired);
		if (!Drops)
		{
			return;
		}
		if (Drops->Num() == 0)
		{
			Row.Error(FString::Printf(TEXT("\"%s\" ist leer"), Field));
		}
		for (int32 Index = 0; Index < Drops->Num(); ++Index)
		{
			const TSharedPtr<FJsonValue>& Value = (*Drops)[Index];
			if (!Value.IsValid() || Value->Type != EJson::Object)
			{
				Row.Error(FString::Printf(TEXT("%s[%d] muss ein Objekt sein"), Field, Index + 1));
				continue;
			}
			FRowReader DropRow(Value->AsObject(), FString::Printf(TEXT("%s, %s[%d]"), *Row.GetContext(), Field, Index + 1), Row.GetReport());
			DropRow.CheckKnownFields({ TEXT("item"), TEXT("min"), TEXT("max") });
			FGamerDrop Drop;
			FString Item;
			const bool bItem = DropRow.GetString(TEXT("item"), Item, true);
			const bool bMin = DropRow.GetInt(TEXT("min"), Drop.Min, true);
			const bool bMax = DropRow.GetInt(TEXT("max"), Drop.Max, true);
			if (bMin && bMax && (Drop.Min < 0 || Drop.Max < 1 || Drop.Min > Drop.Max))
			{
				DropRow.Error(TEXT("es muss gelten: 0 <= min <= max und max >= 1"));
			}
			if (bItem)
			{
				Drop.Item = FName(*Item);
				Out.Add(Drop);
			}
		}
	}

	/** Reads a list of Unreal asset paths (growing stages). */
	void ReadAssetList(FRowReader& Row, const TCHAR* Field, TArray<FSoftObjectPath>& Out, bool bCheckAssetPaths)
	{
		const TArray<TSharedPtr<FJsonValue>>* List = Row.GetArray(Field, false);
		if (!List)
		{
			return;
		}
		for (int32 Stage = 0; Stage < List->Num(); ++Stage)
		{
			FString Path;
			if (!(*List)[Stage].IsValid() || !(*List)[Stage]->TryGetString(Path)
				|| !(Path.StartsWith(TEXT("/Game/")) || Path.StartsWith(TEXT("/Engine/"))))
			{
				Row.Error(FString::Printf(TEXT("%s[%d] muss ein Unreal-Pfad sein (/Game/...)"), Field, Stage + 1));
				continue;
			}
			const FSoftObjectPath Soft(Path);
			if (bCheckAssetPaths && !FPackageName::DoesPackageExist(Soft.GetLongPackageName()))
			{
				Row.Warning(FString::Printf(TEXT("%s[%d]: Asset %s nicht gefunden (noch nicht importiert?)"), Field, Stage + 1, *Path));
			}
			Out.Add(Soft);
		}
	}

	void ParseItems(const TArray<FRow>& Rows, FGamerDataSet& Out, FGamerDataReport& Report, bool bCheckAssetPaths)
	{
		for (const FRow& Source : Rows)
		{
			FRowReader Row(Source.Object, Source.Context, Report);
			Row.CheckKnownFields({ TEXT("id"), TEXT("name"), TEXT("category"), TEXT("tier"), TEXT("max_stack"), TEXT("weight_kg"),
				TEXT("icon"), TEXT("world_mesh"), TEXT("tool"), TEXT("food"), TEXT("spoil_days"), TEXT("spoils_into"), TEXT("plant"),
				TEXT("weapon"), TEXT("deprecated") });

			FGamerItemDef Def;
			if (!ReadId(Row, { TEXT("item_"), TEXT("tool_"), TEXT("weapon_") }, Def.Id))
			{
				continue;
			}
			Row.GetString(TEXT("name"), Def.Name, true);

			FString Category;
			if (Row.GetString(TEXT("category"), Category, true))
			{
				if (Category == TEXT("resource")) { Def.Category = EGamerItemCategory::Resource; }
				else if (Category == TEXT("material")) { Def.Category = EGamerItemCategory::Material; }
				else if (Category == TEXT("food")) { Def.Category = EGamerItemCategory::Food; }
				else if (Category == TEXT("tool")) { Def.Category = EGamerItemCategory::Tool; }
				else if (Category == TEXT("weapon")) { Def.Category = EGamerItemCategory::Weapon; }
				else { Row.Error(FString::Printf(TEXT("category \"%s\" unbekannt (erlaubt: resource, material, food, tool, weapon)"), *Category)); }
			}

			if (Row.GetInt(TEXT("tier"), Def.Tier, true) && (Def.Tier < 1 || Def.Tier > GamerData::MaxTier))
			{
				Row.Error(FString::Printf(TEXT("tier muss zwischen 1 und %d liegen"), GamerData::MaxTier));
			}
			if (Row.GetInt(TEXT("max_stack"), Def.MaxStack, true) && Def.MaxStack < 1)
			{
				Row.Error(TEXT("max_stack muss mindestens 1 sein"));
			}
			double Weight = 0.0;
			if (Row.GetNumber(TEXT("weight_kg"), Weight, true))
			{
				if (Weight < 0.0)
				{
					Row.Error(TEXT("weight_kg darf nicht negativ sein"));
				}
				Def.WeightKg = static_cast<float>(Weight);
			}
			ReadAssetPath(Row, TEXT("icon"), Def.Icon, bCheckAssetPaths);
			ReadAssetPath(Row, TEXT("world_mesh"), Def.WorldMesh, bCheckAssetPaths);
			Row.GetBool(TEXT("deprecated"), Def.bDeprecated);

			const bool bIsToolId = Def.Id.ToString().StartsWith(TEXT("tool_"));
			const bool bIsToolCategory = Def.Category == EGamerItemCategory::Tool;
			if (bIsToolId != bIsToolCategory)
			{
				Row.Error(TEXT("Werkzeuge brauchen ID tool_... und category \"tool\" (beides oder keins)"));
			}
			const TSharedPtr<FJsonObject> Tool = Row.GetObject(TEXT("tool"), bIsToolCategory);
			if (Tool && !bIsToolCategory)
			{
				Row.Error(TEXT("\"tool\" nur bei category \"tool\" erlaubt"));
			}
			if (Tool)
			{
				FRowReader ToolRow(Tool, Row.GetContext() + TEXT(", tool"), Report);
				ToolRow.CheckKnownFields({ TEXT("type"), TEXT("durability"), TEXT("work_speed") });
				FString Type;
				if (ToolRow.GetString(TEXT("type"), Type, true))
				{
					if (!IsSnakeCase(Type))
					{
						ToolRow.Error(TEXT("type muss snake_case sein"));
					}
					Def.Tool.Type = FName(*Type);
				}
				if (ToolRow.GetInt(TEXT("durability"), Def.Tool.Durability, true) && Def.Tool.Durability < 1)
				{
					ToolRow.Error(TEXT("durability muss mindestens 1 sein"));
				}
				double Speed = 0.0;
				if (ToolRow.GetNumber(TEXT("work_speed"), Speed, true))
				{
					if (Speed <= 0.0)
					{
						ToolRow.Error(TEXT("work_speed muss größer als 0 sein"));
					}
					Def.Tool.WorkSpeed = static_cast<float>(Speed);
				}
				if (Def.MaxStack != 1)
				{
					Row.Warning(TEXT("Werkzeuge sollten max_stack 1 haben"));
				}
			}
			// Weapons: ID weapon_... and category "weapon" belong together, like tools.
			const bool bIsWeaponId = Def.Id.ToString().StartsWith(TEXT("weapon_"));
			const bool bIsWeaponCategory = Def.Category == EGamerItemCategory::Weapon;
			if (bIsWeaponId != bIsWeaponCategory)
			{
				Row.Error(TEXT("Waffen brauchen ID weapon_... und category \"weapon\" (beides oder keins)"));
			}
			const TSharedPtr<FJsonObject> Weapon = Row.GetObject(TEXT("weapon"), bIsWeaponCategory);
			if (Weapon && !bIsWeaponCategory)
			{
				Row.Error(TEXT("\"weapon\" nur bei category \"weapon\" erlaubt"));
			}
			if (Weapon)
			{
				FRowReader WeaponRow(Weapon, Row.GetContext() + TEXT(", weapon"), Report);
				WeaponRow.CheckKnownFields({ TEXT("type"), TEXT("damage"), TEXT("durability") });
				FString Type;
				if (WeaponRow.GetString(TEXT("type"), Type, true))
				{
					if (!IsSnakeCase(Type))
					{
						WeaponRow.Error(TEXT("type muss snake_case sein"));
					}
					Def.Weapon.Type = FName(*Type);
				}
				double Damage = 0.0;
				if (WeaponRow.GetNumber(TEXT("damage"), Damage, true))
				{
					if (Damage <= 0.0)
					{
						WeaponRow.Error(TEXT("damage muss größer als 0 sein"));
					}
					Def.Weapon.Damage = static_cast<float>(Damage);
				}
				if (WeaponRow.GetInt(TEXT("durability"), Def.Weapon.Durability, true) && Def.Weapon.Durability < 1)
				{
					WeaponRow.Error(TEXT("durability muss mindestens 1 sein"));
				}
				if (Def.MaxStack != 1)
				{
					Row.Warning(TEXT("Waffen sollten max_stack 1 haben"));
				}
			}
			// Spoiling (decision 2026-11-14: in phase 1). Game days while carried; storage and ground scale it.
			double SpoilDays = 0.0;
			if (Row.GetNumber(TEXT("spoil_days"), SpoilDays, false))
			{
				if (SpoilDays <= 0.0)
				{
					Row.Error(TEXT("spoil_days muss größer als 0 sein (weglassen = verdirbt nicht)"));
				}
				Def.SpoilDays = static_cast<float>(SpoilDays);
				Def.SpoilsInto = TEXT("item_spoiled_food");
			}
			FString SpoilsInto;
			if (Row.GetString(TEXT("spoils_into"), SpoilsInto, false))
			{
				if (Def.SpoilDays <= 0.f)
				{
					Row.Error(TEXT("spoils_into nur zusammen mit spoil_days"));
				}
				Def.SpoilsInto = FName(*SpoilsInto);
			}
			if (!Def.SpoilsInto.IsNone() && Def.SpoilsInto == Def.Id)
			{
				Row.Error(TEXT("spoils_into darf nicht der Gegenstand selbst sein"));
			}
			// Seeds: what grows from them in a field.
			if (const TSharedPtr<FJsonObject> Plant = Row.GetObject(TEXT("plant"), false))
			{
				FRowReader PlantRow(Plant, Row.GetContext() + TEXT(", plant"), Report);
				PlantRow.CheckKnownFields({ TEXT("grow_days"), TEXT("harvest"), TEXT("meshes") });
				double GrowDays = 0.0;
				if (PlantRow.GetNumber(TEXT("grow_days"), GrowDays, true))
				{
					if (GrowDays <= 0.0)
					{
						PlantRow.Error(TEXT("grow_days muss größer als 0 sein"));
					}
					Def.Plant.GrowDays = static_cast<float>(GrowDays);
				}
				ReadDrops(PlantRow, TEXT("harvest"), Def.Plant.Harvest, true);
				ReadAssetList(PlantRow, TEXT("meshes"), Def.Plant.Meshes, bCheckAssetPaths);
			}

			// Meal buffs (decision 2026-11-14, docs/12_taverne_gerichte.md).
			if (const TSharedPtr<FJsonObject> Food = Row.GetObject(TEXT("food"), false))
			{
				FRowReader FoodRow(Food, Row.GetContext() + TEXT(", food"), Report);
				FoodRow.CheckKnownFields({ TEXT("minutes"), TEXT("buffs") });
				if (Def.Category != EGamerItemCategory::Food)
				{
					Row.Error(TEXT("\"food\" nur bei category \"food\""));
				}
				double Minutes = 0.0;
				if (FoodRow.GetNumber(TEXT("minutes"), Minutes, true))
				{
					if (Minutes <= 0.0)
					{
						FoodRow.Error(TEXT("minutes muss größer als 0 sein"));
					}
					Def.Food.Minutes = static_cast<float>(Minutes);
				}
				if (const TArray<TSharedPtr<FJsonValue>>* Buffs = FoodRow.GetArray(TEXT("buffs"), true))
				{
					if (Buffs->Num() == 0)
					{
						FoodRow.Error(TEXT("buffs ist leer"));
					}
					TSet<EGamerBuffStat> Seen;
					for (int32 Index = 0; Index < Buffs->Num(); ++Index)
					{
						const TSharedPtr<FJsonValue>& Value = (*Buffs)[Index];
						if (!Value.IsValid() || Value->Type != EJson::Object)
						{
							FoodRow.Error(FString::Printf(TEXT("buffs[%d] muss ein Objekt { \"stat\", \"value\" } sein"), Index + 1));
							continue;
						}
						FRowReader BuffRow(Value->AsObject(), FString::Printf(TEXT("%s, food.buffs[%d]"), *Row.GetContext(), Index + 1), Report);
						BuffRow.CheckKnownFields({ TEXT("stat"), TEXT("value") });
						FGamerBuffEffect Effect;
						FString Stat;
						if (BuffRow.GetString(TEXT("stat"), Stat, true))
						{
							if (Stat == TEXT("health")) { Effect.Stat = EGamerBuffStat::Health; }
							else if (Stat == TEXT("regen")) { Effect.Stat = EGamerBuffStat::Regen; }
							else if (Stat == TEXT("carry")) { Effect.Stat = EGamerBuffStat::Carry; }
							else if (Stat == TEXT("cold_resist")) { Effect.Stat = EGamerBuffStat::ColdResist; }
							else if (Stat == TEXT("poison_resist")) { Effect.Stat = EGamerBuffStat::PoisonResist; }
							else if (Stat == TEXT("damage")) { Effect.Stat = EGamerBuffStat::Damage; }
							else
							{
								BuffRow.Error(FString::Printf(TEXT("stat \"%s\" unbekannt (erlaubt: health, regen, carry, cold_resist, poison_resist, damage)"), *Stat));
								continue;
							}
						}
						double Amount = 0.0;
						if (BuffRow.GetNumber(TEXT("value"), Amount, true) && Amount <= 0.0)
						{
							BuffRow.Error(TEXT("value muss größer als 0 sein (Essen gibt nur Boni, Regel 5)"));
						}
						Effect.Value = static_cast<float>(Amount);
						if (Seen.Contains(Effect.Stat))
						{
							BuffRow.Error(FString::Printf(TEXT("stat \"%s\" doppelt"), *Stat));
						}
						Seen.Add(Effect.Stat);
						Def.Food.Buffs.Add(Effect);
					}
				}
			}

			if (Out.Items.Contains(Def.Id))
			{
				Row.Error(TEXT("ID doppelt vergeben"));
				continue;
			}
			Out.Items.Add(Def.Id, MoveTemp(Def));
		}
	}

	/** resource_nodes.json "season": yearly fruit cycle in season phases 0..4. */
	void ReadSeason(FRowReader& Row, FGamerResourceNodeDef& Def)
	{
		const TSharedPtr<FJsonObject> Season = Row.GetObject(TEXT("season"), false);
		if (!Season)
		{
			return;
		}
		FRowReader S(Season, Row.GetContext() + TEXT(", season"), Row.GetReport());
		S.CheckKnownFields({ TEXT("bloom"), TEXT("unripe"), TEXT("ripe"), TEXT("fall"), TEXT("bare"), TEXT("pick"),
			TEXT("drop_height_cm"), TEXT("bruise_cm"), TEXT("bruised_item") });
		FGamerSeasonProps& P = Def.Season;
		double V = 0.0;
		if (S.GetNumber(TEXT("bloom"), V, true)) { P.Bloom = static_cast<float>(V); }
		if (S.GetNumber(TEXT("unripe"), V, true)) { P.Unripe = static_cast<float>(V); }
		if (S.GetNumber(TEXT("ripe"), V, true)) { P.Ripe = static_cast<float>(V); }
		if (S.GetNumber(TEXT("bare"), V, true)) { P.Bare = static_cast<float>(V); }
		P.Fall = P.Bare;
		if (S.GetNumber(TEXT("fall"), V, false)) { P.Fall = static_cast<float>(V); }
		if (!(P.Bloom >= 0.f && P.Bloom < P.Unripe && P.Unripe < P.Ripe && P.Ripe <= P.Fall && P.Fall <= P.Bare && P.Bare < 4.f))
		{
			S.Error(TEXT("es muss gelten: 0 <= bloom < unripe < ripe <= fall <= bare < 4 (Jahreszeit-Phasen)"));
		}
		S.GetInt(TEXT("pick"), P.Pick, false);
		if (P.Pick < 0)
		{
			S.Error(TEXT("pick darf nicht negativ sein (0 = alles auf einmal)"));
		}
		if (const TArray<TSharedPtr<FJsonValue>>* Heights = S.GetArray(TEXT("drop_height_cm"), false))
		{
			double Low = 0.0, High = 0.0;
			if (Heights->Num() != 2 || !(*Heights)[0]->TryGetNumber(Low) || !(*Heights)[1]->TryGetNumber(High) || Low < 0.0 || Low > High)
			{
				S.Error(TEXT("drop_height_cm muss [min, max] mit 0 <= min <= max sein"));
			}
			P.DropHeightMin = static_cast<float>(Low);
			P.DropHeightMax = static_cast<float>(High);
		}
		if (S.GetNumber(TEXT("bruise_cm"), V, false))
		{
			P.BruiseHeight = static_cast<float>(V);
		}
		FString Bruised;
		if (S.GetString(TEXT("bruised_item"), Bruised, false))
		{
			P.BruisedItem = FName(*Bruised);
		}
		if (P.Falls() && P.DropHeightMax <= 0.f)
		{
			S.Error(TEXT("mit \"fall\" vor \"bare\" fällt Obst herunter: drop_height_cm fehlt"));
		}
		if ((P.BruiseHeight > 0.f) != !P.BruisedItem.IsNone())
		{
			S.Error(TEXT("bruise_cm und bruised_item nur zusammen"));
		}
		if (Def.Drops.Num() != 1)
		{
			S.Error(TEXT("Vorkommen mit \"season\" brauchen genau einen Eintrag in drops (die Früchte des Jahres)"));
		}
		if (Def.RegrowDays > 0.f)
		{
			S.Warning(TEXT("regrow_days wird bei \"season\" nicht benutzt (Früchte kommen jedes Jahr wieder)"));
		}
	}

	void ParseNodes(const TArray<FRow>& Rows, FGamerDataSet& Out, FGamerDataReport& Report, bool bCheckAssetPaths)
	{
		for (const FRow& Source : Rows)
		{
			FRowReader Row(Source.Object, Source.Context, Report);
			Row.CheckKnownFields({ TEXT("id"), TEXT("name"), TEXT("required_tool"), TEXT("work_seconds"), TEXT("drops"),
				TEXT("regrow_days"), TEXT("meshes"), TEXT("season"), TEXT("deprecated") });

			FGamerResourceNodeDef Def;
			if (!ReadId(Row, { TEXT("res_") }, Def.Id))
			{
				continue;
			}
			Row.GetString(TEXT("name"), Def.Name, true);
			ReadToolRequirement(Row, TEXT("required_tool"), Def.RequiredTool);

			double Seconds = 0.0;
			if (Row.GetNumber(TEXT("work_seconds"), Seconds, true))
			{
				if (Seconds <= 0.0)
				{
					Row.Error(TEXT("work_seconds muss größer als 0 sein"));
				}
				Def.WorkSeconds = static_cast<float>(Seconds);
			}

			ReadDrops(Row, TEXT("drops"), Def.Drops, true);

			double Regrow = 0.0;
			if (Row.GetNumber(TEXT("regrow_days"), Regrow, false))
			{
				if (Regrow < 0.0)
				{
					Row.Error(TEXT("regrow_days darf nicht negativ sein"));
				}
				Def.RegrowDays = static_cast<float>(Regrow);
			}

			if (const TSharedPtr<FJsonObject> Meshes = Row.GetObject(TEXT("meshes"), false))
			{
				FRowReader MeshRow(Meshes, Row.GetContext() + TEXT(", meshes"), Report);
				MeshRow.CheckKnownFields({ TEXT("full"), TEXT("depleted"), TEXT("remains"), TEXT("growing"), TEXT("bloom"), TEXT("unripe") });
				ReadAssetPath(MeshRow, TEXT("bloom"), Def.MeshBloom, bCheckAssetPaths);
				ReadAssetPath(MeshRow, TEXT("unripe"), Def.MeshUnripe, bCheckAssetPaths);
				ReadAssetPath(MeshRow, TEXT("full"), Def.MeshFull, bCheckAssetPaths);
				ReadAssetPath(MeshRow, TEXT("depleted"), Def.MeshDepleted, bCheckAssetPaths);
				ReadAssetPath(MeshRow, TEXT("remains"), Def.MeshRemains, bCheckAssetPaths);
				// Visible regrowth: models shown one after another while the node grows back (sapling, young tree).
				ReadAssetList(MeshRow, TEXT("growing"), Def.MeshGrowing, bCheckAssetPaths);
			}
			ReadSeason(Row, Def);
			Row.GetBool(TEXT("deprecated"), Def.bDeprecated);

			if (Out.ResourceNodes.Contains(Def.Id))
			{
				Row.Error(TEXT("ID doppelt vergeben"));
				continue;
			}
			Out.ResourceNodes.Add(Def.Id, MoveTemp(Def));
		}
	}

	void ParseRecipes(const TArray<FRow>& Rows, FGamerDataSet& Out, FGamerDataReport& Report)
	{
		for (const FRow& Source : Rows)
		{
			FRowReader Row(Source.Object, Source.Context, Report);
			Row.CheckKnownFields({ TEXT("id"), TEXT("name"), TEXT("station"), TEXT("inputs"), TEXT("outputs"), TEXT("work_seconds"), TEXT("deprecated") });

			FGamerRecipeDef Def;
			if (!ReadId(Row, { TEXT("recipe_") }, Def.Id))
			{
				continue;
			}
			FString Station;
			if (Row.GetString(TEXT("station"), Station, false) && !Station.IsEmpty())
			{
				Def.Station = FName(*Station);
			}
			ReadAmounts(Row, TEXT("inputs"), Def.Inputs, true);
			ReadAmounts(Row, TEXT("outputs"), Def.Outputs, true);

			double Seconds = 0.0;
			if (Row.GetNumber(TEXT("work_seconds"), Seconds, true))
			{
				if (Seconds <= 0.0)
				{
					Row.Error(TEXT("work_seconds muss größer als 0 sein"));
				}
				Def.WorkSeconds = static_cast<float>(Seconds);
			}
			Row.GetBool(TEXT("deprecated"), Def.bDeprecated);

			if (Out.Recipes.Contains(Def.Id))
			{
				Row.Error(TEXT("ID doppelt vergeben"));
				continue;
			}
			Out.Recipes.Add(Def.Id, MoveTemp(Def));
		}
	}

	/** buildables.json "farm" (category farm only). */
	void ReadFarm(FRowReader& Row, FGamerBuildableDef& Def)
	{
		const bool bIsFarm = Def.Category == EGamerBuildCategory::Farm;
		const TSharedPtr<FJsonObject> Farm = Row.GetObject(TEXT("farm"), bIsFarm);
		if (!Farm)
		{
			return;
		}
		if (!bIsFarm)
		{
			Row.Error(TEXT("\"farm\" nur bei category \"farm\""));
			return;
		}
		FRowReader F(Farm, Row.GetContext() + TEXT(", farm"), Row.GetReport());
		FGamerFarmProps& P = Def.Farm;
		FString Kind;
		F.GetString(TEXT("kind"), Kind, true);
		auto ReadName = [&F](const TCHAR* Field, FName& Out)
		{
			FString Text;
			if (F.GetString(Field, Text, true))
			{
				Out = FName(*Text);
			}
		};
		auto ReadPositive = [&F](const TCHAR* Field, float& Out)
		{
			double V = 0.0;
			if (F.GetNumber(Field, V, true))
			{
				if (V <= 0.0)
				{
					F.Error(FString::Printf(TEXT("%s muss größer als 0 sein"), Field));
				}
				Out = static_cast<float>(V);
			}
		};
		if (Kind == TEXT("field"))
		{
			P.Kind = EGamerFarmKind::Field;
			F.CheckKnownFields({ TEXT("kind"), TEXT("water_item"), TEXT("water_per_day"), TEXT("fertilizer_item"), TEXT("fertilizer_bonus"),
				TEXT("grow_from"), TEXT("grow_to"), TEXT("min_size_cm"), TEXT("max_size_cm"), TEXT("unit_m2") });
			ReadPositive(TEXT("min_size_cm"), P.MinSizeCm);
			ReadPositive(TEXT("max_size_cm"), P.MaxSizeCm);
			ReadPositive(TEXT("unit_m2"), P.UnitM2);
			if (P.MinSizeCm > P.MaxSizeCm)
			{
				F.Error(TEXT("min_size_cm muss kleiner oder gleich max_size_cm sein"));
			}
			ReadName(TEXT("water_item"), P.WaterItem);
			ReadPositive(TEXT("water_per_day"), P.WaterPerDay);
			ReadName(TEXT("fertilizer_item"), P.FertilizerItem);
			double V = 0.0;
			if (F.GetNumber(TEXT("fertilizer_bonus"), V, true))
			{
				if (V < 0.0) { F.Error(TEXT("fertilizer_bonus darf nicht negativ sein")); }
				P.FertilizerBonus = static_cast<float>(V);
			}
			if (F.GetNumber(TEXT("grow_from"), V, true)) { P.GrowFrom = static_cast<float>(V); }
			if (F.GetNumber(TEXT("grow_to"), V, true)) { P.GrowTo = static_cast<float>(V); }
			if (!(P.GrowFrom >= 0.f && P.GrowFrom < P.GrowTo && P.GrowTo <= 4.f))
			{
				F.Error(TEXT("es muss gelten: 0 <= grow_from < grow_to <= 4 (Jahreszeit-Phasen)"));
			}
		}
		else if (Kind == TEXT("composter"))
		{
			P.Kind = EGamerFarmKind::Composter;
			F.CheckKnownFields({ TEXT("kind"), TEXT("inputs"), TEXT("per_output"), TEXT("output"), TEXT("days_per_output"), TEXT("capacity") });
			if (const TArray<TSharedPtr<FJsonValue>>* Inputs = F.GetArray(TEXT("inputs"), true))
			{
				for (const TSharedPtr<FJsonValue>& Value : *Inputs)
				{
					FString Item;
					if (Value.IsValid() && Value->TryGetString(Item) && !Item.IsEmpty()) { P.Inputs.Add(FName(*Item)); }
					else { F.Error(TEXT("inputs muss eine Liste von Gegenstands-IDs sein")); }
				}
				if (P.Inputs.Num() == 0) { F.Error(TEXT("inputs ist leer")); }
			}
			if (F.GetInt(TEXT("per_output"), P.PerOutput, true) && P.PerOutput < 1) { F.Error(TEXT("per_output muss mindestens 1 sein")); }
			ReadName(TEXT("output"), P.Output);
			ReadPositive(TEXT("days_per_output"), P.Rate);
			if (F.GetInt(TEXT("capacity"), P.Capacity, true) && P.Capacity < P.PerOutput) { F.Error(TEXT("capacity muss mindestens per_output sein")); }
		}
		else if (Kind == TEXT("water"))
		{
			P.Kind = EGamerFarmKind::Water;
			F.CheckKnownFields({ TEXT("kind"), TEXT("output"), TEXT("per_day"), TEXT("capacity") });
			ReadName(TEXT("output"), P.Output);
			ReadPositive(TEXT("per_day"), P.Rate);
			if (F.GetInt(TEXT("capacity"), P.Capacity, true) && P.Capacity < 1) { F.Error(TEXT("capacity muss mindestens 1 sein")); }
		}
		else if (Kind == TEXT("tree"))
		{
			P.Kind = EGamerFarmKind::Tree;
			F.CheckKnownFields({ TEXT("kind"), TEXT("node"), TEXT("grow_days") });
			ReadName(TEXT("node"), P.Node);
			ReadPositive(TEXT("grow_days"), P.TreeGrowDays);
		}
		else if (!Kind.IsEmpty())
		{
			F.Error(FString::Printf(TEXT("kind \"%s\" unbekannt (erlaubt: field, composter, water, tree)"), *Kind));
		}
	}

	void ParseBuildables(const TArray<FRow>& Rows, FGamerDataSet& Out, FGamerDataReport& Report, bool bCheckAssetPaths)
	{
		// Where a structure snaps to (sockets in GamerBuildSnapping.cpp). wall_door = wall with a door opening
		// (also offers a socket for the door), gable/ridge = roof parts that sit differently from roof panels.
		static const TCHAR* const SnapTypes[] = { TEXT("foundation"), TEXT("floor"), TEXT("wall"), TEXT("wall_door"), TEXT("door"),
			TEXT("pillar"), TEXT("beam"), TEXT("roof"), TEXT("gable"), TEXT("ridge"), TEXT("stairs") };

		for (const FRow& Source : Rows)
		{
			FRowReader Row(Source.Object, Source.Context, Report);
			Row.CheckKnownFields({ TEXT("id"), TEXT("name"), TEXT("category"), TEXT("snap"), TEXT("mesh"), TEXT("cost"),
				TEXT("build_seconds"), TEXT("required_tool"), TEXT("farm"), TEXT("workers"), TEXT("upgrades"), TEXT("requires"), TEXT("tavern"),
				TEXT("deprecated") });

			FGamerBuildableDef Def;
			if (!ReadId(Row, { TEXT("building_") }, Def.Id))
			{
				continue;
			}
			Row.GetString(TEXT("name"), Def.Name, true);

			FString Category;
			if (Row.GetString(TEXT("category"), Category, true))
			{
				if (Category == TEXT("structure")) { Def.Category = EGamerBuildCategory::Structure; }
				else if (Category == TEXT("workstation")) { Def.Category = EGamerBuildCategory::Workstation; }
				else if (Category == TEXT("storage")) { Def.Category = EGamerBuildCategory::Storage; }
				else if (Category == TEXT("comfort")) { Def.Category = EGamerBuildCategory::Comfort; }
				else if (Category == TEXT("farm")) { Def.Category = EGamerBuildCategory::Farm; }
				else { Row.Error(FString::Printf(TEXT("category \"%s\" unbekannt (erlaubt: structure, workstation, storage, comfort, farm)"), *Category)); }
			}

			FString Snap;
			const bool bIsStructure = Def.Category == EGamerBuildCategory::Structure;
			if (Row.GetString(TEXT("snap"), Snap, bIsStructure))
			{
				bool bKnown = false;
				for (const TCHAR* Type : SnapTypes)
				{
					bKnown |= Snap == Type;
				}
				if (!bKnown)
				{
					Row.Error(FString::Printf(TEXT("snap \"%s\" unbekannt (erlaubt: foundation, floor, wall, wall_door, door, pillar, beam, roof, gable, ridge, stairs)"), *Snap));
				}
				else if (!bIsStructure)
				{
					Row.Warning(TEXT("snap wird nur bei category \"structure\" benutzt"));
				}
				Def.Snap = FName(*Snap);
			}

			ReadAssetPath(Row, TEXT("mesh"), Def.Mesh, bCheckAssetPaths);
			ReadAmounts(Row, TEXT("cost"), Def.Cost, true);

			double Seconds = 0.0;
			if (Row.GetNumber(TEXT("build_seconds"), Seconds, true))
			{
				if (Seconds <= 0.0)
				{
					Row.Error(TEXT("build_seconds muss größer als 0 sein"));
				}
				Def.BuildSeconds = static_cast<float>(Seconds);
			}
			ReadToolRequirement(Row, TEXT("required_tool"), Def.RequiredTool);
			ReadFarm(Row, Def);
			// Work places and upgrade stages (decision 2026-11-14).
			if (Row.GetInt(TEXT("workers"), Def.Workers, false) && Def.Workers < 0)
			{
				Row.Error(TEXT("workers darf nicht negativ sein"));
			}
			if (const TArray<TSharedPtr<FJsonValue>>* Upgrades = Row.GetArray(TEXT("upgrades"), false))
			{
				for (int32 Index = 0; Index < Upgrades->Num(); ++Index)
				{
					const TSharedPtr<FJsonValue>& Value = (*Upgrades)[Index];
					if (!Value.IsValid() || Value->Type != EJson::Object)
					{
						Row.Error(FString::Printf(TEXT("upgrades[%d] muss ein Objekt sein"), Index + 1));
						continue;
					}
					FRowReader Up(Value->AsObject(), FString::Printf(TEXT("%s, upgrades[%d]"), *Row.GetContext(), Index + 1), Report);
					Up.CheckKnownFields({ TEXT("cost"), TEXT("workers") });
					FGamerUpgradeDef& Stage = Def.Upgrades.AddDefaulted_GetRef();
					ReadAmounts(Up, TEXT("cost"), Stage.Cost, true);
					const int32 Previous = Def.WorkersAt(Index + 1);
					if (Up.GetInt(TEXT("workers"), Stage.Workers, true) && Stage.Workers <= Previous)
					{
						Up.Error(FString::Printf(TEXT("workers muss mehr sein als auf der Stufe davor (%d)"), Previous));
					}
				}
				if (Def.Workers <= 0 && Def.Upgrades.Num() > 0)
				{
					Row.Error(TEXT("upgrades braucht workers (Arbeitsplätze auf Stufe 1)"));
				}
			}
			// Tavern and cooking places (decision 2026-11-14, docs/12_taverne_gerichte.md).
			if (const TSharedPtr<FJsonObject> Requires = Row.GetObject(TEXT("requires"), false))
			{
				FRowReader Req(Requires, Row.GetContext() + TEXT(", requires"), Report);
				Req.CheckKnownFields({ TEXT("building"), TEXT("level"), TEXT("radius_cm") });
				FString Building;
				if (Req.GetString(TEXT("building"), Building, true))
				{
					Def.Requires.Building = FName(*Building);
				}
				if (Req.GetInt(TEXT("level"), Def.Requires.Level, false) && Def.Requires.Level < 1)
				{
					Req.Error(TEXT("level muss mindestens 1 sein"));
				}
				double Radius = 0.0;
				if (Req.GetNumber(TEXT("radius_cm"), Radius, false))
				{
					if (Radius <= 0.0)
					{
						Req.Error(TEXT("radius_cm muss größer als 0 sein"));
					}
					Def.Requires.RadiusCm = static_cast<float>(Radius);
				}
			}
			if (const TSharedPtr<FJsonObject> Tavern = Row.GetObject(TEXT("tavern"), false))
			{
				FRowReader Tav(Tavern, Row.GetContext() + TEXT(", tavern"), Report);
				Tav.CheckKnownFields({ TEXT("stock"), TEXT("mood_per_tier"), TEXT("mood_drink") });
				if (Tav.GetInt(TEXT("stock"), Def.Tavern.Stock, true) && Def.Tavern.Stock < 1)
				{
					Tav.Error(TEXT("stock muss mindestens 1 sein"));
				}
				double Mood = 0.0;
				if (Tav.GetNumber(TEXT("mood_per_tier"), Mood, false))
				{
					Def.Tavern.MoodPerTier = static_cast<float>(Mood);
				}
				if (Tav.GetNumber(TEXT("mood_drink"), Mood, false))
				{
					Def.Tavern.MoodDrink = static_cast<float>(Mood);
				}
				if (Def.Tavern.MoodPerTier < 0.f || Def.Tavern.MoodDrink < 0.f || Def.Tavern.MoodPerTier > 1.f || Def.Tavern.MoodDrink > 1.f)
				{
					Tav.Error(TEXT("mood_per_tier und mood_drink müssen zwischen 0 und 1 liegen"));
				}
			}
			Row.GetBool(TEXT("deprecated"), Def.bDeprecated);

			if (Out.Buildables.Contains(Def.Id))
			{
				Row.Error(TEXT("ID doppelt vergeben"));
				continue;
			}
			Out.Buildables.Add(Def.Id, MoveTemp(Def));
		}
	}

	/** Second pass: every reference must point to an existing row. */
	void ParseJobs(const TArray<FRow>& Rows, FGamerDataSet& Out, FGamerDataReport& Report)
	{
		int32 Fallbacks = 0;
		for (const FRow& Source : Rows)
		{
			FRowReader Row(Source.Object, Source.Context, Report);
			Row.CheckKnownFields({ TEXT("id"), TEXT("name"), TEXT("kind"), TEXT("station"), TEXT("tool"), TEXT("recipes"),
				TEXT("stock_target"), TEXT("radius"), TEXT("fallback"), TEXT("deprecated") });

			FGamerJobDef Def;
			if (!ReadId(Row, { TEXT("job_") }, Def.Id))
			{
				continue;
			}
			Row.GetString(TEXT("name"), Def.Name, true);
			FString Kind;
			if (Row.GetString(TEXT("kind"), Kind, true))
			{
				if (Kind == TEXT("harvest")) { Def.Kind = EGamerJobKind::Harvest; }
				else if (Kind == TEXT("craft")) { Def.Kind = EGamerJobKind::Craft; }
				else if (Kind == TEXT("build")) { Def.Kind = EGamerJobKind::Build; }
				else if (Kind == TEXT("farm")) { Def.Kind = EGamerJobKind::Farm; }
				else if (Kind == TEXT("serve")) { Def.Kind = EGamerJobKind::Serve; }
				else { Row.Error(FString::Printf(TEXT("kind \"%s\" unbekannt (erlaubt: harvest, craft, build, farm, serve)"), *Kind)); }
			}
			FString Station;
			if (Row.GetString(TEXT("station"), Station, false))
			{
				Def.Station = FName(*Station);
			}
			FString Tool;
			if (Row.GetString(TEXT("tool"), Tool, false))
			{
				Def.Tool = FName(*Tool);
			}
			if (const TArray<TSharedPtr<FJsonValue>>* Recipes = Row.GetArray(TEXT("recipes"), Def.Kind == EGamerJobKind::Craft))
			{
				for (const TSharedPtr<FJsonValue>& Value : *Recipes)
				{
					FString RecipeId;
					if (Value.IsValid() && Value->TryGetString(RecipeId))
					{
						Def.Recipes.Add(FName(*RecipeId));
					}
					else
					{
						Row.Error(TEXT("\"recipes\" darf nur Rezept-IDs als Text enthalten"));
					}
				}
			}
			int32 Stock = 0;
			if (Row.GetInt(TEXT("stock_target"), Stock, false))
			{
				if (Stock < 1)
				{
					Row.Error(TEXT("stock_target muss mindestens 1 sein"));
				}
				Def.StockTarget = Stock;
			}
			double Radius = 0.0;
			if (Row.GetNumber(TEXT("radius"), Radius, false))
			{
				if (Radius <= 0.0)
				{
					Row.Error(TEXT("radius muss größer als 0 sein"));
				}
				Def.Radius = static_cast<float>(Radius);
			}
			Row.GetBool(TEXT("fallback"), Def.bFallback);
			Row.GetBool(TEXT("deprecated"), Def.bDeprecated);
			Fallbacks += Def.bFallback ? 1 : 0;

			if (Out.Jobs.Contains(Def.Id))
			{
				Row.Error(TEXT("ID doppelt vergeben"));
				continue;
			}
			Out.Jobs.Add(Def.Id, MoveTemp(Def));
		}
		if (Rows.Num() > 0 && Fallbacks != 1)
		{
			Report.Errors.Add(FString::Printf(TEXT("%s: genau ein Beruf braucht \"fallback\": true (Ausweich-Beruf), gefunden: %d"), JobsFile, Fallbacks));
		}
	}

	void ParseNeeds(const TArray<FRow>& Rows, FGamerDataSet& Out, FGamerDataReport& Report)
	{
		for (const FRow& Source : Rows)
		{
			FRowReader Row(Source.Object, Source.Context, Report);
			Row.CheckKnownFields({ TEXT("id"), TEXT("name"), TEXT("kind"), TEXT("decay_per_day"), TEXT("threshold"), TEXT("restore_per_day"),
				TEXT("foods"), TEXT("work_speed_when_unmet"), TEXT("leave_after_days"), TEXT("deprecated") });

			FGamerNeedDef Def;
			if (!ReadId(Row, { TEXT("need_") }, Def.Id))
			{
				continue;
			}
			Row.GetString(TEXT("name"), Def.Name, true);
			FString Kind;
			if (Row.GetString(TEXT("kind"), Kind, true))
			{
				if (Kind == TEXT("food")) { Def.Kind = EGamerNeedKind::Food; }
				else if (Kind == TEXT("sleep")) { Def.Kind = EGamerNeedKind::Sleep; }
				else if (Kind == TEXT("shelter")) { Def.Kind = EGamerNeedKind::Shelter; }
				else { Row.Error(FString::Printf(TEXT("kind \"%s\" unbekannt (erlaubt: food, sleep, shelter)"), *Kind)); }
			}
			auto ReadFraction = [&Row](const TCHAR* Field, float& OutValue, double Min, double Max)
			{
				double Value = 0.0;
				if (Row.GetNumber(Field, Value, false))
				{
					if (Value < Min || Value > Max)
					{
						Row.Error(FString::Printf(TEXT("%s muss zwischen %g und %g liegen"), Field, Min, Max));
					}
					OutValue = static_cast<float>(Value);
				}
			};
			ReadFraction(TEXT("decay_per_day"), Def.DecayPerDay, 0.0, 100.0);
			ReadFraction(TEXT("threshold"), Def.Threshold, 0.0, 1.0);
			ReadFraction(TEXT("restore_per_day"), Def.RestorePerDay, 0.0, 1000.0);
			ReadFraction(TEXT("work_speed_when_unmet"), Def.WorkSpeedWhenUnmet, 0.1, 1.0);
			ReadFraction(TEXT("leave_after_days"), Def.LeaveAfterDays, 0.0, 1000.0);
			if (const TArray<TSharedPtr<FJsonValue>>* Foods = Row.GetArray(TEXT("foods"), Def.Kind == EGamerNeedKind::Food))
			{
				for (const TSharedPtr<FJsonValue>& Value : *Foods)
				{
					const TSharedPtr<FJsonObject> Food = Value.IsValid() && Value->Type == EJson::Object ? Value->AsObject() : nullptr;
					FString Item;
					double Amount = 0.0;
					if (!Food || !Food->TryGetStringField(TEXT("item"), Item) || !Food->TryGetNumberField(TEXT("value"), Amount))
					{
						Row.Error(TEXT("\"foods\": jeder Eintrag braucht { \"item\": …, \"value\": … }"));
						continue;
					}
					if (Amount <= 0.0 || Amount > 1.0)
					{
						Row.Error(FString::Printf(TEXT("\"foods\": value von %s muss zwischen 0 und 1 liegen"), *Item));
					}
					FGamerFoodValue& Entry = Def.Foods.AddDefaulted_GetRef();
					Entry.Item = FName(*Item);
					Entry.Value = static_cast<float>(Amount);
				}
			}
			Row.GetBool(TEXT("deprecated"), Def.bDeprecated);

			if (Out.Needs.Contains(Def.Id))
			{
				Row.Error(TEXT("ID doppelt vergeben"));
				continue;
			}
			Out.Needs.Add(Def.Id, MoveTemp(Def));
		}
	}

	void CheckVillagerReferences(const FGamerDataSet& Data, FGamerDataReport& Report)
	{
		for (const TPair<FName, FGamerJobDef>& Pair : Data.Jobs)
		{
			const FGamerJobDef& Job = Pair.Value;
			const FString Where = FString::Printf(TEXT("%s (%s)"), JobsFile, *Job.Id.ToString());
			if (!Job.Station.IsNone() && !Data.Buildables.Contains(Job.Station))
			{
				Report.Errors.Add(FString::Printf(TEXT("%s: station \"%s\" gibt es nicht"), *Where, *Job.Station.ToString()));
			}
			if (!Job.Tool.IsNone())
			{
				bool bToolExists = false;
				for (const TPair<FName, FGamerItemDef>& Item : Data.Items)
				{
					bToolExists |= Item.Value.Category == EGamerItemCategory::Tool && Item.Value.Tool.Type == Job.Tool;
				}
				if (!bToolExists)
				{
					Report.Errors.Add(FString::Printf(TEXT("%s: kein Werkzeug vom Typ \"%s\""), *Where, *Job.Tool.ToString()));
				}
			}
			for (const FName RecipeId : Job.Recipes)
			{
				const FGamerRecipeDef* Recipe = Data.Recipes.Find(RecipeId);
				if (!Recipe)
				{
					Report.Errors.Add(FString::Printf(TEXT("%s: Rezept \"%s\" gibt es nicht"), *Where, *RecipeId.ToString()));
				}
				else if (Recipe->Station != Job.Station)
				{
					// Cooking places that belong to the job's building (requires) are fine; the campfire may stand near the tavern too.
					const FGamerBuildableDef* RecipeStation = Data.Buildables.Find(Recipe->Station);
					const bool bBelongs = RecipeStation && RecipeStation->Requires.Building == Job.Station;
					if (!bBelongs && Recipe->Station.IsNone())
					{
						Report.Warnings.Add(FString::Printf(TEXT("%s: Rezept %s gehört zu einer anderen Station"), *Where, *RecipeId.ToString()));
					}
				}
			}
			if (Job.Kind == EGamerJobKind::Serve)
			{
				const FGamerBuildableDef* Station = Data.Buildables.Find(Job.Station);
				if (!Station || !Station->Tavern.IsTavern())
				{
					Report.Errors.Add(FString::Printf(TEXT("%s: Beruf \"serve\" braucht als station eine Taverne (Block \"tavern\")"), *Where));
				}
			}
		}
		for (const TPair<FName, FGamerNeedDef>& Pair : Data.Needs)
		{
			for (const FGamerFoodValue& Food : Pair.Value.Foods)
			{
				const FGamerItemDef* Item = Data.Items.Find(Food.Item);
				if (!Item)
				{
					Report.Errors.Add(FString::Printf(TEXT("%s (%s): Gegenstand \"%s\" gibt es nicht"), NeedsFile, *Pair.Key.ToString(), *Food.Item.ToString()));
				}
				else if (Item->Category != EGamerItemCategory::Food)
				{
					Report.Warnings.Add(FString::Printf(TEXT("%s (%s): %s ist kein Essen (category)"), NeedsFile, *Pair.Key.ToString(), *Food.Item.ToString()));
				}
			}
		}
	}

	void CheckReferences(const FGamerDataSet& Data, FGamerDataReport& Report)
	{
		// Tool types that exist, with the highest tier available for each.
		TMap<FName, int32> ToolTypes;
		for (const TPair<FName, FGamerItemDef>& Pair : Data.Items)
		{
			if (Pair.Value.Category == EGamerItemCategory::Tool && !Pair.Value.Tool.Type.IsNone())
			{
				int32& Best = ToolTypes.FindOrAdd(Pair.Value.Tool.Type, 0);
				Best = FMath::Max(Best, Pair.Value.Tier);
			}
		}

		auto CheckItem = [&](const FString& Where, FName Item)
		{
			const FGamerItemDef* Def = Data.Items.Find(Item);
			if (!Def)
			{
				Report.Errors.Add(FString::Printf(TEXT("%s: Gegenstand \"%s\" gibt es nicht"), *Where, *Item.ToString()));
			}
			else if (Def->bDeprecated)
			{
				Report.Warnings.Add(FString::Printf(TEXT("%s: Gegenstand \"%s\" ist als veraltet markiert"), *Where, *Item.ToString()));
			}
		};
		auto CheckTool = [&](const FString& Where, const FGamerToolRequirement& Required)
		{
			if (!Required.IsRequired())
			{
				return;
			}
			const int32* BestTier = ToolTypes.Find(Required.Type);
			if (!BestTier)
			{
				Report.Errors.Add(FString::Printf(TEXT("%s: Werkzeugtyp \"%s\" gibt es bei keinem Werkzeug"), *Where, *Required.Type.ToString()));
			}
			else if (*BestTier < Required.MinTier)
			{
				Report.Errors.Add(FString::Printf(TEXT("%s: braucht %s ab Stufe %d, das beste Werkzeug hat Stufe %d"),
					*Where, *Required.Type.ToString(), Required.MinTier, *BestTier));
			}
		};

		for (const TPair<FName, FGamerItemDef>& Pair : Data.Items)
		{
			const FString Where = FString::Printf(TEXT("%s (%s)"), ItemsFile, *Pair.Key.ToString());
			if (Pair.Value.SpoilDays > 0.f)
			{
				CheckItem(Where + TEXT(", spoils_into"), Pair.Value.SpoilsInto);
			}
			for (const FGamerDrop& Drop : Pair.Value.Plant.Harvest)
			{
				CheckItem(Where + TEXT(", plant.harvest"), Drop.Item);
			}
		}
		for (const TPair<FName, FGamerResourceNodeDef>& Pair : Data.ResourceNodes)
		{
			const FString Where = FString::Printf(TEXT("%s (%s)"), NodesFile, *Pair.Key.ToString());
			CheckTool(Where, Pair.Value.RequiredTool);
			for (const FGamerDrop& Drop : Pair.Value.Drops)
			{
				CheckItem(Where, Drop.Item);
			}
			if (!Pair.Value.Season.BruisedItem.IsNone())
			{
				CheckItem(Where + TEXT(", season.bruised_item"), Pair.Value.Season.BruisedItem);
			}
		}
		for (const TPair<FName, FGamerRecipeDef>& Pair : Data.Recipes)
		{
			const FString Where = FString::Printf(TEXT("%s (%s)"), RecipesFile, *Pair.Key.ToString());
			if (!Pair.Value.Station.IsNone())
			{
				const FGamerBuildableDef* Station = Data.Buildables.Find(Pair.Value.Station);
				if (!Station)
				{
					Report.Errors.Add(FString::Printf(TEXT("%s: Station \"%s\" gibt es nicht"), *Where, *Pair.Value.Station.ToString()));
				}
				else if (Station->Category == EGamerBuildCategory::Structure)
				{
					Report.Errors.Add(FString::Printf(TEXT("%s: \"%s\" ist ein Bauteil, keine Station"), *Where, *Pair.Value.Station.ToString()));
				}
			}
			for (const FGamerItemAmount& Amount : Pair.Value.Inputs)
			{
				CheckItem(Where, Amount.Item);
			}
			for (const FGamerItemAmount& Amount : Pair.Value.Outputs)
			{
				CheckItem(Where, Amount.Item);
			}
		}
		for (const TPair<FName, FGamerBuildableDef>& Pair : Data.Buildables)
		{
			const FString Where = FString::Printf(TEXT("%s (%s)"), BuildablesFile, *Pair.Key.ToString());
			CheckTool(Where, Pair.Value.RequiredTool);
			for (const FGamerItemAmount& Amount : Pair.Value.Cost)
			{
				CheckItem(Where, Amount.Item);
			}
			const FGamerFarmProps& Farm = Pair.Value.Farm;
			for (const FName Item : { Farm.WaterItem, Farm.FertilizerItem, Farm.Output })
			{
				if (!Item.IsNone())
				{
					CheckItem(Where + TEXT(", farm"), Item);
				}
			}
			for (const FName Item : Farm.Inputs)
			{
				CheckItem(Where + TEXT(", farm.inputs"), Item);
			}
			if (Farm.Kind == EGamerFarmKind::Tree)
			{
				const FGamerResourceNodeDef* Node = Data.ResourceNodes.Find(Farm.Node);
				if (!Node)
				{
					Report.Errors.Add(FString::Printf(TEXT("%s, farm.node: Vorkommen \"%s\" gibt es nicht"), *Where, *Farm.Node.ToString()));
				}
				else if (Node->MeshGrowing.Num() == 0)
				{
					Report.Warnings.Add(FString::Printf(TEXT("%s, farm.node: %s hat keine Wachstumsmodelle (meshes.growing)"), *Where, *Farm.Node.ToString()));
				}
			}
			for (const FGamerUpgradeDef& Stage : Pair.Value.Upgrades)
			{
				for (const FGamerItemAmount& Amount : Stage.Cost)
				{
					CheckItem(Where + TEXT(", upgrades"), Amount.Item);
				}
			}
			if (const FGamerBuildRequirement& Req = Pair.Value.Requires; Req.IsSet())
			{
				const FGamerBuildableDef* Needed = Data.Buildables.Find(Req.Building);
				if (!Needed)
				{
					Report.Errors.Add(FString::Printf(TEXT("%s, requires: Gebäude \"%s\" gibt es nicht"), *Where, *Req.Building.ToString()));
				}
				else if (Req.Level > Needed->MaxLevel())
				{
					Report.Errors.Add(FString::Printf(TEXT("%s, requires: %s hat nur %d Stufen, verlangt ist Stufe %d"), *Where,
						*Req.Building.ToString(), Needed->MaxLevel(), Req.Level));
				}
				else if (Req.Building == Pair.Key)
				{
					Report.Errors.Add(FString::Printf(TEXT("%s, requires: verweist auf sich selbst"), *Where));
				}
			}
		}
	}
}

namespace
{
	/**
	 * Plays the game forward from an empty inventory: gather what needs no (or an already obtainable) tool,
	 * build what can be paid for, craft what has its station and inputs, repeat until nothing new appears.
	 * Everything that never becomes reachable is an error (e.g. "axe needs a log, logs need an axe").
	 */
	/** StartItems: start equipment of the origins (characters.json); only a warning when that is the only source. */
	void CheckReachability(const FGamerDataSet& Data, FGamerDataReport& Report, const TSet<FName>& StartItems)
	{
		TSet<FName> Items;
		TMap<FName, int32> ToolTiers;
		TSet<FName> Nodes;
		TSet<FName> Buildables;
		TSet<FName> Recipes;

		auto ToolAvailable = [&](const FGamerToolRequirement& Required)
		{
			if (!Required.IsRequired())
			{
				return true;
			}
			const int32* Tier = ToolTiers.Find(Required.Type);
			return Tier && *Tier >= Required.MinTier;
		};
		auto HaveAll = [&](const TArray<FGamerItemAmount>& Amounts)
		{
			for (const FGamerItemAmount& Amount : Amounts)
			{
				if (!Items.Contains(Amount.Item))
				{
					return false;
				}
			}
			return true;
		};
		auto Obtain = [&](FName Item)
		{
			Items.Add(Item);
			if (const FGamerItemDef* Def = Data.Items.Find(Item))
			{
				if (Def->Category == EGamerItemCategory::Tool && !Def->Tool.Type.IsNone())
				{
					int32& Best = ToolTiers.FindOrAdd(Def->Tool.Type, 0);
					Best = FMath::Max(Best, Def->Tier);
				}
			}
		};

		bool bChanged = true;
		while (bChanged)
		{
			bChanged = false;
			for (const TPair<FName, FGamerResourceNodeDef>& Pair : Data.ResourceNodes)
			{
				if (!Nodes.Contains(Pair.Key) && ToolAvailable(Pair.Value.RequiredTool))
				{
					Nodes.Add(Pair.Key);
					for (const FGamerDrop& Drop : Pair.Value.Drops)
					{
						if (Drop.Max > 0)
						{
							Obtain(Drop.Item);
						}
					}
					bChanged = true;
				}
			}
			// Required building (tavern) with its upgrade stages up to the needed level.
			auto RequirementMet = [&](const FGamerBuildRequirement& Req)
			{
				if (!Req.IsSet())
				{
					return true;
				}
				const FGamerBuildableDef* Needed = Data.Buildables.Find(Req.Building);
				if (!Needed || !Buildables.Contains(Req.Building))
				{
					return false;
				}
				for (int32 Stage = 0; Stage < Req.Level - 1; ++Stage)
				{
					if (!Needed->Upgrades.IsValidIndex(Stage) || !HaveAll(Needed->Upgrades[Stage].Cost))
					{
						return false;
					}
				}
				return true;
			};
			for (const TPair<FName, FGamerBuildableDef>& Pair : Data.Buildables)
			{
				if (!Buildables.Contains(Pair.Key) && ToolAvailable(Pair.Value.RequiredTool) && HaveAll(Pair.Value.Cost)
					&& RequirementMet(Pair.Value.Requires))
				{
					Buildables.Add(Pair.Key);
					bChanged = true;
				}
			}
			// Farming: spoiling, bruised fruit, fields, composter, rain catcher.
			auto ObtainNew = [&](FName Item)
			{
				if (!Item.IsNone() && !Items.Contains(Item))
				{
					Obtain(Item);
					bChanged = true;
				}
			};
			for (const TPair<FName, FGamerItemDef>& Pair : Data.Items)
			{
				if (Items.Contains(Pair.Key) && Pair.Value.SpoilDays > 0.f)
				{
					ObtainNew(Pair.Value.SpoilsInto);
				}
			}
			for (const FName NodeId : Nodes.Array())
			{
				ObtainNew(Data.ResourceNodes[NodeId].Season.BruisedItem);
			}
			for (const FName BuildableId : Buildables.Array())
			{
				const FGamerFarmProps& Farm = Data.Buildables[BuildableId].Farm;
				if (Farm.Kind == EGamerFarmKind::Water)
				{
					ObtainNew(Farm.Output);
				}
				else if (Farm.Kind == EGamerFarmKind::Composter && Farm.Inputs.ContainsByPredicate([&](FName In) { return Items.Contains(In); }))
				{
					ObtainNew(Farm.Output);
				}
				else if (Farm.Kind == EGamerFarmKind::Field && Items.Contains(Farm.WaterItem))
				{
					for (const TPair<FName, FGamerItemDef>& Seed : Data.Items)
					{
						if (Seed.Value.Plant.IsSeed() && Items.Contains(Seed.Key))
						{
							for (const FGamerDrop& Drop : Seed.Value.Plant.Harvest)
							{
								ObtainNew(Drop.Item);
							}
						}
					}
				}
			}
			for (const TPair<FName, FGamerRecipeDef>& Pair : Data.Recipes)
			{
				const bool bStationOk = Pair.Value.Station.IsNone() || Buildables.Contains(Pair.Value.Station);
				if (!Recipes.Contains(Pair.Key) && bStationOk && HaveAll(Pair.Value.Inputs))
				{
					Recipes.Add(Pair.Key);
					for (const FGamerItemAmount& Output : Pair.Value.Outputs)
					{
						Obtain(Output.Item);
					}
					bChanged = true;
				}
			}
		}

		for (const TPair<FName, FGamerItemDef>& Pair : Data.Items)
		{
			if (!Pair.Value.bDeprecated && !Items.Contains(Pair.Key) && StartItems.Contains(Pair.Key))
			{
				Report.Warnings.Add(FString::Printf(TEXT("%s (%s): nur als Startausrüstung erhältlich (weder sammelbar noch herstellbar)"), ItemsFile, *Pair.Key.ToString()));
			}
			else if (!Pair.Value.bDeprecated && !Items.Contains(Pair.Key))
			{
				Report.Errors.Add(FString::Printf(TEXT("%s (%s): nicht erreichbar ab Spielstart (weder sammelbar noch herstellbar)"), ItemsFile, *Pair.Key.ToString()));
			}
		}
		for (const TPair<FName, FGamerResourceNodeDef>& Pair : Data.ResourceNodes)
		{
			if (!Pair.Value.bDeprecated && !Nodes.Contains(Pair.Key))
			{
				Report.Errors.Add(FString::Printf(TEXT("%s (%s): nicht erreichbar ab Spielstart (Werkzeug %s ab Stufe %d bekommt man nie)"),
					NodesFile, *Pair.Key.ToString(), *Pair.Value.RequiredTool.Type.ToString(), Pair.Value.RequiredTool.MinTier));
			}
		}
		for (const TPair<FName, FGamerBuildableDef>& Pair : Data.Buildables)
		{
			if (!Pair.Value.bDeprecated && !Buildables.Contains(Pair.Key))
			{
				Report.Errors.Add(FString::Printf(TEXT("%s (%s): nicht erreichbar ab Spielstart (Kosten oder Werkzeug nie verfügbar)"), BuildablesFile, *Pair.Key.ToString()));
			}
		}
		for (const TPair<FName, FGamerRecipeDef>& Pair : Data.Recipes)
		{
			if (!Pair.Value.bDeprecated && !Recipes.Contains(Pair.Key))
			{
				Report.Errors.Add(FString::Printf(TEXT("%s (%s): nicht erreichbar ab Spielstart (Station oder Zutaten nie verfügbar)"), RecipesFile, *Pair.Key.ToString()));
			}
		}
	}
}

void FGamerDataSet::Reset()
{
	Items.Reset();
	ResourceNodes.Reset();
	Recipes.Reset();
	Buildables.Reset();
	Jobs.Reset();
	Needs.Reset();
	Checksum = 0;
}

FString GamerData::DefaultDirectory()
{
	return FPaths::Combine(FPaths::ProjectContentDir(), TEXT("Data"));
}

bool GamerData::LoadFromDirectory(const FString& Directory, FGamerDataSet& Out, FGamerDataReport& Report, bool bCheckAssetPaths)
{
	Out.Reset();
	const int32 ErrorsBefore = Report.Errors.Num();

	uint32 Crc = 0;
	TArray<FRow> ItemRows, NodeRows, RecipeRows, BuildableRows, JobRows, NeedRows;
	ReadTable(Directory, ItemsFile, ItemRows, Crc, Report);
	ReadTable(Directory, NodesFile, NodeRows, Crc, Report);
	ReadTable(Directory, RecipesFile, RecipeRows, Crc, Report);
	ReadTable(Directory, BuildablesFile, BuildableRows, Crc, Report);
	ReadTable(Directory, JobsFile, JobRows, Crc, Report, true);
	ReadTable(Directory, NeedsFile, NeedRows, Crc, Report, true);
	Out.Checksum = Crc;

	ParseItems(ItemRows, Out, Report, bCheckAssetPaths);
	ParseNodes(NodeRows, Out, Report, bCheckAssetPaths);
	ParseRecipes(RecipeRows, Out, Report);
	ParseBuildables(BuildableRows, Out, Report, bCheckAssetPaths);
	ParseJobs(JobRows, Out, Report);
	ParseNeeds(NeedRows, Out, Report);
	CheckReferences(Out, Report);
	CheckVillagerReferences(Out, Report);
	TSet<FName> StartItems;
	GamerData::ReadStartItems(Directory, StartItems); // characters.json, if the data set has one
	CheckReachability(Out, Report, StartItems);

	return Report.Errors.Num() == ErrorsBefore;
}
