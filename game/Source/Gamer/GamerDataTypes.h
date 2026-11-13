#pragma once

#include "CoreMinimal.h"
#include "UObject/SoftObjectPath.h"
#include "GamerDataTypes.generated.h"

// Row types of the JSON data tables in Content/Data (format: docs/06_datenformat.md).
// Values are read-only at runtime; gameplay code looks rows up by ID via UGamerDataSubsystem.

UENUM(BlueprintType)
enum class EGamerItemCategory : uint8
{
	Resource,
	Material,
	Food,
	Tool
};

UENUM(BlueprintType)
enum class EGamerBuildCategory : uint8
{
	Structure,
	Workstation,
	Storage,
	Comfort
};

USTRUCT(BlueprintType)
struct FGamerItemAmount
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Data")
	FName Item;

	UPROPERTY(BlueprintReadOnly, Category = "Data")
	int32 Count = 0;
};

USTRUCT(BlueprintType)
struct FGamerToolRequirement
{
	GENERATED_BODY()

	/** Tool type (e.g. "axe"). None = can be done by hand. */
	UPROPERTY(BlueprintReadOnly, Category = "Data")
	FName Type;

	UPROPERTY(BlueprintReadOnly, Category = "Data")
	int32 MinTier = 0;

	bool IsRequired() const { return !Type.IsNone(); }
};

USTRUCT(BlueprintType)
struct FGamerToolProps
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Data")
	FName Type;

	UPROPERTY(BlueprintReadOnly, Category = "Data")
	int32 Durability = 0;

	UPROPERTY(BlueprintReadOnly, Category = "Data")
	float WorkSpeed = 1.f;
};

USTRUCT(BlueprintType)
struct FGamerItemDef
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Data")
	FName Id;

	UPROPERTY(BlueprintReadOnly, Category = "Data")
	FString Name;

	UPROPERTY(BlueprintReadOnly, Category = "Data")
	EGamerItemCategory Category = EGamerItemCategory::Resource;

	UPROPERTY(BlueprintReadOnly, Category = "Data")
	int32 Tier = 1;

	UPROPERTY(BlueprintReadOnly, Category = "Data")
	int32 MaxStack = 1;

	UPROPERTY(BlueprintReadOnly, Category = "Data")
	float WeightKg = 0.f;

	UPROPERTY(BlueprintReadOnly, Category = "Data")
	FSoftObjectPath Icon;

	UPROPERTY(BlueprintReadOnly, Category = "Data")
	FSoftObjectPath WorldMesh;

	/** Only meaningful when Category == Tool. */
	UPROPERTY(BlueprintReadOnly, Category = "Data")
	FGamerToolProps Tool;

	UPROPERTY(BlueprintReadOnly, Category = "Data")
	bool bDeprecated = false;
};

USTRUCT(BlueprintType)
struct FGamerDrop
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Data")
	FName Item;

	UPROPERTY(BlueprintReadOnly, Category = "Data")
	int32 Min = 0;

	UPROPERTY(BlueprintReadOnly, Category = "Data")
	int32 Max = 0;
};

USTRUCT(BlueprintType)
struct FGamerResourceNodeDef
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Data")
	FName Id;

	UPROPERTY(BlueprintReadOnly, Category = "Data")
	FString Name;

	UPROPERTY(BlueprintReadOnly, Category = "Data")
	FGamerToolRequirement RequiredTool;

	UPROPERTY(BlueprintReadOnly, Category = "Data")
	float WorkSeconds = 0.f;

	UPROPERTY(BlueprintReadOnly, Category = "Data")
	TArray<FGamerDrop> Drops;

	/** Game days until it grows back. 0 = never. */
	UPROPERTY(BlueprintReadOnly, Category = "Data")
	float RegrowDays = 0.f;

	UPROPERTY(BlueprintReadOnly, Category = "Data")
	FSoftObjectPath MeshFull;

	UPROPERTY(BlueprintReadOnly, Category = "Data")
	FSoftObjectPath MeshDepleted;

	UPROPERTY(BlueprintReadOnly, Category = "Data")
	FSoftObjectPath MeshRemains;

	UPROPERTY(BlueprintReadOnly, Category = "Data")
	bool bDeprecated = false;
};

USTRUCT(BlueprintType)
struct FGamerRecipeDef
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Data")
	FName Id;

	/** Buildable ID of the station. None = crafted by hand. */
	UPROPERTY(BlueprintReadOnly, Category = "Data")
	FName Station;

	UPROPERTY(BlueprintReadOnly, Category = "Data")
	TArray<FGamerItemAmount> Inputs;

	UPROPERTY(BlueprintReadOnly, Category = "Data")
	TArray<FGamerItemAmount> Outputs;

	UPROPERTY(BlueprintReadOnly, Category = "Data")
	float WorkSeconds = 0.f;

	UPROPERTY(BlueprintReadOnly, Category = "Data")
	bool bDeprecated = false;
};

