#include "GamerVillager.h"
#include "GamerAnimInstance.h"
#include "GamerFigures.h"
#include "Engine/SkeletalMesh.h"
#include "GamerTavern.h"

#include "Gamer.h"
#include "GamerBuilding.h"
#include "GamerDataSubsystem.h"
#include "GamerGameState.h"
#include "GamerPlayerController.h"
#include "GamerFarmBuilding.h"
#include "GamerResourceNode.h"
#include "GamerSettlement.h"
#include "Camera/PlayerCameraManager.h"
#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/TextRenderComponent.h"
#include "EngineUtils.h"
#include "Engine/StaticMesh.h"
#include "Engine/World.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "NavigationData.h"
#include "NavigationInvokerComponent.h"
#include "NavigationSystem.h"
#include "Net/UnrealNetwork.h"
#include "UObject/ConstructorHelpers.h"

namespace
{
	constexpr float StrikeInterval = 0.8f;
	constexpr float StorageReach = 380.f;

	// Navigation: mesh tiles within this radius of each villager are built, beyond the second one removed (cm).
	constexpr float NavBuildRadius = 3000.f;
	constexpr float NavRemoveRadius = 5000.f;
	// A path point counts as reached this close (2D); paths are recomputed this often (new buildings, new tiles).
	constexpr float WaypointReach = 60.f;
	constexpr float RepathSeconds = 3.f;
	constexpr float RepathSecondsWithoutPath = 1.f;

	FLinearColor JobColor(FName JobId)
	{
		if (JobId == TEXT("job_woodcutter")) { return FLinearColor(0.55f, 0.33f, 0.15f); }
		if (JobId == TEXT("job_stonecutter")) { return FLinearColor(0.50f, 0.52f, 0.58f); }
		if (JobId == TEXT("job_cook")) { return FLinearColor(0.90f, 0.55f, 0.20f); }
		if (JobId == TEXT("job_builder")) { return FLinearColor(0.25f, 0.45f, 0.85f); }
		return FLinearColor(0.45f, 0.62f, 0.35f); // gatherer / unknown
	}

	FString VillagerItemName(const UGamerDataSubsystem* Data, FName Id)
	{
		const FGamerItemDef* Def = Data ? Data->FindItem(Id) : nullptr;
		return Def ? Def->Name : Id.ToString();
	}

	FString VillagerAmountsText(const UGamerDataSubsystem* Data, const TArray<FGamerItemAmount>& Amounts)
	{
		TArray<FString> Parts;
		for (const FGamerItemAmount& Amount : Amounts)
		{
			Parts.Add(FString::Printf(TEXT("%d %s"), Amount.Count, *VillagerItemName(Data, Amount.Item)));
		}
		return FString::Join(Parts, TEXT(", "));
	}

	FString ToolLabel(FName ToolType)
	{
		if (ToolType == TEXT("axe")) { return TEXT("Axt"); }
		if (ToolType == TEXT("pickaxe")) { return TEXT("Spitzhacke"); }
		if (ToolType == TEXT("hammer")) { return TEXT("Hammer"); }
		return ToolType.ToString();
	}
}

AGamerVillager::AGamerVillager()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	SetNetCullDistanceSquared(FMath::Square(30000.f));
	AutoPossessAI = EAutoPossessAI::Disabled;
	AIControllerClass = nullptr;

	GetCapsuleComponent()->InitCapsuleSize(40.f, 92.f);
	UCharacterMovementComponent* Movement = GetCharacterMovement();
	Movement->bRunPhysicsWithNoController = true; // moved by the server's own logic, no AI controller
	Movement->bOrientRotationToMovement = true;
	Movement->RotationRate = FRotator(0.f, 400.f, 0.f);
	Movement->MaxWalkSpeed = WalkSpeed;
	bUseControllerRotationYaw = false;

	// Kept as an empty component (no mesh) so existing references stay valid; the figure is GetMesh().
	Body = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Body"));
	Body->SetupAttachment(GetCapsuleComponent());
	Body->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> MannyMesh(TEXT("/Game/Characters/Mannequins/Meshes/SKM_Manny_Simple.SKM_Manny_Simple"));
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> QuinnMesh(TEXT("/Game/Characters/Mannequins/Meshes/SKM_Quinn_Simple.SKM_Quinn_Simple"));
	BodyA = MannyMesh.Object;
	BodyB = QuinnMesh.Object;
	USkeletalMeshComponent* Figure = GetMesh();
	Figure->SetSkeletalMesh(BodyA);
	Figure->SetRelativeLocationAndRotation(FVector(0.f, 0.f, -92.f), FRotator(0.f, -90.f, 0.f));
	Figure->SetAnimInstanceClass(UGamerAnimInstance::StaticClass());
	Figure->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Figure->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::OnlyTickPoseWhenRendered;
	FaceMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FaceMesh"));
	FaceMesh->SetupAttachment(Figure);
	FaceMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	FaceMesh->SetLeaderPoseComponent(Figure);

	Label = CreateDefaultSubobject<UTextRenderComponent>(TEXT("Label"));
	Label->SetupAttachment(GetCapsuleComponent());
	Label->SetRelativeLocation(FVector(0.f, 0.f, 130.f));
	Label->SetHorizontalAlignment(EHTA_Center);
	Label->SetVerticalAlignment(EVRTA_TextBottom);
	Label->SetWorldSize(22.f);
	Label->SetTextRenderColor(FColor(240, 236, 220));
	Label->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	NavInvoker = CreateDefaultSubobject<UNavigationInvokerComponent>(TEXT("NavInvoker"));
	NavInvoker->SetGenerationRadii(NavBuildRadius, NavRemoveRadius);
}

void AGamerVillager::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AGamerVillager, VillagerName);
	DOREPLIFETIME(AGamerVillager, JobId);
	DOREPLIFETIME(AGamerVillager, Activity);
	DOREPLIFETIME(AGamerVillager, AnimWork);
	DOREPLIFETIME(AGamerVillager, Hunger);
	DOREPLIFETIME(AGamerVillager, Sleep);
	DOREPLIFETIME(AGamerVillager, Mood);
	DOREPLIFETIME(AGamerVillager, bShelter);
	DOREPLIFETIME(AGamerVillager, Bed);
	DOREPLIFETIME(AGamerVillager, HeldTool);
}

void AGamerVillager::InitVillager(const FString& InName, AGamerBuilding* InBed)
{
	VillagerName = InName;
	Bed = InBed;
	const FGamerJobDef* Fallback = FallbackJobDef();
	JobId = Fallback ? Fallback->Id : NAME_None;
	Hunger = 1.f;
	Sleep = 1.f;
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	LastPosition = GetActorLocation();
	SetActivity(TEXT("kommt an"));
	ApplyLook();
	ForceNetUpdate();
}

bool AGamerVillager::SetJob(FName InJobId, FString* OutWhyNot)
{
	const UGamerDataSubsystem* Data = UGamerDataSubsystem::Get(this);
	const FGamerJobDef* Job = Data ? Data->FindJob(InJobId) : nullptr;
	if (!HasAuthority() || InJobId == JobId || !Job)
	{
		return InJobId == JobId;
	}
	// Work places (decision 2026-11-14): stations offer a number of them, upgrades add more.
	if (const AGamerSettlement* Settlement = GetSettlement())
	{
		const int32 Capacity = Settlement->GetJobCapacity(InJobId);
		if (Capacity != INDEX_NONE && Settlement->CountWithJob(InJobId) >= Capacity)
		{
			const FGamerBuildableDef* Station = Data->FindBuildable(Job->Station);
			const FString Why = Capacity == 0
				? FString::Printf(TEXT("%s braucht erst %s."), *Job->Name, Station ? *Station->Name : *Job->Station.ToString())
				: FString::Printf(TEXT("Alle %d Arbeitsplätze als %s sind besetzt – Gebäude ausbauen oder ein weiteres bauen."), Capacity, *Job->Name);
			UE_LOG(LogGamer, Log, TEXT("Villager %s: job %s refused (%d of %d places taken)"), *VillagerName, *InJobId.ToString(),
				Settlement->CountWithJob(InJobId), Capacity);
			if (OutWhyNot)
			{
				*OutWhyNot = Why;
			}
			return false;
		}
	}
	JobId = InJobId;
	UE_LOG(LogGamer, Log, TEXT("Villager %s: job set to %s"), *VillagerName, *JobId.ToString());
	if (Task != ETask::Sleep && Task != ETask::Eat)
	{
		FinishTask(); // start the new job right away (the carried goods stay)
	}
	ApplyLook();
	ForceNetUpdate();
	return true;
}

void AGamerVillager::SetShelter(bool bInShelter)
{
	if (bShelter != bInShelter)
	{
		bShelter = bInShelter;
		ForceNetUpdate();
	}
}

void AGamerVillager::OnRep_Look()
{
	ApplyLook();
}

