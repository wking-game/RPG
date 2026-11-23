# Stufen-Schubladen für Waffen, Rüstung und Werkzeuge (Entwurf, Stand 2026-11-22)

*Entwurf des Game Directors nach den zwei Skizzen und den Erklärungen des Nutzers (2026-11-22). Eintrag „offen“ in `docs/01_entscheidungen.md`. **Alles hier ist Vorschlag zum Korrigieren**, nichts ist beschlossen. Spielwerte (Schaden, Haltbarkeit) gehören später in `game/Content/Data/`, nicht hierher.*

## 1. Idee in drei Sätzen
- Es gibt etwa 14 **Schubladen**. Jede hat ein **Stufenfenster**: die Stufe, in der ihre Waffen zuerst vorkommen, und die höchste Stufe, bis zu der man sie aufwerten kann.
- **Jede einzelne Waffe** (nicht „Speer“ als Ganzes) steckt in genau einer Schublade. Ein Steinspeer und ein Stahlspeer sind zwei Waffen in zwei Schubladen.
- **Die Tier-Liste ist fest: T1 bis T10** (Nutzer, 2026-11-22), mit Seltenheitsnamen von Common bis Legendary/Mythic. Welche Tiers und Schubladen man nutzen kann, hängt vom **Spielerlevel** ab.
- Die frühere Liste hatte T1 bis T6 (Rohstufe, Raseneisen, Schmiedeeisen, Stahl, Damast, Relikt) samt Angabe, wann Waffen, Rüstung usw. auftauchen. Diese Verteilung wird auf alle Gegenstände übertragen und in die zehn Tiers eingepflegt (offen, wie T1–T6 auf T1–T10 abgebildet werden, Abschnitt 6).

## 2. Schubladen nach der Skizze (Beispiel, Zahlen „nur ein Beispiel“)
Lesart der Tabelle, von links nach rechts. Die Spalten sind meine Ablesung der Handskizze, bitte prüfen.

| Nr | Fenster | Rolle (Vorschlag) |
|---|---|---|
| S0 | T1 | Anfangsausrüstung und Anfangswaffen, keine Aufwertung |
| S1 | T2 | erste Eisenwaffen, nicht aufwertbar |
| S2 | T2–T3 | einfache Eisenwaffen |
| S3 | T2–T3 | einfache Eisenwaffen, zweite Linie |
| S4 | T3–T4 | Schmiedeeisen bis Stahl |
| S5 | T3–T4 | wie S4 |
| S6 | T3–T4 | wie S4 |
| S7 | T4–T5 | Stahl bis Damast |
| S8 | T4–T5 | wie S7 |
| S9 | T4–T6 | bis Relikt |
| S10 | T4–T7 | über T6 hinaus (offen, Abschnitt 6) |
| S11 | T5–T7 | späte Waffen |
| S12 | T5–T8 | späteste Waffen (offen) |
| S13 | T5 | seltene Einzelstücke, nur Damast |

**Korrigiert am 2026-11-22 nach den Beispielen des Nutzers (Abschnitt 8):** Das Fenster sagt, **wo die Waffe im Spiel einsortiert ist** (Stufe, in der man sie findet oder baut; Tabellenstelle). Wie weit sie sich aufwerten lässt, steht **je Waffe** extra und kann über das Fenster hinausgehen. Beispiel Munition: Eine Sorte ist „T1 gelistet“ (Schublade S0). Eine andere ist „T3 gelistet“, aus S4, und lässt sich auf T4 aufwerten.

## 3. Entwurf: Waffen in Schubladen
Grundlage sind die bisherigen Typen aus `design/ASSET_LIST_WAFFEN.md` und die Platzhalter in den Daten. Die Namen der einzelnen Waffen sind Vorschläge, ein Eintrag je Waffe.

