#include "GamerHealthComponent.h"

#include "Gamer.h"
#include "GamerBuffComponent.h"
#include "GamerBuilding.h"
#include "GamerCharacter.h"
#include "GamerDataSubsystem.h"
#include "GamerEquipmentComponent.h"
#include "GamerEquipmentMath.h"
#include "GamerGameState.h"
#include "GamerGrave.h"
#include "GameFramework/GameStateBase.h"
#include "GamerInventoryComponent.h"
#include "GamerPlayerState.h"
#include "GamerSaveSubsystem.h"
#include "GamerWorkComponent.h"
#include "EngineUtils.h"
#include "Components/CapsuleComponent.h"
#include "Dom/JsonObject.h"
#include "Engine/World.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/PlayerState.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Net/UnrealNetwork.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"

UGamerHealthComponent::UGamerHealthComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickInterval = 0.25f;
	SetIsReplicatedByDefault(true);
}

void UGamerHealthComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION(UGamerHealthComponent, HP, COND_OwnerOnly);
	DOREPLIFETIME(UGamerHealthComponent, bDead);
	DOREPLIFETIME(UGamerHealthComponent, bDowned);
	DOREPLIFETIME(UGamerHealthComponent, DownedLeft);
	DOREPLIFETIME_CONDITION(UGamerHealthComponent, ReviveProgress, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(UGamerHealthComponent, DeadSeconds, COND_OwnerOnly);
}

void UGamerHealthComponent::LoadSettings()
{
	if (bLoaded)
	{
		return;
	}
	bLoaded = true;
	const FString Path = FPaths::ProjectContentDir() / TEXT("Data/health.json");
	FString Text;
	TSharedPtr<FJsonObject> Root;
	if (!FFileHelper::LoadFileToString(Text, *Path) || !FJsonSerializer::Deserialize(TJsonReaderFactory<>::Create(Text), Root) || !Root.IsValid())
	{
		UE_LOG(LogGamer, Warning, TEXT("Health: %s not readable, using defaults"), *Path);
		return;
	}
	double Number = 0.0;
	if (Root->TryGetNumberField(TEXT("base_hp"), Number)) { BaseHP = static_cast<float>(Number); }
	const TSharedPtr<FJsonObject>* Object = nullptr;
	if (Root->TryGetObjectField(TEXT("regen"), Object))
	{
		if ((*Object)->TryGetNumberField(TEXT("base_per_s"), Number)) { RegenPerSecond = static_cast<float>(Number); }
		bool bNeeds = true;
		if ((*Object)->TryGetBoolField(TEXT("needs_fed"), bNeeds)) { bRegenNeedsFed = bNeeds; }
	}
	if (Root->TryGetObjectField(TEXT("fall"), Object))
	{
		if ((*Object)->TryGetNumberField(TEXT("min_height_cm"), Number)) { FallMinHeightCm = static_cast<float>(Number); }
		if ((*Object)->TryGetNumberField(TEXT("damage_per_100cm"), Number)) { FallDamagePer100 = static_cast<float>(Number); }
	}
	if (Root->TryGetObjectField(TEXT("respawn"), Object))
	{
		if ((*Object)->TryGetNumberField(TEXT("min_delay_s"), Number)) { MinRespawnDelay = static_cast<float>(Number); }
		if ((*Object)->TryGetNumberField(TEXT("heal_fraction"), Number)) { RespawnHealFraction = FMath::Clamp(static_cast<float>(Number), 0.1f, 1.f); }
	}
	if (Root->TryGetObjectField(TEXT("grave"), Object))
	{
		if ((*Object)->TryGetNumberField(TEXT("others_after_s"), Number)) { GraveOthersAfter = static_cast<float>(Number); }
		if ((*Object)->TryGetNumberField(TEXT("reach_cm"), Number)) { GraveReachCm = static_cast<float>(Number); }
	}
	if (Root->TryGetObjectField(TEXT("spawn"), Object))
	{
		FString Bed;
		if ((*Object)->TryGetStringField(TEXT("bed_building"), Bed) && !Bed.IsEmpty()) { BedBuilding = FName(*Bed); }
		if ((*Object)->TryGetNumberField(TEXT("bed_check_cm"), Number)) { BedCheckCm = static_cast<float>(Number); }
	}
	if (Root->TryGetNumberField(TEXT("game_day_s"), Number)) { GameDaySeconds = FMath::Max(60.f, static_cast<float>(Number)); }
	if (Root->TryGetObjectField(TEXT("downed"), Object))
	{
		bool bFlag = true;
		if ((*Object)->TryGetBoolField(TEXT("enabled"), bFlag)) { bDownedEnabled = bFlag; }
		if ((*Object)->TryGetNumberField(TEXT("duration_s"), Number)) { DownedDuration = static_cast<float>(Number); }
		if ((*Object)->TryGetNumberField(TEXT("revive_hold_s"), Number)) { ReviveHoldSeconds = FMath::Max(0.1f, static_cast<float>(Number)); }
		if ((*Object)->TryGetNumberField(TEXT("revive_hp_fraction"), Number)) { ReviveHpFraction = FMath::Clamp(static_cast<float>(Number), 0.05f, 1.f); }
		if ((*Object)->TryGetNumberField(TEXT("reach_cm"), Number)) { ReviveReachCm = static_cast<float>(Number); }
	}
	if (Root->TryGetObjectField(TEXT("last_breath"), Object))
	{
		const TArray<TSharedPtr<FJsonValue>>* Items = nullptr;
		if ((*Object)->TryGetArrayField(TEXT("items"), Items))
		{
			LastBreathItems.Reset();
			for (const TSharedPtr<FJsonValue>& Value : *Items) { LastBreathItems.Add(FName(*Value->AsString())); }
		}
		bool bFlag = true;
		if ((*Object)->TryGetBoolField(TEXT("hotbar_only"), bFlag)) { bLastBreathHotbarOnly = bFlag; }
		if ((*Object)->TryGetNumberField(TEXT("hp_fraction"), Number)) { LastBreathHpFraction = FMath::Clamp(static_cast<float>(Number), 0.05f, 1.f); }
	}
	if (Root->TryGetObjectField(TEXT("healing_hands"), Object))
	{
		FString Ability;
		if ((*Object)->TryGetStringField(TEXT("ability"), Ability) && !Ability.IsEmpty()) { HealingHandsAbility = FName(*Ability); }
	}
	UE_LOG(LogGamer, Log, TEXT("Health: base %.0f, regen %.2f/s%s, fall from %.0f cm (%.0f per m), grave protection %.0f s"), BaseHP, RegenPerSecond,
		bRegenNeedsFed ? TEXT(" while fed") : TEXT(""), FallMinHeightCm, FallDamagePer100, GraveOthersAfter);
}