void AGamerVillager::ApplyLook()
{
	if (GetNetMode() != NM_DedicatedServer)
	{
		USkeletalMeshComponent* Figure = GetMesh();
		const uint32 Hash = GetTypeHash(VillagerName);
		const bool bSlim = (Hash & 1) != 0;
		// Villagers: the seeress (job_seer, docs/14_saga_quests.md) has her own figure; others a villager variant
		// (hair, beard, skin), else the simplified MetaHuman ("*_low", Low pipeline); else the player figure
		// with its finest LODs skipped; else the interim Manny/Quinn (LOD plan, docs/16_metahuman_anmeldung.md).
		GamerFigures::FFigure MetaHuman;
		const bool bLow = (JobId == TEXT("job_seer") && GamerFigures::Find(TEXT("npc_seeress"), MetaHuman))
			|| GamerFigures::FindVariant(bSlim ? TEXT("villager_slim_") : TEXT("villager_strong_"), Hash >> 1, MetaHuman)
			|| GamerFigures::Find(bSlim ? TEXT("slim_low") : TEXT("strong_low"), MetaHuman);
		const bool bMetaHuman = bLow || GamerFigures::Find(bSlim ? TEXT("slim") : TEXT("strong"), MetaHuman);
		USkeletalMesh* Wanted = bMetaHuman ? MetaHuman.Body : (bSlim && BodyB ? BodyB.Get() : BodyA.Get());
		if (Wanted && Figure->GetSkeletalMeshAsset() != Wanted)
		{
			Figure->SetSkeletalMesh(Wanted);
		}
		Figure->SetMinLOD(bMetaHuman && !bLow ? 2 : 0);
		FaceMesh->SetSkeletalMesh(bMetaHuman ? MetaHuman.Face : nullptr);
		FaceMesh->SetLeaderPoseComponent(Figure);
		FaceMesh->SetMinLOD(bMetaHuman && !bLow ? 2 : 0);
		GamerFigures::ApplyGrooms(FaceMesh, GroomComponents, MetaHuman);
		if (MetaHuman.Face && MetaHuman.Face != LoggedFace)
		{
			LoggedFace = MetaHuman.Face;
			UE_LOG(LogGamer, Log, TEXT("Villager %s: figure %s, grooms %d"), *VillagerName, *MetaHuman.Face->GetPathName(), MetaHuman.Grooms.Num());
		}
		// Own Norse clothing, by name: slim figures half in long dresses (half of those with apron and brooches),
		// a third of all wear a cloak; the seeress always a dress with apron and a cloak. Colours: style palette.
		const bool bSeer = JobId == TEXT("job_seer");
		const uint32 Pick = Hash >> 3;
		const TCHAR* Outfit = bSeer ? TEXT("Schuerzenkleid")
			: (bSlim && (Pick & 1)) ? ((Pick & 2) ? TEXT("Schuerzenkleid") : TEXT("Kleid")) : TEXT("Tunika");
		const bool bCloak = bSeer || (Pick >> 2) % 3 == 0;
		GamerFigures::ApplyClothing(Figure, ClothMesh, bMetaHuman, bSlim, Outfit,
			GamerFigures::ClothPalette(int32(Hash >> 6)), GamerFigures::ClothPalette(int32(Hash >> 10) + 3));
		GamerFigures::ApplyClothing(Figure, CloakMesh, bMetaHuman && bCloak, bSlim, TEXT("Umhang"),
			GamerFigures::ClothPalette(int32(Hash >> 13) + 1), GamerFigures::ClothPalette(4));
		// Job colour on the painted panels of the interim figure (not on MetaHumans).
		for (int32 i = 0; !bMetaHuman && i < Figure->GetNumMaterials(); ++i)
		{
			if (UMaterialInstanceDynamic* Material = Figure->CreateAndSetMaterialInstanceDynamic(i))
			{
				Material->SetVectorParameterValue(TEXT("Paint Tint"), JobColor(JobId));
			}
		}
	}
	if (Label)
	{
		const UGamerDataSubsystem* Data = UGamerDataSubsystem::Get(this);
		const FGamerJobDef* Job = Data ? Data->FindJob(JobId) : nullptr;
		Label->SetText(FText::FromString(FString::Printf(TEXT("%s (%s)\n%s"), *VillagerName, Job ? *Job->Name : TEXT("-"), *Activity)));
	}
}

void AGamerVillager::SetActivity(const FString& Text)
{
	if (Activity != Text)
	{
		Activity = Text;
		if (HasAuthority())
		{
			UE_LOG(LogGamer, Log, TEXT("Villager %s: %s"), *VillagerName, *Text);
		}
		ApplyLook();
	}
}

void AGamerVillager::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	if (HasAuthority())
	{
		ServerTick(DeltaSeconds);
		// Work animation: only while standing at the work (not on the way there).
		EGamerAnimAction Anim = EGamerAnimAction::None;
		if (GetVelocity().Size2D() < 30.f)
		{
			switch (Task)
			{
			case ETask::Harvest:
				Anim = HeldTool == TEXT("axe") ? EGamerAnimAction::Chop
					: HeldTool == TEXT("pickaxe") ? EGamerAnimAction::Mine : EGamerAnimAction::GatherLow;
				break;
			case ETask::Build: Anim = EGamerAnimAction::Hammer; break;
			case ETask::Craft: Anim = EGamerAnimAction::CraftStation; break; // villagers craft at stations
			case ETask::FarmWork: Anim = EGamerAnimAction::Plant; break;
			case ETask::Eat: Anim = EGamerAnimAction::Consume; break;
			default: break;
			}
		}
		AnimWork = Anim;
	}
	// Name and activity face the local camera (not on a dedicated server).
	if (Label && GetNetMode() != NM_DedicatedServer)
	{
		const APlayerController* PC = GetWorld()->GetFirstPlayerController();
		if (PC && PC->PlayerCameraManager)
		{
			const FVector ToCamera = PC->PlayerCameraManager->GetCameraLocation() - Label->GetComponentLocation();
			Label->SetWorldRotation(FRotator(0.f, ToCamera.Rotation().Yaw, 0.f));
			// The bird's-eye view draws its own name tags (readable from above).
			const AGamerPlayerController* GamerPC = Cast<AGamerPlayerController>(PC);
			Label->SetVisibility(ToCamera.Size() < 4000.f && !(GamerPC && GamerPC->IsInBirdView()));
		}
	}
}

void AGamerVillager::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (AGamerSettlement* Settlement = GetSettlement())
	{
		Settlement->ReleaseAll(this);
	}
	Super::EndPlay(EndPlayReason);
}

// --- Helpers -------------------------------------------------------------------------------------

AGamerSettlement* AGamerVillager::GetSettlement() const
{
	return AGamerSettlement::Get(this);
}

const FGamerJobDef* AGamerVillager::CurrentJobDef() const
{
	const UGamerDataSubsystem* Data = UGamerDataSubsystem::Get(this);
	return Data ? Data->FindJob(JobId) : nullptr;
}

const FGamerJobDef* AGamerVillager::FallbackJobDef() const
{
	const UGamerDataSubsystem* Data = UGamerDataSubsystem::Get(this);
	if (Data)
	{
		for (const TPair<FName, FGamerJobDef>& Pair : Data->GetData().Jobs)
		{
			if (Pair.Value.bFallback && !Pair.Value.bDeprecated)
			{
				return &Pair.Value;
			}
		}
	}
	return nullptr;
}

const FGamerNeedDef* AGamerVillager::NeedOfKind(EGamerNeedKind Kind) const
{
	const UGamerDataSubsystem* Data = UGamerDataSubsystem::Get(this);
	if (Data)
	{
		for (const TPair<FName, FGamerNeedDef>& Pair : Data->GetData().Needs)
		{
			if (Pair.Value.Kind == Kind && !Pair.Value.bDeprecated)
			{
				return &Pair.Value;
			}
		}
	}
	return nullptr;
}

float AGamerVillager::WorkSpeed() const
{
	float Speed = 1.f;
	const UGamerDataSubsystem* Data = UGamerDataSubsystem::Get(this);
	if (const FGamerItemDef* Tool = Data && !HeldTool.IsNone() ? Data->FindItem(HeldTool) : nullptr)
	{
		Speed *= FMath::Max(0.1f, Tool->Tool.WorkSpeed);
	}
	if (const FGamerNeedDef* Food = NeedOfKind(EGamerNeedKind::Food); Food && Hunger < Food->Threshold)
	{
		Speed *= Food->WorkSpeedWhenUnmet;
	}
	if (const FGamerNeedDef* Rest = NeedOfKind(EGamerNeedKind::Sleep); Rest && Sleep < Rest->Threshold)
	{
		Speed *= Rest->WorkSpeedWhenUnmet;
	}
	if (const FGamerNeedDef* Shelter = NeedOfKind(EGamerNeedKind::Shelter); Shelter && !bShelter)
	{
		Speed *= Shelter->WorkSpeedWhenUnmet;
	}
	return Speed;
}

AActor* AGamerVillager::FindStation(FName StationId) const
{
	AActor* Best = nullptr;
	float BestDistance = TNumericLimits<float>::Max();
	for (TActorIterator<AGamerBuilding> It(GetWorld()); It; ++It)
	{
		if (!It->IsPreview() && It->IsFinished() && It->GetBuildableId() == StationId)
		{
			const float Distance = FVector::Dist(It->GetActorLocation(), GetActorLocation());
			if (Distance < BestDistance)
			{
				BestDistance = Distance;
				Best = *It;
			}
		}
	}
	return Best;
}

AGamerResourceNode* AGamerVillager::FindNode(const FGamerJobDef& Job, const FVector& Center, int32 ToolTier) const
{
	const AGamerSettlement* Settlement = GetSettlement();
	const double Now = GetWorld()->GetTimeSeconds();
	AGamerResourceNode* Best = nullptr;
	float BestDistance = TNumericLimits<float>::Max();
	for (TActorIterator<AGamerResourceNode> It(GetWorld()); It; ++It)
	{
		const FGamerResourceNodeDef* Def = It->GetDef();
		if (!Def || It->IsDepleted() || FVector::Dist2D(It->GetActorLocation(), Center) > Job.Radius)
		{
			continue;
		}
		const bool bMatches = Job.Tool.IsNone()
			? !Def->RequiredTool.IsRequired()
			: Def->RequiredTool.Type == Job.Tool && ToolTier >= Def->RequiredTool.MinTier;
		const double* Until = Unreachable.Find(*It);
		if (!bMatches || (Settlement && Settlement->IsReservedByOther(*It, this)) || (Until && *Until > Now) || It->Worker.IsValid())
		{
			continue;
		}
		const float Distance = FVector::Dist2D(It->GetActorLocation(), GetActorLocation());
		if (Distance < BestDistance)
		{
			BestDistance = Distance;
			Best = *It;
		}
	}
	return Best;
}

