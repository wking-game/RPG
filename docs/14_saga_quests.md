# Saga-System und Questlinien (Entwurf)

Stand 2026-11-15. **Alles in diesem Dokument ist ein Vorschlag von Claude**, solange es nicht in `docs/01_entscheidungen.md` als bestätigt steht.
Grundlagen: `docs/01_entscheidungen.md` (Magie, Scan, Bewohner: Tod), `game/Content/Data/listen.json` (Lauschen, Stufen, EP, Boni; von der Gameplay-Session), `design/stil_norden_gezeitenhall.md` (Gezeitenhall, Zeichen), `design/ASSET_LIST_GESAMT.md` (Saga-Buch, Fragmente, Skalde, Raben, Bosse, Jarlssitz).
Hinweis: `docs/13_magie_gespuer.md` gibt es noch nicht; die Gespür-Zahlen hier stammen aus `listen.json`.

**Vorgabe des Nutzers (2026-11-15, über den Game Director):** keine Story-Spiel-Lastigkeit. Die Story ist ein roter Faden, an dem man sich entlanghangeln kann. Erkunden, Überleben und Bauen stehen im Vordergrund. Es soll viele Arten von Nebenquests geben (Gegenstände finden, erste Magie finden und ausprobieren, erkunden). Questgegenstände gibt es, aber unauffällig. Dazu kommt ein Horden-Element. Gewünscht sind **5 verschiedene Storyline- und Saga-Vorschläge**, siehe **Abschnitt 10**. Die Abschnitte 1–9 sind der erste Entwurf und entsprechen ungefähr Vorschlag A. Sie gelten nur, wenn A oder eine Mischung mit A gewählt wird.
Ergänzung des Nutzers (2026-11-15): Die Questline darf doch der Hauptfokus sein, aber nur, wenn die Story wirklich fesselnd, sehr gut geschrieben und immersiv ist. Sonst bleibt sie roter Faden. Vorschlag von Claude: erst eine Probe schreiben (Prolog eines Vorschlags als fertiger Spieltext) und daran entscheiden.

Leitsätze:
- **Lauschen ist der Schlüssel zur Saga.** Magische Spuren führen in die Geschichte, die Geschichte führt zu besseren Spuren.
- **Kein Zeitdruck** (Regel 6): Keine Quest läuft ab, keine scheitert. Nacht, Nebel, Winter und Gezeitenhall-Ereignisse kommen wieder.
- **Solo ist Koop mit einem Spieler** (Regel 2): Jede Quest ist allein lösbar; zu mehreren geht es schneller oder bequemer, nicht anders.
- **Klein anfangen** (Regel 9): Im ersten Schritt nur der Kern und eine Questlinie (Prolog + Dorf-Seherin).

---

## 1. Begriffe

| Begriff | Was es ist | Gehört | Beispiel |
|---|---|---|---|
| **Saga** | Die gesamte Chronik **einer Welt**: alle Stränge, Lieder, Siedlungs-Einträge und Taten. Das Saga-Buch zeigt sie | Welt | „Die Saga von Birkenvik“ |
| **Strang** (Questlinie) | Eine zusammenhängende Geschichte aus mehreren Etappen | Welt | „Der gebrochene Ring“, „Die Stimme im Seeglas“ |
| **Etappe** (Quest) | Ein Abschnitt eines Strangs mit einem oder mehreren Zielen | Welt | „Das Summen am Strand“ |
| **Ziel** | Ein prüfbarer Schritt einer Etappe (hingehen, lauschen, sammeln, bauen, sprechen, besiegen …) | Welt | „Finde drei Seeglas-Splitter“ |
| **Lied** | Ein alter Text in Strophen, verteilt auf Fragmente. Ein vollständiges Lied verrät etwas (Ort, Bauplan, Questbeginn) | Welt | „Lied der Fjordwiesen“ |
| **Fragment** | Eine Strophe eines Lieds, gefunden an Runensteinen und in Ruinen (`item_saga_fragment_*`) | Welt | Strophe 2 des Fjordwiesen-Lieds |
| **Tat** | Ein Eintrag über etwas Bemerkenswertes. Entsteht **automatisch** aus dem Spiel, nicht nur aus Quests | Welt **oder** Spieler | „Im ersten Winter fiel Ragna bei der Verteidigung des Lagers.“ |

Merksatz: **Stränge bestehen aus Etappen, Etappen aus Zielen. Lieder bestehen aus Fragmenten. Taten schreibt das Spiel selbst.**

## 2. Die vier Ebenen der Saga

| Ebene | Inhalt | Fortschritt | Ende |
|---|---|---|---|
| **Hauptsaga** „Der gebrochene Ring“ | Die Geschichte des Gezeitenhalls, in 3 Akten über die 6 Biome, mit den 6 Bossen | gemeinsam für die Welt | Saga-Finale (Sturmkoloss), Jarlssitz |
| **Nebenstränge** | Kürzere Geschichten an Orten und Personen (Dorf-Seherin, Fischerdorf, Schwarzmarkt …) | gemeinsam für die Welt | eigene Belohnung (Freischaltung, Bauplan, Bewohner) |
| **Siedlungs-Saga** | Chronik des eigenen Dorfs: Gründung, erstes Langhaus, erster Bewohner, erster Winter, erstes Fest, Gefallene | automatisch, Welt | nie (wächst mit) |
| **Persönliche Taten** | Was **ein Spieler** getan hat: Gespür-Stufen, erste Funde, Boss-Treffer, verborgene Strophen | automatisch, pro Spieler | nie |

Die **Biom-Stränge** sind Teil der Hauptsaga (je Biom: Lied finden → Steinkreis lauschen → Boss). Wer nur bauen will, kann die Hauptsaga liegen lassen; die Siedlungs-Saga schreibt sich trotzdem.

## 3. Hauptsaga: „Der gebrochene Ring“ (Story-Vorschlag)

Kurzfassung: Lange vor den Siedlern band das Küstenvolk den Gezeitenhall in einen **Ring aus Steinkreisen**, einen in jedem Land. Der Ring ist gebrochen – daher das Zeichen: **ein offener Ring mit drei Kerben**. Seitdem steigen und fallen die Gezeiten unruhig, und an den alten Kreisen sammeln sich Menschen, die die Kraft für sich wollen (Plünderer, der Aschebund, der Seekönig). Die Siedler hören das Summen, finden die Lieder der alten Kreise und stellen sich denen, die die Kreise besetzt halten.

| Akt | Biome und Bosse | Kerbe |
|---|---|---|
| Prolog | Startgebiet: das Summen am Strand, der erste Steinkreis, die Raben | – |
| Akt I „Die Flut“ | Fjordwiesen (Der Alte Keiler), Gezeitenmarsch (Die Schlickmutter) | 1. Kerbe leuchtet |
| Akt II „Die Gräber“ | Hügelgrab-Heide (Der Hügelkönig), Basaltfjell (Der Felsenvater) | 2. Kerbe leuchtet |
| Akt III „Der Sturm“ | Glutküste (Die Aschezunge), Eisrand (Der Sturmkoloss) | 3. Kerbe leuchtet, Saga-Finale |

Je Biom läuft dasselbe Muster (gut für Daten und Umfang):
1. **Hinweis:** Raben kreisen über dem neuen Land, der Skalde oder die Seherin erzählt davon.
2. **Lied finden:** 3–5 Fragmente an Runensteinen und Ruinen. Ein Teil davon ist „hallend“ (nur durch Lauschen zu finden).
3. **Steinkreis lauschen:** Das vollständige Lied verrät den Ort des Kreises. Dort aktiv lauschen (Steinkreis +8 EP beim ersten Mal).
4. **Boss:** Der Kreis zeigt die Arena. Sieg → Bossmaterial (nächste Stufe T2–T6), Trophäe, Saga-Bild.

