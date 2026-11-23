# Gemini-GDD, dritte Fassung: Einordnung (Entwurf 2026-11-22)

Quelle: `design/referenz/gemini_gdd_system_architektur_v3.md` (1691 Zeilen; Original `D:\gamer\heruntergeladenes\game_design_system_architektur (2).md`). Gelesen sind der Designteil (Zeilen 1–394) und die Überschriften des C++-Teils; der C++-Teil (`USkillGraphSubsystem`, ab Zeile 395) wurde **nicht** im Einzelnen geprüft. Stand: Klio hat am 2026-11-22 geantwortet (Abschnitt 6); die Entscheidungen stehen im Entscheidungslog.

Die Küstenasche-Gesamtdokumentation ist unverändert (identisch mit `design/referenz/gemini_kuestenasche_gesamtdokumentation.md`). Die dritte Fassung ergänzt sie nicht um Kreaturen, Biome, Bosse oder Wikinger-Stoff; sie ist ein allgemeines Survival-/Siedlungs-GDD.

## 1. Ähnliche oder schon entschiedene Punkte

| Gemini v3 | Bei uns | Bewertung |
|---|---|---|
| 3 Baustufen, Anlieferung durch Spieler und Bewohner | entschieden (2026-11-22, `docs/23`) | gleich, außer: Gemini will je Stufe ein Modell, wir ein Modell mit Fortschritts-Material. Wir bleiben bei uns |
| Rechte pro Objekt (`OwnerOnly`, `AllowFinishBuild`, `AllowBuildAndBreak`) | entschieden (`docs/23`) | gleich |
| Reservierungen (Lager, Baustelle, Mahlzeit) | entschieden (`docs/23`) | gleich |
| Shelter-Stufen `None`/`Roofed`/`EnclosedOrCave` | entschieden (Wetter und Unterschlupf) | gleich; bei uns wirken Wetter-Folgen nur mit der Weltoption |
| Terraforming nur Ebnen und Sockel | entschieden: nur „Bauplatz ebnen“ | gleich |
| Höhlen als Zonen | entschieden: handgebaute Zonen | gleich |
| Persistente Spieler-ID, Sanitize beim Laden, Reservierungen nach dem Laden neu | vorhanden (`GamerPlayerIdentity`, Speichersystem) | gleich |
| Maximal Level 50 | entschieden | gleich |

## 2. Gute Ideen, die ich vorschlage (je Punkt einzeln entscheiden)

| Nr | Idee | Warum | Aufwand |
|---|---|---|---|
| 1 | **Rückerstattung beim Abriss 75–85 %**, nach Zustand und Stufe | Passt zu den Abriss-Optionen (voll/halb/aus); „voll“ ist bei uns bisher 100 % | klein (Zahl und Staffelung in den Daten) |
| 2 | **Nachwachsen der Vorkommen nach Weltzeit, erst wenn niemand daneben steht**, in Höhlen länger | Wir haben `regrow_days`; die Wartebedingung „nicht direkt daneben“ verhindert, dass man vor den Augen des Sammlers nachspawnt | klein; prüfen, wie `regrow_days` heute zählt |
| 3 | **Türen mit Zustand (offen/zu/abgeschlossen) und Bewohner, die Türen selbst öffnen** | Der Baukasten hat Türen; das NavMesh darf nicht blockieren | mittel (Teil des Bau-Systems) |
| 4 | **Stationsplätze mit festem Standpunkt und Belegung** (Werkbank, Schmelze, Bett) | verhindert Doppelbelegung und Clipping bei Bewohnern | mittel (nach den Berufen sinnvoll) |
| 5 | **Siedlungsmenü in fünf Reitern** (Bewohner, Betten, Lager, Bauaufgaben, Koop-Rechte) | Unsere Dorfverwaltung hat Bewohner und Berufe; Betten und Lager fehlen als Übersicht | mittel (UI) |
| 6 | **Lehrmeister mit Respec** (Voll- und Teil-Respec in Städten) | Löst „Fähigkeitspunkte falsch ausgegeben“ ohne Neustart; erst sinnvoll, wenn es den Fähigkeitenbaum gibt | klein, später |
| 7 | **Händler-Ruf 0–1000 mit drei Stufen** und Preisfaktoren | Wir haben Händler und Hacksilber, aber keinen Fortschritt der Beziehung zum Ort. Braucht feste Orte | mittel, später |
| 8 | **Kompass/Karte: Markierungen bei Nebel, Nacht, Höhle anpassen** | gute Leseregeln für die Karte, wenn es sie gibt | klein, später |