void UGamerHealthComponent::BeginPlay()
{
	Super::BeginPlay();
	LoadSettings();
	if (GetOwner() && GetOwner()->HasAuthority())
	{
		HP = GetMaxHP();
		StartPoint = GetOwner()->GetActorLocation();   // a saved start point (ReadSaveState) replaces this
		bHasStartPoint = true;
	}
}

float UGamerHealthComponent::GetMaxHP() const
{
	const_cast<UGamerHealthComponent*>(this)->LoadSettings();
	const UGamerBuffComponent* Buffs = GetOwner() ? GetOwner()->FindComponentByClass<UGamerBuffComponent>() : nullptr;
	const float MealBonus = Buffs ? Buffs->GetBonus(EGamerBuffStat::Health) : 0.f;
	// Hit points = 100 + 5 x (toughness - 5) + meal (docs/21 section 4); health.json "base" stays the fallback without combat data.
	const UGamerDataSubsystem* Data = UGamerDataSubsystem::Get(this);
	if (Data && Data->GetCombat().bLoaded)
	{
		const APawn* Pawn = Cast<APawn>(GetOwner());
		const AGamerPlayerState* PlayerState = Pawn ? Pawn->GetPlayerState<AGamerPlayerState>() : nullptr;
		const int32 Toughness = (PlayerState && PlayerState->HasCharacter()) ? PlayerState->GetStat(TEXT("stat_toughness")) : Data->GetCombat().ToughnessNeutral;
		return GamerCombat::PlayerMaxHp(Toughness, MealBonus, Data->GetCombat());
	}
	return BaseHP + MealBonus;
}

