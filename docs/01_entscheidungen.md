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