## 3. Punkte, die unseren Entscheidungen oder Regeln widersprechen

| Gemini v3 | Konflikt | Mein Vorschlag |
|---|---|---|
| **Fähigkeitenbaum mit 21 Knoten, 49 Punkte** | Wir haben 59 Punkte (`docs/21`). Der Baum enthält **keinen einzigen Kampfknoten**; es gibt nur Bauen, Höhle, Lager, Handel, Siedler | nicht übernehmen; als Muster für Bau-/Wirtschafts-Knoten und die UND/ODER-Abhängigkeiten nutzen und Kampfknoten ergänzen |
| **Aktive Fähigkeiten kosten Ausdauer; Nässe und Kälte bremsen die Ausdauer-Erholung** | verletzt Regel 5 (keine Ausdauer-Sperren) | Kosten stattdessen als Abklingzeit |
| **XP-Kurve 120 + 25·(L−1) + 1,25·(L−1)²**, nur Bau, Höhle, Handel, Siedler als Quellen | Unsere Kurve aus Spielstunden (122 370 gesamt, `docs/21`), Erfahrung aus allem inkl. Kampf. Gemini: etwa 83 000 gesamt und kein Kampf | nicht übernehmen; seine Meilenstein-Werte als Vergleich für unser Bauen/Handeln nutzen |
| **8 feste NPC-Orte (5 Siedlungen, 2 Städte, Hauptstadt)** | Unsere Welt hat Orte aus der Story (`docs/17`) und sechs Biome. Die Zahl 8 ist kein Entscheid von uns | offen: erst die Weltgestaltung klären, dann Handel und Ruf an die echten Orte hängen |
| **Wetter wirkt immer** (Nässe, Kälte, Ausdauer) | Wetter-Gameplay ist bei uns eine Weltoption, Wetter selbst immer | wie entschieden lassen |
| **Skillgraph-Subsystem in C++ (Blueprint-Strukturen, eigenes SaveGame)** | Wir laden Daten aus JSON und nutzen unser Speichersystem | nicht übernehmen; falls der Baum kommt, als JSON-Tabelle neu bauen |

## 4. Was fehlt und was wir trotzdem brauchen

Kampf, Kreaturen, Beute, Biome, Bosse, Rüstung, Wikinger-Stoff, Sagen: Das GDD ist dafür keine Quelle. Diese Teile stehen in `docs/21` bis `docs/26`.

## 5. Offene Fragen an Klio

1. Welche der Ideen 1–8 sollen weiter verfolgt werden (jeweils ja/nein/später)?
2. Soll der Fähigkeitenbaum aus Geminis Aufbau (Cluster, Anker, Resonanzen) abgeleitet werden, ergänzt um Kampf, oder neu entstehen?
3. Gibt es feste Handelsorte mit Ruf? Wenn ja, wie viele, und wie hängen sie an der Story?

## 6. Antworten von Klio (2026-11-22)

1. **Ideen:** alle acht weiter verfolgen: 1 und 2 klein (jederzeit einschiebbar), 3 bis 5 mit dem Ausbau von Bauen und Bewohnern, 6 bis 8 später.
2. **Fähigkeitenbaum:** aus Geminis Aufbau ableiten, Kampfknoten ergänzen (Entwurf nach dem ersten Kampftest).
3. **Handelsorte:** ja, wie bei Gemini (5 + 2 + 1), angepasst an Story und Biome; Anzahl und Lage klärt die Weltgestaltung.
4. **Kampf 2b:** erst Spieltest von Schritt 2a durch Klio.

5. **Umsetzung (2026-11-22):** Ideen 1 und 2 sind gebaut, siehe `docs/02_phasenplan.md` („Stand: zwei kleine Ideen“).
