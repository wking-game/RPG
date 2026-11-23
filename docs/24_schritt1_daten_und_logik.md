# Schritt 1: Daten und Rechenlogik für Kreaturen, Kampf und Level

Stand 2026-11-22. **Plan, nicht umgesetzt.** Grundlage: `docs/21_kreaturen_level_kampf.md`, `docs/22_kreaturen_daten.md`, `docs/23_bauen_rechte_logistik.md` und der aktuelle Code (`GamerDataLoader`, `GamerCharacterData`, `GamerDifficulty`, `GamerHealthComponent`, Tests in `GamerDataTests.cpp`).

## 1. Ziel und Grenzen

**Ziel:** Alles, was sich ohne Grafik, ohne Kampf im Spiel und ohne Modelle bauen und prüfen lässt: die JSON-Tabellen, die Lader mit Prüfungen, die Rechenfunktionen und die Würfel für Spawn und Beute, jeweils mit automatischen Tests. Danach kann der Kampf (Schritt 2) auf geprüften Daten aufbauen.

**Gehört nicht dazu:** Kampfsteuerung, Gegner-KI, Waffen im Spiel, Ausrüstungsplätze, Erfahrungs-Anzeige im HUD, Skilltree, Modelle und Animationen, Baustufen, Rechte, Logistik (`docs/23`). Es ändert sich also **nichts, was man im Spiel sieht**. Der Beweis sind grüne Tests und ein Datensatz, der auf Server und Client gleich geladen wird.

## 2. Was es schon gibt (daran halten wir uns)

- JSON-Tabellen in `game/Content/Data/`, Format `{ "schema": 1, "rows": [ … ] }`, IDs `snake_case` (`docs/06_datenformat.md`).
- `GamerData::LoadFromDirectory` lädt Items, Vorkommen, Rezepte, Bauteile, Berufe, Bedürfnisse und prüft sie (auch, ob jeder Gegenstand erreichbar ist: `CheckReachability`). Die Datei hat 1 739 Zeilen und soll nicht weiter wachsen.
- Eigene Lader nach demselben Muster: `GamerData::LoadCharacters` (`GamerCharacterData.cpp`) und `GamerData::LoadDifficulty` (`GamerDifficulty.cpp`). Sie prüfen gegen die Basisdaten, melden Fehler auf Deutsch und werden von `UGamerDataSubsystem` gehalten (`Reload` behält alte Daten bei Fehlern).
- Tests als `IMPLEMENT_SIMPLE_AUTOMATION_TEST`, gestartet mit `tools/test_data.ps1 -Filter <Name>` (UnrealEditor-Cmd, ohne Grafik, ca. 1–2 Minuten). Zuletzt 13 Tests grün (Sitzung „Sagen-Bosse“).
- Prüfsumme (CRC32) der Datendateien für Server/Client-Vergleich.

## 3. Arbeitsschritte

Jeder Schritt ist einzeln baubar und testbar. Größe: **K** klein, **M** mittel, **G** groß.

