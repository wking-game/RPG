# Küstenasche – Vollständige Gesamtdokumentation (UE5 Action-RPG Framework)

## Inhaltsverzeichnis

1. [System-Leitplanken, Caps & Kernprinzipien](#1-system-leitplanken-caps--kernprinzipien)
2. [Spieler-Progression (Level 1–50), Waffen, Rüstung & Resistenzen](#2-spieler-progression-level-150-waffen-rüstung--resistenzen)
   * [2.1 Zusammenspiel von Charakterlevel, Gefahren-Rang, Waffen & Rüstung](#21-zusammenspiel-von-charakterlevel-gefahren-rang-waffen--rüstung)
   * [2.2 Detaillierte Progressionstabelle (Charakterlevel 1 bis 50)](#22-detaillierte-progressionstabelle-charakterlevel-1-bis-50)
   * [2.3 Waffen- & Rüstungs-Archetypen sowie Resistenz-Deckel](#23-waffen---rüstungs-archetypen-sowie-resistenz-deckel)
3. [Mathematik: Schadensreduktion, Resistenzen, Poise, Captain-Cap & Nachzügler](#3-mathematik-schadensreduktion-resistenzen-poise-captain-cap--nachzügler)
   * [3.1 Schadensreduktion durch Rüstung (HP-Schaden)](#31-schadensreduktion-durch-rüstung-hp-schaden)
   * [3.2 Resistenzen (Hitze/Asche, Frost/Kälte, Sturm/Böen, Verfall/Sporen)](#32-resistenzen-hitzeasche-frostkälte-sturmböen-verfallsporen)
   * [3.3 Haltungsschaden (Poise) und Haltungswiderstand (PoiseResist)](#33-haltungsschaden-poise-und-haltungswiderstand-poiseresist)
   * [3.4 Gedämpfte Captain-Kurve (Rang 6 bis Captain-Cap 25) & 85 % – 115 % Spawn-Varianz](#34-gedämpfte-captain-kurve-rang-6-bis-captain-cap-25--85----115--spawn-varianz)
   * [3.5 Faire Nachzügler-Loot-Dämpfung (EvaluateFairDownscaledLoot)](#35-faire-nachzügler-loot-dämpfung-evaluatefairdownscaledloot)
4. [Das vollständige 48er-Kreaturen-Roster (Nr. 1–48 – ohne Wasserwesen)](#4-das-vollständige-48er-kreaturen-roster-nr-148--ohne-wasserwesen)
   * [4.1 Erste 21 eigene Kreaturen (Nr. 1–21) – Basiswerte](#41-erste-21-eigene-kreaturen-nr-121--basiswerte)
   * [4.2 Klassische Säulen & Drachen (Nr. 22–30) – Basiswerte](#42-klassische-säulen--drachen-nr-2230--basiswerte)
   * [4.3 18 neue eigene Kreaturen (Nr. 31–48) – Beschreibung, Rolle & Basiswerte](#43-18-neue-eigene-kreaturen-nr-3148--beschreibung-rolle--basiswerte)
5. [Die 5 Gebiete & Spawn-Verteilung der 18 neuen Kreaturen (Nr. 31–48)](#5-die-5-gebiete--spawn-verteilung-der-18-neuen-kreaturen-nr-3148)
   * [5.1 Funktionsweise von bScaleToZoneRank, ScaleToZoneChance, MaxRankCap & FixedRankOverride](#51-funktionsweise-von-bscaletozonerank-scaletozonechance-maxrankcap--fixedrankoverride)
   * [5.2 Moosgeäst & Sumpf-Senken](#52-moosgeäst--sumpf-senken)
   * [5.3 Bruchstein & Gewölbe-Spalten](#53-bruchstein--gewölbe-spalten)
   * [5.4 Glut-Fluren & Schlacke-Böden](#54-glut-fluren--schlacke-böden)
   * [5.5 Schroffen-Grate & Frost-Kanten](#55-schroffen-grate--frost-kanten)
   * [5.6 Böen-Kämme & Sturm-Kuppen](#56-böen-kämme--sturm-kuppen)
   * [5.7 JSON-Beispiel für die neuen Spawn-Regeln](#57-json-beispiel-für-die-neuen-spawn-regeln)
6. [Vollständiger Loot-Katalog aller 48 Kreaturen](#6-vollständiger-loot-katalog-aller-48-kreaturen)
   * [6.1 Loot-Regeln, Zahlenformate & Biom-IDs](#61-loot-regeln-zahlenformate--biom-ids)
   * [6.2 Loot-Tabellen: Erste 21 eigene Kreaturen (Nr. 1–21)](#62-loot-tabellen-erste-21-eigene-kreaturen-nr-121)
   * [6.3 Loot-Tabellen: Klassische Säulen & Drachen (Nr. 22–30)](#63-loot-tabellen-klassische-säulen--drachen-nr-2230)
   * [6.4 Loot-Tabellen: 18 neue eigene Kreaturen (Nr. 31–48)](#64-loot-tabellen-18-neue-eigene-kreaturen-nr-3148)
   * [6.5 JSON-Strukturbeispiele für DT_CreatureLoot (Nr. 31, Nr. 39 & Nr. 48)](#65-json-strukturbeispiele-für-dt_creatureloot-nr-31-nr-39--nr-48)
7. [Item-Kategorien, Stats, Verbauung & Alternativquellen (Kreaturen-Loot)](#7-item-kategorien-stats-verbauung--alternativquellen-kreaturen-loot)
   * [7.1 Überblick der Item-Kategorien](#71-überblick-der-item-kategorien)
   * [7.2 Rüstung, Häute, Schuppen & Panzerplatten](#72-rüstung-häute-schuppen--panzerplatten)
   * [7.3 Waffen, Zähne, Klauen, Hörner & Haltungsschaden](#73-waffen-zähne-klauen-hörner--haltungsschaden)
   * [7.4 Sehnen, Fasern, Gewebe & Bindungen](#74-sehnen-fasern-gewebe--bindungen)
   * [7.5 Kerne, Glut, Frost, Sturm, Sporen & Essen/Tränke](#75-kerne-glut-frost-sturm-sporen--essentränke)
   * [7.6 Biom-Zusatz-Drops & Quest-/Relikt-Gegenstände](#76-biom-zusatz-drops--questrelikt-gegenstände)
8. [Crafting- & Bau-Rezepte über alle Level-50-Phasen](#8-crafting---bau-rezepte-über-alle-level-50-phasen)
   * [8.1 Die 5 Progression-Phasen (Level 1–50)](#81-die-5-progression-phasen-level-150)
   * [8.2 Konkrete Beispiel-Rezepte (Waffen, Rüstungen, Tränke/Essen & 200-cm-Bauwerke)](#82-konkrete-beispiel-rezepte-waffen-rüstungen-tränkeessen--200-cm-bauwerke)
   * [8.3 Durchgehende Rezept-Übersicht über Level 1–50](#83-durchgehende-rezept-übersicht-über-level-150)
9. [UE5 C++-Referenzimplementierungen](#9-ue5-c-referenzimplementierungen)
   * [9.1 ProgressionAndGearTypes.h (Level-Cap 50 & Captain-Cap 25)](#91-progressionandgeartypesh-level-cap-50--captain-cap-25)
   * [9.2 CombatMathLibrary.h (Rüstung, Resistenzen & Poise)](#92-combatmathlibraryh-rüstung-resistenzen--poise)
   * [9.3 CreatureLootRoller.h (RollCreatureLootFromRow & Nachzügler-Auswertung)](#93-creaturelootrollerh-rollcreaturelootfromrow--nachzügler-auswertung)
   * [9.4 CraftingRecipeTypes.h & CraftingExecutionLibrary.h (CanCraftRecipe & ConsumeRecipeIngredients)](#94-craftingrecipetypesh--craftingexecutionlibraryh-cancraftrecipe--consumerecipeingredients)

---

## 1. System-Leitplanken, Caps & Kernprinzipien

* **Charakterlevel-Cap (`50`):** Das Spielerlevel ist auf `50` gedeckelt (`MaxPlayerLevelCap = 50`). Alle Waffen-, Rüstungs-, Resistenz-, Verbrauchs- und Bau-Rezepte sind in 5 Phasen (`Lv. 1–10`, `Lv. 11–20`, `Lv. 21–32`, `Lv. 33–42`, `Lv. 43–50`) auf dieses Fenster abgestimmt.
* **Reines Land-/Fels-/Ruinen-/Sturm-/Asche-Roster (`48` Kreaturen – keine Wasserwesen):** Sämtliche `48` Kreaturen sind an Land, in Senken, Ruinen, Asche- und Schlackeflächen, Felsgraten oder Sturm-Hochlagen verortet. Es gibt **keine Wasserwesen**.
* **Gefahren-Ränge & Captain-Cap (`25`):** Die tabellarischen Basiswerte der Kreaturen decken `Rang 2` bis `Rang 6+` ab. Nach oben hin gilt das **Captain-Cap `25`** (`MaxCaptainRankCap = 25`), ab `Rang 6` über eine gedämpfte Kurve (`t^{0.65}`) skaliert.
* **Individuelle Spawn-Varianz (`85 % – 115 %`):** Jedes gespawnte Exemplar erhält eine individuelle Varianz zwischen `0.85` und `1.15` auf seine Kampfwerte (`FinalHP`, `FinalMaxPoise`, `FinalDmg`).
* **Haltung (`Poise`) & Schwachstellen:** Kreaturen besitzen eine `MaxPoise`-Leiste; Treffer mit Waffen-`PoiseDmg` sowie Schwachstellen- und Angriffs-Multiplikatoren führen zum `Poise Break` (Taumel-/Konterfenster). Umgekehrt schützt der `Haltungswiderstand` (`PoiseResist`) der Spielerrüstung vor Unterbrechungen.
* **Modulares `200-cm`-Bauraster:** Bau- und Befestigungselemente werden auf einem `200 × 200 cm`-Raster platziert und mit Kreaturen- und Biom-Materialien gefertigt.

---

## 2. Spieler-Progression (Level 1–50), Waffen, Rüstung & Resistenzen

### 2.1 Zusammenspiel von Charakterlevel, Gefahren-Rang, Waffen & Rüstung

* **Spieler-Progression (`Level 1` bis `Level 50`):**
  * **Level 1–10 (Einstieg & Dorn-Stufe):** Erste Ausrüstung, leichte Grundwerte, frühe Begegnungen (`Rang 1–2`).
  * **Level 11–20 (Klapper-/Ruinen- & frühe Bollwerk-Stufe):** Mittelstarke Waffen/Rüstungen, erste Haltungs-/Schwachstellen-Checks (`Rang 2–4`).
  * **Level 21–35 (Schlacke-/Bollwerk- & Wurm-/Grat-Stufe):** Gehärtete Rüstungen, schärfere Waffen, schwere Ramm-/Sturz-/Böen-Gegner (`Rang 4–5`).
  * **Level 36–50 (Wyrm-/Koloss-Stufe & Captain-Cap 25):** Spitzen-Ausrüstung (`Tier 4` / Endgame-Stufen), voller `Poise`- & Ausdauer-Druck, Spitzen-Kreaturen und Captain-Spitzen bis Rang 25.
* **Waffen- & Rüstungswerte abgestimmt auf Level 1–50:**
  * **Waffen:** Definieren `BaseWeaponDmg`, `PoiseDmg` (Haltungsschaden), `AttackSpeed` und `StaminaCostPerSwing`. Mit steigendem Spieler-Level/Tier wachsen `BaseWeaponDmg` und `PoiseDmg` stetig, damit `Poise`-Brüche bei höheren Kreaturen-Rängen machbar bleiben, ohne dass schwache Treffer die Rüstung eines `Ironhide` oder `Cragdrake` mühelos durchschlagen.
  * **Rüstungen:** Definieren `ArmorRating` (Schadensreduktion gegen HP-Treffer), `PoiseResist` / `MaxPlayerPoise` (Schutz vor Unterbrechung/Taumeln durch gegnerische Haltungstreffer) und `StaminaRegenMod` / `Elemental/HazardResist` (Hitze, Kälte, Böen, Staub).
* **Kreaturenwerte (`BaseHP / MaxPoise / BaseDmg`) auf Level-50-Cap kalibriert:**
  * Die Werte der Ränge sind so austariert, dass ein **Level-50-Spieler** mit passender Ausrüstung auf **Rang 6 / Captain 25** eine echte, spannungsreiche Herausforderung hat – nicht trivialisiert, aber auch nicht mit unfairen One-Shots überladen.

### 2.2 Detaillierte Progressionstabelle (Charakterlevel 1 bis 50)

* **Charakterlevel (`Lv. 1 – 50`):** Aufgeteilt in 10 Fünfer-Stufen als Ankerpunkte für Progression, Crafting-Tiers und DataTables.
* **Waffe (`Waffen-Schaden` / `Waffen-PoiseDmg`):**
  * *Schaden:* Basis-Schlagwert einer Standard-Einhand-/Allround-Waffe (schwere Waffen darüber, schnelle Waffen darunter).
  * *Haltungsschaden (`PoiseDmg`):* Wie stark ein Treffer die gegnerische `MaxPoise`-Leiste füllt, um Haltungsbruch (`Poise Break`) und Taumel-/Konterfenster auszulösen.
* **Rüstung (`Rüstung` / `Haltungswiderstand`):**
  * *Rüstung:* Reduziert eingehenden HP-Schaden über die Schadensformel.
  * *Haltungswiderstand (`PoiseResist`):* Bestimmt, wie viel Haltungsschaden der Spieler selbst einstecken kann, bevor er durch gegnerische Schläge, Böen oder Rammstöße unterbrochen oder aus dem Gleichgewicht gebracht wird.
* **Resistenzen (`Hitze / Asche` | `Frost / Kälte` | `Sturm / Böen` | `Verfall / Sporen`):** Prozentuale Abschwächung von Umwelt- und Zustands-Effekten aus den 5 Gebieten.

| Charakterlevel | Typischer Gefahren-Rang | Ausrüstungs-Phase | Waffen-Schaden | Waffen-PoiseDmg | Rüstung | Haltungswiderstand | Resistenzen (Hitze / Frost / Sturm / Verfall) |
| :---: | :---: | :--- | :---: | :---: | :---: | :---: | :---: |
| **Lv. 1 – 5** | **Rang 1** (*Falter- / Grenz-Stufe*) | Einfacher Einstieg / Notbehelf | `14 – 18` | `12 – 15` | `15 – 28` | `20 – 28` | `0–5 %` / `0–5 %` / `0–5 %` / `0–5 %` |
| **Lv. 6 – 10** | **Rang 2** (*Dorn-Stufe*) | Frühe Wildnis-Ausrüstung | `19 – 28` | `16 – 22` | `29 – 45` | `29 – 40` | `5–10 %` / `5–10 %` / `5–8 %` / `5–10 %` |
| **Lv. 11 – 15** | **Rang 2 – 3** (*Dorn- bis Klapper-Stufe*) | Gehärtete Feld- & Ruinen-Gear | `29 – 38` | `23 – 29` | `46 – 65` | `41 – 52` | `10–15 %` / `8–14 %` / `8–12 %` / `10–16 %` |
| **Lv. 16 – 20** | **Rang 3** (*Klapper- / Ruinen-Stufe*) | Solide Mittelklasse-Ausrüstung | `39 – 48` | `30 – 36` | `66 – 85` | `53 – 65` | `14–20 %` / `12–18 %` / `10–16 %` / `14–22 %` |
| **Lv. 21 – 25** | **Rang 3 – 4** (*Ruinen- bis Bollwerk-Stufe*) | Verstärkte Bollwerk-/Schlacke-Gear | `49 – 60` | `37 – 45` | `86 – 110` | `66 – 80` | `18–26 %` / `16–24 %` / `14–20 %` / `18–26 %` |
| **Lv. 26 – 30** | **Rang 4** (*Schlacke- / Bollwerk-Stufe*) | Schwere Gebiets- & Krusten-Gear | `61 – 74` | `46 – 54` | `111 – 135` | `81 – 95` | `22–32 %` / `20–30 %` / `18–25 %` / `22–30 %` |
| **Lv. 31 – 35** | **Rang 4 – 5** (*Bollwerk- bis Grat-Stufe*) | Hochrangige Berg-/Glut-Ausrüstung | `75 – 89` | `55 – 64` | `136 – 162` | `96 – 112` | `28–38 %` / `26–36 %` / `22–32 %` / `25–34 %` |
| **Lv. 36 – 40** | **Rang 5** (*Wurm- / Grat-Stufe*) | Gehärtete Drachen-/Grat-Ausrüstung | `90 – 105` | `65 – 75` | `163 – 190` | `113 – 130` | `32–44 %` / `32–42 %` / `28–38 %` / `28–38 %` |
| **Lv. 41 – 45** | **Rang 5 – 6+** (*Grat- bis Wyrm-/Koloss-Stufe*) | Spitzen-Ausrüstung (Frühes Endgame) | `106 – 124` | `76 – 89` | `191 – 224` | `131 – 152` | `38–50 %` / `38–48 %` / `34–46 %` / `32–42 %` |
| **Lv. 46 – 50** | **Rang 6+ bis Captain-Cap 25** (*Maximalstufe*) | Volles Endgame-Set (Cap Lv. 50) | `125 – 145` | `90 – 105` | `225 – 260` | `153 – 175` | `42–55 %` / `42–55 %` / `40–52 %` / `36–48 %` |

### 2.3 Waffen- & Rüstungs-Archetypen sowie Resistenz-Deckel

* **Waffen-Archetypen (Modifikatoren auf die Tabellenwerte):**
  * *Schnelle Klingen / leichte Waffen:* ca. `0.80x` Schaden, `0.65x` `PoiseDmg`, geringer Ausdauerverbrauch – ideal gegen flinke Flankierer wie `Dustmote`, `Cinderling` oder `Skitterclaw`.
  * *Standard-Waffen (Schwerter / Äxte / Speere):* `1.00x` Schaden und `1.00x` `PoiseDmg` als Referenzwert aus der Tabelle.
  * *Schwere Brecher / Zweihänder / Hämmer:* ca. `1.25x–1.40x` Schaden und `1.35x–1.60x` `PoiseDmg` – stark gegen gepanzerte Ziele (`Ironhide`, `Kraggar`, `Slagbrute`, `Mossgolem`) und zum Brechen hoher `MaxPoise`-Leisten.
* **Rüstungs-Archetypen & Resistenzen:**
  * *Leichte / bewegliche Ausrüstung:* `0.75x–0.85x` `Rüstung` und `0.70x–0.80x` `Haltungswiderstand`, dafür bessere Ausdauer-Regeneration und Agilität.
  * *Schwere Bollwerk-/Platten-Ausrüstung:* `1.15x–1.30x` `Rüstung` und `1.20x–1.35x` `Haltungswiderstand` für hohe Standfestigkeit gegen Stöße, Böen und schwere Hiebe.
  * *Resistenz-Deckel:* Selbst mit maximaler Ausrüstung auf Level 50 bleiben Resistenzen sinnvoll gedeckelt (z. B. `60 %` über die Formel), damit Umweltgefahren wie Hitze, Frost, Sturm oder Verfall nie komplett bedeutungslos werden.

---

## 3. Mathematik: Schadensreduktion, Resistenzen, Poise, Captain-Cap & Nachzügler

### 3.1 Schadensreduktion durch Rüstung (HP-Schaden)

Eine hyperbolische Reduktionskurve mit abnehmendem Grenzertrag verhindert, dass Rüstung `100 %` erreicht und Treffer auf `0` fallen. Die Skalierungskonstante $K_{\text{armor}}$ orientiert sich an den Rüstungs-Richtwerten der Level-1–50-Tabelle und wird auf $\text{MaxArmorCap} = 0.75$ (`75 %`) gedeckelt:

$$\text{ArmorReduction} = \min\left(\text{MaxArmorCap},\; \frac{\text{Armor}}{\text{Armor} + K_{\text{armor}}}\right)$$

$$\text{FinalHPDamage} = \max\left(\text{MinDmg},\; \text{RawDmg} \cdot \left(1 - \text{ArmorReduction}\right)\right)$$

* **Typische Richtwerte:** $\text{MaxArmorCap} = 0.75$ (`75 %`), $K_{\text{armor}} = 160$, $\text{MinDmg} = 1.0$.
* **Praxis-Check auf Level 50:** Mit einer starken Rüstung von `240` liegt $\text{ArmorReduction}$ bei $\frac{240}{240 + 160} = 0.60$ (`60 %` Reduktion) – ein `1.15`-Rang-6-Gegner mit `50 BaseDmg` und `1.4x` Schwerangriff (`86.25 RawDmg`) drückt dann noch ca. `34.5` HP durch, statt harmlos zu verpuffen.

### 3.2 Resistenzen (Hitze/Asche, Frost/Kälte, Sturm/Böen, Verfall/Sporen)

Resistenzen wirken als **prozentuale Abschwächung** für Umwelt- und Zustands-Effekte (Asche-Hitze in *Glut-Fluren*, Kälte/Frost an *Schroffen-Graten*, Böendruck auf *Böen-Kämmen* oder Sporen-/Verfall-Reize in *Bruchstein* / *Moosgeäst*). Ein festes Cap ($\text{MaxResistCap} = 0.60$) stellt sicher, dass extreme Gebiete und Wetterlagen spürbar bleiben:

$$\text{EffResist} = \text{clamp}\left(\text{BaseResist} + \text{GearResist} + \text{BuffResist},\; \text{MinResist},\; \text{MaxResistCap}\right)$$

$$\text{FinalHazardEffect} = \text{RawHazardEffect} \cdot \left(1 - \text{EffResist}\right)$$

* **Typische Richtwerte:** $\text{MaxResistCap} = 0.60$ (`60 %`), $\text{MinResist} = -0.25$ (falls ein Debuff oder unpassende Ausrüstung Resistenzen senkt).
* **Praxis-Check:** Ein Spieler mit `45 %` Kälteresistenz (`0.45`) in frostiger Höhe nimmt nur noch `55 %` der Ausdauer-Verlangsamung oder des Kälte-Effekts eines `Frostdrake` oder `Rimestalker` auf.

### 3.3 Haltungsschaden (Poise) und Haltungswiderstand (PoiseResist)

* **Haltungsschaden durch Spieler-Waffen an Kreaturen:**

$$\text{AppliedEnemyPoiseDmg} = \text{WeaponPoiseDmg} \cdot \text{AttackPoiseMult} \cdot \text{WeakPointPoiseMult}$$

$$\text{CurrentEnemyPoise} \leftarrow \text{CurrentEnemyPoise} + \text{AppliedEnemyPoiseDmg}$$

* **Haltungsbruch (`Poise Break`):** Sobald $\text{CurrentEnemyPoise} \ge \text{FinalMaxPoise}$ (inklusive `85 % – 115 %` Spawn-Varianz), löst der Taumel-/Konterzustand aus, $\text{CurrentEnemyPoise}$ wird zurückgesetzt und Treffer im Fenster erhalten den `PoiseBreakDamageMult`.

* **Haltungswiderstand des Spielers gegen Kreaturen-Angriffe:**

$$\text{PoiseResistFactor} = \frac{K_{\text{poise}}}{\text{PlayerPoiseResist} + K_{\text{poise}}}$$

$$\text{FinalPlayerPoiseHit} = \text{EnemyBasePoiseDmg} \cdot \text{AttackPoiseMult} \cdot \text{PoiseResistFactor}$$

* **Typische Richtwerte:** $K_{\text{poise}} = 90$.
* **Praxis-Check auf Level 50:** Mit `150` `PoiseResist` liegt $\text{PoiseResistFactor}$ bei $\frac{90}{150 + 90} = 0.375$ (`62.5 %` Dämpfung der Haltungsbelastung).

### 3.4 Gedämpfte Captain-Kurve (Rang 6 bis Captain-Cap 25) & 85 % – 115 % Spawn-Varianz

* Für `EffectiveRank` zwischen `6` und dem **Captain-Cap `25`** gilt:

$$t = \text{clamp}\left(\frac{\text{EffectiveRank} - 6}{19},\; 0,\; 1\right), \quad \text{Curved} = t^{0.65}$$

$$\text{HP}_{\text{mult}} = 1 + 0.35 \cdot \text{Curved}, \quad \text{Poise}_{\text{mult}} = 1 + 0.20 \cdot \text{Curved}, \quad \text{Dmg}_{\text{mult}} = 1 + 0.25 \cdot \text{Curved}$$

* Danach wird für jedes gespawnte Exemplar eine Spawn-Varianz im Bereich `[0.85, 1.15]` (`85 % – 115 %`) auf HP, Poise und Schaden angewendet.

### 3.5 Faire Nachzügler-Loot-Dämpfung (EvaluateFairDownscaledLoot)

* **Rang-Sperre:** Ein Drop-Eintrag wird **nur** gewürfelt, wenn `Entry.MinEffectiveRank <= EffectiveRank` gilt.
* **Nachzügler-Dämpfung bei `Delta = ZoneRank - EffectiveRank > 0`:**
  * **`Delta == 0`:** `ChanceMult = 1.00`, `CountMult = 1.00`. Wenn `bScaleCountByRankDelta == true` ist, erhöht sich die Menge zusätzlich um `EffectiveRank - Entry.MinEffectiveRank`.
  * **`Delta == 1`:** `ChanceMult = 0.85`, `CountMult = 0.80` (gerundet auf mindestens `1` bei erfolgreichem Wurf).
  * **`Delta == 2`:** `ChanceMult = 0.70`, `CountMult = 0.65` (gerundet auf mindestens `1` bei erfolgreichem Wurf).
  * **`Delta >= 3`:** `ChanceMult = 0.55`, `CountMult = 0.65` (gerundet auf mindestens `1` bei erfolgreichem Wurf).
* **Kerndrop-Schutz (`DropChance >= 0.95`):** Bei `Delta > 0` und `bIsBiomeBonus == false` gilt für sichere Kerndrops mindestens `ChanceMult = 0.90`.
* **Biom-Zusatz-Drops (`bIsBiomeBonus == true`):** Werden nur gewürfelt, wenn `ActiveBiomeID` zum `FLootBiomeOverride.BiomeID` passt; erhalten bei `Delta > 0` denselben `ChanceMult`.

---

## 4. Das vollständige 48er-Kreaturen-Roster (Nr. 1–48 – ohne Wasserwesen)

Alle Basiswerte (`BaseHP / MaxPoise / BaseDmg`) gelten vor dem individuellen **`85 % – 115 %` Spawn-Roll**, vor Biom-/Angriffs-Modifikatoren sowie vor der Captain-Kurve oberhalb von Rang 6 (bis Captain-Cap `25`).

### 4.1 Erste 21 eigene Kreaturen (Nr. 1–21) – Basiswerte

| Nr. | Name | MinRank | Rang 2 | Rang 3 | Rang 4 | Rang 5 | Rang 6+ |
| :---: | :--- | :---: | :---: | :---: | :---: | :---: | :---: |
| **1** | **Vaelgorn** | `3` | – | `640 / 70 / 19` | `890 / 92 / 24` | `1.220 / 118 / 30` | `1.650 / 145 / 38` |
| **2** | **Cinderloom** | `4` | – | – | `940 / 88 / 25` | `1.280 / 112 / 31` | `1.720 / 138 / 39` |
| **3** | **Osskarr** | `3` | – | `620 / 68 / 19` | `860 / 90 / 24` | `1.180 / 114 / 30` | `1.580 / 140 / 37` |
| **4** | **Morrowear** | `3` | – | `660 / 75 / 18` | `910 / 96 / 23` | `1.240 / 120 / 29` | `1.660 / 148 / 36` |
| **5** | **Pyroclast** | `4` | – | – | `1.080 / 122 / 26` | `1.480 / 152 / 33` | `1.980 / 185 / 41` |
| **6** | **Dravik** | `5` | – | – | – | `1.560 / 155 / 34` | `2.120 / 192 / 43` |
| **7** | **Murghul** | `3` | – | `580 / 58 / 19` | `810 / 76 / 24` | `1.100 / 96 / 30` | `1.480 / 118 / 37` |
| **8** | **Kraggar** | `4` | – | – | `1.120 / 130 / 25` | `1.520 / 160 / 32` | `2.050 / 195 / 40` |
| **9** | **Galeborn** | `5` | – | – | – | `1.420 / 132 / 34` | `1.920 / 165 / 43` |
| **10** | **Ashstalker** | `2` | `390 / 40 / 13` | `580 / 56 / 18` | `800 / 74 / 23` | `1.080 / 94 / 29` | `1.440 / 115 / 36` |
| **11** | **Thornmaw** | `2` | `420 / 45 / 13` | `620 / 62 / 18` | `860 / 82 / 23` | `1.150 / 104 / 29` | `1.540 / 128 / 36` |
| **12** | **Dustmote** | `2` | `260 / 25 / 10` | `390 / 35 / 14` | `550 / 48 / 19` | `750 / 64 / 24` | `1.000 / 78 / 30` |
| **13** | **Mirecrawler** | `2` | `450 / 50 / 12` | `660 / 68 / 17` | `910 / 88 / 22` | `1.220 / 110 / 28` | `1.620 / 135 / 35` |
| **14** | **Cinderhound** | `3` | – | `610 / 60 / 19` | `840 / 78 / 24` | `1.140 / 98 / 30` | `1.520 / 120 / 37` |
| **15** | **Graveclaw** | `3` | – | `680 / 76 / 19` | `940 / 98 / 24` | `1.280 / 124 / 31` | `1.700 / 152 / 38` |
| **16** | **Rimestalker** | `4` | – | – | `890 / 82 / 25` | `1.220 / 104 / 32` | `1.640 / 128 / 40` |
| **17** | **Hollowback** | `4` | – | – | `960 / 96 / 24` | `1.320 / 120 / 30` | `1.780 / 148 / 38` |
| **18** | **Ironhide** | `4` | – | – | `1.150 / 135 / 26` | `1.580 / 168 / 33` | `2.150 / 205 / 41` |
| **19** | **Shroudwalker** | `4` | – | – | `880 / 80 / 25` | `1.200 / 102 / 32` | `1.620 / 126 / 40` |
| **20** | **Cragmaw** | `5` | – | – | – | `1.500 / 148 / 34` | `2.020 / 182 / 42` |
| **21** | **Dreadcolossus** | `6` | – | – | – | – | `3.200 / 255 / 54` |

### 4.2 Klassische Säulen & Drachen (Nr. 22–30) – Basiswerte

| Nr. | Name | MinRank | Rang 2 | Rang 3 | Rang 4 | Rang 5 | Rang 6+ |
| :---: | :--- | :---: | :---: | :---: | :---: | :---: | :---: |
| **22** | **Troll** | `4` | – | – | `1.140 / 132 / 27` | `1.560 / 164 / 34` | `2.100 / 200 / 42` |
| **23** | **Riese** | `5` | – | – | – | `1.780 / 180 / 37` | `2.450 / 222 / 46` |
| **24** | **Wiedergänger** | `3` | – | `640 / 72 / 19` | `880 / 94 / 24` | `1.200 / 118 / 30` | `1.600 / 145 / 37` |
| **25** | **Cinderwyrm** | `5` | – | – | – | `1.950 / 168 / 38` | `2.750 / 215 / 48` |
| **26** | **Cragdrake** | `5` | – | – | – | `2.050 / 182 / 37` | `2.850 / 230 / 47` |
| **27** | **Stormmaw** | `6` | – | – | – | – | `3.100 / 235 / 52` |
| **28** | **Ashwing** | `5` | – | – | – | `1.880 / 155 / 37` | `2.650 / 198 / 46` |
| **29** | **Frostdrake** | `5` | – | – | – | `2.000 / 175 / 37` | `2.800 / 220 / 47` |
| **30** | **Dreadwyrm** | `6` | – | – | – | – | `3.400 / 265 / 55` |

### 4.3 18 neue eigene Kreaturen (Nr. 31–48) – Beschreibung, Rolle & Basiswerte

1. **Bramblefang** (*MinRank 2* | Moosgeäst & Sumpf-Senken / Bruchstein): Gedrungenes, dornenbewachsenes Unterholz-Raubtier mit schnellen Schnappangriffen; lässt sich durch Haltungstreffer leicht aus dem Tritt bringen.
2. **Siltstrider** (*MinRank 2* | Moosgeäst & Sumpf-Senken / Glut-Fluren): Langbeiniger, trockener Boden- und Senkenläufer (kein Wasserwesen), der weite Schritte macht und bei Flankentreffern stolpert.
3. **Cinderling** (*MinRank 2* | Glut-Fluren & Schlacke-Böden / Bruchstein): Kleiner, glimmender Asche- und Funkenläufer mit niedrigem `Poise`, der in Gruppen flankiert und bei Treffern kurz Funken/Staub verliert.
4. **Sporeback** (*MinRank 3* | Moosgeäst & Sumpf-Senken / Bruchstein): Buckeliger Wald-/Ruinen-Schleicher mit rissiger Sporenkappe auf dem Rücken; platzt bei Haltungsschaden und stößt eine dämpfende Staub-/Sporenwolke aus.
5. **Skitterclaw** (*MinRank 3* | Bruchstein & Gewölbe-Spalten / Schroffen-Grate): Flinker, vielbeiniger Mauer- und Felskrabbler mit scharfen Klammerfüßen; nutzt enge Spalten und bricht zusammen, sobald sein `Poise` geleert ist.
6. **Gloomstalker** (*MinRank 3* | Bruchstein & Gewölbe-Spalten / Moosgeäst): Schmaler Schatten- und Ruinenpirscher mit gedämpften Schritten; wartet in dunklen Ecken auf unvorsichtige Vorstöße und reagiert empfindlich auf Konter.
7. **Charmaw** (*MinRank 3* | Glut-Fluren & Schlacke-Böden / Bruchstein): Breitmäuliger Glut- und Schlacke-Beißer mit verkohltem Kiefer; beißt sich fest und öffnet ein Konterfenster bei Haltungsschaden.
8. **Rimehound** (*MinRank 4* | Schroffen-Grate & Frost-Kanten / Böen-Kämme): Kälte- und windgehärteter Rudel-/Flankenjäger an frostigen Kanten; seine kalten Stöße bremsen Ausdauer-Regeneration, wenn man nicht ausweicht.
9. **Slagbrute** (*MinRank 4* | Glut-Fluren & Schlacke-Böden / Schroffen-Grate): Schwerer Schlacke- und Gesteinsbrocken-Schläger mit dicker Frontkruste; verlangt Haltungsschaden oder Flankenwechsel statt frontalem Gegenhalten.
10. **Vaultcrawler** (*MinRank 4* | Bruchstein & Gewölbe-Spalten): Gepanzerter Gewölbe- und Pfeilerkletterer mit Greifarmen; lässt sich von Decken-/Mauerpositionen herunterholen, wenn die Haltung bricht.
11. **Mossgolem** (*MinRank 4* | Moosgeäst & Sumpf-Senken / Bruchstein): Aus Moos, Wurzelgeflecht und altem Gestein gewachsener Wächter; extrem zäh gegen leichte Treffer, aber mit klaren Erholungsfenstern nach schweren Hieben.
12. **Galehound** (*MinRank 5* | Böen-Kämme & Sturm-Kuppen / Schroffen-Grate): Schneller, windgepeitschter Hochlagen-Läufer, der Böen ausnutzt und vor großen Stößen hörbar Spannung aufbaut.
13. **Cragstalker** (*MinRank 5* | Schroffen-Grate & Frost-Kanten / Böen-Kämme): Muskulöser Grat- und Klippenjäger mit weiten Sprüngen und hohem `Poise`; nutzt Höhenunterschiede und drängt auf schmalen Pfaden ab.
14. **Ashweaver** (*MinRank 5* | Glut-Fluren & Schlacke-Böden / Bruchstein): Größerer, ruß- und glutdurchzogener Faser- und Asche-Weber mit zähen Asche-Netzen und glimmenden Schwachpunkten.
15. **Frostmauler** (*MinRank 5* | Schroffen-Grate & Frost-Kanten / Böen-Kämme): Massiger Frost- und Fels-Brecher mit wuchtigen Pranken- und Bodenstößen, die Ausdauer und Haltung stark fordern.
16. **Stormwyrmling** (*MinRank 5* | Böen-Kämme & Sturm-Kuppen / Schroffen-Grate): Wendiger, jüngerer Sturm-Drache mit zähen Schwingen und schnellen Böen-Überflügen – der Vorbote der großen Sturm-Spitzen auf Rang 6+.
17. **Cindercolossus** (*MinRank 6* | Glut-Fluren & Schlacke-Böden / Bruchstein): Gewaltiger, glühender Schlacke- und Stein-Koloss ab Rang 6 mit massiven Bodenstößen, hohem `Poise` und klaren Schwachstellen in den Gelenken.
18. **Galewyrm** (*MinRank 6* | Böen-Kämme & Sturm-Kuppen): Mächtiger Sturm-Drache auf Spitzen-Niveau (Rang 6+ bis Captain 25) mit weiten Schwingenböen, Böendruck und klaren Konterfenstern nach Bodenkontakt.

| Nr. | Name | MinRank | Rang 2 | Rang 3 | Rang 4 | Rang 5 | Rang 6+ |
| :---: | :--- | :---: | :---: | :---: | :---: | :---: | :---: |
| **31** | **Bramblefang** | **Rang 2** | `410 / 42 / 13` | `610 / 60 / 18` | `840 / 78 / 23` | `1.120 / 98 / 29` | `1.480 / 120 / 36` |
| **32** | **Siltstrider** | **Rang 2** | `440 / 48 / 12` | `650 / 68 / 17` | `890 / 88 / 22` | `1.190 / 110 / 28` | `1.560 / 135 / 35` |
| **33** | **Cinderling** | **Rang 2** | `280 / 28 / 11` | `410 / 38 / 15` | `580 / 52 / 20` | `790 / 68 / 25` | `1.060 / 85 / 31` |
| **34** | **Sporeback** | **Rang 3** | – | `690 / 72 / 19` | `940 / 92 / 24` | `1.260 / 115 / 30` | `1.680 / 142 / 37` |
| **35** | **Skitterclaw** | **Rang 3** | – | `560 / 55 / 18` | `780 / 72 / 23` | `1.060 / 90 / 29` | `1.420 / 112 / 36` |
| **36** | **Gloomstalker** | **Rang 3** | – | `600 / 62 / 19` | `830 / 80 / 24` | `1.120 / 100 / 30` | `1.500 / 122 / 37` |
| **37** | **Charmaw** | **Rang 3** | – | `720 / 82 / 20` | `980 / 102 / 26` | `1.310 / 126 / 32` | `1.740 / 155 / 39` |
| **38** | **Rimehound** | **Rang 4** | – | – | `860 / 78 / 25` | `1.180 / 98 / 32` | `1.580 / 120 / 40` |
| **39** | **Slagbrute** | **Rang 4** | – | – | `1.100 / 128 / 26` | `1.500 / 158 / 33` | `2.020 / 192 / 41` |
| **40** | **Vaultcrawler** | **Rang 4** | – | – | `930 / 95 / 24` | `1.270 / 118 / 30` | `1.700 / 145 / 37` |
| **41** | **Mossgolem** | **Rang 4** | – | – | `1.160 / 135 / 25` | `1.580 / 165 / 32` | `2.120 / 200 / 40` |
| **42** | **Galehound** | **Rang 5** | – | – | – | `1.220 / 102 / 31` | `1.660 / 126 / 39` |
| **43** | **Cragstalker** | **Rang 5** | – | – | – | `1.480 / 145 / 33` | `1.980 / 178 / 41` |
| **44** | **Ashweaver** | **Rang 5** | – | – | – | `1.340 / 115 / 31` | `1.800 / 142 / 38` |
| **45** | **Frostmauler** | **Rang 5** | – | – | – | `1.650 / 170 / 35` | `2.250 / 210 / 44` |
| **46** | **Stormwyrmling** | **Rang 5** | – | – | – | `1.900 / 160 / 36` | `2.620 / 202 / 45` |
| **47** | **Cindercolossus** | **Rang 6** | – | – | – | – | `2.800 / 245 / 50` |
| **48** | **Galewyrm** | **Rang 6** | – | – | – | – | `2.900 / 215 / 53` |

---

## 5. Die 5 Gebiete & Spawn-Verteilung der 18 neuen Kreaturen (Nr. 31–48)

### 5.1 Funktionsweise von bScaleToZoneRank, ScaleToZoneChance, MaxRankCap & FixedRankOverride

* **`bScaleToZoneRank = true`:** Die Kreatur darf auf den `ZoneRank` (oder bis zu `MaxRankCap`) mitwachsen; `ScaleToZoneChance` legt den Anteil der voll mitgewachsenen Exemplare fest.
* **`bScaleToZoneRank = false`:** Die Kreatur wächst **nicht** mit und bleibt als unskalierter Nachzügler auf ihrem niedrigeren Rang (`FixedRankOverride` / `MinRank`) – mit `85 % – 115 %` Varianz auf genau diesem Rang.
* **`MaxRankCap`:** Deckelt den maximalen `EffectiveRank` nach oben (`0` = kein zusätzliches Cap oberhalb von `MinRank` außer `ZoneRank` und dem globalen Captain-Cap `25`).

### 5.2 Moosgeäst & Sumpf-Senken (*Wald, Unterholz & feuchte Senken* | ZoneRank 2 – 5+)

| Nr. | CreatureID | MinRank | ZoneRank-Beispiel | SpawnWeight | `bScaleToZoneRank` | `ScaleToZoneChance` | `MaxRankCap` | `FixedRankOverride` | Rolle & Kontrast im Gebiet |
| :---: | :--- | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :--- |
| **31** | **Bramblefang** | `2` | `2` | `1.25` | `true` | `1.0` | `0` | `0` | Regulärer Einstieg auf Rang 2 mit `85–115 %` Varianz. |
| **32** | **Siltstrider** | `2` | `4` | `0.75` | `false` | `0.0` | `2` | `2` | Unskalierter Rang-2-Boden-/Senkenläufer als leichter Kontrast in Rang-4-Zonen. |
| **34** | **Sporeback** | `3` | `4` | `1.05` | `true` | `0.85` | `0` | `0` | Wächst meist auf Rang 4 mit; Sporen-/Staubwolke bei `Poise`-Bruch. |
| **36** | **Gloomstalker** | `3` | `5` | `0.80` | `true` | `0.65` | `4` | `0` | Auf Rang 4 gedeckelt als unheimlicher Unterholz-/Schattenpirscher. |
| **41** | **Mossgolem** | `4` | `5` | `0.85` | `true` | `0.85` | `0` | `0` | Zäher moos- und wurzelgeprägter Wächter mit hohem `Poise` und klaren Erholungsfenstern. |

### 5.3 Bruchstein & Gewölbe-Spalten (*Ruinen, Hallen & verfallene Bauten* | ZoneRank 3 – 6+)

| Nr. | CreatureID | MinRank | ZoneRank-Beispiel | SpawnWeight | `bScaleToZoneRank` | `ScaleToZoneChance` | `MaxRankCap` | `FixedRankOverride` | Rolle & Kontrast im Gebiet |
| :---: | :--- | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :--- |
| **33** | **Cinderling** | `2` | `4` | `0.65` | `false` | `0.0` | `2` | `2` | Unskalierter Rang-2-Nachzügler in Ruinentrümmern; fällt leicht über `Poise`. |
| **35** | **Skitterclaw** | `3` | `4` | `1.15` | `true` | `0.85` | `0` | `0` | Flinker Mauer- und Felskrabbler in Gewölben und Spalten. |
| **36** | **Gloomstalker** | `3` | `5` | `0.95` | `true` | `0.80` | `0` | `0` | Lauernder Schattenpirscher in dunklen Gängen und zerfallenen Hallen. |
| **37** | **Charmaw** | `3` | `5` | `0.70` | `true` | `0.60` | `4` | `0` | Auf Rang 4 gedeckelter Kiefer-Beißer für enge Ruinendurchgänge. |
| **40** | **Vaultcrawler** | `4` | `5` | `0.90` | `true` | `0.85` | `0` | `0` | Klettert an Pfeilern und Gewölben; `Poise`-Bruch löst den Halt. |
| **41** | **Mossgolem** | `4` | `6` | `0.60` | `true` | `0.70` | `5` | `0` | Bis Rang 5 gedeckelter Wächter überwucherter Hallen. |
| **44** | **Ashweaver** | `5` | `6` | `0.55` | `true` | `0.80` | `0` | `0` | Spannt ruß-/aschegeprägte Netze in großen Gewölben und Ruinenhöfen. |
| **47** | **Cindercolossus** | `6` | `6` | `0.25` | `true` | `1.0` | `0` | `0` | Seltener Spitzen-Koloss ab Rang 6 (bis Captain-Cap 25) in großen Hallen. |

### 5.4 Glut-Fluren & Schlacke-Böden (*Asche-, Ruß- & Brandzonen* | ZoneRank 2 – 6+)

| Nr. | CreatureID | MinRank | ZoneRank-Beispiel | SpawnWeight | `bScaleToZoneRank` | `ScaleToZoneChance` | `MaxRankCap` | `FixedRankOverride` | Rolle & Kontrast im Gebiet |
| :---: | :--- | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :--- |
| **32** | **Siltstrider** | `2` | `4` | `0.60` | `false` | `0.0` | `2` | `2` | Bleibt auf Rang 2 als staubiger Bodenläufer inmitten heißer Zonen. |
| **33** | **Cinderling** | `2` | `3` | `1.30` | `true` | `0.85` | `3` | `0` | Flankiert auf Rang 2–3 in Asche- und Funkenwolken. |
| **37** | **Charmaw** | `3` | `5` | `1.00` | `true` | `0.85` | `0` | `0` | Zäher Glut-Beißer; `Poise`-Treffer lösen den Kieferdruck. |
| **39** | **Slagbrute** | `4` | `5` | `1.05` | `true` | `0.85` | `0` | `0` | Schwerer Schlacke-Schläger mit hoher Frontkruste und starkem Haltungswiderstand. |
| **44** | **Ashweaver** | `5` | `6` | `0.75` | `true` | `0.85` | `0` | `0` | Großer Asche- und Glut-Weber mit glimmenden Knoten und Staub-/Netz-Druck. |
| **47** | **Cindercolossus** | `6` | `6` | `0.35` | `true` | `1.0` | `0` | `0` | Spitzen-Koloss der Glut-Fluren ab Rang 6 (bis Captain-Cap 25). |

### 5.5 Schroffen-Grate & Frost-Kanten (*Gebirge, Klippen & kalte Höhen* | ZoneRank 3 – 6+)

| Nr. | CreatureID | MinRank | ZoneRank-Beispiel | SpawnWeight | `bScaleToZoneRank` | `ScaleToZoneChance` | `MaxRankCap` | `FixedRankOverride` | Rolle & Kontrast im Gebiet |
| :---: | :--- | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :--- |
| **35** | **Skitterclaw** | `3` | `4` | `0.70` | `false` | `0.0` | `3` | `3` | Unskalierter Rang-3-Felskrabbler an kalten Kanten als Kontrast-Gegner. |
| **38** | **Rimehound** | `4` | `5` | `1.05` | `true` | `0.85` | `0` | `0` | Kälte- und windgehärteter Flankenjäger; dämpft Ausdauer-Regeneration. |
| **39** | **Slagbrute** | `4` | `6` | `0.70` | `true` | `0.65` | `5` | `0` | Auf Rang 5 gedeckelter Schläger an rauen Hängen und Geröllstufen. |
| **43** | **Cragstalker** | `5` | `6` | `0.85` | `true` | `0.85` | `0` | `0` | Muskulöser Grat- und Klippenjäger mit weiten Sprüngen und hohem `Poise`. |
| **45** | **Frostmauler** | `5` | `6` | `0.75` | `true` | `0.85` | `0` | `0` | Massiger Frost- und Fels-Brecher mit wuchtigen Pranken-/Bodenstößen. |
| **46** | **Stormwyrmling** | `5` | `6` | `0.45` | `true` | `0.80` | `0` | `0` | Wendiger Sturm-Drache an exponierten Hochlagen und Felsvorsprüngen. |

### 5.6 Böen-Kämme & Sturm-Kuppen (*Sturm-, Wind- & offene Hochlagen* | ZoneRank 5 – 6+ bis Cap 25)

| Nr. | CreatureID | MinRank | ZoneRank-Beispiel | SpawnWeight | `bScaleToZoneRank` | `ScaleToZoneChance` | `MaxRankCap` | `FixedRankOverride` | Rolle & Kontrast im Gebiet |
| :---: | :--- | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :--- |
| **38** | **Rimehound** | `4` | `6` | `0.75` | `false` | `0.0` | `4` | `4` | Unskalierter Rang-4-Kältejäger in Sturm- und Frostböen. |
| **42** | **Galehound** | `5` | `6` | `1.05` | `true` | `0.90` | `0` | `0` | Schneller Sturm-Läufer mit sicht-/hörbarer Spannungsaufladung vor Stößen. |
| **43** | **Cragstalker** | `5` | `6` | `0.65` | `true` | `0.80` | `0` | `0` | Standfester Grat- und Kammjäger bei starkem Wind. |
| **45** | **Frostmauler** | `5` | `6` | `0.60` | `true` | `0.80` | `0` | `0` | Schwerer Brecher in eisigen Sturm-Kuppen. |
| **46** | **Stormwyrmling** | `5` | `6` | `0.55` | `true` | `0.85` | `0` | `0` | Wendiger Sturm-Drache für dynamische Luft-/Böen-Begegnungen. |
| **48** | **Galewyrm** | `6` | `6` (bis Cap 25) | `0.35` | `true` | `1.0` | `0` | `0` | Spitzen-Sturm-Drache ab Rang 6 (gedämpft bis Captain-Cap 25). |

### 5.7 JSON-Beispiel für die neuen Spawn-Regeln

```json
[
  {
    "CreatureID": "Siltstrider",
    "MinRank": 2,
    "SpawnWeight": 0.75,
    "bScaleToZoneRank": false,
    "ScaleToZoneChance": 0.0,
    "MaxRankCap": 2,
    "FixedRankOverride": 2
  },
  {
    "CreatureID": "Slagbrute",
    "MinRank": 4,
    "SpawnWeight": 1.05,
    "bScaleToZoneRank": true,
    "ScaleToZoneChance": 0.85,
    "MaxRankCap": 0,
    "FixedRankOverride": 0
  },
  {
    "CreatureID": "Galewyrm",
    "MinRank": 6,
    "SpawnWeight": 0.35,
    "bScaleToZoneRank": true,
    "ScaleToZoneChance": 1.0,
    "MaxRankCap": 0,
    "FixedRankOverride": 0
  }
]
```

---

## 6. Vollständiger Loot-Katalog aller 48 Kreaturen

### 6.1 Loot-Regeln, Zahlenformate & Biom-IDs

* **Spaltenformat:** `Item-Name (MinCount–MaxCount | DropChance | MinEffRank | DeltaScale)`
  * `DropChance`: Wahrscheinlichkeit von `0.00` bis `1.00` vor Nachzügler-Dämpfung.
  * `MinEffRank`: Mindest-`EffectiveRank`, ab dem dieser Eintrag gewürfelt werden darf.
  * `DeltaScale`: `Ja` = bei `Delta == 0` erhöht sich die Menge zusätzlich um `EffectiveRank - MinEffectiveRank`.
* **Die 5 Biom-IDs:**
  1. `Biome_Moosgeaest_Sumpfsenken` (*Moosgeäst*)
  2. `Biome_Bruchstein_Gewoelbespalten` (*Bruchstein*)
  3. `Biome_Glutfluren_Schlackeboeden` (*Glut-Fluren*)
  4. `Biome_Schroffengrate_Frostkanten` (*Schroffen-Grate*)
  5. `Biome_Boeenkaemme_Sturmkuppen` (*Böen-Kämme*)

### 6.2 Loot-Tabellen: Erste 21 eigene Kreaturen (Nr. 1–21)

| Nr. | Kreatur (`MinRank`) | Basis-Drops (`Min–Max` \| Chance \| MinEffRank \| DeltaScale) | Rang-Steigerungen (`Min–Max` \| Chance \| MinEffRank \| DeltaScale) | Biom-Zusatz-Drops (`BiomeID` -> Item, Menge, Chance) |
| :---: | :--- | :--- | :--- | :--- |
| **1** | **Vaelgorn** (`Rang 3`) | • Mineral-Krustensplitter (`1–2` \| `0.90` \| R3 \| Nein)<br>• Sehnenfasern (`1–2` \| `0.75` \| R3 \| Nein)<br>• Knochensplitter (`1–2` \| `0.65` \| R3 \| Nein) | • Dichtere Krustenplatten (`1–2` \| `0.60` \| R4 \| Ja)<br>• Verstärkte Sehnenstränge (`1–2` \| `0.50` \| R5 \| Ja) | • *Glut-Fluren:* Asche-Mineral (`1–2`, `0.50`)<br>• *Moosgeäst:* Faser-/Harzreste (`1–2`, `0.45`)<br>• *Schroffen-Grate / Bruchstein:* Gesteinssplitter (`1–2`, `0.45`) |
| **2** | **Cinderloom** (`Rang 4`) | • Verkohlte Faserstränge (`1–2` \| `0.85` \| R4 \| Nein)<br>• Kleine Glutknoten (`1–2` \| `0.75` \| R4 \| Nein)<br>• Ruinen-/Asche-Reste (`1–2` \| `0.65` \| R4 \| Nein) | • Zähe Hitze-Fasern (`1–2` \| `0.55` \| R5 \| Ja)<br>• Stabile Glutkapseln (`1–1` \| `0.35` \| R6 \| Ja) | • *Glut-Fluren:* Glut-Schlacke (`1`, `0.50`)<br>• *Bruchstein:* Alte Stoff-/Reliktfäden (`1–2`, `0.45`)<br>• *Moosgeäst:* Verkohltes Holz-/Faserstück (`1–2`, `0.40`) |
| **3** | **Osskarr** (`Rang 3`) | • Knochengelenke (`1–2` \| `0.85` \| R3 \| Nein)<br>• Trockene Sehnenbänder (`1–2` \| `0.75` \| R3 \| Nein)<br>• Grobe Gewebe-/Stofffetzen (`1–2` \| `0.65` \| R3 \| Nein) | • Gehärtete Knochenstreben (`1–2` \| `0.60` \| R4 \| Ja)<br>• Lange Sehnen (`1–2` \| `0.45` \| R5 \| Ja) | • *Bruchstein:* Verfall-Relikt (`1`, `0.45`)<br>• *Moosgeäst:* Waldfaser (`1–2`, `0.45`)<br>• *Schroffen-Grate:* Felsstaub-/Splitterreste (`1–2`, `0.40`) |
| **4** | **Morrowear** (`Rang 3`) | • Hör-/Schwingungsorgan (`1–1` \| `0.80` \| R3 \| Nein)<br>• Dicke stumpfe Haut (`1–2` \| `0.80` \| R3 \| Nein)<br>• Knorpelreste (`1–2` \| `0.65` \| R3 \| Nein) | • Ausgeprägtere Schwingungsorgane (`1–2` \| `0.55` \| R4 \| Ja)<br>• Zähere Häute (`1–2` \| `0.45` \| R6 \| Ja) | • *Moosgeäst:* Feuchtes Balg-/Lederstück (`1`, `0.50`)<br>• *Glut-Fluren / Bruchstein:* Staub-/Asche-Korn (`1–2`, `0.45`)<br>• *Schroffen-Grate:* Gebirgs-Knorpel (`1`, `0.40`) |
| **5** | **Pyroclast** (`Rang 4`) | • Schlacke-Panzerstücke (`1–2` \| `0.85` \| R4 \| Nein)<br>• Glühendes Gestein (`1–2` \| `0.75` \| R4 \| Nein)<br>• Schwere Grabklauen (`1–2` \| `0.65` \| R4 \| Nein) | • Dichte Schlackeplatten (`1–2` \| `0.55` \| R5 \| Ja)<br>• Hitzebeständige Kerne/Brocken (`1–1` \| `0.35` \| R6 \| Ja) | • *Glut-Fluren:* Schlacke-Kern (`1`, `0.50`)<br>• *Bruchstein:* Ruinen-Gestein (`1–2`, `0.45`)<br>• *Schroffen-Grate:* Geröll-Erzrest (`1`, `0.40`) |
| **6** | **Dravik** (`Rang 5`) | • Dicke Horn-/Schuppenplatten (`1–3` \| `0.90` \| R5 \| Nein)<br>• Grabklauen (`1–2` \| `0.75` \| R5 \| Nein)<br>• Zähe Dravik-Sehnen (`1–2` \| `0.70` \| R5 \| Nein) | • Schwere Spitzen-Schuppen (`1–2` \| `0.50` \| R6 \| Ja)<br>• Seltene Hochrang-Materialien (`1–1` \| `0.35` \| R6 \| Ja) | • *Glut-Fluren:* Asche-Horn (`1`, `0.50`)<br>• *Bruchstein / Schroffen-Grate:* Panzer-Schutt (`1–2`, `0.45`) |
| **7** | **Murghul** (`Rang 3`) | • Sichel-Greifklauen (`1–2` \| `0.85` \| R3 \| Nein)<br>• Chitin-Plattenstücke (`1–2` \| `0.75` \| R3 \| Nein)<br>• Hornstacheln (`1–2` \| `0.65` \| R3 \| Nein) | • Gehärtete Sichelklauen (`1–2` \| `0.60` \| R4 \| Ja)<br>• Dickere Chitinpanzer (`1–2` \| `0.45` \| R6 \| Ja) | • *Bruchstein:* Ruinen-Chitin (`1–2`, `0.50`)<br>• *Schroffen-Grate:* Fels-Stachel (`1`, `0.45`) |
| **8** | **Kraggar** (`Rang 4`) | • Fels-Klauen (`1–2` \| `0.85` \| R4 \| Nein)<br>• Granit-/Schiefer-Hautstücke (`1–2` \| `0.80` \| R4 \| Nein)<br>• Hornkamm-Splitter (`1–2` \| `0.65` \| R4 \| Nein) | • Massive Felsklauen (`1–2` \| `0.55` \| R5 \| Ja)<br>• Gebirgs-/Schmiede-Reste (`1–1` \| `0.35` \| R6 \| Ja) | • *Schroffen-Grate:* Hochgebirgs-Stein (`1–2`, `0.55`)<br>• *Böen-Kämme / Schroffen-Grate:* Kanten-Horn (`1`, `0.45`) |
| **9** | **Galeborn** (`Rang 5`) | • Sturm-Faserbänder (`1–3` \| `0.90` \| R5 \| Nein)<br>• Geladene Kernreste (`1–2` \| `0.70` \| R5 \| Nein)<br>• Hornspangen (`1–2` \| `0.70` \| R5 \| Nein) | • Dichte Sturm-Fasern (`1–2` \| `0.50` \| R6 \| Ja)<br>• Hochlagen-Komponenten (`1–1` \| `0.35` \| R6 \| Ja) | • *Böen-Kämme:* Sturm-Faser (`1–2`, `0.55`)<br>• *Bruchstein / Böen-Kämme:* Ruinen-Spange (`1`, `0.40`) |
| **10** | **Ashstalker** (`Rang 2`) | • Asche-Balg/Haut (`1–2` \| `0.85` \| R2 \| Nein)<br>• Dünne Sehnen (`1–2` \| `0.75` \| R2 \| Nein)<br>• Leichte Knochenreste (`1–1` \| `0.65` \| R2 \| Nein) | • Zähere Häute (`1–2` \| `0.65` \| R3 \| Ja)<br>• Stärkere Sehnen (`1–2` \| `0.45` \| R5 \| Ja) | • *Glut-Fluren:* Asche-Faser (`1–2`, `0.50`)<br>• *Moosgeäst:* Unterholz-Reste (`1–2`, `0.45`)<br>• *Bruchstein:* Trümmer-Staub (`1–2`, `0.40`) |
| **11** | **Thornmaw** (`Rang 2`) | • Dornen-/Horn-Splitter (`1–2` \| `0.85` \| R2 \| Nein)<br>• Verholzte Hautstücke (`1–2` \| `0.75` \| R2 \| Nein)<br>• Kiefer-/Knorpelreste (`1–1` \| `0.65` \| R2 \| Nein) | • Dickere Dorn-/Horn-Teile (`1–2` \| `0.65` \| R3 \| Ja)<br>• Zähere Häute (`1–2` \| `0.45` \| R5 \| Ja) | • *Moosgeäst:* Dornen-Holz (`1–2`, `0.50`)<br>• *Schroffen-Grate:* Gesteins-Horn (`1`, `0.40`) |
| **12** | **Dustmote** (`Rang 2`) | • Staubige Hautlappen (`1–2` \| `0.85` \| R2 \| Nein)<br>• Feine Fühler-/Faserreste (`1–2` \| `0.70` \| R2 \| Nein) | • Dichtere Faser-/Staub-Teile (`1–3` \| `0.60` \| R3 \| Ja) | • *Glut-Fluren / Bruchstein / Moosgeäst:* Feiner Staub-/Asche-Rest (`1–2`, `0.45`) |
| **13** | **Mirecrawler** (`Rang 2`) | • Zähe Feuchthaut (`1–2` \| `0.85` \| R2 \| Nein)<br>• Breite Greifpratzen (`1–2` \| `0.75` \| R2 \| Nein)<br>• Knorpelreste (`1–1` \| `0.65` \| R2 \| Nein) | • Dickere Häute (`1–2` \| `0.65` \| R3 \| Ja)<br>• Kräftigere Pratzen (`1–2` \| `0.45` \| R5 \| Ja) | • *Moosgeäst:* Feucht-Leder/Schlammrest (`1–2`, `0.50`) |
| **14** | **Cinderhound** (`Rang 3`) | • Verkohlte Faser-/Fellreste (`1–2` \| `0.85` \| R3 \| Nein)<br>• Glimmende Funken-/Glut-Reste (`1–2` \| `0.70` \| R3 \| Nein)<br>• Sehnen (`1–2` \| `0.70` \| R3 \| Nein) | • Zähere Brand-Häute (`1–2` \| `0.55` \| R4 \| Ja)<br>• Stärkere Sehnen & Glut-Reste (`1–2` \| `0.45` \| R6 \| Ja) | • *Glut-Fluren:* Glut-Funke (`1–2`, `0.50`)<br>• *Bruchstein / Glut-Fluren:* Asche-Sehne (`1`, `0.40`) |
| **15** | **Graveclaw** (`Rang 3`) | • Grabklauen (`1–2` \| `0.85` \| R3 \| Nein)<br>• Erd-/Schutt-Schulterstücke (`1–2` \| `0.75` \| R3 \| Nein)<br>• Knochenreste (`1–2` \| `0.65` \| R3 \| Nein) | • Schwere Grabklauen (`1–2` \| `0.55` \| R4 \| Ja)<br>• Gehärtete Schulterplatten (`1–2` \| `0.45` \| R6 \| Ja) | • *Bruchstein / Glut-Fluren / Schroffen-Grate:* Grab-Erz/Schuttrest (`1–2`, `0.45`) |
| **16** | **Rimestalker** (`Rang 4`) | • Kalt-graue Hautstücke (`1–2` \| `0.85` \| R4 \| Nein)<br>• Scharfe Glieder-/Hornspitzen (`1–2` \| `0.75` \| R4 \| Nein)<br>• Sehnen (`1–2` \| `0.70` \| R4 \| Nein) | • Dichte Kälte-Häute (`1–2` \| `0.55` \| R5 \| Ja)<br>• Hochlagen-Teile (`1–1` \| `0.35` \| R6 \| Ja) | • *Schroffen-Grate:* Frost-Splitter (`1–2`, `0.55`) |
| **17** | **Hollowback** (`Rang 4`) | • Ausgehöhlte Rücken-/Rumpfstücke (`1–2` \| `0.85` \| R4 \| Nein)<br>• Staub-/Sporen-/Ruß-Reste (`1–2` \| `0.75` \| R4 \| Nein)<br>• Sehnen (`1–2` \| `0.65` \| R4 \| Nein) | • Gehärtete Rumpfstrukturen (`1–2` \| `0.55` \| R5 \| Ja)<br>• Dichtere Wolken-/Materialreste (`1–2` \| `0.40` \| R6 \| Ja) | • *Glut-Fluren / Bruchstein / Moosgeäst:* Umgebungs-Kapsel/Rest (`1`, `0.45`) |
| **18** | **Ironhide** (`Rang 4`) | • Eisen-/Krusten-Panzerplatten (`1–2` \| `0.85` \| R4 \| Nein)<br>• Schwere Hornreste (`1–2` \| `0.75` \| R4 \| Nein)<br>• Dicke Sehnen (`1–2` \| `0.65` \| R4 \| Nein) | • Massive Panzerplatten (`1–2` \| `0.55` \| R5 \| Ja)<br>• Metall-/Mineral-Reste (`1–2` \| `0.35` \| R6 \| Ja) | • *Schroffen-Grate / Bruchstein / Glut-Fluren:* Erz-/Eisen-Kruste (`1–2`, `0.45`) |
| **19** | **Shroudwalker** (`Rang 4`) | • Dunkle Faser-/Gewebebahnen (`1–2` \| `0.85` \| R4 \| Nein)<br>• Glimmende Kernreste (`1–1` \| `0.70` \| R4 \| Nein)<br>• Lange Sehnen (`1–2` \| `0.70` \| R4 \| Nein) | • Dichtere Schatten-/Faserbahnen (`1–2` \| `0.55` \| R5 \| Ja)<br>• Relikt-Teile (`1–1` \| `0.35` \| R6 \| Ja) | • *Bruchstein:* Relikt-Faser (`1–2`, `0.50`) |
| **20** | **Cragmaw** (`Rang 5`) | • Zackige Kiefer-/Horn-Teile (`1–2` \| `0.85` \| R5 \| Nein)<br>• Steinkamm-Stücke (`1–2` \| `0.80` \| R5 \| Nein)<br>• Schwere Vorderlauf-Reste (`1–2` \| `0.70` \| R5 \| Nein) | • Große Kiefer-/Horn-Trophäen (`1–2` \| `0.50` \| R6 \| Ja)<br>• Seltene Rohstoffe (`1–1` \| `0.35` \| R6 \| Ja) | • *Schroffen-Grate / Bruchstein:* Fels-Kieferrest (`1–2`, `0.50`) |
| **21** | **Dreadcolossus** (`Rang 6`) | • Massive Stein-/Mineral-/Relikt-Teile (`2–3` \| `0.95` \| R6 \| Nein)<br>• Koloss-Knochen/Sehnen (`2–3` \| `0.85` \| R6 \| Nein)<br>• Spitzen-Materialien (`1–2` \| `0.70` \| R6 \| Nein) | • Seltener Koloss-Spitzenrest (`1–2` \| `0.45` \| R6 \| Ja) | • *Alle passenden Gebiete:* Seltener Koloss-Kern/Rest (`1–2`, `0.55`) |

### 6.3 Loot-Tabellen: Klassische Säulen & Drachen (Nr. 22–30)

| Nr. | Kreatur (`MinRank`) | Basis-Drops (`Min–Max` \| Chance \| MinEffRank \| DeltaScale) | Rang-Steigerungen (`Min–Max` \| Chance \| MinEffRank \| DeltaScale) | Biom-Zusatz-Drops (`BiomeID` -> Item, Menge, Chance) |
| :---: | :--- | :--- | :--- | :--- |
| **22** | **Troll** (`Rang 4`) | • Dicke Trollhaut (`1–2` \| `0.90` \| R4 \| Nein)<br>• Grobe Knochen (`1–2` \| `0.80` \| R4 \| Nein)<br>• Zähe Sehnen (`1–2` \| `0.70` \| R4 \| Nein) | • Besonders dicke Häute (`1–2` \| `0.55` \| R5 \| Ja)<br>• Schwere Knochen (`1–2` \| `0.40` \| R6 \| Ja) | • *Moosgeäst:* Moos-/Erd-Balg (`1–2`, `0.55`)<br>• *Glut-Fluren / Bruchstein:* Grobe Schlacke-/Steinreste (`1–2`, `0.45`) |
| **23** | **Riese** (`Rang 5`) | • Große Knochen-/Sehnenstücke (`2–3` \| `0.90` \| R5 \| Nein)<br>• Schwere Rüstungs-/Reliktreste (`1–2` \| `0.75` \| R5 \| Nein)<br>• Seltene Rohstoffe (`1–1` \| `0.60` \| R5 \| Nein) | • Kolossale Knochen-/Sehnen-Teile (`1–2` \| `0.50` \| R6 \| Ja)<br>• Spitzen-Relikte (`1–1` \| `0.35` \| R6 \| Ja) | • *Schroffen-Grate / Böen-Kämme:* Uralter Fels-/Mineralrest (`1–2`, `0.50`)<br>• *Bruchstein:* Altes Metall-/Reliktsück (`1–2`, `0.45`) |
| **24** | **Wiedergänger** (`Rang 3`) | • Alte Ausrüstungs-/Metallreste (`1–2` \| `0.85` \| R3 \| Nein)<br>• Knochen-/Sehnenmaterial (`1–2` \| `0.75` \| R3 \| Nein)<br>• Verfall-Reste (`1–2` \| `0.65` \| R3 \| Nein) | • Bessere Metall-/Waffen-/Rüstungsfragmente (`1–2` \| `0.55` \| R4 \| Ja)<br>• Seltenere Relikte (`1–1` \| `0.35` \| R6 \| Ja) | • *Bruchstein:* Altes Eisenstück (`1–2`, `0.50`)<br>• *Glut-Fluren:* Asche-Relikt (`1`, `0.45`)<br>• *Moosgeäst:* Modriges Gewebe-/Relikt (`1`, `0.40`) |
| **25** | **Cinderwyrm** (`Rang 5`) | • Glut-Schuppen (`1–3` \| `0.90` \| R5 \| Nein)<br>• Schlacke-Platten (`1–2` \| `0.80` \| R5 \| Nein)<br>• Zähe Drachensehnen (`1–2` \| `0.75` \| R5 \| Nein)<br>• Hitzebeständige Knochen/Hörner (`1–2` \| `0.65` \| R5 \| Nein) | • Hochwertige Glut-Schuppen (`1–2` \| `0.50` \| R6 \| Ja)<br>• Spitzen-Komponenten (`1–1` \| `0.35` \| R6 \| Ja) | • *Glut-Fluren:* Glut-Schuppe (`1–2`, `0.55`)<br>• *Bruchstein / Schroffen-Grate:* Hitze-Horn (`1`, `0.45`) |
| **26** | **Cragdrake** (`Rang 5`) | • Granit-/Fels-Schuppen (`1–3` \| `0.90` \| R5 \| Nein)<br>• Klammerkrallen (`1–2` \| `0.80` \| R5 \| Nein)<br>• Dicker Drachenknorpel/Sehnen (`1–2` \| `0.75` \| R5 \| Nein) | • Schwere Berg-Schuppen (`1–2` \| `0.50` \| R6 \| Ja)<br>• Hochgebirgs-/Drachen-Werkstoffe (`1–1` \| `0.35` \| R6 \| Ja) | • *Schroffen-Grate / Böen-Kämme:* Granit-Schuppe (`1–2`, `0.55`)<br>• *Bruchstein / Schroffen-Grate:* Fels-Klaue (`1`, `0.45`) |
| **27** | **Stormmaw** (`Rang 6`) | • Sturm-Schwingenhaut (`2–3` \| `0.95` \| R6 \| Nein)<br>• Zähe Drachensehnen (`2–3` \| `0.85` \| R6 \| Nein)<br>• Gebogene Hörner/Zähne (`1–2` \| `0.75` \| R6 \| Nein) | • Seltene Sturm-/Relikt-Materialien (`1–2` \| `0.45` \| R6 \| Ja) | • *Böen-Kämme:* Sturm-Schwingenhaut (`1–2`, `0.60`)<br>• *Bruchstein / Böen-Kämme:* Sturm-Horn (`1`, `0.45`) |
| **28** | **Ashwing** (`Rang 5`) | • Asche-Schwingenhaut (`1–3` \| `0.90` \| R5 \| Nein)<br>• Zähe Drachensehnen (`1–2` \| `0.75` \| R5 \| Nein)<br>• Staubige Horn-/Knochenreste (`1–2` \| `0.70` \| R5 \| Nein) | • Hochwertige Gleit-Schwingenhäute (`1–2` \| `0.50` \| R6 \| Ja)<br>• Asche-/Drachen-Teile (`1–1` \| `0.35` \| R6 \| Ja) | • *Glut-Fluren:* Asche-Haut (`1–2`, `0.55`)<br>• *Bruchstein / Glut-Fluren:* Staub-Klaue (`1`, `0.45`) |
| **29** | **Frostdrake** (`Rang 5`) | • Frost-/Schiefer-Schuppen (`1–3` \| `0.90` \| R5 \| Nein)<br>• Drachensehnen (`1–2` \| `0.75` \| R5 \| Nein)<br>• Kalte Horn-/Knochenreste (`1–2` \| `0.70` \| R5 \| Nein) | • Gehärtete Frost-Schuppen (`1–2` \| `0.50` \| R6 \| Ja)<br>• Hochlagen-Komponenten (`1–1` \| `0.35` \| R6 \| Ja) | • *Schroffen-Grate / Böen-Kämme:* Frost-Schuppe (`1–2`, `0.55`)<br>• *Schroffen-Grate / Bruchstein:* Kalt-Horn (`1`, `0.45`) |
| **30** | **Dreadwyrm** (`Rang 6`) | • Spitzen-Drachenschuppen (`2–3` \| `0.95` \| R6 \| Nein)<br>• Massive Sehnen (`2–3` \| `0.85` \| R6 \| Nein)<br>• Uralte Knochen-/Horn- & Relikt-Teile (`1–2` \| `0.80` \| R6 \| Nein) | • Seltene Wyrm-Spitzenrelikte (`1–2` \| `0.45` \| R6 \| Ja) | • *Alle passenden Gebiete:* Uralte Drachen-Reliktreste (`1–2`, `0.60`) |

### 6.4 Loot-Tabellen: 18 neue eigene Kreaturen (Nr. 31–48)

| Nr. | Kreatur (`MinRank`) | Basis-Drops (`Min–Max` \| Chance \| MinEffRank \| DeltaScale) | Rang-Steigerungen (`Min–Max` \| Chance \| MinEffRank \| DeltaScale) | Biom-Zusatz-Drops (`BiomeID` -> Item, Menge, Chance) |
| :---: | :--- | :--- | :--- | :--- |
| **31** | **Bramblefang** (`Rang 2`) | • Dornen-Balg (`1–2` \| `0.90` \| R2 \| Nein)<br>• Scharfe Fangzähne (`1–2` \| `0.75` \| R2 \| Nein)<br>• Dünne Sehnen (`1–2` \| `0.70` \| R2 \| Nein) | • Gehärteter Dornen-Balg (`1–2` \| `0.65` \| R3 \| Ja)<br>• Verstärkte Fangzähne (`1–2` \| `0.45` \| R5 \| Ja) | • *Moosgeäst:* Moosfaser (`1–2`, `0.55`), Dornen-Holz (`1`, `0.45`)<br>• *Bruchstein:* Verfall-Relikt (`1`, `0.35`), Ruinen-Staubrest (`1–2`, `0.40`) |
| **32** | **Siltstrider** (`Rang 2`) | • Trockene Laufhaut (`1–2` \| `0.85` \| R2 \| Nein)<br>• Lange Laufsehnen (`1–2` \| `0.75` \| R2 \| Nein)<br>• Leichte Knochenreste (`1–1` \| `0.65` \| R2 \| Nein) | • Zähere Laufhaut & längere Sehnen (`1–2` \| `0.65` \| R3 \| Ja)<br>• Gehärtete Laufknochen & dichtere Fasern (`1–2` \| `0.45` \| R5 \| Ja) | • *Moosgeäst:* Feucht-Leder (`1`, `0.50`), Unterholz-Faser (`1–2`, `0.45`)<br>• *Glut-Fluren:* Asche-Mineral (`1`, `0.45`), Ruß-Balgrest (`1–2`, `0.40`) |
| **33** | **Cinderling** (`Rang 2`) | • Funken-Asche (`1–3` \| `0.90` \| R2 \| Nein)<br>• Verkohlte Faser (`1–2` \| `0.70` \| R2 \| Nein)<br>• Glimmender Splitter (`1–1` \| `0.45` \| R2 \| Nein) | • Dichtere Funken-Asche & zähere Fasern (`1–2` \| `0.65` \| R3 \| Ja)<br>• Glimmende Glutkapsel (`1–1` \| `0.40` \| R5 \| Ja) | • *Glut-Fluren:* Schlacke-Kern (`1`, `0.40`), Glut-Schuppe (`1`, `0.30`)<br>• *Bruchstein:* Altes Eisenstück (`1`, `0.35`), Ruinen-Rußrest (`1–2`, `0.45`) |
| **34** | **Sporeback** (`Rang 3`) | • Sporenkappen-Rest (`1–2` \| `0.85` \| R3 \| Nein)<br>• Buckelige Rumpfhaut (`1–2` \| `0.75` \| R3 \| Nein)<br>• Zähe Sehnen (`1–1` \| `0.65` \| R3 \| Nein) | • Konzentrierte Sporenkapsel & gehärtete Rumpfhaut (`1–2` \| `0.50` \| R4 \| Ja)<br>• Seltene Sporen-Essenz (`1–1` \| `0.30` \| R6 \| Ja) | • *Moosgeäst:* Moosfaser (`1–2`, `0.55`), Feucht-Leder (`1`, `0.45`)<br>• *Bruchstein:* Verfall-Relikt (`1`, `0.40`), Kalk-/Mauerstaub-Kapsel (`1`, `0.35`) |
| **35** | **Skitterclaw** (`Rang 3`) | • Klammer-Chitin (`1–2` \| `0.85` \| R3 \| Nein)<br>• Scharfe Krallenspitzen (`1–2` \| `0.75` \| R3 \| Nein)<br>• Dünne Gliedsehnen (`1–2` \| `0.65` \| R3 \| Nein) | • Gehärtetes Klammer-Chitin & dickere Krallen (`1–2` \| `0.60` \| R4 \| Ja)<br>• Seltene Chitin-Platte (`1–1` \| `0.35` \| R6 \| Ja) | • *Bruchstein:* Ruinen-Chitin (`1–2`, `0.50`), Altes Eisenstück (`1`, `0.35`)<br>• *Schroffen-Grate:* Hochgebirgs-Stein (`1`, `0.45`), Granit-Schuppe (`1`, `0.30`) |
| **36** | **Gloomstalker** (`Rang 3`) | • Schatten-Gewebefetzen (`1–2` \| `0.85` \| R3 \| Nein)<br>• Gedämpfte Sehnen (`1–2` \| `0.70` \| R3 \| Nein)<br>• Dunkle Knochensplitter (`1–1` \| `0.60` \| R3 \| Nein) | • Dichtere Schatten-Bahnen & stärkere Sehnen (`1–2` \| `0.60` \| R4 \| Ja)<br>• Glimmender Schattenkern (`1–1` \| `0.30` \| R6 \| Ja) | • *Bruchstein:* Verfall-Relikt (`1`, `0.45`), Ruinen-Chitin (`1`, `0.35`)<br>• *Moosgeäst:* Moosfaser (`1–2`, `0.50`), Dornen-Holz (`1`, `0.35`) |
| **37** | **Charmaw** (`Rang 3`) | • Verkohlter Kieferknochen (`1–1` \| `0.80` \| R3 \| Nein)<br>• Brand-Sehnen (`1–2` \| `0.75` \| R3 \| Nein)<br>• Rußige Kieferzähne (`1–2` \| `0.70` \| R3 \| Nein) | • Gehärteter Kieferknochen & glimmende Zahnreste (`1–2` \| `0.55` \| R4 \| Ja)<br>• Schwerer Glut-Kiefer (`1–1` \| `0.35` \| R6 \| Ja) | • *Glut-Fluren:* Glut-Schuppe (`1`, `0.45`), Asche-Mineral (`1–2`, `0.50`)<br>• *Bruchstein:* Altes Eisenstück (`1`, `0.40`), Verfall-Relikt (`1`, `0.35`) |
| **38** | **Rimehound** (`Rang 4`) | • Kalt-borstiges Fell (`1–2` \| `0.85` \| R4 \| Nein)<br>• Frost-Sehnen (`1–2` \| `0.75` \| R4 \| Nein)<br>• Raue Fangzähne (`1–2` \| `0.65` \| R4 \| Nein) | • Dichtes Hochlagen-Fell & kristalline Sehnen (`1–2` \| `0.55` \| R5 \| Ja)<br>• Frost-Kernstück (`1–1` \| `0.35` \| R6 \| Ja) | • *Schroffen-Grate:* Frost-Splitter (`1–2`, `0.55`), Granit-Schuppe (`1`, `0.40`)<br>• *Böen-Kämme:* Sturm-Faser (`1`, `0.45`), Frost-Splitter (`1`, `0.40`) |
| **39** | **Slagbrute** (`Rang 4`) | • Schlacke-Frontkruste (`1–2` \| `0.85` \| R4 \| Nein)<br>• Grobe Gesteinsbrocken (`1–3` \| `0.80` \| R4 \| Nein)<br>• Schwere Sehnen (`1–2` \| `0.65` \| R4 \| Nein) | • Dichte Schlacke-Platte & gehärtete Gesteinsbrocken (`1–2` \| `0.50` \| R5 \| Ja)<br>• Glühender Schlacke-Kern (`1–1` \| `0.35` \| R6 \| Ja) | • *Glut-Fluren:* Schlacke-Kern (`1`, `0.50`), Asche-Mineral (`1–2`, `0.50`)<br>• *Schroffen-Grate:* Hochgebirgs-Stein (`1–2`, `0.50`), Granit-Schuppe (`1`, `0.35`) |
| **40** | **Vaultcrawler** (`Rang 4`) | • Gewölbe-Panzerplatten (`1–2` \| `0.85` \| R4 \| Nein)<br>• Greifarm-Gelenke (`1–2` \| `0.75` \| R4 \| Nein)<br>• Zähe Sehnen (`1–2` \| `0.65` \| R4 \| Nein) | • Verstärkte Gewölbe-Platten & gehärtete Greifgelenke (`1–2` \| `0.55` \| R5 \| Ja)<br>• Seltene Gewölbe-Reliktplatte (`1–1` \| `0.35` \| R6 \| Ja) | • *Bruchstein:* Ruinen-Chitin (`1–2`, `0.55`), Altes Eisenstück (`1`, `0.45`), Verfall-Relikt (`1`, `0.40`) |
| **41** | **Mossgolem** (`Rang 4`) | • Verwachsene Wurzel-/Steinbrocken (`1–3` \| `0.90` \| R4 \| Nein)<br>• Moosdurchsetzte Rinde (`1–2` \| `0.80` \| R4 \| Nein)<br>• Zähes Bindematerial (`1–2` \| `0.65` \| R4 \| Nein) | • Uralte Wurzel-/Steinkerne & dichtere Rindenplatten (`1–2` \| `0.45` \| R5 \| Ja)<br>• Seltener Wächter-Kern (`1–1` \| `0.30` \| R6 \| Ja) | • *Moosgeäst:* Moosfaser (`2–3`, `0.65`), Dornen-Holz (`1–2`, `0.50`)<br>• *Bruchstein:* Verfall-Relikt (`1`, `0.40`), Ruinen-Chitin (`1`, `0.35`) |
| **42** | **Galehound** (`Rang 5`) | • Sturm-Borstenfell (`1–2` \| `0.85` \| R5 \| Nein)<br>• Gespannte Böen-Sehnen (`1–2` \| `0.75` \| R5 \| Nein)<br>• Flackernde Funken-/Spannungsreste (`1–1` \| `0.55` \| R5 \| Nein) | • Geladener Kernrest & hochgespannte Sehnen (`1–2` \| `0.45` \| R6 \| Ja) | • *Böen-Kämme:* Sturm-Faser (`1–2`, `0.55`), Geladener Kernrest (`1`, `0.40`)<br>• *Schroffen-Grate:* Frost-Splitter (`1`, `0.40`), Hochgebirgs-Stein (`1`, `0.40`) |
| **43** | **Cragstalker** (`Rang 5`) | • Grat-Schuppenhaut (`1–2` \| `0.85` \| R5 \| Nein)<br>• Kräftige Sprungsehnen (`1–2` \| `0.75` \| R5 \| Nein)<br>• Gebogene Kammhörner (`1–2` \| `0.65` \| R5 \| Nein) | • Gehärtete Grat-Schuppen & seltene Hochgebirgs-Trophäe (`1–2` \| `0.35` \| R6 \| Ja) | • *Schroffen-Grate:* Granit-Schuppe (`1–2`, `0.55`), Hochgebirgs-Stein (`1–2`, `0.50`)<br>• *Böen-Kämme:* Sturm-Faser (`1`, `0.45`), Granit-Schuppe (`1`, `0.40`) |
| **44** | **Ashweaver** (`Rang 5`) | • Zähe Asche-Faserstränge (`2–3` \| `0.90` \| R5 \| Nein)<br>• Glimmende Glutknoten (`1–2` \| `0.70` \| R5 \| Nein)<br>• Verkohlte Chitin-/Faserreste (`1–2` \| `0.65` \| R5 \| Nein) | • Hochfester Glut-Faserstrang (`1–2` \| `0.50` \| R6 \| Ja)<br>• Seltene Glutkapsel (`1–1` \| `0.35` \| R6 \| Ja) | • *Glut-Fluren:* Glut-Schuppe (`1`, `0.45`), Schlacke-Kern (`1`, `0.45`), Asche-Mineral (`1–2`, `0.50`)<br>• *Bruchstein:* Verfall-Relikt (`1`, `0.40`), Ruinen-Chitin (`1`, `0.40`) |
| **45** | **Frostmauler** (`Rang 5`) | • Dicke Frost-Schuppenbrocken (`1–2` \| `0.85` \| R5 \| Nein)<br>• Massive Prankenknochen (`1–2` \| `0.75` \| R5 \| Nein)<br>• Zähe Sehnen (`1–2` \| `0.65` \| R5 \| Nein) | • Gehärteter Frost-Panzer (`1–2` \| `0.45` \| R6 \| Ja)<br>• Seltener Hochlagen-Knochen (`1–1` \| `0.35` \| R6 \| Ja) | • *Schroffen-Grate:* Frost-Splitter (`1–2`, `0.60`), Granit-Schuppe (`1–2`, `0.50`)<br>• *Böen-Kämme:* Frost-Splitter (`1`, `0.45`), Sturm-Faser (`1`, `0.40`) |
| **46** | **Stormwyrmling** (`Rang 5`) | • Junge Sturm-Schwingenhaut (`1–2` \| `0.85` \| R5 \| Nein)<br>• Elastische Drachensehnen (`1–2` \| `0.80` \| R5 \| Nein)<br>• Schmale Drachenhörner (`1–2` \| `0.65` \| R5 \| Nein) | • Verdichtete Sturm-Schwingenhaut (`1–2` \| `0.50` \| R6 \| Ja)<br>• Geladener Drachenkernrest (`1–1` \| `0.35` \| R6 \| Ja) | • *Böen-Kämme:* Sturm-Schwingenhaut (`1`, `0.50`), Sturm-Faser (`1–2`, `0.50`), Geladener Kernrest (`1`, `0.35`)<br>• *Schroffen-Grate:* Granit-Schuppe (`1`, `0.40`), Frost-Splitter (`1`, `0.40`) |
| **47** | **Cindercolossus** (`Rang 6`) | • Massive Schlacke-Kolossplatte (`2–3` \| `0.95` \| R6 \| Nein)<br>• Glühender Koloss-Kern (`1–2` \| `0.75` \| R6 \| Nein)<br>• Schwere Gesteins-/Knochenmasse (`2–3` \| `0.80` \| R6 \| Nein) | • Seltener Spitzen-Schlacke-Relikt (`1–2` \| `0.45` \| R6 \| Ja) | • *Glut-Fluren:* Schlacke-Kern (`1–2`, `0.60`), Glut-Schuppe (`1–2`, `0.55`)<br>• *Bruchstein:* Verfall-Relikt (`1–2`, `0.50`), Altes Eisenstück (`1–2`, `0.50`) |
| **48** | **Galewyrm** (`Rang 6`) | • Spitzen-Sturm-Schwingenhaut (`2–3` \| `0.95` \| R6 \| Nein)<br>• Massive Drachensehnen (`2–3` \| `0.85` \| R6 \| Nein)<br>• Gebogene Sturm-Drachenhörner (`1–2` \| `0.75` \| R6 \| Nein) | • Seltener Sturm-Wyrm-Kern (`1–2` \| `0.45` \| R6 \| Ja) | • *Böen-Kämme:* Sturm-Schwingenhaut (`1–2`, `0.65`), Geladener Kernrest (`1–2`, `0.55`), Sturm-Faser (`2–3`, `0.60`) |

### 6.5 JSON-Strukturbeispiele für DT_CreatureLoot (Nr. 31, Nr. 39 & Nr. 48)

```json
[
  {
    "Name": "Bramblefang",
    "CreatureID": "Bramblefang",
    "MinRank": 2,
    "BaseDrops": [
      { "ItemID": "Mat_DornenBalg", "MinCount": 1, "MaxCount": 2, "DropChance": 0.90, "MinEffectiveRank": 2, "bScaleCountByRankDelta": false },
      { "ItemID": "Mat_Fangzahn", "MinCount": 1, "MaxCount": 2, "DropChance": 0.75, "MinEffectiveRank": 2, "bScaleCountByRankDelta": false },
      { "ItemID": "Mat_DuennSehne", "MinCount": 1, "MaxCount": 2, "DropChance": 0.70, "MinEffectiveRank": 2, "bScaleCountByRankDelta": false }
    ],
    "RankEscalationDrops": [
      { "ItemID": "Mat_DornenBalg_Haertet", "MinCount": 1, "MaxCount": 2, "DropChance": 0.65, "MinEffectiveRank": 3, "bScaleCountByRankDelta": true },
      { "ItemID": "Mat_Fangzahn_Stark", "MinCount": 1, "MaxCount": 2, "DropChance": 0.45, "MinEffectiveRank": 5, "bScaleCountByRankDelta": true }
    ],
    "BiomeOverrides": [
      {
        "BiomeID": "Biome_Moosgeaest_Sumpfsenken",
        "BonusLootEntries": [
          { "ItemID": "Mat_Moosfaser", "MinCount": 1, "MaxCount": 2, "DropChance": 0.55, "MinEffectiveRank": 2, "bScaleCountByRankDelta": false },
          { "ItemID": "Mat_DornenHolz", "MinCount": 1, "MaxCount": 1, "DropChance": 0.45, "MinEffectiveRank": 2, "bScaleCountByRankDelta": false }
        ]
      },
      {
        "BiomeID": "Biome_Bruchstein_Gewoelbespalten",
        "BonusLootEntries": [
          { "ItemID": "Rel_VerfallRelikt", "MinCount": 1, "MaxCount": 1, "DropChance": 0.35, "MinEffectiveRank": 2, "bScaleCountByRankDelta": false }
        ]
      }
    ]
  },
  {
    "Name": "Slagbrute",
    "CreatureID": "Slagbrute",
    "MinRank": 4,
    "BaseDrops": [
      { "ItemID": "Mat_SchlackeKruste", "MinCount": 1, "MaxCount": 2, "DropChance": 0.85, "MinEffectiveRank": 4, "bScaleCountByRankDelta": false },
      { "ItemID": "Mat_Gesteinsbrocken", "MinCount": 1, "MaxCount": 3, "DropChance": 0.80, "MinEffectiveRank": 4, "bScaleCountByRankDelta": false }
    ],
    "RankEscalationDrops": [
      { "ItemID": "Mat_SchlackePlatte_Dicht", "MinCount": 1, "MaxCount": 1, "DropChance": 0.50, "MinEffectiveRank": 5, "bScaleCountByRankDelta": false },
      { "ItemID": "Mat_SchlackeKern_Gluehend", "MinCount": 1, "MaxCount": 1, "DropChance": 0.35, "MinEffectiveRank": 6, "bScaleCountByRankDelta": true }
    ],
    "BiomeOverrides": [
      {
        "BiomeID": "Biome_Glutfluren_Schlackeboeden",
        "BonusLootEntries": [
          { "ItemID": "Mat_SchlackeKern", "MinCount": 1, "MaxCount": 1, "DropChance": 0.50, "MinEffectiveRank": 4, "bScaleCountByRankDelta": false },
          { "ItemID": "Mat_AscheMineral", "MinCount": 1, "MaxCount": 2, "DropChance": 0.50, "MinEffectiveRank": 4, "bScaleCountByRankDelta": false }
        ]
      },
      {
        "BiomeID": "Biome_Schroffengrate_Frostkanten",
        "BonusLootEntries": [
          { "ItemID": "Mat_HochgebirgsStein", "MinCount": 1, "MaxCount": 2, "DropChance": 0.50, "MinEffectiveRank": 4, "bScaleCountByRankDelta": false }
        ]
      }
    ]
  },
  {
    "Name": "Galewyrm",
    "CreatureID": "Galewyrm",
    "MinRank": 6,
    "BaseDrops": [
      { "ItemID": "Mat_SturmSchwingenhaut_Spitze", "MinCount": 2, "MaxCount": 3, "DropChance": 0.95, "MinEffectiveRank": 6, "bScaleCountByRankDelta": false },
      { "ItemID": "Mat_Drachensehne_Massiv", "MinCount": 2, "MaxCount": 3, "DropChance": 0.85, "MinEffectiveRank": 6, "bScaleCountByRankDelta": false },
      { "ItemID": "Mat_SturmDrachenhorn", "MinCount": 1, "MaxCount": 2, "DropChance": 0.75, "MinEffectiveRank": 6, "bScaleCountByRankDelta": false }
    ],
    "RankEscalationDrops": [
      { "ItemID": "Rel_SturmWyrmKern", "MinCount": 1, "MaxCount": 1, "DropChance": 0.45, "MinEffectiveRank": 6, "bScaleCountByRankDelta": true }
    ],
    "BiomeOverrides": [
      {
        "BiomeID": "Biome_Boeenkaemme_Sturmkuppen",
        "BonusLootEntries": [
          { "ItemID": "Mat_SturmSchwingenhaut", "MinCount": 1, "MaxCount": 2, "DropChance": 0.65, "MinEffectiveRank": 6, "bScaleCountByRankDelta": false },
          { "ItemID": "Mat_GeladenerKernrest", "MinCount": 1, "MaxCount": 2, "DropChance": 0.55, "MinEffectiveRank": 6, "bScaleCountByRankDelta": false }
        ]
      }
    ]
  }
]
```

---

## 7. Item-Kategorien, Stats, Verbauung & Alternativquellen (Kreaturen-Loot)

### 7.1 Überblick der Item-Kategorien

* **Rüstung & Haltung (`Rüstung / PoiseRes`):** Häute, Bälge, Schuppen, Chitin- und Schlackeplatten – liefern Rüstungswerte, Haltungswiderstand (`PoiseResist`) oder Umwelt-Resistenzen (Hitze, Frost, Sturm, Verfall).
* **Waffen & Haltungsschaden (`Waffen / PoiseDmg`):** Zähne, Klauen, Hörner, Stacheln, Spangen und harte Knochen – fließen in Klingen, Spitzen, Hämmer, Griffe oder Haltungs-Modifikatoren ein.
* **Fasern, Sehnen & Bindung (`Handwerk / Bindung & Ausdauer`):** Sehnen, Fasern, Gewebebahnen, Wurzeln – verbessern Zugkraft, Flexibilität, Bogen-/Waffenbindungen, Ausdauer-Handling oder leichte Ausrüstung.
* **Kerne, Funken & Elemente (`Veredelung / Hitze, Frost, Sturm & Sporen`):** Glutknoten, Schlackekerne, Frost-Splitter, geladene Kernreste, Sporenkapseln – liefern Element-/Umwelt-Eigenschaften, Funken-/Glüheffekte, Licht-/Kälte-/Sturm-Modifikatoren oder Spezial-Tränke/Rationen.
* **Essen, Rationen & Alchemie (`Verbrauchsgüter / Essen & Tränke`):** Verwertbare organische/knorpelige/siedbare Teile (als Zutaten für Feld-Rationen, Brühen, Stärkungen oder Ausdauer-/Regenerations-Tränke).
* **Relikte, Alte Reste & Quest (`Quest / Relikte & Bau`):** Verfall-Relikte, alte Eisenstücke, Ruinen-Reste, Drachen-/Koloss-Kerne – für Quest-Abgaben, Bau-/Befestigungs-Strukturen (`200-cm`-Raster), Ruinen-Forschung oder Hochrang-Schmiede-/Altar-Rezepte.

### 7.2 Rüstung, Häute, Schuppen & Panzerplatten

| Item | Kategorie | Item-Stats & Eigenschaften | Wo wird es verbaut? | Sonstige Quellen (außer Kreatur-Drop) | Verbaut bei Kreaturen (Auswahl) |
| :--- | :--- | :--- | :--- | :--- | :--- |
| **Mineral-Krustensplitter / Dichtere Krustenplatten** | Rüstung / Handwerk | Hohe Abriebfestigkeit; ab Rang 4+ dichter; gibt soliden Rüstungsbonus, neigt bei Druck zu Abrieb/Staub. | Rüstungen (Brust/Schultern), `200-cm`-Barrikaden/Verstärkungen. | Fels-/Ruinen-Adern, Geröll-Abbau. | Vaelgorn |
| **Trockene Laufhaut / Zähere Laufhaut** | Rüstung / Beweglichkeit | Leicht bis mittel; schützt bei Bewegung/Staub, hält Ausdauer-Regeneration hoch. | Leichte Bein-/Rüstungs-Teile, bewegliche Ausrüstung. | Trockene Boden-/Steppen-Funde (selten). | Siltstrider |
| **Buckelige Rumpfhaut / Gehärtete Rumpfhaut** | Rüstung / Verfall | Zäh und polsternd; dämpft Stöße und leichte Sporen-/Staub-Reize. | Rumpf-/Schulter-Schutz, Sporen-/Staub-Schutz-Teile. | Nicht als Roh-Erz; nur über Sammelstellen. | Sporeback |
| **Klammer-Chitin / Gehärtetes Klammer-Chitin / Seltene Chitin-Platte** | Rüstung / Haltung | Sehr gutes Verhältnis aus Gewicht und Festigkeit; stabilisiert Griffe und Kanten-Halt. | Hand-/Arm-/Bein-Schutz, Kletter-/Kanten-Gear. | Ruinen-Chitin-Ablagerungen. | Skitterclaw |
| **Kalt-borstiges Fell / Dichtes Hochlagen-Fell** | Rüstung / Frost | Isoliert gegen Kälte und windige Böen; hält Körperwärme und Ausdauer-Erholung stabil. | Winter-/Hochlagen-Rüstungen, Kragen/Umhänge, Futter. | Kalt-Funde / Hochlagen-Lager. | Rimehound |
| **Schlacke-Frontkruste / Dichte Schlacke-Platte** | Rüstung / Hitze | Extrem hitzebeständig und schwer; hoher Rüstungsbonus, erhöht Gewicht. | Schwere Hitze-/Bollwerk-Rüstungen, Hitzeschutz-Platten. | Schlacke-Halden in Glut-Fluren. | Slagbrute, Pyroclast |
| **Gewölbe-Panzerplatten / Seltene Gewölbe-Reliktplatte** | Rüstung / Bau | Massiv und mauerfest; hoher Schutz gegen Stürze und Trümmer. | Schwere Gewölbe-Rüstungen, `200-cm`-Mauer-/Tür-Verstärkungen. | Eingestürzte Gewölbe / Ruinen-Trümmer. | Vaultcrawler |
| **Moosdurchsetzte Rinde / Uralte Rindenplatten** | Rüstung / Natur | Faserig-gedämpft; puffert Stöße und passt in Wald-/Senken-Tarnelemente. | Wald-/Senken-Rüstungen, Tarn-/Deckungs-Verkleidungen. | Alte Bäume / Unterholz (keine Wasserwesen). | Mossgolem |
| **Sturm-Borstenfell** | Rüstung / Sturm | Windabweisend und widerstandsfähig gegen Böendruck. | Sturm-Kamm-Ausrüstung, Schulter-/Rücken-Schutz. | Hochlagen-Funde bei Sturm. | Galehound |
| **Grat-Schuppenhaut / Gehärtete Grat-Schuppen** | Rüstung / Gebirge | Raue Schiefer-/Granit-Schuppen mit hoher Kratz- und Schlagfestigkeit. | Gebirgs-Rüstungen, Unterarmschutz. | Fels-/Grat-Vorkommen. | Cragstalker |
| **Dicke Frost-Schuppenbrocken / Gehärteter Frost-Panzer** | Rüstung / Frost | Sehr dicke Kältepanzerung; starker Schutz gegen Frost- und Pranken-Schläge. | Schwere Frost-Rüstungen, Hochlagen-Schilde/-Platten. | Frost-Spalten / Gebirgs-Kanten. | Frostmauler |
| **Asche-Balg/Haut / Zähere Häute** | Rüstung / Asche | Leichte bis mittlere Haut; schützt vor Ruß-/Asche-Reiz und Staub. | Leichte/mittlere Rüstungen, Staub-Schutzteile. | Nicht als Erz; nur über Sammelstellen. | Ashstalker |
| **Verholzte Hautstücke / Zähere Häute** | Rüstung / Haltung | Holz-/hornartig zäh; dämpft Bisse und Rammstöße. | Verstärkte Leder-/Holz-Rüstungen, Armschienen. | Unterholz-/Dornen-Funde. | Thornmaw |
| **Staubige Hautlappen** | Rüstung / Leicht | Sehr leicht und flexibel; kaum Rüstung, aber kaum Gewichts-/Ausdauer-Last. | Leichte Flick-/Lauf-Ausrüstung, Staub-Polster. | Staubige Verwehungen (gering). | Dustmote |
| **Zähe Feuchthaut / Dickere Häute** | Rüstung / Feuchte | Zäh und feuchtigkeitsbeständig (reiner Land-/Senken-Bezug, kein Wasserwesen). | Senken-/Wald-Rüstungen, feuchte-resistente Teile. | Feuchte Senken / Unterholz. | Mirecrawler |
| **Verkohlte Faser-/Fellreste / Zähere Brand-Häute** | Rüstung / Hitze | Ruß-/hitzegehärtet; schützt gegen Funken und Glut-Hitze. | Glut-/Asche-Rüstungen, Hand-/Bein-Schutz. | Brand-/Asche-Stellen. | Cinderhound |
| **Erd-/Schutt-Schulterstücke / Gehärtete Schulterplatten** | Rüstung / Haltung | Breiter Schulterschutz gegen Herabfallendes und Haltungsschaden (`PoiseResist`). | Schulterstücke, schwere Rüstungs-Teile. | Schutt-/Geröll-Halden. | Graveclaw |
| **Kalt-graue Hautstücke / Dichte Kälte-Häute** | Rüstung / Frost | Glatt-kristallin; dämpft Kälte-Einflüsse und schneidende Treffer. | Kälte-/Hochlagen-Rüstungen. | Frost-Kanten. | Rimestalker |
| **Ausgehöhlte Rücken-/Rumpfstücke / Gehärtete Rumpfstrukturen** | Rüstung / Sporen-Staub | Strukturierte Rumpf-/Rücken-Teile mit Wolken-/Filter-Charakter. | Rumpf-/Rücken-Ausrüstung, Spezial-Filter/Schutz. | Ruinen-/Asche-/Wald-Funde (selten). | Hollowback |
| **Eisen-/Krusten-Panzerplatten / Massive Panzerplatten** | Rüstung / Schwer | Sehr hoher Rüstungs- und `PoiseResist`-Wert, hohes Gewicht. | Schwere Bollwerk-Rüstungen, `200-cm`-Bollwerk-Verstärkungen. | Erz-/Eisen-/Schlacke-Adern. | Ironhide |
| **Dunkle Faser-/Gewebebahnen / Dichtere Schatten-/Faserbahnen** | Rüstung / Schatten | Leicht, geschichtet und geräusch-/lichtdämpfend. | Schatten-/Pirscher-Rüstungen, Umhänge. | Ruinen-/Schatten-Ecken (selten). | Shroudwalker |
| **Dicke Trollhaut / Besonders dicke Häute** | Rüstung / Schwer | Massiv, zäh und ausdauerfordernd im Verarbeiten; starker Rüstungs- & `PoiseResist`-Wert. | Schwere Troll-/Bollwerk-Rüstungen. | Sehr selten in tiefen Wäldern/Ruinen. | Troll |
| **Glut-Schuppen / Schlacke-Platten** | Rüstung / Drache & Hitze | Hochwertige Drachen-/Schlacke-Panzerung mit starker Hitze- und Schlagfestigkeit. | Drachen-/Endgame-Rüstungen (Level 36–50). | Nur über Drachen-/Glut-Quellen. | Cinderwyrm |
| **Granit-/Fels-Schuppen / Schwere Berg-Schuppen** | Rüstung / Drache & Fels | Extrem widerstandsfähige Gebirgs-Drachenschuppen für hohen Rüstungs- & `PoiseResist`-Wert. | Hochgebirgs-/Endgame-Rüstungen. | Nur über Drachen-/Hochgebirgs-Quellen. | Cragdrake |
| **Sturm-Schwingenhaut / Junge & Spitzen-Sturm-Schwingenhaut** | Rüstung & Gleit / Sturm | Zäh, elastisch und windgeschliffen; schützt gegen Böen und erlaubt leichte/sturmstarke Ausrüstung. | Sturm-/Endgame-Rüstungen, Gleit-/Böen-Ausrüstung. | Nur über Sturm-Drachen-Quellen. | Stormmaw, Stormwyrmling, Galewyrm |
| **Asche-Schwingenhaut / Hochwertige Gleit-Schwingenhäute** | Rüstung & Gleit / Asche | Faserig-schuppig, zäh und staub-/gleitfähig. | Asche-/Gleit-/Endgame-Ausrüstung. | Nur über Asche-Drachen-Quellen. | Ashwing |
| **Frost-/Schiefer-Schuppen / Gehärtete Frost-Schuppen** | Rüstung / Drache & Frost | Dicke Kälte-Drachenschuppen mit hoher Frost-Resistenz und Rüstung. | Frost-/Endgame-Rüstungen. | Nur über Frost-Drachen-Quellen. | Frostdrake |
| **Spitzen-Drachenschuppen** | Rüstung / Spitzen-Endgame | Höchste Rüstungs- und Resistenz-Qualität für Level-50-Endgame-Sets. | Spitzen-Rüstungen (Cap Lv. 50). | Nur über Spitzen-Drachen. | Dreadwyrm |

### 7.3 Waffen, Zähne, Klauen, Hörner & Haltungsschaden

| Item | Kategorie | Item-Stats & Eigenschaften | Wo wird es verbaut? | Sonstige Quellen (außer Kreatur-Drop) | Verbaut bei Kreaturen (Auswahl) |
| :--- | :--- | :--- | :--- | :--- | :--- |
| **Scharfe Fangzähne / Verstärkte Fangzähne** | Waffen / Stich & Schnitt | Gute Schneid-/Stich-Eigenschaften; ab Rang 5+ gehärtet. | Klingen, Speere, Pfeil-/Bolzenspitzen, leichte Waffen. | Selten über Knochen-/Jagdfunde. | Bramblefang, Rimehound |
| **Scharfe Krallenspitzen / Dickere Krallen** | Waffen / Schnitt & Haltung | Scharf und greifend; gibt Schnitt- und leichten `PoiseDmg`-Anteil. | Klauen-/Klingen-/Stangenwaffen, Fallen-Spitzen. | Selten über Ruinen-/Fels-Funde. | Skitterclaw |
| **Verkohlter Kieferknochen / Gehärteter Kieferknochen / Schwerer Glut-Kiefer** | Waffen / Haltung & Hitze | Wuchtig und hitzegehärtet; hoher `PoiseDmg` und Schlag-/Biss-Wucht. | Keulen, Hämmer, schwere Waffen, Haltungs-Aufsätze. | Selten in Brand-/Schlacke-Zonen. | Charmaw |
| **Raue Fangzähne** | Waffen / Kälte & Schnitt | Kalt-rauhe Zähne mit solidem Schnitt- und Kälte-Nuancen-Potenzial. | Klingen, Speere, Kälte-/Hochlagen-Waffen. | Selten an kalten Kanten. | Rimehound |
| **Greifarm-Gelenke / Gehärtete Greifgelenke** | Waffen & Handwerk / Haltung | Beweglich und stabil; gut für Hebel-, Haken- oder Gelenk-Waffen/Werkzeuge. | Haken-/Hebel-Waffen, Kletter-/Greif-Werkzeuge. | Gewölbe-/Ruinen-Funde. | Vaultcrawler |
| **Gebogene Kammhörner** | Waffen / Haltung | Kräftige Hörner mit hoher Stoß- und Haltungswirkung (`PoiseDmg`). | Hörner-/Stoßwaffen, Ramm-/Schlag-Aufsätze. | Hochgebirgs-Funde (selten). | Cragstalker |
| **Massive Prankenknochen / Seltener Hochlagen-Knochen** | Waffen / Schwer & Poise | Sehr schwerer Knochen; liefert hohen Schaden und starken `PoiseDmg`. | Schwere Hämmer/Keulen/Zweihänder. | Selten an Frost-Kanten. | Frostmauler |
| **Schmale & Gebogene Sturm-Drachenhörner** | Waffen / Sturm & Drache | Hochwertiges Drachenhorn; starker Schaden und `PoiseDmg` im Endgame. | Drachen-/Sturm-Waffen (Level 36–50). | Nur über Sturm-Drachen. | Stormwyrmling, Stormmaw, Galewyrm |
| **Dornen-/Horn-Splitter & Dickere Dorn-/Horn-Teile** | Waffen / Einstieg & Haltung | Solide Einstiegs-/Mittelklasse-Spitzen und Haltungs-Verstärker. | Frühe bis mittlere Waffen, Pfeile/Speere. | Dornen-/Fels-Stellen. | Thornmaw |
| **Sichel-Greifklauen & Gehärtete Sichelklauen** | Waffen / Schnitt & Greif | Gebogene Klauen mit starkem Schnitt- und Haken-Charakter. | Sichel-/Klingen-/Haken-Waffen. | Ruinen-/Fels-Funde. | Murghul |
| **Fels-Klauen & Massive Felsklauen** | Waffen / Poise & Fels | Extrem wuchtige Klauen für hohen `PoiseDmg` und harte Schläge. | Schwere Schlag-/Fels-Waffen. | Gebirgs-/Fels-Funde. | Kraggar |
| **Grabklauen & Schwere Grabklauen** | Waffen / Grab & Poise | Schaufel- und reißstarke Klauen; gut gegen Haltung und deckungsnahe Ziele. | Schwere Klauen-/Brech-Waffen, Grab-Werkzeuge. | Schutt-/Erd-Funde. | Graveclaw |
| **Scharfe Glieder-/Hornspitzen** | Waffen / Stich & Kälte | Präzise, kalte Spitzen für Stich- und gezielte Treffer. | Speere, Dolche, Pfeilspitzen. | Kälte-/Hochlagen-Funde. | Rimestalker |
| **Schwere Hornreste / Zackige Kiefer-/Horn-Teile / Große Kiefer-/Horn-Trophäen** | Waffen / Schwer & Poise | Hohe Masse und Schneid-/Stoßkraft für späte Waffen und Trophäen. | Schwere Waffen, Pass-/Koloss-Ausrüstung. | Selten über Berg-/Ruinen-Funde. | Ironhide, Cragmaw |
| **Klammerkrallen & Fels-Klaue** | Waffen / Drache & Fels | Schwere Drachenklauen für wuchtige Schläge und `PoiseDmg`. | Drachen-/Gebirgs-Waffen. | Nur über Drachen-/Fels-Quellen. | Cragdrake |
| **Gebogene Hörner/Zähne (Stormmaw) & Uralte Knochen-/Horn-Teile (Dreadwyrm)** | Waffen / Spitzen-Endgame | Höchste Drachen-Waffen- und Haltungs-Komponenten für Level 50. | Spitzen-Waffen (Cap Lv. 50). | Nur über Spitzen-Drachen. | Stormmaw, Dreadwyrm |

### 7.4 Sehnen, Fasern, Gewebe & Bindungen

| Item | Kategorie | Item-Stats & Eigenschaften | Wo wird es verbaut? | Sonstige Quellen (außer Kreatur-Drop) | Verbaut bei Kreaturen (Auswahl) |
| :--- | :--- | :--- | :--- | :--- | :--- |
| **Dünne Sehnen / Zähe Sehnen / Lange Laufsehnen / Brand-Sehnen / Frost-Sehnen / Schwere Sehnen / Sprungsehnen / Elastische & Massive Drachensehnen** | Handwerk / Bindung | Hohe Zugfestigkeit und Elastizität; verbessert Waffen-Bindungen, Bögen, Griffe, Ausdauer-Handling und Ausrüstungs-Zähigkeit. | Waffen-Griffe/-Bindungen, Bögen, Rüstungs-Nähte, Drachen-Ausrüstung. | Selten über Jagd-/Ruinen-Funde; Drachensehnen nur über Drachen. | Fast alle 48 Kreaturen |
| **Verkohlte Faser / Zähe Hitze-Fasern / Zähe Asche-Faserstränge / Hochfester Glut-Faserstrang** | Handwerk / Hitze-Faser | Hitzebeständige Faserstränge; gut für hitzebeständige Bindungen, Netze oder Ausrüstungs-Nähte. | Hitze-/Asche-Ausrüstung, Netze/Fesseln, Bindungen. | Brand-/Asche-Stellen. | Cinderling, Cinderloom, Ashweaver |
| **Schatten-Gewebefetzen / Dichtere Schatten-/Faserbahnen / Dunkle Faser-/Gewebebahnen** | Handwerk / Schatten-Faser | Weich, licht-/geräuschgedämpft; passt zu Schatten-/Pirscher-Ausrüstung und leichten Bindungen. | Schatten-/Pirscher-Gear, leise Bindungen/Polster. | Ruinen-/Schatten-Stellen (selten). | Gloomstalker, Shroudwalker |
| **Zähes Bindematerial / Knochengelenke / Gehärtete Knochenstreben** | Handwerk / Struktur | Stabilisiert Verbindungen, Gelenke und Rahmen bei Ausrüstung und Geräten. | Waffen-/Rüstungs-Verbindungen, Geräte-/Bau-Gelenke. | Knochen-/Ruinen-Funde. | Mossgolem, Osskarr |

### 7.5 Kerne, Glut, Frost, Sturm, Sporen & Essen/Tränke

| Item | Kategorie | Item-Stats & Eigenschaften | Wo wird es verbaut? | Sonstige Quellen (außer Kreatur-Drop) | Verbaut bei Kreaturen (Auswahl) |
| :--- | :--- | :--- | :--- | :--- | :--- |
| **Funken-Asche / Glimmender Splitter / Glimmende Glutkapsel / Kleine Glutknoten / Stabile Glutkapseln / Glimmende Funken-/Glut-Reste** | Veredelung / Hitze & Licht | Erzeugt oder speichert Glut-/Funken-Wirkung; nützlich für Licht-/Hitze-Effekte, Feuer-/Glut-Veredelungen oder Feld-Signale. | Glut-/Hitze-Veredelungen, Fackel-/Licht-/Signal-Komponenten, Spezial-Mixturen. | Asche-/Glut-Zonen. | Cinderling, Cinderloom, Cinderhound, Ashweaver |
| **Sporenkappen-Rest / Konzentrierte Sporenkapsel / Seltene Sporen-Essenz** | Veredelung & Tränke / Sporen | Enthält Sporen-/Reiz-Wirkung; für Reiz-/Nebel-/Zustands-Mixturen oder Spezial-Anwendungen. | Alchemie-/Zustands-Mixturen, Sporen-/Nebel-Effekte. | Sporen-/Wald-/Ruinen-Funde (selten). | Sporeback |
| **Frost-Kernstück / Geladener Kernrest / Glühender Schlacke-Kern / Glühender Koloss-Kern / Geladener Drachenkernrest / Sturm-Wyrm-Kern** | Veredelung & Spitzen / Element-Kern | Konzentrierter Energie-/Umwelt-Kern (Frost, Sturm/Spannung, Glut/Schlacke oder Drachen-Energie) für Hochrang-Craftings & Spezial-Ausrüstung. | Endgame-Waffen/-Rüstungen (Level 40–50), Spezial-Upgrades, Relikt-Altäre/Stationen. | Extrem selten bei besonderen Ereignissen/Hochlagen; Drachen-/Koloss-Kerne primär über diese Wesen. | Rimehound, Galehound, Slagbrute, Cindercolossus, Stormwyrmling, Galewyrm |
| **Leichte & Gehärtete Laufknochen / Knorpelreste / Hör-/Schwingungsorgan / Ausgeprägtere Schwingungsorgane** | Essen / Brühen, Rationen & Alchemie | Knochen-/Knorpel-/Organ-Komponenten: liefern knorpelige/mineralische Basis für Feld-Brühen, Ausdauer-/Erholungs-Rationen oder Schall-/Wahrnehmungs-Mixturen. | Koch-/Feld-Rationen, Brühen, Ausdauer-/Erholungs-Tränke, Wahrnehmungs-Mixturen. | Jagd-/Lager-Funde; Schwingungsorgane speziell über Morrowear. | Siltstrider, Morrowear, Thornmaw, Mirecrawler |

### 7.6 Biom-Zusatz-Drops & Quest-/Relikt-Gegenstände

| Item | Kategorie | Item-Stats & Eigenschaften | Wo wird es verbaut? | Sonstige Quellen (außer Kreatur-Drop) | Typische Gebiete / Kreaturen |
| :--- | :--- | :--- | :--- | :--- | :--- |
| **Moosfaser / Dornen-Holz / Unterholz-Faser / Feucht-Leder / Ruß-Balgrest / Modriges Gewebe-/Relikt** | Biom-Loot / Handwerk & Bau | Natur-/Unterholz-/Feucht- oder Ruß-Materialien; solide Basis für frühe bis mittlere Ausrüstung, Bindungen und Wald-/Senken-Bau (`200-cm`-Raster). | Frühe/mittlere Rüstungen, Bogen-/Werkzeug-Bindungen, Wald-/Lager-Bauten. | **Ja – häufig:** Sammeln an Bäumen, Unterholz, Büschen, feuchten Senken & Brandstellen (ohne Wasserwesen). | Moosgeäst & Sumpf-Senken, Glut-Fluren |
| **Ruinen-Chitin / Verfall-Relikt / Altes Eisenstück / Kalk-/Mauerstaub-Kapsel / Ruinen-Staubrest / Ruinen-Rußrest / Relikt-Faser** | Biom-Loot & Quest / Ruinen & Bau | Alte Bau-/Metall-/Relikt-Reste; wichtig für Ruinen-Forschung, Befestigungen (`200-cm`-Raster), Reparaturlösungen und Quest-Abgaben. | Ruinen-Gear, Befestigungen/Mauern, Reparaturlösungen, Quest-Abgaben. | **Ja – häufig:** Durchsuchen von Ruinen, Gewölben, Trümmern, alten Lagern und Wrack-/Verfallstellen. | Bruchstein & Gewölbe-Spalten |
| **Asche-Mineral / Schlacke-Kern / Glut-Schuppe / Asche-Horn / Asche-Faser / Glut-Funke / Asche-Sehne / Asche-Haut** | Biom-Loot / Hitze & Veredelung | Hitze-/aschegeprägte Materialien; für Hitze-Resistenzen, Glut-Veredelungen und Asche-Gebiets-Ausrüstung. | Hitze-/Asche-Rüstungen & -Waffen, Feuer-/Glut-Rezepte, Quest-Aufträge. | **Ja – mittel/häufig:** Abbau & Sammeln in Asche-, Ruß- und Schlacke-Zonen. | Glut-Fluren & Schlacke-Böden |
| **Hochgebirgs-Stein / Granit-Schuppe / Frost-Splitter / Fels-Stachel / Kanten-Horn / Gebirgs-Knorpel / Geröll-Erzrest / Fels-Klaue / Kalt-Horn** | Biom-Loot / Fels & Frost | Gebirgs-/Frost-Materialien; für Kälte-Resistenzen, `PoiseResist`, Berg-Ausrüstung und stabile Fundamente. | Gebirgs-/Frost-Rüstungen, Haltungs-/Rüstungs-Upgrades, Fundament-/Kanten-Bau. | **Ja – mittel/häufig:** Abbau an Felsen, Geröll, Klippen und Frost-Kanten. | Schroffen-Grate & Frost-Kanten |
| **Sturm-Faser / Sturm-Schwingenhaut / Geladener Kernrest / Sturm-Horn** | Biom-Loot / Sturm & Hochlage | Sturm-/böengeprägte Materialien; für Sturm-Resistenz, Wind-/Gleit-Ausrüstung und Hochlagen-Rezepte. | Sturm-/Hochlagen-Ausrüstung, Spezial-Veredelungen, Quest-/Expeditions-Ziele. | **Ja – selten/wetterabhängig:** Hochlagen-Funde, Sturm-Ereignisse & Drachen-/Sturm-Quellen. | Böen-Kämme & Sturm-Kuppen |
| **Seltener Koloss-Kern/Rest / Uralte Drachen-Reliktreste / Seltener Wyrm-Spitzenrelikt** | Quest, Trophäe & Spitzen-Endgame | Höchste Seltenheits- und Quest-/Endgame-Komponenten für Level-50-Spitzen-Ausrüstung, Boss-/Quest-Reihen und Trophäen. | Spitzen-Waffen/-Rüstungen (Cap Lv. 50), Haupt-/Meister-Quests, Trophäen-Ausbau. | **Nein / extrem selten:** Hauptsächlich über Kolosse & Drachen sowie besondere Spitzen-Quests/Events. | Dreadcolossus, Cindercolossus, Dreadwyrm, Stormmaw, Galewyrm |

---

## 8. Crafting- & Bau-Rezepte über alle Level-50-Phasen

### 8.1 Die 5 Progression-Phasen (Level 1–50)

* **Phase 1: Einstieg & Dorn (`Lv. 1 – 10` | Gefahren-Rang `1 – 2`):** Erste Behelfswaffen, leichte Dorn-/Laufhaut-Schutzteile, einfache Feld-Brühen und `200-cm`-Grundsicherungen.
* **Phase 2: Ruinen & Pirscher (`Lv. 11 – 20` | Gefahren-Rang `2 – 3`):** Solide Klingen/Spitzen, Chitin-/Schatten-Rüstungen, Reiz-/Sporen- & Ausdauer-Tränke, `200-cm`-Schutz-/Sperrelemente.
* **Phase 3: Schlacke & Bollwerk (`Lv. 21 – 32` | Gefahren-Rang `3 – 4`):** Schwere Haltungs-Waffen (`PoiseDmg`), Schlacke-/Krusten-/Panzer-Rüstungen, Hitze-/Kälte-Mixturen, `200-cm`-Bollwerk-Mauern & -Tore.
* **Phase 4: Grat, Frost & Sturm (`Lv. 33 – 42` | Gefahren-Rang `4 – 5`):** Gehärtete Berg-/Sturm-/Drachen-Waffen & -Rüstungen, Hochlagen-/Böen-Tränke, `200-cm`-Hochlagen- & Turm-Segmente.
* **Phase 5: Wyrm & Koloss (`Lv. 43 – 50` | Gefahren-Rang `6+` bis Captain-Cap `25`):** Spitzen-Ausrüstung (`Cap Lv. 50`) mit hohem Schaden, `PoiseDmg`, Rüstung & Resistenzen, Spitzen-Elixiere, `200-cm`-Endgame-Festungsteile.

### 8.2 Konkrete Beispiel-Rezepte (Waffen, Rüstungen, Tränke/Essen & 200-cm-Bauwerke)

* **Waffen:**
  * *Dornzahn-Klinge* (`Lv. 12–16`): `3x` Scharfe Fangzähne, `2x` Dünne Sehnen, `1x` Dornen-Holz (`~32 Dmg`, `~24 PoiseDmg`).
  * *Schlackekiefer-Brecher* (`Lv. 26–32`): `2x` Gehärteter Kieferknochen, `2x` Schlacke-Frontkruste, `2x` Zähe Sehnen, `1x` Asche-Mineral (`~68 Dmg`, `~56 PoiseDmg`).
  * *Sturmwyrm-Klinge / -Speer* (`Lv. 46–50`): `2x` Spitzen-Sturm-Schwingenhaut, `2x` Gebogene Sturm-Drachenhörner, `2x` Massive Drachensehnen, `1x` Sturm-Wyrm-Kern (`~138 Dmg`, `~98 PoiseDmg`).
* **Rüstungen:**
  * *Klammer-Chitin-Wams & -Armschienen* (`Lv. 14–18`): `3x` Klammer-Chitin, `2x` Ruinen-Chitin, `2x` Dünne Sehnen, `1x` Altes Eisenstück (`~62 Armor`, `~48 PoiseRes`, `~12 %` Verfall-Resistenz).
  * *Frost-Schuppen-Plattenrüstung* (`Lv. 34–40`): `3x` Dicke Frost-Schuppenbrocken, `2x` Kalt-borstiges Fell, `2x` Granit-Schuppe, `2x` Schwere Sehnen (`~175 Armor`, `~122 PoiseRes`, `~36 %` Frost-, `~15 %` Sturm-Resistenz).
  * *Wyrmschuppen-Endgame-Panzer* (`Lv. 47–50`): `3x` Spitzen-Drachenschuppen, `2x` Glut-Schuppen, `2x` Massive Drachensehnen, `1x` Glühender Koloss-Kern (`~250 Armor`, `~168 PoiseRes`, `~48 %` Hitze-, `~44 %` Frost-/Sturm-Resistenz).
* **Tränke & Essen:**
  * *Herzhafte Knorpel- & Laufbrühe* (`Lv. 8–22`): `2x` Knorpelreste, `1x` Leichte Laufknochen, `1x` Unterholz-Faser (`+35 HP`, `+20 %` Ausdauer-Regeneration für `180 s`).
  * *Sporen-Kapsel-Tonikum* (`Lv. 18–35`): `2x` Konzentrierte Sporenkapsel, `1x` Verfall-Relikt, `1x` Moosfaser (`+25 %` Verfall-/Sporen-Resistenz für `120 s`).
  * *Glut- & Sturm-Elixier* (`Lv. 38–50`): `1x` Stabile Glutkapseln, `1x` Geladener Kernrest, `2x` Glimmende Glutknoten (`+18 %` Waffen-`PoiseDmg`, `+20 %` Hitze- & Sturm-Resistenz für `60 s`).
* **200-cm-Bauwerke (`200 × 200 cm` Modulraster):**
  * *200-cm-Verstärkte Palisaden- & Dornenwand:* `4x` Dornen-Holz, `2x` Dornen-Balg, `2x` Dünne Sehnen.
  * *200-cm-Gewölbe-Bollwerk & Torrahmen:* `4x` Gewölbe-Panzerplatten, `3x` Altes Eisenstück, `3x` Grobe Gesteinsbrocken, `2x` Zähes Bindematerial.
  * *200-cm-Schlacke- & Grat-Turmsegment:* `4x` Schlacke-Frontkruste, `3x` Hochgebirgs-Stein, `2x` Verfall-Relikt, `1x` Glühender Schlacke-Kern.

### 8.3 Durchgehende Rezept-Übersicht über Level 1–50

| Phase / Level | Waffe (`Dmg` / `PoiseDmg`) | Rüstung (`Armor` / `PoiseRes` / Resist) | Trank / Essen | `200-cm`-Bauwerk (`200 × 200 cm`) |
| :---: | :--- | :--- | :--- | :--- |
| **Phase 1** (`Lv. 1–10`) | **Dornzweig- & Knochen-Speer** (`~18` / `~15`) – Knochenreste, Dornen-/Horn-Splitter, Dünne Sehnen | **Staub- & Laufhaut-Jacke** (`~28` / `~26`) – Trockene Laufhaut, Staubige Hautlappen, Dünne Sehnen | **Knorpel- & Senken-Brühe** – Knorpelreste, Leichte Laufknochen, Unterholz-Faser | **200-cm-Unterholz- & Dornenzaun** – Dornen-Holz, Dünne Sehnen, Dornen-Balg |
| **Phase 2** (`Lv. 11–20`) | **Dornzahn-Klinge** (`~36` / `~28`) – Scharfe Fangzähne, Dünne Sehnen, Dornen-Holz | **Klammer-Chitin-Wams** (`~65` / `~52` / `~14 %` Verfall) – Klammer-Chitin, Ruinen-Chitin, Dünne Sehnen, Altes Eisenstück | **Sporen-Kapsel-Tonikum** – Konzentrierte Sporenkapsel, Verfall-Relikt, Moosfaser | **200-cm-Ruinen- & Chitin-Sperre** – Gewölbe-Panzerplatten, Altes Eisenstück, Ruinen-Chitin |
| **Phase 3** (`Lv. 21–32`) | **Schlackekiefer-Brecher** (`~68` / `~56`) – Gehärteter Kieferknochen, Schlacke-Frontkruste, Zähe Sehnen, Asche-Mineral | **Schlacke- & Bollwerk-Platten** (`~125` / `~88` / `~26 %` Hitze) – Schlacke-Frontkruste, Eisen-/Krusten-Panzerplatten, Zähe Sehnen, Schlacke-Kern | **Glut- & Ausdauer-Sud** – Stabile Glutkapseln, Asche-Mineral, Brand-Sehnen | **200-cm-Bollwerk- & Schlacke-Mauer** – Schlacke-Frontkruste, Grobe Gesteinsbrocken, Zähes Bindematerial |
| **Phase 4** (`Lv. 33–42`) | **Grat- & Frost-Zweihänder** (`~102` / `~76`) – Massive Prankenknochen, Gebogene Kammhörner, Schwere Sehnen, Frost-Splitter | **Frost-Schuppen-Plattenrüstung** (`~182` / `~124` / `~38 %` Frost) – Dicke Frost-Schuppenbrocken, Kalt-borstiges Fell, Granit-Schuppe, Schwere Sehnen | **Frost- & Haltungs-Gebräu** – Frost-Splitter, Kalt-borstiges Fell, Schwere Sehnen | **200-cm-Hochlagen- & Grat-Turmwand** – Hochgebirgs-Stein, Granit-Schuppe, Schwere Sehnen, Verfall-Relikt |
| **Phase 5** (`Lv. 43–50`) | **Sturmwyrm-Klinge / -Speer** (`~140` / `~100`) – Spitzen-Sturm-Schwingenhaut, Gebogene Sturm-Drachenhörner, Massive Drachensehnen, Sturm-Wyrm-Kern | **Wyrmschuppen-Endgame-Panzer** (`~252` / `~168` / `~48 %` Hitze, `~44 %` Frost/Sturm) – Spitzen-Drachenschuppen, Glut-Schuppen, Massive Drachensehnen, Glühender Koloss-Kern | **Sturm- & Brecher-Elixier** – Geladener Kernrest, Glimmende Glutknoten, Spitzen-Schwingenhaut-/Rest | **200-cm-Koloss- & Wyrm-Bollwerk** – Massive Schlacke-Kolossplatte, Dichte Rinden-/Reliktplatten, Massive Drachensehnen, Glühender Koloss-Kern |

---

## 9. UE5 C++-Referenzimplementierungen

### 9.1 ProgressionAndGearTypes.h (Level-Cap 50 & Captain-Cap 25)

```cpp
#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "ProgressionAndGearTypes.generated.h"

static constexpr int32 MaxPlayerLevelCap = 50;
static constexpr int32 MaxCaptainRankCap = 25;

USTRUCT(BlueprintType)
struct FPlayerProgressionTierRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Progression", meta = (ClampMin = "1", ClampMax = "50"))
	int32 PlayerLevel = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Progression", meta = (ClampMin = "1", ClampMax = "25"))
	int32 RecommendedDangerRank = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gear|Weapon", meta = (ClampMin = "1.0"))
	float TargetWeaponDmg = 14.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gear|Weapon", meta = (ClampMin = "1.0"))
	float TargetWeaponPoiseDmg = 12.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gear|Armor", meta = (ClampMin = "0.0"))
	float TargetArmorRating = 15.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gear|Armor", meta = (ClampMin = "0.0"))
	float TargetPlayerPoiseResist = 20.0f;
};
```

### 9.2 CombatMathLibrary.h (Rüstung, Resistenzen & Poise)

```cpp
#pragma once

#include "CoreMinimal.h"

inline float ComputeArmorDamageReduction(
	float ArmorRating,
	float ArmorConstant = 160.0f,
	float MaxReductionCap = 0.75f)
{
	const float ClampedArmor = FMath::Max(0.0f, ArmorRating);
	const float SafeK = FMath::Max(1.0f, ArmorConstant);
	const float RawReduction = ClampedArmor / (ClampedArmor + SafeK);
	return FMath::Clamp(RawReduction, 0.0f, MaxReductionCap);
}

inline float ApplyArmorToDamage(
	float RawDamage,
	float ArmorRating,
	float MinDamage = 1.0f,
	float ArmorConstant = 160.0f,
	float MaxReductionCap = 0.75f)
{
	const float Reduction = ComputeArmorDamageReduction(ArmorRating, ArmorConstant, MaxReductionCap);
	const float Reduced = RawDamage * (1.0f - Reduction);
	return FMath::Max(MinDamage, Reduced);
}

inline float ApplyResistance(
	float RawHazardValue,
	float TotalResistance,
	float MinResistance = -0.25f,
	float MaxResistanceCap = 0.60f)
{
	const float ClampedResist = FMath::Clamp(TotalResistance, MinResistance, MaxResistanceCap);
	return RawHazardValue * (1.0f - ClampedResist);
}

inline float ComputePlayerPoiseDamageTaken(
	float RawEnemyPoiseDmg,
	float AttackPoiseMult,
	float PlayerPoiseResist,
	float PoiseConstant = 90.0f)
{
	const float ClampedResist = FMath::Max(0.0f, PlayerPoiseResist);
	const float SafeK = FMath::Max(1.0f, PoiseConstant);
	const float ResistFactor = SafeK / (ClampedResist + SafeK);
	return FMath::Max(0.0f, RawEnemyPoiseDmg * AttackPoiseMult * ResistFactor);
}
```

### 9.3 CreatureLootRoller.h (RollCreatureLootFromRow & Nachzügler-Auswertung)

```cpp
#pragma once

#include "CoreMinimal.h"
#include "CreatureMasterTypes.h"

USTRUCT(BlueprintType)
struct FRolledLootItem
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Loot")
	FName ItemID = NAME_None;

	UPROPERTY(BlueprintReadOnly, Category = "Loot")
	int32 Count = 0;

	UPROPERTY(BlueprintReadOnly, Category = "Loot")
	bool bFromBiomeBonus = false;
};

namespace CreatureLootInternal
{
	inline void AddRolledItem(
		TArray<FRolledLootItem>& OutItems,
		FName ItemID,
		int32 Count,
		bool bIsBiomeBonus)
	{
		if (ItemID == NAME_None || Count <= 0)
		{
			return;
		}

		for (FRolledLootItem& Existing : OutItems)
		{
			if (Existing.ItemID == ItemID && Existing.bFromBiomeBonus == bIsBiomeBonus)
			{
				Existing.Count += Count;
				return;
			}
		}

		FRolledLootItem NewItem;
		NewItem.ItemID = ItemID;
		NewItem.Count = Count;
		NewItem.bFromBiomeBonus = bIsBiomeBonus;
		OutItems.Add(NewItem);
	}

	inline void EvaluateLootEntryArray(
		const TArray<FLootEntry>& Entries,
		int32 EffectiveRank,
		int32 ZoneRank,
		bool bIsBiomeBonus,
		FRandomStream& Stream,
		TArray<FRolledLootItem>& OutItems)
	{
		for (const FLootEntry& Entry : Entries)
		{
			const FAdjustedLootRoll Roll = EvaluateFairDownscaledLoot(
				Entry,
				EffectiveRank,
				ZoneRank,
				bIsBiomeBonus,
				Stream);

			if (Roll.RolledCount > 0)
			{
				AddRolledItem(OutItems, Entry.ItemID, Roll.RolledCount, bIsBiomeBonus);
			}
		}
	}
}

inline TArray<FRolledLootItem> RollCreatureLootFromRow(
	const FCreatureLootRow& LootRow,
	int32 EffectiveRank,
	int32 ZoneRank,
	FName ActiveBiomeID,
	FRandomStream& Stream)
{
	TArray<FRolledLootItem> RolledItems;

	const int32 ClampedMinRank = FMath::Max(1, LootRow.MinRank);
	if (EffectiveRank < ClampedMinRank)
	{
		return RolledItems;
	}

	const int32 SafeZoneRank = FMath::Max(EffectiveRank, ZoneRank);

	CreatureLootInternal::EvaluateLootEntryArray(
		LootRow.BaseDrops,
		EffectiveRank,
		SafeZoneRank,
		false,
		Stream,
		RolledItems);

	CreatureLootInternal::EvaluateLootEntryArray(
		LootRow.RankEscalationDrops,
		EffectiveRank,
		SafeZoneRank,
		false,
		Stream,
		RolledItems);

	if (ActiveBiomeID != NAME_None)
	{
		for (const FLootBiomeOverride& BiomeOverride : LootRow.BiomeOverrides)
		{
			if (BiomeOverride.BiomeID == ActiveBiomeID)
			{
				CreatureLootInternal::EvaluateLootEntryArray(
					BiomeOverride.BonusLootEntries,
					EffectiveRank,
					SafeZoneRank,
					true,
					Stream,
					RolledItems);
			}
		}
	}

	return RolledItems;
}
```

### 9.4 CraftingRecipeTypes.h & CraftingExecutionLibrary.h (CanCraftRecipe & ConsumeRecipeIngredients)

```cpp
#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "CraftingRecipeTypes.generated.h"

UENUM(BlueprintType)
enum class ERecipeCategory : uint8
{
	Weapon UMETA(DisplayName = "Weapon"),
	Armor UMETA(DisplayName = "Armor"),
	Consumable UMETA(DisplayName = "Consumable"),
	Structure200cm UMETA(DisplayName = "Structure 200cm")
};

USTRUCT(BlueprintType)
struct FRecipeIngredient
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Recipe")
	FName ItemID = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Recipe", meta = (ClampMin = "1"))
	int32 Count = 1;
};

USTRUCT(BlueprintType)
struct FCraftingRecipeRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Recipe")
	FName RecipeID = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Recipe")
	ERecipeCategory Category = ERecipeCategory::Weapon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Recipe")
	FName OutputItemOrBuildingID = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Recipe", meta = (ClampMin = "1"))
	int32 OutputCount = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Recipe", meta = (ClampMin = "1", ClampMax = "50"))
	int32 RequiredPlayerLevel = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Recipe")
	TArray<FRecipeIngredient> Ingredients;
};
```

```cpp
#pragma once

#include "CoreMinimal.h"
#include "CraftingRecipeTypes.h"

USTRUCT(BlueprintType)
struct FMissingIngredientInfo
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Crafting")
	FName ItemID = NAME_None;

	UPROPERTY(BlueprintReadOnly, Category = "Crafting")
	int32 RequiredCount = 0;

	UPROPERTY(BlueprintReadOnly, Category = "Crafting")
	int32 AvailableCount = 0;

	UPROPERTY(BlueprintReadOnly, Category = "Crafting")
	int32 MissingCount = 0;
};

UENUM(BlueprintType)
enum class ECraftCheckResult : uint8
{
	CanCraft UMETA(DisplayName = "Can Craft"),
	LevelTooLow UMETA(DisplayName = "Level Too Low"),
	MissingIngredients UMETA(DisplayName = "Missing Ingredients"),
	InvalidRecipe UMETA(DisplayName = "Invalid Recipe")
};

inline bool CanCraftRecipe(
	const FCraftingRecipeRow& Recipe,
	int32 PlayerLevel,
	const TMap<FName, int32>& InventoryCounts,
	ECraftCheckResult* OutFailureReason = nullptr,
	TArray<FMissingIngredientInfo>* OutMissingIngredients = nullptr)
{
	if (OutMissingIngredients)
	{
		OutMissingIngredients->Reset();
	}

	if (Recipe.OutputItemOrBuildingID == NAME_None || Recipe.OutputCount <= 0)
	{
		if (OutFailureReason)
		{
			*OutFailureReason = ECraftCheckResult::InvalidRecipe;
		}
		return false;
	}

	const int32 ClampedPlayerLevel = FMath::Clamp(PlayerLevel, 1, 50);
	const int32 RequiredLevel = FMath::Clamp(Recipe.RequiredPlayerLevel, 1, 50);

	const bool bLevelOk = (ClampedPlayerLevel >= RequiredLevel);
	bool bAllIngredientsOk = true;

	for (const FRecipeIngredient& Ingredient : Recipe.Ingredients)
	{
		if (Ingredient.ItemID == NAME_None)
		{
			continue;
		}

		const int32 NeededCount = FMath::Max(1, Ingredient.Count);
		const int32* FoundCountPtr = InventoryCounts.Find(Ingredient.ItemID);
		const int32 AvailableCount = FoundCountPtr ? FMath::Max(0, *FoundCountPtr) : 0;

		if (AvailableCount < NeededCount)
		{
			bAllIngredientsOk = false;

			if (OutMissingIngredients)
			{
				FMissingIngredientInfo Missing;
				Missing.ItemID = Ingredient.ItemID;
				Missing.RequiredCount = NeededCount;
				Missing.AvailableCount = AvailableCount;
				Missing.MissingCount = NeededCount - AvailableCount;
				OutMissingIngredients->Add(Missing);
			}
		}
	}

	if (!bLevelOk)
	{
		if (OutFailureReason)
		{
			*OutFailureReason = ECraftCheckResult::LevelTooLow;
		}
		return false;
	}

	if (!bAllIngredientsOk)
	{
		if (OutFailureReason)
		{
			*OutFailureReason = ECraftCheckResult::MissingIngredients;
		}
		return false;
	}

	if (OutFailureReason)
	{
		*OutFailureReason = ECraftCheckResult::CanCraft;
	}
	return true;
}

inline bool ConsumeRecipeIngredients(
	const FCraftingRecipeRow& Recipe,
	int32 PlayerLevel,
	TMap<FName, int32>& InOutInventoryCounts,
	bool bRemoveZeroEntries = true,
	ECraftCheckResult* OutFailureReason = nullptr,
	TArray<FMissingIngredientInfo>* OutMissingIngredients = nullptr)
{
	if (!CanCraftRecipe(Recipe, PlayerLevel, InOutInventoryCounts, OutFailureReason, OutMissingIngredients))
	{
		return false;
	}

	for (const FRecipeIngredient& Ingredient : Recipe.Ingredients)
	{
		if (Ingredient.ItemID == NAME_None)
		{
			continue;
		}

		const int32 NeededCount = FMath::Max(1, Ingredient.Count);
		int32* CountPtr = InOutInventoryCounts.Find(Ingredient.ItemID);
		if (!CountPtr || *CountPtr < NeededCount)
		{
			return false;
		}

		*CountPtr -= NeededCount;

		if (bRemoveZeroEntries && *CountPtr <= 0)
		{
			InOutInventoryCounts.Remove(Ingredient.ItemID);
		}
	}

	if (OutFailureReason)
	{
		*OutFailureReason = ECraftCheckResult::CanCraft;
	}
	return true;
}
```