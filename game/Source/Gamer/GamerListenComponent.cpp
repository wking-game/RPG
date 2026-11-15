#include "GamerListenComponent.h"

#include "Gamer.h"
#include "GamerCharacter.h"
#include "GamerGameState.h"
#include "GamerInventoryComponent.h"
#include "GamerPlayerState.h"
#include "GamerVillager.h"
#include "GamerWorkComponent.h"
#include "EngineUtils.h"
#include "Components/PrimitiveComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Dom/JsonObject.h"
#include "Engine/StaticMesh.h"
#include "Engine/StaticMeshActor.h"
#include "Engine/World.h"
#include "GameFramework/Pawn.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Net/UnrealNetwork.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"

namespace
{
	const FName MagicTag(TEXT("GamerMagic"));
	const FName HiddenTag(TEXT("GamerMagicHidden"));
	const FName RareTag(TEXT("GamerMagicRare"));
}

UGamerListenComponent::UGamerListenComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickInterval = 0.02f;
	SetIsReplicatedByDefault(true);
}

void UGamerListenComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION(UGamerListenComponent, EP, COND_OwnerOnly);
}

void UGamerListenComponent::WriteSaveState(FGamerSaveRecord& Out) const
{
	Out.SetNumber(TEXT("ep"), EP);
	Out.SetNames(TEXT("counted_targets"), CountedTargets);
}

void UGamerListenComponent::ReadSaveState(const FGamerSaveRecord& In)
{
	EP = FMath::Max(0.f, In.GetFloat(TEXT("ep"), 0.f));
	CountedTargets = In.GetNames(TEXT("counted_targets"));
}

// --- Data ------------------------------------------------------------------------------------

