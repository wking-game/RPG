# Game Design & System-Architektur (GDD)

## 1. Kern-Vision & Spielwelt

* **Genre:** Koop-Survival, Siedlungsbau & Action-RPG
* **Welt & Karte:** Zusammenhängende Open-World-Karte mit unterschiedlichen Biomen, Orientierungspunkten und klaren Ressourcen-Zonen.
* **Höhlen & Untergrund:** Eigenständige Höhlensysteme mit eigener Lichtstimmung, Akustik/Echo, speziellen Rohstoffadern und Gefahren.
* **Terraforming:** Gezieltes Bearbeiten des Geländes (Ebnen von Bauflächen, Aufschütten, Gräben) gekoppelt an Werkzeuge, Ausdauer und Bau-/Ressourcenkosten.
* **Wettersound & Atmosphäre:** Dynamische Wetterwechsel mit geschichtetem Audio-Design und direkter Kopplung an Gameplay, Feuerstellen, Bewohner und Kreaturen.

---

## 2. Blaupausen-System & 3-Stufen-Bau

### Grundprinzip

* **Vorgefertigte & eigene Blaupausen:** Spieler können Standard-Blaupausen nutzen oder eigene Konstruktionen markieren, als Custom-Blueprint speichern und erneut platzieren.
* **3 Bau-Stufen:** Jedes Objekt entsteht in 3 Phasen (z. B. *Stufe 1: Fundament/Gerüst*, *Stufe 2: Rohbau*, *Stufe 3: Fertigstellung*).
* **Reine Stufen-Kosten:** Jede Stufe benötigt **ausschließlich die Ressourcen, die genau für diese Stufe definiert sind** – keine Verrechnung mit vorherigen Stufen.
* **Physische Anlieferung:** Ressourcen müssen vom Spieler oder von Bewohnern direkt zur Blaupause gebracht werden.

### Datenstruktur (C++ / UE5 Architektur)

```cpp
// Einzelner Ressourcen-Eintrag
struct FResourceRequirement {
    FName ResourceID;      // z.B. "Wood_Log", "Stone_Block"
    int32 RequiredAmount;  // Benötigte Menge NUR für diese eine Stufe
};

// Definition einer einzelnen Bau-Stufe (1, 2 oder 3)
struct FBuildStageDefinition {
    int32 StageIndex;                          // 1, 2 oder 3
    UStaticMesh* StageMesh;                    // Visuelles Mesh für diese Stufe (Gerüst -> Rohbau -> Fertig)
    TArray<FResourceRequirement> StageCosts;   // NUR die Ressourcen, die genau diese Stufe kostet
    float BaseBuildWorkRequired;               // Arbeitsaufwand in Sekunden/Punkten für Bewohner oder Spieler
    bool bProvidesWeatherShelter = false;      // True erst ab regendichter Stufe (z.B. fertiges Dach)
};

// Statische Blaupausen-Vorlage
struct FBlueprintDefinition {
    FName BlueprintID;
    FText DisplayName;
    bool bIsCustomPlayerBlueprint;             // Unterscheidung: Standard vs. selbst gebaut & gespeichert
    TArray<FBuildStageDefinition> Stages;      // Immer exakt 3 Einträge (Index 0 = Stufe 1, 1 = Stufe 2, 2 = Stufe 3)
};

// Custom-Blueprint (Zusammengesetzte Spieler-Blaupause)
struct FCustomBlueprintEntry {
    FName SubBlueprintID;         // Referenz auf das Einzelteil (z.B. Wand, Treppe, Pfosten)
    FTransform RelativeTransform; // Position & Rotation relativ zum Ankerpunkt der Custom-Blaupause
};

struct FCustomBlueprintAsset {
    FName CustomID;
    FText CustomName;
    TArray<FCustomBlueprintEntry> Entries;
};
```

---

## 3. Koop-Multiplayer & Objekt-Berechtigungen

* **Objekt-gebundener Besitz:** Der Spieler, der eine Blaupause setzt, wird als `Owner` dieses spezifischen Objekts gespeichert.
* **Granulare Freigabe pro Objekt:** Für jedes einzelne Objekt lässt sich separat festlegen, welche Rechte Mitspieler haben:
  * Nur der Besitzer darf interagieren (`OwnerOnly`)
  * Mitspieler dürfen Ressourcen liefern und fertigbauen (`AllowFinishBuild`)
  * Mitspieler dürfen fertigbauen **und** abreißen/demontieren (`AllowBuildAndBreak`)

