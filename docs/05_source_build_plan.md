# Plan: Source-Build der Engine für den Dedicated Server (Stand 2026-11-12)

Ziel: Phase 0 abschließen – ein Dedicated-Server-Target (`GamerServer`) bauen und mit 2 Clients testen.
Grund: Der Launcher-Build kann keine Server-Targets bauen (Entscheidung in `docs/01_entscheidungen.md`).

## Ausgangslage (am 2026-11-12 gemessen)
| | Wert |
|---|---|
| CPU | Intel Core i5-12400F, 6 Kerne / 12 Threads |
| RAM | 16 GB, Auslagerungsdatei auf `D:` |
| `C:` (NVMe) | 16 GB frei, nach Verlegen der Build Tools auf `E:` **26,3 GB frei** (2026-11-12) |
| `D:` (SATA-SSD) | 876 GB frei |
| `E:` (NVMe) | 322 GB frei |
| Toolchain | Build Tools 2026 in `E:\VS\BuildTools` mit MSVC 14.50, Windows SDK 10.0.26100, .NET Framework 4.8 SDK |
| Zugriff auf `EpicGames/UnrealEngine` | vorhanden über GitHub-Konto `wking-game` (2026-11-12), Tag `5.8.4-release` sichtbar |

## Schätzungen (nicht gemessen, für UE 5.8 nicht verifiziert)
- **Speicher:** Quellcode + Abhängigkeiten (`Setup.bat`) + Build von Editor und Server: grob **150–250 GB**.
- **Dauer erster Engine-Build:** auf 6 Kernen mit 16 GB RAM grob **4–8 Stunden**. Unreal begrenzt die parallelen Compiler-Prozesse nach freiem RAM (beim Projekt-Build waren es 3 statt 6), deshalb eher am oberen Ende.
- Spätere Builds sind inkrementell und deutlich schneller.

## Schritte
### A. Vom Nutzer zu erledigen (Konten, nicht automatisierbar)
1. (entschieden 2026-11-12) GitHub-Konto für den Engine-Zugang: **`wking-game`**.
2. (erledigt 2026-11-12) Auf epicgames.com: Konto → „Apps und Konten“ → GitHub verbinden (mit dem gewählten Konto).
3. (erledigt 2026-11-12) Die E-Mail-Einladung in die GitHub-Organisation `EpicGames` annehmen.
   Danach sieht das Konto `github.com/EpicGames/UnrealEngine`.

### B. Vorbereitung (Claude, nach Freigabe)
4. **`C:` entlasten:** Cache-Pfade der Engine auf `D:`/`E:` legen (z. B. lokaler Derived Data Cache über die Umgebungsvariable `UE-LocalDataCachePath`). Ohne das kann `C:` beim Build volllaufen. Genaue Variablen vor dem Setzen in der Epic-Doku prüfen.
5. Zielordner mit **kurzem Pfad** wählen (Windows-Pfadlängen), Vorschlag `E:\UE58` (NVMe, schneller). Alternative `D:\UE58` (mehr Platz, SATA).
6. Den passenden Release-Tag ermitteln (`git ls-remote --tags`, erwartet etwas wie `5.8.4-release`, passend zur installierten 5.8.4).

### C. Engine bauen (Claude, läuft lange im Hintergrund)
7. Flacher Klon nur dieses Tags: `git clone --depth 1 --branch <tag> https://github.com/EpicGames/UnrealEngine.git E:\UE58`
8. `Setup.bat` (lädt Abhängigkeiten, installiert Voraussetzungen), dann `GenerateProjectFiles.bat`.
9. Engine-Editor bauen (`UnrealEditor Win64 Development` und die Hilfsprogramme wie ShaderCompileWorker).

### D. Projekt umstellen (Kerndateien → jeweils Rückfrage)
10. `Gamer.uproject`: `EngineAssociation` auf den Source-Build umstellen (per Rechtsklick „Switch Unreal Engine version“ oder Eintrag in der Registry).
11. Neue Datei `game/Source/GamerServer.Target.cs` (Type = Server).
12. `tools/build.ps1`: Schritt `Server` ergänzen, `UE_ROOT` auf den Source-Build zeigen lassen.

### E. Prüfen
13. `GamerServer` bauen und als Server-Paket packen (`BuildCookRun -server -noclient -serverplatform=Win64`).
14. Test: Dedicated Server + 2 Clients auf diesem PC; Server-RAM und Tick-Zeit notieren (Basiswert für `docs/04_technische_ziele.md`).
15. Phase 0 im Phasenplan abschließen.

## Risiken und Nachteile
- **Zeit und Speicher:** mehrere Stunden Build, 150–250 GB belegt. Der PC ist in der Zeit stark ausgelastet.
- **`C:` fast voll:** ohne Schritt 4 Risiko eines Abbruchs mitten im Build.
- **Zwei Engines parallel:** Launcher-Build bleibt als Rückfallebene. Projekt-Binaries müssen beim Wechsel neu gebaut werden; Editor-Einstellungen und Plugins (MCP) sind im Source-Build ebenfalls vorhanden, aber dort nicht geprüft.
- **Updates:** Engine-Updates heißen beim Source-Build: neuen Tag holen und neu bauen.
- **Lizenz:** Der Engine-Quellcode fällt unter die Unreal-EULA; er darf nicht in unser öffentliches Repo. Er liegt außerhalb von `D:\gamer\gamer`, also kein Risiko, solange der Ordner nicht ins Repo verschoben wird.

## Alternative
Den Source-Build verschieben und Phase 1 mit Listen-Server beginnen. Nachteil: Code, der versehentlich einen Listen-Server voraussetzt, fällt erst später auf (Architekturregel 10).