**Bosse und Sagenwesen:** Alle Bosse treten in der Sagen-Fassung auf, nur der Alte Keiler bleibt ein Tier; Fantasy-Kreaturen sind Standard-Gegner, die Weltoption „Sagenwelt“ entfällt (bestätigt 2026-11-22; ersetzt „zuerst Menschen und Tiere, Sagenwesen nur als Weltoption“ vom 2026-11-13). Die Sagenwesen gab es schon immer, die alten Lieder kennen sie. Weil der Ring gebrochen ist, sickert der Gezeitenhall durch, und an starken Orten sind sie wach. Die Magie der Spieler bleibt leise (Lauschen, Seeglas, Nebelbilder). Es gibt nur eine Textfassung; die frühere Idee eigener Varianten je Fassung (`text_sagenwelt`) entfällt. Wie die Story die Bosse einbindet: `docs/17_hauptstory_kapitel.md`.

**Offen (für später):** Was passiert im Finale mit dem Ring? Er schließt sich (die Kraft wird gebändigt) oder bleibt offen (die Kraft bleibt wild, die Siedler leben mit ihr). Das ist eine erzählerische Entscheidung des Nutzers.

## 4. Einbindung des Magie-Gespürs (Lauschen)

### 4.1 Erfahrung (EP) – keine neuen Quellen
Die Quellen aus `listen.json` reichen und werden nicht vermehrt: **neuer Fund +2, Seeglas +1, Steinkreis +8, Gezeitenhall-Ereignis +15**, je Fund und Spieler nur einmal.
- Ein **hallendes Fragment** zählt als „neuer Fund“ (+2).
- **Quests geben selbst keine Gespür-EP.** Sie führen zu Steinkreisen, Seeglas und Ereignissen; dort entsteht die EP. So gibt es keine doppelte Zählung, und die Rechnung bleibt durchschaubar.
- Die Dorf-Seherin (+25 %) und die Weltoption „Gespür-Tempo“ wirken wie in `listen.json`.

Rechenbeispiel Solo (Schwellen 0 / 10 / 30 / 75 / 160): Prolog mit 3 Seeglas (+3), 2 Funden (+4) und dem ersten Steinkreis (+8) = 15 EP → **Stufe 2 „Ahnung“** am Ende des Prologs. Die Seherin-Quest bringt etwa weitere 15–20 EP → **Stufe 3 „Deutung“** nach etwa 3–4 Stunden. Das passt zur Annahme in `listen.json`.

### 4.2 Lauschen findet Questziele
- Neuer Ziel-Typ **„lauschend finden“**: Das Ziel ist ein magisches Objekt (Tag `GamerMagic`). Das Saga-Buch nennt nur ein **Gebiet** (Kreis auf der Karte, Text wie „wo die Raben über dem Moor kreisen“), **keinen genauen Marker**. Den Rest erledigt das Lauschen: passiv heiß/kalt am Bildschirmrand, aktiv der türkise Ring.
- **Die Hauptsaga verlangt höchstens Stufe 2.** Niedrige Stufe heißt: länger suchen, nie: festsitzen. So bleibt die Saga für jede Herkunft und für Solo lösbar.
- Höhere Stufen öffnen **Zusätze**, nicht den Weg:
  - **Stufe 3 Deutung:** Name und Menge im Ring; das Saga-Buch zeigt, welche Strophe fehlt.
  - **Stufe 4 Klarblick:** **verborgene Strophen** (Tag `GamerMagicHidden`): jedes Lied hat eine. Sie erzählen, was die anderen verschweigen, und geben einen kleinen Bonus (z. B. Bauplan für Seeglas-Schmuck, ein Saga-Bild, einen Hinweis auf einen Schatz).
  - **Stufe 5 Seidr-Blick:** **seltene Funde** (Tag `GamerMagicRare`): Gezeichnete Orte mit klarem Seeglas für Relikt-Gegenstände.
- **Die Karotte:** Die Boni aus `listen.json` machen höhere Stufen früher erreichbar: Wer Stufe 3 hat, sieht **nachts oder im Nebel am Steinkreis** (+1) schon die verborgene Strophe. Das Spiel sagt das leise an („In klaren Nächten hört man am Kreis mehr.“). Weil Nacht und Nebel wiederkommen, ist das ein Grund wiederzukommen, kein Zeitdruck.

### 4.3 Die Quest der Dorf-Seherin (`quest_village_seer`)
Nebenstrang **„Die Stimme im Seeglas“**. Schaltet die Bedingung `unlock:quest_village_seer` aus `listen.json` frei.

| # | Etappe | Ziele | Gespür |
|---|---|---|---|
| 0 | Start | nach dem Prolog (erster Steinkreis gefunden) – Raben fliegen zum Opferstein | – |
| 1 | „Die Frau am Opferstein“ | zum Opferstein gehen, mit der Seherin (`npc_seeress`) sprechen | – |
| 2 | „Was das Meer zurücklässt“ | 3 Seeglas-Splitter lauschend finden und ihr bringen | Seeglas +1 je Stück |
| 3 | „Wo der Ring summt“ | am Steinkreis lauschen, **nachts oder im Nebel** (wartet geduldig, bis die Bedingung eintritt; Hinweis im Buch) | Steinkreis +8 (beim zweiten Kreis); Bonus +1 Stufe dort |
| 4 | „Ein Haus für die Seherin“ | Seherhütte (`building_seer_hut`) im Dorf bauen | – |
| 5 | Abschluss | Sie zieht ein und wird Bewohnerin mit Beruf `job_seer` | ab jetzt +25 % EP für alle |

- Die Seherin ist **danach eine Bewohnerin** wie andere (Bedürfnisse, kann sterben). Stirbt sie, gilt die Bedingung `job:job_seer` nicht mehr; eine neue Seherin kommt nur über einen Bewohner, dem man den Beruf gibt (Vorschlag: nur möglich, wenn die Quest einmal abgeschlossen ist).
- Spieler-Herkunft Seherin (`origin_seer`, Start auf Stufe 2) bringt hier nur Bequemlichkeit, keinen anderen Weg.
- Hinweis: Etappe 3 braucht Tag/Nacht oder Wetter. Bis das existiert, kann sie auf „Winter“ oder „ohne Bedingung“ gestellt werden (Daten, kein Code).

### 4.4 Die drei Kerben
Das Saga-Buch trägt das Zeichen auf dem Deckel: **offener Ring mit drei Kerben**. Jeder abgeschlossene Akt der Hauptsaga lässt **eine Kerbe in Gezeitenlicht** (`#A9E9DC`) leuchten. Das ist für alle Spieler der Welt gleich (Welt-Fortschritt). Die eigene Gespür-Stufe zeigt das Buch getrennt davon (Abschnitt 6), damit „wie weit ist die Welt“ und „wie gut höre ich“ nicht verwechselt werden.

### 4.5 Gezeitenhall-Ereignis (+15)
Vorschlag: An einem Steinkreis, dessen Lied vollständig ist, tritt **bei Springflut** (alle paar Spieltage, Nacht) ein Ereignis auf: Ringe auf dem Wasser, Nebel in Bögen, ein kurzer Puls. Wer dabei lauscht, bekommt einmalig je Kreis +15. Raben kündigen es einen Spieltag vorher an. Verpassen kostet nichts; es kommt wieder. **Braucht Tag/Nacht und Gezeiten**, also nicht im ersten Schritt.

## 5. Koop und Solo (überholt)

> **Überholt am 2026-11-15:** Queststand pro Spieler, Folgen für die Welt addieren sich, Gespräche mit Entscheidungen (siehe Abschnitte 11 und 12 sowie `docs/01_entscheidungen.md`). Die Tabelle bleibt zum Nachlesen.

