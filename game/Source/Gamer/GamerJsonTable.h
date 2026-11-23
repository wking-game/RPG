#pragma once

#include "CoreMinimal.h"
#include "Dom/JsonObject.h"
#include "Dom/JsonValue.h"
#include "GamerDataLoader.h"
#include "Misc/Crc.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"

/**
 * Small helpers for data tables in game/Content/Data (docs/06_datenformat.md), for the newer loaders
 * (creatures, progression, combat). The older loader in GamerDataLoader.cpp has its own, private copies; they could be
 * merged later. Messages are German on purpose: they are read by whoever edits the JSON files.
 */
namespace GamerJson
{
	/** Reads one field of a JSON object and records problems with a readable location ("creatures.json, Eintrag 3 (creature_x)"). */
	class FRow
	{
	public:
		FRow(TSharedPtr<FJsonObject> InObject, FString InContext, FGamerDataReport& InReport)
			: Object(MoveTemp(InObject)), Context(MoveTemp(InContext)), Report(InReport)
		{
		}

		const FString& GetContext() const { return Context; }
		void SetContext(FString InContext) { Context = MoveTemp(InContext); }
		const TSharedPtr<FJsonObject>& GetObject() const { return Object; }
		FGamerDataReport& GetReport() const { return Report; }

		void Error(const FString& Message) { Report.Errors.Add(FString::Printf(TEXT("%s: %s"), *Context, *Message)); }
		void Warning(const FString& Message) { Report.Warnings.Add(FString::Printf(TEXT("%s: %s"), *Context, *Message)); }

		bool Has(const TCHAR* Field) const { return Object->HasField(Field); }

