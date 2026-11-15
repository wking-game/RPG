#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GamerSaveable.h"
#include "GamerResourceNode.generated.h"

class UStaticMeshComponent;
struct FGamerResourceNodeDef;
struct FGamerItemAmount;

/** Where a season node (fruit tree, wild bush) is in its yearly cycle. */
UENUM()
enum class EGamerFruitStage : uint8
{
	/** Not a season node. */
	None,
	/** No fruit (late autumn, winter, early spring). */
	Bare,
	Bloom,
	Unripe,
	/** Ripe fruit can be picked; from the "fall" phase on, unpicked fruit drops to the ground bit by bit. */
	Ripe
};

/**
 * A resource in the world (tree, rock, bush, loose stones ...). Its values come from
 * resource_nodes.json via NodeId. The server decides when it is harvested and when it grows back;
 * clients only see the replicated "depleted" state.
 * Looks: the models from the data (meshes: full, depleted, growing), else simple engine shapes as placeholders.
 * Visible regrowth: with "growing" models (sapling, young tree) a used-up node shows the depleted model (stump)
 * first and then grows through the stages until it is full again; it can only be harvested when full.
 * Season nodes (data "season": fruit trees, wild bushes; decision 2026-11-14) follow the year instead:
 * bloom -> unripe -> ripe (pick with E) -> unpicked fruit falls as items to the ground -> bare, no fruit in winter.
 * They count as "depleted" whenever there is nothing to pick, so focus, villagers and the HUD need no extra rules.
 */
UCLASS(Config = Game)
class GAMER_API AGamerResourceNode : public AActor, public IGamerSaveable
{
	GENERATED_BODY()

public:
	// IGamerSaveable (save system, implemented in GamerSaveImpl.cpp)
	virtual void WriteSaveState(FGamerSaveRecord& Out) const override;
	virtual void ReadSaveState(const FGamerSaveRecord& In) override;

public:
	AGamerResourceNode();

	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	/** Server: set which data row this node uses (before or right after spawning). */
	void SetNodeId(FName InNodeId);
	FName GetNodeId() const { return NodeId; }

	const FGamerResourceNodeDef* GetDef() const;
	bool IsDepleted() const { return bDepleted; }

	/** Server: mark as harvested and start the regrow timer (if the data says it regrows). */
	void Deplete();

	/**
	 * Server: what one harvest gives (drops, or the next handful of fruit for season nodes) and use the node up
	 * accordingly. The single entry point for players and villagers.
	 */
	TArray<FGamerItemAmount> Harvest();

	EGamerFruitStage GetFruitStage() const { return FruitStage; }
	int32 GetFruitLeft() const { return FruitLeft; }

	/**
	 * Server, before FinishSpawning: a freshly planted sapling (decision 2026-11-14: grow fruit trees yourself).
	 * It shows the growing models (sapling, young) and becomes a normal node after GrowDays game days.
	 */
	void SetPlanted(float GrowDays);
	bool IsPlantedGrowing() const { return bPlantedGrowing; }

	/** Name shown in hints ("Wildapfelbaum (blüht)"). */
	FString GetDisplayName() const;

	/** Server: add strike work (fraction of the whole job, 0..1). Returns true when the node is done. */
	bool AddWork(float Amount);

	/** 0..1, how far players have chopped/mined it (replicated for the HUD). */
	float GetWorkProgress() const { return WorkProgress; }

	/** Server: who is gathering it by hand right now (only one player at a time; not used for strikes). */
	TWeakObjectPtr<AActor> Worker;

protected:
	UPROPERTY(EditAnywhere, ReplicatedUsing = OnRep_NodeId, Category = "Resource")
	FName NodeId;

	UPROPERTY(ReplicatedUsing = OnRep_Depleted)
	bool bDepleted = false;

	/** While depleted: 0 = depleted model (stump), 1..n = growing model n (sapling, young tree ...). */
	UPROPERTY(ReplicatedUsing = OnRep_Depleted)
	uint8 GrowStage = 0;

	UPROPERTY(Replicated)
	float WorkProgress = 0.f;

	UPROPERTY(ReplicatedUsing = OnRep_Depleted)
	EGamerFruitStage FruitStage = EGamerFruitStage::None;

	/** Fruit still on the tree (season nodes). */
	UPROPERTY(Replicated)
	int32 FruitLeft = 0;

	UPROPERTY(VisibleAnywhere, Category = "Visual")
	TObjectPtr<UStaticMeshComponent> Body;

	UPROPERTY(VisibleAnywhere, Category = "Visual")
	TObjectPtr<UStaticMeshComponent> Top;

	/** Length of one game day in seconds (design register: 30 minutes). Scales regrow_days. */
	UPROPERTY(Config, EditDefaultsOnly, Category = "Resource")
	float DayLengthSeconds = 1800.f;

	UFUNCTION()
	void OnRep_NodeId();

	UFUNCTION()
	void OnRep_Depleted();

private:
	void ApplyLook();
	/** Real models from the data (resource_nodes.json "meshes": full, depleted); no depleted model = hidden when used up. */
	void ApplyModel(const FGamerResourceNodeDef& Def);
	void Regrow();
	/** Server: next growing stage (timer), visible to everyone through GrowStage. */
	void Grow();

	/** Server: follow the season (stage, fruit count, falling fruit). */
	void UpdateSeason();
	/** Server: drop one fruit to the ground under the crown (bruised when it falls far). */
	void DropFruit();

	FTimerHandle SeasonTimer;
	void StartSeason();
	/** Planted and still growing: no fruit, no harvest, season waits. */
	bool bPlantedGrowing = false;
	float PlantedGrowDays = 0.f;
	/** Fruit on the tree when falling started, and how many have fallen since (season nodes). */
	int32 FruitAtFallStart = 0;
	int32 FruitFallen = 0;
	bool bFruitRolled = false;

	FTimerHandle RegrowTimer;
	FTimerHandle GrowTimer;
	/** Seconds between growing stages (set when depleted). */
	float GrowStepSeconds = 0.f;
};