void UGamerListenComponent::LoadSettings()
{
	if (bLoaded)
	{
		return;
	}
	bLoaded = true;
	// Defaults (the table of the decision), overwritten by listen.json.
	Levels = {
		{ 1, TEXT("Gespür"), 0.f, 1000.f, 5.f, 15.f, 15.f, 0.30f, TEXT("flicker") },
		{ 2, TEXT("Ahnung"), 10.f, 1500.f, 7.5f, 20.f, 15.f, 0.45f, TEXT("direction_coarse") },
		{ 3, TEXT("Deutung"), 30.f, 2000.f, 10.f, 25.f, 15.f, 0.60f, TEXT("name_amount") },
		{ 4, TEXT("Klarblick"), 75.f, 2500.f, 12.5f, 30.f, 15.f, 0.80f, TEXT("hidden_mark") },
		{ 5, TEXT("Seidr-Blick"), 160.f, 3000.f, 15.f, 40.f, 10.f, 1.00f, TEXT("rare") } };
	StartLevelByOrigin.Add(TEXT("origin_seer"), 2);
	BonusItems.Add(TEXT("item_seaglass_amulet"), 1);
	DConditions = { TEXT("season:winter") };
	VillageSeerConditions = { TEXT("unlock:quest_village_seer"), TEXT("job:job_seer") };
	Sources = { { TEXT("new_find"), 2.f }, { TEXT("seaglass_harvest"), 1.f }, { TEXT("stone_circle"), 8.f }, { TEXT("tide_event"), 15.f } };
	TempoFactors = { { TEXT("slow"), 0.6f }, { TEXT("normal"), 1.f }, { TEXT("fast"), 1.6f } };

	const FString Path = FPaths::ProjectContentDir() / TEXT("Data/listen.json");
	FString Text;
	TSharedPtr<FJsonObject> Root;
	if (!FFileHelper::LoadFileToString(Text, *Path) || !FJsonSerializer::Deserialize(TJsonReaderFactory<>::Create(Text), Root) || !Root.IsValid())
	{
		UE_LOG(LogGamer, Warning, TEXT("Listen: %s not readable, using defaults"), *Path);
		return;
	}
	double Number = 0.0;
	if (Root->TryGetNumberField(TEXT("wave_speed_m_s"), Number)) { WaveSpeedMs = static_cast<float>(Number); }
	const TArray<TSharedPtr<FJsonValue>>* List = nullptr;
	if (Root->TryGetArrayField(TEXT("levels"), List))
	{
		Levels.Reset();
		for (const TSharedPtr<FJsonValue>& Value : *List)
		{
			const TSharedPtr<FJsonObject> Obj = Value->AsObject();
			if (!Obj.IsValid())
			{
				continue;
			}
			FGamerListenLevel Level;
			Level.Level = static_cast<int32>(Obj->GetNumberField(TEXT("level")));
			Level.Name = Obj->GetStringField(TEXT("name"));
			Level.MinEP = static_cast<float>(Obj->GetNumberField(TEXT("min_ep")));
			Level.RangeCm = static_cast<float>(Obj->GetNumberField(TEXT("range_m"))) * 100.f;
			Level.WaveSeconds = static_cast<float>(Obj->GetNumberField(TEXT("wave_s")));
			Level.VisibleSeconds = static_cast<float>(Obj->GetNumberField(TEXT("visible_s")));
			Level.CooldownSeconds = static_cast<float>(Obj->GetNumberField(TEXT("cooldown_s")));
			Level.Strength = static_cast<float>(Obj->GetNumberField(TEXT("strength")));
			Level.Shows = Obj->GetStringField(TEXT("shows"));
			Levels.Add(Level);
		}
	}
	const TSharedPtr<FJsonObject>* Object = nullptr;
	if (Root->TryGetObjectField(TEXT("start_level_by_origin"), Object))
	{
		StartLevelByOrigin.Reset();
		for (const TPair<FString, TSharedPtr<FJsonValue>>& Pair : (*Object)->Values)
		{
			StartLevelByOrigin.Add(FName(*Pair.Key), static_cast<int32>(Pair.Value->AsNumber()));
		}
	}
	if (Root->TryGetObjectField(TEXT("bonus_level"), Object))
	{
		const TSharedPtr<FJsonObject>* Items = nullptr;
		if ((*Object)->TryGetObjectField(TEXT("item"), Items))
		{
			BonusItems.Reset();
			for (const TPair<FString, TSharedPtr<FJsonValue>>& Pair : (*Items)->Values)
			{
				BonusItems.Add(FName(*Pair.Key), static_cast<int32>(Pair.Value->AsNumber()));
			}
		}
		const TArray<TSharedPtr<FJsonValue>>* DList = nullptr;
		if ((*Object)->TryGetArrayField(TEXT("d_conditions"), DList))
		{
			DConditions.Reset();
			for (const TSharedPtr<FJsonValue>& Value : *DList) { DConditions.Add(Value->AsString()); }
		}
		if ((*Object)->TryGetNumberField(TEXT("d_levels"), Number)) { DLevels = static_cast<int32>(Number); }
	}
	if (Root->TryGetObjectField(TEXT("village_seer"), Object))
	{
		if ((*Object)->TryGetNumberField(TEXT("ep_bonus"), Number)) { VillageSeerBonus = static_cast<float>(Number); }
		const TArray<TSharedPtr<FJsonValue>>* CList = nullptr;
		if ((*Object)->TryGetArrayField(TEXT("conditions"), CList))
		{
			VillageSeerConditions.Reset();
			for (const TSharedPtr<FJsonValue>& Value : *CList) { VillageSeerConditions.Add(Value->AsString()); }
		}
	}
	if (Root->TryGetObjectField(TEXT("ep_sources"), Object))
	{
		Sources.Reset();
		for (const TPair<FString, TSharedPtr<FJsonValue>>& Pair : (*Object)->Values) { Sources.Add(FName(*Pair.Key), static_cast<float>(Pair.Value->AsNumber())); }
	}
	if (Root->TryGetObjectField(TEXT("tempo"), Object))
	{
		TempoFactors.Reset();
		for (const TPair<FString, TSharedPtr<FJsonValue>>& Pair : (*Object)->Values) { TempoFactors.Add(FName(*Pair.Key), static_cast<float>(Pair.Value->AsNumber())); }
	}
	if (Root->TryGetObjectField(TEXT("coop"), Object))
	{
		if ((*Object)->TryGetNumberField(TEXT("find_share"), Number)) { FindShare = static_cast<float>(Number); }
		if ((*Object)->TryGetNumberField(TEXT("find_radius_m"), Number)) { FindRadiusCm = static_cast<float>(Number) * 100.f; }
		if ((*Object)->TryGetNumberField(TEXT("group_window_s"), Number)) { GroupWindowSeconds = static_cast<float>(Number); }
		if ((*Object)->TryGetNumberField(TEXT("group_distance_m"), Number)) { GroupDistanceCm = static_cast<float>(Number) * 100.f; }
		if ((*Object)->TryGetNumberField(TEXT("group_range_mul"), Number)) { GroupRangeMul = static_cast<float>(Number); }
		if ((*Object)->TryGetNumberField(TEXT("group_level_minus"), Number)) { GroupLevelMinus = static_cast<int32>(Number); }
	}
	if (Root->TryGetObjectField(TEXT("passive"), Object))
	{
		if ((*Object)->TryGetNumberField(TEXT("interval_s"), Number)) { PassiveInterval = static_cast<float>(Number); }
		if ((*Object)->TryGetNumberField(TEXT("max_edge_alpha"), Number)) { MaxEdgeAlpha = static_cast<float>(Number); }
	}
	UE_LOG(LogGamer, Log, TEXT("Listen: loaded %d levels, wave speed %.1f m/s"), Levels.Num(), WaveSpeedMs);
}

