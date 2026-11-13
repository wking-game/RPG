#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GamerWorkComponent.generated.h"

class AGamerResourceNode;
class UGamerInventoryComponent;
struct FGamerResourceNodeDef;

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

	/** Gather by hand with the interact key (sticks, loose stones, plants). Timed, one player per node. */
	UFUNCTION(Server, Reliable)
	void ServerHarvest(AGamerResourceNode* Node);

	/**
	 * Strike with the item in hand (trees, rocks; needs the right tool selected in the hotbar).
	 * Each strike adds StrikeInterval x tool speed of work; work_seconds in the data is the whole job.
	 * Several players may hit the same node; whoever lands the last strike gets the drops.
	 * Node may be null (a swing into the air).
	 */
	UFUNCTION(Server, Reliable)
	void ServerStrike(AGamerResourceNode* Node);

	UFUNCTION(Server, Reliable)
	void ServerCraft(FName RecipeId);

	/** Development builds only: move the player next to a node or building (automated tests). Ignored in Shipping. */
	UFUNCTION(Server, Reliable)
	void ServerDebugTeleportNear(AActor* Target);

	/** Development builds only: move the player to a point (automated tests, screenshots). Ignored in Shipping. */
	UFUNCTION(Server, Reliable)
	void ServerDebugTeleportTo(FVector Location);

	/** Development builds only: put items into the inventory (automated tests). Ignored in Shipping. */
	UFUNCTION(Server, Reliable)
	void ServerDebugGiveItem(FName Item, int32 Count);

	/** Finished station (work bench, camp fire ...) of that buildable id within reach of the player, or null. */
	class AGamerBuilding* FindStationNearby(FName StationId) const;

	/** How far a station may be for crafting at it (cm, to its pivot). */
	float GetStationReach() const { return StationReach; }

	/** Short feedback for the owning player ("+2 Äste", "Du brauchst eine Axt"). */
	UFUNCTION(Client, Reliable)
	void ClientMessage(const FString& Message);

	/** Local player: show a feedback message without asking the server (e.g. "Nimm den Hammer in die Hand"). */
	void ShowMessage(const FString& Message);

	bool IsBusy() const { return Work.Kind != EGamerWorkKind::None; }
	const FGamerWorkState& GetWork() const { return Work; }

	/** 0..1 progress of the current work, computed from server time (works on clients too). */
	float GetProgress() const;

	const FString& GetLastMessage() const { return LastMessage; }
	double GetLastMessageTime() const { return LastMessageTime; }

	/** How far (cm, centre to centre) a player may stand from a resource to work on it. */
	float GetMaxReach() const { return MaxReach; }

	/** Seconds between two strikes. */
	float GetStrikeInterval() const { return StrikeInterval; }

protected:
	UPROPERTY(Config, EditDefaultsOnly, Category = "Work")
	float MaxReach = 400.f;

	UPROPERTY(Config, EditDefaultsOnly, Category = "Work")
	float StrikeInterval = 0.8f;

	UPROPERTY(Config, EditDefaultsOnly, Category = "Work")
	float StationReach = 500.f;

	UPROPERTY(Replicated)
	FGamerWorkState Work;

private:
	void StartWork(EGamerWorkKind Kind, const FString& Label, float Duration);
	void FinishWork();
	void GiveDrops(AGamerResourceNode* Node, const FGamerResourceNodeDef& Def);
	void Fail(const FString& Message);
	bool IsInReach(const AActor* Target) const;
	UGamerInventoryComponent* GetInventory() const;
	double ServerNow() const;

	TWeakObjectPtr<AGamerResourceNode> TargetNode;
	FName TargetRecipe;
	FTimerHandle WorkTimer;
	double LastStrikeTime = -1000.0;

	FString LastMessage;
	double LastMessageTime = -1000.0;
};
