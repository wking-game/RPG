#include "GamerFarmBuilding.h"

#include "Gamer.h"
#include "GamerCharacter.h"
#include "GamerDataSubsystem.h"
#include "GamerGameState.h"
#include "GamerGroundItem.h"
#include "GamerInventoryComponent.h"
#include "GamerResourceNode.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Engine/World.h"
#include "HAL/IConsoleManager.h"
#include "Math/RandomStream.h"
#include "Misc/CommandLine.h"
#include "Misc/Parse.h"
#include "Net/UnrealNetwork.h"
#include "TimerManager.h"

namespace
{
	TAutoConsoleVariable<float> CVarFarmTimeScale(
		TEXT("gamer.FarmTimeScale"),
		1.f,
		TEXT("Speeds up fields, composters and rain catchers (tests: 100 = a hundred times faster). 1 = normal."));

	/** Length of a game day in seconds (as for resources, villagers and spoiling). */
	constexpr float DayLengthSeconds = 1800.f;
	constexpr float TickSeconds = 2.f;
	/** Distance between crop plants in a field (cm). */
	constexpr float CropSpacing = 50.f;

	FString ItemName(const UObject* Context, FName Item)
	{
		const UGamerDataSubsystem* Data = UGamerDataSubsystem::Get(Context);
		const FGamerItemDef* Def = Data ? Data->FindItem(Item) : nullptr;
		return Def ? Def->Name : Item.ToString();
	}

	int32 Pct(float Value) { return FMath::Clamp(FMath::RoundToInt(Value * 100.f), 0, 100); }
}

AGamerFarmBuilding::AGamerFarmBuilding()
{
	Crops = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("Crops"));
	Crops->SetupAttachment(GetRootComponent());
	// The field is scaled to its drawn size; the plants keep their real size.
	Crops->SetUsingAbsoluteScale(true);
	Crops->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Crops->SetCastShadow(true);

	SoilTiles = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("SoilTiles"));
	SoilTiles->SetupAttachment(GetRootComponent());
	SoilTiles->SetUsingAbsoluteScale(true);
	SoilTiles->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AGamerFarmBuilding::UpdateSoil()
{
	if (!SoilTiles || !Mesh || IsPreview() || GetNetMode() == NM_DedicatedServer || GetKind() != EGamerFarmKind::Field)
	{
		return;
	}
	UStaticMesh* Tile = Mesh->GetStaticMesh();
	const bool bShow = IsFinished() && Tile && !Tile->GetPathName().StartsWith(TEXT("/Engine/"));
	if (bShow == bSoilShown)
	{
		return;
	}
	bSoilShown = bShow;
	Mesh->SetVisibility(!bShow); // hidden, but its collision stays
	SoilTiles->ClearInstances();
	if (!bShow)
	{
		return;
	}
	SoilTiles->SetStaticMesh(Tile);
	const FVector2D Size = GetSizeCm();
	const int32 NX = FMath::Max(1, FMath::CeilToInt(Size.X / 100.f - 0.01f));
	const int32 NY = FMath::Max(1, FMath::CeilToInt(Size.Y / 100.f - 0.01f));
	const FVector TileScale(Size.X / NX / 100.f, Size.Y / NY / 100.f, 1.f);
	TArray<FTransform> Tiles;
	for (int32 X = 0; X < NX; ++X)
	{
		for (int32 Y = 0; Y < NY; ++Y)
		{
			const FVector Local(-Size.X * 0.5f + (X + 0.5f) * Size.X / NX, -Size.Y * 0.5f + (Y + 0.5f) * Size.Y / NY, 0.f);
			Tiles.Add(FTransform(FRotator::ZeroRotator, Local, TileScale));
		}
	}
	SoilTiles->AddInstances(Tiles, false);
}

