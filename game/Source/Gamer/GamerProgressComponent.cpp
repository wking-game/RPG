#include "GamerProgressComponent.h"

#include "EngineUtils.h"
#include "Engine/World.h"
#include "Gamer.h"
#include "GamerDataSubsystem.h"
#include "GamerProgression.h"
#include "GameFramework/Pawn.h"
#include "Net/UnrealNetwork.h"

namespace
{
	const FGamerProgressionData* ProgressionOf(const UObject* Context)
	{
		const UGamerDataSubsystem* Data = UGamerDataSubsystem::Get(Context);
		return (Data && Data->GetProgression().bLoaded) ? &Data->GetProgression() : nullptr;
	}
}

UGamerProgressComponent::UGamerProgressComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
}

void UGamerProgressComponent::BeginPlay()
{
	Super::BeginPlay();
	KnownLevel = GetLevel();
}

void UGamerProgressComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION(UGamerProgressComponent, Xp, COND_OwnerOnly);
}

int32 UGamerProgressComponent::GetLevel() const
{
	const FGamerProgressionData* Data = ProgressionOf(this);
	return Data ? Data->LevelForXp(static_cast<int64>(Xp)) : 1;
}

int32 UGamerProgressComponent::GetMaxLevel() const
{
	const FGamerProgressionData* Data = ProgressionOf(this);
	return Data ? Data->MaxLevel : 1;
}

int32 UGamerProgressComponent::GetXpIntoLevel() const
{
	const FGamerProgressionData* Data = ProgressionOf(this);
	return Data ? static_cast<int32>(static_cast<int64>(Xp) - Data->TotalXpForLevel(GetLevel())) : 0;
}

int32 UGamerProgressComponent::GetXpForNextLevel() const
{
	const FGamerProgressionData* Data = ProgressionOf(this);
	return Data ? Data->XpToNextLevel(GetLevel()) : 0;
}

int32 UGamerProgressComponent::GetTier() const
{
	const FGamerProgressionData* Data = ProgressionOf(this);
	const FGamerTierDef* Tier = Data ? Data->TierForLevel(GetLevel()) : nullptr;
	return Tier ? Tier->Tier : 0;
}

int32 UGamerProgressComponent::GetSkillPointsEarned() const
{
	const FGamerProgressionData* Data = ProgressionOf(this);
	return Data ? Data->SkillPointsForLevel(GetLevel()) : 0;
}

void UGamerProgressComponent::AddXpLocal(float Amount)
{
	const FGamerProgressionData* Data = ProgressionOf(this);
	if (!Data || Amount <= 0.f)
	{
		return;
	}
	const int64 Cap = Data->TotalXpForLevel(Data->MaxLevel);
	Xp = FMath::Min(Xp + Amount, static_cast<float>(Cap));
	const int32 NowLevel = Data->LevelForXp(static_cast<int64>(Xp));
	if (NowLevel > KnownLevel)
	{
		const int32 Points = Data->SkillPointsForLevel(NowLevel) - Data->SkillPointsForLevel(KnownLevel);
		UE_LOG(LogGamer, Log, TEXT("Progress: level %d -> %d (%.0f experience, +%d skill points)"), KnownLevel, NowLevel, Xp, Points);
		KnownLevel = NowLevel;
		ClientLevelUp(NowLevel, Points);
	}
	if (GetOwner()) { GetOwner()->ForceNetUpdate(); }
}

float UGamerProgressComponent::AwardXp(float Amount, EGamerXpSource Source, bool bShareWithNearby)
{
	const FGamerProgressionData* Data = ProgressionOf(this);
	if (!GetOwner() || !GetOwner()->HasAuthority() || !Data || Amount <= 0.f)
	{
		return 0.f;
	}
	AddXpLocal(Amount);
	if (bShareWithNearby && GetWorld())
	{
		const float Share = Source == EGamerXpSource::Work ? Data->CoopShareWork : Data->CoopShareCombat;
		if (Share > 0.f)
		{
			for (TActorIterator<APawn> It(GetWorld()); It; ++It)
			{
				UGamerProgressComponent* Other = It->FindComponentByClass<UGamerProgressComponent>();
				if (Other && Other != this && FVector::Dist(It->GetActorLocation(), GetOwner()->GetActorLocation()) <= Data->CoopRadiusCm)
				{
					Other->AddXpLocal(Amount * Share);
				}
			}
		}
	}
	return Amount;
}

void UGamerProgressComponent::NoteCraft(FName RecipeId, int32 Tier, int32 Count)
{
	const FGamerProgressionData* Data = ProgressionOf(this);
	if (!Data)
	{
		return;
	}
	bool bAlready = false;
	CraftedRecipes.Add(RecipeId, &bAlready);
	AwardXp(Data->CraftXp(Tier, Count, !bAlready), EGamerXpSource::Work);
}

void UGamerProgressComponent::NoteGather(int32 Tier, int32 Count)
{
	if (const FGamerProgressionData* Data = ProgressionOf(this))
	{
		AwardXp(Data->GatherXp(Tier, Count), EGamerXpSource::Work);
	}
}

void UGamerProgressComponent::NoteBuild(int32 Tier)
{
	if (const FGamerProgressionData* Data = ProgressionOf(this))
	{
		AwardXp(Data->BuildXp(Tier), EGamerXpSource::Work);
	}
}

void UGamerProgressComponent::ServerDebugAddXp_Implementation(float Amount)
{
	AwardXp(Amount, EGamerXpSource::Fight, false);
}

void UGamerProgressComponent::ClientLevelUp_Implementation(int32 NewLevel, int32 PointsGained)
{
	LastLevelUp = NewLevel;
	LastLevelUpTime = GetWorld() ? GetWorld()->GetTimeSeconds() : 0.0;
	OnLevelUp.Broadcast(NewLevel);
}

FString UGamerProgressComponent::Describe() const
{
	return FString::Printf(TEXT("level %d (tier %d), %.1f experience (%d / %d in this level), %d skill points"), GetLevel(), GetTier(), Xp,
		GetXpIntoLevel(), GetXpForNextLevel(), GetSkillPointsEarned());
}

void UGamerProgressComponent::WriteSaveState(FGamerSaveRecord& Out) const
{
	Out.SetNumber(TEXT("xp"), Xp);
	TArray<FName> Crafted = CraftedRecipes.Array();
	Crafted.Sort(FNameLexicalLess());
	Out.SetNames(TEXT("crafted"), Crafted);
}

void UGamerProgressComponent::ReadSaveState(const FGamerSaveRecord& In)
{
	Xp = FMath::Max(0.f, In.GetFloat(TEXT("xp")));
	CraftedRecipes.Reset();
	const UGamerDataSubsystem* Data = UGamerDataSubsystem::Get(this);
	for (const FName Recipe : In.GetNames(TEXT("crafted")))
	{
		if (!Data || Data->FindRecipe(Recipe))
		{
			CraftedRecipes.Add(Recipe);
		}
	}
	// A loaded level is not announced as a level-up.
	const FGamerProgressionData* Progression = ProgressionOf(this);
	if (Progression)
	{
		Xp = FMath::Min(Xp, static_cast<float>(Progression->TotalXpForLevel(Progression->MaxLevel)));
	}
	KnownLevel = GetLevel();
	if (GetOwner()) { GetOwner()->ForceNetUpdate(); }
}
