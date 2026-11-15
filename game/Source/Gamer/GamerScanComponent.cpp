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
	AddCategory(TEXT("food"), TEXT("Nahrung"), FLinearColor(0.95f, 0.45f, 0.35f), TEXT("N"));
	AddCategory(TEXT("wood"), TEXT("Holz"), FLinearColor(0.80f, 0.60f, 0.30f), TEXT("H"));
	AddCategory(TEXT("stone"), TEXT("Stein"), FLinearColor(0.70f, 0.75f, 0.85f), TEXT("S"));
	AddCategory(TEXT("plants"), TEXT("Pflanzen"), FLinearColor(0.45f, 0.85f, 0.40f), TEXT("P"));
	AddCategory(TEXT("items"), TEXT("Gegenstände"), FLinearColor(0.95f, 0.85f, 0.30f), TEXT("G"));

	const FString Path = FPaths::ProjectContentDir() / TEXT("Data/scan.json");
	FString Text;
	TSharedPtr<FJsonObject> Root;
	if (!FFileHelper::LoadFileToString(Text, *Path) || !FJsonSerializer::Deserialize(TJsonReaderFactory<>::Create(Text), Root) || !Root.IsValid())
	{
		UE_LOG(LogGamer, Warning, TEXT("Scan: %s not readable, using defaults"), *Path);
		return;
	}
	RadiusCm = static_cast<float>(Root->GetNumberField(TEXT("radius_m"))) * 100.f;
	DurationSeconds = static_cast<float>(Root->GetNumberField(TEXT("duration_s")));
	CooldownSeconds = static_cast<float>(Root->GetNumberField(TEXT("cooldown_s")));
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
	UE_LOG(LogGamer, Log, TEXT("Scan: loaded radius %.0f m, %.0f s visible, %.0f s cooldown, %d categories"),
		RadiusCm / 100.f, DurationSeconds, CooldownSeconds, Categories.Num());
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

bool UGamerScanComponent::IsShowing() const
{
	const UWorld* World = GetWorld();
	return World && Marks.Num() > 0 && World->GetTimeSeconds() < Marks[0].ExpireTime;
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
			Work->ShowMessage(FString::Printf(TEXT("Scan wieder bereit in %.0f s"), FMath::CeilToFloat(Left)));
		}
		return false;
	}
	LastScanTime = World->GetTimeSeconds();
	Marks.Reset();
	Counts.Init(0, Categories.Num());
	const FVector Center = Owner->GetActorLocation();
	const double Expire = World->GetTimeSeconds() + DurationSeconds;
	auto Add = [&](const FVector& Location, int32 Category)
	{
		if (Category != INDEX_NONE && FVector::Dist(Location, Center) <= RadiusCm)
		{
			FGamerScanMark& Mark = Marks.AddDefaulted_GetRef();
			Mark.Location = Location;
			Mark.Category = Category;
			Mark.ExpireTime = Expire;
			++Counts[Category];
		}
	};
	for (TActorIterator<AGamerResourceNode> It(World); It; ++It)
	{
		const FGamerResourceNodeDef* Def = It->GetDef();
		if (Def && !It->IsDepleted())
		{
			// Mark at the middle of the object, not at its feet.
			Add(It->GetActorLocation() + FVector(0.f, 0.f, 60.f), CategoryOfNode(*Def));
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