void AGamerFarmBuilding::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AGamerFarmBuilding, Seed);
	DOREPLIFETIME(AGamerFarmBuilding, Growth);
	DOREPLIFETIME(AGamerFarmBuilding, Water);
	DOREPLIFETIME(AGamerFarmBuilding, bFertilized);
	DOREPLIFETIME(AGamerFarmBuilding, Stored);
	DOREPLIFETIME(AGamerFarmBuilding, Amount);
}

void AGamerFarmBuilding::BeginPlay()
{
	Super::BeginPlay();
	if (HasAuthority() && !IsPreview())
	{
		float Scale = 0.f;
		if (FParse::Value(FCommandLine::Get(), TEXT("GamerFarmTimeScale="), Scale) && Scale > 0.f)
		{
			CVarFarmTimeScale->Set(Scale, ECVF_SetByCommandline);
		}
		GetWorldTimerManager().SetTimer(FarmTimer, this, &AGamerFarmBuilding::TickFarm, TickSeconds, true, FMath::FRandRange(0.5f, TickSeconds));
		// Planting is paid when placing (except "material on build": then once the plan is hammered).
		if (GetKind() == EGamerFarmKind::Tree && (IsFinished() || AGamerGameState::Get(this).BuildFlow != EGamerBuildFlow::MaterialOnBuild))
		{
			PlantTree();
			return;
		}
	}
	UpdateCrops();
	if (GetNetMode() != NM_DedicatedServer && GetKind() == EGamerFarmKind::Field && !IsPreview())
	{
		// The plan becomes a finished field later (hammer): check now and then.
		UpdateSoil();
		GetWorldTimerManager().SetTimer(SoilTimer, this, &AGamerFarmBuilding::UpdateSoil, 1.f, true);
	}
}

