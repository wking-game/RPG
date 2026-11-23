#include "GamerDifficulty.h"

#include "GamerDataLoader.h"
#include "Dom/JsonObject.h"
#include "Dom/JsonValue.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"

// Validation messages are German on purpose: they are read by whoever edits difficulty.json.

namespace
{
	FString Factor(float Value)
	{
		return FString::Printf(TEXT("×%s"), *FString::SanitizeFloat(Value, 1).Replace(TEXT("."), TEXT(",")));
	}
}

void FGamerDifficultyPreset::ApplyTo(FGamerWorldOptions& Options) const
{
	Options.Difficulty = Level;
	Options.DeathMode = Death;
	Options.NeedsRate = NeedsRate;
	Options.DamageTaken = DamageTaken;
	Options.YieldFactor = Yield;
	Options.bHordes = bHordes;
	Options.bVillagersImmortal = bVillagersImmortal;
	Options.Demolish = Demolish;
	Options.GespuerTempo = GespuerTempo;
}

bool FGamerDifficultyPreset::Matches(const FGamerWorldOptions& Options) const
{
	return Options.DeathMode == Death && FMath::IsNearlyEqual(Options.NeedsRate, NeedsRate, 0.001f)
		&& FMath::IsNearlyEqual(Options.DamageTaken, DamageTaken, 0.001f) && FMath::IsNearlyEqual(Options.YieldFactor, Yield, 0.001f)
		&& Options.bHordes == bHordes && Options.bVillagersImmortal == bVillagersImmortal && Options.Demolish == Demolish
		&& Options.GespuerTempo == GespuerTempo;
}

FString FGamerDifficultyPreset::Summary() const
{
	const TCHAR* DeathText = Death == EGamerDeathMode::None ? TEXT("nichts verlieren") : Death == EGamerDeathMode::Hardcore ? TEXT("alles verlieren") : TEXT("Grab");
	const TCHAR* Tear = Demolish == EGamerDemolish::FullRefund ? TEXT("voll") : Demolish == EGamerDemolish::HalfRefund ? TEXT("halb") : Demolish == EGamerDemolish::PartialRefund ? TEXT("etwa 80 %") : TEXT("aus");
	const TCHAR* Tempo = GespuerTempo == EGamerGespuerTempo::Slow ? TEXT("langsam") : GespuerTempo == EGamerGespuerTempo::Fast ? TEXT("schnell") : TEXT("normal");
	return FString::Printf(TEXT("Tod: %s · Hunger/Durst/Erschöpfung %s · Schaden %s · Ertrag %s · Horden %s · Bewohner %s · Abreißen %s · Gespür %s"),
		DeathText, *Factor(NeedsRate), *Factor(DamageTaken), *Factor(Yield), bHordes ? TEXT("an") : TEXT("aus"),
		bVillagersImmortal ? TEXT("unsterblich") : TEXT("sterblich"), Tear, Tempo);
}

const FGamerDifficultyPreset* GamerData::FindDifficulty(const TArray<FGamerDifficultyPreset>& Presets, EGamerDifficulty Level)
{
	return Presets.FindByPredicate([Level](const FGamerDifficultyPreset& P) { return P.Level == Level; });
}

FString GamerData::DifficultyName(const TArray<FGamerDifficultyPreset>& Presets, EGamerDifficulty Level)
{
	if (const FGamerDifficultyPreset* Preset = FindDifficulty(Presets, Level))
	{
		return Preset->Name;
	}
	return Level == EGamerDifficulty::Custom ? TEXT("Benutzerdefiniert") : TEXT("?");
}

bool GamerData::EnforceDifficulty(const TArray<FGamerDifficultyPreset>& Presets, FGamerWorldOptions& Options)
{
	const FGamerDifficultyPreset* Preset = FindDifficulty(Presets, Options.Difficulty);
	if (!Preset || Preset->Matches(Options))
	{
		return false;
	}
	Preset->ApplyTo(Options);
	return true;
}