| Kategorie | S0 (T1) | S2 (T2–T3) | S4 (T3–T4) | S7 (T4–T5) | S9 (T4–T6) |
|---|---|---|---|---|---|
| Axt einhändig | Steinbeil | Raseneisen-Bartaxt | Schmiede-Bartaxt | Stahl-Bartaxt | Reliktaxt |
| Axt zweihändig | | Breitaxt (Raseneisen) | | Stahl-Breitaxt | |
| Kurzklinge | Knochendolch, Hackbeil | Sax (Raseneisen) | Sax (Schmiedeeisen) | Damast-Sax | |
| Speer | Holzspeer mit Steinspitze | Eisenspeer | | Flügelspeer (Stahl) | Reliktspeer |
| Wurfspeer | Wurfspeer (Stein) | Eisen-Wurfspeer | | | |
| Schwert | | | Schwert (Schmiedeeisen) | Stahlschwert | Reliktschwert |
| Schild | Holzschild | Rundschild (Raseneisen) | Rundschild mit Beschlag | Stahlbeschlag-Schild | Reliktschild |
| Bogen | Kurzbogen | verstärkter Bogen | Hornbogen | | Reliktbogen |
| Armbrust | | | Armbrust (Mechanik braucht Schmiedeeisen) | Horn-Armbrust (T4 als Importware, siehe Entscheidung) | |
| Keule, Schleuder | Keule, Schleuder | | | | |
| Munition | Pfeil (Stein), Schleuderstein | Pfeil (Eisen) | Bolzen (Eisen) | Pfeil, Bolzen (Stahl, Damast) | |

Das ist **kein** vollständiges Raster: Leere Felder heißen „noch nicht festgelegt“, nicht „gibt es nicht“. Die Schubladen S1, S3, S5, S6, S8 und S10 bis S13 sind noch leer. Dort würden seltene, besondere oder spät erreichbare Stücke hinkommen, zum Beispiel Beute- und Bosswaffen.

## 4. Entwurf: Rüstung in Schubladen
Die Rüstung hat acht Plätze mit Mesh (Helm, Schulter, Oberkörper, Hände, Beine, Füße, Umhang, zwei Ringe). Nach den Formfamilien aus `design/ASSET_LIST_RUESTUNG.md` (leicht, mittel, schwer, Relikt):

| Formfamilie | Schublade | Fenster |
|---|---|---|
| leicht (Fell, Stoff, Leder) | S0 | T1 |
| mittel (Gambeson, Eisenbeschläge) | S2 und S4 | T2–T3 bzw. T3–T4 |
| schwer (Kettenhemd, Stahl, Damast) | S7 und S8 | T4–T5 |
| Relikt | S9 | T4–T6 |

Aktuell gibt es acht Rüstungsteile in T1 (Leder) in den Daten. Sie gehören alle zu S0.

## 5. Werkzeuge: Stufenaufstiege bis Stufe 4
Nach Aussage des Nutzers sind Werkzeuge anders: Sie haben **keine Schubladen**, sondern **Stufenaufstiege** bis höchstens **Stufe 4**.

| Stufe | Wirkung (Vorschlag, nur Beispiele) |
|---|---|
| 1 | Anfang: kleine Bäume, kleine Steine, einfache Werkstatt |
| 2 | dickere Bäume, mittlere Felsen |
| 3 | große Felsen, verbesserte Werkstatt nutzbar |
| 4 | die dicksten Bäume und Felsen, beste Werkstatt |

Betroffene Werkzeuge in den Daten: Axt, Spitzhacke, Hammer, Sichel, Schmiedehammer (je `_t1`). **Widerspruch zu Beschlossenem:** Die Zeile „Waffen: Stufen-Leiter“ (2026-11-12) sagt, T1 bis T6 gilt auch für Werkzeuge, und die IDs tragen die Stufe (`tool_axe_t1`). Bei „nur Stufe 4“ gilt das nicht mehr. Wer das ändert, ist der Nutzer.

## 6. Offene Fragen
1. **Beantwortet:** T1 bis T10 ist fest, mit Seltenheitsnamen. **Offen bleibt:** Wie werden die alten T1 bis T6 abgebildet (Vorschlag: bleiben T1 bis T6, T7 bis T10 kommen darüber), wie heißen die Tiers 1 bis 10 (Vorschlag: Common, Uncommon, Rare, Epic, Legendary, Mythic reichen nur für sechs; welche Namen für zehn?) und welche Materialien und Optik haben T7 bis T10? Gerichte, Haltbarkeit von Essen und Rohstoffe haben bisher nur T1 bis T6.
2. **Zahl der Schubladen:** Ich lese 14 ab (S0 bis S13). Stimmt das?
3. **Freischaltung:** Beantwortet: hängt vom **Spielerlevel** ab. Offen: die Level-Schwellen je Tier (Level 1 bis Maximum, siehe `progression.json`) und ob zusätzlich Boss oder Schmiede-Stufe nötig sind. Die alte T1–T6-Liste (wann was auftaucht) liefert die Reihenfolge und wird übertragen.
4. **Werkzeuge:** nur Stufe 1 bis 4, wie in Abschnitt 5? Und gilt das auch für Spitzhacke, Sichel und Hammer?
5. **Aufwertung:** Kostet sie Material der Zielstufe und Schmiede-Stufe (Vorschlag), oder etwas anderes?