| Frage | Regel (Vorschlag) |
|---|---|
| Wem gehört der Quest-Fortschritt? | **Der Welt.** Jeder Spieler kann jedes Ziel erfüllen; alle sehen denselben Stand. Wer später beitritt, sieht den Fortschritt, den die anderen schon gemacht haben |
| Wer bekommt die Belohnung? | **Welt-Belohnungen** (Freischaltungen, Baupläne, Bewohner, Gegenstände) gehen an die Siedlung, Gegenstände ins **Lager**. Kein Streit, wer den letzten Schlag hatte |
| Und was bekommt der Einzelne? | **Taten** (Saga-Buch, unter seinem Namen) und **Gespür-EP** aus den Funden, wie in `listen.json` (gemeinsamer Fund: Mitspieler im Umkreis von 30 m bekommen die Hälfte) |
| Fragmente und Questgegenstände | gehen beim Aufnehmen **direkt ins Saga-Buch der Welt**, nicht ins Inventar: kein Gewicht, kein Verlust beim Tod, kein „der Freund mit dem Logbuch ist offline“ |
| Gespräche | Jeder Spieler kann mit Quest-Personen sprechen. Im ersten Schritt **keine sich ausschließenden Entscheidungen** in Gesprächen (sonst müsste man klären, wer für die Gruppe entscheidet) |
| Koop-Bonus | nur Bequemlichkeit: gemeinsames Lauschen (größere Reichweite, `group_range_mul`), gemeinsamer Fund. Keine Quest verlangt zwei Spieler |
| Zeit | keine Fristen. Bedingungen wie „nachts“ sind Fenster, die wiederkommen. Kein Ziel kann scheitern |

## 6. Das Saga-Buch (`ui_saga`)

Öffnen mit eigener Taste (Vorschlag J, umbelegbar). Pergament-Seiten nach der Stilvorgabe, Zeichen auf dem Deckel. Fünf Teile:

1. **Saga:** aktive Stränge mit der aktuellen Etappe und ihren Zielen. Ziele als kurze, erzählende Sätze; Gebiet auf der Karte als Kreis. Abgeschlossene Etappen als kurzer Rückblick.
2. **Lieder:** jedes Lied mit seinen Strophen; fehlende Strophen als Lücke „…“. Ab Stufe 3 steht dabei, wie die fehlende zu finden ist („hallend“, „verborgen“).
3. **Siedlung:** Chronik des Dorfs mit Jahr und Jahreszeit („Im zweiten Winter …“). Gefallene Bewohner bekommen **immer** einen Eintrag (Name, Tat, Zeitpunkt). Zusätzlich kann man einen **Gedenkstein** (`building_runestone`) bauen, der auf den Eintrag verweist. (Bestätigt ist „Saga-Eintrag oder Gedenkstein“; Vorschlag hier: Eintrag immer, Stein freiwillig.)
4. **Taten:** je Spieler eine Seite mit seinen Taten.
5. **Gespür:** eigene Stufe (Name, Balken bis zur nächsten Stufe), gerade wirkende Boni (Amulett, Nacht, Dorf-Seherin …), die drei Kerben des Welt-Fortschritts.

Der **Skalde** (`npc_skald`, später ein Bewohner-Beruf an der Taverne) liest bei Festen Einträge aus der Siedlungs-Saga vor. Vorschlag für später: Das gibt Stimmung, keine neue Währung.

## 7. Datenformat (Entwurf, angepasst an die Antworten vom 2026-11-15)

Nach `docs/06_datenformat.md`: eine JSON-Datei pro Tabelle in `game/Content/Data/`, Aufbau `{ "schema", "rows" }` (Einstellungsdateien wie `listen.json` als einzelnes Objekt), IDs `snake_case`, nie wiederverwenden, Wegfallendes bekommt `"deprecated": true`.
Grundlage sind die bestätigten Punkte:
- alles spawnt nach;
- Bossbeute: beim ersten Sieg sicher, danach mit Chance je Stufe;
- Gespräche mit Entscheidungen, große Folgen in der Hauptstory;
- Folgen für die Welt addieren sich;
- Questhilfe über Gespräche, im Menü abschaltbar;
- alles vertont (Deutsch und Englisch).

Voraussetzung dafür ist der Queststand pro Spieler. Er ist noch Vorschlag, wird aber von den anderen Antworten vorausgesetzt.

### 7.0 Übersicht der Dateien

| Datei | Inhalt | Präfix |
|---|---|---|
| `saga.json` | Einstellungen (ein Objekt): Radien, Nachspawn-Zeiten, Bossbeute-Chancen | – |
| `saga_lines.json` | Stränge (Hauptstory, Nebenstränge) und ihre Wege | `saga_`, `path_` |
| `quests.json` | Etappen mit Zielen, Belohnungen und Folgen | `quest_` |
| `dialogues.json` | Gespräche: Knoten, Antworten, Entscheidungen, Hilfen | `dlg_`, `choice_` |
| `lore.json` | Schriftstücke: Logbuchseiten, Lied-Strophen, Inschriften | `lore_` (Lied-Strophen behalten `item_saga_fragment_*`) |
| `visions.json` | Nebelbilder (Seidr-Visionen beim Lauschen) | `vision_` |
| `deeds.json` | Taten (Siedlungs-Chronik und persönliche Taten) | `deed_` |
| `lines.json` | jede gesprochene oder angezeigte Zeile: Text und Ton je Sprache | `line_` |
| `speakers.json` | Sprecher: Stimme je Sprache, KI oder Sprecher | `spk_` |
| Gegner-Tabelle (Phase 3) | Nachspawnen und Bossbeute je Gegner | `enemy_`, `boss_` |

Merksatz: **Jeder Text im Spiel ist eine `line_…`.** Quests, Gespräche, Logbuch und Saga-Buch verweisen nur auf Zeilen. Dadurch hängen Übersetzung, Untertitel und Vertonung an einer Stelle, und jede Zeile weiß, ob ihr Ton noch eine KI-Stimme ist.

### 7.1 `saga.json` – Einstellungen
```json
{ "schema": "saga_1",
  "share_radius_m": 50,
  "quest_enemy_respawn_days": 1,
  "boss_respawn_days": 3,
  "boss_repeat_drop_chance": { "2": 0.5, "3": 0.4, "4": 0.3, "5": 0.2, "6": 0.1 },
  "boss_pity_after": 4,
  "thoughts_default_on": true,
  "quest_help_default_on": true }
```
Alle Zahlen sind bestätigte Startwerte (2026-11-15). Die beiden `*_default_on` sind Vorschlag.

### 7.2 Bedingungen
Gleiche Schreibweise wie in `listen.json` (`art:wert`), ein Prüfbaustein für alles. Neu ist: **Jede Bedingung gilt entweder für den Spieler oder für die Welt.**