const FGamerListenLevel* UGamerListenComponent::GetLevelDef(int32 Level) const
{
	const_cast<UGamerListenComponent*>(this)->LoadSettings();
	return Levels.FindByPredicate([Level](const FGamerListenLevel& L) { return L.Level == Level; });
}

// --- Conditions, experience, level ------------------------------------------------------------

bool UGamerListenComponent::EvaluateCondition(const FString& Condition) const
{
	FString Kind, Value;
	if (!Condition.Split(TEXT(":"), &Kind, &Value))
	{
		return false;   // night, fog, ... need day/night and weather, which do not exist yet
	}
	const AActor* Owner = GetOwner();
	if (Kind == TEXT("origin") || Kind == TEXT("ability"))
	{
		const APawn* Pawn = Cast<APawn>(Owner);
		const AGamerPlayerState* PlayerState = Pawn ? Pawn->GetPlayerState<AGamerPlayerState>() : nullptr;
		if (!PlayerState)
		{
			return false;
		}
		return Kind == TEXT("origin") ? PlayerState->GetProfile().Origin == FName(*Value) : PlayerState->GetAbilities().Contains(FName(*Value));
	}
	if (Kind == TEXT("item"))
	{
		const UGamerInventoryComponent* Inventory = Owner ? Owner->FindComponentByClass<UGamerInventoryComponent>() : nullptr;
		return (Inventory && Inventory->CountOf(FName(*Value)) > 0) || (bDebugAmulet && Value == TEXT("item_seaglass_amulet"));
	}
	if (Kind == TEXT("job"))
	{
		for (TActorIterator<AGamerVillager> It(GetWorld()); It; ++It)
		{
			if (It->GetJobId() == FName(*Value))
			{
				return true;
			}
		}
		return false;
	}
	if (Kind == TEXT("season"))
	{
		const AGamerGameState* State = GetWorld() ? GetWorld()->GetGameState<AGamerGameState>() : nullptr;
		return State && AGamerGameState::DescribeSeason(State->GetSeasonPhase()).Contains(Value, ESearchCase::IgnoreCase);
	}
	// unlock:<quest> - quests do not exist yet, so nothing is unlocked and the village seer gives no bonus.
	return false;
}

void UGamerListenComponent::AddEP(float Amount)
{
	if (GetOwner() && GetOwner()->HasAuthority() && Amount > 0.f)
	{
		EP += Amount;
		UE_LOG(LogGamer, Log, TEXT("Listen: experience +%.2f = %.2f"), Amount, EP);
	}
}

