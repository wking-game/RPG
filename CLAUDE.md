# CLAUDE.md – Regeln für Claude Code in diesem Projekt

Projekt: Open-World-Koop-Siedlungs-RPG (Wikinger/Mittelalter), Unreal Engine 5.8 (geplant), Leitspiel Aska.
Konzept: `docs/00_vision.md` · Entscheidungsstand: `docs/01_entscheidungen.md` · Phasen: `docs/02_phasenplan.md`

## Rollen
- **Claude Code:** C++-Code, Systeme, Datenformate, Tools, Projektstruktur, Build und Tests, Umsetzung der Design-Vorgaben.
- **ChatGPT:** Design (Art Direction, UI/HUD-Konzepte, Looks). Seine Ergebnisse liegen in `design/inbox/`. Er arbeitet nicht in `game/`.
- **Nutzer (Klio):** trifft alle Entscheidungen. Spricht oft per Spracheingabe, Wortlaut kann unscharf sein: Deutung nennen, nicht still raten.

## Verhaltensregeln (vom Nutzer vorgegeben, gelten immer)
1. **Ehrlich schreiben.** Keine Schönfärberei, Unsicheres als unsicher kennzeichnen.
2. **Eigene Ergebnisse selbst prüfen** (bauen, testen, Datei öffnen), bevor „fertig“ gemeldet wird. Was nicht geprüft werden konnte, ausdrücklich nennen.
3. **Anfragen und Ergebnisse hinterfragen**, auch die des Nutzers: Widersprüche, unrealistischen Umfang und Nachteile ansprechen.
4. **Bestätigungspflicht, jedes Mal neu** (keine Generalfreigabe, keine Generalablehnung): Vor dem **Verschieben, Umbenennen oder Löschen** von Dateien und vor dem **Bearbeiten kernelementarer Dateien** ausdrücklich fragen und die Antwort abwarten.
   Kernelementar sind: `CLAUDE.md`, `AGENTS.md`, `.gitignore`, `.gitattributes`, `*.uproject`, `game/Config/*`, `*.Build.cs`, `*.Target.cs`, Plugin-Einstellungen.
   Neue Dateien anlegen ist ohne Rückfrage erlaubt. Einträge in `docs/01_entscheidungen.md` nur ergänzen, getroffene Entscheidungen nie ohne Bestätigung ändern.

## Architekturregeln
Regeln 1, 3, 4, 7 sind Vorschläge von Claude; ihr Status steht in `docs/01_entscheidungen.md`.

1. **Server-autoritativ:** Der Host entscheidet über Spielzustand. Clients senden nur Anfragen (RPCs), nie Zustandsänderungen.
2. **Solo = Koop mit 1 Spieler:** derselbe Server-Code-Pfad. Der Host kann Listen- oder Dedicated Server sein. Kein separater Einzelspieler-Code.
3. **Datengetrieben:** Items, Rezepte, Skills, Dorfbewohner-Jobs, Gebäude, Gegner liegen in DataTables/DataAssets bzw. JSON (`data/`). Keine Spielwerte hartkodiert.
4. **C++ für Systeme**, Blueprints nur für UI-Verdrahtung und Feinschliff (KI-gesteuerte Blueprint-Bearbeitung ist fehleranfällig).
5. **Keine Ausdauer-Sperren:** Laufen, Ducken und Waffengebrauch werden nie durch einen Energiebalken blockiert. Essen bleibt ein Buff-System (Gesundheit, Regeneration, Traglast, Resistenzen, Schaden).
   Für Kampftiefe stattdessen: Parierfenster, Gegner-Haltungsleiste, Abklingzeiten für Spezialangriffe.
6. **Kein Zeitdruck als Kernschleife.** Angriffe auf die Basis sind eine optionale Weltoption.
7. **Dorfbewohner-Simulation läuft nur auf dem Host.** Bedürfnisse statt Uhr-Zeitplan, Prioritätslisten mit Fallback-Jobs, ein Lager pro Siedlung statt vieler Kisten.
   Im Vertical Slice klein bleiben: 3–4 Jobs, 3 Bedürfnisse.
8. **Weltoptionen** (freies Bauen, Horden, „Survivor“, Portale, Angriffe …) sind ein kombinierbares Modifier-Set pro Welt.
9. **Umfang klein halten:** erst Vertical Slice, dann ausbauen. Neue Features nur, wenn der Slice stabil läuft.
10. **Dedicated-Server-fähig ab Phase 0.** Kein Code darf einen Listen-Server voraussetzen. Die Spielerzahl ist konfigurierbar (höchstens 8, Start mit 2–4; `MaxPlayers` in `game/Config/DefaultGame.ini`), nie fest verdrahtet.
11. **Budgets:** fertiges Spiel höchstens ca. 20 GB, läuft mit mindestens 8 GB RAM (empfohlen 16 GB). Installationsgröße und Client-RAM ab Phase 1 laufend messen.

## Arbeitsweise
- Git läuft auf der Windows-Seite des Nutzers. Branch pro Aufgabe, kleine Commits mit klarer Nachricht.
- Unreal-Build nur bei geschlossenem Editor (UnrealBuildTool). Live-Coding nur für Funktionskörper.
- Nur **ein** Agent steuert den Editor gleichzeitig.
- Jede Gameplay-Funktion mit Host + 1 Client testen, sobald ein Projekt existiert.
- Fremdsoftware, Plugins und Assets nur mit dokumentierter Quelle und Lizenz. Keine ungeprüften Plugins.
- KI-generierte Inhalte (Bild, Audio, Text) in `design/asset_log.md` eintragen (Steam verlangt Angaben zu KI-Inhalten).

## Konventionen
- Code und Bezeichner Englisch, Kommentare kurz (Deutsch erlaubt), Doku Deutsch.
- Unreal-Präfixe: `A` Actor, `U` UObject, `F` Struct, `E` Enum, `I` Interface.
- IDs in Daten: `snake_case`, stabil, nie wiederverwenden.

## Aktueller Stand (2026-11-12)
- Projekt „Gamer“ liegt in `game/` (C++, Vorlage TP_Blank), noch nie gebaut. Projektordner: `D:\gamer\gamer`.
- Unreal Engine 5.8.4 (Launcher-Build) in `D:\gamer\unreal\UE_5.8`. Dedicated-Server-Target erst mit späterem Source-Build.
- Git ist vorhanden (`D:\Program Files\Git`).
- Entwicklungs-PC: RTX 3070 (8 GB VRAM), 16 GB RAM.
- Details: `docs/04_technische_ziele.md`, `docs/03_setup_checkliste.md`.
