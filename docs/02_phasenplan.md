# Phasenplan (Stand 2026-11-12)

Grundsatz: erst ein kleiner, spielbarer Vertical Slice, dann ausbauen. Reihenfolge ist ein Vorschlag.

| Phase | Inhalt | Fertig, wenn … |
|---|---|---|
| 0 Fundament | UE-5.8-Projekt in `game/`, Git, MCP-Anbindung, Build-Pipeline, Dedicated-Server-Target | Projekt baut, Server und Client verbinden sich |
| 1 Vertical Slice „Siedlung“ | Spielfigur (Koop), Bauteile, Ressourcen, 3–4 Dorfbewohner-Jobs, 3 Bedürfnisse, Lager | Eine Siedlung läuft zu zweit ohne ständiges Eingreifen |
| 2 Siedlung ausbauen | ca. 10 Jobs, Zufriedenheit/Perks, Schichten, Kommandoansicht (Vogelperspektive), Miliz | Dorf versorgt sich selbst über mehrere Spieltage |
| 3 Kampf und Ausrüstung | KCD2-inspiriertes Kampfsystem ohne Ausdauer-Sperre, Ausrüstungs-Slots, Schmieden, Skilltree | Kämpfe machen Spaß, Ausrüstung hat spürbaren Effekt |
| 4 Welt | Biome, Questlinien, Bosse, Schwierigkeitsstufen | Erste zwei Biome mit Boss spielbar |
| 5 Spätspiel | Portale, optionale Angriffswellen, Weltoptionen/Modi, Skalierungstests bis 8 Spieler | Weltoptionen kombinierbar, Portale transportieren zwischen Lagern |

## Stand Phase 0 (2026-11-12)
- Erledigt: Projekt baut (`GamerEditor Win64 Development`), Git + GitHub, MCP-Anbindung (siehe `docs/03_setup_checkliste.md`).
- **Verbindungstest Listen-Server + 1 Client bestanden** (Launcher-Build, beide Prozesse ohne Grafik auf demselben PC):
  Host `UnrealEditor.exe Gamer.uproject /Engine/Maps/Templates/Template_Default?listen -game -nullrhi -nosound`,
  Client `UnrealEditor.exe Gamer.uproject 127.0.0.1:7777 -game -nullrhi -nosound`.
  Host-Log: „Join succeeded“, Client-Log: „Welcomed by server“. Getestet nur lokal (127.0.0.1), nicht über zwei PCs oder das Internet.
- Auffällig: Im `-game`-Modus melden die Python-Skripte der Editor-Toolsets Fehler (`module 'unreal' has no attribute 'AgentSkill'` u. a.), weil es dort keine Editor-Klassen gibt. Für den Test folgenlos.
- **Build-Pipeline:** `tools/build.ps1` mit den Schritten `Editor`, `Game`, `Package`, `ProjectFiles` (Aufruf siehe Kopf des Skripts). Getestet 2026-11-12:
  Editor-Sperre greift bei offenem Editor; `Game` baut `Gamer.exe` in ca. 130 s; `Package -Config Development` in ca. 150 s, 0 Fehler, **0,96 GB** (leeres Projekt mit Engine-Karte `OpenWorld`); das gepackte Spiel startet (ohne Grafik geprüft) und lädt die Karte.
- Offen in Phase 0: Dedicated-Server-Target (braucht Source-Build, siehe unten). **Erledigt am 2026-11-13**, siehe nächster Abschnitt.

## Stand Phase 0 (2026-11-13): Dedicated Server
- Engine aus dem Quellcode gebaut (`E:\UE58`, Tag 5.8.4-release, 9306 Schritte, 9,9 h, 0 Fehler); `game/Gamer.uproject` zeigt auf diese Engine.
- Neues Target `GamerServer` (`game/Source/GamerServer.Target.cs`), gebaut in 2,6 h, 0 Fehler. `tools/build.ps1` kennt dafür `Server` und `PackageServer`.
- Setup der Engine lief ohne Mac/iOS/Android/Linux; das Paketier-Werkzeug (UAT) braucht aber die iOS-Dateien zum Übersetzen. Nachgeladen am 2026-11-13 (535 Dateien, 3,1 GB).
- **Mehrspieler-Test bestanden** mit `tools/mp_test.ps1` (Server + 2 Spieler ohne Grafik, lokal 127.0.0.1): einmal Editor als Server, einmal das paketierte `GamerServer.exe` (Paket 2,76 GB, davon 2,3 GB Debug-Symbole). Spieler-Clients waren ungecookte Editor-Instanzen und konnten sich mit dem gecookten Server verbinden.
- Spielerobergrenze `MaxPlayers=8` in `game/Config/DefaultGame.ini` (Entscheidung 2026-11-13).

## Hinweis zum Dedicated-Server-Target (Phase 0)
Der Launcher-Build von Unreal kann keine Server-Targets bauen, dafür braucht es einen Source-Build der Engine (Epic-Doku „Setting Up Dedicated Servers“). Das „Dedicated-Server-Target“ in Phase 0 geht deshalb erst mit Source-Build. Bis dahin kann mit dem Launcher-Build entwickelt und mit Listen-Server getestet werden. Die Entscheidung ist offen (siehe `docs/01_entscheidungen.md`).

## Parallel laufend
- **Design (ChatGPT):** Art Direction und Siedlungs-/UI-Konzepte vor Phase 1; danach phasenweise.
- **Daten:** Spielinhalte entstehen als Tabellen in `data/`, nicht im Code.
- **Messen:** ab Phase 1 regelmäßig Installationsgröße (gecookt), Client-RAM und Server-Tick; ab Phase 2 Skalierungstests mit Bots (2 → 4 → 8 Spieler).

## Ehrliche Einschätzung
Das Dorfbewohner-System ist der aufwendigste Teil (KI, Bedürfnisse, Speichern/Laden, Koop-Replikation). Zum Vergleich: Askas Entwickler
haben es über Jahre im Early Access nachgebessert. Phase 1 deshalb bewusst klein halten.