float UGamerListenComponent::EPMultiplier() const
{
	const_cast<UGamerListenComponent*>(this)->LoadSettings();
	const EGamerGespuerTempo Tempo = AGamerGameState::Get(this).GespuerTempo;
	const FName Key = Tempo == EGamerGespuerTempo::Slow ? FName(TEXT("slow")) : Tempo == EGamerGespuerTempo::Fast ? FName(TEXT("fast")) : FName(TEXT("normal"));
	float Factor = TempoFactors.Contains(Key) ? TempoFactors[Key] : 1.f;
	bool bVillageSeer = VillageSeerConditions.Num() > 0;
	for (const FString& Condition : VillageSeerConditions)
	{
		bVillageSeer &= EvaluateCondition(Condition);
	}
	if (bVillageSeer)
	{
		Factor *= 1.f + VillageSeerBonus;
	}
	return Factor;
}

bool UGamerListenComponent::TryCountTarget(FName TargetId)
{
	if (TargetId.IsNone())
	{
		return true;
	}
	if (CountedTargets.Contains(TargetId))
	{
		return false;
	}
	CountedTargets.Add(TargetId);
	return true;
}

void UGamerListenComponent::AwardSource(FName Source, const FVector& Where, FName TargetId)
{
	LoadSettings();
	if (!GetOwner() || !GetOwner()->HasAuthority() || !Sources.Contains(Source))
	{
		return;
	}
	const float Base = Sources[Source];
	if (TryCountTarget(TargetId))
	{
		AddEP(Base * EPMultiplier());
	}
	else
	{
		UE_LOG(LogGamer, Log, TEXT("Listen: find %s was already counted, no experience"), *TargetId.ToString());
	}
	// Shared find: teammates close by get a part of it (with their own multipliers). One player: nobody else, nothing happens.
	for (TActorIterator<APawn> It(GetWorld()); It; ++It)
	{
		UGamerListenComponent* Other = It->FindComponentByClass<UGamerListenComponent>();
		if (Other && Other != this && FVector::Dist(It->GetActorLocation(), Where) <= FindRadiusCm && Other->TryCountTarget(TargetId))
		{
			Other->AddEP(Base * FindShare * Other->EPMultiplier());
		}
	}
}

void UGamerListenComponent::ServerDebugAward_Implementation(FName Source, FName TargetId)
{
	AwardSource(Source, GetOwner() ? GetOwner()->GetActorLocation() : FVector::ZeroVector, TargetId);
}

int32 UGamerListenComponent::GetLevel(FString* OutReason) const
{
	const_cast<UGamerListenComponent*>(this)->LoadSettings();
	int32 MaxLevel = 1;
	int32 Base = 1;
	for (const FGamerListenLevel& Level : Levels)
	{
		MaxLevel = FMath::Max(MaxLevel, Level.Level);
		if (EP >= Level.MinEP)
		{
			Base = FMath::Max(Base, Level.Level);
		}
	}
	int32 Result = Base;
	FString Reason = FString::Printf(TEXT("experience %.1f -> %d"), EP, Base);
	if (!bDebugNoOrigin)
	{
		const APawn* Pawn = Cast<APawn>(GetOwner());
		const AGamerPlayerState* PlayerState = Pawn ? Pawn->GetPlayerState<AGamerPlayerState>() : nullptr;
		if (PlayerState)
		{
			if (const int32* Start = StartLevelByOrigin.Find(PlayerState->GetProfile().Origin); Start && *Start > Result)
			{
				Result = *Start;
				Reason += FString::Printf(TEXT(", origin start %d"), *Start);
			}
		}
	}
	for (const TPair<FName, int32>& Bonus : BonusItems)
	{
		if (EvaluateCondition(TEXT("item:") + Bonus.Key.ToString()))
		{
			Result += Bonus.Value;
			Reason += FString::Printf(TEXT(", %s +%d"), *Bonus.Key.ToString(), Bonus.Value);
		}
	}
	for (const FString& Condition : DConditions)
	{
		if (EvaluateCondition(Condition))
		{
			Result += DLevels;
			Reason += FString::Printf(TEXT(", %s +%d"), *Condition, DLevels);
			break;
		}
	}
	Result = FMath::Min(Result, MaxLevel);
	if (OutReason)
	{
		*OutReason = Reason;
	}
	return Result;
}

