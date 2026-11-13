#include "GamerWorkComponent.h"

#include "Gamer.h"
#include "GamerDataSubsystem.h"
#include "GamerInventoryComponent.h"
#include "GamerResourceNode.h"
#include "Engine/World.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerState.h"
#include "Net/UnrealNetwork.h"
#include "TimerManager.h"

namespace
{
	FString ToolName(FName Type)
	{
		if (Type == TEXT("axe")) { return TEXT("eine Axt"); }
		if (Type == TEXT("pickaxe")) { return TEXT("eine Spitzhacke"); }
		if (Type == TEXT("hammer")) { return TEXT("einen Hammer"); }
		return Type.ToString();
	}

	int32 PlayerIdOf(const AActor* Owner)
	{
		const APawn* Pawn = Cast<APawn>(Owner);
		const APlayerState* PS = Pawn ? Pawn->GetPlayerState() : nullptr;
		return PS ? PS->GetPlayerId() : -1;
	}
}

UGamerWorkComponent::UGamerWorkComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
}

void UGamerWorkComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UGamerWorkComponent, Work);
}

double UGamerWorkComponent::ServerNow() const
{
	const UWorld* World = GetWorld();
	const AGameStateBase* GameState = World ? World->GetGameState() : nullptr;
	return GameState ? GameState->GetServerWorldTimeSeconds() : (World ? World->GetTimeSeconds() : 0.0);
}

float UGamerWorkComponent::GetProgress() const
{
	if (!IsBusy() || Work.Duration <= 0.f)
	{
		return 0.f;
	}
	return FMath::Clamp(static_cast<float>((ServerNow() - Work.StartTime) / Work.Duration), 0.f, 1.f);
}

UGamerInventoryComponent* UGamerWorkComponent::GetInventory() const
{
	return GetOwner() ? GetOwner()->FindComponentByClass<UGamerInventoryComponent>() : nullptr;
}

bool UGamerWorkComponent::IsInReach(const AActor* Target) const
{
	return Target && GetOwner() && FVector::Dist2D(Target->GetActorLocation(), GetOwner()->GetActorLocation()) <= MaxReach;
}

void UGamerWorkComponent::Fail(const FString& Message)
{
	UE_LOG(LogGamer, Log, TEXT("Work refused for player %d: %s"), PlayerIdOf(GetOwner()), *Message);
	ClientMessage(Message);
}

void UGamerWorkComponent::ClientMessage_Implementation(const FString& Message)
{
	LastMessage = Message;
	LastMessageTime = GetWorld() ? GetWorld()->GetTimeSeconds() : 0.0;
	UE_LOG(LogGamer, Log, TEXT("Message: %s"), *Message);
}

void UGamerWorkComponent::StartWork(EGamerWorkKind Kind, const FString& Label, float Duration)
{
	Work.Kind = Kind;
	Work.Label = Label;
	Work.StartTime = ServerNow();
	Work.Duration = FMath::Max(0.1f, Duration);
	GetWorld()->GetTimerManager().SetTimer(WorkTimer, this, &UGamerWorkComponent::FinishWork, Work.Duration, false);
	GetOwner()->ForceNetUpdate();
}

void UGamerWorkComponent::ServerHarvest_Implementation(AGamerResourceNode* Node)
{
	if (IsBusy())
	{
		return Fail(TEXT("Du arbeitest schon an etwas."));
	}
	const FGamerResourceNodeDef* Def = Node ? Node->GetDef() : nullptr;
	if (!Def)
	{
		return Fail(TEXT("Das kann man nicht sammeln."));
	}
	if (Node->IsDepleted())
	{
		return Fail(FString::Printf(TEXT("%s ist schon abgeerntet."), *Def->Name));
	}
	if (!IsInReach(Node))
	{
		return Fail(TEXT("Zu weit weg."));
	}
	if (Node->Worker.IsValid() && Node->Worker.Get() != GetOwner())
	{
		return Fail(TEXT("Daran arbeitet schon jemand."));
	}

	float Speed = 1.f;
	if (Def->RequiredTool.IsRequired())
	{
		const UGamerInventoryComponent* Inventory = GetInventory();
		const FGamerItemDef* Tool = Inventory ? Inventory->FindBestTool(Def->RequiredTool.Type, Def->RequiredTool.MinTier) : nullptr;
		if (!Tool)
		{
			return Fail(FString::Printf(TEXT("Für %s brauchst du %s (ab Stufe %d)."), *Def->Name, *ToolName(Def->RequiredTool.Type), Def->RequiredTool.MinTier));
		}
		Speed = FMath::Max(0.1f, Tool->Tool.WorkSpeed);
	}

	TargetNode = Node;
	Node->Worker = GetOwner();
	StartWork(EGamerWorkKind::Harvest, Def->Name, Def->WorkSeconds / Speed);
}

