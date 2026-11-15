#include "GamerSaveable.h"

#include "Dom/JsonObject.h"
#include "Dom/JsonValue.h"

FGamerSaveRecord::FGamerSaveRecord()
	: Json(MakeShared<FJsonObject>())
{
}

FGamerSaveRecord::FGamerSaveRecord(TSharedPtr<FJsonObject> InJson)
	: Json(InJson.IsValid() ? InJson : MakeShared<FJsonObject>())
{
}

bool FGamerSaveRecord::Has(const FString& Key) const
{
	return Json->HasField(Key);
}

void FGamerSaveRecord::SetNumber(const FString& Key, double Value)
{
	Json->SetNumberField(Key, Value);
}

void FGamerSaveRecord::SetBool(const FString& Key, bool Value)
{
	Json->SetBoolField(Key, Value);
}

void FGamerSaveRecord::SetString(const FString& Key, const FString& Value)
{
	Json->SetStringField(Key, Value);
}

void FGamerSaveRecord::SetName(const FString& Key, FName Value)
{
	Json->SetStringField(Key, Value.IsNone() ? FString() : Value.ToString());
}

static TArray<TSharedPtr<FJsonValue>> NumbersToJson(std::initializer_list<double> Values)
{
	TArray<TSharedPtr<FJsonValue>> Out;
	for (double Value : Values)
	{
		Out.Add(MakeShared<FJsonValueNumber>(Value));
	}
	return Out;
}

static bool JsonToNumbers(const FJsonObject& Json, const FString& Key, int32 Count, double* Out)
{
	const TArray<TSharedPtr<FJsonValue>>* Array = nullptr;
	if (!Json.TryGetArrayField(Key, Array) || !Array || Array->Num() != Count)
	{
		return false;
	}
	for (int32 Index = 0; Index < Count; ++Index)
	{
		double Value = 0.0;
		if (!(*Array)[Index].IsValid() || !(*Array)[Index]->TryGetNumber(Value) || !FMath::IsFinite(Value))
		{
			return false;
		}
		Out[Index] = Value;
	}
	return true;
}

void FGamerSaveRecord::SetVector(const FString& Key, const FVector& Value)
{
	Json->SetArrayField(Key, NumbersToJson({ Value.X, Value.Y, Value.Z }));
}

void FGamerSaveRecord::SetRotator(const FString& Key, const FRotator& Value)
{
	Json->SetArrayField(Key, NumbersToJson({ Value.Pitch, Value.Yaw, Value.Roll }));
}

void FGamerSaveRecord::SetTransform(const FString& Key, const FTransform& Value)
{
	FGamerSaveRecord Child;
	Child.SetVector(TEXT("loc"), Value.GetLocation());
	Child.SetRotator(TEXT("rot"), Value.Rotator());
	if (!Value.GetScale3D().Equals(FVector::OneVector, 0.0001))
	{
		Child.SetVector(TEXT("scale"), Value.GetScale3D());
	}
	SetRecord(Key, Child);
}

void FGamerSaveRecord::SetItems(const FString& Key, const TArray<FGamerItemAmount>& Items)
{
	TArray<TSharedPtr<FJsonValue>> Array;
	for (const FGamerItemAmount& Amount : Items)
	{
		if (Amount.Item.IsNone() || Amount.Count <= 0)
		{
			continue;
		}
		TSharedRef<FJsonObject> Entry = MakeShared<FJsonObject>();
		Entry->SetStringField(TEXT("item"), Amount.Item.ToString());
		Entry->SetNumberField(TEXT("count"), Amount.Count);
		if (Amount.Freshness < 0.9999f)
		{
			Entry->SetNumberField(TEXT("fresh"), Amount.Freshness);
		}
		Array.Add(MakeShared<FJsonValueObject>(Entry));
	}
	Json->SetArrayField(Key, Array);
}

void FGamerSaveRecord::SetNames(const FString& Key, const TArray<FName>& Names)
{
	TArray<TSharedPtr<FJsonValue>> Array;
	for (const FName& Name : Names)
	{
		Array.Add(MakeShared<FJsonValueString>(Name.ToString()));
	}
	Json->SetArrayField(Key, Array);
}

void FGamerSaveRecord::SetRecord(const FString& Key, const FGamerSaveRecord& Child)
{
	Json->SetObjectField(Key, Child.Json);
}

void FGamerSaveRecord::SetRecords(const FString& Key, const TArray<FGamerSaveRecord>& Children)
{
	TArray<TSharedPtr<FJsonValue>> Array;
	for (const FGamerSaveRecord& Child : Children)
	{
		Array.Add(MakeShared<FJsonValueObject>(Child.Json));
	}
	Json->SetArrayField(Key, Array);
}

double FGamerSaveRecord::GetNumber(const FString& Key, double Default) const
{
	double Value = Default;
	return Json->TryGetNumberField(Key, Value) && FMath::IsFinite(Value) ? Value : Default;
}