| # | Schritt | Neu oder geändert | Prüfung | Größe |
|---|---|---|---|---|
| 1.0 | Doku-Stand: Spieler-ID-Korrektur (erledigt), Fehler in Dokument 21 §3 beheben (Abschnitt 6) | `docs/21`, `docs/23` | – | K |
| 1.1 | **Daten erzeugen:** das Skript `tools/data/creatures_from_gemini.py` schreibt zusätzlich JSON (siehe Abschnitt 4) | Skript, 7 JSON-Dateien | Skript meldet Zählungen (48 Kreaturen, 162 Gegenstände, Beute-Einträge); JSON ist gültig | M |
| 1.2 | **Typen und Lader** für Kreaturen, Tiere, Spawn und Beute mit Querprüfungen | `GamerCreatureData.h/.cpp` (neu), `GamerDataSubsystem.*`, Erreichbarkeit | `Gamer.Creatures.*` (Abschnitt 5) | G |
| 1.3 | **Kampfmathematik** als reine Funktionen und `combat.json` | `GamerCombatMath.h/.cpp` (neu) | `Gamer.Combat.*` | M |
| 1.4 | **Würfel:** Rang und Streuung beim Spawn, Anführer-Stufen, Beute (Mengen-Deckel, Dämpfung, `yield`) | `GamerCreatureRoll.h/.cpp` (neu) | `Gamer.Loot.*`, `Gamer.Spawn.*` | M |
| 1.5 | **Level und Erfahrung:** `progression.json` (Kurve, Stufen-Grenzen, Ausrüstungswerte je Stufe, Fähigkeitspunkte), reine Funktionen, `min_level` an Rezepten (nur lesen, noch nicht erzwingen) | `GamerProgression.h/.cpp` (neu), `GamerDataTypes.h`, Rezept-Lader | `Gamer.Progression.*` | M |
| 1.6 | **LP-Formel** (100 + 5 × (Zähigkeit − 5) + Buff) als reine Funktion mit Test; **Einbau** in `GamerHealthComponent` erst in Schritt 2 (dort liegt das Profil des Spielers) | `GamerCombatMath` | `Gamer.Combat.PlayerHp` | K |
| 1.7 | **Zusammenführen:** Subsystem hält alles, `Reload` und Prüfsumme umfassen die neuen Dateien; Mehrspieler-Test: Server und Client melden dieselbe Prüfsumme | `GamerDataSubsystem.*`, `GamerDataLoader.cpp` | `tools/mp_test.ps1`, `Gamer.Data.*` | K |
| 1.8 | **Doku:** `docs/06_datenformat.md` (neue Tabellen), `docs/02_phasenplan.md` (Stand), Entscheidungslog (nur Ergänzungen) | Docs | – | K |

Reihenfolge: 1.0 → 1.1 → 1.2 → 1.3 (parallel zu 1.2 möglich) → 1.4 → 1.5 → 1.6 → 1.7 → 1.8. Nach jedem Schritt: bauen, Tests laufen lassen, Ergebnis melden.

## 4. Daten (Schritt 1.1)

Nach der Erzeugung ist **JSON die Quelle** (Regel 3, lesbare Unterschiede in Git). Das Skript wird danach nicht mehr gebraucht; `docs/22` bleibt als Stand der Erzeugung stehen (offene Frage 1).

| Datei | Inhalt (je Zeile) | Umfang |
|---|---|---|
| `creatures.json` | `id`, `name` (Anzeigename deutsch), `gemini_name`, `min_rank`, `home_biome`, `wave`, `role`, Werte je Rang (`hp`, `poise`, `dmg`), Trefferzonen-Faktoren | 48 |
| `animals.json` | wie Kreaturen, dazu Verhalten (`flees`, `pack`, `ambush` …); Tiere nach Dokument 21, Abschnitt 8 | 12 (offene Frage 2) |
| `spawns.json` | je Biom: Kreatur, `weight`, `scale_to_zone`, `scale_chance`, `max_rank`, `fixed_rank`, `only_places` | ca. 90 |
| `creature_loot.json` | je Kreatur: `base`, `rank`, `biome` mit Gegenstand, Menge, Chance, `min_rank`, `scale_count` | 48 Kreaturen, 361 Einträge (davon geteilte und gestrichene) |
| `items_creature.json` | Beute-Gegenstände (Kategorie `material`, Stufe, Gewicht, Stapel; Icons und Modelle leer) | 162 + Tierteile (ca. 20) |
| `progression.json` | Level-Kurve, Stufen-Grenzen (T1 1–8 …), Ausrüstungswerte je Stufe, Fähigkeitspunkte, Erfahrungsquellen | 1 |
| `combat.json` | Konstanten: Rüstung K = 160 und Deckel 0,75; Resistenz −25 % bis 60 %; Haltung K = 90; Zonenfaktoren; Angriffsfaktoren; Streuung; Anführer-Stufen; `creature_hp_scale` | 1 |

