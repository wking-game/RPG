#include "GamerStaminaComponent.h"

#include "Gamer.h"
#include "GamerBuffComponent.h"
#include "GamerBuilding.h"
#include "GamerCharacter.h"
#include "GamerGameState.h"
#include "GamerHealthComponent.h"
#include "GamerWorkComponent.h"
#include "Dom/JsonObject.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Net/UnrealNetwork.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"

UGamerStaminaComponent::UGamerStaminaComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickInterval = 0.1f;
	SetIsReplicatedByDefault(true);
}

void UGamerStaminaComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION(UGamerStaminaComponent, Stamina, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(UGamerStaminaComponent, Exhaustion, COND_OwnerOnly);
}

void UGamerStaminaComponent::LoadSettings()
{
	if (bLoaded)
	{
		return;
	}
	bLoaded = true;
	FString Text;
	TSharedPtr<FJsonObject> Root;
	if (!FFileHelper::LoadFileToString(Text, *(FPaths::ProjectContentDir() / TEXT("Data/stamina.json")))
		|| !FJsonSerializer::Deserialize(TJsonReaderFactory<>::Create(Text), Root) || !Root.IsValid())
	{
		UE_LOG(LogGamer, Warning, TEXT("Stamina: stamina.json not readable, using defaults"));
		return;
	}
	auto Number = [](const TSharedPtr<FJsonObject>& Object, const TCHAR* Field, float& Value)
	{
		double Read = 0.0;
		if (Object.IsValid() && Object->TryGetNumberField(Field, Read)) { Value = static_cast<float>(Read); }
	};
	Number(Root, TEXT("base_max"), BaseMax);
	Number(Root, TEXT("regen_per_s"), RegenPerSecond);
	Number(Root, TEXT("regen_delay_s"), RegenDelay);
	Number(Root, TEXT("not_fed_regen_factor"), NotFedRegenFactor);
	Number(Root, TEXT("sprint_cost_per_s"), SprintCostPerSecond);
	Number(Root, TEXT("jump_cost"), JumpCost);
	Number(Root, TEXT("strike_cost"), StrikeCost);
	const TSharedPtr<FJsonObject>* Object = nullptr;
	if (Root->TryGetObjectField(TEXT("empty"), Object))
	{
		Number(*Object, TEXT("speed_factor"), EmptySpeedFactor);
		Number(*Object, TEXT("strike_factor"), EmptyStrikeFactor);
		Number(*Object, TEXT("recover_fraction"), RecoverFraction);
	}
	if (Root->TryGetObjectField(TEXT("exhaustion"), Object))
	{
		Number(*Object, TEXT("rise_per_game_day"), RisePerGameDay);
		Number(*Object, TEXT("max_penalty"), MaxPenalty);
		Number(*Object, TEXT("work_speed_penalty"), WorkSpeedPenalty);
		Number(*Object, TEXT("sleep_recovery"), SleepRecovery);
		Number(*Object, TEXT("sleep_min_exhaustion"), SleepMinExhaustion);
		Number(*Object, TEXT("sleep_cooldown_s"), SleepCooldown);
	}
	UE_LOG(LogGamer, Log, TEXT("Stamina: max %.0f, regen %.0f/s, exhaustion %.2f per game day"), BaseMax, RegenPerSecond, RisePerGameDay);
}

float UGamerStaminaComponent::BuffBonus(uint8 Stat) const
{
	const UGamerBuffComponent* Buffs = GetOwner() ? GetOwner()->FindComponentByClass<UGamerBuffComponent>() : nullptr;
	return Buffs ? Buffs->GetBonus(static_cast<EGamerBuffStat>(Stat)) : 0.f;
}

bool UGamerStaminaComponent::IsFed() const
{
	const UGamerBuffComponent* Buffs = GetOwner() ? GetOwner()->FindComponentByClass<UGamerBuffComponent>() : nullptr;
	return Buffs && Buffs->GetActiveMeals().Num() > 0;
}

float UGamerStaminaComponent::GetBaseMax() const
{
	const_cast<UGamerStaminaComponent*>(this)->LoadSettings();
	return BaseMax + BuffBonus(static_cast<uint8>(EGamerBuffStat::StaminaMax));
}

