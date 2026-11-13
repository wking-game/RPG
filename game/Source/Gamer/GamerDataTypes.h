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