AGamerBuilding* AGamerVillager::FindPlan(const FVector& Center, float Radius, TArray<FGamerItemAmount>& OutMissing) const
{
	const AGamerSettlement* Settlement = GetSettlement();
	const bool bMaterialOnBuild = AGamerGameState::Get(this).BuildFlow == EGamerBuildFlow::MaterialOnBuild;
	const double Now = GetWorld()->GetTimeSeconds();
	AGamerBuilding* Best = nullptr;
	float BestDistance = TNumericLimits<float>::Max();
	for (TActorIterator<AGamerBuilding> It(GetWorld()); It; ++It)
	{
		const FGamerBuildableDef* Def = It->IsPreview() || It->IsFinished() ? nullptr : It->GetDef();
		const double* Until = Unreachable.Find(*It);
		if (!Def || FVector::Dist2D(It->GetActorLocation(), Center) > Radius || (Until && *Until > Now))
		{
			continue;
		}
		if (bMaterialOnBuild && Settlement)
		{
			// The rest of the cost must be in the storage (it is fetched before hammering).
			TArray<FGamerItemAmount> Rest;
			for (const FGamerItemAmount& Cost : Def->Cost)
			{
				const FGamerItemAmount* Spent = It->GetSpent().FindByPredicate([&Cost](const FGamerItemAmount& A) { return A.Item == Cost.Item; });
				const int32 Need = Cost.Count - (Spent ? Spent->Count : 0) - (Carry.ContainsByPredicate([&Cost](const FGamerItemAmount& A) { return A.Item == Cost.Item; })
					? Carry.FindByPredicate([&Cost](const FGamerItemAmount& A) { return A.Item == Cost.Item; })->Count : 0);
				if (Need > 0 && Settlement->CountInStorage(Cost.Item) < Need)
				{
					FGamerItemAmount& Missing = OutMissing.AddDefaulted_GetRef();
					Missing.Item = Cost.Item;
					Missing.Count = Need - Settlement->CountInStorage(Cost.Item);
				}
			}
			if (OutMissing.Num() > 0)
			{
				continue;
			}
		}
		const float Distance = FVector::Dist2D(It->GetActorLocation(), GetActorLocation());
		if (Distance < BestDistance)
		{
			BestDistance = Distance;
			Best = *It;
		}
	}
	if (Best)
	{
		OutMissing.Reset();
	}
	return Best;
}

void AGamerVillager::AddCarry(FName Item, int32 Count)
{
	if (Count <= 0)
	{
		return;
	}
	if (FGamerItemAmount* Found = Carry.FindByPredicate([Item](const FGamerItemAmount& A) { return A.Item == Item; }))
	{
		Found->Count += Count;
	}
	else
	{
		FGamerItemAmount& Added = Carry.AddDefaulted_GetRef();
		Added.Item = Item;
		Added.Count = Count;
	}
}

int32 AGamerVillager::CarryCount() const
{
	int32 Total = 0;
	for (const FGamerItemAmount& Amount : Carry)
	{
		Total += Amount.Count;
	}
	return Total;
}

FString AGamerVillager::Describe() const
{
	static const TCHAR* const TaskNames[] = { TEXT("none"), TEXT("eat"), TEXT("sleep"), TEXT("deliver"), TEXT("fetch_tool"), TEXT("return_tool"),
		TEXT("harvest"), TEXT("fetch_ingredients"), TEXT("craft"), TEXT("fetch_material"), TEXT("build"), TEXT("wait") };
	TArray<FString> CarryParts;
	for (const FGamerItemAmount& Amount : Carry)
	{
		CarryParts.Add(FString::Printf(TEXT("%s x%d"), *Amount.Item.ToString(), Amount.Count));
	}
	return FString::Printf(TEXT("%s job=%s task=%s activity=\"%s\" hunger=%.2f sleep=%.2f shelter=%d bed=%d tool=%s carry=[%s] paths=%d nopath=%d"),
		*VillagerName, *JobId.ToString(), TaskNames[static_cast<int32>(Task)], *Activity, Hunger, Sleep, bShelter ? 1 : 0,
		Bed ? 1 : 0, *HeldTool.ToString(), *FString::Join(CarryParts, TEXT(", ")), PathsFound, PathsMissing);
}

// --- Server logic --------------------------------------------------------------------------------

void AGamerVillager::ServerTick(float DeltaSeconds)
{
	AGamerSettlement* Settlement = GetSettlement();
	if (!Settlement)
	{
		return;
	}
	UpdateNeeds(Settlement->ToGameDays(DeltaSeconds));
	if (IsActorBeingDestroyed())
	{
		return;
	}
	if (Task == ETask::None)
	{
		ThinkTimer -= DeltaSeconds;
		if (ThinkTimer <= 0.f)
		{
			ThinkTimer = 1.f;
			Think();
		}
	}
	if (Task != ETask::None && Task != ETask::Eat && Task != ETask::Sleep && NeedIsUrgent())
	{
		UE_LOG(LogGamer, Log, TEXT("Villager %s interrupts the task: urgent need"), *VillagerName);
		FinishTask();
	}
	if (Task != ETask::None)
	{
		TaskAge += DeltaSeconds;
		if (TaskAge > MaxTaskSeconds && Task != ETask::Sleep && Task != ETask::Wait)
		{
			if (Target.IsValid())
			{
				Unreachable.Add(Target, GetWorld()->GetTimeSeconds() + 60.0);
			}
			UE_LOG(LogGamer, Log, TEXT("Villager %s gives up a task that took too long (%s)"), *VillagerName, *Activity);
			SetActivity(TEXT("gibt auf, versucht etwas anderes"));
			FinishTask();
		}
		else
		{
			RunTask(DeltaSeconds);
		}
	}

	// Shelter: is there a finished building piece above the own bed? (checked every few seconds)
	ShelterTimer -= DeltaSeconds;
	if (ShelterTimer <= 0.f)
	{
		ShelterTimer = 3.f;
		bool bCovered = false;
		if (Bed)
		{
			FHitResult Hit;
			FCollisionQueryParams Params(SCENE_QUERY_STAT(GamerShelter), false, Bed);
			const FVector Start = Bed->GetActorLocation() + FVector(0.f, 0.f, 120.f);
			if (GetWorld()->LineTraceSingleByChannel(Hit, Start, Start + FVector(0.f, 0.f, 1500.f), ECC_Visibility, Params))
			{
				const AGamerBuilding* Roof = Cast<AGamerBuilding>(Hit.GetActor());
				bCovered = Roof && Roof->IsFinished();
			}
		}
		SetShelter(bCovered);
	}
}

void AGamerVillager::UpdateNeeds(float GameDays)
{
	if (const FGamerNeedDef* Food = NeedOfKind(EGamerNeedKind::Food))
	{
		Hunger = FMath::Max(0.f, Hunger - Food->DecayPerDay * GameDays);
		if (Hunger <= 0.f && Food->LeaveAfterDays > 0.f)
		{
			HungerEmptyDays += GameDays;
			if (HungerEmptyDays >= Food->LeaveAfterDays)
			{
				Leave(TEXT("Hunger"));
				return;
			}
		}
		else
		{
			HungerEmptyDays = 0.f;
		}
	}
	if (const FGamerNeedDef* Rest = NeedOfKind(EGamerNeedKind::Sleep); Rest && Task != ETask::Sleep)
	{
		Sleep = FMath::Max(0.f, Sleep - Rest->DecayPerDay * GameDays);
	}

	// Mood: base from the needs (hunger 40 %, sleep 30 %, roof 30 %) plus the fading tavern bonus; it moves slowly (about 4 per game day).
	const float Base = 0.4f * Hunger + 0.3f * Sleep + 0.3f * (bShelter ? 1.f : 0.3f);
	PendingMoodGain = FMath::Max(0.f, PendingMoodGain - 0.1f * GameDays);
	const float MoodTarget = FMath::Clamp(Base + PendingMoodGain, 0.f, 1.f);
	Mood = FMath::Clamp(Mood + (MoodTarget - Mood) * FMath::Min(1.f, 4.f * GameDays), 0.f, 1.f);
}