| Bedingung | gilt für | Bedeutung |
|---|---|---|
| `quest_done:<id>` | Spieler | Etappe in seinem Queststand abgeschlossen |
| `quest_active:<id>` | Spieler | Etappe läuft bei ihm |
| `choice:<choice_id>=<option>` | Spieler | er hat so entschieden |
| `path:<path_id>` | Spieler | er geht diesen Weg (z. B. `path_circle`, `path_farm`) |
| `lore:<lore_id>` | Spieler | Schriftstück gefunden |
| `song:<song>` | Spieler | alle nötigen Strophen eines Lieds gefunden |
| `listen_level:<n>` | Spieler | Gespür-Stufe mindestens n (inkl. Boni) |
| `setting:<name>` | Spieler | persönliche Menü-Einstellung (z. B. `setting:quest_help`) |
| `unlock:<id>` | Welt | Freischaltung gesetzt. Etappen mit `"scope": "world"` setzen beim Abschluss `unlock:<quest_id>` (daher `unlock:quest_village_seer` in `listen.json`) |
| `world_choice:<choice_id>=<option>` | Welt | **mindestens ein** Spieler hat so entschieden (Folgen addieren sich) |
| `deed:<deed_id>` | Welt | Welt-Tat geschehen |
| `job:<id>`, `building:<id>` | Welt | wie bisher |
| `season:`, `night`, `fog`, `place:` | Welt/Ort | wie in `listen.json` |
| `option:<name>` | Welt | Weltoption aktiv (z. B. `option:hordes`; `option:sagenwelt` entfällt seit 2026-11-22) |
| `stance:<hueter\|rufer>` | Spieler | Haltung aus Kapitel 3 (bestätigt 2026-11-16); vor Kapitel 3 nicht gesetzt |
| `tide:<low\|high>` | Welt/Ort | Ebbe oder Flut (bestätigt 2026-11-21: zweimal pro Spieltag) |
| `mode:<saga\|free>` | Welt | Spielmodus; im Freien Spiel startet kein `main`-Strang |

### 7.3 `saga_lines.json` – Stränge und Wege
```json
{ "schema": 1, "rows": [
  { "id": "saga_first_ones", "name_line": "line_saga_first_ones_title", "kind": "main",
    "quests": ["quest_fo_beach", "quest_fo_farm", "quest_fo_hum", "quest_fo_pages", "quest_fo_graves"],
    "paths": [
      { "id": "path_circle", "requires": ["choice:choice_empty_stone=ring"],
        "quests": ["quest_fo_circle_1", "quest_fo_circle_2", "quest_fo_circle_3"], "rejoin": "quest_fo_meeting" },
      { "id": "path_farm", "requires": ["choice:choice_empty_stone=name"],
        "quests": ["quest_fo_farm_1", "quest_fo_farm_2", "quest_fo_farm_3"], "rejoin": "quest_fo_meeting" }
    ],
    "after_rejoin": ["quest_fo_meeting"] },
  { "id": "saga_seer_voice", "name_line": "line_saga_seer_title", "kind": "side",
    "start": ["quest_done:quest_fo_graves"],
    "quests": ["quest_seer_meet", "quest_seer_glass", "quest_seer_circle", "quest_seer_hut", "quest_village_seer"] }
] }
```
- `kind`:
  - `main`: große Folgen erlaubt;
  - `side`: mäßige Folgen;
  - `settlement`: nur Taten.
- `paths` gibt es nur in der Hauptstory: getrennte Kapitel nach einer großen Entscheidung. `rejoin` ist die Etappe, an der sie wieder zusammenlaufen (bestätigt: 2–3 Kapitel getrennt).

### 7.4 `quests.json` – Etappen
| Feld | Pflicht | Bedeutung |
|---|:-:|---|
| `id` | ✓ | `quest_…` |
| `line` | ✓ | `saga_…` |
| `name_line` | ✓ | Titel (Zeile) |
| `scope` | ✓ | `player`: jeder erlebt die Etappe selbst (Standard). `world`: verändert die Welt und kommt nur einmal vor; wer sie später erreicht, bekommt sie als „davon gehört“ angerechnet |
| `start` | | Bedingungen; fehlt es, beginnt die Etappe nach der vorigen im Strang |
| `hint_line` | ✓ | erzählender Satz im Saga-Buch |
| `area` | | Gebiet auf der Karte: `{ "place": "poi_…", "radius_m": 120 }` |
| `area_help` | | kleineres Gebiet, das eine Gesprächshilfe freischaltet: `{ "radius_m": 30 }` (nur mit `setting:quest_help`) |
| `objectives` | ✓ | Ziele (Tabelle unten), `"order": "any"` oder `"sequence"` |
| `rewards` | | für den Spieler: `{ "items": [...], "deed": "deed_…", "lore": [...] }` |
| `world_effects` | | für die Welt, addieren sich: `{ "unlock": [...], "spawn_npcs": [...], "villagers_join": [...], "blueprints": [...] }` |
| `saga_line` | ✓ | Eintrag im Saga-Buch nach Abschluss |

**Gemeinsame Gutschrift:** Ein erfülltes Ziel zählt für alle Spieler im Umkreis von `share_radius_m` (50 m), die dieselbe Etappe aktiv haben.

Ziel-Typen (`objectives[].type`):

| Typ | Felder | Erfüllt, wenn … |
|---|---|---|
| `go_to` | `place`, `radius_m` | der Spieler dort ist |
| `listen_find` | `target` (Tag oder `kind:`), `count` | er die Ziele beim Lauschen aufgedeckt und aufgenommen hat |
| `listen_at` | `place`, `when`, `when_mode` | er dort aktiv lauscht, während `when` gilt |
| `vision` | `vision` | er das Nebelbild gesehen hat |
| `read` | `lore` | er das Schriftstück gefunden hat |
| `collect` | `item`, `count`, `deliver_to` (`npc_…` / `storage`) | übergeben bzw. im Lager |
| `talk` | `dialogue` | er das Gespräch bis zu einem Ende geführt hat |
| `choose` | `choice` | er die Entscheidung getroffen hat (egal wie) |
| `build` | `buildable` | Gebäude steht (Welt; zählt für jeden, der die Etappe aktiv hat) |
| `defeat` | `enemy` / `boss` | besiegt (mit Gutschrift im Umkreis) |
| `assign_job` | `job` | ein Bewohner hat den Beruf (Welt) |
| `puzzle` | `puzzle` (ID eines Rätsels, z. B. Steine in richtiger Reihenfolge legen) | das Rätsel gelöst ist (keine Uhr, beliebig viele Versuche; im Koop zählt für alle in der Nähe, bestätigt 2026-11-21) |

Questgegner und Questgegenstände werden **pro Spieler gezeigt**: Hat ein Spieler sie schon, sieht er sie nicht mehr; wer sie noch braucht, sieht sein eigenes Exemplar. Questgegner spawnen nach `quest_enemy_respawn_days` nach.

### 7.5 `dialogues.json` – Gespräche, Entscheidungen, Hilfen
```json
{ "schema": 1,
  "choices": [
    { "id": "choice_empty_stone", "weight": "major", "options": ["ring", "name"] },
    { "id": "choice_graves_axe", "weight": "minor", "options": ["leave", "take"] },
    { "id": "choice_seer_trust", "weight": "minor", "options": ["help", "refuse"] }
  ],
  "rows": [
  { "id": "dlg_seer_first", "npc": "npc_seeress", "start": ["quest_active:quest_seer_meet"],
    "nodes": [
      { "id": "n1", "line": "line_seer_first_01",
        "options": [
          { "line": "line_seer_first_ask_where", "next": "n_help", "requires": ["setting:quest_help"] },
          { "line": "line_seer_first_help", "next": "n_yes", "choice": "choice_seer_trust=help" },
          { "line": "line_seer_first_refuse", "next": "n_no", "choice": "choice_seer_trust=refuse" } ] },
      { "id": "n_help", "line": "line_seer_first_where", "effects": { "quest_help": "quest_seer_glass" }, "next": "n1" },
      { "id": "n_yes", "line": "line_seer_first_yes", "end": true },
      { "id": "n_no", "line": "line_seer_first_no", "end": true }
    ] }
] }
```
- **Entscheidung:** Eine Antwort mit `choice` speichert die Wahl im Queststand des Spielers. `weight` legt das Gewicht fest:
  - `major`: nur in `main`-Strängen, große Folgen;
  - `minor`: überall, mäßige Folgen.

  Entscheidungen ohne Gespräch, etwa am leeren Stein, laufen über denselben Block: Das Spiel bietet die Optionen als Handlung an.
