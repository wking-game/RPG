# Technische Ziele (Stand 2026-11-12)

## Vorgaben des Nutzers
| Thema | Vorgabe | Von Claude gedeutet als |
|---|---|---|
| RAM | mindestens 8 GB, optimal 16 GB | Anforderung an den Spieler-PC des fertigen Spiels |
| Speicherplatz | höchstens ca. 20 GB auf SSD (pauschal, vorläufig) | Installationsgröße des fertigen Spiels |
| Spielerzahl | später bis zu 16 Spieler | technisches Maximum; Start mit 2–4 |

Falls die 20 GB den Platz für die **Entwicklung** meinen: das reicht nicht, allein Unreal Engine 5.8 braucht mehr.

## Entwicklungs-PC (am 2026-11-12 im Task-Manager abgelesen)
- **GPU:** NVIDIA GeForce RTX 3070, 8,0 GB dedizierter Grafikspeicher, Treiber 32.0.16.1088 (22.07.2026), DirectX 12 (FL 12.2)
- **RAM:** 16 GB (15,9 GB nutzbar). Im normalen Betrieb mit Chrome, ChatGPT, Claude und Steam waren ca. 70 % belegt (11,2 von 15,9 GB).
- **Datenträger:** `C:` SSD (NVMe), `E:` SSD (NVMe), `D:` SSD (SATA). Kapazität und freien Platz habe ich nicht abgelesen.
- **Nicht abgelesen:** CPU-Modell (Takt lag bei ca. 3–4 GHz).
- **Unreal Engine:** auf keinem der drei Laufwerke gefunden. **Git:** vorhanden (`D:\Program Files\Git`).

Folgen: Unreal auf eine NVMe-SSD installieren. 16 GB RAM sind für den Editor knapp (eine genaue Empfehlung habe ich nicht verifiziert). 8 GB Grafikspeicher reichen für ein kleines Projekt mit Budgets, lassen aber wenig Reserve.

## Einschätzung zu den Vorgaben (Claudes Einschätzung, keine Messwerte)

### Bis zu 16 Spieler
- 16 ist in Unreal ohnehin der Standardwert für die Spielerobergrenze (`MaxPlayers=16` in `BaseGame.ini`, laut einem älteren Epic-Forenbeitrag).
- **Ein Spieler-PC als Host für 16 Spieler plus Dorfbewohner-Simulation ist unrealistisch**, besonders mit 8 GB RAM. Deshalb: **Dedicated-Server-fähig ab Phase 0**, Listen-Server höchstens für kleine Gruppen.
- Die Server-Tickzeit wächst grob linear mit der Zahl der Verbindungen (Forenbeitrag zu UE 5.1 ohne Iris/Push-Model). Ein Spielerbericht zu Conan Exiles Enhanced (UE5) nennt Lags ab etwa 15–18 Spielern, auch bei KI und Kampf. Das ist ein Einzelbericht, ungeprüft, zeigt aber das Risiko.
- Iris (Epics neues Replikationssystem) ist optional. Laut einem Blog war es in UE 5.7 noch experimentell; den Stand in 5.8 habe ich nicht verifiziert. Entscheidung erst nach Messungen.
- Folgen fürs Design: Dorfbewohner-Simulation serverseitig und billig (Zustandsmaschinen statt aufwendiger KI), Bauteile ruhend (dormant), Relevanzradien, Spielerzahl konfigurierbar statt fest verdrahtet.

### 8 GB RAM als Minimum
- Referenz: Manor Lords (UE5) nennt 8 GB RAM als Minimum (Corsair-Blog). Machbar, aber nur mit Disziplin: niedrige Qualitätsstufe ohne Pflicht auf Lumen/Nanite, Texturpool begrenzen, früh messen.
- Aska selbst steht bei mehreren Aggregator-Seiten mit **16 GB RAM Minimum** (die Steam-Seite war nicht direkt lesbar). Unser Ziel ist also niedriger als beim Leitspiel.

### 20 GB Installationsgröße
- Askas Speicherangaben sind in den Quellen widersprüchlich (6, 40 oder 50 GB), also keine verlässliche Referenz.
- Vorschlag: Größe des gecookten Builds ab Phase 1 laufend messen, Texturgrößen und Audio früh deckeln, keine Massen an hochauflösenden Fremd-Assets. Die Vorgabe steht auch im ChatGPT-Briefing.

## Zielwerte (Vorschlag, noch nicht bestätigt)
Konkrete Zahlen für Client-RAM, Server-Tick und Bandbreite lege ich erst fest, wenn eine Leer-Szene und danach eine Siedlung gemessen sind (Phase 0 und 1).

## Offen
- GPU-Mindestanforderung für Spieler (und ob Lumen/Nanite Pflicht sind)
- Freier Platz je Laufwerk, CPU-Modell
- Wer hostet Server: Spieler selbst (Dedicated-Server-Programm) oder gemietet?

## Quellen
- Aska-Anforderungen (Aggregator): https://www.pcgamebenchmark.com/aska-system-requirements
- Manor Lords-Anforderungen: https://www.corsair.com/explorer/gamer/gaming-pcs/manor-lords-system-requirements-and-recommended-specs/
- MaxPlayers=16 (Epic-Forum, älter): https://answers.unrealengine.com/questions/824729/view.html
- Server-Tick skaliert mit Verbindungen: https://forums.unrealengine.com/t/server-networking-replication-performance-with-hundreds-of-clients/740652
- Conan Exiles Enhanced, Spielerbericht: https://forums.funcom.com/t/ue5-dedicated-server-severe-desync-lag-scaling-with-player-count-voice-chat-rubberbanding-interaction-delay/298887
- Iris-Einordnung (Blog): https://www.strayspark.studio/blog/tag/multiplayer
