# Entscheidungslog (Stand 2026-11-12)

Status: **bestätigt** = vom Nutzer gesagt · **Vorschlag** = von Claude, noch nicht bestätigt · **offen** = fehlt noch · **festgestellt** = von Claude abgelesen/geprüft

| Thema | Inhalt | Status |
|---|---|---|
| Setting | Mittelalter / Wikinger | bestätigt |
| Engine | Unreal Engine 5 | bestätigt |
| Engine-Version | 5.8 | Vorschlag |
| Koop | Mehrspieler-Koop | bestätigt |
| Spielerzahl, Netzwerk (alter Vorschlag) | 2–4 Spieler, Listen-Server zuerst | ersetzt am 2026-11-12 (siehe die Zeilen darunter) |
| Maximale Spielerzahl (alt) | später bis zu 16 Spieler unterstützen | ersetzt am 2026-11-13 (siehe nächste Zeile) |
| Maximale Spielerzahl | höchstens 8 Spieler pro Welt; umgesetzt als `MaxPlayers=8` in `game/Config/DefaultGame.ini` | bestätigt (2026-11-13) |
| Start-Spielerzahl | zuerst mit 2–4 Spielern entwickeln und testen, danach 8 (vorher: „danach 8, dann 16“, angepasst an die neue Höchstzahl) | Vorschlag |
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
| Schmuck-Slot | nein; Schmuck ist nur Handelsware | bestätigt (2026-11-12); präzisiert am 2026-11-13: Ringe sind die Ausnahme (siehe „Ring-Slots“ unten) |
| Feldbau (Gerste, Flachs) | ab Phase 2 | bestätigt (2026-11-12) |
| Armbrust: Freischaltung | über bestimmte Attribute plus eine Fähigkeit im Skilltree (nicht über einen Meilenstein) | bestätigt (2026-11-12) |
| Armbrust: ab welcher Stufe | T3 (Claude) oder T4 als seltene Import-/Schwarzmarktware (ChatGPT/Codex, `design/inbox/05_assets_armbrust.md`) | offen |
| Haltbarkeit Rüstung | ja (einfach gehalten wie Waffen und Werkzeuge) | bestätigt (2026-11-12) |
| Ausrüstungs-Slots | Helm, Schulter, **Oberkörper**, Hände, 2 × Ring, Beine, Füße, Umhang. Der Umhang wird nach dem ersten Winter freigeschaltet | bestätigt (2026-11-12; Oberkörper ergänzt am 2026-11-13) |
| Ausrüstungs-Slot Oberkörper (Brust/Rüstung) | ja, ein Slot (Tunika, Gambeson, Lederpanzer, Kettenhemd); Unterkleidung gehört zur Grundfigur | bestätigt (2026-11-13, Nutzer: „Oberkörper einbinden“; „ein Slot“ ist der bisherige Vorschlag) |
| Ring-Slots vs. „kein Schmuck-Slot“ | Ringe sind die Ausnahme: 2 Ring-Slots für Ringe mit Wirkung bleiben. Fibeln, Armringe, Ketten bleiben Handelsware ohne Slot | bestätigt (2026-11-13, Nutzer: „Schmuck bleibt“, von Claude so gedeutet) |
| Jahreszeiten | Es gibt Winter; der erste Winter ist nicht hart (abgeleitet aus der Slot-Antwort) | bestätigt (2026-11-12) |
| Verderben – Dauer | Fleisch 4 Tage, Obst und Gemüse 2,5 Tage, wenn nicht trocken gelagert; sonstige Lebensmittel 3 Tage | bestätigt (2026-11-12) |
| Verderben – Auslegung | Spieltage? Hält trockene Lagerung unbegrenzt oder nur länger? Verderben auch gekochte Speisen („sonstige Lebensmittel 3 Tage“) – das widerspricht „nur Rohes“? | offen |
| Datenformat Spielinhalte | JSON-Dateien, eine pro Tabelle (`items`, `resource_nodes`, `recipes`, `buildables`), Aufbau `{ "schema", "rows" }`, Details in `docs/06_datenformat.md` | bestätigt (2026-11-13) |
| Ort der Datendateien | `game/Content/Data/` (statt `data/`, damit sie beim Paketieren mitkommen); `data/README.md` bleibt als Wegweiser | bestätigt (2026-11-13) |
| Werkzeug-IDs | Stufe in der ID (`tool_axe_t1`, `tool_axe_t2` …); `tool_axe` bleibt der Name des gemeinsamen Modells | bestätigt (2026-11-13) |
| Startwerte Phase 1 | Werte aus `docs/06_datenformat.md` Abschnitt 6 zum Ausprobieren, inklusive Nachwachsen von Bäumen, Felsen, Sträuchern | bestätigt (2026-11-13, als Startwerte zum Anpassen) |
| Spielstart und erstes Werkzeug | Zu Beginn werden alle Grundressourcen von Hand gesammelt (lose Steine, herabgefallene Äste, Pflanzenfasern); daraus stellt man im Anfangs-Crafting-Menü (Handwerk ohne Station) die Grundwerkzeuge her: Bauhammer, Axt, Spitzhacke. Keine Startausrüstung | bestätigt (2026-11-13, Nutzer); Mengen und Rezepte in `docs/06_datenformat.md` sind Vorschlag |
| Natürliche Nahrungsquellen | Apfelbäume, Beerensträucher, Pilze und ähnliche Quellen, realistisch dargestellt. Von Claude gedeutet als: botanisch richtig und gut erkennbar (echte Arten, Wuchsform, sichtbare Früchte, abgeerntet/voll), im Rahmen der stilisierten Art Direction „Klarform-Saga“, nicht fotorealistisch. Umsetzung Phase 1: Wildapfelbaum, Beerenstrauch, Pilzstelle, Haselstrauch (Haselnuss = Vorschlag für „ähnliche“) | bestätigt (2026-11-13, Nutzer); Arten und Werte Vorschlag |
| Vorbilder für Sammeln und Pflanzen | The Forest für Sammeln, Anfangs-Crafting und erkennbare Pflanzen in der Welt; thematisch Medieval Dynasty und Aska. Nur als Vorbild für Spielgefühl und Mechanik – keine Modelle, Texturen, Namen oder Optik übernehmen | bestätigt (2026-11-13, Nutzer) |
| Art Direction: wie realistisch? | The Forest und Medieval Dynasty sind deutlich realistischer als „Klarform-Saga“ (stilisiert, bestätigt 2026-11-12). Bleibt es bei stilisiert, oder soll die Darstellung realistischer werden? Bis zur Antwort gilt „Klarform-Saga“ | offen |
| Herkunft 3D-Assets | Mittelweg: Baukasten (Wände, Böden, Dächer, Pfosten, Türen) erzeugt Claude per Python-Skript in Blender nach den Steckbriefen; organische Einzelobjekte (Felsen, Stümpfe, Werkzeuge, Items, Kessel …) aus einem KI-3D-Werkzeug mit automatischer Nachbearbeitung in Blender; Texturen (z. B. Trim-Sheet) per KI-Bildgenerator, vom Nutzer geprüft; Figuren später. Welcher KI-3D-Dienst: offen (Lizenzvergleich vorher). Alle KI-Inhalte ins Asset-Log | bestätigt (2026-11-12) |
| Figuren: Körpertypen | zwei Körperbauten (schmal, kräftig), frei mit jedem Kopf, Bart und jeder Frisur kombinierbar, nicht an „männlich/weiblich“ gebunden; ein gemeinsames Skelett | bestätigt (2026-11-13, Empfehlung von Claude übernommen) |
| Figuren: Alter der Bewohner | nur Erwachsene; Ältere später als günstige Variante (graue Haare, gebeugte Haltung); keine Kinder im ersten Wurf | bestätigt (2026-11-13, Empfehlung übernommen) |
| Bewohner: Tod | Bewohner können sterben, endgültig, aber nur durch Gewalt; Hunger und Unzufriedenheit führen zum Weggehen, nicht zum Tod. Weltoption „Bewohner unsterblich“. Gefallene bekommen Saga-Eintrag oder Gedenkstein | bestätigt (2026-11-13, Empfehlung übernommen) |
| Bewohner: Persönlichkeit | Namen im altnordischen Stil und 1–2 Eigenschaften aus einer kleinen Liste (z. B. fleißig, Langschläfer, kräftig, geschickt, gesellig, Einzelgänger), Wirkung ±10–20 %; ab Phase 2. Vorlieben später | bestätigt (2026-11-13, Empfehlung übernommen) |
| Sprache und Dialoge | keine Sprachausgabe, keine Lippen-Synchronität; Gespräche als Text mit Gesten, dazu kurze Laute/Rufe in einer erfundenen, altnordisch klingenden Sprache | bestätigt (2026-11-13, Empfehlung übernommen) |
| Gegnerarten und Bosse | zuerst nur Menschen und Tiere; Bosse in der realistischen Fassung. Sagenwesen höchstens später als Weltoption „Sagenwelt“ (beantwortet die offene Frage aus dem Design-Register) | bestätigt (2026-11-13, Empfehlung übernommen) |
| Reit- und Zugtiere | kein Reiten im ersten Wurf; Zugtiere für Karren eventuell später, gesteuert von Dorfbewohnern, nicht vom Spieler | bestätigt (2026-11-13, Empfehlung übernommen) |
| Tod im Koop | erst niedergeschlagen, Mitspieler können ca. 45 s lang wiederbeleben; danach (oder solo) Aufwachen am eigenen Bett bzw. im Lager. Ausrüstung bleibt am Körper, Rucksackinhalt liegt als Bündel am Todesort, ohne Ablaufzeit. Strengere Regeln nur als Weltoption | bestätigt (2026-11-13, Empfehlung übernommen) |
| Proportionen der Figuren | fast realistisch, leicht überzeichnet (Hände, Füße, Schultern ca. 10 % kräftiger), klare Silhouetten; passend zu „Klarform-Saga“ und zum UE5-Standardskelett | bestätigt (2026-11-13, Empfehlung übernommen) |
| Spieler im Koop unterscheiden | Namensschild über dem Kopf (blendet mit Entfernung aus, abschaltbar) plus feste Spielerfarbe in Koop-Liste, Karte und als kleiner Ring/Rand, unabhängig von der Kleidung | bestätigt (2026-11-13, Empfehlung übernommen) |
| Kampf: Angriffe | Richtungsangriffe: drei Hiebrichtungen (links, rechts, oben) plus Stoß; zusätzlich ein einfacher Modus ohne Richtungen für Gamepad und Einsteiger | bestätigt (2026-11-13, Empfehlung übernommen) |
| Kampf: Zielerfassung | ja, zuschaltbar, mit Zielwechsel in Gruppen | bestätigt (2026-11-13, Empfehlung übernommen) |
| Kampf: Ausweichen | kurzer Ausweichschritt, keine Rolle; gegen Dauer-Ausweichen eine kurze Erholungszeit, keine Ausdauer | bestätigt (2026-11-13, Empfehlung übernommen) |
| Kampf: Friendly Fire | Weltoption, standardmäßig aus | bestätigt (2026-11-13, Empfehlung übernommen) |
| Kampf: Trefferzonen | drei Zonen: Kopf mehr Schaden, Körper normal, Arme/Beine weniger; geschützt durch die passenden Rüstungs-Slots (Körper durch den Oberkörper-Slot, seit 2026-11-13) | bestätigt (2026-11-13, Empfehlung übernommen) |
| Abnutzung von Waffen | voller Schaden bis Haltbarkeit 0, dann „beschädigt“ mit halbem Schaden; die Waffe verschwindet nicht, Reparatur stellt sie wieder her | bestätigt (2026-11-13, Empfehlung übernommen) |
