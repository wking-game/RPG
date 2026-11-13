#include "GamerBuilding.h"

#include "Gamer.h"
#include "GamerDataSubsystem.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/CollisionProfile.h"
#include "Engine/StaticMesh.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Net/UnrealNetwork.h"

namespace
{
	const TCHAR* const PlanMaterialPath = TEXT("/Game/Gamer/Materials/M_BuildPreview.M_BuildPreview");
	const TCHAR* const ShapeMaterialPath = TEXT("/Engine/BasicShapes/BasicShapeMaterial.BasicShapeMaterial");

	/** Placeholder for pieces without a model yet (work stations): engine shape, size in cm, colour. Pivot bottom centre. */
	struct FPlaceholder
	{
		const TCHAR* Shape;
		FVector SizeCm;
		FLinearColor Color;
	};

	FPlaceholder PlaceholderFor(FName Id)
	{
		const FLinearColor Wood(0.35f, 0.22f, 0.12f);
		const FLinearColor Stone(0.45f, 0.45f, 0.43f);
		if (Id == TEXT("building_workbench")) { return { TEXT("Cube"), FVector(160.f, 80.f, 90.f), Wood }; }
		if (Id == TEXT("building_campfire")) { return { TEXT("Cylinder"), FVector(120.f, 120.f, 30.f), Stone }; }
		if (Id == TEXT("building_bed")) { return { TEXT("Cube"), FVector(200.f, 100.f, 35.f), FLinearColor(0.75f, 0.65f, 0.35f) }; }
		if (Id == TEXT("building_storage")) { return { TEXT("Cube"), FVector(600.f, 300.f, 300.f), Wood }; }
		if (Id == TEXT("building_woodcutter")) { return { TEXT("Cube"), FVector(250.f, 120.f, 80.f), Wood }; }
		if (Id == TEXT("building_stonecutter")) { return { TEXT("Cube"), FVector(200.f, 120.f, 80.f), Stone }; }
		return { TEXT("Cube"), FVector(100.f, 100.f, 100.f), FLinearColor(0.6f, 0.6f, 0.6f) };
	}
}

AGamerBuilding::AGamerBuilding()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
	SetReplicatingMovement(false);
	// Buildings are big and do not move: keep them relevant further away than characters (400 m).
	SetNetCullDistanceSquared(FMath::Square(40000.f));

	// Root = the piece's pivot (as in Assets/manifest.json); the mesh sits on it (placeholders are lifted).
	SetRootComponent(CreateDefaultSubobject<USceneComponent>(TEXT("Root")));
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(GetRootComponent());
}

void AGamerBuilding::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AGamerBuilding, BuildableId);
	DOREPLIFETIME(AGamerBuilding, Progress);
	DOREPLIFETIME(AGamerBuilding, Spent);
}

void AGamerBuilding::BeginPlay()
{
	Super::BeginPlay();
	ApplyLook();
}

void AGamerBuilding::InitBuilding(FName InBuildableId, float InProgress)
{
	BuildableId = InBuildableId;
	Progress = FMath::Clamp(InProgress, 0.f, 1.f);
	ApplyLook();
}

void AGamerBuilding::InitPreview(FName InBuildableId)
{
	bIsPreview = true;
	SetReplicates(false);
	BuildableId = InBuildableId;
	Progress = 0.f;
	ApplyLook();
}

void AGamerBuilding::SetPreviewValid(bool bValid)
{
	if (bPreviewValid != bValid)
	{
		bPreviewValid = bValid;
		ApplyLook();
	}
}

const FGamerBuildableDef* AGamerBuilding::GetDef() const
{
	const UGamerDataSubsystem* Data = UGamerDataSubsystem::Get(this);
	return Data ? Data->FindBuildable(BuildableId) : nullptr;
}

bool AGamerBuilding::AddProgress(float Amount)
{
	if (!HasAuthority() || IsFinished())
	{
		return false;
	}
	Progress = FMath::Min(1.f, Progress + FMath::Max(0.f, Amount));
	if (Progress >= 0.999f)
	{
		Progress = 1.f;
	}
	ApplyLook();
	ForceNetUpdate();
	return IsFinished();
}

void AGamerBuilding::SetSpent(const TArray<FGamerItemAmount>& InSpent)
{
	Spent = InSpent;
	ForceNetUpdate();
}

void AGamerBuilding::OnRep_State()
{
	ApplyLook();
}

EGamerBuildingLook AGamerBuilding::CurrentLook() const
{
	if (bIsPreview)
	{
		return bPreviewValid ? EGamerBuildingLook::PreviewValid : EGamerBuildingLook::PreviewInvalid;
	}
	return IsFinished() ? EGamerBuildingLook::Finished : EGamerBuildingLook::Plan;
}