int32 UGamerListenComponent::GetEffectiveLevel() const
{
	return IsShowing() ? FMath::Max(FrozenLevel, GroupLevel) : GetLevel();
}

float UGamerListenComponent::GetRangeCm() const
{
	const FGamerListenLevel* Def = GetLevelDef(GetEffectiveLevel());
	return (Def ? Def->RangeCm : 1000.f) * (IsShowing() ? RangeMul : 1.f);
}

float UGamerListenComponent::GetWaveSeconds() const
{
	const FGamerListenLevel* Def = GetLevelDef(GetEffectiveLevel());
	return (Def ? Def->WaveSeconds : 5.f) * RangeMul;
}

float UGamerListenComponent::GetVisibleSeconds() const
{
	const FGamerListenLevel* Def = GetLevelDef(GetEffectiveLevel());
	return (Def ? Def->VisibleSeconds : 15.f) * RangeMul;
}

bool UGamerListenComponent::CanMark() const
{
	return GetEffectiveLevel() >= 4;
}

// --- Active listening --------------------------------------------------------------------------

float UGamerListenComponent::GetAgeSeconds() const
{
	if (DebugAge >= 0.f)
	{
		return DebugAge;
	}
	const UWorld* World = GetWorld();
	return World ? static_cast<float>((bListening ? World->GetTimeSeconds() : EndTime) - StartTime) : 0.f;
}

bool UGamerListenComponent::IsListening() const
{
	return bListening;
}

bool UGamerListenComponent::IsShowing() const
{
	const UWorld* World = GetWorld();
	return DebugAge >= 0.f || bListening || (World && World->GetTimeSeconds() < ShowUntil);
}

float UGamerListenComponent::GetCooldownLeft() const
{
	const UWorld* World = GetWorld();
	return (World && !bListening) ? FMath::Max(0.f, static_cast<float>(CooldownEnd - World->GetTimeSeconds())) : 0.f;
}

float UGamerListenComponent::GetWaveRadiusCm() const
{
	const float Wave = FMath::Max(0.1f, GetWaveSeconds());
	return GetRangeCm() * FMath::Clamp(GetAgeSeconds() / Wave, 0.f, 1.f);
}

bool UGamerListenComponent::IsEligible(const AActor* Actor, int32 Level) const
{
	if (!Actor || !Actor->ActorHasTag(MagicTag))
	{
		return false;
	}
	if (Actor->ActorHasTag(HiddenTag) && Level < 4)
	{
		return false;
	}
	if (Actor->ActorHasTag(RareTag) && Level < 5)
	{
		return false;
	}
	return true;
}

void UGamerListenComponent::BuildFinds()
{
	TArray<FGamerListenFind> Old = Finds;
	Finds.Reset();
	const int32 Level = FMath::Max(FrozenLevel, GroupLevel);
	const float Range = GetRangeCm();
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}
	for (TActorIterator<AActor> It(World); It; ++It)
	{
		const float Distance = FVector::Dist(It->GetActorLocation(), Center);
		if (!IsEligible(*It, Level) || Distance > Range)
		{
			continue;
		}
		FGamerListenFind Find;
		Find.Actor = *It;
		Find.Location = It->GetActorLocation();
		Find.Distance = Distance;
		Find.bHidden = It->ActorHasTag(HiddenTag);
		Find.bRare = It->ActorHasTag(RareTag);
		for (const FName& Tag : It->Tags)
		{
			const FString Text = Tag.ToString();
			if (Text.StartsWith(TEXT("kind:"))) { Find.Kind = Text.Mid(5); }
			else if (Text.StartsWith(TEXT("coarse:"))) { Find.Coarse = Text.Mid(7); }
			else if (Text.StartsWith(TEXT("name:"))) { Find.Name = Text.Mid(5); }
			else if (Text.StartsWith(TEXT("amount:"))) { Find.Amount = FMath::Max(1, FCString::Atoi(*Text.Mid(7))); }
		}
		if (Find.Coarse.IsEmpty()) { Find.Coarse = Find.Kind; }
		for (const FGamerListenFind& Previous : Old)
		{
			if (Previous.Actor == Find.Actor)
			{
				Find.RevealTime = Previous.RevealTime;
			}
		}
		Finds.Add(Find);
	}
}