## 7. Veranschaulichung des Nutzers: Herstellen und Aufwerten (Beispiel, 2026-11-22)
Der Nutzer hat eine Beispieltabelle eingefügt („gut erklärt“). Die **Namen sind nur Beispiel** und werden nicht übernommen; wichtig sind Aufbau und Mengen. Jede Waffe hat ein Tier-Fenster, ein Rezept aus zwei Zutaten und zwei Aufwertungen:

| Beispielwaffe | Tier-Fenster | Herstellen | Lv. 1 → 2 | Lv. 2 → 3 |
|---|---|---|---|---|
| Cleaver | T1 | 2 A + 2 B | 2 A + 1 B | 3 A + 2 B |
| Voulge | T1–2 | 2 A + 1 B | 2 A + 2 B | 3 A + 2 B |
| Maul | T2 | 2 A + 2 B | 2 A + 3 B | 3 A + 3 B |
| Halbarde | T2–3 | 3 A + 2 B | 3 A + 2 B | 4 A + 3 B |
| Glefe | T3 | 1 A + 1 B | 1 A + 2 B | 2 A + 2 B |
| Sensen-Cleaver | T3–4 | 1 A + 1 B | 2 A + 1 B | 2 A + 2 B |
| Maw-Breaker | T4 | 1 A + 1 B | 1 A + 2 B | 2 A + 2 B |

(A = Hauptzutat, B = Nebenzutat; höhere Tiers brauchen weniger, aber seltenere Zutaten.)

**Meine Lesart (zur Bestätigung):**
- Es gibt **zwei getrennte Achsen**: das **Tier** (Schublade, Seltenheit) und die **Aufwertungsstufe Lv. 1 bis 3** innerhalb der Waffe. Eine reine Tier-1-Waffe hat trotzdem Lv. 1 bis 3.
- Das Fenster „T1–2“ heißt: die Waffe ist beim Herstellen T1 und wird über die Aufwertung bis T2 stark (Deutung, nicht sicher).
- Jede Aufwertung kostet die **gleichen Zutaten wie das Rezept**, in wachsender Menge. Beim Herstellen von höheren Tiers sind es weniger, aber seltenere Zutaten (Boss- und Kreaturenteile statt Eisen).
- Das passt zu den Kreaturen-Drops in `items_creature.json` und zum Wiederholungs-Abfall bei Bossmaterial.

**Nutzer-Klarstellung (2026-11-22):** Das Beispiel nutzt Kreaturenteile als Zutaten, das Muster (zwei Zutaten, Herstellen, Aufwertung Lv. 1 bis 3 mit wachsender Menge, höhere Tiers brauchen weniger, aber seltenere Zutaten) gilt für **alle Waffen und Rüstungen**, nicht nur für Kreaturenwaffen. **Zweite Klarstellung (2026-11-22):** Das Tier- und Aufwertungssystem gilt **nur für die fertige Waffe oder Rüstung**, nicht für Komponenten oder Zutaten. Die Rezepte und Mengen in der Tabelle sind nur Veranschaulichung des Prinzips; welche Zutaten ein Rezept hat, regeln die Rezepte selbst, nicht die Tier-Liste.

## 8. Beispiele des Nutzers: Einordnung und Aufwertung je Waffe (2026-11-22)
Drei Beispiele zeigen, dass **Fenster (Einordnung)** und **Aufwertung** zwei getrennte Eigenschaften je Waffe sind:

| Beispiel | Einordnung (Schublade) | Aufwertung |
|---|---|---|
| Kurzer Bogen mit leichten Jagdpfeilen, reguläre Materialien | T1 (Common) | **keine**, bleibt T1 |
| Panzer-Armbrust mit schweren Bolzen, aus Kreaturenteilen | T2–T3 (Uncommon/Rare) | **zweimal** in sich aufwertbar |
| Sensen-Cleaver | T3–T4 (Mittelspiel, so in der Tabelle einsortiert) | aufwertbar **bis T6**, ab T3 und so weiter |

**Meine Lesart (zur Bestätigung):**
- Das Fenster (T1, T2–T3, T3–T4 …) bestimmt, **wann** eine Waffe im Spiel auftaucht (abhängig vom Spielerlevel). Das ist die Tabellenstelle.
- Die **Aufwertung ist je Waffe eingestellt**: manche gar nicht (T1-Waffen), manche zweimal, manche bis T6 oder höher. Sie kann über das Fenster hinausgehen.
- Daten-Vorschlag je Waffe: `tier_min`, `tier_max` (Fenster) und `max_upgrade_tier` (oder Anzahl der Aufwertungen).
- Munition (Pfeile, Bolzen) gehört zur Waffe und bekommt ein eigenes Tier-Fenster.

**Nutzer-Antwort (2026-11-22, „auch Tiers, aber bis 10 gecapt“):** Eine Aufwertung hebt die Waffe um **ein Tier** (kein getrennter Lv.-Zähler); die Obergrenze ist **T10**. Die „Lv. 1 → 2 → 3“-Zeilen der Beispieltabelle (Abschnitt 7) sind demnach Tier-Aufstiege. Damit ist „zweimal aufwertbar“ = zwei Tiers höher (Armbrust T2–T3 würde bis T4/T5 gehen, je nach Einstiegstier), und der Sensen-Cleaver geht T3 → T4 → T5 → T6. Die Obergrenze je Waffe (`max_upgrade_tier`) liegt höchstens bei T10. Zu klären: Beginnt „zweimal“ bei der unteren oder oberen Fenster-Stufe?

**Nutzer-Klarstellung (2026-11-22):** Jedes Tier einer Waffe ist eine **eigene Fassung mit eigenen verbesserten Werten (Stats)** und **leicht angepasster Optik**. Eine Aufwertung wechselt in die nächste Fassung. Folgen: Pro Waffe eine Tabellenzeile je Tier in den Daten (Werte steigen), das Aussehen entsteht über Material, Farbe und kleine Zusatzteile, nicht über neue Meshes je Tier (passt zur Teile-Strategie in `design/ASSET_LIST_WAFFEN.md` Abschnitt 4 und zum 20-GB-Budget).

## 9. Eingefügter Entwurf einer anderen KI (T1–T6 über alle Waffen und Rüstungen, 2026-11-22)
Der Nutzer hat einen langen Text eingefügt (Herkunft: ein anderer KI-Assistent, vermutlich Design-Seite). **Nichts daraus ist übernommen**; Namen und Zahlen stehen dort ausdrücklich als „illustrative Richtwerte“. Falls Inhalte übernommen werden, gehören sie in `design/asset_log.md` (KI-Inhalte).

**Was den Aufbau bestätigt oder ergänzt:**
- Je Waffenkategorie eine **10er-Reihe #1 bis #10**. #1 bleibt T1 und wird nicht aufgewertet, #2 geht bis T2, #3 bis T3, #4 bis T4, #5 bis #7 bilden den Kern T4 bis T6, #8 bis #10 führen über T6 hinaus. Das entspricht den Schubladen aus Abschnitt 2 (die Reihe #n ist die n-te Schublade einer Kategorie).
- **Neun Kategorien** mit je 10 Waffen: Kurzschwert, Breitaxt, Kriegspickel, Stangenwaffe, Langschwert, Schwerer Hammer, Runenschwert, Bögen, Armbrust, dazu Munition (= 100 Waffen). Zwei Herkunftslinien je Stufe: **regulär** (Schmiedearbeit) und **aus Kreaturenteilen** (z. B. Knochen, Dorn, Panzerplatte).
- **Rüstung** in drei Gewichtsklassen **leicht, mittel, schwer**, je T1 bis T6; „nicht hochlevelbar“ bei T1.
- **Richtwerte Stats gegenüber T1:** T2 +15–20 %, T3 +35–45 %, T4 +55–70 %, T5 +80–95 %, T6 +105–125 %. Drop-Chancen sinken von 30–45 % (T1) auf 1–5 % (T6).