bool UGamerHealthComponent::IsFed() const
{
	const UGamerBuffComponent* Buffs = GetOwner() ? GetOwner()->FindComponentByClass<UGamerBuffComponent>() : nullptr;
	return Buffs && Buffs->GetActiveMeals().Num() > 0;
}

int32 UGamerHealthComponent::DayNow() const
{
	const_cast<UGamerHealthComponent*>(this)->LoadSettings();
	const AGameStateBase* State = GetWorld() ? GetWorld()->GetGameState() : nullptr;
	const double Now = State ? State->GetServerWorldTimeSeconds() : (GetWorld() ? GetWorld()->GetTimeSeconds() : 0.0);
	return FMath::FloorToInt(static_cast<float>(Now / GameDaySeconds));
}

bool UGamerHealthComponent::HasHealingHands() const
{
	const APawn* Pawn = Cast<APawn>(GetOwner());
	const AGamerPlayerState* PlayerState = Pawn ? Pawn->GetPlayerState<AGamerPlayerState>() : nullptr;
	return bDebugHealingHands || (PlayerState && PlayerState->GetAbilities().Contains(HealingHandsAbility));
}

bool UGamerHealthComponent::AnyOtherAlivePlayer() const
{
	for (TActorIterator<APawn> It(GetWorld()); It; ++It)
	{
		const UGamerHealthComponent* Other = It->FindComponentByClass<UGamerHealthComponent>();
		if (Other && Other != this && !Other->IsIncapacitated())
		{
			return true;
		}
	}
	return false;
}

float UGamerHealthComponent::GetRespawnWait() const
{
	return FMath::Max(0.f, MinRespawnDelay - DeadSeconds);
}

void UGamerHealthComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (!GetOwner() || !GetOwner()->HasAuthority())
	{
		return;
	}
	LoadSettings();
	if (!bHpInitialized)
	{
		const APawn* Pawn = Cast<APawn>(GetOwner());
		const AGamerPlayerState* PlayerState = Pawn ? Pawn->GetPlayerState<AGamerPlayerState>() : nullptr;
		if (PlayerState && PlayerState->HasCharacter())
		{
			HP = GetMaxHP();
			bHpInitialized = true;
		}
	}
	if (bDead)
	{
		DeadSeconds += DeltaTime;
		return;
	}
	if (bDowned)
	{
		// Bleeding out: the revive window runs; if nobody is left who could help, it is over at once.
		DownedLeft -= DeltaTime;
		if (DownedLeft <= 0.f || !AnyOtherAlivePlayer())
		{
			FGamerDamageInfo Info;
			Info.Type = TEXT("bleed_out");
			bDowned = false;
			Die(Info);
		}
		return;
	}
	// Reviving a downed teammate: hold E for ReviveHoldSeconds (healing hands: at once, once a day).
	if (AActor* TargetActor = ReviveTarget.Get())
	{
		UGamerHealthComponent* Target = TargetActor->FindComponentByClass<UGamerHealthComponent>();
		if (!Target || !Target->bDowned || FVector::Dist(TargetActor->GetActorLocation(), GetOwner()->GetActorLocation()) > ReviveReachCm * 1.3f)
		{
			ReviveTarget.Reset();
			ReviveProgress = 0.f;
		}
		else
		{
			bool bHands = false;
			if (HasHealingHands() && HealingHandsDay != DayNow())
			{
				bHands = true;
			}
			ReviveProgress = bHands ? 1.f : FMath::Min(1.f, ReviveProgress + DeltaTime / ReviveHoldSeconds);
			if (ReviveProgress >= 1.f)
			{
				if (bHands)
				{
					HealingHandsDay = DayNow();
				}
				Target->Revive(ReviveHpFraction, bHands ? TEXT("healing hands") : TEXT("a teammate"));
				ReviveTarget.Reset();
				ReviveProgress = 0.f;
			}
		}
	}
	const float Max = GetMaxHP();
	HP = FMath::Min(HP, Max);   // a meal's health buff ended
	if (HP < Max && (!bRegenNeedsFed || IsFed()))
	{
		// Slow healing while fed; the "Regeneration" buff of the meals adds to it. Hungry = standing still, no penalty (rule 5).
		const UGamerBuffComponent* Buffs = GetOwner()->FindComponentByClass<UGamerBuffComponent>();
		const float Rate = RegenPerSecond + (Buffs ? Buffs->GetBonus(EGamerBuffStat::Regen) : 0.f);
		HP = FMath::Min(Max, HP + Rate * DeltaTime);
	}
}

