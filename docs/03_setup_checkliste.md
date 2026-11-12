# Setup-Checkliste für deinen PC

## Was ich gesehen habe (nur Ordnernamen, Stand 2026-11-12)
- Erster Stand: Kein Epic-Games- oder Unreal-Ordner auf `C:`, `D:` oder `E:`.
- Späterer Stand (2026-11-12): Epic Games Launcher ist installiert (`C:\Program Files\Epic Games\Launcher`). Unreal Engine 5.8.4 liegt in `D:\gamer\unreal\UE_5.8` (`Engine\Binaries\Win64\UnrealEditor.exe`, `Build.version`, `ToolComplete.txt` vorhanden). Den Editor habe ich nicht gestartet.
- `Microsoft Visual Studio` ist unter `C:\Program Files (x86)` vorhanden. Welche Version und Komponenten, steht weiter unten.
- Git ist vorhanden: `D:\Program Files\Git`. Ob Git LFS dabei ist, sehe ich nicht. (Meine erste Einschätzung „Git fehlt“ war falsch, ich hatte nur `C:` geprüft.)
- Freier Platz (im Explorer abgelesen, 2026-11-12): `C:` 22,7 GB von 236 GB (knapp), `D:` 904 GB von 931 GB, `E:` 321 GB von 931 GB.
- Visual Studio Installer zeigt: „Visual Studio Build Tools 2022“ (17.14.31) und „Visual Studio Build Tools 2026“ (18.3.2). Das sind Build Tools, keine IDE. Welche Komponenten (C++-Workload, Profiling-Tools, AddressSanitizer, Windows SDK) enthalten sind, habe ich noch nicht abgelesen.
- Projektort: Der Ordner `Desktop\gamer` ist leer, der Inhalt liegt jetzt in `D:\gamer\gamer` (15 eigene Dateien, `reference/` mit 889 Dateien). Der Epic-Installer `EpicInstaller-20.3.3-unrealEngine.exe` (252 MB, heruntergeladen am 2026-11-12) liegt in `D:\gamer\programme`.
- Nachtrag (2026-11-12, per `vswhere -all -products *` geprüft): Gefunden wird nur noch „Visual Studio Build Tools 2026“ (18.12.12405.243) in `C:\Program Files (x86)\Microsoft Visual Studio\18\BuildTools`. Build Tools 2022 tauchen nicht mehr auf. Erster Build `GamerEditor Win64 Development` (2026-11-12): **fehlgeschlagen (RulesError)**, weil das **.NET Framework SDK (NetFxSDK, ≥ 4.6)** fehlt (benötigt von `SwarmInterface`). Außerdem warnt UBT: Der vorhandene MSVC 14.52 ist neuer als der bevorzugte 14.50 und wenig getestet. Windows SDK 10.0.26100.0 ist vorhanden. Danach über den Visual Studio Installer nachinstalliert: `Microsoft.Net.Component.4.8.SDK` und `Microsoft.VisualStudio.Component.VC.14.50.18.0.x86.x64`. Zweiter Build (2026-11-12): **erfolgreich**, mit MSVC 14.50.35717 und Windows SDK 10.0.26100.0, Dauer ca. 113 s. Verbleibende Warnung: „Visual Studio SDK nicht gefunden, Editor-Integration deaktiviert“ (betrifft nur die VS-IDE-Anbindung, nicht den Build).
- Git LFS 3.7.1 ist vorhanden.
- Node.js (`C:\Program Files\nodejs`) und Python (`C:\Python314`) sind vorhanden.
- Hardware (am 2026-11-12 im Task-Manager abgelesen): RTX 3070 mit 8 GB, 16 GB RAM (im Alltag schon zu ca. 70 % belegt), SSDs: `C:` NVMe, `E:` NVMe, `D:` SATA. CPU-Modell habe ich nicht abgelesen, freier Platz siehe unten. Details: `docs/04_technische_ziele.md`.

## Schritte (in dieser Reihenfolge)
1. (erledigt, Ort weicht vom Vorschlag ab: `D:` statt `E:`) **Epic Games Launcher** installieren, dann **Unreal Engine 5.8** (braucht viele GB freien Platz). Der Installer liegt schon in `D:\gamer\programme`. Ziel für Launcher und Engine: `E:` (NVMe, 321 GB frei), nicht `C:` (nur 22,7 GB frei) und nicht die SATA-SSD `D:`. Die Engine lädt der Launcher erst nach deinem Epic-Login herunter.
2. **Visual Studio 2022** mit der Komponente „Spieleentwicklung mit C++“ (inkl. Windows SDK) – bei vorhandenem VS im Visual Studio Installer prüfen/ergänzen.
3. (erledigt 2026-11-12: `git init -b main`, `git lfs install --local`, erster Commit 1d32729) **Git LFS** prüfen bzw. installieren (Git selbst ist vorhanden) und in `D:\gamer\gamer` ein Repository anlegen (`git init`, `git lfs install`).
4. (erledigt, Version 2.1.177) **Claude Code lokal** installieren (PowerShell, ohne Administrator: `irm https://claude.ai/install.ps1 | iex`) und in `D:\gamer\gamer` starten (so kann es direkt bauen und den Editor ansteuern).
5. (erledigt 2026-11-12: `https://github.com/wking-game/RPG`, **öffentlich** nach Entscheidung des Nutzers, Push als Konto `wking-game`; LFS-Lock-Prüfung lokal abgeschaltet: `lfs.<remote>/info/lfs.locksverify false`) Privates **GitHub-Repository** anlegen (dient auch zur Abstimmung zwischen den Agenten).
6. (angelegt, VS-Projektdateien erzeugt über `Engine\Build\BatchFiles\Build.bat -projectfiles`, `GamerEditor Win64 Development` am 2026-11-12 erfolgreich gebaut, Editor damit noch nicht geöffnet) **Unreal-Projekt** in `game/`: C++, leere Vorlage `TP_Blank` als `Gamer`, kein Starter Content. Erster Start: siehe `game/README.md`.
7. Erst danach **MCP/Editor-Anbindung** (Epics experimentelles MCP-Plugin oder ein geprüftes Drittprojekt).

## Hinweise
- Ich habe bewusst kein `git init` in dem verbundenen Ordner ausgeführt: Über diese Verbindung kann Git seine Sperrdateien nicht löschen, das würde das Repository blockieren. Git bitte auf der Windows-Seite initialisieren.
- Alles unter `reference/` ist fremder Code zum Nachlesen. Er wird nicht ausgeführt und ist nicht Teil des Spiels.
- Mit 16 GB RAM wird es beim Arbeiten im Editor eng. Browser und Chat-Fenster beim Bauen schließen.
