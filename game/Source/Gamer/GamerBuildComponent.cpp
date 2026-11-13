#include "GamerBuildComponent.h"

#include "Gamer.h"
#include "GamerBuilding.h"
#include "GamerCharacter.h"
#include "GamerDataSubsystem.h"
#include "GamerGameState.h"
#include "GamerInventoryComponent.h"
#include "GamerWorkComponent.h"
#include "Engine/World.h"
#include "GameFramework/PlayerState.h"

namespace
{
	int32 PlayerIdOf(const AActor* Owner)
	{
		const APawn* Pawn = Cast<APawn>(Owner);
		const APlayerState* PS = Pawn ? Pawn->GetPlayerState() : nullptr;
		return PS ? PS->GetPlayerId() : -1;
	}

	FGamerItemAmount MakeAmount(FName Item, int32 Count)
	{
		FGamerItemAmount Amount;
		Amount.Item = Item;
		Amount.Count = Count;
		return Amount;
	}

	FString ItemName(const UGamerDataSubsystem* Data, FName Id)
	{
		const FGamerItemDef* Def = Data ? Data->FindItem(Id) : nullptr;
		return Def ? Def->Name : Id.ToString();
	}

	FString AmountsText(const UGamerDataSubsystem* Data, const TArray<FGamerItemAmount>& Amounts)
	{
		TArray<FString> Parts;
		for (const FGamerItemAmount& Amount : Amounts)
		{
			if (Amount.Count > 0)
			{
				Parts.Add(FString::Printf(TEXT("%d %s"), Amount.Count, *ItemName(Data, Amount.Item)));
			}
		}
		return FString::Join(Parts, TEXT(", "));
	}

	TArray<FGamerItemAmount> Missing(const UGamerInventoryComponent& Inventory, const TArray<FGamerItemAmount>& Amounts)
	{
		TArray<FGamerItemAmount> Result;
		for (const FGamerItemAmount& Amount : Amounts)
		{
			const int32 Have = Inventory.CountOf(Amount.Item);
			if (Have < Amount.Count)
			{
				Result.Add(MakeAmount(Amount.Item, Amount.Count - Have));
			}
		}
		return Result;
	}

	const TCHAR* FlowName(EGamerBuildFlow Flow)
	{
		return Flow == EGamerBuildFlow::Blueprint ? TEXT("blueprint") : Flow == EGamerBuildFlow::Instant ? TEXT("instant") : TEXT("material");
	}
}

UGamerBuildComponent::UGamerBuildComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;
	SetIsReplicatedByDefault(true);
}

UGamerInventoryComponent* UGamerBuildComponent::GetInventory() const
{
	return GetOwner() ? GetOwner()->FindComponentByClass<UGamerInventoryComponent>() : nullptr;
}

UGamerWorkComponent* UGamerBuildComponent::GetWork() const
{
	return GetOwner() ? GetOwner()->FindComponentByClass<UGamerWorkComponent>() : nullptr;
}

void UGamerBuildComponent::Message(const FString& Text) const
{
	UE_LOG(LogGamer, Log, TEXT("Build message for player %d: %s"), PlayerIdOf(GetOwner()), *Text);
	if (UGamerWorkComponent* Work = GetWork())
	{
		if (GetOwner()->HasAuthority())
		{
			Work->ClientMessage(Text);
		}
		else
		{
			Work->ShowMessage(Text);
		}
	}
}

bool UGamerBuildComponent::HasToolFor(const FGamerBuildableDef& Def) const
{
	const UGamerInventoryComponent* Inventory = GetInventory();
	return Inventory && (!Def.RequiredTool.IsRequired() || Inventory->IsHoldingTool(Def.RequiredTool.Type, Def.RequiredTool.MinTier));
}

// --- Local player ---------------------------------------------------------------------------

void UGamerBuildComponent::SelectBuildable(FName BuildableId)
{
	if (Selected != BuildableId)
	{
		Selected = BuildableId;
		RotationSteps = 0;
		HidePreview();
	}
}

bool UGamerBuildComponent::IsPlacing() const
{
	const UGamerDataSubsystem* Data = UGamerDataSubsystem::Get(this);
	const FGamerBuildableDef* Def = Data && !Selected.IsNone() ? Data->FindBuildable(Selected) : nullptr;
	return Def && HasToolFor(*Def);
}