USTRUCT(BlueprintType)
struct FGamerBuildableDef
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Data")
	FName Id;

	UPROPERTY(BlueprintReadOnly, Category = "Data")
	FString Name;

	UPROPERTY(BlueprintReadOnly, Category = "Data")
	EGamerBuildCategory Category = EGamerBuildCategory::Structure;

	/** Snap type for structures (foundation, wall, roof ...). None for other categories. */
	UPROPERTY(BlueprintReadOnly, Category = "Data")
	FName Snap;

	UPROPERTY(BlueprintReadOnly, Category = "Data")
	FSoftObjectPath Mesh;

	UPROPERTY(BlueprintReadOnly, Category = "Data")
	TArray<FGamerItemAmount> Cost;

	UPROPERTY(BlueprintReadOnly, Category = "Data")
	float BuildSeconds = 0.f;

	UPROPERTY(BlueprintReadOnly, Category = "Data")
	FGamerToolRequirement RequiredTool;

	UPROPERTY(BlueprintReadOnly, Category = "Data")
	bool bDeprecated = false;
};

UENUM(BlueprintType)
enum class EGamerJobKind : uint8
{
	/** Fell trees, mine rocks or gather by hand near the job's station (or the storage). */
	Harvest,
	/** Make recipes at the station from storage ingredients (cook). */
	Craft,
	/** Hammer unfinished building plans. */
	Build
};

/** A villager job (jobs.json). The player assigns jobs in the village management. */
USTRUCT(BlueprintType)
struct FGamerJobDef
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Data")
	FName Id;

	UPROPERTY(BlueprintReadOnly, Category = "Data")
	FString Name;

	UPROPERTY(BlueprintReadOnly, Category = "Data")
	EGamerJobKind Kind = EGamerJobKind::Harvest;

	/** Buildable the job works from (must be finished). None = works from the storage. */
	UPROPERTY(BlueprintReadOnly, Category = "Data")
	FName Station;

	/** Tool type the villager takes from the storage (axe, pickaxe, hammer). None = by hand. */
	UPROPERTY(BlueprintReadOnly, Category = "Data")
	FName Tool;

	/** Craft jobs: recipes they make. */
	UPROPERTY(BlueprintReadOnly, Category = "Data")
	TArray<FName> Recipes;

	/** Craft jobs: stop when the storage holds this many of the output. */
	UPROPERTY(BlueprintReadOnly, Category = "Data")
	int32 StockTarget = 10;

	/** Work area around the station or storage (cm). */
	UPROPERTY(BlueprintReadOnly, Category = "Data")
	float Radius = 3000.f;

	/** The job a villager falls back to when its own job is not possible (exactly one job). */
	UPROPERTY(BlueprintReadOnly, Category = "Data")
	bool bFallback = false;

	UPROPERTY(BlueprintReadOnly, Category = "Data")
	bool bDeprecated = false;
};

UENUM(BlueprintType)
enum class EGamerNeedKind : uint8
{
	Food,
	Sleep,
	Shelter
};

USTRUCT(BlueprintType)
struct FGamerFoodValue
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Data")
	FName Item;

	/** How much one piece fills the need (0..1). */
	UPROPERTY(BlueprintReadOnly, Category = "Data")
	float Value = 0.f;
};

/** A villager need (needs.json). Values run from 1 (fine) to 0 (empty). */
USTRUCT(BlueprintType)
struct FGamerNeedDef
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Data")
	FName Id;

	UPROPERTY(BlueprintReadOnly, Category = "Data")
	FString Name;

	UPROPERTY(BlueprintReadOnly, Category = "Data")
	EGamerNeedKind Kind = EGamerNeedKind::Food;

	/** Loss per game day while awake (food, sleep). */
	UPROPERTY(BlueprintReadOnly, Category = "Data")
	float DecayPerDay = 1.f;

	/** Below this the villager takes care of the need first. */
	UPROPERTY(BlueprintReadOnly, Category = "Data")
	float Threshold = 0.3f;

	/** Sleep: gain per game day while sleeping in a bed (half without a bed). */
	UPROPERTY(BlueprintReadOnly, Category = "Data")
	float RestorePerDay = 0.f;

	/** Food: what may be eaten, best first. */
	UPROPERTY(BlueprintReadOnly, Category = "Data")
	TArray<FGamerFoodValue> Foods;

	/** Effect on work speed while the need is not met (e.g. 0.8 = 20 % slower). */
	UPROPERTY(BlueprintReadOnly, Category = "Data")
	float WorkSpeedWhenUnmet = 1.f;

	/** Food: leaves the settlement after this many game days at zero (0 = never). */
	UPROPERTY(BlueprintReadOnly, Category = "Data")
	float LeaveAfterDays = 0.f;

	UPROPERTY(BlueprintReadOnly, Category = "Data")
	bool bDeprecated = false;
};
