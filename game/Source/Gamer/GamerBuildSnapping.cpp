#include "GamerBuildSnapping.h"

#include "GamerBuilding.h"
#include "GamerDataTypes.h"
#include "EngineUtils.h"
#include "Engine/World.h"

namespace
{
	using namespace GamerBuildSnap;

	struct FSnapSocket
	{
		FVector Location;
		float Yaw = 0.f;
	};

	bool IsCell(const FGamerBuildableDef* Def)
	{
		return Def && (Def->Snap == TEXT("foundation") || Def->Snap == TEXT("floor"));
	}

	float NormalizeYaw(float Yaw)
	{
		return FRotator::NormalizeAxis(Yaw);
	}

	/** Sockets the existing building B offers for a new piece with snap type Snap. */
	void AddSockets(TArray<FSnapSocket>& Out, const AGamerBuilding& B, const FGamerBuildableDef& BDef, FName Snap, int32 Steps)
	{
		const FTransform T = B.GetActorTransform();
		const FVector L = T.GetLocation();
		const float Yaw = T.Rotator().Yaw;
		const FVector X = T.GetRotation().GetForwardVector();
		const FVector Y = T.GetRotation().GetRightVector();
		const FVector Up = FVector::UpVector;
		const bool bFlip = (Steps % 2) != 0;

		auto AddEdges = [&](float Height, bool bRoof)
		{
			const FVector Dirs[] = { X, -X, Y, -Y };
			for (const FVector& D : Dirs)
			{
				const FVector Location = L + Up * Height + D * (Grid * 0.5f);
				float SocketYaw;
				if (bRoof)
				{
					// The panel rises towards +Y in Blender, which is -Y in Unreal (the FBX import mirrors Y):
					// so local -Y must point into the cell, i.e. local +Y points out over the eave.
					SocketYaw = FRotationMatrix::MakeFromYZ(D, Up).Rotator().Yaw;
				}
				else
				{
					// Walls, gables, beams run along the edge; R flips them to the other side.
					SocketYaw = FRotationMatrix::MakeFromXZ(FVector::CrossProduct(Up, D), Up).Rotator().Yaw + (bFlip ? 180.f : 0.f);
				}
				Out.Add({ Location, NormalizeYaw(SocketYaw) });
			}
		};

		if (Snap == TEXT("foundation"))
		{
			if (BDef.Snap == TEXT("foundation"))
			{
				for (const FVector& D : { X, -X, Y, -Y })
				{
					Out.Add({ L + D * Grid, Yaw });
				}
			}
		}
		else if (Snap == TEXT("floor"))
		{
			if (IsCell(&BDef))
			{
				Out.Add({ L + Up * WallHeight, Yaw }); // upper storey, on top of the walls
			}
			if (BDef.Snap == TEXT("floor"))
			{
				for (const FVector& D : { X, -X, Y, -Y })
				{
					Out.Add({ L + D * Grid, Yaw });
				}
			}
		}
		else if (IsCell(&BDef))
		{
			if (Snap == TEXT("wall") || Snap == TEXT("wall_door"))
			{
				AddEdges(0.f, false);
			}
			else if (Snap == TEXT("gable"))
			{
				AddEdges(WallHeight, false);
			}
			else if (Snap == TEXT("beam"))
			{
				AddEdges(WallHeight - BeamHalfHeight, false);
			}
			else if (Snap == TEXT("roof"))
			{
				AddEdges(WallHeight, true);
			}
			else if (Snap == TEXT("ridge"))
			{
				Out.Add({ L + Up * (WallHeight + RoofRise), NormalizeYaw(Yaw + (bFlip ? 90.f : 0.f)) });
			}
			else if (Snap == TEXT("pillar"))
			{
				for (const float SX : { -1.f, 1.f })
				{
					for (const float SY : { -1.f, 1.f })
					{
						Out.Add({ L + X * (SX * Grid * 0.5f) + Y * (SY * Grid * 0.5f), Yaw });
					}
				}
			}
		}
		else if (Snap == TEXT("door") && BDef.Snap == TEXT("wall_door"))
		{
			// Hinge at one side of the 130 cm opening; R hangs the door on the other side.
			Out.Add(bFlip ? FSnapSocket{ L + X * DoorHingeOffset, NormalizeYaw(Yaw + 180.f) } : FSnapSocket{ L - X * DoorHingeOffset, Yaw });
		}
	}