void UGamerListenComponent::UpdateFinds()
{
	const UWorld* World = GetWorld();
	if (!World || !IsShowing())
	{
		return;
	}
	const float Radius = GetWaveRadiusCm();
	for (FGamerListenFind& Find : Finds)
	{
		if (Find.RevealTime <= 0.0 && Radius >= Find.Distance)
		{
			Find.RevealTime = World->GetTimeSeconds();
		}
	}
}

bool UGamerListenComponent::TryListen()
{
	LoadSettings();
	AActor* Owner = GetOwner();
	UWorld* World = GetWorld();
	if (!Owner || !World || bListening)
	{
		return false;
	}
	UGamerWorkComponent* Work = Owner->FindComponentByClass<UGamerWorkComponent>();
	if (Owner->GetVelocity().Size2D() > 60.f)
	{
		if (Work) { Work->ShowMessage(TEXT("Zum Lauschen stehen bleiben.")); }
		return false;
	}
	if (const float Left = GetCooldownLeft(); Left > 0.f)
	{
		if (Work) { Work->ShowMessage(FString::Printf(TEXT("Lauschen wieder bereit in %.0f s"), FMath::CeilToFloat(Left))); }
		return false;
	}
	FrozenLevel = GetLevel();
	GroupLevel = 0;
	RangeMul = 1.f;
	StartTime = World->GetTimeSeconds();
	bListening = true;
	Center = Owner->GetActorLocation();
	BuildFinds();
	ShowUntil = StartTime + GetWaveSeconds() + GetVisibleSeconds();
	FString Reason;
	GetLevel(&Reason);
	UE_LOG(LogGamer, Log, TEXT("Listen: started, level %d (%s), range %.0f m, wave %.1f s, %d finds"), FrozenLevel, *Reason, GetRangeCm() / 100.f, GetWaveSeconds(), Finds.Num());
	if (Owner->HasAuthority())
	{
		ServerStartListen_Implementation();
	}
	else
	{
		ServerStartListen();
	}
	return true;
}

void UGamerListenComponent::EndListening()
{
	const UWorld* World = GetWorld();
	if (!World || !bListening)
	{
		return;
	}
	bListening = false;
	EndTime = World->GetTimeSeconds();
	const FGamerListenLevel* Def = GetLevelDef(FMath::Max(FrozenLevel, GroupLevel));
	CooldownEnd = EndTime + (Def ? Def->CooldownSeconds : 15.f);
	ShowUntil = EndTime + GetVisibleSeconds();
}

void UGamerListenComponent::ServerStartListen_Implementation()
{
	// Everybody who listened within the window and close by lifts the others: at least the best level minus 1, range +25 %.
	LoadSettings();
	UWorld* World = GetWorld();
	AActor* Owner = GetOwner();
	if (!World || !Owner)
	{
		return;
	}
	const double Now = World->GetTimeSeconds();
	ServerListenTime = Now;
	TArray<UGamerListenComponent*> Group;
	int32 Best = 0;
	for (TActorIterator<APawn> It(World); It; ++It)
	{
		UGamerListenComponent* Other = It->FindComponentByClass<UGamerListenComponent>();
		if (Other && (Other == this || (Now - Other->ServerListenTime <= GroupWindowSeconds && FVector::Dist(It->GetActorLocation(), Owner->GetActorLocation()) <= GroupDistanceCm)))
		{
			Group.Add(Other);
			Best = FMath::Max(Best, Other->GetLevel());
		}
	}
	if (Group.Num() >= 2)
	{
		for (UGamerListenComponent* Member : Group)
		{
			Member->ClientListenResult(FMath::Max(Member->GetLevel(), Best - GroupLevelMinus), GroupRangeMul);
		}
		UE_LOG(LogGamer, Log, TEXT("Listen: group of %d, best level %d, everybody at least %d, range x%.2f"), Group.Num(), Best, Best - GroupLevelMinus, GroupRangeMul);
	}
	else
	{
		ClientListenResult(GetLevel(), 1.f);
	}
}