// --- Damage ------------------------------------------------------------------------------------

bool UGamerHealthComponent::TryLastBreath()
{
	// "Letzter Atem": a remedy in the hotbar is used up and the player stays on its feet with a quarter of the hit points; once a day.
	LoadSettings();
	if (LastBreathDay == DayNow())
	{
		UE_LOG(LogGamer, Log, TEXT("Health: last breath already used today (day %d)"), LastBreathDay);
		return false;
	}
	UGamerInventoryComponent* Inventory = GetOwner()->FindComponentByClass<UGamerInventoryComponent>();
	if (!Inventory)
	{
		return false;
	}
	const TArray<FGamerInventorySlot>& Slots = Inventory->GetSlots();
	const int32 Limit = bLastBreathHotbarOnly ? Inventory->GetHotbarSize() : Slots.Num();
	for (int32 Index = 0; Index < Limit && Index < Slots.Num(); ++Index)
	{
		if (!Slots[Index].IsEmpty() && LastBreathItems.Contains(Slots[Index].Item))
		{
			FGamerItemAmount Use;
			Use.Item = Slots[Index].Item;
			Use.Count = 1;
			if (Inventory->RemoveItems({ Use }))
			{
				LastBreathDay = DayNow();
				HP = FMath::Max(1.f, GetMaxHP() * LastBreathHpFraction);
				UE_LOG(LogGamer, Log, TEXT("Health: last breath - %s used up, %.0f hit points left"), *Use.Item.ToString(), HP);
				if (UGamerWorkComponent* Work = GetOwner()->FindComponentByClass<UGamerWorkComponent>())
				{
					Work->ClientMessage(TEXT("Letzter Atem: der Verband rettet dich."));
				}
				return true;
			}
		}
	}
	return false;
}

float UGamerHealthComponent::ReduceDamage(const FGamerDamageInfo& Info) const
{
	// Worn armor and resistances (docs/21 5.1/5.2). Without zone and kind nothing changes: falls, old damage types stay as they were.
	const UGamerDataSubsystem* Data = UGamerDataSubsystem::Get(this);
	const UGamerEquipmentComponent* Equipment = GetOwner() ? GetOwner()->FindComponentByClass<UGamerEquipmentComponent>() : nullptr;
	if (!Data || !Data->GetCombat().bLoaded || !Equipment)
	{
		return Info.Amount;
	}
	const FGamerCombatData& Combat = Data->GetCombat();
	float Amount = Info.Amount;
	if (Info.Zone >= 0 && Info.Zone <= 2)
	{
		Amount = GamerEquipment::HitDamage(Amount, static_cast<EGamerHitZone>(Info.Zone), Equipment->GetTotals(), Combat);
	}
	if (!Info.Resist.IsNone())
	{
		const UGamerBuffComponent* Buffs = GetOwner()->FindComponentByClass<UGamerBuffComponent>();
		float BuffResist = 0.f;
		if (Buffs && Info.Resist == TEXT("cold")) { BuffResist = Buffs->GetBonus(EGamerBuffStat::ColdResist) / 100.f; }
		else if (Buffs && Info.Resist == TEXT("poison")) { BuffResist = Buffs->GetBonus(EGamerBuffStat::PoisonResist) / 100.f; }
		Amount = GamerEquipment::HazardEffect(Amount, Info.Resist, Equipment->GetTotals(), BuffResist, Combat);
	}
	return Amount;
}

void UGamerHealthComponent::TakeDamage(const FGamerDamageInfo& Info)
{
	if (!GetOwner() || !GetOwner()->HasAuthority() || bDead || bDowned || Info.Amount <= 0.f)
	{
		return;
	}
	// World option "erlittener Schaden" (difficulty): factor on every hit.
	const float Amount = ReduceDamage(Info) * FMath::Max(0.f, AGamerGameState::Get(this).DamageTaken);
	HP -= Amount;
	UE_LOG(LogGamer, Log, TEXT("Health: %.1f damage (%s from %s), %.1f / %.0f left"), Amount, *Info.Type.ToString(),
		*GetNameSafe(Info.Source.Get()), FMath::Max(0.f, HP), GetMaxHP());
	if (HP <= 0.f)
	{
		if (TryLastBreath())
		{
			return;
		}
		LoadSettings();
		if (bDownedEnabled && AnyOtherAlivePlayer())
		{
			GoDown(Info);   // co-op: knocked down, teammates may help
		}
		else
		{
			Die(Info);      // solo (nobody else alive): straight to death, same code
		}
	}
}