	bool IsOccupied(UWorld* World, FName Slot, const FVector& Location, float Tolerance)
	{
		for (TActorIterator<AGamerBuilding> It(World); It; ++It)
		{
			const FGamerBuildableDef* Def = It->IsPreview() ? nullptr : It->GetDef();
			if (Def && SlotClass(*Def) == Slot && FVector::Dist(It->GetActorLocation(), Location) < Tolerance)
			{
				return true;
			}
		}
		return false;
	}

	/** Free pieces and free foundations must keep some distance to their own kind. */
	FString CheckFreeSpacing(UWorld* World, const FGamerBuildableDef& Def, const FVector& Location)
	{
		const bool bFoundation = Def.Snap == TEXT("foundation");
		for (TActorIterator<AGamerBuilding> It(World); It; ++It)
		{
			const FGamerBuildableDef* Other = It->IsPreview() ? nullptr : It->GetDef();
			if (!Other)
			{
				continue;
			}
			const FVector Delta = It->GetActorLocation() - Location;
			if (bFoundation && IsCell(Other) && FVector(Delta.X, Delta.Y, 0.f).Size() < Grid * 0.95f && FMath::Abs(Delta.Z) < 100.f)
			{
				return TEXT("Zu nah an einem anderen Fundament");
			}
			if (!bFoundation && IsFreePiece(*Other) && Delta.Size() < 150.f)
			{
				return FString::Printf(TEXT("Zu nah an: %s"), *Other->Name);
			}
		}
		return FString();
	}

	bool HasGroundBelow(UWorld* World, const FVector& Location, float MaxDepth)
	{
		FHitResult Hit;
		FCollisionQueryParams Params(SCENE_QUERY_STAT(GamerBuildGround), false);
		return World->LineTraceSingleByChannel(Hit, Location + FVector(0.f, 0.f, 50.f), Location - FVector(0.f, 0.f, MaxDepth), ECC_Visibility, Params);
	}

	float FreeYaw(float ViewYaw, int32 Steps, EGamerPlacement Mode)
	{
		const float Step = Mode == EGamerPlacement::WorldGrid ? 90.f : 15.f;
		return NormalizeYaw(FMath::RoundToFloat(ViewYaw / Step) * Step + Steps * Step);
	}

	FString NoSocketProblem(FName Snap)
	{
		if (Snap == TEXT("door"))
		{
			return TEXT("Braucht eine Wand mit Türöffnung");
		}
		if (Snap == TEXT("stairs"))
		{
			return TEXT("Treppen gibt es noch nicht");
		}
		return TEXT("Braucht ein Fundament oder einen Boden in der Nähe");
	}
}

FName GamerBuildSnap::SlotClass(const FGamerBuildableDef& Def)
{
	if (Def.Category != EGamerBuildCategory::Structure)
	{
		return TEXT("free");
	}
	if (Def.Snap == TEXT("foundation") || Def.Snap == TEXT("floor"))
	{
		return TEXT("cell");
	}
	if (Def.Snap == TEXT("wall") || Def.Snap == TEXT("wall_door"))
	{
		return TEXT("wall");
	}
	if (Def.Snap == TEXT("roof") || Def.Snap == TEXT("gable"))
	{
		return TEXT("roofedge"); // a roof panel and a gable never share an edge
	}
	return Def.Snap;
}

bool GamerBuildSnap::IsFreePiece(const FGamerBuildableDef& Def)
{
	return Def.Category != EGamerBuildCategory::Structure;
}

