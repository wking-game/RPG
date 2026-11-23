#include "GamerBuilding.h"

#include "Gamer.h"
#include "GamerDataSubsystem.h"
#include "GamerFarmBuilding.h"
#include "GamerTavern.h"
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
		// Fields are drawn: 1 m square here, the actor scale gives the real size.
		if (Id == TEXT("building_field")) { return { TEXT("Cube"), FVector(100.f, 100.f, 18.f), FLinearColor(0.20f, 0.13f, 0.07f) }; }
		if (Id == TEXT("building_farmstead")) { return { TEXT("Cube"), FVector(600.f, 400.f, 280.f), FLinearColor(0.42f, 0.30f, 0.16f) }; }
		if (Id == TEXT("building_composter")) { return { TEXT("Cube"), FVector(120.f, 120.f, 90.f), Wood }; }
		if (Id == TEXT("building_rain_catcher")) { return { TEXT("Cylinder"), FVector(90.f, 90.f, 70.f), Stone }; }
		// Tavern and cooking places (Taverne-Session 2026-11-14), placeholders until the "Gelebter Norden" models.
		if (Id == TEXT("building_tavern")) { return { TEXT("Cube"), FVector(800.f, 500.f, 320.f), FLinearColor(0.40f, 0.26f, 0.14f) }; }
		if (Id == TEXT("building_hearth")) { return { TEXT("Cube"), FVector(140.f, 100.f, 60.f), Stone }; }
		if (Id == TEXT("building_clay_oven")) { return { TEXT("Sphere"), FVector(160.f, 140.f, 120.f), FLinearColor(0.55f, 0.38f, 0.25f) }; }
		if (Id == TEXT("building_brew_kettle")) { return { TEXT("Cylinder"), FVector(110.f, 110.f, 100.f), FLinearColor(0.22f, 0.20f, 0.18f) }; }
		if (Id == TEXT("building_feast_table")) { return { TEXT("Cube"), FVector(400.f, 120.f, 80.f), Wood }; }
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
	DOREPLIFETIME(AGamerBuilding, Level);
}

int32 AGamerBuilding::UnitsFor(const FGamerBuildableDef& Def, const FVector& Scale)
{
	if (!Def.IsAreaPiece())
	{
		return 1;
	}
	const float AreaM2 = Scale.X * Scale.Y; // scale 1 = 1 m per side
	return FMath::Max(1, FMath::CeilToInt(AreaM2 / FMath::Max(0.1f, Def.Farm.UnitM2) - 0.01f));
}

TArray<FGamerItemAmount> AGamerBuilding::CostFor(const FGamerBuildableDef& Def, const FVector& Scale)
{
	TArray<FGamerItemAmount> Cost = Def.Cost;
	const int32 Units = UnitsFor(Def, Scale);
	for (FGamerItemAmount& Amount : Cost)
	{
		Amount.Count *= Units;
	}
	return Cost;
}

FBox AGamerBuilding::LocalBounds(const FGamerBuildableDef& Def)
{
	if (Def.Mesh.IsValid())
	{
		if (const UStaticMesh* StaticMesh = Cast<UStaticMesh>(Def.Mesh.TryLoad()))
		{
			return StaticMesh->GetBoundingBox();
		}
	}
	const FVector Size = PlaceholderFor(Def.Id).SizeCm;
	return FBox(FVector(-Size.X * 0.5f, -Size.Y * 0.5f, 0.f), FVector(Size.X * 0.5f, Size.Y * 0.5f, Size.Z));
}

FVector AGamerBuilding::ClampScale(const FGamerBuildableDef& Def, const FVector& Scale)
{
	if (!Def.IsAreaPiece())
	{
		return FVector::OneVector;
	}
	const float Min = Def.Farm.MinSizeCm / 100.f;
	const float Max = Def.Farm.MaxSizeCm / 100.f;
	// Not drawn (scale 1, e.g. tests or old saves): a 3 x 1.5 m bed.
	if (Scale.Equals(FVector::OneVector))
	{
		return FVector(FMath::Clamp(3.f, Min, Max), FMath::Clamp(1.5f, Min, Max), 1.f);
	}
	return FVector(FMath::Clamp(FMath::Abs(Scale.X), Min, Max), FMath::Clamp(FMath::Abs(Scale.Y), Min, Max), 1.f);
}

int32 AGamerBuilding::GetWorkers() const
{
	const FGamerBuildableDef* Def = GetDef();
	return Def ? Def->WorkersAt(Level) : 0;
}

const FGamerUpgradeDef* AGamerBuilding::GetNextUpgrade() const
{
	const FGamerBuildableDef* Def = GetDef();
	return Def && Def->Upgrades.IsValidIndex(Level - 1) ? &Def->Upgrades[Level - 1] : nullptr;
}

void AGamerBuilding::Upgrade()
{
	if (HasAuthority() && GetNextUpgrade())
	{
		++Level;
		ForceNetUpdate();
		UE_LOG(LogGamer, Log, TEXT("Building %s (%s) upgraded to level %d, %d work places"), *GetName(), *BuildableId.ToString(), Level, GetWorkers());
	}
}

void AGamerBuilding::BeginPlay()
{
	Super::BeginPlay();
	ApplyLook();
}

UClass* AGamerBuilding::ClassFor(const UObject* WorldContext, FName InBuildableId)
{
	const UGamerDataSubsystem* Data = UGamerDataSubsystem::Get(WorldContext);
	const FGamerBuildableDef* Def = Data ? Data->FindBuildable(InBuildableId) : nullptr;
	if (Def && Def->Tavern.IsTavern())
	{
		return AGamerTavern::StaticClass();
	}
	return Def && Def->Category == EGamerBuildCategory::Farm ? AGamerFarmBuilding::StaticClass() : AGamerBuilding::StaticClass();
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
