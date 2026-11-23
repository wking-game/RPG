#pragma once

#include "CoreMinimal.h"
#include "GamerGameState.h"

struct FGamerDataReport;

/**
 * One difficulty preset (Content/Data/difficulty.json, decision 2026-11-16): which world options and factors
 * "Einfach / Mittel / Schwer / Hardcore" set. Changing any of them afterwards in the menu makes the world "Benutzerdefiniert".
 */
struct GAMER_API FGamerDifficultyPreset
{
	FName Id;
	EGamerDifficulty Level = EGamerDifficulty::Medium;
	FString Name;
	FString Description;

	EGamerDeathMode Death = EGamerDeathMode::Grave;
	float NeedsRate = 1.f;
	float DamageTaken = 1.f;
	float Yield = 1.f;
	bool bHordes = false;
	bool bVillagersImmortal = false;
	EGamerDemolish Demolish = EGamerDemolish::FullRefund;
	EGamerGespuerTempo GespuerTempo = EGamerGespuerTempo::Normal;

	/** Writes the preset's values into Options and sets Options.Difficulty to this level. */
	void ApplyTo(FGamerWorldOptions& Options) const;

	/** True if Options has exactly this preset's values (the difficulty field itself is not compared). */
	bool Matches(const FGamerWorldOptions& Options) const;

	/** "Tod: Grab · Hunger ×1,3 · Schaden ×1,4 · Ertrag ×0,8 · Horden an · ..." for the menu and logs. */
	FString Summary() const;
};

namespace GamerData
{
	inline const TCHAR* const DifficultyFile = TEXT("difficulty.json");

	/**
	 * Loads and checks difficulty.json: every level easy, medium, hard, hardcore exactly once, factors 0.1..5,
	 * known enum values, no unknown fields. Missing file: error when bRequired. Returns true without errors.
	 */
	GAMER_API bool LoadDifficulty(const FString& Directory, TArray<FGamerDifficultyPreset>& Out, FGamerDataReport& Report, bool bRequired = true);

	/** Preset for a level, or null (Custom has none). */
	GAMER_API const FGamerDifficultyPreset* FindDifficulty(const TArray<FGamerDifficultyPreset>& Presets, EGamerDifficulty Level);

	/** German name of a level ("Benutzerdefiniert" for Custom). */
	GAMER_API FString DifficultyName(const TArray<FGamerDifficultyPreset>& Presets, EGamerDifficulty Level);

	/**
	 * Server, when a world starts: for a preset level the server's own data wins over what the URL carried
	 * (server-authoritative); Custom keeps the URL values. Returns true if something had to be corrected.
	 */
	GAMER_API bool EnforceDifficulty(const TArray<FGamerDifficultyPreset>& Presets, FGamerWorldOptions& Options);
}
