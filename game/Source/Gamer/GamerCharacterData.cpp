#include "GamerCharacterData.h"

#include "GamerDataLoader.h"
#include "Dom/JsonObject.h"
#include "Dom/JsonValue.h"
#include "Misc/Crc.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"

// Validation messages are German on purpose: they are read by whoever edits characters.json.

void FGamerCharacterProfile::SetLook(FName Slot, FName Option)
{
	for (FGamerLookChoice& Choice : Appearance)
	{
		if (Choice.Slot == Slot)
		{
			Choice.Option = Option;
			return;
		}
	}
	Appearance.Add({ Slot, Option });
}

FString FGamerCharacterProfile::ToLogString() const
{
	FString Text = FString::Printf(TEXT("\"%s\" (%s)"), *Name, *Origin.ToString());
	for (const FGamerLookChoice& Choice : Appearance)
	{
		Text += FString::Printf(TEXT(" %s=%s"), *Choice.Slot.ToString(), *Choice.Option.ToString());
	}
	return Text;
}

int32 FGamerCharacterDataSet::AbilityCostFor(const FGamerAbilityDef& Ability, FName Origin) const
{
	const float Cost = Ability.Branch == Origin ? Ability.Cost * OwnBranchCostFactor : static_cast<float>(Ability.Cost);
	return FMath::Max(1, FMath::RoundToInt(Cost));
}

bool FGamerCharacterDataSet::Sanitize(FGamerCharacterProfile& Profile) const
{
	bool bOk = true;
	const FGamerOriginDef* Origin = FindOrigin(Profile.Origin);
	if (!Origin && Origins.Num() > 0)
	{
		Origin = &Origins[0];
		Profile.Origin = Origin->Id;
		bOk = false;
	}

	// Name: printable characters only, trimmed, limited length.
	FString Clean;
	for (const TCHAR C : Profile.Name)
	{
		if (FChar::IsAlnum(C) || C == TEXT(' ') || C == TEXT('-') || C == TEXT('\''))
		{
			Clean.AppendChar(C);
		}
	}
	Clean.TrimStartAndEndInline();
	if (Clean.Len() > NameMaxLength)
	{
		Clean.LeftInline(NameMaxLength);
		Clean.TrimEndInline();
	}
	if (Clean.IsEmpty())
	{
		Clean = Names.Num() > 0 ? Names[0] : FString(TEXT("Wanderer"));
	}
	if (Clean != Profile.Name)
	{
		Profile.Name = Clean;
		bOk = false;
	}

	// Appearance: exactly one known option per slot, in slot order.
	TArray<FGamerLookChoice> Looks;
	for (const FGamerLookSlotDef& Slot : LookSlots)
	{
		FName Option = Profile.GetLook(Slot.Id);
		if (!Slot.HasOption(Option))
		{
			bOk &= Option.IsNone(); // a missing slot (new in the data) is fine, an unknown option is not
			Option = NAME_None;
			if (Origin)
			{
				const FGamerLookChoice* Suggested = Origin->Appearance.FindByPredicate([&Slot](const FGamerLookChoice& C) { return C.Slot == Slot.Id; });
				Option = Suggested ? Suggested->Option : NAME_None;
			}
			if (!Slot.HasOption(Option) && Slot.Options.Num() > 0)
			{
				Option = Slot.Options[0].Id;
			}
		}
		Looks.Add({ Slot.Id, Option });
	}
	bOk &= Looks.Num() == Profile.Appearance.Num();
	Profile.Appearance = MoveTemp(Looks);
	return bOk;
}

FGamerCharacterProfile FGamerCharacterDataSet::MakeProfile(FName Origin, const FString& Name) const
{
	FGamerCharacterProfile Profile;
	Profile.Origin = Origin;
	Profile.Name = Name;
	if (const FGamerOriginDef* Def = FindOrigin(Origin))
	{
		Profile.Appearance = Def->Appearance;
	}
	Sanitize(Profile);
	return Profile;
}