**Wichtig:** Der Lader liest heute nur `items.json`. Er bekommt eine Liste von Item-Dateien (`items.json`, `items_creature.json`). Die Gegenstände bekommen leere Icon- und Modellpfade; das ist laut Lader eine **Warnung**, kein Fehler („Modell fehlt noch“). Die Erreichbarkeitsprüfung meldet sonst jeden Beute-Gegenstand als „nicht erreichbar“, weil er weder gesammelt noch hergestellt wird. Deshalb liest sie nach dem Muster von `GamerCharacterData.cpp` (leichtes Vorablesen) die Beute-Tabelle als Quelle mit.

## 5. Tests (Zielzustand)

| Test | Prüft | Erwartung |
|---|---|---|
| `Gamer.Creatures.ShippedDataIsValid` | alle neuen Tabellen laden ohne Fehler | 48 Kreaturen, jede hat ein Spawn-Biom |
| `Gamer.Creatures.LootItemsExist` | jede Gegenstands-ID der Beute steht in `items.json` oder `items_creature.json` | keine unbekannte ID |
| `Gamer.Creatures.HomeRule` | Kreatur kommt nie vor ihrer Welle vor | keine Verletzung |
| `Gamer.Creatures.RankRules` | kein Spawn unter MinRang, feste Ränge ≥ MinRang, Drachensehne nie unter Rang 5 | keine Verletzung |
| `Gamer.Creatures.ValidatorFindsErrors` | absichtlich kaputte Daten werden gemeldet (prüft den Prüfer, wie bei den bestehenden Tests) | alle Fälle gemeldet |
| `Gamer.Combat.Armor` | `Reduktion(240) = 0,6`; Deckel 0,75; Mindestschaden 1 | exakt |
| `Gamer.Combat.Resist` | Klemmung bei −25 % und 60 % | exakt |
| `Gamer.Combat.Poise` | Faktor bei 150 Widerstand = 0,375; Haltungsbruch setzt die Leiste zurück | exakt |
| `Gamer.Combat.Zones` | Rüstung je Zone aus den Slots; fehlender Helm = Kopf ohne Schutz | exakt |
| `Gamer.Combat.PlayerHp` | Zähigkeit 3 / 5 / 8 → 90 / 100 / 115 | exakt |
| `Gamer.Loot.Deterministic` | gleicher Startwert = gleiche Beute | gleich |
| `Gamer.Loot.Statistics` | 100 000 Würfe: gemessene Chance und Menge liegen innerhalb von 2 % der Erwartung | in Toleranz |
| `Gamer.Loot.CountCap` | Mengenbonus höchstens + 2, auch bei Rang 6 und „ab Rang 2“ | gedeckelt |
| `Gamer.Loot.Dampening` | Dämpfung 0,85 / 0,70 / 0,55 nur bei Biom-Zusatzbeute | nur dort |
| `Gamer.Spawn.RankAndSpread` | „wächst mit 85 %“: Anteil am Zonenrang 85 % ± 2 %; ein Wurf für alle Werte (LP, Haltung, Schaden skalieren gleich) | in Toleranz |
| `Gamer.Spawn.Leader` | Anführer-Stufen nur in erlaubten Rängen; Häufigkeit 5 / 3 / 2 % ± 1 % | in Toleranz |
| `Gamer.Progression.Curve` | Kurve steigt streng, Level 50 erreichbar, jede Stufe T1–T6 hat Level-Bereich ohne Lücke und Überlappung | gültig |
| `Gamer.Recipes.MinLevel` | Rezepte mit `min_level` laden; ohne Angabe 0 | gültig |

Dazu bleiben alle 13 bestehenden Tests grün. Die Würfel-Tests laufen mit festem Startwert, damit sie nicht zufällig scheitern.

## 6. Bekannter Fehler in Dokument 21, den dieser Schritt behebt