void AGamerVillager::Think()
{
	AGamerSettlement* Settlement = GetSettlement();
	const UGamerDataSubsystem* Data = UGamerDataSubsystem::Get(this);
	AGamerBuilding* Storage = Settlement ? Settlement->FindNearestStorageBuilding(GetActorLocation()) : nullptr;

	// 1. Hunger: in the tavern if it has food (mood bonus, decision 2026-11-15), else from the storage.
	if (const FGamerNeedDef* Food = NeedOfKind(EGamerNeedKind::Food); Food && Hunger < Food->Threshold)
	{
		if (AGamerTavern* Tavern = FindTavernWithFood())
		{
			Task = ETask::Eat;
			Target = Tavern;
			SetActivity(TEXT("geht in die Taverne essen"));
			return;
		}
	}
	if (const FGamerNeedDef* Food = NeedOfKind(EGamerNeedKind::Food); Food && Hunger < Food->Threshold && Storage)
	{
		const bool bAnyFood = Food->Foods.ContainsByPredicate([Settlement](const FGamerFoodValue& F) { return Settlement->CountInStorage(F.Item) > 0; });
		if (bAnyFood)
		{
			Task = ETask::Eat;
			Target = Storage;
			SetActivity(TEXT("geht essen"));
			return;
		}
	}
	// 2. Sleep.
	if (const FGamerNeedDef* Rest = NeedOfKind(EGamerNeedKind::Sleep); Rest && Sleep < Rest->Threshold)
	{
		Task = ETask::Sleep;
		Target = Bed.Get();
		SetActivity(Bed ? TEXT("geht schlafen") : TEXT("legt sich hin (kein Bett)"));
		return;
	}
	// 3. Full hands: to the storage.
	if (CarryCount() >= CarryLimit && Storage)
	{
		Task = ETask::Deliver;
		Target = Storage;
		SetActivity(TEXT("bringt Waren ins Lager"));
		return;
	}
	// 4. Own job.
	const FGamerJobDef* Job = CurrentJobDef();
	FString WhyNot;
	if (Job && !Job->bFallback && PlanJob(*Job, false, WhyNot))
	{
		bWorkingOnFallback = false;
		return;
	}
	// 5. Fallback job (the reason why the own job does not work stays visible).
	const FGamerJobDef* Fallback = FallbackJobDef();
	FString FallbackWhy;
	if (Fallback && PlanJob(*Fallback, true, FallbackWhy))
	{
		bWorkingOnFallback = Job && !Job->bFallback;
		if (bWorkingOnFallback && !WhyNot.IsEmpty())
		{
			SetActivity(FString::Printf(TEXT("%s – %s: %s"), *Activity, *Job->Name, *WhyNot));
		}
		return;
	}
	// 6. Something carried but nothing to do: bring it in. Otherwise wait.
	if (CarryCount() > 0 && Storage)
	{
		Task = ETask::Deliver;
		Target = Storage;
		SetActivity(TEXT("bringt Waren ins Lager"));
		return;
	}
	Task = ETask::Wait;
	Target = Storage ? static_cast<AActor*>(Storage) : static_cast<AActor*>(Bed.Get());
	TaskTimer = 5.f;
	const FString& Reason = !WhyNot.IsEmpty() ? WhyNot : FallbackWhy;
	SetActivity(Reason.IsEmpty() ? FString(TEXT("hat nichts zu tun")) : TEXT("wartet: ") + Reason);
}

bool AGamerVillager::PlanJob(const FGamerJobDef& Job, bool bIsFallback, FString& OutWhyNot)
{
	AGamerSettlement* Settlement = GetSettlement();
	const UGamerDataSubsystem* Data = UGamerDataSubsystem::Get(this);
	AGamerBuilding* Storage = Settlement ? Settlement->FindNearestStorageBuilding(GetActorLocation()) : nullptr;
	if (!Storage || !Data)
	{
		OutWhyNot = TEXT("kein Lager");
		return false;
	}

	// Tool: the own job gives back a tool it does not use (the fallback job just keeps it).
	// A missing tool is fetched only further down, once it is clear there is work for it.
	const FGamerItemDef* Held = HeldTool.IsNone() ? nullptr : Data->FindItem(HeldTool);
	const bool bHeldFits = Held && !Job.Tool.IsNone() && Held->Tool.Type == Job.Tool;
	if (Held && !bHeldFits && !bIsFallback)
	{
		Task = ETask::ReturnTool;
		Target = Storage;
		SetActivity(FString::Printf(TEXT("bringt %s zurück"), *Held->Name));
		return true;
	}
	const FGamerItemDef* Tool = bHeldFits ? Held : nullptr;
	FName ToolToFetch;
	if (!Job.Tool.IsNone() && !Tool)
	{
		ToolToFetch = Settlement->FindToolInStorage(Job.Tool);
		if (ToolToFetch.IsNone())
		{
			OutWhyNot = FString::Printf(TEXT("keine %s im Lager"), *ToolLabel(Job.Tool));
			return false;
		}
		Tool = Data->FindItem(ToolToFetch);
	}
	auto FetchTool = [&]()
	{
		Task = ETask::FetchTool;
		Target = Storage;
		TaskRecipe = ToolToFetch;
		SetActivity(FString::Printf(TEXT("holt %s aus dem Lager"), *VillagerItemName(Data, ToolToFetch)));
		return true;
	};

	// Station.
	AActor* Station = nullptr;
	if (!Job.Station.IsNone())
	{
		Station = FindStation(Job.Station);
		if (!Station)
		{
			const FGamerBuildableDef* StationDef = Data->FindBuildable(Job.Station);
			OutWhyNot = FString::Printf(TEXT("braucht %s"), StationDef ? *StationDef->Name : *Job.Station.ToString());
			return false;
		}
	}
	const FVector Center = Station ? Station->GetActorLocation() : Storage->GetActorLocation();

	switch (Job.Kind)
	{
	case EGamerJobKind::Harvest:
	{
		AGamerResourceNode* Node = FindNode(Job, Center, Tool ? Tool->Tier : 0);
		if (!Node)
		{
			OutWhyNot = TEXT("nichts zu ernten in der Nähe");
			return false;
		}
		if (!ToolToFetch.IsNone())
		{
			return FetchTool();
		}
		Settlement->Reserve(Node, this);
		Task = ETask::Harvest;
		Target = Node;
		TaskTimer = 0.f;
		StrikeTimer = 0.5f;
		const FGamerResourceNodeDef* Def = Node->GetDef();
		const TCHAR* Verb = Job.Tool == TEXT("axe") ? TEXT("fällt") : Job.Tool == TEXT("pickaxe") ? TEXT("baut ab:") : TEXT("sammelt");
		SetActivity(FString::Printf(TEXT("%s %s"), Verb, Def ? *Def->Name : TEXT("")));
		return true;
	}
	case EGamerJobKind::Craft:
	{
		for (const FName RecipeId : Job.Recipes)
		{
			const FGamerRecipeDef* Recipe = Data->FindRecipe(RecipeId);
			if (!Recipe || Recipe->Outputs.Num() == 0)
			{
				continue;
			}
			const FGamerItemAmount& Output = Recipe->Outputs[0];
			if (Settlement->CountInStorage(Output.Item) >= Job.StockTarget)
			{
				OutWhyNot = FString::Printf(TEXT("genug %s im Lager"), *VillagerItemName(Data, Output.Item));
				continue;
			}
			if (!CraftStationFor(*Recipe, Job, Station))
			{
				const FGamerBuildableDef* Place = Data->FindBuildable(Recipe->Station);
				OutWhyNot = FString::Printf(TEXT("für %s fehlt %s in der Nähe"), *VillagerItemName(Data, Output.Item), Place ? *Place->Name : *Recipe->Station.ToString());
				continue;
			}
			if (!Settlement->HasInStorage(Recipe->Inputs))
			{
				OutWhyNot = FString::Printf(TEXT("für %s fehlt: %s"), *VillagerItemName(Data, Output.Item), *VillagerAmountsText(Data, Recipe->Inputs));
				continue;
			}
			TaskRecipe = RecipeId;
			Task = ETask::FetchIngredients;
			Target = Storage;
			SetActivity(FString::Printf(TEXT("holt Zutaten für %s"), *VillagerItemName(Data, Output.Item)));
			return true;
		}
		return false;
	}
	case EGamerJobKind::Farm:
		return PlanFarm(Job, Center, Storage, OutWhyNot);
	case EGamerJobKind::Serve:
		return PlanServe(Job, Station, Storage, OutWhyNot);
	case EGamerJobKind::Build:
	{
		TArray<FGamerItemAmount> Missing;
		AGamerBuilding* Plan = FindPlan(Center, Job.Radius, Missing);
		if (!Plan)
		{
			OutWhyNot = Missing.Num() > 0 ? TEXT("für den Bauplan fehlt: ") + VillagerAmountsText(Data, Missing) : FString(TEXT("keine Baupläne"));
			return false;
		}
		if (!ToolToFetch.IsNone())
		{
			return FetchTool();
		}
		const bool bMaterialOnBuild = AGamerGameState::Get(this).BuildFlow == EGamerBuildFlow::MaterialOnBuild;
		Task = bMaterialOnBuild ? ETask::FetchMaterial : ETask::Build;
		Target = bMaterialOnBuild ? static_cast<AActor*>(Storage) : static_cast<AActor*>(Plan);
		TaskTimer = 0.f;
		StrikeTimer = 0.5f;
		const FGamerBuildableDef* Def = Plan->GetDef();
		const FString PlanName = Def ? Def->Name : FString();
		if (bMaterialOnBuild)
		{
			SetActivity(FString::Printf(TEXT("holt Material für %s"), *PlanName));
		}
		else
		{
			SetActivity(FString::Printf(TEXT("baut %s"), *PlanName));
		}
		// Remember the plan while fetching material.
		Unreachable.Remove(Plan);
		if (bMaterialOnBuild)
		{
			Settlement->Reserve(Plan, this);
		}
		BuildPlan = Plan;
		return true;
	}
	}
	return false;
}

