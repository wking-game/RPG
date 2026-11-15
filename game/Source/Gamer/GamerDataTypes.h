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
	Tool,
	/** Weapons (ID weapon_..., block "weapon"); combat itself comes in phase 3. */
	Weapon
};

/** items.json "weapon": what a weapon is. Durability works like tools (half damage when worn out). */
USTRUCT(BlueprintType)
struct FGamerWeaponProps
{
	GENERATED_BODY()

	/** Weapon type (bow, dagger, bearded_axe, round_shield, cleaver, rune_staff ...). */
	UPROPERTY(BlueprintReadOnly, Category = "Data")
	FName Type;

	/** Damage per hit (shields: blocked damage). */
	UPROPERTY(BlueprintReadOnly, Category = "Data")
	float Damage = 0.f;

	UPROPERTY(BlueprintReadOnly, Category = "Data")
	int32 Durability = 0;
};

UENUM(BlueprintType)
enum class EGamerBuildCategory : uint8
{
	Structure,
	Workstation,
	Storage,
	Comfort,
	/** Fields, composter, rain catcher (block "farm" in buildables.json). */
	Farm
};

/** What a farm building does (buildables.json "farm": { "kind": ... }). */
UENUM(BlueprintType)
enum class EGamerFarmKind : uint8
{
	None,
	/** Bed: sow a seed, water, fertilize, harvest. */
	Field,
	/** Turns spoiled food into fertilizer over time. */
	Composter,
	/** Fills up with water over time (primitive rain catcher). */
	Water,
	/** Plant a tree: the placed piece turns into a growing resource node (sapling -> young -> full). */
	Tree
};

USTRUCT(BlueprintType)
struct FGamerItemAmount
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Data")
	FName Item;

	UPROPERTY(BlueprintReadOnly, Category = "Data")
	int32 Count = 0;

	/** Only for stored stacks (settlement storage, villager loads): 1 = fresh .. 0 = spoiled. Not read from JSON. */
	UPROPERTY(BlueprintReadOnly, Category = "Data")
	float Freshness = 1.f;
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

/** A seed: what grows from it in a field (items.json "plant"). */
USTRUCT(BlueprintType)
struct FGamerPlantProps
{
	GENERATED_BODY()

	/** Game days from sowing to ripe while the field is watered and the season allows growth. 0 = not a seed. */
	UPROPERTY(BlueprintReadOnly, Category = "Data")
	float GrowDays = 0.f;

	/** What a ripe field gives (before the fertilizer bonus). */
	UPROPERTY(BlueprintReadOnly, Category = "Data")
	TArray<FGamerDrop> Harvest;

	/** Models for the growing stages in the field (sprout, young, ripe); the last one is shown when ripe. */
	UPROPERTY(BlueprintReadOnly, Category = "Data")
	TArray<FSoftObjectPath> Meshes;

	bool IsSeed() const { return GrowDays > 0.f; }
};

/** What a meal buff improves (architecture rule 5: food is a buff system, never a stamina lock). */
UENUM(BlueprintType)
enum class EGamerBuffStat : uint8
{
	/** Max health (points). */
	Health,
	/** Health regeneration (points per second). */
	Regen,
	/** Carry weight (kg). */
	Carry,
	/** Cold resistance (percent). */
	ColdResist,
	/** Poison resistance (percent). */
	PoisonResist,
	/** Damage (percent). */
	Damage
};

USTRUCT(BlueprintType)
struct FGamerBuffEffect
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Data")
	EGamerBuffStat Stat = EGamerBuffStat::Health;

	UPROPERTY(BlueprintReadOnly, Category = "Data")
	float Value = 0.f;
};

/** items.json "food": a dish or drink the player eats for buffs (decision 2026-11-14, docs/12_taverne_gerichte.md). */
USTRUCT(BlueprintType)
struct FGamerFoodProps
{
	GENERATED_BODY()

	/** How long the buffs last (real-time minutes). */
	UPROPERTY(BlueprintReadOnly, Category = "Data")
	float Minutes = 0.f;

	UPROPERTY(BlueprintReadOnly, Category = "Data")
	TArray<FGamerBuffEffect> Buffs;

	bool IsMeal() const { return Minutes > 0.f && Buffs.Num() > 0; }
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

	/** Only meaningful when Category == Weapon. */
	UPROPERTY(BlueprintReadOnly, Category = "Data")
	FGamerWeaponProps Weapon;

	/** Game days until it spoils when carried (storage slower, on the ground faster). 0 = does not spoil. */
	UPROPERTY(BlueprintReadOnly, Category = "Data")
	float SpoilDays = 0.f;

	/** What it turns into when spoiled (default item_spoiled_food). */
	UPROPERTY(BlueprintReadOnly, Category = "Data")
	FName SpoilsInto;