- **Hilfe:** Antworten mit `requires: ["setting:quest_help"]` erscheinen nur, wenn die Questhilfe im Menü an ist. Ihr Effekt `quest_help` schaltet für diesen Spieler `area_help` frei. Beim Ausschalten im Menü erklärt ein Hinweis, dass diese Fragen dann fehlen.
- **Koop:** Jeder spricht für sich (eigener Queststand). Mitspieler in der Nähe hören die vertonten Zeilen mit, aber nur der Sprechende wählt.
- **Gemeinsame Entscheidung** (bestätigt 2026-11-21): Eine Entscheidung mit `"group": true` und `"fallback": "<option>"` öffnet sich für alle Spieler im Umkreis gleichzeitig; jeder wählt, keine Uhr. Einig: das gilt. Uneinig: `fallback` (die umkehrbare Option). Gebraucht für Entscheidungen, die die Welt nur auf eine Art verändern können (z. B. `choice_mud_mother` mit `fallback: "capture"`).

### 7.6 `lore.json` – Schriftstücke
```json
{ "schema": 1, "rows": [
  { "id": "lore_asgerd_01", "kind": "journal_page", "group": "journal_asgerd", "order": 1,
    "found": "plain", "place": "poi_first_farm", "line": "line_lore_asgerd_01", "handwriting": "asgerd" },
  { "id": "item_saga_fragment_fjord_2", "kind": "song_fragment", "group": "song_fjord", "order": 2,
    "found": "listen", "line": "line_song_fjord_2" }
] }
```
- `kind`: `journal_page`, `song_fragment`, `inscription`.
- `found`:
  - `plain`: sichtbar;
  - `listen`: hallend, Tag `GamerMagic`, zählt als neuer Fund +2;
  - `hidden`: ab Stufe 4;
  - `rare`: ab Stufe 5.
- Schriftstücke gehen beim Aufnehmen in das Saga-Buch des Spielers, nicht ins Inventar. Andere Questgegenstände sind normale Gegenstände aus `items.json` (Leinsamen, Seeglas, Hallbjörns Ring).
- `handwriting` wählt die Schrift der Handschrift-Ansicht; daneben steht immer der lesbare Drucktext.

### 7.7 `visions.json` – Nebelbilder
```json
{ "schema": 1, "rows": [
  { "id": "vision_empty_stone", "place": "poi_graves_first", "radius_m": 6,
    "when": [], "min_listen_level": 1, "duration_s": 14, "once_per_player": false,
    "figures": [ { "anim": "vis_hallbjorn_leaves", "look": "ghost_male" },
                 { "anim": "vis_asgerd_stays", "look": "ghost_female" } ],
    "lines": ["line_vis_empty_stone_sfx"], "reveals": "item_ring_hallbjorn",
    "thought": "line_thought_empty_stone" }
] }
```
Nebelbilder lassen sich wiederholen (`once_per_player: false`). Die Folge (z. B. den Ring freilegen) gibt es pro Spieler einmal. Ob die Gestalten Worte sprechen oder nur Laute, ist offen (siehe `docs/01_entscheidungen.md`, „Vertonung“).

### 7.8 `lines.json` und `speakers.json` – Text und Vertonung
```json
{ "schema": 1, "rows": [
  { "id": "line_lore_asgerd_01", "speaker": "spk_asgerd",
    "text": { "de": "Dritter Tag an Land. Wir sind siebzehn …", "en": "Third day ashore. We are seventeen …" },
    "audio": { "de": "/Game/Audio/VO/de/lore_asgerd_01", "en": "/Game/Audio/VO/en/lore_asgerd_01" },
    "voice_source": { "de": "ai", "en": "ai" },
    "vars": [] }
] }
```
```json
{ "schema": 1, "rows": [
  { "id": "spk_asgerd", "name": "Asgerd", "voice": { "de": "…", "en": "…" } },
  { "id": "spk_narrator", "name": "Skalde (Erzähler)", "voice": { "de": "…", "en": "…" } },
  { "id": "spk_player", "name": "Spielerfigur", "voice_by_choice": true }
] }
```
- `voice_source` (`ai` / `actor`) zeigt je Sprache, welche Zeilen noch eine KI-Stimme haben. Daraus entstehen die Liste für die spätere Neuaufnahme mit Sprechern und die Angaben für Asset-Log und Steam.
- Zeilen mit Platzhaltern (`{player}`, `{villager}`) werden nicht mit Namen vertont. Der Erzähler sagt dann „sie“ oder umschreibt. Das betrifft vor allem Taten und Gefallene. (Alternative: jeden Namen aus der Namensliste einzeln aufnehmen; teuer.)
- Fehlt der Ton einer Sprache, zeigt das Spiel nur den Untertitel. Untertitel sind immer verfügbar (Lesbarkeit bestätigt).

### 7.9 Gegner-Tabelle (Phase 3): Nachspawnen und Bossbeute
```json
{ "id": "boss_old_boar", "tier": 2, "respawn_days": 3,
  "first_kill_drops": [ { "item": "item_trophy_boss_old_boar", "count": 1 }, { "item": "item_boar_tusk_old", "count": 2 } ],
  "drops": [ { "item": "item_meat_raw", "min": 4, "max": 6 }, { "item": "item_hide", "min": 2, "max": 3 } ],
  "repeat_boss_drops": [ { "item": "item_boar_tusk_old", "count": 1 } ] }
```
- **Erster Sieg pro Spieler:** `first_kill_drops` + `drops`.
- **Jeder weitere Sieg:** `drops`, dazu `repeat_boss_drops` mit der Chance aus `saga.json` je Stufe. Nach `boss_pity_after` (4) Siegen ohne Bossmaterial gibt es beim nächsten sicher welches.
- **Koop:** Jeder Beteiligte im Umkreis bekommt eigene Beute nach seinem eigenen Stand (erster oder wiederholter Sieg).

### 7.10 Spielstand
Im Weltspielstand (`world.json`) werden zwei Bereiche gespeichert:
- **Welt:**
  - gesetzte Freischaltungen;
  - Welt-Entscheidungen (`world_choice`: welche Optionen mindestens einmal gewählt wurden);
  - abgeschlossene Etappen mit `scope: world`;
  - Welt-Taten mit Zeitpunkt;
  - Zeitpunkte fürs Nachspawnen (Bosse, Questgegner).
- **Pro Spieler** (Spieler-ID):
  - Zustand jeder Etappe (`locked` / `active` / `done` / `heard` = „davon gehört“) mit Ziel-Fortschritt;
  - Entscheidungen und Weg (`path_…`);
  - gefundene Schriftstücke und gesehene Nebelbilder;
  - freigeschaltete Gesprächshilfen;
  - persönliche Taten;
  - Gespür-EP und schon gezählte Funde;
  - Bosse mit erstem Sieg und Pech-Zähler je Boss.

Der Server prüft alle Ziele und Bedingungen (Regel 1). Spieler schicken nur Anfragen: sprechen, Antwort wählen, aufnehmen, lauschen.
Der Queststand bleibt immer pro Welt, auch bei der Weltoption „Inventar wandert mit dem Charakter“ (bestätigt 2026-11-15).

## 8. Umfang und Reihenfolge (Vorschlag)

| Schritt | Inhalt | Braucht |
|---|---|---|
| 1 | Quest-Kern: Daten lesen, Bedingungen, Ziel-Typen `go_to`, `listen_find`, `collect`, `build`, `talk`; Saga-Buch Teile Saga, Gespür; speichern | Lauschen (in Arbeit), Speichersystem |
| 2 | Prolog + „Die Stimme im Seeglas“ (5 Etappen), Seherhütte, Beruf `job_seer`; Siedlungs-Taten (erster Bewohner, erster Winter, Gefallene) | erste magische Objekte (Seeglas, Steinkreis), `npc_seeress` als Figur |
| 3 | Lieder und Fragmente, Teil Lieder im Buch, verborgene Strophen | Runensteine/Ruinen als Orte |
| 4 | Hauptsaga Akt I (2 Biome, 2 Bosse) | Phase 3 (Kampf) und Phase 4 (Biome) |
| später | Gezeitenhall-Ereignis, Skalde, Akte II–III, Finale, Jarlssitz | Tag/Nacht, Gezeiten, Wetter |

