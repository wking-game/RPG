#include "GamerScanComponent.h"

#include "Gamer.h"
#include "GamerDataSubsystem.h"
#include "GamerGroundItem.h"
#include "GamerResourceNode.h"
#include "GamerWorkComponent.h"
#include "EngineUtils.h"
#include "Dom/JsonObject.h"
#include "Engine/World.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"

UGamerScanComponent::UGamerScanComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UGamerScanComponent::LoadSettings()
{
	if (bLoaded)
	{
		return;
	}
	bLoaded = true;
	auto AddCategory = [this](const TCHAR* Id, const TCHAR* Name, FLinearColor Color, const TCHAR* Symbol)
	{
		FGamerScanCategory& Category = Categories.AddDefaulted_GetRef();
		Category.Id = Id;
		Category.Name = Name;
		Category.Color = Color;
		Category.Symbol = Symbol;
	};
	// Defaults, overwritten by scan.json when present.
	AddCategory(TEXT("food"), TEXT("Nahrung"), FLinearColor(0.715f, 0.298f, 0.056f), TEXT("N"));
	AddCategory(TEXT("wood"), TEXT("Holz"), FLinearColor(0.405f, 0.334f, 0.22f), TEXT("H"));
	AddCategory(TEXT("stone"), TEXT("Stein"), FLinearColor(0.33f, 0.399f, 0.384f), TEXT("S"));
	AddCategory(TEXT("plants"), TEXT("Pflanzen"), FLinearColor(0.212f, 0.33f, 0.145f), TEXT("P"));
	AddCategory(TEXT("items"), TEXT("Gegenstände"), FLinearColor(0.687f, 0.442f, 0.164f), TEXT("G"));

	const FString Path = FPaths::ProjectContentDir() / TEXT("Data/scan.json");
	FString Text;
	TSharedPtr<FJsonObject> Root;
	if (!FFileHelper::LoadFileToString(Text, *Path) || !FJsonSerializer::Deserialize(TJsonReaderFactory<>::Create(Text), Root) || !Root.IsValid())
	{
		UE_LOG(LogGamer, Warning, TEXT("Scan: %s not readable, using defaults"), *Path);
		return;
	}
	double Number = 0.0;
	if (Root->TryGetNumberField(TEXT("radius_m"), Number)) { RadiusCm = static_cast<float>(Number) * 100.f; }
	if (Root->TryGetNumberField(TEXT("spread_s"), Number)) { SpreadSeconds = FMath::Max(0.5f, static_cast<float>(Number)); }
	if (Root->TryGetNumberField(TEXT("visible_s"), Number)) { VisibleSeconds = static_cast<float>(Number); }
	if (Root->TryGetNumberField(TEXT("cooldown_s"), Number)) { CooldownSeconds = static_cast<float>(Number); }
	const TSharedPtr<FJsonObject>* Wave = nullptr;
	if (Root->TryGetObjectField(TEXT("wave"), Wave))
	{
		if ((*Wave)->TryGetNumberField(TEXT("amplitude_cm"), Number)) { WaveAmplitudeCm = static_cast<float>(Number); }
		if ((*Wave)->TryGetNumberField(TEXT("waves"), Number)) { WaveCount = FMath::Max(1, static_cast<int32>(Number)); }
		if ((*Wave)->TryGetNumberField(TEXT("ripple_speed"), Number)) { RippleSpeed = static_cast<float>(Number); }
	}
	const TArray<TSharedPtr<FJsonValue>>* List = nullptr;
	if (Root->TryGetArrayField(TEXT("categories"), List))
	{
		Categories.Reset();
		for (const TSharedPtr<FJsonValue>& Value : *List)
		{
			const TSharedPtr<FJsonObject> Obj = Value->AsObject();
			if (!Obj.IsValid())
			{
				continue;
			}
			FGamerScanCategory& Category = Categories.AddDefaulted_GetRef();
			Category.Id = FName(*Obj->GetStringField(TEXT("id")));
			Category.Name = Obj->GetStringField(TEXT("name"));
			Category.Symbol = Obj->GetStringField(TEXT("symbol"));
			const TArray<TSharedPtr<FJsonValue>>* Rgb = nullptr;
			if (Obj->TryGetArrayField(TEXT("color"), Rgb) && Rgb->Num() >= 3)
			{
				Category.Color = FLinearColor((*Rgb)[0]->AsNumber(), (*Rgb)[1]->AsNumber(), (*Rgb)[2]->AsNumber());
			}
		}
	}
	UE_LOG(LogGamer, Log, TEXT("Scan: loaded radius %.0f m, spread %.1f s, %.0f s visible, %.0f s cooldown, %d categories"),
		RadiusCm / 100.f, SpreadSeconds, VisibleSeconds, CooldownSeconds, Categories.Num());
}

