# Phasenplan (Stand 2026-11-12)

Grundsatz: erst ein kleiner, spielbarer Vertical Slice, dann ausbauen. Reihenfolge ist ein Vorschlag.

| Phase | Inhalt | Fertig, wenn … |
|---|---|---|
| 0 Fundament | UE-5.8-Projekt in `game/`, Git, MCP-Anbindung, Build-Pipeline, Dedicated-Server-Target | Projekt baut, Server und Client verbinden sich |
| 1 Vertical Slice „Siedlung“ | Spielfigur (Koop), Bauteile, Ressourcen, 3–4 Dorfbewohner-Jobs, 3 Bedürfnisse, Lager | Eine Siedlung läuft zu zweit ohne ständiges Eingreifen |
| 2 Siedlung ausbauen | ca. 10 Jobs, Zufriedenheit/Perks, Schichten, Kommandoansicht (Vogelperspektive), Miliz | Dorf versorgt sich selbst über mehrere Spieltage |
| 3 Kampf und Ausrüstung | KCD2-inspiriertes Kampfsystem ohne Ausdauer-Sperre, Ausrüstungs-Slots, Schmieden, Skilltree | Kämpfe machen Spaß, Ausrüstung hat spürbaren Effekt |
| 4 Welt | Biome, Questlinien, Bosse, Schwierigkeitsstufen | Erste zwei Biome mit Boss spielbar |
| 5 Spätspiel | Portale, optionale Angriffswellen, Weltoptionen/Modi, Skalierungstests bis 16 Spieler | Weltoptionen kombinierbar, Portale transportieren zwischen Lagern |

## Hinweis zum Dedicated-Server-Target (Phase 0)
Der Launcher-Build von Unreal kann keine Server-Targets bauen, dafür braucht es einen Source-Build der Engine (Epic-Doku „Setting Up Dedicated Servers“). Das „Dedicated-Server-Target“ in Phase 0 geht deshalb erst mit Source-Build. Bis dahin kann mit dem Launcher-Build entwickelt und mit Listen-Server getestet werden. Die Entscheidung ist offen (siehe `docs/01_entscheidungen.md`).

## Parallel laufend
- **Design (ChatGPT):** Art Direction und Siedlungs-/UI-Konzepte vor Phase 1; danach phasenweise.
- **Daten:** Spielinhalte entstehen als Tabellen in `data/`, nicht im Code.
- **Messen:** ab Phase 1 regelmäßig Installationsgröße (gecookt), Client-RAM und Server-Tick; ab Phase 2 Skalierungstests mit Bots (2 → 4 → 8 → 16 Spieler).

## Ehrliche Einschätzung
Das Dorfbewohner-System ist der aufwendigste Teil (KI, Bedürfnisse, Speichern/Laden, Koop-Replikation). Zum Vergleich: Askas Entwickler
haben es über Jahre im Early Access nachgebessert. Phase 1 deshalb bewusst klein halten.