void AGamerVillager::RunTask(float DeltaSeconds)
{
	AGamerSettlement* Settlement = GetSettlement();
	const UGamerDataSubsystem* Data = UGamerDataSubsystem::Get(this);
	AActor* TargetActor = Target.Get();
	if (!Settlement || !Data)
	{
		return;
	}

	if (Task == ETask::FetchFarmInput || Task == ETask::FarmWork)
	{
		RunFarmWork(DeltaSeconds);
		return;
	}
	switch (Task)
	{
	case ETask::Wait:
		TaskTimer -= DeltaSeconds;
		if (TargetActor)
		{
			MoveTo(TargetActor->GetActorLocation(), 450.f, DeltaSeconds);
		}
		if (TaskTimer <= 0.f)
		{
			FinishTask();
		}
		break;

	case ETask::Eat:
	{
		if (!TargetActor)
		{
			FinishTask();
			break;
		}
		if (!MoveTo(TargetActor->GetActorLocation(), StorageReach, DeltaSeconds))
		{
			break;
		}
		TaskTimer += DeltaSeconds;
		if (TaskTimer < 1.5f)
		{
			break;
		}
		AGamerTavern* Tavern = Cast<AGamerTavern>(TargetActor);
		TArray<FString> Eaten;
		if (Tavern)
		{
			// In the tavern: eat from its stock, then one drink if there is one; both raise the mood.
			const FGamerBuildableDef* TavernDef = Tavern->GetDef();
			int32 BestTier = 0;
			if (const FGamerNeedDef* Food = NeedOfKind(EGamerNeedKind::Food))
			{
				for (int32 Bite = 0; Bite < 12 && Hunger < 0.95f; ++Bite)
				{
					const FGamerFoodValue* Choice = Food->Foods.FindByPredicate([Tavern](const FGamerFoodValue& F) { return Tavern->CountStock(F.Item) > 0; });
					if (!Choice || Tavern->TakeStock(Choice->Item, 1) == 0)
					{
						break;
					}
					Hunger = FMath::Min(1.f, Hunger + Choice->Value);
					Eaten.Add(VillagerItemName(Data, Choice->Item));
					const FGamerItemDef* Def = Data->FindItem(Choice->Item);
					BestTier = FMath::Max(BestTier, Def ? Def->Tier : 1);
				}
				// A drink: a meal item that is not on the hunger list.
				const TArray<FGamerItemAmount> StockCopy = Tavern->GetStock();
				for (const FGamerItemAmount& Amount : StockCopy)
				{
					const FGamerItemDef* Def = Data->FindItem(Amount.Item);
					const bool bFills = Food->Foods.ContainsByPredicate([&Amount](const FGamerFoodValue& F) { return F.Item == Amount.Item; });
					if (Def && Def->Food.IsMeal() && !bFills && Tavern->TakeStock(Amount.Item, 1) > 0)
					{
						Eaten.Add(VillagerItemName(Data, Amount.Item));
						OnTavernMeal(TavernDef ? TavernDef->Tavern.MoodDrink : 0.f);
						break;
					}
				}
			}
			if (BestTier > 0)
			{
				OnTavernMeal(TavernDef ? TavernDef->Tavern.MoodPerTier * BestTier : 0.f);
			}
		}
		else
		{
			// At the storage: hand in what is carried, then eat the best food until full.
			for (const FGamerItemAmount& Amount : Carry)
			{
				Settlement->AddToStorage(Amount.Item, Amount.Count);
			}
			Carry.Reset();
			if (const FGamerNeedDef* Food = NeedOfKind(EGamerNeedKind::Food))
			{
				for (int32 Bite = 0; Bite < 12 && Hunger < 0.95f; ++Bite)
				{
					const FGamerFoodValue* Choice = Food->Foods.FindByPredicate([Settlement](const FGamerFoodValue& F) { return Settlement->CountInStorage(F.Item) > 0; });
					if (!Choice || Settlement->TakeFromStorage(Choice->Item, 1) == 0)
					{
						break;
					}
					Hunger = FMath::Min(1.f, Hunger + Choice->Value);
					Eaten.Add(VillagerItemName(Data, Choice->Item));
				}
			}
		}
		UE_LOG(LogGamer, Log, TEXT("Villager %s ate [%s]%s, hunger %.2f"), *VillagerName, *FString::Join(Eaten, TEXT(", ")),
			Tavern ? TEXT(" in the tavern") : TEXT(""), Hunger);
		SetActivity(Eaten.Num() > 0 ? TEXT("hat gegessen") : TEXT("findet nichts zu essen"));
		FinishTask();
		break;
	}

	case ETask::Sleep:
	{
		if (TargetActor && !MoveTo(TargetActor->GetActorLocation(), 180.f, DeltaSeconds))
		{
			break;
		}
		const FGamerNeedDef* Rest = NeedOfKind(EGamerNeedKind::Sleep);
		const float Rate = Rest ? Rest->RestorePerDay * (Bed ? 1.f : 0.5f) : 10.f;
		if (Activity != TEXT("schläft"))
		{
			SetActivity(TEXT("schläft"));
		}
		Sleep = FMath::Min(1.f, Sleep + Rate * Settlement->ToGameDays(DeltaSeconds));
		if (Sleep >= 0.98f)
		{
			Sleep = 1.f;
			UE_LOG(LogGamer, Log, TEXT("Villager %s woke up (%s)"), *VillagerName, Bed ? TEXT("bed") : TEXT("ground"));
			FinishTask();
		}
		break;
	}

	case ETask::ServeFetch:
	{
		if (!TargetActor)
		{
			FinishTask();
			break;
		}
		if (!MoveTo(TargetActor->GetActorLocation(), StorageReach, DeltaSeconds))
		{
			break;
		}
		AActor* Tavern = ServeTavern.Get();
		ServeCount = Tavern ? Settlement->TakeFromStorage(ServeItem, ServeWanted, &ServeFreshness) : 0;
		if (ServeCount <= 0)
		{
			ServeCount = 0;
			FinishTask();
			break;
		}
		Task = ETask::Serve;
		Target = Tavern;
		SetActivity(FString::Printf(TEXT("bringt %d %s in die Taverne"), ServeCount, *VillagerItemName(Data, ServeItem)));
		break;
	}

	case ETask::Serve:
	{
		AGamerTavern* Tavern = Cast<AGamerTavern>(TargetActor);
		if (!Tavern)
		{
			FinishTask(); // tavern gone: the load goes back with the next delivery
			break;
		}
		if (!MoveTo(Tavern->GetActorLocation(), StorageReach, DeltaSeconds))
		{
			break;
		}
		const int32 Put = Tavern->AddStock(ServeItem, ServeCount, ServeFreshness);
		ServeCount -= Put;
		UE_LOG(LogGamer, Log, TEXT("Villager %s served %d %s - tavern stock: %s"), *VillagerName, Put, *ServeItem.ToString(), *Tavern->DescribeStock());
		FinishTask();
		break;
	}

	case ETask::Deliver:
	{
		if (!TargetActor)
		{
			FinishTask();
			break;
		}
		if (!MoveTo(TargetActor->GetActorLocation(), StorageReach, DeltaSeconds))
		{
			break;
		}
		TArray<FGamerItemAmount> Kept;
		TArray<FString> Delivered;
		for (const FGamerItemAmount& Amount : Carry)
		{
			const int32 NotFitting = Settlement->AddToStorage(Amount.Item, Amount.Count);
			if (Amount.Count - NotFitting > 0)
			{
				Delivered.Add(FString::Printf(TEXT("%d %s"), Amount.Count - NotFitting, *VillagerItemName(Data, Amount.Item)));
			}
			if (NotFitting > 0)
			{
				FGamerItemAmount& Rest = Kept.AddDefaulted_GetRef();
				Rest.Item = Amount.Item;
				Rest.Count = NotFitting;
			}
		}
		Carry = Kept;
		UE_LOG(LogGamer, Log, TEXT("Villager %s delivered [%s] - %s"), *VillagerName, *FString::Join(Delivered, TEXT(", ")), *Settlement->Describe());
		if (Kept.Num() > 0)
		{
			Task = ETask::Wait;
			TaskTimer = 10.f;
			SetActivity(TEXT("wartet: Lager voll"));
			break;
		}
		FinishTask();
		break;
	}

	case ETask::FetchTool:
	case ETask::ReturnTool:
	{
		if (!TargetActor)
		{
			FinishTask();
			break;
		}
		if (!MoveTo(TargetActor->GetActorLocation(), StorageReach, DeltaSeconds))
		{
			break;
		}
		if (Task == ETask::FetchTool)
		{
			if (Settlement->TakeFromStorage(TaskRecipe, 1) == 1)
			{
				HeldTool = TaskRecipe;
				UE_LOG(LogGamer, Log, TEXT("Villager %s took %s from the storage"), *VillagerName, *HeldTool.ToString());
			}
		}
		else if (!HeldTool.IsNone())
		{
			Settlement->AddToStorage(HeldTool, 1);
			UE_LOG(LogGamer, Log, TEXT("Villager %s returned %s to the storage"), *VillagerName, *HeldTool.ToString());
			HeldTool = NAME_None;
		}
		ApplyLook();
		ForceNetUpdate();
		FinishTask();
		break;
	}

	case ETask::Harvest:
	{
		AGamerResourceNode* Node = Cast<AGamerResourceNode>(TargetActor);
		const FGamerResourceNodeDef* Def = Node ? Node->GetDef() : nullptr;
		if (!Def || Node->IsDepleted())
		{
			FinishTask();
			break;
		}
		if (!MoveTo(Node->GetActorLocation(), 230.f, DeltaSeconds))
		{
			break;
		}
		bool bDone = false;
		if (Def->RequiredTool.IsRequired())
		{
			StrikeTimer -= DeltaSeconds;
			if (StrikeTimer <= 0.f)
			{
				StrikeTimer = StrikeInterval;
				bDone = Node->AddWork(StrikeInterval * WorkSpeed() / FMath::Max(0.1f, Def->WorkSeconds));
			}
		}
		else
		{
			TaskTimer += DeltaSeconds * WorkSpeed();
			bDone = TaskTimer >= Def->WorkSeconds;
		}
		if (bDone)
		{
			const TArray<FGamerItemAmount> Gained = Node->Harvest();
			for (const FGamerItemAmount& Entry : Gained)
			{
				AddCarry(Entry.Item, Entry.Count);
			}
			UE_LOG(LogGamer, Log, TEXT("Villager %s harvested %s: [%s], carrying %d"), *VillagerName, *Def->Id.ToString(),
				*VillagerAmountsText(Data, Gained), CarryCount());
			FinishTask();
		}
		break;
	}

	case ETask::FetchIngredients:
	{
		if (!TargetActor)
		{
			FinishTask();
			break;
		}
		if (!MoveTo(TargetActor->GetActorLocation(), StorageReach, DeltaSeconds))
		{
			break;
		}
		const FGamerRecipeDef* Recipe = Data->FindRecipe(TaskRecipe);
		const FGamerJobDef* Job = CurrentJobDef();
		AActor* JobStation = Job && !Job->Station.IsNone() ? FindStation(Job->Station) : nullptr;
		AActor* Station = Recipe && Job ? CraftStationFor(*Recipe, *Job, JobStation) : nullptr;
		if (!Recipe || !Station || !Settlement->TakeAllFromStorage(Recipe->Inputs))
		{
			FinishTask();
			break;
		}
		Task = ETask::Craft;
		Target = Station;
		TaskTimer = 0.f;
		SetActivity(FString::Printf(TEXT("kocht %s"), Recipe->Outputs.Num() > 0 ? *VillagerItemName(Data, Recipe->Outputs[0].Item) : TEXT("")));
		break;
	}

	case ETask::Craft:
	{
		const FGamerRecipeDef* Recipe = Data->FindRecipe(TaskRecipe);
		if (!Recipe)
		{
			FinishTask();
			break;
		}
		if (!TargetActor)
		{
			// Station gone: the ingredients go back.
			for (const FGamerItemAmount& Input : Recipe->Inputs)
			{
				Settlement->AddToStorage(Input.Item, Input.Count);
			}
			FinishTask();
			break;
		}
		if (!MoveTo(TargetActor->GetActorLocation(), 260.f, DeltaSeconds))
		{
			break;
		}
		TaskTimer += DeltaSeconds * WorkSpeed();
		if (TaskTimer >= Recipe->WorkSeconds)
		{
			for (const FGamerItemAmount& Output : Recipe->Outputs)
			{
				AddCarry(Output.Item, Output.Count);
			}
			UE_LOG(LogGamer, Log, TEXT("Villager %s made %s"), *VillagerName, *VillagerAmountsText(Data, Recipe->Outputs));
			bDeliverNow = true;
			FinishTask();
		}
		break;
	}

	case ETask::FetchMaterial:
	{
		AGamerBuilding* Plan = BuildPlan.Get();
		const FGamerBuildableDef* Def = Plan ? Plan->GetDef() : nullptr;
		if (!TargetActor || !Def || Plan->IsFinished())
		{
			FinishTask();
			break;
		}
		if (!MoveTo(TargetActor->GetActorLocation(), StorageReach, DeltaSeconds))
		{
			break;
		}
		// Take what the plan still needs (minus what is already carried).
		TArray<FGamerItemAmount> Take;
		for (const FGamerItemAmount& Cost : Def->Cost)
		{
			const FGamerItemAmount* Spent = Plan->GetSpent().FindByPredicate([&Cost](const FGamerItemAmount& A) { return A.Item == Cost.Item; });
			const FGamerItemAmount* Carried = Carry.FindByPredicate([&Cost](const FGamerItemAmount& A) { return A.Item == Cost.Item; });
			const int32 Need = Cost.Count - (Spent ? Spent->Count : 0) - (Carried ? Carried->Count : 0);
			if (Need > 0)
			{
				FGamerItemAmount& Entry = Take.AddDefaulted_GetRef();
				Entry.Item = Cost.Item;
				Entry.Count = Need;
			}
		}
		if (!Settlement->TakeAllFromStorage(Take))
		{
			SetActivity(TEXT("Material fehlt im Lager"));
			FinishTask();
			break;
		}
		for (const FGamerItemAmount& Amount : Take)
		{
			AddCarry(Amount.Item, Amount.Count);
		}
		Task = ETask::Build;
		Target = Plan;
		StrikeTimer = 0.5f;
		SetActivity(FString::Printf(TEXT("baut %s"), *Def->Name));
		break;
	}

	case ETask::Build:
	{
		AGamerBuilding* Plan = Cast<AGamerBuilding>(TargetActor);
		const FGamerBuildableDef* Def = Plan ? Plan->GetDef() : nullptr;
		if (!Def || Plan->IsFinished())
		{
			FinishTask();
			break;
		}
		if (!MoveTo(Plan->GetActorLocation(), 320.f, DeltaSeconds))
		{
			break;
		}
		StrikeTimer -= DeltaSeconds;
		if (StrikeTimer > 0.f)
		{
			break;
		}
		StrikeTimer = StrikeInterval;
		const float Speed = WorkSpeed();
		if (AGamerGameState::Get(this).BuildFlow == EGamerBuildFlow::MaterialOnBuild)
		{
			// Same rule as for players: every strike uses its share of the cost, here from the carried material.
			const int32 Strikes = FMath::Max(1, FMath::CeilToInt(Def->BuildSeconds / (StrikeInterval * Speed)));
			const int32 Strike = FMath::Min(Strikes, Plan->GetStrikesDone() + 1);
			TArray<FGamerItemAmount> Spent = Plan->GetSpent();
			for (const FGamerItemAmount& Cost : Def->Cost)
			{
				const int32 Due = FMath::CeilToInt(static_cast<float>(Cost.Count) * Strike / Strikes);
				FGamerItemAmount* Already = Spent.FindByPredicate([&Cost](const FGamerItemAmount& A) { return A.Item == Cost.Item; });
				const int32 Delta = Due - (Already ? Already->Count : 0);
				FGamerItemAmount* Carried = Carry.FindByPredicate([&Cost](const FGamerItemAmount& A) { return A.Item == Cost.Item; });
				if (Delta > 0 && (!Carried || Carried->Count < Delta))
				{
					SetActivity(TEXT("Material fehlt"));
					FinishTask();
					return;
				}
				if (Delta > 0)
				{
					Carried->Count -= Delta;
					if (Already)
					{
						Already->Count += Delta;
					}
					else
					{
						FGamerItemAmount& Added = Spent.AddDefaulted_GetRef();
						Added.Item = Cost.Item;
						Added.Count = Delta;
					}
				}
			}
			Carry.RemoveAll([](const FGamerItemAmount& A) { return A.Count <= 0; });
			Plan->SetSpent(Spent);
			Plan->SetStrikesDone(Strike);
			Plan->AddProgress(Strike >= Strikes ? 1.f : 1.f / Strikes);
		}
		else
		{
			Plan->AddProgress(StrikeInterval * Speed / FMath::Max(0.1f, Def->BuildSeconds));
		}
		if (Plan->IsFinished())
		{
			UE_LOG(LogGamer, Log, TEXT("Villager %s finished building %s"), *VillagerName, *Def->Id.ToString());
			FinishTask();
		}
		break;
	}

	case ETask::None:
		break;
	}
}