void AGamerFarmBuilding::PlantTree()
{
	const FGamerFarmProps* Farm = GetFarm();
	if (!Farm || IsActorBeingDestroyed())
	{
		return;
	}
	const FTransform Transform(FRotator(0.f, GetActorRotation().Yaw, 0.f), GetActorLocation());
	AGamerResourceNode* Node = GetWorld()->SpawnActorDeferred<AGamerResourceNode>(AGamerResourceNode::StaticClass(), Transform, nullptr, nullptr,
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
	if (Node)
	{
		Node->SetNodeId(Farm->Node);
		Node->SetPlanted(Farm->TreeGrowDays);
		Node->FinishSpawning(Transform);
		UE_LOG(LogGamer, Log, TEXT("Tree planted: %s at %s, full grown in %g game days"), *Farm->Node.ToString(), *Transform.GetLocation().ToCompactString(),
			Farm->TreeGrowDays);
	}
	Destroy();
}

const FGamerFarmProps* AGamerFarmBuilding::GetFarm() const
{
	const FGamerBuildableDef* Def = GetDef();
	return Def && Def->Farm.Kind != EGamerFarmKind::None ? &Def->Farm : nullptr;
}

EGamerFarmKind AGamerFarmBuilding::GetKind() const
{
	const FGamerFarmProps* Farm = GetFarm();
	return Farm ? Farm->Kind : EGamerFarmKind::None;
}

FVector2D AGamerFarmBuilding::GetSizeCm() const
{
	const FVector Scale = GetActorScale3D();
	return FVector2D(Scale.X * 100.f, Scale.Y * 100.f);
}

int32 AGamerFarmBuilding::GetUnits() const
{
	const FGamerFarmProps* Farm = GetFarm();
	if (!Farm || Farm->Kind != EGamerFarmKind::Field)
	{
		return 1;
	}
	const FVector2D Size = GetSizeCm();
	const float AreaM2 = Size.X * Size.Y / 10000.f;
	return FMath::Max(1, FMath::CeilToInt(AreaM2 / FMath::Max(0.1f, Farm->UnitM2) - 0.01f));
}

float AGamerFarmBuilding::ToGameDays(float Seconds) const
{
	return Seconds * FMath::Max(0.f, CVarFarmTimeScale.GetValueOnGameThread()) / DayLengthSeconds;
}

void AGamerFarmBuilding::TickFarm()
{
	const FGamerFarmProps* Farm = GetFarm();
	if (!Farm || !IsFinished())
	{
		return;
	}
	if (Farm->Kind == EGamerFarmKind::Tree)
	{
		PlantTree(); // a hammered plan in "material on build"
		return;
	}
	const float Days = ToGameDays(TickSeconds);
	const UGamerDataSubsystem* Data = UGamerDataSubsystem::Get(this);
	switch (Farm->Kind)
	{
	case EGamerFarmKind::Field:
	{
		const FGamerItemDef* SeedDef = Data && !Seed.IsNone() ? Data->FindItem(Seed) : nullptr;
		if (!SeedDef || Growth >= 1.f)
		{
			return;
		}
		const AGamerGameState* State = GetWorld()->GetGameState<AGamerGameState>();
		const float Phase = State ? State->GetSeasonPhase() : 1.f;
		const bool bSeason = Phase >= Farm->GrowFrom && Phase < Farm->GrowTo;
		if (Water > 0.f && bSeason)
		{
			const float Before = Growth;
			Growth = FMath::Min(1.f, Growth + Days / FMath::Max(0.01f, SeedDef->Plant.GrowDays));
			Water = FMath::Max(0.f, Water - Days * Farm->WaterPerDay);
			if (FMath::FloorToInt(Before * 3.f) != FMath::FloorToInt(Growth * 3.f) || Growth >= 1.f)
			{
				UE_LOG(LogGamer, Log, TEXT("Field %s: %s %d %%, water %d %%%s"), *GetName(), *Seed.ToString(), Pct(Growth), Pct(Water),
					Growth >= 1.f ? TEXT(" - ripe") : TEXT(""));
			}
			UpdateCrops();
			ForceNetUpdate();
		}
		return;
	}
	case EGamerFarmKind::Composter:
	{
		// Amount: whole part = finished fertilizer, fraction = progress of the one in work.
		const int32 Ready = FMath::FloorToInt(Amount);
		if (Stored >= Farm->PerOutput && Ready < Farm->Capacity)
		{
			Amount += Days / FMath::Max(0.01f, Farm->Rate);
			if (FMath::FloorToInt(Amount) > Ready)
			{
				Stored -= Farm->PerOutput;
				UE_LOG(LogGamer, Log, TEXT("Composter %s: +1 %s (%d ready, %d inside)"), *GetName(), *Farm->Output.ToString(), FMath::FloorToInt(Amount), Stored);
			}
			ForceNetUpdate();
		}
		return;
	}
	case EGamerFarmKind::Water:
	{
		// No weather yet: the rain catcher fills at the average rate from the data.
		if (Amount < Farm->Capacity)
		{
			const int32 Before = FMath::FloorToInt(Amount);
			Amount = FMath::Min(static_cast<float>(Farm->Capacity), Amount + Days * Farm->Rate);
			if (FMath::FloorToInt(Amount) != Before)
			{
				UE_LOG(LogGamer, Log, TEXT("Rain catcher %s: %d of %d"), *GetName(), FMath::FloorToInt(Amount), Farm->Capacity);
			}
			ForceNetUpdate();
		}
		return;
	}
	default:
		return;
	}
}

// --- Actions -------------------------------------------------------------------------------------

EGamerFieldNeed AGamerFarmBuilding::GetFieldNeed() const
{
	if (GetKind() != EGamerFarmKind::Field || !IsFinished())
	{
		return EGamerFieldNeed::None;
	}
	if (IsRipe())
	{
		return EGamerFieldNeed::Harvest;
	}
	if (Seed.IsNone())
	{
		return EGamerFieldNeed::Sow;
	}
	if (Water < 0.5f)
	{
		return EGamerFieldNeed::Water;
	}
	return bFertilized ? EGamerFieldNeed::None : EGamerFieldNeed::Fertilize;
}

int32 AGamerFarmBuilding::WaterItemsNeeded() const
{
	return FMath::Max(0, FMath::CeilToInt((1.f - Water) * GetUnits() - 0.01f));
}

bool AGamerFarmBuilding::Sow(FName SeedItem)
{
	if (!HasAuthority() || GetKind() != EGamerFarmKind::Field || !Seed.IsNone() || SeedItem.IsNone())
	{
		return false;
	}
	Seed = SeedItem;
	Growth = 0.f;
	bFertilized = false;
	UpdateCrops();
	ForceNetUpdate();
	UE_LOG(LogGamer, Log, TEXT("Field %s sown: %s on %d units (water %d %%)"), *GetName(), *Seed.ToString(), GetUnits(), Pct(Water));
	return true;
}

void AGamerFarmBuilding::AddWater(int32 Count)
{
	if (!HasAuthority() || Count <= 0)
	{
		return;
	}
	Water = FMath::Min(1.f, Water + static_cast<float>(Count) / GetUnits());
	ForceNetUpdate();
	UE_LOG(LogGamer, Log, TEXT("Field %s watered with %d (now %d %%)"), *GetName(), Count, Pct(Water));
}

bool AGamerFarmBuilding::Fertilize()
{
	if (!HasAuthority() || bFertilized || Seed.IsNone())
	{
		return false;
	}
	bFertilized = true;
	ForceNetUpdate();
	UE_LOG(LogGamer, Log, TEXT("Field %s fertilized"), *GetName());
	return true;
}

TArray<FGamerItemAmount> AGamerFarmBuilding::HarvestCrop()
{
	TArray<FGamerItemAmount> Out;
	const FGamerFarmProps* Farm = GetFarm();
	const UGamerDataSubsystem* Data = UGamerDataSubsystem::Get(this);
	const FGamerItemDef* SeedDef = Data && !Seed.IsNone() ? Data->FindItem(Seed) : nullptr;
	if (!HasAuthority() || !IsRipe() || !Farm || !SeedDef)
	{
		return Out;
	}
	const int32 Units = GetUnits();
	for (const FGamerDrop& Drop : SeedDef->Plant.Harvest)
	{
		int32 Count = 0;
		for (int32 Unit = 0; Unit < Units; ++Unit)
		{
			Count += FMath::RandRange(Drop.Min, Drop.Max);
		}
		if (bFertilized)
		{
			Count = FMath::RoundToInt(Count * (1.f + Farm->FertilizerBonus));
		}
		if (Count > 0)
		{
			FGamerItemAmount& Entry = Out.AddDefaulted_GetRef();
			Entry.Item = Drop.Item;
			Entry.Count = Count;
		}
	}
	UE_LOG(LogGamer, Log, TEXT("Field %s harvested (%s, %d units, fertilized %d)"), *GetName(), *Seed.ToString(), Units, bFertilized ? 1 : 0);
	Seed = NAME_None;
	Growth = 0.f;
	bFertilized = false;
	UpdateCrops();
	ForceNetUpdate();
	return Out;
}

int32 AGamerFarmBuilding::GetReadyOutput() const
{
	const EGamerFarmKind Kind = GetKind();
	return Kind == EGamerFarmKind::Composter || Kind == EGamerFarmKind::Water ? FMath::FloorToInt(Amount) : 0;
}

int32 AGamerFarmBuilding::TakeOutput(int32 Max)
{
	const int32 Taken = FMath::Clamp(Max, 0, GetReadyOutput());
	if (HasAuthority() && Taken > 0)
	{
		Amount -= Taken;
		ForceNetUpdate();
	}
	return Taken;
}

int32 AGamerFarmBuilding::GetInputSpace() const
{
	const FGamerFarmProps* Farm = GetFarm();
	return Farm && Farm->Kind == EGamerFarmKind::Composter ? FMath::Max(0, Farm->Capacity - Stored) : 0;
}

void AGamerFarmBuilding::PutInput(int32 Count)
{
	if (HasAuthority() && Count > 0)
	{
		Stored += FMath::Min(Count, GetInputSpace());
		ForceNetUpdate();
	}
}

// --- Players ---------------------------------------------------------------------------------------

int32 AGamerFarmBuilding::Give(AGamerCharacter* User, FName Item, int32 Count) const
{
	if (Count <= 0 || Item.IsNone())
	{
		return 0;
	}
	UGamerInventoryComponent* Inventory = User ? User->GetInventory() : nullptr;
	const int32 Lost = Inventory ? Inventory->AddItem(Item, Count) : Count;
	if (Lost > 0)
	{
		// Does not fit: put it on the ground next to the building instead of losing it.
		AGamerGroundItem::SpawnFallen(GetWorld(), Item, Lost, 1.f, GetActorLocation() + FVector(0.f, 0.f, 5.f), 0.f);
	}
	return Count;
}

FString AGamerFarmBuilding::Use(AGamerCharacter* User)
{
	const FGamerFarmProps* Farm = GetFarm();
	if (!HasAuthority() || !Farm || !User || !User->GetInventory())
	{
		return FString();
	}
	if (!IsFinished())
	{
		return TEXT("Erst fertig bauen (Hammer).");
	}
	switch (Farm->Kind)
	{
	case EGamerFarmKind::Field: return UseField(User, *Farm);
	case EGamerFarmKind::Composter: return UseComposter(User, *Farm);
	case EGamerFarmKind::Water: return UseWater(User, *Farm);
	default: return FString();
	}
}

FString AGamerFarmBuilding::UseField(AGamerCharacter* User, const FGamerFarmProps& Farm)
{
	UGamerInventoryComponent* Inventory = User->GetInventory();
	const UGamerDataSubsystem* Data = UGamerDataSubsystem::Get(this);
	if (!Data)
	{
		return FString();
	}
	const int32 Units = GetUnits();
	auto Take = [Inventory](FName Item, int32 Count)
	{
		FGamerItemAmount Amount;
		Amount.Item = Item;
		Amount.Count = Count;
		return Count > 0 && Inventory->RemoveItems({ Amount });
	};

	// 1) Ripe: harvest (fertilizer adds its bonus to every product).
	if (IsRipe())
	{
		TArray<FString> Parts;
		for (const FGamerItemAmount& Entry : HarvestCrop())
		{
			Give(User, Entry.Item, Entry.Count);
			Parts.Add(FString::Printf(TEXT("+%d %s"), Entry.Count, *ItemName(this, Entry.Item)));
		}
		UE_LOG(LogGamer, Log, TEXT("Field %s harvested by a player: %s - inventory: %s"), *GetName(), *FString::Join(Parts, TEXT(" ")), *Inventory->Describe());
		return TEXT("Geerntet: ") + FString::Join(Parts, TEXT("  "));
	}

	// 2) Empty: sow the seed in hand, else the first seed in the inventory; one seed per unit.
	if (Seed.IsNone())
	{
		FName Chosen;
		if (const FGamerItemDef* Held = Inventory->GetSelectedItemDef(); Held && Held->Plant.IsSeed())
		{
			Chosen = Held->Id;
		}
		for (const FGamerInventorySlot& Slot : Inventory->GetSlots())
		{
			const FGamerItemDef* Def = Slot.IsEmpty() || !Chosen.IsNone() ? nullptr : Data->FindItem(Slot.Item);
			if (Def && Def->Plant.IsSeed())
			{
				Chosen = Def->Id;
			}
		}
		if (Chosen.IsNone())
		{
			return TEXT("Du brauchst Saatgut (z. B. Möhrensamen, Saatkartoffeln, Leinsamen).");
		}
		if (!Take(Chosen, Units))
		{
			return FString::Printf(TEXT("Für dieses Feld brauchst du %d %s (je %g m² eines)."), Units, *ItemName(this, Chosen), Farm.UnitM2);
		}
		Sow(Chosen);
		return FString::Printf(TEXT("Gesät: %d %s.%s"), Units, *ItemName(this, Seed), Water <= 0.f ? TEXT(" Jetzt gießen.") : TEXT(""));
	}

	// 3) Growing: water when it is getting dry (as much as needed and carried), then fertilize once.
	const int32 HaveWater = Inventory->CountOf(Farm.WaterItem);
	if (Water < 0.5f && HaveWater > 0)
	{
		const int32 Poured = FMath::Min(HaveWater, WaterItemsNeeded());
		if (Take(Farm.WaterItem, Poured))
		{
			AddWater(Poured);
			return FString::Printf(TEXT("Gegossen (%d %s, Feld %d %%)."), Poured, *ItemName(this, Farm.WaterItem), Pct(Water));
		}
	}
	if (!bFertilized && Inventory->CountOf(Farm.FertilizerItem) > 0)
	{
		if (!Take(Farm.FertilizerItem, Units))
		{
			return FString::Printf(TEXT("Zum Düngen brauchst du %d %s."), Units, *ItemName(this, Farm.FertilizerItem));
		}
		Fertilize();
		return FString::Printf(TEXT("Gedüngt: +%d %% Ertrag."), Pct(Farm.FertilizerBonus));
	}
	if (Water <= 0.f)
	{
		return FString::Printf(TEXT("Das Feld ist trocken. Du brauchst %s (Regenfänger)."), *ItemName(this, Farm.WaterItem));
	}
	return GetStatusText();
}

FString AGamerFarmBuilding::UseComposter(AGamerCharacter* User, const FGamerFarmProps& Farm)
{
	UGamerInventoryComponent* Inventory = User->GetInventory();
	if (GetReadyOutput() > 0)
	{
		const int32 Fits = TakeOutput(FMath::Min(GetReadyOutput(), Inventory->SpaceFor(Farm.Output)));
		if (Fits <= 0)
		{
			return TEXT("Kein Platz im Inventar.");
		}
		Inventory->AddItem(Farm.Output, Fits);
		UE_LOG(LogGamer, Log, TEXT("Composter %s: took %d %s"), *GetName(), Fits, *Farm.Output.ToString());
		return FString::Printf(TEXT("+%d %s"), Fits, *ItemName(this, Farm.Output));
	}
	int32 Put = 0;
	for (const FName Input : Farm.Inputs)
	{
		const int32 Count = FMath::Min(Inventory->CountOf(Input), GetInputSpace());
		if (Count > 0)
		{
			FGamerItemAmount Take;
			Take.Item = Input;
			Take.Count = Count;
			if (Inventory->RemoveItems({ Take }))
			{
				PutInput(Count);
				Put += Count;
			}
		}
	}
	if (Put > 0)
	{
		UE_LOG(LogGamer, Log, TEXT("Composter %s: %d put in (%d of %d)"), *GetName(), Put, Stored, Farm.Capacity);
		return FString::Printf(TEXT("%d in den Komposter gelegt (%d/%d)."), Put, Stored, Farm.Capacity);
	}
	return Stored >= Farm.Capacity ? TEXT("Der Komposter ist voll.") : GetStatusText();
}

FString AGamerFarmBuilding::UseWater(AGamerCharacter* User, const FGamerFarmProps& Farm)
{
	UGamerInventoryComponent* Inventory = User->GetInventory();
	if (GetReadyOutput() <= 0)
	{
		return GetStatusText();
	}
	const int32 Fits = TakeOutput(FMath::Min(GetReadyOutput(), Inventory->SpaceFor(Farm.Output)));
	if (Fits <= 0)
	{
		return TEXT("Kein Platz im Inventar.");
	}
	Inventory->AddItem(Farm.Output, Fits);
	UE_LOG(LogGamer, Log, TEXT("Rain catcher %s: took %d %s"), *GetName(), Fits, *Farm.Output.ToString());
	return FString::Printf(TEXT("+%d %s"), Fits, *ItemName(this, Farm.Output));
}

FString AGamerFarmBuilding::GetUseHint(const UGamerInventoryComponent* Inventory) const
{
	const FGamerFarmProps* Farm = GetFarm();
	if (!Farm || !IsFinished())
	{
		return FString();
	}
	switch (Farm->Kind)
	{
	case EGamerFarmKind::Field:
		if (IsRipe()) { return TEXT("ernten"); }
		if (Seed.IsNone()) { return FString::Printf(TEXT("säen (%d Saatgut)"), GetUnits()); }
		if (Water < 0.5f && Inventory && Inventory->CountOf(Farm->WaterItem) > 0) { return TEXT("gießen"); }
		if (!bFertilized && Inventory && Inventory->CountOf(Farm->FertilizerItem) > 0) { return FString::Printf(TEXT("düngen (%d)"), GetUnits()); }
		return TEXT("ansehen");
	case EGamerFarmKind::Composter:
		return Amount >= 1.f ? TEXT("Dünger nehmen") : TEXT("Verdorbenes hineinlegen");
	case EGamerFarmKind::Water:
		return Amount >= 1.f ? TEXT("Wasser nehmen") : TEXT("ansehen");
	default:
		return FString();
	}
}

FString AGamerFarmBuilding::GetStatusText() const
{
	const FGamerFarmProps* Farm = GetFarm();
	const FGamerBuildableDef* Def = GetDef();
	if (!Farm || !Def)
	{
		return FString();
	}
	switch (Farm->Kind)
	{
	case EGamerFarmKind::Field:
	{
		const FVector2D Size = GetSizeCm();
		const FString Name = FString::Printf(TEXT("%s %.1f × %.1f m"), *Def->Name, Size.X / 100.f, Size.Y / 100.f);
		if (Seed.IsNone())
		{
			return FString::Printf(TEXT("%s: leer, Wasser %d %%"), *Name, Pct(Water));
		}
		const AGamerGameState* State = GetWorld() ? GetWorld()->GetGameState<AGamerGameState>() : nullptr;
		const float Phase = State ? State->GetSeasonPhase() : 1.f;
		const bool bSeason = Phase >= Farm->GrowFrom && Phase < Farm->GrowTo;
		return FString::Printf(TEXT("%s: %s %d %%, Wasser %d %%, %s%s"), *Name, *ItemName(this, Seed), Pct(Growth), Pct(Water),
			bFertilized ? TEXT("gedüngt") : TEXT("ungedüngt"),
			IsRipe() ? TEXT(" – reif") : !bSeason ? TEXT(" – ruht im Winter") : Water <= 0.f ? TEXT(" – trocken, wächst nicht") : TEXT(""));
	}
	case EGamerFarmKind::Composter:
		return FString::Printf(TEXT("%s: %d/%d drin, %d %s fertig, nächster %d %%"), *Def->Name, Stored, Farm->Capacity, FMath::FloorToInt(Amount),
			*ItemName(this, Farm->Output), Stored >= Farm->PerOutput ? Pct(FMath::Frac(Amount)) : 0);
	case EGamerFarmKind::Water:
		return FString::Printf(TEXT("%s: %d/%d %s (füllt sich)"), *Def->Name, FMath::FloorToInt(Amount), Farm->Capacity, *ItemName(this, Farm->Output));
	default:
		return FString();
	}
}

void AGamerFarmBuilding::OnRep_Farm()
{
	UpdateCrops();
}

void AGamerFarmBuilding::UpdateCrops()
{
	if (!Crops || GetNetMode() == NM_DedicatedServer)
	{
		return;
	}
	const UGamerDataSubsystem* Data = UGamerDataSubsystem::Get(this);
	const FGamerItemDef* SeedDef = Data && !Seed.IsNone() ? Data->FindItem(Seed) : nullptr;
	if (GetKind() != EGamerFarmKind::Field || !SeedDef)
	{
		if (Crops->GetInstanceCount() > 0)
		{
			Crops->ClearInstances();
		}
		AppliedStage = -1;
		AppliedSeed = NAME_None;
		return;
	}
	// Stage: sprout, young, ripe (as many as the data has models; placeholders grow in size).
	const int32 Stages = FMath::Max(1, SeedDef->Plant.Meshes.Num());
	const int32 Stage = Growth >= 1.f ? Stages - 1 : FMath::Clamp(FMath::FloorToInt(Growth * Stages), 0, FMath::Max(0, Stages - 2));
	const FVector Scale = GetActorScale3D();
	if (Stage == AppliedStage && Seed == AppliedSeed && Scale.Equals(AppliedScale) && SeedDef->Plant.Meshes.Num() > 0)
	{
		return;
	}
	AppliedStage = Stage;
	AppliedSeed = Seed;
	AppliedScale = Scale;
	UStaticMesh* Model = SeedDef->Plant.Meshes.IsValidIndex(Stage) ? Cast<UStaticMesh>(SeedDef->Plant.Meshes[Stage].TryLoad()) : nullptr;
	float Size = 1.f;
	const bool bPlaceholder = !Model;
	if (bPlaceholder)
	{
		Model = LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Cone.Cone"));
		Size = 0.1f + 0.3f * FMath::Clamp(Growth, 0.f, 1.f);
	}
	if (Crops->GetStaticMesh() != Model)
	{
		Crops->SetStaticMesh(Model);
	}
	Crops->ClearInstances();

	// Rows over the whole drawn field (instance positions are in cm: the component ignores the field's scale),
	// each plant put down on the ground under it so the rows follow small bumps.
	const FVector2D Field = GetSizeCm();
	const int32 Columns = FMath::Max(1, FMath::FloorToInt(Field.X / CropSpacing));
	const int32 Rows = FMath::Max(1, FMath::FloorToInt(Field.Y / CropSpacing));
	FRandomStream Random(GetTypeHash(GetActorLocation().ToString()));
	const FTransform ActorNoScale(GetActorRotation(), GetActorLocation());
	FCollisionQueryParams Params(SCENE_QUERY_STAT(GamerCropGround), false, this);
	TArray<FTransform> Instances;
	for (int32 Row = 0; Row < Rows; ++Row)
	{
		for (int32 Column = 0; Column < Columns; ++Column)
		{
			FVector Local((Column + 0.5f) * Field.X / Columns - Field.X * 0.5f + Random.FRandRange(-6.f, 6.f),
				(Row + 0.5f) * Field.Y / Rows - Field.Y * 0.5f + Random.FRandRange(-5.f, 5.f), 18.f);
			const FVector World = ActorNoScale.TransformPosition(Local);
			FHitResult Hit;
			if (GetWorld()->LineTraceSingleByChannel(Hit, World + FVector(0.f, 0.f, 150.f), World - FVector(0.f, 0.f, 200.f), ECC_WorldStatic, Params))
			{
				Local.Z = FMath::Max(Local.Z, ActorNoScale.InverseTransformPosition(Hit.ImpactPoint).Z);
			}
			const float PlantSize = Size * Random.FRandRange(0.85f, 1.15f);
			Local.Z += bPlaceholder ? PlantSize * 50.f : 0.f;
			Instances.Add(FTransform(FRotator(0.f, Random.FRandRange(0.f, 360.f), 0.f), Local, FVector(PlantSize)));
		}
	}
	Crops->AddInstances(Instances, false);
}
