# Datenformat für Spielinhalte (Stand 2026-11-13)

Status: **bestätigt am 2026-11-13** (JSON, Ort `game/Content/Data/`, Werkzeug-IDs mit Stufe, Startwerte aus Abschnitt 6 zum Ausprobieren; siehe `docs/01_entscheidungen.md`). Gilt ab Phase 1, Schritt 2.

Technischer Hinweis: Das Spielmodul braucht das Engine-Modul `Json` als eigene Abhängigkeit in `Gamer.Build.cs`. Über die Engine ist es nur zum Einbinden der Header sichtbar, nicht zum Verknüpfen; der Build scheitert sonst an fehlenden JSON-Funktionen (am 2026-11-13 festgestellt).

## 1. Worum es geht
Architekturregel 3 sagt: Gegenstände, Rezepte, Bauteile, Jobs, Gegner usw. stehen in Tabellen, nicht im Code. Dieses Dokument legt fest, **wie** diese Tabellen aussehen, **wo** sie liegen und **wie Unreal sie liest**.

Warum das wichtig ist: Später wird es mehrere hundert Einträge geben. Wenn das Format am Anfang unklar ist, muss man es später überall ändern. Ein klares Format erlaubt außerdem, dass Claude, ChatGPT und du Werte ändern, ohne C++ anzufassen oder neu zu bauen.

## 2. Empfehlung in einem Satz
**JSON-Dateien (eine pro Tabelle) in `game/Content/Data/`, die ein C++-Baustein beim Start auf Server und Spielern einliest und prüft.**

### 2.1 Warum JSON (und nicht Unreal-DataTables oder Excel/CSV)
| | JSON-Dateien (empfohlen) | Unreal-DataTable (.uasset) | CSV (Excel) |
|---|---|---|---|
| Lesbar und in Git vergleichbar | ja, Zeile für Zeile | nein (Binärdatei) | ja |
| Von Claude/ChatGPT direkt bearbeitbar | ja | nur über den Editor | ja |
| Verschachtelte Angaben (z. B. „Rezept braucht 2 Zutaten“) | ja | ja | umständlich |
| Ändern ohne Neubau | ja (neu laden per Befehl) | nein, Editor nötig | ja |
| Prüfung im Unreal-Editor | nein, dafür eigener Prüfbaustein | ja | nein |

Typischer Fehler, den das vermeidet: Bei DataTables als Binärdatei sieht man in Git nicht, *welcher* Wert sich geändert hat, und zwei Leute, die gleichzeitig ändern, überschreiben sich gegenseitig.

Nachteil von JSON: Unreal kennt die Verweise auf Modelle und Icons darin nicht von selbst. Deshalb (1) prüft der eigene Baustein beim Start jeden Verweis, und (2) wird der Asset-Ordner beim Paketieren immer mitgenommen (Einstellung `DirectoriesToAlwaysCook`).

### 2.2 Warum `game/Content/Data/` statt `data/` im Hauptordner
Beim Paketieren nimmt Unreal nur Dateien **innerhalb des Projektordners `game/`** mit. `data/` liegt eine Ebene darüber und würde im fertigen Spiel und im Server-Paket fehlen. Die Alternative (Sonderregel in `Gamer.Build.cs`) ist eine Kerndatei mehr und schwerer zu durchschauen.
Vorschlag: `data/README.md` bleibt als Wegweiser stehen und zeigt auf `game/Content/Data/`. Unreal ignoriert `.json`-Dateien im Content-Ordner im Editor; beim Paketieren werden sie über `DirectoriesToAlwaysStageAsUFS=(Path="Data")` in `DefaultGame.ini` eingepackt.