FGamerPlacement UGamerBuildComponent::ComputePlacement(FName BuildableId, const FVector& AimPoint, bool bAimHit, float ViewYaw,
	const FVector& AimNormal) const
{
	const UGamerDataSubsystem* Data = UGamerDataSubsystem::Get(this);
	const FGamerBuildableDef* Def = Data ? Data->FindBuildable(BuildableId) : nullptr;
	if (!Def)
	{
		return FGamerPlacement();
	}
	const EGamerPlacement Mode = AGamerGameState::Get(this).Placement;
	FGamerPlacement Result = GamerBuildSnap::FindPlacement(GetWorld(), *Def, AimPoint, bAimHit, ViewYaw, RotationSteps, Mode, AimNormal);
	if (Result.bValid && GetOwner() && FVector::Dist(GetOwner()->GetActorLocation(), Result.Transform.GetLocation()) > BuildReach)
	{
		Result.bValid = false;
		Result.Problem = TEXT("Zu weit weg");
	}
	return Result;
}

void UGamerBuildComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	const APawn* Pawn = Cast<APawn>(GetOwner());
	if (Pawn && Pawn->IsLocallyControlled())
	{
		UpdatePreview();
	}
}

void UGamerBuildComponent::UpdatePreview()
{
	const AGamerCharacter* Character = Cast<AGamerCharacter>(GetOwner());
	if (!Character || !IsPlacing())
	{
		HidePreview();
		Placement = FGamerPlacement();
		return;
	}

	// Aim like the gathering focus: from the camera through the crosshair.
	FVector ViewLocation, ViewDirection;
	Character->GetAimView(ViewLocation, ViewDirection);
	FHitResult Hit;
	FCollisionQueryParams Params(SCENE_QUERY_STAT(GamerBuildAim), false, GetOwner());
	if (Preview.IsValid())
	{
		Params.AddIgnoredActor(Preview.Get());
	}
	const FVector End = ViewLocation + ViewDirection * (Character->GetCameraDistance() + BuildReach);
	const bool bHit = GetWorld()->LineTraceSingleByChannel(Hit, ViewLocation, End, ECC_Visibility, Params);
	const FVector AimPoint = bHit ? Hit.ImpactPoint : End;
	const float ViewYaw = Character->GetControlRotation().Yaw;
	Placement = ComputePlacement(Selected, AimPoint, bHit, ViewYaw, bHit ? FVector(Hit.ImpactNormal) : FVector::UpVector);

	if (!Preview.IsValid() || Preview->GetBuildableId() != Selected)
	{
		HidePreview();
		AGamerBuilding* NewPreview = GetWorld()->SpawnActorDeferred<AGamerBuilding>(AGamerBuilding::StaticClass(),
			Placement.Transform, nullptr, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
		if (NewPreview)
		{
			NewPreview->InitPreview(Selected);
			NewPreview->FinishSpawning(Placement.Transform);
			Preview = NewPreview;
		}
	}
	if (AGamerBuilding* Shown = Preview.Get())
	{
		Shown->SetActorTransform(Placement.Transform);
		Shown->SetPreviewValid(Placement.bValid);
		Shown->SetActorHiddenInGame(false);
	}
}

void UGamerBuildComponent::HidePreview()
{
	if (AGamerBuilding* Shown = Preview.Get())
	{
		Shown->Destroy();
	}
	Preview.Reset();
}

void UGamerBuildComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	HidePreview();
	Super::EndPlay(EndPlayReason);
}

bool UGamerBuildComponent::TryPlace()
{
	if (!IsPlacing())
	{
		return false;
	}
	if (!Placement.bValid)
	{
		Message(Placement.Problem.IsEmpty() ? TEXT("Hier geht das nicht.") : Placement.Problem);
		return false;
	}
	const UGamerDataSubsystem* Data = UGamerDataSubsystem::Get(this);
	const FGamerBuildableDef* Def = Data ? Data->FindBuildable(Selected) : nullptr;
	const UGamerInventoryComponent* Inventory = GetInventory();
	if (Def && Inventory && AGamerGameState::Get(this).BuildFlow != EGamerBuildFlow::MaterialOnBuild)
	{
		const TArray<FGamerItemAmount> Lacking = Missing(*Inventory, Def->Cost);
		if (Lacking.Num() > 0)
		{
			Message(TEXT("Es fehlt: ") + AmountsText(Data, Lacking));
			return false;
		}
	}
	ServerPlace(Selected, Placement.Transform);
	return true;
}

// --- Server ---------------------------------------------------------------------------------