Dokument 21, Abschnitt 3 gibt die Erfahrung „100 × Level^1,5 je Level“ an und für einen besiegten Gegner „10 × Rang“. Rechnet man das nach, braucht man bis Level 8 (Ende T1) rund 6 100 Erfahrung, also etwa **600 besiegte Rang-1-Gegner**, und bis Level 50 rund 690 000. Das ist offensichtlich nicht gemeint. **Behebung in Schritt 1.5:** Zuerst wird festgelegt, wie viele Spielstunden jede Stufe dauern soll (offene Frage 3). Daraus wird die Kurve berechnet, mit einer kleinen Simulation einer „typischen Stunde“ (Mischung aus Kämpfen, Sammeln, Bauen, Herstellen). Erst dann stehen die Erfahrungswerte fest. Die Prüfung `Gamer.Progression.Curve` bekommt die Stunden-Ziele als Grenzwerte.

**Stand 2026-11-22 (Schritt 1.1):** Die neue Kurve steht in `progression.json` und ist in `docs/21`, Abschnitt 3, beschrieben (Annahme 6/8/10/12/14/16 Stunden, 460 Erfahrung je Stufe und Stunde, 122 370 Erfahrung bis Level 50, Bedarf je Level steigt überall).

## 7. Abnahme

- Alle neuen und alten Tests grün (Log unter `game/Saved/Tests/`).
- Build für Editor und Server grün (`tools/build.ps1`).
- Server und Client melden dieselbe Daten-Prüfsumme (`tools/mp_test.ps1`).
- Das Spiel startet und verhält sich unverändert (kurzer Spieltest: Menü, Welt erstellen, Laufen, Sammeln).
- `docs/06`, `docs/02` und das Entscheidungslog sind angepasst.

## 8. Risiken und Absprachen

- **Zwei Sitzungen im selben Ordner.** Die Sitzung „Sagen-Bosse“ arbeitet ebenfalls in `game/` (Menü, Tests) und baut dort. Es kann immer nur **ein** Unreal-Build laufen, und der Editor muss geschlossen sein (`CLAUDE.md`). Vor jedem Build prüfe ich, ob die andere Sitzung fertig ist (sie meldet sich nach jedem Durchgang).
- **Kernelementare Dateien:** Dieser Plan fasst keine an (`game/Config/*`, `*.Build.cs`, `CLAUDE.md`). `Gamer.Build.cs` braucht kein neues Modul (Json ist schon drin). Sollte sich das ändern, frage ich vorher.
- **Aufgeblähter Lader:** Neue Lader kommen in eigene Dateien (wie `GamerCharacterData.cpp`), nicht in die 1 739 Zeilen von `GamerDataLoader.cpp`.
- **Prüfsumme und alte Spielstände:** Neue Dateien ändern die Prüfsumme. Alte Welten laden weiter, weil keine bestehende Tabelle geändert wird; die neuen Tabellen sind für Spielstände zunächst ohne Wirkung.
- **Erreichbarkeit:** Ohne die Anpassung (Abschnitt 4) meldet die Prüfung 160 „nicht erreichbare“ Gegenstände als Fehler. Das ist die wahrscheinlichste Stolperstelle.
- **Zahlen bleiben Startwerte.** Schritt 1 legt sie fest und prüft ihre Form, nicht ihre Spielbarkeit.

## 9. Offene Fragen (vor dem Start)

1. **JSON als Quelle, Skript danach nicht mehr:** Empfehlung ja.
2. **Tiere gleich mit in Schritt 1** (nur Daten, 12 Tiere, ca. 20 Tierteile): Empfehlung ja, weil sie die einfachsten Gegner sind und Stufe-1-Teile brauchen.
3. **Spielstunden je Stufe** für die Erfahrungskurve. Vorschlag als Ausgangspunkt (solo): Fjordwiesen 6 h, Gezeitenmarsch 8 h, Heide 10 h, Basaltfjell 12 h, Glutküste 14 h, Eisrand 16 h, zusammen etwa 66 h bis Level 50. Im Koop schneller durch die Anteile. Das ist nur eine Annahme; die Story-Kapitel sagen dazu nichts.
