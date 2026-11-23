# Schritt 2a: Ausrüstung und Spielerwerte (Plan 2026-11-22, umgesetzt)

Baut auf `docs/24` (Daten und Logik, fertig) und `docs/21` (Regeln) auf. **Umgesetzt am 2026-11-22 (Abschnitt 8). Abweichung: neun Plätze statt „10“ (Helm, Schulter, Oberkörper, Hände, Beine, Füße, Umhang, 2 Ringe).**

## 1. Ziel und Grenzen

**Ziel:** Der Spieler bekommt Ausrüstungsplätze, Rüstung und Resistenzen aus Gegenständen, die LP-Formel mit Zähigkeit und ein Level mit Erfahrung. Alles wird gespeichert, auf dem Server entschieden und im HUD sichtbar. Dafür braucht es noch **keinen Gegner**: Prüfbar über Testbefehle (Schaden mit Typ und Zone auf sich selbst) und automatische Tests.

**Gehört nicht dazu (2b und später):** Angriff und Trefferzonen am Gegner, Haltungsleiste im Kampf, Gegner-Spawn, KI, Beute-Auslösung, Skilltree (die 59 Fähigkeitspunkte werden gezählt, aber nicht ausgegeben), Modelle der Rüstung am Körper (`docs/11` Kleidung läuft getrennt), Erfahrung aus Kämpfen (die Quellen Herstellen, Bauen, Sammeln, Entdecken können aber schon zählen).

## 2. Was es schon gibt (daran halten wir uns)

- `GamerCombatMath` / `GamerProgression`: alle Formeln fertig und getestet, Daten in `combat.json` / `progression.json`.
- `UGamerHealthComponent`: ein Schadens-Eingang (`FGamerDamageInfo`, `IGamerDamageable`), Tod, Niederschlag, Heilung. Maximale LP = Basis + Buff „Gesundheit“.
- Statuswerte im Charakter (`stat_toughness` usw., `GamerPlayerState::GetStat`).
- Waffen sind Gegenstände der Kategorie `weapon` mit Typ, Schaden, Haltbarkeit; sie liegen in der Schnellleiste.
- Speichern pro Spieler über `IGamerSaveable` (Schlüssel je Komponente).

**Lücken:** Es gibt keine Rüstungs-Gegenstände, keine Ausrüstungsplätze, kein Level am Spieler, und Schaden kennt weder Zone noch Resistenzart.

## 3. Arbeitsschritte