void UGamerHealthComponent::GoDown(const FGamerDamageInfo& Info)
{
	HP = 0.f;
	bDowned = true;
	DownedLeft = DownedDuration;
	ReviveProgress = 0.f;
	ReviveTarget.Reset();
	AActor* Owner = GetOwner();
	if (ACharacter* Character = Cast<ACharacter>(Owner))
	{
		Character->GetCharacterMovement()->DisableMovement();   // no crawling
	}
	Owner->ForceNetUpdate();
	UE_LOG(LogGamer, Log, TEXT("Health: knocked down (%s), %.0f s to be revived"), *Info.Type.ToString(), DownedLeft);
}

void UGamerHealthComponent::Revive(float HpFraction, const TCHAR* Why)
{
	if (!bDowned)
	{
		return;
	}
	bDowned = false;
	DownedLeft = 0.f;
	HP = FMath::Max(1.f, GetMaxHP() * HpFraction);
	if (ACharacter* Character = Cast<ACharacter>(GetOwner()))
	{
		Character->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
	}
	GetOwner()->ForceNetUpdate();
	UE_LOG(LogGamer, Log, TEXT("Health: revived by %s with %.0f hit points"), Why, HP);
}

void UGamerHealthComponent::NotifyLanded(float FallHeightCm)
{
	LoadSettings();
	if (FallHeightCm > FallMinHeightCm)
	{
		FGamerDamageInfo Info;
		Info.Amount = (FallHeightCm - FallMinHeightCm) / 100.f * FallDamagePer100;
		Info.Type = TEXT("fall");
		TakeDamage(Info);
	}
}

FString UGamerHealthComponent::GetPlayerKey() const
{
	const APawn* Pawn = Cast<APawn>(GetOwner());
	const AController* Controller = Pawn ? Pawn->GetController() : nullptr;
	if (UWorld* World = GetWorld())
	{
		if (UGamerSaveSubsystem* Saves = World->GetSubsystem<UGamerSaveSubsystem>())
		{
			const FString Id = Saves->GetPlayerId(Controller);
			if (!Id.IsEmpty())
			{
				return Id;
			}
		}
	}
	const APlayerState* PlayerState = Pawn ? Pawn->GetPlayerState() : nullptr;
	return PlayerState ? FString::FromInt(PlayerState->GetPlayerId()) : FString();
}

void UGamerHealthComponent::Die(const FGamerDamageInfo& Info)
{
	AActor* Owner = GetOwner();
	UWorld* World = GetWorld();
	HP = 0.f;
	bDead = true;
	DeadSeconds = 0.f;
	DeathTime = World ? World->GetTimeSeconds() : 0.0;

	// What is lost follows the world option "Tod" (decision 2026-11-16).
	const EGamerDeathMode Mode = AGamerGameState::Get(this).DeathMode;
	UGamerInventoryComponent* Inventory = Owner->FindComponentByClass<UGamerInventoryComponent>();
	int32 Lost = 0;
	if (Inventory && Mode != EGamerDeathMode::None)
	{
		TArray<FGamerItemAmount> Everything = Inventory->TakeEverything();
		if (UGamerEquipmentComponent* Equipment = Owner->FindComponentByClass<UGamerEquipmentComponent>())
		{
			Everything.Append(Equipment->TakeEverything());   // worn pieces go with the inventory
		}
		for (const FGamerItemAmount& Amount : Everything) { Lost += Amount.Count; }
		if (Mode == EGamerDeathMode::Grave && Everything.Num() > 0)
		{
			const APawn* Pawn = Cast<APawn>(Owner);
			const AGamerPlayerState* PlayerState = Pawn ? Pawn->GetPlayerState<AGamerPlayerState>() : nullptr;
			FString Name = PlayerState && PlayerState->HasCharacter() ? PlayerState->GetProfile().Name : FString();
			if (Name.IsEmpty() && Pawn && Pawn->GetPlayerState()) { Name = Pawn->GetPlayerState()->GetPlayerName(); }
			AGamerGrave::Create(World, Owner->GetActorLocation() - FVector(0.f, 0.f, 90.f), GetPlayerKey(), Name, Everything);
		}
	}
	UE_LOG(LogGamer, Log, TEXT("Health: died (%s), mode %s, %d pieces %s"), *Info.Type.ToString(),
		Mode == EGamerDeathMode::None ? TEXT("none") : Mode == EGamerDeathMode::Grave ? TEXT("grave") : TEXT("hardcore"), Lost,
		Mode == EGamerDeathMode::Grave ? TEXT("into the grave") : Mode == EGamerDeathMode::Hardcore ? TEXT("lost") : TEXT("kept"));

	// A dead player blocks nobody: invisible, no collision, no movement.
	if (ACharacter* Character = Cast<ACharacter>(Owner))
	{
		Character->GetCharacterMovement()->DisableMovement();
	}
	Owner->SetActorEnableCollision(false);
	Owner->SetActorHiddenInGame(true);
	Owner->ForceNetUpdate();
}