float UGamerStaminaComponent::GetMaxStamina() const
{
	return FMath::Max(10.f, GetBaseMax() * (1.f - MaxPenalty * Exhaustion));
}

float UGamerStaminaComponent::GetSpeedFactor() const
{
	return bEmpty ? EmptySpeedFactor : 1.f;
}

float UGamerStaminaComponent::GetStrikeFactor() const
{
	return bEmpty ? EmptyStrikeFactor : 1.f;
}

float UGamerStaminaComponent::GetWorkSpeedFactor() const
{
	const_cast<UGamerStaminaComponent*>(this)->LoadSettings();
	return FMath::Max(0.2f, 1.f - WorkSpeedPenalty * Exhaustion);
}

void UGamerStaminaComponent::Spend(float Amount)
{
	Stamina = FMath::Max(0.f, Stamina - Amount);
	LastSpendTime = GetWorld()->GetTimeSeconds();
}

void UGamerStaminaComponent::NoteJump()
{
	if (GetOwner() && GetOwner()->HasAuthority())
	{
		LoadSettings();
		Spend(JumpCost);
	}
}

void UGamerStaminaComponent::NoteStrike()
{
	if (GetOwner() && GetOwner()->HasAuthority())
	{
		LoadSettings();
		Spend(StrikeCost);
	}
}

void UGamerStaminaComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	LoadSettings();
	AGamerCharacter* Character = Cast<AGamerCharacter>(GetOwner());
	if (!Character)
	{
		return;
	}
	const bool bAuthority = Character->HasAuthority();
	if (!bInitialised && bAuthority)
	{
		bInitialised = true;
		if (Stamina >= 99.9f && Exhaustion <= 0.f) { Stamina = GetMaxStamina(); }   // a loaded value stays
	}
	const float Max = GetMaxStamina();
	if (bAuthority)
	{
		const UGamerHealthComponent* Health = Character->GetHealth();
		if (Health && Health->IsDead())
		{
			return;
		}
		const double Now = GetWorld()->GetTimeSeconds();
		// Sprinting while really moving on the ground costs stamina; the sprint itself is never blocked.
		const UCharacterMovementComponent* Movement = Character->GetCharacterMovement();
		const bool bRunning = Character->IsSprinting() && Movement->IsMovingOnGround() && Character->GetVelocity().Size2D() > 200.f;
		if (bRunning)
		{
			Spend(SprintCostPerSecond * DeltaTime);
		}
		else if (Now - LastSpendTime >= RegenDelay)
		{
			const float Rate = (RegenPerSecond + BuffBonus(static_cast<uint8>(EGamerBuffStat::StaminaRegen))) * (IsFed() ? 1.f : NotFedRegenFactor);
			Stamina = FMath::Min(Max, Stamina + Rate * DeltaTime);
		}
		Stamina = FMath::Min(Stamina, Max);
		// Exhaustion rises with time (world option NeedsRate scales it).
		const float Rate = AGamerGameState::Get(this).NeedsRate;
		Exhaustion = FMath::Min(1.f, Exhaustion + RisePerGameDay / FMath::Max(60.f, GameDaySeconds) * Rate * TimeLapse * DeltaTime);
	}
	// Empty with hysteresis, on the server and on the owning client (both slow the walk speed).
	const bool bWasEmpty = bEmpty;
	if (Stamina <= 0.5f) { bEmpty = true; }
	else if (Stamina >= Max * RecoverFraction) { bEmpty = false; }
	if (bEmpty != bWasEmpty)
	{
		Character->ApplyMoveSpeed();
	}
}

bool UGamerStaminaComponent::CanSleepNow() const
{
	return Exhaustion >= SleepMinExhaustion;
}

