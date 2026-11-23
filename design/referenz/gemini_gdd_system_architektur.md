# Game Design & System-Architektur (GDD)

## 1. Kern-Vision & Spielwelt
- **Genre:** Koop-Survival, Siedlungsbau & Action-RPG
- **Welt & Karte:** Zusammenhängende Open-World-Karte mit unterschiedlichen Biomen, Orientierungspunkten und klaren Ressourcen-Zonen.
- **Höhlen & Untergrund:** Eigenständige Höhlensysteme mit eigener Lichtstimmung, Akustik/Echo, speziellen Rohstoffadern und Gefahren.
- **Terraforming:** Gezieltes Bearbeiten des Geländes (Ebnen von Bauflächen, Aufschütten, Gräben) gekoppelt an Werkzeuge, Ausdauer und Bau-/Ressourcenkosten.
- **Wettersound & Atmosphäre:** Dynamische Wetterwechsel (Nieselregen, Sturm, Gewitter) mit geschichtetem Audio-Design (materialabhängige Aufprallgeräusche, Wind in Bäumen/Höhlen, distanzabhängiger Donner).

---

## 2. Blaupausen-System & 3-Stufen-Bau

### Grundprinzip
- **Vorgefertigte & eigene Blaupausen:** Spieler können Standard-Blaupausen nutzen oder eigene Konstruktionen markieren, als Custom-Blueprint speichern und erneut platzieren.
- **3 Bau-Stufen:** Jedes Objekt entsteht in 3 Phasen (z. B. *Stufe 1: Fundament/Gerüst*, *Stufe 2: Rohbau*, *Stufe 3: Fertigstellung*).
- **Reine Stufen-Kosten:** Jede Stufe benötigt **ausschließlich die Ressourcen, die genau für diese Stufe definiert sind** – keine Verrechnung mit vorherigen Stufen.
- **Physische Anlieferung:** Ressourcen müssen vom Spieler oder von Bewohnern direkt zur Blaupause gebracht werden.

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

- **Objekt-gebundener Besitz:** Der Spieler, der eine Blaupause setzt, wird als `Owner` dieses spezifischen Objekts gespeichert.
- **Granulare Freigabe pro Objekt:** Für jedes einzelne Objekt lässt sich separat festlegen, welche Rechte Mitspieler haben:
  - Nur der Besitzer darf interagieren (`OwnerOnly`)
  - Mitspieler dürfen Ressourcen liefern und fertigbauen (`AllowFinishBuild`)
  - Mitspieler dürfen fertigbauen **und** abreißen/demontieren (`AllowBuildAndBreak`)

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
    int32 CurrentStage;                                // 1, 2 oder 3 (4 = Komplett fertiggestellt)
    TArray<FDeliveredResource> DeliveredInCurrentStage; // Wird bei Stufenaufstieg geleert!
    float CurrentWorkProgress;                         // 0.0 bis BaseBuildWorkRequired
    bool bAreResourcesComplete;                        // True, sobald alle StageCosts geliefert wurden
    FObjectPermissions Permissions;                    // Individuelle Rechte nur für dieses eine Objekt
};
```

---

## 4. Bewohner-KI & Logistik- / Reservierungssystem

### Zustandsmaschine (Villager State Machine)
1. `IDLE` $\rightarrow$ Prüft auf zugewiesene oder freie Bau-/Logistik-Jobs.
2. `EVALUATE_TASK_TYPE` $\rightarrow$ Prüft, ob in `CurrentStage` noch Ressourcen fehlen oder ob gebaut werden kann.
3. **Logistik-Pfad (falls Ressourcen fehlen):**
   `FIND_RESOURCE_SRC` $\rightarrow$ `MOVE_TO_SOURCE` $\rightarrow$ `PICKUP_RESOURCE` $\rightarrow$ `MOVE_TO_SITE` $\rightarrow$ `DELIVER_RESOURCE`
4. **Bau-Pfad (sobald Stufe komplett versorgt ist):**
   `MOVE_TO_SITE` $\rightarrow$ `WORK_ON_STAGE` $\rightarrow$ `ADVANCE_STAGE`
5. **Stufenwechsel:** Bei Erreichen von 100 % Arbeitsfortschritt steigt `CurrentStage` um 1, `DeliveredInCurrentStage` wird geleert und die KI prüft neu, ob für die nächste Stufe wieder Ressourcen angeliefert werden müssen.

### Reservierungslogik (gegen Doppelwege & leere Lager)
- **Am Lager:** $\text{AvailableToReserve} = \text{TotalStored} - \text{ReservedForPickup}$
- **An der Baustelle:** $\text{StillNeeded} = \max(0,\; \text{RequiredForStage} - \text{DeliveredInCurrentStage} - \text{IncomingReserved})$

```cpp
enum class EVillagerJobType : uint8 {
    None,
    HaulResourceToSite, // Material aus Lager/Quelle zur Blaupause bringen
    ConstructStage,     // Aktuelle Stufe am Baufeld fertigbauen
    DemolishObject      // Objekt/Stufe abbauen (falls freigegeben)
};