void UGamerHealthComponent::SetSpawnPoint(const FVector& Location)
{
	if (GetOwner() && GetOwner()->HasAuthority())
	{
		SpawnPoint = Location;
		bHasSpawnPoint = true;
		UE_LOG(LogGamer, Log, TEXT("Health: respawn point set to the bed at %s"), *Location.ToCompactString());
	}
}

void UGamerHealthComponent::NoteBuilt(FName BuildableId, const FVector& Location)
{
	LoadSettings();
	if (BuildableId == BedBuilding)
	{
		SetSpawnPoint(Location);
	}
}

void UGamerHealthComponent::Respawn()
{
	AActor* Owner = GetOwner();
	UWorld* World = GetWorld();
	if (!Owner || !World || !bDead)
	{
		return;
	}
	// The bed the player built last, if it still stands; otherwise the start point.
	FVector Target = bHasStartPoint ? StartPoint : Owner->GetActorLocation();
	bool bAtBed = false;
	if (bHasSpawnPoint)
	{
		for (TActorIterator<AGamerBuilding> It(World); It; ++It)
		{
			if (!It->IsPreview() && It->IsFinished() && It->GetBuildableId() == BedBuilding && FVector::Dist(It->GetActorLocation(), SpawnPoint) <= BedCheckCm)
			{
				Target = It->GetActorLocation() + FVector(0.f, 0.f, 110.f);
				bAtBed = true;
				break;
			}
		}
	}
	Owner->SetActorEnableCollision(true);
	Owner->SetActorHiddenInGame(false);
	Owner->TeleportTo(Target + FVector(0.f, 0.f, 20.f), Owner->GetActorRotation());
	if (ACharacter* Character = Cast<ACharacter>(Owner))
	{
		Character->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
		Character->GetCharacterMovement()->Velocity = FVector::ZeroVector;
	}
	bDead = false;
	bDowned = false;
	DeadSeconds = 0.f;
	HP = FMath::Max(1.f, GetMaxHP() * RespawnHealFraction);
	Owner->ForceNetUpdate();
	UE_LOG(LogGamer, Log, TEXT("Health: got up again %s at %s with %.0f hit points"), bAtBed ? TEXT("at the bed") : TEXT("at the start point"), *Target.ToCompactString(), HP);
}

void UGamerHealthComponent::ServerRequestRespawn_Implementation()
{
	LoadSettings();
	if (bDead && DeadSeconds >= MinRespawnDelay)
	{
		Respawn();
	}
}

// --- Graves --------------------------------------------------------------------------------------

AActor* UGamerHealthComponent::FindDownedNearby() const
{
	const AActor* Owner = GetOwner();
	UWorld* World = GetWorld();
	AActor* Best = nullptr;
	float BestDistance = ReviveReachCm;
	if (!Owner || !World || bDead || bDowned)
	{
		return nullptr;
	}
	for (TActorIterator<APawn> It(World); It; ++It)
	{
		const UGamerHealthComponent* Other = It->FindComponentByClass<UGamerHealthComponent>();
		if (Other && Other != this && Other->bDowned)
		{
			const float Distance = FVector::Dist(It->GetActorLocation(), Owner->GetActorLocation());
			if (Distance <= BestDistance)
			{
				BestDistance = Distance;
				Best = *It;
			}
		}
	}
	return Best;
}