```cpp
enum class ECoopBuildPermission : uint8 {
    OwnerOnly,          // Nur der Ersteller darf interagieren
    AllowFinishBuild,   // Mitspieler dürfen Ressourcen liefern & Stufe fertigbauen
    AllowBuildAndBreak  // Mitspieler dürfen fertigbauen UND abreißen/demontieren
};

struct FObjectPermissions {
    FString OwnerPlayerID;                               // Eindeutige ID (z.B. Steam/EOS ID) des Erstellers
    ECoopBuildPermission DefaultCoopAccess;              // Standard-Freigabe für alle anderen Mitspieler
    TMap<FString, ECoopBuildPermission> PlayerOverrides; // Optional: Einzelne Freunde gezielt freischalten
};

struct FDeliveredResource {
    FName ResourceID;
    int32 CurrentAmount; // Bereits in die aktuelle Stufe eingezahlte Menge
};

struct FBuildableInstanceState {
    FGuid InstanceID;                                  // Eindeutige ID für Savegames & Bewohner-Zuweisung
    FName BlueprintID;                                 // Verweis auf FBlueprintDefinition
    int32 CurrentStage;                                // 1, 2, 3 (4 = Komplett fertiggestellt)
    TArray<FDeliveredResource> DeliveredInCurrentStage; // Wird bei Stufenaufstieg geleert!
    float CurrentWorkProgress;                         // 0.0 bis BaseBuildWorkRequired
    bool bAreResourcesComplete;                        // True, sobald alle StageCosts geliefert wurden
    FObjectPermissions Permissions;                    // Individuelle Rechte nur für dieses eine Objekt
};
```

---

## 4. Wettersystem, Audio-Schichten & Shelter-Erkennung

### Wetterzustände & Gameplay-Effekte

* **Zustände:** `Clear`, `Overcast`, `LightRain`, `HeavyRain`, `Storm`, `Fog` (biom- und tageszeitabhängig mit weichen Übergängen).
* **3-Schichten-Audio:**
  1. *Globales Ambient-Bett* (Wind/Regen-Grundrauschen).
  2. *Material- & Raum-Reaktion* (Regen auf Holz/Stein/Laub; Dämpfung unter Dach und in Höhlen).
  3. *Punktuell getriggerte One-Shots* (Blitz/Donner mit distanzabhängiger Laufzeitverzögerung).
* **Gameplay-Auswirkungen:**
  * **Regen:** Erhöht `Wetness` im Freien; ungeschützte offene Feuer brennen schneller ab oder erlöschen; Stufe 2/3 im Freien baut bei Starkregen langsamer.
  * **Sturm:** Winddruck im Freien, ungenauerer Fernkampf, offene Feuer ohne Windschutz erlöschen leicht, Bewohner suchen Deckung.
  * **Nebel:** Reduziert Sichtweite und Wahrnehmungsradius (erleichtert Anschleichen, erschwert Orientierung).
  * **Gewitter:** Blitzrisiko auf exponierten Höhen und hohen ungeschützten Strukturen (Stufe 2/3); Bewohner mit hoher Belastung suchen Schutz.

### Shelter-System (`UShelterDetectorComponent`)

* **3 Schutzstufen (`EShelterLevel`):**
  * `None`: Ungeschützt im Freien.
  * `Roofed`: Überdacht durch fertiges Dach-Bauteil (`bProvidesWeatherShelter == true`) oder Felsvorsprung (ermittelt über 5-Punkt-Trace nach oben; mindestens 3/5 Treffer).
  * `EnclosedOrCave`: In einer `CaveZone` oder einem umschlossenen Innenraum.
* **Feuerstellen-Kopplung:** Offene Feuer (`ACampfireActor`) prüfen per `UShelterDetectorComponent`, ob sie unter einem Dach stehen. Fertige Dächer schützen das Feuer; unfertige Stufe-1-Gerüste bieten keinen Schutz.