Ehrlich: Schritt 1–2 sind in einigen Arbeitstagen machbar, sobald es magische Objekte gibt. Die Hauptsaga ist an Kampf, Biome und Bosse gebunden und damit Phase 4. Jede Etappe braucht Text; der Text ist mehr Arbeit, als man denkt (Vorschlag: Claude schreibt Entwürfe, der Nutzer liest gegen).

## 9. Offene Fragen (überholt)

> **Überholt am 2026-11-15:** beantwortet in Abschnitt 12 und `docs/01_entscheidungen.md`. Noch offen sind Frage 6 (Finale) und Frage 8 (Saga-Buch-Taste).

| # | Frage | Vorschlag von Claude |
|---|---|---|
| 1 | Grundstruktur (4 Ebenen, Fortschritt gehört der Welt) | wie Abschnitt 2 und 5 |
| 2 | Hauptsaga-Idee „Der gebrochene Ring“ (Steinkreise je Biom, 3 Akte = 3 Kerben) | wie Abschnitt 3 |
| 3 | Wie genau zeigen Quests ihr Ziel? | nur Gebiet + Lauschen; Weltoption „Questhilfe“ mit genauem Marker für alle, die das wollen |
| 4 | Fragmente direkt ins Saga-Buch statt ins Inventar | ja |
| 5 | Entscheidungen in Gesprächen | im ersten Schritt keine |
| 6 | Finale: schließt sich der Ring oder bleibt er offen? | später |
| 7 | Gefallene: Eintrag immer, Gedenkstein freiwillig | ja |
| 8 | Saga-Buch-Taste | J |

---

## 10. Fünf Storyline- und Saga-Vorschläge

Grundlage für alle fünf Vorschläge:
- **Roter Faden statt Story-Spiel.** Keine Pflicht-Zwischensequenzen, keine langen Dialoge (bestätigt: Text mit Gesten, keine Sprachausgabe). Jeder Strang lässt sich jederzeit liegen lassen.
- **Questgegenstände sind unauffällig.** Es gibt kein eigenes Questinventar und kein Leuchten. Ein Questgegenstand ist ein normaler Gegenstand mit Alltagsnutzen (Seeglas, Logbuch, Runenabrieb, Saatgut) oder landet als Fundstück im Saga-Buch. Höchstens eine kleine Rune im Tooltip zeigt: „gehört zu einer Geschichte“.
- **Horden sind bisher nur eine Weltoption.** In `docs/01_entscheidungen.md` steht: Weltoptionen „freies Bauen, Horden, Survivor, kombinierbar“ (bestätigt), Basisangriffe „optional per Menü“ (bestätigt), „Survivor“ offen. Regel 6 lautet: kein Zeitdruck als Kernschleife. Deshalb ist in allen Vorschlägen keine Story-Etappe an Horden gebunden. Die Horden geben der Story nur einen Grund und eine Farbe. ⚑ Der Nutzer schreibt jetzt: „es wird auch ein Horden-Survive-Element geben“. Ob Horden damit **standardmäßig an** sind (abschaltbar) oder weiter **nur auf Wunsch**, ist offen.
- Die Gespür-Werte aus `listen.json` bleiben unverändert.

### A „Der gebrochene Ring“: eine Kette alter Orte

**Roter Faden:** Ein altes Volk band den Gezeitenhall in einen Ring aus Steinkreisen, einen in jedem Land. Der Ring ist gebrochen, die Gezeiten sind unruhig. Pro Biom findet man das Lied des Kreises, lauscht dort und stellt sich dem, der den Ort besetzt hält (Boss). Drei Akte füllen die drei Kerben des Zeichens.
- **Spürbare Story:** mittel. Es gibt eine erzählte Hauptlinie mit Akten, aber kaum Personen.
- **Saga-System:** Abschnitte 1–9 dieses Dokuments: Stränge, Etappen, Lieder aus Fragmenten, Kerben als Welt-Fortschritt.
- **Nebenquests:** Lied-Fragmente sammeln, Seeglas finden, verborgene Strophen (ab Stufe 4), die Dorf-Seherin, Ruinen erkunden.
- **Questgegenstände:** Fragmente (direkt ins Buch), Seeglas (auch Handwerksstoff), Bossmaterial.
- **Magie und Lauschen:** zentral. Lauschen ist der Weg zu jedem Kreis.
- **Horden:** Unruhige Kreise ziehen in Flutnächten Plünderer an. Ein beruhigter Kreis macht das Land drumherum ruhiger. Mit Horden aus passiert stattdessen nichts.
- **Solo/Koop:** gut. Alles lässt sich allein lösen.
- **Vorteile:** klare Struktur, passt genau zu Biomen, Bossen und Stil, gut als Daten.
- **Nachteile:** am meisten „Story“ der fünf. Das Muster wiederholt sich in jedem Biom. Die Hauptlinie hängt an Phase 3 und 4 (Kampf, Biome).

### B „Die Chronik der Siedlung“: das Dorf ist die Geschichte

**Roter Faden:** Es gibt keine vorgeschriebene Handlung. Der Faden ist der Weg der Siedlung, vom Lagerfeuer über Hof, Dorf und Marktflecken bis zum Jarlssitz. Jede Ausbaustufe hat ein paar „Bewährungen“: erster Winter, erstes Fest, erster Händler, erste Verteidigung. Danach wächst der Ruf der Siedlung und neue Leute kommen mit eigenen Bitten.
- **Spürbare Story:** gering. Die Geschichte entsteht aus dem eigenen Spiel, der Skalde liest sie am Fest vor.
- **Saga-System:** Die Siedlungs-Saga ist das Herz. Taten werden automatisch geschrieben, Gefallene bekommen Einträge und Gedenksteine. Die „Stufen der Siedlung“ ersetzen Akte, die Kerben stehen für Hof, Dorf und Marktflecken.
- **Nebenquests:** **Bitten der Bewohner** (eine Bäuerin will Flachssaat, der Koch ein Gericht der Stufe 2, ein Neuer braucht ein Bett), Händleraufträge, Erkundungsaufträge („bring mir Kunde vom Moor“). Die Seherin kommt als Zugezogene mit ihrer Bitte (`quest_village_seer`).
- **Questgegenstände:** fast nur normale Waren (Essen, Saat, Felle). Einzelne Erbstücke der Bewohner landen im Buch.
- **Magie und Lauschen:** Nebensache. Die Seherin bringt Magie ins Dorf. Magie-Bitten (Seeglas für ein Amulett, Lauschen am Hain) sind eine eigene Art Nebenquest.
- **Horden:** passt am besten. Angriffe sind „Bewährungen“ der Siedlung, Gefallene werden zur Chronik. Ohne Horden zählen andere Bewährungen (harter Winter, Fest).
- **Solo/Koop:** sehr gut. Die Siedlung gehört allen.
- **Vorteile:** am nächsten an Aska und Medieval Dynasty, wenig Text, viele Quests lassen sich aus Bausteinen erzeugen, nutzt Bewohner, Taverne, Feldbau und Jahreszeiten.
- **Nachteile:** Es fehlt ein Geheimnis, das in die Welt hinauszieht. Erkunden und Magie bekommen wenig Grund. Erzeugte Bitten können sich schnell wiederholen.