**Was nicht zum Projekt passt oder offen ist:**
1. **Namen:** Fantasy-Phantasienamen („Riftbone“, „Carapace“, „Umbral“, „Abyssal“, „Ember“) passen nicht zum nordisch-historischen Stil (`design/stil_norden_gezeitenhall.md`, Stufen-Leiter Raseneisen bis Damast). Vorschlag: nur die Struktur nutzen, Namen nordisch neu.
2. **Kategorien:** Der Text hat Kriegspickel, Stangenwaffe, Langschwert, Schwerer Hammer, Runenschwert. Beschlossen sind Bartaxt, Sax, Speer, Schwert, Rundschild, Bogen, Pfeile, Armbrust. Rundschild und Wurfwaffen fehlen im Text. Welche Kategorien es gibt, entscheidet der Nutzer.
3. **Magie:** „Runenschwert“ berührt die Magie-Entscheidung (Gespür/Lauschen); nicht still übernehmen.
4. **Munition** als eigene Kategorie mit 10er-Reihe ist viel (Mengenabschätzung offen).
5. **Umfang:** 100 Waffen mit je 6 oder mehr Fassungen sind weit über den bisherigen MVP (7 Typen) und das Asset-Budget; die Teile-Strategie (wenige Meshes, Stufen über Material) bleibt Pflicht.
6. **Tiers 7 bis 10** sind im Text nur angedeutet („Spitzen-Tiers“), nicht ausgearbeitet.

## 10. Optimiertes Schubladensystem für Waffen und Rüstung (Entwurf, 2026-11-22)
Der Nutzer hat die Werte aus Abschnitt 9 gutgeheißen, die Phantasienamen ausdrücklich gewollt („Mittelalter-Fantasy mit nordischen Elementen, kein Realismus“, damit gilt Punkt 1 aus Abschnitt 9 nicht mehr) und das Schubladensystem für Waffen und Rüstung freigegeben mit dem Auftrag, es dafür zu optimieren. Vorschlag des Game Directors, ein gemeinsames Modell:

### 10.1 Gemeinsame Regeln
1. **Eine Schublade = ein Eintrag je Kategorie.** Jede Kategorie (Waffe oder Rüstungsteil) hat feste Positionen #1 bis #10. Eine Position ist **eine einzelne Waffe oder ein einzelnes Rüstungsteil** mit Namen, Herkunftslinie und eigenem Tier-Fenster.
2. **Fenster = Einordnung** (ab welchem Spielerlevel und in welchen Tiers es die Waffe gibt). **Aufwertung = je Waffe eingestellt**, ein Tier pro Schritt, höchstens T10. #1 (T1) ist nicht aufwertbar.
3. **Jedes Tier ist eine eigene Fassung** mit eigenen Werten und leicht angepasster Optik (Material, Farbe, kleine Zusatzteile; ein Modell je Waffe).
4. **Zwei Herkunftslinien:** regulär (Schmiede, Leder, Stoff) und Kreaturenteile. Beide haben dieselben Tier-Werte; sie unterscheiden sich nur in den Zutaten und den Beute-Orten.
5. **Rüstung:** je Slot (Helm, Schulter, Oberkörper, Hände, Beine, Füße, Umhang, Ringe) drei Gewichtsklassen leicht, mittel, schwer; gleiche Positionen und Tier-Regeln wie bei Waffen. Rüstung hat Schutz- und Haltungswerte statt Schaden.

### 10.2 Stat-Kurve je Tier (Startwerte, Mittelwerte aus dem eingefügten Entwurf)
Faktor gegenüber T1 und Drop-Chance für Funde (Beute in der Welt); T7 bis T10 sind **eigene Verlängerung** (Vorschlag), Seltenheitsnamen ab T4 ebenfalls Vorschlag (T1 Common, T2 Uncommon, T3 Rare sind aus den Beispielen des Nutzers).