```cpp
enum class EWeatherType : uint8 {
    Clear,
    Overcast,
    LightRain,
    HeavyRain,
    Storm,
    Fog
};

struct FWeatherState {
    EWeatherType CurrentWeather;
    EWeatherType TargetWeather;
    float TransitionAlpha;     // 0.0 bis 1.0
    float WindIntensity;       // 0.0 bis 1.0
    float PrecipitationRate;   // 0.0 bis 1.0
    float LightningFrequency;  // Einschläge pro Minute
    float TimeRemainingInState;
};

struct FWeatherGameplayModifiers {
    float RainWetnessRate;
    float WindPushStrength;
    float VisibilityMultiplier;
    float OutdoorBuildSpeedMult;
    float OpenFireBurnDrainMult;
    bool bAllowLightningStrikes;
};

enum class EShelterLevel : uint8 {
    None,
    Roofed,
    EnclosedOrCave
};

struct FShelterEvaluation {
    EShelterLevel ShelterLevel;
    float OverheadCoverage;      // 0.0 bis 1.0 (Anteil getroffener Dach-/Decken-Traces)
    bool bIsInCaveZone;
    bool bIsWindSheltered;
    AActor* PrimaryShelterActor;
};
```

---

## 5. Bewohner-KI, Logistik-, Betten- & Nahrungssystem

### Zustandsmaschine (Villager State Machine)

1. `IDLE` $\rightarrow$ Prüft auf akute Bedürfnisse (`Hunger`, `Fatigue`, `SEEK_SHELTER`), manuelle Befehle oder freie Jobs.
2. `EVALUATE_TASK_TYPE` $\rightarrow$ Prüft, ob in `CurrentStage` noch Ressourcen fehlen oder ob gebaut werden kann.
3. **Logistik-Pfad (falls Ressourcen fehlen):**
   `FIND_RESOURCE_SRC` $\rightarrow$ `MOVE_TO_SOURCE` $\rightarrow$ `PICKUP_RESOURCE` $\rightarrow$ `MOVE_TO_SITE` $\rightarrow$ `DELIVER_RESOURCE`
4. **Bau-Pfad (sobald Stufe komplett versorgt ist):**
   `MOVE_TO_SITE` $\rightarrow$ `WORK_ON_STAGE` $\rightarrow$ `ADVANCE_STAGE`
5. **Stufenwechsel:** Bei 100 % Arbeitsfortschritt steigt `CurrentStage` um 1, `DeliveredInCurrentStage` wird geleert und die KI prüft neu, welche Ressourcen die nächste Stufe benötigt.

### Reservierungslogik (für Bau-Materialien & Nahrung)

* **Am Lager:** $\text{AvailableToReserve} = \text{TotalStored} - \text{ReservedForPickup}$
* **An der Baustelle:** $\text{StillNeeded} = \max(0,\; \text{RequiredForStage} - \text{DeliveredInCurrentStage} - \text{IncomingReserved})$
* Dieselbe Ticket-Logik (`FResourceReservation`) verhindert sowohl Doppelwege zu Baustellen als auch gleichzeitiges Zugreifen mehrerer hungriger Bewohner auf dieselbe letzte Nahrungsration.

### Rekrutierung, Betten, Bedürfnisse & UI

* **Rekrutierung:** Rettung/Ansprache gestrandeter NPCs in der Welt sowie Anwerbung über Siedlungs-Attraktivität (fertige Unterkünfte, Betten, Nahrungspuffer).
* **Betten (`FBedSlotState`):** Exklusive Zuweisung (`OccupantVillagerID`), nutzbar erst nach entsprechender Baustufe (`bIsUsable`), bevorzugt überdacht (`Roofed`). Ohne Bett nutzen Bewohner Not-Schlafplätze mit Moral-/Erholungs-Malus.
* **Siedlungs-UI:**
  * *Kontext-Interaktion in der Welt:* Direktes Rekrutieren, Bett-Zuweisung am Bett-Actor, schwebende Warn-Icons nur bei Problemen (Hunger, kein Bett, Nässe/Schutz).
  * *Siedlungs-Menü:* Obere Kopfzeile (Betten-Kapazität, freie Nahrungsportionen, Wetter), linke Bewohnerliste (Rolle, Status, Warn-Icon), rechtes Detail-Panel (Rollen-Umschalter, Bett-Dropdown, aktiver Auftrag & Direktbefehl, Bedürfnis-Balken).

