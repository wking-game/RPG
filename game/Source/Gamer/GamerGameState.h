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
	Disabled,
	/** About 80 % of the material of a finished piece (decision 2026-11-22, Gemini GDD v3 idea 1). Appended last: saved worlds keep their meaning. */
	PartialRefund
};

/** Share of the material a finished piece gives back with EGamerDemolish::PartialRefund. */
inline constexpr float PartialRefundFraction = 0.8f;

/** Pieces of one material given back when tearing down. Unfinished plans give back everything spent; finished pieces as the world option says. */
inline int32 DemolishRefundCount(int32 Spent, EGamerDemolish Mode, bool bFinished)
{
	if (!bFinished)
	{
		return Spent;
	}
	switch (Mode)
	{
	case EGamerDemolish::HalfRefund: return Spent / 2;
	case EGamerDemolish::PartialRefund: return static_cast<int32>(static_cast<float>(Spent) * PartialRefundFraction + 0.5f);
	default: return Spent;
	}
}

/** What a player loses when dying (world option, decision 2026-11-16). */
UENUM()
enum class EGamerDeathMode : uint8
{
	/** Nothing is lost. */
	None,
	/** The inventory lies in a grave at the place of death and can be fetched; equipment stays on the body. */
	Grave,
	/** Everything in the inventory is lost. */
	Hardcore
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

/** Game mode of a world (decision 2026-11-16). Saga content does not exist yet; the option is stored for later. */
UENUM()
enum class EGamerPlayMode : uint8
{
	/** Main story as the red thread. */
	Saga,
	/** No main story; side quests, magic and exploring stay. */
	Free
};

/** Difficulty a world was created with (decision 2026-11-16). Presets in Content/Data/difficulty.json; not changeable afterwards. */
UENUM()
enum class EGamerDifficulty : uint8
{
	Easy,
	Medium,
	Hard,
	Hardcore,
	/** Set as soon as one option differs from the preset ("Benutzerdefiniert"). */
	Custom
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

	/** What dying costs (decision 2026-11-16): none / grave / hardcore. Default grave. ?Death=none|grave|hardcore */
	UPROPERTY()
	EGamerDeathMode DeathMode = EGamerDeathMode::Grave;

	// --- Mode, difficulty and its factors (decision 2026-11-16, menu "Welt erstellen"). ---
	// Stored and sent with the world now; options marked "later" have no effect yet and show "kommt später" in the menu.

	/** ?Mode=saga|free */
	UPROPERTY()
	EGamerPlayMode PlayMode = EGamerPlayMode::Saga;

	/** ?Difficulty=easy|medium|hard|hardcore|custom */
	UPROPERTY()
	EGamerDifficulty Difficulty = EGamerDifficulty::Medium;

	/** Factor on hunger, thirst and exhaustion rates. ?NeedsRate=1 */
	UPROPERTY()
	float NeedsRate = 1.f;

	/** Factor on damage players take. ?Damage=1 */
	UPROPERTY()
	float DamageTaken = 1.f;

	/** Factor on what gathering gives. ?Yield=1 */
	UPROPERTY()
	float YieldFactor = 1.f;

	/** Hordes (later). ?Hordes=on|off */
	UPROPERTY()
	bool bHordes = false;

	/** Villagers cannot die (later, villagers cannot die yet anyway). ?Immortal=on|off */
	UPROPERTY()
	bool bVillagersImmortal = false;

	/** Free building without material (later). ?FreeBuild=on|off */
	UPROPERTY()
	bool bFreeBuild = false;

	/** Players can hurt each other (later; decision 2026-11-13: default off). ?FriendlyFire=on|off */
	UPROPERTY()
	bool bFriendlyFire = false;

	/** Player limit, 1..8; AGameSession reads the same URL option. ?MaxPlayers=8 */
	UPROPERTY()
	int32 MaxPlayers = 8;

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