void UGamerStaminaComponent::ServerSleep_Implementation()
{
	LoadSettings();
	AGamerCharacter* Character = Cast<AGamerCharacter>(GetOwner());
	UGamerWorkComponent* Work = Character ? Character->FindComponentByClass<UGamerWorkComponent>() : nullptr;
	if (!Character || !Character->GetHealth() || !Character->GetHealth()->IsNearOwnBed() || Character->GetHealth()->IsIncapacitated())
	{
		return;
	}
	const double Now = GetWorld()->GetTimeSeconds();
	if (!CanSleepNow() || Now - LastSleepTime < SleepCooldown)
	{
		if (Work) { Work->ClientMessage(TEXT("Du bist nicht müde genug zum Schlafen.")); }
		UE_LOG(LogGamer, Log, TEXT("Stamina: sleep refused (exhaustion %.2f)"), Exhaustion);
		return;
	}
	LastSleepTime = Now;
	Exhaustion = FMath::Max(0.f, Exhaustion - SleepRecovery);
	Stamina = GetMaxStamina();
	UE_LOG(LogGamer, Log, TEXT("Stamina: slept at the bed, exhaustion now %.2f"), Exhaustion);
	if (Work) { Work->ClientMessage(TEXT("Du hast geschlafen und fühlst dich ausgeruht.")); }
}

void UGamerStaminaComponent::ServerSleepAt_Implementation(AGamerBuilding* Bed)
{
	LoadSettings();
	AGamerCharacter* Character = Cast<AGamerCharacter>(GetOwner());
	UGamerWorkComponent* Work = Character ? Character->FindComponentByClass<UGamerWorkComponent>() : nullptr;
	if (!Character || !Character->GetHealth() || Character->GetHealth()->IsIncapacitated() || !Bed || !Bed->IsFinished()
		|| Bed->GetBuildableId() != TEXT("building_bed") || FVector::Dist(Bed->GetActorLocation(), Character->GetActorLocation()) > 450.f)
	{
		return;
	}
	// Any bed: it becomes the place to get up again, and tells so.
	Character->GetHealth()->SetSpawnPoint(Bed->GetActorLocation());
	const double Now = GetWorld()->GetTimeSeconds();
	if (!CanSleepNow() || Now - LastSleepTime < SleepCooldown)
	{
		if (Work) { Work->ClientMessage(TEXT("Bett als Aufstehpunkt gemerkt. Du bist nicht müde genug zum Schlafen.")); }
		return;
	}
	LastSleepTime = Now;
	Exhaustion = FMath::Max(0.f, Exhaustion - SleepRecovery);
	Stamina = GetMaxStamina();
	UE_LOG(LogGamer, Log, TEXT("Stamina: slept at a bed, exhaustion now %.2f"), Exhaustion);
	if (Work) { Work->ClientMessage(TEXT("Du hast geschlafen und fühlst dich ausgeruht. Das Bett ist dein Aufstehpunkt.")); }
}

void UGamerStaminaComponent::ServerDebugSet_Implementation(float NewStamina, float NewExhaustion)
{
	LoadSettings();
	if (NewExhaustion >= 0.f) { Exhaustion = FMath::Clamp(NewExhaustion, 0.f, 1.f); }
	if (NewStamina >= 0.f) { Stamina = FMath::Clamp(NewStamina, 0.f, GetMaxStamina()); LastSpendTime = GetWorld()->GetTimeSeconds(); }
}

void UGamerStaminaComponent::ServerDebugTimeLapse_Implementation(float Factor)
{
	TimeLapse = FMath::Max(0.f, Factor);
}

FString UGamerStaminaComponent::DescribeState() const
{
	return FString::Printf(TEXT("stamina %.1f / %.0f (base %.0f) empty %d exhaustion %.2f speed x%.2f work x%.2f"), Stamina, GetMaxStamina(), GetBaseMax(),
		bEmpty ? 1 : 0, Exhaustion, GetSpeedFactor(), GetWorkSpeedFactor());
}

void UGamerStaminaComponent::WriteSaveState(FGamerSaveRecord& Out) const
{
	Out.SetNumber(TEXT("stamina"), Stamina);
	Out.SetNumber(TEXT("exhaustion"), Exhaustion);
}

void UGamerStaminaComponent::ReadSaveState(const FGamerSaveRecord& In)
{
	Exhaustion = FMath::Clamp(In.GetFloat(TEXT("exhaustion"), 0.f), 0.f, 1.f);
	Stamina = FMath::Max(0.f, In.GetFloat(TEXT("stamina"), GetBaseMax()));
	bInitialised = true;
}