void UGamerHealthComponent::ServerSetReviving_Implementation(AActor* TargetPawn, bool bOn)
{
	LoadSettings();
	if (!bOn || bDead || bDowned || !TargetPawn)
	{
		ReviveTarget.Reset();
		ReviveProgress = 0.f;
		return;
	}
	const UGamerHealthComponent* Other = TargetPawn->FindComponentByClass<UGamerHealthComponent>();
	if (Other && Other != this && Other->bDowned && FVector::Dist(TargetPawn->GetActorLocation(), GetOwner()->GetActorLocation()) <= ReviveReachCm * 1.3f)
	{
		if (ReviveTarget.Get() != TargetPawn)
		{
			ReviveProgress = 0.f;
		}
		ReviveTarget = TargetPawn;
	}
}

bool UGamerHealthComponent::IsNearOwnBed() const
{
	const_cast<UGamerHealthComponent*>(this)->LoadSettings();
	if (!bHasSpawnPoint || !GetOwner())
	{
		return false;
	}
	for (TActorIterator<AGamerBuilding> It(GetWorld()); It; ++It)
	{
		if (It->IsFinished() && It->GetBuildableId() == BedBuilding && FVector::Dist(It->GetActorLocation(), SpawnPoint) < 150.f
			&& FVector::Dist(It->GetActorLocation(), GetOwner()->GetActorLocation()) <= BedCheckCm)
		{
			return true;
		}
	}
	return false;
}

bool UGamerHealthComponent::IsMyGrave(const AGamerGrave* Grave) const
{
	if (!Grave)
	{
		return false;
	}
	if (Grave->GetOwnerKey() == GetPlayerKey())
	{
		return true;
	}
	if (!GetOwner()->HasAuthority())
	{
		const APawn* Pawn = Cast<APawn>(GetOwner());
		const AGamerPlayerState* PlayerState = Pawn ? Pawn->GetPlayerState<AGamerPlayerState>() : nullptr;
		return PlayerState && PlayerState->HasCharacter() && PlayerState->GetProfile().Name == Grave->GetOwnerName();
	}
	return false;
}

AGamerGrave* UGamerHealthComponent::FindNearbyGrave() const
{
	const AActor* Owner = GetOwner();
	UWorld* World = GetWorld();
	AGamerGrave* Best = nullptr;
	float BestDistance = const_cast<UGamerHealthComponent*>(this)->GraveReachCm;
	if (!Owner || !World)
	{
		return nullptr;
	}
	for (TActorIterator<AGamerGrave> It(World); It; ++It)
	{
		const float Distance = FVector::Dist(It->GetActorLocation(), Owner->GetActorLocation());
		if (Distance <= BestDistance)
		{
			BestDistance = Distance;
			Best = *It;
		}
	}
	return Best;
}

bool UGamerHealthComponent::MayOpenGrave(const AGamerGrave* Grave) const
{
	if (!Grave)
	{
		return false;
	}
	const_cast<UGamerHealthComponent*>(this)->LoadSettings();
	// The owner at once; teammates after the protection time (they bring the things back for the dead one).
	if (Grave->GetOwnerKey().IsEmpty())
	{
		return true;
	}
	if (Grave->GetOwnerKey() == GetPlayerKey() || Grave->GetAgeSeconds() >= GraveOthersAfter)
	{
		return true;
	}
	// The server is strict (save id or session id). A client does not know its save id, so for the HUD hint only it goes by the name.
	if (!GetOwner()->HasAuthority())
	{
		const APawn* Pawn = Cast<APawn>(GetOwner());
		const AGamerPlayerState* PlayerState = Pawn ? Pawn->GetPlayerState<AGamerPlayerState>() : nullptr;
		return PlayerState && PlayerState->HasCharacter() && PlayerState->GetProfile().Name == Grave->GetOwnerName();
	}
	return false;
}