namespace
{
	enum class EFarmAction : uint8 { None, Harvest, Sow, Water, Fertilize, CollectWater, CollectCompost, FillComposter };
}

bool AGamerVillager::PlanFarm(const FGamerJobDef& Job, const FVector& Center, AActor* Storage, FString& OutWhyNot)
{
	AGamerSettlement* Settlement = GetSettlement();
	const UGamerDataSubsystem* Data = UGamerDataSubsystem::Get(this);
	if (!Settlement || !Data || !Storage)
	{
		return false;
	}
	// Farm buildings in the work area, nearest first.
	TArray<AGamerFarmBuilding*> Farms;
	for (TActorIterator<AGamerFarmBuilding> It(GetWorld()); It; ++It)
	{
		if (!It->IsPreview() && It->IsFinished() && FVector::Dist2D(It->GetActorLocation(), Center) <= Job.Radius
			&& !Settlement->IsReservedByOther(*It, this) && !Unreachable.Contains(*It))
		{
			Farms.Add(*It);
		}
	}
	Farms.Sort([this](const AGamerFarmBuilding& A, const AGamerFarmBuilding& B)
	{
		return FVector::DistSquared(A.GetActorLocation(), GetActorLocation()) < FVector::DistSquared(B.GetActorLocation(), GetActorLocation());
	});
	if (Farms.Num() == 0)
	{
		OutWhyNot = TEXT("keine Felder beim Hof");
		return false;
	}

	auto Start = [&](AGamerFarmBuilding* Farm, EFarmAction Action, FName Item, int32 Count, const FString& Text)
	{
		Settlement->Reserve(Farm, this);
		FarmTarget = Farm;
		FarmAction = static_cast<uint8>(Action);
		FarmItem = Item;
		FarmWanted = Count;
		FarmCount = 0;
		TaskTimer = 0.f;
		if (!Item.IsNone() && Count > 0)
		{
			Task = ETask::FetchFarmInput;
			Target = Storage;
			SetActivity(FString::Printf(TEXT("holt %d %s für %s"), Count, *VillagerItemName(Data, Item), *Text));
		}
		else
		{
			Task = ETask::FarmWork;
			Target = Farm;
			SetActivity(Text);
		}
		return true;
	};

	FString Missing;
	// 1) Harvest ripe fields first (nothing spoils on the field, but the harvest feeds the village).
	for (AGamerFarmBuilding* Farm : Farms)
	{
		if (Farm->GetFieldNeed() == EGamerFieldNeed::Harvest)
		{
			return Start(Farm, EFarmAction::Harvest, NAME_None, 0, TEXT("erntet ein Feld"));
		}
	}
	for (AGamerFarmBuilding* Farm : Farms)
	{
		const FGamerFarmProps* Props = Farm->GetFarm();
		const EGamerFieldNeed Need = Farm->GetFieldNeed();
		const int32 Units = Farm->GetUnits();
		if (Need == EGamerFieldNeed::Water)
		{
			const int32 Have = Settlement->CountInStorage(Props->WaterItem);
			if (Have > 0)
			{
				return Start(Farm, EFarmAction::Water, Props->WaterItem, FMath::Min3(Have, Farm->WaterItemsNeeded(), CarryLimit * 2), TEXT("gießt ein Feld"));
			}
			Missing = VillagerItemName(Data, Props->WaterItem);
		}
		else if (Need == EGamerFieldNeed::Sow)
		{
			// The seed the storage has most of (enough for the whole field).
			FName Best;
			int32 BestCount = 0;
			for (const FGamerItemAmount& Stock : Settlement->GetStorage())
			{
				const FGamerItemDef* Def = Data->FindItem(Stock.Item);
				if (Def && Def->Plant.IsSeed() && Stock.Count >= Units && Stock.Count > BestCount)
				{
					Best = Stock.Item;
					BestCount = Stock.Count;
				}
			}
			if (!Best.IsNone())
			{
				return Start(Farm, EFarmAction::Sow, Best, Units, TEXT("sät ein Feld"));
			}
			Missing = FString::Printf(TEXT("%d Saatgut"), Units);
		}
		else if (Need == EGamerFieldNeed::Fertilize && Settlement->CountInStorage(Props->FertilizerItem) >= Units)
		{
			return Start(Farm, EFarmAction::Fertilize, Props->FertilizerItem, Units, TEXT("düngt ein Feld"));
		}
	}
	// 2) Keep water and compost moving: empty rain catchers and composters into the storage, fill composters.
	for (AGamerFarmBuilding* Farm : Farms)
	{
		const FGamerFarmProps* Props = Farm->GetFarm();
		if (Farm->GetKind() == EGamerFarmKind::Water && Farm->GetReadyOutput() >= 2)
		{
			return Start(Farm, EFarmAction::CollectWater, NAME_None, 0, TEXT("holt Wasser vom Regenfänger"));
		}
		if (Farm->GetKind() == EGamerFarmKind::Composter && Farm->GetReadyOutput() >= 1)
		{
			return Start(Farm, EFarmAction::CollectCompost, NAME_None, 0, TEXT("holt Kompost"));
		}
		if (Farm->GetKind() == EGamerFarmKind::Composter && Farm->GetInputSpace() > 0)
		{
			for (const FName Input : Props->Inputs)
			{
				const int32 Have = Settlement->CountInStorage(Input);
				if (Have >= Props->PerOutput)
				{
					return Start(Farm, EFarmAction::FillComposter, Input, FMath::Min3(Have, Farm->GetInputSpace(), CarryLimit), TEXT("füllt den Komposter"));
				}
			}
		}
	}
	OutWhyNot = Missing.IsEmpty() ? FString(TEXT("keine Feldarbeit")) : TEXT("im Lager fehlt ") + Missing;
	return false;
}

