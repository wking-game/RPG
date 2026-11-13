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

## Stand Phase 1 (2026-11-13): Spielfigur im Koop
- C++: `AGamerCharacter` (Third-Person, Platzhalter-Körper aus Engine-Zylinder, Farbe je Spieler), `AGamerPlayerController`, `AGamerGameMode` (Spielerfigur und Controller setzen, Spieler am Startpunkt nebeneinander aufstellen). Steuerung mit Enhanced Input, im Code aufgebaut (WASD, Maus, Leertaste, Gamepad). Bewegungswerte als Config-Werte (`[/Script/Gamer.GamerCharacter]` in `DefaultGame.ini`), Rückfallwerte im Code.
- **Test bestanden** (`tools/mp_test.ps1`, Karte `Template_Default` mit `?game=/Script/Gamer.GamerGameMode`, Server + 2 Spieler ohne Grafik): Spieler 1 lief 3 s geradeaus, Server, Spieler 1 und Spieler 2 sahen dieselbe Endposition (1317 cm), Spieler 2 blieb stehen.
- Nicht automatisch getestet: Tastatur-/Maussteuerung und Kamera (brauchen Grafik). Zum Selbst-Ausprobieren: `tools/play_local.ps1`.
- Projekt-Standards in `DefaultEngine.ini` (2026-11-13): `GlobalDefaultGameMode=/Script/Gamer.GamerGameMode`, `ServerDefaultMap=Template_Default` (vorher `Entry`, eine leere Karte, in der Figuren fallen würden). Getestet: Server ohne Zusatzangaben lädt `Template_Default` mit `GamerGameMode`, 2 Spieler, Bewegung kommt beim anderen Spieler an.

## Stand Phase 1, Schritt 2 (2026-11-13): Datenbasis
- Format bestätigt: `docs/06_datenformat.md`. Vier JSON-Tabellen in `game/Content/Data/`: `items` (8), `resource_nodes` (6), `recipes` (5), `buildables` (16), Startwerte aus Abschnitt 6 des Formatdokuments. Modell- und Icon-Pfade noch leer (Assets nicht importiert).
- C++: `GamerDataTypes.h` (Zeilentypen), `GamerDataLoader` (Einlesen und Prüfen: Pflichtfelder, unbekannte Felder, ID-Format und -Präfix, doppelte IDs, Wertebereiche, Verweise auf Gegenstände/Stationen/Werkzeugtypen, Asset-Pfade), `UGamerDataSubsystem` (lädt beim Start auf Server und Spielern, Abfrage per ID, Konsolenbefehl `gamer.ReloadData`, Prüfsumme im Protokoll).
- Kerndateien (mit Freigabe): `Gamer.Build.cs` bekommt das Modul `Json` (über die Engine nur für Header sichtbar, nicht zum Linken), `DefaultGame.ini` packt den Ordner `Data` beim Paketieren ein.
- **Tests bestanden:** `tools/test_data.ps1` → `Gamer.Data.ShippedDataIsValid` und `Gamer.Data.ValidatorFindsErrors` (absichtlich kaputte Daten, alle 10 Fehlerarten erkannt) grün, ca. 70 s. Mehrspieler: Server und 2 Spieler laden dieselben Daten, Prüfsumme `C0022D82`, 0 Fehler, 0 Warnungen – sowohl mit dem Editor als Server als auch mit dem paketierten `GamerServer.exe` (Paket mit `Template_Default`, 5,6 min; die JSON-Dateien kommen also mit ins Paket).
- Noch nicht umgesetzt: Prüfsummen-Vergleich beim Beitritt (Spieler mit anderen Daten abweisen) – laut Formatdokument vor dem ersten öffentlichen Test. Die Daten werden noch von keiner Spielmechanik benutzt; das kommt mit Sammeln, Inventar und Bauen (Schritte 3 und 4).