bool GamerData::LoadDifficulty(const FString& Directory, TArray<FGamerDifficultyPreset>& Out, FGamerDataReport& Report, bool bRequired)
{
	Out.Reset();
	const int32 ErrorsBefore = Report.Errors.Num();
	const FString Path = FPaths::Combine(Directory, DifficultyFile);
	FString Text;
	if (!FPaths::FileExists(Path))
	{
		if (bRequired)
		{
			Report.Errors.Add(FString::Printf(TEXT("%s: Datei fehlt (%s)"), DifficultyFile, *Path));
		}
		return !bRequired;
	}
	TSharedPtr<FJsonObject> Root;
	const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(FFileHelper::LoadFileToString(Text, *Path) ? Text : FString());
	if (!FJsonSerializer::Deserialize(Reader, Root) || !Root.IsValid())
	{
		Report.Errors.Add(FString::Printf(TEXT("%s: kein gültiges JSON (%s)"), DifficultyFile, *Reader->GetErrorMessage()));
		return false;
	}
	double Schema = 0.0;
	if (!Root->TryGetNumberField(TEXT("schema"), Schema) || static_cast<int32>(Schema) != GamerData::SchemaVersion)
	{
		Report.Errors.Add(FString::Printf(TEXT("%s: \"schema\" fehlt oder ist nicht %d"), DifficultyFile, GamerData::SchemaVersion));
		return false;
	}
	const TArray<TSharedPtr<FJsonValue>>* Rows = nullptr;
	if (!Root->TryGetArrayField(TEXT("presets"), Rows) || !Rows)
	{
		Report.Errors.Add(FString::Printf(TEXT("%s: Liste \"presets\" fehlt"), DifficultyFile));
		return false;
	}

	for (int32 Index = 0; Index < Rows->Num(); ++Index)
	{
		FString Where = FString::Printf(TEXT("%s, presets[%d]"), DifficultyFile, Index + 1);
		const TSharedPtr<FJsonValue>& Value = (*Rows)[Index];
		if (!Value.IsValid() || Value->Type != EJson::Object)
		{
			Report.Errors.Add(Where + TEXT(": muss ein Objekt { ... } sein"));
			continue;
		}
		const TSharedPtr<FJsonObject> Row = Value->AsObject();
		auto Error = [&Report, &Where](const FString& Message) { Report.Errors.Add(FString::Printf(TEXT("%s: %s"), *Where, *Message)); };

		static const TCHAR* const Known[] = { TEXT("id"), TEXT("level"), TEXT("name"), TEXT("description"), TEXT("death"), TEXT("needs_rate"),
			TEXT("damage_taken"), TEXT("yield"), TEXT("hordes"), TEXT("villagers_immortal"), TEXT("demolish"), TEXT("gespuer_tempo") };
		for (const TPair<FString, TSharedPtr<FJsonValue>>& Pair : Row->Values)
		{
			bool bKnown = false;
			for (const TCHAR* Name : Known)
			{
				bKnown |= Pair.Key.Equals(Name, ESearchCase::CaseSensitive);
			}
			if (!bKnown)
			{
				Error(FString::Printf(TEXT("unbekanntes Feld \"%s\" (Tippfehler?)"), *Pair.Key));
			}
		}

		FGamerDifficultyPreset Preset;
		FString Id;
		if (!Row->TryGetStringField(TEXT("id"), Id) || !Id.StartsWith(TEXT("difficulty_")))
		{
			Error(TEXT("\"id\" fehlt oder beginnt nicht mit difficulty_"));
			continue;
		}
		Where += FString::Printf(TEXT(" (%s)"), *Id);
		Preset.Id = FName(*Id);

		auto ReadEnum = [&](const TCHAR* Field, std::initializer_list<const TCHAR*> Allowed, int32& OutIndex)
		{
			FString Text;
			if (!Row->TryGetStringField(Field, Text))
			{
				Error(FString::Printf(TEXT("Pflichtfeld \"%s\" fehlt"), Field));
				return false;
			}
			int32 Position = 0;
			FString List;
			for (const TCHAR* Option : Allowed)
			{
				if (Text == Option)
				{
					OutIndex = Position;
					return true;
				}
				List += (List.IsEmpty() ? TEXT("") : TEXT(", ")) + FString(Option);
				++Position;
			}
			Error(FString::Printf(TEXT("%s \"%s\" unbekannt (erlaubt: %s)"), Field, *Text, *List));
			return false;
		};
		auto ReadFactor = [&](const TCHAR* Field, float& OutValue)
		{
			double Number = 0.0;
			if (!Row->TryGetNumberField(Field, Number))
			{
				Error(FString::Printf(TEXT("Pflichtfeld \"%s\" fehlt oder ist keine Zahl"), Field));
			}
			else if (Number < 0.1 || Number > 5.0)
			{
				Error(FString::Printf(TEXT("%s muss zwischen 0,1 und 5 liegen"), Field));
			}
			else
			{
				OutValue = static_cast<float>(Number);
			}
		};
		auto ReadBool = [&](const TCHAR* Field, bool& OutValue)
		{
			// Strict: the engine would also accept strings like "no" as a bool.
			const TSharedPtr<FJsonValue> FieldValue = Row->TryGetField(Field);
			if (FieldValue.IsValid() && FieldValue->Type == EJson::Boolean)
			{
				OutValue = FieldValue->AsBool();
			}
			else
			{
				Error(FString::Printf(TEXT("Pflichtfeld \"%s\" fehlt oder ist nicht true/false"), Field));
			}
		};

		int32 Choice = 0;
		if (ReadEnum(TEXT("level"), { TEXT("easy"), TEXT("medium"), TEXT("hard"), TEXT("hardcore") }, Choice))
		{
			Preset.Level = static_cast<EGamerDifficulty>(Choice);
		}
		if (!Row->TryGetStringField(TEXT("name"), Preset.Name) || Preset.Name.IsEmpty())
		{
			Error(TEXT("Pflichtfeld \"name\" fehlt"));
		}
		Row->TryGetStringField(TEXT("description"), Preset.Description);
		if (ReadEnum(TEXT("death"), { TEXT("none"), TEXT("grave"), TEXT("hardcore") }, Choice))
		{
			Preset.Death = static_cast<EGamerDeathMode>(Choice);
		}
		ReadFactor(TEXT("needs_rate"), Preset.NeedsRate);
		ReadFactor(TEXT("damage_taken"), Preset.DamageTaken);
		ReadFactor(TEXT("yield"), Preset.Yield);
		ReadBool(TEXT("hordes"), Preset.bHordes);
		ReadBool(TEXT("villagers_immortal"), Preset.bVillagersImmortal);
		if (ReadEnum(TEXT("demolish"), { TEXT("full"), TEXT("half"), TEXT("off"), TEXT("partial") }, Choice))
		{
			Preset.Demolish = static_cast<EGamerDemolish>(Choice);
		}
		if (ReadEnum(TEXT("gespuer_tempo"), { TEXT("slow"), TEXT("normal"), TEXT("fast") }, Choice))
		{
			Preset.GespuerTempo = static_cast<EGamerGespuerTempo>(Choice);
		}

		if (FindDifficulty(Out, Preset.Level))
		{
			Error(TEXT("level doppelt vergeben"));
			continue;
		}
		if (Out.ContainsByPredicate([&Preset](const FGamerDifficultyPreset& P) { return P.Id == Preset.Id; }))
		{
			Error(TEXT("ID doppelt vergeben"));
			continue;
		}
		Out.Add(MoveTemp(Preset));
	}

	for (const EGamerDifficulty Level : { EGamerDifficulty::Easy, EGamerDifficulty::Medium, EGamerDifficulty::Hard, EGamerDifficulty::Hardcore })
	{
		if (!FindDifficulty(Out, Level))
		{
			static const TCHAR* const Names[] = { TEXT("easy"), TEXT("medium"), TEXT("hard"), TEXT("hardcore") };
			Report.Errors.Add(FString::Printf(TEXT("%s: Stufe \"%s\" fehlt"), DifficultyFile, Names[static_cast<int32>(Level)]));
		}
	}
	Out.Sort([](const FGamerDifficultyPreset& A, const FGamerDifficultyPreset& B) { return A.Level < B.Level; });
	return Report.Errors.Num() == ErrorsBefore;
}