namespace
{
	bool IsSnakeCaseId(const FString& S, const TCHAR* Prefix)
	{
		const int32 PrefixLen = FCString::Strlen(Prefix);
		if (S.Len() <= PrefixLen || (PrefixLen > 0 && !S.StartsWith(Prefix, ESearchCase::CaseSensitive)) || !FChar::IsLower(S[0]))
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

	/** Small reader with readable error locations ("characters.json, origins[2] (origin_smith)"). */
	struct FReader
	{
		TSharedPtr<FJsonObject> Object;
		FString Where;
		FGamerDataReport& Report;

		void Error(const FString& Message) const { Report.Errors.Add(FString::Printf(TEXT("%s: %s"), *Where, *Message)); }

		void CheckKnown(std::initializer_list<const TCHAR*> Allowed) const
		{
			for (const TPair<FString, TSharedPtr<FJsonValue>>& Pair : Object->Values)
			{
				bool bKnown = false;
				for (const TCHAR* Name : Allowed)
				{
					bKnown |= Pair.Key.Equals(Name, ESearchCase::CaseSensitive);
				}
				if (!bKnown)
				{
					Error(FString::Printf(TEXT("unbekanntes Feld \"%s\" (Tippfehler?)"), *Pair.Key));
				}
			}
		}

		bool String(const TCHAR* Field, FString& Out, bool bRequired) const
		{
			const TSharedPtr<FJsonValue> Value = Object->TryGetField(Field);
			if (!Value.IsValid() || Value->Type == EJson::Null)
			{
				if (bRequired)
				{
					Error(FString::Printf(TEXT("Pflichtfeld \"%s\" fehlt"), Field));
				}
				return false;
			}
			if (Value->Type != EJson::String || (bRequired && Value->AsString().IsEmpty()))
			{
				Error(FString::Printf(TEXT("\"%s\" muss ein nicht leerer Text sein"), Field));
				return false;
			}
			Out = Value->AsString();
			return true;
		}

		bool Number(const TCHAR* Field, double& Out, bool bRequired, double Min, double Max, bool bWhole) const
		{
			const TSharedPtr<FJsonValue> Value = Object->TryGetField(Field);
			if (!Value.IsValid() || Value->Type == EJson::Null)
			{
				if (bRequired)
				{
					Error(FString::Printf(TEXT("Pflichtfeld \"%s\" fehlt"), Field));
				}
				return false;
			}
			if (Value->Type != EJson::Number)
			{
				Error(FString::Printf(TEXT("\"%s\" muss eine Zahl sein"), Field));
				return false;
			}
			Out = Value->AsNumber();
			if (bWhole && Out != FMath::RoundToDouble(Out))
			{
				Error(FString::Printf(TEXT("\"%s\" muss eine ganze Zahl sein"), Field));
				return false;
			}
			if (Out < Min || Out > Max)
			{
				Error(FString::Printf(TEXT("\"%s\" muss zwischen %g und %g liegen"), Field, Min, Max));
				return false;
			}
			return true;
		}

		/** "id" with prefix; extends Where with the ID. */
		bool Id(const TCHAR* Prefix, FName& Out)
		{
			FString Text;
			if (!String(TEXT("id"), Text, true))
			{
				return false;
			}
			Where += FString::Printf(TEXT(" (%s)"), *Text);
			if (!IsSnakeCaseId(Text, Prefix))
			{
				Error(FString::Printf(TEXT("ID muss snake_case sein und mit %s beginnen"), Prefix));
				return false;
			}
			Out = FName(*Text);
			return true;
		}

		/** Objects of the array Field, each with its own reader. */
		TArray<FReader> Objects(const TCHAR* Field, bool bRequired) const
		{
			TArray<FReader> Result;
			const TArray<TSharedPtr<FJsonValue>>* List = nullptr;
			if (!Object->TryGetArrayField(Field, List) || !List)
			{
				if (bRequired)
				{
					Error(FString::Printf(TEXT("Liste \"%s\" fehlt"), Field));
				}
				return Result;
			}
			for (int32 Index = 0; Index < List->Num(); ++Index)
			{
				const FString Sub = FString::Printf(TEXT("%s, %s[%d]"), *Where, Field, Index + 1);
				const TSharedPtr<FJsonValue>& Value = (*List)[Index];
				if (!Value.IsValid() || Value->Type != EJson::Object)
				{
					Report.Errors.Add(FString::Printf(TEXT("%s: muss ein Objekt { ... } sein"), *Sub));
					continue;
				}
				Result.Add(FReader{ Value->AsObject(), Sub, Report });
			}
			if (bRequired && List->Num() == 0)
			{
				Error(FString::Printf(TEXT("Liste \"%s\" ist leer"), Field));
			}
			return Result;
		}
	};

	void ParseRules(const FReader& Root, FGamerCharacterDataSet& Out)
	{
		const TSharedPtr<FJsonObject>* Rules = nullptr;
		if (!Root.Object->TryGetObjectField(TEXT("rules"), Rules) || !Rules)
		{
			Root.Error(TEXT("Objekt \"rules\" fehlt"));
			return;
		}
		const FReader R{ *Rules, Root.Where + TEXT(", rules"), Root.Report };
		R.CheckKnown({ TEXT("stat_sum"), TEXT("stat_min"), TEXT("stat_max"), TEXT("name_max_length"), TEXT("own_branch_cost_factor") });
		double V = 0.0;
		if (R.Number(TEXT("stat_sum"), V, true, 1, 1000, true)) { Out.StatSum = static_cast<int32>(V); }
		if (R.Number(TEXT("stat_min"), V, true, 0, 100, true)) { Out.StatMin = static_cast<int32>(V); }
		if (R.Number(TEXT("stat_max"), V, true, 1, 100, true)) { Out.StatMax = static_cast<int32>(V); }
		if (R.Number(TEXT("name_max_length"), V, true, 3, 64, true)) { Out.NameMaxLength = static_cast<int32>(V); }
		if (R.Number(TEXT("own_branch_cost_factor"), V, true, 0.0, 1.0, false)) { Out.OwnBranchCostFactor = static_cast<float>(V); }
		if (Out.StatMin > Out.StatMax)
		{
			R.Error(TEXT("stat_min ist größer als stat_max"));
		}
	}

	void ParseStats(const FReader& Root, FGamerCharacterDataSet& Out)
	{
		for (FReader R : Root.Objects(TEXT("stats"), true))
		{
			R.CheckKnown({ TEXT("id"), TEXT("name"), TEXT("short"), TEXT("affects") });
			FGamerStatDef Def;
			if (!R.Id(TEXT("stat_"), Def.Id))
			{
				continue;
			}
			R.String(TEXT("name"), Def.Name, true);
			R.String(TEXT("short"), Def.Short, true);
			R.String(TEXT("affects"), Def.Affects, true);
			if (Out.FindStat(Def.Id))
			{
				R.Error(TEXT("ID doppelt vergeben"));
				continue;
			}
			Out.Stats.Add(MoveTemp(Def));
		}
	}

	void ParseAbilities(const FReader& Root, FGamerCharacterDataSet& Out)
	{
		for (FReader R : Root.Objects(TEXT("abilities"), true))
		{
			R.CheckKnown({ TEXT("id"), TEXT("name"), TEXT("description"), TEXT("branch"), TEXT("cost") });
			FGamerAbilityDef Def;
			if (!R.Id(TEXT("ability_"), Def.Id))
			{
				continue;
			}
			R.String(TEXT("name"), Def.Name, true);
			R.String(TEXT("description"), Def.Description, true);
			FString Branch;
			if (R.String(TEXT("branch"), Branch, false))
			{
				Def.Branch = FName(*Branch); // checked against the origins later
			}
			double Cost = 1.0;
			if (R.Number(TEXT("cost"), Cost, true, 1, 100, true))
			{
				Def.Cost = static_cast<int32>(Cost);
			}
			if (Out.FindAbility(Def.Id))
			{
				R.Error(TEXT("ID doppelt vergeben"));
				continue;
			}
			Out.Abilities.Add(MoveTemp(Def));
		}
	}

	void ParseLooks(const FReader& Root, FGamerCharacterDataSet& Out)
	{
		TSet<FName> AllOptions;
		for (FReader R : Root.Objects(TEXT("appearance"), true))
		{
			R.CheckKnown({ TEXT("id"), TEXT("name"), TEXT("options") });
			FGamerLookSlotDef Def;
			if (!R.Id(TEXT("look_"), Def.Id))
			{
				continue;
			}
			R.String(TEXT("name"), Def.Name, true);
			for (FReader O : R.Objects(TEXT("options"), true))
			{
				O.CheckKnown({ TEXT("id"), TEXT("name") });
				FString Id;
				if (!O.String(TEXT("id"), Id, true))
				{
					continue;
				}
				if (!IsSnakeCaseId(Id, TEXT("")) || Id.StartsWith(TEXT("look_")))
				{
					O.Error(FString::Printf(TEXT("Option \"%s\" muss snake_case sein und darf nicht mit look_ beginnen"), *Id));
					continue;
				}
				FGamerLookOption Option;
				Option.Id = FName(*Id);
				O.String(TEXT("name"), Option.Name, true);
				if (AllOptions.Contains(Option.Id))
				{
					O.Error(FString::Printf(TEXT("Option \"%s\" doppelt vergeben (IDs sind über alle Bereiche eindeutig)"), *Id));
					continue;
				}
				AllOptions.Add(Option.Id);
				Def.Options.Add(MoveTemp(Option));
			}
			if (Out.FindLookSlot(Def.Id))
			{
				R.Error(TEXT("ID doppelt vergeben"));
				continue;
			}
			Out.LookSlots.Add(MoveTemp(Def));
		}
	}

	void ParseNames(const FReader& Root, FGamerCharacterDataSet& Out)
	{
		const TArray<TSharedPtr<FJsonValue>>* List = nullptr;
		if (!Root.Object->TryGetArrayField(TEXT("names"), List) || !List || List->Num() == 0)
		{
			Root.Error(TEXT("Liste \"names\" (Namensvorschläge) fehlt oder ist leer"));
			return;
		}
		for (const TSharedPtr<FJsonValue>& Value : *List)
		{
			FString Name;
			if (!Value.IsValid() || !Value->TryGetString(Name) || Name.IsEmpty())
			{
				Root.Error(TEXT("\"names\": jeder Eintrag muss ein nicht leerer Text sein"));
				continue;
			}
			if (Name.Len() > Out.NameMaxLength)
			{
				Root.Error(FString::Printf(TEXT("\"names\": \"%s\" ist länger als name_max_length"), *Name));
			}
			Out.Names.Add(Name);
		}
	}

	void ParseOrigins(const FReader& Root, const FGamerDataSet& Base, FGamerCharacterDataSet& Out)
	{
		for (FReader R : Root.Objects(TEXT("origins"), true))
		{
			R.CheckKnown({ TEXT("id"), TEXT("name"), TEXT("role"), TEXT("description"), TEXT("stats"), TEXT("start_items"), TEXT("start_ability"),
				TEXT("appearance") });
			FGamerOriginDef Def;
			if (!R.Id(TEXT("origin_"), Def.Id))
			{
				continue;
			}
			R.String(TEXT("name"), Def.Name, true);
			R.String(TEXT("role"), Def.Role, true);
			R.String(TEXT("description"), Def.Description, true);

			// Values: every stat exactly once, each within the limits, sum fixed (fair start).
			const TSharedPtr<FJsonObject>* Stats = nullptr;
			if (!R.Object->TryGetObjectField(TEXT("stats"), Stats) || !Stats)
			{
				R.Error(TEXT("Objekt \"stats\" fehlt"));
			}
			else
			{
				const FReader S{ *Stats, R.Where + TEXT(", stats"), R.Report };
				int32 Sum = 0;
				for (const TPair<FString, TSharedPtr<FJsonValue>>& Pair : (*Stats)->Values)
				{
					if (!Out.FindStat(FName(*Pair.Key)))
					{
						S.Error(FString::Printf(TEXT("Wert \"%s\" gibt es nicht (Liste stats)"), *Pair.Key));
					}
				}
				for (const FGamerStatDef& Stat : Out.Stats)
				{
					double V = 0.0;
					if (S.Number(*Stat.Id.ToString(), V, true, Out.StatMin, Out.StatMax, true))
					{
						Def.Stats.Add({ Stat.Id, static_cast<int32>(V) });
						Sum += static_cast<int32>(V);
					}
				}
				if (Def.Stats.Num() == Out.Stats.Num() && Sum != Out.StatSum)
				{
					S.Error(FString::Printf(TEXT("Summe der Werte ist %d, muss %d sein"), Sum, Out.StatSum));
				}
			}

			// Start equipment: real items, at most one stack each.
			for (FReader Item : R.Objects(TEXT("start_items"), true))
			{
				Item.CheckKnown({ TEXT("item"), TEXT("count") });
				FString Id;
				double Count = 0.0;
				if (!Item.String(TEXT("item"), Id, true) || !Item.Number(TEXT("count"), Count, true, 1, 9999, true))
				{
					continue;
				}
				const FGamerItemDef* ItemDef = Base.Items.Find(FName(*Id));
				if (!ItemDef)
				{
					Item.Error(FString::Printf(TEXT("Gegenstand \"%s\" gibt es nicht (items.json)"), *Id));
					continue;
				}
				if (ItemDef->bDeprecated)
				{
					Item.Error(FString::Printf(TEXT("Gegenstand \"%s\" ist veraltet"), *Id));
				}
				if (Count > ItemDef->MaxStack)
				{
					Item.Error(FString::Printf(TEXT("%g Stück \"%s\" passen nicht in einen Platz (max_stack %d)"), Count, *Id, ItemDef->MaxStack));
				}
				Def.StartItems.Add({ ItemDef->Id, static_cast<int32>(Count) });
			}

			FString Ability;
			if (R.String(TEXT("start_ability"), Ability, true))
			{
				Def.StartAbility = FName(*Ability);
				const FGamerAbilityDef* AbilityDef = Out.FindAbility(Def.StartAbility);
				if (!AbilityDef)
				{
					R.Error(FString::Printf(TEXT("start_ability \"%s\" gibt es nicht"), *Ability));
				}
				else if (AbilityDef->Branch != Def.Id)
				{
					R.Error(FString::Printf(TEXT("start_ability \"%s\" gehört nicht zum eigenen Zweig (branch)"), *Ability));
				}
			}

			const TSharedPtr<FJsonObject>* Looks = nullptr;
			if (R.Object->TryGetObjectField(TEXT("appearance"), Looks) && Looks)
			{
				for (const TPair<FString, TSharedPtr<FJsonValue>>& Pair : (*Looks)->Values)
				{
					const FGamerLookSlotDef* Slot = Out.FindLookSlot(FName(*Pair.Key));
					FString Option;
					if (!Slot)
					{
						R.Error(FString::Printf(TEXT("appearance: Bereich \"%s\" gibt es nicht"), *Pair.Key));
					}
					else if (!Pair.Value.IsValid() || !Pair.Value->TryGetString(Option) || !Slot->HasOption(FName(*Option)))
					{
						R.Error(FString::Printf(TEXT("appearance: \"%s\" ist keine Option von %s"), *Option, *Pair.Key));
					}
					else
					{
						Def.Appearance.Add({ Slot->Id, FName(*Option) });
					}
				}
			}

			if (Out.FindOrigin(Def.Id))
			{
				R.Error(TEXT("ID doppelt vergeben"));
				continue;
			}
			Out.Origins.Add(MoveTemp(Def));
		}

		// Abilities may only name existing origins as their branch; every origin should have its own branch.
		for (const FGamerAbilityDef& Ability : Out.Abilities)
		{
			if (!Ability.Branch.IsNone() && !Out.FindOrigin(Ability.Branch))
			{
				Root.Report.Errors.Add(FString::Printf(TEXT("%s, abilities (%s): branch \"%s\" ist keine Herkunft"), *Root.Where,
					*Ability.Id.ToString(), *Ability.Branch.ToString()));
			}
		}
		for (const FGamerOriginDef& Origin : Out.Origins)
		{
			const int32 Own = Out.Abilities.FilterByPredicate([&Origin](const FGamerAbilityDef& A) { return A.Branch == Origin.Id; }).Num();
			if (Own < 2)
			{
				Root.Report.Warnings.Add(FString::Printf(TEXT("%s (%s): eigener Zweig hat nur %d Fähigkeit(en)"), *Root.Where, *Origin.Id.ToString(), Own));
			}
		}
	}
}

void GamerData::ReadStartItems(const FString& Directory, TSet<FName>& OutItems)
{
	// Light read for the item reachability check: full checks happen in LoadCharacters.
	FString Text;
	TSharedPtr<FJsonObject> Root;
	if (!FFileHelper::LoadFileToString(Text, *FPaths::Combine(Directory, CharactersFile)))
	{
		return;
	}
	const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Text);
	const TArray<TSharedPtr<FJsonValue>>* Origins = nullptr;
	if (!FJsonSerializer::Deserialize(Reader, Root) || !Root.IsValid() || !Root->TryGetArrayField(TEXT("origins"), Origins))
	{
		return;
	}
	for (const TSharedPtr<FJsonValue>& Origin : *Origins)
	{
		const TArray<TSharedPtr<FJsonValue>>* Items = nullptr;
		if (Origin.IsValid() && Origin->Type == EJson::Object && Origin->AsObject()->TryGetArrayField(TEXT("start_items"), Items))
		{
			for (const TSharedPtr<FJsonValue>& Item : *Items)
			{
				FString Id;
				if (Item.IsValid() && Item->Type == EJson::Object && Item->AsObject()->TryGetStringField(TEXT("item"), Id))
				{
					OutItems.Add(FName(*Id));
				}
			}
		}
	}
}