	/** Seeds only: what grows from it in a field. */
	UPROPERTY(BlueprintReadOnly, Category = "Data")
	FGamerPlantProps Plant;

	/** Dishes and drinks: buffs when the player eats it. */
	UPROPERTY(BlueprintReadOnly, Category = "Data")
	FGamerFoodProps Food;

	bool Spoils() const { return SpoilDays > 0.f && !SpoilsInto.IsNone(); }

	UPROPERTY(BlueprintReadOnly, Category = "Data")
	bool bDeprecated = false;
};

/**
 * Yearly cycle of a fruit tree or wild bush (resource_nodes.json "season"), in season phases 0..4
 * (0 spring, 1 summer, 2 autumn, 3 winter). Order: bloom < unripe < ripe <= fall <= bare.
 * The node's single drop is the fruit of the whole year; it can only be picked while ripe.
 */
USTRUCT(BlueprintType)
struct FGamerSeasonProps
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Data")
	float Bloom = 0.f;

	UPROPERTY(BlueprintReadOnly, Category = "Data")
	float Unripe = 0.f;

	UPROPERTY(BlueprintReadOnly, Category = "Data")
	float Ripe = 0.f;

	/** From here unpicked fruit falls off bit by bit (as items on the ground). Equal to Bare = nothing falls. */
	UPROPERTY(BlueprintReadOnly, Category = "Data")
	float Fall = 0.f;

	/** From here until the next bloom: no fruit (always before winter for fruit trees). */
	UPROPERTY(BlueprintReadOnly, Category = "Data")
	float Bare = 0.f;

	/** Fruit taken per pick (0 = everything at once). */
	UPROPERTY(BlueprintReadOnly, Category = "Data")
	int32 Pick = 0;

	/** Height range (cm above the ground) fruit falls from. */
	UPROPERTY(BlueprintReadOnly, Category = "Data")
	float DropHeightMin = 0.f;

	UPROPERTY(BlueprintReadOnly, Category = "Data")
	float DropHeightMax = 0.f;

	/** Falling further than this bruises the fruit (becomes BruisedItem). 0 = never. */
	UPROPERTY(BlueprintReadOnly, Category = "Data")
	float BruiseHeight = 0.f;

	UPROPERTY(BlueprintReadOnly, Category = "Data")
	FName BruisedItem;

	bool IsSet() const { return Bare > 0.f; }
	bool Falls() const { return IsSet() && Fall < Bare; }
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

	/** Shown one after another while a used-up node grows back (e.g. sapling, young tree). */
	UPROPERTY(BlueprintReadOnly, Category = "Data")
	TArray<FSoftObjectPath> MeshGrowing;

	UPROPERTY(BlueprintReadOnly, Category = "Data")
	FSoftObjectPath MeshDepleted;

	UPROPERTY(BlueprintReadOnly, Category = "Data")
	FSoftObjectPath MeshRemains;

	/** Season nodes: models while blooming and with unripe fruit (else the depleted model is shown). */
	UPROPERTY(BlueprintReadOnly, Category = "Data")
	FSoftObjectPath MeshBloom;

	UPROPERTY(BlueprintReadOnly, Category = "Data")
	FSoftObjectPath MeshUnripe;

	/** Yearly fruit cycle (fruit trees, wild bushes). Not set = regrows after RegrowDays as before. */
	UPROPERTY(BlueprintReadOnly, Category = "Data")
	FGamerSeasonProps Season;

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