| Tier | Seltenheit | Stat-Faktor | Drop-Chance | Spielerlevel (aus `progression.json`) |
|---|---|---|---|---|
| T1 | Common | 1,00 | 30–45 % | 1–8 |
| T2 | Uncommon | 1,18 | 18–30 % | 9–16 |
| T3 | Rare | 1,40 | 12–22 % | 17–25 |
| T4 | Superior | 1,62 | 6–16 % | 26–34 |
| T5 | Epic | 1,88 | 3–9 % | 35–42 |
| T6 | Heroic | 2,15 | 1–5 % | 43–50 |
| T7 | Legendary | ca. 2,45 | 1–3 % | **offen** |
| T8 | Ancient | ca. 2,75 | 0,5–2 % | **offen** |
| T9 | Exalted | ca. 3,05 | 0,3–1 % | **offen** |
| T10 | Mythic | ca. 3,35 | 0,1–0,5 % | **offen** |

### 10.3 Was beim Optimieren auffällt
1. **Level-Obergrenze 50 deckt nur T1 bis T6** (`progression.json`: Tier 1 Level 1–8 bis Tier 6 Level 43–50, jede Stufe mit einem Biom). Für T7 bis T10 gibt es kein Level. Möglichkeiten: Level-Obergrenze anheben, oder T7 bis T10 nur durch **Aufwertung** über Level 50 hinaus und durch Endgame-Beute (Prestige-Stufen). Entscheidung des Nutzers.
2. **Zehn Biome** wären ein zweites Problem: Bisher ist jede Tier-Stufe an ein Biom gebunden (6 Biome). T7 bis T10 bräuchten Orte oder Bosse.
3. **Reihe #1 bis #10 und Tier-Fenster** hängen zusammen: #n beginnt in Tier n (bis T6); #7 bis #10 beginnen höher. Das stimmt mit der Skizze (Anfang T1, später breitere Fenster) überein.
4. **Umfang:** Je Kategorie zehn Waffen mit bis zu sechs Tier-Fassungen sind viel Datenarbeit, aber nur Zeilen in JSON. Die Modelle bleiben wenige (Teile-Strategie).
5. **Datenformat:** je Waffe `tier_min`, `tier_max`, `upgrade_max_tier`, `line` (`regular`/`creature`), `slot` (bzw. Kategorie), `stats` je Tier als Faktor aus der Kurve; keine Einzelwerte von Hand.

### 10.4 Noch offen
- Welche Waffenkategorien gibt es genau (Beschluss: 8; Eingefügter Text: 9 plus Munition)?
- Namen der zehn Stufen, wenn die Vorschläge ab T4 nicht gefallen.
- Level für T7 bis T10 (10.3, Punkt 1) und Orte (Punkt 2).
- Welche konkreten Waffen auf welche Position kommen: Der Entwurf aus Abschnitt 9 ist eine Quelle, muss auf die bestätigten Kategorien zugeschnitten werden.

### 10.5 Kategorien (Nutzer: „ergänzen“, 2026-11-22)
Die acht beschlossenen Waffentypen bleiben, ergänzt um zwei Kategorien (meine Deutung von „ergänzen“, zur Bestätigung):
Bartaxt (einhändig), Sax (Kurzklinge), Speer, Schwert, Rundschild, Bogen, Armbrust, Munition (Pfeile und Bolzen, gehört zu Bogen und Armbrust), **neu: Wuchtwaffe** (Keule, Hammer), **neu: Stangenwaffe** (Hellebarde, Glefe). Breitaxt (zweihändig) und Wurfspeer/Schleuder aus `design/ASSET_LIST_WAFFEN.md` bleiben Kandidaten. Runenschwert und Kriegspickel aus dem eingefügten Text sind **nicht** übernommen. Gesamt: zehn Kategorien mit je bis zu zehn Waffen.

## 11. Lösung: Gefahrenstufe statt Biom-Kopplung (Entwurf, 2026-11-22)
**Auftrag des Nutzers:** Kreaturen sollen nicht immer an ihr Kernbiom gebunden sein; die feste Zuordnung Stufe ↔ Biom (sechs Biome, sechs Stufen) „sprengt“ sonst das System; Gegenstände sollen bis T10 gehen und über einen **schwereren Weg** zu bekommen sein.