```cpp
enum class EVillagerRole : uint8 {
    Generalist,
    Builder,
    Hauler,
    Caretaker
};

enum class EReservationPurpose : uint8 {
    BuildStageDelivery, // Anlieferung an 3-Stufen-Blaupause
    VillagerMeal        // Nahrungsaufnahme eines Bewohners
};

enum class EReservationState : uint8 {
    PendingPickup,
    CarryingToSite,
    Completed,
    Cancelled
};

struct FResourceReservation {
    FGuid ReservationID;
    FGuid VillagerID;
    EReservationPurpose Purpose;
    FName ResourceID;
    int32 Amount;
    FGuid SourceStorageID;
    FGuid TargetSiteID;
    int32 TargetStage;
    EReservationState State;
    float CreatedTimestamp; // Timeout-Schutz gegen dauerhafte Blockaden
};

struct FBedSlotState {
    FGuid BedInstanceID;
    FGuid OccupantVillagerID; // Leer, wenn frei
    bool bIsUsable;           // False, wenn noch im Bau oder beschädigt
};

struct FVillagerNeeds {
    float Hunger;          // 0.0 (satt) bis 1.0 (verhungert)
    float Fatigue;         // 0.0 (ausgeruht) bis 1.0 (erschöpft)
    float Morale;          // 0.0 bis 1.0
    float WeatherExposure; // 0.0 bis 1.0 (aus Shelter & Wettersystem)
};

struct FVillagerData {
    FGuid VillagerID;
    FText Name;
    EVillagerRole Role;
    FGuid AssignedBedID;
    FGuid ManualTargetID;
    FVillagerNeeds Needs;
    float WorkSpeedMultiplier;
};
```

---

## 6. Charakter-System: Hintergründe, Basis-Stats & Fähigkeiten

* **Hintergrundgeschichten (Origins):** Wählbar im Charakter-Editor; verleihen Start-Stat-Boni, +20 % Lern-Affinität auf einen Skill und ein passives Start-Perk:
  * *Siedlungs-Zimmermann:* Baukunst, Tragkraft, schnelleres Bauen auf Stufe 2/3.
  * *Fährtenleser:* Wildnis & Orientierung, langsamerer Nässe-Aufbau, bessere Sicht bei Nebel.
  * *Stollengräber:* Bergbau & Terraforming, weniger Stamina-Kosten beim Graben, bessere Höhlenorientierung.
  * *Karawanen-Vogt:* Führung & Logistik, Boni für zugewiesene Bewohner.
  * *Kräuterkundiger:* Überleben & Versorgung, längere Feuer-Brenndauer, stärkere Nahrungswirkung.
* **5 Basis-Stats (Level-Cap 50):** `Vitality` (Gesundheit/Resistenz), `Endurance` (Ausdauer), `Strength` (Tragkraft/Nahkampf), `Agility` (Tempo/Ausweichen/Fernkampf), `Resolve` (Mentale/Wetter-Resistenz & Führungsradius).
* **6 Fähigkeiten (Learning-by-Doing + Meilenstein-Perks):** `Construction`, `MiningTerraform`, `Wilderness`, `Leadership`, `SurvivalCraft`, `Combat`.

```cpp
enum class ECharacterSkillType : uint8 {
    Construction,
    MiningTerraform,
    Wilderness,
    Leadership,
    SurvivalCraft,
    Combat
};

struct FCharacterCoreStats {
    int32 Vitality = 10;
    int32 Endurance = 10;
    int32 Strength = 10;
    int32 Agility = 10;
    int32 Resolve = 10;
};

struct FSkillProgress {
    int32 CurrentLevel = 1;
    float CurrentXP = 0.0f;
    float LearnRateMultiplier = 1.0f;
    TArray<FName> UnlockedPerks;
};

struct FPlayerCharacterProfile {
    FString CharacterName;
    FName BackgroundID;
    int32 CharacterLevel = 1;
    int32 MaxCharacterLevel = 50;
    int32 UnspentStatPoints = 0;
    FCharacterCoreStats Stats;
    TMap<ECharacterSkillType, FSkillProgress> Skills;
};
```