## 3. Regeln für alle Tabellen
- **Eine Datei pro Tabelle**, Aufbau immer gleich: `{ "schema": 1, "rows": [ … ] }`. `schema` zählt hoch, wenn sich das Format ändert.
- **IDs:** `snake_case`, stabil, **nie wiederverwenden**. Präfixe wie im Asset-Plan: `item_` Gegenstand, `tool_` Werkzeug, `res_` Ressourcen-Vorkommen (Baum, Fels, Strauch), `building_` Bauteil oder Gebäude, `recipe_` Rezept. Später: `job_`, `need_`, `enemy_`, `skill_`.
- **Löschen gibt es nicht:** Ein Eintrag, der wegfällt, bekommt `"deprecated": true`. So zeigen alte Spielstände nie auf eine ID, die es plötzlich für etwas anderes gibt.
- **Einheiten:** Länge in cm (wie Unreal), Zeit in Sekunden, Gewicht in kg, Spielzeit in Spieltagen.
- **Anzeigenamen:** vorerst `"name"` auf Deutsch. Übersetzungen kommen später über Unreals String-Tables; die ID bleibt dabei der Schlüssel.
- **Verweise auf Modelle/Icons:** voller Unreal-Pfad als Text, z. B. `"/Game/Assets/Buildings/WoodKit/SM_Building_Wall_Wood"`. Fehlt ein Modell noch, bleibt das Feld leer, und das Spiel zeigt einen Platzhalter.
- **Werte sind Startwerte.** Alles in Abschnitt 6 ist ein Vorschlag zum Ausprobieren und wird beim Spielen angepasst.

## 4. Die Tabellen für Phase 1
### 4.1 `items.json` – Gegenstände und Werkzeuge
| Feld | Pflicht | Bedeutung |
|---|:-:|---|
| `id` | ✓ | `item_…` oder `tool_…` |
| `name` | ✓ | Anzeigename |
| `category` | ✓ | `resource` (Rohstoff), `material` (hergestellt), `food`, `tool` |
| `tier` | ✓ | Stufe 1–6 (T1 Rohstufe … T6 Relikt) |
| `max_stack` | ✓ | wie viele in einen Inventarplatz passen |
| `weight_kg` | ✓ | für die Traglast |
| `icon` | | Pfad zum Icon |
| `world_mesh` | | Pfad zum Modell, wenn man den Gegenstand in der Welt sieht |
| `tool` | nur Werkzeuge | `{ "type": "axe", "durability": 150, "work_speed": 1.0 }` |
| `food` | ab Phase 2 | Buffs und Verderben (Format folgt mit dem Buff-System) |

Beispiel:
```json
{ "id": "tool_axe_t1", "name": "Steinaxt", "category": "tool", "tier": 1,
  "max_stack": 1, "weight_kg": 2.0,
  "tool": { "type": "axe", "durability": 150, "work_speed": 1.0 } }
```
Werkzeuge bekommen die Stufe in der ID (`tool_axe_t1`, `tool_axe_t2` …), weil jede Stufe ein eigener Gegenstand mit eigenen Werten ist. `tool_axe` aus dem Asset-Plan bleibt der Name des **Modells**, das sich alle Stufen teilen.

### 4.2 `resource_nodes.json` – Bäume, Felsen, Sträucher
| Feld | Pflicht | Bedeutung |
|---|:-:|---|
| `id` | ✓ | `res_…` |
| `name` | ✓ | Anzeigename |
| `required_tool` | | `{ "type": "axe", "min_tier": 1 }`; fehlt es, geht es mit der Hand |
| `work_seconds` | ✓ | Arbeitszeit bei Werkzeug-Tempo 1,0 |
| `drops` | ✓ | Liste `{ "item": "item_log", "min": 2, "max": 3 }` |
| `regrow_days` | | nach wie vielen Spieltagen es nachwächst; fehlt es, wächst nichts nach |
| `meshes` | | `{ "full": …, "depleted": …, "remains": … }` (stehend / abgeerntet / Stumpf) |

### 4.3 `recipes.json` – Herstellung
| Feld | Pflicht | Bedeutung |
|---|:-:|---|
| `id` | ✓ | `recipe_…` |
| `station` | | `building_…`, an dem hergestellt wird; fehlt es, geht es aus der Hand |
| `inputs` | ✓ | Liste `{ "item": …, "count": … }` |
| `outputs` | ✓ | Liste `{ "item": …, "count": … }` |
| `work_seconds` | ✓ | Herstellungsdauer |