		/** Unknown fields are errors: a typo like "regrow_dayz" would otherwise be ignored silently. Fields starting with "_" are comments. */
		void CheckKnown(std::initializer_list<const TCHAR*> Allowed)
		{
			for (const TPair<FString, TSharedPtr<FJsonValue>>& Pair : Object->Values)
			{
				if (Pair.Key.StartsWith(TEXT("_")))
				{
					continue;
				}
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

		bool GetString(const TCHAR* Field, FString& Out, bool bRequired = false)
		{
			if (const TSharedPtr<FJsonValue> Value = Find(Field, bRequired))
			{
				if (Value->Type == EJson::String)
				{
					Out = Value->AsString();
					return true;
				}
				Error(FString::Printf(TEXT("\"%s\" muss Text sein"), Field));
			}
			return false;
		}

		bool GetFloat(const TCHAR* Field, float& Out, bool bRequired = false)
		{
			if (const TSharedPtr<FJsonValue> Value = Find(Field, bRequired))
			{
				if (Value->Type == EJson::Number)
				{
					Out = static_cast<float>(Value->AsNumber());
					return true;
				}
				Error(FString::Printf(TEXT("\"%s\" muss eine Zahl sein"), Field));
			}
			return false;
		}

		bool GetInt(const TCHAR* Field, int32& Out, bool bRequired = false)
		{
			if (const TSharedPtr<FJsonValue> Value = Find(Field, bRequired))
			{
				if (Value->Type == EJson::Number)
				{
					const double Number = Value->AsNumber();
					if (FMath::FloorToDouble(Number) == Number)
					{
						Out = static_cast<int32>(Number);
						return true;
					}
					Error(FString::Printf(TEXT("\"%s\" muss eine ganze Zahl sein"), Field));
					return false;
				}
				Error(FString::Printf(TEXT("\"%s\" muss eine Zahl sein"), Field));
			}
			return false;
		}

		bool GetBool(const TCHAR* Field, bool& Out, bool bRequired = false)
		{
			if (const TSharedPtr<FJsonValue> Value = Find(Field, bRequired))
			{
				if (Value->Type == EJson::Boolean)
				{
					Out = Value->AsBool();
					return true;
				}
				Error(FString::Printf(TEXT("\"%s\" muss true oder false sein"), Field));
			}
			return false;
		}

		const TArray<TSharedPtr<FJsonValue>>* GetArray(const TCHAR* Field, bool bRequired = false)
		{
			if (const TSharedPtr<FJsonValue> Value = Find(Field, bRequired))
			{
				if (Value->Type == EJson::Array)
				{
					return &Value->AsArray();
				}
				Error(FString::Printf(TEXT("\"%s\" muss eine Liste [ ... ] sein"), Field));
			}
			return nullptr;
		}

		TSharedPtr<FJsonObject> GetSubObject(const TCHAR* Field, bool bRequired = false)
		{
			if (const TSharedPtr<FJsonValue> Value = Find(Field, bRequired))
			{
				if (Value->Type == EJson::Object)
				{
					return Value->AsObject();
				}
				Error(FString::Printf(TEXT("\"%s\" muss ein Objekt { ... } sein"), Field));
			}
			return nullptr;
		}

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

	/** One row object plus where it came from. */
	struct FRowSource
	{
		TSharedPtr<FJsonObject> Object;
		FString Context;
	};

	/**
	 * Reads a table file { "schema": 1, ..., "rows": [ ... ] }, adds its content to the checksum (line endings ignored, same as the
	 * older loader) and returns the root object and the row objects. Returns false if the file is missing (bOptional = no error) or broken.
	 */
	inline bool ReadTable(const FString& Directory, const TCHAR* FileName, TSharedPtr<FJsonObject>& OutRoot, TArray<FRowSource>& OutRows,
		uint32& InOutCrc, FGamerDataReport& Report, bool bOptional = false)
	{
		const FString Path = FPaths::Combine(Directory, FileName);
		FString Text;
		if (bOptional && !FPaths::FileExists(Path))
		{
			return false;
		}
		if (!FFileHelper::LoadFileToString(Text, *Path))
		{
			Report.Errors.Add(FString::Printf(TEXT("%s: Datei fehlt oder ist nicht lesbar (%s)"), FileName, *Path));
			return false;
		}
		const FString Normalized = Text.Replace(TEXT("\r"), TEXT(""));
		const FTCHARToUTF8 Utf8(*Normalized);
		InOutCrc = FCrc::MemCrc32(Utf8.Get(), Utf8.Length(), InOutCrc);

		const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Text);
		if (!FJsonSerializer::Deserialize(Reader, OutRoot) || !OutRoot.IsValid())
		{
			Report.Errors.Add(FString::Printf(TEXT("%s: kein gültiges JSON (%s)"), FileName, *Reader->GetErrorMessage()));
			return false;
		}
		double Schema = 0.0;
		if (!OutRoot->TryGetNumberField(TEXT("schema"), Schema) || static_cast<int32>(Schema) != GamerData::SchemaVersion)
		{
			Report.Errors.Add(FString::Printf(TEXT("%s: \"schema\" fehlt oder ist nicht %d"), FileName, GamerData::SchemaVersion));
			return false;
		}
		const TArray<TSharedPtr<FJsonValue>>* Rows = nullptr;
		if (!OutRoot->TryGetArrayField(TEXT("rows"), Rows) || !Rows)
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

	/** Reads a single-object file { "schema": 1, ... } (progression.json, combat.json). */
	inline bool ReadObjectFile(const FString& Directory, const TCHAR* FileName, TSharedPtr<FJsonObject>& OutRoot, uint32& InOutCrc,
		FGamerDataReport& Report, bool bOptional = false)
	{
		const FString Path = FPaths::Combine(Directory, FileName);
		FString Text;
		if (bOptional && !FPaths::FileExists(Path))
		{
			return false;
		}
		if (!FFileHelper::LoadFileToString(Text, *Path))
		{
			Report.Errors.Add(FString::Printf(TEXT("%s: Datei fehlt oder ist nicht lesbar (%s)"), FileName, *Path));
			return false;
		}
		const FString Normalized = Text.Replace(TEXT("\r"), TEXT(""));
		const FTCHARToUTF8 Utf8(*Normalized);
		InOutCrc = FCrc::MemCrc32(Utf8.Get(), Utf8.Length(), InOutCrc);

		const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Text);
		if (!FJsonSerializer::Deserialize(Reader, OutRoot) || !OutRoot.IsValid())
		{
			Report.Errors.Add(FString::Printf(TEXT("%s: kein gültiges JSON (%s)"), FileName, *Reader->GetErrorMessage()));
			return false;
		}
		double Schema = 0.0;
		if (!OutRoot->TryGetNumberField(TEXT("schema"), Schema) || static_cast<int32>(Schema) != GamerData::SchemaVersion)
		{
			Report.Errors.Add(FString::Printf(TEXT("%s: \"schema\" fehlt oder ist nicht %d"), FileName, GamerData::SchemaVersion));
			return false;
		}
		return true;
	}

	inline bool IsSnakeCase(const FString& S)
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
}