int32 FGamerSaveRecord::GetInt(const FString& Key, int32 Default) const
{
	const double Value = GetNumber(Key, Default);
	return static_cast<int32>(FMath::Clamp(FMath::RoundToDouble(Value), static_cast<double>(MIN_int32), static_cast<double>(MAX_int32)));
}

// Strict types: the JSON library would turn numbers into bools/strings; a wrong type means "not there".
bool FGamerSaveRecord::GetBool(const FString& Key, bool Default) const
{
	const TSharedPtr<FJsonValue> Value = Json->TryGetField(Key);
	return Value.IsValid() && Value->Type == EJson::Boolean ? Value->AsBool() : Default;
}

FString FGamerSaveRecord::GetString(const FString& Key, const FString& Default) const
{
	const TSharedPtr<FJsonValue> Value = Json->TryGetField(Key);
	return Value.IsValid() && Value->Type == EJson::String ? Value->AsString() : Default;
}

FName FGamerSaveRecord::GetName(const FString& Key, FName Default) const
{
	const TSharedPtr<FJsonValue> Field = Json->TryGetField(Key);
	if (!Field.IsValid() || Field->Type != EJson::String)
	{
		return Default;
	}
	const FString Value = Field->AsString();
	return Value.IsEmpty() ? NAME_None : FName(*Value);
}

FVector FGamerSaveRecord::GetVector(const FString& Key, const FVector& Default) const
{
	double V[3];
	return JsonToNumbers(*Json, Key, 3, V) ? FVector(V[0], V[1], V[2]) : Default;
}

FRotator FGamerSaveRecord::GetRotator(const FString& Key, const FRotator& Default) const
{
	double V[3];
	return JsonToNumbers(*Json, Key, 3, V) ? FRotator(V[0], V[1], V[2]) : Default;
}

FTransform FGamerSaveRecord::GetTransform(const FString& Key, const FTransform& Default) const
{
	if (!Has(Key))
	{
		return Default;
	}
	const FGamerSaveRecord Child = GetRecord(Key);
	return FTransform(Child.GetRotator(TEXT("rot"), Default.Rotator()), Child.GetVector(TEXT("loc"), Default.GetLocation()),
		Child.GetVector(TEXT("scale"), FVector::OneVector));
}

TArray<FGamerItemAmount> FGamerSaveRecord::GetItems(const FString& Key) const
{
	TArray<FGamerItemAmount> Out;
	const TArray<TSharedPtr<FJsonValue>>* Array = nullptr;
	if (!Json->TryGetArrayField(Key, Array) || !Array)
	{
		return Out;
	}
	for (const TSharedPtr<FJsonValue>& Value : *Array)
	{
		const TSharedPtr<FJsonObject>* Entry = nullptr;
		if (!Value.IsValid() || !Value->TryGetObject(Entry) || !Entry)
		{
			continue;
		}
		const FGamerSaveRecord Record(*Entry);
		FGamerItemAmount Amount;
		Amount.Item = Record.GetName(TEXT("item"));
		Amount.Count = Record.GetInt(TEXT("count"));
		Amount.Freshness = FMath::Clamp(Record.GetFloat(TEXT("fresh"), 1.f), 0.f, 1.f);
		if (!Amount.Item.IsNone() && Amount.Count > 0)
		{
			Out.Add(Amount);
		}
	}
	return Out;
}

TArray<FName> FGamerSaveRecord::GetNames(const FString& Key) const
{
	TArray<FName> Out;
	const TArray<TSharedPtr<FJsonValue>>* Array = nullptr;
	if (Json->TryGetArrayField(Key, Array) && Array)
	{
		for (const TSharedPtr<FJsonValue>& Value : *Array)
		{
			FString Text;
			if (Value.IsValid() && Value->TryGetString(Text) && !Text.IsEmpty())
			{
				Out.Add(FName(*Text));
			}
		}
	}
	return Out;
}

FGamerSaveRecord FGamerSaveRecord::GetRecord(const FString& Key) const
{
	const TSharedPtr<FJsonObject>* Child = nullptr;
	return Json->TryGetObjectField(Key, Child) && Child ? FGamerSaveRecord(*Child) : FGamerSaveRecord();
}

TArray<FGamerSaveRecord> FGamerSaveRecord::GetRecords(const FString& Key) const
{
	TArray<FGamerSaveRecord> Out;
	const TArray<TSharedPtr<FJsonValue>>* Array = nullptr;
	if (Json->TryGetArrayField(Key, Array) && Array)
	{
		for (const TSharedPtr<FJsonValue>& Value : *Array)
		{
			const TSharedPtr<FJsonObject>* Child = nullptr;
			if (Value.IsValid() && Value->TryGetObject(Child) && Child)
			{
				Out.Add(FGamerSaveRecord(*Child));
			}
		}
	}
	return Out;
}