int32 UGamerScanComponent::CategoryIndex(FName Id) const
{
	return Categories.IndexOfByPredicate([Id](const FGamerScanCategory& C) { return C.Id == Id; });
}

int32 UGamerScanComponent::CategoryOfNode(const FGamerResourceNodeDef& Def) const
{
	// From what it gives: food, wood (logs, sticks), stone, otherwise plants.
	const UGamerDataSubsystem* Data = UGamerDataSubsystem::Get(this);
	for (const FGamerDrop& Drop : Def.Drops)
	{
		const FGamerItemDef* Item = Data ? Data->FindItem(Drop.Item) : nullptr;
		const FString Name = Drop.Item.ToString();
		if (Item && Item->Category == EGamerItemCategory::Food)
		{
			return CategoryIndex(TEXT("food"));
		}
		if (Name.Contains(TEXT("log")) || Name.Contains(TEXT("stick")) || Name.Contains(TEXT("plank")))
		{
			return CategoryIndex(TEXT("wood"));
		}
		if (Name.Contains(TEXT("stone")))
		{
			return CategoryIndex(TEXT("stone"));
		}
	}
	return CategoryIndex(TEXT("plants"));
}

float UGamerScanComponent::GetCooldownLeft() const
{
	const UWorld* World = GetWorld();
	return World ? FMath::Max(0.f, CooldownSeconds - static_cast<float>(World->GetTimeSeconds() - LastScanTime)) : 0.f;
}

float UGamerScanComponent::GetAgeSeconds() const
{
	if (DebugAge >= 0.f)
	{
		return DebugAge;
	}
	const UWorld* World = GetWorld();
	return World ? static_cast<float>(World->GetTimeSeconds() - LastScanTime) : 1000.f;
}

bool UGamerScanComponent::IsShowing() const
{
	const UWorld* World = GetWorld();
	return World && (DebugAge >= 0.f || World->GetTimeSeconds() < ShowUntil);
}

bool UGamerScanComponent::TryScan()
{
	LoadSettings();
	const AActor* Owner = GetOwner();
	UWorld* World = GetWorld();
	if (!Owner || !World)
	{
		return false;
	}
	if (const float Left = GetCooldownLeft(); Left > 0.f)
	{
		if (UGamerWorkComponent* Work = Owner->FindComponentByClass<UGamerWorkComponent>())
		{
			Work->ShowMessage(FString::Printf(TEXT("Umschauen wieder bereit in %.0f s"), FMath::CeilToFloat(Left)));
		}
		return false;
	}
	const double Start = World->GetTimeSeconds();
	LastScanTime = Start;
	Marks.Reset();
	Counts.Init(0, Categories.Num());
	Center = Owner->GetActorLocation();
	ShowUntil = Start + SpreadSeconds + VisibleSeconds;
	auto Add = [&](const FVector& Location, int32 Category)
	{
		const float Distance = FVector::Dist(Location, Center);
		if (Category != INDEX_NONE && Distance <= RadiusCm)
		{
			FGamerScanMark& Mark = Marks.AddDefaulted_GetRef();
			Mark.Location = Location;
			Mark.Category = Category;
			Mark.RevealTime = Start + SpreadSeconds * (Distance / RadiusCm);   // the wave reaches it
			Mark.ExpireTime = Mark.RevealTime + VisibleSeconds;
			++Counts[Category];
		}
	};
	for (TActorIterator<AGamerResourceNode> It(World); It; ++It)
	{
		const FGamerResourceNodeDef* Def = It->GetDef();
		if (Def && !It->IsDepleted())
		{
			Add(It->GetActorLocation() + FVector(0.f, 0.f, 60.f), CategoryOfNode(*Def));   // middle of the object, not its feet
		}
	}
	const int32 ItemsCategory = CategoryIndex(TEXT("items"));
	for (TActorIterator<AGamerGroundItem> It(World); It; ++It)
	{
		Add(It->GetActorLocation() + FVector(0.f, 0.f, 30.f), ItemsCategory);
	}
	UE_LOG(LogGamer, Log, TEXT("Scan: %d marks within %.0f m (%s)"), Marks.Num(), RadiusCm / 100.f, *DescribeCounts());
	return true;
}

FString UGamerScanComponent::DescribeCounts() const
{
	TArray<FString> Parts;
	for (int32 Index = 0; Index < Categories.Num(); ++Index)
	{
		Parts.Add(FString::Printf(TEXT("%s=%d"), *Categories[Index].Id.ToString(), GetCount(Index)));
	}
	return FString::Join(Parts, TEXT(" "));
}
