# Review 02 – Arbeitsstätten (3. Fassung), Armbrust, Rüstung, Entscheidungsdelta (2026-11-12)

*Von Claude Code. Geprüft: `design/inbox/04`–`07` gegen `docs/01_entscheidungen.md` und die Asset-Listen in `design/`.*
*Für ChatGPT: Bitte Punkt 3 (Slots) in einer überarbeiteten `06` umsetzen. Die offenen Fragen klärt der Nutzer.*

## 1. `04_assets_arbeitsstaetten.md` (3. Fassung) – passt
Beeren und Mahlzeiten liegen jetzt in `prop_basket` und `prop_sack`, wie in `design/ASSET_LIST_GEGENSTAENDE.md` vorgeschlagen. Keine weiteren Anmerkungen. Der kleine Punkt aus Review 01 (Sockel des Lagers am Hang) gilt weiter beim Modellieren.

## 2. `05_assets_armbrust.md` – passt, eine offene Stufe
- Teile-Strategie, Budgets (Ziel 2.000 / max. 4.000) und Benennung passen zur Waffenliste.
- **Offen:** Codex schlägt **ab T4** (seltene Import-/Schwarzmarktware) vor, Claude **ab T3**. Entscheidet der Nutzer. T4 passt gut zum bestätigten Nacht-Händler.
- **Neu bestätigt vom Nutzer:** Freischaltung über bestimmte Attribute plus eine Fähigkeit im Skilltree. Das sollte in 05 ergänzt werden.

## 3. `06_assets_ruestung_kleidung.md` – Slots müssen angepasst werden
Der Nutzer hat am 2026-11-12 diese Slots festgelegt: **Helm, Schulter, Hände, 2 × Ring, Beine, Füße, Umhang** (Umhang nach dem ersten Winter freigeschaltet).

| Punkt | 06 (Codex) | Nutzer-Entscheidung | Änderung |
|---|---|---|---|
| Schulter | fehlt | eigener Slot | ergänzen (`slot_shoulders`) |
| Ringe | kein Schmuck | 2 × Ring | ergänzen, nur Icons; Verhältnis zu „kein Schmuck-Slot“ ist offen |
| Oberkörper | `slot_torso` | nicht genannt | **offen**, vermutlich vergessen – bis zur Klärung beibehalten |
| Umhang | optional kosmetisch | Slot, Freischaltung nach dem ersten Winter | anpassen; Kälteschutz als offene Frage |
| Haltbarkeit Rüstung | offen | **ja** | übernehmen |

Gute Punkte aus 06, die Claude übernommen hat: ein Oberkörper-Slot statt zwei Lagen, Name `T_Clothing_Atlas`, einfache Umhang-Physik, `fresh`/`worn` als einzige optische Zustände. Abgeglichene Liste: `design/ASSET_LIST_RUESTUNG.md`.

## 4. `07_assetentscheidungen_2026-11-12.md` – zwei Abweichungen
- **Tierhaltung:** 07 schreibt „später“. Der Nutzer hatte „ja, einfach gehalten“ gesagt, ohne Zeitpunkt. Eingetragen ist „ja“.
- Die Antworten des Nutzers auf die offenen Fragen in 07 stehen nicht in der Datei, sondern in `docs/01_entscheidungen.md`.

## 5. Neue Entscheidungen des Nutzers (eingetragen in `docs/01_entscheidungen.md`)
- Feldbau ab Phase 2.
- Armbrust: Freischaltung über Attribute plus eine Fähigkeit.
- Haltbarkeit auch für Rüstung.
- Slots wie in Punkt 3.
- Es gibt **Jahreszeiten mit Winter**; der erste Winter ist nicht hart.
- Verderben: Fleisch 4 Tage, Obst und Gemüse 2,5 Tage, wenn nicht trocken gelagert; sonstige Lebensmittel 3 Tage.

## 6. Offene Fragen an den Nutzer
1. Armbrust ab **T3 oder T4**?
2. **Oberkörper-Slot** – vergessen oder bewusst weggelassen?
3. **Ringe vs. „kein Schmuck-Slot“** – sind Ringe die Ausnahme?
4. **Verderben:** Spieltage? Hält trockene Lagerung unbegrenzt? Verderben auch gekochte Speisen?
5. **Umhang:** auch Kälteschutz im Winter?
6. **Jahreszeiten** sind ein neues System (Wetter, Schnee, Feldbau, Kälte). In welcher Phase? Vorschlag: Phase 2 zusammen mit dem Feldbau, der Slice bleibt ohne Jahreszeiten.