void AGamerVillager::RunFarmWork(float DeltaSeconds)
{
	AGamerSettlement* Settlement = GetSettlement();
	const UGamerDataSubsystem* Data = UGamerDataSubsystem::Get(this);
	AGamerFarmBuilding* Farm = FarmTarget.Get();
	auto GiveBack = [this]()
	{
		// Whatever was fetched and not used goes back with the next delivery.
		if (FarmCount > 0)
		{
			AddCarry(FarmItem, FarmCount);
			bDeliverNow = true;
		}
		FarmCount = 0;
	};
	if (!Settlement || !Data || !Farm)
	{
		GiveBack();
		FinishTask();
		return;
	}
	if (Task == ETask::FetchFarmInput)
	{
		AActor* Storage = Target.Get();
		if (!Storage)
		{
			FinishTask();
			return;
		}
		if (!MoveTo(Storage->GetActorLocation(), StorageReach, DeltaSeconds))
		{
			return;
		}
		FarmCount = Settlement->TakeFromStorage(FarmItem, FarmWanted);
		const bool bSowing = FarmAction == static_cast<uint8>(EFarmAction::Sow);
		if (FarmCount <= 0 || (bSowing && FarmCount < Farm->GetUnits()))
		{
			GiveBack();
			FinishTask();
			return;
		}
		Task = ETask::FarmWork;
		Target = Farm;
		TaskTimer = 0.f;
		return;
	}

	// At the building: the size of a drawn field counts for the walking distance.
	const FVector2D Size = Farm->GetSizeCm();
	if (!MoveTo(Farm->GetActorLocation(), 200.f + 0.5f * FMath::Max(Size.X, Size.Y), DeltaSeconds))
	{
		return;
	}
	TaskTimer += DeltaSeconds * WorkSpeed();
	if (TaskTimer < 2.f + 0.15f * Farm->GetUnits())
	{
		return;
	}
	TArray<FGamerItemAmount> Gained;
	switch (static_cast<EFarmAction>(FarmAction))
	{
	case EFarmAction::Harvest:
		Gained = Farm->HarvestCrop();
		break;
	case EFarmAction::Sow:
		if (Farm->GetFieldNeed() == EGamerFieldNeed::Sow && FarmCount >= Farm->GetUnits() && Farm->Sow(FarmItem))
		{
			FarmCount -= Farm->GetUnits();
		}
		break;
	case EFarmAction::Water:
	{
		const int32 Poured = FMath::Min(FarmCount, Farm->WaterItemsNeeded());
		Farm->AddWater(Poured);
		FarmCount -= Poured;
		break;
	}
	case EFarmAction::Fertilize:
		if (FarmCount >= Farm->GetUnits() && Farm->Fertilize())
		{
			FarmCount -= Farm->GetUnits();
		}
		break;
	case EFarmAction::CollectWater:
	case EFarmAction::CollectCompost:
	{
		const FGamerFarmProps* Props = Farm->GetFarm();
		const int32 Taken = Props ? Farm->TakeOutput(CarryLimit) : 0;
		if (Taken > 0)
		{
			FGamerItemAmount& Entry = Gained.AddDefaulted_GetRef();
			Entry.Item = Props->Output;
			Entry.Count = Taken;
		}
		break;
	}
	case EFarmAction::FillComposter:
	{
		const int32 Put = FMath::Min(FarmCount, Farm->GetInputSpace());
		Farm->PutInput(Put);
		FarmCount -= Put;
		break;
	}
	default:
		break;
	}
	for (const FGamerItemAmount& Entry : Gained)
	{
		AddCarry(Entry.Item, Entry.Count);
		bDeliverNow = true;
	}
	UE_LOG(LogGamer, Log, TEXT("Villager %s farm work %d at %s: gained [%s], left over %d - %s"), *VillagerName, FarmAction, *Farm->GetName(),
		*VillagerAmountsText(Data, Gained), FarmCount, *Farm->GetStatusText());
	GiveBack();
	FinishTask();
}

bool AGamerVillager::NeedIsUrgent() const
{
	const AGamerSettlement* Settlement = GetSettlement();
	if (const FGamerNeedDef* Rest = NeedOfKind(EGamerNeedKind::Sleep); Rest && Sleep < Rest->Threshold * 0.4f)
	{
		return true;
	}
	if (const FGamerNeedDef* Food = NeedOfKind(EGamerNeedKind::Food); Food && Settlement && Hunger < Food->Threshold * 0.4f)
	{
		// Only urgent if there is something to eat; otherwise working on is the better choice.
		return Food->Foods.ContainsByPredicate([Settlement](const FGamerFoodValue& F) { return Settlement->CountInStorage(F.Item) > 0; });
	}
	return false;
}

void AGamerVillager::FinishTask()
{
	// Farm input fetched but not used (task given up): it goes back with the next delivery.
	if (FarmCount > 0)
	{
		AddCarry(FarmItem, FarmCount);
		FarmCount = 0;
		bDeliverNow = true;
	}
	// Innkeeper load not put into the tavern: back with the next delivery.
	if (ServeCount > 0)
	{
		AddCarry(ServeItem, ServeCount);
		ServeCount = 0;
		bDeliverNow = true;
	}
	Task = ETask::None;
	TaskAge = 0.f;
	PathPoints.Reset();
	RepathTimer = 0.f;
	Target.Reset();
	BuildPlan.Reset();
	TaskTimer = 0.f;
	StuckCount = 0;
	ThinkTimer = 0.2f;
	if (AGamerSettlement* Settlement = GetSettlement())
	{
		Settlement->ReleaseAll(this);
	}
	if (bDeliverNow)
	{
		bDeliverNow = false;
		AGamerSettlement* Settlement = GetSettlement();
		if (AGamerBuilding* Storage = Settlement ? Settlement->FindNearestStorageBuilding(GetActorLocation()) : nullptr)
		{
			Task = ETask::Deliver;
			Target = Storage;
			SetActivity(TEXT("bringt Waren ins Lager"));
		}
	}
}