void UGamerWorkComponent::ServerCraft_Implementation(FName RecipeId)
{
	if (IsBusy())
	{
		return Fail(TEXT("Du arbeitest schon an etwas."));
	}
	const UGamerDataSubsystem* Data = UGamerDataSubsystem::Get(this);
	const FGamerRecipeDef* Recipe = Data ? Data->FindRecipe(RecipeId) : nullptr;
	UGamerInventoryComponent* Inventory = GetInventory();
	if (!Recipe || !Inventory || Recipe->Outputs.Num() == 0)
	{
		return Fail(TEXT("Dieses Rezept gibt es nicht."));
	}
	if (!Recipe->Station.IsNone())
	{
		const FGamerBuildableDef* Station = Data->FindBuildable(Recipe->Station);
		return Fail(FString::Printf(TEXT("Dafür brauchst du: %s."), Station ? *Station->Name : *Recipe->Station.ToString()));
	}
	if (!Inventory->HasAll(Recipe->Inputs))
	{
		TArray<FString> Missing;
		for (const FGamerItemAmount& Input : Recipe->Inputs)
		{
			const int32 Have = Inventory->CountOf(Input.Item);
			if (Have < Input.Count)
			{
				const FGamerItemDef* Item = Data->FindItem(Input.Item);
				Missing.Add(FString::Printf(TEXT("%d %s"), Input.Count - Have, Item ? *Item->Name : *Input.Item.ToString()));
			}
		}
		return Fail(TEXT("Es fehlt: ") + FString::Join(Missing, TEXT(", ")));
	}
	for (const FGamerItemAmount& Output : Recipe->Outputs)
	{
		if (Inventory->SpaceFor(Output.Item) < Output.Count)
		{
			return Fail(TEXT("Kein Platz im Inventar."));
		}
	}

	const FGamerItemDef* First = Data->FindItem(Recipe->Outputs[0].Item);
	TargetRecipe = RecipeId;
	StartWork(EGamerWorkKind::Craft, First ? First->Name : RecipeId.ToString(), Recipe->WorkSeconds);
}

void UGamerWorkComponent::FinishWork()
{
	const EGamerWorkKind Kind = Work.Kind;
	Work = FGamerWorkState();
	GetOwner()->ForceNetUpdate();

	UGamerInventoryComponent* Inventory = GetInventory();
	const UGamerDataSubsystem* Data = UGamerDataSubsystem::Get(this);
	if (!Inventory || !Data)
	{
		return;
	}

	if (Kind == EGamerWorkKind::Harvest)
	{
		AGamerResourceNode* Node = TargetNode.Get();
		TargetNode.Reset();
		const FGamerResourceNodeDef* Def = Node ? Node->GetDef() : nullptr;
		if (!Def || Node->IsDepleted())
		{
			return Fail(TEXT("Schon abgeerntet."));
		}
		if (!IsInReach(Node))
		{
			Node->Worker.Reset();
			return Fail(TEXT("Abgebrochen: zu weit weggegangen."));
		}

		TArray<FString> Gained;
		for (const FGamerDrop& Drop : Def->Drops)
		{
			const int32 Count = FMath::RandRange(Drop.Min, Drop.Max);
			if (Count <= 0)
			{
				continue;
			}
			const int32 Lost = Inventory->AddItem(Drop.Item, Count);
			const FGamerItemDef* Item = Data->FindItem(Drop.Item);
			Gained.Add(FString::Printf(TEXT("+%d %s"), Count - Lost, Item ? *Item->Name : *Drop.Item.ToString()));
			if (Lost > 0)
			{
				Gained.Add(FString::Printf(TEXT("(%d passten nicht mehr ins Inventar)"), Lost));
			}
		}
		Node->Deplete();
		UE_LOG(LogGamer, Log, TEXT("Harvest done: player=%d node=%s gained [%s] inventory: %s"),
			PlayerIdOf(GetOwner()), *Def->Id.ToString(), *FString::Join(Gained, TEXT(" ")), *Inventory->Describe());
		ClientMessage(FString::Join(Gained, TEXT("  ")));
	}
	else if (Kind == EGamerWorkKind::Craft)
	{
		const FGamerRecipeDef* Recipe = Data->FindRecipe(TargetRecipe);
		TargetRecipe = NAME_None;
		if (!Recipe || !Inventory->RemoveItems(Recipe->Inputs))
		{
			return Fail(TEXT("Herstellen fehlgeschlagen: Zutaten fehlen."));
		}
		TArray<FString> Made;
		for (const FGamerItemAmount& Output : Recipe->Outputs)
		{
			Inventory->AddItem(Output.Item, Output.Count);
			const FGamerItemDef* Item = Data->FindItem(Output.Item);
			Made.Add(FString::Printf(TEXT("+%d %s"), Output.Count, Item ? *Item->Name : *Output.Item.ToString()));
		}
		UE_LOG(LogGamer, Log, TEXT("Craft done: player=%d recipe=%s made [%s] inventory: %s"),
			PlayerIdOf(GetOwner()), *Recipe->Id.ToString(), *FString::Join(Made, TEXT(" ")), *Inventory->Describe());
		ClientMessage(TEXT("Hergestellt: ") + FString::Join(Made, TEXT(", ")));
	}
}

void UGamerWorkComponent::ServerDebugTeleportNear_Implementation(AGamerResourceNode* Node)
{
#if UE_BUILD_SHIPPING
	return;
#else
	APawn* Pawn = Cast<APawn>(GetOwner());
	if (!Pawn || !Node)
	{
		return;
	}
	// Stand 220 cm from the node centre (clear of large rocks), on the side facing the pawn, at the pawn's height.
	FVector Direction = Pawn->GetActorLocation() - Node->GetActorLocation();
	Direction.Z = 0.f;
	Direction = Direction.IsNearlyZero() ? FVector::ForwardVector : Direction.GetSafeNormal();
	FVector Target = Node->GetActorLocation() + Direction * 220.f;
	Target.Z = Pawn->GetActorLocation().Z;
	Pawn->TeleportTo(Target, (-Direction).Rotation(), false, true);
#endif
}