**Befund in den Daten (nachgesehen):**
- `creatures.json`: jede Kreatur hat `home_biome`, `min_rank` und eine Liste `ranks` mit Werten je Rang (derzeit Rang 1 bis 6).
- `spawns.json`: Zeilen je (Biom, Kreatur) mit `weight`, `scale_to_zone`, `scale_chance`; der Rang folgt dem **Biom-Rang** (`rank` je Biom, `place_rank` für gefährliche Orte).
- `creature_loot.json`: Grund- und Rang-Beute hängen am **Rang** (`min_rank`), nur die Zusatz-Beute hängt am Biom.
- Das Datenformat erlaubt schon eine Kreatur in mehreren Biomen (eine Zeile je Biom). Gebunden ist vor allem der **Rang an das Biom**.

**Vorschlag: drei getrennte Achsen**
1. **Biom** = Aussehen, Klima, Rohstoffe, Ort der Geschichte. Es bestimmt keine Stärke mehr.
2. **Gefahrenstufe 1 bis 10 als Kartenebene (Zone).** Jede Zone hat eine Gefahrenstufe, unabhängig vom Biom. Das Biom liefert nur den **Standardwert** (bisher 1 bis 6); Zonen können ihn überschreiben (gefährliche Orte, Prüfungsorte, Gipfel, Wracks). Gefahr 7 bis 10 gibt es nur an wenigen besonderen Orten in beliebigen Biomen (Endgame).
3. **Kreatur** hat eine **Rangspanne** (`min_rank` bis `max_rank`, schon vorhanden) und eine **Heimat** nur als Gewichtung. Neu: eine Chance `wander_chance`, dass sie auch in fremden Biomen erscheint (z. B. 10 bis 25 %); ihr Rang folgt der Gefahrenstufe der Zone, begrenzt durch die Spanne.

**Beute und Tier der Gegenstände:**
- **Tier eines Fundes = Rang der getöteten Kreatur**, mit prozentualer Streuung (Beispiel Rang 4: meist T4, oft T3, selten T5 oder T6, sehr selten höher). Drop-Chancen wie in Abschnitt 10.2; bei Bossen die Wiederholungs-Chancen aus „Saga: Bossbeute bei Wiederholung“, verlängert auf T7 bis T10.
- **Schwerer Weg für bessere Tiers:** höhere Tiers fallen nur bei Elite-Zielen (Veteran, Anführer, Bosse) und aus gefährlichen Zonen; zusätzlich braucht die Aufwertung Teile der Stufe darunter plus ein seltenes Teil, das nur aus solchen Zielen kommt.
- **Level** schaltet nur das **Benutzen und Herstellen** frei (bleibt wie bestätigt), nicht das Finden. Die Zuordnung Level 1–50 ↔ T1–T6 bleibt; T7 bis T10 brauchen Level 50 (Vorschlag).

**Was das ändert (Aufwand):**
- `spawns.json`: Zonen mit `danger`; Biom-`rank` wird zum Standard. `wander_chance` je Zeile neu.
- `creatures.json`: `ranks` bei Bedarf bis Rang 10 erweitern (Rechnung statt Handarbeit: Wachstum pro Rang wie bisher, etwa LP ×1,35).
- `progression.json`: Feld `biome` je Stufe wird Hinweis, nicht Regel.
- Story, Kapitel und Bosse bleiben an den sechs Biomen. Die Doku `docs/21` Abschnitt 2.1 wird „Standardwerte“ statt „feste Zuordnung“.
- Kein Umbau der Beute-Logik nötig, sie läuft schon über den Rang.

**Folge für die „prozentuale Beute“:** Sie bleibt, wird aber nach Rang gewürfelt und nicht mehr nach Biom.

**Entscheidung des Nutzers (2026-11-22, nach Abschnitt 11):** Es gilt die einfachere Fassung. Welt und Gegner bleiben an die Biome gebunden; nur das **Tier der Gegenstände** wird prozentual nach dem Rang des Gegners gewürfelt (Rang 6 und gefährlichste Orte bis T7–T10, kleine Chancen; Bosse und Elite-Ziele als Zugang zu den hohen Tiers). Der Entwurf mit Gefahrenstufe als Kartenebene und Wander-Chance der Kreaturen in Abschnitt 11 ist damit **ersetzt** und wird nicht umgesetzt. Offen bleibt, ob Kreaturen gelegentlich außerhalb ihres Heimat-Bioms vorkommen sollen.