---

## 7. Tiere, Kreaturen & Höhlen-Zonen

* **Wildtiere (`Wildlife`):** Scheu (`Passive`) oder territorial (`Defensive`); folgen Tagesrhythmen, fliehen bei Sturm/Starkregen unter Bäume/Überhänge (`SeekNaturalShelter`) und liefern Fleisch, Fell und Sehnen.
* **Kreaturen (`Creature`):** Aggressiv oder lauernd (`Hostile` / `Ambush`); besonders aktiv bei Nacht, Nebel, Gewitter oder in tiefen Höhlen.
* **Höhlen-Zonen (`ACaveZoneVolume`):**
  * Unterteilt in Gefahrenstufen (*Eingang*, *Stollen*, *Tiefe Kaverne*).
  * Setzen `bIsInCaveZone = true` (`EnclosedOrCave`), wechseln auf Höhlen-Hall (Reverb), dämpfen Außenregen und bieten spezielle Erz-/Kristalladern.
* **Skill-Verzahnung:** `Wilderness` verbessert Fährtenlesen und Schleichen bei Nebel; `MiningTerraform` steigert Höhlenerträge; `Combat` verbessert Verteidigung im Dunkeln.

```cpp
enum class ECreatureTemperament : uint8 {
    Passive,
    Defensive,
    Territorial,
    Hostile,
    Ambush
};

struct FEntityActivityRules {
    bool bActiveDay = true;
    bool bActiveNight = false;
    bool bSeeksShelterInStorm = true;
    bool bEmpoweredInFogOrStorm = false;
    float BasePerceptionRadius = 1500.0f;
};

struct FCaveZoneConfig {
    FName CaveID;
    int32 DangerTier = 1; // 1 = Eingang, 2 = Stollen, 3 = Tiefe Kaverne
    float AmbientLightLevel = 0.05f;
    float AcousticReverbStrength = 0.8f;
    TArray<FName> SpawnableEntityIDs;
    TArray<FName> RareResourceNodes;
};
```

---

## 8. Multiplayer-Replikation & Server-Autorität

* **Server-Autorität:** Wetterzustand (`FWeatherState`), Feuerstellen-Status (`bIsLit`, `FuelRemaining`), Bewohner-KI, Reservierungen, Inventar-Abzüge, Stufenwechsel (`CurrentStage`) und Ko-op-Rechteprüfungen (`CanPlayerPerformAction`) laufen ausschließlich auf dem Server.
* **Client-Lokalisierung:** Lokale 5-Punkt-Shelter-Traces für HUD/Audio-Occlusion, weiche Wetter-Interpolation und Vorschau-Ghosts beim Platzieren von Blaupausen laufen lokal auf den Clients.
* **Join-in-Progress:** Replizierte Zustände sorgen dafür, dass spät beitretende Spieler sofort den aktuellen Baufortschritt, das laufende Wetter und brennende/erloschene Feuerstellen korrekt sehen.

---

## 9. Offene Punkte & Checkliste (Nächste Ausbauschritte)

* [ ] **Überlebenswerte des Spielers:** Exakte Formeln für `Wetness`, Körpertemperatur/Kälte, Ausdauer-Regeneration sowie Tod/Respawn-Regeln.
* [ ] **Kampf, Werkzeuge & Crafting-Stationen:** Nah-/Fernkampf-Mechanik, Werkzeug-Haltbarkeit und Rezept-Progression (Werkbank, Schmiede, Kochstelle).
* [ ] **Abriss-, Reparatur- & Statik-Regeln:** Ressourcen-Rückerstattung beim Abriss über abgeschlossene Stufen hinweg, strukturelle Stabilität und Reparatur von Wetterschäden.
* [ ] **Spieler-Tutorial & Karten-UX:** Schrittweiser Einstieg für 3-Stufen-Blaupausen und Bewohner-Zuweisung sowie Fog of War und Koop-Karten-Pings.
* [ ] **Savegame-Serialisierung:** Speichern von Welt-Objekten, Custom-Blueprints, Bewohnern, Charakterprofilen und Terraforming-Änderungen.