void UGamerBuildComponent::ServerPlace_Implementation(FName BuildableId, FTransform Transform)
{
	const UGamerDataSubsystem* Data = UGamerDataSubsystem::Get(this);
	const FGamerBuildableDef* Def = Data ? Data->FindBuildable(BuildableId) : nullptr;
	UGamerInventoryComponent* Inventory = GetInventory();
	AActor* Owner = GetOwner();
	if (!Def || !Inventory || !Owner || Def->bDeprecated)
	{
		return Message(TEXT("Dieses Bauteil gibt es nicht."));
	}
	if (!HasToolFor(*Def))
	{
		return Message(TEXT("Nimm den Hammer in die Hand."));
	}
	Transform.SetScale3D(FVector::OneVector);
	if (FVector::Dist(Owner->GetActorLocation(), Transform.GetLocation()) > BuildReach + 100.f)
	{
		return Message(TEXT("Zu weit weg."));
	}
	const FGamerWorldOptions& Options = AGamerGameState::Get(this);
	const FString Problem = GamerBuildSnap::CheckTransform(GetWorld(), *Def, Transform, Options.Placement);
	if (!Problem.IsEmpty())
	{
		return Message(Problem + TEXT("."));
	}

	const bool bPayNow = Options.BuildFlow != EGamerBuildFlow::MaterialOnBuild;
	if (bPayNow)
	{
		const TArray<FGamerItemAmount> Lacking = Missing(*Inventory, Def->Cost);
		if (Lacking.Num() > 0 || !Inventory->RemoveItems(Def->Cost))
		{
			return Message(TEXT("Es fehlt: ") + AmountsText(Data, Lacking));
		}
	}

	const float StartProgress = Options.BuildFlow == EGamerBuildFlow::Instant ? 1.f : 0.f;
	AGamerBuilding* Building = GetWorld()->SpawnActorDeferred<AGamerBuilding>(AGamerBuilding::StaticClass(), Transform,
		nullptr, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
	if (!Building)
	{
		if (bPayNow)
		{
			for (const FGamerItemAmount& Amount : Def->Cost)
			{
				Inventory->AddItem(Amount.Item, Amount.Count);
			}
		}
		return Message(TEXT("Bauen fehlgeschlagen."));
	}
	Building->InitBuilding(BuildableId, StartProgress);
	if (bPayNow)
	{
		Building->SetSpent(Def->Cost);
	}
	Building->FinishSpawning(Transform);

	UE_LOG(LogGamer, Log, TEXT("Build placed: player=%d id=%s flow=%s at %s yaw %.0f"), PlayerIdOf(Owner), *BuildableId.ToString(),
		FlowName(Options.BuildFlow), *Transform.GetLocation().ToCompactString(), Transform.Rotator().Yaw);
	if (Options.BuildFlow == EGamerBuildFlow::Instant)
	{
		Message(FString::Printf(TEXT("Gebaut: %s"), *Def->Name));
	}
	else
	{
		Message(FString::Printf(TEXT("Bauplan gesetzt: %s – mit dem Hammer fertig schlagen"), *Def->Name));
	}
}

void UGamerBuildComponent::ServerHammer_Implementation(AGamerBuilding* Building)
{
	const FGamerBuildableDef* Def = Building ? Building->GetDef() : nullptr;
	UGamerInventoryComponent* Inventory = GetInventory();
	UGamerWorkComponent* Work = GetWork();
	AActor* Owner = GetOwner();
	if (!Def || !Inventory || !Work || !Owner || Building->IsFinished() || Building->IsPreview())
	{
		return;
	}
	const float Interval = Work->GetStrikeInterval();
	const double Now = GetWorld()->GetTimeSeconds();
	if (Now - LastHammerTime < Interval * 0.8f || Work->IsBusy())
	{
		return;
	}
	LastHammerTime = Now;
	if (FVector::Dist(Owner->GetActorLocation(), Building->GetActorLocation()) > WorkReach)
	{
		return Message(TEXT("Zu weit weg."));
	}
	if (!HasToolFor(*Def))
	{
		return Message(TEXT("Nimm den Hammer in die Hand."));
	}

	const FGamerItemDef* Tool = Inventory->GetSelectedItemDef();
	const float Speed = Tool ? FMath::Max(0.1f, Tool->Tool.WorkSpeed) : 1.f;
	const FGamerWorldOptions& Options = AGamerGameState::Get(this);
	const UGamerDataSubsystem* Data = UGamerDataSubsystem::Get(this);

	if (Options.BuildFlow == EGamerBuildFlow::MaterialOnBuild)
	{
		// Every strike takes its share of the material; the last strike completes the cost.
		const int32 Strikes = FMath::Max(1, FMath::CeilToInt(Def->BuildSeconds / (Interval * Speed)));
		const int32 Strike = FMath::Min(Strikes, Building->GetStrikesDone() + 1);
		TArray<FGamerItemAmount> Spent = Building->GetSpent();
		TArray<FGamerItemAmount> Take;
		for (const FGamerItemAmount& Cost : Def->Cost)
		{
			const int32 Due = FMath::CeilToInt(static_cast<float>(Cost.Count) * Strike / Strikes);
			FGamerItemAmount* Already = Spent.FindByPredicate([&Cost](const FGamerItemAmount& A) { return A.Item == Cost.Item; });
			const int32 Delta = Due - (Already ? Already->Count : 0);
			if (Delta > 0)
			{
				Take.Add(MakeAmount(Cost.Item, Delta));
			}
		}
		const TArray<FGamerItemAmount> Lacking = Missing(*Inventory, Take);
		if (Lacking.Num() > 0 || !Inventory->RemoveItems(Take))
		{
			return Message(TEXT("Zum Weiterbauen fehlt: ") + AmountsText(Data, Lacking));
		}
		for (const FGamerItemAmount& Amount : Take)
		{
			FGamerItemAmount* Already = Spent.FindByPredicate([&Amount](const FGamerItemAmount& A) { return A.Item == Amount.Item; });
			if (Already)
			{
				Already->Count += Amount.Count;
			}
			else
			{
				Spent.Add(Amount);
			}
		}
		Building->SetSpent(Spent);
		Building->SetStrikesDone(Strike);
		Building->AddProgress(Strike >= Strikes ? 1.f : 1.f / Strikes);
	}
	else
	{
		Building->AddProgress(Interval * Speed / FMath::Max(0.1f, Def->BuildSeconds));
	}

	UE_LOG(LogGamer, Log, TEXT("Hammer: player=%d id=%s progress=%.2f"), PlayerIdOf(Owner), *Def->Id.ToString(), Building->GetProgress());
	if (Building->IsFinished())
	{
		UE_LOG(LogGamer, Log, TEXT("Build finished: player=%d id=%s inventory: %s"), PlayerIdOf(Owner), *Def->Id.ToString(), *Inventory->Describe());
		Message(FString::Printf(TEXT("Fertig: %s"), *Def->Name));
	}
}

void UGamerBuildComponent::ServerDemolish_Implementation(AGamerBuilding* Building)
{
	const FGamerBuildableDef* Def = Building ? Building->GetDef() : nullptr;
	UGamerInventoryComponent* Inventory = GetInventory();
	AActor* Owner = GetOwner();
	if (!Def || !Inventory || !Owner || Building->IsPreview())
	{
		return;
	}
	const FGamerWorldOptions& Options = AGamerGameState::Get(this);
	if (Building->IsFinished() && Options.Demolish == EGamerDemolish::Disabled)
	{
		return Message(TEXT("Abreißen ist in dieser Welt ausgeschaltet."));
	}
	if (FVector::Dist(Owner->GetActorLocation(), Building->GetActorLocation()) > WorkReach)
	{
		return Message(TEXT("Zu weit weg."));
	}
	if (!HasToolFor(*Def))
	{
		return Message(TEXT("Nimm den Hammer in die Hand."));
	}

	// Unfinished plans give back everything spent; finished pieces as the world option says.
	TArray<FGamerItemAmount> Refund = Building->GetSpent();
	if (Building->IsFinished() && Options.Demolish == EGamerDemolish::HalfRefund)
	{
		for (FGamerItemAmount& Amount : Refund)
		{
			Amount.Count /= 2;
		}
	}
	TArray<FGamerItemAmount> Given;
	int32 Lost = 0;
	for (const FGamerItemAmount& Amount : Refund)
	{
		if (Amount.Count > 0)
		{
			const int32 NotFitting = Inventory->AddItem(Amount.Item, Amount.Count);
			Lost += NotFitting;
			Given.Add(MakeAmount(Amount.Item, Amount.Count - NotFitting));
		}
	}
	const UGamerDataSubsystem* Data = UGamerDataSubsystem::Get(this);
	UE_LOG(LogGamer, Log, TEXT("Demolished: player=%d id=%s finished=%d refund [%s] inventory: %s"), PlayerIdOf(Owner),
		*Def->Id.ToString(), Building->IsFinished() ? 1 : 0, *AmountsText(Data, Given), *Inventory->Describe());
	FString Text = FString::Printf(TEXT("Abgerissen: %s"), *Def->Name);
	if (Given.Num() > 0)
	{
		Text += TEXT(" (+") + AmountsText(Data, Given) + TEXT(")");
	}
	if (Lost > 0)
	{
		Text += FString::Printf(TEXT(" – %d passten nicht mehr ins Inventar"), Lost);
	}
	Building->Destroy();
	Message(Text);
}
