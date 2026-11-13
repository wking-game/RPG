#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GamerWorkComponent.generated.h"

class AGamerResourceNode;
class UGamerInventoryComponent;

UENUM()
enum class EGamerWorkKind : uint8
{
	None,
	Harvest,
	Craft
};

/** What the player is doing right now. Replicated so the HUD (and later animations) can show it. */
USTRUCT()
struct FGamerWorkState
{
	GENERATED_BODY()

	UPROPERTY()
	EGamerWorkKind Kind = EGamerWorkKind::None;

	UPROPERTY()
	FString Label;

	/** Server world time when the work started (GameState::GetServerWorldTimeSeconds). */
	UPROPERTY()
	double StartTime = 0.0;

	UPROPERTY()
	float Duration = 0.f;
};

/**
 * Timed player actions: harvesting resource nodes and crafting by hand.
 * Clients only ask (Server RPCs); the server checks reach, tools, ingredients and inventory space,
 * runs the timer and hands out the items. One action at a time.
 */
UCLASS(ClassGroup = (Gamer), Config = Game)
class GAMER_API UGamerWorkComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UGamerWorkComponent();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(Server, Reliable)
	void ServerHarvest(AGamerResourceNode* Node);

	UFUNCTION(Server, Reliable)
	void ServerCraft(FName RecipeId);

	/** Development builds only: move the player next to a node (automated tests). Ignored in Shipping. */
	UFUNCTION(Server, Reliable)
	void ServerDebugTeleportNear(AGamerResourceNode* Node);

	/** Short feedback for the owning player ("+2 Äste", "Du brauchst eine Axt"). */
	UFUNCTION(Client, Reliable)
	void ClientMessage(const FString& Message);

	bool IsBusy() const { return Work.Kind != EGamerWorkKind::None; }
	const FGamerWorkState& GetWork() const { return Work; }

	/** 0..1 progress of the current work, computed from server time (works on clients too). */
	float GetProgress() const;

	const FString& GetLastMessage() const { return LastMessage; }
	double GetLastMessageTime() const { return LastMessageTime; }

	/** How far (cm, centre to centre) a player may stand from a resource to work on it. */
	float GetMaxReach() const { return MaxReach; }

protected:
	UPROPERTY(Config, EditDefaultsOnly, Category = "Work")
	float MaxReach = 400.f;

	UPROPERTY(Replicated)
	FGamerWorkState Work;

private:
	void StartWork(EGamerWorkKind Kind, const FString& Label, float Duration);
	void FinishWork();
	void Fail(const FString& Message);
	bool IsInReach(const AActor* Target) const;
	UGamerInventoryComponent* GetInventory() const;
	double ServerNow() const;

	TWeakObjectPtr<AGamerResourceNode> TargetNode;
	FName TargetRecipe;
	FTimerHandle WorkTimer;

	FString LastMessage;
	double LastMessageTime = -1000.0;
};
