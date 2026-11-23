# Game Design Document (GDD): Multiplayer Survival & Siedlungsbau (UE5)

## Inhaltsverzeichnis

1. [Spielvision & Core-Loop](#1-spielvision--core-loop)
2. [Weltstruktur & die 8 festen NPC-Orte](#2-weltstruktur--die-8-festen-npc-orte)
   - [2.1 Hierarchie der 8 Orte](#21-hierarchie-der-8-orte)
   - [2.2 Schutz- & Interaktionsregeln der NPC-Orte](#22-schutz---interaktionsregeln-der-npc-orte)
   - [2.3 Höhlen-Zonen (`CaveZone`) & Ressourcen-Regeneration](#23-höhlen-zonen-cavezone--ressourcen-regeneration)
3. [Überlebenssysteme: Wetter, Shelter & Vitalwerte](#3-überlebenssysteme-wetter-shelter--vitalwerte)
   - [3.1 Shelter-Stufen (`EShelterLevel`)](#31-shelter-stufen-eshelterlevel)
   - [3.2 Wechselwirkungen der Vitalwerte](#32-wechselwirkungen-der-vitalwerte)
4. [Modulares 3-Stufen-Bausystem, begehbare Räume & Custom-Blueprints](#4-modulares-3-stufen-bausystem-begehbare-räume--custom-blueprints)
   - [4.1 Die drei Baustufen (`CurrentStage` 1 → 3)](#41-die-drei-baustufen-currentstage-1--3)
   - [4.2 Türen & Innenraum-Slots (`FInteriorStationSlot`)](#42-türen--innenraum-slots-finteriorstationslot)
   - [4.3 Custom-Blueprints, Platzierung & Geländeanpassung](#43-custom-blueprints-platzierung--geländeanpassung)
   - [4.4 Haltbarkeit, Stabilität, Reparatur & Abriss](#44-haltbarkeit-stabilität-reparatur--abriss)
5. [Bewohner-KI, Rollen & Logistik](#5-bewohner-ki-rollen--logistik)
   - [5.1 Bedürfnisse & Tagesablauf (`AVillagerCharacter`)](#51-bedürfnisse--tagesablauf-avillagercharacter)
   - [5.2 Die vier Bewohner-Rollen (`EVillagerRole`)](#52-die-vier-bewohner-rollen-evillagerrole)
   - [5.3 Reservierungssystem (`FResourceReservation`)](#53-reservierungssystem-fresourcereservation)
6. [UI/UX, Karten-Navigation & Onboarding](#6-uiux-karten-navigation--onboarding)
   - [6.1 Siedlungs-UI (5-Reiter-Übersicht)](#61-siedlungs-ui-5-reiter-übersicht)
   - [6.2 Weltkarte, Kompass & atmosphärische Sichtbarkeitsregeln](#62-weltkarte-kompass--atmosphärische-sichtbarkeitsregeln)
   - [6.3 Onboarding-Loop & Übergang in die offene Welt](#63-onboarding-loop--übergang-in-die-offene-welt)
7. [SaveGame-Architektur, Grenzfall-Bereinigung & Koop-Multiplayer](#7-savegame-architektur-grenzfall-bereinigung--koop-multiplayer)
   - [7.1 SaveGame-Modell (`USurvivalWorldSaveGame`)](#71-savegame-modell-usurvivalworldsavegame)
   - [7.2 Sanitize- & Reconcile-Regeln beim Laden](#72-sanitize---reconcile-regeln-beim-laden)
   - [7.3 Koop-Multiplayer-Architektur (Server-Autorität)](#73-koop-multiplayer-architektur-server-autorität)
8. [Eigenentwickeltes Skill-Graph-System (21 Knoten & 4 Anker)](#8-eigenentwickeltes-skill-graph-system-21-knoten--4-anker)
   - [8.1 Netz-Topologie, UND/ODER-Abhängigkeiten & Dependency-Lock](#81-netz-topologie-undoder-abhängigkeiten--dependency-lock)
   - [8.2 Die 6 Anker-Resonanzen (`A01`–`A04`)](#82-die-6-anker-resonanzen-a01a04)
   - [8.3 Aktive Fähigkeiten, Gegenstände, Blaupausen & Lehrmeister-Respec](#83-aktive-fähigkeiten-gegenstände-blaupausen--lehrmeister-respec)
   - [8.4 Komplette Übersicht aller 21 Knoten (`N01`–`N16` & `A01`–`A04`)](#84-komplette-übersicht-aller-21-knoten-n01n16--a01a04)
9. [Händler-Stufen, Orts-Ruf (`0–1000`) & Auftrags-/Meilenstein-Quellen](#9-händler-stufen-orts-ruf-01000--auftrags-meilenstein-quellen)
   - [9.1 Ruf-Stufen & Staffelung über die 8 Karten-Orte](#91-ruf-stufen--staffelung-über-die-8-karten-orte)
   - [9.2 Aufträge, Lieferungen & Meilensteine nach Ortstyp](#92-aufträge-lieferungen--meilensteine-nach-ortstyp)
10. [XP-Progression bis Level 50 (49 Skillpunkte) & Meilenstein-XP](#10-xp-progression-bis-level-50-49-skillpunkte--meilenstein-xp)
    - [10.1 XP-Kurve & Skillpunkt-Budget](#101-xp-kurve--skillpunkt-budget)
    - [10.2 Konkrete `BaseXPAmount`-Richtwerte nach Kategorie](#102-konkrete-basexpamount-richtwerte-nach-kategorie)
11. [C++-Implementierungsreferenz (`USkillGraphSubsystem`)](#11-c-implementierungsreferenz-uskillgraphsubsystem)
    - [11.1 Header-Strukturen & Subsystem-Schnittstelle (`SkillGraphSubsystem.h`)](#111-header-strukturen--subsystem-schnittstelle-skillgraphsubsystemh)
    - [11.2 Initialisierung, Resonanzen, Ruf, Handel & XP (`SkillGraphSubsystem.cpp`)](#112-initialisierung-resonanzen-ruf-handel--xp-skillgraphsubsystemcpp)

---

## 1. Spielvision & Core-Loop

Ein kooperatives 3D-Survival- und Siedlungsbauspiel in der Unreal Engine 5. Spieler erkunden eine lebendige, wetterdynamische Welt mit **8 festen NPC-geschützten Orten** (5 Siedlungen, 2 Städte, 1 Hauptstadt) sowie gefährlichen **Höhlen-Zonen (`CaveZone`)**, errichten modulare **3-Stufen-Bauwerke** und **Custom-Blueprints** mit begehbaren Innenräumen und rekrutieren **NPC-Bewohner**, die Bedürfnisse haben, Betten belegen und automatisierte Logistik-, Bau- und Versorgungsaufgaben übernehmen.

### Kern-Kreislauf (Core Loop)

1. **Erkunden & Überleben:** Ressourcen sammeln, Höhlen erkunden, Wetter (`Rain`, `Storm`, `Fog`) und Tageszeiten meistern, `Wetness`, `ColdExposure` und `Stamina` über `Shelter` und Feuerstellen (`ACampfireActor`) regulieren.
2. **Planen & Bauen (3 Stufen):** Einzelbauten oder selbst erstellte **Custom-Blueprints** mit Gelände-Sockelanpassung (`FoundationSkirtExtension`) und Snap-System platzieren und über **Stufe 1 (Fundament)** $\rightarrow$ **Stufe 2 (Rohbau/Begehbar)** $\rightarrow$ **Stufe 3 (Fertiggestellt/Stationen)** ausbauen.
3. **Siedlung & Bewohner führen:** Bewohner in den 8 Karten-Orten rekrutieren, Betten (`ABedActor`) zuweisen, Rollen (`Builder`, `Hauler`, `Caretaker`, `Generalist`) festlegen und Lager- sowie Bau-Prioritäten über die Siedlungs-UI steuern.
4. **Handel & Progression:** Ruf und Ressourcen nutzen, um in Siedlungen, Städten und der Hauptstadt fortgeschrittene 3-Stufen-Blaupausen freizuschalten und die eigene Siedlung im Koop-Team weiterzuentwickeln.

## 2. Weltstruktur & die 8 festen NPC-Orte

Die Welt kombiniert freies Baugelände und unterirdische Höhlen mit **8 festen, geschützten NPC-Orten**, die als sichere Ankerpunkte für Handel, Rekrutierung und Blaupausen-Progression dienen.

### 2.1 Hierarchie der 8 Orte

| Ortstyp | Anzahl | Charakter & Rolle in der Welt | Freischaltungen & Angebot |
| :--- | :---: | :--- | :--- |
| **Kleinere Siedlungen (`SmallSettlement`)** | **5** | Regionale Außenposten mit spezifischem Biom-Fokus (z. B. Holz, Fischerei, Erz, Kräuter, Weide) | Basis-Proviant, frühe Rekruten (`Generalist`), erste Basis-Blaupausen; max. Händler-Stufe 1 |
| **Kleinere Städte (`Town`)** | **2** | Regionale Handels- und Handwerksknoten an wichtigen Routenübergängen | Verarbeitete Güter, größere Handelsmengen, spezialisierte Rekruten (`Builder`, `Hauler`, `Caretaker`), regionale Blaupausen; max. Händler-Stufe 2; Lehrmeister für Voll-Respec |
| **Hauptstadt (`Capital`)** | **1** | Zentrales Macht-, Handwerks- und Handelszentrum mit mehreren Vierteln | Seltene 3-Stufen-Blaupausen, erfahrene Spezialisten, hochwertige Ausrüstung & anspruchsvolle Aufträge; max. Händler-Stufe 3; Lehrmeister für Voll- & Teil-Respec |

### 2.2 Schutz- & Interaktionsregeln der NPC-Orte

* **`NoBuildProtectionRadius`:** Jeder der 8 Orte besitzt einen serverseitig geprüften Schutzradius. Innerhalb dieses Bereichs sind eigenes Bauen (`ABuildableActor`), Custom-Blueprints und das Abreißen oder Beschädigen bestehender Ortsstrukturen blockiert.
* **Sichere Zuflucht:** Reisende Spieler finden in überdachten und geschlossenen Gebäuden der 8 Orte vollen Wetterschutz (`EShelterLevel::Roofed` / `EnclosedOrCave`).

### 2.3 Höhlen-Zonen (`CaveZone`) & Ressourcen-Regeneration

* **Umwelt & Shelter:** In der Tiefe einer `CaveZone` greift `EShelterLevel::EnclosedOrCave` – Außenregen und Sturm treffen den Spieler nicht, aber Dunkelheit und konstante Kälte (`AmbientColdRate`) erfordern Lichtquellen (Fackeln, Laternen, Feuerstellen) und Kälteresistenz.
* **Ressourcen & Gefahren:** Höhlen enthalten hochwertige Erze, Mineralien und seltene Rohstoffe (`MiningTerraform`), werden aber durch Dunkelheit, Engstellen und Höhlenkreaturen geschützt.
* **Anti-Exploit-Regeneration:** Erschöpfte Vorkommen speichern ihren Zeitpunkt (`DepletedAtWorldHours`). Die Regeneration richtet sich ausschließlich nach der **serverseitigen Weltzeit (`TotalWorldElapsedHours`)** (Oberfläche kürzer, Höhlen länger), blockiert bei Überbauung und spawnt erst nach, wenn kein Spieler/Bewohner direkt daneben steht (`MinClearanceRadiusFromPlayers`).

```cpp
USTRUCT(BlueprintType)
struct FCaveZoneConfig {
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite) FName CaveZoneID;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) FText DisplayName;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float AmbientColdRate = 0.02f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float MoistureRetentionFactor = 1.0f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) bool bRequiresLightForGoodVisibility = true;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) TArray<FName> RichResourceItemIDs;
};

USTRUCT(BlueprintType)
struct FResourceNodeSaveState {
    GENERATED_BODY()

    UPROPERTY(SaveGame) FGuid NodeID;
    UPROPERTY(SaveGame) FName ResourceNodeTypeID;
    UPROPERTY(SaveGame) bool bIsDepleted = false;
    UPROPERTY(SaveGame) float DepletedAtWorldHours = -1.0f;
    UPROPERTY(SaveGame) float RespawnCooldownWorldHours = 72.0f;
};
```

## 3. Überlebenssysteme: Wetter, Shelter & Vitalwerte

Die Überlebenswerte (`Wetness`, `ColdExposure`, `Stamina`) wirken nicht isoliert, sondern beeinflussen sich gegenseitig und werden durch `UShelterDetectorComponent`, Wärmequellen und Charakter-Stats moduliert.

### 3.1 Shelter-Stufen (`EShelterLevel`)

Ein 5-Punkt-Ray-Trace-Check (`UShelterDetectorComponent`) ermittelt kontinuierlich den Schutzgrad von Spielern und Bewohnern:

1. **`None`:** Ungeschützt im Freien; Regen und Sturm erhöhen `Wetness`, Wind und Kälte steigern `ColdExposure`.
2. **`Roofed`:** Überdachter Bereich (z. B. Stufe-2-Teildach, Unterstand, fertiges Dach); stoppt direkten Nässezuwachs durch Regen.
3. **`EnclosedOrCave`:** Voll umschlossener Innenraum (Stufe 3) oder `CaveZone`; schirmt Außenwetter und Seitenwind vollständig ab und dämpft Außen-Wettersounds.

### 3.2 Wechselwirkungen der Vitalwerte

* **`Wetness` ($0.0 \rightarrow 1.0$):** Steigt bei Regen/Sturm unter `EShelterLevel::None` sowie bei Wasserkontakt. Sinkt unter `Roofed`/`EnclosedOrCave` und beschleunigt nahe eines aktiven `ACampfireActor`. Hohe Nässe beschleunigt den Anstieg von `ColdExposure`.
* **`ColdExposure` ($0.0 \rightarrow 1.0$):** Steigt bei Nacht, Sturm, Kältebiomen und in Höhlen ohne Wärmequelle – besonders schnell bei hoher `Wetness`. Sinkt an Feuerstellen und in geschlossenen Innenräumen; wird durch Stats wie `Vitality`, `Resolve` und `SurvivalCraft` gepuffert.
* **`Stamina`:** Wird für Sprinten, Klettern, Kampf und Ressourcenabbau verbraucht. Hohe `Wetness` und `ColdExposure` verlangsamen die `Stamina`-Regeneration spürbar; Wärme, trockener Shelter und Nahrung stellen die volle Regeneration wieder her.

## 4. Modulares 3-Stufen-Bausystem, begehbare Räume & Custom-Blueprints

### 4.1 Die drei Baustufen (`CurrentStage` 1 $\rightarrow$ 3)

Jedes Bauwerk (`ABuildableActor`) durchläuft drei physische und funktionale Entwicklungsstufen, die stückweise über `DeliveredInCurrentStage` beliefert und errichtet werden:

| Stufe | Bezeichnung | Begehbarkeit & Shelter | Funktionen & Stationen |
| :---: | :--- | :--- | :--- |
| **Stufe 1** | **Fundament / Grundriss** | `FoundationOnly` – Bodenkollision aktiv, kein Dach (`EShelterLevel::None`) | Baustellen-Anlieferung aktiv; Innen-Stationen noch inaktiv |
| **Stufe 2** | **Rohbau / Wände & Teildach** | `PartialInterior` – Innenraum betretbar, Türen einsetzbar, Dach schützt lokal (`Roofed`) | Erste Basis-Zugänge nutzbar; Struktur wird aufgerichtet |
| **Stufe 3** | **Fertiggestellt** | `FullyWalkable` – Vollständiges Dach & Wände (`Roofed` / `EnclosedOrCave`) | Alle `FInteriorStationSlot`-Plätze, Betten und Arbeitsstationen voll aktiv |

### 4.2 Türen & Innenraum-Slots (`FInteriorStationSlot`)

* **Türen (`FInteriorDoorConfig`):** Besitzen replizierte Zustände (`bIsOpen`, `bIsLocked`) und definierte `EntryPointTransforms`. Spieler öffnen/schließen per Interaktion (`Server_ToggleDoor`); Bewohner-KI öffnet unverschlossene Türen beim Durchqueren selbstständig, damit das `NavMesh` nie blockiert.
* **Arbeits- & Interaktions-Slots (`FInteriorStationSlot`):** Jede Station (Werkbank, Schmelze, Lagerregal, Bett) definiert einen exakten `StandTransform`, eine `RequiredBuildingStage` und eine serverseitige Belegung (`bIsOccupied`, `OccupyingEntityID`), um Clipping und Doppelbelegung zu verhindern.

### 4.3 Custom-Blueprints, Platzierung & Geländeanpassung

Selbst erstellte **Custom-Blueprints** speichern eine Gruppe relativer Einzelteile (`FCustomBlueprintPart`) inklusive Stufen-Zuordnung (`ActiveFromStage` 1–3) und verhalten sich beim Bau wie ein einheitliches 3-Stufen-Projekt:

* **Snap-System (`FBlueprintSnapSocket`):** Kompatible Sockets (z. B. `FoundationEdge`, `CorridorLink`) rasten innerhalb von `SnapRadius` ein; Snap-Anschlüsse haben Vorrang vor freier Höhe.
* **Geländeanpassung (`FoundationSkirtExtension`):** Wände, Dächer und Stationen bleiben waagerecht/lotrecht, während Fundamente einen Sockel nach unten bis ins Gelände ziehen, solange das Gefälle unter `MaxTerrainSlopeDelta` bleibt.
* **Kollision & Vorschau-Feedback:** Die lokale Client-Vorschau hebt konflikthafte Teile (`ConflictingPartIndices`) gezielt hervor und zeigt im Bau-HUD den klaren Grund (`EPlacementRejectionReason`, z. B. Schutzzone, zu steiles Gelände oder Kollision).

### 4.4 Haltbarkeit, Stabilität, Reparatur & Abriss

* **Zustand & Stabilität (`FBuildableConditionState`):** Jedes Bauwerk besitzt `CurrentHP`, `MaxHP` und `Stability`. Sturm, Kreaturen oder fehlende Stützstrukturen reduzieren HP/Stabilität (`bIsDamaged`).
* **Reparatur:** Spieler oder `Builder`-Bewohner stellen `CurrentHP` mit anteiligen Baumaterialien wieder her.
* **Abriss & Rückerstattung:** Ein gültiger Abriss (nur durch berechtigte Spieler außerhalb geschützter NPC-Orte) storniert offene Ressourcen-Reservierungen, löst Bett-/Slot-Verknüpfungen und erstattet **75–85 %** der verbauten Materialien (skaliert nach Zustand/Stufe).

```cpp
USTRUCT(BlueprintType)
struct FInteriorDoorConfig {
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite) FName DoorID;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) int32 UnlockedAtStage = 2;
    UPROPERTY(ReplicatedUsing=OnRep_DoorState, BlueprintReadWrite) bool bIsOpen = false;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) bool bIsLocked = false;
};

USTRUCT(BlueprintType)
struct FCustomBlueprintPart {
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame) FName PartID;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame) FTransform RelativeTransform;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame) int32 ActiveFromStage = 1;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame) FName StationSlotTag;
};

USTRUCT(BlueprintType)
struct FCustomBlueprintDefinition {
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame) FGuid CustomBlueprintID;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame) FText BlueprintName;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame) FGuid CreatorPlayerID;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame) TArray<FCustomBlueprintPart> Parts;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame) TMap<FName, int32> TotalRequiredResources;
};
```

## 5. Bewohner-KI, Rollen & Logistik

### 5.1 Bedürfnisse & Tagesablauf (`AVillagerCharacter`)

Jeder Bewohner verwaltet eigene Bedürfnisse (`Hunger`, `Fatigue`, `Wetness`, `ColdExposure`, `Morale`) und wird über `AVillagerAIController` gesteuert:

* **Schlaf & Betten (`ABedActor`):** Jeder Bewohner kann einem festen Bett (`AssignedBedID` $\leftrightarrow$ `OccupantVillagerID`) zugewiesen werden. Fehlt ein Bett, sinkt die `Morale` und es greift ein Not-Schlafplatz-Fallback.
* **Umwelt-Reaktion:** Bei Sturm oder starker Kälte suchen Bewohner geschützte Innenräume (`Roofed`/`EnclosedOrCave`) oder Feuerstellen auf, bevor ihre Arbeitsleistung durch Kälte/Nässe stark abfällt.

### 5.2 Die vier Bewohner-Rollen (`EVillagerRole`)

| Rolle | Primäre Aufgaben | Besonderer Nutzen im Spielverlauf |
| :--- | :--- | :--- |
| **`Builder`** | Ausbau der Stufen `1` $\rightarrow$ `2` $\rightarrow$ `3`, Arbeit an Handwerks-Slots, Reparatur beschädigter Bauten | Beschleunigt große 3-Stufen-Projekte und hält die Siedlung nach Stürmen instand |
| **`Hauler`** | Transport zwischen Lagern (`StorageContainer`), Sammelpunkten und 3-Stufen-Baustellen | Hält Materialflüsse am Laufen, während Spieler Städte oder Höhlen erkunden |
| **`Caretaker`** | Nahrungs- und Wärmeversorgung, Pflege der Basis-Infrastruktur | Stabilisiert `Hunger`, `ColdExposure` und `Morale` wachsender Siedlungen |
| **`Generalist`** | Flexible Aushilfe bei Bau, Transport und Grundversorgung | Idealer Allrounder für frühe Siedlungsphasen und wechselnde Engpässe |

### 5.3 Reservierungssystem (`FResourceReservation`)

Um Doppelwege und Konflikte zwischen mehreren Bewohnern zu vermeiden, arbeitet der `ULogisticsSubsystem` mit atomaren Reservierungen (`ReservedForPickup` für Lager-Entnahmen und `ReservedIncoming` für Baustellen-Lieferungen). Wird ein Job abgebrochen oder ein Bau abgerissen, werden Reservierungen sofort freigegeben.

## 6. UI/UX, Karten-Navigation & Onboarding

### 6.1 Siedlungs-UI (5-Reiter-Übersicht)

1. **Bewohner (`Villagers`):** Rollen, aktueller Job/Slot, Bedürfnisse (`Hunger`, `Fatigue`, `Morale`), Bett-Status und Direktzuweisung.
2. **Betten & Unterkünfte (`Beds & Housing`):** Übersicht aller `ABedActor` nach Gebäude, Bauzustand, Shelter-Level und Belegung.
3. **Lager & Ressourcen (`Storage & Logistics`):** Gesamtbestand, reservierte Menge (`ReservedForPickup`), freier Bestand und Überlauf-Hinweise.
4. **3-Stufen-Aufgaben (`Construction & Stages`):** Offene Stufen `1/3` bis `3/3`, fehlende Materialien, HP/Reparaturbedarf und Prioritätssteuerung.
5. **Ko-op & Rechte (`Co-op & Permissions`):** Eigentümer (`OwnerPlayerID`), Rechte-Stufen (`OwnerOnly`, `AllowFinishBuild`, `AllowBuildAndBreak`) und Online-/Offline-Status der Mitspieler.

### 6.2 Weltkarte, Kompass & atmosphärische Sichtbarkeitsregeln

* **Weltkarte (`WorldMap`):** Zeigt die **8 festen NPC-Orte** (inkl. optionalem `NoBuildProtectionRadius`), entdeckte **Höhlen-Zonen**, **eigene 3-Stufen-Bauten** (mit Stufen- und Schadens-Status), **Koop-Mitspieler** sowie persönliche/geteilte Wegpunkte (`CustomWaypoint`).
* **Dynamische Sichtbarkeit bei Wetter, Nacht & Höhlen:**
  * Bei **Nebel (`Fog`)** und **Sturm (`Storm`)** sinkt die Reichweite ungetrackter 3D-Welt-Marker, während getrackte Wegpunkte und nahe Shelter/Orte kontrastreich im Kompass lesbar bleiben.
  * Bei **Nacht** nutzen Marker gedämpfte, kontraststarke Konturen ohne grelles Neon-Leuchten.
  * In **Höhlen-Zonen (`CaveZone`)** schaltet der Kompass auf den **Nah-/Raum-Modus** (Höhlenausgang, eigene gesetzte Markierungen, Koop-Partner im selben Bereich) und blendet ferne Außen-Orte aus.

### 6.3 Onboarding-Loop & Übergang in die offene Welt

1. **Etappe 1 – Ankunft:** Bewegung, Ressourcenaufnahme und Kompass/Wegpunkt-Orientierung Richtung nächster `SmallSettlement`.
2. **Etappe 2 – Erstes Shelter & Feuer:** Wetterumschwung erleben, Dach (`Roofed`) und `ACampfireActor` nutzen, um `Wetness` und `ColdExposure` abzubauen.
3. **Etappe 3 – Erster 3-Stufen-Bau:** Erste Blaupause platzieren und schrittweise über Stufe 1, 2 und 3 bis zur begehbaren Nutzung ausbauen.
4. **Etappe 4 – Erster Bewohner:** Bewohner gewinnen, Bett zuweisen und Rolle (`Builder` / `Hauler`) aktivieren.
5. **Übergang in die offene Welt:** Begrenzte lokale Vorkommen und höhere Stufen-/Custom-Blueprint-Ziele führen natürlich zu den **5 Siedlungen**, in **Höhlen-Zonen** und weiter über die **2 Städte** bis zur **Hauptstadt**.

## 7. SaveGame-Architektur, Grenzfall-Bereinigung & Koop-Multiplayer

### 7.1 SaveGame-Modell (`USurvivalWorldSaveGame`)

Persistente Daten werden als strukturierte Records gespeichert und beim Laden deterministisch rekonstruiert:

* **Welt & Wetter:** `SaveVersion`, `DayCount`, `WorldTimeOfDayHours`, `TotalWorldElapsedHours`, `ActiveWeatherType`, `LocationProgress` (für alle 8 Orte), `DiscoveredCaveIDs`, `CaveStates` und `CustomWaypoints`.
* **Bauten & Custom-Blueprints:** `CustomBlueprintDefinitions` sowie alle `Buildables` (`InstanceID`, `BlueprintID`, `WorldTransform`, `CurrentStage`, `DeliveredInCurrentStage`, `CurrentHP`, `Stability`, `OwnerPlayerID`, Türzustände).
* **Bewohner & Lager:** `Villagers` (`VillagerID`, Rolle, `AssignedBedID`, Bedürfnisse, Inventar) und `StorageInventories`.

```cpp
UCLASS()
class USurvivalWorldSaveGame : public USaveGame {
    GENERATED_BODY()

public:
    UPROPERTY(SaveGame) int32 SaveVersion = 1;
    UPROPERTY(SaveGame) FDateTime SavedAtUTC;
    UPROPERTY(SaveGame) float WorldTimeOfDayHours = 8.0f;
    UPROPERTY(SaveGame) float TotalWorldElapsedHours = 0.0f;
    UPROPERTY(SaveGame) int32 DayCount = 1;
    UPROPERTY(SaveGame) uint8 ActiveWeatherType = 0;

    UPROPERTY(SaveGame) TArray<FCustomBlueprintDefinition> CustomBlueprintDefinitions;
    UPROPERTY(SaveGame) TArray<FSavedBuildableActor> Buildables;
    UPROPERTY(SaveGame) TMap<FGuid, TArray<FSavedInventoryItem>> StorageInventories;
    UPROPERTY(SaveGame) TArray<FSavedVillagerRecord> Villagers;
    UPROPERTY(SaveGame) TArray<FSavedLocationProgress> LocationProgress;
    UPROPERTY(SaveGame) TArray<FCaveZoneSavedState> CaveStates;
    UPROPERTY(SaveGame) TArray<FSavedCustomWaypoint> CustomWaypoints;
    UPROPERTY(SaveGame) TArray<FName> GloballyUnlockedBlueprints;
};
```

### 7.2 Sanitize- & Reconcile-Regeln beim Laden

* **Fehlende oder unfertige Betten:** Existiert `AssignedBedID` nicht mehr oder ist das Bett nicht nutzbar, wird die Zuweisung bereinigt, der Bewohner fällt auf freie Betten/Not-Schlafplätze zurück und die Siedlungs-UI meldet den Engpass.
* **Lager-Überlauf & Reservierungen:** Flüchtige `ReservedForPickup`-Zähler werden beim Laden auf `0` gesetzt und neu berechnet. Überschreitet ein gespeichertes Inventar die aktuelle Container-Kapazität, werden überschüssige Items in freie Lager umverteilt oder als Sammel-Bündel am Lager gesichert.
* **Schema-Migration (`SaveVersion`):** Ältere Spielstände durchlaufen vor dem Welt-Spawn eine gestufte Migrationskette (`MigrateSave_vN_to_vNPlus1`), die neue Felder mit sicheren Standardwerten belegt.

### 7.3 Koop-Multiplayer-Architektur (Server-Autorität)

* **Autoritative Simulation:** Bauplatzierung, `NoBuildProtectionRadius`, Stufenfortschritt (`1–3`), Tür- und Slot-Belegungen, Ressourcen-Reservierungen, Vorkommen-Regeneration sowie Wetter und Tageszeit laufen serverseitig und werden repliziert.
* **Persistente Spieler-Identität:** Eigentum (`OwnerPlayerID`), Baurechte (`FObjectPermissions`) und persönliche Wegpunkte sind an eine dauerhafte Spieler-ID gebunden, sodass Bauten und Bewohner auch dann aktiv bleiben, wenn einzelne Mitspieler offline sind oder Sessions wechseln.

## 8. Eigenentwickeltes Skill-Graph-System (21 Knoten & 4 Anker)

Das Skill-System ist als eigenständiger, datengetriebener **Skill-Graph (`USkillGraphSubsystem`)** entworfen und verbindet **16 reguläre Knoten (`N01`–`N16`)** sowie **4 große Anker-Meilensteine (`A01`–`A04`)** direkt mit `CaveZone`, 3-Stufen-Bau, Stationen/`FInteriorStationSlot`, Lager/`StorageContainer`, Bewohnern (`Builder`, `Hauler`, `Caretaker`, `Generalist`) und den **8 Karten-Orten**.

### 8.1 Netz-Topologie, UND/ODER-Abhängigkeiten & Dependency-Lock

* **21 Knoten insgesamt:** `16` reguläre Knoten (`N01`–`N16`) plus `4` große Anker-Meilensteine (`A01`–`A04`), gegliedert in vier thematische Cluster (`ClusterA_SurvivalCave`, `ClusterB_ThreeStageBuild`, `ClusterC_StationLogistics`, `ClusterD_HubsSettlers`) inklusive Querbrücken (`N12`, `N16`).
* **`UND` / `ODER`-Abhängigkeiten (`ESkillDependencyMode`):** Knoten können `AnyIncomingEdge` (`ODER`) oder `AllIncomingEdges` (`UND`) verlangen – inklusive Rang-Schwellen einzelner Vorgängerknoten und Level-Anforderungen.
* **Dependency-Lock:** Ein Knoten oder Anker kann nicht abgebaut/zurückgesetzt werden, solange andere aktive Knoten oder Anker auf ihn als Voraussetzung angewiesen sind.

### 8.2 Die 6 Anker-Resonanzen (`A01`–`A04`)

Sobald mindestens **zwei** der vier großen Anker (`Rang >= 1`) gleichzeitig aktiv sind, greifen **6 definierte Anker-Resonanzen** als Synergie-Boni ohne zusätzlichen Skillpunkt-Aufwand:

1. **Außenposten-Pionier (`A01` + `A02`):** Verbessert Bau-Tempo (`Build.StageSpeed`) und dämpft Nässe-Zuwachs (`Survival.WetnessRate`).
2. **Tiefenschmied (`A01` + `A03`):** Stärkt Fertigungstempo (`Station.CraftSpeed`) und Erz-Ausbeute in Höhlen (`Cave.OreYieldBonus`).
3. **Meister-Manufaktur (`A02` + `A03`):** Hebt Werkstätten-Tempo (`Station.CraftSpeed`) und Lager-Kapazität (`Storage.CapacityBonus`) in 3-Stufen-Bauten.
4. **Architekt des Reiches (`A02` + `A04`):** Verbessert Bau-Stabilität (`Build.StabilityBonus`) und Orts-Ruf-Zuwachs (`Hub.ReputationGainMult`).
5. **Handels-Kontor (`A03` + `A04`):** Beschleunigt Transport-/Logistikketten (`Storage.HaulerSpeed`) und stärkt Orts-Ruf (`Hub.ReputationGainMult`).
6. **Pfadfinder der Krone (`A01` + `A04`):** Senkt Kältebelastung (`Survival.ColdExposureRate`) und steigert Orts-Ruf (`Hub.ReputationGainMult`).

### 8.3 Aktive Fähigkeiten, Gegenstände, Blaupausen & Lehrmeister-Respec

* **Aktive Fähigkeiten:** Ausgewählte Knoten und Anker schalten ab einem bestimmten Rang aktive Fähigkeiten mit Cooldown und Stamina-Kosten frei (z. B. *Tiefensicht-Ruf*, *Not-Abstützung*, *Präzisions-Takt*, *Arbeits-Appell* sowie die Anker-Fähigkeiten *Sturm-Trotz*, *Blaupausen-Richtfest*, *Nachschub-Befehl* und *Geleit- & Gilden-Präsenz*).
* **Gegenstände, 3-Stufen-Blaupausen & Händler-Waren (`FSkillUnlockedItemGrant`):** Knoten und Anker schalten Werkzeuge, Ausrüstung, 3-Stufen-Blaupausen und Sonderwaren in den **2 kleineren Städten** und der **Hauptstadt** frei.
* **Single Source of Truth & Respec:** Freischaltungen hängen an `UnlockedSkillRanks`. Bei einem **Respec** beim **Gilden-Lehrmeister** (Voll-Respec in den **2 kleineren Städten** und der **Hauptstadt**; **Teil-Respec** zusätzlich in der **Hauptstadt**) entfallen nicht mehr aktive Rezepte, Blaupausen-Zugänge und Fähigkeiten sofort – **bereits gebaute 3-Stufen-Strukturen oder fertige Items im Inventar/Lager bleiben erhalten**.

### 8.4 Komplette Übersicht aller 21 Knoten (`N01`–`N16` & `A01`–`A04`)

| ID | Name | Rang-Cap | UND / ODER-Abhängigkeiten | Modifikator-Hooks | Aktive Fähigkeit | Gegenstände / Blaupausen / Händler-Waren |
| :--- | :--- | :---: | :--- | :--- | :--- | :--- |
| **`N01`** | Pionier-Instinkt | `3` | Startknoten | `Survival.WetnessRate`, `Player.CarryWeightBonus` | — | **Rang 1:** Wander-Rucksack & Holz-Schlegel (`CraftingRecipe`) |
| **`N02`** | Wetterfeste Kleidung | `3` | **ODER** nach `N01` ($\ge 1$) | `Survival.WetnessRate`, `Survival.ColdExposureRate` | — | **Rang 2:** Geölter Regenumhang & Wachs-Kappe (`CraftingRecipe`) |
| **`N03`** | Fährtenleser im Nebel | `2` | **ODER** nach `N02` ($\ge 1$) | `Player.SprintStaminaCost`, `Cave.FocusPingRange` | — | **Rang 2:** Signal-Fackel & Markierungs-Pfähle (`CraftingRecipe`) |
| **`N04`** | Höhlenläufer & Erzspürer | `3` | **UND** `N02` ($\ge 2$) **UND** `N03` ($\ge 1$) | `Cave.OreYieldBonus`, `Survival.ColdExposureRate` | **Rang 2:** *Tiefensicht-Ruf* | **Rang 2:** Gruben-Laterne & Verstärkte Spitzhacke (`CraftingRecipe`) |
| **`N05`** | Zupackender Bauherr | `3` | **ODER** nach `N01` ($\ge 1$) | `Build.StageSpeed`, `Player.BuildStaminaCost` | — | **Rang 2:** Zimmermanns-Hammer (gehärtet) (`CraftingRecipe`) |
| **`N06`** | Hang- & Fundament-Statik | `2` | **ODER** nach `N05` ($\ge 1$) | `Build.StabilityBonus`, `Build.SlopeTolerance` | **Rang 2:** *Not-Abstützung* | **Rang 2:** Stelzen- & Hangfundament Stufe 1–3 (`BuildBlueprint`) |
| **`N07`** | Begehbarer Innenausbau | `3` | **UND** `N05` ($\ge 2$) **UND** `N06` ($\ge 1$) | `Build.InteriorMaxHP`, `Build.ShelterRadius` | — | **Rang 2:** Gepolstertes Bett (`ABedActor`) & Vorrats-/Werkraum (`CraftingRecipe` + `BuildBlueprint`) |
| **`N08`** | Sauberer Rückbau | `2` | **ODER** nach `N07` ($\ge 1$) | `Build.DeconstructRefund`, `Build.RepairMaterialCost` | — | **Rang 2:** Brecheisen & Rückbau-Schablone (`CraftingRecipe`) |
| **`N09`** | Schwerer Träger | `3` | **ODER** nach `N01` ($\ge 1$) oder `N05` ($\ge 1$) | `Player.CarryWeightBonus`, `Player.HaulStaminaCost` | — | **Rang 2:** Handkarren-Gurt & Schwerlast-Tragegestell (`CraftingRecipe`) |
| **`N10`** | Präzisions-Werkstatt | `3` | **ODER** nach `N09` ($\ge 1$) | `Station.CraftSpeed`, `Station.ToolDurabilityMult` | **Rang 2:** *Präzisions-Takt* | **Rang 2:** Feinwerkzeug-Satz (`CraftingRecipe`) |
| **`N11`** | Lager-Ordnung | `2` | **ODER** nach `N09` ($\ge 1$) oder `N10` ($\ge 1$) | `Storage.CapacityBonus`, `Storage.PickupSyncMult` | — | **Rang 2:** Verstärkte Lagertruhe / Vorratsregal (`BuildBlueprint`) |
| **`N12`** | Rohstoff-Veredelung | `2` | **UND** `N04` ($\ge 1$) **UND** `N10` ($\ge 1$) | `Station.RefineYieldBonus`, `Cave.OreYieldBonus` | — | **Rang 1:** Schmelz-/Poch-Einsatz für Werkstation (`CraftingRecipe`) |
| **`N13`** | Vertrauenswürdiger Reisender | `3` | **ODER** nach `N01` ($\ge 1$) oder `N09` ($\ge 1$) | `Hub.ReputationGainMult`, `Hub.VendorPriceDiscount` | — | **Rang 2:** Reise-Proviant & Siegel-Marken in 2 Städten & Hauptstadt (`VendorStock`) |
| **`N14`** | Vorbild für die Gemeinschaft | `3` | **ODER** nach `N13` ($\ge 1$) | `Settler.WorkSpeedMultiplier`, `Settler.MoraleBonus` | **Rang 2:** *Arbeits-Appell* | **Rang 2:** Festtafel-Set / Gemeinschafts-Banner (`CraftingRecipe`) |
| **`N15`** | Versorgungs-Planer | `2` | **ODER** nach `N13` ($\ge 1$) oder `N14` ($\ge 1$) | `Settler.HungerFatigueRate`, `Settler.BedRecoveryMult` | — | **Rang 2:** Siedlungs-Speisekammer / Ruhekoje (`BuildBlueprint`) |
| **`N16`** | Gilden-Kontakte | `2` | **UND** `N07` ($\ge 1$) **UND** `N14` ($\ge 1$) | `Hub.RecruitCostDiscount`, `Hub.BlueprintCostDiscount` | — | **Rang 1:** Gilden-Empfehlungsschreiben & Sonder-Blaupausen (`VendorStock`) |
| **`A01`** | Herr der Elemente & Tiefe | `1` | **UND** `N04` ($\ge 3$) + (`N03` $\ge 1$ oder `N12` $\ge 1$) | `Survival.WetnessRate`, `Survival.ColdExposureRate`, `Cave.ShelterBonus` | *Sturm-Trotz* | **Rang 1:** Tiefen-Schutzunterstand 3-Stufen (`BuildBlueprint`) + *Sturm-Harz-Öl* (`VendorStock`) |
| **`A02`** | Meister der Drei Stufen | `1` | **UND** `N07` ($\ge 3$) + (`N06` $\ge 1$ oder `N16` $\ge 1$) | `Build.CustomBlueprintLimit`, `Build.Stage2StationBonus`, `Build.StabilityBonus` | *Blaupausen-Richtfest* | **Rang 1:** Große Hallen- & Wach-Struktur 3-Stufen (`BuildBlueprint`) + *Gilden-Balkenwerk* (`VendorStock`) |
| **`A03`** | Meister der Werkstätten & Lager | `1` | **UND** `N10` ($\ge 3$) **UND** `N11` ($\ge 1$) | `Storage.DirectAccessEnabled`, `Station.CraftSpeed`, `Storage.CapacityBonus` | *Nachschub-Befehl* | **Rang 1:** Kontor-Lagerhaus 3-Stufen (`BuildBlueprint`) + *Präzisions-Gussformen* (`VendorStock`) |
| **`A04`** | Stimme der Acht Orte | `1` | **UND** `N14` ($\ge 3$) **UND** `N15` ($\ge 1$) (+ Stadt/Hauptstadt) | `Settler.WeatherMoraleResist`, `Hub.ReputationGainMult`, `Hub.RareAccessUnlocked` | *Geleit- & Gilden-Präsenz* | **Rang 1:** Versammlungs-/Handelshalle 3-Stufen (`BuildBlueprint`) + *Reichs-Blaupausen & Spezial-Ausrüstung* (`VendorStock`) |

## 9. Händler-Stufen, Orts-Ruf (`0–1000`) & Auftrags-/Meilenstein-Quellen

### 9.1 Ruf-Stufen & Staffelung über die 8 Karten-Orte

Jeder der **8 Orte** führt einen eigenen Rufwert (`0–1000`):

* **Stufe 1 – Bekannter (`0–299`):** Basis-Versorgung & Einstiegs-Rezepte; Preisfaktor `1.10x`.
* **Stufe 2 – Vertrauter (`300–699`):** Regionale 3-Stufen-Blaupausen, bessere Werkzeuge & Sonderwaren (bei erfüllter Skill-Bedingung); Preisfaktor `1.00x`.
* **Stufe 3 – Ehrenbürger / Gildenfreund (`700–1000`):** Spitzen-Blaupausen, seltene Sonderwaren & Spezial-Rekruten (bei erfüllter Skill-Bedingung); Preisfaktor `0.85x`.

**Ortstyp-Obergrenzen:**

* **5 kleinere Siedlungen:** Max. Händler-Stufe `1`; Alltagswaren, Grund-Proviant, einfache Reparatur-/Werkzeug-Items & frühe 3-Stufen-Grundbauten; **kein Lehrmeister / kein Respec**.
* **2 kleinere Städte:** Max. Händler-Stufe `2`; regionale 3-Stufen-Blaupausen, mittlere Werkzeuge/Stationen, Zwischenprodukte & Sonderwaren; **Gilden-Lehrmeister für Voll-Respec**.
* **1 Hauptstadt:** Max. Händler-Stufe `3`; seltene 3-Stufen-Blaupausen, Repräsentations-/Großbauten, Spitzen-Werkstoffe & exklusive Sonderwaren; **Gilden-Lehrmeister für Voll- & Teil-Respec**.

### 9.2 Aufträge, Lieferungen & Meilensteine nach Ortstyp

* **5 kleinere Siedlungen:**
  * *Aufträge:* Nah-Gefahren sichern, Wege/Laternen/Brücken instand setzen, Späher/Vorräte suchen.
  * *Lieferungen:* Brennstoff, Proviant, einfaches Werkzeug oder Bauholz.
  * *Meilensteine:* Angrenzende `CaveZone`/Gefahrenstelle sichern; Erreichen von `300` Ruf (`Stufe 2`).
* **2 kleinere Städte:**
  * *Aufträge:* Gilden-/Handwerksaufträge, tiefere `CaveZone`-Abschnitte räumen, Stadtausbau unterstützen.
  * *Lieferungen:* Veredelte Güter und größere Rohstoff-Kontingente.
  * *Meilensteine:* Gilden-Anerkennung durch mehrere Aufträge; Erreichen von `700` Ruf (`Stufe 3`).
* **1 Hauptstadt:**
  * *Aufträge:* Reichs-/Sonderaufträge, seltene Tiefen-/Handelsgüter beschaffen, überregionale Versorgung.
  * *Lieferungen:* Seltene Werkstoffe und Spitzen-Erzeugnisse.
  * *Meilensteine:* Große 3-Stufen-Bauwerke & Anker-Meilensteine (`A01`–`A04`); Erreichen von `300` und `700` Ruf.

## 10. XP-Progression bis Level 50 (49 Skillpunkte) & Meilenstein-XP

### 10.1 XP-Kurve & Skillpunkt-Budget

* **Maximaler Charakter-Level:** `50` mit insgesamt **`49` Skillpunkten** (`+1` Skillpunkt pro Level-Up von Level `1` auf `50`).
* **Benötigte XP für nächstes Level (`1..49`):**

$$\text{Benötigte XP}(\text{Level}) = \max\left(50,\; \text{gerundet}\left(120 + 25 \cdot (\text{Level} - 1) + 1.25 \cdot (\text{Level} - 1)^2\right)\right)$$

### 10.2 Konkrete `BaseXPAmount`-Richtwerte nach Kategorie

| Kategorie | Aktion / Meilenstein | `BaseXPAmount` | `bIsMajorMilestone` |
| :--- | :--- | :---: | :---: |
| **`ThreeStageBuild`** | Rohbau / Stufe 1 abgeschlossen | `35` | `false` |
| **`ThreeStageBuild`** | Ausbau / Stufe 2 abgeschlossen | `75` | `false` |
| **`ThreeStageBuild`** | Stufe 3 fertiggestellt (Vollendung) | `180` | `true` |
| **`ThreeStageBuild`** | Custom-Blueprint errichtet | `240` | `true` |
| **`ThreeStageBuild`** | Beschädigtes Bauteil repariert | `25` | `false` |
| **`CaveZone`** | Neue `CaveZone` / tieferer Sektor betreten | `65` | `false` |
| **`CaveZone`** | Wertvolle Erz-Ader / Rohstoff-Knoten abgebaut | `45` | `false` |
| **`CaveZone`** | Rauer Wetter-/Höhlen-Trip unbeschadet überstanden | `80` | `false` |
| **`CaveZone`** | Höhlen-Meilenstein / Tiefen-Sicherung | `220` | `true` |
| **`TradeAndHubs`** | Kleine Lieferung / Tagesauftrag (5 Siedlungen) | `40` | `false` |
| **`TradeAndHubs`** | Gilden-Auftrag / Lieferkette (2 kleinere Städte) | `110` | `false` |
| **`TradeAndHubs`** | Reichs- / Sonderauftrag (1 Hauptstadt) | `210` | `true` |
| **`TradeAndHubs`** | Ruf-Stufe erreicht (`Stufe 2` / `Stufe 3`) | `190` | `true` |
| **`TradeAndHubs`** | Neue Blaupause beim Händler erworben | `95` | `false` |
| **`SettlerMilestone`** | Neuer Bewohner angeschlossen / zugewiesen | `90` | `false` |
| **`SettlerMilestone`** | Dauerhaft hohe Moral / Versorgung gesichert | `70` | `false` |
| **`SettlerMilestone`** | Siedlungs-Ausbau / Gemeinschafts-Meilenstein | `230` | `true` |

## 11. C++-Implementierungsreferenz (`USkillGraphSubsystem`)

### 11.1 Header-Strukturen & Subsystem-Schnittstelle (`SkillGraphSubsystem.h`)

```cpp
#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "SkillGraphSubsystem.generated.h"

UENUM(BlueprintType)
enum class ESkillDependencyMode : uint8 {
    AllIncomingEdges,
    AnyIncomingEdge
};

UENUM(BlueprintType)
enum class ESkillNodeVisualType : uint8 {
    StandardNode,
    AnchorMilestone
};

UENUM(BlueprintType)
enum class EHubSettlementType : uint8 {
    MinorSettlement,
    MinorCity,
    Capital
};

UENUM(BlueprintType)
enum class ERespecScope : uint8 {
    FullTreeRespec,
    PartialCapitalRespec
};

UENUM(BlueprintType)
enum class EReputationTier : uint8 {
    Tier1_Acquaintance = 1,
    Tier2_Trusted      = 2,
    Tier3_Honored      = 3
};

UENUM(BlueprintType)
enum class EAnchorResonanceType : uint8 {
    None,
    OutpostPioneer,
    DeepSmith,
    MasterManufactory,
    RealmArchitect,
    TradeEmporium,
    CrownPathfinder
};

UENUM(BlueprintType)
enum class EXPMilestoneCategory : uint8 {
    ThreeStageBuild,
    CaveZone,
    TradeAndHubs,
    SettlerMilestone
};

USTRUCT(BlueprintType)
struct FSkillDependencyRule {
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName RequiredSkillID = NAME_None;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 MinRequiredRank = 1;
};

USTRUCT(BlueprintType)
struct FSkillModifierHook {
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName ModifierKey = NAME_None;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float ValuePerRank = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bIsMultiplicative = true;
};

USTRUCT(BlueprintType)
struct FSkillUnlockedAbility {
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName AbilityID = NAME_None;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FText AbilityName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 RequiredRankToUnlock = 2;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float CooldownSeconds = 30.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float StaminaCost = 10.0f;
};

USTRUCT(BlueprintType)
struct FSkillUnlockedItemGrant {
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName ItemOrRecipeID = NAME_None;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FText DisplayName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 RequiredRankToUnlock = 2;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName UnlockCategory = TEXT("CraftingRecipe");
};

USTRUCT(BlueprintType)
struct FSkillGraphNode {
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName SkillID = NAME_None;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FText DisplayName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName ClusterTag = NAME_None;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    ESkillNodeVisualType VisualType = ESkillNodeVisualType::StandardNode;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 MaxRank = 3;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 PointCostPerRank = 1;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 MinCharacterLevel = 1;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    ESkillDependencyMode DependencyMode = ESkillDependencyMode::AnyIncomingEdge;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FSkillDependencyRule> Prerequisites;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FSkillModifierHook> Modifiers;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bGrantsAbility = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FSkillUnlockedAbility UnlockedAbility;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FSkillUnlockedItemGrant> UnlockedItems;
};

USTRUCT(BlueprintType)
struct FAnchorResonanceDefinition {
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    EAnchorResonanceType ResonanceType = EAnchorResonanceType::None;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName AnchorA = NAME_None;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName AnchorB = NAME_None;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FText DisplayName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FText Description;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FSkillModifierHook> ResonanceModifiers;
};

USTRUCT(BlueprintType)
struct FHubVendorOffer {
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName OfferID = NAME_None;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FText OfferTitle;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName ItemOrBlueprintID = NAME_None;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 RequiredVendorTier = 1;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 RequiredLocalReputation = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName RequiredSkillID = NAME_None;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 RequiredSkillRank = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 BasePrice = 100;
};

USTRUCT(BlueprintType)
struct FPlayerSkillTreeState {
    GENERATED_BODY()

    UPROPERTY(SaveGame, BlueprintReadWrite)
    int32 SchemaVersion = 1;

    UPROPERTY(SaveGame, BlueprintReadWrite)
    int32 CharacterLevel = 1;

    UPROPERTY(SaveGame, BlueprintReadWrite)
    int32 CurrentXP = 0;

    UPROPERTY(SaveGame, BlueprintReadWrite)
    int32 AvailableSkillPoints = 0;

    UPROPERTY(SaveGame, BlueprintReadWrite)
    TMap<FName, int32> UnlockedSkillRanks;

    UPROPERTY(SaveGame, BlueprintReadWrite)
    TMap<FName, float> ActiveAbilityCooldowns;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSkillTreeStateChanged, const FPlayerSkillTreeState&, UpdatedState);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnPlayerLeveledUp, int32, OldLevel, int32, NewLevel, int32, AwardedSkillPoints);

UCLASS()
class USkillGraphSubsystem : public UGameInstanceSubsystem {
    GENERATED_BODY()

public:
    static constexpr int32 CurrentSkillSaveSchemaVersion = 1;
    static constexpr int32 MaxCharacterLevelCap = 50;
    static constexpr int32 MaxTotalSkillPointsCap = 49;
    static constexpr int32 MinLocalReputation = 0;
    static constexpr int32 MaxLocalReputation = 1000;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SkillGraph")
    TMap<FName, FSkillGraphNode> SkillNodes;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SkillGraph|Resonance")
    TArray<FAnchorResonanceDefinition> ConfiguredResonances;

    UPROPERTY(BlueprintAssignable, Category = "SkillGraph|Events")
    FOnSkillTreeStateChanged OnSkillTreeStateChanged;

    UPROPERTY(BlueprintAssignable, Category = "SkillGraph|Progression")
    FOnPlayerLeveledUp OnPlayerLeveledUp;

    virtual void Initialize(FSubsystemCollectionBase& Collection) override;

    UFUNCTION(BlueprintCallable, Category = "SkillGraph")
    void InitializeDefaultSkillGraph();

    UFUNCTION(BlueprintCallable, Category = "SkillGraph")
    int32 GetNodeRank(const FPlayerSkillTreeState& State, FName SkillID) const;

    UFUNCTION(BlueprintCallable, Category = "SkillGraph")
    int32 GetMaxTotalSkillPointsForLevel(int32 CharacterLevel) const;

    UFUNCTION(BlueprintCallable, Category = "SkillGraph")
    bool CanUnlockNode(const FPlayerSkillTreeState& State, FName SkillID, FText& OutFailureReason) const;

    UFUNCTION(BlueprintCallable, Category = "SkillGraph")
    bool IsNodeLockedByDependency(const FPlayerSkillTreeState& State, FName SkillID, FName& OutBlockingSkillID) const;

    UFUNCTION(BlueprintCallable, Category = "SkillGraph")
    bool CanRespecAtHub(EHubSettlementType HubType, ERespecScope Scope) const;

    UFUNCTION(BlueprintCallable, Category = "SkillGraph|Modifiers")
    float GetAggregatedModifier(const FPlayerSkillTreeState& State, FName ModifierKey, float BaseValue) const;

    UFUNCTION(BlueprintCallable, Category = "SkillGraph|Resonance")
    TArray<FAnchorResonanceDefinition> EvaluateActiveResonances(const FPlayerSkillTreeState& State) const;

    UFUNCTION(BlueprintCallable, Category = "SkillGraph|Modifiers")
    float GetAggregatedModifierWithResonances(const FPlayerSkillTreeState& State, FName ModifierKey, float BaseValue) const;

    UFUNCTION(BlueprintCallable, Category = "SkillGraph|Items")
    TArray<FSkillUnlockedItemGrant> GetUnlockedItemsForState(const FPlayerSkillTreeState& State) const;

    UFUNCTION(BlueprintCallable, Category = "SkillGraph|Reputation")
    EReputationTier GetReputationTier(int32 CurrentReputation) const;

    UFUNCTION(BlueprintCallable, Category = "SkillGraph|Reputation")
    int32 ApplyReputationGain(
        const FPlayerSkillTreeState& State,
        UPARAM(ref) int32& InOutReputation,
        int32 BaseReputationGain,
        EHubSettlementType HubType,
        bool bIsCaveOrRouteMilestone = false
    ) const;

    UFUNCTION(BlueprintCallable, Category = "SkillGraph|Vendor")
    int32 GetMaxVendorTierForHubType(EHubSettlementType HubType) const;

    UFUNCTION(BlueprintCallable, Category = "SkillGraph|Vendor")
    bool CanPurchaseVendorOffer(
        const FPlayerSkillTreeState& State,
        const FHubVendorOffer& Offer,
        EHubSettlementType HubType,
        int32 CurrentLocalReputation,
        FText& OutDenialReason
    ) const;

    UFUNCTION(BlueprintCallable, Category = "SkillGraph|Vendor")
    int32 CalculateFinalVendorPrice(
        const FPlayerSkillTreeState& State,
        int32 BasePrice,
        EHubSettlementType HubType,
        int32 CurrentReputation,
        bool bIsBlueprintOrRareStock = false
    ) const;

    UFUNCTION(BlueprintCallable, Category = "SkillGraph|Progression")
    int32 GetXPRequiredForNextLevel(int32 CurrentLevel) const;

    UFUNCTION(BlueprintCallable, Category = "SkillGraph|Progression")
    int32 GetCumulativeXPForLevel(int32 TargetLevel) const;

    UFUNCTION(BlueprintCallable, Category = "SkillGraph|Progression")
    int32 AwardMilestoneXP(
        UObject* WorldContextObject,
        FPlayerSkillTreeState& InOutState,
        EXPMilestoneCategory Category,
        int32 BaseXPAmount,
        EHubSettlementType HubType = EHubSettlementType::MinorSettlement,
        bool bIsMajorMilestone = false
    );

    UFUNCTION(BlueprintCallable, Category = "SkillGraph|SaveGame")
    void ValidateAndSanitizeSaveState(FPlayerSkillTreeState& InOutState) const;

private:
    bool MatchesAnchorPair(FName FirstAnchor, FName SecondAnchor, FName ExpectedA, FName ExpectedB) const;
    FAnchorResonanceDefinition BuildDefaultResonanceDefinition(EAnchorResonanceType Type, FName AnchorA, FName AnchorB) const;
};
```

### 11.2 Initialisierung, Resonanzen, Ruf, Handel & XP (`SkillGraphSubsystem.cpp`)

```cpp
#include "SkillGraphSubsystem.h"

namespace SkillGraphInitHelpers {
    FSkillDependencyRule MakeRule(const TCHAR* SkillID, int32 MinRank) {
        FSkillDependencyRule Rule;
        Rule.RequiredSkillID = FName(SkillID);
        Rule.MinRequiredRank = MinRank;
        return Rule;
    }

    FSkillModifierHook MakeMod(const TCHAR* Key, float ValuePerRank, bool bMultiplicative) {
        FSkillModifierHook Hook;
        Hook.ModifierKey = FName(Key);
        Hook.ValuePerRank = ValuePerRank;
        Hook.bIsMultiplicative = bMultiplicative;
        return Hook;
    }

    FSkillUnlockedAbility MakeAbility(const TCHAR* AbilityID, const FText& Name, int32 RequiredRank, float CooldownSec, float StaminaCost) {
        FSkillUnlockedAbility Ability;
        Ability.AbilityID = FName(AbilityID);
        Ability.AbilityName = Name;
        Ability.RequiredRankToUnlock = RequiredRank;
        Ability.CooldownSeconds = CooldownSec;
        Ability.StaminaCost = StaminaCost;
        return Ability;
    }

    FSkillUnlockedItemGrant MakeItemGrant(const TCHAR* ItemOrRecipeID, const FText& Name, int32 RequiredRank, const TCHAR* Category) {
        FSkillUnlockedItemGrant Grant;
        Grant.ItemOrRecipeID = FName(ItemOrRecipeID);
        Grant.DisplayName = Name;
        Grant.RequiredRankToUnlock = RequiredRank;
        Grant.UnlockCategory = FName(Category);
        return Grant;
    }
}

void USkillGraphSubsystem::Initialize(FSubsystemCollectionBase& Collection) {
    Super::Initialize(Collection);
    if (SkillNodes.Num() == 0) {
        InitializeDefaultSkillGraph();
    }
}

void USkillGraphSubsystem::InitializeDefaultSkillGraph() {
    using namespace SkillGraphInitHelpers;
    SkillNodes.Reset();

    {
        FSkillGraphNode Node;
        Node.SkillID = TEXT("N01");
        Node.DisplayName = NSLOCTEXT("SkillGraph", "N01_Name", "Pionier-Instinkt");
        Node.ClusterTag = TEXT("ClusterA_SurvivalCave");
        Node.VisualType = ESkillNodeVisualType::StandardNode;
        Node.MaxRank = 3;
        Node.MinCharacterLevel = 1;
        Node.DependencyMode = ESkillDependencyMode::AnyIncomingEdge;
        Node.Modifiers.Add(MakeMod(TEXT("Survival.WetnessRate"), -0.05f, true));
        Node.Modifiers.Add(MakeMod(TEXT("Player.CarryWeightBonus"), 10.0f, false));
        Node.UnlockedItems.Add(MakeItemGrant(TEXT("Recipe_PioneerPackAndMallet"), NSLOCTEXT("SkillGraph", "N01_Item1", "Wander-Rucksack & Holz-Schlegel"), 1, TEXT("CraftingRecipe")));
        SkillNodes.Add(Node.SkillID, Node);
    }
    {
        FSkillGraphNode Node;
        Node.SkillID = TEXT("N02");
        Node.DisplayName = NSLOCTEXT("SkillGraph", "N02_Name", "Wetterfeste Kleidung");
        Node.ClusterTag = TEXT("ClusterA_SurvivalCave");
        Node.VisualType = ESkillNodeVisualType::StandardNode;
        Node.MaxRank = 3;
        Node.MinCharacterLevel = 2;
        Node.DependencyMode = ESkillDependencyMode::AnyIncomingEdge;
        Node.Prerequisites.Add(MakeRule(TEXT("N01"), 1));
        Node.Modifiers.Add(MakeMod(TEXT("Survival.WetnessRate"), -0.10f, true));
        Node.Modifiers.Add(MakeMod(TEXT("Survival.ColdExposureRate"), -0.10f, true));
        Node.UnlockedItems.Add(MakeItemGrant(TEXT("Recipe_OiledCloakAndWaxCap"), NSLOCTEXT("SkillGraph", "N02_Item1", "Geölter Regenumhang & Wachs-Kappe"), 2, TEXT("CraftingRecipe")));
        SkillNodes.Add(Node.SkillID, Node);
    }
    {
        FSkillGraphNode Node;
        Node.SkillID = TEXT("N03");
        Node.DisplayName = NSLOCTEXT("SkillGraph", "N03_Name", "Fährtenleser im Nebel");
        Node.ClusterTag = TEXT("ClusterA_SurvivalCave");
        Node.VisualType = ESkillNodeVisualType::StandardNode;
        Node.MaxRank = 2;
        Node.MinCharacterLevel = 4;
        Node.DependencyMode = ESkillDependencyMode::AnyIncomingEdge;
        Node.Prerequisites.Add(MakeRule(TEXT("N02"), 1));
        Node.Modifiers.Add(MakeMod(TEXT("Player.SprintStaminaCost"), -0.10f, true));
        Node.Modifiers.Add(MakeMod(TEXT("Cave.FocusPingRange"), 0.20f, true));
        Node.UnlockedItems.Add(MakeItemGrant(TEXT("Recipe_SignalTorchAndMarkers"), NSLOCTEXT("SkillGraph", "N03_Item1", "Signal-Fackel & Markierungs-Pfähle"), 2, TEXT("CraftingRecipe")));
        SkillNodes.Add(Node.SkillID, Node);
    }
    {
        FSkillGraphNode Node;
        Node.SkillID = TEXT("N04");
        Node.DisplayName = NSLOCTEXT("SkillGraph", "N04_Name", "Höhlenläufer & Erzspürer");
        Node.ClusterTag = TEXT("ClusterA_SurvivalCave");
        Node.VisualType = ESkillNodeVisualType::StandardNode;
        Node.MaxRank = 3;
        Node.MinCharacterLevel = 7;
        Node.DependencyMode = ESkillDependencyMode::AllIncomingEdges;
        Node.Prerequisites.Add(MakeRule(TEXT("N02"), 2));
        Node.Prerequisites.Add(MakeRule(TEXT("N03"), 1));
        Node.Modifiers.Add(MakeMod(TEXT("Cave.OreYieldBonus"), 0.12f, true));
        Node.Modifiers.Add(MakeMod(TEXT("Survival.ColdExposureRate"), -0.08f, true));
        Node.bGrantsAbility = true;
        Node.UnlockedAbility = MakeAbility(TEXT("Ability_DeepSightPulse"), NSLOCTEXT("SkillGraph", "N04_Ability", "Tiefensicht-Ruf"), 2, 35.0f, 12.0f);
        Node.UnlockedItems.Add(MakeItemGrant(TEXT("Recipe_MinerLanternAndPickaxe"), NSLOCTEXT("SkillGraph", "N04_Item1", "Gruben-Laterne & Verstärkte Spitzhacke"), 2, TEXT("CraftingRecipe")));
        SkillNodes.Add(Node.SkillID, Node);
    }
    {
        FSkillGraphNode Node;
        Node.SkillID = TEXT("A01");
        Node.DisplayName = NSLOCTEXT("SkillGraph", "A01_Name", "Herr der Elemente & Tiefe");
        Node.ClusterTag = TEXT("ClusterA_SurvivalCave");
        Node.VisualType = ESkillNodeVisualType::AnchorMilestone;
        Node.MaxRank = 1;
        Node.MinCharacterLevel = 12;
        Node.DependencyMode = ESkillDependencyMode::AllIncomingEdges;
        Node.Prerequisites.Add(MakeRule(TEXT("N04"), 3));
        Node.Modifiers.Add(MakeMod(TEXT("Survival.WetnessRate"), -0.20f, true));
        Node.Modifiers.Add(MakeMod(TEXT("Survival.ColdExposureRate"), -0.20f, true));
        Node.Modifiers.Add(MakeMod(TEXT("Cave.ShelterBonus"), 0.25f, true));
        Node.bGrantsAbility = true;
        Node.UnlockedAbility = MakeAbility(TEXT("Ability_StormDefiance"), NSLOCTEXT("SkillGraph", "A01_Ability", "Sturm-Trotz"), 1, 75.0f, 20.0f);
        Node.UnlockedItems.Add(MakeItemGrant(TEXT("Blueprint_DeepShelter_3Stage"), NSLOCTEXT("SkillGraph", "A01_Item1", "Blaupause: Tiefen-Schutzunterstand (3-Stufen)"), 1, TEXT("BuildBlueprint")));
        Node.UnlockedItems.Add(MakeItemGrant(TEXT("Vendor_StormResinOil"), NSLOCTEXT("SkillGraph", "A01_Item2", "Händlerware: Sturm-Harz-Öl"), 1, TEXT("VendorStock")));
        SkillNodes.Add(Node.SkillID, Node);
    }
    {
        FSkillGraphNode Node;
        Node.SkillID = TEXT("N05");
        Node.DisplayName = NSLOCTEXT("SkillGraph", "N05_Name", "Zupackender Bauherr");
        Node.ClusterTag = TEXT("ClusterB_ThreeStageBuild");
        Node.VisualType = ESkillNodeVisualType::StandardNode;
        Node.MaxRank = 3;
        Node.MinCharacterLevel = 1;
        Node.DependencyMode = ESkillDependencyMode::AnyIncomingEdge;
        Node.Prerequisites.Add(MakeRule(TEXT("N01"), 1));
        Node.Modifiers.Add(MakeMod(TEXT("Build.StageSpeed"), 0.12f, true));
        Node.Modifiers.Add(MakeMod(TEXT("Player.BuildStaminaCost"), -0.10f, true));
        Node.UnlockedItems.Add(MakeItemGrant(TEXT("Recipe_HardenedCarpenterHammer"), NSLOCTEXT("SkillGraph", "N05_Item1", "Zimmermanns-Hammer (gehärtet)"), 2, TEXT("CraftingRecipe")));
        SkillNodes.Add(Node.SkillID, Node);
    }
    {
        FSkillGraphNode Node;
        Node.SkillID = TEXT("N06");
        Node.DisplayName = NSLOCTEXT("SkillGraph", "N06_Name", "Hang- & Fundament-Statik");
        Node.ClusterTag = TEXT("ClusterB_ThreeStageBuild");
        Node.VisualType = ESkillNodeVisualType::StandardNode;
        Node.MaxRank = 2;
        Node.MinCharacterLevel = 4;
        Node.DependencyMode = ESkillDependencyMode::AnyIncomingEdge;
        Node.Prerequisites.Add(MakeRule(TEXT("N05"), 1));
        Node.Modifiers.Add(MakeMod(TEXT("Build.StabilityBonus"), 0.15f, true));
        Node.Modifiers.Add(MakeMod(TEXT("Build.SlopeTolerance"), 0.20f, true));
        Node.bGrantsAbility = true;
        Node.UnlockedAbility = MakeAbility(TEXT("Ability_EmergencyBrace"), NSLOCTEXT("SkillGraph", "N06_Ability", "Not-Abstützung"), 2, 45.0f, 15.0f);
        Node.UnlockedItems.Add(MakeItemGrant(TEXT("Blueprint_StiltFoundation_3Stage"), NSLOCTEXT("SkillGraph", "N06_Item1", "Blaupause: Stelzen- & Hangfundament (Stufe 1–3)"), 2, TEXT("BuildBlueprint")));
        SkillNodes.Add(Node.SkillID, Node);
    }
    {
        FSkillGraphNode Node;
        Node.SkillID = TEXT("N07");
        Node.DisplayName = NSLOCTEXT("SkillGraph", "N07_Name", "Begehbarer Innenausbau");
        Node.ClusterTag = TEXT("ClusterB_ThreeStageBuild");
        Node.VisualType = ESkillNodeVisualType::StandardNode;
        Node.MaxRank = 3;
        Node.MinCharacterLevel = 7;
        Node.DependencyMode = ESkillDependencyMode::AllIncomingEdges;
        Node.Prerequisites.Add(MakeRule(TEXT("N05"), 2));
        Node.Prerequisites.Add(MakeRule(TEXT("N06"), 1));
        Node.Modifiers.Add(MakeMod(TEXT("Build.InteriorMaxHP"), 0.15f, true));
        Node.Modifiers.Add(MakeMod(TEXT("Build.ShelterRadius"), 0.10f, true));
        Node.UnlockedItems.Add(MakeItemGrant(TEXT("Recipe_PaddedBed"), NSLOCTEXT("SkillGraph", "N07_Item1", "Gepolstertes Bett (ABedActor)"), 2, TEXT("CraftingRecipe")));
        Node.UnlockedItems.Add(MakeItemGrant(TEXT("Blueprint_StorageWorkRoom"), NSLOCTEXT("SkillGraph", "N07_Item2", "Blaupause: Vorrats-/Werkraum"), 2, TEXT("BuildBlueprint")));
        SkillNodes.Add(Node.SkillID, Node);
    }
    {
        FSkillGraphNode Node;
        Node.SkillID = TEXT("N08");
        Node.DisplayName = NSLOCTEXT("SkillGraph", "N08_Name", "Sauberer Rückbau");
        Node.ClusterTag = TEXT("ClusterB_ThreeStageBuild");
        Node.VisualType = ESkillNodeVisualType::StandardNode;
        Node.MaxRank = 2;
        Node.MinCharacterLevel = 9;
        Node.DependencyMode = ESkillDependencyMode::AnyIncomingEdge;
        Node.Prerequisites.Add(MakeRule(TEXT("N07"), 1));
        Node.Modifiers.Add(MakeMod(TEXT("Build.DeconstructRefund"), 0.15f, true));
        Node.Modifiers.Add(MakeMod(TEXT("Build.RepairMaterialCost"), -0.10f, true));
        Node.UnlockedItems.Add(MakeItemGrant(TEXT("Recipe_CrowbarAndTemplate"), NSLOCTEXT("SkillGraph", "N08_Item1", "Brecheisen & Rückbau-Schablone"), 2, TEXT("CraftingRecipe")));
        SkillNodes.Add(Node.SkillID, Node);
    }
    {
        FSkillGraphNode Node;
        Node.SkillID = TEXT("A02");
        Node.DisplayName = NSLOCTEXT("SkillGraph", "A02_Name", "Meister der Drei Stufen");
        Node.ClusterTag = TEXT("ClusterB_ThreeStageBuild");
        Node.VisualType = ESkillNodeVisualType::AnchorMilestone;
        Node.MaxRank = 1;
        Node.MinCharacterLevel = 14;
        Node.DependencyMode = ESkillDependencyMode::AllIncomingEdges;
        Node.Prerequisites.Add(MakeRule(TEXT("N07"), 3));
        Node.Modifiers.Add(MakeMod(TEXT("Build.CustomBlueprintLimit"), 2.0f, false));
        Node.Modifiers.Add(MakeMod(TEXT("Build.Stage2StationBonus"), 0.20f, true));
        Node.Modifiers.Add(MakeMod(TEXT("Build.StabilityBonus"), 0.20f, true));
        Node.bGrantsAbility = true;
        Node.UnlockedAbility = MakeAbility(TEXT("Ability_BlueprintToppingOut"), NSLOCTEXT("SkillGraph", "A02_Ability", "Blaupausen-Richtfest"), 1, 80.0f, 18.0f);
        Node.UnlockedItems.Add(MakeItemGrant(TEXT("Blueprint_GrandHallWatch_3Stage"), NSLOCTEXT("SkillGraph", "A02_Item1", "Blaupause: Große Hallen- & Wach-Struktur (3-Stufen)"), 1, TEXT("BuildBlueprint")));
        Node.UnlockedItems.Add(MakeItemGrant(TEXT("Vendor_GuildTimberWork"), NSLOCTEXT("SkillGraph", "A02_Item2", "Händlerware: Gilden-Balkenwerk"), 1, TEXT("VendorStock")));
        SkillNodes.Add(Node.SkillID, Node);
    }
    {
        FSkillGraphNode Node;
        Node.SkillID = TEXT("N09");
        Node.DisplayName = NSLOCTEXT("SkillGraph", "N09_Name", "Schwerer Träger");
        Node.ClusterTag = TEXT("ClusterC_StationLogistics");
        Node.VisualType = ESkillNodeVisualType::StandardNode;
        Node.MaxRank = 3;
        Node.MinCharacterLevel = 2;
        Node.DependencyMode = ESkillDependencyMode::AnyIncomingEdge;
        Node.Prerequisites.Add(MakeRule(TEXT("N01"), 1));
        Node.Prerequisites.Add(MakeRule(TEXT("N05"), 1));
        Node.Modifiers.Add(MakeMod(TEXT("Player.CarryWeightBonus"), 20.0f, false));
        Node.Modifiers.Add(MakeMod(TEXT("Player.HaulStaminaCost"), -0.10f, true));
        Node.UnlockedItems.Add(MakeItemGrant(TEXT("Recipe_HandcartHarness"), NSLOCTEXT("SkillGraph", "N09_Item1", "Handkarren-Gurt & Schwerlast-Tragegestell"), 2, TEXT("CraftingRecipe")));
        SkillNodes.Add(Node.SkillID, Node);
    }
    {
        FSkillGraphNode Node;
        Node.SkillID = TEXT("N10");
        Node.DisplayName = NSLOCTEXT("SkillGraph", "N10_Name", "Präzisions-Werkstatt");
        Node.ClusterTag = TEXT("ClusterC_StationLogistics");
        Node.VisualType = ESkillNodeVisualType::StandardNode;
        Node.MaxRank = 3;
        Node.MinCharacterLevel = 6;
        Node.DependencyMode = ESkillDependencyMode::AnyIncomingEdge;
        Node.Prerequisites.Add(MakeRule(TEXT("N09"), 1));
        Node.Modifiers.Add(MakeMod(TEXT("Station.CraftSpeed"), 0.14f, true));
        Node.Modifiers.Add(MakeMod(TEXT("Station.ToolDurabilityMult"), 0.12f, true));
        Node.bGrantsAbility = true;
        Node.UnlockedAbility = MakeAbility(TEXT("Ability_PrecisionCadence"), NSLOCTEXT("SkillGraph", "N10_Ability", "Präzisions-Takt"), 2, 50.0f, 10.0f);
        Node.UnlockedItems.Add(MakeItemGrant(TEXT("Recipe_FineToolSet"), NSLOCTEXT("SkillGraph", "N10_Item1", "Feinwerkzeug-Satz"), 2, TEXT("CraftingRecipe")));
        SkillNodes.Add(Node.SkillID, Node);
    }
    {
        FSkillGraphNode Node;
        Node.SkillID = TEXT("N11");
        Node.DisplayName = NSLOCTEXT("SkillGraph", "N11_Name", "Lager-Ordnung");
        Node.ClusterTag = TEXT("ClusterC_StationLogistics");
        Node.VisualType = ESkillNodeVisualType::StandardNode;
        Node.MaxRank = 2;
        Node.MinCharacterLevel = 8;
        Node.DependencyMode = ESkillDependencyMode::AnyIncomingEdge;
        Node.Prerequisites.Add(MakeRule(TEXT("N09"), 1));
        Node.Prerequisites.Add(MakeRule(TEXT("N10"), 1));
        Node.Modifiers.Add(MakeMod(TEXT("Storage.CapacityBonus"), 0.20f, true));
        Node.Modifiers.Add(MakeMod(TEXT("Storage.PickupSyncMult"), 0.15f, true));
        Node.UnlockedItems.Add(MakeItemGrant(TEXT("Blueprint_ReinforcedStorageChest"), NSLOCTEXT("SkillGraph", "N11_Item1", "Blaupause: Verstärkte Lagertruhe / Vorratsregal"), 2, TEXT("BuildBlueprint")));
        SkillNodes.Add(Node.SkillID, Node);
    }
    {
        FSkillGraphNode Node;
        Node.SkillID = TEXT("N12");
        Node.DisplayName = NSLOCTEXT("SkillGraph", "N12_Name", "Rohstoff-Veredelung");
        Node.ClusterTag = TEXT("ClusterC_StationLogistics");
        Node.VisualType = ESkillNodeVisualType::StandardNode;
        Node.MaxRank = 2;
        Node.MinCharacterLevel = 10;
        Node.DependencyMode = ESkillDependencyMode::AllIncomingEdges;
        Node.Prerequisites.Add(MakeRule(TEXT("N04"), 1));
        Node.Prerequisites.Add(MakeRule(TEXT("N10"), 1));
        Node.Modifiers.Add(MakeMod(TEXT("Station.RefineYieldBonus"), 0.15f, true));
        Node.Modifiers.Add(MakeMod(TEXT("Cave.OreYieldBonus"), 0.10f, true));
        Node.UnlockedItems.Add(MakeItemGrant(TEXT("Recipe_SmeltStampInsert"), NSLOCTEXT("SkillGraph", "N12_Item1", "Schmelz-/Poch-Einsatz für Werkstation"), 1, TEXT("CraftingRecipe")));
        SkillNodes.Add(Node.SkillID, Node);
    }
    {
        FSkillGraphNode Node;
        Node.SkillID = TEXT("A03");
        Node.DisplayName = NSLOCTEXT("SkillGraph", "A03_Name", "Meister der Werkstätten & Lager");
        Node.ClusterTag = TEXT("ClusterC_StationLogistics");
        Node.VisualType = ESkillNodeVisualType::AnchorMilestone;
        Node.MaxRank = 1;
        Node.MinCharacterLevel = 16;
        Node.DependencyMode = ESkillDependencyMode::AllIncomingEdges;
        Node.Prerequisites.Add(MakeRule(TEXT("N10"), 3));
        Node.Prerequisites.Add(MakeRule(TEXT("N11"), 1));
        Node.Modifiers.Add(MakeMod(TEXT("Storage.DirectAccessEnabled"), 1.0f, false));
        Node.Modifiers.Add(MakeMod(TEXT("Station.CraftSpeed"), 0.18f, true));
        Node.Modifiers.Add(MakeMod(TEXT("Storage.CapacityBonus"), 0.25f, true));
        Node.bGrantsAbility = true;
        Node.UnlockedAbility = MakeAbility(TEXT("Ability_SupplyCommand"), NSLOCTEXT("SkillGraph", "A03_Ability", "Nachschub-Befehl"), 1, 60.0f, 15.0f);
        Node.UnlockedItems.Add(MakeItemGrant(TEXT("Blueprint_CountingWarehouse_3Stage"), NSLOCTEXT("SkillGraph", "A03_Item1", "Blaupause: Kontor-Lagerhaus (3-Stufen)"), 1, TEXT("BuildBlueprint")));
        Node.UnlockedItems.Add(MakeItemGrant(TEXT("Vendor_PrecisionCastingMolds"), NSLOCTEXT("SkillGraph", "A03_Item2", "Händlerware: Präzisions-Gussformen"), 1, TEXT("VendorStock")));
        SkillNodes.Add(Node.SkillID, Node);
    }
    {
        FSkillGraphNode Node;
        Node.SkillID = TEXT("N13");
        Node.DisplayName = NSLOCTEXT("SkillGraph", "N13_Name", "Vertrauenswürdiger Reisender");
        Node.ClusterTag = TEXT("ClusterD_HubsSettlers");
        Node.VisualType = ESkillNodeVisualType::StandardNode;
        Node.MaxRank = 3;
        Node.MinCharacterLevel = 3;
        Node.DependencyMode = ESkillDependencyMode::AnyIncomingEdge;
        Node.Prerequisites.Add(MakeRule(TEXT("N01"), 1));
        Node.Prerequisites.Add(MakeRule(TEXT("N09"), 1));
        Node.Modifiers.Add(MakeMod(TEXT("Hub.ReputationGainMult"), 0.12f, true));
        Node.Modifiers.Add(MakeMod(TEXT("Hub.VendorPriceDiscount"), 0.04f, true));
        Node.UnlockedItems.Add(MakeItemGrant(TEXT("Vendor_TravelRationsAndSeals"), NSLOCTEXT("SkillGraph", "N13_Item1", "Händlerware: Reise-Proviant & Siegel-Marken"), 2, TEXT("VendorStock")));
        SkillNodes.Add(Node.SkillID, Node);
    }
    {
        FSkillGraphNode Node;
        Node.SkillID = TEXT("N14");
        Node.DisplayName = NSLOCTEXT("SkillGraph", "N14_Name", "Vorbild für die Gemeinschaft");
        Node.ClusterTag = TEXT("ClusterD_HubsSettlers");
        Node.VisualType = ESkillNodeVisualType::StandardNode;
        Node.MaxRank = 3;
        Node.MinCharacterLevel = 6;
        Node.DependencyMode = ESkillDependencyMode::AnyIncomingEdge;
        Node.Prerequisites.Add(MakeRule(TEXT("N13"), 1));
        Node.Modifiers.Add(MakeMod(TEXT("Settler.WorkSpeedMultiplier"), 0.12f, true));
        Node.Modifiers.Add(MakeMod(TEXT("Settler.MoraleBonus"), 8.0f, false));
        Node.bGrantsAbility = true;
        Node.UnlockedAbility = MakeAbility(TEXT("Ability_WorkAppeal"), NSLOCTEXT("SkillGraph", "N14_Ability", "Arbeits-Appell"), 2, 70.0f, 15.0f);
        Node.UnlockedItems.Add(MakeItemGrant(TEXT("Recipe_FeastTableAndBanner"), NSLOCTEXT("SkillGraph", "N14_Item1", "Festtafel-Set / Gemeinschafts-Banner"), 2, TEXT("CraftingRecipe")));
        SkillNodes.Add(Node.SkillID, Node);
    }
    {
        FSkillGraphNode Node;
        Node.SkillID = TEXT("N15");
        Node.DisplayName = NSLOCTEXT("SkillGraph", "N15_Name", "Versorgungs-Planer");
        Node.ClusterTag = TEXT("ClusterD_HubsSettlers");
        Node.VisualType = ESkillNodeVisualType::StandardNode;
        Node.MaxRank = 2;
        Node.MinCharacterLevel = 9;
        Node.DependencyMode = ESkillDependencyMode::AnyIncomingEdge;
        Node.Prerequisites.Add(MakeRule(TEXT("N13"), 1));
        Node.Prerequisites.Add(MakeRule(TEXT("N14"), 1));
        Node.Modifiers.Add(MakeMod(TEXT("Settler.HungerFatigueRate"), -0.12f, true));
        Node.Modifiers.Add(MakeMod(TEXT("Settler.BedRecoveryMult"), 0.15f, true));
        Node.UnlockedItems.Add(MakeItemGrant(TEXT("Blueprint_SettlementPantrySickbay"), NSLOCTEXT("SkillGraph", "N15_Item1", "Blaupause: Siedlungs-Speisekammer / Ruhekoje"), 2, TEXT("BuildBlueprint")));
        SkillNodes.Add(Node.SkillID, Node);
    }
    {
        FSkillGraphNode Node;
        Node.SkillID = TEXT("N16");
        Node.DisplayName = NSLOCTEXT("SkillGraph", "N16_Name", "Gilden-Kontakte");
        Node.ClusterTag = TEXT("ClusterD_HubsSettlers");
        Node.VisualType = ESkillNodeVisualType::StandardNode;
        Node.MaxRank = 2;
        Node.MinCharacterLevel = 11;
        Node.DependencyMode = ESkillDependencyMode::AllIncomingEdges;
        Node.Prerequisites.Add(MakeRule(TEXT("N07"), 1));
        Node.Prerequisites.Add(MakeRule(TEXT("N14"), 1));
        Node.Modifiers.Add(MakeMod(TEXT("Hub.RecruitCostDiscount"), 0.10f, true));
        Node.Modifiers.Add(MakeMod(TEXT("Hub.BlueprintCostDiscount"), 0.10f, true));
        Node.UnlockedItems.Add(MakeItemGrant(TEXT("Vendor_GuildLettersAndSpecialBlueprints"), NSLOCTEXT("SkillGraph", "N16_Item1", "Händlerware: Gilden-Empfehlungsschreiben & Sonder-Blaupausen"), 1, TEXT("VendorStock")));
        SkillNodes.Add(Node.SkillID, Node);
    }
    {
        FSkillGraphNode Node;
        Node.SkillID = TEXT("A04");
        Node.DisplayName = NSLOCTEXT("SkillGraph", "A04_Name", "Stimme der Acht Orte");
        Node.ClusterTag = TEXT("ClusterD_HubsSettlers");
        Node.VisualType = ESkillNodeVisualType::AnchorMilestone;
        Node.MaxRank = 1;
        Node.MinCharacterLevel = 18;
        Node.DependencyMode = ESkillDependencyMode::AllIncomingEdges;
        Node.Prerequisites.Add(MakeRule(TEXT("N14"), 3));
        Node.Prerequisites.Add(MakeRule(TEXT("N15"), 1));
        Node.Modifiers.Add(MakeMod(TEXT("Settler.WeatherMoraleResist"), 0.25f, true));
        Node.Modifiers.Add(MakeMod(TEXT("Hub.ReputationGainMult"), 0.20f, true));
        Node.Modifiers.Add(MakeMod(TEXT("Hub.RareAccessUnlocked"), 1.0f, false));
        Node.bGrantsAbility = true;
        Node.UnlockedAbility = MakeAbility(TEXT("Ability_GuildPresence"), NSLOCTEXT("SkillGraph", "A04_Ability", "Geleit- & Gilden-Präsenz"), 1, 90.0f, 15.0f);
        Node.UnlockedItems.Add(MakeItemGrant(TEXT("Blueprint_AssemblyTradeHall_3Stage"), NSLOCTEXT("SkillGraph", "A04_Item1", "Blaupause: Versammlungs-/Handelshalle (3-Stufen)"), 1, TEXT("BuildBlueprint")));
        Node.UnlockedItems.Add(MakeItemGrant(TEXT("Vendor_RealmBlueprintsAndGear"), NSLOCTEXT("SkillGraph", "A04_Item2", "Händlerware: Reichs-Blaupausen & Spezial-Ausrüstung"), 1, TEXT("VendorStock")));
        SkillNodes.Add(Node.SkillID, Node);
    }
}

int32 USkillGraphSubsystem::GetNodeRank(const FPlayerSkillTreeState& State, FName SkillID) const {
    if (const int32* Found = State.UnlockedSkillRanks.Find(SkillID)) {
        return FMath::Max(0, *Found);
    }
    return 0;
}

int32 USkillGraphSubsystem::GetMaxTotalSkillPointsForLevel(int32 CharacterLevel) const {
    const int32 ClampedLevel = FMath::Clamp(CharacterLevel, 1, MaxCharacterLevelCap);
    return FMath::Clamp(ClampedLevel - 1, 0, MaxTotalSkillPointsCap);
}

bool USkillGraphSubsystem::CanUnlockNode(const FPlayerSkillTreeState& State, FName SkillID, FText& OutFailureReason) const {
    const FSkillGraphNode* Node = SkillNodes.Find(SkillID);
    if (!Node) {
        OutFailureReason = NSLOCTEXT("SkillGraph", "UnknownNode", "Unbekannter Skill-Knoten.");
        return false;
    }

    const int32 CurrentRank = GetNodeRank(State, SkillID);
    if (CurrentRank >= Node->MaxRank) {
        OutFailureReason = NSLOCTEXT("SkillGraph", "MaxRankReached", "Maximaler Rang erreicht.");
        return false;
    }

    if (State.CharacterLevel < Node->MinCharacterLevel) {
        OutFailureReason = NSLOCTEXT("SkillGraph", "LevelTooLow", "Charakter-Level zu niedrig.");
        return false;
    }

    if (State.AvailableSkillPoints < FMath::Max(1, Node->PointCostPerRank)) {
        OutFailureReason = NSLOCTEXT("SkillGraph", "NotEnoughPoints", "Nicht genügend Skillpunkte.");
        return false;
    }

    if (Node->Prerequisites.Num() > 0) {
        if (Node->DependencyMode == ESkillDependencyMode::AllIncomingEdges) {
            for (const FSkillDependencyRule& Rule : Node->Prerequisites) {
                if (GetNodeRank(State, Rule.RequiredSkillID) < Rule.MinRequiredRank) {
                    OutFailureReason = NSLOCTEXT("SkillGraph", "PrereqNotMet", "Voraussetzungen nicht erfüllt.");
                    return false;
                }
            }
        }
        else {
            bool bAnyMet = false;
            for (const FSkillDependencyRule& Rule : Node->Prerequisites) {
                if (GetNodeRank(State, Rule.RequiredSkillID) >= Rule.MinRequiredRank) {
                    bAnyMet = true;
                    break;
                }
            }
            if (!bAnyMet) {
                OutFailureReason = NSLOCTEXT("SkillGraph", "AnyPrereqNotMet", "Keiner der erforderlichen Vorgänger ist erfüllt.");
                return false;
            }
        }
    }

    return true;
}

bool USkillGraphSubsystem::IsNodeLockedByDependency(const FPlayerSkillTreeState& State, FName SkillID, FName& OutBlockingSkillID) const {
    OutBlockingSkillID = NAME_None;
    const int32 CurrentRank = GetNodeRank(State, SkillID);
    if (CurrentRank <= 0) {
        return false;
    }

    const int32 RankAfterStep = CurrentRank - 1;
    for (const TPair<FName, FSkillGraphNode>& Pair : SkillNodes) {
        const FName& OtherID = Pair.Key;
        const FSkillGraphNode& OtherNode = Pair.Value;
        if (GetNodeRank(State, OtherID) <= 0) {
            continue;
        }

        for (const FSkillDependencyRule& Rule : OtherNode.Prerequisites) {
            if (Rule.RequiredSkillID == SkillID && RankAfterStep < Rule.MinRequiredRank) {
                OutBlockingSkillID = OtherID;
                return true;
            }
        }
    }

    return false;
}

bool USkillGraphSubsystem::CanRespecAtHub(EHubSettlementType HubType, ERespecScope Scope) const {
    if (Scope == ERespecScope::FullTreeRespec) {
        return HubType == EHubSettlementType::MinorCity || HubType == EHubSettlementType::Capital;
    }
    if (Scope == ERespecScope::PartialCapitalRespec) {
        return HubType == EHubSettlementType::Capital;
    }
    return false;
}

float USkillGraphSubsystem::GetAggregatedModifier(const FPlayerSkillTreeState& State, FName ModifierKey, float BaseValue) const {
    float AdditiveBonus = 0.0f;
    float MultiplicativeSum = 0.0f;

    for (const TPair<FName, int32>& Entry : State.UnlockedSkillRanks) {
        const int32 Rank = Entry.Value;
        if (Rank <= 0) {
            continue;
        }
        const FSkillGraphNode* Node = SkillNodes.Find(Entry.Key);
        if (!Node) {
            continue;
        }
        for (const FSkillModifierHook& Hook : Node->Modifiers) {
            if (Hook.ModifierKey != ModifierKey) {
                continue;
            }
            const float Total = Hook.ValuePerRank * static_cast<float>(Rank);
            if (Hook.bIsMultiplicative) {
                MultiplicativeSum += Total;
            }
            else {
                AdditiveBonus += Total;
            }
        }
    }

    return (BaseValue * FMath::Max(0.0f, 1.0f + MultiplicativeSum)) + AdditiveBonus;
}

bool USkillGraphSubsystem::MatchesAnchorPair(FName FirstAnchor, FName SecondAnchor, FName ExpectedA, FName ExpectedB) const {
    return (FirstAnchor == ExpectedA && SecondAnchor == ExpectedB) ||
           (FirstAnchor == ExpectedB && SecondAnchor == ExpectedA);
}

FAnchorResonanceDefinition USkillGraphSubsystem::BuildDefaultResonanceDefinition(EAnchorResonanceType Type, FName AnchorA, FName AnchorB) const {
    FAnchorResonanceDefinition Def;
    Def.ResonanceType = Type;
    Def.AnchorA = AnchorA;
    Def.AnchorB = AnchorB;

    switch (Type) {
    case EAnchorResonanceType::OutpostPioneer:
        Def.DisplayName = NSLOCTEXT("SkillGraph", "Res_OutpostPioneer", "Außenposten-Pionier");
        Def.ResonanceModifiers.Add({ TEXT("Build.StageSpeed"), 0.15f, true });
        Def.ResonanceModifiers.Add({ TEXT("Survival.WetnessRate"), -0.15f, true });
        break;
    case EAnchorResonanceType::DeepSmith:
        Def.DisplayName = NSLOCTEXT("SkillGraph", "Res_DeepSmith", "Tiefenschmied");
        Def.ResonanceModifiers.Add({ TEXT("Station.CraftSpeed"), 0.15f, true });
        Def.ResonanceModifiers.Add({ TEXT("Cave.OreYieldBonus"), 0.20f, true });
        break;
    case EAnchorResonanceType::MasterManufactory:
        Def.DisplayName = NSLOCTEXT("SkillGraph", "Res_MasterManufactory", "Meister-Manufaktur");
        Def.ResonanceModifiers.Add({ TEXT("Station.CraftSpeed"), 0.20f, true });
        Def.ResonanceModifiers.Add({ TEXT("Storage.CapacityBonus"), 0.25f, true });
        break;
    case EAnchorResonanceType::RealmArchitect:
        Def.DisplayName = NSLOCTEXT("SkillGraph", "Res_RealmArchitect", "Architekt des Reiches");
        Def.ResonanceModifiers.Add({ TEXT("Build.StabilityBonus"), 0.20f, true });
        Def.ResonanceModifiers.Add({ TEXT("Hub.ReputationGainMult"), 0.15f, true });
        break;
    case EAnchorResonanceType::TradeEmporium:
        Def.DisplayName = NSLOCTEXT("SkillGraph", "Res_TradeEmporium", "Handels-Kontor");
        Def.ResonanceModifiers.Add({ TEXT("Storage.HaulerSpeed"), 0.20f, true });
        Def.ResonanceModifiers.Add({ TEXT("Hub.ReputationGainMult"), 0.15f, true });
        break;
    case EAnchorResonanceType::CrownPathfinder:
        Def.DisplayName = NSLOCTEXT("SkillGraph", "Res_CrownPathfinder", "Pfadfinder der Krone");
        Def.ResonanceModifiers.Add({ TEXT("Survival.ColdExposureRate"), -0.15f, true });
        Def.ResonanceModifiers.Add({ TEXT("Hub.ReputationGainMult"), 0.20f, true });
        break;
    default:
        break;
    }

    return Def;
}

TArray<FAnchorResonanceDefinition> USkillGraphSubsystem::EvaluateActiveResonances(const FPlayerSkillTreeState& State) const {
    TArray<FAnchorResonanceDefinition> ActiveResonances;
    const TArray<FName> AnchorOrder = { TEXT("A01"), TEXT("A02"), TEXT("A03"), TEXT("A04") };
    TArray<FName> ActiveAnchors;

    for (const FName& AnchorID : AnchorOrder) {
        if (GetNodeRank(State, AnchorID) >= 1) {
            ActiveAnchors.Add(AnchorID);
        }
    }

    if (ActiveAnchors.Num() < 2) {
        return ActiveResonances;
    }

    for (int32 i = 0; i < ActiveAnchors.Num(); ++i) {
        for (int32 j = i + 1; j < ActiveAnchors.Num(); ++j) {
            const FName FirstID = ActiveAnchors[i];
            const FName SecondID = ActiveAnchors[j];
            EAnchorResonanceType MatchedType = EAnchorResonanceType::None;

            if (MatchesAnchorPair(FirstID, SecondID, TEXT("A01"), TEXT("A02"))) {
                MatchedType = EAnchorResonanceType::OutpostPioneer;
            }
            else if (MatchesAnchorPair(FirstID, SecondID, TEXT("A01"), TEXT("A03"))) {
                MatchedType = EAnchorResonanceType::DeepSmith;
            }
            else if (MatchesAnchorPair(FirstID, SecondID, TEXT("A02"), TEXT("A03"))) {
                MatchedType = EAnchorResonanceType::MasterManufactory;
            }
            else if (MatchesAnchorPair(FirstID, SecondID, TEXT("A02"), TEXT("A04"))) {
                MatchedType = EAnchorResonanceType::RealmArchitect;
            }
            else if (MatchesAnchorPair(FirstID, SecondID, TEXT("A03"), TEXT("A04"))) {
                MatchedType = EAnchorResonanceType::TradeEmporium;
            }
            else if (MatchesAnchorPair(FirstID, SecondID, TEXT("A01"), TEXT("A04"))) {
                MatchedType = EAnchorResonanceType::CrownPathfinder;
            }

            if (MatchedType != EAnchorResonanceType::None) {
                ActiveResonances.Add(BuildDefaultResonanceDefinition(MatchedType, FirstID, SecondID));
            }
        }
    }

    return ActiveResonances;
}

float USkillGraphSubsystem::GetAggregatedModifierWithResonances(const FPlayerSkillTreeState& State, FName ModifierKey, float BaseValue) const {
    float AdditiveBonus = 0.0f;
    float MultiplicativeSum = 0.0f;

    for (const TPair<FName, int32>& RankEntry : State.UnlockedSkillRanks) {
        const int32 Rank = RankEntry.Value;
        if (Rank <= 0) {
            continue;
        }
        const FSkillGraphNode* Node = SkillNodes.Find(RankEntry.Key);
        if (!Node) {
            continue;
        }
        for (const FSkillModifierHook& Hook : Node->Modifiers) {
            if (Hook.ModifierKey != ModifierKey) {
                continue;
            }
            const float TotalHookValue = Hook.ValuePerRank * static_cast<float>(Rank);
            if (Hook.bIsMultiplicative) {
                MultiplicativeSum += TotalHookValue;
            }
            else {
                AdditiveBonus += TotalHookValue;
            }
        }
    }

    const TArray<FAnchorResonanceDefinition> ActiveResonances = EvaluateActiveResonances(State);
    for (const FAnchorResonanceDefinition& Resonance : ActiveResonances) {
        for (const FSkillModifierHook& Hook : Resonance.ResonanceModifiers) {
            if (Hook.ModifierKey != ModifierKey) {
                continue;
            }
            if (Hook.bIsMultiplicative) {
                MultiplicativeSum += Hook.ValuePerRank;
            }
            else {
                AdditiveBonus += Hook.ValuePerRank;
            }
        }
    }

    return (BaseValue * FMath::Max(0.0f, 1.0f + MultiplicativeSum)) + AdditiveBonus;
}

TArray<FSkillUnlockedItemGrant> USkillGraphSubsystem::GetUnlockedItemsForState(const FPlayerSkillTreeState& State) const {
    TArray<FSkillUnlockedItemGrant> Result;
    for (const TPair<FName, FSkillGraphNode>& Pair : SkillNodes) {
        const int32 CurrentRank = GetNodeRank(State, Pair.Key);
        for (const FSkillUnlockedItemGrant& Grant : Pair.Value.UnlockedItems) {
            if (!Grant.ItemOrRecipeID.IsNone() && CurrentRank >= FMath::Max(1, Grant.RequiredRankToUnlock)) {
                Result.Add(Grant);
            }
        }
    }
    return Result;
}

EReputationTier USkillGraphSubsystem::GetReputationTier(int32 CurrentReputation) const {
    const int32 ClampedRep = FMath::Clamp(CurrentReputation, MinLocalReputation, MaxLocalReputation);
    if (ClampedRep >= 700) {
        return EReputationTier::Tier3_Honored;
    }
    if (ClampedRep >= 300) {
        return EReputationTier::Tier2_Trusted;
    }
    return EReputationTier::Tier1_Acquaintance;
}

int32 USkillGraphSubsystem::ApplyReputationGain(
    const FPlayerSkillTreeState& State,
    UPARAM(ref) int32& InOutReputation,
    int32 BaseReputationGain,
    EHubSettlementType HubType,
    bool bIsCaveOrRouteMilestone
) const {
    InOutReputation = FMath::Clamp(InOutReputation, MinLocalReputation, MaxLocalReputation);
    if (BaseReputationGain <= 0) {
        return 0;
    }

    const int32 N13Rank = GetNodeRank(State, TEXT("N13"));
    float Multiplier = 1.0f + (0.12f * static_cast<float>(FMath::Max(0, N13Rank)));

    const int32 A04Rank = GetNodeRank(State, TEXT("A04"));
    if (A04Rank > 0) {
        if (HubType == EHubSettlementType::MinorCity || HubType == EHubSettlementType::Capital) {
            Multiplier += 0.20f;
        }
        if (bIsCaveOrRouteMilestone) {
            Multiplier += 0.15f;
        }
    }

    const int32 FinalGain = FMath::Max(1, FMath::RoundToInt(static_cast<float>(BaseReputationGain) * Multiplier));
    const int32 PreviousRep = InOutReputation;
    InOutReputation = FMath::Clamp(InOutReputation + FinalGain, MinLocalReputation, MaxLocalReputation);
    return InOutReputation - PreviousRep;
}

int32 USkillGraphSubsystem::GetMaxVendorTierForHubType(EHubSettlementType HubType) const {
    switch (HubType) {
    case EHubSettlementType::MinorSettlement:
        return 1;
    case EHubSettlementType::MinorCity:
        return 2;
    case EHubSettlementType::Capital:
        return 3;
    default:
        return 1;
    }
}

bool USkillGraphSubsystem::CanPurchaseVendorOffer(
    const FPlayerSkillTreeState& State,
    const FHubVendorOffer& Offer,
    EHubSettlementType HubType,
    int32 CurrentLocalReputation,
    FText& OutDenialReason
) const {
    OutDenialReason = FText::GetEmpty();

    const int32 RequiredTier = FMath::Clamp(Offer.RequiredVendorTier, 1, 3);
    const int32 MaxHubTier = GetMaxVendorTierForHubType(HubType);
    if (MaxHubTier < RequiredTier) {
        OutDenialReason = NSLOCTEXT("SkillGraph", "VendorTierTooLow", "Händler-Stufe in diesem Ort zu niedrig.");
        return false;
    }

    const int32 ClampedRep = FMath::Clamp(CurrentLocalReputation, MinLocalReputation, MaxLocalReputation);
    const int32 RequiredRep = FMath::Clamp(Offer.RequiredLocalReputation, MinLocalReputation, MaxLocalReputation);
    if (ClampedRep < RequiredRep) {
        OutDenialReason = NSLOCTEXT("SkillGraph", "LocalReputationTooLow", "Lokaler Ruf in diesem Ort nicht ausreichend.");
        return false;
    }

    if (!Offer.RequiredSkillID.IsNone()) {
        const int32 NeededRank = FMath::Max(1, Offer.RequiredSkillRank);
        if (GetNodeRank(State, Offer.RequiredSkillID) < NeededRank) {
            OutDenialReason = NSLOCTEXT("SkillGraph", "VendorSkillMissing", "Erforderlicher Skill-Knoten oder Anker fehlt.");
            return false;
        }
    }

    return true;
}

int32 USkillGraphSubsystem::CalculateFinalVendorPrice(
    const FPlayerSkillTreeState& State,
    int32 BasePrice,
    EHubSettlementType HubType,
    int32 CurrentReputation,
    bool bIsBlueprintOrRareStock
) const {
    if (BasePrice <= 0) {
        return 0;
    }

    float PriceMultiplier = 1.0f;
    switch (GetReputationTier(CurrentReputation)) {
    case EReputationTier::Tier1_Acquaintance:
        PriceMultiplier *= 1.10f;
        break;
    case EReputationTier::Tier2_Trusted:
        PriceMultiplier *= 1.00f;
        break;
    case EReputationTier::Tier3_Honored:
        PriceMultiplier *= 0.85f;
        break;
    default:
        break;
    }

    switch (HubType) {
    case EHubSettlementType::MinorSettlement:
        PriceMultiplier *= bIsBlueprintOrRareStock ? 1.15f : 0.95f;
        break;
    case EHubSettlementType::MinorCity:
        PriceMultiplier *= 1.00f;
        break;
    case EHubSettlementType::Capital:
        PriceMultiplier *= bIsBlueprintOrRareStock ? 1.10f : 0.98f;
        break;
    default:
        break;
    }

    const int32 N13Rank = GetNodeRank(State, TEXT("N13"));
    const int32 A04Rank = GetNodeRank(State, TEXT("A04"));
    const float SkillDiscount = (0.04f * static_cast<float>(FMath::Max(0, N13Rank))) + ((A04Rank > 0) ? 0.08f : 0.0f);
    PriceMultiplier *= FMath::Clamp(1.0f - SkillDiscount, 0.55f, 1.50f);

    return FMath::Max(1, FMath::RoundToInt(static_cast<float>(BasePrice) * PriceMultiplier));
}

int32 USkillGraphSubsystem::GetXPRequiredForNextLevel(int32 CurrentLevel) const {
    if (CurrentLevel < 1 || CurrentLevel >= MaxCharacterLevelCap) {
        return 0;
    }
    const double LevelDouble = static_cast<double>(CurrentLevel);
    const double RequiredXP = 120.0 + (25.0 * (LevelDouble - 1.0)) + (1.25 * FMath::Square(LevelDouble - 1.0));
    return FMath::Max(50, FMath::RoundToInt(RequiredXP));
}

int32 USkillGraphSubsystem::GetCumulativeXPForLevel(int32 TargetLevel) const {
    const int32 ClampedTarget = FMath::Clamp(TargetLevel, 1, MaxCharacterLevelCap);
    int32 TotalXP = 0;
    for (int32 Lvl = 1; Lvl < ClampedTarget; ++Lvl) {
        TotalXP += GetXPRequiredForNextLevel(Lvl);
    }
    return TotalXP;
}

int32 USkillGraphSubsystem::AwardMilestoneXP(
    UObject* WorldContextObject,
    FPlayerSkillTreeState& InOutState,
    EXPMilestoneCategory Category,
    int32 BaseXPAmount,
    EHubSettlementType HubType,
    bool bIsMajorMilestone
) {
    if (WorldContextObject && WorldContextObject->GetWorld()) {
        if (WorldContextObject->GetWorld()->GetNetMode() == NM_Client) {
            return 0;
        }
    }

    if (BaseXPAmount <= 0) {
        return 0;
    }

    InOutState.CharacterLevel = FMath::Clamp(InOutState.CharacterLevel, 1, MaxCharacterLevelCap);
    InOutState.CurrentXP = FMath::Max(0, InOutState.CurrentXP);

    float CategoryMult = 1.0f;
    switch (Category) {
    case EXPMilestoneCategory::ThreeStageBuild:
        CategoryMult = GetAggregatedModifierWithResonances(InOutState, TEXT("Build.StageSpeed"), 1.0f);
        break;
    case EXPMilestoneCategory::CaveZone:
        CategoryMult = GetAggregatedModifierWithResonances(InOutState, TEXT("Cave.OreYieldBonus"), 1.0f);
        break;
    case EXPMilestoneCategory::TradeAndHubs:
        CategoryMult = GetAggregatedModifierWithResonances(InOutState, TEXT("Hub.ReputationGainMult"), 1.0f);
        break;
    case EXPMilestoneCategory::SettlerMilestone:
        CategoryMult = GetAggregatedModifierWithResonances(InOutState, TEXT("Settler.WorkSpeedMultiplier"), 1.0f);
        break;
    default:
        break;
    }

    const float EffectiveMult = FMath::Clamp(1.0f + ((CategoryMult - 1.0f) * 0.5f), 0.5f, 2.0f)
                              * (bIsMajorMilestone ? 1.5f : 1.0f);
    const int32 GrantedXP = FMath::Max(1, FMath::RoundToInt(static_cast<float>(BaseXPAmount) * EffectiveMult));

    if (InOutState.CharacterLevel >= MaxCharacterLevelCap) {
        InOutState.CurrentXP = 0;
        return 0;
    }

    InOutState.CurrentXP += GrantedXP;
    const int32 OldLevel = InOutState.CharacterLevel;
    int32 AwardedPoints = 0;

    while (InOutState.CharacterLevel < MaxCharacterLevelCap) {
        const int32 NeededForNext = GetXPRequiredForNextLevel(InOutState.CharacterLevel);
        if (NeededForNext <= 0 || InOutState.CurrentXP < NeededForNext) {
            break;
        }
        InOutState.CurrentXP -= NeededForNext;
        InOutState.CharacterLevel += 1;
        InOutState.AvailableSkillPoints += 1;
        AwardedPoints += 1;
    }

    if (InOutState.CharacterLevel >= MaxCharacterLevelCap) {
        InOutState.CharacterLevel = MaxCharacterLevelCap;
        InOutState.CurrentXP = 0;
    }

    if (InOutState.CharacterLevel > OldLevel) {
        OnPlayerLeveledUp.Broadcast(OldLevel, InOutState.CharacterLevel, AwardedPoints);
    }
    OnSkillTreeStateChanged.Broadcast(InOutState);
    return GrantedXP;
}

void USkillGraphSubsystem::ValidateAndSanitizeSaveState(FPlayerSkillTreeState& InOutState) const {
    if (InOutState.SchemaVersion <= 0) {
        InOutState.SchemaVersion = CurrentSkillSaveSchemaVersion;
    }

    InOutState.CharacterLevel = FMath::Clamp(InOutState.CharacterLevel, 1, MaxCharacterLevelCap);
    InOutState.CurrentXP = FMath::Max(0, InOutState.CurrentXP);
    if (InOutState.CharacterLevel >= MaxCharacterLevelCap) {
        InOutState.CurrentXP = 0;
    }

    int32 SpentPoints = 0;
    TArray<FName> KeysToRemove;

    for (TPair<FName, int32>& Pair : InOutState.UnlockedSkillRanks) {
        const FSkillGraphNode* Node = SkillNodes.Find(Pair.Key);
        if (!Node || Pair.Value <= 0) {
            KeysToRemove.Add(Pair.Key);
            continue;
        }
        Pair.Value = FMath::Clamp(Pair.Value, 1, Node->MaxRank);
        SpentPoints += Pair.Value * FMath::Max(1, Node->PointCostPerRank);
    }

    for (const FName& Key : KeysToRemove) {
        InOutState.UnlockedSkillRanks.Remove(Key);
    }

    const int32 MaxPointsForLevel = GetMaxTotalSkillPointsForLevel(InOutState.CharacterLevel);
    if (SpentPoints > MaxPointsForLevel) {
        InOutState.UnlockedSkillRanks.Reset();
        InOutState.AvailableSkillPoints = MaxPointsForLevel;
    }
    else {
        InOutState.AvailableSkillPoints = FMath::Clamp(InOutState.AvailableSkillPoints, 0, MaxPointsForLevel - SpentPoints);
    }
}