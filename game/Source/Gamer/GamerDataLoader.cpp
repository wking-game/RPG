#include "GamerDataLoader.h"

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
	bool ReadTable(const FString& Directory, const TCHAR* FileName, TArray<FRow>& OutRows, uint32& InOutCrc, FGamerDataReport& Report)
	{
		const FString Path = FPaths::Combine(Directory, FileName);
		FString Text;
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

	void ParseItems(const TArray<FRow>& Rows, FGamerDataSet& Out, FGamerDataReport& Report, bool bCheckAssetPaths)
	{
		for (const FRow& Source : Rows)
		{
			FRowReader Row(Source.Object, Source.Context, Report);
			Row.CheckKnownFields({ TEXT("id"), TEXT("name"), TEXT("category"), TEXT("tier"), TEXT("max_stack"), TEXT("weight_kg"),
				TEXT("icon"), TEXT("world_mesh"), TEXT("tool"), TEXT("food"), TEXT("deprecated") });

			FGamerItemDef Def;
			if (!ReadId(Row, { TEXT("item_"), TEXT("tool_") }, Def.Id))
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
				else { Row.Error(FString::Printf(TEXT("category \"%s\" unbekannt (erlaubt: resource, material, food, tool)"), *Category)); }
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
			if (Row.Has(TEXT("food")))
			{
				Row.Warning(TEXT("\"food\" wird erst mit dem Buff-System (Phase 2) ausgewertet"));
			}

			if (Out.Items.Contains(Def.Id))
			{
				Row.Error(TEXT("ID doppelt vergeben"));
				continue;
			}
			Out.Items.Add(Def.Id, MoveTemp(Def));
		}
	}

	void ParseNodes(const TArray<FRow>& Rows, FGamerDataSet& Out, FGamerDataReport& Report, bool bCheckAssetPaths)
	{
		for (const FRow& Source : Rows)
		{
			FRowReader Row(Source.Object, Source.Context, Report);
			Row.CheckKnownFields({ TEXT("id"), TEXT("name"), TEXT("required_tool"), TEXT("work_seconds"), TEXT("drops"),
				TEXT("regrow_days"), TEXT("meshes"), TEXT("deprecated") });

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

			if (const TArray<TSharedPtr<FJsonValue>>* Drops = Row.GetArray(TEXT("drops"), true))
			{
				if (Drops->Num() == 0)
				{
					Row.Error(TEXT("\"drops\" ist leer"));
				}
				for (int32 Index = 0; Index < Drops->Num(); ++Index)
				{
					const TSharedPtr<FJsonValue>& Value = (*Drops)[Index];
					if (!Value.IsValid() || Value->Type != EJson::Object)
					{
						Row.Error(FString::Printf(TEXT("drops[%d] muss ein Objekt sein"), Index + 1));
						continue;
					}
					FRowReader DropRow(Value->AsObject(), FString::Printf(TEXT("%s, drops[%d]"), *Row.GetContext(), Index + 1), Report);
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
						Def.Drops.Add(Drop);
					}
				}
			}

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
				MeshRow.CheckKnownFields({ TEXT("full"), TEXT("depleted"), TEXT("remains") });
				ReadAssetPath(MeshRow, TEXT("full"), Def.MeshFull, bCheckAssetPaths);
				ReadAssetPath(MeshRow, TEXT("depleted"), Def.MeshDepleted, bCheckAssetPaths);
				ReadAssetPath(MeshRow, TEXT("remains"), Def.MeshRemains, bCheckAssetPaths);
			}
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

	void ParseBuildables(const TArray<FRow>& Rows, FGamerDataSet& Out, FGamerDataReport& Report, bool bCheckAssetPaths)
	{
		static const TCHAR* const SnapTypes[] = { TEXT("foundation"), TEXT("floor"), TEXT("wall"), TEXT("door"),
			TEXT("pillar"), TEXT("beam"), TEXT("roof"), TEXT("stairs") };

		for (const FRow& Source : Rows)
		{
			FRowReader Row(Source.Object, Source.Context, Report);
			Row.CheckKnownFields({ TEXT("id"), TEXT("name"), TEXT("category"), TEXT("snap"), TEXT("mesh"), TEXT("cost"),
				TEXT("build_seconds"), TEXT("required_tool"), TEXT("deprecated") });

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
				else { Row.Error(FString::Printf(TEXT("category \"%s\" unbekannt (erlaubt: structure, workstation, storage, comfort)"), *Category)); }
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
					Row.Error(FString::Printf(TEXT("snap \"%s\" unbekannt (erlaubt: foundation, floor, wall, door, pillar, beam, roof, stairs)"), *Snap));
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

		for (const TPair<FName, FGamerResourceNodeDef>& Pair : Data.ResourceNodes)
		{
			const FString Where = FString::Printf(TEXT("%s (%s)"), NodesFile, *Pair.Key.ToString());
			CheckTool(Where, Pair.Value.RequiredTool);
			for (const FGamerDrop& Drop : Pair.Value.Drops)
			{
				CheckItem(Where, Drop.Item);
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
		}
	}
}

void FGamerDataSet::Reset()
{
	Items.Reset();
	ResourceNodes.Reset();
	Recipes.Reset();
	Buildables.Reset();
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
	TArray<FRow> ItemRows, NodeRows, RecipeRows, BuildableRows;
	ReadTable(Directory, ItemsFile, ItemRows, Crc, Report);
	ReadTable(Directory, NodesFile, NodeRows, Crc, Report);
	ReadTable(Directory, RecipesFile, RecipeRows, Crc, Report);
	ReadTable(Directory, BuildablesFile, BuildableRows, Crc, Report);
	Out.Checksum = Crc;

	ParseItems(ItemRows, Out, Report, bCheckAssetPaths);
	ParseNodes(NodeRows, Out, Report, bCheckAssetPaths);
	ParseRecipes(RecipeRows, Out, Report);
	ParseBuildables(BuildableRows, Out, Report, bCheckAssetPaths);
	CheckReferences(Out, Report);

	return Report.Errors.Num() == ErrorsBefore;
}