bool GamerData::LoadCharacters(const FString& Directory, const FGamerDataSet& Base, FGamerCharacterDataSet& Out, FGamerDataReport& Report,
	bool bRequired)
{
	Out.Reset();
	const int32 ErrorsBefore = Report.Errors.Num();
	const FString Path = FPaths::Combine(Directory, CharactersFile);
	FString Text;
	if (!FPaths::FileExists(Path))
	{
		if (bRequired)
		{
			Report.Errors.Add(FString::Printf(TEXT("%s: Datei fehlt (%s)"), CharactersFile, *Path));
		}
		return !bRequired;
	}
	if (!FFileHelper::LoadFileToString(Text, *Path))
	{
		Report.Errors.Add(FString::Printf(TEXT("%s: Datei nicht lesbar (%s)"), CharactersFile, *Path));
		return false;
	}
	const FString Normalized = Text.Replace(TEXT("\r"), TEXT(""));
	const FTCHARToUTF8 Utf8(*Normalized);
	Out.Checksum = FCrc::MemCrc32(Utf8.Get(), Utf8.Length());

	TSharedPtr<FJsonObject> Root;
	const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Text);
	if (!FJsonSerializer::Deserialize(Reader, Root) || !Root.IsValid())
	{
		Report.Errors.Add(FString::Printf(TEXT("%s: kein gültiges JSON (%s)"), CharactersFile, *Reader->GetErrorMessage()));
		return false;
	}
	const FReader R{ Root, CharactersFile, Report };
	R.CheckKnown({ TEXT("schema"), TEXT("rules"), TEXT("stats"), TEXT("abilities"), TEXT("appearance"), TEXT("names"), TEXT("origins") });
	double Schema = 0.0;
	if (!Root->TryGetNumberField(TEXT("schema"), Schema) || static_cast<int32>(Schema) != GamerData::SchemaVersion)
	{
		R.Error(FString::Printf(TEXT("\"schema\" fehlt oder ist nicht %d"), GamerData::SchemaVersion));
		return false;
	}

	ParseRules(R, Out);
	ParseStats(R, Out);
	ParseAbilities(R, Out);
	ParseLooks(R, Out);
	ParseNames(R, Out);
	ParseOrigins(R, Base, Out);
	return Report.Errors.Num() == ErrorsBefore;
}