## Stand Phase 1, Schritt 3 (2026-11-13): Sammeln, Inventar, Anfangs-Crafting
- Spielstart (Entscheidung 2026-11-13): Grundressourcen von Hand sammeln (lose Steine, Äste, Pflanzenfasern), Werkzeuge im Anfangs-Crafting-Menü. Dazu natürliche Nahrungsquellen: Wildapfelbaum, Beerenstrauch, Pilzstelle, Haselstrauch. Vorbilder: The Forest, thematisch Medieval Dynasty und Aska.
- Datenprüfer erweitert: **Erreichbarkeit ab Spielstart** (fand den Kreislauf „Axt braucht Stamm, Stamm braucht Axt“ der alten Startwerte). Test `Gamer.Data.DetectsToolDeadlock`.
- C++: `UGamerInventoryComponent` (24 Plätze, Stapel und Gewicht aus den Daten, nur der Server ändert, nur der Besitzer sieht es), `AGamerResourceNode` (Vorkommen mit Platzhalter-Formen, abgeerntet/voll repliziert, Nachwachsen nach `regrow_days` × Spieltag 30 min, Test-Beschleunigung `gamer.RegrowScale`), `UGamerWorkComponent` (Sammeln und Herstellen mit Zeit; der Server prüft Reichweite, Werkzeug, Zutaten, Platz; nur ein Spieler pro Vorkommen), `AGamerHUD` (Platzhalter-Anzeige: Inventar, Hinweis, Fortschritt, Crafting-Menü). Steuerung: E sammeln, Tab Crafting-Menü, 1–9 herstellen.
- Startgebiet (vorläufig, bis zur Weltgenerierung): `AGamerGameMode` streut 63 Vorkommen aller Arten um den Startpunkt (fester Seed 4711), wenn die Karte keine hat.
- **Test bestanden** (Server + 2 Spieler ohne Grafik, Testskript `-GamerTestScript`): Spieler 1 sammelt Äste, Steine, Fasern, stellt die Steinaxt her (Zutaten korrekt abgezogen), fällt danach eine Kiefer (+3 Stämme), pflückt Beeren und Äpfel; Spieler 2 sieht alle 10 Vorkommen als abgeerntet. 0 Fehler.
- Nicht automatisch getestet: Tastatur (E, Tab, 1–9), Anzeige und Aussehen der Platzhalter (braucht Grafik → `Spiel_testen.bat`), Nachwachsen, Ablehnungen (z. B. „Für Kiefer brauchst du eine Axt“). Werkzeug-Haltbarkeit und Traglast-Grenze sind noch nicht eingebaut.
- Werkzeuge: `tools/mp_test.ps1` und `tools/play_local.ps1` nehmen jetzt den tatsächlich vergebenen Port (ein übrig gebliebener Testserver hatte 7777 belegt) und beenden beim Aufräumen auch Kindprozesse (das paketierte `GamerServer.exe` ist nur ein Starter). Neu: `Spiel_testen.bat` zum Doppelklicken.

## Hinweis zum Dedicated-Server-Target (Phase 0)
Der Launcher-Build von Unreal kann keine Server-Targets bauen, dafür braucht es einen Source-Build der Engine (Epic-Doku „Setting Up Dedicated Servers“). Das „Dedicated-Server-Target“ in Phase 0 geht deshalb erst mit Source-Build. Bis dahin kann mit dem Launcher-Build entwickelt und mit Listen-Server getestet werden. Die Entscheidung ist offen (siehe `docs/01_entscheidungen.md`).

## Parallel laufend
- **Design (ChatGPT):** Art Direction und Siedlungs-/UI-Konzepte vor Phase 1; danach phasenweise.
- **Daten:** Spielinhalte entstehen als JSON-Tabellen in `game/Content/Data/` (Format: `docs/06_datenformat.md`), nicht im Code.
- **Messen:** ab Phase 1 regelmäßig Installationsgröße (gecookt), Client-RAM und Server-Tick; ab Phase 2 Skalierungstests mit Bots (2 → 4 → 8 Spieler).

## Ehrliche Einschätzung
Das Dorfbewohner-System ist der aufwendigste Teil (KI, Bedürfnisse, Speichern/Laden, Koop-Replikation). Zum Vergleich: Askas Entwickler
haben es über Jahre im Early Access nachgebessert. Phase 1 deshalb bewusst klein halten.
