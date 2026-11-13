#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
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

	/** Reads the options from a map URL option string; unknown values keep the default and are logged. */
	static FGamerWorldOptions FromUrlOptions(const FString& Options);

	/** German one-liner for the menu ("Bauplan + Hammer · frei mit Einrasten · Abreißen: volles Material"). */
	FString Describe() const;

	/** Short form for logs and tests ("BuildFlow=blueprint Placement=free Demolish=full"). */
	FString ToLogString() const;

	/** Map URL options for starting a world with these options ("?BuildFlow=blueprint?Placement=free?Demolish=full"). */
	FString ToUrlOptions() const;
};

/** Replicated game state: world options for everyone (clients need them for the build preview). */
UCLASS()
class GAMER_API AGamerGameState : public AGameStateBase
{
	GENERATED_BODY()

public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	/** Server: set once when the world starts. */
	void SetWorldOptions(const FGamerWorldOptions& InOptions);
	const FGamerWorldOptions& GetWorldOptions() const { return WorldOptions; }

	/** Options of the world the object lives in (defaults if there is no game state yet). */
	static const FGamerWorldOptions& Get(const UObject* WorldContext);

protected:
	UPROPERTY(Replicated)
	FGamerWorldOptions WorldOptions;
};