/** buildables.json "farm": what a field, composter or rain catcher does. Times in game days. */
USTRUCT(BlueprintType)
struct FGamerFarmProps
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Data")
	EGamerFarmKind Kind = EGamerFarmKind::None;

	// --- field ---

	/** Water used per game day (a full field holds 1). */
	UPROPERTY(BlueprintReadOnly, Category = "Data")
	float WaterPerDay = 1.f;

	/** Item that waters the field (fills it to 1). */
	UPROPERTY(BlueprintReadOnly, Category = "Data")
	FName WaterItem;

	/** Item that fertilizes the field (once per sowing). */
	UPROPERTY(BlueprintReadOnly, Category = "Data")
	FName FertilizerItem;

	/** Extra harvest with fertilizer (0.5 = +50 %). */
	UPROPERTY(BlueprintReadOnly, Category = "Data")
	float FertilizerBonus = 0.f;

	/** Field drawn as a rectangle by the player: side length limits (cm) and the area of one "unit" (m²).
	 *  Seeds, water, fertilizer, cost and harvest are counted per unit. */
	UPROPERTY(BlueprintReadOnly, Category = "Data")
	float MinSizeCm = 200.f;

	UPROPERTY(BlueprintReadOnly, Category = "Data")
	float MaxSizeCm = 1200.f;

	UPROPERTY(BlueprintReadOnly, Category = "Data")
	float UnitM2 = 4.5f;

	/** Season phases in which plants grow (e.g. 0 .. 3 = not in winter). */
	UPROPERTY(BlueprintReadOnly, Category = "Data")
	float GrowFrom = 0.f;

	UPROPERTY(BlueprintReadOnly, Category = "Data")
	float GrowTo = 4.f;

	// --- composter ---

	/** What may go in (composter). */
	UPROPERTY(BlueprintReadOnly, Category = "Data")
	TArray<FName> Inputs;

	/** Pieces put in per output piece (composter). */
	UPROPERTY(BlueprintReadOnly, Category = "Data")
	int32 PerOutput = 1;

	// --- composter and water ---

	/** What comes out (fertilizer, water). */
	UPROPERTY(BlueprintReadOnly, Category = "Data")
	FName Output;

	/** Composter: game days per output piece. Water: pieces per game day. */
	UPROPERTY(BlueprintReadOnly, Category = "Data")
	float Rate = 1.f;

	/** Composter: input pieces it holds. Water: output pieces it holds. */
	UPROPERTY(BlueprintReadOnly, Category = "Data")
	int32 Capacity = 1;

	// --- tree ---

	/** Resource node that grows from the planted sapling. */
	UPROPERTY(BlueprintReadOnly, Category = "Data")
	FName Node;

	/** Game days from planting to a full tree. */
	UPROPERTY(BlueprintReadOnly, Category = "Data")
	float TreeGrowDays = 1.f;
};

/** One upgrade stage of a work building (buildables.json "upgrades"): what it costs, how many work places it then has. */
USTRUCT(BlueprintType)
struct FGamerUpgradeDef
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Data")
	TArray<FGamerItemAmount> Cost;

	UPROPERTY(BlueprintReadOnly, Category = "Data")
	int32 Workers = 0;
};

/** buildables.json "requires": may only be placed near a finished building of at least that level (cooking places near the tavern). */
USTRUCT(BlueprintType)
struct FGamerBuildRequirement
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Data")
	FName Building;

	UPROPERTY(BlueprintReadOnly, Category = "Data")
	int32 Level = 1;

	UPROPERTY(BlueprintReadOnly, Category = "Data")
	float RadiusCm = 1500.f;

	bool IsSet() const { return !Building.IsNone(); }
};

/** buildables.json "tavern": the tavern keeps a small stock of dishes and drinks the innkeeper brings from the storage. */
USTRUCT(BlueprintType)
struct FGamerTavernProps
{
	GENERATED_BODY()

	/** Pieces the tavern holds (0 = not a tavern). */
	UPROPERTY(BlueprintReadOnly, Category = "Data")
	int32 Stock = 0;

	/** Villager mood from a meal there: per tier of the dish, plus a flat bonus for a drink (0..1 scale). */
	UPROPERTY(BlueprintReadOnly, Category = "Data")
	float MoodPerTier = 0.f;

	UPROPERTY(BlueprintReadOnly, Category = "Data")
	float MoodDrink = 0.f;

	bool IsTavern() const { return Stock > 0; }
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

	/** Category Farm only. */
	UPROPERTY(BlueprintReadOnly, Category = "Data")
	FGamerFarmProps Farm;

	/** Work places at level 1 for the jobs that use this building as station (0 = no limit). */
	UPROPERTY(BlueprintReadOnly, Category = "Data")
	int32 Workers = 0;

	/** Upgrade stages: level 2, 3, ... (decision 2026-11-14: every work building can be upgraded for more workers). */
	UPROPERTY(BlueprintReadOnly, Category = "Data")
	TArray<FGamerUpgradeDef> Upgrades;

	/** Placement only near a finished building of a level (e.g. clay oven: tavern level 2 within 15 m). */
	UPROPERTY(BlueprintReadOnly, Category = "Data")
	FGamerBuildRequirement Requires;

	/** Tavern only. */
	UPROPERTY(BlueprintReadOnly, Category = "Data")
	FGamerTavernProps Tavern;

	int32 MaxLevel() const { return 1 + Upgrades.Num(); }
	int32 WorkersAt(int32 Level) const
	{
		return Level >= 2 && Upgrades.IsValidIndex(FMath::Min(Level, MaxLevel()) - 2) ? Upgrades[FMath::Min(Level, MaxLevel()) - 2].Workers : Workers;
	}
	bool IsAreaPiece() const { return Farm.Kind == EGamerFarmKind::Field; }

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
	Build,
	/** Work the fields around the station (farmstead): harvest, sow, water, fertilize, fetch rain water. */
	Farm,
	/** Innkeeper: carry dishes and drinks from the storage into the tavern (station) up to its stock. */
	Serve
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