| Nr | Inhalt | Dateien | Tests | Größe |
|---|---|---|---|---|
| 2a.1 | **Gegenstands-Daten:** Kategorie `armor` mit Block `armor` (Platz, Rüstungswert, Haltungs-Widerstand, Resistenzen je Art); ein Satz Starter-Rüstung pro Stufe T1 als Beispiel (Lederhelm, -wams, -handschuhe, -hose, -schuhe, Umhang, Ring) | `GamerDataTypes.h`, `GamerDataLoader.cpp`, `items.json` | `Gamer.Data.*` (Wertebereiche je Stufe aus `progression.json`), Erreichbarkeit | M |
| 2a.2 | **`UGamerEquipmentComponent`:** 10 Plätze (Helm, Schulter, Oberkörper, Hände, 2 × Ring, Beine, Füße, Umhang; Waffenhand und Schild bleiben Schnellleiste), Anlegen/Ablegen per Server-RPC, repliziert an den Besitzer, gespeichert | neu `GamerEquipmentComponent.h/.cpp`, `GamerCharacter` | `Gamer.Equipment.*` (Platz passt, Tausch, Speichern/Laden) | L |
| 2a.3 | **Werte-Zusammenfassung:** aus den Plätzen Rüstung je Zone, Haltungs-Widerstand und Resistenzen berechnen (reine Funktion, nutzt `GamerCombat::ZoneArmor` usw.) | `GamerCombatMath` oder neu `GamerEquipmentMath` | `Gamer.Equipment.Totals` | M |
| 2a.4 | **Schadens-Tür erweitern:** `FGamerDamageInfo` bekommt Zone und Resistenzart; `TakeDamage` rechnet Rüstung und Resistenz ein. Alter Schaden (Fall, Kälte, Tier) bleibt unverändert, solange Zone/Art leer sind | `GamerHealthComponent` | `Gamer.Health.*` (alte bleiben grün) plus neue Rechenfälle | M |
| 2a.5 | **LP-Formel einbauen:** `GetMaxHP` = `GamerCombat::PlayerMaxHp(Zähigkeit, Mahlzeit)`, ersetzt die feste Basis 100 aus `health.json` (Wert bleibt als Rückfall) | `GamerHealthComponent` | `Gamer.Health.MaxHpFromToughness` | K |
| 2a.6 | **Level und Erfahrung am Spieler:** `UGamerProgressComponent` (Gesamt-Erfahrung, Level, Fähigkeitspunkte gezählt), Server-Vergabe aus Herstellen/Bauen/Sammeln/Entdecken nach `xp_sources` mit Koop-Anteil, Speichern | neu `GamerProgressComponent.*`, Hooks in Sammeln/Herstellen/Bauen | `Gamer.Progress.*` | L |
| 2a.7 | **`min_level` erzwingen** bei Rezepten (bisher nur gelesen), mit klarer Meldung im Herstellmenü | Herstell-Code, HUD | `Gamer.Recipes.MinLevel` erweitern | K |
| 2a.8 | **HUD/Menü:** Ausrüstungsfenster (10 Plätze, Werte-Summe), Level und Erfahrungsbalken, Meldung bei Levelaufstieg | `GamerHUD*.cpp` | manuell + Screenshot | L |
| 2a.9 | **Testbefehle** (nur Test): `ServerDebugDamage` mit Zone und Art, Erfahrung geben; Mehrspieler-Test mit gleichen Werten auf Server und Client | `GamerHealthComponent`, `mp_test.ps1`-Ablauf | Mehrspieler-Test | M |
| 2a.10 | **Doku:** `docs/02`, `docs/06`, `docs/21` (Stand), Entscheidungslog nur ergänzen | docs | – | K |

Reihenfolge: 2a.1 → 2a.3 → 2a.2 → 2a.4 → 2a.5 → 2a.6 → 2a.7 → 2a.8 → 2a.9 → 2a.10. Nach 2a.5 ist der erste Build-/Test-Halt sinnvoll, nach 2a.8 der Spieltest.

## 4. Entwürfe, die du bestätigen sollst

1. **Waffe und Schild bleiben in der Schnellleiste**, die 10 Plätze sind nur Rüstung. Vorteil: nichts an der Waffen-Hand ändert sich. Nachteil: Schild gibt nur Rüstung, wenn er in der Hand ist; das passt zum Blocken in 2b.
2. **Ringe:** Die Resistenzen kommen vor allem aus Ringen, Umhang und Rüstungsteilen (`docs/21` 5.2); Werte je Stufe aus `progression.json` (`resist_cap`).
3. **Rüstungsgewicht:** Gegenstände haben `weight_kg`; schwere Rüstung belastet die Traglast wie alles andere. Keine Ausdauer (Regel 5).
4. **Alter Spielstand:** Spieler ohne Ausrüstung/Level werden als „Level 1, nichts angelegt“ geladen; kein Datenverlust (Schema-Stufe wie bei `docs/06`).
5. **Erfahrung vor dem Kampf:** Ohne Gegner wächst das Level nur aus Herstellen, Bauen, Sammeln und Entdecken (laut `docs/21` Abschnitt 3 ohnehin alle Quellen). Das ist gewollt und prüft die Kurve früh.
6. **Spielerwerte:** Zähigkeit liest der Server aus dem Charakter; Änderungen durch Lernen kommen später.

