#pragma once

#include "CoreMinimal.h"
#include "GamerGameState.h"

struct FGamerBuildableDef;
class UWorld;

/** Where a piece would go, and whether it may go there. */
struct FGamerPlacement
{
	FTransform Transform;
	bool bValid = false;
	bool bSnapped = false;
	/** German reason for the HUD when not valid ("Braucht ein Fundament in der Nähe"). */
	FString Problem;
};

/**
 * Snapping rules of the wood kit (300 cm grid, walls 300 cm high, roof 45 degrees).
 * Pivots as in Assets/manifest.json: foundation/floor = top centre, wall/gable = bottom centre on the
 * grid line, pillar = bottom centre, beam = centre of its length, roof = eave bottom centre (rises to +Y in Blender = -Y in Unreal),
 * ridge = ridge line, door = hinge edge (door leaf towards +X).
 * A "cell" is a foundation or floor; walls, roofs etc. snap to the edges and corners of cells.
 * The same functions run in the client preview, on the server (check of a client's placement) and in tests.
 */
namespace GamerBuildSnap
{
	constexpr float Grid = 300.f;
	constexpr float WallHeight = 300.f;
	constexpr float RoofRise = 150.f;
	constexpr float BeamHalfHeight = 12.5f;
	constexpr float DoorHingeOffset = 63.f;
	constexpr float FoundationLift = 20.f;
	constexpr float SnapRadius = 250.f;

	/** Two pieces of the same slot class may not share a spot (wall and door wall share "wall"). */
	GAMER_API FName SlotClass(const FGamerBuildableDef& Def);

	/** True for pieces that are placed freely on the ground (work stations, beds ...). */
	GAMER_API bool IsFreePiece(const FGamerBuildableDef& Def);

	/**
	 * Where would Def go? AimPoint: what the camera looks at (bAimHit: the trace hit something).
	 * ViewYaw: camera yaw in degrees. RotationSteps: number of "rotate" presses (45 degree steps when free, base yaw in 15 degree steps,
	 * 90 on the world grid; on snapped pieces odd values flip the side).
	 * AimNormal: surface normal at the aim point; free pieces need a roughly level surface.
	 */
	GAMER_API FGamerPlacement FindPlacement(UWorld* World, const FGamerBuildableDef& Def, const FVector& AimPoint, bool bAimHit,
		float ViewYaw, int32 RotationSteps, EGamerPlacement Mode, const FVector& AimNormal = FVector::UpVector);

	/** Server: may Def be placed exactly at Transform (sent by a client)? Returns an empty string if yes. */
	GAMER_API FString CheckTransform(UWorld* World, const FGamerBuildableDef& Def, const FTransform& Transform, EGamerPlacement Mode);

	/**
	 * Freely placed pieces (stations, fields, a first foundation): their footprint may not cut into another building or plan
	 * (no stations on roofs or inside walls). The box starts 10 cm above the pivot, so standing on a floor or next to a wall is fine.
	 * Returns an empty string if the spot is free.
	 */
	GAMER_API FString CheckOverlap(UWorld* World, const FGamerBuildableDef& Def, const FTransform& Transform);
}