struct FVillagerTask {
    FGuid TaskID;
    EVillagerJobType JobType;
    FGuid TargetBuildableID;   // Ziel-Blaupause / -Objekt
    AActor* SourceActor;       // Lagerkiste, Rohstoffstapel oder Quelle
    FName ResourceToHaul;      // Falls HaulResourceToSite
    int32 AssignedAmount;    // Reservierte Menge
    int32 TargetStageSnapshot; // Stufe, für die der Job erstellt wurde
};

enum class EReservationState : uint8 {
    PendingPickup,  // Auf dem Weg zum Lager oder dort am Aufladen
    CarryingToSite, // Material aufgenommen, auf dem Weg zur Baustelle
    Completed,      // Abgeliefert und freigegeben
    Cancelled       // Abgebrochen / Timeout / Ziel ungültig
};

struct FResourceReservation {
    FGuid ReservationID;
    FGuid VillagerID;       // Welcher Bewohner hält diese Reservierung
    FName ResourceID;       // z.B. "Wood_Log"
    int32 Amount;           // Reservierte Menge
    FGuid SourceStorageID;  // Lager oder Quelle
    FGuid TargetSiteID;     // Ziel-Blaupause / -Objekt
    int32 TargetStage;      // Stufe (1, 2 oder 3), für die reserviert wurde
    EReservationState State;
    float CreatedTimestamp; // Timeout-Schutz gegen dauerhafte Blockaden
};
```

---

## 5. Charakter-System, Kreaturen & Tutorial
- **Charakter-Editor:** Visuelle Anpassung + Wahl einer **Hintergrundgeschichte** (beeinflusst Start-Boni/Flair).
- **Stats & Fähigkeiten:** Basiswerte (Ausdauer, Tragkraft, Gesundheit, Tempo) sowie trainierbare Skills (Bauen, Terraforming, Abbau, Führung/Bewohner-Effizienz).
- **Tiere & Kreaturen:**
  - *Tiere:* Biom- und tageszeitabhängige Wildtiere (Flucht, Weiden, Wetterschutz).
  - *Kreaturen:* Gefahren in Höhlen, bei Nacht oder in speziellen Zonen mit eigenen Angriffs- und Beutemechaniken.
- **Tutorial:** Geführter Einstieg über das Setzen der ersten Blaupause, Ressourcen-Anlieferung über alle 3 Stufen und das Zuweisen eines Bewohners.

---

## 6. Offene Punkte & Checkliste (Noch auszuarbeiten)

- [ ] **Kampfsystem & Ausrüstung:** Nahkampf, Fernkampf, Ausweichen/Blocken, Haltbarkeit von Werkzeugen/Waffen sowie Crafting-Stationen (Schmiede, Werkbank, Kochstelle).
- [ ] **Überlebens-Mechaniken (Survival-Loop):** Hunger, Durst, Ausdauer, Temperatur-/Wetter-Effekte auf den Charakter (z. B. Unterkühlung bei Sturm oder in Höhlen) und Tod/Respawn-Regeln.
- [ ] **Bewohner-Bedürfnisse & Verwaltung:** Betten, Nahrung, Zufriedenheit/Lohn, Rekrutierung (Befreien, Anwerben, Siedlungsruf) sowie UI zur Berufs- und Prioritäten-Zuweisung.
- [ ] **Abriss-, Reparatur- & Statik-Regeln:** Ressourcen-Rückerstattung beim Abriss über abgeschlossene Stufen hinweg, strukturelle Stabilität und Schaden durch Wetter/Kreaturen.
- [ ] **UI / UX & Karten-Funktionen:** Fog of War, Kartenmarkierungen/Pings im Koop, Bau-Menü, Custom-Blueprint-Verwaltung und HUD für die 3 Bau-Stufen.
- [ ] **Progression & Endgame-Ziel:** Biom-Bosse, tiefe Höhlen-Dungeons, Technologie-/Blaupausen-Freischaltung, Siedlungsausbau oder Story-Quests.
- [ ] **Technische Basis (Multiplayer & Savegame):** Host-Client vs. Dedicated Server, Speicherung von Terraforming-Änderungen und Welt-Synchronisation für nachjoinende Koop-Spieler.