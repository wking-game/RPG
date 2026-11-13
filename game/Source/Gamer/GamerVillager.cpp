#include "GamerVillager.h"

#include "Gamer.h"
#include "GamerBuilding.h"
#include "GamerDataSubsystem.h"
#include "GamerGameState.h"
#include "GamerPlayerController.h"
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

	FString ItemName(const UGamerDataSubsystem* Data, FName Id)
	{
		const FGamerItemDef* Def = Data ? Data->FindItem(Id) : nullptr;
		return Def ? Def->Name : Id.ToString();
	}

	FString AmountsText(const UGamerDataSubsystem* Data, const TArray<FGamerItemAmount>& Amounts)
	{
		TArray<FString> Parts;
		for (const FGamerItemAmount& Amount : Amounts)
		{
			Parts.Add(FString::Printf(TEXT("%d %s"), Amount.Count, *ItemName(Data, Amount.Item)));
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

	static ConstructorHelpers::FObjectFinder<UStaticMesh> CylinderMesh(TEXT("/Engine/BasicShapes/Cylinder.Cylinder"));
	Body = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Body"));
	Body->SetupAttachment(GetCapsuleComponent());
	Body->SetRelativeScale3D(FVector(0.8f, 0.8f, 1.84f));
	Body->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	if (CylinderMesh.Succeeded())
	{
		Body->SetStaticMesh(CylinderMesh.Object);
	}

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
	DOREPLIFETIME(AGamerVillager, Hunger);
	DOREPLIFETIME(AGamerVillager, Sleep);
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

void AGamerVillager::SetJob(FName InJobId)
{
	const UGamerDataSubsystem* Data = UGamerDataSubsystem::Get(this);
	if (!HasAuthority() || InJobId == JobId || !Data || !Data->FindJob(InJobId))
	{
		return;
	}
	JobId = InJobId;
	UE_LOG(LogGamer, Log, TEXT("Villager %s: job set to %s"), *VillagerName, *JobId.ToString());
	if (Task != ETask::Sleep && Task != ETask::Eat)
	{
		FinishTask(); // start the new job right away (the carried goods stay)
	}
	ApplyLook();
	ForceNetUpdate();
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
	if (Body && Body->GetStaticMesh())
	{
		if (UMaterialInstanceDynamic* Material = Body->CreateAndSetMaterialInstanceDynamic(0))
		{
			Material->SetVectorParameterValue(TEXT("Color"), JobColor(JobId));
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
}

void AGamerVillager::Think()
{
	AGamerSettlement* Settlement = GetSettlement();
	const UGamerDataSubsystem* Data = UGamerDataSubsystem::Get(this);
	AGamerBuilding* Storage = Settlement ? Settlement->FindNearestStorageBuilding(GetActorLocation()) : nullptr;

	// 1. Hunger, if there is food in the storage.
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
		SetActivity(FString::Printf(TEXT("holt %s aus dem Lager"), *ItemName(Data, ToolToFetch)));
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
				OutWhyNot = FString::Printf(TEXT("genug %s im Lager"), *ItemName(Data, Output.Item));
				continue;
			}
			if (!Settlement->HasInStorage(Recipe->Inputs))
			{
				OutWhyNot = FString::Printf(TEXT("für %s fehlt: %s"), *ItemName(Data, Output.Item), *AmountsText(Data, Recipe->Inputs));
				continue;
			}
			TaskRecipe = RecipeId;
			Task = ETask::FetchIngredients;
			Target = Storage;
			SetActivity(FString::Printf(TEXT("holt Zutaten für %s"), *ItemName(Data, Output.Item)));
			return true;
		}
		return false;
	}
	case EGamerJobKind::Build:
	{
		TArray<FGamerItemAmount> Missing;
		AGamerBuilding* Plan = FindPlan(Center, Job.Radius, Missing);
		if (!Plan)
		{
			OutWhyNot = Missing.Num() > 0 ? TEXT("für den Bauplan fehlt: ") + AmountsText(Data, Missing) : FString(TEXT("keine Baupläne"));
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
		// At the storage: hand in what is carried, then eat the best food until full.
		for (const FGamerItemAmount& Amount : Carry)
		{
			Settlement->AddToStorage(Amount.Item, Amount.Count);
		}
		Carry.Reset();
		TArray<FString> Eaten;
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
				Eaten.Add(ItemName(Data, Choice->Item));
			}
		}
		UE_LOG(LogGamer, Log, TEXT("Villager %s ate [%s], hunger %.2f"), *VillagerName, *FString::Join(Eaten, TEXT(", ")), Hunger);
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
				Delivered.Add(FString::Printf(TEXT("%d %s"), Amount.Count - NotFitting, *ItemName(Data, Amount.Item)));
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
			TArray<FGamerItemAmount> Gained;
			for (const FGamerDrop& Drop : Def->Drops)
			{
				const int32 Count = FMath::RandRange(Drop.Min, Drop.Max);
				AddCarry(Drop.Item, Count);
				FGamerItemAmount& Entry = Gained.AddDefaulted_GetRef();
				Entry.Item = Drop.Item;
				Entry.Count = Count;
			}
			Node->Deplete();
			UE_LOG(LogGamer, Log, TEXT("Villager %s harvested %s: [%s], carrying %d"), *VillagerName, *Def->Id.ToString(),
				*AmountsText(Data, Gained), CarryCount());
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
		AActor* Station = Job && !Job->Station.IsNone() ? FindStation(Job->Station) : nullptr;
		if (!Recipe || !Station || !Settlement->TakeAllFromStorage(Recipe->Inputs))
		{
			FinishTask();
			break;
		}
		Task = ETask::Craft;
		Target = Station;
		TaskTimer = 0.f;
		SetActivity(FString::Printf(TEXT("kocht %s"), Recipe->Outputs.Num() > 0 ? *ItemName(Data, Recipe->Outputs[0].Item) : TEXT("")));
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
			UE_LOG(LogGamer, Log, TEXT("Villager %s made %s"), *VillagerName, *AmountsText(Data, Recipe->Outputs));
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
