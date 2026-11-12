# game/

Unreal-Engine-5.8-Projekt (C++), Arbeitsname **Gamer**. Der Name ist nur der interne Modulname, nicht der Spieltitel.

## Stand (2026-11-12)
- Angelegt als Kopie der Epic-Vorlage `TP_Blank` (C++, leer) aus `D:\gamer\unreal\UE_5.8\Templates\TP_Blank`, umbenannt in `Gamer`. Kein Starter Content, keine Spielfigur.
- Enthalten: `Gamer.uproject` (`EngineAssociation` „5.8“ = Launcher-Build), `Config/`, `Source/Gamer.Target.cs`, `Source/GamerEditor.Target.cs`, `Source/Gamer/` (leeres Primary-Game-Modul).
- **Noch nie gebaut und nie im Editor geöffnet.** Die Dateien stammen aus der Vorlage; ob alles baut, zeigt erst der erste Build.
- Kein Server-Target: Der Launcher-Build kann keine Server-Targets bauen (siehe `docs/01_entscheidungen.md`). Kommt mit dem späteren Source-Build.

## Erster Start (auf der Windows-Seite)
1. Rechtsklick auf `Gamer.uproject` → „Visual Studio-Projektdateien generieren“.
2. Danach den Editor-Target bauen (z. B. `Gamer Editor`, Development) oder die `.uproject` per Doppelklick öffnen (der Editor baut dann selbst).
3. Dafür muss die C++-Workload mit Windows SDK in Visual Studio bzw. den Build Tools vorhanden sein.

Hinweis: Bauen nur mit geschlossenem Editor (siehe `CLAUDE.md`).