### 4.4 `buildables.json` – Bauteile und Arbeitsstätten
| Feld | Pflicht | Bedeutung |
|---|:-:|---|
| `id` | ✓ | `building_…` |
| `name` | ✓ | Anzeigename |
| `category` | ✓ | `structure` (Baukasten), `workstation`, `storage`, `comfort` (Bett, Feuer) |
| `snap` | nur `structure` | `foundation`, `floor`, `wall`, `wall_door` (Wand mit Türöffnung, bietet den Platz für die Tür), `door`, `pillar`, `beam`, `roof` (Dachfläche), `gable` (Giebel), `ridge` (First), `stairs`; Raster 300 cm wie der Holzbaukasten. `wall_door`, `gable`, `ridge` seit 2026-11-13 (vorher liefen Giebel und First unter `roof`, die Türwand unter `wall`; das Einrasten braucht die Unterscheidung) |
| `mesh` | | Unreal-Pfad zum Modell, z. B. `/Game/Gamer/Buildings/WoodKit/SM_Building_Wall_Wood.SM_Building_Wall_Wood` (Import aus `Assets/` mit `tools/import_assets.ps1`). Ohne `mesh` zeigt das Spiel einen Platzhalter aus Grundformen |
| `cost` | ✓ | Liste `{ "item": …, "count": … }` |
| `build_seconds` | ✓ | Bauzeit |
| `required_tool` | | meist `{ "type": "hammer", "min_tier": 1 }` |

### 4.5 `jobs.json` – Berufe der Dorfbewohner (seit 2026-11-13)
| Feld | Pflicht | Bedeutung |
|---|:-:|---|
| `id` | ✓ | `job_…` |
| `name` | ✓ | Anzeigename („Holzfäller“) |
| `kind` | ✓ | `harvest` (Vorkommen abbauen/sammeln), `craft` (Rezepte an der Station), `build` (Baupläne hämmern) |
| `station` | | Bauteil, von dem aus gearbeitet wird (muss fertig stehen); ohne: vom Lager aus |
| `tool` | | Werkzeugtyp, den der Bewohner aus dem Lager nimmt (`axe`, `pickaxe`, `hammer`); ohne: von Hand |
| `recipes` | bei `craft` | Rezept-IDs |
| `stock_target` | | `craft`: aufhören, wenn so viel vom Ergebnis im Lager liegt (Standard 10) |
| `radius` | | Arbeitsbereich in cm um Station bzw. Lager (Standard 3000) |
| `fallback` | | genau ein Beruf hat `true`: der Ausweich-Beruf, wenn der eigene nicht geht |

### 4.6 `needs.json` – Bedürfnisse der Dorfbewohner (seit 2026-11-13)
Werte laufen von 1 (gut) bis 0 (leer), Zeiten in Spieltagen (1 Spieltag = 30 min).
| Feld | Pflicht | Bedeutung |
|---|:-:|---|
| `id` | ✓ | `need_…` |
| `name` | ✓ | Anzeigename |
| `kind` | ✓ | `food`, `sleep`, `shelter` (eigenes Bett unter einem Dach) |
| `decay_per_day` | | Abnahme pro Spieltag (wach) |
| `threshold` | | darunter kümmert sich der Bewohner zuerst darum |
| `restore_per_day` | | `sleep`: Zunahme pro Spieltag im Bett (ohne Bett die Hälfte) |
| `foods` | bei `food` | `[{ "item": …, "value": 0..1 }]`, bestes zuerst |
| `work_speed_when_unmet` | | Arbeitstempo, solange das Bedürfnis nicht erfüllt ist (z. B. 0,7) |
| `leave_after_days` | | `food`: geht nach so vielen Spieltagen bei 0 (0 = nie) |

`jobs.json` und `needs.json` sind optional (fehlen sie, gibt es keine Bewohner-Daten, aber keinen Fehler). Der Prüfer kontrolliert Stationen, Werkzeugtypen, Rezepte und Speisen.

Später dazu: Gegner, Fähigkeiten, Quests.

## 5. Wie Unreal die Daten benutzt
1. **Einlesen:** Ein C++-Baustein (`UGamerDataSubsystem`) liest beim Start alle Dateien – auf dem Server **und** bei jedem Spieler, damit alle dieselben Zahlen kennen.
2. **Prüfen** direkt beim Einlesen, mit klarer Fehlermeldung im Protokoll:
   doppelte IDs · Verweise auf IDs, die es nicht gibt (z. B. Rezept nennt `item_plnk`) · fehlende Pflichtfelder · Werte außerhalb des Sinnvollen (Stufe 7, negative Mengen) · Pfade zu Modellen/Icons, die es nicht gibt (nur Warnung, solange Modelle fehlen).
   **Erreichbarkeit (seit 2026-11-13):** Ausgehend von „nichts im Inventar“ wird durchgespielt, was man mit der Hand sammeln, daraus herstellen und mit den neuen Werkzeugen weiter abbauen oder bauen kann. Jeder Gegenstand, jedes Vorkommen, Rezept und Bauteil, das dabei nie erreichbar wird, ist ein Fehler (z. B. „Axt braucht Stamm, Stamm braucht Axt“). Gegenstände, die es später nur bei Händlern oder Bossen gibt, bekommen dafür eine eigene Kennzeichnung, sobald es Händler und Bosse gibt.
