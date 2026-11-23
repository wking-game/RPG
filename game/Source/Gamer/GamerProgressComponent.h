#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GamerSaveable.h"
#include "GamerProgressComponent.generated.h"

/** Where experience comes from; decides the co-op share (docs/21 section 3: fight, boss, quest, discovery full; work 25 %). */
enum class EGamerXpSource : uint8
{
	Fight,
	Discovery,
	Work
};

/**
 * Level and experience of a player (decision 2026-11-22: level 1-50, experience from everything, docs/21 section 3, docs/25).
 * Server-authoritative; experience is replicated to the owner and saved per player (key "progress"). The curve comes from
 * `progression.json`. Skill points are counted (1 per level, a bonus every 5th level) but not spent yet: the skill tree is missing.
 * Gathering, crafting and building give experience now; fights, quests and discoveries use `AwardXp` once those systems exist.
 * Players within the co-op radius share what one of them earns: full for fights and discoveries, a quarter for work.
 */
UCLASS(ClassGroup = (Gamer), meta = (BlueprintSpawnableComponent))
class GAMER_API UGamerProgressComponent : public UActorComponent, public IGamerSaveable
{
	GENERATED_BODY()

public:
	UGamerProgressComponent();

	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// IGamerSaveable
	virtual FString GetSaveKey() const override { return TEXT("progress"); }
	virtual void WriteSaveState(FGamerSaveRecord& Out) const override;
	virtual void ReadSaveState(const FGamerSaveRecord& In) override;

	// --- Queries (server and owning client) ---

	float GetXp() const { return Xp; }
	int32 GetLevel() const;
	int32 GetMaxLevel() const;
	/** Experience gathered inside the current level and the size of the level (0 at the maximum level). */
	int32 GetXpIntoLevel() const;
	int32 GetXpForNextLevel() const;
	/** Tier (1-6) of the current level, 0 without data. */
	int32 GetTier() const;
	/** Skill points earned so far (not spent yet). */
	int32 GetSkillPointsEarned() const;

	// --- Server ---

	/** Gives experience to this player and, by the source, a share to others nearby. Returns the amount this player got. */
	float AwardXp(float Amount, EGamerXpSource Source, bool bShareWithNearby = true);
	/** The player made a recipe (first time counts more). */
	void NoteCraft(FName RecipeId, int32 Tier, int32 Count);
	/** The player gathered pieces of a tier. */
	void NoteGather(int32 Tier, int32 Count);
	/** The player finished (placed) a building piece of a tier. */
	void NoteBuild(int32 Tier);

	/** Tests only: gives experience (no sharing). */
	UFUNCTION(Server, Reliable)
	void ServerDebugAddXp(float Amount);
	UFUNCTION(Client, Reliable)
	void ClientLevelUp(int32 NewLevel, int32 PointsGained);

	FString Describe() const;

	/** Last level-up for the HUD: the new level and when (world seconds on this machine). */
	int32 GetLastLevelUp() const { return LastLevelUp; }
	double GetLastLevelUpTime() const { return LastLevelUpTime; }

	DECLARE_MULTICAST_DELEGATE_OneParam(FOnLevelUp, int32);
	FOnLevelUp OnLevelUp;

private:
	/** Adds experience on this player only and handles level-ups. */
	void AddXpLocal(float Amount);

	UPROPERTY(Replicated)
	float Xp = 0.f;

	/** Level the player had when the last level-up was announced. */
	int32 KnownLevel = 1;
	/** Recipes made at least once (first-time bonus). Saved. */
	TSet<FName> CraftedRecipes;
	int32 LastLevelUp = 0;
	double LastLevelUpTime = -100.0;
};