void AGamerBuilding::ApplyLook()
{
	const FGamerBuildableDef* Def = GetDef();
	if (!Def || !Mesh)
	{
		return;
	}

	// Model: from the data, or an engine shape as placeholder.
	bool bPlaceholder = false;
	FPlaceholder Placeholder = PlaceholderFor(BuildableId);
	if (AppliedId != BuildableId)
	{
		AppliedId = BuildableId;
		UStaticMesh* StaticMesh = Def->Mesh.IsValid() ? Cast<UStaticMesh>(Def->Mesh.TryLoad()) : nullptr;
		if (StaticMesh)
		{
			Mesh->SetStaticMesh(StaticMesh);
			Mesh->SetRelativeScale3D(FVector::OneVector);
			Mesh->SetRelativeLocation(FVector::ZeroVector);
		}
		else
		{
			if (Def->Mesh.IsValid())
			{
				UE_LOG(LogGamer, Warning, TEXT("Building %s: model %s not found, using a placeholder"), *BuildableId.ToString(), *Def->Mesh.ToString());
			}
			StaticMesh = LoadObject<UStaticMesh>(nullptr, *FString::Printf(TEXT("/Engine/BasicShapes/%s.%s"), Placeholder.Shape, Placeholder.Shape));
			Mesh->SetStaticMesh(StaticMesh);
			// Engine shapes are 100 cm and centred: scale to size and lift so the pivot is the bottom centre.
			Mesh->SetRelativeScale3D(Placeholder.SizeCm / 100.f);
			Mesh->SetRelativeLocation(FVector(0.f, 0.f, Placeholder.SizeCm.Z * 0.5f));
		}
	}
	bPlaceholder = !Def->Mesh.IsValid() || Mesh->GetStaticMesh() == nullptr
		|| Mesh->GetStaticMesh()->GetPathName().StartsWith(TEXT("/Engine/BasicShapes/"));

	const EGamerBuildingLook Look = CurrentLook();
	if (Look == EGamerBuildingLook::Finished)
	{
		Mesh->EmptyOverrideMaterials();
		if (bPlaceholder)
		{
			if (UMaterialInterface* Base = LoadObject<UMaterialInterface>(nullptr, ShapeMaterialPath))
			{
				UMaterialInstanceDynamic* Shape = UMaterialInstanceDynamic::Create(Base, this);
				Shape->SetVectorParameterValue(TEXT("Color"), Placeholder.Color);
				Mesh->SetMaterial(0, Shape);
			}
		}
		Mesh->SetCollisionProfileName(UCollisionProfile::BlockAll_ProfileName);
	}
	else
	{
		if (!PlanMaterial)
		{
			UMaterialInterface* Base = LoadObject<UMaterialInterface>(nullptr, PlanMaterialPath);
			if (!Base)
			{
				Base = LoadObject<UMaterialInterface>(nullptr, ShapeMaterialPath);
			}
			PlanMaterial = Base ? UMaterialInstanceDynamic::Create(Base, this) : nullptr;
		}
		if (PlanMaterial)
		{
			// No red anywhere (readability decision 2026-11-13): invalid previews are orange.
			const FLinearColor Color = Look == EGamerBuildingLook::Plan ? FLinearColor(0.35f, 0.75f, 1.f)
				: Look == EGamerBuildingLook::PreviewValid ? FLinearColor(0.4f, 1.f, 0.5f) : FLinearColor(1.f, 0.55f, 0.1f);
			PlanMaterial->SetVectorParameterValue(TEXT("Color"), Color);
			PlanMaterial->SetScalarParameterValue(TEXT("Opacity"), Look == EGamerBuildingLook::Plan ? 0.35f : 0.45f);
			for (int32 Index = 0; Index < Mesh->GetNumMaterials(); ++Index)
			{
				Mesh->SetMaterial(Index, PlanMaterial);
			}
		}
		if (Look == EGamerBuildingLook::Plan)
		{
			// Players walk through plans, but aiming (visibility traces) hits them so they can be hammered.
			Mesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
			Mesh->SetCollisionResponseToAllChannels(ECR_Ignore);
			Mesh->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
		}
		else
		{
			Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}
	}

	if (bAppliedOnce && bAppliedFinished != IsFinished() && !bIsPreview)
	{
		UE_LOG(LogGamer, Log, TEXT("Building %s (%s) is now %s%s"), *GetName(), *BuildableId.ToString(),
			IsFinished() ? TEXT("finished") : TEXT("a plan"), HasAuthority() ? TEXT("") : TEXT(" (replicated)"));
	}
	bAppliedFinished = IsFinished();
	bAppliedOnce = true;
}