FGamerPlacement GamerBuildSnap::FindPlacement(UWorld* World, const FGamerBuildableDef& Def, const FVector& AimPoint, bool bAimHit,
	float ViewYaw, int32 RotationSteps, EGamerPlacement Mode, const FVector& AimNormal)
{
	FGamerPlacement Result;
	if (!World)
	{
		return Result;
	}

	// 1) Snap to existing pieces (also to unfinished plans, so a whole house can be planned).
	if (!IsFreePiece(Def))
	{
		const FName Slot = SlotClass(Def);
		bool bAnyOccupied = false;
		float BestDistance = SnapRadius;
		for (TActorIterator<AGamerBuilding> It(World); It; ++It)
		{
			const FGamerBuildableDef* BDef = It->IsPreview() ? nullptr : It->GetDef();
			if (!BDef || FVector::Dist(It->GetActorLocation(), AimPoint) > 1500.f)
			{
				continue;
			}
			TArray<FSnapSocket> Sockets;
			AddSockets(Sockets, **It, *BDef, Def.Snap, RotationSteps);
			for (const FSnapSocket& Socket : Sockets)
			{
				const float Distance = FVector::Dist(Socket.Location, AimPoint);
				if (Distance >= BestDistance)
				{
					continue;
				}
				if (IsOccupied(World, Slot, Socket.Location, 30.f))
				{
					bAnyOccupied = true;
					continue;
				}
				BestDistance = Distance;
				Result.Transform = FTransform(FRotator(0.f, Socket.Yaw, 0.f), Socket.Location);
				Result.bSnapped = true;
			}
		}
		if (Result.bSnapped)
		{
			Result.bValid = true;
			return Result;
		}
		if (Def.Snap != TEXT("foundation"))
		{
			Result.Problem = bAnyOccupied ? TEXT("Platz belegt") : NoSocketProblem(Def.Snap);
			Result.Transform = FTransform(FRotator(0.f, FreeYaw(ViewYaw, RotationSteps, Mode), 0.f), AimPoint);
			return Result;
		}
	}

	// 2) Free placement: first foundation of a building, or a work station.
	FVector Location = AimPoint;
	if (Def.Snap == TEXT("foundation"))
	{
		Location.Z += FoundationLift;
		if (Mode == EGamerPlacement::WorldGrid)
		{
			Location.X = FMath::RoundToFloat(Location.X / Grid) * Grid;
			Location.Y = FMath::RoundToFloat(Location.Y / Grid) * Grid;
		}
	}
	Result.Transform = FTransform(FRotator(0.f, FreeYaw(ViewYaw, RotationSteps, Mode), 0.f), Location);
	if (!bAimHit)
	{
		Result.Problem = TEXT("Kein Boden in Reichweite");
		return Result;
	}
	if (AimNormal.Z < 0.7f)
	{
		Result.Problem = TEXT("Nur auf ebenem Boden");
		return Result;
	}
	Result.Problem = CheckFreeSpacing(World, Def, Location);
	Result.bValid = Result.Problem.IsEmpty();
	return Result;
}

FString GamerBuildSnap::CheckTransform(UWorld* World, const FGamerBuildableDef& Def, const FTransform& Transform, EGamerPlacement Mode)
{
	if (!World)
	{
		return TEXT("Keine Welt");
	}
	const FVector Location = Transform.GetLocation();
	const float Yaw = Transform.Rotator().Yaw;

	if (!IsFreePiece(Def))
	{
		// Must match a free socket exactly (both flip variants are tried).
		const FName Slot = SlotClass(Def);
		for (TActorIterator<AGamerBuilding> It(World); It; ++It)
		{
			const FGamerBuildableDef* BDef = It->IsPreview() ? nullptr : It->GetDef();
			if (!BDef || FVector::Dist(It->GetActorLocation(), Location) > 1500.f)
			{
				continue;
			}
			for (const int32 Steps : { 0, 1 })
			{
				TArray<FSnapSocket> Sockets;
				AddSockets(Sockets, **It, *BDef, Def.Snap, Steps);
				for (const FSnapSocket& Socket : Sockets)
				{
					if (FVector::Dist(Socket.Location, Location) < 5.f && FMath::Abs(FRotator::NormalizeAxis(Socket.Yaw - Yaw)) < 1.f)
					{
						return IsOccupied(World, Slot, Location, 30.f) ? TEXT("Platz belegt") : FString();
					}
				}
			}
		}
		if (Def.Snap != TEXT("foundation"))
		{
			return NoSocketProblem(Def.Snap);
		}
	}

	// Free foundation or work station: ground below, distance to others, grid rules.
	if (Mode == EGamerPlacement::WorldGrid)
	{
		if (FMath::Abs(FRotator::NormalizeAxis(Yaw - FMath::RoundToFloat(Yaw / 90.f) * 90.f)) > 1.f)
		{
			return TEXT("Auf dem Raster nur in 90-Grad-Schritten");
		}
		auto OffGrid = [](double Value)
		{
			const double Rest = FMath::Abs(FMath::Fmod(Value, static_cast<double>(Grid)));
			return Rest > 1.0 && Rest < Grid - 1.0;
		};
		if (Def.Snap == TEXT("foundation") && (OffGrid(Location.X) || OffGrid(Location.Y)))
		{
			return TEXT("Nicht auf dem Raster");
		}
	}
	const float Depth = Def.Snap == TEXT("foundation") ? FoundationLift + 120.f : 100.f;
	if (!HasGroundBelow(World, Location, Depth))
	{
		return TEXT("Kein Boden darunter");
	}
	return CheckFreeSpacing(World, Def, Location);
}
