# Vision (Stand 2026-11-12)

Arbeitstitel: offen · Plattform: PC · Engine: Unreal Engine 5.8 (geplant)

## Kurzbeschreibung
Wikinger-/Mittelalter-Open-World mit Koop. Wie in **Aska** führst du als Anführer eine Siedlung: Dorfbewohner übernehmen
die Routinearbeit, du erkundest die Welt, kämpfst, baust und besiegst Bosse. Mehrere Questlinien tragen die Open World.

## Leitspiel und Entlehnungen
| Spiel | Wir übernehmen |
|---|---|
| **Aska (Leitspiel)** | Dorfbewohner-/Arbeitersystem, Siedlungsführung, Koop |
| Valheim | Bosse und Endgame |
| Kingdom Come: Deliverance 2 | Kampfsystem (ohne Ausdauer-Sperren) |
| Medieval Dynasty | Aufbau, Siedlungs-Setup |
| Manor Lords | Detailtiefe, Strukturen |
| ~~Tribes of Midgard~~ | gestrichen |

## Kernmerkmale
- **Koop:** Start mit 2–4 Spielern, später bis zu **16 Spieler** (Ziel); solo spielbar. Ab mehr als ca. 4 Spielern vermutlich Dedicated Server (siehe `docs/04_technische_ziele.md`).
- **Basisbau** mit Bauteilen (Snapping), Siedlung mit Dorfbewohnern.
- **Ausrüstung** wie in KCD: Slots, levelbasiert, schmiedbar. **Skilltree** für Fähigkeiten.
- **Kamera:** Third-Person als Hauptansicht; Vogelperspektive/2.5D (Vorschlag: als Bau- und Dorf-Kommandoansicht).
- **Keine Ausdauer-Sperren** beim Laufen, Ducken, Waffengebrauch. Das Essens-Buff-System bleibt.
- **Portalsystem** erst im späteren Spiel: große Ressourcenmengen von A nach B für den Basisausbau und zur Verteidigung großer Angriffswellen.
- **Weltoptionen im Menü, kombinierbar:** freies Bauen, Horden bekämpfen, „Survivor“ (Bedeutung noch offen). Basisangriffe optional.
- **Freies Tempo:** kein Zeitdruck als Kernschleife.
- **Schwierigkeit:** steigt stufenweise (Vorschlag: nach Biom und Boss, nicht stufenlos exponentiell).

## Das Dorfbewohner-System (Kernrisiko)
Askas bekannte Schwächen: zu viel Babysitting, schwache KI, klobige Lager-/Menü-Bedienung. Unser Gegenentwurf (Vorschläge):
- Bedürfnisse statt Uhr-Zeitplan.
- Prioritätslisten mit Fallback-Jobs.
- Ein Lager pro Siedlung.
- Übersichtliche Dorf-Kommandoansicht.
- Simulation nur auf dem Host bzw. Server.
- Portale verbinden Siedlungslager; Dorfbewohner arbeiten lokal.

## Bewusst nicht im Umfang (vorerst)
Voxel-Terraforming (Enshrouded-Stil).