3. **Benutzen:** Spielcode fragt nur per ID: „Was ist `item_log`?“ Übers Netz gehen nur IDs und Mengen, nie ganze Datensätze – das spart Bandbreite.
4. **Ändern während der Entwicklung:** Konsolenbefehl `gamer.ReloadData` liest neu ein, ohne Neustart.
5. **Automatischer Test:** ein Unreal-Automationstest „Gamer.Data“, der alle Dateien lädt und bei jedem Fehler fehlschlägt. Läuft ohne Grafik in ca. 1 Minute; ein Skript `tools/test_data.ps1` startet ihn.
6. **Später (vor dem ersten öffentlichen Test):** Der Server schickt beim Beitritt eine Prüfsumme der Daten; ein Spieler mit anderen Daten wird abgewiesen, statt mit falschen Werten zu spielen.

Typische Fehlerquellen und was dagegen hilft:
- **Tippfehler in einer ID** → Prüfung in Schritt 2 meldet die Stelle.
- **Server und Spieler haben unterschiedliche Daten** (z. B. nur einer neu gebaut) → Prüfsumme in Schritt 6.
- **Modell fehlt im fertigen Spiel**, weil nur die JSON-Datei darauf zeigt → Asset-Ordner wird immer mitgepackt (Abschnitt 2.1).
- **Werte driften auseinander** (Design-Dokument sagt A, Datei sagt B) → die JSON-Datei ist die einzige Quelle für Zahlen; Design-Dokumente beschreiben nur Absichten.

## 6. Phase-1-Inhalt (Startwerte, Vorschlag)
### Gegenstände
| ID | Name | Kategorie | Stufe | Stapel | kg |
|---|---|---|:-:|--:|--:|
| `item_log` | Baumstamm | resource | 1 | 10 | 20 |
| `item_stone` | Stein | resource | 1 | 20 | 5 |
| `item_stick` | Äste | resource | 1 | 30 | 0,5 |
| `item_fiber` | Pflanzenfasern | resource | 1 | 50 | 0,1 |
| `item_plank` | Bretter | material | 1 | 40 | 2 |
| `item_berries` | Beeren | food | 1 | 50 | 0,1 |
| `item_apple` | Wildäpfel | food | 1 | 30 | 0,2 |
| `item_mushrooms` | Pilze | food | 1 | 30 | 0,1 |
| `item_hazelnuts` | Haselnüsse | food | 1 | 50 | 0,05 |
| `item_meal` | einfache Mahlzeit | food | 1 | 10 | 0,5 |
| `tool_axe_t1` | Steinaxt | tool | 1 | 1 | 2 |
| `tool_pickaxe_t1` | Steinspitzhacke | tool | 1 | 1 | 3 |
| `tool_hammer_t1` | Steinhammer | tool | 1 | 1 | 1,5 |

Werkzeuge T1: Haltbarkeit 150 Einsätze, Tempo 1,0.

### Vorkommen
| ID | Werkzeug | Sekunden | Ertrag | Nachwachsen |
|---|---|--:|---|--:|
| `res_tree_pine` | Axt T1 | 6 | 2–3 Stämme | 3 Tage |
| `res_tree_birch` | Axt T1 | 5 | 2 Stämme | 3 Tage |
| `res_rock_small` | Spitzhacke T1 | 3 | 2 Steine | 5 Tage |
| `res_rock_medium` | Spitzhacke T1 | 6 | 4 Steine | 5 Tage |
| `res_rock_large` | Spitzhacke T1 | 10 | 8 Steine | 5 Tage |
| `res_berry_bush` | Hand | 2 | 3–5 Beeren | 1 Tag |
| `res_tree_apple` | Hand (pflücken) | 2,5 | 3–6 Wildäpfel | 2 Tage |
| `res_mushroom_patch` | Hand | 1,5 | 2–4 Pilze | 1 Tag |
| `res_hazel_bush` | Hand | 2 | 4–8 Haselnüsse | 2 Tage |
| `res_loose_stone` | Hand | 1,5 | 1–2 Steine | 1 Tag |
| `res_fallen_branch` | Hand | 1,5 | 1–2 Äste | 1 Tag |
| `res_fiber_plant` | Hand | 2 | 2–3 Fasern | 1 Tag |

