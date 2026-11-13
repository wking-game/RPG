#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GamerResourceNode.generated.h"

class UStaticMeshComponent;
struct FGamerResourceNodeDef;

/**
 * A resource in the world (tree, rock, bush, loose stones ...). Its values come from
 * resource_nodes.json via NodeId. The server decides when it is harvested and when it grows back;
 * clients only see the replicated "depleted" state.
 * Looks: simple engine shapes as placeholders until the real models are imported.
 */
UCLASS(Config = Game)
class GAMER_API AGamerResourceNode : public AActor
{
	GENERATED_BODY()

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

	UPROPERTY(Replicated)
	float WorkProgress = 0.f;

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
	void Regrow();

	FTimerHandle RegrowTimer;
};
