#pragma once

#include "CoreMinimal.h"

class UGroomAsset;
class UGroomBindingAsset;
class UMaterialInterface;
class USceneComponent;
class USkeletalMesh;
class USkeletalMeshComponent;

/**
 * Optional MetaHuman figures from Content/Data/figures.json (written by tools/metahuman.ps1 Link).
 * Without the file or an entry the callers keep the Manny/Quinn interim figures.
 * Keys: "strong", "slim" (players), "strong_low", "slim_low" (villagers, distant figures),
 * "villager_strong_<n>", "villager_slim_<n>" (villager variants), "npc_seeress" (seeress, job_seer).
 */
namespace GamerFigures
{
	struct FGroom
	{
		UGroomAsset* Groom = nullptr;
		UGroomBindingAsset* Binding = nullptr;
		FName Kind; // "Hair", "Beard" (also goatees, moustaches), "Eyebrows"; from the binding asset name
		/** Material overrides of the assembled blueprint (MI_WI_*: this groom's card atlas); empty = asset default. */
		TArray<UMaterialInterface*> Materials;
	};

	struct FFigure
	{
		USkeletalMesh* Body = nullptr;
		USkeletalMesh* Face = nullptr; // may be null (body only)
		TArray<FGroom> Grooms;         // hair, beard, eyebrows; bound to the face mesh
		/** Groom LOD to force (-1 = automatic). Low assemblies (keys *_low, villager_*, npc_*) keep their visible hair
		 *  only in the helmet LODs 5-7; the assembled blueprint's LODSync maps to 5 near, 7 far. */
		int32 GroomLOD = -1;
		/** Guide simulation for the grooms (players: Mid/High assemblies); Low helmets are static. */
		bool bSimulateHair = false;
	};

	/** Loads the meshes on first use; returns false if this figure is not available. Not on a dedicated server. */
	GAMER_API bool Find(FName Key, FFigure& Out);

	/** Picks one of the figures whose key starts with Prefix (sorted, Hash % count). False if there is none. */
	GAMER_API bool FindVariant(const FString& Prefix, uint32 Hash, FFigure& Out);

	/**
	 * Character-menu hair choice (characters.json look_hair / look_beard) on a player figure: replaces the figure's
	 * hair and beard grooms by the matching hair kit (figures.json "hair_kits", Medium assemblies, see
	 * tools/unreal/metahuman_looks.py HAIR_KITS); eyebrows stay. hair_none / beard_none remove the groom. Options
	 * without a kit keep the figure's own groom of that kind.
	 */
	GAMER_API void ApplyHairChoice(FFigure& InOut, bool bSlimBody, FName HairOption, FName BeardOption);

	/** Stil-Palette (design/stil_norden_gezeitenhall.md) for cloth: Moosgrün, Waidblau, Aschebraun, Ocker, Krapprot, Wollgrau, ungefärbt. */
	GAMER_API FLinearColor ClothPalette(int32 Index);

	/**
	 * Own Norse clothing (tools/clothing.ps1) for the strong or slim body, as a leader-posed mesh on Body:
	 * Outfit "Tunika" (tunic, trousers, leg wraps, belt), "Kleid" (long dress, shoes, belt), "Schuerzenkleid"
	 * (dress + apron with brooches and beads) or the extra layer "Umhang" (cloak). Created on demand into InOutCloth;
	 * hidden when bShow is false (interim Manny/Quinn), Outfit is empty or the asset is missing.
	 * Main colours Wool/Cloak slots, Accent colours Trim/Apron slots.
	 */
	GAMER_API void ApplyClothing(USkeletalMeshComponent* Body, TObjectPtr<USkeletalMeshComponent>& InOutCloth, bool bShow,
		bool bSlimBody, const TCHAR* Outfit, const FLinearColor& Main, const FLinearColor& Accent);

	/**
	 * Shows the figure's grooms on FaceMesh: reuses/creates groom components (owned by the face mesh's actor),
	 * hides the ones not needed. Without the HairStrands rendering the components simply stay invisible.
	 */
	GAMER_API void ApplyGrooms(USkeletalMeshComponent* FaceMesh, TArray<TObjectPtr<USceneComponent>>& InOutComponents, const FFigure& Figure);

	/**
	 * Hair colour from the character menu (characters.json look_hair_color: hair_color_blond, _red, _brown, _black,
	 * _grey, _ash) on all groom components (hair, beard, eyebrows) via the MetaHuman hair material parameters
	 * hairMelanin, hairRedness, WhiteAmount. Unknown or empty option: the assembled colour stays.
	 */
	GAMER_API void SetHairColour(const TArray<TObjectPtr<USceneComponent>>& Components, FName ColourOption);

	/**
	 * Wind for the simulated hair of one figure (docs/01_entscheidungen.md, "Haare und Bärte"): air velocity in cm/s
	 * (world space) and air drag. Meant to be fed by the weather system; until then the default breeze from
	 * ApplyGrooms applies. Only grooms with simulation (hair cards/strands, not the static helmet LODs) react.
	 */
	GAMER_API void SetHairWind(const TArray<TObjectPtr<USceneComponent>>& Components, const FVector& AirVelocity, float AirDrag = 0.1f);
}
