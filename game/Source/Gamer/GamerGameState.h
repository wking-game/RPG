#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "GamerSaveable.h"
#include "GamerGameState.generated.h"

/** How a placed building piece comes into being (world option). */
UENUM()
enum class EGamerBuildFlow : uint8
{
	/** Material is taken when placing; a see-through plan is hammered until finished. */
	Blueprint,
	/** Material is taken when placing; the piece is finished at once. */
	Instant,
	/** The plan costs nothing; every hammer strike takes its share of the material. */
	MaterialOnBuild
};

/** How freely pieces are placed (world option). */
UENUM()
enum class EGamerPlacement : uint8
{
	/** First foundation anywhere at any angle; everything else snaps to existing pieces (300 cm grid). */
	FreeSnap,
	/** One fixed 300 cm world grid, rotation in 90 degree steps. */
	WorldGrid
};

/** What tearing down a finished piece gives back (world option). Unfinished plans always give back everything spent. */
UENUM()
enum class EGamerDemolish : uint8
{
	FullRefund,
	HalfRefund,
	Disabled
};

/** How fast the magic sense (listening) improves in this world: a factor on the experience (world option, 2026-11-15). */
UENUM()
enum class EGamerGespuerTempo : uint8
{
	/** x0.6 */
	Slow,
	/** x1 */
	Normal,
	/** x1.6 */
	Fast
};

/**
 * Rules of one world, chosen by the host when creating it (decision 2026-11-13: in the start menu).
 * Until the start menu exists they come from the map URL: ?BuildFlow=blueprint|instant|material
 * ?Placement=free|grid ?Demolish=full|half|off (tools/play_local.ps1 -BuildFlow ... passes them).
 */
USTRUCT()
struct FGamerWorldOptions
{
	GENERATED_BODY()

	UPROPERTY()
	EGamerBuildFlow BuildFlow = EGamerBuildFlow::Blueprint;

	UPROPERTY()
	EGamerPlacement Placement = EGamerPlacement::FreeSnap;

	UPROPERTY()
	EGamerDemolish Demolish = EGamerDemolish::FullRefund;

	/** Real minutes per season (spring, summer, autumn, winter). Decision 2026-11-13: 60 (one year = 4 hours). ?SeasonMinutes=60 */
	UPROPERTY()
	float SeasonMinutes = 60.f;

	/**
	 * Player inventory (decision 2026-11-15, chosen when creating a world): false = it stays in this world (saved with
	 * the world), true = it travels with the character into every world (kept in the player's character file).
	 * ?Inventory=world|character
	 */
	UPROPERTY()
	bool bCarryInventory = false;

	/** Tempo of the magic sense (decision 2026-11-15): slow x0.6, normal x1, fast x1.6 on the experience (values in listen.json). ?Tempo=slow|normal|fast */
	UPROPERTY()
	EGamerGespuerTempo GespuerTempo = EGamerGespuerTempo::Normal;

	/** Reads the options from a map URL option string; unknown values keep the default and are logged. */
	static FGamerWorldOptions FromUrlOptions(const FString& Options);

	/** German one-liner for the menu ("Bauplan + Hammer · frei mit Einrasten · Abreißen: volles Material"). */
	FString Describe() const;

	/** Short form for logs and tests ("BuildFlow=blueprint Placement=free Demolish=full"). */
	FString ToLogString() const;

	/** Map URL options for starting a world with these options ("?BuildFlow=blueprint?Placement=free?Demolish=full"). */
	FString ToUrlOptions() const;
};

/**
 * Replicated game state: world options for everyone (clients need them for the build preview) and the season.
 *
 * Season (decision 2026-11-13): a phase 0..4, cyclic: 0 = start of spring, 1 = summer, 2 = autumn, 3 = winter.
 * The server owns it (anchor phase + anchor time); every machine computes the same phase from the synced
 * server time, so nothing is sent while it simply runs. Machines with rendering push it into the material
 * parameter collection MPC_GamerWorld (scalar "SeasonPhase"), which the vegetation materials read.
 */
UCLASS()
class GAMER_API AGamerGameState : public AGameStateBase, public IGamerSaveable
{
	GENERATED_BODY()

public:
	// IGamerSaveable (save system, implemented in GamerSaveImpl.cpp)
	virtual FString GetSaveKey() const override { return TEXT("game_state"); }
	virtual void WriteSaveState(FGamerSaveRecord& Out) const override;
	virtual void ReadSaveState(const FGamerSaveRecord& In) override;

public:
	AGamerGameState();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	/** Current season phase 0..4 (same value on server and clients). */
	float GetSeasonPhase() const;

	/** Server: jump to a season phase (tests, later save games); the season keeps running from there. */
	void SetSeasonPhase(float Phase);

	/** German text for a phase: "Sommer, Mitte". */
	static FString DescribeSeason(float Phase);

	/** Server: set once when the world starts. */
	void SetWorldOptions(const FGamerWorldOptions& InOptions);
	const FGamerWorldOptions& GetWorldOptions() const { return WorldOptions; }

	/** Options of the world the object lives in (defaults if there is no game state yet). */
	static const FGamerWorldOptions& Get(const UObject* WorldContext);

protected:
	UPROPERTY(Replicated)
	FGamerWorldOptions WorldOptions;

	/** Phase of a new world: late spring, so the first hour shows spring turning into summer. */
	UPROPERTY(EditDefaultsOnly, Category = "Season")
	float StartSeasonPhase = 0.6f;

	/** Collection read by the vegetation materials (scalar SeasonPhase). */
	UPROPERTY(EditDefaultsOnly, Category = "Season")
	TSoftObjectPtr<class UMaterialParameterCollection> WorldParameters;

	/** Season phase at SeasonAnchorTime (server world seconds). */
	UPROPERTY(Replicated)
	float SeasonAnchorPhase = 0.6f;

	UPROPERTY(Replicated)
	double SeasonAnchorTime = 0.0;

private:
	void PushSeasonToMaterials();

	bool bWarnedMissingParameters = false;
	float LoggedSeasonQuarter = -1.f;
};
