#pragma once

#include "CoreMinimal.h"
#include "GamerAnimTypes.generated.h"

/** Work and action animations (loops and one-shot pulses). Order is stable: replicated as uint8. */
UENUM()
enum class EGamerAnimAction : uint8
{
	None,
	Chop,     // axe at a tree
	Mine,     // pickaxe at a rock
	Hammer,   // hammer at a building plan
	Gather,   // picking up, gathering plants by hand
	Craft,    // crafting by hand (kneeling)
	Plant,    // sowing
	Harvest,  // field harvest
	Water,    // watering
	Consume,  // eating, drinking
	GatherLow,    // crouching and grabbing from the ground (hand-gathered things, picking up)
	CraftStation, // standing work with the hands in front (hearth, oven, kettle, feast table)
	CraftBench,   // standing, hammering/shaping at table height (workbench)
	CraftLow,     // crouched at a low place (camp fire)
	Count UMETA(Hidden)
};

/** A one-shot action that remote machines replay: kind plus a counter that changes on every new action. */
USTRUCT()
struct FGamerActionPulse
{
	GENERATED_BODY()

	UPROPERTY()
	EGamerAnimAction Kind = EGamerAnimAction::None;

	UPROPERTY()
	uint8 Count = 0;

	/** The owning player already played it locally when acting (strikes): its own machine skips the copy. */
	UPROPERTY()
	bool bPredicted = false;
};

/** What the animation needs from its pawn each frame (game thread). */
struct FGamerAnimInputs
{
	float Speed = 0.f;          // horizontal, cm/s
	float VerticalSpeed = 0.f;  // cm/s
	bool bFalling = false;
	bool bSprinting = false;
	EGamerAnimAction Loop = EGamerAnimAction::None;
	FGamerActionPulse Pulse;
};