void UGamerHealthComponent::ServerCollectGrave_Implementation(AGamerGrave* Grave)
{
	AActor* Owner = GetOwner();
	UGamerWorkComponent* Work = Owner ? Owner->FindComponentByClass<UGamerWorkComponent>() : nullptr;
	LoadSettings();
	if (!Grave || !Owner || bDead || FVector::Dist(Grave->GetActorLocation(), Owner->GetActorLocation()) > GraveReachCm * 1.2f)
	{
		return;
	}
	if (!MayOpenGrave(Grave))
	{
		if (Work) { Work->ClientMessage(FString::Printf(TEXT("Das Grab von %s gehört ihm: noch %.0f s Schutzzeit."), *Grave->GetOwnerName(), FMath::Max(0.f, GraveOthersAfter - Grave->GetAgeSeconds()))); }
		return;
	}
	const int32 Before = Grave->GetItemCount();
	const int32 Moved = Grave->CollectInto(Owner->FindComponentByClass<UGamerInventoryComponent>());
	UE_LOG(LogGamer, Log, TEXT("Health: grave of %s opened by %s: %d of %d pieces taken"), *Grave->GetOwnerName(), *GetPlayerKey(), Moved, Before);
	if (Work)
	{
		Work->ClientMessage(Moved >= Before ? FString::Printf(TEXT("Grab von %s geöffnet: alles geborgen (%d)"), *Grave->GetOwnerName(), Moved)
			: FString::Printf(TEXT("Grab von %s: %d geborgen, %d passen nicht mehr und bleiben im Grab"), *Grave->GetOwnerName(), Moved, Before - Moved));
	}
}

// --- Save --------------------------------------------------------------------------------------

void UGamerHealthComponent::WriteSaveState(FGamerSaveRecord& Out) const
{
	Out.SetNumber(TEXT("hp"), bDead ? FMath::Max(1.f, GetMaxHP()) : HP);   // a dead player wakes up healthy after loading
	if (bHasSpawnPoint) { Out.SetVector(TEXT("spawn_point"), SpawnPoint); }
	if (bHasStartPoint) { Out.SetVector(TEXT("start_point"), StartPoint); }
}

void UGamerHealthComponent::ReadSaveState(const FGamerSaveRecord& In)
{
	HP = FMath::Max(1.f, In.GetFloat(TEXT("hp"), GetMaxHP()));
	bHpInitialized = true;
	if (In.Has(TEXT("spawn_point")))
	{
		SpawnPoint = In.GetVector(TEXT("spawn_point"));
		bHasSpawnPoint = true;
	}
	if (In.Has(TEXT("start_point")))
	{
		StartPoint = In.GetVector(TEXT("start_point"));
		bHasStartPoint = true;
	}
}

// --- Tests ---------------------------------------------------------------------------------------

void UGamerHealthComponent::ServerDebugDamage_Implementation(float Amount, FName Type)
{
	FGamerDamageInfo Info;
	Info.Amount = Amount;
	Info.Type = Type.IsNone() ? FName(TEXT("debug")) : Type;
	TakeDamage(Info);
}

void UGamerHealthComponent::ServerDebugHit_Implementation(float Amount, int32 Zone, FName ResistKind)
{
	FGamerDamageInfo Info;
	Info.Amount = Amount;
	Info.Type = TEXT("debug");
	Info.Zone = Zone;
	Info.Resist = ResistKind;
	TakeDamage(Info);
}

void UGamerHealthComponent::ServerDebugFall_Implementation(float HeightCm)
{
	NotifyLanded(HeightCm);
}

void UGamerHealthComponent::ServerDebugRespawnNow_Implementation()
{
	Respawn();
}

void UGamerHealthComponent::ServerDebugHealingHands_Implementation(bool bOn)
{
	bDebugHealingHands = bOn;
}

void UGamerHealthComponent::ServerDebugNewDay_Implementation()
{
	LastBreathDay = -1;
	HealingHandsDay = -1;
}

FString UGamerHealthComponent::DescribeState() const
{
	return FString::Printf(TEXT("hp %.1f / %.0f dead %d downed %d (%.1f s) fed %d wait %.1f revive %.2f"), HP, GetMaxHP(), bDead ? 1 : 0, bDowned ? 1 : 0,
		DownedLeft, IsFed() ? 1 : 0, GetRespawnWait(), ReviveProgress);
}