### C „Die Spur der Ersten“: Erkunden einer verschwundenen Siedlung

**Roter Faden:** Vor euch kam schon eine Siedlergruppe an diese Küste, „die Ersten“. Sie ist verschwunden. Ihre Spuren liegen überall: verlassene Höfe, ein Logbuch, Vorratsgruben, Grabsteine, Markierungen an Bäumen. Raben führen zu den Stellen. Stück für Stück zeigt sich, was ihnen geschah: Sie fanden den Gezeitenhall, gruben zu tief, die Plünderer kamen, und ein Teil von ihnen lebt noch im Norden.
- **Spürbare Story:** gering bis mittel. Erzählt wird fast nur über Funde und Orte (wie in The Forest), kaum über Gespräche. Wer nicht liest, merkt kaum etwas, wer liest, bekommt ein Rätsel.
- **Saga-System:** Das Saga-Buch ist ein **Fundbuch**: Seiten des Logbuchs, Orte der Ersten auf der Karte, offene Fragen („Wohin ging Thorgrim?“). Etappen sind Fundorte statt Aufträge. Daneben läuft die Siedlungs-Chronik (aus B) automatisch.
- **Nebenquests:** verlassene Orte durchsuchen, Vorratsverstecke der Ersten (Saatgut, Werkzeug, Baupläne), ihre ersten Magie-Versuche nachmachen (eine Rune, die sie ritzten, selbst probieren), ihre Gräber finden und Gedenksteine setzen, Überlebende im Norden.
- **Questgegenstände:** Logbuchseiten (Buch), dazu **nützliche Funde**: alte Saat (Feldbau), Bauplan Palisade, rostiges Werkzeug zum Reparieren. Der Gegenstand ist zugleich Belohnung.
- **Magie und Lauschen:** wichtig, aber nicht allein. Manche Spuren sind „hallend“ (nur durch Lauschen), die Seherin der Ersten hinterließ Seeglas-Zeichen. Die Dorf-Seherin kann eine Nachfahrin sein.
- **Horden:** Die Ersten wurden überrannt. Ihre Ruinen zeigen, wie (verbrannte Palisade, Fluchtweg) und geben Baupläne zur Verteidigung. Mit Horden ist das eine Warnung, ohne Horden bleibt es Geschichte.
- **Solo/Koop:** sehr gut. Fundorte sind für alle gleich, jeder kann lesen.
- **Vorteile:** Erkunden steht vorn, Story bleibt leise und freiwillig, Questgegenstände haben Alltagsnutzen, verbindet Magie, Feldbau, Horden und Gedenksteine. Funktioniert schon im ersten Biom, ohne Bosse.
- **Nachteile:** Umweltgeschichten brauchen gut gebaute Orte (Level-Design-Aufwand). Das Rätsel ist einmal gelöst und dann vorbei (Wiederspielwert über neue Welten gering).

### D „Die fünf Wege“: eine kleine Saga pro Herkunft

**Roter Faden:** Jede Herkunft trägt eine eigene kurze Geschichte, 4–6 Etappen:
- **Jägerin:** der weiße Hirsch, der nur im Winter erscheint;
- **Krieger:** ein gebrochener Eid und der Jarl, der ihn verlangte;
- **Schmied:** das Geheimnis des Seeglas-Stahls;
- **Bäuerin:** die alte Saat, die in jedem Klima wächst;
- **Seherin:** die Stimme im Seeglas.

Alle fünf führen am Ende zum selben Ort (Steinkreis oder Jarlssitz), wo sie sich treffen.
- **Spürbare Story:** mittel, aber persönlich und kurz.
- **Saga-System:** **Fortschritt pro Spieler** (das ist hier zwingend). Die Welt hat nur die Siedlungs-Chronik. In Koop helfen Mitspieler bei fremden Wegen und bekommen eine Tat dafür.
- **Nebenquests:** je Herkunft passend (Jagd, Kampf, Schmieden, Feldbau, Magie). Auch andere Herkünfte können einen Weg später übernehmen, passend zu „jeder kann alles lernen“.
- **Questgegenstände:** Herkunftsgegenstände (Hirschgeweih, Eidring, Seeglas-Barren, Ursaat, Seherstab), alle mit Nutzen.
- **Magie und Lauschen:** stark beim Seherin- und Schmied-Weg, schwach bei den anderen.
- **Horden:** im Krieger-Weg (der Jarl schickt Leute), sonst kaum.
- **Solo/Koop:** Solo bekommt man nur einen Weg zu sehen (es sei denn, man übernimmt weitere). Koop ist reicher.
- **Vorteile:** macht die Herkunftswahl bedeutsam, jede Runde mit anderer Herkunft fühlt sich anders an.
- **Nachteile:** fünfmal Inhalt (teuer). Der Fortschritt pro Spieler braucht die Regeln aus der Frage WoW/Mischung. Die Welt selbst hat keinen gemeinsamen Faden.

### E „Das Jahresrad“: Geschichte im Takt der Jahreszeiten

**Roter Faden:** Die Geschichte folgt den Jahreszeiten (eine Stunde Echtzeit je Jahreszeit, bestätigt). Jedes Jahr ist ein Kapitel mit einem Fest:
- **Frühling:** Aussaat;
- **Mittsommer:** Gezeitenhall am stärksten;
- **Herbst:** Ernte und Opfer;
- **Winter:** Julnächte.

An jedem Fest zeigt sich ein Zeichen, das in die Welt hinausführt (ein Rabe, eine Flut, ein Fremder). Ein Kapitel wartet, bis man so weit ist. Nichts geht verloren, das Fest kommt nächstes Jahr wieder.
- **Spürbare Story:** gering bis mittel, als Rhythmus spürbar.
- **Saga-System:** Das Saga-Buch ist ein **Jahrbuch**: Seiten je Jahr und Jahreszeit, mit Taten, Ernte, Festen und Gefallenen. Kapitel entsprechen Jahren.
- **Nebenquests:** Jahreszeiten-Aufgaben (Frühling: wilde Saat finden; Sommer: Seeglas bei Niedrigwasser; Herbst: Vorräte für den Winter; Winter: Spuren im Schnee lesen), Feste vorbereiten (Taverne, Gerichte).
- **Questgegenstände:** Festgaben (Gerichte, Met, Felle), alles normale Waren.
- **Magie und Lauschen:** an Zeiten gebunden, passend zu den Gespür-Boni (Winter, Nacht, Nebel +1). Am Mittsommer ist der Gezeitenhall-Fund am größten.
- **Horden:** Wintereinfälle (Plünderer, Wölfe) als Bedrohung zur Jahreszeit, nur mit der Weltoption.
- **Solo/Koop:** gut.
- **Vorteile:** nutzt Jahreszeiten, Feldbau, Taverne und Gespür-Boni stark. Der Rhythmus passt zu einem Überlebensspiel.
- **Nachteile:** Warten auf die nächste Jahreszeit kann sich wie Zeitdruck oder Leerlauf anfühlen, im Widerspruch zum Sinn von Regel 6. Mit 4 Stunden pro Jahr ist ein Kapitel lang. Hängt an Tag/Nacht und Festen, die es noch nicht gibt.

### Vergleich

| | A Ring | B Chronik | C Spur der Ersten | D Fünf Wege | E Jahresrad |
|---|---|---|---|---|---|
| Spürbare Story | mittel | gering | gering–mittel | mittel | gering–mittel |
| Erkunden als Antrieb | hoch | gering | **sehr hoch** | mittel | mittel |
| Überleben/Siedlung | gering | **sehr hoch** | mittel | gering | hoch |
| Magie/Lauschen | **sehr hoch** | gering | hoch | gemischt | hoch |
| Horden eingebunden | mittel | **hoch** | mittel | gering | mittel |
| Inhaltsaufwand | hoch | gering–mittel | mittel | **sehr hoch** | mittel |
| Früh spielbar (ohne Bosse/Biome) | nein | ja | **ja** | teils | nein (braucht Tag/Nacht) |