void UGamerListenComponent::ClientListenResult_Implementation(int32 EffectiveLevel, float InRangeMul)
{
	if (!IsShowing())
	{
		return;
	}
	GroupLevel = EffectiveLevel;
	RangeMul = InRangeMul;
	BuildFinds();
	ShowUntil = FMath::Max(ShowUntil, StartTime + GetWaveSeconds() + GetVisibleSeconds());
	UE_LOG(LogGamer, Log, TEXT("Listen: effective level %d, range x%.2f -> %.0f m, %d finds"), GetEffectiveLevel(), InRangeMul, GetRangeCm() / 100.f, Finds.Num());
}

// --- Passive signal and the silhouettes --------------------------------------------------------------

void UGamerListenComponent::UpdatePassive()
{
	const UWorld* World = GetWorld();
	if (!World || World->GetTimeSeconds() < NextPassive)
	{
		return;
	}
	NextPassive = World->GetTimeSeconds() + PassiveInterval;
	const int32 Level = GetEffectiveLevel();
	const float Range = GetRangeCm();
	float Nearest = TNumericLimits<float>::Max();
	for (TActorIterator<AActor> It(World); It; ++It)
	{
		if (IsEligible(*It, Level))
		{
			Nearest = FMath::Min(Nearest, static_cast<float>(FVector::Dist(It->GetActorLocation(), GetOwner()->GetActorLocation())));
		}
	}
	Proximity = Nearest <= Range ? FMath::Pow(1.f - Nearest / FMath::Max(1.f, Range), 1.5f) : 0.f;
	if (Proximity > 0.f)
	{
		OnMagicHum.Broadcast(Proximity);   // audio hook: the hum gets louder the nearer
	}
}

void UGamerListenComponent::SetMagicStencil(AActor* Actor, bool bOn)
{
	if (!Actor)
	{
		return;
	}
	TArray<UPrimitiveComponent*> Primitives;
	Actor->GetComponents<UPrimitiveComponent>(Primitives);
	for (UPrimitiveComponent* Primitive : Primitives)
	{
		if (bOn)
		{
			Primitive->SetCustomDepthStencilValue(2);
		}
		Primitive->SetRenderCustomDepth(bOn);
	}
}

void UGamerListenComponent::ClearOutline()
{
	for (const TWeakObjectPtr<AActor>& Actor : Outlined)
	{
		SetMagicStencil(Actor.Get(), false);
	}
	Outlined.Reset();
	if (const AGamerCharacter* Character = Cast<AGamerCharacter>(GetOwner()))
	{
		if (UMaterialInstanceDynamic* Material = Character->GetOutlineMaterial())
		{
			Material->SetScalarParameterValue(TEXT("MagicOpacity"), 0.f);
		}
	}
}