## 5. Risiken

- **Schadens-Tür:** Fall, Kälte, Tiere nutzen sie schon. Neue Felder dürfen deren Verhalten nicht ändern. Absicherung: die vorhandenen Tests plus ein Test „ohne Zone gleich wie vorher“.
- **Sichtbarkeit:** Rüstung erscheint nicht am Körper (Modelle fehlen). Das HUD zeigt nur Werte. Ehrlich so melden.
- **Speicherformat:** neue Schlüssel `equipment`, `progress` im Spieler-Datensatz; ältere Dateien müssen laden (Test `Gamer.Save.*`).
- **Umfang:** 2a.2, 2a.6 und 2a.8 sind die großen Posten. Reicht die Zeit nicht, schneidet man 2a.8 (Fenster) auf eine Textliste zu.
- **Ruhelage der HUD-Dateien:** `GamerHUD*.cpp` sind bereits groß; neues Fenster in eigene Datei `GamerHUDEquipment.cpp`.

## 6. Abnahme

- Alle alten und neuen Tests grün, Editor- und Server-Build grün.
- Mehrspieler-Test: Server und Client zeigen dieselbe Prüfsumme, Ausrüstung und Level kommen auf dem richtigen Spieler an.
- Spieltest von Hand: Rüstung anlegen, Werte ändern sich, Fallschaden wird mit Rüstung nicht kleiner (Fall ignoriert Rüstung, `docs/21`), Neustart: alles bleibt.
- Doku und Entscheidungslog angepasst.

## 7. Offen (vor dem Start)

- Fällt Fall-/Kälteschaden unter Rüstung? Vorschlag: nein (Fall = reiner Schaden, Kälte = Resistenz „Kälte“ statt Rüstung).
- Sollen Ringe/Umhang schon Resistenzen haben, obwohl Hitze und Sturm erst mit Wetter wirken? Vorschlag: ja, Daten jetzt, Wirkung später.
- Welches Level-Aufstiegs-Verhalten: nur Meldung, oder auch Heilung? Vorschlag: nur Meldung.

## 8. Stand der Umsetzung (2026-11-22)

Erledigt: 2a.1 bis 2a.10. Zu den offenen Fragen aus Abschnitt 7 wurden die Vorschläge umgesetzt: Fall und Kälte ohne Rüstung (Fall hat keine Zone, Kälte läuft über die Resistenz „cold“), Resistenzen an Ringen und Umhang als Daten jetzt, Levelaufstieg nur mit Meldung.

Abweichungen und Kleinigkeiten:
- **Bedienung:** Taste C öffnet das Ausrüstungsfenster (rechts oben); die Zahlentasten 1–7 legen Helm, Schulter, Oberkörper, Hände, Beine, Füße, Umhang an bzw. ab (angelegt wird das beste Stück der Stufe aus dem Inventar), 8 gilt für beide Ringe. Kein Ziehen mit der Maus.
- **Erfahrung:** Die Stufe einer Quelle ist die höchste Stufe der Gegenstände, um die es geht (Ernte, Ergebnis des Rezepts, Baukosten). Bauen zählt beim Platzieren, nicht beim Fertigstellen.
- **Lebenspunkte:** Ein neuer Spieler bekommt volle LP, sobald sein Charakter (und damit die Zähigkeit) bekannt ist; geladene Spielstände behalten ihren Wert.
- **Tod:** Angelegte Stücke gehen mit dem Inventar ins Grab bzw. verloren.
- **Testbefehle** (Testskript `-GamerTestScript`): `give`, `equip:<id>[|<platz>]`, `unequip:<platz>`, `gear`, `hit:<menge>|<zone head/body/limbs/none>[|<resistenzart>]`, `xp[:<menge>]`, `gearmenu:on/off`.
- **Nicht geprüft:** siehe `docs/02_phasenplan.md` („Stand Schritt 2a“).