### Empfehlung von Claude

**C als roter Faden, B als Saga-System darunter.** Begründung:
- C zieht in die Welt hinaus, erzählt leise über Funde statt Dialoge und gibt dem Lauschen einen Zweck.
- B schreibt die Geschichte der eigenen Siedlung mit: Bitten, Bewährungen, Gefallene, Gedenksteine. Das hält Überleben und Bauen im Vordergrund.
- Beide gehen ohne Bosse und Biome, also früh.
- Aus A lassen sich die Steinkreise und Lieder als Orte übernehmen, die die Spur der Ersten berührt. Die Seherin-Quest (`quest_village_seer`) passt in alle Vorschläge, in C als Nachfahrin der Ersten.

**Korrektur des Nutzers (2026-11-15):** Der Siedlungsaufbau ist nur **zweitrangig**. Deshalb soll B nicht die Hauptsache sein, sondern nur eine kleine automatische Chronik im Saga-Buch: Taten, Gefallene, Gedenksteine. Die Empfehlung verschiebt sich auf **C als Kern**, mit den Steinkreisen und Liedern aus A als Orten.

Nicht empfohlen als Hauptlinie: **D** (fünffacher Aufwand, kein gemeinsamer Faden) und **E** (Warten auf Jahreszeiten kollidiert mit „kein Zeitdruck“).

## 11. Questfortschritt pro Spieler, alles spawnt nach (Entwurf)

Nutzer (2026-11-15): „Es kann alles nachspawnen.“ **Deutung von Claude:** Antwort auf die Frage nach WoW. Jeder Spieler hat seinen eigenen Queststand. Questgegner, Bosse und Questgegenstände kommen wieder, damit jeder sie selbst erleben kann. Das ersetzt Abschnitt 5 („Fortschritt gehört der Welt“). Noch nicht bestätigt.

| Frage | Regel (Vorschlag) |
|---|---|
| Queststand | **pro Spieler**, gespeichert unter der Spieler-ID im Weltspielstand |
| Gemeinsame Gutschrift | Wird ein Ziel erfüllt, bekommen es alle Spieler im Umkreis von 50 m, die diese Etappe gerade aktiv haben (wie in WoW). Man muss also nichts doppelt machen, wenn man zusammen spielt |
| Questgegenstände, Fragmente, Logbuchseiten | **pro Spieler sichtbar:** Jeder, der den Fund noch braucht, sieht und nimmt sein eigenes Exemplar. Wer ihn schon hat, sieht an der Stelle nichts mehr. Technisch: Der Server zeigt das Ding nur den Spielern, die es noch brauchen (kleines Phasing nur für Fundstücke, gut machbar) |
| Questgegner | spawnen nach (Vorschlag: nach 1 Spieltag, oder sofort, wenn ein weiterer Spieler mit aktiver Etappe kommt) |
| Bosse | spawnen nach (Vorschlag: nach 3 Spieltagen). **Erster Sieg (pro Spieler): volle Beute mit Bossmaterial. Jeder weitere Sieg: normale Beute, Bossmaterial nur mit einer Wahrscheinlichkeit, die von der Stufe abhängt** (Nutzer 2026-11-15: „beim ersten besiegen normaler lot dann kann prozentual t basierender lot droppen“; gedeutet als: Chance je Stufe). Vorschlag von Claude: T2 50 %, T3 40 %, T4 30 %, T5 20 %, T6 10 %; nach 4 Wiederholungen ohne Bossmaterial gibt es beim nächsten Sieg sicher welches (gegen Pechsträhnen). Daten: `"repeat_drop_chance"` je Boss, `"pity_after": 4`. Die Saga-Tat „Boss besiegt“ gibt es pro Spieler nur einmal |
| Was die Welt verändert | gibt es **nur einmal**: Die Seherhütte steht oder nicht, die Seherin zieht einmal ein, ein freigelegter Weg bleibt frei. Wer die Etappe später erreicht, bekommt sie als erledigt angerechnet, mit dem Vermerk „davon gehört“. Gespräche, Lauschen und Funde dazu kann er trotzdem selbst nachholen (eigene Taten, eigene Gespür-EP) |
| Kein echtes Phasing | Das Dorf und die Landschaft sehen für alle gleich aus. Nur Fundstücke und Questgegner werden je Spieler gezeigt bzw. nachgespawnt |
| Gespür-EP | unverändert aus `listen.json`: pro Fund und Spieler einmal. Ein nachgespawntes Seeglas zählt für denselben Spieler nicht noch einmal als „neuer Fund“, Seeglas-Ernte (+1) aber schon (Deutung: Seeglas ist ein nachwachsender Rohstoff) |
| Solo | Solo spielt genauso, nur ohne Mitspieler-Gutschrift |

Für die Daten heißt das: Jede Etappe bekommt ein Feld `"scope": "player"` (Standard) oder `"world"` (verändert die Welt, gibt es nur einmal). Gegner und Bosse bekommen `"respawn_days"`.

## 12. Antworten des Nutzers (2026-11-15)

Eingetragen in `docs/01_entscheidungen.md`. Kurzfassung:
1. Queststand pro Spieler, alles spawnt nach (Abschnitt 11): keine Vorliebe angegeben, bleibt **Vorschlag**.
2. Bossbeute: erster Sieg volle Beute, danach Bossmaterial nur mit Chance je Stufe: **bestätigt**.
3. Startwerte (50 m, 1 und 3 Spieltage, 50–10 %, nach 4 Fehlversuchen sicher): **bestätigt als Startwerte**.
4. Story-Richtung C „Spur der Ersten“, Steinkreise als Orte, Chronik klein: **bestätigt**.
5. Probe schreiben: ja, siehe `docs/15_probe_spur_der_ersten.md`.
6. Horden: bleiben Weltoption in den Einstellungen, keine Story-Etappe braucht sie: **bestätigt**.
7. Questhilfe: Gebiet, genauere Hilfe über passende Gesprächswahl, im Menü abschaltbar: **bestätigt** (Deutung).
8. Gefallene: Eintrag immer, Gedenkstein freiwillig: **bestätigt**.
9. Gespräche mit Entscheidungen von Anfang an: **bestätigt** (gegen die Empfehlung). Offen: Wer entscheidet bei Entscheidungen, die die Welt verändern?

Folgen für den Entwurf: Abschnitt 5 („Fortschritt gehört der Welt“) gilt nicht mehr, sobald Abschnitt 11 bestätigt ist. Abschnitt 3 (Hauptsaga A) ist nicht mehr die Hauptlinie, Steinkreise und Lieder bleiben als Orte. Datenformat (Abschnitt 7) braucht noch: `scope` je Etappe, `respawn_days`, `repeat_drop_chance`, `pity_after`, Gesprächsknoten mit Wahl und Folgen.

### Nachtrag: weitere Antworten (2026-11-15)
- Queststand pro Spieler: **bestätigt**. Abschnitt 11 gilt.
- Queststand bleibt immer pro Welt, auch bei „Inventar wandert mit“.
- Vertonung: Namen werden umschrieben; Spielerfigur mit 4 wählbaren Stimmen; Nebelbilder mit halb verwehten Sätzen; Lippen aus dem Ton (MetaHuman).
- Finale: Der Spieler entscheidet (Ring schließen oder offen lassen), zwei Enden. Im Koop zeigt die Welt das zuerst gewählte Ende (Vorschlag, Ausnahme von „Folgen addieren sich“).
- Saga-Buch-Taste: J.
- Damit sind die offenen Fragen aus Abschnitt 9 alle beantwortet.