void UGamerListenComponent::UpdateOutline()
{
	const AGamerCharacter* Character = Cast<AGamerCharacter>(GetOwner());
	UMaterialInstanceDynamic* Material = Character ? Character->GetOutlineMaterial() : nullptr;
	const UWorld* World = GetWorld();
	if (!Material || !World)
	{
		return;
	}
	if (!IsShowing())
	{
		if (Outlined.Num() > 0)
		{
			ClearOutline();
		}
		return;
	}
	// The revealed finds are outlined (custom stencil 2) while they are visible.
	const double Now = World->GetTimeSeconds();
	const float Visible = GetVisibleSeconds();
	TArray<TWeakObjectPtr<AActor>> Wanted;
	double LatestEnd = 0.0;
	for (const FGamerListenFind& Find : Finds)
	{
		if (Find.RevealTime > 0.0 && (DebugAge >= 0.f || Now < Find.RevealTime + Visible))
		{
			Wanted.Add(Find.Actor);
			LatestEnd = FMath::Max(LatestEnd, Find.RevealTime + Visible);
		}
	}
	for (const TWeakObjectPtr<AActor>& Old : Outlined)
	{
		if (!Wanted.Contains(Old))
		{
			SetMagicStencil(Old.Get(), false);
		}
	}
	Outlined = Wanted;
	for (const TWeakObjectPtr<AActor>& Actor : Outlined)
	{
		SetMagicStencil(Actor.Get(), true);
	}
	const FGamerListenLevel* Def = GetLevelDef(GetEffectiveLevel());
	const float Strength = Def ? Def->Strength : 0.3f;
	const float Ramp = FMath::Clamp(GetAgeSeconds() / 0.5f, 0.f, 1.f);
	const float FadeOut = DebugAge >= 0.f ? 1.f : FMath::Clamp(static_cast<float>(LatestEnd - Now) / 1.5f, 0.f, 1.f);
	const float Flicker = GetEffectiveLevel() <= 1 ? 0.6f + 0.4f * FMath::Sin(static_cast<float>(Now) * 11.f) : 1.f;
	Material->SetScalarParameterValue(TEXT("MagicOpacity"), Outlined.Num() > 0 ? Strength * 0.85f * Ramp * FadeOut * Flicker : 0.f);
	Material->SetScalarParameterValue(TEXT("MagicThickness"), FMath::Lerp(1.2f, 3.2f, Strength));
}

void UGamerListenComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	const APawn* Pawn = Cast<APawn>(GetOwner());
	if (!Pawn || !Pawn->IsLocallyControlled())
	{
		return;
	}
	LoadSettings();
	if (bListening)
	{
		if (GetOwner()->GetVelocity().Size2D() > 60.f)
		{
			UE_LOG(LogGamer, Log, TEXT("Listen: broken off by moving"));
			EndListening();   // moving ends the wave
		}
		else if (GetAgeSeconds() >= GetWaveSeconds())
		{
			EndListening();
		}
	}
	UpdateFinds();
	UpdateOutline();
	UpdatePassive();
}

// --- Tests -----------------------------------------------------------------------------------

void UGamerListenComponent::AddDebugTarget(const FVector& Location, const FString& Kind, const FString& Name, bool bHidden, bool bRare)
{
	UWorld* World = GetWorld();
	AStaticMeshActor* Target = World ? World->SpawnActor<AStaticMeshActor>(Location, FRotator::ZeroRotator) : nullptr;
	if (!Target)
	{
		return;
	}
	Target->SetMobility(EComponentMobility::Movable);
	if (UStaticMesh* Sphere = LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Sphere.Sphere")))
	{
		Target->GetStaticMeshComponent()->SetStaticMesh(Sphere);
	}
	Target->GetStaticMeshComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Target->SetActorScale3D(FVector(0.5f, 0.5f, 1.1f));
	Target->Tags.Add(MagicTag);
	if (bHidden) { Target->Tags.Add(HiddenTag); }
	if (bRare) { Target->Tags.Add(RareTag); }
	Target->Tags.Add(*(TEXT("kind:") + Kind));
	Target->Tags.Add(*(TEXT("coarse:") + Kind));
	Target->Tags.Add(*(TEXT("name:") + Name));
	Target->Tags.Add(TEXT("amount:3"));
	DebugActors.Add(Target);
}

FString UGamerListenComponent::DescribeState() const
{
	FString Reason;
	const int32 Level = GetLevel(&Reason);
	const FGamerListenLevel* Def = GetLevelDef(Level);
	return FString::Printf(TEXT("level %d (%s) ep %.2f range %.0f m wave %.1f s visible %.0f s [%s]"), Level, *Reason, EP,
		Def ? Def->RangeCm / 100.f : 0.f, Def ? Def->WaveSeconds : 0.f, Def ? Def->VisibleSeconds : 0.f, Def ? *Def->Shows : TEXT("-"));
}