void AGamerVillager::Leave(const FString& Reason)
{
	AGamerSettlement* Settlement = GetSettlement();
	if (Settlement)
	{
		if (!HeldTool.IsNone())
		{
			Settlement->AddToStorage(HeldTool, 1);
		}
		for (const FGamerItemAmount& Amount : Carry)
		{
			Settlement->AddToStorage(Amount.Item, Amount.Count);
		}
		Settlement->Broadcast(FString::Printf(TEXT("%s hat die Siedlung verlassen (%s)."), *VillagerName, *Reason));
	}
	UE_LOG(LogGamer, Log, TEXT("Villager %s left the settlement: %s"), *VillagerName, *Reason);
	Bed = nullptr;
	Destroy();
}

bool AGamerVillager::FindPath(const FVector& Destination)
{
	PathPoints.Reset();
	PathIndex = 0;
	PathGoal = Destination;
	RepathTimer = RepathSecondsWithoutPath;

	UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
	const ANavigationData* NavData = NavSys ? NavSys->GetDefaultNavDataInstance(FNavigationSystem::DontCreate) : nullptr;
	if (!NavData)
	{
		return false;
	}
	FPathFindingQuery Query(this, *NavData, GetNavAgentLocation(), Destination, NavData->GetDefaultQueryFilter());
	Query.SetAllowPartialPaths(true);
	Query.SetRequireNavigableEndLocation(false); // the target itself (tree, storage) blocks the mesh: walk as close as possible
	const FPathFindingResult Result = NavSys->FindPathSync(Query);
	if (!Result.IsSuccessful() || !Result.Path.IsValid() || Result.Path->GetPathPoints().Num() < 2)
	{
		return false;
	}
	for (const FNavPathPoint& Point : Result.Path->GetPathPoints())
	{
		PathPoints.Add(Point.Location);
	}
	PathIndex = 1; // point 0 is where the villager stands
	RepathTimer = RepathSeconds;
	UE_LOG(LogGamer, Verbose, TEXT("Villager %s: path with %d points%s, %.0f cm to the goal at the end"), *VillagerName, PathPoints.Num(),
		Result.IsPartial() ? TEXT(" (partial)") : TEXT(""), FVector::Dist2D(PathPoints.Last(), Destination));
	return true;
}

bool AGamerVillager::MoveTo(const FVector& Destination, float Accept, float DeltaSeconds)
{
	const FVector Location = GetActorLocation();
	FVector Delta = Destination - Location;
	Delta.Z = 0.f;
	const float Distance = Delta.Size();
	if (Distance <= Accept)
	{
		StuckTimer = 0.f;
		StuckCount = 0;
		LastPosition = Location;
		PathPoints.Reset();
		RepathTimer = 0.f;
		return true;
	}

	// Path from the navigation mesh: again for a new goal, every few seconds (new buildings, tiles appearing
	// around the walking villager) and after getting stuck. After the last path point: straight to the goal.
	RepathTimer -= DeltaSeconds;
	if (RepathTimer <= 0.f || FVector::Dist2D(PathGoal, Destination) > 100.f)
	{
		if (FindPath(Destination))
		{
			++PathsFound;
		}
		else
		{
			++PathsMissing;
			if (PathsMissing == 1 || PathsMissing == 10 || PathsMissing % 100 == 0)
			{
				UE_LOG(LogGamer, Log, TEXT("Villager %s: no navigation path, walking straight (%d times so far)"), *VillagerName, PathsMissing);
			}
		}
	}
	FVector Aim = Destination;
	while (PathPoints.IsValidIndex(PathIndex))
	{
		if (FVector::Dist2D(Location, PathPoints[PathIndex]) > WaypointReach)
		{
			Aim = PathPoints[PathIndex];
			break;
		}
		++PathIndex;
	}
	FVector Direction = (Aim - Location).GetSafeNormal2D();
	if (Direction.IsNearlyZero())
	{
		Direction = Delta / Distance;
	}
	if (SidestepTimer > 0.f)
	{
		SidestepTimer -= DeltaSeconds;
		Direction = (Direction + FVector::CrossProduct(FVector::UpVector, Direction) * SidestepSign * 1.5f).GetSafeNormal();
	}
	AddMovementInput(Direction, 1.f);

	// Stuck (blocked by a tree, rock or wall): side-step, alternating; give up after a while.
	StuckTimer += DeltaSeconds;
	if (StuckTimer >= 1.f)
	{
		const float Moved = FVector::Dist2D(GetActorLocation(), LastPosition);
		LastPosition = GetActorLocation();
		StuckTimer = 0.f;
		if (Moved < 40.f)
		{
			++StuckCount;
			SidestepTimer = 1.2f;
			SidestepSign = (StuckCount % 2) ? 1.f : -1.f;
			RepathTimer = 0.f; // look for a new path from wherever the side-step leads
			if (StuckCount >= 6)
			{
				if (Target.IsValid())
				{
					Unreachable.Add(Target, GetWorld()->GetTimeSeconds() + 60.0);
				}
				UE_LOG(LogGamer, Log, TEXT("Villager %s: cannot reach the target, trying something else"), *VillagerName);
				SetActivity(TEXT("kommt nicht hin"));
				FinishTask();
			}
		}
		else
		{
			StuckCount = 0;
		}
	}
	return false;
}

// --- Tavern: cook at the cooking places, innkeeper, meals in the tavern (Taverne-Session 2026-11-15) ---

AActor* AGamerVillager::CraftStationFor(const FGamerRecipeDef& Recipe, const FGamerJobDef& Job, AActor* JobStation) const
{
	if (Recipe.Station.IsNone() || Recipe.Station == Job.Station)
	{
		return JobStation;
	}
	// A cooking place of the recipe near the job's building (tavern), e.g. clay oven or camp fire.
	return JobStation ? AGamerTavern::FindNear(GetWorld(), Recipe.Station, 1, JobStation->GetActorLocation(), Job.Radius) : nullptr;
}

AGamerTavern* AGamerVillager::FindTavernWithFood() const
{
	const FGamerNeedDef* Food = NeedOfKind(EGamerNeedKind::Food);
	AGamerTavern* Best = nullptr;
	float BestDistance = TNumericLimits<float>::Max();
	for (TActorIterator<AGamerTavern> It(GetWorld()); Food && It; ++It)
	{
		if (It->IsPreview() || !It->IsFinished() || Unreachable.Contains(TWeakObjectPtr<AActor>(*It)))
		{
			continue;
		}
		AGamerTavern* Tavern = *It;
		const bool bHasFood = Food->Foods.ContainsByPredicate([Tavern](const FGamerFoodValue& F) { return Tavern->CountStock(F.Item) > 0; });
		const float Distance = FVector::Dist(Tavern->GetActorLocation(), GetActorLocation());
		if (bHasFood && Distance < BestDistance)
		{
			BestDistance = Distance;
			Best = Tavern;
		}
	}
	return Best;
}

bool AGamerVillager::PlanServe(const FGamerJobDef& Job, AActor* Station, AActor* Storage, FString& OutWhyNot)
{
	AGamerTavern* Tavern = Cast<AGamerTavern>(Station);
	AGamerSettlement* Settlement = GetSettlement();
	const UGamerDataSubsystem* Data = UGamerDataSubsystem::Get(this);
	if (!Tavern || !Settlement || !Data)
	{
		OutWhyNot = TEXT("keine Taverne");
		return false;
	}
	const int32 Free = FMath::Min(Tavern->FreeSpace(), Job.StockTarget - Tavern->TotalStock());
	if (Free <= 0)
	{
		OutWhyNot = TEXT("die Taverne ist voll");
		return false;
	}
	// The dish or drink the tavern has least of (only things with meal buffs, i.e. cooked).
	FName Pick;
	int32 PickStock = TNumericLimits<int32>::Max();
	for (const FGamerItemAmount& Amount : Settlement->GetStorage())
	{
		const FGamerItemDef* Def = Data->FindItem(Amount.Item);
		const int32 InTavern = Tavern->CountStock(Amount.Item);
		if (Def && Def->Food.IsMeal() && Amount.Count > 0 && InTavern < PickStock)
		{
			Pick = Amount.Item;
			PickStock = InTavern;
		}
	}
	if (Pick.IsNone())
	{
		OutWhyNot = TEXT("keine Gerichte im Lager");
		return false;
	}
	ServeItem = Pick;
	ServeCount = 0;
	const int32 Want = FMath::Min3(Free, 4, Settlement->CountInStorage(Pick));
	ServeTavern = Tavern;
	Task = ETask::ServeFetch;
	Target = Storage;
	// ServeCount is set when the pieces are taken at the storage.
	ServeWanted = Want;
	SetActivity(FString::Printf(TEXT("holt %s für die Taverne"), *VillagerItemName(Data, Pick)));
	return true;
}

void AGamerVillager::OnTavernMeal(float MoodGain)
{
	if (MoodGain <= 0.f)
	{
		return;
	}
	AddMood(MoodGain);
	UE_LOG(LogGamer, Log, TEXT("Villager %s: tavern mood +%.2f (bonus %.2f, mood %.2f)"), *VillagerName, MoodGain, PendingMoodGain, Mood);
}

void AGamerVillager::AddMood(float Delta)
{
	PendingMoodGain = FMath::Clamp(PendingMoodGain + Delta, 0.f, 0.4f);
}
