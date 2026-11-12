# Entscheidungslog (Stand 2026-11-12)

Status: **bestätigt** = vom Nutzer gesagt · **Vorschlag** = von Claude, noch nicht bestätigt · **offen** = fehlt noch · **festgestellt** = von Claude abgelesen/geprüft

| Thema | Inhalt | Status |
|---|---|---|
| Setting | Mittelalter / Wikinger | bestätigt |
| Engine | Unreal Engine 5 | bestätigt |
| Engine-Version | 5.8 | Vorschlag |
| Koop | Mehrspieler-Koop | bestätigt |
| Spielerzahl, Netzwerk (alter Vorschlag) | 2–4 Spieler, Listen-Server zuerst | ersetzt am 2026-11-12 (siehe die Zeilen darunter) |
| Maximale Spielerzahl | später bis zu 16 Spieler unterstützen | bestätigt (2026-11-12) |
| Start-Spielerzahl | zuerst mit 2–4 Spielern entwickeln und testen, danach 8, dann 16 | Vorschlag |
| Netzwerk | Dedicated-Server-fähig ab Phase 0; Listen-Server höchstens für kleine Gruppen | Vorschlag |
| Zielhardware Spieler-PC | RAM mindestens 8 GB, optimal 16 GB | bestätigt (2026-11-12; von Claude als Anforderung an das fertige Spiel gedeutet) |
| Installationsgröße | höchstens ca. 20 GB auf SSD (Pauschalwert, vorläufig) | bestätigt (2026-11-12; von Claude als Größe des fertigen Spiels gedeutet) |
| Entwicklungs-PC | RTX 3070 (8 GB VRAM), 16 GB RAM, SSDs: C: NVMe, E: NVMe, D: SATA | festgestellt (2026-11-12) |
| Freier Platz (2026-11-12) | C: 22,7 GB von 236 GB, D: 904 GB von 931 GB, E: 321 GB von 931 GB | festgestellt (2026-11-12) |
| Projektordner | `D:\gamer\gamer` (vom Nutzer von `Desktop\gamer` hierher verschoben), Installer in `D:\gamer\programme` | festgestellt (2026-11-12) |
| Engine-Installation | Epic Launcher auf `C:`, Unreal Engine 5.8.4 (Launcher-Build) in `D:\gamer\unreal\UE_5.8` (SATA-SSD) | festgestellt (2026-11-12) |
| Engine-Build für Dedicated Server | Server-Targets brauchen einen Source-Build der Engine (Epic-Doku); der Launcher-Build kann sie nicht bauen. Zuerst mit dem Launcher-Build entwickeln, vor den ersten Server-Tests auf Source-Build umstellen | bestätigt (2026-11-12) |
| Projektname (intern) | Modulname „Gamer“ (Arbeitsname, nicht der Spieltitel); Projekt in `game/` als Kopie der Vorlage TP_Blank angelegt, noch nicht gebaut | Vorschlag (2026-11-12) |
| Solo | Solo spielbar (Koop-Code mit 1 Spieler) | Vorschlag |
| Leitspiel | Aska | bestätigt |
| Valheim | nur Bosse und Endgame | bestätigt |
| KCD2 | Kampfsystem | bestätigt |
| Medieval Dynasty / Manor Lords | Aufbau bzw. Detailtiefe und Strukturen | bestätigt |
| Tribes of Midgard | gestrichen | bestätigt |
| Ausrüstung | KCD-Slots, levelbasiert, schmiedbar | bestätigt |
| Skilltree | Fähigkeiten freischaltbar | bestätigt |
| Ausdauer | keine Sperren bei Laufen, Ducken, Waffen | bestätigt |
| Essen | Buff-System bleibt | bestätigt |
| Portale | späte Spielphase | bestätigt |
| Weltoptionen | freies Bauen, Horden, „Survivor“, kombinierbar | bestätigt (Survivor offen) |
| Basisangriffe | optional per Menü | bestätigt |
| Tempo | frei, kein Zeitdruck | abgeleitet (Midgard gestrichen) |
| Design-Werkzeug | ChatGPT übernimmt das Design | bestätigt (2026-11-12) |
| Kamera | Third-Person Hauptansicht, Vogelperspektive nur als Bau-/Kommandoansicht | Vorschlag |
| Schwierigkeit | stufenweise nach Biom/Boss statt stufenlos exponentiell | Vorschlag |
| Dorfbewohner-Konzept | Bedürfnisse statt Uhr, Prioritäten + Fallback-Jobs, ein Lager pro Siedlung | Vorschlag |
| Portale und Dorfbewohner | Portale verbinden Siedlungslager, Dorfbewohner arbeiten lokal | Vorschlag |
| Architektur | server-autoritativ, datengetrieben, C++ für Systeme | Vorschlag |
| Arbeitstitel / Name | – | offen |
| Art-Stil (realistisch vs. stilisiert) | – | offen |
| „Survivor“-Modus | Überlebensbedürfnisse oder Horden-Überleben (Vampire-Survivors-Stil)? | offen |
| „Realtime-Daten“ | Recherche-Hilfe oder Live-Daten im Spiel? | offen |
| CPU-Modell | – | offen |
| GPU-Mindestanforderung für Spieler, Lumen/Nanite ja oder nein | – | offen |
| Server-Hosting (Spieler hosten selbst oder gemieteter Server) | – | offen |
| Veröffentlichung (Steam?) | – | offen |
| GitHub-Repository | `wking-game/RPG`, öffentlich (bewusst, statt privat wie in der Setup-Checkliste geplant) | bestätigt (2026-11-12) |
| GitHub-Konto für Engine-Quellcode (Source-Build) | `wking-game` | bestätigt (2026-11-12) |
| Art Direction | „Klarform-Saga“ (`style_klarform_saga`): stilisiert, klare Formen, kräftige Farben, nordisch-frühmittelalterlich, zurückhaltende Abnutzung. Löst die offene Zeile „Art-Stil“ oben. Details: `design/inbox/01_art_direction.md` | bestätigt (2026-11-12, vom Nutzer in ChatGPT gewählt) |
| Dachdeckung Holzstufe | Holz mit schmaler Turf-Kante (kein vollständiges Turf-Dach, wegen Lesbarkeit in der Vogelperspektive) | bestätigt (2026-11-12) |
| Lagergröße (Vertical Slice) | 300 × 600 cm, First in Längsrichtung, passt zum 300-cm-Dachbaukasten; geschlossener Raum + offene Regalbucht | bestätigt (2026-11-12) |
| Kochstelle (Vertical Slice) | offenes Lagerfeuer mit Holzgalgen und Kessel | bestätigt (2026-11-12) |
| Waffen: Stufen-Leiter | T1 Rohstufe → T2 Raseneisen → T3 Schmiedeeisen → T4 Stahl → T5 Damast → T6 Relikt; gilt auch für Werkzeuge, Rüstung und Rohstoffe | bestätigt (2026-11-12) |
| Relikt-Stufe T6 | ja (Bossmaterialien, eigene Formen) | bestätigt (2026-11-12) |
| Waffentypen MVP Phase 3 | 7 Typen: Bartaxt, Sax, Speer, Schwert (ab T3), Rundschild, Bogen, Pfeile | bestätigt (2026-11-12; Antwort „ja“ auf „passend?“, von Claude so gedeutet) |
| Fernkampf | Bogen und Armbrust (Armbrust ab T3) | bestätigt (2026-11-12; „ab T3“ ist Vorschlag) |
| Gewaltdarstellung | im Menü einstellbar: Blut und abtrennbare Körperteile, jeweils schaltbar | bestätigt (2026-11-12) |
| Farb- und Wappengestaltung | Schild u. a. individuell gestaltbar im Charakter-Editor und im „Lager-Editor“ (von Claude gedeutet als Siedlungs-Editor für Farben/Wappen der Siedlung) | bestätigt (2026-11-12; Deutung „Lager-Editor“ zu bestätigen) |
| Verderben von Nahrung | nur Rohes (Fleisch, Fisch) verdirbt, langsam | bestätigt (2026-11-12), präzisiert durch „Verderben – Dauer“ weiter unten |
| Haltbarkeit | Werkzeuge und Waffen haben Haltbarkeit, einfach gehalten (Reparatur) | bestätigt (2026-11-12) |
| Handel | Händler; nachts zusätzlich ein Schwarzmarkt-Händler. Währung Hacksilber | bestätigt (2026-11-12; Hacksilber ist Vorschlag) |
| Tierhaltung | ja, einfach gehalten; Tierarten passend zu den Tier-Stufen (von Claude so gedeutet) | bestätigt (2026-11-12) |
| Schmuck-Slot | nein; Schmuck ist nur Handelsware | bestätigt (2026-11-12) |
| Feldbau (Gerste, Flachs) | ab Phase 2 | bestätigt (2026-11-12) |
| Armbrust: Freischaltung | über bestimmte Attribute plus eine Fähigkeit im Skilltree (nicht über einen Meilenstein) | bestätigt (2026-11-12) |
| Armbrust: ab welcher Stufe | T3 (Claude) oder T4 als seltene Import-/Schwarzmarktware (ChatGPT/Codex, `design/inbox/05_assets_armbrust.md`) | offen |
| Haltbarkeit Rüstung | ja (einfach gehalten wie Waffen und Werkzeuge) | bestätigt (2026-11-12) |
| Ausrüstungs-Slots | Helm, Schulter, Hände, 2 × Ring, Beine, Füße, Umhang. Der Umhang wird nach dem ersten Winter freigeschaltet | bestätigt (2026-11-12) |
| Ausrüstungs-Slot Oberkörper (Brust/Rüstung) | in der Slot-Antwort nicht genannt – vermutlich vergessen? | offen |
| Ring-Slots vs. „kein Schmuck-Slot“ | Widerspruch: oben „Schmuck-Slot: nein“, jetzt „2 × Ring“. Sind Ringe eine Ausnahme (z. B. besondere Ringe mit Wirkung) und Fibeln/Armringe bleiben Handelsware? | offen |
| Jahreszeiten | Es gibt Winter; der erste Winter ist nicht hart (abgeleitet aus der Slot-Antwort) | bestätigt (2026-11-12) |
| Verderben – Dauer | Fleisch 4 Tage, Obst und Gemüse 2,5 Tage, wenn nicht trocken gelagert; sonstige Lebensmittel 3 Tage | bestätigt (2026-11-12) |
| Verderben – Auslegung | Spieltage? Hält trockene Lagerung unbegrenzt oder nur länger? Verderben auch gekochte Speisen („sonstige Lebensmittel 3 Tage“) – das widerspricht „nur Rohes“? | offen |
| Herkunft 3D-Assets | Mittelweg: Baukasten (Wände, Böden, Dächer, Pfosten, Türen) erzeugt Claude per Python-Skript in Blender nach den Steckbriefen; organische Einzelobjekte (Felsen, Stümpfe, Werkzeuge, Items, Kessel …) aus einem KI-3D-Werkzeug mit automatischer Nachbearbeitung in Blender; Texturen (z. B. Trim-Sheet) per KI-Bildgenerator, vom Nutzer geprüft; Figuren später. Welcher KI-3D-Dienst: offen (Lizenzvergleich vorher). Alle KI-Inhalte ins Asset-Log | bestätigt (2026-11-12) |