Nachwachsen ist ein Vorschlag aus dem Design-Register (Kapitel 15), noch nicht bestätigt.
Die drei Hand-Vorkommen (seit 2026-11-13) sind der Einstieg: Damit kommt man ohne Werkzeug an Steine, Äste und Fasern für die ersten Werkzeuge. Felsen geben mit der Spitzhacke deutlich mehr Steine.

### Rezepte
| ID | Wo | aus | wird zu | Sekunden |
|---|---|---|---|--:|
| `recipe_axe_t1` | Hand (Anfangs-Crafting) | 1 Ast + 2 Steine + 2 Fasern | Steinaxt | 5 |
| `recipe_pickaxe_t1` | Hand (Anfangs-Crafting) | 2 Äste + 3 Steine + 2 Fasern | Steinspitzhacke | 6 |
| `recipe_hammer_t1` | Hand (Anfangs-Crafting) | 1 Ast + 1 Stein + 1 Faser | Steinhammer (Bauhammer) | 4 |
| `recipe_plank` | Werkbank | 1 Stamm | 4 Bretter | 4 |
| `recipe_meal` | Lagerfeuer | 3 Beeren | 1 Mahlzeit | 10 |

Hinweis zur Reihenfolge: Die Werkbank kostet **keine Bretter** (sonst bräuchte man Bretter, um Bretter herzustellen).
Bis 2026-11-13 brauchten die Werkzeug-Rezepte einen Baumstamm – den es aber nur mit Axt gibt. Deshalb jetzt Äste statt Stämme, und der Datenprüfer kontrolliert, ob jeder Gegenstand vom Spielstart aus erreichbar ist (Abschnitt 5).

### Bauteile und Arbeitsstätten (Hammer T1 nötig)
| ID | Kosten | Sekunden |
|---|---|--:|
| `building_foundation_wood` | 2 Stämme + 4 Steine | 4 |
| `building_floor_wood` | 6 Bretter | 3 |
| `building_wall_wood` | 6 Bretter | 3 |
| `building_wall_door_wood` | 5 Bretter | 3 |
| `building_door_wood` | 4 Bretter | 2 |
| `building_pillar_wood`, `building_beam_wood` | je 2 Bretter | 2 |
| `building_roof_wood` | 6 Bretter | 3 |
| `building_roof_ridge_wood` | 3 Bretter | 2 |
| `building_roof_gable_wood` | 4 Bretter | 3 |
| `building_workbench` | 3 Stämme + 4 Steine | 6 |
| `building_campfire` | 10 Steine + 2 Stämme | 5 |
| `building_bed` | 4 Bretter | 4 |
| `building_storage` | 30 Bretter + 10 Steine | 20 |
| `building_woodcutter` | 2 Stämme + 4 Bretter | 8 |
| `building_stonecutter` | 2 Stämme + 8 Steine | 8 |

## 7. Offene Fragen an den Nutzer
1. **Format:** JSON wie empfohlen?
2. **Ort:** `game/Content/Data/` statt `data/` (wegen Paketieren)?
3. **Werkzeug-IDs mit Stufe** (`tool_axe_t1`) statt einer ID mit Stufenfeld?
4. **Startwerte in Abschnitt 6:** so zum Ausprobieren übernehmen?

## 8. Was danach passiert (nach Bestätigung)
1. Die vier JSON-Dateien mit den Werten aus Abschnitt 6 anlegen.
2. `UGamerDataSubsystem` mit Einlesen und Prüfung in C++.
3. Automationstest „Gamer.Data“ und `tools/test_data.ps1`.
4. Zwei Einträge in `DefaultGame.ini` (Kerndatei, Rückfrage): Daten-Ordner einpacken, Asset-Ordner immer mitnehmen.
5. Test: Server und 2 Spieler lesen dieselben Daten (Prüfsumme im Protokoll vergleichen).
