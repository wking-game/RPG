# Küstenasche – Vollständige Projekt-Gesamtdokumentation

Zusammengeführt am 22.11.2026 aus allen Projekt-Dokumenten (`docs/`, `PROJEKT_STATUS.md`, `design/`).

**Hinweis zur Zusammenführung:** Die Inhalte sind vollständig und unverändert übernommen, nicht gekürzt oder zusammengefasst. Geändert wurden nur die Überschriften-Ebenen (jede Quelldatei ist ein nummeriertes Kapitel, ihre Abschnitte sind Unterkapitel) und je Kapitel ein Quellenvermerk. Die Einzeldateien bleiben die maßgebliche Arbeitsgrundlage; bei Änderungen dort muss diese Datei neu erzeugt werden.

Nicht enthalten: die externe Gemini-Referenz `design/referenz/gemini_kuestenasche_gesamtdokumentation.md` (deren bereinigte Übernahme steht in Kapitel „Kreaturen, Level und Kampfwerte“), Eingangsmaterial in `design/inbox/`, Fremdreferenzen in `reference/` sowie Regeldateien (`CLAUDE.md`, `AGENTS.md`).

<a id="inhaltsverzeichnis"></a>

## Inhaltsverzeichnis

- **[Teil I – Projektüberblick, Status und Entscheidungen](#teil-1)**
  - [1. Vision (Stand 2026-11-12)](#d1)
    - [1.1 Kurzbeschreibung](#d1-1)
    - [1.2 Leitspiel und Entlehnungen](#d1-2)
    - [1.3 Kernmerkmale](#d1-3)
    - [1.4 Das Dorfbewohner-System (Kernrisiko)](#d1-4)
    - [1.5 Bewusst nicht im Umfang (vorerst)](#d1-5)
  - [2. Projekt-Status „Gamer“ – Übersicht für den PC-Umzug](#d2)
    - [2.1 Das Wichtigste in drei Sätzen](#d2-1)
    - [2.2 1. Was schon funktioniert](#d2-2)
    - [2.3 2. Welche Dateien wichtig sind](#d2-3)
    - [2.4 3. Was als Nächstes sinnvoll wäre](#d2-4)
    - [2.5 4. Deine Wunschliste vom 22.11.2026](#d2-5)
  - [3. Entscheidungslog (Stand 2026-11-12)](#d3)
- **[Teil II – Planung, Technik und Pipeline](#teil-2)**
  - [4. Phasenplan (Stand 2026-11-12)](#d4)
    - [4.1 Stand Phase 0 (2026-11-12)](#d4-1)
    - [4.2 Stand Phase 0 (2026-11-13): Dedicated Server](#d4-2)
    - [4.3 Stand Phase 1 (2026-11-13): Spielfigur im Koop](#d4-3)
    - [4.4 Stand Phase 1, Schritt 2 (2026-11-13): Datenbasis](#d4-4)
    - [4.5 Stand Phase 1, Schritt 3 (2026-11-13): Sammeln, Inventar, Anfangs-Crafting](#d4-5)
    - [4.6 Erster Spieltest mit Grafik und Änderungen danach (2026-11-13)](#d4-6)
    - [4.7 Stand Phase 1, Schritt 4 (2026-11-13): Bauen](#d4-7)
    - [4.8 Startmenü (2026-11-13)](#d4-8)
    - [4.9 Stand Phase 1, Schritt 5 (2026-11-13): Dorfbewohner](#d4-9)
    - [4.10 Wegfindung der Bewohner und Maus-Fehler nach dem Startmenü (2026-11-13)](#d4-10)
    - [4.11 Vogelperspektive, erste Fassung (2026-11-13)](#d4-11)
    - [4.12 Grafik-Einstellungen (2026-11-13)](#d4-12)
    - [4.13 Vegetation fotorealistisch, erste Fassung (2026-11-13)](#d4-13)
    - [4.14 Weltkarte, PCG-Vegetation, Jahreszeiten, Totholz (2026-11-13/14)](#d4-14)
    - [4.15 Spieler-Wünsche vom 2026-11-14: Sprint, Werkzeugwahl, Mausrad, Bauen (umgesetzt) und Plan für den Rest](#d4-15)
    - [4.16 Charakterauswahl, Charaktererstellung, Charaktereditor (2026-11-14)](#d4-16)
    - [4.17 Taverne, Kochstellen, Gerichte mit Buffs (2026-11-15)](#d4-17)
    - [4.18 Spieler-Wünsche, zweite Runde (2026-11-15)](#d4-18)
    - [4.19 Icons aus den 3D-Modellen und Kacheln im Herstellungsmenü (2026-11-15)](#d4-19)
    - [4.20 Speichersystem (2026-11-15)](#d4-20)
    - [4.21 Phase 2, Schritt 1: Spielfigur mit Animationen (2026-11-15)](#d4-21)
    - [4.22 Umriss statt Rechteck, Scan-Welle, Animations-Hinweise (2026-11-15)](#d4-22)
    - [4.23 „Welt erstellen“: Spielmodus, Schwierigkeit, Erweiterte Einstellungen (2026-11-16)](#d4-23)
    - [4.24 Leistungsmessung Phase 1 (2026-11-21)](#d4-24)
    - [4.25 Hinweis zum Dedicated-Server-Target (Phase 0)](#d4-25)
    - [4.26 Parallel laufend](#d4-26)
    - [4.27 Ehrliche Einschätzung](#d4-27)
  - [5. Setup-Checkliste für deinen PC](#d5)
    - [5.1 Was ich gesehen habe (nur Ordnernamen, Stand 2026-11-12)](#d5-1)
    - [5.2 Schritte (in dieser Reihenfolge)](#d5-2)
    - [5.3 Hinweise](#d5-3)
    - [5.4 Immer nur ein Unreal gleichzeitig (Sperre, seit 2026-11-16)](#d5-4)
  - [6. Technische Ziele (Stand 2026-11-12)](#d6)
    - [6.1 Vorgaben des Nutzers](#d6-1)
    - [6.2 Entwicklungs-PC (am 2026-11-12 im Task-Manager abgelesen)](#d6-2)
    - [6.3 Einschätzung zu den Vorgaben (Claudes Einschätzung, keine Messwerte)](#d6-3)
    - [6.4 Zielwerte (Vorschlag, noch nicht bestätigt)](#d6-4)
    - [6.5 Offen](#d6-5)
    - [6.6 Quellen](#d6-6)
  - [7. Plan: Source-Build der Engine für den Dedicated Server (Stand 2026-11-12)](#d7)
    - [7.1 Ausgangslage (am 2026-11-12 gemessen)](#d7-1)
    - [7.2 Schätzungen (nicht gemessen, für UE 5.8 nicht verifiziert)](#d7-2)
    - [7.3 Schritte](#d7-3)
    - [7.4 Risiken und Nachteile](#d7-4)
    - [7.5 Alternative](#d7-5)
  - [8. Datenformat für Spielinhalte (Stand 2026-11-13)](#d8)
    - [8.1 1. Worum es geht](#d8-1)
    - [8.2 2. Empfehlung in einem Satz](#d8-2)
    - [8.3 3. Regeln für alle Tabellen](#d8-3)
    - [8.4 4. Die Tabellen für Phase 1](#d8-4)
    - [8.5 5. Wie Unreal die Daten benutzt](#d8-5)
    - [8.6 6. Phase-1-Inhalt (Startwerte, Vorschlag)](#d8-6)
    - [8.7 7. Offene Fragen an den Nutzer](#d8-7)
    - [8.8 8. Was danach passiert (nach Bestätigung)](#d8-8)
    - [8.9 Ergänzung 2026-11-14: Verderb, Obst-Zyklus, Feldbau, Ausbaustufen](#d8-9)
    - [8.10 Ergänzung 2026-11-16: `difficulty.json` – Schwierigkeitsstufen](#d8-10)
  - [9. KI-3D-Dienste: Lizenz- und Eignungsvergleich (Stand 2026-11-13)](#d9)
    - [9.1 Bedingungen für dieses Projekt](#d9-1)
    - [9.2 Vergleich](#d9-2)
    - [9.3 Empfehlung](#d9-3)
    - [9.4 Stand: kein Bezahldienst (Nutzer, 2026-11-13)](#d9-4)
    - [9.5 Gewählt (zurückgenommen): Meshy Pro (Nutzer, 2026-11-13: „Gleich Meshy Pro“)](#d9-5)
    - [9.6 Pipeline (`tools/ki3d.ps1`)](#d9-6)
    - [9.7 Was für den Slice gebraucht wird (`design/ASSET_LIST_GESAMT.md`, Abschnitt 8)](#d9-7)
    - [9.8 Quellen](#d9-8)
- **[Teil III – Welt, Umgebung und Spielsysteme](#teil-3)**
  - [10. Umgebung erstellen: Methoden aus den Tutorials und unsere Umsetzung](#d10)
    - [10.1 Quellen (Videos des Nutzers)](#d10-1)
    - [10.2 Methoden](#d10-2)
    - [10.3 Referenzbild als „Rezept“ (V5-Methode)](#d10-3)
    - [10.4 Was wir nicht übernehmen](#d10-4)
  - [11. Vegetations-Material und Jahreszeiten](#d11)
    - [11.1 Ursache der „verbrannten“ grauen Grasbüschel (2026-11-13)](#d11-1)
    - [11.2 Shading](#d11-2)
    - [11.3 Jahreszeit: `MPC_GamerWorld.SeasonPhase`](#d11-3)
    - [11.4 Parameter je Material-Instanz](#d11-4)
    - [11.5 Farbgebung](#d11-5)
    - [11.6 Variation gegen Eintönigkeit](#d11-6)
    - [11.7 Bekannte Grenzen](#d11-7)
  - [12. Nahrung, Obst, Saatgut und Feldbau – Anforderungen](#d12)
    - [12.1 Wünsche des Nutzers (wörtlich zusammengefasst)](#d12-1)
    - [12.2 Kreislauf](#d12-2)
    - [12.3 Vorschlag zur Umsetzung (Claude, noch nicht bestätigt)](#d12-3)
    - [12.4 Offene Fragen an den Nutzer](#d12-4)
    - [12.5 Umsetzung (Stand 2026-11-14 abends, Session „Obst, Saatgut und Feldbau“)](#d12-5)
  - [13. Taverne, Kochstellen und Gerichte (T1–T6)](#d13)
    - [13.1 Regeln für Essens-Buffs (Architekturregel 5)](#d13-1)
    - [13.2 Gerichte](#d13-2)
    - [13.3 Neue Zutaten](#d13-3)
    - [13.4 Taverne (Gebäude)](#d13-4)
    - [13.5 Kochstellen](#d13-5)
    - [13.6 Technik](#d13-6)
    - [13.7 Tests (2026-11-15)](#d13-7)
    - [13.8 Grenzen, ehrlich](#d13-8)
  - [14. Magie-Gespür: Umschauen und Lauschen](#d14)
    - [14.1 Zweck](#d14-1)
    - [14.2 Umschauen (Z)](#d14-2)
    - [14.3 Lauschen (Q)](#d14-3)
    - [14.4 Koop](#d14-4)
    - [14.5 Zieltempo Solo (Annahme, nicht gemessen)](#d14-5)
    - [14.6 Weltoption „Gespür-Tempo“](#d14-6)
    - [14.7 Magische Objekte (Konvention für die Magie-Phase)](#d14-7)
    - [14.8 Technik und Tests](#d14-8)
    - [14.9 Offen](#d14-9)
  - [15. Charaktererstellung, Charakterauswahl, Charaktereditor](#d15)
    - [15.1 Ablauf (Vorschlag)](#d15-1)
    - [15.2 Werte (fünf, keine Ausdauer-Sperre)](#d15-2)
    - [15.3 Fünf Herkünfte](#d15-3)
    - [15.4 Entscheidungen (2026-11-14)](#d15-4)
    - [15.5 Offene Fragen (Stand vor der Entscheidung)](#d15-5)
  - [16. MetaHuman-Figuren: Was nach der Anmeldung zu tun ist](#d16)
    - [16.1 Was schon vorbereitet ist](#d16-1)
    - [16.2 Schritt für Schritt](#d16-2)
    - [16.3 Stand 16.11.: Zusammenbau per Skript erledigt](#d16-3)
    - [16.4 So prüfst du, ob es geklappt hat](#d16-4)
    - [16.5 Detailstufen (Ziel 8 GB Grafikspeicher)](#d16-5)
    - [16.6 Was nur mit Anmeldung geht / was offen ist](#d16-6)
    - [16.7 Kleidung (eigene Umsetzung, Stand 21.11.)](#d16-7)
    - [16.8 Spielerfiguren in Mittel-Qualität (Haarbewegung), Stand 2026-11-22](#d16-8)
- **[Teil IV – Story, Saga und Quests](#teil-4)**
  - [17. Saga-System und Questlinien (Entwurf)](#d17)
    - [17.1 1. Begriffe](#d17-1)
    - [17.2 2. Die vier Ebenen der Saga](#d17-2)
    - [17.3 3. Hauptsaga: „Der gebrochene Ring“ (Story-Vorschlag)](#d17-3)
    - [17.4 4. Einbindung des Magie-Gespürs (Lauschen)](#d17-4)
    - [17.5 5. Koop und Solo (überholt)](#d17-5)
    - [17.6 6. Das Saga-Buch (`ui_saga`)](#d17-6)
    - [17.7 7. Datenformat (Entwurf, angepasst an die Antworten vom 2026-11-15)](#d17-7)
    - [17.8 8. Umfang und Reihenfolge (Vorschlag)](#d17-8)
    - [17.9 9. Offene Fragen (überholt)](#d17-9)
    - [17.10 10. Fünf Storyline- und Saga-Vorschläge](#d17-10)
    - [17.11 11. Questfortschritt pro Spieler, alles spawnt nach (Entwurf)](#d17-11)
    - [17.12 12. Antworten des Nutzers (2026-11-15)](#d17-12)
  - [18. Probe: Prolog „Die Spur der Ersten“](#d18)
    - [18.1 1. Der Strand](#d18-1)
    - [18.2 2. Die Raben](#d18-2)
    - [18.3 3. Der verlassene Hof](#d18-3)
    - [18.4 4. Das Summen](#d18-4)
    - [18.5 5. Zwei weitere Seiten](#d18-5)
    - [18.6 6. Die Gräber](#d18-6)
    - [18.7 7. Ende des Prologs](#d18-7)
    - [18.8 Was die Probe zeigen soll](#d18-8)
    - [18.9 Fragen an den Nutzer zur Probe](#d18-9)
  - [19. Hauptstory „Die Spur der Ersten“: Kapitelplan (Entwurf)](#d19)
    - [19.1 Leitplanken](#d19-1)
    - [19.2 Was bisher geschah (Prolog, Probe)](#d19-2)
    - [19.3 Hintergrund (nur für das Team, wird im Spiel Stück für Stück gefunden)](#d19-3)
    - [19.4 Kapitelübersicht](#d19-4)
    - [19.5 Kapitel 1K: Der Kreis im Norden (Weg des Kreises)](#d19-5)
    - [19.6 Kapitel 1H: Rauch am Fjord (Weg des Hofs)](#d19-6)
    - [19.7 Kapitel 2K: Was die Ebbe freigibt (Weg des Kreises)](#d19-7)
    - [19.8 Kapitel 2H: Das Boot der Ersten (Weg des Hofs)](#d19-8)
    - [19.9 Kapitel 3: Das zweite Logbuch (die Wege treffen sich)](#d19-9)
    - [19.10 Kapitel 4 (später): Der Eid des Hügelkönigs](#d19-10)
    - [19.11 Kapitel 5 (später): Der Mann im Eis](#d19-11)
    - [19.12 Kapitel 6 (später): Die Aschezunge](#d19-12)
    - [19.13 Kapitel 7 (später): Der Sturm (Finale)](#d19-13)
    - [19.14 Umfang und Aufwand (ehrlich)](#d19-14)
    - [19.15 Antworten des Nutzers](#d19-15)
    - [19.16 Noch offen](#d19-16)
    - [19.17 Kapitel 4–7 im Detail (Entwurf, 2026-11-21)](#d19-17)
  - [20. Kapitel 1: Spieltext (Entwurf)](#d20)
    - [20.1 1K.1 Der Ring wird warm](#d20-1)
    - [20.2 1K.2 Das Lager am Hang](#d20-2)
    - [20.3 1K.3 Die Suhle an der Quelle](#d20-3)
    - [20.4 1K.4 Die Steine aufrichten](#d20-4)
    - [20.5 1K.5 Das Lied der Fjordwiesen (Nebelbild)](#d20-5)
    - [20.6 1K.6 Abschied am Lager](#d20-6)
    - [20.7 1H.1 Die Rauchsäule](#d20-7)
    - [20.8 1H.2 Das versteckte Lager](#d20-8)
    - [20.9 1H.3 Der Winter, der kommt](#d20-9)
    - [20.10 1H.4 Der Alte Keiler](#d20-10)
    - [20.11 1H.5 Asgerds Kamm](#d20-11)
    - [20.12 1H.6 Die Frage des Ortes](#d20-12)
    - [20.13 Anmerkungen für das Team](#d20-13)
  - [21. Kapitel 2: Spieltext (Entwurf)](#d21)
    - [21.1 2K.1 Am Rand der Marsch](#d21-1)
    - [21.2 2K.2 Der Kreis im Watt](#d21-2)
    - [21.3 2K.3 Große Entscheidung: Den Kreis rufen lassen?](#d21-3)
    - [21.4 2K.4 Das Wrack der Schlickmutter](#d21-4)
    - [21.5 2K.5 Der Lotse](#d21-5)
    - [21.6 2K.6 Das Logbuch](#d21-6)
    - [21.7 2H.1 Arnfinns Karte](#d21-7)
    - [21.8 2H.2 Durch die Marsch](#d21-8)
    - [21.9 2H.3 Das Wrack und das Boot](#d21-9)
    - [21.10 2H.4 Große Entscheidung: Die Schlickmutter](#d21-10)
    - [21.11 2H.5 Eyvind](#d21-11)
    - [21.12 2H.6 Das Boot kommt heim](#d21-12)
    - [21.13 Anmerkungen für das Team](#d21-13)
  - [22. Kapitel 3: Das zweite Logbuch (Spieltext, Entwurf)](#d22)
    - [22.1 3.1 Das Buch am Feuer](#d22-1)
    - [22.2 3.2 Die Seiten](#d22-2)
    - [22.3 3.3 Der Thingplatz](#d22-3)
    - [22.4 3.4 Der Thing](#d22-4)
    - [22.5 3.5 Große Entscheidung: Hüter oder Rufer](#d22-5)
    - [22.6 3.6 Was danach im Dorf geschieht](#d22-6)
    - [22.7 3.7 Die erste Kerbe](#d22-7)
    - [22.8 Anmerkungen für das Team](#d22-8)
- **[Teil V – Kreaturen, Level und Kampf](#teil-5)**
  - [23. Kreaturen, Level und Kampfwerte (bereinigte Fassung „Küstenasche“)](#d23)
    - [23.1 1. Was feststeht](#d23-1)
    - [23.2 2. Level, Stufen und Biome](#d23-2)
    - [23.3 3. Erfahrung und Fähigkeitspunkte](#d23-3)
    - [23.4 4. Lebenspunkte und Haltung des Spielers](#d23-4)
    - [23.5 5. Kampfformeln](#d23-5)
    - [23.6 6. Gegner-Rang, Streuung und Anführer-Stufen](#d23-6)
    - [23.7 7. Kreaturen](#d23-7)
    - [23.8 8. Tiere und Menschen](#d23-8)
    - [23.9 9. Bosse (Sagen-Fassung)](#d23-9)
    - [23.10 10. Beute-Regeln](#d23-10)
    - [23.11 11. Herstellen: Metall plus Kreaturenteile](#d23-11)
    - [23.12 12. Technik](#d23-12)
    - [23.13 13. Was sich gegenüber Gemini ändert](#d23-13)
    - [23.14 14. Offen](#d23-14)
  - [24. Kreaturen-Daten (erzeugt)](#d24)
    - [24.1 1. Überblick](#d24-1)
    - [24.2 2. Kreaturen nach Modell-Welle](#d24-2)
    - [24.3 3. Einordnung aller 48 Kreaturen](#d24-3)
    - [24.4 4. Werte je Rang (LP / Haltung / Schaden)](#d24-4)
    - [24.5 5. Spawn-Tabellen je Biom](#d24-5)
    - [24.6 6. Beute je Kreatur (bereinigt)](#d24-6)
    - [24.7 7. Gegenstands-Katalog](#d24-7)
- **[Teil VI – Art Direction, Stil und Figuren](#teil-6)**
  - [25. Stilidee „Gelebter Norden“ (Vorschlag)](#d25)
    - [25.1 Was wir von wem lernen](#d25-1)
    - [25.2 Kernidee in einem Satz](#d25-2)
    - [25.3 Sechs Stilregeln](#d25-3)
    - [25.4 Material- und Farbpalette](#d25-4)
    - [25.5 Menschen](#d25-5)
    - [25.6 Anzeige (UI)](#d25-6)
    - [25.7 Klang (passend zur vorhandenen Musik)](#d25-7)
    - [25.8 Technik-Leitplanken](#d25-8)
    - [25.9 Was das für die laufenden Arbeiten heißt](#d25-9)
  - [26. Stil „Gelebter Norden“ mit Gezeitenhall (Mischung)](#d26)
    - [26.1 Kernidee in einem Satz](#d26-1)
    - [26.2 Was woher kommt](#d26-2)
    - [26.3 Sieben Stilregeln](#d26-3)
    - [26.4 Grundpalette](#d26-4)
    - [26.5 Kleidung und Kleidungsfarben (Charaktereditor)](#d26-5)
    - [26.6 Menschen](#d26-6)
    - [26.7 Materialien](#d26-7)
    - [26.8 Welt](#d26-8)
    - [26.9 Kreaturen](#d26-9)
    - [26.10 Gezeitenhall: Fähigkeiten und Effekte](#d26-10)
    - [26.11 Licht und Atmosphäre](#d26-11)
    - [26.12 Anzeige (UI) und Klang](#d26-12)
    - [26.13 Regeln für einheitliche Assets](#d26-13)
    - [26.14 Technik-Leitplanken](#d26-14)
    - [26.15 Gemeinsamer Stilprompt (für Bild- und 3D-Werkzeuge)](#d26-15)
  - [27. Briefing für das Design (ChatGPT): Fotorealistische Vegetation](#d27)
    - [27.1 Was sich geändert hat](#d27-1)
    - [27.2 Bitte um Vorschlag](#d27-2)
    - [27.3 Rahmenbedingungen](#d27-3)
    - [27.4 Ergebnis bitte nach](#d27-4)
  - [28. Briefing für ChatGPT – Design-Lead](#d28)
    - [28.1 Deine Rolle](#d28-1)
    - [28.2 Das Spiel in Kürze](#d28-2)
    - [28.3 Harte Vorgaben](#d28-3)
    - [28.4 Was du liefern sollst (in dieser Reihenfolge)](#d28-4)
    - [28.5 Technische Budgets (gelten auch fürs Design)](#d28-5)
    - [28.6 Format der Ergebnisse](#d28-6)
    - [28.7 Ablauf](#d28-7)
    - [28.8 Erster Auftrag an ChatGPT](#d28-8)
  - [29. Leitfaden: Blender- und Unreal-Assets, Gesichter, Animation](#d29)
    - [29.1 1. Was ausgewertet wurde](#d29-1)
    - [29.2 2. Kritische Befunde (vor allem anderen lesen)](#d29-2)
    - [29.3 3. Lehren aus den 3D-Archiven (was man richtig und falsch sieht)](#d29-3)
    - [29.4 4. Pipeline Blender → Unreal (Einstellungen, die Fehler vermeiden)](#d29-4)
    - [29.5 5. Gesichter](#d29-5)
    - [29.6 6. Animation](#d29-6)
    - [29.7 7. Offene Fragen an den Nutzer](#d29-7)
    - [29.8 Quellen (Recherche 2026-11-21)](#d29-8)
  - [30. Kleidung für die Figuren: kostenlose Kandidaten (Fab, geprüft 2026-11-16)](#d30)
    - [30.1 Referenzbilder des Nutzers (2026-11-16)](#d30-1)
    - [30.2 Vorschlag](#d30-2)
  - [31. Review 01 – Art Direction, Holzbaukasten, Arbeitsstätten (2026-11-12)](#d31)
    - [31.1 Gesamturteil](#d31-1)
    - [31.2 1. Lager passt nicht zum Dach-Baukasten (muss geklärt werden)](#d31-2)
    - [31.3 2. Dreiecksbudgets: Ziel- statt Höchstwerte für Module](#d31-3)
    - [31.4 3. Fundament auf unebenem Gelände](#d31-4)
    - [31.5 4. Drehpunkte (Pivots) und Ecken](#d31-5)
    - [31.6 5. Turf-Material](#d31-6)
    - [31.7 Kleinigkeiten (optional)](#d31-7)
    - [31.8 Nachprüfung der überarbeiteten Fassung (2026-11-12)](#d31-8)
    - [31.9 Entscheidungen des Nutzers](#d31-9)
  - [32. Review 02 – Arbeitsstätten (3. Fassung), Armbrust, Rüstung, Entscheidungsdelta (2026-11-12)](#d32)
    - [32.1 1. `04_assets_arbeitsstaetten.md` (3. Fassung) – passt](#d32-1)
    - [32.2 2. `05_assets_armbrust.md` – passt, eine offene Stufe](#d32-2)
    - [32.3 3. `06_assets_ruestung_kleidung.md` – Slots müssen angepasst werden](#d32-3)
    - [32.4 4. `07_assetentscheidungen_2026-11-12.md` – zwei Abweichungen](#d32-4)
    - [32.5 5. Neue Entscheidungen des Nutzers (eingetragen in `docs/01_entscheidungen.md`)](#d32-5)
    - [32.6 6. Offene Fragen an den Nutzer](#d32-6)
- **[Teil VII – Assets, Asset-Listen und Prompts](#teil-7)**
  - [33. Asset-Plan für ChatGPT (Stand 2026-11-12)](#d33)
    - [33.1 1. Zweck](#d33-1)
    - [33.2 2. Was du lieferst – und was nicht](#d33-2)
    - [33.3 3. Voraussetzung](#d33-3)
    - [33.4 4. Technische Vorgaben (für jedes Konzeptblatt)](#d33-4)
    - [33.5 5. Asset-Liste Phase 1 (Vertical Slice)](#d33-5)
    - [33.6 6. Format deiner Lieferung](#d33-6)
    - [33.7 7. Regeln](#d33-7)
    - [33.8 8. Ausblick (noch nicht bearbeiten)](#d33-8)
    - [33.9 9. Offene Entscheidungen (Nutzer)](#d33-9)
    - [33.10 Erster Auftrag an ChatGPT (nach der Stilwahl)](#d33-10)
  - [34. Gesamtliste: Gegenstände, Assets und Inhalte (Stand 2026-11-13)](#d34)
    - [34.1 0. Legende](#d34-1)
    - [34.2 1. Überblick: Mengen (grobe Schätzung)](#d34-2)
    - [34.3 2. Figuren](#d34-3)
    - [34.4 3. Tiere (`animal_`)](#d34-4)
    - [34.5 4. Menschliche Gegner (`enemy_`) ◐](#d34-5)
    - [34.6 5. Sagenwesen (`creature_`) – zurückgestellt](#d34-6)
    - [34.7 6. Bosse (`boss_`) ◐ – ein Boss pro Biom](#d34-7)
    - [34.8 7. Landschaften (`biome_`) ◐](#d34-8)
    - [34.9 8. Ressourcen-Vorkommen (`res_`)](#d34-9)
    - [34.10 9. Orte (`poi_`) ◐](#d34-10)
    - [34.11 10. Siedlung: Baukasten (`building_`)](#d34-11)
    - [34.12 11. Siedlung: Gebäude und Arbeitsstätten](#d34-12)
    - [34.13 12. Möbel, Requisiten, Deko (`prop_`, `deco_`)](#d34-13)
    - [34.14 13. Gegenstände](#d34-14)
    - [34.15 14. Waffen, Schilde, Rüstung, Kleidung](#d34-15)
    - [34.16 15. Transport und Fahrzeuge ○ (offen, nur Vorschlag)](#d34-16)
    - [34.17 16. Animationen (`anim_`)](#d34-17)
    - [34.18 17. Effekte (`vfx_`)](#d34-18)
    - [34.19 18. Ton und Musik](#d34-19)
    - [34.20 19. Oberflächen (`ui_`)](#d34-20)
    - [34.21 20. Gestaltung im Charakter- und Lager-Editor ✔](#d34-21)
    - [34.22 21. Technik und Grundlagen (für alle Assets)](#d34-22)
    - [34.23 22. Texturen ○ (Vorschlag, ergänzt am 2026-11-13)](#d34-23)
    - [34.24 23. Deko-Modelle und Kleinkram ○ (ergänzt am 2026-11-13)](#d34-24)
    - [34.25 24. Was als Nächstes sinnvoll ist](#d34-25)
  - [35. Asset-Liste Waffen mit Tier-Stufen (Planung für Phase 3, Stand 2026-11-12)](#d35)
    - [35.1 1. Grundidee](#d35-1)
    - [35.2 2. Tier-Stufen](#d35-2)
    - [35.3 3. Waffentypen und Verfügbarkeit](#d35-3)
    - [35.4 4. Mesh-Strategie pro Typ](#d35-4)
    - [35.5 5. Budgets](#d35-5)
    - [35.6 6. Benennung](#d35-6)
    - [35.7 7. Weitere Assets, die an Waffen hängen](#d35-7)
    - [35.8 8. Mengenabschätzung (MVP Phase 3)](#d35-8)
    - [35.9 9. Entscheidungen](#d35-9)
  - [36. Asset-Liste Rüstung und Kleidung mit Tier-Stufen (Planung, Stand 2026-11-12, überarbeitet)](#d36)
    - [36.1 1. Grundidee](#d36-1)
    - [36.2 2. Slots (bestätigt am 2026-11-12)](#d36-2)
    - [36.3 3. Ausrüstung nach Stufe](#d36-3)
    - [36.4 4. Kleidung (ohne Schutzwert)](#d36-4)
    - [36.5 5. Miliz (Phase 2)](#d36-5)
    - [36.6 6. Budgets](#d36-6)
    - [36.7 7. Weitere Assets](#d36-7)
    - [36.8 8. Mengenabschätzung](#d36-8)
    - [36.9 9. Entscheidungen](#d36-9)
  - [37. Asset-Liste Gegenstände mit Tier-Stufen (Planung, Stand 2026-11-12)](#d37)
    - [37.1 1. Grundidee](#d37-1)
    - [37.2 2. Rohstoffe](#d37-2)
    - [37.3 3. Zwischenprodukte (hergestellt)](#d37-3)
    - [37.4 4. Nahrung (Buff-System, kein Ausdauer-Ersatz)](#d37-4)
    - [37.5 5. Werkzeuge mit Stufen](#d37-5)
    - [37.6 6. Verbrauchsgüter](#d37-6)
    - [37.7 7. Wertgegenstände, Handel, Quest](#d37-7)
    - [37.8 8. Behälter- und Ablage-Props](#d37-8)
    - [37.9 8a. Tierhaltung (bestätigt, einfach gehalten)](#d37-9)
    - [37.10 9. Budgets](#d37-10)
    - [37.11 10. Mengenabschätzung](#d37-11)
    - [37.12 11. Entscheidungen](#d37-12)
  - [38. Prompt-Standard für KI-erzeugte Assets (Stand 2026-11-12)](#d38)
    - [38.1 1. Grundsatz und ehrliche Grenze](#d38-1)
    - [38.2 2. Pflichtfelder jedes Prompt-Pakets](#d38-2)
    - [38.3 3. Feste Textblöcke (wörtlich verwenden, nie umformulieren)](#d38-3)
    - [38.4 4. Vorlagen nach Asset-Typ](#d38-4)
    - [38.5 5. Ablauf](#d38-5)
    - [38.6 6. Ablage](#d38-6)
    - [38.7 7. Abnahme-Kriterien (messbar)](#d38-7)
    - [38.8 8. Ausgearbeitete Beispiele](#d38-8)
  - [39. Prompt-Paket `item_log` – Baumstamm (Phase 1, Prio A)](#d39)
    - [39.1 Prompt 1 – Konzeptblatt (V1)](#d39-1)
    - [39.2 Prompt 2 – Bild-zu-3D (V2)](#d39-2)
    - [39.3 Abnahme](#d39-3)
  - [40. Prompt-Paket `trim_wood` – Holz-Trim-Sheet (Phase 1, Prio A)](#d40)
    - [40.1 Streifen-Layout (Zielpositionen im fertigen Atlas)](#d40-1)
    - [40.2 Prompt 1 – plank](#d40-2)
    - [40.3 Prompt 2 – timber](#d40-3)
    - [40.4 Prompt 3 – end](#d40-4)
    - [40.5 Prompt 4 – dark](#d40-5)
    - [40.6 Prompt 5 – metal](#d40-6)
    - [40.7 Abnahme](#d40-7)
  - [41. Prompt-Paket `ui_icon_res_wood` – Icon Baumstamm (Phase 1, Prio A)](#d41)
    - [41.1 Prompt (V4)](#d41-1)
    - [41.2 Abnahme](#d41-2)
  - [42. Asset-Log](#d42)
- **[Teil VIII – Anhang: Tier-1-Asset-Karten](#teil-8)**
  - [43. Tier-1 Asset-Status / Tier-1 Asset Status](#d43)
    - [43.1 Deutsch](#d43-1)
    - [43.2 English](#d43-2)
  - [44. KA_T01_Tol_BuildHammer — Bauhammer / Build Hammer](#d44)
    - [44.1 Deutsch](#d44-1)
    - [44.2 English (script source)](#d44-2)
  - [45. KA_T01_Tol_BuildTool — Bauwerkzeug / Build Tool](#d45)
    - [45.1 Deutsch](#d45-1)
    - [45.2 English (script source)](#d45-2)
  - [46. KA_T01_Tol_Hammer — Hammer / Hammer](#d46)
    - [46.1 Deutsch](#d46-1)
    - [46.2 English (script source)](#d46-2)
  - [47. KA_T01_Tol_Hoe — Hacke / Hoe](#d47)
    - [47.1 Deutsch](#d47-1)
    - [47.2 English (script source)](#d47-2)
  - [48. KA_T01_Tol_Pickaxe — Spitzhacke / Pickaxe](#d48)
    - [48.1 Deutsch](#d48-1)
    - [48.2 English (script source)](#d48-2)
  - [49. KA_T01_Tol_Rake — Rechen / Rake](#d49)
    - [49.1 Deutsch](#d49-1)
    - [49.2 English (script source)](#d49-2)
  - [50. KA_T01_Tol_Saw — Säge / Saw](#d50)
    - [50.1 Deutsch](#d50-1)
    - [50.2 English (script source)](#d50-2)
  - [51. KA_T01_Tol_Shovel — Spaten / Shovel](#d51)
    - [51.1 Deutsch](#d51-1)
    - [51.2 English (script source)](#d51-2)
  - [52. KA_T01_Tol_Sickle — Sichel / Sickle](#d52)
    - [52.1 Deutsch](#d52-1)
    - [52.2 English (script source)](#d52-2)
  - [53. KA_T01_Tol_SmithHammer — Schmiedehammer / Smithing Hammer](#d53)
    - [53.1 Deutsch](#d53-1)
    - [53.2 English (script source)](#d53-2)
  - [54. KA_T01_Tol_Tongs — Schmiedezange / Smithing Tongs](#d54)
    - [54.1 Deutsch](#d54-1)
    - [54.2 English (script source)](#d54-2)
  - [55. KA_T01_Tol_Torch — Fackel / Torch](#d55)
    - [55.1 Deutsch](#d55-1)
    - [55.2 English (script source)](#d55-2)
  - [56. KA_T01_Tol_WoodAxe — Holzfälleraxt / Wood Axe](#d56)
    - [56.1 Deutsch](#d56-1)
    - [56.2 English (script source)](#d56-2)
  - [57. KA_T01_Wep_Axe — Standardaxt / Standard Axe](#d57)
    - [57.1 Deutsch](#d57-1)
    - [57.2 English (script source)](#d57-2)
  - [58. KA_T01_Wep_Mace — Streitkolben / Mace](#d58)
    - [58.1 Deutsch](#d58-1)
    - [58.2 English (script source)](#d58-2)
  - [59. KA_T01_Wep_Seax — Seax](#d59)
    - [59.1 Deutsch](#d59-1)
    - [59.2 English (script source)](#d59-2)
  - [60. KA_T01_Wep_Shield_Round — Rundschild / Round Shield](#d60)
    - [60.1 Deutsch](#d60-1)
    - [60.2 English (script source)](#d60-2)
  - [61. KA_T01_Wep_Spear — Speer / Spear](#d61)
    - [61.1 Deutsch](#d61-1)
    - [61.2 English (script source)](#d61-2)
  - [62. KA_T01_Wep_Warhammer — Kriegshammer / Warhammer](#d62)
    - [62.1 Deutsch](#d62-1)
    - [62.2 English (script source)](#d62-2)

---

<a id="teil-1"></a>

# Teil I – Projektüberblick, Status und Entscheidungen

<a id="d1"></a>

## 1. Vision (Stand 2026-11-12)

> Quelle: `docs/00_vision.md`

Arbeitstitel: offen · Plattform: PC · Engine: Unreal Engine 5.8 (geplant)

<a id="d1-1"></a>

### 1.1 Kurzbeschreibung
Wikinger-/Mittelalter-Open-World mit Koop. Wie in **Aska** führst du als Anführer eine Siedlung: Dorfbewohner übernehmen
die Routinearbeit, du erkundest die Welt, kämpfst, baust und besiegst Bosse. Mehrere Questlinien tragen die Open World.

<a id="d1-2"></a>

### 1.2 Leitspiel und Entlehnungen
| Spiel | Wir übernehmen |
|---|---|
| **Aska (Leitspiel)** | Dorfbewohner-/Arbeitersystem, Siedlungsführung, Koop |
| Valheim | Bosse und Endgame |
| Kingdom Come: Deliverance 2 | Kampfsystem (ohne Ausdauer-Sperren) |
| Medieval Dynasty | Aufbau, Siedlungs-Setup |
| Manor Lords | Detailtiefe, Strukturen |
| ~~Tribes of Midgard~~ | gestrichen |

<a id="d1-3"></a>

### 1.3 Kernmerkmale
- **Koop:** Start mit 2–4 Spielern, höchstens **8 Spieler** (seit 2026-11-13, vorher Ziel 16); solo spielbar. Ab mehr als ca. 4 Spielern vermutlich Dedicated Server (siehe `docs/04_technische_ziele.md`).
- **Basisbau** mit Bauteilen (Snapping), Siedlung mit Dorfbewohnern.
- **Ausrüstung** wie in KCD: Slots, levelbasiert, schmiedbar. **Skilltree** für Fähigkeiten.
- **Kamera:** Third-Person als Hauptansicht; Vogelperspektive/2.5D (Vorschlag: als Bau- und Dorf-Kommandoansicht).
- **Keine Ausdauer-Sperren** beim Laufen, Ducken, Waffengebrauch. Das Essens-Buff-System bleibt.
- **Portalsystem** erst im späteren Spiel: große Ressourcenmengen von A nach B für den Basisausbau und zur Verteidigung großer Angriffswellen.
- **Weltoptionen im Menü, kombinierbar:** freies Bauen, Horden bekämpfen, „Survivor“ (Bedeutung noch offen). Basisangriffe optional.
- **Freies Tempo:** kein Zeitdruck als Kernschleife.
- **Schwierigkeit:** steigt stufenweise (Vorschlag: nach Biom und Boss, nicht stufenlos exponentiell).

<a id="d1-4"></a>

### 1.4 Das Dorfbewohner-System (Kernrisiko)
Askas bekannte Schwächen: zu viel Babysitting, schwache KI, klobige Lager-/Menü-Bedienung. Unser Gegenentwurf (Vorschläge):
- Bedürfnisse statt Uhr-Zeitplan.
- Prioritätslisten mit Fallback-Jobs.
- Ein Lager pro Siedlung.
- Übersichtliche Dorf-Kommandoansicht.
- Simulation nur auf dem Host bzw. Server.
- Portale verbinden Siedlungslager; Dorfbewohner arbeiten lokal.

<a id="d1-5"></a>

### 1.5 Bewusst nicht im Umfang (vorerst)
Voxel-Terraforming (Enshrouded-Stil).


[↑ zum Inhaltsverzeichnis](#inhaltsverzeichnis)

<a id="d2"></a>

## 2. Projekt-Status „Gamer“ – Übersicht für den PC-Umzug

> Quelle: `PROJEKT_STATUS.md`

Stand: 22.11.2026 · Erstellt von Claude nach einem Scan von `game/Source` und `game/Content` (plus kurzer Blick auf die Nachbarordner unter `D:\gamer`).
**Es wurde nichts gelöscht, verschoben oder geändert.** Diese Datei ist nur eine Bestandsaufnahme.

---

<a id="d2-1"></a>

### 2.1 Das Wichtigste in drei Sätzen

1. **Dein Spiel selbst ist klein und ordentlich:** der ganze Programm-Code (ca. 100 Dateien) wird benutzt, da ist kein Müll drin.
2. **Der Content-Ordner ist riesig (ca. 20 GB), aber drei Viertel davon sind gekaufte/geschenkte Asset-Pakete, die dein Spiel im Moment gar nicht benutzt.**
3. **Achtung beim Umzug:** Ein großer Teil deiner neueren Arbeit (u. a. die MetaHuman-Figuren, neue Waffen- und Werkzeug-Modelle) ist **noch nicht in Git gesichert**. Wenn du nur „aus GitHub neu herunterlädst“, fehlt das auf dem neuen PC.

---

<a id="d2-2"></a>

### 2.2 1. Was schon funktioniert

So steht es in deinen Projekt-Notizen (`docs/02_phasenplan.md`), dort jeweils mit Tests belegt. Ich habe das Spiel für diese Übersicht **nicht selbst gestartet**.

| Bereich | Was das im Spiel heißt |
|---|---|
| **Koop / Mehrspieler** | Bis zu 8 Spieler in einer Welt. Einer ist Gastgeber, andere treten über „Beitreten“ bei. Auch ein eigener Server ohne Bild ist möglich. Getestet nur auf einem PC, noch nicht über zwei echte PCs. |
| **Startmenü** | Welt erstellen (mit Spielmodus, Schwierigkeit, Bau-Optionen), Beitreten, Steuerung, Grafik, Beenden. |
| **Spielfigur** | Laufen, Springen, Sprinten (ohne Ausdauerbalken), Kamera über die Schulter, Animationen. |
| **Sammeln & Abbauen** | Äste, Steine, Fasern von Hand; Bäume und Felsen mit Axt/Spitzhacke per Schlag. Werkzeug wird automatisch gewählt. Pflanzen wachsen sichtbar nach. |
| **Inventar & Schnellleiste** | 24 Plätze, 8er-Schnellleiste, Mausrad zum Durchschalten. |
| **Herstellen** | Menü mit Bildern (Icons), Rezepte an Werkbank und Lagerfeuer. |
| **Bauen** | Holzhaus aus Fundament, Wänden, Tür, Dach usw. Erst Bauplan, dann mit Hammer fertigbauen. Abreißen. |
| **Dorfbewohner** | Ziehen in freie Betten ein, haben Hunger, Schlaf, Unterkunft. Berufe: Sammler, Holzfäller, Steinmetz, Koch, Baumeister, Bauer. Laufen um Häuser herum (Wegfindung). |
| **Lager** | Ein gemeinsames Lager für die ganze Siedlung. |
| **Essen & Feldbau** | Essen verdirbt, Obstbäume, Wildpflanzen, Saatgut, Felder, Komposter, Regenfänger, Taverne mit Gerichten. |
| **Welt** | Eigene Karte (250 × 250 m) mit Gras, Farnen, Bäumen, Felsen, Jahreszeiten. |
| **Vogelperspektive** | Taste B: von oben auf das Dorf schauen, Bewohner anklicken, Berufe ändern. |
| **Leben & Tod** | Lebenspunkte, Heilung, Niedergeschlagen/Wiederbeleben, Grab mit deinen Sachen nach dem Tod. |
| **Speichern** | Spielstände für Welten und Spieler. |
| **Umschauen / Lauschen** | Eine „Scan-Welle“, die Dinge in der Nähe hervorhebt (Grundlage für Magie später). |
| **Einstellungen** | Tasten frei umbelegbar, Grafik-Einstellungen, Schriftgröße. |
| **Leistung** | Gemessen am 21.11.: braucht ca. 2,7 GB RAM und ca. 1 GB Grafikspeicher, deutlich unter deinen Grenzen. **Aber:** noch ohne MetaHuman-Figuren gemessen. |

**Noch nicht da:** Kämpfen mit Waffen, Gegner/Tiere, Quests im Spiel (die Geschichte ist erst als Text in `docs/` geschrieben), Tag/Nacht und Wetter.

---

<a id="d2-3"></a>

### 2.3 2. Welche Dateien wichtig sind

#### A) Unverzichtbar – das IST dein Spiel

| Ort | Was ist das? | In Git gesichert? |
|---|---|---|
| `game/Source/` | Der Programm-Code (alle Spielregeln). ~100 Dateien, **alle werden benutzt**. | Ja |
| `game/Gamer.uproject` | Die „Startdatei“ des Projekts. Doppelklick öffnet es. | Ja |
| `game/Config/` | Einstellungen des Projekts. | Ja – **außer** `DefaultEngine.ini`, die enthält ein privates Token und wird absichtlich nicht hochgeladen. **Diese Datei von Hand mitnehmen!** |
| `game/Content/Data/` | Die Spielwerte (Gegenstände, Rezepte, Berufe, Gebäude …) als JSON-Tabellen. | Ja (größtenteils) |
| `game/Content/Gamer/` | Deine eigenen Modelle, Materialien, Karten (`World`, `Showcase`), Icons, Pflanzen. | **Nur teilweise**, siehe unten |
| `game/Content/Characters/` | Die Standard-Figuren „Manny“ und „Quinn“, die deine Spielfigur gerade benutzt. | Ja |
| `docs/` | Deine Konzepte, Entscheidungen, Phasenplan, Story-Kapitel. | Ja (mit ungespeicherten Änderungen) |
| `design/` | Design-Unterlagen und die KI-Inhalte-Liste (`asset_log.md`, wichtig für Steam). | Ja (mit ungespeicherten Änderungen) |
| `Assets/` | Die Rohdateien deiner Modelle (Blender, FBX, Bilder), aus denen die Unreal-Inhalte gebaut werden. | **Nur teilweise** |
| `tools/` | Hilfs-Skripte zum Bauen, Testen, Importieren. | Ja |
| `CLAUDE.md`, `AGENTS.md`, `README.md` | Anleitungen für die KI-Helfer und dich. | Ja |
| `Spiel_starten.bat`, `Spiel_testen.bat` | Doppelklick zum Spielen/Testen. | Ja |

#### B) Wichtig, aber noch NICHT in Git gesichert (beim Umzug unbedingt mitkopieren!)

- **`game/Content/Gamer/Characters/MetaHuman/`** – ca. **5 GB**, rund 1000 Dateien. Deine MetaHuman-Figuren.
- **`game/Content/Gamer/Characters/Clothing/`** – Kleidung für die Figuren.
- Teile von `game/Content/Gamer/Buildings/` und `game/Content/Gamer/Nature/`.
- **`Assets/Items/`** – die neuen Werkzeuge und Waffen (Axt, Schwert, Speer, Schild, Hammer …) als FBX.
- **`Assets/Source/`** – die zugehörigen Blender-Dateien.
- `Assets/Buildings/Stations/`, neue Texturen in `Assets/Buildings/StyleTest/`, `Assets/Previews/`.
- Dazu rund 30 bereits gesicherte Dateien, die du seitdem **geändert** hast (Icons, Doku, Config).
- `game/Config/DefaultEngine.ini` (absichtlich nicht in Git, siehe oben).

Außerdem gibt es **9 Arbeits-Zweige** (Branches) in Git, aber online auf GitHub liegt nur `main`. Wenn die anderen Zweige noch gebraucht werden, müssen sie vor dem Umzug hochgeladen oder der ganze Ordner kopiert werden.

#### C) Fremde Asset-Pakete – liegen im Projekt, werden aber NICHT benutzt

Ich habe in deinem eigenen Inhalt, im Code und in den Einstellungen nach Verweisen auf diese Pakete gesucht und **keinen einzigen gefunden**. Sie wurden alle am 17.11. hinzugefügt (vermutlich aus dem Fab/Marketplace) und sind auch nicht in Git.

| Paket | Größe | Was drin ist (laut Name) |
|---|---|---|
| `FantasyBundle` | ~5,2 GB | Burg, Höhle, Dorf (Demo-Karten) |
| `MegascansMeadowPack` | ~2,2 GB | Wiesen-Pflanzen |
| `Viking_Village_Pack` | ~1,9 GB | Wikingerdorf |
| `Weapon_Pack` | ~1,2 GB | Waffen |
| `Elves_Wizard` | ~0,9 GB | Elfen-Figuren |
| `QuadrapedCreatures` | ~0,6 GB | Drache, Greif, Zentaur, Barghest |
| `Viking` | ~0,5 GB | Wikinger |
| `Free_Idle_Animation_Pack_anim` | ~0,4 GB | Stand-Animationen |
| `Stylized_Cloth_Casual_01_for_Kvinsi` | ~0,4 GB | Kleidung |
| `AnimatedWoodenCrateV1` | ~0,4 GB | Holzkiste (die gefundenen Verweise kommen nur aus der eigenen Demo-Karte des Pakets) |
| `ModuleManager` | ~0,4 GB | unklar |
| `Viking_Warrior` | ~0,2 GB | Krieger-Figur |
| `PN_GroundFoliage` | ~0,2 GB | Bodenpflanzen |
| `FreeMusicPack` | ~0,1 GB | Musik |
| `HighPoly_Tree_Model` | ~40 MB | ein Baum |
| `RPG_Fantasy_SFX` | ~10 MB | Soundeffekte |
| **Summe** | **ca. 15 GB** | |

**Wichtig:** „Nicht benutzt“ heißt nicht „Müll“. Vielleicht wolltest du daraus noch Dinge nehmen (z. B. Musik, Soundeffekte, Tiere, Waffen). Und: Wenn du diese Pakete in dein Spiel einbaust, werden sie beim Fertigstellen mitgepackt – dein Ziel ist höchstens ca. 20 GB fürs fertige Spiel. Meine Suche war eine Textsuche in den Dateien; sie ist zuverlässig, aber nicht 100 % sicher. Vor einem Löschen sollte man das im Unreal-Editor mit dem „Reference Viewer“ noch einmal gegenprüfen.

#### D) Automatisch erzeugte Ordner – muss man NICHT umziehen

Diese Ordner baut Unreal bzw. Visual Studio von selbst neu. Sie mitzukopieren schadet nicht, kostet aber Zeit und Platz:

- `game/Binaries/`, `game/Intermediate/`, `game/DerivedDataCache/`, `game/Build/`, `game/.vs/`, `game/Saved/` (Ausnahme: in `Saved/` liegen ggf. **Spielstände und deine Tasten-/Grafik-Einstellungen** – wenn dir die wichtig sind, `Saved/SaveGames` und `Saved/Config` mitnehmen)
- `game/*.sln`, `game/*.slnx` (Projektdateien für Visual Studio, werden neu erzeugt)
- `D:\gamer\unreal\DDC` – **ca. 38 GB** Zwischenspeicher. Wird neu aufgebaut (der erste Start auf dem neuen PC dauert dann länger, weil Shader neu übersetzt werden).

#### E) Außerhalb des Projektordners (unter `D:\gamer`)

| Ordner | Größe | Einschätzung |
|---|---|---|
| `unreal\UE_5.8` | ~30 GB | **Alte Unreal-Installation.** Laut `CLAUDE.md` wurde sie am 16.11. gelöscht und durch die selbst gebaute Engine in `E:\UE58` ersetzt – sie ist aber noch da (Datum 17.11.). Dein Projekt benutzt sie nicht. Bitte prüfen, ob sie noch gebraucht wird. |
| `heruntergeladenes` | ~7,6 GB | Downloads (davon 6,3 GB Sounds, dazu ZIPs wie Medieval Village MegaKit, Fantasy Props, Animationen). Rohmaterial, nicht im Spiel. Mitnehmen, wenn du es noch verwenden willst. |
| `Gamer_Playtest_2026-11-15_v3.zip` | ~1,4 GB | Alte, fertig gepackte Testversion vom 15.11. Kann jederzeit neu gebaut werden. |
| `programme\whisper` | ~1,7 GB | Spracherkennungs-Programm (für deine Spracheingabe?). Nicht Teil des Spiels. |
| `referensbilder - Modell` | ~1,4 GB | Referenzbilder und Test-3D-Modelle. |
| `cc0_quellen` | ~0,75 GB | Freie Original-Texturen/Modelle, aus denen deine Pflanzen und Felsen gebaut sind. **Mitnehmen**, damit sie sich neu bauen lassen. |
| `reference_contact_temp.jpg` | 1 MB | Sieht nach einer Testdatei aus („temp“). |
| `gegenstände blender`, `ki3d_rohdaten` | leer | Leere Ordner. |

**Nicht vergessen – liegt auf Laufwerk E:**
- `E:\UE58` – **die Unreal Engine, mit der dein Projekt wirklich läuft** (selbst gebaut, das hat ca. 10 Stunden gedauert). Auf dem neuen PC entweder komplett mitkopieren oder neu bauen (Anleitung in `docs/05_source_build_plan.md` und `docs/03_setup_checkliste.md`).
- `E:\Git`, `E:\VS\BuildTools`, `E:\Blender` – Programme, auf dem neuen PC neu installieren.

---

<a id="d2-4"></a>

### 2.4 3. Was als Nächstes sinnvoll wäre

#### Vor dem Umzug (in dieser Reihenfolge)

1. **Alles Wichtige in Git sichern.** Die ungesicherten Dateien aus Abschnitt 2B „committen“ und zu GitHub hochladen. Achtung: der MetaHuman-Ordner ist ca. 5 GB groß – vorher klären, ob der in Git (mit LFS) soll oder lieber separat kopiert wird. Claude kann dabei helfen.
2. **Die anderen Git-Zweige hochladen** oder bewusst entscheiden, dass sie weg können.
3. **`game/Config/DefaultEngine.ini` separat sichern** (USB-Stick o. Ä.), weil sie nicht in Git ist.
4. **Am sichersten:** zusätzlich den ganzen Ordner `D:\gamer\gamer` auf eine externe Festplatte kopieren. Die Ordner aus Abschnitt 2D kannst du dabei weglassen.
5. **Engine klären:** `E:\UE58` mitnehmen (am schnellsten) oder auf dem neuen PC neu bauen.

#### Auf dem neuen PC

6. Git, Visual Studio Build Tools, Blender installieren (Liste: `docs/03_setup_checkliste.md`).
7. Engine einrichten, Projekt kopieren/herunterladen, `Gamer.uproject` öffnen. Der erste Start dauert lange (Shader).
8. Einmal `Spiel_testen.bat` starten und kurz prüfen, ob Sammeln, Bauen und Bewohner gehen.
9. Pfade in `CLAUDE.md` anpassen, falls die Laufwerke anders heißen (dort stehen z. B. `E:\UE58` und `E:\Git`) – das ist eine „kernelementare“ Datei, Claude fragt vorher.

#### Aufräumen (erst nach dem Umzug, nur mit deiner Zustimmung)

10. Entscheiden, welche der **nicht benutzten Asset-Pakete** (ca. 15 GB) du behalten willst. Die übrigen könnte man aus dem Projekt nehmen – vorher im Editor gegenprüfen.
11. Die alte Engine `D:\gamer\unreal\UE_5.8` (30 GB) entfernen, wenn sicher ist, dass sie nicht gebraucht wird.
12. `CLAUDE.md` (Abschnitt „Aktueller Stand“) und `README.md` aktualisieren – die `README.md` ist vom 12.11. und sagt noch „Unreal-Projekt noch nicht angelegt“.

#### Für das Spiel selbst (laut deinem Phasenplan)

13. **Leistung neu messen, sobald die MetaHuman-Figuren im Spiel sind** – die werden deutlich mehr Grafikspeicher brauchen (deine Grenze: 8 GB).
14. Danach geht es laut Plan mit **Kampf und Ausrüstung** (Phase 3) weiter; die Waffen-Modelle dafür liegen schon in `Assets/Items/`.

---

<a id="d2-5"></a>

### 2.5 4. Deine Wunschliste vom 22.11.2026

Du hast per Sprache diktiert. So habe ich es verstanden. **Wo ich unsicher bin, steht „Deutung“ dabei, bitte bestätigen oder korrigieren.**

#### Bauen mit dem Bauplan-System (der größte und genaueste Wunsch)

| Wunsch | Was es schon gibt | Was neu wäre |
|---|---|---|
| Bauen läuft über Baupläne (Blaupausen) | Ja: Bauplan setzen, dann mit dem Hammer fertig schlagen (Weltoption „Bauplan + Hammer“) | – |
| Die Rohstoffe für jedes Objekt müssen **angeliefert** werden | Teilweise: Bei „Material beim Bauen“ holt der Baumeister das Material aus dem Lager | Rohstoffe werden sichtbar **zum Bauplan gebracht** (von Bewohnern oder vom Spieler), erst dann geht es weiter |
| Objekte entstehen in **3 Stufen**, jede Stufe braucht andere Rohstoffe, und es wird nur geliefert, was die jeweilige Stufe kostet | Nein. Es gibt „Ausbaustufen“ für Arbeitsgebäude, aber das ist etwas anderes (Verbesserung nach dem Bau) | z. B. Stufe 1 Gerüst/Fundament (Stämme), Stufe 2 Rohbau (Bretter, Steine), Stufe 3 Fertigstellung (Fasern, Lehm …). Jede Stufe sieht im Spiel anders aus |
| Spieler setzt den Bauplan, Bewohner bauen ihn auf | Ja, der Beruf Baumeister macht das schon | Bewohner liefern **und** bauen je Stufe |
| Spieler können auch **selbst** bauen | Ja, mit dem Hammer | **Deutung:** Der Spieler kann statt der Bewohner selbst liefern und hämmern. **Oder meintest du:** Spieler können eigene Baupläne *entwerfen* (z. B. ein ganzes Haus als Vorlage speichern und wieder setzen)? |
| **Koop-Rechte pro Objekt:** Wer den Bauplan setzt, bestimmt für genau dieses Objekt, ob Mitspieler nur **mitbauen** dürfen oder auch **abreißen** | Nein. Abreißen ist bisher nur eine Weltoption für alle | Neue Einstellung je Objekt, nur der Besitzer darf sie ändern; der Server prüft sie |

**Offene Fragen dazu:**
- Gelten die 3 Stufen für **jedes** Bauteil (auch für eine einzelne Wand) oder nur für ganze Gebäude und Arbeitsstätten? Bei einzelnen Wänden würde das sehr kleinteilig.
- Wie verhalten sich die 3 Stufen zu den bestehenden Weltoptionen „sofort fertig“ und „Material beim Bauen“? Vorschlag: Die Stufen werden der neue Standard für „Bauplan + Hammer“, „sofort fertig“ bleibt als Option.
- Sollen Mitspieler standardmäßig „nur mitbauen“ dürfen? (Vorschlag: ja.)
- Was passiert beim Abreißen einer halbfertigen Stufe – gibt es alles zurück, was schon geliefert wurde? (Vorschlag: ja.)

#### Weitere Wünsche (nur Stichworte, noch nicht genauer besprochen)

| Wunsch | Was es schon gibt |
|---|---|
| **Karte** (Weltkarte, evtl. Minikarte) | Nein. Die Spielerfarbe „für die Karte“ ist schon beschlossen |
| **Tutorial** | Nein, nur einzelne Anfänger-Tipps (z. B. beim Verderb) |
| **Charakter-Ersteller und -Anpassung** mit Hintergrundgeschichte, Werten, Fähigkeiten | Teilweise: Daten und Prüfung fertig (5 Werte, 24 Fähigkeiten, 5 Herkünfte, Aussehen in `characters.json`), Entwurf in `docs/11_charaktere.md`, MetaHuman beschlossen. **Ob das Menü schon im Spiel nutzbar ist, habe ich nicht geprüft** |
| **Kreaturen und Tiere** | Nein. Beschlossen: zuerst nur Menschen und Tiere, Sagenwesen höchstens später als Weltoption. Hinweis: Im Projekt liegt ein ungenutztes Paket mit Drache, Greif usw. – das passt nicht zu diesem Beschluss |
| **Höhlen** | Nein. In der Story gibt es eine Eishöhle (Kapitel 5) |
| **Wetter** | Nein, aber beschlossen (Regen, Gewitter, Nebel, Schnee, Sturm, mit leichter Wirkung auf Felder usw.) |
| **Sound** (Geräusche, Musik) | Nein. Es gibt noch keine Geräusche und keine Lautstärke-Einstellung |
| **Terraforming** (Boden umgraben, ebnen, Hügel abtragen) | Nein. Achtung: Die Welt hat noch keine echte Landschaft (nur einen flachen Boden), Terraforming braucht die zuerst. Im Koop ist es außerdem technisch aufwendig |

#### Ehrliche Einschätzung zum Umfang

Das sind zusammen **mehrere Monate Arbeit**. Deine eigene Regel 9 lautet: „erst Vertical Slice, dann ausbauen“. Höhlen, Kreaturen und Terraforming gehören laut Phasenplan eher in Phase 4 (Welt).

**Vorschlag für die Reihenfolge:**
1. **Erst der Umzug** (Abschnitt 3), damit nichts verloren geht.
2. **Bauen mit 3 Stufen, Anlieferung und Koop-Rechten**, weil es direkt auf dem bestehenden Bausystem aufbaut und Kern der Siedlung ist.
3. **Sound (Grundgeräusche) und Karte**: klein, macht das Spiel sofort greifbarer.
4. **Tutorial**: erst sinnvoll, wenn sich die Grundabläufe nicht mehr ändern.
5. **Charakter-Ersteller im Spiel fertigstellen** (Daten gibt es schon).
6. **Tiere, dann Wetter**.
7. **Später (Phase 4):** Höhlen, echte Landschaft, Terraforming.


[↑ zum Inhaltsverzeichnis](#inhaltsverzeichnis)

<a id="d3"></a>

## 3. Entscheidungslog (Stand 2026-11-12)

> Quelle: `docs/01_entscheidungen.md`

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
| Sprache und Dialoge | keine Sprachausgabe, keine Lippen-Synchronität; Gespräche als Text mit Gesten, dazu kurze Laute/Rufe in einer erfundenen, altnordisch klingenden Sprache | bestätigt (2026-11-13, Empfehlung übernommen); **ersetzt am 2026-11-15** durch „Vertonung“ unten |
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
| Abbauen von Bäumen und Felsen | durch Schlagen (Linksklick, Gamepad rechter Trigger) mit dem passenden Werkzeug in der Hand, nicht mit E. E bleibt für das Aufsammeln von Hand (Äste, lose Steine, Pflanzen, Beeren, Pilze). Mehrere Spieler können gemeinsam an einem Baum schlagen; den Ertrag bekommt, wer den letzten Schlag setzt | bestätigt (2026-11-13, Nutzer: „nicht mit E abbauen, das soll mit Schlagen passieren“); dass Aufsammeln bei E bleibt und die Ertragsregel im Koop sind Deutung/Vorschlag von Claude |
| Schnellleiste | 8 Plätze, Auswahl mit den Tasten 1–8; das Gewählte ist „in der Hand“. Umsetzung: die ersten 8 Inventarplätze; Werkzeuge und Essen landen bevorzugt dort, Baumaterial zuerst im Rucksack. Bei offenem Herstellungsmenü wählen die Zahlen weiter Rezepte | bestätigt (2026-11-13, Nutzer); Umsetzungsdetails Vorschlag von Claude |
| Tastenbelegung | alle Tasten sind im Menü unter „Steuerung“ frei umbelegbar | bestätigt (2026-11-13, Nutzer); noch nicht umgesetzt |
| Lesbarkeit der Anzeige | große Schrift (wächst mit der Fenstergröße), Schatten und dunkle Unterlage; kein Rot für Text, Fehlendes in Orange und immer auch als Klartext | bestätigt (2026-11-13, Rückmeldung aus dem ersten Spieltest) |
| Bau-Regeln als Weltoptionen | Bauablauf (Bauplan + Hammer / sofort fertig / Material erst beim Bauen), Platzierung (frei mit Einrasten am Vorhandenen / festes 300-cm-Weltraster) und Abreißen (volles Material zurück / halbes / kein Abreißen) sind Weltoptionen. Der Gastgeber wählt sie im Startmenü beim Erstellen der Welt; sie gelten für alle Spieler dieser Welt. Voreinstellung: Bauplan + Hammer, frei mit Einrasten, volles Material zurück | bestätigt (2026-11-13, Nutzer: „im Hauptmenü wählbar“, auf Nachfrage: Startmenü vor dem Spiel); Voreinstellungen = Empfehlungen von Claude |
| Baumenü öffnen | persönliche Einstellung: Rechtsklick mit dem Hammer in der Hand, unter „Steuerung“ umbelegbar | Vorschlag von Claude (2026-11-13), offen für Änderung |
| Berufe der Dorfbewohner | Der Spieler legt den Beruf jedes Bewohners in der Dorfverwaltung fest. Im Slice wählbar: Holzfäller, Steinmetz, Koch, Baumeister. Ohne Beruf, oder wenn der Beruf gerade nicht geht (z. B. keine Axt im Lager): Sammler (Beeren, Äste, Steine von Hand) | bestätigt (2026-11-13, Nutzer: „wird vom Spieler eingestellt in der Dorfverwaltung“); Auswahl der Berufe und der Sammler als Ausweich-Job = Vorschlag von Claude |
| Bedürfnisse der Bewohner (Slice) | Hunger, Schlaf, Unterkunft (eigenes Bett unter einem Dach); Wärme später mit dem Winter | Vorschlag (2026-11-13, nicht beantwortet) |
| Zuzug neuer Bewohner (Slice) | jedes freie Bett zieht nach einiger Zeit einen Bewohner an | Vorschlag (2026-11-13, nicht beantwortet) |
| Wegfindung der Bewohner | vorerst gerade Linie mit einfachem Ausweichen; Unreal-Navigation braucht Kerndatei-Änderungen (Build.cs, DefaultEngine.ini) und wird gesondert vorgeschlagen | Vorschlag (2026-11-13) |
| Vogelperspektive (erste Fassung) | eigene Taste (Standard B, umbelegbar), Kamera über der Siedlung (bewegen, zoomen, drehen), Bewohner/Gebäude anklicken für Infos, Beruf direkt wechseln; höchstens 120 m von der eigenen Figur; Bauen von oben später | Umsetzung beauftragt (2026-11-13, Nutzer: „Erst Vogelperspektive“); Funktionsumfang, Taste und Reichweite = Vorschlag von Claude |
| Grafik- und Toneinstellungen | Vorschlag: Grafik-Einstellungen (Qualitätsstufe, Auflösung, Fenster/Vollbild, Bildratenbegrenzung, V-Sync, Schriftgröße) als kleiner nächster Schritt; Lautstärkeregler zusammen mit den ersten Geräuschen | offen (Nutzer fragte am 2026-11-13 nach dem Zeitpunkt) |
| Grafik-Einstellungen (Umsetzung) | Seite „Grafik“ im Esc- und Startmenü: Qualität, Fenster/Vollbild randlos, Fenstergröße, Render-Auflösung, Bildratengrenze, V-Sync, Schriftgröße; „Automatisch einstellen“ und „Standard“. Kein exklusiver Vollbild. `Spiel_starten.bat` mit einem Fenster nutzt die gespeicherte Anzeige statt fester 960×540 | Umsetzung beauftragt (2026-11-13, Nutzer: „Grafik-Einstellungen“); Auswahl der Werte und Standardwerte = Vorschlag von Claude |
| Herkunft der Modelle („Meshauswahl“) | Schon entschieden am 2026-11-12, siehe Zeile „Herkunft 3D-Assets“ (Baukasten per Blender-Skript, organische Objekte per KI-3D + Blender). Offen ist nur der KI-3D-Dienst; Lizenzvergleich in `docs/07_ki3d_dienste.md` | Korrektur (2026-11-13): Claude hatte die Frage zuerst fälschlich als offen eingetragen |
| Vegetation: kostenlos selbst erzeugen | Bäume, Sträucher, Pflanzen, Gras (und Felsen) werden mit kostenlosen Werkzeugen selbst erzeugt, kein bezahlter KI-3D-Dienst; Meshy wird nicht abgeschlossen. Wunsch: Bäume wachsen sichtbar nach, Gras, feine Blätter. Ersetzt für Vegetation den KI-3D-Teil der Zeile „Herkunft 3D-Assets“ | bestätigt (2026-11-13, Nutzer: „da brauche ich nicht extra irgendwie Geld ausgeben“, Vorschlag Unreal-Werkzeuge oder freie Projekte); welches Werkzeug genau = offen, Empfehlung von Claude in `docs/07_ki3d_dienste.md` |
| Stil der Vegetation | fotorealistisch (Fotos als Vorlage, Texturen aus frei nutzbaren Fotos: eigene oder CC0). Weicht von der Art Direction „Klarform-Saga“ (stilisiert) ab; ob Gebäude, Figuren und Werkzeuge ebenfalls realistischer werden, ist offen und eine Design-Aufgabe (ChatGPT, Briefing `design/briefing_fotorealismus.md`) | bestätigt (2026-11-13, Nutzer: „Fotorealistisch“, nach Hinweis auf den Widerspruch zur Art Direction und die höheren Leistungskosten); Gesamtstil offen |
| Vegetation und Tiere | Vorschlag: Pflanzen und Felsen, die im Slice gesammelt werden, bald als Aussehens-Durchgang; Bodendecker und Biome mit Phase 4; Tiere erst nach Festlegung, welche und wozu (Kulisse, Jagd, Gefahr) | offen |
| Wegfindung der Bewohner: Unreal-Navigation | Bewohner nutzen das Unreal-Navigationsnetz (ersetzt die gerade Linie). Das Netz entsteht nur rund um die Bewohner und wird im Spiel laufend neu gebaut, nur auf dem Server. Kerndateien `Gamer.Build.cs` und `DefaultEngine.ini` dafür geändert | bestätigt (2026-11-13, Nutzer: „Ja, Wegfindung“, mit Freigabe der beiden Kerndateien); technische Umsetzung (feste Grenzen 1 km × 1 km, Tausch der Navigation im Spielmodus) = Vorschlag von Claude |
| Eigene Weltkarte und PCG | Das Spiel läuft auf der eigenen Karte `/Game/Gamer/Maps/World` (Kopie der Vorlage, Boden 250 m × 250 m) statt auf der Engine-Vorlage; PCG-Plugin eingeschaltet (Kerndatei `Gamer.uproject`) | bestätigt (2026-11-13, Nutzer: „Ja, freigeben“) |
| Was PCG verteilt | PCG verteilt nur Deko ohne Kollision (Gras, Kiesel, Zweige, später Bodendecker); erntbare Bäume, Sträucher, Felsen und Pflanzen bleiben Vorkommen, die der Server setzt. Erzeugt wird im Editor und mit der Karte gespeichert (auf allen Rechnern gleich); Erzeugung zur Laufzeit um den Spieler herum später für große Welten | Vorschlag von Claude (2026-11-13) |
| Methoden aus den Tutorials | QuickScatter, Ultra Dynamic Sky und die Fab-Pakete aus den Videos sind kostenpflichtig und werden nicht genutzt; die Methoden werden mit PCG und Bordmitteln nachgebaut. Übersicht `docs/08_umgebung_methoden.md` | Vorschlag von Claude (2026-11-13), Prüfung der Links durch Recherche |
| CC0-Dateien, zweite Liste | Bodentexturen (Sand, Sand mit Nadeln, Nadelwaldboden, Moosboden, Erde), Stumpf, liegendes Totholz, Farn, zwei Sträucher, Sandstein-Felswand und der Zweig-Atlas der Poly-Haven-Kiefer, zusammen etwa 121 MB | bestätigt (2026-11-13, Nutzer: „Ja, alle + Zweigtextur“) |
| Bäume verbessern | die eigenen Blender-Bäume verbessern (Waldkiefer höher und schlanker, oben orange Rinde, feinere Nadel- und Blattbüschel aus CC0-Zweigtextur); Megaplants (Fab, experimentell) vorerst nicht | bestätigt (2026-11-13, Nutzer: „Blender-Bäume verbessern“, nach Rückmeldung „die Bäume sehen nicht gut aus“) |
| Jahreszeiten: Optik und Dauer | Gras, Laub und Sträucher passen sich sichtbar der Jahreszeit an (Frühling frisch, Sommer satt, Herbst gelb-braun, Winter strohig bzw. kahl; Nadelbäume bleiben grün). Eine Jahreszeit dauert 1 Stunde Echtzeit (ein Jahr = 4 Stunden), Wert in den Spieldaten, später Weltoption | bestätigt (2026-11-13, Nutzer: „soll sich dynamisch an die Jahreszeit anpassen“, Dauer „1 Stunde“); Farbverlauf und Umsetzung = Vorschlag von Claude |
| Deko darf nicht wie ein Rohstoff aussehen | Was wie ein aufsammelbarer oder erntbarer Gegenstand aussieht (Steine, Zweige, Äste, Beeren-/Haselsträucher, Totholz), wird nicht als reine Deko verteilt, sondern ist nutzbar oder fehlt. Die PCG-Kiesel und -Zweige wurden deshalb am 2026-11-14 entfernt | abgeleitet aus der Nutzer-Rückmeldung (2026-11-14: „Man kann nicht alle Gegenstände, die auf dem Boden liegen, aufheben“); Regel = Vorschlag von Claude |
| Totholz erntbar | Alte Baumstümpfe und liegendes Totholz sind Vorkommen (Axt): Stumpf gibt 1 Baumstamm + 1–3 Äste, Totholz 1–2 Baumstämme + 2–3 Äste, beides kommt nach 4 Spieltagen wieder. Im Startgebiet je 3 Stück. Modelle: Poly Haven `tree_stump_01`, `dead_tree_trunk` (CC0) | bestätigt (2026-11-14, Nutzer: „Ja, erntbar machen“); Erträge, Werkzeug und Anzahl = Vorschlag von Claude |
| Mitwirkende: Sounds | Urheber fremder Sounds werden in den Mitwirkenden (Credits) namentlich genannt, unabhängig davon, ob die Lizenz es verlangt; Quelle und Lizenz jedes Pakets in `design/asset_log.md` | bestätigt (2026-11-14, Nutzer: „namentlich erwähnen bei Mitwirkenden für Sounds“) |
| Waffenarten (Wunsch) | Schwerter, Dolche, Hackmesser, Äxte, Speere, Hämmer, Bögen, Sicheln, Zauberstab. Der Zauberstab setzt Magie voraus | Wunsch des Nutzers (2026-11-14) |
| Magie | Es gibt Magie, nordisch geprägt (Runen, Seidr, Tränke); eigenes System, Umfang und Phase offen | bestätigt (2026-11-14, Nutzer: „Ja, nordische Magie“); Ausgestaltung offen |
| Obst, Saatgut, Feldbau | Äpfel und Birnen an Bäumen mit Jahreszeiten-Zyklus, Fallobst mit Fallschaden und Verderb; Feldfrüchte Zwiebeln, Möhren, Kartoffeln, Flachs; Saatgut aus Früchten, Aussaat, Düngen, Kompost, Regenfänger; Wildpflanzen aus dem Referenzbild. Wird jetzt gebaut (nicht erst Phase 2). Details in `docs/10_nahrung_feldbau.md` | bestätigt (2026-11-14, Nutzer: „Alles jetzt“); historische Passung (Kartoffeln, Melone) offen |
| Umsetzung Jahreszeit | Weltoption `SeasonMinutes` (URL `?SeasonMinutes=60`, Standard 60) statt eigener Datendatei; Zeile im Startmenü folgt. Neue Welten beginnen im späten Frühling (Phase 0,6) | Vorschlag von Claude (2026-11-14) |
| Sprint | Sprinten mit gehaltener Taste (Standard Umschalt links, Gamepad linker Stick drücken), 750 statt 450 cm/s, ohne Ausdauer (Regel 5) | bestätigt (2026-11-14, Nutzer: „Sprint fehlt“); Taste und Tempo = Vorschlag von Claude |
| Automatische Werkzeugwahl | Beim Schlagen nimmt die Figur das beste passende Werkzeug aus der Schnellleiste in die Hand (beste Axt am Baum, Spitzhacke am Fels, Hammer an einem Bauplan). Werkzeuge im Rucksack werden nicht geholt | bestätigt (2026-11-14, Nutzer); „nur aus der Schnellleiste“ = Vorschlag von Claude |
| Schnellleiste mit dem Mausrad | Mausrad runter = nächster Platz, hoch = voriger, läuft rundum, auch über leere Plätze; beides unter „Steuerung“ umbelegbar | bestätigt (2026-11-14, Nutzer); Richtung = Vorschlag von Claude |
| Bauen: Hand leert sich | Nach dem Platzieren ist das Bauteil abgestellt und nicht mehr „in der Hand“ (Vorschau weg); der Hammer bleibt in der Hand, damit man den Bauplan fertig schlagen kann | bestätigt (2026-11-14, Nutzer); dass der Hammer in der Hand bleibt, ist Deutung von Claude |
| Mehrfachbau | Umschalt beim Platzieren halten = Bauteil bleibt gewählt, man kann weitere setzen. Teilt sich die Taste mit Sprint (bewusst, beide getrennt umbelegbar). Angezeigt wird, für wie viele das Material im Inventar noch reicht | bestätigt (2026-11-14, Nutzer: „Shift halten“) |
| Anzeigen Hunger, Schlaf, Stimmung | Für die **Bewohner** als Symbol mit Leiste (Vogelperspektive, Dorfverwaltung); Stimmung kommt als neuer Wert dazu. Der **Spieler** bekommt nur eine „Satt“-Anzeige: satt = Essens-Buffs aktiv, leer = keine Buffs, aber keine Strafe. Regel 5 (Essen bleibt Buff-System) und Regel 6 (kein Zeitdruck) bleiben unverändert | bestätigt (2026-11-14, Nutzer wählte die Empfehlung) |
| Icons für Gegenstände | Jedes Icon (Gegenstand, Werkzeug, Waffe, Rüstung, Deko, Werkstatt, Essen) wird automatisch aus dem echten 3D-Modell gerendert (gleiches Licht, gleicher Winkel); keine KI-Bilder. Ohne Modell vorerst Platzhalter-Icon. Das Angebot von ChatGPT (KI-Bildbibliothek) wird nicht genutzt | bestätigt (2026-11-14, Nutzer wählte die Empfehlung) |
| Herstellungsmenü | dynamischer, mit Bildern (Icons aus der Zeile oben) | bestätigt (2026-11-14, Nutzer); Aufbau offen, Vorschlag im Phasenplan |
| Verderb: Zeitpunkt und Hinweise | Verderb kommt noch in Phase 1 (Slice). Hinweis, wenn ein verderblicher Gegenstand 70 % seiner Haltbarkeit verbraucht hat („noch 30 %“), dazu einmalig der Anfänger-Tipp „Verderbliches trocken und geschützt lagern“. Die Auslegungsfragen aus „Verderben – Auslegung“ bleiben offen | bestätigt (2026-11-14, Nutzer wählte die Empfehlung) |
| Komposter, Regenfänger, Kreislauf Feldbau/Wasser/Dünger | ~~kommen mit dem Feldbau in Phase 2~~ **geändert 2026-11-14: kommen jetzt, zusammen mit dem Feldbau** (Nutzer auf Nachfrage zum Widerspruch: „Ja, alles jetzt“). Vorschläge von ChatGPT: Komposter macht aus Verdorbenem Kompost (= Dünger); primitiver Regenfänger früh baubar aus 1 Baumstamm, 1 großem Stein, 3 kleinen Steinen, 5 Ästen | bestätigt (2026-11-14, Nutzer wählte die Empfehlung); Rezepte = Vorschlag (ChatGPT) |
| Verderben – Auslegung (Antwort) | Trockene, geschützte Lagerung **verlängert** die Haltbarkeit nur, sie hält nicht unbegrenzt. **Auch Gekochtes verdirbt**, aber deutlich langsamer als Rohes, zusätzlich abhängig von der Stufe (T1–T6) des Lebensmittels. Damit gilt „nur Rohes verdirbt“ (2026-11-12) nicht mehr. Offen bleibt: ob die Dauern in Spieltagen gemeint sind und in welche Richtung die Stufe wirkt (Deutung von Claude: höhere Stufe hält länger) | bestätigt (2026-11-14, Nutzer: „nur verlängert, auch gekochtes verdirbt aber deutlich deutlich langsamer, auch abhängig von der T-Stufe des Lebensmittels“); Deutung der Stufen-Richtung offen |
| Verderben – Stufe und Zeiteinheit | Höhere Stufe (T1–T6) hält länger. Alle Verderb-Dauern sind **Spieltage** | bestätigt (2026-11-14, Nutzer: „ja stimmt, Spieltage“) |
| Gesamtstil | Eigene Stilmittel, angelehnt an Aska: realistisch texturiert (Foto-/Scan-Materialien), aber klar lesbare Formen, nordische Stimmung. Gilt für Gebäude, Figuren, Werkzeuge und Vegetation; ersetzt für den Gesamtstil die stilisierte Art Direction „Klarform-Saga“. Fremde Pakete werden an diesen Stil angepasst (Materialien tauschen, Geometrie nacharbeiten) | bestätigt (2026-11-14, Nutzer: „eigene Stilmittel angelehnt an Aska“); konkrete Stilregeln = Design-Aufgabe |
| Heruntergeladene Pakete (`D:\gamer\heruntergeladenes`) | Werden erst nach der Stil-Entscheidung eingebaut. Lizenzen geprüft: Quaternius Medieval Village / Fantasy Props / Universal Animation Library 2 = CC0; Kevin Iglesias Human Basic Motions / Crafting Animations = Standard Asset Store EULA (kommerziell erlaubt, Weiterverkauf nicht); 14 AlkaKrab-Musikpakete = eigene Lizenz (kommerziell in Spielen erlaubt, keine Weitergabe der Rohdateien, kein Remix, Credit „Music by AlkaKrab“); Vegetation.zip = „Vegetation“ von Elbolilloduro (elbolilloduro.itch.io/vegetation, CC0 laut Seite, PSX-/Low-Poly-Stil, geprüft 2026-11-14); HighLandsFantasyBuildings.zip = „Highlands – Fantasy 3D Buildings Pack“ von Visions Mind (visionsmind.itch.io/highlands-fantasy-pack-3d, CC0 laut Seite, Nennung freiwillig, stilisiert, 4k-Texturen, für UE4 mit UCX-Kollision, geprüft 2026-11-14); tripo-share-Bild KI-generiert. Geräuscheffekte (Environment FX) sind nicht dabei | bestätigt (2026-11-14, Nutzer: „nach Stil-Entscheidung“) |
| Charakter: Fähigkeiten | Jeder kann später jede Fähigkeit lernen. Die Herkunft gibt Startwerte, Startausrüstung, eine Startfähigkeit und einen eigenen Zweig, der günstiger ist (Faktor `own_branch_cost_factor` 0,5 in `characters.json`) | bestätigt (2026-11-14, Nutzer wählte die Empfehlung); Faktor = Vorschlag von Claude |
| Charakter: pro Spieler | Ein Charakter gehört dem Spieler und ist in jede Welt mitnehmbar. Bis es ein Speichersystem gibt, liegt er lokal in `Saved/Config/<Plattform>/GameUserSettings.ini` (`[/Script/Gamer.GamerCharacterSettings]`). Ohne Weltspeicherung bekommt der Spieler die Startausrüstung bei jedem Beitritt neu | bestätigt (2026-11-14, Nutzer wählte die Empfehlung); Speicherort und Startausrüstung je Beitritt = Vorschlag von Claude (Übergangslösung) |
| Charakter: Figurenmodell | **MetaHuman** (gegen die Empfehlung „eigenes modulares Modell“). Nachteile, auf die hingewiesen wurde: groß (oft 0,5–1 GB je Figur mit Texturen, Budget 20 GB), teuer bei vielen Bewohnern und knapp für 8 GB VRAM, kein Wikinger-Stil vorgefertigt, der MetaHuman-Editor läuft nicht im Spiel (der Charaktereditor im Spiel muss trotzdem selbst gebaut werden). Bis die Figur da ist, speichert der Editor das Aussehen als Auswahl-IDs (Körperbau, Gesicht, Hautton, Frisur, Bart, Haarfarbe, Bemalung, Kleidungsfarbe, Schildfarbe). Offen: Plugin/Download (braucht Freigabe), ob Bewohner ebenfalls MetaHumans werden, und Verhältnis zur bestätigten Entscheidung „zwei Körperbauten, ein Skelett“ (2026-11-13) | bestätigt (2026-11-14, Nutzer: „MetaHuman“); Umsetzung offen |
| Charakter: Werte und Herkünfte | Fünf Werte (Stärke, Geschick, Zähigkeit, Handwerk, Wille), fünf Herkünfte wie in `docs/11_charaktere.md`, Daten in `game/Content/Data/characters.json`. Summe 25, je Wert **2–8** (nicht 3–8: Krieger und Schmied haben Wille 2). **Seherin geändert** von 2/5/4/4/8 (Summe 23) auf 3/5/5/4/8 (Summe 25) | Vorschlag von Claude (2026-11-14), Korrektur der Widersprüche im Vorschlag; zu bestätigen |
| Charakter: Startausrüstung als Gegenstände | Neue Gegenstände für die Startausrüstung: Kurzbogen, Pfeile, Dolch, Bartaxt, Rundschild, Hackmesser, Runenstab (Waffen, Stufe 1), Schmiedehammer (Werkzeug Hammer, kann auch Baupläne hämmern), Sichel (Werkzeug). Kämpfen kann man damit noch nicht (Kampf = Phase 3) | Vorschlag von Claude (2026-11-14) |
| Charakter: Ablauf im Startmenü | „Welt erstellen“ und „Beitreten“ führen zuerst auf die Seite „Charakter“ (Herkunft, Name, Werte, Startausrüstung, Startfähigkeit, eigener Zweig; „Aussehen bearbeiten“), „Weiter“ speichert und geht zu den Weltoptionen bzw. zur Adresse. Der Server prüft die Wahl und nimmt Werte und Ausrüstung aus seinen eigenen Daten (server-autoritativ) | Vorschlag von Claude (2026-11-14) |
| Charakter: Aussehens-Optionen nach „Gelebter Norden“ | Kleidung in Naturtönen (ungefärbt, Wollgrau, braun), Farbe als Wohlstand (Ocker, Krapprot, Waidblau, selten Moosgrün); neu Wettergerbung (kaum/deutlich/stark), Tätowierung/Bemalung (Runen, Knotenmuster, Ruß, Waid-Streifen), Schmuck (Bronzefibel, Silberarmring, Glasperlenkette); Schild in gedämpften Farben. Grundlage: `design/stil_gelebter_norden.md` (selbst noch Vorschlag) | Vorschlag von Claude (2026-11-14) |
| Verderb: Werte und Orte | Lager (unter Dach) verlängert ×4 (Faktor 0,25), am Boden doppelt so schnell; jede Stufe über T1 +50 % Haltbarkeit; Gekochtes 8 Spieltage, Fallobst mit Druckstellen 1 Spieltag, Haselnüsse und Leinsamen verderben nicht; Verdorbenes wird `item_spoiled_food`. Stapel mischen = Mittelwert | Vorschlag von Claude (2026-11-14), Rahmen bestätigt (Zeilen „Verderben – Auslegung“, „Stufe und Zeiteinheit“); Zahlen offen |
| Obst-Zyklus und Wildpflanzen: Werte | Phasen, Fruchtmengen, Fallhöhen und 2-m-Grenze für Druckstellen wie in `docs/10_nahrung_feldbau.md`, Abschnitt „Umsetzung“. Himbeere bleibt vorerst ganzjährig (Startnahrung) | Vorschlag von Claude (2026-11-14) |
| Feld aufziehen | Zwei Klicks (Ecke, Gegenecke), je Seite 2–12 m; je 4,5 m² eine Einheit für Kosten (2 Äste, 1 Stein), Saatgut, Wasser, Dünger und Ertrag | Wunsch bestätigt (2026-11-14, Nutzer über die Vegetations-Session); Maße, Einheit, Kosten = Vorschlag von Claude |
| Hof und Bauer | Feldarbeit der Bewohner nur mit Hof (`building_farmstead`); Beruf Bauer erntet, gießt, sät, düngt im Umkreis von 30 m und leert Regenfänger/Komposter | Wunsch bestätigt (2026-11-14); Ablauf, Kosten, Umkreis = Vorschlag von Claude |
| Ausbaustufen und Arbeitsplätze | Alle Arbeitsgebäude haben Arbeitsplätze und Ausbaustufen (`workers`, `upgrades` in `buildables.json`); Ausbau per E mit Hammer, Kosten sofort; volle Plätze sperren den Beruf. Holzfäller/Steinmetz 2→3→4, Hof 2→4→6, Lagerfeuer 1→2 | Wunsch bestätigt (2026-11-14); Zahlen und Bedienung = Vorschlag von Claude |
| Erstes Saatgut | aus Wildvorkommen im Startgebiet: Wilde Möhre, Wildzwiebel, Wilder Flachs, verwilderte Kartoffelstaude | Vorschlag von Claude (2026-11-14) |
| Großer Stein | neuer Gegenstand `item_stone_large` für den Regenfänger; kommt aus großen (immer 1) und mittleren Felsen (0–1) | Vorschlag von Claude (2026-11-14) |
| Kartoffeln historisch | Kartoffeln sind im Spiel (ausdrücklicher Wunsch), obwohl sie erst im 16. Jh. nach Europa kamen | offen (Rückfrage an den Nutzer) |
| Kartoffeln | bleiben im Spiel, trotz der historischen Unstimmigkeit (erst ab 16. Jh. in Europa) | bestätigt (2026-11-14, Nutzer: „Ja Kartoffel bleibt“) |
| Himbeere ganzjährig | Die Himbeere („Beeren“) trägt das ganze Jahr, nicht nach Jahreszeit (Startnahrung) | bestätigt (2026-11-14, Nutzer: „Ja ganzjährig lassen“) |
| Obstbäume selbst aufziehen | Apfel- und Birnbäume vom Setzling aufziehen: aus Obst werden Kerne (Handrezept), Kerne im Feld gesät ergeben Setzlinge, den Setzling pflanzt man über das Baumenü; er wächst über Setzling und Jungbaum zum tragenden Baum | bestätigt (2026-11-14, Nutzer: „Vom Setzling aufziehen“); Ablauf über Feld und Baumenü, Mengen und Zeiten = Vorschlag von Claude |
| Umgebungs-Scan | Eigene Taste Z (umbelegbar), Radius 40 m, Markierungen 8 s sichtbar auch durch Wände, Abklingzeit 12 s, keine Ausdauer. Kategorien Nahrung, Holz, Stein, Pflanzen, liegende Gegenstände (später Tiere/Feinde) mit Farbe, Symbol und Anzahl am Bildschirmrand. Herkunft Jäger („Spuren lesen“): 60 m und zeigt Tiere. Werte in JSON | bestätigt (2026-11-15, Nutzer: „So übernehmen“); Werte = Vorschlag des Game Directors |
| Aufsammeln: Hervorhebung | Das anvisierte Objekt bekommt eine Umrandung (Outline). Liegen mehrere Dinge auf einem Haufen, wechselt man gezielt zwischen ihnen (kleine Liste am Fadenkreuz) und nimmt das Gewünschte mit E | Umrandung bestätigt (2026-11-15, Nutzer: „das Hervorheben soll durch eine Umrandung stattfinden“); Wechseltaste F (umbelegbar) = Deutung von Claude, Nutzer hat keine Taste genannt |
| Stationsfenster | Mit E an einer fertigen Station (Werkbank, Lagerfeuer, später Schmiede) öffnet sich ein Stationsfenster mit den Rezepten dieser Station; Tab bleibt für Handwerk und Stationen in der Nähe | Wunsch des Nutzers (2026-11-14, laut Game Director: Station über Tab nicht auffindbar); Umsetzung = Vorschlag von Claude |
| Einzelspieler | Jedes Feature wird auch mit einem einzelnen Spieler getestet (Solo = Koop mit 1 Spieler) | bestätigt (2026-11-14, Nutzer, über den Game Director) |
| Taverne und Gerichte T1–T6 | Gerichteliste aus `docs/12_taverne_gerichte.md` übernommen (historisch nordische Küche, je Stufe 2–5 Gerichte). Gerichte mit Fleisch, Fisch und Milch kommen erst mit diesen Zutaten | bestätigt (2026-11-15, Nutzer: „übernehmen“) |
| Gerste, Hafer, Honig | kommen jetzt ins Spiel: Gerste und Hafer als Wildpflanze und Feldfrucht, Honig aus dem Wildbienennest | bestätigt (2026-11-15, Nutzer: „ja mit einfügen“); Mengen und Zeiten = Vorschlag von Claude |
| Kochstellen | eigene Bauteile (Herdstelle, Lehmbackofen, Braukessel, Festtafel), nur im Umkreis von 15 m einer Taverne der nötigen Stufe baubar; Lagerfeuer bleibt T1-Kochstelle überall | bestätigt (2026-11-15, Nutzer wählte die Empfehlung); Umkreis und Stufen = Vorschlag von Claude |
| Essens-Buffs: Regeln | Katalog Gesundheit, Regeneration, Traglast, Kälte-/Giftresistenz, Schaden; höchstens 3 Mahlzeiten gleichzeitig, gleiches Gericht frischt nur auf, neues ersetzt das mit der kürzesten Restzeit; Dauer und Stärke steigen mit der Stufe (T1 10 min … T6 40 min); Essen per Linksklick mit dem Gericht in der Hand | Rahmen bestätigt (2026-11-15, mit der Liste übernommen); Zahlen und Bedienung = Vorschlag von Claude |
| Koch und Wirt | Der Koch arbeitet an der Taverne (nicht mehr am Lagerfeuer) und kocht an den Kochstellen im Umkreis; neuer Beruf Wirt bringt Gerichte und Getränke in die Taverne; Bewohner essen dort und gewinnen Stimmung. Arbeitsplätze je Beruf 1 → 2 → 3 | Vorschlag von Claude (2026-11-15); weicht vom Vorschlag „Wirt 1/1/2“ ab (Mechanismus kennt eine Zahl je Gebäude) |
| Alles aufnehmen | E halten (0,5 s) nimmt alles Aufhebbare im Umkreis von 3 m auf; Umschalt + E nimmt alles vom Typ des anvisierten Dings (ebenfalls 3 m). Server entscheidet; was nicht mehr ins Inventar passt, bleibt liegen, mit Meldung. Umbelegbar (Aktion „Alles gleicher Art“) | Nutzerwunsch (2026-11-15, über den Game Director: „alle Gegenstände gleichzeitig aufnehmen, und alle nur vom selben Typ“); Tasten und 3 m = Vorschlag des Game Directors |
| Stimmung der Bewohner | Abgeleiteter Wert 0..1 (kein viertes Bedürfnis, Regel 7 bleibt): Hunger 40 %, Schlaf 30 %, Dach 30 % plus Bonus durch Taverne-Mahlzeiten (klingt in einigen Spieltagen ab, höchstens +0,4). Anzeige als Symbol mit Leiste in der Dorfverwaltung und im Info-Feld der Vogelperspektive | Vorschlag von Claude (2026-11-15), Gewichte zum Ausprobieren |
| Spieler-Anzeige „Satt“ | Links über der Schnellleiste: Symbol mit Leiste (Restzeit der längsten Mahlzeit) und die aktiven Mahlzeiten mit Restzeit. Keine Strafe bei „nicht satt“ | umgesetzt (2026-11-15) nach Entscheidung vom 2026-11-14 |
| Speichersystem: Format und Ort | JSON-Dateien (lesbar, robust gegen fehlende/neue Felder und unbekannte IDs), Schema-Version mit Migrationsstufen; pro Welt `Saved/SaveGames/Worlds/<welt_id>/world.json` + `meta.json` (für die Liste) + `world.bak.json` (vorige Fassung). Schreiben über eine Zwischendatei; unlesbare Dateien werden nie überschrieben, sondern beiseitegelegt. Unbekannte IDs werden beim Laden mit Warnung weggelassen | Vorschlag von Claude (2026-11-15) ⚑ |
| Speichersystem: Zeitpunkte | Autosave alle **5 Minuten** (`AutosaveMinutes`, Test `-GamerAutosaveSeconds=N`), beim Verlassen der Welt (Host → Hauptmenü), beim Beenden, beim Herunterfahren des Servers, manuell „Speichern“ im Esc-Menü (Listen-Server: nur der Gastgeber; Dedicated Server: jeder Spieler, höchstens alle 10 s) | Vorschlag von Claude (2026-11-15) ⚑ (Intervall) |
| Speichersystem: welche Welten | Gespeichert wird nur eine Welt mit Welt-ID: Startmenü setzt sie immer; Dedicated Server mit `-GamerWorld=<id>` (und `-GamerWorldName=`). Ohne ID wird nichts gespeichert, damit Tests (mp_test) keine Spielstände anlegen oder verändern | Vorschlag von Claude (2026-11-15) ⚑ |
| Speichersystem: Spieler-ID | Jeder Spieler hat eine lokale Kennung (zufällige GUID in `GameUserSettings.ini`, Test `-GamerPlayerId=`), die der Client nach dem Beitritt schickt; der Server gibt bekannten Spielern Position, Inventar, Schnellleiste und aktive Mahlzeiten zurück und keine zweite Startausrüstung. Später Steam-/Online-ID. Nachteil: Ein Mitspieler könnte eine fremde Kennung vortäuschen (für Koop unter Freunden hinnehmbar); zwei Spiele auf einem PC teilen sich die Kennung (der zweite bekommt „-2“) | Vorschlag von Claude (2026-11-15) ⚑ |
| Speichersystem: Weltname | Neue Welten heißen automatisch „Welt 1“, „Welt 2“ … (kein Textfeld im Slice); Umbenennen später | Vorschlag von Claude (2026-11-15) ⚑ |
| Speichersystem: Inventar pro Welt oder mit dem Charakter | Umgesetzt ist **Inventar pro Welt** (wie im Auftrag). Das weicht von „Charakter pro Spieler, in jede Welt mitnehmbar“ (2026-11-14) ab, falls dort auch die Ausrüstung gemeint war; Nachteil der Alternative: Ausrüstung wandert zwischen Welten. Charakter (Herkunft, Name, Aussehen) bleibt weltunabhängig in `GameUserSettings.ini`; eine Charakterdatei `Saved/SaveGames/Characters/<id>.json` ist vorbereitet abgesprochen, nicht gebaut | **offen, Entscheidung des Nutzers** |
| Speichersystem: was nicht gespeichert wird | Laufende Aufgaben der Bewohner (sie planen nach dem Laden neu; Getragenes inkl. Saat/Gerichte unterwegs kommt in ihr Gepäck), Sammel-Warteschlange, Scan, Fokus, Sprint, offene Fenster, Arbeitsbalken | Vorschlag von Claude (2026-11-15), abgestimmt mit Gameplay- und Taverne-Session |
| Inventar pro Welt oder mit dem Charakter (Antwort) | Wird **beim Erstellen einer Welt abgefragt** (Weltoption „Inventar“): „Bleibt in dieser Welt“ (Standard, im Weltspielstand) oder „Wandert mit dem Charakter“ (in der Charakterdatei des Spielers `Saved/SaveGames/Characters/<spieler_id>.json` auf dessen eigenem PC; in jeder Welt mit dieser Option bringt er es mit). Position und Mahlzeiten bleiben immer pro Welt. Nachteil der zweiten Variante: Der Spieler könnte die Datei bei sich bearbeiten (der Server prüft nur, dass die Gegenstände existieren und die Stapelgrößen passen) | bestätigt (2026-11-15, Nutzer: „bei der Neuerstellung abfragen“); Deutung als Weltoption, Standard und Ablage = Vorschlag von Claude |
| MetaHuman: Weg | MetaHuman auf dem Unreal-Standardskelett, Animationen per Retargeting; Plugin `MetaHumanCharacter` eingeschaltet. Bis die MetaHuman-Figur steht: Epic Manny (kräftig) / Quinn (schmal) als Zwischenfigur | bestätigt (2026-11-15, Nutzer: „dann mach das so“, Plugin „Ja, einschalten“, Zwischenfigur „Ja“) |
| Animationen Kevin Iglesias | werden voll genutzt, obwohl die Lizenz für Unreal nicht schriftlich belegt ist (Seite: Lizenzabschnitt leer) | Nutzer-Entscheidung (2026-11-15: „Kevin voll nutzen“); Klärung per support@keviniglesias.com offen |
| Animation ohne AnimBlueprint | Posen werden in C++ gemischt (`UGamerAnimInstance` mit eigenem Proxy), Arbeitsaktionen als replizierter Impuls | Vorschlag von Claude (2026-11-15), folgt Architekturregel 4 |
| Hervorhebung: echter Umriss | Das anvisierte Ding bekommt eine Kontur, die seiner Form folgt (Silhouette in warmem Bernstein), kein Rechteck mehr. Umsetzung: Custom-Stencil nur am eigenen Ziel (lokal, nichts wird übertragen) plus Post-Process-Material `M_Outline` an der Kamera; `r.CustomDepth` wird zur Laufzeit auf 3 gesetzt (keine Änderung an `DefaultEngine.ini`). Bei Haufen nur das mit F gewählte Ding. Das HUD-Rechteck bleibt nur als Rückfall, wenn das Material fehlt | bestätigt (2026-11-15, Nutzer: „der Umriss des anvisierten Gegenstandes soll die Hervorhebung sein“); Farbe und Technik = Vorschlag von Claude |
| Scan: Visualisierung | Eine Welle (Ringe am Boden) läuft in 1 s bis zum Radius, die Markierungen erscheinen, wenn die Welle sie erreicht | Nutzerwunsch (2026-11-15: „die Visualisierung für den Scan fehlt“, gedeutet als fehlende Wirkung/Welle, nachdem der Zeichenblock durch einen gleichzeitigen Schreibvorgang verloren gegangen war); Details = Vorschlag von Claude |
| Animationen Sammeln und Herstellen | Beim Aufheben vom Boden soll sich die Figur hinhocken und nach dem Material greifen (nicht nur eine Greifbewegung nach vorn). Das Herstellen der Anfangsgegenstände (Hammer, Axt und ähnliche) braucht eine andere Bewegung als sonstiges Herstellen | Nutzerwunsch (2026-11-15), noch nicht umgesetzt (Animation, Figuren-Session) |
| Stil: Mischung „Gelebter Norden“ + „Küstenasche“ | `design/stil_norden_gezeitenhall.md`: Grundlage bleibt „Gelebter Norden“ (Alltag glaubwürdig, Jahreszeiten, Spuren des Lebens); übernommen aus „Küstenasche“: Magie „Gezeitenhall“ (Seeglas, gebrochene Ringe, Rhythmus sammeln–ausbreiten–abklingen), Zeichen „offener Ring mit drei Kerben“, Kreaturen, feste Grundpalette mit Farbwerten (Jahreszeit verschiebt sie), Kleidungsfarben des Editors auf die Palette gelegt, Türkis nur für Magie | Nutzerwunsch Mischung (2026-11-15: „erstelle eine passende Mischung“); Inhalt = Vorschlag von Claude, nicht bestätigt |
| Stil: Mischung festgelegt | `design/stil_norden_gezeitenhall.md` ist die Stilvorgabe (ersetzt `design/stil_gelebter_norden.md` und `design/inbox/08_stilbibel_kuestenasche.md` als Vorgabe) | bestätigt (2026-11-15, Nutzer: „Ja, festlegen“) |
| Stil-Umsetzung im HUD (Umriss, Scan, Kacheln, Rahmen) | Umriss in Herdfeuer `#DB9345`, leicht aufgehellt; Gezeitenlicht `#A9E9DC` nur für magische Ziele, sobald es solche gibt. Kacheln und Icons nach der Palette (Platzhalterrahmen aus der Palette, Icons mit kühlem Nebelgrau-Licht neu gerendert). Rahmen nur Stufe A (Palettenfarben, schlichte Kerben, keine Texturen); über Stufe B (geschnitzte Holz-/Lederrahmen als Texturen) entscheidet der Nutzer später | bestätigt (2026-11-15, Nutzer, über den Game Director); Reihenfolge und Details = Vorschlag von Claude |
| Scan: wer was sieht | Der Scan ist für alle Spieler ein normales Können (Ringe als offener Ring mit drei Kerben, rotierend, auslaufend, in Alltagsfarben Treibholz/Nebelgrau). Magische Dinge zeigt der Scan nur einer bestimmten Klasse (vermutlich Seherin), bei ihr türkis in Gezeitenlicht. Umsetzung: Haken `requires` an einer Scan-Kategorie in `scan.json`; magische Ziele gibt es noch nicht. Die Unterscheidung Solo/Koop wird nicht gebaut (Solo ist Koop mit 1 Spieler, Regel 2) | Wortlaut des Nutzers: „singelplayer normales können multiplayer-Koop scenn kann jeder aber nur -{Klasse}- kann magie sehen“; **Deutung, Klasse offen** (nicht vom Nutzer bestätigt) |
| Scan: Sicht auf Magie | Stufen der Magie-Sicht (Daten in `scan.json` „magic“), gültig für Solo und Koop (Regel 2): **1 Gespür**: jeder Spieler sieht magische Ziele im Scan von Anfang an als schwaches türkises Flimmern, ohne Art und Namen (Option A); **2 Art erkennbar**; **3 volle Sicht**: Name, größere Reichweite (60 m statt 40 m) und auch Verstecktes. Die Stufe steigt von Gespür an mit einem Erfahrungszähler pro Spieler (Schwellen 0 / 5 / 15, Server, repliziert, gespeichert; wie Erfahrung entsteht, kommt mit den magischen Objekten) (Option G). Direkt auf volle Sicht heben: **A** Herkunft Seherin (`origin_seer`, „Seidr-Blick“; andere können ihn später lernen), **C** Seeglas-Amulett (Seherin startet damit, andere finden oder stellen es her), **F** Dorf-Seherin (Bewohner-Beruf mit eigenem Haus: solange beides im Dorf existiert, sehen alle Spieler Magie voll), **D** später: zu besonderen Zeiten und an besonderen Orten (Nacht, Nebel, Steinkreise, Winter) zeigt sich Magie für alle voll. Eigene Sache, keine Scan-Stufe: ein magisches Ziel kann in der Welt selbst flimmern, wenn es im Sichtfeld und nah ist (Haken, Effekt kommt mit den magischen Objekten). Jetzt gebaut: nur das Sichtbarkeitsmodell mit Zähler und Schwellen; Amulett, Beruf Seherin, D und echte magische Objekte folgen mit der Magie-Phase. **D braucht Tag/Nacht und Wetter** (nur „Winter“ ginge schon über die Jahreszeit) | bestätigt (Nutzer 2026-11-15: „dein Vorschlag ist gut, ich finde dazu noch D interessant“); G („wachsendes Gespür“, Zusatz „G*“) = Deutung des Game Directors; korrigiert am 2026-11-15 auf Anweisung des Game Directors: die zwischenzeitliche „Stufe 0, Scan zeigt Magie nicht“ (Deutung eines Nutzerwunsches) widerspricht Option A und entfällt, der Start ist Gespür für alle |
| Scan: Darstellung (Nutzerkorrektur) | Die Scan-Welle ist **wellenartig, nicht kerbenartig**: eine weiche, transparente Farbwelle über dem Boden (Treibholz-Ton), die nach außen läuft und ausläuft, dezent, aber sichtbar. Das ersetzt die „offenen Ringe mit drei Kerben“ vom selben Tag. **Magie im Scan** erscheint als **Silhouette** des Gegenstands (türkise Kontur mit sanftem Glühen, kein Quadrat). Sie ist **anfangs schwach** und wird mit der Benutzung für den einzelnen Charakter **immer stärker** (Deckkraft und Kontur-Dicke wachsen mit der Magie-Erfahrung von 0,25 auf 1 bei Erfahrung 30; Werte in `scan.json` „strength“). Das gilt **auch für die Seherin** und alle mit voller Sicht: sie sehen früher mehr Information (Art, Name), aber die Darstellung ist bei ihnen anfangs ebenfalls dezent. Erfahrung entsteht durch Scannen, das Magisches aufdeckt (+1 je Scan, der Server prüft selbst) | Nutzerwunsch (2026-11-15, Wortlaut: „nicht kerbenartig, sondern wellenartig … Farbe, Welle … eher transparent … bei der Magie mehr um die Silhouette … anfangs eher weniger sehen … auch für die Seherin … durch mehrfache Benutzung beziehungsweise durch das Aufdecken immer ein Stück stärker“); Werte 0,25 / 30 und „+1 je aufdeckendem Scan“ = Vorschlag von Claude |
| Umschauen und Lauschen getrennt | Material und Magie sind zwei getrennte Sinne. **Umschauen** (Taste Z, der bisherige Scan): nur Material (Holz, Stein, Pflanzen, Nahrung, Gegenstände), keine Stufen, kein Fortschritt, keine Magie; die Welle ist eine Sinuskurve (wellige Linie in Treibholz und Nebelgrau), 40 m, Ausbreitung ca. 5 s, Funde ca. 20 s sichtbar (Werte in `scan.json`). **Lauschen** (eigene umbelegbare Taste, Standard Q): Magie (Gezeitenhall). Passiv: ist ein magisches Ziel in Reichweite, schimmert der Bildschirmrand türkis (heiß/kalt, je näher desto stärker), dazu ein Audio-Haken fürs Summen. Aktiv: die Figur bleibt stehen, ein langsamer türkiser Sinus-Ring (offener Ring mit drei Kerben) breitet sich aus, Bewegung bricht ihn ab, eigene Abklingzeit. Das schreibt „Scan: Sicht auf Magie“ und „Scan: Darstellung (Nutzerkorrektur)“ fort (die Magie gehört jetzt zum Lauschen); die alten Zeilen bleiben stehen. Details: `docs/13_magie_gespuer.md` | bestätigt (Nutzer 2026-11-15: „Das kann so angepasst werden“; die Freigabe zum Umbau hat der Game Director aus den Antworten des Nutzers gedeutet) |
| Magie-Gespür: Stufen und Boni | Lauschen hat 5 Stufen, die mit der Erfahrung (EP) steigen: 1 Gespür (0 EP, 10 m, Welle 5 s, Funde 15 s, Flimmern), 2 Ahnung (10 EP, 15 m, 7,5 s, 20 s, Richtung und grobe Art), 3 Deutung (30 EP, 20 m, 10 s, 25 s, Name und Menge), 4 Klarblick (75 EP, 25 m, 12,5 s, 30 s, Versteckte Funde, Markieren), 5 Seidr-Blick (160 EP, 30 m, 15 s, 40 s, Seltene Funde, kürzere Abklingzeit); die Ausbreitung läuft mit 2 m/s, die Werte stehen einzeln in den Daten, die Reichweite gilt auch fürs passive Signal. EP-Quellen (nur Daten, die Objekte fehlen noch): neuer Fund +2, Seeglas ernten +1, Steinkreis einstimmen +8 (einmal pro Kreis), Gezeitenhall-Ereignis +15. **Boni statt Sofort-Maximum** (die Liste `full_if` entfällt): Herkunft Seherin startet auf Stufe 2; Seeglas-Amulett +1 Stufe, solange man es trägt; Dorf-Seherin +25 % EP für alle, solange sie freigeschaltet ist und im Dorf lebt (Freischaltung durch eine **Quest**, Bedingung `unlock:quest_village_seer` + `job:job_seer` vorgemerkt, noch nicht geprüft, Quests gibt es nicht); D (Nacht, Nebel, Steinkreis, Winter) +1 Stufe, solange die Bedingung gilt (geprüft wird bisher nur Winter; **D braucht Tag/Nacht und Wetter**); zusammen höchstens Stufe 5. Koop: gemeinsamer Fund gibt Mitspielern im Umkreis von 30 m 50 % der EP; gemeinsames Lauschen (zwei oder mehr Spieler innerhalb von 10 s und 15 m): jeder sieht mindestens die Stufe des Besten minus 1, Reichweite und Dauer +25 %; Markieren ab Stufe 4 für alle sichtbar (vorgemerkt); eigene Markierungen sollen auch solo gespeichert bleiben (vorbereitet). Solo ist der Maßstab, derselbe Code für Solo und Koop (Regel 2), keine Sonderregel. **Weltoption „Gespür-Tempo“**: langsam ×0,6, normal ×1, schnell ×1,6 als EP-Faktor, gespeichert mit der Welt, als Zeile unter „Welt erstellen“. Zieltempo Solo (Annahme): Stufe 3 nach ca. 3–4 h, Stufe 5 nach ca. 25–30 h. Der Erfahrungszähler pro Spieler (Server, repliziert, gespeichert) ist der bisherige | bestätigt (Nutzer 2026-11-15, über den Game Director); Zahlen = Startwerte zum Justieren, Zieltempo = Annahme |
| Saga: Story-Richtung | Kern der Story ist „Die Spur der Ersten“: Eine frühere Siedlergruppe ist verschwunden, man folgt ihren Spuren (Logbuchseiten, verlassene Höfe, Funde, die man durch Lauschen findet). Die alten Steinkreise mit ihren Liedern sind Orte auf diesem Weg. Die Siedlungs-Chronik läuft nur klein und automatisch im Saga-Buch mit. Ob die Story Hauptfokus wird oder roter Faden bleibt, entscheidet eine Textprobe (Prolog). Details `docs/14_saga_quests.md` | bestätigt (2026-11-15, Nutzer wählte die Empfehlung); Vorgabe des Nutzers: Hauptfokus nur, wenn die Story wirklich fesselt |
| Saga: Queststand pro Spieler, alles spawnt nach | Jeder Spieler hat seinen eigenen Queststand; ein erfülltes Ziel zählt für alle Mitspieler mit derselben aktiven Etappe in der Nähe; Questgegenstände sieht jeder, der sie noch braucht; Questgegner und Bosse spawnen nach; was die Welt verändert (z. B. Seherhütte), gibt es nur einmal und wird Späteren angerechnet | Vorschlag von Claude (2026-11-15), Deutung von „Es kann alles nachspawnen“; Nutzer hat bei der Nachfrage keine Vorliebe angegeben, die Antworten zu Bossbeute und Startwerten bauen darauf auf |
| Saga: Bossbeute bei Wiederholung | Erster Sieg (pro Spieler): volle Beute mit Bossmaterial. Jeder weitere Sieg: normale Beute, Bossmaterial nur mit einer Wahrscheinlichkeit je Stufe | bestätigt (2026-11-15, Nutzer: „beim ersten besiegen normaler lot dann kann prozentual t basierender lot droppen“; Deutung „Chance je Stufe“ von Claude) |
| Saga: Startwerte | Gutschrift im Umkreis von 50 m; Questgegner nach 1 Spieltag, Bosse nach 3 Spieltagen wieder da; Bossmaterial bei Wiederholung T2 50 %, T3 40 %, T4 30 %, T5 20 %, T6 10 %, nach 4 Fehlversuchen sicher | bestätigt als Startwerte (2026-11-15, Nutzer: „Passt als Startwert“) |
| Horden in der Story | Horden bleiben eine Weltoption, in den Einstellungen wählbar; keine Story-Etappe setzt Horden voraus. Voreinstellung (an oder aus) nicht neu entschieden, bleibt wie bisher | bestätigt (2026-11-15, Nutzer: „Ist optional einstellbar. in den einstellungen“); „Einstellungen“ von Claude als Weltoptionen beim Erstellen gedeutet |
| Questhilfe | Das Saga-Buch zeigt das Ziel als Gebiet; wer im Gespräch die passende Frage wählt, bekommt eine Hilfe (genauerer Hinweis). Diese Hilfen lassen sich im Menü ausschalten, mit einem erklärenden Hinweis | bestätigt (2026-11-15, Nutzer: „bei auswahl der passenden dialigen, kann es hilfe geben. Kann aber mit hinweis ausgeschaltet werden im menü“); Deutung von Claude |
| Gefallene Bewohner: Saga | Eintrag im Saga-Buch immer automatisch (Name, wie, wann); Gedenkstein zusätzlich freiwillig baubar, verweist auf den Eintrag. Präzisiert „Saga-Eintrag oder Gedenkstein“ (2026-11-13) | bestätigt (2026-11-15, Nutzer wählte die Empfehlung) |
| Gespräche: Entscheidungen | Gespräche haben von Anfang an Entscheidungen, die den Verlauf ändern. Bei eigenem Queststand entscheidet jeder Spieler für sich; Entscheidungen, die die Welt verändern, gelten für alle (Regel, wer dann entscheidet: offen) | bestätigt (2026-11-15, Nutzer: „Entscheidungen von Anfang an“, gegen die Empfehlung „erst nur Hinweise“) |
| Magie-Erfahrung: einmal pro Fund | Erfahrung (EP) gibt es **pro Fund und Spieler nur einmal**: neuer Fund aufgedeckt +2, Seeglas ernten +1, Steinkreis einstimmen +8 (einmal pro Kreis), Gezeitenhall-Ereignis +15. Der Server merkt sich pro Spieler, welche Funde schon gezählt sind; die Liste wird mit dem Spieler gespeichert. So kann man nicht neben einem einzigen Ziel endlos lauschen und EP sammeln. Das ersetzt den Vorschlag „+1 EP je Scan, der Magisches aufdeckt“ aus der Zeile „Scan: Darstellung (Nutzerkorrektur)“, der nie bestätigt wurde. Die Stufen-Schwellen sind 0 / 10 / 30 / 75 / 160 (ersetzen 0 / 5 / 15). Für Tests setzt ein Testschritt EP direkt (`magicevent:<Quelle>[\|<Fund-ID>]`) | bestätigt (Game Director 2026-11-15, aus dem Gesamtauftrag und der Korrektur dazu); Details in `docs/13_magie_gespuer.md` |
| Gespräche: Gewicht der Entscheidungen | Entscheidungen in der Hauptstory haben große Folgen (andere Kapitel, andere Verbündete); in Nebensträngen mäßige Folgen (Belohnung, Ton, kleine Abweichungen). Die Story soll noch immersiver werden als die erste Probe | bestätigt (2026-11-15, Nutzer: „haupt story größere auswirklungen neben story eher mäßig aber gerne noch inversiver“, „inversiver“ von Claude als „immersiver“ gedeutet) |
| Vertonung | Alles wird vertont: Gespräche, Logbücher, Saga-Einträge, Gedanken der Figur. Ersetzt „Sprache und Dialoge“ (2026-11-13). Stimmen: zuerst KI-Stimmen zum Entwickeln und Testen, wichtige Rollen später mit echten Sprechern neu aufnehmen (KI-Dienst und Lizenz vorher prüfen, KI-Stimmen ins Asset-Log, Steam-Angabe). Sprachen: Deutsch und Englisch. Offen: Lippenbewegung (MetaHuman aus Audio), Stimmen der Spielerfigur (Auswahl im Editor?), ob Nebelbilder Worte bekommen | bestätigt (2026-11-15, Nutzer: „es soll alles vertont werden“, auf Nachfrage „Ja, ersetzen“, „KI-Stimmen erst, Sprecher später“, „Deutsch und Englisch“); was „alles“ umfasst, ist Deutung von Claude |
| Hauptstory: große Entscheidung im Prolog | Am leeren Stein: Hallbjörns Ring nehmen (Weg des Kreises: Magie, Steinkreise) oder seinen Namen ritzen (Weg des Hofs: Asgerds Überlebende, später Bewohner). Die Wege laufen 2–3 Kapitel getrennt und treffen sich wieder. Text: `docs/15_probe_spur_der_ersten.md` | bestätigt (2026-11-15, Nutzer: „Ja, passt“) |
| Koop: Entscheidungen, die die Welt verändern | Jeder Spieler entscheidet für sich; Folgen für die Welt addieren sich (in einer Welt können beide Wege stattfinden), keine Abstimmung, kein Vorrecht des Gastgebers. Setzt einen Queststand pro Spieler voraus (siehe „Saga: Queststand pro Spieler“) | bestätigt (2026-11-15, Nutzer wählte die Empfehlung) |
| Story: Mittel für Immersion | Nebelbilder (Seidr-Visionen beim Lauschen), einzelne Gedanken der Figur (abschaltbar), Logbuchseiten in Handschrift mit lesbarem Drucktext daneben, Klang statt Musik, alles vertont. Gilt für die ganze Hauptstory; Ob die Story Hauptfokus ist: nach der Probe ja, sie gilt als „fesselnd genug“ | bestätigt (2026-11-15, Nutzer: „Ja, immersiv genug“); dass die Story damit Hauptfokus sein darf, ist Deutung von Claude aus der Vorgabe „Hauptfokus nur, wenn fesselnd“ |
| Queststand und Weltoption „Inventar wandert mit“ | Der Queststand (Etappen, Entscheidungen, Wege, Schriftstücke, persönliche Taten) bleibt immer pro Welt, auch wenn das Inventar mit dem Charakter wandert | bestätigt (2026-11-15, Nutzer: „Gena pro weld“, von Claude als „Genau, pro Welt“ gedeutet) |
| Saga: Queststand pro Spieler (Bestätigung) | Bestätigt die Zeile „Saga: Queststand pro Spieler, alles spawnt nach“ (vorher Vorschlag): eigener Queststand je Spieler, gespeichert pro Welt; Gutschrift für Mitspieler in der Nähe; was die Welt verändert, passiert nur einmal | bestätigt (2026-11-15, Nutzer: „Ja, bestätigt“) |
| Vertonung: Namen | Frei gewählte Namen (Bewohner, Spieler) werden nicht gesprochen: Der Erzähler umschreibt („eine der Euren“, „sie“), der Name steht im Untertitel und im Saga-Buch | bestätigt (2026-11-15, Nutzer wählte die Empfehlung) |
| Vertonung: Stimme der Spielerfigur | Im Charakter-Editor 4 Stimmen zur Wahl (je 2 tief und hell), unabhängig vom Körperbau; jeder Gedanke der Figur wird 4-mal aufgenommen (je Sprache) | bestätigt (2026-11-15, Nutzer wählte die Empfehlung); Anzahl 4 = Vorschlag von Claude |
| Vertonung: Nebelbilder | Die Gestalten sprechen kurze, halb verwehte Satzfetzen mit Hall, mit Untertitel | bestätigt (2026-11-15, Nutzer wählte die Empfehlung) |
| Vertonung: Lippen | Lippen- und Gesichtsbewegung werden mit MetaHuman aus der Audiodatei erzeugt, einmal beim Aufnehmen (auch für KI-Stimmen) | bestätigt (2026-11-15, Nutzer wählte die Empfehlung); Werkzeug im Detail noch zu prüfen |
| Finale der Hauptstory | Große Entscheidung am Ende: den Ring der Steinkreise schließen (Kraft gezähmt, Magie leiser) oder offen lassen (Kraft bleibt wild); zwei Enden. Im Koop entscheidet jeder für sich; da sich zwei Enden nicht addieren lassen, zeigt die Welt danach das zuerst gewählte Ende | bestätigt (2026-11-15, Nutzer wählte die Empfehlung); Koop-Regel „zuerst gewähltes Ende gilt für die Welt“ = Vorschlag von Claude, Ausnahme von „Folgen addieren sich“ |
| Saga-Buch: Taste | J, unter „Steuerung“ umbelegbar | bestätigt (2026-11-15, Nutzer wählte die Empfehlung) |
| MetaHuman-Projekteinstellungen versioniert | `r.GPUSkin.Support16BitBoneIndex`, `r.GPUSkin.UnlimitedBoneInfluences`, `r.SkinCache.CompileShaders` stehen zusätzlich in `game/Config/Windows/WindowsEngine.ini` (ohne Token, kommt ins Repo), weil `DefaultEngine.ini` nie committet wird | bestätigt (2026-11-16, Nutzer: „Ja, eintragen“) |
| Tag und Nacht | Ein voller Tag dauert 30 Minuten (Nacht ca. 10 Minuten), also 2 Tage je Jahreszeit-Stunde; passt zum bisherigen Spieltag (30 min) der Bewohner und des Nachwachsens. Nacht dunkel, aber spielbar (Mondlicht, Umrisse sichtbar; Feuer und Fackeln machen einen Unterschied, Stilregel „warm gegen kalt“) | bestätigt (2026-11-16, Nutzer wählte die Empfehlungen) |
| Wetter | Regen und Gewitter, Nebel, Schnee und Frost, Sturm und Wind. Leichte Wirkung: Regen wässert Felder und füllt Regenfänger, Nebel verbessert das Lauschen (Option D), Schnee/Frost machen kalt (Kälteresistenz der Gerichte), Sturm vorerst nur Optik; keine harten Strafen. Häufigkeit je Jahreszeit = Vorschlag | bestätigt (2026-11-16, Nutzer); Häufigkeiten und Zahlen offen |
| Spielertod | Weltoption beim Erstellen der Welt: nichts verlieren / Grab mit Inventar am Todesort (Ausrüstung bleibt am Körper) / alles verlieren (Hardcore). Wiedereinstieg am eigenen Bett, sonst am Startpunkt. Voreinstellung = Vorschlag: Grab mit Inventar | bestätigt (2026-11-16, Nutzer: „Als Weltoption“); Voreinstellung und Wiedereinstieg = Vorschlag von Claude |
| Heilung | Lebenspunkte füllen sich langsam von selbst, solange man satt ist; Gerichte mit Regenerations-Buff beschleunigen das (nutzt das vorhandene Buff-System) | bestätigt (2026-11-16, Nutzer wählte die Empfehlung) |
| Tiere: erste Arten | Friedliche Jagdtiere (Hirsch, Reh, Hase), wehrhafte Tiere (Wildschwein, Elch), Raubtiere (Wolf, Bär, Luchs, vor allem nachts aktiv), Nutztiere (Hühner, Ziegen, Schafe) | bestätigt (2026-11-16, Nutzer wählte alle vier Gruppen); konkrete Arten = Vorschlag von Claude |
| Jagd vor dem Kampfsystem | Tiere mit Verhalten und Lebenspunkten kommen jetzt, mit einfachem Angriff (Speer, Bogen) als Vorstufe; das richtige Kampfsystem (Phase 3) ersetzt ihn später | bestätigt (2026-11-16, Nutzer wählte die Empfehlung) |
| Ausdauer | Es gibt Ausdauer (kurzfristig, z. B. Sprinten, Schlagen, Springen). Leere Ausdauer sperrt nichts: man wird nur langsamer und schlägt schwächer (Regel 5 „keine Ausdauer-Sperren“ bleibt). Verbessert wird sie über Essen, Trinken und Ausrüstung (mehr Maximum, schnellere Erholung; neue Buff-Arten). Dazu langfristige Erschöpfung: sinkt über den Tag, senkt die maximale Ausdauer und das Arbeitstempo, steigt mit Schlaf und Essen | bestätigt (2026-11-16, Nutzer: „Verbesserungs System mit essen trinken und Ausrüstung“, „Nur langsamer“, Erschöpfung „Ja“); Zahlen = Vorschlag |
| Durst (Spieler) | Echter Durst als Leiste, die über die Zeit sinkt und durch Trinken steigt (Wasser aus Regenfänger/Wasserstelle, Getränke der Taverne; Behälter wie Wasserschlauch = Vorschlag). Wirkung = Vorschlag, im Sinn von „leichte Wirkung“: durstig = keine Lebenspunkt-Regeneration und langsamere Ausdauer-Erholung, keine Sperren; Hitze/Sturm später stärker. Nur Spieler; Bewohner bleiben bei 3 Bedürfnissen (Regel 7). Umsetzung zusammen mit Tag/Nacht und Wetter | bestätigt (2026-11-16, Nutzer: „Ja“ zu echtem Durst); Wirkung und Zahlen = Vorschlag von Claude |
| Niedergeschlagen und Wiederbeleben | Präzisiert „Tod im Koop“ (2026-11-13): Koop 15 s niedergeschlagen (statt ca. 45 s), Mitspieler hält 3 s E zum Wiederbeleben, danach Aufwachen am Bett. Solo: kein Niedergeschlagensein, direkt Tod bzw. Aufwachen. Fähigkeit „Heilende Hände“ (Seherin-Zweig, von allen lernbar): sofortiges Wiederbeleben, 1× pro Spieltag (vorerst Datenhaken, Lernsystem fehlt). „Letzter Atem“: fallen die LP auf 0 und liegt ein Heilmittel in der Schnellleiste, wird es automatisch verbraucht, man bleibt mit 25 % LP stehen, 1× pro Spieltag, solo und Koop gleich. Markierung des Grabs im HUD mit Entfernung (Rabe später mit den Tieren). Bild beim Niedergeschlagensein grau mit leichtem türkisem Rand, kein Kriechen. Später vorgemerkt: Lebensrune, Heilerin als Dorfberuf | bestätigt (2026-11-16, Nutzer: „15 Sekunden“, Fähigkeit, Solo direkt; Fassung nach Prüfung „Ja“) |
| Spielmodus und Schwierigkeit | „Welt erstellen“ Seite 1: Spielmodus **Saga** (Hauptstory als roter Faden) oder **Freies Spiel** (ohne Hauptstory; Nebenquests, Magie, Erkunden bleiben; Optionen frei kombinierbar) und Schwierigkeit **Einfach / Mittel / Schwer / Hardcore** als Voreinstellung mehrerer Weltoptionen und Faktoren (Tod, Raten für Hunger/Durst/Erschöpfung, erlittener Schaden 0,6/1/1,4/1,8, Ertrag 1,5/1/0,8/0,7, Horden, Bewohner unsterblich, Abreißen, Gespür-Tempo; Tabelle im Phasenplan). Seite 2 „Erweiterte Einstellungen“: alle Weltoptionen nach Bereichen (Bauen, Überleben, Welt, Dorf, Koop); eine Änderung setzt „Benutzerdefiniert“. Schwierigkeit nach dem Erstellen nicht änderbar. Noch nicht gebaute Optionen ausgegraut („kommt später“). Questhilfen als Spieler-Einstellung, nicht pro Welt. „Survivor“ geht in Hardcore auf | bestätigt (2026-11-16, Nutzer: „Ja“ zum Vorschlag); Zahlen der Tabelle = Vorschlag von Claude |
| Lebenspunkte und Schaden (Umsetzung) | Grundwert 100 LP, dazu der Buff „Gesundheit“ der Mahlzeiten (+10/+14 …) als Maximum. Schaden läuft über eine allgemeine Tür (`FGamerDamageInfo`: Quelle, Menge, Art; Schnittstelle `IGamerDamageable` als Haken für Tiere, Bewohner und später den Kampf). Fallschaden ab 400 cm Fallhöhe, 12 LP je weiteren Meter (800 cm = 48 LP). Heilung: 0,5 LP/s, **nur solange man satt ist** (eine Mahlzeit aktiv), der Buff „Regeneration“ addiert sich (Eintopf: +0,7); hungrig bleibt es stehen, ohne Strafe (Regel 5). Werte in `game/Content/Data/health.json` | Vorschlag von Claude (2026-11-16), Zahlen zum Justieren |
| Grab und Wiedereinstieg (Umsetzung) | **Grab:** ein Actor am Todesort mit dem ganzen Inventar; der Besitzer kann es sofort öffnen (E), **Mitspieler erst nach 120 s Schutzzeit** (danach dürfen sie es für den Toten bergen, die Dinge gehen an den, der öffnet); was nicht mehr ins Inventar passt, bleibt im Grab. Wird mit der Welt gespeichert, die Schutzzeit läuft über Speichern und Laden weiter. Es gibt noch **kein Ausrüstungssystem**: „Ausrüstung bleibt am Körper“ greift erst, wenn es Ausrüstungsplätze gibt; bis dahin zählt das ganze Inventar (auch Waffen aus der Schnellleiste). **Aufstehen:** nach ca. 2,5 s mit E (Todesbildschirm mit „Aufstehen“-Schaltfläche), mit voller Gesundheit; **am Strohlager, das der Spieler zuletzt gebaut hat** (gemerkt beim Bauen, gespeichert; steht es nicht mehr, am Startpunkt), sonst am Startpunkt der Figur. Ein toter Spieler ist unsichtbar und ohne Kollision und blockiert niemanden. Gleich für Solo und Koop (Regel 2). Weltoption „Tod“ (nichts verlieren / Grab / Hardcore, Voreinstellung Grab) steht als Zeile unter „Welt erstellen“, URL `?Death=none\|grave\|hardcore` | Vorschlag von Claude (2026-11-16) zur bestätigten Zeile „Spielertod“: Schutzzeit 120 s, „Bett zuletzt gebaut“, Aufstehen mit E |
| Hauptstory: Kern-Umfang | Erstes Ziel: Prolog bis Kapitel 3 (Fjordwiesen und Gezeitenmarsch, Bosse Alter Keiler und Schlickmutter, beide Wege nach dem leeren Stein). Kapitel 4–7 später. Ab Kapitel 2 braucht es das Kampfsystem (Phase 3). Kapitelplan: `docs/17_hauptstory_kapitel.md` | bestätigt (2026-11-16, Nutzer wählte die Empfehlung) |
| Hauptstory: Hintergrund | Hallbjörn ließ Seeglas im Steinkreis „rufen“, das Licht sah man über das Meer. Der Aschebund (Glutküste) sucht Menschen, die rufen können, und bezahlt den Seekönig und seine Plünderer dafür; deshalb wurden die Ersten überfallen. Damit hängen die späteren Bosse an einem Faden | bestätigt (2026-11-16, Nutzer wählte die Empfehlung) |
| Hauptstory: Haltung „Hüter oder Rufer“ | Zweite große Entscheidung in Kapitel 3 (Wege treffen sich): Hüter (den Ring heilen) oder Rufer (den Gezeitenhall nutzen). Bestimmt in Kapitel 4–6 Verbündete und Feinde, ohne getrennte Kapitel; legt das Finale nicht fest. Jeder Spieler wählt für sich | bestätigt (2026-11-16, Nutzer wählte die Empfehlung) |
| Hauptstory: Hallbjörn lebt | Hallbjörn lebt; Begegnung in Kapitel 5 (Eishöhle im Basaltfjell). Entscheidung: heimbringen (wird Bewohner) oder im Eis lassen (bleibt Stimme im Kreis, Lauschen stärker) | bestätigt (2026-11-21, Nutzer wählte die Empfehlung) |
| Hauptstory: Eid des Hügelkönigs | Kapitel 4: Eid schwören (kein Kampf, Verbündeter, Tribut aus dem Lager, Eid später brechbar, dann Bosskampf) oder herausfordern (Bosskampf). Offen: Wie wirkt der Tribut im Koop, wenn nur ein Spieler schwört (das Lager gehört allen)? | bestätigt (2026-11-21, Nutzer wählte die Empfehlung); Koop-Frage offen |
| Gezeiten | Es gibt Ebbe und Flut: Der Wasserstand an Küste und Marsch steigt und fällt zweimal pro Spieltag (alle 15 Minuten wechselnd). Steinkreis und Wrack in der Gezeitenmarsch sind nur bei Ebbe erreichbar; Ebbe kommt immer wieder (kein Zeitdruck) | bestätigt (2026-11-21, Nutzer wählte die Empfehlung); Takt = Vorschlag von Claude |
| Hügelkönig: Tribut im Koop | Hat mindestens ein Spieler geschworen, nimmt der Hügelkönig einen kleinen Anteil aus dem gemeinsamen Lager (Folgen addieren sich). Spieler, die nicht geschworen haben, können ihn trotzdem herausfordern; dann gilt der Eid für alle als gebrochen. Beantwortet die offene Koop-Frage aus „Hauptstory: Eid des Hügelkönigs“ | bestätigt (2026-11-21, Nutzer: „Ja, wie vorgeschlagen“); Höhe des Anteils offen |
| Hauptstory: der Siebzehnte | Von den 17 Ersten: 4 auf dem Gräberhügel, Hallbjörn mit 3 Gefährten (Sæunn, Orm, Kolla), Asgerd (tot), Tove mit 6 Überlebenden; der Siebzehnte ist „der Lotse“, der die Plünderer zur Bucht führte. Er taucht in Kapitel 2 bei der Schlickmutter auf (Verräter oder Gefangener, je nach Weg). Rechenfehler im Prolog („Acht sind noch irgendwo“) auf „Neun“ korrigiert. Spieltext Kapitel 1: `docs/18_kapitel_1_text.md` | bestätigt (2026-11-21, Nutzer wählte die Empfehlung); Namen der Figuren = Vorschlag von Claude |
| Menschliche Bosse: Nachspawnen | Menschliche Bosse kommen nicht als dieselbe Person wieder: Nach `boss_respawn_days` übernimmt ein Nachfolger mit denselben Boss-Werten (z. B. „Die Schlickmutter“ als Titel, eine neue Kapitänin auf dem Wrack). Die benannte Figur (Hrefna) bleibt einmalig. Gilt für alle menschlichen Bosse | bestätigt (2026-11-21, Nutzer wählte die Empfehlung) |
| Koop: gemeinsame Entscheidung über die Welt im selben Moment | Wenn mehrere Spieler gleichzeitig vor einer Entscheidung stehen, die die Welt nur auf eine Art verändern kann (z. B. Schlickmutter töten oder gefangen nehmen): gemeinsames Gespräch für alle in der Nähe, jeder wählt, keine Uhr (die Lage wartet). Einig: das gilt. Uneinig: die umkehrbare Option gilt (gefangen nehmen; töten bleibt später möglich). Solo: normales Gespräch | bestätigt (2026-11-21, Nutzer fragte nach einer Gesprächsentscheidung und wählte dann „Gemeinsames Gespräch“) |
| Haltung Rufer: Bonus | Wer in Kapitel 3 „Rufer“ wählt, bekommt +25 % Gespür-EP an Steinkreisen; Hüter bekommen dafür mehr Vertrauen bei Toves Leuten und leichteren Handel. Spieltext Kapitel 3: `docs/20_kapitel_3_text.md` | bestätigt (2026-11-21, Nutzer wählte die Empfehlung) |
| Hrefna gefangen: Schicksal | Eine gefangene Hrefna bleibt ohne Frist in der Siedlung. Später entscheidet die Siedlung als gemeinsames Gespräch: hinrichten, freilassen oder als Lotsin zum Eisrand mitnehmen (Kapitel 7). Uneinig: Sie bleibt vorerst gefangen (umkehrbar, Vorschlag von Claude nach dem bestätigten Muster) | bestätigt (2026-11-21, Nutzer wählte die Empfehlung); Regel bei Uneinigkeit = Vorschlag |
| Hauptstory: Faden „Ringvolk“ | Ab Kapitel 4 zeigen manche Nebelbilder das alte Volk, das den Ring band (Kapitel 4: Bau, Kapitel 6: es brach den Ring selbst an drei Stellen, mit Absicht, Kapitel 7: warum). Keine Seite der Endentscheidung ist eindeutig richtig. Nur Satzfetzen, keine eigenen Figuren mit Stimme. Plan: `docs/17_hauptstory_kapitel.md`, „Kapitel 4–7 im Detail“ | bestätigt (2026-11-21, Nutzer wählte die Empfehlung) |
| Kapitel 5: Hallbjörn bei Uneinigkeit | Gemeinsames Gespräch; uneinig: Er bleibt im Eis (umkehrbar, man kann ihn später holen) | bestätigt (2026-11-21, Nutzer wählte die Empfehlung) |
| Kapitel 6: Rätsel und Aschezunge | Ein Runenrätsel des Ringvolks öffnet den Weg zum Steinkreis der Glutküste (Seeglas und Obsidian richtig legen; keine Uhr, beliebig viele Versuche). Bei Uneinigkeit über die Aschezunge (Kampf, Verrat, Folgen) entkommt sie und steht im Finale an der Seite des Seekönigs | bestätigt (2026-11-21, Nutzer: „binde ein rätsel mit ein“, auf Nachfrage „Rätsel als Weg zum Kreis“); Ausgestaltung des Rätsels = Vorschlag |
| Finale im Koop | Die Finalentscheidung läuft als gemeinsames Gespräch; uneinig: Der Ring bleibt offen (schließen ist später noch möglich). Ersetzt den Vorschlag „die Welt zeigt das zuerst gewählte Ende“ aus der Zeile „Finale der Hauptstory“ | bestätigt (2026-11-21, Nutzer wählte die Empfehlung) |
| Haare und Bärte: Detail und Bewegung (Generalanweisung) | Haare werden hochwertig als Strähnen (MetaHuman-Grooms) angelegt und für das Spiel automatisch heruntergerechnet: nah Strähnen, mittel Haar-Karten, fern Haarhaube (LODs aus dem Zusammenbau). Haare bewegen sich passend zu Bewegung, Wetter und Umständen: Simulation über Führungshaare (Schwerkraft, Laufen, Kopfdrehung), Wind und Luftwiderstand pro Figur vom Wettersystem gesetzt, Nässe als Materialänderung. Simulation für Spieler und nahe Bewohner (Strähnen/Karten), ferne Figuren ohne Simulation. Grenzen nach Messung; **8 GB Grafikspeicher sind absolutes Maximum** (Nutzer 2026-11-21), nicht nur Ziel | bestätigt (2026-11-21, Nutzer: „als Generalanweisung“) |
| Art-Stil: Gesamtlinie | Gemischt: Welt, Gebäude, Werkzeuge und Figuren realistisch (wie Vegetation und MetaHumans), HUD und Menüs stilisiert im Stil „Gezeitenhall“. Löst die offenen Zeilen „Art-Stil (realistisch vs. stilisiert)“ und „Art Direction: wie realistisch?“; „Klarform-Saga“ gilt nur noch für HUD/Menüs. Folge: Holzbaukasten und Stil-Testgebäude müssen realistischer nachgezogen werden (Design-Aufgabe) | bestätigt (2026-11-21, Nutzer: „Gemischt“) |
| Mindest-Hardware: Grafikkarte | GTX 1660 / RX 5600 (6 GB Grafikspeicher). Qualität Niedrig ohne Lumen, Lumen ab Hoch; Nanite nur, wo es auf der Mindestkarte nicht bremst. Folge: auf schwächerer Hardware bzw. mit gedrosselter Auflösung messen, sobald die MetaHumans drin sind. CPU und RAM-Mindestwert (8 GB, Regel 11) folgen nach der Messung. Löst „GPU-Mindestanforderung“ | bestätigt (2026-11-21, Nutzer wählte die Empfehlung) |
| Server-Hosting | Spieler hosten selbst: Listen-Server im Spiel plus Dedicated-Server zum Selbsthosten. Keine eigenen Server. Löst „Server-Hosting“ | bestätigt (2026-11-21, Nutzer wählte die Empfehlung) |
| Veröffentlichung | Steam, als Early Access. KI-Angaben über `design/asset_log.md`. Löst „Veröffentlichung (Steam?)“ | bestätigt (2026-11-21, Nutzer wählte die Empfehlung) |
| „Survivor“-Modus | Überlebens-Modus als Weltoption: Bedürfnisse (Hunger, Kälte, später Durst) wirken stärker und schneller, ohne Ausdauer-Sperre (Regel 5). Kein Horden-Modus unter diesem Namen (Horden bleiben eigene Weltoption). Löst „„Survivor“-Modus“ | bestätigt (2026-11-21, Nutzer wählte die Empfehlung) |
| Magie: Phase | Das Magie-System (Runen, Seidr, Tränke) wird mit Phase 3 (Kampf und Ausrüstung) gebaut. Bis dahin nur Gespür/Scan und Tränke als Essens-Buffs | bestätigt (2026-11-21, Nutzer wählte die Empfehlung) |
| Armbrust: Stufe | T4 als seltene Import- bzw. Schwarzmarktware, nicht regulär herstellbar. Löst „Armbrust: ab welcher Stufe“ | bestätigt (2026-11-21, Nutzer wählte die Empfehlung) |
| Gegnerarten: Fantasy-Kreaturen (Antwort) | Die 48 Kreaturen aus Geminis „Küstenasche – Gesamtdokumentation“ (`D:\gamer\heruntergeladenes\k_stenasche_gesamtdokumentation.md`, liegt außerhalb des Projekts) werden Standard-Gegner, neben Tieren und Menschen; keine Wasserwesen. Ersetzt „zuerst nur Menschen und Tiere“ aus der Zeile „Gegnerarten und Bosse“ (2026-11-13); die alte Zeile bleibt stehen. Folge: Der zurückgestellte Abschnitt „Sagenwesen“ in `design/ASSET_LIST_GESAMT.md` wird aktiv; Namen und IDs (`creature_`) folgen in der bereinigten Fassung | bestätigt (2026-11-22, Nutzer wählte „Fantasy wie bei Gemini“, nachdem Claude auf den Aufwand und die Entscheidung vom 13.11. hingewiesen hatte) |
| Bosse: Sagen-Fassung als Standard | Alle Bosse treten in der Sagen-Fassung aus `design/ASSET_LIST_GESAMT.md` auf: Schlickmutter als Moorhexe mit Irrlichtern, Hügelkönig als Draugr-Fürst, Felsenvater als Troll, Aschezunge als Lindwurm, Sturmkoloss als Frostriese; der Alte Keiler bleibt realistisch. Die Weltoption „Sagenwelt“ entfällt. Ersetzt „Bosse in der realistischen Fassung“ aus „Gegnerarten und Bosse“ (2026-11-13). Folge: Kapitel 2 (`docs/19_kapitel_2_text.md`) und der Kapitelplan 4–7 (`docs/17_hauptstory_kapitel.md`) müssen angepasst werden; berührt sind auch „Hauptstory: Eid des Hügelkönigs“ und „Kapitel 6: Rätsel und Aschezunge“ (2026-11-21) | bestätigt (2026-11-22, Nutzer wählte „Sagen-Fassung als Standard“); Anpassung der Story offen |
| Fortschritt: Spielerlevel 1–50 | Spielerlevel mit Obergrenze 50 wie im Gemini-Dokument: Das Level bestimmt, ab wann Rezepte gehen, und damit die Waffen-, Rüstungs- und Resistenzwerte; die Gegner-Ränge richten sich danach. Passt zu „Ausrüstung levelbasiert“ (Vision). Die Stufen-Leiter T1–T6 bleibt; die Zuordnung Level ↔ T-Stufe ↔ Biom folgt als Vorschlag in der bereinigten Fassung | bestätigt (2026-11-22, Nutzer wählte „Level 1–50 wie Gemini“); Zuordnung offen |
| Erfahrung (Level) | Erfahrung aus allem: Kämpfe, Bosse, Quests, Entdecken, Herstellen, Bauen, Sammeln; Mitspieler in der Nähe bekommen einen Anteil. Jeder Aufstieg gibt Fähigkeitspunkte für den Skilltree, die Werte steigen weiter durch Tun (`docs/11_charaktere.md`). Rezepte haben eine Level-Anforderung | bestätigt (2026-11-22, Nutzer wählte die Empfehlung); Zahlen (Kurve, Anteil, Punkte je Stufe) offen |
| Gebiete: Geminis Gebiete zugeordnet | Die 6 Biome und die Story bleiben. Geminis Gebiete werden zugeordnet: Moosgeäst & Sumpf-Senken → Fjordwiesen und Gezeitenmarsch, Bruchstein & Gewölbe-Spalten → Hügelgrab-Heide, Glut-Fluren & Schlacke-Böden → Glutküste, Schroffen-Grate & Frost-Kanten → Basaltfjell, Böen-Kämme & Sturm-Kuppen → Eisrand. Resistenzen: Hitze → Glutküste, Frost → Basaltfjell und Eisrand, Sturm → Eisrand, Verfall/Sporen → Gezeitenmarsch und Hügelgrab-Heide | bestätigt (2026-11-22, Nutzer wählte die Empfehlung) |
| Beute: Kreaturen fein, Tiere schlank | Kreaturen: feine Beute wie im Gemini-Dokument (Basis-, Rang- und Biom-Zusatzbeute); unklare Sammelbegriffe („Seltene Rohstoffe“, „Spitzen-Materialien“ …) werden zu konkreten Gegenständen. Tiere: schlank, pro Tierart 1–2 typische Teile (z. B. Wolfsfell, Bärenfett, Hauer), dazu Fleisch; Sehne und Knochen je T-Stufe. Bosse: eigenes Bossmaterial | bestätigt (2026-11-22, Nutzer: „kreaturen wie gemini tiere dein forschlag“); Deutung und Umwandlung der Sammelbegriffe von Claude |
| Herstellen: Metall plus Kreaturenteile | Die Stufen-Leiter T1–T6 (Rohstufe, Raseneisen, Schmiedeeisen, Stahl, Damast, Relikt) bleibt das Rückgrat für Waffen, Werkzeuge und Rüstung. Kreaturenteile kommen als Zusatz dazu (Griffe, Bindungen, Polster, Resistenzen, Haltungsschaden) und für einzelne Spezialsets (z. B. Frost-Schuppenrüstung). Das Level legt fest, ab wann ein Rezept geht | bestätigt (2026-11-22, Nutzer wählte die Empfehlung); Rezepte = Vorschlag |
| Kreaturen: Reihenfolge | Alle 48 Kreaturen kommen zuerst als Daten (Werte, Beute, Spawn-Regeln). Modelle und Animationen gestaffelt, zuerst der Kern: Fjordwiesen (Rang 1) mit Tieren und Waldbanditen, Boss Alter Keiler; Gezeitenmarsch (Rang 2) mit Bramblefang, Siltstrider, Mirecrawler, Thornmaw und Strandplünderern, Boss Schlickmutter (jetzt als Moorhexe mit Irrlichtern). Danach Biom für Biom: Heide, Fjell, Glutküste, Eisrand | bestätigt (2026-11-22, Nutzer wählte die Empfehlung) |


[↑ zum Inhaltsverzeichnis](#inhaltsverzeichnis)


---

<a id="teil-2"></a>

# Teil II – Planung, Technik und Pipeline

<a id="d4"></a>

## 4. Phasenplan (Stand 2026-11-12)

> Quelle: `docs/02_phasenplan.md`

Grundsatz: erst ein kleiner, spielbarer Vertical Slice, dann ausbauen. Reihenfolge ist ein Vorschlag.

| Phase | Inhalt | Fertig, wenn … |
|---|---|---|
| 0 Fundament | UE-5.8-Projekt in `game/`, Git, MCP-Anbindung, Build-Pipeline, Dedicated-Server-Target | Projekt baut, Server und Client verbinden sich |
| 1 Vertical Slice „Siedlung“ | Spielfigur (Koop), Bauteile, Ressourcen, 3–4 Dorfbewohner-Jobs, 3 Bedürfnisse, Lager | Eine Siedlung läuft zu zweit ohne ständiges Eingreifen |
| 2 Siedlung ausbauen | ca. 10 Jobs, Zufriedenheit/Perks, Schichten, Kommandoansicht (Vogelperspektive), Miliz | Dorf versorgt sich selbst über mehrere Spieltage |
| 3 Kampf und Ausrüstung | KCD2-inspiriertes Kampfsystem ohne Ausdauer-Sperre, Ausrüstungs-Slots, Schmieden, Skilltree | Kämpfe machen Spaß, Ausrüstung hat spürbaren Effekt |
| 4 Welt | Biome, Questlinien, Bosse, Schwierigkeitsstufen | Erste zwei Biome mit Boss spielbar |
| 5 Spätspiel | Portale, optionale Angriffswellen, Weltoptionen/Modi, Skalierungstests bis 8 Spieler | Weltoptionen kombinierbar, Portale transportieren zwischen Lagern |

<a id="d4-1"></a>

### 4.1 Stand Phase 0 (2026-11-12)
- Erledigt: Projekt baut (`GamerEditor Win64 Development`), Git + GitHub, MCP-Anbindung (siehe `docs/03_setup_checkliste.md`).
- **Verbindungstest Listen-Server + 1 Client bestanden** (Launcher-Build, beide Prozesse ohne Grafik auf demselben PC):
  Host `UnrealEditor.exe Gamer.uproject /Engine/Maps/Templates/Template_Default?listen -game -nullrhi -nosound`,
  Client `UnrealEditor.exe Gamer.uproject 127.0.0.1:7777 -game -nullrhi -nosound`.
  Host-Log: „Join succeeded“, Client-Log: „Welcomed by server“. Getestet nur lokal (127.0.0.1), nicht über zwei PCs oder das Internet.
- Auffällig: Im `-game`-Modus melden die Python-Skripte der Editor-Toolsets Fehler (`module 'unreal' has no attribute 'AgentSkill'` u. a.), weil es dort keine Editor-Klassen gibt. Für den Test folgenlos.
- **Build-Pipeline:** `tools/build.ps1` mit den Schritten `Editor`, `Game`, `Package`, `ProjectFiles` (Aufruf siehe Kopf des Skripts). Getestet 2026-11-12:
  Editor-Sperre greift bei offenem Editor; `Game` baut `Gamer.exe` in ca. 130 s; `Package -Config Development` in ca. 150 s, 0 Fehler, **0,96 GB** (leeres Projekt mit Engine-Karte `OpenWorld`); das gepackte Spiel startet (ohne Grafik geprüft) und lädt die Karte.
- Offen in Phase 0: Dedicated-Server-Target (braucht Source-Build, siehe unten). **Erledigt am 2026-11-13**, siehe nächster Abschnitt.

<a id="d4-2"></a>

### 4.2 Stand Phase 0 (2026-11-13): Dedicated Server
- Engine aus dem Quellcode gebaut (`E:\UE58`, Tag 5.8.4-release, 9306 Schritte, 9,9 h, 0 Fehler); `game/Gamer.uproject` zeigt auf diese Engine.
- Neues Target `GamerServer` (`game/Source/GamerServer.Target.cs`), gebaut in 2,6 h, 0 Fehler. `tools/build.ps1` kennt dafür `Server` und `PackageServer`.
- Setup der Engine lief ohne Mac/iOS/Android/Linux; das Paketier-Werkzeug (UAT) braucht aber die iOS-Dateien zum Übersetzen. Nachgeladen am 2026-11-13 (535 Dateien, 3,1 GB).
- **Mehrspieler-Test bestanden** mit `tools/mp_test.ps1` (Server + 2 Spieler ohne Grafik, lokal 127.0.0.1): einmal Editor als Server, einmal das paketierte `GamerServer.exe` (Paket 2,76 GB, davon 2,3 GB Debug-Symbole). Spieler-Clients waren ungecookte Editor-Instanzen und konnten sich mit dem gecookten Server verbinden.
- Spielerobergrenze `MaxPlayers=8` in `game/Config/DefaultGame.ini` (Entscheidung 2026-11-13).

<a id="d4-3"></a>

### 4.3 Stand Phase 1 (2026-11-13): Spielfigur im Koop
- C++: `AGamerCharacter` (Third-Person, Platzhalter-Körper aus Engine-Zylinder, Farbe je Spieler), `AGamerPlayerController`, `AGamerGameMode` (Spielerfigur und Controller setzen, Spieler am Startpunkt nebeneinander aufstellen). Steuerung mit Enhanced Input, im Code aufgebaut (WASD, Maus, Leertaste, Gamepad). Bewegungswerte als Config-Werte (`[/Script/Gamer.GamerCharacter]` in `DefaultGame.ini`), Rückfallwerte im Code.
- **Test bestanden** (`tools/mp_test.ps1`, Karte `Template_Default` mit `?game=/Script/Gamer.GamerGameMode`, Server + 2 Spieler ohne Grafik): Spieler 1 lief 3 s geradeaus, Server, Spieler 1 und Spieler 2 sahen dieselbe Endposition (1317 cm), Spieler 2 blieb stehen.
- Nicht automatisch getestet: Tastatur-/Maussteuerung und Kamera (brauchen Grafik). Zum Selbst-Ausprobieren: `tools/play_local.ps1`.
- Projekt-Standards in `DefaultEngine.ini` (2026-11-13): `GlobalDefaultGameMode=/Script/Gamer.GamerGameMode`, `ServerDefaultMap=Template_Default` (vorher `Entry`, eine leere Karte, in der Figuren fallen würden). Getestet: Server ohne Zusatzangaben lädt `Template_Default` mit `GamerGameMode`, 2 Spieler, Bewegung kommt beim anderen Spieler an.

<a id="d4-4"></a>

### 4.4 Stand Phase 1, Schritt 2 (2026-11-13): Datenbasis
- Format bestätigt: `docs/06_datenformat.md`. Vier JSON-Tabellen in `game/Content/Data/`: `items` (8), `resource_nodes` (6), `recipes` (5), `buildables` (16), Startwerte aus Abschnitt 6 des Formatdokuments. Modell- und Icon-Pfade noch leer (Assets nicht importiert).
- C++: `GamerDataTypes.h` (Zeilentypen), `GamerDataLoader` (Einlesen und Prüfen: Pflichtfelder, unbekannte Felder, ID-Format und -Präfix, doppelte IDs, Wertebereiche, Verweise auf Gegenstände/Stationen/Werkzeugtypen, Asset-Pfade), `UGamerDataSubsystem` (lädt beim Start auf Server und Spielern, Abfrage per ID, Konsolenbefehl `gamer.ReloadData`, Prüfsumme im Protokoll).
- Kerndateien (mit Freigabe): `Gamer.Build.cs` bekommt das Modul `Json` (über die Engine nur für Header sichtbar, nicht zum Linken), `DefaultGame.ini` packt den Ordner `Data` beim Paketieren ein.
- **Tests bestanden:** `tools/test_data.ps1` → `Gamer.Data.ShippedDataIsValid` und `Gamer.Data.ValidatorFindsErrors` (absichtlich kaputte Daten, alle 10 Fehlerarten erkannt) grün, ca. 70 s. Mehrspieler: Server und 2 Spieler laden dieselben Daten, Prüfsumme `C0022D82`, 0 Fehler, 0 Warnungen – sowohl mit dem Editor als Server als auch mit dem paketierten `GamerServer.exe` (Paket mit `Template_Default`, 5,6 min; die JSON-Dateien kommen also mit ins Paket).
- Noch nicht umgesetzt: Prüfsummen-Vergleich beim Beitritt (Spieler mit anderen Daten abweisen) – laut Formatdokument vor dem ersten öffentlichen Test. Die Daten werden noch von keiner Spielmechanik benutzt; das kommt mit Sammeln, Inventar und Bauen (Schritte 3 und 4).

<a id="d4-5"></a>

### 4.5 Stand Phase 1, Schritt 3 (2026-11-13): Sammeln, Inventar, Anfangs-Crafting
- Spielstart (Entscheidung 2026-11-13): Grundressourcen von Hand sammeln (lose Steine, Äste, Pflanzenfasern), Werkzeuge im Anfangs-Crafting-Menü. Dazu natürliche Nahrungsquellen: Wildapfelbaum, Beerenstrauch, Pilzstelle, Haselstrauch. Vorbilder: The Forest, thematisch Medieval Dynasty und Aska.
- Datenprüfer erweitert: **Erreichbarkeit ab Spielstart** (fand den Kreislauf „Axt braucht Stamm, Stamm braucht Axt“ der alten Startwerte). Test `Gamer.Data.DetectsToolDeadlock`.
- C++: `UGamerInventoryComponent` (24 Plätze, Stapel und Gewicht aus den Daten, nur der Server ändert, nur der Besitzer sieht es), `AGamerResourceNode` (Vorkommen mit Platzhalter-Formen, abgeerntet/voll repliziert, Nachwachsen nach `regrow_days` × Spieltag 30 min, Test-Beschleunigung `gamer.RegrowScale`), `UGamerWorkComponent` (Sammeln und Herstellen mit Zeit; der Server prüft Reichweite, Werkzeug, Zutaten, Platz; nur ein Spieler pro Vorkommen), `AGamerHUD` (Platzhalter-Anzeige: Inventar, Hinweis, Fortschritt, Crafting-Menü). Steuerung: E sammeln, Tab Crafting-Menü, 1–9 herstellen.
- Startgebiet (vorläufig, bis zur Weltgenerierung): `AGamerGameMode` streut 63 Vorkommen aller Arten um den Startpunkt (fester Seed 4711), wenn die Karte keine hat.
- **Test bestanden** (Server + 2 Spieler ohne Grafik, Testskript `-GamerTestScript`): Spieler 1 sammelt Äste, Steine, Fasern, stellt die Steinaxt her (Zutaten korrekt abgezogen), fällt danach eine Kiefer (+3 Stämme), pflückt Beeren und Äpfel; Spieler 2 sieht alle 10 Vorkommen als abgeerntet. 0 Fehler.
- Nicht automatisch getestet: Tastatur (E, Tab, 1–9), Anzeige und Aussehen der Platzhalter (braucht Grafik → `Spiel_testen.bat`), Nachwachsen, Ablehnungen (z. B. „Für Kiefer brauchst du eine Axt“). Werkzeug-Haltbarkeit und Traglast-Grenze sind noch nicht eingebaut.
- Werkzeuge: `tools/mp_test.ps1` und `tools/play_local.ps1` nehmen jetzt den tatsächlich vergebenen Port (ein übrig gebliebener Testserver hatte 7777 belegt) und beenden beim Aufräumen auch Kindprozesse (das paketierte `GamerServer.exe` ist nur ein Starter). Neu: `Spiel_testen.bat` zum Doppelklicken.

<a id="d4-6"></a>

### 4.6 Erster Spieltest mit Grafik und Änderungen danach (2026-11-13)
- Der Nutzer hat mit `Spiel_testen.bat` gespielt (erster Start ca. 7 min Shader-Übersetzung). Sammeln von Hand lief. Rückmeldung: Springen zu langsam, Aufsammeln zu langsam, Texte zu klein, Rot unlesbar, Abbauen soll per Schlag gehen, Schnellleiste mit 8 Plätzen, Tasten im Menü umbelegbar.
- Sprung: Startgeschwindigkeit 700 statt 500, Schwerkraft ×2 (neuer Wert `GravityScale` an der Figur): etwa gleich hoch (ca. 1,25 m), aber 0,7 s statt 1 s in der Luft.
- Aufsammeln von Hand etwa doppelt so schnell (`work_seconds`: Äste und lose Steine 0,6 s, Pilze 0,8 s, Fasern 0,9 s, Beeren und Hasel 1,0 s, Apfelbaum 1,2 s). Bäume und Felsen unverändert.
- Abbauen per Schlag: Linksklick (gedrückt halten = weiterschlagen), alle 0,8 s ein Schlag (`StrikeInterval`), jeder Schlag bringt 0,8 s × Werkzeug-Tempo Arbeit; `work_seconds` bleibt die Gesamtarbeit. Kiefer 8 Schläge, kleiner Fels 4, mittlerer Fels 8. Fortschritt wird am Vorkommen repliziert und als Balken angezeigt. E an Baum oder Fels wird abgelehnt mit Hinweis.
- Schnellleiste: 8 Plätze unten in der Mitte, Tasten 1–8, gewählter Platz wird repliziert. Werkzeug muss gewählt sein; sonst Hinweis „Taste N: Steinaxt in die Hand nehmen“.
- Anzeige: Schrift mindestens 1,4-fach und mit der Fensterhöhe wachsend, Schatten, dunkle Unterlagen, kein Rot mehr. Feinjustierung im Spiel über die Konsole: `gamer.HudScale 1.3`.
- **Test bestanden** (Server + 2 Spieler ohne Grafik): sammeln, Axt und Spitzhacke herstellen (landen auf Platz 1 und 2), E an der Kiefer abgelehnt, Kiefer mit Axt gefällt, kleiner und mittlerer Fels mit Spitzhacke abgebaut, Beeren gepflückt; Spieler 2 sieht alle 13 Abbauvorgänge. Datentests 3/3 grün.
- Nicht getestet: alles, was Grafik braucht (Größe und Lage der Anzeige, Gefühl von Sprung und Schlagen), zwei Spieler am selben Baum. Das paketierte `GamerServer.exe` ist noch nicht neu gebaut.
- Menü und Tastenbelegung (umgesetzt am selben Tag): **Esc** öffnet ein Menü („Weiter“, „Steuerung“, „Spiel beenden“); das Spiel läuft dabei weiter (Koop), nur die eigene Figur ist gesperrt. Unter „Steuerung“ lassen sich alle Tastatur- und Maustasten umbelegen (Laufen, Springen, Schlagen, Sammeln, Herstellen-Menü, Schnellleiste 1–8): Aktion anklicken, neue Taste drücken; ist die Taste schon vergeben, werden die beiden getauscht. „Standard wiederherstellen“ setzt zurück. Gespeichert pro Benutzer in `Saved/Config/<Plattform>/GameUserSettings.ini` (Klasse `UGamerKeyBindings`), nicht im Projekt. Fest bleiben: Esc, Maus zum Umsehen, Controller-Tasten. Alle Hinweise in der Anzeige zeigen die tatsächliche Taste; Server-Meldungen nennen keine Tasten mehr.
- Mehrfaches Drücken der Sammeln-Taste während des Sammelns wird jetzt still ignoriert (vorher Meldung „Du arbeitest schon an etwas“ bei jedem Druck).
- **Getestet:** Umbelegen, Tauschen, Speichern, Laden beim nächsten Start und Zurücksetzen (Testskript-Schritte `bind`, `keys`, `resetkeys`); Menü über simulierte Tastendrücke (Esc öffnet, Aktion wählen, R drücken → „Schlagen: R“, Esc zurück, Esc schließt). Mit Grafik aufgenommen und angesehen: Spielanzeige mit Schnellleiste und Hinweisen, Menü, Seite „Steuerung“ (auch im Zustand „Taste drücken …“). Nicht getestet: echte Mausklicks auf die Menüknöpfe (die Treffer-Prüfung ist dieselbe, die die Anzeige zum Hervorheben nutzt), Spielen mit umbelegten Tasten über längere Zeit.
- Testwerkzeuge: neue Schritte im Testskript `bind:<aktion>=<Taste>`, `resetkeys`, `keys`, `menu:main|controls|close`, `button:<id>`, `press:<Taste>`, `shot`; `tools/mp_test.ps1 -Render` öffnet das Spielfenster jetzt sichtbar statt minimiert (ein minimiertes Fenster zeichnet nicht). Die Engine-eigenen Bildschirmfotos landen in diesem Modus nur in der Leistungsaufzeichnung; Fensterbilder werden deshalb von außen über Windows aufgenommen.

<a id="d4-7"></a>

### 4.7 Stand Phase 1, Schritt 4 (2026-11-13): Bauen
- **Assets in Unreal:** `tools/import_assets.ps1` (ruft `tools/unreal/import_assets.py` ohne Editor-Fenster auf) importiert alles aus `Assets/manifest.json` nach `/Game/Gamer/…`: 10 Bauteile des Holzbaukastens, Baumstamm, 8 Texturen, Icon, Grundmaterial `M_Gamer_Base` mit Instanzen `M_Trim_Wood`, `M_Turf`, `M_Item_Log`, dazu `M_BuildPreview` (durchsichtig) für Baupläne und Vorschau. Kollision der Bauteile = das Modell selbst (Türöffnung begehbar). Wiederholbar, ca. 15 s.
- **Weltoptionen** (Entscheidung 2026-11-13, im Startmenü wählbar, bis dahin über die Karten-URL bzw. `Spiel_testen.bat -BuildFlow … -Placement … -Demolish …`): `AGamerGameState` hält sie und überträgt sie an alle. Bauablauf `blueprint` (Standard) / `instant` / `material`, Platzierung `free` (Standard) / `grid`, Abreißen `full` (Standard) / `half` / `off`. Das Esc-Menü und das Baumenü zeigen die Optionen der Welt an.
- **Bauen:** Hammer in die Hand, Rechtsklick öffnet das Baumenü (Baukasten links, Arbeitsstätten rechts, Kosten grün/orange). Das gewählte Teil folgt als Vorschau dem Fadenkreuz (grün = möglich, orange = nicht möglich, mit Grund), R dreht, Linksklick platziert. Plätze: Fundamente neben Fundamenten, Böden auf den Wänden, Wände/Türwände/Giebel/Balken an den Kanten, Pfosten an den Ecken, Dachflächen an den Kanten nach innen steigend, First in der Mitte, Tür in der Türwand. Freie Teile (Werkbank, Lagerfeuer …) nur auf ebenem Boden. Ein Platz kann nur einmal belegt werden. Der Server prüft alles noch einmal (Hammer in der Hand, Reichweite 10 m, passender Einrastpunkt, Material).
- **Bauplan + Hammer:** Ein Linksklick auf einen unfertigen Bauplan hämmert ihn (wie Abbauen, Mitspieler können helfen); Baupläne sind durchsichtig und begehbar. X reißt ab: unfertige Pläne geben alles zurück, fertige Teile je nach Weltoption.
- **Herstellen an Stationen:** Steht eine fertige Werkbank bzw. ein Lagerfeuer in 5 m Nähe, erscheinen deren Rezepte im Herstellen-Menü (z. B. „Bretter [Werkbank]“).
- **Kamera:** leicht über die rechte Schulter versetzt (Config `CameraShoulderOffset`), damit die eigene Figur die Stelle unter dem Fadenkreuz nicht verdeckt.
- **Neue Tasten** (umbelegbar): Baumenü (Rechtsklick), Bauteil drehen (R), Abreißen (X).
- **Getestet** (Server + 2 Spieler ohne Grafik): Werkbank bauen und Bretter daran herstellen; Haus aus Fundament, 3 Wänden, Türwand, Tür, 2 Dachflächen, 2 Giebeln, First planen, belegten Platz abgelehnt, alles fertig gehämmert, Tür abgerissen (+4 Bretter); Variante „sofort fertig + Raster + halb“ (Fundament auf X 600 / Y 300, Abriss gibt 3 von 6 Brettern); Variante „Material beim Bauen + kein Abreißen“ (Material schlagweise, Stopp bei fehlenden Brettern, Abreißen abgelehnt); Spieler 2 sieht die Teile. Mit Grafik angesehen: Haus von drei Seiten (Satteldach richtig), Baupläne, Baumenü, Vorschau „Nur auf ebenem Boden“, Herstellen an der Werkbank, Seite „Steuerung“.
- **Beim Testen gefunden und behoben:** Dachflächen standen gespiegelt (Blender → Unreal spiegelt die Y-Achse); die Weltoptionen wurden anfangs nicht gesetzt (Spielstand entsteht erst nach `InitGame`).
- **Nicht getestet / offen:** echtes Spielen mit Maus (Zielen, Vorschau, Drehen); zwei Spieler am selben Bauplan; Böden im Obergeschoss, Pfosten, Balken (Einrastpunkte vorhanden, nicht ausprobiert). Arbeitsstätten haben Platzhalter-Formen. Bauteile werden noch nicht gespeichert (es gibt noch kein Speichern) und brauchen keinen Halt (Wand bleibt stehen, wenn das Fundament abgerissen wird). Vor dem nächsten Paketieren muss `/Game/Gamer` in `DefaultGame.ini` unter „immer mitkochen“ stehen, weil die Modelle nur über die JSON-Daten verwiesen werden (Kerndatei, braucht Freigabe).
- **Nächster Schritt laut Entscheidung:** Startmenü (Welt erstellen mit Weltoptionen, Beitreten). Erledigt, siehe nächster Abschnitt.

<a id="d4-8"></a>

### 4.8 Startmenü (2026-11-13)
- Das Spiel startet im **Startmenü** (Karte `/Engine/Maps/Entry`, Spielmodus `AGamerMenuGameMode`, keine Figur): „Welt erstellen“, „Beitreten“, „Steuerung“, „Beenden“. Kerndatei mit Freigabe: `DefaultEngine.ini` → `GameDefaultMap=/Engine/Maps/Entry` (vorher die OpenWorld-Vorlage) und `GameModeMapPrefixes` Entry → Startmenü.
- **Welt erstellen:** die drei Bau-Weltoptionen (Wert anklicken wechselt ihn, mit kurzer Erklärung), dann „Welt starten“. Dieser Spieler wird Gastgeber (Listen-Server, Port 7777) auf `Template_Default` mit den Optionen in der Karten-URL. Die Auswahl wird pro Benutzer gemerkt (`UGamerMenuSettings` in `GameUserSettings.ini`).
- **Beitreten:** Adresse des Gastgebers eintippen (Ziffern, Buchstaben, Punkt, Umschalt+Punkt für „:“, Rücktaste, Eingabe verbindet), z. B. `192.168.0.12` oder `127.0.0.1:7777`. Die letzte Adresse wird gemerkt.
- **Im Spiel:** Esc-Menü hat jetzt „Hauptmenü“. Ein Mitspieler trennt sich nur; verlässt der Gastgeber die Welt, landen alle wieder im Startmenü. Verbindungsfehler stehen dort in Orange (`UGamerSessionSubsystem`), z. B. „Der Gastgeber hat die Welt geschlossen.“ oder „Keine Antwort vom Gastgeber (Zeitüberschreitung).“
- **Starten:** `Spiel_starten.bat` (bzw. `tools/start_menu.ps1 -Windows 2`) öffnet Fenster im Startmenü ohne extra Server. `Spiel_testen.bat` startet weiter direkt mit separatem Server.
- **Getestet:** Gastgeber erstellt aus dem Startmenü eine Welt (auch mit geänderter Option „sofort fertig“), baut ein Fundament; ein zweites Spiel tritt über „Beitreten“ mit `127.0.0.1:7777` bei und sieht das Fundament; der Gastgeber geht ins Hauptmenü, beide landen im Startmenü, der Mitspieler mit Meldung (`tools/menu_test.ps1`, ohne Grafik). Mit Grafik angesehen: Startseite, „Welt erstellen“, „Beitreten“, Esc-Menü mit „Hauptmenü“, Fehlermeldung im Startmenü. Der bisherige Weg mit separatem Server und die Datentests laufen weiter.
- **Nicht getestet:** zwei echte PCs im Netz (Firewall-Freigabe für Port 7777 fragt Windows beim ersten Mal); Tippen mit echter Tastatur (simuliert getestet: Klicks und Tasten über denselben Eingabeweg); Listen-Server über längere Zeit. Es gibt noch keine Spielstände: Jede neue Welt beginnt leer.
- **Testwerkzeug:** `tools/menu_test.ps1` (Gastgeber + N Mitspieler durch das Startmenü, Schritte über `-GamerMenuScript`).

<a id="d4-9"></a>

### 4.9 Stand Phase 1, Schritt 5 (2026-11-13): Dorfbewohner
- **Siedlung** (`AGamerSettlement`, eine pro Welt, vom Server angelegt): **ein Lager** – jedes fertige Lagergebäude ist ein Zugang zum selben Bestand, je Lagergebäude 500 Gegenstände Platz. **Zuzug:** Jedes freie Bett (Strohlager) zieht nach 0,1 Spieltagen (3 min) einen Bewohner an; er bekommt das Bett. Namen im altnordischen Stil. Nachrichten an alle Spieler („Neu in der Siedlung: Astrid“).
- **Bewohner** (`AGamerVillager`, Entscheidungen nur auf dem Server): Bedürfnisse statt Uhr – Hunger (isst das beste Essen aus dem Lager), Schlaf (im eigenen Bett, sonst langsamer auf dem Boden), Unterkunft (Bett unter einem Dach, sonst 10 % langsamer). Reihenfolge: Essen → Schlafen → volle Hände ins Lager → eigener Beruf → Ausweich-Beruf Sammler → warten. Dringende Bedürfnisse unterbrechen die Arbeit; eine Aufgabe, die über 45 s dauert, wird aufgegeben (Sicherheitsnetz gegen Hängenbleiben). Leerer Magen 2 Spieltage lang → verlässt die Siedlung (Entscheidung: Hunger führt zum Weggehen).
- **Berufe** (Dorfverwaltung, Taste **V**): Sammler (Ausweich-Beruf, von Hand), Holzfäller (Holzfällerplatz + Axt aus dem Lager), Steinmetz (Steinmetzplatz + Spitzhacke), Koch (Lagerfeuer, Mahlzeiten aus Beeren, bis 10 im Lager), Baumeister (Hammer, hämmert Baupläne der Spieler; bei „Material beim Bauen“ holt er das Material aus dem Lager). Werkzeuge holt der Bewohner nur, wenn es Arbeit dafür gibt. Geht der Beruf nicht, steht der Grund in der Tätigkeit („sammelt Haselstrauch – Holzfäller: keine Axt im Lager“).
- **Lager für Spieler:** E am Lagergebäude öffnet das Lager (Stapel nehmen, einlagern, „Alles außer Werkzeug einlagern“).
- **Bewegung:** seit 2026-11-13 mit Unreal-Wegfindung (Abschnitt „Wegfindung der Bewohner“ unten); vorher gerade Linie mit Ausweichschritten.
- **Daten:** `jobs.json`, `needs.json` (Format: `docs/06_datenformat.md` 4.5/4.6). Startwerte Vorschlag: Hunger 1,0/Tag, Mahlzeit 0,8, Beeren 0,15 (Kochen lohnt sich), Schlaf 1,0/Tag, Erholung 6/Tag.
- **Testhilfen:** `-GamerVillagerTimeScale=30` am Server (1 Spieltag = 1 min), Testschritte `deposit`, `depositall`, `take`, `spawnvillager`, `setjob`, `villagers`, `settlement`, `menu:village|storage`.
- **Getestet** (Server + 2 Spieler ohne Grafik, Zeit ×30): Lager, Betten, Holzfällerplatz, Lagerfeuer gebaut; 3 Bewohner kamen und bekamen Betten; Holzfällerin holte die Axt und fällte Kiefern und Birken; Köchin kochte Mahlzeiten; Baumeisterin holte den Hammer erst, als ein Bauplan da war, und baute die Wand fertig; alle aßen, schliefen, lieferten ins Lager; Unterkunft nur beim Bett unter dem Boden-Dach; eine Bewohnerin verließ wegen Hunger die Siedlung, ein neuer zog ins freie Bett; Spieler 2 sieht alles. Mit Grafik angesehen: Bewohner mit Namen und Tätigkeit, Dorfverwaltung, Lagerfenster.
- **Beim Testen gefunden und behoben:** Baumeisterin holte und brachte den Hammer endlos (jetzt Werkzeug nur bei vorhandener Arbeit); ein Bewohner hing über eine Minute fest (Zeitgrenze, dringende Bedürfnisse unterbrechen).
- **Offen / bekannt:** Bewohner essen Beeren roh, bevor der Koch sie verkocht (später: Vorratsregeln, z. B. „Beeren für den Koch zurückhalten“). Farben der Platzhalter (Bewohner-Berufe, Spieler) greifen nicht – das Engine-Material hat den Farbparameter nicht (schon vorher so, betrifft nur Platzhalter). Bewohner werden nicht gespeichert. Steinmetz und Bewegung um Häuser herum nur kurz bzw. nicht getestet. Nicht mit echter Maus gespielt.

<a id="d4-10"></a>

### 4.10 Wegfindung der Bewohner und Maus-Fehler nach dem Startmenü (2026-11-13)
- **Wegfindung** (Entscheidung: Nutzer „Ja, Wegfindung“, mit Freigabe für `Gamer.Build.cs` und `DefaultEngine.ini`): Bewohner laufen auf dem Unreal-Navigationsnetz um Häuser, Bäume und Felsen herum und durch Türöffnungen. Das Netz entsteht nur rund um die Bewohner (30 m, Abbau ab 50 m) und wird im Spiel laufend neu gebaut, also auch für neu gebaute Häuser. Nur auf dem Server.
  - `UGamerNavigationSystem`: feste Grenzen 1 km × 1 km um den Ursprung, Höhe −50 m bis +100 m (einstellbar über `NavigableHalfSize`, `NavigableMinZ`, `NavigableMaxZ`). Grund: Die Karten haben kein NavMeshBoundsVolume, und die Engine-Variante „ganze Welt begehbar“ nimmt die Höhe vom flachen Boden, sodass Wände fehlen würden.
  - Die Engine-Vorlagenkarte `Template_Default` legt ihre Navigations-Klasse selbst fest und ignoriert die Einstellung in `DefaultEngine.ini`. `AGamerGameMode::InitGame` tauscht deshalb auf dem Server die Navigation gegen unsere Klasse (die Engine richtet die Navigation erst nach `InitGame` für das Spiel ein).
  - `AGamerVillager`: Navigations-Invoker, Weg aus dem Netz (auch Teilwege, weil Ziele wie Bäume oder das Lager selbst nicht begehbar sind), neue Wegsuche bei neuem Ziel, alle 3 s und nach Hängenbleiben; nach dem letzten Wegpunkt gerade zum Ziel. Ohne Netz (z. B. die erste Sekunde nach der Ankunft) wie bisher gerade Linie mit Ausweichschritten. `Describe()` zählt Wege (`paths=`, `nopath=`).
  - Einträge in `DefaultEngine.ini`: `NavigationSystemClassName=/Script/Gamer.GamerNavigationSystem`, `bGenerateNavigationOnlyAroundNavigationInvokers=True`, `RuntimeGeneration=Dynamic`; `Gamer.Build.cs`: `AIModule`, `NavigationSystem`.
- **Getestet** (Server + 1 Spieler ohne Grafik, Zeit ×60): Haus aus Fundament, drei Wänden und einer Türwand auf der vom Lager abgewandten Seite, das einzige Bett drinnen. Vorher (ohne Netz): Die Bewohnerin kam einmal ins Bett, blieb danach im Haus hängen, gab das Essen auf, verhungerte und verließ die Siedlung. Jetzt: 57 Wege berechnet, der Weg vom Lager ins Bett hat 5 Punkte (ums Haus, durch die Tür), vier Schlaf-Essen-Runden ohne „kommt nicht hin“ oder Aufgeben; einmal kein Weg direkt nach der Ankunft (Netz noch im Aufbau). Startmenü-Test (Gastgeber + Mitspieler) danach weiter bestanden.
- **Maus-Fehler behoben** (Spieltest-Rückmeldung: „kann mich mit der Maus nicht umschauen, Figur nur gerade steuern“): Das Startmenü gibt die Maus frei (nur bei gedrückter Taste eingefangen). Diese Einstellung gehört zum Spielfenster und blieb nach dem Wechsel in die Welt bestehen. Jetzt holt `AGamerPlayerController::BeginPlay` in der Welt die Maus zurück (`FInputModeGameOnly`). Geprüft über das Protokoll (Fangmodus 1 = dauerhaft, bei Gastgeber und Mitspieler); mit echter Maus noch nicht nachgespielt.
- **Testwerkzeug:** `tools/mp_test.ps1` startet jetzt standardmäßig auf `Template_Default` (auf `Entry`, dem Startmenü, gibt es keine Spielfigur, Testskripte liefen dort nicht). Wege im Protokoll: Server mit `-ini:Engine:[Core.Log]:LogGamer=Verbose` starten.
- **Offen / bekannt (Wegfindung):** Türen lassen sich nicht öffnen; eine eingesetzte Tür ist für Bewohner (und Spieler) eine Wand. Das Netz rechnet mit der Standardgröße der Engine (Radius 34 cm, Höhe 144 cm) statt mit der Bewohnergröße (40 cm, 184 cm); bei Durchgängen unter 1,84 m Höhe könnte ein Bewohner hängen bleiben. Das Netz behandelt das Bett (35 cm) als begehbar. Nicht getestet: mehrere Bewohner gleichzeitig im Haus, mehrstöckige Häuser, sehr weite Wege (> 30 m), Leistung mit vielen Bewohnern, paketierter Server.

<a id="d4-11"></a>

### 4.11 Vogelperspektive, erste Fassung (2026-11-13)
- Auftrag: Nutzer „Erst Vogelperspektive“ (vor den Grafik-Einstellungen). Funktionsumfang = Vorschlag von Claude, als Kommandoansicht; Bauen von oben noch nicht.
- **Bedienung:** Taste **B** (unter „Steuerung“ umbelegbar) schaltet um, B oder Esc zurück. Lauftasten verschieben die Kamera, Mausrad zoomt (8–80 m), rechte Maustaste halten dreht und neigt, Linksklick wählt einen Bewohner, ein Gebäude oder ein Vorkommen aus. Unten links stehen die Infos (Bewohner: Beruf, Tätigkeit, Hunger, Schlaf, Unterkunft, Werkzeug; Gebäude: Bauzustand, Lagerstand, wessen Bett, für welchen Beruf; Vorkommen: bereit oder abgeerntet), bei Bewohnern mit „Beruf wechseln“. V öffnet die Dorfverwaltung, schließen führt zurück in die Vogelperspektive. Über den Bewohnern stehen Name und Beruf (die 3D-Schilder sind dabei ausgeblendet).
- **Technik:** nur beim jeweiligen Spieler (`AGamerPlayerController`, eigene `ACameraActor`, nichts wird übertragen); die Figur steht, das Spiel läuft weiter. Die Kamera bleibt höchstens 120 m von der eigenen Figur entfernt (kein freies Erkunden im Koop). Bewohner werden über ihre Position auf dem Bildschirm gewählt (45 Pixel bei 1080p), Gebäude und Vorkommen über einen Strahl. Den Beruf ändert weiter der Server (`ServerSetVillagerJob`).
- **Getestet:** ohne Grafik (Server + 1 Spieler): Umschalten über die echte Tastenverarbeitung (B an, B aus, Esc aus), Kameraposition stimmt mit der Rechnung, Blickziel wechselt zwischen Kamera und Figur, Mauszeiger an/aus. Mit Fenster (960×540, 4 Bilder): Bewohnerin und Lager per Bildschirmposition getroffen, „Beruf wechseln“ machte sie auf dem Server zur Baumeisterin, Zoom auf 65 m und Drehung, Rückkehr zur Figur mit Hilfezeile „B Vogelperspektive“.
- **Testschritte:** `bird:on|off`, `birdfocus:<Ziel>`, `birdzoom:<cm>`, `birdturn:<Gier>,<Neigung>`, `birdselect:<Ziel>` (klickt auf die Bildschirmposition), `birdinfo`; Ziel = `villager:<n>` oder eine Bauteil-ID. Auswahl braucht ein Fenster (ohne Grafik gibt es keine Bildschirmpositionen).
- **Offen / bekannt:** Ganz herausgezoomt überlappen sich Namensschilder. Die Kamera folgt der Höhe unter dem Bildmittelpunkt, auf Dächern also etwas höher. Gamepad hat keine Taste für die Vogelperspektive. Bauen von oben, Befehle an Bewohner (z. B. „geh dorthin“) und Markierungen auf dem Boden fehlen. Nicht mit echter Maus gespielt.

<a id="d4-12"></a>

### 4.12 Grafik-Einstellungen (2026-11-13)
- Auftrag: Nutzer „Grafik-Einstellungen“. Seite **Grafik** im Esc-Menü und im Startmenü (neben „Steuerung“). Wert anklicken schaltet weiter, wirkt sofort und wird gespeichert.
- **Werte:** Qualität (Niedrig, Mittel, Hoch, Episch; „Gemischt“, wenn die Bereiche verschieden stehen), Anzeige (Fenster oder Vollbild randlos), Fenstergröße (nur im Fenster, passende Größen dieses Bildschirms), Render-Auflösung (Automatisch, 50, 58, 67, 100 %, die Vorgaben der Engine, wird hochgerechnet), Bildrate begrenzen (unbegrenzt, 30, 60, 90, 120, 144), V-Sync, Schriftgröße (80–160 %). Dazu die gemessene Bildrate und die Knöpfe „Automatisch einstellen“ (Leistungstest der Engine), „Standard“ (Qualität Hoch, Render-Auflösung automatisch, keine Grenze, V-Sync an, Schrift 100 %; die Anzeige bleibt) und „Zurück“.
- **Technik:** `GamerGraphicsSettings.h/.cpp` auf Basis von Unreals `UGameUserSettings` (pro Benutzer in `Saved/Config/<Plattform>/GameUserSettings.ini`, die Engine lädt sie beim Start selbst); die Schriftgröße steht in `UGamerMenuSettings` (`HudScale`). Qualität und Render-Auflösung sind getrennt (die Qualitätsstufen der Engine würden sonst die Auflösung mitändern). Exklusiver Vollbild wird nicht angeboten (ein falscher Modus kann ein schwarzes Bild geben); randloser Vollbild nutzt immer die Bildschirmauflösung. Keine Kerndatei geändert.
- **Startskript:** `Spiel_starten.bat` (`tools/start_menu.ps1`) gibt bei einem Fenster keine feste Größe mehr vor, sonst wären Anzeige und Fenstergröße wirkungslos. Bei mehreren Fenstern (lokaler Koop-Test) bleiben es kleine Fenster nebeneinander. Folge: Das nächste Spiel startet mit der gespeicherten Einstellung (beim Nutzer zuletzt randloser Vollbild).
- **Getestet:** ohne Grafik: alle Werte einmal durchgeschaltet und in der Datei gespeichert gefunden; Bildratengrenze wirkt sofort (gemessen 30, dann 60); Wechsel randlos → Fenster wählt 1600×1000, Fenstergröße schaltet weiter; „Standard“ und „Automatisch“ (Ergebnis „Gemischt“). Mit Fenster (3 Bilder): Seite lesbar, Schriftgröße 140 %, Qualität Hoch, zurück ins Spiel. Startmenü-Test: Grafik → Esc → Grafik → Zurück → Welt erstellen, Mitspieler tritt bei; in der Welt Esc-Menü → Grafik → Esc zurück. Die Benutzerdatei des Nutzers wurde vor den Tests gesichert und danach wiederhergestellt.
- **Offen / bekannt:** Die Wirkung der Schriftgröße auf die Spielanzeige nur im Protokoll geprüft, nicht im Bild (im kleinen Testfenster verkleinert sich die Menüseite selbst wieder). Kein Rückfrage-Countdown nach Anzeige-Wechseln (nur sichere Modi). Einzelne Bereiche (Schatten, Texturen …) nicht einzeln einstellbar. Gamepad bedient das Menü nicht. Toneinstellungen folgen mit den ersten Geräuschen.

<a id="d4-13"></a>

### 4.13 Vegetation fotorealistisch, erste Fassung (2026-11-13)
- Entscheidungen (Nutzer): kein Geld für Asset-Dienste (Meshy nicht abgeschlossen), Vegetation fotorealistisch. Lizenz- und Werkzeugvergleich: `docs/07_ki3d_dienste.md`; Design-Briefing für den Gesamtstil: `design/briefing_fotorealismus.md`.
- **Quellen (CC0, kostenlos):** `tools/cc0/fetch_cc0.py` lädt nach Freigabe 9 Pakete von Poly Haven und ambientCG (ca. 66 MB) nach `D:\gamer\cc0_quellen\` (außerhalb des Repos, Prüfsummen, Quellenliste `sources.json`). Ins Repo kommen nur aufbereitete Modelle und Texturen (LFS).
- **Fotoscans** (`tools/cc0/nature.json` + `tools/blender/process_nature.py`): kleiner, mittlerer, großer Fels, lose Steine und Geröll aus einem bemoosten Felsen-Set, Äste, Brennnessel (voll und abgeschnitten); Texturen auf Unreal umgerechnet (Normal DirectX, ORM), Brennnessel mit Transparenz.
- **Generator** (`tools/blender/build_trees.py`): Kiefer, Birke (je ausgewachsen, jung, Setzling, Stumpf), Wildapfelbaum, Haselstrauch, Himbeerstrauch (je mit und ohne Früchte). Verzweigung nach Weber und Penn, Rinde aus CC0-Fotos (Birke erzeugt), Blatt- und Nadelbüschel-Texturen mit artgerechter Blattform erzeugt (Oberfläche aus dem CC0-Blattfoto), Früchte als kleine Modelle außen an der Krone, Kollision nur am Stamm (Sträucher: Hülle um den Busch, blockiert nicht). Vorschau ohne Unreal: `tools/blender/preview_nature.py`.
- **Unreal:** Import liest zusätzlich `Assets/manifest_nature.json`; neues Grundmaterial `M_Gamer_Foliage` (maskiert, doppelseitig). Vorkommen zeigen die Modelle mit zufälliger Drehung und ±15 % Größe; **sichtbares Nachwachsen**: gefällter Baum = Stumpf, dann Setzling, junger Baum, wieder fällbar (Datenfeld `meshes.growing`).
- **Getestet:** Datentests 3/3 (alle Modellpfade gültig). Mit Fenster: Bäume, Sträucher, Felsen, Äste, Brennnesseln in der Welt mit Schatten und durchbrochenem Laub; Kiefer gefällt, Server-Protokoll: Stufe 1 (Setzling), Stufe 2 (junger Baum), nachgewachsen (Nachwachszeit für den Test auf 11 s verkürzt).
- **Offen / bekannt:** Noch nicht fotorealistisch, eher gutes Spielniveau (bessere Blattfotos, Wind, Lichtdurchlässigkeit und LODs fehlen). Boden ist noch das Karomuster der Testkarte; Gras fehlt; Pilzstelle ist Platzhalter; Birkenblätter aus der Nähe zu groß; Birkenrinde erzeugt statt Foto; Bilder der Wachstumsstufen im Spiel fehlen (Kamera war verdeckt), belegt nur über das Protokoll.

<a id="d4-14"></a>

### 4.14 Weltkarte, PCG-Vegetation, Jahreszeiten, Totholz (2026-11-13/14)

- **Eigene Weltkarte** `/Game/Gamer/Maps/World` (Kopie von Template_Default, Boden 250 m × 250 m), gebaut mit `tools/build_world.ps1` (Python-Commandlet + PCG-Builder-Commandlet, wiederholbar). „Welt erstellen“, `mp_test.ps1` und `play_local.ps1` nutzen sie. PCG-Plugin in `Gamer.uproject` (freigegeben). `ServerDefaultMap` in `DefaultEngine.ini` zeigt noch auf Template_Default (Kerndatei, nicht geändert).
- **PCG** (`PCG_WorldVegetation`, Methoden aus den Tutorials, Übersicht `docs/08_umgebung_methoden.md`): Grasflecken nach der Saatpunkt-Methode (≈ 87 000 Büschel aus eigenem CC0-Gras), Farne auf Moosflächen (≈ 4 900), drei Sandstein-Felswände am Rand. Erzeugt im Editor und mit der Karte gespeichert (11,3 MB). „World Ray Hit Query“ liefert im Builder keine Punkte, daher Punkt-Raster auf Z=0. Kiesel/Zweige als Deko wieder entfernt (sahen aus wie Aufsammelbares).
- **Boden:** `M_Gamer_Ground` aus fünf CC0-Bodentexturen (Welt-Koordinaten, Makro-Variation), Flecken passend zu Gras und Farn.
- **Pflanzen-Material** (`M_Gamer_Foliage`, `docs/09_vegetation_material.md`): Two Sided Foliage, Streuung je Instanz/Objekt, Jahreszeiten über `MPC_GamerWorld.SeasonPhase`. Ursache des grauen Grases: fehlende Instanz-Freigabe am Material.
- **Jahreszeit im Spiel:** Server-autoritativ in `AGamerGameState`, 60 min je Jahreszeit (Weltoption `?SeasonMinutes=`), neue Welt im späten Frühling; Testschritt `season:<0..4>`.
- **Bäume neu** (`build_trees.py` v2): Waldkiefer 20 m mit zweifarbiger Rinde und Nadelbüscheln aus CC0-Zweigatlas, feineres Laub bei Birke, Apfel, Hasel, Himbeere; alle unter dem Dreiecksbudget.
- **Totholz erntbar:** `res_dead_trunk`, `res_dead_stump` (Axt), je 3 im Startgebiet.
- **Getestet (2026-11-14):** Datentests 3/3; Spiel mit Fenster: Totholz und Stumpf mit der Axt geerntet (+2 Baumstamm +2 Äste bzw. +1/+1), Jahreszeiten 0,5–3,4 per Testschritt angesehen, Bilder von Boden, Gras, Farnen, Bäumen.
- **Offen / bekannt:** Birkenlaub im Spiel etwas zu hell; Äpfel und Haselnüsse in den dichteren Kronen schlecht sichtbar; Früchte bleiben im Winter hängen; kein LOD-Import; Leistung (87 000 Gras-Instanzen, Boden-Shader) nicht gemessen; langsamer Jahreszeitenwechsel über eine Stunde nicht beobachtet; Kollision an Felswand/Totholz nicht begangen; Boden von oben noch leicht gekachelt; Start-Menüzeile für die Jahreszeit fehlt; echte Landscape fehlt.

<a id="d4-15"></a>

### 4.15 Spieler-Wünsche vom 2026-11-14: Sprint, Werkzeugwahl, Mausrad, Bauen (umgesetzt) und Plan für den Rest
Auftrag: Wunschliste des Nutzers (9 Punkte). Entscheidungen dazu in `docs/01_entscheidungen.md` (Zeilen vom 2026-11-14). Punkte 1–4 sind umgesetzt, 5–9 hier eingeplant.

#### Umgesetzt
- **Sprint** (`AGamerCharacter`): Umschalt links halten (Gamepad: linken Stick drücken), 750 statt 450 cm/s, Wert `SprintSpeed` (Config, Standard im Code). Keine Ausdauer (Regel 5). Der Spieler schaltet sofort um, der Server bekommt es per RPC und gibt es an die anderen weiter. Öffnet sich ein Menü, endet der Sprint.
- **Automatische Werkzeugwahl:** Schlagen nimmt das beste passende Werkzeug aus der Schnellleiste in die Hand (Axt am Baum und Totholz, Spitzhacke am Fels, Hammer an einem unfertigen Bauplan). Der Spieler wählt es selbst vor dem Schlag, der Server macht dasselbe noch einmal als Absicherung (`UGamerInventoryComponent::SelectBestToolInHotbar`). Werkzeuge im Rucksack werden nicht geholt. Die Anzeige unter dem Fadenkreuz sagt „nimmt Steinaxt in die Hand“ statt „Taste 1: … in die Hand nehmen“.
- **Mausrad:** runter = nächster Platz, hoch = voriger, rundum, auch über leere Plätze; Gamepad Steuerkreuz links/rechts. In der Vogelperspektive zoomt das Mausrad weiter (dort hat es Vorrang).
- **Bauen:** Nach dem Platzieren ist das Bauteil abgestellt, die Vorschau verschwindet, der Hammer bleibt in der Hand. Mit gehaltener Umschalttaste bleibt das Bauteil gewählt (Mehrfachbau). Die Bauzeile zeigt „Material reicht für N“ (bzw. „für keins mehr“ / „ohne Kosten“) und den Hinweis „Umschalt halten: mehrere bauen“.
- **Steuerung:** vier neue umbelegbare Aktionen (`sprint`, `build_multi`, `hotbar_next`, `hotbar_prev`). Sprint und Mehrfachbau teilen sich absichtlich Umschalt und werden beim Umbelegen nicht gegeneinander getauscht. Hilfezeile oben nennt Sprint und Mausrad.
- **Testschritte:** `sprint:on|off`, `wheel:1|-1`, `hand` (was ist in der Hand, Tempo), `autoharvest:<Vorkommen>` (wie `harvest`, aber ohne Werkzeug vorzuwählen), `place:<id>@x,y+multi`.
- **Getestet** (`tools/mp_test.ps1`, Server + 2 Spieler ohne Grafik, PASS): Mausrad 1→2→1→8 (rundum)→1; mit dem Hammer in der Hand eine Kiefer geschlagen: Server nahm die Steinaxt, Baum gefällt; dann mit dem Hammer an einen Fels: Server nahm die Spitzhacke; Sprint an (Spieler und Server 750), aus (450); Fundament ohne Umschalt gesetzt → nicht mehr gewählt, mit „+multi“ → bleibt gewählt; „Material reicht für“ 5 → 4 nach einem Bauplan. Mit Fenster (1 Spieler, 2 Bilder): Hilfezeile, Bauzeile mit „Material reicht für keins mehr“ und Seite „Steuerung“ mit den neuen Zeilen passen auf 960×540.
- **Nicht geprüft:** echte Tasten und echtes Mausrad (die Tests rufen dieselben Funktionen direkt auf); ob Spieler 2 den Sprint flüssig sieht und ob es beim Umschalten kurze Rückstell-Ruckler gibt (Tempo-Wechsel läuft per RPC, nicht über die Bewegungsvorhersage der Engine – bei hoher Latenz möglich; falls störend, später als Flag in der Bewegungskomponente); automatischer Hammer an einem Bauplan (Pfad ist da, nur ohne Fadenkreuz-Ziel im Test nicht auslösbar); eine bessere Axt (T2) wählen – es gibt bisher nur T1-Werkzeuge.
- Keine Kerndatei geändert. `SprintSpeed` lässt sich später in `DefaultGame.ini` setzen (Kerndatei, nur mit Rückfrage).

#### Plan für die übrigen Wünsche (Reihenfolge = Vorschlag)
1. **Verderb mit Hinweisen (jetzt, Session „Obst, Saatgut und Feldbau“).** Auslegung geklärt (2026-11-14): trockene Lagerung verlängert nur, auch Gekochtes verdirbt (deutlich langsamer), höhere Stufe hält länger, Dauern in Spieltagen. Kern (Haltbarkeit pro Platz, `spoil_days`, `item_spoiled_food`, Hinweis bei 30 %, Anfänger-Tipp) baut die Feldbau-Session; der Haltbarkeitsbalken am Inventarplatz ist noch nicht vergeben.
2. **Anzeigen.** Bewohner: Hunger, Schlaf, Stimmung als Symbol mit Leiste in Vogelperspektive und Dorfverwaltung; Stimmung = neuer Wert aus Hunger, Schlaf, Unterkunft (später Arbeit, Eigenschaften). Achtung Regel 7 „3 Bedürfnisse im Slice“: Stimmung ist kein viertes Bedürfnis, sondern ein abgeleiteter Wert. Spieler: „Satt“-Anzeige mit Leiste; braucht das Essens-Buff-System (`food` in `docs/06_datenformat.md`, bisher „ab Phase 2“) – Vorschlag: einfache erste Fassung (ein Buff „satt“: +Regeneration) noch in Phase 1, volle Buffs in Phase 2.
3. **Icons aus den 3D-Modellen.** Blender-Skript rendert jedes Modell aus `Assets/` (gleiche Kamera, gleiches Licht, transparenter Hintergrund, 256×256), Import als Texturen, Datenfeld `icon` je Item/Bauteil; Platzhalter für Gegenstände ohne Modell (Essen hat teils noch keins). Keine KI, kein Eintrag im Asset-Log nötig (Modelle aus KI-3D-Diensten sind dort bereits eingetragen). Berührt `tools/blender/*` und `tools/unreal/*` – erst, wenn die Vegetations-Session dort fertig ist.
4. **Herstellungsmenü mit Bildern.** Kacheln mit Icon, Name, Zutaten (fehlende in Orange), Filter nach Station und Kategorie, herstellbare zuerst; Zahlentasten wählen weiter. Hängt an 3. Das Baumenü bekommt dieselben Kacheln.
5. **Feldbau, Komposter, Regenfänger, Wasser und Dünger: jetzt** (Nutzer 2026-11-14: „Ja, alles jetzt“, ersetzt den Vorschlag „Phase 2“). Umsetzung: Session „Obst, Saatgut und Feldbau“. Regenfänger nach Vorschlag ChatGPT: 1 Baumstamm, 1 großer Stein, 3 kleine Steine, 5 Äste („großer Stein“ gibt es als Item noch nicht).
   **Stand 2026-11-14 abends:** gebaut – Verderb (Inventar, Lager, Boden, 30-%-Hinweis, Anfänger-Tipp), Obst-Zyklus Apfel/Birne mit Fallobst und Druckstellen, sechs Wildpflanzen mit Jahreszyklus, Saatgut aus Wildvorkommen und Ernte, Felder zum Aufziehen, Komposter, Regenfänger, Hof mit Beruf Bauer, Ausbaustufen/Arbeitsplätze für alle Arbeitsgebäude. Modelle per `build_trees.py`. Obstbäume aus Kernen über Setzlinge selbst aufziehen. Offen: Haltbarkeitsbalken im HUD, Modelle für Feld/Komposter/Regenfänger/Hof, Durst. Details `docs/10_nahrung_feldbau.md`.

<a id="d4-16"></a>

### 4.16 Charakterauswahl, Charaktererstellung, Charaktereditor (2026-11-14)
- **Daten:** `game/Content/Data/characters.json` (Regeln, 5 Werte, 24 Fähigkeiten in 5 Zweigen, 9 Aussehens-Bereiche, Namensvorschläge, 5 Herkünfte). Prüfung in `GamerCharacterData.cpp` (Summe/Grenzen der Werte, Startausrüstung gegen `items.json`, Startfähigkeit im eigenen Zweig, Aussehens-Optionen, Tippfehler in Feldnamen). Tests `Gamer.Data.CharactersAreValid` und `Gamer.Data.CharacterValidatorFindsErrors` (laufen mit `tools/test_data.ps1`).
- **Neue Gegenstände:** Kategorie `weapon` (ID `weapon_…`, Block `weapon`: type, damage, durability) mit Kurzbogen, Dolch, Bartaxt, Rundschild, Hackmesser, Runenstab; dazu Pfeile, Schmiedehammer (Hammer), Sichel. Kämpfen geht damit noch nicht. Gegenstände, die es nur als Startausrüstung gibt, meldet die Datenprüfung als Warnung.
- **Startmenü:** „Welt erstellen“/„Beitreten“ öffnen zuerst die Seite „Charakter“ (Herkunft, Name mit Zufallsname, Werte als Balken, Startausrüstung, Startfähigkeit, eigener Zweig, Aussehen) und „Aussehen bearbeiten“ (Wert anklicken = nächste Möglichkeit). „Weiter“ speichert lokal (`GameUserSettings.ini`, `[/Script/Gamer.GamerCharacterSettings]`). Code: `GamerHUDCharacter.cpp`, `GamerPlayerControllerCharacter.cpp`.
- **Server:** neuer `AGamerPlayerState`. Der eigene Client schickt seine Auswahl (`ServerSetCharacter`), der Server korrigiert ungültige Angaben, nimmt Werte, Startfähigkeit und Startausrüstung aus seinen eigenen Daten, setzt den Spielernamen und legt die Ausrüstung einmal ins Inventar. Werte und Name sind für alle sichtbar (Esc-Menü zeigt den eigenen Charakter). Test ohne Menü: `-GamerCharacter=origin_warrior:Ragnar`.
- **Getestet (2026-11-14):** Datentests grün (5/5). Dedicated Server + 2 Clients (`mp_test.ps1`, Client 1 mit `-GamerCharacter=origin_warrior:Ragnar`): Server übernimmt beide, korrigiert das leere Profil von Client 2 auf die erste Herkunft. Solo über das Startmenü mit Grafik (`menu_test.ps1 -Clients 0 -Render`, Bildschirmfotos der Seiten „Charakter“ und „Aussehen“ und des Esc-Menüs geprüft). Host + 1 Client über das Startmenü (Seherin + Bauer), beide mit eigener Startausrüstung. Dabei gefunden und behoben: als Host/Solo kam die Startausrüstung, bevor das Inventar bereit war.
- **Achtung:** Die Menütests speichern ihren Testcharakter in die echte `GameUserSettings.ini` dieses PCs (wie schon Adresse und Weltoptionen).
- **Noch nicht:** Wirkung der Werte (kommt mit Kampf, Handwerk, Magie), Fähigkeiten lernen (Punkte, Menü), Figurenmodell (MetaHuman, braucht Plugin-Freigabe), mehrere Charaktere pro Spieler, Umlaute im Namensfeld (Tastatur-Eingabe nur A–Z, Leerzeichen, Bindestrich), Speichern des Fortschritts (Startausrüstung kommt ohne Weltspeicherung bei jedem Beitritt neu).

<a id="d4-17"></a>

### 4.17 Taverne, Kochstellen, Gerichte mit Buffs (2026-11-15)

- Taverne mit 3 Ausbaustufen (Arbeitsplätze Koch und Wirt 1 → 2 → 3), Vorrat für 16 Gerichte und Getränke; Kochstellen Herdstelle (Stufe 1), Lehmbackofen (2), Braukessel und Festtafel (3), nur in 15 m Umkreis baubar.
- 19 Gerichte und Getränke T1–T3, T5, T6 mit Buffs (`items.json` `food`), neue Zutaten Gerste, Hafer, Honig. T4 und die Fleisch-, Fisch- und Milchgerichte folgen mit diesen Zutaten.
- Buff-System server-autoritativ (`UGamerBuffComponent`): höchstens 3 Mahlzeiten, Restzeit, Übertragung an den eigenen Spieler; Essen per Linksklick mit dem Gericht in der Hand.
- Bewohner: Koch kocht an den Kochstellen bei der Taverne, Wirt füllt die Taverne, Bewohner essen und trinken dort (Stimmungswert vorbereitet, Anzeige baut die Gameplay-Session).
- **Test bestanden:** Datentests 6/6; Server + 2 Spieler und Server + 1 Spieler mit Testskript (Bauplatz-Regel, Kochen, Essen, Buff-Ablauf, Ausbau, Koch, Wirt, Essen in der Taverne). Nicht getestet: echter Linksklick, Kochen der Bewohner an Herdstelle/Ofen/Braukessel/Festtafel.
- Offen: Buff-Icons im HUD (Gameplay-Session), Wirkung der Buffs (es gibt noch keine Lebenspunkte, Traglast-Grenze, Kälte, keinen Kampf), Modelle und Icons, Verderb im Taverne-Vorrat. Details: `docs/12_taverne_gerichte.md`.

<a id="d4-18"></a>

### 4.18 Spieler-Wünsche, zweite Runde (2026-11-15)
Umgesetzt, gebaut (`GamerEditor`, 0 Fehler) und getestet (`tools/mp_test.ps1`: Solo mit Grafik und Server + 2 Spieler ohne Grafik, beide PASS; Bilder angesehen):
- **Stationsfenster:** E an einer fertigen Station mit Rezepten (Werkbank, Lagerfeuer, Kochstellen der Taverne-Session) öffnet oben rechts ein Fenster mit genau deren Rezepten, E oder Tab schließt, es schließt sich beim Weggehen. Hinweis am Fadenkreuz „E: Werkbank benutzen (herstellen)“. Tab bleibt für Handwerk und Stationen in der Nähe.
- **Umrandung und Haufen:** Das anvisierte Ding bekommt eine Rahmen-Umrandung (Rechteck um seine Bounds, kein Post-Process). Liegen aufhebbare Dinge (Pflanzen, Äste, Bodengegenstände) bis 1,5 m beieinander, erscheint rechts vom Fadenkreuz eine Liste; F wechselt, E nimmt das Gewählte (Taste umbelegbar, Gamepad Steuerkreuz hoch).
- **Alles aufnehmen:** E halten (0,5 s) oder Umschalt + E, siehe `docs/01_entscheidungen.md`. Bodengegenstände sofort, Pflanzen nacheinander über eine Warteschlange (jeweils mit Arbeitsbalken). Getestet: 2 Pflanzen nacheinander, 0 Bodengegenstände (es lagen keine).
- **Umgebungs-Scan:** Z, 40 m, 8 s, 12 s Abklingzeit, Markierungen mit Farbe und Buchstabe durch Wände, Zähler am rechten Rand; Werte in `game/Content/Data/scan.json`. Testwelt: 80 Markierungen. Der Jäger-Bonus (60 m, Tiere) steht in den Daten, wirkt aber noch nicht (keine Herkünfte, keine Tiere).
- **Anzeigen:** Dorfverwaltung zeigt Hunger, Schlaf und Stimmung mit Symbol und Leiste; „Satt“-Anzeige und aktive Mahlzeiten links über der Schnellleiste; Frische-Balken an Schnellleistenplätzen mit Verderblichem (Bild des Balkens nicht gesehen, es lag nichts Verderbliches im Test).
- **Neue Testschritte:** `scan`, `station:<id>`, `pile`, `focusnext`, `pickall`, `picksame`.

Nicht geprüft / offen:
- Echte Tasten (F, Z, E halten, Umschalt+E) und die Umrandung an Bäumen und großen Dingen mit der echten Maus; Umrandung bei halb verdeckten Objekten (Rechteck um alle Ecken, fehlt, wenn eine Ecke hinter der Kamera liegt).
- Herstellungsmenü mit Kacheln und Bildern, und die Icons: noch nicht gebaut; sie brauchen `tools/blender` und `tools/unreal`, die der Vegetations-Session gehören. Bis dahin Platzhalter-Symbole (farbiges Quadrat mit Buchstabe).
- Verderb-Hinweise (30 %, Anfänger-Tipp): Kern und Meldung liefert die Feldbau-Session (`ClientSpoilNotice`), der Balken am Platz ist gebaut.
- Stimmung wirkt bisher nur als Anzeige; Folgen (z. B. Arbeitstempo, Weggehen) sind nicht entschieden.

<a id="d4-19"></a>

### 4.19 Icons aus den 3D-Modellen und Kacheln im Herstellungsmenü (2026-11-15)
- **Weg** (Entscheidung 2026-11-14): Icons werden aus den echten Modellen gerendert, keine KI-Bilder. `tools/blender/render_icons.py` liest `tools/blender/icon_map.json` (ID → FBX), rendert jedes Modell mit denselben Texturen (aus den Materialnamen), demselben Licht, demselben Blickwinkel (30° von oben, 35° gedreht) und 80 % Füllung, 256×256, transparent, und schreibt `Assets/UI/Icons/T_Icon_<id>.png` und `Assets/manifest_icons.json`. `tools/unreal/import_assets.py` (kleine Änderung) importiert sie nach `/Game/Gamer/UI/Icons/`. Aufruf: `E:\Blender\blender.exe -b --factory-startup -P tools/blender/render_icons.py -- Assets`, dann `tools/import_assets.ps1`.
- **Vorhanden: 16 Icons** – Baumstamm, Stein, Äste, Pflanzenfaser, Apfel, Birne und die 10 Holz-Bauteile. Alles andere hat (noch) kein Modell und bekommt im Spiel eine benannte Platzhalter-Kachel (farbiger Rahmen, die Farbe folgt aus der ID, und die ersten Buchstaben des Namens). Neue Modelle: in `icon_map.json` eintragen, Skript und Import neu laufen lassen.
- **Im Spiel** (`GamerIcons`, `AGamerHUD::DrawIconTile`): Schnellleiste zeigt das Icon statt des Namens (der volle Name steht über der Leiste), Verderbliches mit Frische-Balken; das Herstellungs- und Stationsfenster zeigt je Rezept eine Kachel mit Icon oder Platzhalter, daneben Name und Zutaten.
- **Getestet:** Bild mit Fenster: Apfel-Icon samt Frische-Balken in der Schnellleiste, Platzhalter „Br“ in der Kachel des Werkbank-Fensters. Beim ersten Import scheiterte das Speichern einer Textur (vermutlich gesperrte Datei, ein zweiter Lauf ging durch).
- **Nicht gemacht / offen:** Kacheln im Baumenü und in der Lagerliste (dort noch Text), Icons für Werkzeuge, Waffen, Rüstung, Deko, Essen und Werkstätten (es gibt keine Modelle; sobald sie da sind, nur die Zuordnung ergänzen), Umrandung der Icons im Stil „Gelebter Norden“ (nur Licht und Winkel vereinheitlicht), keine Auswahl per Maus im Herstellungsfenster (nur Zahlentasten).

<a id="d4-20"></a>

### 4.20 Speichersystem (2026-11-15)
- Auftrag: Nutzer „sehr wichtig ist, dass als Nächstes erstmal das Speichersystem erstellt wird, damit der erste Phasenplan abgeschlossen werden kann“. Entscheidungen/Vorschläge: `docs/01_entscheidungen.md`, Zeilen „Speichersystem: …“.
- **Code:** `GamerSaveable.h/.cpp` (`IGamerSaveable` + `FGamerSaveRecord`, JSON-Block mit Standardwerten), `GamerSaveSubsystem.h/.cpp` (Server: Laden in `AGamerGameMode::StartPlay`, Autosave, Speichern beim Verlassen/Beenden, Spieler-Zuordnung, Weltliste), `GamerSaveImpl.cpp` (Speichern/Laden der vorhandenen Klassen), `GamerHUDSave.cpp`/`GamerPlayerControllerSave.cpp` (Startmenü „Welt laden“, „Speichern“ im Esc-Menü), `GamerSaveTests.cpp`. Neue Systeme speichern, indem sie `IGamerSaveable` umsetzen; Komponenten an Spielfigur/PlayerState werden automatisch pro Spieler gespeichert.
- **Gespeichert:** Weltoptionen inkl. `SeasonMinutes`, Jahreszeit-Phase; alle Vorkommen (abgeerntet, Nachwachs- und Wachstums-Restzeit, Wachstumsstufe, Schlagfortschritt, Obst-Zyklus, gepflanzte Setzlinge); Bodengegenstände mit Frische und Verrottungszeit; Gebäude (Bauteil, Lage/Größe, Baufortschritt, eingesetztes Material, Schläge, Ausbaustufe), Felder (Saat, Wachstum, Wasser, Dünger), Komposter und Regenfänger, Taverne-Vorrat; Siedlung (Lager mit Frische, Zuzug, Namenszähler); Bewohner (Name, Beruf, Bett, Hunger, Schlaf, Stimmung, Taverne-Bonus, Dach, Werkzeug, Gepäck); Spieler pro Welt (Position, Blickrichtung, Inventar mit Frische, gewählter Schnellleistenplatz, aktive Mahlzeiten mit Restzeit).
- **Startmenü:** „Welt erstellen“ legt einen Spielstand an („Welt N“), „Welt laden“ listet Name, Speicherzeit, Spielzeit und Weltoptionen (6 neueste). Esc-Menü: „Speichern“.
- **Getestet:** Automation-Tests 10/10 grün (`tools/test_data.ps1 -Filter Gamer`; neu: `Gamer.Save.RecordRoundTrip`, `.FilesBackupAndList`, `.SchemaAndCompare`, `.WorldRoundTrip` mit den echten Spielklassen, auch unbekannte IDs). Dedicated Server + 2 Clients (`mp_test.ps1`, Port 7795): Welt verändert (Lager, Bett, Fundament, Bewohnerin zieht ein, Jahreszeit Herbst, Inventar), gespeichert per Testschritt und beim Beenden des Servers; neu gestartet → Rundreise-Prüfung `-GamerSaveVerify` **PASS (0 Unterschiede)**, beide Spieler mit Position und Inventar zurück, keine zweite Startausrüstung. Solo über das Startmenü: Welt erstellen → verändern → „Spiel beenden“ (speichert) → „Welt laden“ → Inventar identisch, „Speichern“ im Esc-Menü. Listen-Host lädt Welt, Mitspieler tritt übers Startmenü bei. Autosave (Test alle 10 s).
- **Beim Testen gefunden und behoben:** Obstbäume warfen beim Laden im Herbst sofort Fallobst ab (doppelte Bodengegenstände).
- **Inventar pro Welt oder mit dem Charakter** (Nutzer: „bei der Neuerstellung abfragen“): neue Zeile „Inventar“ unter „Welt erstellen“ (URL `?Inventory=world|character`). Bei „mit dem Charakter“ schickt der Client beim Beitritt das Inventar aus seiner Charakterdatei mit, der Server übernimmt es (keine zweite Startausrüstung), der Client hält die Datei danach aktuell. Getestet (Dedicated + 1 Client): Welt A sammelt 7 Steine → Charakterdatei; Welt B (auch „mit Charakter“) übernimmt sie; Welt C („pro Welt“) gibt nur die Startausrüstung. Menüzeile über das Startmenü umgeschaltet (Protokoll), nicht im Bild angesehen.
- **Nicht getestet / offen:** Seite „Welt laden“ nicht mit Grafik angesehen (nur über Testschritte bedient); zwei echte PCs; lange Koop-Sitzung mit mehreren Autosaves; große Welten (Speicherzeit heute 2–6 ms bei ~110 Objekten); Absturz während des Schreibens (abgesichert über Zwischendatei und Sicherungskopie, nicht provoziert); Spielstand löschen/umbenennen im Menü fehlt; Werkzeug-Haltbarkeit gibt es noch nicht (wird als Feld im Inventarplatz mitgespeichert, sobald es sie gibt). Zwei Spiele auf einem PC teilen sich Spieler-ID und Charakterdatei (Test: `-GamerPlayerId=`).

<a id="d4-21"></a>

### 4.21 Phase 2, Schritt 1: Spielfigur mit Animationen (2026-11-15)
- **Figur:** Zwischenfigur Epic Manny (Körperbau „kräftig“) bzw. Quinn (Körperbau „schmal“) auf dem UE5-Skelett ersetzt den Zylinder in `AGamerCharacter` und `AGamerVillager` (Bewohner: Figur nach Namen, Berufsfarbe als Tönung). Körperbau ist nicht an ein Geschlecht gebunden (Entscheidung 2026-11-13). Kleidungsfarbe aus dem Charaktereditor als Tönung „Paint Tint“ des Mannequin-Materials (im Paket-Test sichtbar). Profil, das nach der Figur ankommt, wird im Tick nachgezogen.
- **Animation in C++** (`GamerAnimInstance.*`, `GamerAnimTypes.h`, kein AnimBlueprint): Stehen/Gehen/Joggen/Sprint nach Tempo mit gemeinsamer Schrittphase, Sprung/Fallen/Landen, Arbeitsschicht (ganzer Körper im Stand, nur Oberkörper ab `spine_01` in Bewegung). Arbeitsschleifen aus dem replizierten Arbeitszustand (Sammeln, Herstellen), Einzelschläge (Hacken, Pickel, Hämmern, Essen, Aufheben) als replizierter Impuls `ActionPulse`; der handelnde Spieler spielt Schläge sofort lokal und bekommt sie vom Server nicht doppelt. Bewohner: Server setzt `AnimWork` (Hacken/Pickel/Sammeln/Hämmern/Herstellen/Feldarbeit/Essen), wenn sie am Ziel stehen. Beim Schlagen dreht sich die Figur zum Ziel. Dedicated Server berechnet keine Posen.
- **Animationen:** Epic-Mannequin (Stehen, Gehen, Joggen, Sprung, Fallen, Landen); Kevin Iglesias (Sprint, Sammeln, Pickel, Hämmern) und Quaternius UAL2 (Holzhacken, Säen, Ernten, Gießen, Essen) per IK-Retargeter übertragen. Werkzeug: `tools/import_characters.ps1` (kopiert die Mannequin-Inhalte aus den Engine-Vorlagen, importiert, baut IK-Rigs/Retargeter, schreibt `/Game/Gamer/Characters/Anims/A_*`; leert dabei jedes Mal diesen Ausgabeordner).
- **MetaHuman:** Plugin `MetaHumanCharacter` in `Gamer.uproject` eingeschaltet (Freigabe 2026-11-15). Eine MetaHuman-Figur ist **noch nicht** erstellt: Rig („Auto-Rigging“) und Texturen kommen nur über Epics Cloud-Dienst, dafür muss sich der Nutzer im Editor mit seinem Epic-Konto anmelden, und das Erstellen im MetaHuman Creator ist Editor-Handarbeit. Danach wird nur das Mesh getauscht (gleiches Skelett).
- **Detailstufen (Plan, nicht umgesetzt):** Spieler mit MetaHuman „High“ (Gesichts-Rig, Strähnen-Haar nur nah), Bewohner und Spieler auf Entfernung mit „Low“/UEFN-Low-Pipeline (Haar-Karten, wenige LODs, kein Gesichts-Rig), Ziel 8 GB Grafikspeicher.
- **Getestet (2026-11-15):** gebaut (GamerEditor); Datentests grün (6/6). `mp_test.ps1 -Render`: Server + 1 Client: Gehen, Sprint, Sprung, Holzhacken, Pickel mit Bildern; Server + 2 Clients: Client 2 sieht Client 1 am Baum hacken (14 replizierte Schläge abgespielt, Bilder). Neue Testbefehle `walk:<s>`, `jump`, `burst:<n>:<s>` (Bilder im Takt), `mp_test.ps1 -Client2Args`. Grob gemessen bei 960×540, Qualität Mittel, ohne Bewohner: 144 fps (Begrenzung), Grafikspeicher gesamt 3,1 GB (vorher 2,35 GB Grundlast).
- **Paket-Test (2026-11-15):** `build.ps1 Package` und `PackageServer` ohne Fehler (Cook: 0 Fehler, 1 fremde Warnung). Spielpaket 3,81 GB, davon 2,47 GB Debug-Symbole (`Gamer.pdb`, wird nicht ausgeliefert), Spielinhalte (`Gamer-Windows.ucas`) ca. 1,0 GB; Serverpaket 3,37 GB inkl. Symbole. Paketierter Server + paketiertes Spiel: Beitritt ok, Figur (Quinn, Körperbau schmal aus dem Profil, Kleidungstönung sichtbar) geht und sprintet im Bild, 7 Schläge am Baum laut Log; der Moment des Schlags ist im Paket-Lauf nicht im Bild (Bilder lagen vor dem Teleport zum Baum).
- **Nachtrag (Nutzerwunsch über die Gameplay-Session):** Von Hand sammeln und Gegenstände aufheben = Hocken und Greifen (`GatherLow`, Quaternius Farm_Harvest), Anfangswerkzeuge ohne Station herstellen = knieendes Werkeln (`Craft`), Herstellen an Stationen = stehende Arbeit (`CraftStation`); Grundlage `FGamerWorkState::bLowOrStation`. Knien beim Herstellen der Steinaxt im Bild geprüft.
- **Korrektur Sammeln (Nutzer: „immer noch keine greifende Bewegung“, „zu viel Abstand“):** Ursache: Hock-Greifen nutzte Quaternius Farm_Harvest (nur Bücken mit Schwung, Becken 75 cm) bzw. davor Kevin Gathering01 (stehend, Hand nie unter 105 cm). Jetzt Kevin Gathering03 (`A_Gather3`: Becken 49 cm, Hand bis 41 cm), ausgewählt per Posen-Messung in `tools/unreal/import_characters.py` (Zeilen „POSE“ im Log). Abstand: Bei E auf etwas von Hand Sammelbares oder einen liegenden Gegenstand läuft der eigene Spieler erst bis `GrabDistance` = 70 cm heran (höchstens `GrabApproachSeconds` = 1,5 s, bricht ab, wenn er nicht näher kommt), dreht sich hin und greift; der Server prüft die Reichweite wie bisher. Im Bild geprüft (Äste: Heranlaufen, Hocken mit der Hand am Boden, Aufstehen). Neuer Testbefehl `grab:<vorkommen>`; Log-Zeile „Anim …: clip …“ zeigt, welcher Clip spielt.
- **MetaHuman-Stand:** Projekteinstellungen für MetaHuman (16-Bit-Knochenindex, unbegrenzte Knocheneinflüsse, Skin Cache) vom Nutzer im Editor übernommen; stehen nur in der nicht versionierten `DefaultEngine.ini`. `MHC_Grundfigur` angelegt; Epic-Anmeldung im Creator noch nicht gelungen (`EOS_InvalidAuth`), daher noch kein Rig und keine Texturen.
- **MetaHuman bis zur Anmeldung vorbereitet (2026-11-16):** `tools/metahuman.ps1 Prepare` legt `MHC_Kraeftig` (182 cm) und `MHC_Schmal` (170 cm) in `/Game/Gamer/Characters/MetaHuman` an und belegt Körpermaße und Haut vor (gespeichert, nach erneutem Lauf geprüft). `tools/metahuman.ps1 Link` sucht nach dem Zusammenbau Körper/Kopf, macht das Skelett mit SK_Mannequin verträglich und schreibt `Content/Data/figures.json`; `GamerFigures.*` liest sie, `AGamerCharacter`/`AGamerVillager` nutzen dann MetaHuman (Kopf folgt dem Körper), sonst Manny/Quinn. Bewohner: „Low“-Zusammenbau, sonst Spielerfigur ohne die zwei feinsten LODs. Rückfall mit Grafik geprüft (Spieler + Bewohnerin), Datentests grün. Anleitung für den Nutzer: `docs/16_metahuman_anmeldung.md`.
- **Noch nicht / offen:** MetaHuman-Figuren (Login des Nutzers), Bewohner mit Figur nicht im Bild geprüft, Feldarbeit/Aufheben/Essen nur im Code, nicht im Bild; Fußgleiten und Sprunghöhe nicht feinjustiert (Tempowerte in `GamerAnimInstance.h`); Lizenz Kevin Iglesias außerhalb Unity nicht schriftlich belegt (siehe Asset-Log).

<a id="d4-22"></a>

### 4.22 Umriss statt Rechteck, Scan-Welle, Animations-Hinweise (2026-11-15)
- **Umriss:** Das anvisierte Ding hat jetzt eine Kontur, die seiner Form folgt (warmes Bernstein), nicht mehr das Rechteck. Technik: Custom-Stencil (Wert 1) nur am eigenen Ziel, lokal, nichts wird übertragen (`AGamerCharacter::UpdateOutline`); Post-Process-Material `/Game/Gamer/Materials/M_Outline` (gebaut mit `tools/unreal/build_outline_material.py`, Parameter OutlineColor, Thickness, Opacity) hängt an der Kamera der Figur; `r.CustomDepth` wird zur Laufzeit auf 3 gesetzt (keine Änderung in `DefaultEngine.ini`). Fehlt das Material, zeichnet die Anzeige weiter das Rechteck. Bei Haufen bekommt nur das mit F gewählte Ding die Kontur.
- **Getestet (mit Grafik):** Kontur an Baumstamm, Ästen, losen Steinen und einer Pilzstelle sichtbar und der Form folgend; zwei Spieler: jeder umrandet nur sein eigenes Ziel (Zähler im Protokoll: Spieler 1 `GamerResourceNode_5`, Spieler 2 `GamerResourceNode_14`). Erster Shader-Versuch scheiterte (`SceneTextureLookup` nicht deklariert), behoben mit Scene-Texture-Knoten im Material.
- **Nicht geprüft / offen:** Fels und Bodengegenstand (es lagen keine im Blickfeld; Pflanzen und Steine ja); Umriss durch Verdecktes (die Kontur erscheint auch hinter Gras, aber nicht an Dingen, die ganz verdeckt sind); in der Vogelperspektive gibt es keine Kontur (andere Kamera); Paketierung noch nicht geprüft (Material und CVar-Setzen zur Laufzeit sollten greifen).
- **Scan-Visualisierung:** Der Zeichenblock des Scans (Markierungen, Zähler) war aus `GamerHUD.cpp` verschwunden (vermutlich ein gleichzeitiger Schreibvorgang einer anderen Session überschrieb ihn; auch im Commit fehlte er). Das war wohl der Grund für „die Visualisierung für den Scan fehlt“. Wiederhergestellt und erweitert: eine Welle (3 Ringe am Boden) läuft in 1 s zum Radius, Markierungen erscheinen, wenn die Welle sie erreicht; Zähler am rechten Rand etwas höher. Im Bild gesehen.
- **Animationen (nicht umgesetzt, Figuren-Session):** Wunsch des Nutzers: beim Aufheben vom Boden hinhocken und zum Material greifen statt nur einer Greifbewegung nach vorn; Herstellen der Anfangsgegenstände (Hammer, Axt und ähnliche) mit einer eigenen Bewegung, anders als sonstiges Herstellen. Die Grundlage steht: `FGamerWorkState` (repliziert) hat jetzt `Subject` (Vorkommen-ID bzw. Rezept-ID) und `bLowOrStation` (Ernte: Ziel liegt tief, also hocken; Herstellen: an einer Station). Zugriff `UGamerWorkComponent::GetWork()`.

#### Nachtrag 2026-11-15: Baumenü mit Kacheln, Kontur an Fels
- **Baumenü:** Jede Zeile hat links eine Kachel (`DrawIconTile`): gerendertes Icon bei den Holz-Bauteilen, benannter Platzhalter bei den Stationen (Werkbank, Lager, Komposter …). Im Bild gesehen. Die Kacheln sind klein (so hoch wie eine Zeile, ca. 20 px bei 960×540), weil das Menü schon fast die ganze Höhe füllt; größere Kacheln bräuchten zwei Spalten mit weniger Zeilen oder ein scrollbares Raster.
- **Kontur an Fels:** mittlerer Fels im Bild mit Kontur gesehen. Bodengegenstände (Fallobst) liegen in der Testwelt nicht, bleiben deshalb ungeprüft.
- **Testhinweis:** Alle Tests schreiben ihre Bilder in denselben Ordner (`game/Saved/Screenshots/WindowsEditor/gamer_test_N.png`), und zwei Sessions starteten ihren Test einmal im selben Moment (Bilder und Protokoll vermischt). Bilder nach dem Lauf sofort kopieren und mit der Zeit im Protokoll („Screenshot requested“) abgleichen. Außerdem: Änderung und Build nie im selben Schritt parallel starten (der Build kompilierte sonst die alte Datei).

#### Nachtrag 2026-11-15: Stil „Gezeitenhall“ im HUD (Stufe A)
Umgesetzt nach der Entscheidung des Nutzers (siehe `docs/01_entscheidungen.md`, Zeilen „Stil-Umsetzung im HUD“ und „Scan: wer was sieht“), gebaut und mit Grafik gesehen:
- **Umriss:** Herdfeuer `#DB9345`, leicht aufgehellt (`M_Outline`, Standardwert OutlineColor). Gezeitenlicht bleibt magischen Zielen vorbehalten (gibt es noch nicht).
- **Rahmen Stufe A:** Panels in Basaltschwarz mit dünnem Treibholz-Rand und kurzen Eckkerben; Knöpfe in dunklem Basalt mit Treibholz-Rand, aktiv in Herdfeuer-Braun; Leisten in gedämpftem Moosgrün, mittel in Sandgelb, knapp in Herdfeuer. Textfarben bleiben hell und lesbar (Rot-Regel vom 2026-11-13); die Hinweis-, Gut- und Fehlt-Farben sind aufgehellte Palettenwerte. Keine Texturen; Stufe B (geschnitzte Holz-/Lederrahmen) entscheidet der Nutzer später.
- **Kacheln:** dunkles Leder, Treibholz-Rahmen; Platzhalterfarben aus der Palette statt aus einem Zufallswert. Die 16 Icons sind mit kühlem Nebelgrau-Licht (`#9AA8A5`) neu gerendert und importiert.
- **Scan:** offene Ringe mit drei Kerben, gegenläufig rotierend, in Treibholz/Nebelgrau mit dunkler Unterlage (auf hellem Sand sonst kaum lesbar); Markierungen und Zähler in Palettenfarben aus `scan.json` (lineare Werte; „Gegenstände“ in Ocker statt Rostrot, weil Rot als Textfarbe schwer lesbar war). Haken für die Klassenfähigkeit: Kategorien mit `"requires"` erscheinen nur bei einem Spieler, der diese Fähigkeit hat (`UGamerScanComponent::HasScanAbility`, gibt noch jedem `false`); die Kategorie `magic` (Gezeitenlicht, `requires: seer`) steht bereits in der Datei. Die Unterscheidung Solo/Koop ist nicht gebaut (Entscheidung beim Game Director).
- **Testschritt neu:** `scanage:<s>` friert die Scan-Welle auf ein Alter ein (für Bilder).
- **Offen:** Ringe sind aus der Ferne dezent (flacher Blickwinkel auf den Boden); Gezeitenlicht-Ring für die Seherin ist nur als Datenhaken vorbereitet, nicht gezeichnet.

#### Nachtrag 2026-11-15: Sichtbarkeitsmodell für Magie im Scan
- **Gebaut:** In `scan.json` steht der Block „magic“ (Stufen, Schwellen, `full_if`-Bedingungen, Reichweiten); `UGamerScanComponent` berechnet die Stufe (Erfahrung oder eine erfüllte Bedingung), zeigt Magie im Scan für jeden von Anfang an als Flimmern (Stufe 1, Gespür), mit Art (2, ab Erfahrung 5) oder voll mit Name und 60 m Reichweite (3, ab Erfahrung 15). Eine „Stufe 0“ ohne Magie im Scan gibt es nicht mehr (Korrektur: widersprach Option A); das Flimmern des Ziels in der Welt bleibt eine eigene Sache (Haken `ShouldShimmerInWorld`). Der Erfahrungszähler (`MagicExperience`) liegt auf dem Server, wird an den Besitzer repliziert und über `IGamerSaveable` gespeichert (Schlüssel `scan`, Feld `magic_experience`). Bedingungen datengetrieben: `origin:`, `ability:`, `item:`, `job:`, `season:`; Zeit und Ort (Option D, `full_if_planned`) werden noch nicht geprüft.
- **Abhängigkeit:** **D braucht Tag/Nacht und Wetter** (nur „Winter“ ginge schon über die Jahreszeit).
- **Nicht gebaut (Magie-Phase):** Seeglas-Amulett, Beruf Seherin samt Haus, echte magische Objekte und wie Erfahrung entsteht, der Flimmer-Effekt des Ziels in der Welt (Haken: `ShouldShimmerInWorld`, keine Scan-Stufe), die Fähigkeit „Seidr-Blick“ zum Lernen (Bedingung `ability:ability_seidr_sight`, die Fähigkeit gibt es noch nicht; `ability_seidr_vision` in `characters.json` ist die Wettervorhersage).
- **Test:** Testschritte `magictarget:<Art>|<Name>`, `magicexp:<n>`, `magiclevel`; kein echtes magisches Ziel, nur ein Testziel.

#### Nachtrag 2026-11-15: Scan-Welle und Magie als Silhouette (Nutzerkorrektur)
- **Welle:** statt der Kerben-Ringe eine weiche, transparente Farbwelle (dichte, durchscheinende Punkte mit nachlaufendem Abklingen und leichtem Wellen-Kräuseln, Treibholz-Ton). Im Bild gesehen als zarter Schleier auf dem Boden. Nah an der Figur wirkt das Punktmuster noch etwas körnig; eine richtige Boden-Projektion per Post-Process (Tiefe → Weltposition) wäre sauberer, ist aber aufwendiger und wurde nicht gebaut.
- **Magie als Silhouette:** `M_Outline` kennt jetzt zwei Stencil-Werte (1 = Ziel im Fadenkreuz in Herdfeuer, 2 = aufgedeckte Magie in Gezeitenlicht mit Glühen); die Parameter `MagicOpacity` und `MagicThickness` setzt `UGamerScanComponent` (Deckkraft = Stärke × Stufenfaktor × Ein- und Ausblenden, beim „Gespür“ leicht flimmernd). Magische Dinge sind Actors mit dem Tag `GamerMagic` (und `kind:<Art>`, `name:<Name>`); der Scan findet sie, die Anzeige schreibt Art (ab Stufe 2) und Name (Stufe 3) darüber. Es gibt noch keine echten magischen Objekte; getestet mit einer Test-Kugel (`magictarget`).
- **Stärke wächst:** `GetMagicStrength()` = 0,25 + 0,75 × (Erfahrung / 30). Erfahrung: +1 je Scan, der Magisches aufdeckt; der Server zählt selbst (`ServerRegisterMagicScan`), der Client meldet nur, dass es etwas gab. Im Bild gesehen: Erfahrung 0 kaum sichtbar, Erfahrung 30 deutliche türkise Kontur mit Name; die Seherin ist bei Erfahrung 0 ebenfalls dezent.
- **Offen:** Die Welt-Flimmer-Stufe vor dem ersten Scan bleibt ein Haken; die Kontur hat eine Stencil-Grenze (ein Ziel im Fadenkreuz und Magie am selben Ding: Magie gewinnt nicht, es gilt der zuletzt gesetzte Wert); Stärke wirkt auf alle magischen Ziele im Scan gleich (nicht je Entfernung); echte Magie, Amulett, Beruf Seherin folgen mit der Magie-Phase.

#### Nachtrag 2026-11-15: Umschauen und Lauschen getrennt, Magie-Gespür als Fortschritt
Ausführlich in `docs/13_magie_gespuer.md`. Gebaut und getestet (Build ohne Fehler; Host + 2 Spieler, Solo mit Grafik):
- **Umschauen (Z):** nur Material, Sinus-Welle, 40 m, 5 s, Funde 20 s (`scan.json`). Im Bild: zarte wellige Punktlinie auf dem Boden, Markierungen erscheinen, wenn die Welle sie erreicht. Geprüft: Das Umschauen zählt mit einem Testziel in der Nähe keine Magie (80 Markierungen, nur die fünf Material-Kategorien).
- **Lauschen (Q, Gamepad linke Schultertaste, umbelegbar):** passives Randschimmern (heiß/kalt, im Bild bei Stufe 3 nah deutlich, bei Stufe 1 fern praktisch aus), aktiver Sinus-Ring mit Kerben, Bewegung bricht ab, Silhouette mit Beschriftung je Stufe (Stufe 3: „Seeglas-Ader ×3“, Status „Lauschen · Stufe 3 · Q“), Audio-Haken `OnMagicHum`. Die alte Magie-Logik im Scan (Erfahrung, `full_if`) ist entfernt; der Erfahrungszähler ist jetzt Teil des Lauschens (Speicherschlüssel `listen`).
- **Stufen und Boni:** Schwellen 0/10/30/75/160 EP, Seherin startet auf Stufe 2, Amulett (+1, per Testschritt `amulet:on`) und Winter (+1, `season:3`) heben je eine Stufe, höchstens 5; Dorf-Seherin (+25 % EP) nur als Bedingung vorgemerkt.
- **EP-Quellen und Tempo:** `magicevent:<Quelle>` (wie ein magisches Objekt es auslösen wird); Tempo langsam ergibt +1,2 statt +2 je Fund.
- **Koop:** gemeinsamer Fund (Mitspieler bekommt 50 %: 20 von 40 EP) und gemeinsames Lauschen (Stufe 3 bekommt Reichweite 25 m statt 20 m, Welle 12,5 s statt 10 s; der Mitspieler auf Stufe 2 hat mindestens Stufe 2 und 19 m).
- **Weltoption „Gespür-Tempo“:** Zeile unter „Welt erstellen“ (im Bild gesehen), URL `?Tempo=…`, wird mit der Welt gespeichert und beim Laden wiederhergestellt (Lauf 1 mit `?Tempo=slow`, Lauf 2 ohne Option: „World options from the save: … Tempo=slow“; die Erfahrung 2,4 kehrte zurück und die Seherin startete auf Stufe 2).
- **Einmal pro Fund:** EP zählen pro Fund und Spieler nur einmal (`counted_targets`, serverseitig und gespeichert); getestet: derselbe Fund zweimal ergibt nur einmal EP, auch nach Speichern und Laden („find_a was already counted“). Der frühere „+1 je Scan“-Vorschlag ist gestrichen.
- **Offen:** echte magische Objekte, Amulett, Beruf Seherin und Quest, Seidr-Blick lernen, D mit Nacht/Nebel/Steinkreis (braucht Tag/Nacht und Wetter), Markieren und gespeicherte Markierungen, Summen (Audio) und Flimmern in der Welt; die Sinus-Welle des Umschauens ist auf hellem Sand dezent (Alpha und Amplitude in `scan.json`/HUD einstellbar).

#### Nachtrag 2026-11-16: Lebenspunkte, Spielertod, Heilung
Gebaut (Build ohne Fehler) und getestet (Host + 2 Spieler ohne Grafik, Solo mit Grafik):
- **Komponente `UGamerHealthComponent`** an der Spielfigur: LP serverseitig, an den Besitzer repliziert, Totzustand an alle, gespeichert (`health`: LP, Startpunkt, Bett-Respawnpunkt). Maximum = 100 + Buff „Gesundheit“. Allgemeine Schadenstür (`TakeDamage(FGamerDamageInfo)`), Fallschaden über `AGamerCharacter::Landed`, Haken `IGamerDamageable` für Tiere und Bewohner (Bewohner haben noch keine LP).
- **Heilung:** hungrig keine, satt 0,5/s, Eintopf mit Regenerations-Buff schneller. Gemessen: hungrig 70 → 70 nach 7 s; satt (Maximum 110) 71,8 → 76,0 in ca. 8 s; mit Eintopf (Maximum 124, +0,7/s) 82,2 → 92,7 in ca. 8 s. Fallschaden: 300 cm nichts, 800 cm = 48 LP.
- **Tod und Weltoption „Tod“:** drei Modi getestet: `none` (Inventar bleibt), `grave` (27 Teile ins Grab, Inventar leer, Abholen bringt alles zurück), `hardcore` (27 Teile verloren). Die Option steht unter „Welt erstellen“ und wird mit der Welt gespeichert. Grab-Schutzzeit: Mitspieler wird vor 120 s abgewiesen, öffnet nach 124 s. Wiedereinstieg am zuletzt gebauten Strohlager getestet („got up again at the bed“), sonst Startpunkt. Speichern und Laden: 60 LP kehrten zurück, das Grab (27 Teile) wurde wiederhergestellt und vom Besitzer geöffnet.
- **HUD:** Lebensleiste links unten (Symbol „L“ und Leiste in Rostrot als Fläche, Zahlen in hellem Text), kurzer rostroter Randschimmer bei Schaden, Hinweis am Grab, Todesbildschirm mit „Aufstehen: E“. Im Bild gesehen. Die Schaltfläche reagiert auf E, nicht auf die Maus (kein Menü-Cursor im Todesbildschirm).
- **Testschritte:** `damage:<n>[|<Art>]`, `fall:<cm>`, `hp`, `inv`, `getup` (wie die Taste E, mit Mindestwartezeit), `getupnow`, `gotograve`, `opengrave`.
- **Offen / Hinweise:** Ausrüstungsplätze gibt es nicht, das ganze Inventar geht ins Grab; Bewohner und Tiere haben noch keine LP (Haken ist da); der Mausklick auf „Aufstehen“; die Client-Anzeige „darf das Grab öffnen“ erkennt den Besitzer in gespeicherten Welten am Namen der Figur, der Server prüft streng mit der Spieler-ID; Schadensarten für Kälte und Tiere kommen mit Wetter und Tieren.

<a id="d4-23"></a>

### 4.23 „Welt erstellen“: Spielmodus, Schwierigkeit, Erweiterte Einstellungen (2026-11-16)

Entscheidung „Spielmodus und Schwierigkeit“ (`docs/01_entscheidungen.md`). Die Zahlen sind ein Vorschlag von Claude und stehen in `game/Content/Data/difficulty.json`:

| | Einfach | Mittel | Schwer | Hardcore |
|---|---|---|---|---|
| Tod | nichts verlieren | Grab | Grab | alles verlieren |
| Hunger/Durst/Erschöpfung | ×0,7 | ×1 | ×1,3 | ×1,3 |
| Erlittener Schaden | ×0,6 | ×1 | ×1,4 | ×1,8 |
| Ertrag beim Abbauen | ×1,5 | ×1 | ×0,8 | ×0,7 |
| Horden | aus | aus | an | an |
| Bewohner unsterblich | ja | nein | nein | nein |
| Abreißen | voll | voll | halb | halb |
| Gespür-Tempo | schnell | normal | normal | langsam |

- **Seite 1 „Welt erstellen“** (nach der Charakterseite): Spielmodus Saga/Freies Spiel (`?Mode=`, Saga-Inhalte gibt es noch nicht), Schwierigkeit (Klick = nächste Stufe, mit Beschreibung und Werten), „Welt starten“, „Erweiterte Einstellungen“. Code: `GamerHUDWorld.cpp`, `GamerPlayerControllerWorld.cpp`.
- **Seite 2 „Erweiterte Einstellungen“** nach Bereichen: Bauen (Bauablauf, Platzierung, Abreißen, Freies Bauen), Überleben (Tod, Inventar, Raten-, Schadens-, Ertragsfaktor), Welt (Jahreszeit-Dauer, Gespür-Tempo, Horden), Dorf (Bewohner unsterblich), Koop (Friendly Fire, Höchstzahl Spieler 1–8). Ausgegraut mit „kommt später“: Freies Bauen, Horden, Bewohner unsterblich, Friendly Fire – ihr Wert (aus der Stufe) wird trotzdem gespeichert. Die Zeile „Sagenwelt“ ist am 2026-11-22 entfallen (Entscheidung „Bosse: Sagen-Fassung als Standard“); ältere Spielstände mit `?Sagenwelt=` und alte Menü-Einstellungen mit `bSagaWorld` laden weiter, der Wert wird übergangen (Test `Gamer.Data.WorldOptionsOldSaves`).
- **Benutzerdefiniert:** Weicht ein Wert nach einer Änderung auf Seite 2 von der Stufe ab, steht die Schwierigkeit auf „Benutzerdefiniert“; passt die Zusammenstellung wieder genau zu einer Stufe, gilt diese. Deutung von Claude: Optionen, die keine Stufe festlegt (Bauablauf, Platzierung, Inventar, Jahreszeit, Spielerzahl), machen die Welt nicht „Benutzerdefiniert“.
- **Server-autoritativ:** alle Felder in `FGamerWorldOptions` (Karten-URL, Replikation, Spielstand). Bei einer Stufe nimmt der Server die Werte aus seiner eigenen `difficulty.json`. Die Schwierigkeit hat nach dem Erstellen keinen Schalter mehr; geladene Welten nehmen sie aus dem Spielstand. „Welt laden“ zeigt Modus und Schwierigkeit in der Optionszeile (nur für Welten, die ab heute gespeichert wurden).
- **Höchstzahl Spieler:** `?MaxPlayers=` liest auch die Engine (`AGameSession`), die Grenze wirkt also sofort; höchstens 8.
- **Getestet (2026-11-16/21):** Build grün, Datentests 8/8 (neu: `Gamer.Data.DifficultyPresets` prüft u. a. „Mittel = Standardwerte“, Übertragung jeder Stufe über die Karten-URL und dass der Server manipulierte Stufenwerte korrigiert; `Gamer.Data.DifficultyValidatorFindsErrors`). Menütests über das Startmenü: Einfach, Mittel, Schwer, Hardcore und Benutzerdefiniert gestartet, der Server übernimmt jeweils die Werte (Protokoll „World options: … Difficulty=…“); Seite 1, Seite 2, „Benutzerdefiniert“ nach Änderung, „Welt laden“ und Esc-Menü mit Grafik angesehen; gespeicherte Welten „Hardcore“ und „Benutzerdefiniert“ geladen, Modus und Schwierigkeit kommen aus dem Spielstand; Host lädt „Benutzerdefiniert“, ein Mitspieler tritt bei. Nachtrag 2026-11-22: Host lädt „Hardcore“, der Mitspieler sieht im eigenen Esc-Menü „Freies Spiel · Hardcore“ mit allen Werten (Screenshot geprüft).
- **Wirkung noch offen:** Raten-, Schadens- und Ertragsfaktor sind gespeichert, die Wirkung baut die Gameplay-Session (Lebenspunkte, Ausdauer). Questhilfen sind eine Spieler-Einstellung (`UGamerMenuSettings::bQuestHints`, Schalter auf der Seite „Grafik“), Wirkung mit der Saga.

#### Nachtrag 2026-11-21: Niedergeschlagen, Wiederbeleben, Letzter Atem
- **Niedergeschlagen (nur Koop):** bei 0 LP 15 s am Boden, wenn ein anderer lebender Spieler da ist; sonst (solo) direkt Tod (gleicher Code). Ein Mitspieler hält E 3 s (Fortschrittsbalken), Wiederbelebter hat 30 % LP (Vorschlag); läuft die Zeit ab, gilt die Weltoption „Tod“. Niedergeschlagen: grauer Bildschirm mit türkisem Rand und Zähler; Mitspieler sehen „Niedergeschlagen N s“ über der Figur.
- **Heilende Hände** (`ability_healing_hands`): belebt sofort, 1× pro Spieltag (1800 s). Nur Datenhaken, Testschritt `hands:on`.
- **Letzter Atem:** Kräuterverband (`item_herb_bandage`, 2 Fasern + 2 Beeren, Vorschlag; Kategorie „food“, damit er in die Schnellleiste kommt) in der Schnellleiste wird bei 0 LP verbraucht, man bleibt mit 25 % LP stehen; 1× pro Spieltag. Werte in `health.json`.
- **Grabmarkierung** „Mein Grab N m“ im HUD für den Besitzer.
- Getestet (Host + 2, Logs): Niedergeschlagen → Wiederbeleben nach ca. 3 s mit 30 LP; Ablauf der 15 s → Tod; Heilende Hände sofort; Letzter Atem (25 LP), zweiter Treffer am selben Tag → Tod, `newday` setzt zurück; Solo kein Niedergeschlagensein. **Nicht getestet:** Bilder der Niedergeschlagen-Anzeige und der Balken mit Grafik, Rezept im Spiel, Speichern der Tageszähler (nicht gespeichert).
- Testschritte: `hands:on|off`, `newday`, `revive:start|stop`, `gotodowned`.

<a id="d4-24"></a>

### 4.24 Leistungsmessung Phase 1 (2026-11-21)
Gemessen mit `tools/perf_measure.ps1` (neu; nutzt die Unreal-Sperre): paketiertes Development-Spiel vom 15.11. (`game/Build/Packages/Development/Windows`), Karte World, Fenster 1920×1080, ohne Bildratengrenze und V-Sync, Unreals CSV-Profiler, je 60–200 s Messzeit nach 15–50 s Einlauf. RAM und Grafikspeicher von außen (Windows-Zähler „GPU Process Memory“) alle 2 s. Rohdaten: `game/Saved/Perf/<Zeit>_<Szenario>_q<Qualität>/` (`profile.csv`, `memory.csv`, `result.json`).

| Szenario | Qualität | FPS Mittel | FPS 1 % tief | Bild ms | Game ms | GPU ms | RAM (privat) | Grafikspeicher |
|---|---|--:|--:|--:|--:|--:|--:|--:|
| Solo, Startgebiet | Mittel | 184 | 156 | 5,4 | 1,9 | 5,0 | 2,44 GB | 0,88 GB |
| Solo, Startgebiet | Hoch | 127 | 109 | 7,9 | 2,2 | 7,5 | 2,68 GB | 1,11 GB |
| Solo + 8 Bewohner | Mittel | 172 | 147 | 5,8 | 2,2 | 5,4 | 2,42 GB | 0,85 GB |
| Solo + 8 Bewohner | Hoch | 124 | 106 | 8,1 | 2,8 | 7,6 | 2,70 GB | 1,14 GB |
| Gastgeber + Mitspieler, ein PC | Mittel | 76 | 62 | 13,1 | 2,6 | 12,5 | 2,40 GB | 0,82 GB |
| Gastgeber + Mitspieler, ein PC | Hoch | 53 | 44 | 18,9 | 3,5 | 16,8 | 2,66 GB | 1,09 GB |
| Vergleich: Solo ohne Instanzen (Gras, Farne, Felswände) | Hoch | 139 | 119 | 7,2 | 2,0 | 6,8 | 2,66 GB | 1,12 GB |

RAM = höchster Wert „Private Bytes“ des Spielprozesses (Arbeitssatz 1,4–1,6 GB). Gemessen am Gastgeber-Prozess; der Mitspieler lief als zweites Fenster auf dem zweiten Bildschirm.

**Einordnung gegen die Budgets** (`CLAUDE.md` Regel 11, `docs/04_technische_ziele.md`):
- **RAM:** höchstens 2,7 GB im Spiel, weit unter „mindestens 8 GB“ (Windows und andere Programme brauchen zusätzlich ca. 3–4 GB). Erfüllt, mit Reserve.
- **Installationsgröße:** Paket 4,25 GB, davon 2,45 GB Debug-Symbole (`.pdb`, werden nicht ausgeliefert) → **ca. 1,8 GB** echte Spielgröße bei 20 GB Budget. Erfüllt.
- **Grafikspeicher:** 0,8–1,1 GB, auf der RTX 3070 (8 GB) unkritisch. Eine Mindest-Grafikkarte ist noch nicht entschieden.
- **Bildrate:** auf diesem PC überall über 100 FPS solo und über 44 FPS mit zwei Spielen auf einer Grafikkarte.

**Was am meisten kostet:**
- **Grafikkarte begrenzt** in allen Szenarien (GPU-Zeit ≈ Bildzeit; die Render-Thread-Zeit ist so hoch, weil er auf die Grafikkarte wartet). Spiel-Logik nur 2–3,5 ms.
- **Qualität Hoch statt Mittel:** +2,5 ms GPU (+45 %), +0,25 GB Grafikspeicher.
- **Gras, Farne, Felswände (≈ 87 000 Instanzen):** etwa 0,7 ms GPU bei Hoch (≈ 9 %) in dieser Ansicht; kleiner als befürchtet. Boden-Shader nicht getrennt gemessen.
- **8 Bewohner:** +0,3–0,6 ms Spiel-Logik und +0,1–0,4 ms GPU (Figuren ohne eigene LOD-Stufen). Hochgerechnet auf 20–30 Bewohner noch tragbar, aber nicht gemessen.
- **Zwei Spiele auf einem PC** teilen sich eine Grafikkarte: Bildrate etwa halbiert. Das ist ein Test-Sonderfall, kein Koop über zwei PCs.

**Grenzen der Messung (ehrlich):** stehende Kamera am Startpunkt (kein Laufen, kein Kameradrehen); nur der Entwicklungs-PC (RTX 3070, 16 GB RAM), kein schwächerer Rechner; vor den Läufen waren nur 2,8–4,3 GB RAM frei (andere Programme offen); je Szenario ein Lauf; Spielfiguren noch Manny/Quinn, **keine MetaHumans** (die werden Grafikspeicher und GPU-Zeit deutlich erhöhen); Welt nur 250 m × 250 m; kein Unreal Insights.

**Empfehlungen (noch nichts optimiert):**
1. **Nach den MetaHumans neu messen** (größter erwarteter Sprung bei Grafikspeicher und GPU). Aufwand klein (Skript vorhanden).
2. **Messlauf mit Bewegung:** Testschritte `walk` und Kameradrehen in `perf_measure.ps1` einbauen, damit Nachladen und Sichtweite mitzählen. Aufwand klein.
3. **Mindest-Hardware festlegen** (offen in `docs/01_entscheidungen.md`) und auf einem schwächeren PC oder mit gedrosselter Auflösung messen. Aufwand mittel (Hardware nötig).
4. **LOD-Stufen** für Bäume, Sträucher und Figuren, bevor die Welt größer wird bzw. die MetaHumans kommen. Aufwand mittel.
5. **Bewohner-Skalierung:** Messung mit 20–30 Bewohnern (Testschritt `spawnvillager`). Aufwand klein.
6. **Auslieferung ohne Debug-Symbole** (Shipping-Build oder `.pdb` ausschließen). Aufwand klein.
7. Gras und Boden-Shader erst angehen, wenn eine schwächere Grafikkarte zu langsam ist; derzeit nicht nötig.

#### Nachtrag 2026-11-22: Ausdauer und Erschöpfung
- **Komponente `UGamerStaminaComponent`**, Werte in `game/Content/Data/stamina.json` (alles Vorschläge): Maximum 100, Erholung 14/s nach 1,2 s Pause (ohne Mahlzeit ×0,7), Sprint 14/s, Sprung 8, Schlag 5. **Leer sperrt nichts:** Tempo ×0,65, Schlagstärke ×0,7 (Regel 5), Hysterese bis 15 %.
- **Erschöpfung** steigt um 0,7 pro Spieltag (Weltoption `NeedsRate` skaliert), senkt das Maximum um bis zu 40 % und das Handarbeitstempo (Sammeln/Herstellen) um bis zu 30 %. **Schlaf:** E am eigenen Bett (zuletzt gebautes Bett), ab 10 % Erschöpfung, setzt sie auf 0 und füllt die Ausdauer, 60 s Abklingzeit; einfache Lösung ohne Zeitsprung.
- **Buff-Arten** `stamina_max`, `stamina_regen` (Prüfer und Anzeige kennen sie); Ausrüstungsregel greift erst mit Ausrüstungsplätzen; Tavernen-Gerichte haben noch keine Werte. Erlittener Schaden: Faktor `DamageTaken` der Weltoption ist in `TakeDamage` eingebunden.
- **HUD:** Ausdauerleiste (Ocker) neben der Lebensleiste, dunkles Ende = was die Erschöpfung nimmt, dünner Streifen = Erschöpfung; Hinweis „schlafen“ am Bett.
- Getestet (1 Spieler, Logs): Sprint ca. −14/s, Erholung ca. +14/s, leer → Tempo ×0,65, Zeitraffer (`lapse:`) hebt Erschöpfung (0,49 → Maximum 80, Arbeitstempo ×0,85), Schlaf setzt auf 0, zweiter Schlaf wird abgewiesen. **Nicht getestet:** Speichern/Laden der Werte, Koop mit 2 Spielern, Bilder der Leisten, spürbare Verlangsamung beim Gehen (nur Faktor im Log).
- Testschritte: `stam[:<A>|<E>]`, `lapse:<f>`, `sleep`.

<a id="d4-25"></a>

### 4.25 Hinweis zum Dedicated-Server-Target (Phase 0)
Der Launcher-Build von Unreal kann keine Server-Targets bauen, dafür braucht es einen Source-Build der Engine (Epic-Doku „Setting Up Dedicated Servers“). Das „Dedicated-Server-Target“ in Phase 0 geht deshalb erst mit Source-Build. Bis dahin kann mit dem Launcher-Build entwickelt und mit Listen-Server getestet werden. Die Entscheidung ist offen (siehe `docs/01_entscheidungen.md`).

<a id="d4-26"></a>

### 4.26 Parallel laufend
- **Design (ChatGPT):** Art Direction und Siedlungs-/UI-Konzepte vor Phase 1; danach phasenweise.
- **Daten:** Spielinhalte entstehen als JSON-Tabellen in `game/Content/Data/` (Format: `docs/06_datenformat.md`), nicht im Code.
- **Messen:** ab Phase 1 regelmäßig Installationsgröße (gecookt), Client-RAM und Server-Tick; ab Phase 2 Skalierungstests mit Bots (2 → 4 → 8 Spieler).

<a id="d4-27"></a>

### 4.27 Ehrliche Einschätzung
Das Dorfbewohner-System ist der aufwendigste Teil (KI, Bedürfnisse, Speichern/Laden, Koop-Replikation). Zum Vergleich: Askas Entwickler
haben es über Jahre im Early Access nachgebessert. Phase 1 deshalb bewusst klein halten.


[↑ zum Inhaltsverzeichnis](#inhaltsverzeichnis)

<a id="d5"></a>

## 5. Setup-Checkliste für deinen PC

> Quelle: `docs/03_setup_checkliste.md`

<a id="d5-1"></a>

### 5.1 Was ich gesehen habe (nur Ordnernamen, Stand 2026-11-12)
- Erster Stand: Kein Epic-Games- oder Unreal-Ordner auf `C:`, `D:` oder `E:`.
- Späterer Stand (2026-11-12): Epic Games Launcher ist installiert (`C:\Program Files\Epic Games\Launcher`). Unreal Engine 5.8.4 liegt in `D:\gamer\unreal\UE_5.8` (`Engine\Binaries\Win64\UnrealEditor.exe`, `Build.version`, `ToolComplete.txt` vorhanden). Den Editor habe ich nicht gestartet.
- `Microsoft Visual Studio` ist unter `C:\Program Files (x86)` vorhanden. Welche Version und Komponenten, steht weiter unten.
- Git ist vorhanden: `D:\Program Files\Git`. Ob Git LFS dabei ist, sehe ich nicht. (Meine erste Einschätzung „Git fehlt“ war falsch, ich hatte nur `C:` geprüft.)
- Freier Platz (im Explorer abgelesen, 2026-11-12): `C:` 22,7 GB von 236 GB (knapp), `D:` 904 GB von 931 GB, `E:` 321 GB von 931 GB.
- Visual Studio Installer zeigt: „Visual Studio Build Tools 2022“ (17.14.31) und „Visual Studio Build Tools 2026“ (18.3.2). Das sind Build Tools, keine IDE. Welche Komponenten (C++-Workload, Profiling-Tools, AddressSanitizer, Windows SDK) enthalten sind, habe ich noch nicht abgelesen.
- Projektort: Der Ordner `Desktop\gamer` ist leer, der Inhalt liegt jetzt in `D:\gamer\gamer` (15 eigene Dateien, `reference/` mit 889 Dateien). Der Epic-Installer `EpicInstaller-20.3.3-unrealEngine.exe` (252 MB, heruntergeladen am 2026-11-12) liegt in `D:\gamer\programme`.
- Nachtrag (2026-11-12, per `vswhere -all -products *` geprüft): Gefunden wird nur noch „Visual Studio Build Tools 2026“ (18.12.12405.243) in `C:\Program Files (x86)\Microsoft Visual Studio\18\BuildTools`. Build Tools 2022 tauchen nicht mehr auf. Erster Build `GamerEditor Win64 Development` (2026-11-12): **fehlgeschlagen (RulesError)**, weil das **.NET Framework SDK (NetFxSDK, ≥ 4.6)** fehlt (benötigt von `SwarmInterface`). Außerdem warnt UBT: Der vorhandene MSVC 14.52 ist neuer als der bevorzugte 14.50 und wenig getestet. Windows SDK 10.0.26100.0 ist vorhanden. Danach über den Visual Studio Installer nachinstalliert: `Microsoft.Net.Component.4.8.SDK` und `Microsoft.VisualStudio.Component.VC.14.50.18.0.x86.x64`. Zweiter Build (2026-11-12): **erfolgreich**, mit MSVC 14.50.35717 und Windows SDK 10.0.26100.0, Dauer ca. 113 s. Verbleibende Warnung: „Visual Studio SDK nicht gefunden, Editor-Integration deaktiviert“ (betrifft nur die VS-IDE-Anbindung, nicht den Build).
- Nachtrag (2026-11-12): Build Tools 2026 nach `E:\VS\BuildTools` verlegt (neu installiert mit `tools/vs-buildtools.vsconfig`, alte Instanz auf `C:` deinstalliert, Download-Cache dabei geleert). Testbuild danach erfolgreich mit MSVC 14.50 von `E:`. Windows SDK bleibt in `C:\Program Files (x86)\Windows Kits`. `C:` danach 26,3 GB frei.
- Nachtrag (2026-11-12): Git wurde vom Nutzer beim Aufräumen deinstalliert und per `winget` neu installiert: Git für Windows 2.56.0.4 mit Git LFS 3.8.1 in `E:\Git` (vorher `D:\Program Files\Git`, Git LFS 3.7.1).
- Blender 5.3.0 (GPL-3.0) per winget in E:\Blender installiert (2026-11-12). Headless-Test bestanden: Python-Skript erzeugt Testwand 300 x 20 x 300 cm (1 Blender-Einheit = 1 cm, Ursprung unten mittig) und exportiert FBX. Import in Unreal noch nicht getestet.
- Node.js (`C:\Program Files\nodejs`) und Python (`C:\Python314`) sind vorhanden.
- Hardware (am 2026-11-12 im Task-Manager abgelesen): RTX 3070 mit 8 GB, 16 GB RAM (im Alltag schon zu ca. 70 % belegt), SSDs: `C:` NVMe, `E:` NVMe, `D:` SATA. CPU-Modell habe ich nicht abgelesen, freier Platz siehe unten. Details: `docs/04_technische_ziele.md`.

<a id="d5-2"></a>

### 5.2 Schritte (in dieser Reihenfolge)
1. (erledigt, Ort weicht vom Vorschlag ab: `D:` statt `E:`) **Epic Games Launcher** installieren, dann **Unreal Engine 5.8** (braucht viele GB freien Platz). Der Installer liegt schon in `D:\gamer\programme`. Ziel für Launcher und Engine: `E:` (NVMe, 321 GB frei), nicht `C:` (nur 22,7 GB frei) und nicht die SATA-SSD `D:`. Die Engine lädt der Launcher erst nach deinem Epic-Login herunter.
2. **Visual Studio 2022** mit der Komponente „Spieleentwicklung mit C++“ (inkl. Windows SDK) – bei vorhandenem VS im Visual Studio Installer prüfen/ergänzen.
3. (erledigt 2026-11-12: `git init -b main`, `git lfs install --local`, erster Commit 1d32729) **Git LFS** prüfen bzw. installieren (Git selbst ist vorhanden) und in `D:\gamer\gamer` ein Repository anlegen (`git init`, `git lfs install`).
4. (erledigt, Version 2.1.177) **Claude Code lokal** installieren (PowerShell, ohne Administrator: `irm https://claude.ai/install.ps1 | iex`) und in `D:\gamer\gamer` starten (so kann es direkt bauen und den Editor ansteuern).
5. (erledigt 2026-11-12: `https://github.com/wking-game/RPG`, **öffentlich** nach Entscheidung des Nutzers, Push als Konto `wking-game`; LFS-Lock-Prüfung lokal abgeschaltet: `lfs.<remote>/info/lfs.locksverify false`) Privates **GitHub-Repository** anlegen (dient auch zur Abstimmung zwischen den Agenten).
6. (angelegt, VS-Projektdateien erzeugt über `Engine\Build\BatchFiles\Build.bat -projectfiles`, `GamerEditor Win64 Development` am 2026-11-12 erfolgreich gebaut; Editor-Start geprüft: lädt in ca. 72 s, Modul `UnrealEditor-Gamer.dll` geladen, 17× „LogAutomationTest: Error: Condition failed“ direkt nach der Engine-Prüfung auf uninitialisierte Struct-Member, Ursache vermutlich Engine-seitig, nicht geklärt) **Unreal-Projekt** in `game/`: C++, leere Vorlage `TP_Blank` als `Gamer`, kein Starter Content. Erster Start: siehe `game/README.md`.
7. (eingerichtet 2026-11-12: Epics Engine-Plugin `ModelContextProtocol` (experimentell) + `EditorToolset`, `ConfigSettingsToolset`, `AutomationTestToolset`, `LiveCodingToolset`, nur Editor-Target. Server lauscht nur auf `127.0.0.1:8000/mcp`, Autostart in lokaler `Saved/Config/WindowsEditor/EditorPerProjectUserSettings.ini` (nicht in Git). Claude-Code-Anbindung über `.mcp.json` im Projektordner. Handshake und `list_toolsets` per curl geprüft; Aus Claude Code heraus geprüft (2026-11-12): `claude mcp list` meldet „Connected“, `list_toolsets` liefert über 20 Toolsets (u. a. Actor, Asset, Blueprint, DataTable, DataAsset, Material, Scene). Voraussetzung: Ordner `D:\gamer\gamer` in Claude Code als vertrauenswürdig bestätigt, Editor läuft. EULA-Hinweis im Log: Über das Plugin übertragene Daten sind „Licensed Technology“, der LLM-Anbieter darf sie nicht zum Training nutzen, UE-EULA Abschnitt 6(e)) Erst danach **MCP/Editor-Anbindung** (Epics experimentelles MCP-Plugin oder ein geprüftes Drittprojekt).

<a id="d5-3"></a>

### 5.3 Hinweise
- Ich habe bewusst kein `git init` in dem verbundenen Ordner ausgeführt: Über diese Verbindung kann Git seine Sperrdateien nicht löschen, das würde das Repository blockieren. Git bitte auf der Windows-Seite initialisieren.
- Alles unter `reference/` ist fremder Code zum Nachlesen. Er wird nicht ausgeführt und ist nicht Teil des Spiels.
- Mit 16 GB RAM wird es beim Arbeiten im Editor eng. Browser und Chat-Fenster beim Bauen schließen.

<a id="d5-4"></a>

### 5.4 Immer nur ein Unreal gleichzeitig (Sperre, seit 2026-11-16)
- **Warum:** Mehrere Sessions starteten Unreal gleichzeitig. Bilder in `Saved/Screenshots`, Protokolle und gespeicherte Menü-Einstellungen mischten sich, Testserver nahmen sich den Port weg, der Speicher reichte nicht.
- **Wie:** `tools/unreal_lock.ps1` legt beim Start die Sperrdatei `game/Saved/unreal.lock` an (nicht im Repo; Inhaber-Skript, PID, Startzeit). Belegt ist Unreal, wenn die Sperrdatei einen laufenden Prozess nennt **oder** irgendein Unreal-Prozess ohne Sperre läuft (offener Editor des Nutzers, MetaHuman-Arbeit, Reste eines Laufs). Dann wartet das Skript und meldet alle 30 s „Warte auf Unreal: …“, höchstens 60 Minuten (Umgebungsvariable `GAMER_UNREAL_LOCK_TIMEOUT` in Minuten). Eine Sperre, deren Prozess nicht mehr existiert, wird übernommen. Freigabe in `finally`, also auch bei Fehler oder Abbruch.
- **Wer sie nutzt:** `mp_test.ps1`, `menu_test.ps1`, `play_local.ps1` (Spiel_testen.bat), `start_menu.ps1` (Spiel_starten.bat), `test_data.ps1`, `build.ps1` (außer `ProjectFiles`; `-Force` überspringt die Sperre), `build_world.ps1`, `import_assets.ps1`, `import_characters.ps1`, `metahuman.ps1`, `build_showcase.ps1`. Verschachtelte Aufrufe (ein Skript mit Sperre ruft ein anderes auf) nutzen dieselbe Sperre weiter (`GAMER_UNREAL_LOCK_TOKEN`).
- **Neues Skript, das Unreal startet:** `. (Join-Path $PSScriptRoot 'unreal_lock.ps1')`, `$lock = Enter-UnrealLock -Owner 'name.ps1'`, Arbeit in `try { … } finally { Exit-UnrealLock $lock }`.
- **Bilder:** `mp_test.ps1` und `menu_test.ps1` kopieren die Bildschirmfotos ihres Laufs nach `<Log-Ordner>/shots` (`Copy-UnrealScreenshots`).
- **Von Hand:** `.\tools\unreal_lock.ps1 -Status` zeigt Inhaber und laufende Unreal-Prozesse; `-Clear` löscht die Sperrdatei (nur, wenn sicher nichts mehr läuft).
- **Grenze:** Wer Unreal direkt startet (Editor per Doppelklick, eigene Befehle), nimmt keine Sperre; die Skripte warten dann trotzdem, weil sie den Prozess sehen. Die Menütests schreiben weiterhin in die echte `GameUserSettings.ini` dieses PCs.


[↑ zum Inhaltsverzeichnis](#inhaltsverzeichnis)

<a id="d6"></a>

## 6. Technische Ziele (Stand 2026-11-12)

> Quelle: `docs/04_technische_ziele.md`

<a id="d6-1"></a>

### 6.1 Vorgaben des Nutzers
| Thema | Vorgabe | Von Claude gedeutet als |
|---|---|---|
| RAM | mindestens 8 GB, optimal 16 GB | Anforderung an den Spieler-PC des fertigen Spiels |
| Speicherplatz | höchstens ca. 20 GB auf SSD (pauschal, vorläufig) | Installationsgröße des fertigen Spiels |
| Spielerzahl | höchstens 8 Spieler (bis 2026-11-12: bis zu 16) | technisches Maximum (`MaxPlayers=8`); Start mit 2–4 |

Falls die 20 GB den Platz für die **Entwicklung** meinen: das reicht nicht, allein Unreal Engine 5.8 braucht mehr.

<a id="d6-2"></a>

### 6.2 Entwicklungs-PC (am 2026-11-12 im Task-Manager abgelesen)
- **GPU:** NVIDIA GeForce RTX 3070, 8,0 GB dedizierter Grafikspeicher, Treiber 32.0.16.1088 (22.07.2026), DirectX 12 (FL 12.2)
- **RAM:** 16 GB (15,9 GB nutzbar). Im normalen Betrieb mit Chrome, ChatGPT, Claude und Steam waren ca. 70 % belegt (11,2 von 15,9 GB).
- **Datenträger:** `C:` SSD (NVMe), `E:` SSD (NVMe), `D:` SSD (SATA). Kapazität und freien Platz habe ich nicht abgelesen.
- **Nicht abgelesen:** CPU-Modell (Takt lag bei ca. 3–4 GHz).
- **Unreal Engine:** auf keinem der drei Laufwerke gefunden. **Git:** vorhanden (`D:\Program Files\Git`).

Folgen: Unreal auf eine NVMe-SSD installieren. 16 GB RAM sind für den Editor knapp (eine genaue Empfehlung habe ich nicht verifiziert). 8 GB Grafikspeicher reichen für ein kleines Projekt mit Budgets, lassen aber wenig Reserve.

<a id="d6-3"></a>

### 6.3 Einschätzung zu den Vorgaben (Claudes Einschätzung, keine Messwerte)

#### Höchstens 8 Spieler (bis 2026-11-12: bis zu 16)
- Unreals Standardwert ist 16 (`MaxPlayers=16` in `Engine/Config/BaseGame.ini`, am 2026-11-13 in der Source-Engine 5.8.4 nachgesehen). Das Projekt setzt ihn in `game/Config/DefaultGame.ini` auf 8; der 9. Spieler bekommt vom Server „Server full.“
- **Ein Spieler-PC als Host für 8 Spieler plus Dorfbewohner-Simulation ist weiterhin anspruchsvoll**, besonders mit 8 GB RAM. Deshalb: **Dedicated-Server-fähig ab Phase 0**, Listen-Server höchstens für kleine Gruppen.
- Die Server-Tickzeit wächst grob linear mit der Zahl der Verbindungen (Forenbeitrag zu UE 5.1 ohne Iris/Push-Model). Ein Spielerbericht zu Conan Exiles Enhanced (UE5) nennt Lags ab etwa 15–18 Spielern, auch bei KI und Kampf. Das ist ein Einzelbericht, ungeprüft, zeigt aber das Risiko.
- Iris (Epics neues Replikationssystem) ist optional. Laut einem Blog war es in UE 5.7 noch experimentell; den Stand in 5.8 habe ich nicht verifiziert. Entscheidung erst nach Messungen.
- Folgen fürs Design: Dorfbewohner-Simulation serverseitig und billig (Zustandsmaschinen statt aufwendiger KI), Bauteile ruhend (dormant), Relevanzradien, Spielerzahl konfigurierbar statt fest verdrahtet.

#### 8 GB RAM als Minimum
- Referenz: Manor Lords (UE5) nennt 8 GB RAM als Minimum (Corsair-Blog). Machbar, aber nur mit Disziplin: niedrige Qualitätsstufe ohne Pflicht auf Lumen/Nanite, Texturpool begrenzen, früh messen.
- Aska selbst steht bei mehreren Aggregator-Seiten mit **16 GB RAM Minimum** (die Steam-Seite war nicht direkt lesbar). Unser Ziel ist also niedriger als beim Leitspiel.

#### 20 GB Installationsgröße
- Askas Speicherangaben sind in den Quellen widersprüchlich (6, 40 oder 50 GB), also keine verlässliche Referenz.
- Vorschlag: Größe des gecookten Builds ab Phase 1 laufend messen, Texturgrößen und Audio früh deckeln, keine Massen an hochauflösenden Fremd-Assets. Die Vorgabe steht auch im ChatGPT-Briefing.

<a id="d6-4"></a>

### 6.4 Zielwerte (Vorschlag, noch nicht bestätigt)
Konkrete Zahlen für Client-RAM, Server-Tick und Bandbreite lege ich erst fest, wenn eine Leer-Szene und danach eine Siedlung gemessen sind (Phase 0 und 1).

<a id="d6-5"></a>

### 6.5 Offen
- GPU-Mindestanforderung für Spieler (und ob Lumen/Nanite Pflicht sind)
- Freier Platz je Laufwerk, CPU-Modell
- Wer hostet Server: Spieler selbst (Dedicated-Server-Programm) oder gemietet?

<a id="d6-6"></a>

### 6.6 Quellen
- Aska-Anforderungen (Aggregator): https://www.pcgamebenchmark.com/aska-system-requirements
- Manor Lords-Anforderungen: https://www.corsair.com/explorer/gamer/gaming-pcs/manor-lords-system-requirements-and-recommended-specs/
- MaxPlayers=16 (Epic-Forum, älter): https://answers.unrealengine.com/questions/824729/view.html
- Server-Tick skaliert mit Verbindungen: https://forums.unrealengine.com/t/server-networking-replication-performance-with-hundreds-of-clients/740652
- Conan Exiles Enhanced, Spielerbericht: https://forums.funcom.com/t/ue5-dedicated-server-severe-desync-lag-scaling-with-player-count-voice-chat-rubberbanding-interaction-delay/298887
- Iris-Einordnung (Blog): https://www.strayspark.studio/blog/tag/multiplayer


[↑ zum Inhaltsverzeichnis](#inhaltsverzeichnis)

<a id="d7"></a>

## 7. Plan: Source-Build der Engine für den Dedicated Server (Stand 2026-11-12)

> Quelle: `docs/05_source_build_plan.md`

Ziel: Phase 0 abschließen – ein Dedicated-Server-Target (`GamerServer`) bauen und mit 2 Clients testen.
Grund: Der Launcher-Build kann keine Server-Targets bauen (Entscheidung in `docs/01_entscheidungen.md`).

<a id="d7-1"></a>

### 7.1 Ausgangslage (am 2026-11-12 gemessen)
| | Wert |
|---|---|
| CPU | Intel Core i5-12400F, 6 Kerne / 12 Threads |
| RAM | 16 GB, Auslagerungsdatei auf `D:` |
| `C:` (NVMe) | 16 GB frei, nach Verlegen der Build Tools auf `E:` **26,3 GB frei** (2026-11-12) |
| `D:` (SATA-SSD) | 876 GB frei |
| `E:` (NVMe) | 322 GB frei |
| Toolchain | Build Tools 2026 in `E:\VS\BuildTools` mit MSVC 14.50, Windows SDK 10.0.26100, .NET Framework 4.8 SDK |
| Zugriff auf `EpicGames/UnrealEngine` | vorhanden über GitHub-Konto `wking-game` (2026-11-12), Tag `5.8.4-release` sichtbar |

<a id="d7-2"></a>

### 7.2 Schätzungen (nicht gemessen, für UE 5.8 nicht verifiziert)
- **Speicher:** Quellcode + Abhängigkeiten (`Setup.bat`) + Build von Editor und Server: grob **150–250 GB**.
- **Dauer erster Engine-Build:** auf 6 Kernen mit 16 GB RAM grob **4–8 Stunden**. Unreal begrenzt die parallelen Compiler-Prozesse nach freiem RAM (beim Projekt-Build waren es 3 statt 6), deshalb eher am oberen Ende.
- Spätere Builds sind inkrementell und deutlich schneller.

<a id="d7-3"></a>

### 7.3 Schritte
#### A. Vom Nutzer zu erledigen (Konten, nicht automatisierbar)
1. (entschieden 2026-11-12) GitHub-Konto für den Engine-Zugang: **`wking-game`**.
2. (erledigt 2026-11-12) Auf epicgames.com: Konto → „Apps und Konten“ → GitHub verbinden (mit dem gewählten Konto).
3. (erledigt 2026-11-12) Die E-Mail-Einladung in die GitHub-Organisation `EpicGames` annehmen.
   Danach sieht das Konto `github.com/EpicGames/UnrealEngine`.

#### B. Vorbereitung (Claude, nach Freigabe)
4. **`C:` entlasten:** Cache-Pfade der Engine auf `D:`/`E:` legen (z. B. lokaler Derived Data Cache über die Umgebungsvariable `UE-LocalDataCachePath`). Ohne das kann `C:` beim Build volllaufen. Genaue Variablen vor dem Setzen in der Epic-Doku prüfen.
5. Zielordner mit **kurzem Pfad** wählen (Windows-Pfadlängen), Vorschlag `E:\UE58` (NVMe, schneller). Alternative `D:\UE58` (mehr Platz, SATA).
6. Den passenden Release-Tag ermitteln (`git ls-remote --tags`, erwartet etwas wie `5.8.4-release`, passend zur installierten 5.8.4).

#### C. Engine bauen (Claude, läuft lange im Hintergrund)
7. Flacher Klon nur dieses Tags: `git clone --depth 1 --branch <tag> https://github.com/EpicGames/UnrealEngine.git E:\UE58`
8. `Setup.bat` (lädt Abhängigkeiten, installiert Voraussetzungen), dann `GenerateProjectFiles.bat`.
9. Engine-Editor bauen (`UnrealEditor Win64 Development` und die Hilfsprogramme wie ShaderCompileWorker).

#### D. Projekt umstellen (Kerndateien → jeweils Rückfrage)
10. `Gamer.uproject`: `EngineAssociation` auf den Source-Build umstellen (per Rechtsklick „Switch Unreal Engine version“ oder Eintrag in der Registry).
11. Neue Datei `game/Source/GamerServer.Target.cs` (Type = Server).
12. `tools/build.ps1`: Schritt `Server` ergänzen, `UE_ROOT` auf den Source-Build zeigen lassen.

#### E. Prüfen
13. `GamerServer` bauen und als Server-Paket packen (`BuildCookRun -server -noclient -serverplatform=Win64`).
14. Test: Dedicated Server + 2 Clients auf diesem PC; Server-RAM und Tick-Zeit notieren (Basiswert für `docs/04_technische_ziele.md`).
15. Phase 0 im Phasenplan abschließen.

<a id="d7-4"></a>

### 7.4 Risiken und Nachteile
- **Zeit und Speicher:** mehrere Stunden Build, 150–250 GB belegt. Der PC ist in der Zeit stark ausgelastet.
- **`C:` fast voll:** ohne Schritt 4 Risiko eines Abbruchs mitten im Build.
- **Zwei Engines parallel:** Launcher-Build bleibt als Rückfallebene. Projekt-Binaries müssen beim Wechsel neu gebaut werden; Editor-Einstellungen und Plugins (MCP) sind im Source-Build ebenfalls vorhanden, aber dort nicht geprüft.
- **Updates:** Engine-Updates heißen beim Source-Build: neuen Tag holen und neu bauen.
- **Lizenz:** Der Engine-Quellcode fällt unter die Unreal-EULA; er darf nicht in unser öffentliches Repo. Er liegt außerhalb von `D:\gamer\gamer`, also kein Risiko, solange der Ordner nicht ins Repo verschoben wird.

<a id="d7-5"></a>

### 7.5 Alternative
Den Source-Build verschieben und Phase 1 mit Listen-Server beginnen. Nachteil: Code, der versehentlich einen Listen-Server voraussetzt, fällt erst später auf (Architekturregel 10).


[↑ zum Inhaltsverzeichnis](#inhaltsverzeichnis)

<a id="d8"></a>

## 8. Datenformat für Spielinhalte (Stand 2026-11-13)

> Quelle: `docs/06_datenformat.md`

Status: **bestätigt am 2026-11-13** (JSON, Ort `game/Content/Data/`, Werkzeug-IDs mit Stufe, Startwerte aus Abschnitt 6 zum Ausprobieren; siehe `docs/01_entscheidungen.md`). Gilt ab Phase 1, Schritt 2.

Technischer Hinweis: Das Spielmodul braucht das Engine-Modul `Json` als eigene Abhängigkeit in `Gamer.Build.cs`. Über die Engine ist es nur zum Einbinden der Header sichtbar, nicht zum Verknüpfen; der Build scheitert sonst an fehlenden JSON-Funktionen (am 2026-11-13 festgestellt).

<a id="d8-1"></a>

### 8.1 1. Worum es geht
Architekturregel 3 sagt: Gegenstände, Rezepte, Bauteile, Jobs, Gegner usw. stehen in Tabellen, nicht im Code. Dieses Dokument legt fest, **wie** diese Tabellen aussehen, **wo** sie liegen und **wie Unreal sie liest**.

Warum das wichtig ist: Später wird es mehrere hundert Einträge geben. Wenn das Format am Anfang unklar ist, muss man es später überall ändern. Ein klares Format erlaubt außerdem, dass Claude, ChatGPT und du Werte ändern, ohne C++ anzufassen oder neu zu bauen.

<a id="d8-2"></a>

### 8.2 2. Empfehlung in einem Satz
**JSON-Dateien (eine pro Tabelle) in `game/Content/Data/`, die ein C++-Baustein beim Start auf Server und Spielern einliest und prüft.**

#### 2.1 Warum JSON (und nicht Unreal-DataTables oder Excel/CSV)
| | JSON-Dateien (empfohlen) | Unreal-DataTable (.uasset) | CSV (Excel) |
|---|---|---|---|
| Lesbar und in Git vergleichbar | ja, Zeile für Zeile | nein (Binärdatei) | ja |
| Von Claude/ChatGPT direkt bearbeitbar | ja | nur über den Editor | ja |
| Verschachtelte Angaben (z. B. „Rezept braucht 2 Zutaten“) | ja | ja | umständlich |
| Ändern ohne Neubau | ja (neu laden per Befehl) | nein, Editor nötig | ja |
| Prüfung im Unreal-Editor | nein, dafür eigener Prüfbaustein | ja | nein |

Typischer Fehler, den das vermeidet: Bei DataTables als Binärdatei sieht man in Git nicht, *welcher* Wert sich geändert hat, und zwei Leute, die gleichzeitig ändern, überschreiben sich gegenseitig.

Nachteil von JSON: Unreal kennt die Verweise auf Modelle und Icons darin nicht von selbst. Deshalb (1) prüft der eigene Baustein beim Start jeden Verweis, und (2) wird der Asset-Ordner beim Paketieren immer mitgenommen (Einstellung `DirectoriesToAlwaysCook`).

#### 2.2 Warum `game/Content/Data/` statt `data/` im Hauptordner
Beim Paketieren nimmt Unreal nur Dateien **innerhalb des Projektordners `game/`** mit. `data/` liegt eine Ebene darüber und würde im fertigen Spiel und im Server-Paket fehlen. Die Alternative (Sonderregel in `Gamer.Build.cs`) ist eine Kerndatei mehr und schwerer zu durchschauen.
Vorschlag: `data/README.md` bleibt als Wegweiser stehen und zeigt auf `game/Content/Data/`. Unreal ignoriert `.json`-Dateien im Content-Ordner im Editor; beim Paketieren werden sie über `DirectoriesToAlwaysStageAsUFS=(Path="Data")` in `DefaultGame.ini` eingepackt.

<a id="d8-3"></a>

### 8.3 3. Regeln für alle Tabellen
- **Eine Datei pro Tabelle**, Aufbau immer gleich: `{ "schema": 1, "rows": [ … ] }`. `schema` zählt hoch, wenn sich das Format ändert.
- **IDs:** `snake_case`, stabil, **nie wiederverwenden**. Präfixe wie im Asset-Plan: `item_` Gegenstand, `tool_` Werkzeug, `res_` Ressourcen-Vorkommen (Baum, Fels, Strauch), `building_` Bauteil oder Gebäude, `recipe_` Rezept. Später: `job_`, `need_`, `enemy_`, `skill_`.
- **Löschen gibt es nicht:** Ein Eintrag, der wegfällt, bekommt `"deprecated": true`. So zeigen alte Spielstände nie auf eine ID, die es plötzlich für etwas anderes gibt.
- **Einheiten:** Länge in cm (wie Unreal), Zeit in Sekunden, Gewicht in kg, Spielzeit in Spieltagen.
- **Anzeigenamen:** vorerst `"name"` auf Deutsch. Übersetzungen kommen später über Unreals String-Tables; die ID bleibt dabei der Schlüssel.
- **Verweise auf Modelle/Icons:** voller Unreal-Pfad als Text, z. B. `"/Game/Assets/Buildings/WoodKit/SM_Building_Wall_Wood"`. Fehlt ein Modell noch, bleibt das Feld leer, und das Spiel zeigt einen Platzhalter.
- **Werte sind Startwerte.** Alles in Abschnitt 6 ist ein Vorschlag zum Ausprobieren und wird beim Spielen angepasst.

<a id="d8-4"></a>

### 8.4 4. Die Tabellen für Phase 1
#### 4.1 `items.json` – Gegenstände und Werkzeuge
| Feld | Pflicht | Bedeutung |
|---|:-:|---|
| `id` | ✓ | `item_…` oder `tool_…` |
| `name` | ✓ | Anzeigename |
| `category` | ✓ | `resource` (Rohstoff), `material` (hergestellt), `food`, `tool` |
| `tier` | ✓ | Stufe 1–6 (T1 Rohstufe … T6 Relikt) |
| `max_stack` | ✓ | wie viele in einen Inventarplatz passen |
| `weight_kg` | ✓ | für die Traglast |
| `icon` | | Pfad zum Icon |
| `world_mesh` | | Pfad zum Modell, wenn man den Gegenstand in der Welt sieht |
| `tool` | nur Werkzeuge | `{ "type": "axe", "durability": 150, "work_speed": 1.0 }` |
| `food` | nur Gerichte/Getränke | Mahlzeiten-Buffs: `{ "minutes": 10, "buffs": [ { "stat": "health", "value": 10 } ] }`. `stat`: `health`, `regen`, `carry`, `cold_resist`, `poison_resist`, `damage` (nur Boni, Regel 5). Nur bei `category` `food`. Siehe `docs/12_taverne_gerichte.md` |

Beispiel:
```json
{ "id": "tool_axe_t1", "name": "Steinaxt", "category": "tool", "tier": 1,
  "max_stack": 1, "weight_kg": 2.0,
  "tool": { "type": "axe", "durability": 150, "work_speed": 1.0 } }
```
Werkzeuge bekommen die Stufe in der ID (`tool_axe_t1`, `tool_axe_t2` …), weil jede Stufe ein eigener Gegenstand mit eigenen Werten ist. `tool_axe` aus dem Asset-Plan bleibt der Name des **Modells**, das sich alle Stufen teilen.

#### 4.2 `resource_nodes.json` – Bäume, Felsen, Sträucher
| Feld | Pflicht | Bedeutung |
|---|:-:|---|
| `id` | ✓ | `res_…` |
| `name` | ✓ | Anzeigename |
| `required_tool` | | `{ "type": "axe", "min_tier": 1 }`; fehlt es, geht es mit der Hand |
| `work_seconds` | ✓ | Arbeitszeit bei Werkzeug-Tempo 1,0 |
| `drops` | ✓ | Liste `{ "item": "item_log", "min": 2, "max": 3 }` |
| `regrow_days` | | nach wie vielen Spieltagen es nachwächst; fehlt es, wächst nichts nach |
| `meshes` | | `{ "full": …, "depleted": …, "remains": … }` (stehend / abgeerntet / Stumpf). Umgesetzt (2026-11-13): `full` und `depleted` (bei Bäumen der Stumpf, bei Sträuchern dieselbe Form ohne Früchte); fehlt `depleted`, verschwindet das Vorkommen beim Ernten. Dazu `"growing": [ …, … ]`: Modelle fürs sichtbare Nachwachsen (z. B. Setzling, junger Baum); in der ersten Hälfte der Nachwachszeit steht `depleted`, in der zweiten Hälfte folgen die Stufen gleichmäßig, geerntet werden kann erst wieder voll. `remains` wird noch nicht genutzt. Ohne `meshes` zeigt das Spiel Platzhalter-Formen |

#### 4.3 `recipes.json` – Herstellung
| Feld | Pflicht | Bedeutung |
|---|:-:|---|
| `id` | ✓ | `recipe_…` |
| `station` | | `building_…`, an dem hergestellt wird; fehlt es, geht es aus der Hand |
| `inputs` | ✓ | Liste `{ "item": …, "count": … }` |
| `outputs` | ✓ | Liste `{ "item": …, "count": … }` |
| `work_seconds` | ✓ | Herstellungsdauer |

#### 4.4 `buildables.json` – Bauteile und Arbeitsstätten
| Feld | Pflicht | Bedeutung |
|---|:-:|---|
| `id` | ✓ | `building_…` |
| `name` | ✓ | Anzeigename |
| `category` | ✓ | `structure` (Baukasten), `workstation`, `storage`, `comfort` (Bett, Feuer) |
| `snap` | nur `structure` | `foundation`, `floor`, `wall`, `wall_door` (Wand mit Türöffnung, bietet den Platz für die Tür), `door`, `pillar`, `beam`, `roof` (Dachfläche), `gable` (Giebel), `ridge` (First), `stairs`; Raster 300 cm wie der Holzbaukasten. `wall_door`, `gable`, `ridge` seit 2026-11-13 (vorher liefen Giebel und First unter `roof`, die Türwand unter `wall`; das Einrasten braucht die Unterscheidung) |
| `mesh` | | Unreal-Pfad zum Modell, z. B. `/Game/Gamer/Buildings/WoodKit/SM_Building_Wall_Wood.SM_Building_Wall_Wood` (Import aus `Assets/` mit `tools/import_assets.ps1`). Ohne `mesh` zeigt das Spiel einen Platzhalter aus Grundformen |
| `cost` | ✓ | Liste `{ "item": …, "count": … }` |
| `build_seconds` | ✓ | Bauzeit |
| `required_tool` | | meist `{ "type": "hammer", "min_tier": 1 }` |

#### 4.5 `jobs.json` – Berufe der Dorfbewohner (seit 2026-11-13)
| Feld | Pflicht | Bedeutung |
|---|:-:|---|
| `id` | ✓ | `job_…` |
| `name` | ✓ | Anzeigename („Holzfäller“) |
| `kind` | ✓ | `harvest` (Vorkommen abbauen/sammeln), `craft` (Rezepte an der Station), `build` (Baupläne hämmern) |
| `station` | | Bauteil, von dem aus gearbeitet wird (muss fertig stehen); ohne: vom Lager aus |
| `tool` | | Werkzeugtyp, den der Bewohner aus dem Lager nimmt (`axe`, `pickaxe`, `hammer`); ohne: von Hand |
| `recipes` | bei `craft` | Rezept-IDs |
| `stock_target` | | `craft`: aufhören, wenn so viel vom Ergebnis im Lager liegt (Standard 10) |
| `radius` | | Arbeitsbereich in cm um Station bzw. Lager (Standard 3000) |
| `fallback` | | genau ein Beruf hat `true`: der Ausweich-Beruf, wenn der eigene nicht geht |

#### 4.6 `needs.json` – Bedürfnisse der Dorfbewohner (seit 2026-11-13)
Werte laufen von 1 (gut) bis 0 (leer), Zeiten in Spieltagen (1 Spieltag = 30 min).
| Feld | Pflicht | Bedeutung |
|---|:-:|---|
| `id` | ✓ | `need_…` |
| `name` | ✓ | Anzeigename |
| `kind` | ✓ | `food`, `sleep`, `shelter` (eigenes Bett unter einem Dach) |
| `decay_per_day` | | Abnahme pro Spieltag (wach) |
| `threshold` | | darunter kümmert sich der Bewohner zuerst darum |
| `restore_per_day` | | `sleep`: Zunahme pro Spieltag im Bett (ohne Bett die Hälfte) |
| `foods` | bei `food` | `[{ "item": …, "value": 0..1 }]`, bestes zuerst |
| `work_speed_when_unmet` | | Arbeitstempo, solange das Bedürfnis nicht erfüllt ist (z. B. 0,7) |
| `leave_after_days` | | `food`: geht nach so vielen Spieltagen bei 0 (0 = nie) |

`jobs.json` und `needs.json` sind optional (fehlen sie, gibt es keine Bewohner-Daten, aber keinen Fehler). Der Prüfer kontrolliert Stationen, Werkzeugtypen, Rezepte und Speisen.

Später dazu: Gegner, Fähigkeiten, Quests.

<a id="d8-5"></a>

### 8.5 5. Wie Unreal die Daten benutzt
1. **Einlesen:** Ein C++-Baustein (`UGamerDataSubsystem`) liest beim Start alle Dateien – auf dem Server **und** bei jedem Spieler, damit alle dieselben Zahlen kennen.
2. **Prüfen** direkt beim Einlesen, mit klarer Fehlermeldung im Protokoll:
   doppelte IDs · Verweise auf IDs, die es nicht gibt (z. B. Rezept nennt `item_plnk`) · fehlende Pflichtfelder · Werte außerhalb des Sinnvollen (Stufe 7, negative Mengen) · Pfade zu Modellen/Icons, die es nicht gibt (nur Warnung, solange Modelle fehlen).
   **Erreichbarkeit (seit 2026-11-13):** Ausgehend von „nichts im Inventar“ wird durchgespielt, was man mit der Hand sammeln, daraus herstellen und mit den neuen Werkzeugen weiter abbauen oder bauen kann. Jeder Gegenstand, jedes Vorkommen, Rezept und Bauteil, das dabei nie erreichbar wird, ist ein Fehler (z. B. „Axt braucht Stamm, Stamm braucht Axt“). Gegenstände, die es später nur bei Händlern oder Bossen gibt, bekommen dafür eine eigene Kennzeichnung, sobald es Händler und Bosse gibt.
3. **Benutzen:** Spielcode fragt nur per ID: „Was ist `item_log`?“ Übers Netz gehen nur IDs und Mengen, nie ganze Datensätze – das spart Bandbreite.
4. **Ändern während der Entwicklung:** Konsolenbefehl `gamer.ReloadData` liest neu ein, ohne Neustart.
5. **Automatischer Test:** ein Unreal-Automationstest „Gamer.Data“, der alle Dateien lädt und bei jedem Fehler fehlschlägt. Läuft ohne Grafik in ca. 1 Minute; ein Skript `tools/test_data.ps1` startet ihn.
6. **Später (vor dem ersten öffentlichen Test):** Der Server schickt beim Beitritt eine Prüfsumme der Daten; ein Spieler mit anderen Daten wird abgewiesen, statt mit falschen Werten zu spielen.

Typische Fehlerquellen und was dagegen hilft:
- **Tippfehler in einer ID** → Prüfung in Schritt 2 meldet die Stelle.
- **Server und Spieler haben unterschiedliche Daten** (z. B. nur einer neu gebaut) → Prüfsumme in Schritt 6.
- **Modell fehlt im fertigen Spiel**, weil nur die JSON-Datei darauf zeigt → Asset-Ordner wird immer mitgepackt (Abschnitt 2.1).
- **Werte driften auseinander** (Design-Dokument sagt A, Datei sagt B) → die JSON-Datei ist die einzige Quelle für Zahlen; Design-Dokumente beschreiben nur Absichten.

<a id="d8-6"></a>

### 8.6 6. Phase-1-Inhalt (Startwerte, Vorschlag)
#### Gegenstände
| ID | Name | Kategorie | Stufe | Stapel | kg |
|---|---|---|:-:|--:|--:|
| `item_log` | Baumstamm | resource | 1 | 10 | 20 |
| `item_stone` | Stein | resource | 1 | 20 | 5 |
| `item_stick` | Äste | resource | 1 | 30 | 0,5 |
| `item_fiber` | Pflanzenfasern | resource | 1 | 50 | 0,1 |
| `item_plank` | Bretter | material | 1 | 40 | 2 |
| `item_berries` | Beeren | food | 1 | 50 | 0,1 |
| `item_apple` | Wildäpfel | food | 1 | 30 | 0,2 |
| `item_mushrooms` | Pilze | food | 1 | 30 | 0,1 |
| `item_hazelnuts` | Haselnüsse | food | 1 | 50 | 0,05 |
| `item_meal` | einfache Mahlzeit | food | 1 | 10 | 0,5 |
| `tool_axe_t1` | Steinaxt | tool | 1 | 1 | 2 |
| `tool_pickaxe_t1` | Steinspitzhacke | tool | 1 | 1 | 3 |
| `tool_hammer_t1` | Steinhammer | tool | 1 | 1 | 1,5 |

Werkzeuge T1: Haltbarkeit 150 Einsätze, Tempo 1,0.

#### Vorkommen
| ID | Werkzeug | Sekunden | Ertrag | Nachwachsen |
|---|---|--:|---|--:|
| `res_tree_pine` | Axt T1 | 6 | 2–3 Stämme | 3 Tage |
| `res_tree_birch` | Axt T1 | 5 | 2 Stämme | 3 Tage |
| `res_rock_small` | Spitzhacke T1 | 3 | 2 Steine | 5 Tage |
| `res_rock_medium` | Spitzhacke T1 | 6 | 4 Steine | 5 Tage |
| `res_rock_large` | Spitzhacke T1 | 10 | 8 Steine | 5 Tage |
| `res_berry_bush` | Hand | 2 | 3–5 Beeren | 1 Tag |
| `res_tree_apple` | Hand (pflücken) | 2,5 | 3–6 Wildäpfel | 2 Tage |
| `res_mushroom_patch` | Hand | 1,5 | 2–4 Pilze | 1 Tag |
| `res_hazel_bush` | Hand | 2 | 4–8 Haselnüsse | 2 Tage |
| `res_loose_stone` | Hand | 1,5 | 1–2 Steine | 1 Tag |
| `res_fallen_branch` | Hand | 1,5 | 1–2 Äste | 1 Tag |
| `res_fiber_plant` | Hand | 2 | 2–3 Fasern | 1 Tag |

Nachwachsen ist ein Vorschlag aus dem Design-Register (Kapitel 15), noch nicht bestätigt.
Die drei Hand-Vorkommen (seit 2026-11-13) sind der Einstieg: Damit kommt man ohne Werkzeug an Steine, Äste und Fasern für die ersten Werkzeuge. Felsen geben mit der Spitzhacke deutlich mehr Steine.

#### Rezepte
| ID | Wo | aus | wird zu | Sekunden |
|---|---|---|---|--:|
| `recipe_axe_t1` | Hand (Anfangs-Crafting) | 1 Ast + 2 Steine + 2 Fasern | Steinaxt | 5 |
| `recipe_pickaxe_t1` | Hand (Anfangs-Crafting) | 2 Äste + 3 Steine + 2 Fasern | Steinspitzhacke | 6 |
| `recipe_hammer_t1` | Hand (Anfangs-Crafting) | 1 Ast + 1 Stein + 1 Faser | Steinhammer (Bauhammer) | 4 |
| `recipe_plank` | Werkbank | 1 Stamm | 4 Bretter | 4 |
| `recipe_meal` | Lagerfeuer | 3 Beeren | 1 Mahlzeit | 10 |

Hinweis zur Reihenfolge: Die Werkbank kostet **keine Bretter** (sonst bräuchte man Bretter, um Bretter herzustellen).
Bis 2026-11-13 brauchten die Werkzeug-Rezepte einen Baumstamm – den es aber nur mit Axt gibt. Deshalb jetzt Äste statt Stämme, und der Datenprüfer kontrolliert, ob jeder Gegenstand vom Spielstart aus erreichbar ist (Abschnitt 5).

#### Bauteile und Arbeitsstätten (Hammer T1 nötig)
| ID | Kosten | Sekunden |
|---|---|--:|
| `building_foundation_wood` | 2 Stämme + 4 Steine | 4 |
| `building_floor_wood` | 6 Bretter | 3 |
| `building_wall_wood` | 6 Bretter | 3 |
| `building_wall_door_wood` | 5 Bretter | 3 |
| `building_door_wood` | 4 Bretter | 2 |
| `building_pillar_wood`, `building_beam_wood` | je 2 Bretter | 2 |
| `building_roof_wood` | 6 Bretter | 3 |
| `building_roof_ridge_wood` | 3 Bretter | 2 |
| `building_roof_gable_wood` | 4 Bretter | 3 |
| `building_workbench` | 3 Stämme + 4 Steine | 6 |
| `building_campfire` | 10 Steine + 2 Stämme | 5 |
| `building_bed` | 4 Bretter | 4 |
| `building_storage` | 30 Bretter + 10 Steine | 20 |
| `building_woodcutter` | 2 Stämme + 4 Bretter | 8 |
| `building_stonecutter` | 2 Stämme + 8 Steine | 8 |

<a id="d8-7"></a>

### 8.7 7. Offene Fragen an den Nutzer
1. **Format:** JSON wie empfohlen?
2. **Ort:** `game/Content/Data/` statt `data/` (wegen Paketieren)?
3. **Werkzeug-IDs mit Stufe** (`tool_axe_t1`) statt einer ID mit Stufenfeld?
4. **Startwerte in Abschnitt 6:** so zum Ausprobieren übernehmen?

<a id="d8-8"></a>

### 8.8 8. Was danach passiert (nach Bestätigung)
1. Die vier JSON-Dateien mit den Werten aus Abschnitt 6 anlegen.
2. `UGamerDataSubsystem` mit Einlesen und Prüfung in C++.
3. Automationstest „Gamer.Data“ und `tools/test_data.ps1`.
4. Zwei Einträge in `DefaultGame.ini` (Kerndatei, Rückfrage): Daten-Ordner einpacken, Asset-Ordner immer mitnehmen.
5. Test: Server und 2 Spieler lesen dieselben Daten (Prüfsumme im Protokoll vergleichen).

<a id="d8-9"></a>

### 8.9 Ergänzung 2026-11-14: Verderb, Obst-Zyklus, Feldbau, Ausbaustufen

Details und Werte: `docs/10_nahrung_feldbau.md` (Abschnitt „Umsetzung“). Neue Felder:

| Tabelle | Feld | Bedeutung |
|---|---|---|
| `items.json` | `spoil_days` | Haltbarkeit in Spieltagen, getragen (Lager ×4, Boden ×0,5, je Stufe über T1 +50 %). Fehlt = verdirbt nicht |
| `items.json` | `spoils_into` | wird dazu, wenn verdorben (Standard `item_spoiled_food`) |
| `items.json` | `plant` | Saatgut: `{ "grow_days", "harvest": [{item,min,max}], "meshes": [Keimling, jung, reif] }`, Ertrag je Feld-Einheit |
| `resource_nodes.json` | `season` | Jahreszyklus: `bloom`, `unripe`, `ripe`, `fall` (optional), `bare` (Phasen 0–4, aufsteigend), `pick`, `drop_height_cm` [min,max], `bruise_cm` + `bruised_item`; genau ein Eintrag in `drops` = Früchte pro Jahr |
| `resource_nodes.json` | `meshes.bloom`, `meshes.unripe` | Modelle für Blüte und unreife Früchte |
| `buildables.json` | `category: "farm"` + `farm` | `kind`: `field` (`water_item`, `water_per_day`, `fertilizer_item`, `fertilizer_bonus`, `grow_from`, `grow_to`, `min_size_cm`, `max_size_cm`, `unit_m2`), `composter` (`inputs`, `per_output`, `output`, `days_per_output`, `capacity`), `water` (`output`, `per_day`, `capacity`) |
| `buildables.json` | `workers`, `upgrades` | Arbeitsplätze auf Stufe 1; Ausbaustufen `[{ "cost": [...], "workers": N }]`, jede mit mehr Plätzen |
| `jobs.json` | `kind: "farm"` | Feldarbeit rund um die Station (Hof) |
| `buildables.json` | `requires` | `{ "building", "level", "radius_cm" }`: nur in der Nähe eines fertigen Gebäudes dieser Stufe baubar (Kochstellen bei der Taverne) |
| `buildables.json` | `tavern` | `{ "stock", "mood_per_tier", "mood_drink" }`: Taverne mit Vorrat für Gerichte und Getränke, Stimmung beim Essen dort |
| `jobs.json` | `kind: "serve"` | Wirt: bringt Gerichte und Getränke aus dem Lager in die Taverne (Station mit `tavern`) bis `stock_target` |
| `jobs.json` | `kind: "craft"` + `radius` | Rezepte, deren `station` zur Station gehört (`requires`), kocht der Koch an der nächsten solchen Kochstelle im Umkreis |

<a id="d8-10"></a>

### 8.10 Ergänzung 2026-11-16: `difficulty.json` – Schwierigkeitsstufen

Eine Datei mit `"schema": 1` und der Liste `"presets"` (nicht `"rows"`). Jede Stufe ist eine Voreinstellung mehrerer Weltoptionen; die Werte stehen nur hier, nicht im Code. Geladen von `GamerDifficulty.cpp`, geprüft von den Tests `Gamer.Data.DifficultyPresets` und `Gamer.Data.DifficultyValidatorFindsErrors`.

| Feld | Bedeutung |
|---|---|
| `id` | `difficulty_…`, stabil |
| `level` | `easy`, `medium`, `hard`, `hardcore` – jede genau einmal (Weltoption `?Difficulty=`; `custom` hat keinen Eintrag) |
| `name`, `description` | Anzeige im Menü |
| `death` | `none`, `grave`, `hardcore` (Weltoption `?Death=`) |
| `needs_rate` | Faktor auf Hunger/Durst/Erschöpfung (0,1–5, `?NeedsRate=`) |
| `damage_taken` | Faktor auf erlittenen Schaden (0,1–5, `?Damage=`) |
| `yield` | Faktor auf den Ertrag beim Abbauen (0,1–5, `?Yield=`) |
| `hordes`, `villagers_immortal` | `true`/`false` (streng, kein Text) |
| `demolish` | `full`, `half`, `off` |
| `gespuer_tempo` | `slow`, `normal`, `fast` |

Regeln: „Mittel“ muss den Standardwerten einer neuen Welt entsprechen (sonst wäre eine unveränderte Welt „Benutzerdefiniert“; der Test prüft das). Startet eine Welt mit einer Stufe, übernimmt der Server die Werte aus seiner eigenen Datei, auch wenn die Karten-URL andere enthält; bei `custom` gelten die URL-Werte.



[↑ zum Inhaltsverzeichnis](#inhaltsverzeichnis)

<a id="d9"></a>

## 9. KI-3D-Dienste: Lizenz- und Eignungsvergleich (Stand 2026-11-13)

> Quelle: `docs/07_ki3d_dienste.md`

Grundlage: Entscheidung „Herkunft 3D-Assets“ vom 2026-11-12 (`docs/01_entscheidungen.md`): organische Einzelobjekte aus einem KI-3D-Werkzeug mit automatischer Nachbearbeitung in Blender. Offen war nur, welcher Dienst. Dieses Blatt ist Claudes Recherche und Empfehlung, keine Entscheidung.

**Ehrlicher Hinweis:** Die offiziellen Nutzungsbedingungen (Terms of Service) der Dienste habe ich nicht selbst gelesen. Die Angaben stammen aus Hilfeseiten und Preisseiten der Anbieter, Lizenztexten auf GitHub/Hugging Face und Testberichten, die sich teils widersprechen. Vor dem Abschluss eines Abos die aktuellen Bedingungen auf der Seite des Anbieters prüfen.

<a id="d9-1"></a>

### 9.1 Bedingungen für dieses Projekt
- Verkauf auf Steam: Nutzung muss kommerziell erlaubt sein; KI-Inhalte werden im Steam-Formular angegeben und in `design/asset_log.md` eingetragen.
- Standort EU (Deutschland).
- Entwicklungs-PC: RTX 3070 mit 8 GB Grafikspeicher, Windows.
- Budget klein; Konto und Bezahlung macht der Nutzer selbst (Claude legt keine Konten an und zahlt nichts).

<a id="d9-2"></a>

### 9.2 Vergleich

| Dienst | Kommerziell | Kosten | Läuft wo | Eignung hier |
|---|---|---|---|---|
| **Meshy** | Bezahlte Stufen: private Modelle, kommerziell ohne Namensnennung. Kostenlose Stufe: CC BY 4.0 (kommerziell mit Namensnennung, Modelle öffentlich). Modelle aus der Abo-Zeit behalten die Rechte nach einer Kündigung (laut Hilfeseite) | Pro ca. 20 $/Monat (1.000 Credits, verfallen monatlich), Programmierschnittstelle (API) ab Pro | Cloud | **gut**: ausgereift, API für eine automatische Pipeline, Polygonzahl einstellbar |
| **Tripo** | Bezahlte Stufen: privat, kommerziell. Kostenlose Stufe: öffentlich, CC BY 4.0; ob kommerziell erlaubt, ist widersprüchlich beschrieben | ab ca. 12–20 $/Monat | Cloud | gut, ähnlich Meshy; kostenlose Stufe rechtlich unklar |
| **Hyper3D Rodin** | kommerziell erst in bezahlten Stufen, welche genau, ist widersprüchlich (Creator ca. 30 $ oder Business ca. 120 $/Monat) | höher | Cloud | möglich, aber teurer und unklarer |
| **Stable Fast 3D / SPAR3D** (Stability AI) | Community License: kostenlos, auch kommerziell, bis 1 Mio. $ Jahresumsatz | kostenlos | lokal, ca. 7 GB Grafikspeicher (Angaben Dritter) | möglich: keine Abokosten, aber schwächere Qualität und mehr Einrichtung; Modellzugang auf Hugging Face nur nach Zustimmung zur Lizenz |
| **Hunyuan3D** (Tencent) | Lizenz gilt **nicht in der EU**, im Vereinigten Königreich und Südkorea | – | – | **scheidet aus** (Standort EU) |
| **TRELLIS.2** (Microsoft) | MIT-Lizenz, frei | kostenlos | lokal nur mit mind. 24 GB Grafikspeicher, nur Linux getestet | **scheidet lokal aus** (8 GB); nur mit gemieteter Cloud-GPU |

<a id="d9-3"></a>

### 9.3 Empfehlung
1. **Erst kostenlos testen:** 2–3 Vorkommen (z. B. Kiefer, Felsen, Beerenstrauch) in der kostenlosen Meshy-Stufe über die Weboberfläche erzeugen, als GLB/FBX in `Assets/Source/ki3d/` ablegen. Claude baut die Blender-Nachbearbeitung (Maßstab, Drehpunkt, Polygonzahl, Material, Zustände voll/abgeerntet/Rest) und den Import. Danach im Spiel beurteilen. Diese Testmodelle sind CC BY 4.0 und öffentlich: für den Test in Ordnung, für das fertige Spiel nur mit Namensnennung.
2. **Wenn die Qualität passt: Meshy Pro für einen Monat** (ca. 20 $) für alle Slice-Pflanzen und Felsen, über die API automatisiert. Der Schlüssel liegt dann in einer Umgebungsvariable auf dem PC des Nutzers, nie im Repository.
3. Wenn keine Abokosten gewünscht sind: Stable Fast 3D lokal, mit geringerer Qualität.

<a id="d9-4"></a>

### 9.4 Stand: kein Bezahldienst (Nutzer, 2026-11-13)
Zuerst gewählt: Meshy Pro („Gleich Meshy Pro“). Noch am selben Tag geändert: Vegetation kostenlos selbst erzeugen („da brauche ich nicht extra irgendwie Geld ausgeben“). Es wurde nichts bei Meshy erzeugt und kein Credit verbraucht. Das Abrufskript bleibt als Option liegen; die Blender-Nachbearbeitung und der Import taugen für jedes GLB (auch aus freien Generatoren).

#### Kostenlose Wege (Recherche 2026-11-13)
| Weg | Lizenz | Vorteile | Nachteile |
|---|---|---|---|
| **Eigener Generator in Blender (Python)**, im vorhandenen Asset-Skript | eigenes Werk | volle Kontrolle über den Stil (Klarform-Saga: breite, klare Formen), Wachstumsstufen per Parameter, Varianten per Zufallszahl, ohne Editor-Klicks wiederholbar, kein Nanite nötig, klein im Speicher | stilisiert statt fotorealistisch; Qualität hängt an der Ausarbeitung, braucht Rückmeldung per Bild |
| **Procedural Vegetation Editor (PVE)** in Unreal 5.8 | Teil der Engine | wächst Bäume „botanisch“, Wind, Nanite Foliage | experimentell (5.7-Dateien gehen in 5.8 nicht mehr), keine dokumentierte Skript-Schnittstelle (nur Editor-Oberfläche), Ausgabe auf Nanite Foliage ausgelegt (Export als normales Modell laut Forum teils fehlerhaft), Gras nicht dokumentiert, Wachstum nur im Editor (nicht im Spiel), eher realistisch; fertige Vorlagen von Drittanbietern kosten Geld |
| **EZ-Tree** (Three.js, Dan Greenheck) | MIT | 15 Baumvorlagen, Export als GLB mit LODs, als npm-Paket per Skript nutzbar | eher realistisch, Nacharbeit nötig, kein Gras |
| **tree-gen** (friggog), **Modular Tree** (Blender) | GPL-3.0 (gilt für das Werkzeug, nicht für erzeugte Modelle) | erprobte Verzweigungs-Algorithmen in Blender | eher realistisch, Einarbeitung |

Quellen: [Epic: Procedural Vegetation Editor](https://dev.epicgames.com/documentation/unreal-engine/procedural-vegetation-editor-in-unreal-engine), [Unreal Engine 5.8 Neuerungen](https://www.unrealengine.com/news/unreal-engine-5-8-is-now-available), [Forum: PVE exportiert kein Non-Nanite](https://forums.unrealengine.com/t/procedural-vegetation-editor-cannot-export-non-nanite-foliage/2718593), [Forum: statische vs. animierte PVE-Bäume](https://forums.unrealengine.com/t/please-export-pp2-texture-maps-for-the-procedural-vegetation-static-mesh/2662240), [Skyroid: PVE in der Praxis](https://www.skyroidstudios.com/insights/procedural-vegetation-editor-unreal-environment-teams), [EZ-Tree auf GitHub](https://github.com/dgreenheck/ez-tree), [tree-gen auf GitHub](https://github.com/friggog/tree-gen), [Modular Tree](https://extensions.blender.org/add-ons/modular-tree/).

#### Empfehlung
Eigener Generator in Blender für alle Slice-Pflanzen, Felsen und Grasbüschel, angelehnt an bekannte Verfahren (rekursive Verzweigung nach Weber und Penn, wie bei tree-gen). Dazu im Spiel: sichtbares Nachwachsen (Stumpf → Setzling → junger Baum → ausgewachsen) und Gras als Büschel, die auf dem Boden verteilt werden (eigener Code, kein Plugin nötig). PVE später für einzelne Bäume ausprobieren, wenn es nicht mehr experimentell ist.

<a id="d9-5"></a>

### 9.5 Gewählt (zurückgenommen): Meshy Pro (Nutzer, 2026-11-13: „Gleich Meshy Pro“)

<a id="d9-6"></a>

### 9.6 Pipeline (`tools/ki3d.ps1`)
1. **Abrufen** (`tools/ki3d/meshy_generate.py`): liest `tools/ki3d/flora.json` (Beschreibungstexte, Zielgröße, Dreiecksbudget, Zustände). Pro Modell ein Vorschau-Auftrag (Form, 20 Credits) und pro Zustand ein Textur-Auftrag (10 Credits) aus derselben Form, so bleiben z. B. „voll“ und „abgeerntet“ beim Beerenstrauch exakt gleich geformt. Lädt GLB und Vorschaubild sofort herunter (Meshy löscht API-Aufträge nach 3 Tagen) nach `D:\gamer\ki3d_rohdaten\meshy\` (außerhalb des Repositorys, weil `.glb` nicht über LFS läuft) und protokolliert jeden Auftrag mit Text und Credits in `meshy_log.json` (Grundlage für `design/asset_log.md` und die Steam-Angabe). Schlüssel nur aus der Benutzervariable `MESHY_API_KEY`, wird nie ausgegeben. `-DryRun` zeigt Plan und Kosten, `-MaxCredits` setzt eine Obergrenze, fertige Modelle werden übersprungen.
2. **Nachbearbeiten** (`tools/blender/process_ki3d.py`): Teile zusammenfügen, auf Zielgröße skalieren, Drehpunkt an den Fuß (Mitte der untersten 4 %, bei Bäumen der Stamm), Dreiecke begrenzen, Grundfarb-Textur verkleinern, ein Materialplatz `M_Nature_<Name>`; FBX und PNG nach `Assets/Nature/`, eigenes Manifest `Assets/manifest_ki3d.json` (das Baukasten-Skript überschreibt `Assets/manifest.json`). Normal- und ORM-Karte sind für alle Pflanzen gleich und flach (matt), passend zur stilisierten Art Direction und sparsam im Speicher.
3. **Import** (`tools/import_assets.ps1`): liest beide Manifeste, legt je Modell eine Materialinstanz von `M_Gamer_Base` an, Kollision = Modell.
4. **Spiel:** `resource_nodes.json` bekommt `meshes` (voll, abgeerntet). Ein Vorkommen mit Modellen zeigt diese statt der Platzhalter, mit zufälliger Drehung und ±15 % Größe aus seiner Position (auf allen Rechnern gleich). Ohne Modell für „abgeerntet“ verschwindet es beim Ernten.

Plan für den Slice: 16 Formen, 19 Texturen, geschätzt 510 Credits.

<a id="d9-7"></a>

### 9.7 Was für den Slice gebraucht wird (`design/ASSET_LIST_GESAMT.md`, Abschnitt 8)
Kiefer und Birke (stehend, gefällt, Stumpf), Felsen klein/mittel/groß (voll, abgebaut), Beerenstrauch (voll/leer), Wildapfelbaum (voll/abgeerntet), Pilzstelle, Haselstrauch, lose Steine, herabgefallene Äste, Faserpflanze (Brennnessel). Etwa 10 Vorkommen mit zusammen rund 20 Modellen. Budget laut `design/ASSET_PLAN.md`: Baum bis 15.000 Dreiecke.

<a id="d9-8"></a>

### 9.8 Quellen
- Meshy: [Pricing & Credits](https://docs.meshy.ai/en/webapp/pricing), [Ownership](https://help.meshy.ai/en/articles/10137554-what-is-the-ownership-of-the-generated-models), [Commercial use](https://help.meshy.ai/en/articles/16102098-can-i-use-meshy-assets-commercially), [Preise](https://meshy.ai/pricing)
- Tripo: [Image to 3D (Lizenzhinweis)](https://www.tripo3d.ai/features/image-to-3d-model), [Blog: kommerzielle Nutzung](https://www.tripo3d.ai/blog/commercial-use-ai-3d-models), [Test MakerStack](https://makerstack.co/reviews/tripo-ai-review/)
- Hyper3D Rodin: [Test Affinco](https://affinco.com/hyper3d-review/), [GitHub-Issue mit Zitat der Bedingungen](https://github.com/DarkLordGeo/age-of-wars/issues/2)
- Stability AI: [Stable Fast 3D](https://stability.ai/news/introducing-stable-fast-3d), [SPAR3D](https://stability.ai/news/stable-point-aware-3d), [Lizenzen](https://stability.ai/license)
- Hunyuan3D: [Lizenz 2.0](https://github.com/Tencent-Hunyuan/Hunyuan3D-2/blob/main/LICENSE), [Lizenz 2.1](https://github.com/Tencent-Hunyuan/Hunyuan3D-2.1/blob/main/LICENSE)
- TRELLIS.2: [Modellkarte Hugging Face](https://huggingface.co/microsoft/TRELLIS.2-4B/blob/main/README.md)


[↑ zum Inhaltsverzeichnis](#inhaltsverzeichnis)


---

<a id="teil-3"></a>

# Teil III – Welt, Umgebung und Spielsysteme

<a id="d10"></a>

## 10. Umgebung erstellen: Methoden aus den Tutorials und unsere Umsetzung

> Quelle: `docs/08_umgebung_methoden.md`

Stand 2026-11-13. Sammlung der Methoden aus den Videos, die der Nutzer geschickt hat, und wie wir sie **ohne Kosten** umsetzen.
Bezahlte Werkzeuge (QuickScatter, Ultra Dynamic Sky, Quixel-/Fab-Pakete) nutzen wir nicht. Wir übernehmen die Methode und bauen sie mit Engine-Bordmitteln (PCG, Höhennebel, Landscape) und CC0-Dateien nach.

<a id="d10-1"></a>

### 10.1 Quellen (Videos des Nutzers)

| Nr. | Thema | Kernidee |
|---|---|---|
| V1 | Straßen mit Landscape-Splines und PCG | Spline formt das Gelände und trägt Weg-Meshes, PCG spart den Weg aus |
| V2 | Natürliches Gras mit PCG | Saatpunkte, Punktraster je Saatpunkt, Größe nach Abstand zum Saatpunkt, ohne Kollision |
| V3 | Fotorealer Wald in 7 Minuten (PCG) | Schichten: Bodenplatten, Bäume, Sträucher, Kleinzeug; je Schicht Rauschen, Filter, Zufallsdrehung |
| V4 | Bodennebel (mit Ultra Dynamic Sky) | Volumetrischer Nebel, Bodennebel nur in bestimmten Zonen, dezent einstellen |
| V5 | Offene-Welt-Wald (mit QuickScatter) | „Beim Laden“ gegen „zur Laufzeit“, räumliches Rauschen für Klumpen, Biom-Karte, Spline-Masken |
| Bild | Referenz: lichter Kiefernwald auf Sand | Zutaten für das „Rezept“ (siehe unten) |

<a id="d10-2"></a>

### 10.2 Methoden

| Methode | Woher | Unsere Umsetzung | Stand |
|---|---|---|---|
| **Schichten** aufbauen: erst Boden, dann große, dann mittlere, dann kleine Objekte. Je Schicht: Punkte auf der Oberfläche → Dichte-Rauschen → Dichte-Filter → Zufallsdrehung 0–360°, Größe 0,7–1,2, leicht in den Boden versetzt → Mesh-Verteiler | V3 | PCG-Graph mit einem Strang je Schicht (`tools/unreal/build_world.py`): Gras (Wiesen), Farne (Bodendecker auf Moos), Sandstein-Felswände am Rand. Kiesel/Zweige abgeschaltet, Stümpfe/Totholz werden Vorkommen, CC0-Sträucher nicht verteilt (wirken wie Zweige) | umgesetzt (Stand 2026-11-14) |
| **Grasbüschel** statt Einzelhalme: wenige Saatpunkte, je Saatpunkt ein Raster, Größe nimmt mit dem Abstand zum Saatpunkt ab, Projektion auf den Boden, ohne Kollision | V2 | eigene Büschel aus CC0-Atlas `Foliage006` (`tools/blender/build_grass.py`), PCG-Strang „Gras“ | in Arbeit |
| **Platzbedarf großer Objekte** (Bounds Modifier), damit sich Bäume nicht überlappen | V3 | Bäume, Sträucher, Felsen sind bei uns **Vorkommen** (ernten, nachwachsen, Server-Logik). PCG setzt sie nicht; nur Deko | entschieden (vorläufig) |
| **„Beim Laden“ gegen „zur Laufzeit“**: große Objekte mit Kollision immer vorhanden; dichte Bodendecker nur in Kacheln um den Spieler erzeugen (Kachel 1600 oder 3200 cm) | V5 | große Objekte = Vorkommen vom Server (repliziert, mit Kollision). Bodendecker = PCG „Generate at Runtime“ + partitioniert, nur auf Spieler-Rechnern; der Dedicated Server braucht kein Gras | geplant, Mehrspieler-Verhalten wird geprüft |
| **Räumliches Rauschen** für natürliche Klumpen: Rauschgröße 500–3000 cm, Stärke regelt Lücken; jede Gruppe eigener Seed, damit nicht alles an derselben Stelle klumpt | V5 | PCG Spatial Noise (Perlin2D) mit festen Feldern; dieselben Felder rechnet `M_Gamer_Ground` nach (Farne auf Moosflecken, Gras auf den Wiesen, Boden dort erdig-grünlich); um den Start (0,0) dünnt eine Distanz-Rampe die Farne aus (bis 9 m keine, bis 30 m wenige) | umgesetzt |
| **Biom-Karte**: Farbbild von oben, jede Farbe ein Biom (Wald, Busch, Strand …) | V5 | PCG-Textur-Abfrage auf eine Biom-Textur; sinnvoll erst mit echtem Gelände/Weltgenerator | später |
| **Wege und Flüsse ausschneiden** per Spline (Breite einstellbar) | V1, V5 | PCG „Difference“ mit Spline-Daten; Landscape-Splines für Wege. Wasser-Plugin erst nach Prüfung | später |
| **Kontaktschatten** statt echter Schatten für kleines Bodenzeug (billig, folgt trotzdem der Sonne) | V5 | im Mesh-Verteiler Schatten aus, am Sonnenlicht „Contact Shadow Length“ setzen | geplant |
| **Einblenden mit der Entfernung** (Pflanzen wachsen/blenden weich ein statt aufzuploppen) | V5 | Cull-Distanz im Verteiler, später Dither-Überblendung in `M_Gamer_Foliage` | geplant |
| **Bodenmaterial** passend zu den Objekten, mit Farbvariation; mehrere Bodenschichten (Sand, Nadelstreu, Moos, Weg) | V3, V5 | `M_Gamer_Ground` (`tools/unreal/build_ground_material.py`) auf dem flachen Boden: 5 CC0-Schichten (Nadelwaldboden, Sand mit Nadeln, heller Sand, Moos, Erde), weltbezogen, zwei Maßstäbe, Höhenüberblendung, großflächige Farbvariation, Jahreszeit über `MPC_GamerWorld.SeasonPhase`. Später auf die Landscape übertragen | Boden umgesetzt, Landscape offen |
| **Volumetrischer Nebel**, dezent; **Bodennebel** nur in Senken/über Wasser | V4 | ohne Ultra Dynamic Sky: Exponential Height Fog mit „Volumetric Fog“, dazu „Local Fog Volume“ (seit UE 5.3 eingebaut) für örtlichen Bodennebel | geplant |
| **Gelände** (Landscape) statt flachem Boden; viele Methoden setzen es voraus | V1, V3, V5 | nächster großer Schritt nach dem Gras; Erzeugung per Skript wird geprüft | offen |
| **Mesh-Gruppen** zentral pflegen (Liste je Biom mit Dichte, Größe, Rauschen) | V5 | Schichten-Tabelle an einer Stelle im Weltskript, später als Datei in `data/` | geplant |

<a id="d10-3"></a>

### 10.3 Referenzbild als „Rezept“ (V5-Methode)

Hauptzutaten, die das Bild ausmachen:
1. **Waldkiefer** (Pinus sylvestris): hohe, gerade Stämme, Krone erst im oberen Drittel, **oben orange „Spiegelrinde“, unten graubraune Borke**. Passt zum Wikinger-Norden.
   Folge für uns: Kiefer in `build_trees.py` höher und schlanker, Rinde nach Höhe überblenden (unten Borke, oben orange). Noch nicht umgesetzt.
2. **Heller Sandboden**, der zwischen den Pflanzen durchscheint → Boden-Textur „Sand“.
3. **Niedrige grüne Sträucher** in Gruppen → Bodendecker-Schicht mit Klumpen-Rauschen.
4. **Totholz**: Stümpfe, liegende Äste → Deko-Schicht (kleiner als die erntbaren Äste, damit man sie nicht verwechselt).
5. **Fels** (geschichteter Sandstein) → Felsen-Vorkommen, evtl. weiterer Scan.

<a id="d10-4"></a>

### 10.4 Was wir nicht übernehmen

Geprüft am 2026-11-13 (Seiten und Fab-API gelesen, nichts heruntergeladen):

| Produkt aus den Videos | Preis | Lizenz / Konto | Entscheidung |
|---|---|---|---|
| QuickScatter (AzielArts) | 59 $ (regulär 79 $) | eigene Lizenz, Kauf per E-Mail | nein; nur ein GPU-PCG-Blueprint, PCG kann das selbst |
| Ultra Dynamic Sky | 49,99 $ / 199,99 $ | Fab, Epic-Konto | nein; Sky Atmosphere, Volumetric Clouds, Height Fog, Local Fog Volume |
| AzielArts Landscape Material | laut Video gratis, auf der Seite kein Preis | Lizenz unklar, E-Mail-Anmeldung | nein, solange die Lizenz für ein kommerzielles Spiel nicht geklärt ist; eigenes Landscape-Material bauen |
| Nanite Bushes (Fab) | 27,13 € | Fab, Epic-Konto | nein; Poly-Haven-Sträucher und eigene Büsche |
| „Pine Forest – PCG Biome“ (Fab, im Video „Supplemental Assets“) | 72,38 € | Fab, Epic-Konto | nein |
| Modular Lighthouse (Fab) | 18,08 € | Fab, Epic-Konto | nein, passt nicht zum Spiel |

Kostenlose Ersatzquellen: Poly Haven und ambientCG (CC0). Gratis auf Fab (Fab Standard License, nur mit Epic-Login des Nutzers): Megascans-Pilze, Zapfen, Kiefernäste; „Megaplants Baltic Pine“ braucht experimentelle Plugins (Procedural Vegetation Editor, Nanite Foliage) und ist deshalb vorerst nicht vorgesehen.
Vorschlagsliste mit Größen: siehe `design/asset_log.md` (nach dem Download) bzw. die Freigabe-Anfrage im Chat vom 2026-11-13.
- PCG-Bäume als reine Deko im Spielbereich: würden aussehen wie fällbare Bäume, sind es aber nicht. Allenfalls später als ferne Kulisse außerhalb der Spielfläche.


[↑ zum Inhaltsverzeichnis](#inhaltsverzeichnis)

<a id="d11"></a>

## 11. Vegetations-Material und Jahreszeiten

> Quelle: `docs/09_vegetation_material.md`

Stand 2026-11-14. Gilt für das gemeinsame Blatt-/Pflanzenmaterial `/Game/Gamer/Materials/M_Gamer_Foliage` (Laub, Nadeln, Gras, Kräuter).
Gebaut wird es per Python in `tools/unreal/import_assets.py` (Aufruf: `tools/import_assets.ps1`). Änderungen im Editor gehen beim nächsten Import verloren.

<a id="d11-1"></a>

### 11.1 Ursache der „verbrannten“ grauen Grasbüschel (2026-11-13)

Nicht die Normalen oder der Glanz: Das Client-Log meldete
`Material M_Nature_Grass missing usage flag InstancedStaticMeshes! Default Material will be used in game.`
PCG setzt das Gras als Instanzen (ISM). Ohne das Nutzungs-Flag „Used with Instanced Static Meshes“ zeigt das Spiel (`-game`, ohne Editor)
das graue Standard-Gittermaterial. Das betraf auch die PCG-Kiesel und -Zweige (`M_Gamer_Base`). Beide Eltern-Materialien setzen das Flag jetzt.

<a id="d11-2"></a>

### 11.2 Shading

| Einstellung | Wert | Grund |
|---|---|---|
| Shading Model | Two Sided Foliage | Licht scheint durch Halme und Blätter |
| Blend Mode | Masked, zweiseitig | Alpha der BaseColor-Textur |
| Subsurface Color | Jahreszeiten-Farbe × 0,4 | gedämpftes Durchlicht |
| Specular | 0,25 | weniger weißlicher Himmelsglanz |
| Roughness | 0,6 + 0,35 × ORM.G (0,6–0,95) | nie glänzend |
| Normal | Normal-Map × TwoSidedSign | Rückseiten behalten die Normale der Vorderseite. Die Engine dreht sie sonst um, und weil Gras und Baumkronen nach oben bzw. außen gebogene Normalen haben, würden Rückseiten dunkel. |
| Ambient Occlusion | ORM.R | wie bisher |

<a id="d11-3"></a>

### 11.3 Jahreszeit: `MPC_GamerWorld.SeasonPhase`

Material Parameter Collection `/Game/Gamer/Materials/MPC_GamerWorld`, Skalar **`SeasonPhase`** (Standard 1,3 = Sommer). Das Spiel (C++, `AGamerGameState`) setzt den Wert zur Laufzeit.
Name und Pfad nicht ändern. Der Import legt den Parameter nur an, wenn er fehlt (Materialien verweisen über seine ID darauf), und setzt den Standardwert jedes Mal auf 1,3.

| Phase | Zeitpunkt (Nordeuropa) |
|---|---|
| 0 | Frühlingsanfang (~20. März) |
| 1 | Sommeranfang (~21. Juni) |
| 2 | Herbstanfang (~22. September) |
| 3 | Winteranfang (~21. Dezember) |
| 4 | = 0, zyklisch |

Testen im Spiel: Testschritt `season:<phase>`, z. B. `-GamerTestScript=season:2.5;wait:3;shot`.

<a id="d11-4"></a>

### 11.4 Parameter je Material-Instanz

| Parameter | Standard | Bedeutung |
|---|---|---|
| `Evergreen` | 0 | 1 = immergrün (Nadeln): im Winter nur dunkler, olivgrüner |
| `LeafDrop` | 0 | 1 = Laub fällt ab: Herbstfarben, im Spätherbst ausblenden, im Frühling austreiben. Hat Vorrang vor `Evergreen`. |
| beide 0 | | Gras, Kräuter: Farbwechsel ohne Ausblenden |

Werte: 0 oder 1 (Schwelle 0,5). `import_assets.py` setzt sie nach Namensregeln (Tabelle `SEASON_RULES`, erster Treffer gilt, Groß-/Kleinschreibung egal).
Die Regeln stehen bewusst nicht im Manifest, weil die Blender-Skripte ihre Manifest-Einträge neu schreiben.

| Name enthält | Verhalten | Beispiele |
|---|---|---|
| `Fern` | deciduous (`LeafDrop` 1) | M_Nature_Fern02 (Farn fern_02) |
| `Shrub` | grass | M_Nature_LeavesShrub02, …Shrub04 (Sträucher; Regel steht vor `Leaves`) |
| `Needles` | evergreen (`Evergreen` 1) | M_Nature_NeedlesPine |
| `Leaves` | deciduous (`LeafDrop` 1) | M_Nature_LeavesBirch, …Apple, …Hazel, …Raspberry |
| sonst (maskiert) | grass | M_Nature_Grass, M_Nature_Nettle |

Nur maskierte Materialien (Eltern `M_Gamer_Foliage`) haben Jahreszeiten. Rinde, Früchte (`M_Nature_Fruit`), Stümpfe, Felsen, Totholz bleiben unverändert.

<a id="d11-5"></a>

### 11.5 Farbgebung

Die Textur wird nicht ersetzt, sondern umgefärbt: Die Helligkeit jedes Texels bleibt (Details bleiben), der Farbton wird mit dem Anteil *A* zur Jahreszeitenfarbe gezogen und mit dem Faktor *B* aufgehellt oder abgedunkelt.
Zwischen den Stützstellen wird weich interpoliert (smoothstep), Phase 4 schließt nahtlos an 0 an.

#### Farben (sRGB, im Code linear)

| Name | sRGB | Verwendung |
|---|---|---|
| BUD | 150 185 60 | Laubaustrieb, hell gelbgrün |
| FRESH | 140 185 45 | frisches Frühlingsgrün |
| SUMMER | 70 130 35 | sattes Sommergrün |
| OLIVE | 125 135 55 | Spätsommer, leicht trocken |
| DRY | 150 140 70 | trockene Flecken |
| LUSH | 52 112 40 | feuchte, sattgrüne Flecken |
| AUTGRASS | 160 130 60 | Herbstgras, gelbbraun |
| STRAW | 165 145 105 | Wintergras, strohig |
| YELLOW / ORANGE / RUST | 215 170 40 / 205 115 30 / 150 60 30 | Herbstlaub (je Objekt gemischt, etwa 45 % gelb, 30 % orange, 25 % rotbraun) |
| DEADLEAF | 140 90 45 | Laub kurz vor dem Fallen |
| NEEDLEWINTER | 75 82 42 | Nadeln im Winter (oliv) |

#### Stützstellen (Phase: Farbe, Anteil *A*, Helligkeit *B*)

| Typ | Frühling | Sommer | Herbst | Winter |
|---|---|---|---|---|
| Gras/Kräuter | 0,0 STRAW .78/.95 → 0,4 FRESH .60/1.18 → 0,85 FRESH .35/1.08 | 1,15 SUMMER .35/.92 → 1,95 OLIVE .45/.97 | 2,5 AUTGRASS .72/1.03 | 3,1 STRAW .82/1.0 → 3,9 STRAW .78/.95 |
| Laub (`LeafDrop`) | kahl bis 0,30, Austrieb 0,30–0,62; 0,45 BUD .65/1.18 → 0,85 FRESH .35/1.08 | 1,15 SUMMER .30/.92 → 1,75 SUMMER .28/.90 → 2,0 OLIVE .30/.92 | 2,35 Herbstfarbe .90/1.0–1.35; Laubfall 2,36–2,86, dabei → DEADLEAF .92/.85 | kahl ab ~2,86 |
| Nadeln (`Evergreen`) | 0,5 SUMMER .10/.95 → 0,85 FRESH .22/1.05 (Maitriebe) | 1,3 SUMMER .12/.97 | 2,7 OLIVE .25/.88 | 3,1 NEEDLEWINTER .50/.75 |

Laubfall und Austrieb laufen über die Opacity-Mask: ein 3D-Rauschen (35 cm und 13 cm) entscheidet, welche Blattbereiche zuerst verschwinden; die Krone lichtet sich fleckig statt auf einmal.
Blattbereiche, die früher fallen, verfärben sich auch früher.

<a id="d11-6"></a>

### 11.6 Variation gegen Eintönigkeit

| Quelle | Wirkung |
|---|---|
| Zufall je Objekt: `PerInstanceRandom` (PCG-Instanzen) + Hash der Objektposition (Einzel-Actors wie Bäume, dort ist PerInstanceRandom 0) | Helligkeit ±11 %, Farbton wärmer/kühler (±6–10 % in Rot/Blau), Zeitversatz der Jahreszeit (Gras ±0,05, Nadeln ±0,06, Laub ±0,09 Phasen), Wahl der Herbstfarbe |
| Welt-Rauschen (Wertrauschen 28 m + 11 m) | trockene und satte Flecken von etwa 15–40 m; trockene Flecken sind gelblicher und schneller im Herbst, feuchte im Sommer satter und dunkler |

<a id="d11-7"></a>

### 11.7 Bekannte Grenzen

- Früchte (`M_Nature_Fruit`) bleiben im Winter sichtbar, auch wenn der Strauch kahl ist. Ob Beeren/Nüsse im Winter erntbar sein sollen, ist eine Spiel-Entscheidung.
- Kein Schnee. Winter = strohiges Gras, kahles Laub, dunklere Nadeln.
- Kosten: ein Custom-Node (HLSL) mit Hash-Rauschen; das 3D-Rauschen läuft nur für `LeafDrop`-Materialien (Verzweigung auf dem Parameter).


[↑ zum Inhaltsverzeichnis](#inhaltsverzeichnis)

<a id="d12"></a>

## 12. Nahrung, Obst, Saatgut und Feldbau – Anforderungen

> Quelle: `docs/10_nahrung_feldbau.md`

Stand 2026-11-14. Sammlung der Nutzer-Wünsche (Chat 2026-11-14) und der Ergänzungen von ChatGPT (Komposter, Regenfänger). Umsetzung offen; Entscheidungen stehen in `docs/01_entscheidungen.md`.

<a id="d12-1"></a>

### 12.1 Wünsche des Nutzers (wörtlich zusammengefasst)

1. **Obst nur an Bäumen: Äpfel und Birnen.** Es geht mit der Jahreszeit (Blüte → wächst → reif), wird es nicht geerntet, **fällt es herunter**, bekommt je nach **Fallhöhe Schaden** (Druckstellen) und **verdirbt**, wenn man es nicht aufsammelt.
2. **Feldfrüchte:** Zwiebeln, Möhren, Kartoffeln, Flachs.
3. **Saatgut:** Aus den Früchten gibt es Samen. Samen werden auf Feldern **ausgesät**, das Feld wird **gedüngt**; die Ernte wird zu **Lebensmitteln** oder zu **Dünger**.
4. **Wildpflanzen** aus dem Referenzbild (`design/` folgt): Walderdbeere, Blaubeere, Himbeere (gibt es schon), Brombeere, Hagebutte, Schlehe, Holunder, Zuckermelone.
5. Aus der Gameplay-Liste vom selben Tag: **Verderb** mit Hinweis bei 30 % Restzeit und Anfänger-Tipp („verderbliche Gegenstände trocken und geschützt lagern“); **Komposter** (verdorbene Nahrung → Kompost → Dünger); **primitiver Regenfänger** (1 Holzstamm, 1 großer Stein, 3 kleine Steine, 5 Äste) für Trink- und Gießwasser.

<a id="d12-2"></a>

### 12.2 Kreislauf

Sammeln/Ernten → Essen oder Lagern → (verdirbt) → Komposter → Dünger → Feld (Saatgut + Wasser + Dünger) → Ernte → Saatgut zurück, Rest Essen/Dünger.

<a id="d12-3"></a>

### 12.3 Vorschlag zur Umsetzung (Claude, noch nicht bestätigt)

- **Obstbaum-Zyklus** an die Jahreszeit (`SeasonPhase`) gekoppelt: Frühling Blüte, Sommer grüne Früchte, Spätsommer/Herbst reif (erntbar), danach fallen ungeerntete Früchte nach und nach als einzelne Gegenstände (Actor am Boden, vom Server gesetzt). Schaden = Fallhöhe: unter 2 m keiner, darüber „angeschlagen“ (verdirbt doppelt so schnell). Winter: keine Früchte (behebt „Äpfel hängen im Winter am kahlen Baum“).
- **Verderb** als Eigenschaft der Gegenstände in `items.json` (Haltbarkeit in Spieltagen; schneller am Boden/im Regen, langsamer im Lager unter Dach). Hinweis bei 30 % Restzeit, einmaliger Anfänger-Tipp.
- **Saatgut** als eigene Gegenstände (`seed_apple`, `seed_pear`, `seed_onion`, `seed_carrot`, `seed_potato` = Saatkartoffel, `seed_flax`), entsteht beim Verarbeiten/Essen bzw. als Teil der Ernte.
- **Feld** als Baustück (Beet), Zustände: leer → gesät → wächst (Stufen) → reif; Dünger beschleunigt bzw. erhöht den Ertrag, Wasser nötig (Regen oder Gießen aus dem Regenfänger). Wachstum an die Jahreszeit gebunden (Winter kein Wachstum).
- **Flachs** liefert Fasern (Kleidung, Seile) und Samen (Leinsamen, essbar).

<a id="d12-4"></a>

### 12.4 Offene Fragen an den Nutzer

- Historische Passung: Kartoffeln (erst ab 16. Jh. in Europa) und Zuckermelone (nicht im Norden der Wikingerzeit) – trotzdem rein? Vorläufig: Kartoffeln rein (ausdrücklicher Wunsch), Melone vorerst nicht.
- ~~Zeitpunkt~~: beantwortet 2026-11-14, alles jetzt.
- ~~Bauer~~: beantwortet 2026-11-14, ja, über den Hof (Arbeitsplätze, Ausbau).
- ~~Himbeere~~: bleibt ganzjährig (Nutzer 2026-11-14).
- ~~Bäume pflanzen~~: ja, vom Setzling (Nutzer 2026-11-14), gebaut.
- ~~Kartoffeln~~: bleiben (Nutzer 2026-11-14).

<a id="d12-5"></a>

### 12.5 Umsetzung (Stand 2026-11-14 abends, Session „Obst, Saatgut und Feldbau“)

Alle Zahlen unten sind **Vorschläge von Claude** (⚑), soweit nicht anders vermerkt. Sie stehen in den Daten und lassen sich dort ändern. Ein Spieltag dauert 30 Minuten, eine Jahreszeit 60 Minuten, also zwei Spieltage. Jahreszeit-Phasen: 0 = Frühlingsanfang, 1 = Sommer, 2 = Herbst, 3 = Winter.

#### Verderb (`items.json`: `spoil_days`, optional `spoils_into`)
- Die Haltbarkeit in Spieltagen gilt „getragen“, also im Inventar oder in der Hand eines Bewohners. Das Lager (unter Dach) verlängert sie, aber nie unbegrenzt: Faktor 0,25, also viermal so lang ⚑. Am Boden verdirbt es doppelt so schnell ⚑.
- Höhere Stufe hält länger (bestätigt). Formel: × (1 + 0,5 × (Stufe − 1)) ⚑.
- Werte: Obst, Beeren, Pilze und Gemüse 2,5 Tage (bestätigt am 2026-11-12). Fallobst mit Druckstellen 1 Tag ⚑. Gekochtes (Mahlzeit, Eintopf) 8 Tage ⚑; bestätigt ist nur „deutlich deutlich langsamer“. Haselnüsse und Leinsamen verderben nicht ⚑.
- Verdorbenes wird zu `item_spoiled_food`; am Boden verrottet es nach einem halben Spieltag ganz.
- Stapel mit unterschiedlicher Frische werden zusammengelegt, ihre Frische als Mittelwert.
- Hinweis bei 30 % („Bald verdorben: …“), einmaliger Anfänger-Tipp, Meldung beim Verderben; im Lager als Meldung an alle. Frische pro Platz: `FGamerInventorySlot::Freshness`, im Lager `FGamerItemAmount::Freshness`. Einen Haltbarkeitsbalken am Inventarplatz gibt es noch nicht (Anzeige liegt bei der Gameplay-Session).

#### Obst und Wildpflanzen (`resource_nodes.json`: Block `season`)
| Vorkommen | Blüte | unreif | reif | fällt ab | kahl | Früchte/Jahr |
|---|---|---|---|---|---|---|
| Wildapfelbaum | 0,3 | 0,6 | 1,8 | 2,2 | 2,9 | 8–14, je Pflücken 5 |
| Wildbirnbaum (neu) | 0,25 | 0,55 | 1,6 | 2,0 | 2,7 | 6–12, je Pflücken 5 |
| Walderdbeere | 0,25 | 0,5 | 0,8 | – | 1,5 | 4–8 |
| Blaubeere | 0,35 | 0,65 | 1,2 | – | 2,0 | 8–14 |
| Brombeere | 0,8 | 1,1 | 1,6 | – | 2,6 | 8–12 |
| Hundsrose (Hagebutten) | 0,9 | 1,2 | 2,2 | – | 3,5 | 6–10 |
| Schlehe | 0,2 | 0,5 | 2,4 | – | 3,5 | 8–12 |
| Holunder | 0,8 | 1,1 | 1,8 | – | 2,5 | 10–16 |

- Alle Phasen ⚑. Hagebutte und Schlehe reichen bis in den frühen Winter (botanisch: Ernte nach dem ersten Frost). Obstbäume tragen im Winter nichts.
- Was ungepflückt hängen bleibt, fällt zwischen „fällt ab“ und „kahl“ nach und nach als einzelner Gegenstand herunter. Fallhöhe 1,2–4,2 m beim Apfel, 1,5–5,5 m bei der Birne ⚑. Über 2 m gibt es Druckstellen ⚑, das Obst wird dann zu „Fallobst (…, Druckstellen)“. Aufheben mit E.
- Die Himbeere („Beeren“) bleibt ganzjährig wie bisher. Sonst gäbe es in einer neuen Welt (Start im späten Frühling) erst nach rund 40 Minuten die Beeren für die erste Mahlzeit. ⚑, bitte entscheiden.
- Holunderbeeren sind roh giftig. Die Bewohner essen sie nicht roh, am Lagerfeuer werden 4 davon zu einer Mahlzeit.

#### Saatgut und Felder
- Erstes Saatgut liefern Wildvorkommen ⚑: Wilde Möhre (Möhrensamen), Wildzwiebel (Zwiebelsamen), Wilder Flachs (Leinsamen), verwilderte Kartoffelstaude (Saatkartoffeln, historisch unpassend, siehe offene Frage). Danach kommt es aus der Ernte.
- Ertrag je Einheit (4,5 m² ⚑):

  | Saat | Wachstum | Ertrag |
  |---|---|---|
  | Zwiebelsamen | 2 Tage | 3–5 Zwiebeln, 1–2 Samen |
  | Möhrensamen | 2 Tage | 3–5 Möhren, 1–2 Samen |
  | Saatkartoffel | 2,5 Tage | 4–6 Kartoffeln |
  | Leinsamen (Flachs) | 2 Tage | 4–6 Pflanzenfasern, 2–3 Leinsamen |

  Dünger bringt +50 % Ertrag ⚑. Alles ⚑.
- **Feld aufziehen** (Nutzer 2026-11-14): Feld wählen, erster Klick setzt die Ecke, zweiter Klick die Gegenecke. Je Seite 2 m bis 12 m ⚑. Das Feld liegt auf der tieferen Ecke; die Pflanzen werden einzeln auf den Boden gesetzt.
- Kosten je Einheit: 2 Äste und 1 Stein ⚑. Pro Einheit braucht das Feld 1 Saatgut zum Säen, 1 Wasser für eine volle Füllung und 1 Dünger.
- Wasser reicht für einen Spieltag ⚑. Ohne Wasser und im Winter (Phase 3–4) wächst nichts, das Feld stirbt aber nicht (Regel 6).
- E am Feld macht jeweils das Nächste: ernten → säen → gießen → düngen → Zustand anzeigen.

#### Obstbäume selbst aufziehen (Nutzer 2026-11-14: „Vom Setzling aufziehen“)
- Kerne: 1 Apfel oder 1 Fallobst ergibt 3 Kerne (Handrezept), bei der Birne genauso ⚑.
- Die Kerne sät man im Feld. Nach 2 Spieltagen gibt jede Einheit einen Setzling ⚑.
- Pflanzen über das Baumenü: „Apfelbaum pflanzen“ oder „Birnbaum pflanzen“ kostet 1 Setzling, ohne Werkzeug. Der Baum wächst in 4 Spieltagen ⚑ vom Setzling über den Jungbaum zum tragenden Baum und folgt danach dem Jahreszyklus wie die wilden Bäume.
- Daten: `buildables.json` `farm.kind = "tree"` mit `node` und `grow_days`; die Wachstumsmodelle stehen in `resource_nodes.json` unter `meshes.growing`.
- Grenze: Im Feld stehen die Setzlinge als Modell im 50-cm-Raster und wirken dichter als die tatsächliche Zahl (1 je Einheit).

#### Komposter, Regenfänger
- Der Komposter nimmt Verdorbenes und Fallobst mit Druckstellen, bis zu 12 Stück. Aus je 3 entsteht 1 Kompost (Dünger), einer dauert 0,25 Spieltage ⚑.
- Regenfänger: 1 Baumstamm, 1 großer Stein, 3 Steine, 5 Äste (nach dem Vorschlag von ChatGPT). Neu ist der Gegenstand „großer Stein“: er fällt beim großen Fels immer, beim mittleren manchmal ⚑. Der Regenfänger füllt 4 Wasser je Spieltag, höchstens 6 ⚑. Wetter gibt es noch nicht.
- Trinken: Ein Durst-Bedürfnis gibt es noch nicht. Wasser ist vorerst nur zum Gießen da.

#### Hof, Bauer, Ausbau (Nutzer 2026-11-14)
- **Hof** (`building_farmstead`): 4 Baumstämme, 16 Bretter, 8 Steine ⚑. Er stellt Arbeitsplätze für den Beruf **Bauer**. Ohne Hof keine Feldarbeit durch Bewohner.
- Der Bauer arbeitet im Umkreis von 30 m um den Hof ⚑ in dieser Reihenfolge: reife Felder ernten; trockene Felder gießen, leere säen und Felder düngen (alles aus dem Lager); Regenfänger und Komposter leeren; den Komposter mit Verdorbenem füllen.
- **Ausbaustufen für alle Arbeitsgebäude** (`buildables.json`: `workers`, `upgrades`). Arbeitsplätze ⚑:

  | Gebäude | Stufen |
  |---|---|
  | Holzfällerplatz | 2 → 3 → 4 |
  | Steinmetzplatz | 2 → 3 → 4 |
  | Hof | 2 → 4 → 6 |
  | Lagerfeuer (Koch) | 1 → 2 |

  Ausbauen: E mit dem Hammer in der Schnellleiste am fertigen Gebäude, die Kosten der Stufe werden sofort bezahlt ⚑. Beim Abreißen kommt auch das Ausbaumaterial zurück. Sind alle Plätze besetzt, lehnt die Dorfverwaltung den Beruf mit einer Meldung ab.

#### Grenzen, ehrlich
- Modelle stammen aus `tools/blender/build_trees.py`. Brombeere, Hundsrose, Walderdbeere und Holunder nutzen den Blattatlas der Himbeere mit; Schlehe und Blaubeere teilen sich einen neuen Atlas mit kleinen Blättern. Das spart Texturspeicher, die Blattformen sind aber nicht artgenau.
- Feldfrüchte und Fallobst sind einfache Formen in Volltonfarben ohne Blatt-Textur. Feld, Komposter, Regenfänger, Hof und die übrigen Stationen haben seit 2026-11-22 einfache Modelle aus `tools/blender/build_stations.py` (Icons über `tools/blender/icon_map.json`). Ein gezogenes Feld legt die 1-m-Kachel `SM_Station_FieldTile` als Raster aus (`AGamerFarmBuilding::SoilTiles`); die gestreckte Kachel bleibt unsichtbar und gibt nur die Kollision.
- Das Feld ist eine flache Platte. Am Hang kann sie einsinken oder an einer Ecke frei schweben, denn sie wird nicht an das Gelände angepasst.


[↑ zum Inhaltsverzeichnis](#inhaltsverzeichnis)

<a id="d13"></a>

## 13. Taverne, Kochstellen und Gerichte (T1–T6)

> Quelle: `docs/12_taverne_gerichte.md`

Stand 2026-11-15. Wunsch des Nutzers (2026-11-14): „richte als Nächstes eine Taverne oder Ähnliches ein und baue es mit Kochgelegenheit ein und suche dir mit tierstufen passende Gerichte mit dazugehörige puffs“.
Bestätigt (2026-11-15): Gerichteliste übernehmen, Gerste, Hafer und Honig jetzt einbauen, Kochstellen als eigene Bauteile mit Taverne-Stufe.
Alle Zahlen sind Vorschläge von Claude (⚑) und stehen in den Daten (`items.json`, `recipes.json`, `buildables.json`, `jobs.json`, `needs.json`).

<a id="d13-1"></a>

### 13.1 Regeln für Essens-Buffs (Architekturregel 5)

- Essen gibt nur Boni, nie Strafen. Kein Essen = keine Buffs („Satt“-Anzeige leer), keine Sperre.
- Katalog: **Gesundheit** (max. Lebenspunkte), **Regeneration** (LP pro Sekunde), **Traglast** (kg), **Kälteresistenz**, **Giftresistenz** (%), **Schaden** (%). Der Datenprüfer lehnt andere Werte und negative Zahlen ab.
- Höchstens **3 Mahlzeiten gleichzeitig** ⚑ (`UGamerBuffComponent::MaxMeals`). Dasselbe Gericht noch einmal frischt nur die Dauer auf. Ist alles voll, ersetzt das neue Gericht das mit der kürzesten Restzeit.
- Server-autoritativ: Der Client bittet „essen“ (`ServerEat`), der Server prüft das Inventar, zieht 1 ab und setzt den Buff. Die Liste geht nur an den eigenen Spieler (Gerichts-ID + Endzeit in Serverzeit).
- Essen: **Linksklick mit einem Gericht in der Hand** ⚑ (Schnellleiste). Eigene Taste möglich, Absprache mit der Gameplay-Session.

#### Stärke und Dauer je Stufe ⚑

| Stufe | Gesundheit | Regeneration | Traglast | Resistenz | Schaden | Dauer (Echtzeit) | Haltbarkeit (getragen) |
|---|---|---|---|---|---|---|---|
| T1 | +10 | +0,5/s | +5 kg | 10 % | +3 % | 10 min | 8 Spieltage |
| T2 | +20 | +1,0/s | +10 kg | 15 % | +5 % | 15 min | 12 |
| T3 | +30 | +1,5/s | +15 kg | 20 % | +8 % | 20 min | 16 |
| T4 | +45 | +2,0/s | +20 kg | 25 % | +10 % | 25 min | 20 |
| T5 | +60 | +2,5/s | +25 kg | 30 % | +12 % | 30 min | 24 |
| T6 | +80 | +3,0/s | +30 kg | 40 % | +15 % | 40 min | 28 |

- Ein Buff: voller Wert. Zwei Buffs: je 70 %. Drei: je 70 %.
- Haltbarkeit über die bestehende Verderb-Formel: `spoil_days` 8 × (1 + 0,5 × (Stufe − 1)), im Lager viermal so lang. Auch Gekochtes verdirbt (bestätigt), nur langsamer.

<a id="d13-2"></a>

### 13.2 Gerichte

**Im Spiel (19):**

| Stufe | Gericht (ID) | Kochstelle | Zutaten | Ergibt | Buffs |
|---|---|---|---|---|---|
| T1 | Einfache Mahlzeit (`item_meal`, bestehend) | Lagerfeuer | 3 Beeren *oder* 4 Holunderbeeren | 1 | Gesundheit +10 |
| T1 | Pilzspieß | Lagerfeuer | 3 Pilze, 1 Ast | 1 | Schaden +3 % |
| T1 | Gebackene Wildäpfel mit Haselnüssen | Lagerfeuer | 2 Wildäpfel, 2 Haselnüsse | 1 | Traglast +5 |
| T1 | Holundersuppe | Lagerfeuer | 4 Holunderbeeren, 1 Wasser | 1 | Giftresistenz 10 % |
| T1 | Hagebuttensud (Getränk) | Lagerfeuer | 3 Hagebutten, 1 Wasser | 1 | Kälteresistenz 10 % |
| T2 | Gemüseeintopf (`item_stew`, bestehend, jetzt T2) | Herdstelle | 1 Zwiebel, 2 Möhren, 2 Kartoffeln | 2 | Gesundheit +14, Regeneration +0,7 |
| T2 | Zwiebel-Pilz-Pfanne | Herdstelle | 2 Zwiebeln, 3 Pilze | 1 | Schaden +5 % |
| T2 | Nuss-Leinsamen-Fladen | Herdstelle | 4 Leinsamen, 3 Haselnüsse, 1 Wasser | 2 | Traglast +10 |
| T2 | Schlehen-Hagebutten-Mus | Herdstelle | 3 Schlehen, 3 Hagebutten | 1 | Kälteresistenz 15 % |
| T2 | Waldbeerenmus | Herdstelle | je 2 Blaubeeren, Walderdbeeren, Brombeeren | 1 | Regeneration +1,0 |
| T3 | Gerstengrütze mit Beeren | Lehmbackofen | 2 Gerste, 2 Beeren, 1 Wasser | 1 | Gesundheit +21, Regeneration +1,05 |
| T3 | Gerstenfladenbrot | Lehmbackofen | 3 Gerste, 1 Wasser | 2 | Traglast +15 |
| T3 | Haferbrei mit Nüssen und Apfel | Lehmbackofen | 2 Hafer, 2 Haselnüsse, 1 Wildapfel | 1 | Regeneration +1,5 |
| T3 | Ofenkartoffeln mit Zwiebeln | Lehmbackofen | 3 Kartoffeln, 1 Zwiebel | 1 | Kälteresistenz 20 % |
| T3 | Ofenbirnen mit Leinsamen | Lehmbackofen | 2 Wildbirnen, 2 Leinsamen | 1 | Gesundheit +30 |
| T5 | Gerstenbier (Getränk) | Braukessel | 2 Gerste, 2 Wasser | 2 | Schaden +8 %, Kälteresistenz 21 % |
| T5 | Met (Getränk) | Braukessel | 2 Honig, 2 Wasser | 2 | Gesundheit +42, Kälteresistenz 21 % |
| T6 | Gewürzter Schlehenmet (Getränk) | Festtafel | 1 Met, 3 Schlehen, 2 Holunderbeeren | 1 | Kälte- und Giftresistenz je 28 % |
| T6 | Honigkuchen mit Nüssen und Birnen | Festtafel | 1 Honig, 2 Gerste, 3 Haselnüsse, 2 Wildbirnen | 2 | Traglast +21, Schaden +10,5 % |

**Noch nicht im Spiel (fehlende Zutaten: Fleisch, Fisch, Milch):** Räucherfisch, Trockenfleisch, Fischsuppe, Wildeintopf (T4, Räucherkammer); Skyr mit Blaubeeren, Braten mit Äpfeln und Zwiebeln (T5); Julbraten mit Met-Soße, Festtags-Eintopf, Fischplatte mit Skyr und Fladen (T6). Werte wie im Vorschlag vom 2026-11-14. Der Datenprüfer erlaubt keine unerreichbaren Gegenstände, deshalb kommen sie erst mit Jagd, Fischerei und Tierhaltung. Die Räucherkammer kommt dann ebenfalls, sie hätte heute keine Rezepte.

<a id="d13-3"></a>

### 13.3 Neue Zutaten

| Zutat | Herkunft | Anmerkung |
|---|---|---|
| Gerste (`item_barley`) | Wildgerste im Startgebiet (2–4), danach Feldfrucht: 2 Spieltage, 4–6 je Einheit | Saatgut = Korn selbst, verdirbt nicht ⚑ |
| Hafer (`item_oats`) | Wildhafer im Startgebiet, Feldfrucht wie Gerste | wie Gerste |
| Honig (`item_honey`) | Wildbienennest (von Hand, 1–2, kommt nach 2 Spieltagen wieder), 2 Stück im Startgebiet | verdirbt nicht (stimmt auch historisch) |

Gerste und Hafer haben vorläufig die Flachs-Modelle im Feld und als Wildpflanze. Das Bienennest ist ein Platzhalter aus Grundformen.

<a id="d13-4"></a>

### 13.4 Taverne (Gebäude)

- `building_tavern` (`AGamerTavern`), Platzhalter-Quader 8 × 5 m. Kosten 6 Baumstämme, 20 Bretter, 10 Steine.
- **Arbeitsplätze** für **Koch** und **Wirt**, je 1 → 2 → 3 ⚑. Die Arbeitsplätze gelten je Beruf, weil der Ausbau-Mechanismus nur eine Zahl je Gebäude kennt. Der Vorschlag „Wirt 1, 1, 2“ ließ sich so nicht abbilden.
- **Ausbau** (E mit Hammer, wie bei den anderen Arbeitsgebäuden): Stufe 2 kostet 16 Bretter und 12 Steine, Stufe 3 kostet 4 Baumstämme, 20 Bretter und 8 Steine.
- **Vorrat** der Taverne: 16 Gerichte und Getränke ⚑. Der Wirt holt sie aus dem Lager, zuerst das, wovon die Taverne am wenigsten hat. Nur Gekochtes mit Buffs, kein Rohes.
- **Bewohner essen in der Taverne**, wenn sie Essen hat, sonst wie bisher am Lager. Dazu trinken sie ein Getränk, wenn eines da ist. **Stimmung:** +0,05 je Stufe des besten Gerichts, +0,1 für ein Getränk ⚑ (`tavern.mood_per_tier`, `mood_drink`). Die Stimmung baut die Gameplay-Session: `OnTavernMeal` ruft `AGamerVillager::AddMood` auf (Bonus bis 0,4, klingt in etwa 4 Spieltagen ab; eingehängt am 2026-11-15, noch nicht gebaut und getestet). Bewohner bekommen keine Kampf-Buffs.
- Das Lagerfeuer hat keine Arbeitsplätze mehr (der Koch arbeitet an der Taverne). Mit der Feldbau-Session abgesprochen.

<a id="d13-5"></a>

### 13.5 Kochstellen

| Kochstelle | Kosten | braucht | Rezepte |
|---|---|---|---|
| Lagerfeuer (bestehend) | wie bisher | – | T1 |
| Herdstelle mit Kessel und Backstein | 8 Steine, 2 Baumstämme, 2 Bretter | Taverne Stufe 1 im Umkreis von 15 m | T2 |
| Lehmbackofen | 12 Steine, 1 großer Stein, 4 Bretter | Taverne Stufe 2 | T3 |
| Braukessel | 6 Steine, 6 Bretter, 2 Baumstämme | Taverne Stufe 3 | T5 (Bier, Met) |
| Festtafel | 2 Baumstämme, 16 Bretter | Taverne Stufe 3 | T6 |

- Der Bauplatz wird schon in der Vorschau geprüft und auf dem Server noch einmal („Braucht eine fertige Taverne (Stufe 2) im Umkreis von 15 m“).
- Der **Spieler** kocht selbst an der Kochstelle: E öffnet das Stationsfenster der Gameplay-Session, gekocht wird über `UGamerWorkComponent::ServerCraft`.
- Der **Koch** (`job_cook`, Station Taverne, Umkreis 15 m) kocht alle Rezepte der Reihe nach, solange davon weniger als 6 im Lager sind ⚑, jeweils an der nächsten passenden Kochstelle bei der Taverne. T1 braucht deshalb ein Lagerfeuer in der Nähe der Taverne.

<a id="d13-6"></a>

### 13.6 Technik

- Daten: `items.json` `food`, `buildables.json` `requires` und `tavern`, `jobs.json` `kind: "serve"` (Format: `docs/06_datenformat.md`).
- Code: `GamerBuffComponent` (an `AGamerCharacter`), `GamerTavern` (Gebäude, Vorrat, Bauplatz-Prüfung), Bewohner in `GamerVillager.cpp`, Testschritte `eat:<item>`, `meals`, `tavern` in `GamerPlayerController.cpp`, Datentest `Gamer.Data.MealsAndTavern`.
- Tests: `-GamerBuffMinuteSeconds=N` verkürzt eine Buff-Minute (nicht in Shipping).
- Schnittstelle für die Anzeige (Gameplay-Session): `GetBuffs()->GetActiveMeals()`, `GetRemainingSeconds(i)`, `GetBonus(stat)`, `OnMealsChanged`.

<a id="d13-7"></a>

### 13.7 Tests (2026-11-15)

- Datentests: 6 von 6 bestanden, darunter der neue Test `Gamer.Data.MealsAndTavern`.
- Host + 2 Clients (`tools/mp_test.ps1`, Testskript) und Einzelspieler (Server + 1 Spieler), jeweils bestanden:
  - **Bauplatz:** Die Herdstelle ohne Taverne wird abgelehnt, der Lehmbackofen bei Taverne Stufe 1 auch. Nach dem Ausbau auf Stufe 3 sind beide gültig.
  - **Kochen und Essen:** Eintopf an der Herdstelle gekocht. Eintopf, Honigkuchen und Met gegessen, die Buffs stimmen und kommen beim Client an. Die vierte Mahlzeit ersetzt die mit der kürzesten Restzeit. „Baumstamm“ wird als Mahlzeit abgelehnt. Nach Ablauf sind keine Buffs mehr aktiv.
  - **Taverne und Bewohner:** Die Arbeitsplätze steigen 1 → 2 → 3. Der Koch kocht am Lagerfeuer bei der Taverne. Der Wirt bringt Mahlzeiten, Eintopf und Bier in die Taverne, der Client sieht den Vorrat. Die Bewohner essen und trinken dort, der Stimmungswert wird geloggt.

**Nachtest 2026-11-21** mit dem gemeinsamen Stand (nach Speichersystem, HUD-Anzeige und Stimmung): Build ok, Datentest `Gamer.Data.MealsAndTavern` grün. Dasselbe Testskript mit Host + 2 Clients und mit Server + 1 Spieler bestanden: Bauplatz-Regel, Kochen, Essen, Ersetzen der kürzesten Mahlzeit, Ablauf, Ausbau 1 → 3, Koch am Lagerfeuer, Wirt (7 Lieferungen), 6 Mahlzeiten in der Taverne. Die Stimmung wird jetzt über `AddMood` erhöht (im Test bis zu +0,15 Bonus je Bewohner). Die HUD-Anzeige selbst wurde ohne Grafik nicht angesehen.

<a id="d13-8"></a>

### 13.8 Grenzen, ehrlich

- **Die meisten Buffs wirken noch nicht:** Es gibt noch keine Lebenspunkte, keinen Kampf, keine Traglast-Grenze und keine Kälte. Die Buffs werden gesetzt, übertragen und abgefragt (`GetBonus`); die Systeme müssen sie lesen, sobald es sie gibt.
- **HUD:** Die „Satt“-Anzeige und die Mahlzeiten mit Restzeit (links unten) hat die Gameplay-Session eingebaut, noch nicht gebaut und getestet. Echte Icons kommen mit dem Icon-System.
- **Nicht getestet:**
  - Essen per echtem Linksklick, der Test ruft `ServerEat` direkt auf.
  - Bewohner, die an Herdstelle, Ofen, Braukessel oder Festtafel kochen (getestet ist nur das Lagerfeuer).
  - Verderb im Vorrat der Taverne: Der Vorrat verdirbt dort nicht, das ist eine Lücke.
- **Modelle:** Gebäude, Kochstellen und Bienennest sind Platzhalter, die Gerichte haben noch kein Modell und kein Icon. Später kommen sie im Stil „Gelebter Norden“.
- **Historisch:** Kartoffeln bleiben ein Zugeständnis (offene Frage). Gerste, Hafer, Honig, Met, Bier, Grütze und Fladenbrot sind für die Wikingerzeit belegt.


[↑ zum Inhaltsverzeichnis](#inhaltsverzeichnis)

<a id="d14"></a>

## 14. Magie-Gespür: Umschauen und Lauschen

> Quelle: `docs/13_magie_gespuer.md`

Stand 2026-11-15. Gehört zu den Entscheidungen „Umschauen und Lauschen getrennt“ und „Magie-Gespür: Stufen und Boni“ in `docs/01_entscheidungen.md`. Alle Zahlen sind **Startwerte zum Justieren im Spiel**, sie stehen in `game/Content/Data/scan.json` (Umschauen) und `game/Content/Data/listen.json` (Lauschen). Es gibt noch keine echten magischen Objekte; getestet wird mit einem Testziel.

<a id="d14-1"></a>

### 14.1 Zweck

Material und Magie sind zwei getrennte Sinne:

- **Umschauen** (Taste Z) zeigt alltägliches Material. Es hat keine Stufen und keinen Fortschritt.
- **Lauschen** (Taste Q) spürt den Gezeitenhall, also Magie. Man verbessert sich darin, indem man aktiv nach Magie sucht („Karotte vor der Nase“); das wirkt auch im Mehrspieler. Solo-Spiel ist der Maßstab, Koop ist ein Zusatz (Regel 2: derselbe Code, die Gruppen-Boni greifen bei einem Spieler einfach nicht).

<a id="d14-2"></a>

### 14.2 Umschauen (Z)

- Zeigt Nahrung, Holz, Stein, Pflanzen und liegende Gegenstände, auch durch Wände. Keine Magie, keine Stufen.
- 40 m Radius, Ausbreitung 5 s, Funde bleiben 20 s sichtbar (ab dem Moment, in dem die Welle sie erreicht), Abklingzeit 12 s. Keine Ausdauer.
- Die Welle ist eine **Sinuskurve**: ein weicher, transparenter Ring in Treibholz und Nebelgrau läuft als wellige Linie aus. Amplitude (110 cm), Wellenzahl (7) und Tempo des Kräuselns stehen in `scan.json` („wave“).
- Markierungen erscheinen, wenn die Welle sie erreicht, und blenden aus.

<a id="d14-3"></a>

### 14.3 Lauschen (Q)

#### Passiv

Ist ein magisches Ziel innerhalb der Reichweite der aktuellen Stufe, schimmert der **Bildschirmrand türkis** (Gezeitenlicht `#A9E9DC`), **je näher, desto stärker und tiefer** (heiß/kalt). Ein weit entferntes Ziel in Reichweite zeigt kaum etwas. Für das **Summen** gibt es nur einen Audio-Haken (`UGamerListenComponent::OnMagicHum`, Intensität 0..1); es gibt noch keine Sounds.

#### Aktiv

Eigene umbelegbare Taste (Standard Q, Gamepad linke Schultertaste). Die Figur bleibt stehen und lauscht: ein **langsamer türkiser Sinus-Ring** (offener Ring mit drei Kerben, Stilvorgabe) breitet sich mit konstant 2 m/s aus. **Bewegung bricht das Lauschen ab** (die Welle endet). Danach gilt eine Abklingzeit. Gefundene Dinge erscheinen als **Silhouette** (türkise Kontur mit Glühen, Kontur-Material `M_Outline`, Stencil 2), dazu eine Beschriftung je nach Stufe.

#### Stufen

Reichweite, Wellendauer und Sichtdauer wachsen mit der Stufe (die Dauer ergibt sich aus der Reichweite bei 2 m/s; sie steht je Stufe einzeln in den Daten, damit man sie getrennt justieren kann). Die Reichweite gilt auch für das passive Signal.

| Stufe | ab EP | Reichweite | Wellendauer | Funde sichtbar | Abklingzeit | Wirkung |
|---|---:|---:|---:|---:|---:|---|
| 1 Gespür | 0 | 10 m | 5 s | 15 s | 15 s | Flimmern (nur Silhouette, schwach) |
| 2 Ahnung | 10 | 15 m | 7,5 s | 20 s | 15 s | Richtung (Pfeil am Rand) und grobe Art |
| 3 Deutung | 30 | 20 m | 10 s | 25 s | 15 s | Name und Menge |
| 4 Klarblick | 75 | 25 m | 12,5 s | 30 s | 15 s | Versteckte Funde (Tag `GamerMagicHidden`), Markieren möglich |
| 5 Seidr-Blick | 160 | 30 m | 15 s | 40 s | 10 s | Seltene Funde (Tag `GamerMagicRare`), kürzere Abklingzeit |

Die Deckkraft der Silhouette wächst mit der Stufe (0,30 → 1,00). Jeder startet auf Stufe 1; es gibt kein Sofort-Maximum.

#### Erfahrung (EP)

Der Zähler liegt pro Spieler auf dem **Server**, wird an den Besitzer **repliziert** und **gespeichert** (Speicherschlüssel `listen`, Feld `ep`). Quellen (nur als Daten, die Objekte gibt es noch nicht; sie rufen `AwardSource` auf):

| Quelle | EP |
|---|---:|
| neuer Fund entdeckt (`new_find`) | +2 |
| Seeglas ernten (`seaglass_harvest`) | +1 |
| Steinkreis einstimmen (`stone_circle`, einmal pro Kreis) | +8 |
| Gezeitenhall-Ereignis (`tide_event`) | +15 |

**Jeder Fund zählt pro Spieler nur einmal.** Der Server merkt sich pro Spieler, welche Funde (ID) schon gezählt sind (`UGamerListenComponent::AwardSource(Quelle, Ort, Fund-ID)`), und speichert die Liste (Feld `counted_targets`). Mitspieler, die einen geteilten Fund mit 50 % bekommen, zählen ihn ebenfalls nur einmal. Ereignisse ohne Fund-ID zählen immer. Einen „+1 je Lauschen“ gibt es nicht: Man soll nicht neben einem einzigen Ziel endlos lauschen können.

Faktor **Gespür-Tempo** (Weltoption): langsam ×0,6, normal ×1, schnell ×1,6.

#### Boni (kein Sofort-Maximum, höchstens Stufe 5)

- **Seherin** (`origin:origin_seer`): startet auf Stufe 2 (kleiner Bonus; alle anderen verbessern sich von Grund auf).
- **Seeglas-Amulett** (`item_seaglass_amulet`): +1 Stufe, solange man es trägt. Das Amulett gibt es noch nicht.
- **Dorf-Seherin**: +25 % EP für alle Spieler, solange sie freigeschaltet ist und im Dorf lebt. Sie wird durch eine **Quest** freigeschaltet; Quests gibt es noch nicht. Bedingung vorgemerkt: `unlock:quest_village_seer` und `job:job_seer`; `unlock:` wird nicht geprüft (gilt als nicht erfüllt).
- **D** (Nacht, Nebel, Steinkreis, Winter): +1 Stufe, solange eine Bedingung gilt. Geprüft wird bisher nur `season:winter`; Nacht und Nebel brauchen Tag/Nacht und Wetter, Steinkreis braucht Orte. **D braucht Tag/Nacht und Wetter.**

<a id="d14-4"></a>

### 14.4 Koop

- **Gemeinsamer Fund:** Mitspieler im Umkreis von 30 m bekommen 50 % der EP.
- **Gemeinsames Lauschen:** lauschen zwei oder mehr Spieler innerhalb von 10 s und 15 m, sieht jeder mindestens die Stufe des Besten minus 1, und Reichweite und Dauer steigen um 25 %. Der Server rechnet das (`ServerStartListen`) und schickt jedem das Ergebnis.
- **Markieren ab Stufe 4, für alle sichtbar:** nur vorgemerkt (`CanMark()`, Daten „marks“); es gibt noch keine Funde zum Markieren. Eigene Markierungen sollen auch solo gespeichert bleiben (Speicherung folgt mit den Funden).

<a id="d14-5"></a>

### 14.5 Zieltempo Solo (Annahme, nicht gemessen)

Die Zahlen sind auf Solo ausgelegt. Annahme: **Stufe 3 nach etwa 3–4 Stunden** Spielzeit (30 EP, also grob 8–10 Funde mit Steinkreisen oder einem Ereignis), **Stufe 5 nach etwa 25–30 Stunden** (160 EP). Das hängt davon ab, wie oft es Funde gibt; erst mit den magischen Objekten lässt es sich justieren. Zum Anpassen dienen die Schwellen und Quellen in `listen.json` sowie das Gespür-Tempo der Welt.

<a id="d14-6"></a>

### 14.6 Weltoption „Gespür-Tempo“

`EGamerGespuerTempo` (langsam, normal, schnell) in `FGamerWorldOptions`, URL-Option `?Tempo=slow|normal|fast`. Sie steht als Zeile „Gespür-Tempo“ unter „Welt erstellen“, wird wie die anderen Weltoptionen mit der Welt gespeichert (Teil des Texts `options` im Spielstand) und gilt für alle Spieler der Welt. Die Faktoren stehen in `listen.json` („tempo“).

<a id="d14-7"></a>

### 14.7 Magische Objekte (Konvention für die Magie-Phase)

Ein magisches Ding ist ein Actor mit dem Tag `GamerMagic`. Optional: `GamerMagicHidden` (sichtbar ab Stufe 4), `GamerMagicRare` (ab Stufe 5), `kind:<Art>`, `coarse:<grobe Art>` (Stufe 2), `name:<Name>` (Stufe 3), `amount:<Menge>`. Beim Entdecken, Ernten, Einstimmen ruft das Objekt auf dem Server `UGamerListenComponent::AwardSource(<Quelle>, <Ort>)` auf.

<a id="d14-8"></a>

### 14.8 Technik und Tests

- Dateien: `GamerListenComponent.h/.cpp` (Stufen, EP, Boni, Lauschen, Gruppe, Silhouette), `GamerScanComponent.h/.cpp` (nur noch Umschauen), `GamerHUDListen.cpp` (Randschimmern, Ring, Beschriftung), Daten `scan.json` und `listen.json`, Material `tools/unreal/build_outline_material.py`.
- Testschritte: `scan`, `scanage:<s>`, `listen`, `listenage:<s>`, `listenstate`, `magictarget:<Art>|<Name>[|hidden][|rare]`, `magicevent:<Quelle>[|<Fund-ID>]` (setzt EP direkt, mit Fund-ID einmal pro Fund), `magicorigin:off|on`, `amulet:on|off`, `magiclevel`; Weltoption per Karten-URL `?Tempo=slow`.
- Gebaut und getestet (2026-11-15): Schwellen 0/10/30/75/160, Seherin auf Stufe 2, Amulett und Winter je +1, Tempo langsam (+1,2 statt +2 je Fund), gemeinsamer Fund (Mitspieler 50 %), gemeinsames Lauschen mit Host + 2 Spielern (Reichweite ×1,25), Speichern und Laden von EP und Weltoption, „Umschauen zeigt keine Magie“. Bilder: Sinus-Welle beim Umschauen, Sinus-Ring und Silhouette beim Lauschen, Randschimmern nah und fern, Menüzeile.

<a id="d14-9"></a>

### 14.9 Offen

- Echte magische Objekte, Amulett, Beruf Seherin samt Quest, Seidr-Blick lernen (Bedingung `ability:ability_seidr_sight`; `ability_seidr_vision` in `characters.json` ist die Wettervorhersage und etwas anderes), D mit Nacht/Nebel/Steinkreis.
- Markieren und gespeicherte eigene Markierungen; Summen (Audio) und Flimmern der Dinge in der Welt selbst.
- Die Silhouette nutzt Stencil 2; ein Ziel im Fadenkreuz (Stencil 1) und Magie am selben Ding gleichzeitig ist nicht gelöst.
- Die Rechnung der Gruppe nutzt die Serverwerte der Spieler; eine Abbruchmeldung bei „zu weit auseinander“ gibt es nicht.


[↑ zum Inhaltsverzeichnis](#inhaltsverzeichnis)

<a id="d15"></a>

## 15. Charaktererstellung, Charakterauswahl, Charaktereditor

> Quelle: `docs/11_charaktere.md`

Stand 2026-11-14. Wunsch des Nutzers: Charaktererstellung bzw. -auswahl und ein Charaktereditor. Bei der Auswahl geht es vor allem um **freischaltbare Fähigkeiten, Werte und Anfangswaffen**; das Aussehen ist nur ein Vorschlag, den der Spieler im Editor ändern kann.

Alles hier ist ein **Vorschlag von Claude**, nicht bestätigt.

<a id="d15-1"></a>

### 15.1 Ablauf (Vorschlag)

1. **Auswahl:** fünf Herkünfte (Klassen) zur Wahl, jede mit Vorschlags-Aussehen, Startwerten, Startausrüstung und einer Startfähigkeit.
2. **Editor:** Aussehen frei ändern (Körper, Gesicht, Haare, Bart, Farben, Tätowierungen/Runenbemalung, Kleidung aus der Startausrüstung); die Werte der Herkunft bleiben.
3. **Name**, dann ins Spiel. Der Charakter gehört zum Spieler, nicht zur Welt (in Koop-Welten mitnehmbar) – offene Frage.

<a id="d15-2"></a>

### 15.2 Werte (fünf, keine Ausdauer-Sperre)

| Wert | Wirkt auf |
|---|---|
| Stärke | Nahkampfschaden, Traglast, Abbau-Tempo |
| Geschick | Fernkampf, Ausweichen, Sammel- und Ernte-Tempo |
| Zähigkeit | Gesundheit, Regeneration, Kälte- und Hungerresistenz |
| Handwerk | Herstell- und Bautempo, Haltbarkeit hergestellter Dinge, Reparatur |
| Wille | Magie (Runen, Seidr), Tränke, Wirkung auf Bewohner (Stimmung) |

Startwerte je Herkunft: Summe 25, jeder Wert 3–8. Später steigen Werte durch Tun und Fähigkeitspunkte.

<a id="d15-3"></a>

### 15.3 Fünf Herkünfte

| | Jägerin/Jäger | Krieger/in | Schmied/in | Bäuerin/Bauer | Seherin/Seher (Völva) |
|---|---|---|---|---|---|
| **Aussehen (Vorschlag)** | Leder, Fellumhang, Kapuze, schlank | Kettenhemd-Andeutung, Rundschild auf dem Rücken, Narben | kräftig, Lederschürze, Rußspuren, Zöpfe | Leinenkittel, Strohhut oder Kopftuch, Gürteltasche | dunkler Umhang, Stab mit Runen, Perlen, Bemalung im Gesicht |
| **Werte** S/G/Z/H/W | 4/8/5/4/4 | 8/5/7/3/2 | 7/3/6/7/2 | 4/5/6/6/4 | 2/5/4/4/8 |
| **Anfangswaffe** | Kurzbogen + 20 Pfeile, Dolch | Bartaxt + Rundschild | Schmiedehammer (Waffe und Werkzeug) | Sichel, Hackmesser | Zauberstab (Runenstab), Dolch |
| **Startfähigkeit** | Spuren lesen (Tiere und Vorkommen in der Nähe markiert) | Schildstoß (bricht die Haltung des Gegners) | Feldreparatur (Werkzeuge unterwegs reparieren) | Grüner Daumen (+1 Ertrag bei Feld und Wildpflanzen) | Rune des Schutzes (kleiner Schild auf sich oder Mitspieler) |
| **Freischaltbar (Beispiele)** | Leiser Gang, Fallen, Zielschuss, Häuten (mehr Fell/Fleisch) | Kampfschrei (Gruppe stärker), Haltungsbruch, Berserkerwut (Abklingzeit, keine Ausdauer) | bessere Waffen und Werkzeuge, Erz verhütten, Rüstung schmieden, Baumeister-Bonus | Saatgut sparen, längere Haltbarkeit, Tiere halten, Kompost-Meister | Seidr-Vision (Wetter/Jahreszeit vorhersehen), Heiltränke, Runen auf Waffen, Segen auf Felder |
| **Stärke im Koop** | Erkunden, Jagd | Schutz, Front | Ausrüstung für alle | Nahrung für Siedlung | Unterstützung, Heilung |

<a id="d15-4"></a>

### 15.4 Entscheidungen (2026-11-14)

- Fähigkeiten: jeder kann alles lernen, die Herkunft gibt Vorsprung und einen günstigeren eigenen Zweig.
- Charakter pro Spieler (in jede Welt mitnehmbar).
- Figurenmodell: MetaHuman (Nachteile siehe `docs/01_entscheidungen.md`).
- Umsetzung und Stand: `docs/02_phasenplan.md`, Abschnitt „Charakterauswahl …“. Werte-Korrektur (Seherin 3/5/5/4/8, Grenzen 2–8) ist ein Vorschlag von Claude.

<a id="d15-5"></a>

### 15.5 Offene Fragen (Stand vor der Entscheidung)

- Sind Fähigkeiten an die Herkunft gebunden oder kann später jeder alles lernen (Herkunft = Vorsprung)? Vorschlag: jeder kann alles lernen, die Herkunft gibt Startwerte, Startfähigkeit und einen günstigeren Zweig.
- Geschlecht/Körperform: frei im Editor, unabhängig von der Herkunft (Vorschlag).
- Charakter pro Spieler oder pro Welt?
- Technik: Figurenmodell und Gesichts-/Haar-Varianten fehlen noch (Platzhalter-Zylinder). MetaHuman ist kostenlos in Unreal, aber groß und nicht im Wikinger-Stil vorgefertigt; Alternative eigenes modulares Modell. Muss entschieden werden, bevor der Editor gebaut wird.


[↑ zum Inhaltsverzeichnis](#inhaltsverzeichnis)

<a id="d16"></a>

## 16. MetaHuman-Figuren: Was nach der Anmeldung zu tun ist

> Quelle: `docs/16_metahuman_anmeldung.md`

Stand 2026-11-16. Alles, was ohne Epic-Anmeldung geht, ist erledigt. Diese Anleitung beschreibt die restlichen Klicks.
Bezeichnungen im Editor können leicht abweichen (Unreal 5.8, Sprache Deutsch/Englisch gemischt).

<a id="d16-1"></a>

### 16.1 Was schon vorbereitet ist

- Zwei Figuren im Inhaltsbrowser unter **Gamer → Characters → MetaHuman**:
  - `MHC_Kraeftig` (Körperbau „kräftig“, 182 cm, breite Schultern) – ersetzt später Manny
  - `MHC_Schmal` (Körperbau „schmal“, 170 cm) – ersetzt später Quinn
  - Körpermaße und Hautton sind nach der Stilvorgabe (`design/stil_norden_gezeitenhall.md`) vorbelegt: wettergegerbt, etwas rauere Haut. Körperbau ist nicht an ein Geschlecht gebunden.
- Projekteinstellungen für MetaHumans sind aktiv (hast du am 15.11. selbst bestätigt).
- Das Spiel benutzt die MetaHumans automatisch, sobald sie zusammengebaut und verknüpft sind; bis dahin bleiben Manny und Quinn. Animationen bleiben dieselben.
- `MHC_Grundfigur` (Ordner Characters) ist dein erster Versuch und wird nicht gebraucht.

<a id="d16-2"></a>

### 16.2 Schritt für Schritt

**Vorher:** Editor nur über `Gamer.uproject` oder `E:\UE58\Engine\Binaries\Win64\UnrealEditor.exe` öffnen, **nie über den Epic Launcher**.

#### 1. Anmelden (einmal)
1. Inhaltsbrowser → Gamer → Characters → MetaHuman → **`MHC_Kraeftig` doppelklicken**. Der MetaHuman Creator öffnet sich.
2. In der Werkzeugleiste oben die **Anmelde-Schaltfläche** (Epic-Konto) klicken.
3. Im Browser mit deinem Epic-Konto anmelden und bestätigen. Der Editor wartet nur ca. 90 Sekunden darauf.
4. Erfolg: Die Schaltfläche zeigt deinen Namen bzw. „angemeldet“.

#### 2. Aussehen anpassen (freiwillig, ohne Anmeldung möglich)
Nach Geschmack, passend zum Stil: Kopf/Gesicht wählen, **Haare: Zöpfe oder lang**, **Bart: Vollbart oder geflochten**, Haarfarbe blond, rot, braun oder grau. Körperbau bitte nicht stark ändern (das Spiel ordnet „kräftig/schmal“ zu).

#### 3. Rig erzeugen (braucht Anmeldung)
Bereich **„Rig“** (bzw. „Auto-Rig“) → **„Create Full Rig“ / „Vollständiges Rig erstellen“**. Dauert 1–3 Minuten. Erfolg: Meldung „rigged“, kein Fehler im Nachrichten-Log.

#### 4. Texturen holen (braucht Anmeldung)
Bereich **„Textures“** → **„Download Texture Sources“**, Auflösung **2k** (reicht, spart Speicher). Dauert einige Minuten.

#### 5. Zusammenbauen – zweimal je Figur
Bereich **„Assembly“ / „Zusammenbauen“**:

| Durchgang | Pipeline / Qualität | Zielordner („Root Directory“) |
|---|---|---|
| Spieler | **UE Optimized, High** | `/Game/Gamer/Characters/MetaHuman/Kraeftig` |
| Bewohner, Entfernung | **UE Optimized, Low** | `/Game/Gamer/Characters/MetaHuman/Kraeftig_Low` |

Jeweils **„Assemble“** klicken und warten. Danach **Strg+S** (alles speichern).

**Wenn sich der Zielordner nicht einstellen lässt:** Der Creator schreibt dann nach `/Game/MetaHumans/MHC_Kraeftig` (bzw. `MHC_Schmal`). Das ist in Ordnung, mein Verknüpfungs-Skript findet auch diesen Ordner, aber nur für die **High**-Fassung (Spieler). Den Low-Durchgang dann weglassen; Bewohner nehmen die High-Figur und überspringen deren feinste Detailstufen.

**Speicher (Absturz „Nicht genügend Arbeitsspeicher“, 16.11.):** Das Zusammenbauen braucht viel RAM.
- Auslagerungsdatei vergrößern (fest, 32–48 GB auf D:, danach Neustart).
- Vor dem Zusammenbau alle anderen Programme schließen (Browser, Spiele-Tests).
- **Eine Figur pro Editor-Sitzung:** Kraeftig zusammenbauen, speichern, Editor schließen und neu öffnen, dann Schmal.
- Reicht es weiterhin nicht: Texturen mit **1k** statt 2k holen (Schritt 4 wiederholen).

#### 6. Dasselbe für `MHC_Schmal`
Schritte 3–5 mit `MHC_Schmal`, Zielordner `…/MetaHuman/Schmal` und `…/MetaHuman/Schmal_Low`.

#### 7. Editor schließen und Bescheid geben
Editor zu, dann sag mir Bescheid. Ich führe aus:
```
tools\metahuman.ps1 Link
```
Das findet die zusammengebauten Körper und Köpfe, macht ihr Skelett mit den vorhandenen Animationen verträglich und schreibt `game/Content/Data/figures.json`. Danach baue und teste ich.

<a id="d16-3"></a>

### 16.3 Stand 16.11.: Zusammenbau per Skript erledigt

Nach Anmeldung und Auto-Rig (vom Nutzer im Creator) läuft der Rest ohne Klicks:
```
tools\metahuman.ps1 Assemble -Figure MHC_Kraeftig
tools\metahuman.ps1 Assemble -Figure MHC_Schmal
tools\metahuman.ps1 Link
```
- `Assemble` holt fehlende Textur-Quellen, baut High und Low und speichert. Es startet einen **normalen Editor** (Fenster erscheint und schließt sich selbst), weil TextureGraph (backt die Gesichtstexturen) im Hintergrund-Commandlet gesperrt ist bzw. mit `TextureGraph.AllowCommandlets=1` abstürzt.
- Ergebnis: Kraeftig 205 MB / Kraeftig_Low 116 MB (Schmal ähnlich), `figures.json` mit 4 Figuren.
- Grafiktest (1 Spieler, Bewohnerin Astrid): Spieler = MetaHuman Kraeftig, Bewohnerin = Schmal_Low; Animationen laufen, Log `Figure strong: body ok, face ok`. 140 fps bei Mittel, 960×540.
- **Offen:** Spielerfigur hat keine Haare, beide tragen nur die Creator-Unterwäsche (die Tönung der Zwischenfigur wirkt auf MetaHumans nicht). Haare/Kleidung im Creator wählen und `Assemble` erneut laufen lassen.
- **VRAM-Messung 16.11.** (Spieler + 15 Bewohner im Bild, Mittel, 960×540, nvidia-smi alle 2 s): Grafikkarte gesamt höchstens 1562 MiB, davon ca. 580 MiB Desktop ohne Spiel → Spiel (inkl. lokalem Server) ca. 1,0 GB. 144 fps (Begrenzung erreicht). Ziel 8 GB weit unterschritten; bei Vollbild/Hoch und größerer Welt nachmessen.
- Alle Bewohner eines Körperbaus sehen gleich aus (ein Gesicht je Figur). Mehr Abwechslung bräuchte weitere MetaHumans oder Variationen (Haare, Hautton) – noch nicht umgesetzt.

<a id="d16-4"></a>

### 16.4 So prüfst du, ob es geklappt hat

- In den vier Zielordnern liegen Skeletal Meshes mit „Body“ und „Face“ im Namen.
- Nach meinem `Link`-Schritt steht im Spiel-Log: `Figures: 4 MetaHuman figure(s)` und `Figure strong: body ok, face ok`.
- Im Spiel (`Spiel_starten.bat`) ist deine Figur ein MetaHuman statt der grauen Puppe; Bewohner ebenfalls.

<a id="d16-5"></a>

### 16.5 Detailstufen (Ziel 8 GB Grafikspeicher)

- **Spieler:** „High“-Zusammenbau (feine Detailstufen nah, gröbere automatisch auf Entfernung).
- **Bewohner:** „Low“-Zusammenbau; fehlt er, nimmt das Spiel den Spieler-Zusammenbau und überspringt dessen zwei feinsten Detailstufen.
- Skin Cache ist an (für MetaHuman-Köpfe nötig) und kostet etwas Grafikspeicher; nach dem Zusammenbau messe ich mit 10–20 Bewohnern nach.

<a id="d16-6"></a>

### 16.6 Was nur mit Anmeldung geht / was offen ist

- Rig, Texturen und Zusammenbau gehen nur angemeldet (Epics Cloud-Dienst).
- Kleidung: Für diesen Schritt reicht die Standardkleidung des Creators in gedämpften Farben; eigene Wikinger-Kleidung kommt später.
- Gesichtsanimation (Mimik) ist noch nicht angebunden: Der Kopf folgt dem Körper, das Gesicht bleibt neutral.
- Ungeprüft, bis der Zusammenbau da ist: ob die Ordner- und Dateinamen des Zusammenbaus zu `tools/unreal/metahuman_link.py` passen (sucht „Body“/„Face“ im Namen) und wie gut die Animationen auf den MetaHuman-Proportionen sitzen.

<a id="d16-7"></a>

### 16.7 Kleidung (eigene Umsetzung, Stand 21.11.)

`tools\clothing.ps1 All` (Export → Build → Import, Unreal-Schritte mit Sperre):
- **Export:** Körper Kraeftig/Schmal als FBX nach `game/Saved/Clothing` (läuft im normalen Editor-Fenster; als Commandlet stürzt der Skeletal-Mesh-Export ab).
- **Build:** `tools/blender/build_clothing.py` schneidet die Kleidung aus der Körperoberfläche (behält die Skin-Gewichte), glättet sie, damit sie locker sitzt, und ergänzt Röcke und Gürtel als Ellipsen-Röhren; deren Gewichte kommen nur von Rumpf und Beinen (sonst folgen sie den Händen). Vorschau: `tools/blender/preview_clothing.py`.
- **Import:** `/Game/Gamer/Characters/Clothing/<Körper>/SKM_Cloth_<Körper>_<Tunika|Kleid>`, Material `M_Cloth` (Parameter `Tint`, `Roughness`), Slots Wool/Trim/Linen/Leather.
- **Spiel:** `GamerFigures::ApplyClothing` (Leader Pose auf dem Körper). Spieler: Tunika in der gewählten Kleiderfarbe. Bewohner: Farben aus der Stilpalette nach Name, schmale Figuren zur Hälfte im langen Kleid.
- Dazu (21.11.): **Schürzenkleid** (Kleid + Schürze vorn/hinten, zwei ovale Fibeln, Perlenbogen) und **Umhang** als eigene Schicht (Schulterteil, Rückenbahn bis Knie, Fibel). Spieler: Tunika + Umhang. Bewohner: schmale Figuren halb Kleid (davon halb mit Schürze), ein Drittel mit Umhang; Seherin immer Schürzenkleid + Umhang.
- Haare (gelöst 22.11.): Die Gegenprüfung mit den unveränderten MetaHuman-Blueprints (`tools/unreal/hair_check.py`, Szenen-Kamera, Bilder in `game/Saved/HairCheck`) zeigte: Low-Blueprints haben eine `LODSyncComponent`, die Haare/Bärte fest auf die Haarhauben-LODs 5 (nah) bis 7 (fern) legt; Stufe 0 ist in der Low-Fassung leer. `GamerFigures` erzwingt deshalb für Low-Figuren Groom-LOD 5 (`FFigure::GroomLOD`). Seitdem haben alle Bewohner und die Spieler Haare. Die High-Fassung zeigt die lange Frisur `Hair_M_Layered` selbst im Blueprint nicht (Bart schon) – offen; Spieler nutzen bis dahin die Low-Fassung. Strähnen nah (Generalanweisung) brauchen die funktionierende High-Fassung bzw. einen Mittel-Zusammenbau; noch nicht umgesetzt, ebenso Haarsimulation/Wind.
- Hinweis Werkzeug: `-ExecutePythonScript` beendet den Editor nach dem Skript; für Skripte mit Tick-Rückruf `-ExecCmds="py <datei>"` nehmen.
- Messung 21.11. (1920×1080, Qualität Episch, Spieler + 15 Bewohner mit Kleidung): Grafikspeicher Spiel ca. 2,45 GB (nvidia-smi 4,25 GB gesamt minus 1,8 GB andere Programme), 60 fps mit V-Sync. 8 GB sind absolutes Maximum.
- Offen: Fellkragen, Lederweste (Referenzbilder `design/referenz/figuren`); Stoffstruktur nur als feines Rauschen; Bewohner-Haare noch unsichtbar.

<a id="d16-8"></a>

### 16.8 Spielerfiguren in Mittel-Qualität (Haarbewegung), Stand 2026-11-22
- `tools/metahuman.ps1 Assemble -Figure MHC_Kraeftig -Quality Mid` (ebenso MHC_Schmal) baut „UE Optimized Medium“ nach `.../Kraeftig_Mid` bzw. `.../Schmal_Mid`; `Link` trägt sie als `strong_mid`/`slim_mid` ein (figures.json: 18 Figuren). Die Spielerfigur nimmt zuerst `*_mid`, dann `*_low`, dann High.
- Die Medium-Blueprints ordnen die Haar-LODs per LODSync auf 3 (nah), 5 und 7 (fern) zu. In diesem Build zeigen die Stufen 0 bis 2 kein Haar; getestet: Bei Stufe 2 ist die Figur kahl. `GamerFigures` erzwingt für `*_mid` deshalb Groom-LOD 3 und schaltet die Simulation ein (`bSimulateHair`, Wind über `SetHairWind`). Die Bewohner bleiben bei LOD 5 (Helm, unbewegt).
- Epic liefert die Kartenstufen 3 und 4 mit ausgeschalteter Simulation aus (`Simulation = Disable`). `tools/unreal/metahuman_hairsim.py` schaltet sie für Haar und Bart auf `Enable`; die Augenbrauen bleiben ohne Physik. `metahuman_assemble.py` ruft das nach jedem Mid-Build auf, es lässt sich aber auch einzeln starten.
- Prüfung im Spiel: Beim Erzeugen schreibt `GamerFigures` je Haargruppe die Zeile „Hair <Asset> group n: … asset at LOD 3: 1, component: 1“. Für die kräftige Figur (`-GamerCharacter=origin_warrior:Ragnar`) bestätigt das Log, dass Haar und Bart simulieren. Die Zöpfe der schmalen Figur sind auf Stufe 3 ein starres Mesh ohne Physik und bewegen sich daher nicht; bei eng geflochtenen Zöpfen ist das vertretbar.
- Grafikspeicher (Test 1920×1080, 1 Spieler): höchstens 3,3 GB gesamt, deutlich unter der 8-GB-Grenze.
- Sichtprüfung: Auf den Standbildern von oben sind nur kleine Formänderungen beim Sprung zu erkennen, weil das Haar kurz ist. Eine deutlich sichtbare Bewegung bräuchte eine lange Frisur; das gehört zum Anbinden der Frisurwahl aus dem Charaktermenü (noch offen).
- Tool: `metahuman.ps1` wartet jetzt mit `WaitForExit()` statt `-Wait`, weil `-Wait` auch auf zenserver wartete und der Lauf deshalb hing.


[↑ zum Inhaltsverzeichnis](#inhaltsverzeichnis)


---

<a id="teil-4"></a>

# Teil IV – Story, Saga und Quests

<a id="d17"></a>

## 17. Saga-System und Questlinien (Entwurf)

> Quelle: `docs/14_saga_quests.md`

Stand 2026-11-15. **Alles in diesem Dokument ist ein Vorschlag von Claude**, solange es nicht in `docs/01_entscheidungen.md` als bestätigt steht.
Grundlagen: `docs/01_entscheidungen.md` (Magie, Scan, Bewohner: Tod), `game/Content/Data/listen.json` (Lauschen, Stufen, EP, Boni; von der Gameplay-Session), `design/stil_norden_gezeitenhall.md` (Gezeitenhall, Zeichen), `design/ASSET_LIST_GESAMT.md` (Saga-Buch, Fragmente, Skalde, Raben, Bosse, Jarlssitz).
Hinweis: `docs/13_magie_gespuer.md` gibt es noch nicht; die Gespür-Zahlen hier stammen aus `listen.json`.

**Vorgabe des Nutzers (2026-11-15, über den Game Director):** keine Story-Spiel-Lastigkeit. Die Story ist ein roter Faden, an dem man sich entlanghangeln kann. Erkunden, Überleben und Bauen stehen im Vordergrund. Es soll viele Arten von Nebenquests geben (Gegenstände finden, erste Magie finden und ausprobieren, erkunden). Questgegenstände gibt es, aber unauffällig. Dazu kommt ein Horden-Element. Gewünscht sind **5 verschiedene Storyline- und Saga-Vorschläge**, siehe **Abschnitt 10**. Die Abschnitte 1–9 sind der erste Entwurf und entsprechen ungefähr Vorschlag A. Sie gelten nur, wenn A oder eine Mischung mit A gewählt wird.
Ergänzung des Nutzers (2026-11-15): Die Questline darf doch der Hauptfokus sein, aber nur, wenn die Story wirklich fesselnd, sehr gut geschrieben und immersiv ist. Sonst bleibt sie roter Faden. Vorschlag von Claude: erst eine Probe schreiben (Prolog eines Vorschlags als fertiger Spieltext) und daran entscheiden.

Leitsätze:
- **Lauschen ist der Schlüssel zur Saga.** Magische Spuren führen in die Geschichte, die Geschichte führt zu besseren Spuren.
- **Kein Zeitdruck** (Regel 6): Keine Quest läuft ab, keine scheitert. Nacht, Nebel, Winter und Gezeitenhall-Ereignisse kommen wieder.
- **Solo ist Koop mit einem Spieler** (Regel 2): Jede Quest ist allein lösbar; zu mehreren geht es schneller oder bequemer, nicht anders.
- **Klein anfangen** (Regel 9): Im ersten Schritt nur der Kern und eine Questlinie (Prolog + Dorf-Seherin).

---

<a id="d17-1"></a>

### 17.1 1. Begriffe

| Begriff | Was es ist | Gehört | Beispiel |
|---|---|---|---|
| **Saga** | Die gesamte Chronik **einer Welt**: alle Stränge, Lieder, Siedlungs-Einträge und Taten. Das Saga-Buch zeigt sie | Welt | „Die Saga von Birkenvik“ |
| **Strang** (Questlinie) | Eine zusammenhängende Geschichte aus mehreren Etappen | Welt | „Der gebrochene Ring“, „Die Stimme im Seeglas“ |
| **Etappe** (Quest) | Ein Abschnitt eines Strangs mit einem oder mehreren Zielen | Welt | „Das Summen am Strand“ |
| **Ziel** | Ein prüfbarer Schritt einer Etappe (hingehen, lauschen, sammeln, bauen, sprechen, besiegen …) | Welt | „Finde drei Seeglas-Splitter“ |
| **Lied** | Ein alter Text in Strophen, verteilt auf Fragmente. Ein vollständiges Lied verrät etwas (Ort, Bauplan, Questbeginn) | Welt | „Lied der Fjordwiesen“ |
| **Fragment** | Eine Strophe eines Lieds, gefunden an Runensteinen und in Ruinen (`item_saga_fragment_*`) | Welt | Strophe 2 des Fjordwiesen-Lieds |
| **Tat** | Ein Eintrag über etwas Bemerkenswertes. Entsteht **automatisch** aus dem Spiel, nicht nur aus Quests | Welt **oder** Spieler | „Im ersten Winter fiel Ragna bei der Verteidigung des Lagers.“ |

Merksatz: **Stränge bestehen aus Etappen, Etappen aus Zielen. Lieder bestehen aus Fragmenten. Taten schreibt das Spiel selbst.**

<a id="d17-2"></a>

### 17.2 2. Die vier Ebenen der Saga

| Ebene | Inhalt | Fortschritt | Ende |
|---|---|---|---|
| **Hauptsaga** „Der gebrochene Ring“ | Die Geschichte des Gezeitenhalls, in 3 Akten über die 6 Biome, mit den 6 Bossen | gemeinsam für die Welt | Saga-Finale (Sturmkoloss), Jarlssitz |
| **Nebenstränge** | Kürzere Geschichten an Orten und Personen (Dorf-Seherin, Fischerdorf, Schwarzmarkt …) | gemeinsam für die Welt | eigene Belohnung (Freischaltung, Bauplan, Bewohner) |
| **Siedlungs-Saga** | Chronik des eigenen Dorfs: Gründung, erstes Langhaus, erster Bewohner, erster Winter, erstes Fest, Gefallene | automatisch, Welt | nie (wächst mit) |
| **Persönliche Taten** | Was **ein Spieler** getan hat: Gespür-Stufen, erste Funde, Boss-Treffer, verborgene Strophen | automatisch, pro Spieler | nie |

Die **Biom-Stränge** sind Teil der Hauptsaga (je Biom: Lied finden → Steinkreis lauschen → Boss). Wer nur bauen will, kann die Hauptsaga liegen lassen; die Siedlungs-Saga schreibt sich trotzdem.

<a id="d17-3"></a>

### 17.3 3. Hauptsaga: „Der gebrochene Ring“ (Story-Vorschlag)

Kurzfassung: Lange vor den Siedlern band das Küstenvolk den Gezeitenhall in einen **Ring aus Steinkreisen**, einen in jedem Land. Der Ring ist gebrochen – daher das Zeichen: **ein offener Ring mit drei Kerben**. Seitdem steigen und fallen die Gezeiten unruhig, und an den alten Kreisen sammeln sich Menschen, die die Kraft für sich wollen (Plünderer, der Aschebund, der Seekönig). Die Siedler hören das Summen, finden die Lieder der alten Kreise und stellen sich denen, die die Kreise besetzt halten.

| Akt | Biome und Bosse | Kerbe |
|---|---|---|
| Prolog | Startgebiet: das Summen am Strand, der erste Steinkreis, die Raben | – |
| Akt I „Die Flut“ | Fjordwiesen (Der Alte Keiler), Gezeitenmarsch (Die Schlickmutter) | 1. Kerbe leuchtet |
| Akt II „Die Gräber“ | Hügelgrab-Heide (Der Hügelkönig), Basaltfjell (Der Felsenvater) | 2. Kerbe leuchtet |
| Akt III „Der Sturm“ | Glutküste (Die Aschezunge), Eisrand (Der Sturmkoloss) | 3. Kerbe leuchtet, Saga-Finale |

Je Biom läuft dasselbe Muster (gut für Daten und Umfang):
1. **Hinweis:** Raben kreisen über dem neuen Land, der Skalde oder die Seherin erzählt davon.
2. **Lied finden:** 3–5 Fragmente an Runensteinen und Ruinen. Ein Teil davon ist „hallend“ (nur durch Lauschen zu finden).
3. **Steinkreis lauschen:** Das vollständige Lied verrät den Ort des Kreises. Dort aktiv lauschen (Steinkreis +8 EP beim ersten Mal).
4. **Boss:** Der Kreis zeigt die Arena. Sieg → Bossmaterial (nächste Stufe T2–T6), Trophäe, Saga-Bild.

**Realistisch und Sagenwelt:** Die Geschichte funktioniert mit beiden Boss-Fassungen (bestätigt: zuerst Menschen und Tiere, Sagenwesen nur als Weltoption „Sagenwelt“). Der Gezeitenhall bleibt in beiden Fassungen leise; Texte können je Fassung eine eigene Variante haben (`text_sagenwelt`).

**Offen (für später):** Was passiert im Finale mit dem Ring? Er schließt sich (die Kraft wird gebändigt) oder bleibt offen (die Kraft bleibt wild, die Siedler leben mit ihr). Das ist eine erzählerische Entscheidung des Nutzers.

<a id="d17-4"></a>

### 17.4 4. Einbindung des Magie-Gespürs (Lauschen)

#### 4.1 Erfahrung (EP) – keine neuen Quellen
Die Quellen aus `listen.json` reichen und werden nicht vermehrt: **neuer Fund +2, Seeglas +1, Steinkreis +8, Gezeitenhall-Ereignis +15**, je Fund und Spieler nur einmal.
- Ein **hallendes Fragment** zählt als „neuer Fund“ (+2).
- **Quests geben selbst keine Gespür-EP.** Sie führen zu Steinkreisen, Seeglas und Ereignissen; dort entsteht die EP. So gibt es keine doppelte Zählung, und die Rechnung bleibt durchschaubar.
- Die Dorf-Seherin (+25 %) und die Weltoption „Gespür-Tempo“ wirken wie in `listen.json`.

Rechenbeispiel Solo (Schwellen 0 / 10 / 30 / 75 / 160): Prolog mit 3 Seeglas (+3), 2 Funden (+4) und dem ersten Steinkreis (+8) = 15 EP → **Stufe 2 „Ahnung“** am Ende des Prologs. Die Seherin-Quest bringt etwa weitere 15–20 EP → **Stufe 3 „Deutung“** nach etwa 3–4 Stunden. Das passt zur Annahme in `listen.json`.

#### 4.2 Lauschen findet Questziele
- Neuer Ziel-Typ **„lauschend finden“**: Das Ziel ist ein magisches Objekt (Tag `GamerMagic`). Das Saga-Buch nennt nur ein **Gebiet** (Kreis auf der Karte, Text wie „wo die Raben über dem Moor kreisen“), **keinen genauen Marker**. Den Rest erledigt das Lauschen: passiv heiß/kalt am Bildschirmrand, aktiv der türkise Ring.
- **Die Hauptsaga verlangt höchstens Stufe 2.** Niedrige Stufe heißt: länger suchen, nie: festsitzen. So bleibt die Saga für jede Herkunft und für Solo lösbar.
- Höhere Stufen öffnen **Zusätze**, nicht den Weg:
  - **Stufe 3 Deutung:** Name und Menge im Ring; das Saga-Buch zeigt, welche Strophe fehlt.
  - **Stufe 4 Klarblick:** **verborgene Strophen** (Tag `GamerMagicHidden`): jedes Lied hat eine. Sie erzählen, was die anderen verschweigen, und geben einen kleinen Bonus (z. B. Bauplan für Seeglas-Schmuck, ein Saga-Bild, einen Hinweis auf einen Schatz).
  - **Stufe 5 Seidr-Blick:** **seltene Funde** (Tag `GamerMagicRare`): Gezeichnete Orte mit klarem Seeglas für Relikt-Gegenstände.
- **Die Karotte:** Die Boni aus `listen.json` machen höhere Stufen früher erreichbar: Wer Stufe 3 hat, sieht **nachts oder im Nebel am Steinkreis** (+1) schon die verborgene Strophe. Das Spiel sagt das leise an („In klaren Nächten hört man am Kreis mehr.“). Weil Nacht und Nebel wiederkommen, ist das ein Grund wiederzukommen, kein Zeitdruck.

#### 4.3 Die Quest der Dorf-Seherin (`quest_village_seer`)
Nebenstrang **„Die Stimme im Seeglas“**. Schaltet die Bedingung `unlock:quest_village_seer` aus `listen.json` frei.

| # | Etappe | Ziele | Gespür |
|---|---|---|---|
| 0 | Start | nach dem Prolog (erster Steinkreis gefunden) – Raben fliegen zum Opferstein | – |
| 1 | „Die Frau am Opferstein“ | zum Opferstein gehen, mit der Seherin (`npc_seeress`) sprechen | – |
| 2 | „Was das Meer zurücklässt“ | 3 Seeglas-Splitter lauschend finden und ihr bringen | Seeglas +1 je Stück |
| 3 | „Wo der Ring summt“ | am Steinkreis lauschen, **nachts oder im Nebel** (wartet geduldig, bis die Bedingung eintritt; Hinweis im Buch) | Steinkreis +8 (beim zweiten Kreis); Bonus +1 Stufe dort |
| 4 | „Ein Haus für die Seherin“ | Seherhütte (`building_seer_hut`) im Dorf bauen | – |
| 5 | Abschluss | Sie zieht ein und wird Bewohnerin mit Beruf `job_seer` | ab jetzt +25 % EP für alle |

- Die Seherin ist **danach eine Bewohnerin** wie andere (Bedürfnisse, kann sterben). Stirbt sie, gilt die Bedingung `job:job_seer` nicht mehr; eine neue Seherin kommt nur über einen Bewohner, dem man den Beruf gibt (Vorschlag: nur möglich, wenn die Quest einmal abgeschlossen ist).
- Spieler-Herkunft Seherin (`origin_seer`, Start auf Stufe 2) bringt hier nur Bequemlichkeit, keinen anderen Weg.
- Hinweis: Etappe 3 braucht Tag/Nacht oder Wetter. Bis das existiert, kann sie auf „Winter“ oder „ohne Bedingung“ gestellt werden (Daten, kein Code).

#### 4.4 Die drei Kerben
Das Saga-Buch trägt das Zeichen auf dem Deckel: **offener Ring mit drei Kerben**. Jeder abgeschlossene Akt der Hauptsaga lässt **eine Kerbe in Gezeitenlicht** (`#A9E9DC`) leuchten. Das ist für alle Spieler der Welt gleich (Welt-Fortschritt). Die eigene Gespür-Stufe zeigt das Buch getrennt davon (Abschnitt 6), damit „wie weit ist die Welt“ und „wie gut höre ich“ nicht verwechselt werden.

#### 4.5 Gezeitenhall-Ereignis (+15)
Vorschlag: An einem Steinkreis, dessen Lied vollständig ist, tritt **bei Springflut** (alle paar Spieltage, Nacht) ein Ereignis auf: Ringe auf dem Wasser, Nebel in Bögen, ein kurzer Puls. Wer dabei lauscht, bekommt einmalig je Kreis +15. Raben kündigen es einen Spieltag vorher an. Verpassen kostet nichts; es kommt wieder. **Braucht Tag/Nacht und Gezeiten**, also nicht im ersten Schritt.

<a id="d17-5"></a>

### 17.5 5. Koop und Solo (überholt)

> **Überholt am 2026-11-15:** Queststand pro Spieler, Folgen für die Welt addieren sich, Gespräche mit Entscheidungen (siehe Abschnitte 11 und 12 sowie `docs/01_entscheidungen.md`). Die Tabelle bleibt zum Nachlesen.

| Frage | Regel (Vorschlag) |
|---|---|
| Wem gehört der Quest-Fortschritt? | **Der Welt.** Jeder Spieler kann jedes Ziel erfüllen; alle sehen denselben Stand. Wer später beitritt, sieht den Fortschritt, den die anderen schon gemacht haben |
| Wer bekommt die Belohnung? | **Welt-Belohnungen** (Freischaltungen, Baupläne, Bewohner, Gegenstände) gehen an die Siedlung, Gegenstände ins **Lager**. Kein Streit, wer den letzten Schlag hatte |
| Und was bekommt der Einzelne? | **Taten** (Saga-Buch, unter seinem Namen) und **Gespür-EP** aus den Funden, wie in `listen.json` (gemeinsamer Fund: Mitspieler im Umkreis von 30 m bekommen die Hälfte) |
| Fragmente und Questgegenstände | gehen beim Aufnehmen **direkt ins Saga-Buch der Welt**, nicht ins Inventar: kein Gewicht, kein Verlust beim Tod, kein „der Freund mit dem Logbuch ist offline“ |
| Gespräche | Jeder Spieler kann mit Quest-Personen sprechen. Im ersten Schritt **keine sich ausschließenden Entscheidungen** in Gesprächen (sonst müsste man klären, wer für die Gruppe entscheidet) |
| Koop-Bonus | nur Bequemlichkeit: gemeinsames Lauschen (größere Reichweite, `group_range_mul`), gemeinsamer Fund. Keine Quest verlangt zwei Spieler |
| Zeit | keine Fristen. Bedingungen wie „nachts“ sind Fenster, die wiederkommen. Kein Ziel kann scheitern |

<a id="d17-6"></a>

### 17.6 6. Das Saga-Buch (`ui_saga`)

Öffnen mit eigener Taste (Vorschlag J, umbelegbar). Pergament-Seiten nach der Stilvorgabe, Zeichen auf dem Deckel. Fünf Teile:

1. **Saga:** aktive Stränge mit der aktuellen Etappe und ihren Zielen. Ziele als kurze, erzählende Sätze; Gebiet auf der Karte als Kreis. Abgeschlossene Etappen als kurzer Rückblick.
2. **Lieder:** jedes Lied mit seinen Strophen; fehlende Strophen als Lücke „…“. Ab Stufe 3 steht dabei, wie die fehlende zu finden ist („hallend“, „verborgen“).
3. **Siedlung:** Chronik des Dorfs mit Jahr und Jahreszeit („Im zweiten Winter …“). Gefallene Bewohner bekommen **immer** einen Eintrag (Name, Tat, Zeitpunkt). Zusätzlich kann man einen **Gedenkstein** (`building_runestone`) bauen, der auf den Eintrag verweist. (Bestätigt ist „Saga-Eintrag oder Gedenkstein“; Vorschlag hier: Eintrag immer, Stein freiwillig.)
4. **Taten:** je Spieler eine Seite mit seinen Taten.
5. **Gespür:** eigene Stufe (Name, Balken bis zur nächsten Stufe), gerade wirkende Boni (Amulett, Nacht, Dorf-Seherin …), die drei Kerben des Welt-Fortschritts.

Der **Skalde** (`npc_skald`, später ein Bewohner-Beruf an der Taverne) liest bei Festen Einträge aus der Siedlungs-Saga vor. Vorschlag für später: Das gibt Stimmung, keine neue Währung.

<a id="d17-7"></a>

### 17.7 7. Datenformat (Entwurf, angepasst an die Antworten vom 2026-11-15)

Nach `docs/06_datenformat.md`: eine JSON-Datei pro Tabelle in `game/Content/Data/`, Aufbau `{ "schema", "rows" }` (Einstellungsdateien wie `listen.json` als einzelnes Objekt), IDs `snake_case`, nie wiederverwenden, Wegfallendes bekommt `"deprecated": true`.
Grundlage sind die bestätigten Punkte:
- alles spawnt nach;
- Bossbeute: beim ersten Sieg sicher, danach mit Chance je Stufe;
- Gespräche mit Entscheidungen, große Folgen in der Hauptstory;
- Folgen für die Welt addieren sich;
- Questhilfe über Gespräche, im Menü abschaltbar;
- alles vertont (Deutsch und Englisch).

Voraussetzung dafür ist der Queststand pro Spieler. Er ist noch Vorschlag, wird aber von den anderen Antworten vorausgesetzt.

#### 7.0 Übersicht der Dateien

| Datei | Inhalt | Präfix |
|---|---|---|
| `saga.json` | Einstellungen (ein Objekt): Radien, Nachspawn-Zeiten, Bossbeute-Chancen | – |
| `saga_lines.json` | Stränge (Hauptstory, Nebenstränge) und ihre Wege | `saga_`, `path_` |
| `quests.json` | Etappen mit Zielen, Belohnungen und Folgen | `quest_` |
| `dialogues.json` | Gespräche: Knoten, Antworten, Entscheidungen, Hilfen | `dlg_`, `choice_` |
| `lore.json` | Schriftstücke: Logbuchseiten, Lied-Strophen, Inschriften | `lore_` (Lied-Strophen behalten `item_saga_fragment_*`) |
| `visions.json` | Nebelbilder (Seidr-Visionen beim Lauschen) | `vision_` |
| `deeds.json` | Taten (Siedlungs-Chronik und persönliche Taten) | `deed_` |
| `lines.json` | jede gesprochene oder angezeigte Zeile: Text und Ton je Sprache | `line_` |
| `speakers.json` | Sprecher: Stimme je Sprache, KI oder Sprecher | `spk_` |
| Gegner-Tabelle (Phase 3) | Nachspawnen und Bossbeute je Gegner | `enemy_`, `boss_` |

Merksatz: **Jeder Text im Spiel ist eine `line_…`.** Quests, Gespräche, Logbuch und Saga-Buch verweisen nur auf Zeilen. Dadurch hängen Übersetzung, Untertitel und Vertonung an einer Stelle, und jede Zeile weiß, ob ihr Ton noch eine KI-Stimme ist.

#### 7.1 `saga.json` – Einstellungen
```json
{ "schema": "saga_1",
  "share_radius_m": 50,
  "quest_enemy_respawn_days": 1,
  "boss_respawn_days": 3,
  "boss_repeat_drop_chance": { "2": 0.5, "3": 0.4, "4": 0.3, "5": 0.2, "6": 0.1 },
  "boss_pity_after": 4,
  "thoughts_default_on": true,
  "quest_help_default_on": true }
```
Alle Zahlen sind bestätigte Startwerte (2026-11-15). Die beiden `*_default_on` sind Vorschlag.

#### 7.2 Bedingungen
Gleiche Schreibweise wie in `listen.json` (`art:wert`), ein Prüfbaustein für alles. Neu ist: **Jede Bedingung gilt entweder für den Spieler oder für die Welt.**

| Bedingung | gilt für | Bedeutung |
|---|---|---|
| `quest_done:<id>` | Spieler | Etappe in seinem Queststand abgeschlossen |
| `quest_active:<id>` | Spieler | Etappe läuft bei ihm |
| `choice:<choice_id>=<option>` | Spieler | er hat so entschieden |
| `path:<path_id>` | Spieler | er geht diesen Weg (z. B. `path_circle`, `path_farm`) |
| `lore:<lore_id>` | Spieler | Schriftstück gefunden |
| `song:<song>` | Spieler | alle nötigen Strophen eines Lieds gefunden |
| `listen_level:<n>` | Spieler | Gespür-Stufe mindestens n (inkl. Boni) |
| `setting:<name>` | Spieler | persönliche Menü-Einstellung (z. B. `setting:quest_help`) |
| `unlock:<id>` | Welt | Freischaltung gesetzt. Etappen mit `"scope": "world"` setzen beim Abschluss `unlock:<quest_id>` (daher `unlock:quest_village_seer` in `listen.json`) |
| `world_choice:<choice_id>=<option>` | Welt | **mindestens ein** Spieler hat so entschieden (Folgen addieren sich) |
| `deed:<deed_id>` | Welt | Welt-Tat geschehen |
| `job:<id>`, `building:<id>` | Welt | wie bisher |
| `season:`, `night`, `fog`, `place:` | Welt/Ort | wie in `listen.json` |
| `option:<name>` | Welt | Weltoption aktiv (z. B. `option:hordes`, `option:sagenwelt`) |
| `stance:<hueter\|rufer>` | Spieler | Haltung aus Kapitel 3 (bestätigt 2026-11-16); vor Kapitel 3 nicht gesetzt |
| `tide:<low\|high>` | Welt/Ort | Ebbe oder Flut (bestätigt 2026-11-21: zweimal pro Spieltag) |
| `mode:<saga\|free>` | Welt | Spielmodus; im Freien Spiel startet kein `main`-Strang |

#### 7.3 `saga_lines.json` – Stränge und Wege
```json
{ "schema": 1, "rows": [
  { "id": "saga_first_ones", "name_line": "line_saga_first_ones_title", "kind": "main",
    "quests": ["quest_fo_beach", "quest_fo_farm", "quest_fo_hum", "quest_fo_pages", "quest_fo_graves"],
    "paths": [
      { "id": "path_circle", "requires": ["choice:choice_empty_stone=ring"],
        "quests": ["quest_fo_circle_1", "quest_fo_circle_2", "quest_fo_circle_3"], "rejoin": "quest_fo_meeting" },
      { "id": "path_farm", "requires": ["choice:choice_empty_stone=name"],
        "quests": ["quest_fo_farm_1", "quest_fo_farm_2", "quest_fo_farm_3"], "rejoin": "quest_fo_meeting" }
    ],
    "after_rejoin": ["quest_fo_meeting"] },
  { "id": "saga_seer_voice", "name_line": "line_saga_seer_title", "kind": "side",
    "start": ["quest_done:quest_fo_graves"],
    "quests": ["quest_seer_meet", "quest_seer_glass", "quest_seer_circle", "quest_seer_hut", "quest_village_seer"] }
] }
```
- `kind`:
  - `main`: große Folgen erlaubt;
  - `side`: mäßige Folgen;
  - `settlement`: nur Taten.
- `paths` gibt es nur in der Hauptstory: getrennte Kapitel nach einer großen Entscheidung. `rejoin` ist die Etappe, an der sie wieder zusammenlaufen (bestätigt: 2–3 Kapitel getrennt).

#### 7.4 `quests.json` – Etappen
| Feld | Pflicht | Bedeutung |
|---|:-:|---|
| `id` | ✓ | `quest_…` |
| `line` | ✓ | `saga_…` |
| `name_line` | ✓ | Titel (Zeile) |
| `scope` | ✓ | `player`: jeder erlebt die Etappe selbst (Standard). `world`: verändert die Welt und kommt nur einmal vor; wer sie später erreicht, bekommt sie als „davon gehört“ angerechnet |
| `start` | | Bedingungen; fehlt es, beginnt die Etappe nach der vorigen im Strang |
| `hint_line` | ✓ | erzählender Satz im Saga-Buch |
| `area` | | Gebiet auf der Karte: `{ "place": "poi_…", "radius_m": 120 }` |
| `area_help` | | kleineres Gebiet, das eine Gesprächshilfe freischaltet: `{ "radius_m": 30 }` (nur mit `setting:quest_help`) |
| `objectives` | ✓ | Ziele (Tabelle unten), `"order": "any"` oder `"sequence"` |
| `rewards` | | für den Spieler: `{ "items": [...], "deed": "deed_…", "lore": [...] }` |
| `world_effects` | | für die Welt, addieren sich: `{ "unlock": [...], "spawn_npcs": [...], "villagers_join": [...], "blueprints": [...] }` |
| `saga_line` | ✓ | Eintrag im Saga-Buch nach Abschluss |

**Gemeinsame Gutschrift:** Ein erfülltes Ziel zählt für alle Spieler im Umkreis von `share_radius_m` (50 m), die dieselbe Etappe aktiv haben.

Ziel-Typen (`objectives[].type`):

| Typ | Felder | Erfüllt, wenn … |
|---|---|---|
| `go_to` | `place`, `radius_m` | der Spieler dort ist |
| `listen_find` | `target` (Tag oder `kind:`), `count` | er die Ziele beim Lauschen aufgedeckt und aufgenommen hat |
| `listen_at` | `place`, `when`, `when_mode` | er dort aktiv lauscht, während `when` gilt |
| `vision` | `vision` | er das Nebelbild gesehen hat |
| `read` | `lore` | er das Schriftstück gefunden hat |
| `collect` | `item`, `count`, `deliver_to` (`npc_…` / `storage`) | übergeben bzw. im Lager |
| `talk` | `dialogue` | er das Gespräch bis zu einem Ende geführt hat |
| `choose` | `choice` | er die Entscheidung getroffen hat (egal wie) |
| `build` | `buildable` | Gebäude steht (Welt; zählt für jeden, der die Etappe aktiv hat) |
| `defeat` | `enemy` / `boss` | besiegt (mit Gutschrift im Umkreis) |
| `assign_job` | `job` | ein Bewohner hat den Beruf (Welt) |
| `puzzle` | `puzzle` (ID eines Rätsels, z. B. Steine in richtiger Reihenfolge legen) | das Rätsel gelöst ist (keine Uhr, beliebig viele Versuche; im Koop zählt für alle in der Nähe, bestätigt 2026-11-21) |

Questgegner und Questgegenstände werden **pro Spieler gezeigt**: Hat ein Spieler sie schon, sieht er sie nicht mehr; wer sie noch braucht, sieht sein eigenes Exemplar. Questgegner spawnen nach `quest_enemy_respawn_days` nach.

#### 7.5 `dialogues.json` – Gespräche, Entscheidungen, Hilfen
```json
{ "schema": 1,
  "choices": [
    { "id": "choice_empty_stone", "weight": "major", "options": ["ring", "name"] },
    { "id": "choice_graves_axe", "weight": "minor", "options": ["leave", "take"] },
    { "id": "choice_seer_trust", "weight": "minor", "options": ["help", "refuse"] }
  ],
  "rows": [
  { "id": "dlg_seer_first", "npc": "npc_seeress", "start": ["quest_active:quest_seer_meet"],
    "nodes": [
      { "id": "n1", "line": "line_seer_first_01",
        "options": [
          { "line": "line_seer_first_ask_where", "next": "n_help", "requires": ["setting:quest_help"] },
          { "line": "line_seer_first_help", "next": "n_yes", "choice": "choice_seer_trust=help" },
          { "line": "line_seer_first_refuse", "next": "n_no", "choice": "choice_seer_trust=refuse" } ] },
      { "id": "n_help", "line": "line_seer_first_where", "effects": { "quest_help": "quest_seer_glass" }, "next": "n1" },
      { "id": "n_yes", "line": "line_seer_first_yes", "end": true },
      { "id": "n_no", "line": "line_seer_first_no", "end": true }
    ] }
] }
```
- **Entscheidung:** Eine Antwort mit `choice` speichert die Wahl im Queststand des Spielers. `weight` legt das Gewicht fest:
  - `major`: nur in `main`-Strängen, große Folgen;
  - `minor`: überall, mäßige Folgen.

  Entscheidungen ohne Gespräch, etwa am leeren Stein, laufen über denselben Block: Das Spiel bietet die Optionen als Handlung an.
- **Hilfe:** Antworten mit `requires: ["setting:quest_help"]` erscheinen nur, wenn die Questhilfe im Menü an ist. Ihr Effekt `quest_help` schaltet für diesen Spieler `area_help` frei. Beim Ausschalten im Menü erklärt ein Hinweis, dass diese Fragen dann fehlen.
- **Koop:** Jeder spricht für sich (eigener Queststand). Mitspieler in der Nähe hören die vertonten Zeilen mit, aber nur der Sprechende wählt.
- **Gemeinsame Entscheidung** (bestätigt 2026-11-21): Eine Entscheidung mit `"group": true` und `"fallback": "<option>"` öffnet sich für alle Spieler im Umkreis gleichzeitig; jeder wählt, keine Uhr. Einig: das gilt. Uneinig: `fallback` (die umkehrbare Option). Gebraucht für Entscheidungen, die die Welt nur auf eine Art verändern können (z. B. `choice_mud_mother` mit `fallback: "capture"`).

#### 7.6 `lore.json` – Schriftstücke
```json
{ "schema": 1, "rows": [
  { "id": "lore_asgerd_01", "kind": "journal_page", "group": "journal_asgerd", "order": 1,
    "found": "plain", "place": "poi_first_farm", "line": "line_lore_asgerd_01", "handwriting": "asgerd" },
  { "id": "item_saga_fragment_fjord_2", "kind": "song_fragment", "group": "song_fjord", "order": 2,
    "found": "listen", "line": "line_song_fjord_2" }
] }
```
- `kind`: `journal_page`, `song_fragment`, `inscription`.
- `found`:
  - `plain`: sichtbar;
  - `listen`: hallend, Tag `GamerMagic`, zählt als neuer Fund +2;
  - `hidden`: ab Stufe 4;
  - `rare`: ab Stufe 5.
- Schriftstücke gehen beim Aufnehmen in das Saga-Buch des Spielers, nicht ins Inventar. Andere Questgegenstände sind normale Gegenstände aus `items.json` (Leinsamen, Seeglas, Hallbjörns Ring).
- `handwriting` wählt die Schrift der Handschrift-Ansicht; daneben steht immer der lesbare Drucktext.

#### 7.7 `visions.json` – Nebelbilder
```json
{ "schema": 1, "rows": [
  { "id": "vision_empty_stone", "place": "poi_graves_first", "radius_m": 6,
    "when": [], "min_listen_level": 1, "duration_s": 14, "once_per_player": false,
    "figures": [ { "anim": "vis_hallbjorn_leaves", "look": "ghost_male" },
                 { "anim": "vis_asgerd_stays", "look": "ghost_female" } ],
    "lines": ["line_vis_empty_stone_sfx"], "reveals": "item_ring_hallbjorn",
    "thought": "line_thought_empty_stone" }
] }
```
Nebelbilder lassen sich wiederholen (`once_per_player: false`). Die Folge (z. B. den Ring freilegen) gibt es pro Spieler einmal. Ob die Gestalten Worte sprechen oder nur Laute, ist offen (siehe `docs/01_entscheidungen.md`, „Vertonung“).

#### 7.8 `lines.json` und `speakers.json` – Text und Vertonung
```json
{ "schema": 1, "rows": [
  { "id": "line_lore_asgerd_01", "speaker": "spk_asgerd",
    "text": { "de": "Dritter Tag an Land. Wir sind siebzehn …", "en": "Third day ashore. We are seventeen …" },
    "audio": { "de": "/Game/Audio/VO/de/lore_asgerd_01", "en": "/Game/Audio/VO/en/lore_asgerd_01" },
    "voice_source": { "de": "ai", "en": "ai" },
    "vars": [] }
] }
```
```json
{ "schema": 1, "rows": [
  { "id": "spk_asgerd", "name": "Asgerd", "voice": { "de": "…", "en": "…" } },
  { "id": "spk_narrator", "name": "Skalde (Erzähler)", "voice": { "de": "…", "en": "…" } },
  { "id": "spk_player", "name": "Spielerfigur", "voice_by_choice": true }
] }
```
- `voice_source` (`ai` / `actor`) zeigt je Sprache, welche Zeilen noch eine KI-Stimme haben. Daraus entstehen die Liste für die spätere Neuaufnahme mit Sprechern und die Angaben für Asset-Log und Steam.
- Zeilen mit Platzhaltern (`{player}`, `{villager}`) werden nicht mit Namen vertont. Der Erzähler sagt dann „sie“ oder umschreibt. Das betrifft vor allem Taten und Gefallene. (Alternative: jeden Namen aus der Namensliste einzeln aufnehmen; teuer.)
- Fehlt der Ton einer Sprache, zeigt das Spiel nur den Untertitel. Untertitel sind immer verfügbar (Lesbarkeit bestätigt).

#### 7.9 Gegner-Tabelle (Phase 3): Nachspawnen und Bossbeute
```json
{ "id": "boss_old_boar", "tier": 2, "respawn_days": 3,
  "first_kill_drops": [ { "item": "item_trophy_boss_old_boar", "count": 1 }, { "item": "item_boar_tusk_old", "count": 2 } ],
  "drops": [ { "item": "item_meat_raw", "min": 4, "max": 6 }, { "item": "item_hide", "min": 2, "max": 3 } ],
  "repeat_boss_drops": [ { "item": "item_boar_tusk_old", "count": 1 } ] }
```
- **Erster Sieg pro Spieler:** `first_kill_drops` + `drops`.
- **Jeder weitere Sieg:** `drops`, dazu `repeat_boss_drops` mit der Chance aus `saga.json` je Stufe. Nach `boss_pity_after` (4) Siegen ohne Bossmaterial gibt es beim nächsten sicher welches.
- **Koop:** Jeder Beteiligte im Umkreis bekommt eigene Beute nach seinem eigenen Stand (erster oder wiederholter Sieg).

#### 7.10 Spielstand
Im Weltspielstand (`world.json`) werden zwei Bereiche gespeichert:
- **Welt:**
  - gesetzte Freischaltungen;
  - Welt-Entscheidungen (`world_choice`: welche Optionen mindestens einmal gewählt wurden);
  - abgeschlossene Etappen mit `scope: world`;
  - Welt-Taten mit Zeitpunkt;
  - Zeitpunkte fürs Nachspawnen (Bosse, Questgegner).
- **Pro Spieler** (Spieler-ID):
  - Zustand jeder Etappe (`locked` / `active` / `done` / `heard` = „davon gehört“) mit Ziel-Fortschritt;
  - Entscheidungen und Weg (`path_…`);
  - gefundene Schriftstücke und gesehene Nebelbilder;
  - freigeschaltete Gesprächshilfen;
  - persönliche Taten;
  - Gespür-EP und schon gezählte Funde;
  - Bosse mit erstem Sieg und Pech-Zähler je Boss.

Der Server prüft alle Ziele und Bedingungen (Regel 1). Spieler schicken nur Anfragen: sprechen, Antwort wählen, aufnehmen, lauschen.
Der Queststand bleibt immer pro Welt, auch bei der Weltoption „Inventar wandert mit dem Charakter“ (bestätigt 2026-11-15).

<a id="d17-8"></a>

### 17.8 8. Umfang und Reihenfolge (Vorschlag)

| Schritt | Inhalt | Braucht |
|---|---|---|
| 1 | Quest-Kern: Daten lesen, Bedingungen, Ziel-Typen `go_to`, `listen_find`, `collect`, `build`, `talk`; Saga-Buch Teile Saga, Gespür; speichern | Lauschen (in Arbeit), Speichersystem |
| 2 | Prolog + „Die Stimme im Seeglas“ (5 Etappen), Seherhütte, Beruf `job_seer`; Siedlungs-Taten (erster Bewohner, erster Winter, Gefallene) | erste magische Objekte (Seeglas, Steinkreis), `npc_seeress` als Figur |
| 3 | Lieder und Fragmente, Teil Lieder im Buch, verborgene Strophen | Runensteine/Ruinen als Orte |
| 4 | Hauptsaga Akt I (2 Biome, 2 Bosse) | Phase 3 (Kampf) und Phase 4 (Biome) |
| später | Gezeitenhall-Ereignis, Skalde, Akte II–III, Finale, Jarlssitz | Tag/Nacht, Gezeiten, Wetter |

Ehrlich: Schritt 1–2 sind in einigen Arbeitstagen machbar, sobald es magische Objekte gibt. Die Hauptsaga ist an Kampf, Biome und Bosse gebunden und damit Phase 4. Jede Etappe braucht Text; der Text ist mehr Arbeit, als man denkt (Vorschlag: Claude schreibt Entwürfe, der Nutzer liest gegen).

<a id="d17-9"></a>

### 17.9 9. Offene Fragen (überholt)

> **Überholt am 2026-11-15:** beantwortet in Abschnitt 12 und `docs/01_entscheidungen.md`. Noch offen sind Frage 6 (Finale) und Frage 8 (Saga-Buch-Taste).

| # | Frage | Vorschlag von Claude |
|---|---|---|
| 1 | Grundstruktur (4 Ebenen, Fortschritt gehört der Welt) | wie Abschnitt 2 und 5 |
| 2 | Hauptsaga-Idee „Der gebrochene Ring“ (Steinkreise je Biom, 3 Akte = 3 Kerben) | wie Abschnitt 3 |
| 3 | Wie genau zeigen Quests ihr Ziel? | nur Gebiet + Lauschen; Weltoption „Questhilfe“ mit genauem Marker für alle, die das wollen |
| 4 | Fragmente direkt ins Saga-Buch statt ins Inventar | ja |
| 5 | Entscheidungen in Gesprächen | im ersten Schritt keine |
| 6 | Finale: schließt sich der Ring oder bleibt er offen? | später |
| 7 | Gefallene: Eintrag immer, Gedenkstein freiwillig | ja |
| 8 | Saga-Buch-Taste | J |

---

<a id="d17-10"></a>

### 17.10 10. Fünf Storyline- und Saga-Vorschläge

Grundlage für alle fünf Vorschläge:
- **Roter Faden statt Story-Spiel.** Keine Pflicht-Zwischensequenzen, keine langen Dialoge (bestätigt: Text mit Gesten, keine Sprachausgabe). Jeder Strang lässt sich jederzeit liegen lassen.
- **Questgegenstände sind unauffällig.** Es gibt kein eigenes Questinventar und kein Leuchten. Ein Questgegenstand ist ein normaler Gegenstand mit Alltagsnutzen (Seeglas, Logbuch, Runenabrieb, Saatgut) oder landet als Fundstück im Saga-Buch. Höchstens eine kleine Rune im Tooltip zeigt: „gehört zu einer Geschichte“.
- **Horden sind bisher nur eine Weltoption.** In `docs/01_entscheidungen.md` steht: Weltoptionen „freies Bauen, Horden, Survivor, kombinierbar“ (bestätigt), Basisangriffe „optional per Menü“ (bestätigt), „Survivor“ offen. Regel 6 lautet: kein Zeitdruck als Kernschleife. Deshalb ist in allen Vorschlägen keine Story-Etappe an Horden gebunden. Die Horden geben der Story nur einen Grund und eine Farbe. ⚑ Der Nutzer schreibt jetzt: „es wird auch ein Horden-Survive-Element geben“. Ob Horden damit **standardmäßig an** sind (abschaltbar) oder weiter **nur auf Wunsch**, ist offen.
- Die Gespür-Werte aus `listen.json` bleiben unverändert.

#### A „Der gebrochene Ring“: eine Kette alter Orte

**Roter Faden:** Ein altes Volk band den Gezeitenhall in einen Ring aus Steinkreisen, einen in jedem Land. Der Ring ist gebrochen, die Gezeiten sind unruhig. Pro Biom findet man das Lied des Kreises, lauscht dort und stellt sich dem, der den Ort besetzt hält (Boss). Drei Akte füllen die drei Kerben des Zeichens.
- **Spürbare Story:** mittel. Es gibt eine erzählte Hauptlinie mit Akten, aber kaum Personen.
- **Saga-System:** Abschnitte 1–9 dieses Dokuments: Stränge, Etappen, Lieder aus Fragmenten, Kerben als Welt-Fortschritt.
- **Nebenquests:** Lied-Fragmente sammeln, Seeglas finden, verborgene Strophen (ab Stufe 4), die Dorf-Seherin, Ruinen erkunden.
- **Questgegenstände:** Fragmente (direkt ins Buch), Seeglas (auch Handwerksstoff), Bossmaterial.
- **Magie und Lauschen:** zentral. Lauschen ist der Weg zu jedem Kreis.
- **Horden:** Unruhige Kreise ziehen in Flutnächten Plünderer an. Ein beruhigter Kreis macht das Land drumherum ruhiger. Mit Horden aus passiert stattdessen nichts.
- **Solo/Koop:** gut. Alles lässt sich allein lösen.
- **Vorteile:** klare Struktur, passt genau zu Biomen, Bossen und Stil, gut als Daten.
- **Nachteile:** am meisten „Story“ der fünf. Das Muster wiederholt sich in jedem Biom. Die Hauptlinie hängt an Phase 3 und 4 (Kampf, Biome).

#### B „Die Chronik der Siedlung“: das Dorf ist die Geschichte

**Roter Faden:** Es gibt keine vorgeschriebene Handlung. Der Faden ist der Weg der Siedlung, vom Lagerfeuer über Hof, Dorf und Marktflecken bis zum Jarlssitz. Jede Ausbaustufe hat ein paar „Bewährungen“: erster Winter, erstes Fest, erster Händler, erste Verteidigung. Danach wächst der Ruf der Siedlung und neue Leute kommen mit eigenen Bitten.
- **Spürbare Story:** gering. Die Geschichte entsteht aus dem eigenen Spiel, der Skalde liest sie am Fest vor.
- **Saga-System:** Die Siedlungs-Saga ist das Herz. Taten werden automatisch geschrieben, Gefallene bekommen Einträge und Gedenksteine. Die „Stufen der Siedlung“ ersetzen Akte, die Kerben stehen für Hof, Dorf und Marktflecken.
- **Nebenquests:** **Bitten der Bewohner** (eine Bäuerin will Flachssaat, der Koch ein Gericht der Stufe 2, ein Neuer braucht ein Bett), Händleraufträge, Erkundungsaufträge („bring mir Kunde vom Moor“). Die Seherin kommt als Zugezogene mit ihrer Bitte (`quest_village_seer`).
- **Questgegenstände:** fast nur normale Waren (Essen, Saat, Felle). Einzelne Erbstücke der Bewohner landen im Buch.
- **Magie und Lauschen:** Nebensache. Die Seherin bringt Magie ins Dorf. Magie-Bitten (Seeglas für ein Amulett, Lauschen am Hain) sind eine eigene Art Nebenquest.
- **Horden:** passt am besten. Angriffe sind „Bewährungen“ der Siedlung, Gefallene werden zur Chronik. Ohne Horden zählen andere Bewährungen (harter Winter, Fest).
- **Solo/Koop:** sehr gut. Die Siedlung gehört allen.
- **Vorteile:** am nächsten an Aska und Medieval Dynasty, wenig Text, viele Quests lassen sich aus Bausteinen erzeugen, nutzt Bewohner, Taverne, Feldbau und Jahreszeiten.
- **Nachteile:** Es fehlt ein Geheimnis, das in die Welt hinauszieht. Erkunden und Magie bekommen wenig Grund. Erzeugte Bitten können sich schnell wiederholen.

#### C „Die Spur der Ersten“: Erkunden einer verschwundenen Siedlung

**Roter Faden:** Vor euch kam schon eine Siedlergruppe an diese Küste, „die Ersten“. Sie ist verschwunden. Ihre Spuren liegen überall: verlassene Höfe, ein Logbuch, Vorratsgruben, Grabsteine, Markierungen an Bäumen. Raben führen zu den Stellen. Stück für Stück zeigt sich, was ihnen geschah: Sie fanden den Gezeitenhall, gruben zu tief, die Plünderer kamen, und ein Teil von ihnen lebt noch im Norden.
- **Spürbare Story:** gering bis mittel. Erzählt wird fast nur über Funde und Orte (wie in The Forest), kaum über Gespräche. Wer nicht liest, merkt kaum etwas, wer liest, bekommt ein Rätsel.
- **Saga-System:** Das Saga-Buch ist ein **Fundbuch**: Seiten des Logbuchs, Orte der Ersten auf der Karte, offene Fragen („Wohin ging Thorgrim?“). Etappen sind Fundorte statt Aufträge. Daneben läuft die Siedlungs-Chronik (aus B) automatisch.
- **Nebenquests:** verlassene Orte durchsuchen, Vorratsverstecke der Ersten (Saatgut, Werkzeug, Baupläne), ihre ersten Magie-Versuche nachmachen (eine Rune, die sie ritzten, selbst probieren), ihre Gräber finden und Gedenksteine setzen, Überlebende im Norden.
- **Questgegenstände:** Logbuchseiten (Buch), dazu **nützliche Funde**: alte Saat (Feldbau), Bauplan Palisade, rostiges Werkzeug zum Reparieren. Der Gegenstand ist zugleich Belohnung.
- **Magie und Lauschen:** wichtig, aber nicht allein. Manche Spuren sind „hallend“ (nur durch Lauschen), die Seherin der Ersten hinterließ Seeglas-Zeichen. Die Dorf-Seherin kann eine Nachfahrin sein.
- **Horden:** Die Ersten wurden überrannt. Ihre Ruinen zeigen, wie (verbrannte Palisade, Fluchtweg) und geben Baupläne zur Verteidigung. Mit Horden ist das eine Warnung, ohne Horden bleibt es Geschichte.
- **Solo/Koop:** sehr gut. Fundorte sind für alle gleich, jeder kann lesen.
- **Vorteile:** Erkunden steht vorn, Story bleibt leise und freiwillig, Questgegenstände haben Alltagsnutzen, verbindet Magie, Feldbau, Horden und Gedenksteine. Funktioniert schon im ersten Biom, ohne Bosse.
- **Nachteile:** Umweltgeschichten brauchen gut gebaute Orte (Level-Design-Aufwand). Das Rätsel ist einmal gelöst und dann vorbei (Wiederspielwert über neue Welten gering).

#### D „Die fünf Wege“: eine kleine Saga pro Herkunft

**Roter Faden:** Jede Herkunft trägt eine eigene kurze Geschichte, 4–6 Etappen:
- **Jägerin:** der weiße Hirsch, der nur im Winter erscheint;
- **Krieger:** ein gebrochener Eid und der Jarl, der ihn verlangte;
- **Schmied:** das Geheimnis des Seeglas-Stahls;
- **Bäuerin:** die alte Saat, die in jedem Klima wächst;
- **Seherin:** die Stimme im Seeglas.

Alle fünf führen am Ende zum selben Ort (Steinkreis oder Jarlssitz), wo sie sich treffen.
- **Spürbare Story:** mittel, aber persönlich und kurz.
- **Saga-System:** **Fortschritt pro Spieler** (das ist hier zwingend). Die Welt hat nur die Siedlungs-Chronik. In Koop helfen Mitspieler bei fremden Wegen und bekommen eine Tat dafür.
- **Nebenquests:** je Herkunft passend (Jagd, Kampf, Schmieden, Feldbau, Magie). Auch andere Herkünfte können einen Weg später übernehmen, passend zu „jeder kann alles lernen“.
- **Questgegenstände:** Herkunftsgegenstände (Hirschgeweih, Eidring, Seeglas-Barren, Ursaat, Seherstab), alle mit Nutzen.
- **Magie und Lauschen:** stark beim Seherin- und Schmied-Weg, schwach bei den anderen.
- **Horden:** im Krieger-Weg (der Jarl schickt Leute), sonst kaum.
- **Solo/Koop:** Solo bekommt man nur einen Weg zu sehen (es sei denn, man übernimmt weitere). Koop ist reicher.
- **Vorteile:** macht die Herkunftswahl bedeutsam, jede Runde mit anderer Herkunft fühlt sich anders an.
- **Nachteile:** fünfmal Inhalt (teuer). Der Fortschritt pro Spieler braucht die Regeln aus der Frage WoW/Mischung. Die Welt selbst hat keinen gemeinsamen Faden.

#### E „Das Jahresrad“: Geschichte im Takt der Jahreszeiten

**Roter Faden:** Die Geschichte folgt den Jahreszeiten (eine Stunde Echtzeit je Jahreszeit, bestätigt). Jedes Jahr ist ein Kapitel mit einem Fest:
- **Frühling:** Aussaat;
- **Mittsommer:** Gezeitenhall am stärksten;
- **Herbst:** Ernte und Opfer;
- **Winter:** Julnächte.

An jedem Fest zeigt sich ein Zeichen, das in die Welt hinausführt (ein Rabe, eine Flut, ein Fremder). Ein Kapitel wartet, bis man so weit ist. Nichts geht verloren, das Fest kommt nächstes Jahr wieder.
- **Spürbare Story:** gering bis mittel, als Rhythmus spürbar.
- **Saga-System:** Das Saga-Buch ist ein **Jahrbuch**: Seiten je Jahr und Jahreszeit, mit Taten, Ernte, Festen und Gefallenen. Kapitel entsprechen Jahren.
- **Nebenquests:** Jahreszeiten-Aufgaben (Frühling: wilde Saat finden; Sommer: Seeglas bei Niedrigwasser; Herbst: Vorräte für den Winter; Winter: Spuren im Schnee lesen), Feste vorbereiten (Taverne, Gerichte).
- **Questgegenstände:** Festgaben (Gerichte, Met, Felle), alles normale Waren.
- **Magie und Lauschen:** an Zeiten gebunden, passend zu den Gespür-Boni (Winter, Nacht, Nebel +1). Am Mittsommer ist der Gezeitenhall-Fund am größten.
- **Horden:** Wintereinfälle (Plünderer, Wölfe) als Bedrohung zur Jahreszeit, nur mit der Weltoption.
- **Solo/Koop:** gut.
- **Vorteile:** nutzt Jahreszeiten, Feldbau, Taverne und Gespür-Boni stark. Der Rhythmus passt zu einem Überlebensspiel.
- **Nachteile:** Warten auf die nächste Jahreszeit kann sich wie Zeitdruck oder Leerlauf anfühlen, im Widerspruch zum Sinn von Regel 6. Mit 4 Stunden pro Jahr ist ein Kapitel lang. Hängt an Tag/Nacht und Festen, die es noch nicht gibt.

#### Vergleich

| | A Ring | B Chronik | C Spur der Ersten | D Fünf Wege | E Jahresrad |
|---|---|---|---|---|---|
| Spürbare Story | mittel | gering | gering–mittel | mittel | gering–mittel |
| Erkunden als Antrieb | hoch | gering | **sehr hoch** | mittel | mittel |
| Überleben/Siedlung | gering | **sehr hoch** | mittel | gering | hoch |
| Magie/Lauschen | **sehr hoch** | gering | hoch | gemischt | hoch |
| Horden eingebunden | mittel | **hoch** | mittel | gering | mittel |
| Inhaltsaufwand | hoch | gering–mittel | mittel | **sehr hoch** | mittel |
| Früh spielbar (ohne Bosse/Biome) | nein | ja | **ja** | teils | nein (braucht Tag/Nacht) |

#### Empfehlung von Claude

**C als roter Faden, B als Saga-System darunter.** Begründung:
- C zieht in die Welt hinaus, erzählt leise über Funde statt Dialoge und gibt dem Lauschen einen Zweck.
- B schreibt die Geschichte der eigenen Siedlung mit: Bitten, Bewährungen, Gefallene, Gedenksteine. Das hält Überleben und Bauen im Vordergrund.
- Beide gehen ohne Bosse und Biome, also früh.
- Aus A lassen sich die Steinkreise und Lieder als Orte übernehmen, die die Spur der Ersten berührt. Die Seherin-Quest (`quest_village_seer`) passt in alle Vorschläge, in C als Nachfahrin der Ersten.

**Korrektur des Nutzers (2026-11-15):** Der Siedlungsaufbau ist nur **zweitrangig**. Deshalb soll B nicht die Hauptsache sein, sondern nur eine kleine automatische Chronik im Saga-Buch: Taten, Gefallene, Gedenksteine. Die Empfehlung verschiebt sich auf **C als Kern**, mit den Steinkreisen und Liedern aus A als Orten.

Nicht empfohlen als Hauptlinie: **D** (fünffacher Aufwand, kein gemeinsamer Faden) und **E** (Warten auf Jahreszeiten kollidiert mit „kein Zeitdruck“).

<a id="d17-11"></a>

### 17.11 11. Questfortschritt pro Spieler, alles spawnt nach (Entwurf)

Nutzer (2026-11-15): „Es kann alles nachspawnen.“ **Deutung von Claude:** Antwort auf die Frage nach WoW. Jeder Spieler hat seinen eigenen Queststand. Questgegner, Bosse und Questgegenstände kommen wieder, damit jeder sie selbst erleben kann. Das ersetzt Abschnitt 5 („Fortschritt gehört der Welt“). Noch nicht bestätigt.

| Frage | Regel (Vorschlag) |
|---|---|
| Queststand | **pro Spieler**, gespeichert unter der Spieler-ID im Weltspielstand |
| Gemeinsame Gutschrift | Wird ein Ziel erfüllt, bekommen es alle Spieler im Umkreis von 50 m, die diese Etappe gerade aktiv haben (wie in WoW). Man muss also nichts doppelt machen, wenn man zusammen spielt |
| Questgegenstände, Fragmente, Logbuchseiten | **pro Spieler sichtbar:** Jeder, der den Fund noch braucht, sieht und nimmt sein eigenes Exemplar. Wer ihn schon hat, sieht an der Stelle nichts mehr. Technisch: Der Server zeigt das Ding nur den Spielern, die es noch brauchen (kleines Phasing nur für Fundstücke, gut machbar) |
| Questgegner | spawnen nach (Vorschlag: nach 1 Spieltag, oder sofort, wenn ein weiterer Spieler mit aktiver Etappe kommt) |
| Bosse | spawnen nach (Vorschlag: nach 3 Spieltagen). **Erster Sieg (pro Spieler): volle Beute mit Bossmaterial. Jeder weitere Sieg: normale Beute, Bossmaterial nur mit einer Wahrscheinlichkeit, die von der Stufe abhängt** (Nutzer 2026-11-15: „beim ersten besiegen normaler lot dann kann prozentual t basierender lot droppen“; gedeutet als: Chance je Stufe). Vorschlag von Claude: T2 50 %, T3 40 %, T4 30 %, T5 20 %, T6 10 %; nach 4 Wiederholungen ohne Bossmaterial gibt es beim nächsten Sieg sicher welches (gegen Pechsträhnen). Daten: `"repeat_drop_chance"` je Boss, `"pity_after": 4`. Die Saga-Tat „Boss besiegt“ gibt es pro Spieler nur einmal |
| Was die Welt verändert | gibt es **nur einmal**: Die Seherhütte steht oder nicht, die Seherin zieht einmal ein, ein freigelegter Weg bleibt frei. Wer die Etappe später erreicht, bekommt sie als erledigt angerechnet, mit dem Vermerk „davon gehört“. Gespräche, Lauschen und Funde dazu kann er trotzdem selbst nachholen (eigene Taten, eigene Gespür-EP) |
| Kein echtes Phasing | Das Dorf und die Landschaft sehen für alle gleich aus. Nur Fundstücke und Questgegner werden je Spieler gezeigt bzw. nachgespawnt |
| Gespür-EP | unverändert aus `listen.json`: pro Fund und Spieler einmal. Ein nachgespawntes Seeglas zählt für denselben Spieler nicht noch einmal als „neuer Fund“, Seeglas-Ernte (+1) aber schon (Deutung: Seeglas ist ein nachwachsender Rohstoff) |
| Solo | Solo spielt genauso, nur ohne Mitspieler-Gutschrift |

Für die Daten heißt das: Jede Etappe bekommt ein Feld `"scope": "player"` (Standard) oder `"world"` (verändert die Welt, gibt es nur einmal). Gegner und Bosse bekommen `"respawn_days"`.

<a id="d17-12"></a>

### 17.12 12. Antworten des Nutzers (2026-11-15)

Eingetragen in `docs/01_entscheidungen.md`. Kurzfassung:
1. Queststand pro Spieler, alles spawnt nach (Abschnitt 11): keine Vorliebe angegeben, bleibt **Vorschlag**.
2. Bossbeute: erster Sieg volle Beute, danach Bossmaterial nur mit Chance je Stufe: **bestätigt**.
3. Startwerte (50 m, 1 und 3 Spieltage, 50–10 %, nach 4 Fehlversuchen sicher): **bestätigt als Startwerte**.
4. Story-Richtung C „Spur der Ersten“, Steinkreise als Orte, Chronik klein: **bestätigt**.
5. Probe schreiben: ja, siehe `docs/15_probe_spur_der_ersten.md`.
6. Horden: bleiben Weltoption in den Einstellungen, keine Story-Etappe braucht sie: **bestätigt**.
7. Questhilfe: Gebiet, genauere Hilfe über passende Gesprächswahl, im Menü abschaltbar: **bestätigt** (Deutung).
8. Gefallene: Eintrag immer, Gedenkstein freiwillig: **bestätigt**.
9. Gespräche mit Entscheidungen von Anfang an: **bestätigt** (gegen die Empfehlung). Offen: Wer entscheidet bei Entscheidungen, die die Welt verändern?

Folgen für den Entwurf: Abschnitt 5 („Fortschritt gehört der Welt“) gilt nicht mehr, sobald Abschnitt 11 bestätigt ist. Abschnitt 3 (Hauptsaga A) ist nicht mehr die Hauptlinie, Steinkreise und Lieder bleiben als Orte. Datenformat (Abschnitt 7) braucht noch: `scope` je Etappe, `respawn_days`, `repeat_drop_chance`, `pity_after`, Gesprächsknoten mit Wahl und Folgen.

#### Nachtrag: weitere Antworten (2026-11-15)
- Queststand pro Spieler: **bestätigt**. Abschnitt 11 gilt.
- Queststand bleibt immer pro Welt, auch bei „Inventar wandert mit“.
- Vertonung: Namen werden umschrieben; Spielerfigur mit 4 wählbaren Stimmen; Nebelbilder mit halb verwehten Sätzen; Lippen aus dem Ton (MetaHuman).
- Finale: Der Spieler entscheidet (Ring schließen oder offen lassen), zwei Enden. Im Koop zeigt die Welt das zuerst gewählte Ende (Vorschlag, Ausnahme von „Folgen addieren sich“).
- Saga-Buch-Taste: J.
- Damit sind die offenen Fragen aus Abschnitt 9 alle beantwortet.



[↑ zum Inhaltsverzeichnis](#inhaltsverzeichnis)

<a id="d18"></a>

## 18. Probe: Prolog „Die Spur der Ersten“

> Quelle: `docs/15_probe_spur_der_ersten.md`

Stand 2026-11-15. **Probetext von Claude** (KI-generiert, Eintrag in `design/asset_log.md`). Zweck: Der Nutzer entscheidet daran, ob die Story Hauptfokus wird oder roter Faden bleibt (siehe `docs/14_saga_quests.md`, Abschnitt 10, Vorschlag C).
Rahmen: **seit 2026-11-15 wird alles vertont** (Deutsch und Englisch, zuerst KI-Stimmen); Logbuchseiten liest Asgerds Stimme, Saga-Zeilen ein Erzähler (Skalde), Gedanken die Stimme der Figur; erzählt wird über Funde, Orte und kurze Einträge. Länge im Spiel: etwa 45–60 Minuten, nebenbei zum Sammeln und ersten Bauen. Nichts davon ist Pflicht; man kann jederzeit liegen lassen.

Lesehilfe: **Saga-Buch** = Eintrag im Buch nach einem Schritt. **Fund** = Text eines Fundstücks. **Hinweis** = kurzer Satz am Bildschirmrand. *Kursiv* = was der Spieler sieht oder hört (Regieanweisung, nicht im Spiel zu lesen).

**Überarbeitung 2 (2026-11-15):** Nutzer: Hauptstory mit großen Folgen, Nebenstränge mäßig, „gerne noch immersiver“. Neu sind die Nebelbilder (Abschnitte 4–6), die Gedanken der Figur und die große Entscheidung am leeren Stein (Abschnitt 6).

#### Mittel für Immersion
- **Nebelbilder (Seidr-Visionen):** Wo starkes Seeglas liegt, zeigt das Lauschen manchmal für einige Sekunden eine Szene der Ersten. Halbdurchsichtige Gestalten aus Nebel in Gezeitenlicht spielen einen Moment nach, mit gedämpften Lauten der erfundenen Sprache, ohne Worte. Man kann um sie herumgehen. Das passt zur Stilvorgabe („Seidr zeigt sich als Nebel und Visionen“). Wiederverwendbar: ein gemeinsamer Effekt und vorhandene Bewohner-Animationen.
- **Gedanken der Figur:** Hier und da eine einzelne kurze Zeile am unteren Rand, in der Ich-Form, abschaltbar. Nie mehr als ein Satz.
- **Handschrift:** Logbuchseiten erscheinen als Pergament mit Handschrift, Flecken und Durchstreichungen. Ein lesbarer Drucktext liegt daneben (Lesbarkeit hat Vorrang, bestätigt).
- **Die Welt antwortet:** Was man liest, findet man wieder: der hohle Baum, das umgedrehte Boot, der Flachs. Kein Text erzählt etwas, das man nicht sehen kann.
- **Klang statt Musik:** Wind, Brandung, Raben. Das Summen ist der einzige „magische“ Ton. Musik setzt erst am Ende des Prologs leise ein.

---

<a id="d18-1"></a>

### 18.1 1. Der Strand

*Grauer Morgen, Nebel über dem Wasser. Die Figur steht am Ufer zwischen Tang und Treibholz. Das eigene Boot liegt halb auf dem Kies. Weiter oben am Strand: ein zweiter Bootsrumpf, älter, kieloben, von Sand halb begraben.*

**Hinweis** (nur, wenn man 2 Minuten nichts tut): „Treibholz, Steine, Fasern. Ohne Werkzeug kommt man hier nicht weit.“

Der alte Rumpf lässt sich untersuchen (E).

**Fund: Kieloben liegendes Boot**
> Ein Boot wie eures, nur älter. Die Planken sind grau, die Nieten rostig. Jemand hat es umgedreht und mit Steinen beschwert, damit der Wind es nicht nimmt – als wollte er wiederkommen.
> In den Kiel ist ein Zeichen geschnitten: ein Ring, nicht geschlossen, mit drei schrägen Kerben.

**Saga-Buch:**
> *Sie landeten nicht als Erste an dieser Küste. Ein umgedrehtes Boot lag schon dort, beschwert mit Steinen, wie von jemandem, der wiederkommen wollte.*

<a id="d18-2"></a>

### 18.2 2. Die Raben

*Sobald die ersten Werkzeuge hergestellt sind (Bauhammer, Axt oder Spitzhacke): zwei Raben landen auf einem Stumpf in 30 m Entfernung, krächzen, fliegen ein Stück landeinwärts, setzen sich wieder. Folgt man nicht, warten sie eine Weile und fliegen dann allein weiter. Am nächsten Spieltag sitzen sie wieder am Strand.*

**Hinweis** (einmalig): „Raben sitzen gern dort, wo Menschen waren.“

<a id="d18-3"></a>

### 18.3 3. Der verlassene Hof

*Eine Lichtung 150 m vom Strand. Ein eingesunkenes Grassodendach, eine Wand eingefallen. Ein Feuerplatz, kalt und überwachsen. Hinter dem Haus ein Feldrand, kaum noch zu erkennen: Flachs, verwildert. Die Raben sitzen auf dem Dachfirst.*

Im Haus, in einer Nische hinter einem verrotteten Bett, eine Holzkiste mit Wachstuch.

**Fund: Logbuch der Ersten, Seite 1** (ins Saga-Buch, nicht ins Inventar)
> *Dritter Tag an Land.*
> Wir sind siebzehn, und das Boot hat gehalten. Thorgrim sagt, das Land ist gut, weil der Flachs hier wild wächst, und wo Flachs wild wächst, wächst er auch zahm. Ich sage, das Land ist gut, weil keiner hier ist.
> Die Kinder sammeln Muscheln. Ich habe ihnen verboten, das grüne Glas aufzuheben, das am Wasser liegt. Ich weiß nicht, warum. Es ist zu schön für einen Strand.
> – Asgerd, Steuerfrau

**Fund in derselben Kiste:** `item_seed_flax` ×4 (*„Leinsamen, in Wachstuch gewickelt, trocken. Jemand wollte, dass sie keimen.“*)
Die Samen sind normales Saatgut (Feldbau), also Questgegenstand und Alltagsnutzen in einem.

**Saga-Buch:**
> *Im ersten verlassenen Hof fanden sie das Logbuch einer Steuerfrau namens Asgerd und Saat, die siebzehn Menschen nicht mehr gesät hatten.*

**Offene Frage im Saga-Buch** (Teil „Fundbuch“): *„Wer waren die siebzehn?“*

<a id="d18-4"></a>

### 18.4 4. Das Summen

*Auf dem Rückweg zum Strand, beim Treibholzhaufen in einer Senke: Ein kaum hörbarer, tiefer Ton, wie ein Finger auf dem Rand eines Glases. Der Bildschirmrand schimmert schwach türkis, links stärker als rechts. Geht man darauf zu, wird es stärker; geht man weg, verblasst es. (Passives Lauschen, Stufe 1 „Gespür“.)*

**Hinweis** (einmalig, nach 5 Sekunden Schimmer): „Da ist etwas. Bleib stehen und lausche.“ (Taste wird angezeigt.)

*Aktives Lauschen: Die Figur bleibt stehen, legt den Kopf leicht schräg, eine Hand gehoben. Ein langsamer türkiser Ring läuft wellig über den Boden aus. Wo er das Treibholz streift, flackert für einen Moment eine Kontur: ein Stück Glas, halb im Sand.*

**Fund: Seeglas** (`item_seaglass`, Gespür: neuer Fund +2, Seeglas +1)
> Milchig, grünlich, kühl wie Wasser, auch wenn die Sonne darauf scheint. Hält man es ans Ohr, hört man nichts. Legt man es weg, hört man es wieder.

**Nebelbild 1: Muscheln** (*nach dem Aufheben, einmalig*)
*Der Ton wird tiefer. Über dem Kies verdichtet sich Nebel zu drei kleinen Gestalten, Kinder, gebückt, die etwas im Tang suchen. Eines hebt etwas Helles auf und hält es gegen das Licht. Weiter oben am Strand richtet sich eine größere Gestalt auf und ruft, ein langer, scharfer Laut. Die Kinder lassen fallen, was sie halten, und laufen zu ihr. Der Nebel fällt in sich zusammen. Am Boden liegt das Seeglas, das man selbst gerade aufgehoben hat, wieder im Kies: ein zweites Stück, an derselben Stelle.*

**Gedanke** (einmalig): „Sie hat sie gerufen wie vor einem Feuer.“

**Saga-Buch** (persönliche Tat des Spielers):
> *{Spieler} hörte zum ersten Mal das Summen, das die Ersten fürchteten, und sah im Nebel ihre Kinder am Strand.*

<a id="d18-5"></a>

### 18.5 5. Zwei weitere Seiten

Beide Seiten sind **hallend**: Sie liegen in der Nähe von Seeglas, und das Summen führt dorthin. Man findet sie mit Lauschen oder, mit viel Glück, beim Umschauen.

**Fund: Logbuch der Ersten, Seite 2** (in einem hohlen Baum am Bach, daneben ein Seeglas-Splitter)
> *Elfter Tag.*
> Hallbjörn hat das grüne Glas gesammelt. Er sagt, es singt nachts, wenn man es in einen Kreis legt. Er hat es den Kindern vorgemacht. Ich habe ihn angeschrien, vor allen, und dann habe ich mich geschämt, weil ich nicht sagen konnte, warum.
> Heute Nacht habe ich es selbst gehört. Es singt nicht. Es antwortet.

**Nebelbild 2: Der Kreis aus Glas** (*nur nachts oder im Nebel am hohlen Baum lauschen; sonst bleibt es beim Text*)
*Eine kniende Gestalt legt Splitter in einen Ring auf einen flachen Stein, einen nach dem anderen, und lässt eine Lücke. Sie wartet. Der Ring leuchtet einmal auf, schwach, und eine Welle läuft über das Wasser des Bachs, gegen die Strömung. Die Gestalt lacht, lautlos, wirft den Kopf zurück. Hinter ihr, am Rand des Bilds, steht eine zweite Gestalt mit verschränkten Armen und rührt sich nicht.*

**Gedanke:** „Er hat die Lücke gelassen. Mit Absicht.“

**Fund: Logbuch der Ersten, Seite 3** (unter einem Stein am Felsenrand, nur durch Lauschen)
> *Vierundzwanzigster Tag. Erster Frost.*
> Hallbjörn ist nach Norden gegangen, zu den Steinen, die man vom Hügel aus sieht. Drei sind mit ihm gegangen. Thorgrim sagt, sie kommen zurück, wenn ihnen kalt wird.
> Ich habe das Boot umgedreht und Steine darauf gelegt.

**Saga-Buch:**
> *Die Seiten erzählten von Hallbjörn, der das grüne Glas sammelte, und von Asgerd, die es nicht anrühren wollte. Und von Steinen im Norden.*

**Offene Frage im Saga-Buch:** *„Was antwortet im Glas?“*

<a id="d18-6"></a>

### 18.6 6. Die Gräber

*Auf dem Hügel hinter dem Hof, im Windschatten einer Kiefer: fünf flache Steinsetzungen in Schiffsform, überwachsen. In vier davon ist ein Name geritzt. In die fünfte nicht; sie ist leer, und frisch ist sie auch nicht. Von hier aus sieht man im Norden, bei klarem Wetter, eine Reihe aufrechter Steine auf einer Anhöhe.*

**Fund: Grabsteine**
> *Ulf. Gyda. Ketil der Junge. Ragna.*
> Der fünfte Stein ist ohne Namen. Unter dem Moos ist nur das Zeichen eingeritzt: der offene Ring mit den drei Kerben.

#### Kleine Entscheidung (Nebenstrang-Gewicht: mäßige Folgen)

*Neben den Gräbern liegt ein rostiges Beil, die Klinge noch brauchbar, der Stiel morsch.*

| Wahl | Folge |
|---|---|
| **Liegen lassen, die Gräber vom Moos befreien** | Persönliche Tat „Hüter der Gräber“; der Gedenkstein für die Ersten wird baubar |
| **Mitnehmen** | `tool_axe_t2` als „rostiges Beil“ (reparierbar, früher als sonst); keine Tat |

#### Nebelbild 3: Der leere Stein (*Lauschen am fünften Stein*)

*Das Summen ist hier lauter als irgendwo sonst. Der Nebel formt zwei Gestalten, die sich am leeren Stein gegenüberstehen. Die größere, ein Mann mit einem Ring am Finger, der schwach türkis glimmt, zeigt nach Norden. Die kleinere, eine Frau, schüttelt den Kopf, langsam, immer wieder. Er zieht den Ring ab, legt ihn auf den leeren Stein und geht. Sie bleibt stehen, bis er weg ist. Dann kniet sie sich hin, nimmt einen Stein und hebt ihn über den Ring, um ihn zu zerschlagen. Sie tut es nicht. Sie deckt ihn mit Moos zu. Der Nebel verweht.*

*Unter dem Moos des fünften Steins: ein Ring aus dunklem Eisen mit einem eingelassenen Splitter Seeglas.*

**Gedanke:** „Sie hat das Grab für ihn gegraben. Und er lebt vielleicht noch.“

#### Große Entscheidung (Hauptstory: große Folgen)

| Wahl | Text | Folge |
|---|---|---|
| **Hallbjörns Ring nehmen** | *„Wenn das Glas antwortet, will ich hören, was es sagt.“* | Ring für den Ring-Slot: „Hallbjörns Ring“, Lauschen-Reichweite +20 % (Vorschlag). **Weg des Kreises:** Die Leute um Hallbjörn am Steinkreis im Norden nehmen dich als seinen Erben auf. Deine nächsten Kapitel führen tiefer in den Gezeitenhall: Steinkreise, Lieder, Seidr. Asgerds Leute begegnen dir mit Misstrauen: Handel ja, Zuzug nein |
| **Den Ring liegen lassen und Hallbjörns Namen in den Stein ritzen** | *„Sie hat auf ihn gewartet. Jetzt muss sie es nicht mehr.“* | **Weg des Hofs:** Du erfüllst, was Asgerd nicht konnte. Ihre überlebenden Leute, verstecken sich an einem Fjord. Sie erkennen das Zeichen und vertrauen dir. Deine nächsten Kapitel führen zu ihnen; später können sie als Bewohner in die Siedlung ziehen, mit eigenem Wissen (Flachs, Boote, alte Saat). Hallbjörns Kreis bleibt dir verschlossen, die Steinkreise erreichst du auf einem härteren Weg |

- Beide Wege führen zum Ende der Hauptstory. Sie zeigen aber andere Kapitel, andere Orte und andere Verbündete. Der andere Weg ist für diesen Spieler zu.
- **Koop:** Jeder entscheidet für sich. Was die Welt verändert, **addiert sich**, statt sich auszuschließen. Hat ein Spieler den Hof-Weg gewählt, ziehen Asgerds Leute ins Dorf; hat ein anderer den Ring genommen, spricht Hallbjörns Kreis mit ihm. So braucht es keine Abstimmung (Vorschlag von Claude als Antwort auf die offene Frage „Wer entscheidet bei Entscheidungen, die die Welt verändern?“).
- **Aufwand ehrlich:** Große Folgen heißen doppelte Kapitel ab hier. Vorschlag: Die Wege laufen 2–3 Kapitel getrennt und treffen sich dann wieder (wie ein Zopf), damit der Inhalt nicht auf das Doppelte des ganzen Spiels wächst.

**Saga-Buch** (je nach Wahl):
> *Am leeren Stein fand {Spieler} Hallbjörns Ring und steckte ihn an. Das Glas wurde warm.*
> *Am leeren Stein ritzte {Spieler} den Namen, der fehlte: Hallbjörn. Der Wind drehte.*

<a id="d18-7"></a>

### 18.7 7. Ende des Prologs

*Vom Gräberhügel aus: Das Summen im Norden ist schwach, weit weg, aber gleichmäßig wie ein Atem. Die Raben fliegen hin und kommen nicht zurück. Wer den Namen geritzt hat, sieht dazu im Osten, über dem Fjord, eine dünne Rauchsäule. Sie war vorher nicht da.*

*Erst jetzt setzt leise Musik ein: eine Leier, ein tiefer Ton darunter.*

**Saga-Buch** (Abschluss, Kapitelzeile):
> *Siebzehn kamen vor ihnen. Vier liegen auf dem Hügel. Einer ging nach Norden und nahm drei mit. Neun sind noch irgendwo, und eine von ihnen hat ein Grab für einen Lebenden gegraben.*

**Offene Frage im Saga-Buch:** je nach Weg *„Was hört Hallbjörn im Norden?“* oder *„Wer zündet das Feuer am Fjord an?“*

Danach: Weg des Kreises → Steinkreis im Norden (Steinkreis +8 beim ersten Lauschen). Weg des Hofs → Rauch am Fjord. Die Dorf-Seherin (`quest_village_seer`) kommt auf beiden Wegen. Auf dem Weg des Kreises ist sie eine von Hallbjörns Leuten, auf dem Weg des Hofs Asgerds Tochter.

---

<a id="d18-8"></a>

### 18.8 Was die Probe zeigen soll

- **Story, wenn man hinschaut, und die Welt erzählt mit:** Nebelbilder zeigen statt zu erzählen, Text bleibt kurz.
- **Questgegenstände sind Alltagsdinge:** Leinsamen, Seeglas, Beil, ein Ring für den Ring-Slot. Nur Logbuchseiten gehen ins Buch.
- **Lauschen ist der Schlüssel:** Seiten 2–3 und alle Nebelbilder gibt es nur durch Lauschen. Nebelbild 2 belohnt das Wiederkommen in der Nacht (Gespür-Bonus), ohne Pflicht.
- **Folgen:** kleine Wahl (Beil) mit mäßigen, große Wahl (Ring oder Name) mit großen Folgen.
- **Kein Zeitdruck:** Raben warten, Nebelbilder kommen wieder.

<a id="d18-9"></a>

### 18.9 Fragen an den Nutzer zur Probe

1. Ist es jetzt immersiv genug, oder noch mehr (z. B. längere Nebelbilder, mehr Gedanken der Figur)?
2. Die große Entscheidung (Ring nehmen oder Namen ritzen): passt das als Art von „großer Folge“?
3. Koop: Folgen, die die Welt verändern, addieren sich (beide Wege können in einer Welt stattfinden). Einverstanden?


[↑ zum Inhaltsverzeichnis](#inhaltsverzeichnis)

<a id="d19"></a>

## 19. Hauptstory „Die Spur der Ersten“: Kapitelplan (Entwurf)

> Quelle: `docs/17_hauptstory_kapitel.md`

Stand 2026-11-16. **Alles hier ist ein Vorschlag von Claude**, solange es nicht in `docs/01_entscheidungen.md` bestätigt ist.
Auftrag des Nutzers (2026-11-16, über den Game Director): „So wie vorgeschlagen“, also die Kapitel nach dem Prolog ausarbeiten.
Grundlagen: `docs/14_saga_quests.md`, `docs/15_probe_spur_der_ersten.md`, `docs/01_entscheidungen.md`. Die bestätigten Saga-Zeilen sind:
- Kern „Spur der Ersten“, Steinkreise als Orte, Queststand pro Spieler (pro Welt);
- große Folgen in der Hauptstory, mäßige in Nebensträngen, Folgen für die Welt addieren sich;
- Wege 2–3 Kapitel getrennt, Finale „Ring schließen oder offen lassen“;
- alles vertont (DE/EN), Nebelbilder.

Neu einbezogen sind die Entscheidungen vom 2026-11-16: Spielmodus Saga bzw. Freies Spiel, Schwierigkeit, Tag/Nacht (30 min), Wetter, Durst, Ausdauer und Erschöpfung, Tod und Grab, Tiere.

<a id="d19-1"></a>

### 19.1 Leitplanken

- **Roter Faden, kein Story-Spiel.** Jedes Kapitel ist vor allem ein **Ort, den man erkundet und in dem man überlebt**. Die Story liegt in Funden, Nebelbildern und wenigen Gesprächen. Ein Kapitel ist in etwa 1–2 Stunden Spielzeit zu schaffen, nebenbei zu Bauen und Sammeln.
- **Ein Biom, ein Steinkreis, ein Boss je Kapitel** (Biome und Bosse aus `design/ASSET_LIST_GESAMT.md`). Bosse in der realistischen Fassung (bestätigt), Sagen-Fassung nur mit Weltoption „Sagenwelt“.
- **Lauschen:** Die Hauptstory verlangt höchstens Gespür-Stufe 2 (`docs/14_saga_quests.md` 4.2). Höhere Stufen öffnen verborgene Strophen und Nebelbilder als Zusatz.
- **Horden nur optional:** Wo eine Bedrohung die Siedlung trifft, gibt es mit der Weltoption einen Angriff, ohne sie einen gefundenen Schauplatz (verbranntes Lager, Spuren). Keine Etappe braucht einen Angriff.
- **Spielmodus Freies Spiel:** keine Hauptstory. Biome, Steinkreise, Bosse, Lieder und Nebenstränge bleiben; Bosse findet man durch Erkunden (Arena als Ort), ohne Kapiteltext.
- **Kerben:** Kapitel 3, 5 und das Finale lassen je eine Kerbe des Zeichens leuchten.

<a id="d19-2"></a>

### 19.2 Was bisher geschah (Prolog, Probe)

17 Siedler, „die Ersten“, landeten vor euch. Vier liegen auf dem Gräberhügel. Hallbjörn sammelte Seeglas, hörte es „antworten“ und ging mit drei anderen nach Norden zu den Steinen. Asgerd, die Steuerfrau, grub ein Grab für ihn, obwohl er lebte. Neun sind verschollen.
Große Entscheidung am leeren Stein:
- **Ring nehmen:** Weg des Kreises;
- **Namen ritzen:** Weg des Hofs.

<a id="d19-3"></a>

### 19.3 Hintergrund (nur für das Team, wird im Spiel Stück für Stück gefunden)

- Der **Gezeitenhall** ist eine alte Kraft im Meer und im Fels. Ein altes Volk hat ihn in einem **Ring aus Steinkreisen** gebunden, einer je Land. Der Ring ist an drei Stellen gebrochen (die drei Kerben).
- Wer Seeglas in einen Kreis legt und eine Lücke lässt, „ruft“. Hallbjörn tat das. Das Licht in den Nächten sah man weit über das Meer.
- Der **Aschebund**, Priester von der Glutküste, sucht seit Langem nach Menschen, die rufen können. Er bezahlt den **Seekönig** und seine Plünderer, damit sie solche Leute bringen. Die Plünderer kamen zu den Ersten **wegen Hallbjörns Licht**.
- Hallbjörn floh weiter in den Norden, um niemanden mehr anzulocken, und verlor sich im Fjell, halb im Gezeitenhall. Asgerds Leute versteckten sich am Fjord. Asgerd starb im zweiten Winter. Ihre Tochter führt die Überlebenden.
- Das Finale entscheidet, ob der Ring wieder geschlossen wird (Kraft gezähmt, Gezeiten ruhig, Magie leiser) oder offen bleibt (Kraft wild, die Siedler leben mit ihr).

<a id="d19-4"></a>

### 19.4 Kapitelübersicht

| # | Kapitel | Biom (Stufe) | Boss | Kern oder später |
|---|---|---|---|---|
| P | Die Spur der Ersten (Prolog) | Startfjord, Fjordwiesen (T1) | – | **Kern** |
| 1K / 1H | Der Kreis im Norden / Rauch am Fjord | Fjordwiesen (T1) | Der Alte Keiler | **Kern** |
| 2K / 2H | Was die Ebbe freigibt / Das Boot der Ersten | Gezeitenmarsch (T2) | Die Schlickmutter | **Kern** |
| 3 | Das zweite Logbuch (Wege treffen sich) | Gezeitenmarsch + Siedlung | – | **Kern** · Kerbe 1 |
| 4 | Der Eid des Hügelkönigs | Hügelgrab-Heide (T3) | Der Hügelkönig | später |
| 5 | Der Mann im Eis | Basaltfjell (T4) | Der Felsenvater | später · Kerbe 2 |
| 6 | Die Aschezunge | Glutküste (T5) | Die Aschezunge | später |
| 7 | Der Sturm (Finale) | Eisrand (T6) | Der Sturmkoloss | später · Kerbe 3, Jarlssitz |

**Kern** = Prolog bis Kapitel 3: zwei Biome, zwei Bosse, beide Wege. Das deckt das Ziel von Phase 4 („Erste zwei Biome mit Boss spielbar“). Alles danach kommt später und kann sich noch ändern.

---

<a id="d19-5"></a>

### 19.5 Kapitel 1K: Der Kreis im Norden (Weg des Kreises)

**Kern:** Hallbjörns Ring führt zum Steinkreis auf der Anhöhe. Dort lagern drei seiner Gefährten, verwildert und misstrauisch; sie erkennen den Ring. Der Kreis ist verstummt, weil an seiner Quelle der **Alte Keiler** wühlt und die Steine umgeworfen hat. Wer den Kreis wieder aufrichtet und dort lauscht, hört das Lied der Fjordwiesen und sieht im Nebelbild, wohin Hallbjörn ging: in die Marsch, „wo das Meer zweimal am Tag den Weg freigibt“.
- **Ort:** Anhöhe über den Fjordwiesen, Steinkreis, Lager der Gefährten, Keilersuhle.
- **Große Entscheidung:** keine. Kleine Entscheidung: Den Gefährten Seeglas geben (sie bleiben am Kreis und werden später Verbündete) oder es selbst behalten (mehr Gespür-EP und Material).
- **Systeme:**
  - Lauschen (Stufe 1–2, Steinkreis +8);
  - Nacht: Nebelbild nur nachts oder bei Nebel klar;
  - Tiere: der Keiler als Boss; Wölfe nachts am Kreis;
  - Jagd vor dem Kampfsystem: Speer und Bogen reichen gegen den Keiler (Boss mit Haltungsbruch erst mit Phase 3).
- **Questgegenstände:** Hallbjörns Ring (Ring-Slot, aus dem Prolog), Lied-Strophen (ins Buch), Keilerhauer (Bossmaterial: Bauplan Rennofen T2, wie in der Asset-Liste).
- **Nebenstränge:** „Die Stimme im Seeglas“ (Dorf-Seherin; hier ist sie eine der Gefährtinnen), verborgene Strophe des Fjordlieds (Stufe 4 oder Stufe 3 nachts am Kreis).

<a id="d19-6"></a>

### 19.6 Kapitel 1H: Rauch am Fjord (Weg des Hofs)

**Kern:** Die Rauchsäule führt zu einem versteckten Lager am Fjord: Asgerds Tochter und sechs Überlebende, ausgehungert, mit einem kaputten Boot. Sie erkennen Hallbjörns Namen im Stein, den du geritzt hast, und lassen dich herein. Sie brauchen Vorräte für den Winter. Ihre einzige Wiese wühlt der **Alte Keiler** um. Mit dem Keiler erlegt und dem Lager versorgt, zeigen sie dir Asgerds letzten Wunsch: das Boot der Ersten aus der Marsch zurückholen, in dem ihre Saatkiste liegt.
- **Ort:** Fjordufer, verstecktes Lager, Wiese, Keilersuhle.
- **Große Entscheidung:** keine. Kleine Entscheidung: Die Überlebenden sofort in deine Siedlung einladen (sie ziehen nach Kapitel 3 ein; früher Gewinn an Bewohnern) oder ihnen helfen, ihr eigenes Lager zu halten (sie bleiben dort als Handelsposten mit eigener Ware: Fisch, Leinen).
- **Systeme:**
  - Überleben: Vorräte sammeln, Gerichte kochen und bringen (Taverne, Verderb);
  - Durst: Regenfänger fürs Lager bauen;
  - Jahreszeit: ihr Winter ist das Thema, aber ohne Frist; fehlt es, warten sie;
  - Tiere: Keiler, Hirsche zum Jagen.
- **Questgegenstände:** Asgerds Kamm (Erbstück, ins Buch), Gerichte und Felle (normale Waren), Keilerhauer.
- **Nebenstränge:** „Die Stimme im Seeglas“ (hier ist die Dorf-Seherin Asgerds Tochter), Gedenkstein für die Ersten (aus dem Prolog), Fischerdorf (`npc_fishing_villager`) als erster Handelspartner.

<a id="d19-7"></a>

### 19.7 Kapitel 2K: Was die Ebbe freigibt (Weg des Kreises)

**Kern:** Hallbjörns Spur führt in die Gezeitenmarsch zu einem Steinkreis, der nur bei Ebbe aus dem Schlick ragt. Dort hat er Seeglas gelegt und gerufen. Im Nebelbild sieht man, was danach kam: Segel am Horizont. Auf einem Wrack in der Marsch sitzt die **Schlickmutter**, eine Plündererin des Seekönigs, die damals die Ersten überfiel. In ihrem Lager liegt Hallbjörns Seeglas-Beutel und das zweite Logbuch.
- **Ort:** Gezeitenmarsch, Priele, Kreis im Watt, Wrack der Schlickmutter.
- **Große Entscheidung (Weg des Kreises):** Den Kreis im Watt **wieder rufen lassen** (Seeglas legen: das Lied wird vollständig, du bekommst deutlich mehr Gespür-EP und ein starkes Nebelbild, aber das Licht lockt Plünderer an; mit Weltoption Horden ein Angriff, sonst härtere Wachen am Wrack) oder **den Kreis stumm lassen** (sicher, aber Hallbjörns Spur wird kälter: In Kapitel 5 findet man ihn schwerer, es gibt eine längere Suche).
- **Systeme:**
  - Ebbe und Flut (braucht Gezeiten; ohne sie: Kreis nur bei Nacht erreichbar);
  - Nebel (Wetter) stärkt das Lauschen (Option D);
  - Lauschen am Kreis;
  - Boss Schlickmutter (Mensch, Kampf Phase 3, bis dahin Jagd-Angriff unpassend: **Kapitel 2 braucht das Kampfsystem**);
  - Ausdauer im Schlick (langsamer, kein Sperren).
- **Questgegenstände:** Hallbjörns Seeglas-Beutel (Seeglas, normal nutzbar), Logbuch Teil 2 (`item_logbook` aus der Asset-Liste, ins Buch), Bossmaterial (T3).
- **Nebenstränge:** Lied der Marsch, Schiffswrack (nur bei Ebbe), Schwarzmarkt-Händler (nachts, kauft Seeglas).

<a id="d19-8"></a>

### 19.8 Kapitel 2H: Das Boot der Ersten (Weg des Hofs)

**Kern:** Asgerds Leute führen dich in die Marsch zum Boot der Ersten, das die Plünderer damals mitnahmen. Es liegt jetzt im Lager der **Schlickmutter**, auf ihrem Wrack. Die Saatkiste darin ist alles, was von Asgerds Hof übrig ist. Wer sie zurückholt, findet auch das zweite Logbuch: Asgerds letzte Seiten, die sie den Plünderern nie verraten hat.
- **Ort:** Gezeitenmarsch, Pfahlstege, Wrack der Schlickmutter.
- **Große Entscheidung (Weg des Hofs):** Die Schlickmutter **töten** (sicher, Beute) oder sie **gefangen nehmen und ausfragen** (sie verrät, dass der Seekönig für den Aschebund Menschen sucht, „die rufen können“; sie kann später als Lotsin für den Weg zum Eisrand dienen. Folge: Asgerds Leute sind entsetzt und misstrauen dir eine Weile).
- **Systeme:** Siedlung (die Saatkiste gibt Ursaat für den Feldbau, bessere Erträge), Kampf (Phase 3), Gezeiten wie in 2K, Tod und Grab (Schlick als gefährlicher Ort für das Grab: Mitspieler bergen).
- **Questgegenstände:** Asgerds Saatkiste (Ursaat: Gerste, Flachs, Möhren mit höherem Ertrag), Logbuch Teil 2, Bossmaterial.
- **Nebenstränge:** Fischerdorf (Ruf), Torfstich, Lied der Marsch.

<a id="d19-9"></a>

### 19.9 Kapitel 3: Das zweite Logbuch (die Wege treffen sich)

**Kern:** Beide Wege enden beim zweiten Logbuch. Darin steht, was die Ersten auseinanderriss: Hallbjörns Licht holte die Plünderer, und die Plünderer arbeiten für jemanden im Süden, „Männer mit Asche im Gesicht“. In der Siedlung kommen die Fäden zusammen: Hallbjörns Gefährten und/oder Asgerds Leute (je nachdem, was in der Welt gewählt wurde; beides geht) treffen sich am **Thingplatz**. Am Ende leuchtet die **erste Kerbe**: Die Spur ist nicht mehr nur die der Ersten, sondern eure.
- **Ort:** eigene Siedlung, Thingplatz (`building_thing_site`).
- **Große Entscheidung:** Was tut die Siedlung mit dem Wissen? **Den Ersten folgen und den Ring heilen wollen** (Ausrichtung „Hüter“) oder **den Gezeitenhall für sich nutzen wollen** (Ausrichtung „Rufer“). Das ist eine Haltung des Spielers mit großen Folgen für Kapitel 4–6 (wer euch traut, wer euch jagt) und führt zur Finalentscheidung, legt sie aber nicht fest.
- **Systeme:** Siedlung (Thingplatz bauen), Bewohner ziehen zu (Asgerds Leute, wenn eingeladen), Gedenksteine, Saga-Buch (Kerbe 1).
- **Questgegenstände:** keine neuen; das Logbuch wird im Buch vollständig.
- **Nebenstränge:** Skalde (`npc_skald`) kommt mit dem Thingplatz; Bitten der neuen Bewohner.

---

<a id="d19-10"></a>

### 19.10 Kapitel 4 (später): Der Eid des Hügelkönigs

**Kern:** In der Hügelgrab-Heide herrscht ein Eidbrecher-Jarl aus einer Grabfestung, der **Hügelkönig**. Er verlangt Tribut von allen Siedlungen und hat den Steinkreis der Heide in seine Festung gebaut. Die **Brückenrunen** (`item_bridge_runes`) öffnen den Weg über den Heidefluss.
- **Große Entscheidung:** **Ihm einen Eid schwören** (kein Kampf; seine Leute schützen eure Wege, er wird Verbündeter gegen den Seekönig, aber er nimmt Tribut aus dem Lager, und den Kreis öffnet er nur „Rufern“) oder **ihn herausfordern** (Bosskampf, Kreis frei, Heide offen). Wer schwört, kann den Eid später brechen: Dann kommt er als Boss doch.
- **Systeme:** Kampf, Horden optional (seine Kriegsschar holt Tribut mit Gewalt), Erschöpfung (lange Wege), Krieger-Herkunft mit eigenem Gesprächszweig (gebrochener Eid).
- **Questgegenstände:** Brückenrunen, Eidring (Ring-Slot), Bossmaterial (T4).
- **Nebenstränge:** Grabhügel als kleine Dungeons, Lied der Heide.

<a id="d19-11"></a>

### 19.11 Kapitel 5 (später): Der Mann im Eis

**Kern:** Im Basaltfjell findet man **Hallbjörn**, lebend, in einer Eishöhle, halb im Gezeitenhall. Er spricht nur noch in Bruchstücken. Vor der Höhle haust der **Felsenvater**, ein riesiger Bär. Hallbjörn kennt die Stelle, an der der Ring zuerst brach.
- **Große Entscheidung:** **Hallbjörn heimbringen** (er wird zum Bewohner mit eigenem Beruf: Er kann Seeglas „stimmen“; Asgerds Leute verzeihen oder nicht, je nach Kapitel 3) oder **ihn im Eis lassen, wo er den Ring hört** (er bleibt eine Stimme im Kreis: Lauschen im Fjell wird stärker, die verborgenen Strophen aller Lieder werden für dich hörbar).
- **Systeme:** Winter und Kälte (Kälteresistenz durch Gerichte; die Höhle ist im Winter über den gefrorenen Fluss nah, sonst über einen längeren Bergweg erreichbar, ohne Frist), Lauschen (Nebelbilder der Ersten), Tiere (Bär, Wölfe), Durst wenig, Kälte viel. **Kerbe 2.**
- **Questgegenstände:** Hallbjörns Stab mit Seeglas, Bärenfell des Felsenvaters (Bossmaterial T5).

<a id="d19-12"></a>

### 19.12 Kapitel 6 (später): Die Aschezunge

**Kern:** An der Glutküste sitzt der **Aschebund**. Sein Hohepriester, die **Aschezunge**, will den Ring ganz brechen, damit der Gezeitenhall frei durch das Land läuft und er ihn lenken kann. Hier erfährt man, wie man den Ring schließt und was es kostet: Das Schließen braucht Seeglas aus allen sechs Kreisen und nimmt der Welt einen Teil ihrer Magie.
- **Große Entscheidung:** Die Haltung aus Kapitel 3 kommt zurück. **Hüter** sind hier Feinde des Bundes. **Rufer** werden vom Bund umworben: Man kann die Aschezunge verraten oder ihr folgen (folgen = sie greift im Finale an eurer Seite in den Sturm ein, dafür kein Ring-Schließen mehr möglich).
- **Systeme:** **Hitze und Durst** stärker (Bestätigung „Hitze später stärker“), Wasser mitnehmen, Dampfspalten (Fallschaden, Verbrennung), heiße Quellen als Rast.
- **Questgegenstände:** Aschesiegel, Obsidian, Bossmaterial (Relikt T6).

<a id="d19-13"></a>

### 19.13 Kapitel 7 (später): Der Sturm (Finale)

**Kern:** Der **Seekönig** kommt mit seiner Flotte zum Eisrand, wo der erste Kreis im Gletscher liegt. Der **Sturmkoloss** (realistisch: der Seekönig auf seinem Flaggschiff im Eis; Sagenwelt: Frostriese). Danach steht man im Herz des Rings.
- **Finalentscheidung (bestätigt):** **Ring schließen** oder **offen lassen**. Im Koop entscheidet jeder für sich, die Welt zeigt das zuerst gewählte Ende (Vorschlag).
- **Folgen:** Schließen: ruhige Gezeiten, Magie leiser (Lauschen-Reichweite sinkt, Seeglas wird selten). Offen: Gezeitenhall-Ereignisse häufiger, Seidr-Fähigkeiten stärker, gezeichnete Kreaturen (nur mit Sagenwelt). **Kerbe 3**, Saga-Finale, Jarlssitz (`building_jarl_hall`) als Belohnung.
- **Nach dem Finale:** Die Welt läuft weiter (Freies Tempo, Bosse spawnen nach, Nebenstränge bleiben).

---

<a id="d19-14"></a>

### 19.14 Umfang und Aufwand (ehrlich)

| Teil | Kapitel-Einheiten | Bosse | Sprechzeilen (Schätzung) | Hängt an |
|---|---|---|---|---|
| Kern | P, 1K, 1H, 2K, 2H, 3 = 6 | 2 | ca. 800–1 200 je Sprache | Lauschen, Tag/Nacht, Wetter, Tiere, Saga-Kern (Quest-System), **Kampfsystem (Phase 3) ab Kapitel 2**, Gezeiten für die Marsch |
| Später | 4, 5, 6, 7 = 4 | 4 | ca. 800–1 200 je Sprache | Biome 3–6, Kampf, Winter/Hitze |

- Getrennte Wege kosten fast doppelt Inhalt in Kapitel 1–2. Deshalb teilen sich beide Wege **Orte und Bosse** (Keiler, Schlickmutter); nur Anlass, Gespräche und Belohnung unterscheiden sich.
- Ab Kapitel 4 gibt es **keine getrennten Wege** mehr, sondern Haltung und Entscheidungen innerhalb eines Kapitels. Sonst wächst der Inhalt zu stark.
- Text und Vertonung sind der größte Posten. Vorschlag: erst Text aller Kern-Kapitel, dann KI-Vertonung, dann spielen und kürzen.

<a id="d19-15"></a>

### 19.15 Antworten des Nutzers

Eingetragen in `docs/01_entscheidungen.md`:
1. Kern = Prolog bis Kapitel 3: **bestätigt** (2026-11-16).
2. Hintergrund (Aschebund bezahlt die Plünderer, Hallbjörns Licht lockte sie): **bestätigt** (2026-11-16).
3. Haltung „Hüter oder Rufer“ in Kapitel 3: **bestätigt** (2026-11-16).
4. Hallbjörn lebt (Kapitel 5): **bestätigt** (2026-11-21).
5. Kapitel 4: Eid schwören oder herausfordern: **bestätigt** (2026-11-21).
6. Ebbe und Flut (zweimal pro Spieltag): **bestätigt** (2026-11-21).

<a id="d19-16"></a>

### 19.16 Noch offen

- ~~Tribut im Koop~~ **bestätigt am 2026-11-21** (Nutzer: „Ja, wie vorgeschlagen“); nur die Höhe des Anteils ist offen. Regel: Der Hügelkönig nimmt Tribut, sobald mindestens ein Spieler geschworen hat (Folgen addieren sich), aber nur einen kleinen Anteil, und Spieler, die nicht geschworen haben, können ihn trotzdem herausfordern; dann gilt der Eid für alle als gebrochen.
- Wo Kapitel 4–7 im Detail landen, ist absichtlich grob; es kommt erst nach dem Kern.

---

<a id="d19-17"></a>

### 19.17 Kapitel 4–7 im Detail (Entwurf, 2026-11-21)

Auf Wunsch des Nutzers („Erst Plan genauer machen“) sind die späteren Kapitel hier genauer geplant. Fertigen Spieltext gibt es dafür noch nicht. **Alles in diesem Abschnitt ist Vorschlag**, außer dem, was als bestätigt markiert ist.

Gemeinsam für alle vier Kapitel:
- **Haltung aus Kapitel 3** (bestätigt): Hüter werden vom Aschebund **gejagt**, Rufer werden **umworben**. Gespräche erkennen die Haltung des Sprechenden (`stance:`).
- **Entscheidungen**, die die Welt nur auf eine Art verändern, laufen als **gemeinsames Gespräch** (bestätigt 2026-11-21): Jeder wählt, es gibt keine Uhr, und bei Uneinigkeit gilt die umkehrbare Option.
- **Horden** bleiben optional. Wo im Folgenden ein Angriff steht, gibt es ohne die Weltoption stattdessen einen gefundenen Schauplatz.
- Jedes Kapitel bringt einen **neuen Steinkreis** mit Lied. Für das Ende (Ring schließen) braucht man Seeglas aus allen sechs Kreisen. Man sammelt es nebenbei, Kapitel für Kapitel.
- **Neuer Faden: das Ringvolk** (bestätigt 2026-11-21). Ab Kapitel 4 zeigen manche Nebelbilder nicht mehr die Ersten, sondern das alte Volk, das den Ring band. Das bereitet das Finale vor.

#### Kapitel 4: Der Eid des Hügelkönigs (Hügelgrab-Heide, T3)

**Kern:** Ein Jarl aus der Heide, der **Hügelkönig** (Vorschlag: Jarl Sigvat Grabwart), hat sich in einer alten Grabfestung eingerichtet. Er verlangt Tribut von allen Siedlungen. Den Steinkreis der Heide hat er in seine Mauern gebaut. Er fürchtet den Seekönig und sucht Verbündete.

| # | Etappe | Inhalt | Ziel-Typen |
|---|---|---|---|
| 4.1 | Der Tributbote | Ein Bote des Jarls kommt zum Thingplatz und fordert Tribut. Rufer sehen zugleich einen zweiten Fremden mit Asche-Streifen, der ihnen „Hilfe gegen den Jarl“ anbietet (erster Kontakt zum Aschebund) | `talk` |
| 4.2 | Die Brückenrunen | Die Brücke über den Heidefluss ist zerstört. Drei Runensteine mit den **Brückenrunen** (`item_bridge_runes`) liegen verstreut, zwei davon „hallend“ (durch Lauschen). Daraus entsteht ein Bauplan für eine Runenbrücke | `listen_find`, `read`, `build` |
| 4.3 | Die Heide | Grabhügel als kleine Dungeons (Fackeln, Nacht), Lied-Strophen der Heide. **Nebelbild 7:** Menschen des Ringvolks setzen einen Stein in den Kreis, singend. Zum ersten Mal sieht man, wer den Ring baute | `go_to`, `vision` |
| 4.4 | Die Grabfestung | Gespräch mit dem Jarl. Krieger-Herkunft hat einen eigenen Gesprächszweig (gebrochener Eid). **Große Entscheidung** (bestätigt) | `talk`, `choose` |
| 4.5a | Der Eid | Kein Kampf. Tribut: ein kleiner Anteil aus dem gemeinsamen Lager, sobald mindestens ein Spieler geschworen hat (bestätigt). Die Wege der Heide werden sicherer, der Jarl wird Verbündeter im Finale (Schiffe). Den Kreis öffnet er nur für **Rufer**. Hüter müssen den Eid brechen oder auf einen anderen Weg warten (siehe 4.6) | – |
| 4.5b | Die Herausforderung | Bosskampf gegen den Hügelkönig (T4-Material). Seine Leute zerstreuen sich (mit Horden: Plünderzüge in der Heide). Der Kreis ist frei | `defeat` |
| 4.6 | Der Kreis der Heide | Lauschen im Kreis (in der Festung). **Nebelbild 8:** Hallbjörn, allein, zieht weiter nach Norden ins Fjell. Hüter mit Eid: Der Jarl lässt sie einmal hinein, wenn sie ihm einen Gefallen tun (Nebenstrang „Der Grabräuber“). Das ist ein längerer Weg, aber kein Eidbruch | `listen_at`, `vision` |

- **Eid brechen:** Wer geschworen hat, kann jederzeit am Jarl den Eid brechen. Dann kommt es zum Bosskampf, und der Eid gilt für alle als gebrochen (bestätigt).
- **Systeme:** Kampf, Erschöpfung (lange Wege über die Heide), Wetter (Gewitter über der Heide), Tiere (Elch, Luchs), Nacht in den Grabhügeln.
- **Questgegenstände:** Brückenrunen, Eidring (Ring-Slot; Vorschlag: Bewohner der eigenen Siedlung sind auf den Heidewegen sicherer), Bossmaterial T4.
- **Nebenstränge:** Grabhügel, „Der Grabräuber“ (für Hüter mit Eid), Lied der Heide mit verborgener Strophe, Krieger: der gebrochene Eid.

#### Kapitel 5: Der Mann im Eis (Basaltfjell, T4)

**Kern:** Hallbjörn lebt (bestätigt). Er sitzt allein in einer Eishöhle im Basaltfjell, in einem Kreis aus gefrorenem Seeglas, halb im Gezeitenhall verloren. Vor der Höhle haust der **Felsenvater**, ein riesiger Bär. Hallbjörn weiß, wo der Ring zuerst brach und was ihn heilt oder ganz öffnet.

| # | Etappe | Inhalt | Ziel-Typen |
|---|---|---|---|
| 5.1 | Ins Fjell | Hallbjörns Spur aus Kapitel 4. Hat man in 2K den Kreis rufen lassen, führt Nebelbild 6 direkt; sonst gibt es einen zusätzlichen Ort (bestätigte Folge). **Im Winter** ist der Fluss gefroren und eine Abkürzung offen, sonst führt ein längerer Bergweg hin. Es gibt keine Frist | `go_to` |
| 5.2 | Hallbjörns Lager | Drei verlassene Lager mit **Inschriften** in Hallbjörns Stimme (seine Seite der Geschichte, kurze in Fels geritzte Sätze) und Seeglas-Verstecken (Lauschen) | `read`, `listen_find` |
| 5.3 | Der Felsenvater | Bosskampf gegen den Bären vor der Höhle (T5-Material). Wölfe in der Nacht, Kälte | `defeat` |
| 5.4 | Der Mann im Eis | Gespräch mit Hallbjörn in Bruchstücken, mit Hall in der Stimme. Er erzählt vom Herz des Rings am Eisrand und davon, dass man den Ring mit Seeglas aus allen sechs Kreisen schließen kann. Rufer erfahren zusätzlich, wie man ihn ganz öffnet („in jedem Kreis eine Lücke“) | `talk` |
| 5.5 | Heimbringen oder lassen | **Große Entscheidung** (bestätigt). Da Hallbjörn eine einzige Person ist, läuft sie als gemeinsames Gespräch | `choose` |
| 5.6 | Die zweite Kerbe | Wieder in der Siedlung leuchtet die zweite Kerbe | – |

- **Heimbringen:** Hallbjörn wird Bewohner mit eigenem Beruf. Vorschlag: „Glasstimmer“, der Seeglas für Schmuck und Runen veredelt. Toves Leute reagieren je nach Haltung der Spieler: Hüter vermitteln, Rufer nicht.
- **Im Eis lassen:** Er bleibt eine Stimme im Kreis. Für alle, die dabei waren, werden die verborgenen Strophen aller Lieder hörbar (bestätigte Folge). Bei Uneinigkeit gilt **im Eis lassen**, weil man ihn später noch holen kann (bestätigt 2026-11-21).
- **Systeme:** Kälte und Winter (Kälteresistenz der Gerichte), Schnee, Lawinen nur als Optik, Tiere (Bär, Wölfe), Lauschen (Nebelbilder der Ersten und des Ringvolks).
- **Questgegenstände:** Hallbjörns Stab mit Seeglas (Waffe oder Werkzeug, Vorschlag), Inschriften (ins Buch), Bärenfell des Felsenvaters (T5).
- **Nebenstränge:** Eishöhle mit gefrorenem Schiff (POI aus der Asset-Liste), Lied des Fjells, Jägerin: der weiße Hirsch im Winter.

#### Kapitel 6: Die Aschezunge (Glutküste, T5)

**Kern:** An der Glutküste sitzt der **Aschebund**. Sein Hohepriester, die **Aschezunge**, will den Ring ganz brechen, damit der Gezeitenhall frei durch das Land fließt und er ihn lenken kann. Hier erfährt man, was das Schließen kostet: Es nimmt der Welt einen Teil ihrer Magie.

| # | Etappe | Inhalt | Ziel-Typen |
|---|---|---|---|
| 6.1 | Asche auf der Schwelle | Hüter: Aschebund-Späher überfallen einen Weg nahe der Siedlung (mit Horden: Angriff auf die Siedlung). Rufer: Ein Bote bringt ein Aschesiegel, eine Einladung an die Glutküste | `talk` / `defeat` |
| 6.2 | Die Glutküste | Reise über Lavafelder: **Hitze und Durst** stärker (bestätigt: „Hitze später stärker“), Wasserschlauch mitnehmen, heiße Quellen als Rast, Dampfspalten (Verbrennung, Fallschaden) | `go_to` |
| 6.3 | Die Gefangenen | Im Lager des Bundes sitzen Menschen aus anderen Siedlungen, „die rufen können“. Befreien (Nebenstrang), mit Folgen: Sie ziehen als Bewohner zu oder gehen heim | `talk`, `defeat` |
| 6.3b | Das Rätsel des Ringvolks | Der Weg zum Steinkreis der Glutküste ist mit Runen verschlossen (bestätigt 2026-11-21). Am Tor liegen Seeglas- und Obsidian-Steine, dazu eine Inschrift des Ringvolks als Rätsel. Vorschlag: Man muss die Steine so legen, dass der Ring drei Lücken hat, wie das Zeichen. Wer Lieder der früheren Kreise kennt, hat Hinweise (Strophen nennen die Reihenfolge). Lauschen zeigt, welcher Stein „antwortet“. Keine Uhr, beliebig viele Versuche; falsches Legen kostet nichts | `puzzle` |
| 6.4 | Die Aschezunge | Gespräch im Tempel, am Steinkreis der Glutküste, der vom Bund mit Obsidian „geöffnet“ wurde. Die Aschezunge erklärt den Preis des Schließens und den Preis des Öffnens | `talk` |
| 6.5 | Der Weg der Asche | **Große Entscheidung**, je nach Haltung (siehe unten) | `choose`, `defeat` |
| 6.6 | Der Kreis der Asche | Lauschen. **Nebelbild 9:** Das Ringvolk bricht den Ring selbst, an drei Stellen, mit Absicht. Damit kommt die Frage auf, ob es den Ring überhaupt geschlossen haben wollte | `listen_at`, `vision` |

- **Große Entscheidung:**
  - **Hüter** kämpfen gegen die Aschezunge (Boss, Relikt-Material T6).
  - **Rufer** können sie **verraten** (Kampf von innen, leichter, der Bund wird zum Feind) oder ihr **folgen** (kein Bosskampf; sie kämpft im Finale an der eigenen Seite; dafür ist für diesen Spieler das Schließen des Rings im Finale nicht mehr möglich).
  - Weil die Aschezunge eine einzige Person ist, läuft das als gemeinsames Gespräch, wenn mehrere Spieler dabei sind. Bei Uneinigkeit **entkommt** sie und steht im Finale an der Seite des Seekönigs (bestätigt 2026-11-21).
- **Systeme:** Hitze, Durst, Kampf, Wetter (Asche-Regen als Optik).
- **Questgegenstände:** Aschesiegel, Obsidian (Handwerk), Relikt-Material T6.
- **Nebenstränge:** die Gefangenen, Lied der Glutküste, heiße Quellen (Rast-Buff, Vorschlag).

#### Kapitel 7: Der Sturm (Eisrand, T6, Finale)

**Kern:** Der **Seekönig** sammelt seine Flotte am Eisrand, wo im Gletscher das **Herz des Rings** liegt, der erste und älteste Kreis. Wer dort zuerst steht, entscheidet über den Gezeitenhall.

| # | Etappe | Inhalt | Ziel-Typen |
|---|---|---|---|
| 7.1 | Raben im Norden | Raben kündigen den Sturm an. Wie man zum Eisrand kommt, hängt von früheren Entscheidungen ab: mit **Hrefna als Lotsin** über den kurzen Seeweg, mit den **Schiffen des Jarls** (Eid), sonst über den langen Weg über den Gletscher | `talk`, `go_to` |
| 7.2 | Der Thing vor dem Sturm | Versammlung am Thingplatz. Wer kommt, hängt von früheren Entscheidungen ab: Jarl, Hallbjörn (heim oder als Stimme), Tove, Sæunn, Eyvind, die befreiten Gefangenen, die Aschezunge (wenn ihr ein Rufer folgte) | `talk` |
| 7.3 | Seeglas der sechs Kreise | Für das Schließen: aus jedem Kreis ein Splitter (man hat sie meist schon, sonst nachholen). Für das Öffnen: die Lücken, von denen Hallbjörn sprach | `collect` |
| 7.4 | Der Abschied | Mit Horden: Der Seekönig greift vor dem Aufbruch die Siedlung an. Ohne Horden: ein verbrannter Außenposten | `defeat` / `go_to` |
| 7.5 | Das Schiff im Eis | Bosskampf gegen den **Sturmkoloss**: realistisch der Seekönig auf seinem im Eis festgefrorenen Flaggschiff, mit Sagenwelt ein Frostriese. Sturm als Wetter (Sicht, Wind) | `defeat` |
| 7.6 | Das Herz des Rings | **Nebelbild 10:** das Ringvolk beim Binden des Rings, und warum es ihn an drei Stellen brach. Dann die **Finalentscheidung** (bestätigt): schließen oder offen lassen | `vision`, `choose` |
| 7.7 | Die Saga | Dritte Kerbe, der Skalde singt die Saga, Jarlssitz (`building_jarl_hall`) als Belohnung. Die Welt läuft weiter | – |

- **Folgen des Endes** (aus dem Kapitelplan):
  - **Schließen:** ruhige Gezeiten, Magie leiser (Lauschen-Reichweite sinkt, Seeglas wird selten).
  - **Offen lassen:** Gezeitenhall-Ereignisse häufiger, Seidr stärker, gezeichnete Kreaturen nur mit Sagenwelt.
- **Systeme:** Sturm, Kälte, Kampf, alle früheren Entscheidungen als Verbündete.
- **Questgegenstände:** Seeglas der sechs Kreise, Relikt-Material.

#### Umfang Kapitel 4–7 (ehrlich)

- Je Kapitel etwa 6 Etappen, 120–200 Sprechzeilen je Sprache, 1–2 Stunden Spielzeit.
- Alle vier Kapitel hängen an Biomen, die es noch nicht gibt (Heide, Fjell, Glutküste, Eisrand), an je einem Boss und am Kampfsystem.
- Das Finale ist das teuerste Kapitel: viele Verbündete, je nach Entscheidungen, mit eigenen Zeilen.
- Vorschlag: Das Finale bekommt Verbündete als **kurze Auftritte**, nicht als lange Gespräche, damit der Aufwand nicht explodiert.

**Finale im Koop** (bestätigt 2026-11-21): Die Finalentscheidung läuft als gemeinsames Gespräch. Sind die Spieler uneinig, bleibt der Ring offen, schließen geht später noch. Das ersetzt den früheren Vorschlag „zuerst gewähltes Ende“.


[↑ zum Inhaltsverzeichnis](#inhaltsverzeichnis)

<a id="d20"></a>

## 20. Kapitel 1: Spieltext (Entwurf)

> Quelle: `docs/18_kapitel_1_text.md`

Stand 2026-11-21. **Spieltext von Claude** (KI-generiert, Eintrag in `design/asset_log.md`). Grundlage: `docs/17_hauptstory_kapitel.md` (Kapitel 1K und 1H, bestätigter Kern), Stil und Mittel aus der Prolog-Probe `docs/15_probe_spur_der_ersten.md`.
Alles wird vertont (DE/EN, zuerst KI-Stimmen). Sprecherangaben in eckigen Klammern entsprechen den `spk_`-IDs.
- **Gedanke:** Stimme der Spielerfigur (4 wählbare Stimmen).
- **Saga-Buch:** liest der Erzähler (`spk_narrator`).
- **Nebelbilder:** halb verwehte Satzfetzen mit Hall.

Gesprächsantworten mit ◇ sind Hilfe-Fragen. Sie erscheinen nur, wenn die Questhilfe im Menü an ist.

**Neue Namen (Vorschlag):**
- **Sæunn**: Seherin, eine von Hallbjörns Gefährten; Dorf-Seherin auf dem Weg des Kreises.
- **Orm** und **Kolla**: die beiden anderen Gefährten.
- **Tove**: Asgerds Tochter; Dorf-Seherin auf dem Weg des Hofs.
- **Arnfinn**: ein alter Bootsbauer bei Tove.

---

## 1K: Der Kreis im Norden (Weg des Kreises)

Voraussetzung: `choice:choice_empty_stone=ring`. Ziel-Gespür: Stufe 1–2.

<a id="d20-1"></a>

### 20.1 1K.1 Der Ring wird warm

*Trägt man Hallbjörns Ring, wird das Summen im Norden mit der Zeit stärker, je näher man der Anhöhe kommt. Der Ring wirkt wie ein zweites, schwächeres Lauschen: Der Rand des Bildschirms schimmert nur in seine Richtung.*

**Hinweis** (einmalig): „Der Ring zieht. Folge ihm, wenn du willst.“
**Gedanke** (bei 200 m): „Es ist, als würde jemand in einem anderen Raum meinen Namen sagen.“

**Saga-Buch** (Aufgabe): *Hallbjörns Ring zog nach Norden, zu den Steinen auf der Anhöhe.*

<a id="d20-2"></a>

### 20.2 1K.2 Das Lager am Hang

*Unterhalb der Anhöhe: ein Unterstand aus Fellen und Ästen, ein Feuer, das nur glimmt, Knochen, Fischgräten. Drei Menschen, mager, in geflickter Wolle. Als die Figur näher kommt, steht ein Mann auf (Orm) und hebt einen Speer, nicht ganz. Eine Frau (Sæunn) bleibt sitzen und sieht nur auf die Hand der Figur, auf den Ring.*

**[Orm]** „Bleib, wo du bist. Wir haben nichts mehr, das sich zu stehlen lohnt.“
**[Sæunn]** „Lass ihn, Orm. Sieh auf seine Hand.“
*Orm senkt den Speer. Kolla, eine junge Frau, wendet das Gesicht ab.*
**[Sæunn]** „Das ist Hallbjörns Ring. Er hat ihn niemandem gegeben. Also hast du ihn genommen. Vom Stein, nehme ich an. Vom leeren.“

**Antworten:**
- „Ich habe ihn unter dem Moos gefunden. Ich wusste nicht, wem er gehört.“ → **[Sæunn]** „Jetzt weißt du es. Und er weiß es vermutlich auch.“
- „Er lag dort für den, der ihn braucht.“ → **[Sæunn]** *lächelt kurz.* „So hätte er es auch gesagt.“
- ◇ „Was ist mit dem Kreis dort oben?“ → **[Orm]** „Er schweigt. Seit dem Frühjahr. Seit das Vieh an der Quelle wühlt. Geh nachts hin, dann siehst du, was ich meine.“ *(schaltet `area_help` frei: Keilersuhle wird auf der Karte eingegrenzt)*

**[Sæunn]** „Hallbjörn ist weitergegangen. Wir drei sind geblieben, weil der Kreis ihn hier noch gehört hat. Jetzt hört er nichts mehr. Und wir auch nicht.“
**[Kolla]** *(leise, ohne aufzusehen)* „Wir haben Hunger. Das ist alles, was wir noch hören.“

**Kleine Entscheidung (Nebenstrang-Gewicht):** Etwas zu essen geben (ein beliebiges Gericht aus dem Inventar) oder nicht.
- **Geben:** **[Kolla]** „…Danke.“ *Sie isst sofort, mit beiden Händen.* Kolla wird später zugänglicher, und ihr Gesprächszweig öffnet sich: Sie weiß, wo Hallbjörn Seeglas versteckte (Nebenfund).
- **Nicht geben:** kein Vorwurf. **[Orm]** „Schon gut. Hier oben teilt niemand mehr gern.“

<a id="d20-3"></a>

### 20.3 1K.3 Die Suhle an der Quelle

*Oben auf der Anhöhe: sieben aufrechte Steine im Kreis, zwei umgestürzt, einer schief. In der Mitte eine Quelle, aufgewühlt zu Schlamm. Tiefe Spuren, Borsten am Stein. Bei Tag liegt der Ort still. Bei Nacht kommt der **Alte Keiler**: ein riesiges, narbiges Tier, das an den Steinen scheuert.*

**Gedanke** (beim ersten Blick auf die umgeworfenen Steine): „Er hat sie nicht umgestoßen. Er hat sich an ihnen gerieben, bis sie fielen.“
**Hinweis** (einmalig, wenn der Keiler auftaucht): „Ein Keiler dieser Größe flieht nicht. Mit dem Speer von vorn, mit dem Bogen aus der Höhe.“

**Bosskampf:** Der Alte Keiler, Stufe T2. Bis das Kampfsystem fertig ist (Phase 3), kämpft man mit dem einfachen Angriff (Speer, Bogen). Fällt man, liegt das Grab in der Suhle; Mitspieler können es nach 120 s bergen. Wölfe kommen nachts an den Rand des Kreises, angelockt vom Kampf.

**Saga-Buch** (nach dem Sieg, persönliche Tat): *{Spieler} erlegte den Alten Keiler an der Quelle des Kreises. Die Steine standen wieder still.*

<a id="d20-4"></a>

### 20.4 1K.4 Die Steine aufrichten

*Die zwei umgestürzten Steine lassen sich mit Werkzeug aufrichten (Bauhammer, E halten, mit Arbeitsbalken). Im Koop geht es schneller, wenn mehrere halten. Danach klärt sich das Wasser der Quelle, und zum ersten Mal ist das Summen hier oben deutlich, gleichmäßig, wie ein Atem.*

**[Sæunn]** *(kommt den Hang herauf, bleibt am Rand des Kreises stehen)* „Du hast ihn wieder aufgeweckt.“ *Pause.* „Lausch. Aber nicht am Tag. Am Tag ist er zu laut für das, was er sagen will.“

**Antworten:**
- „Warum nicht am Tag?“ → **[Sæunn]** „Weil am Tag alles andere auch redet. Der Wind. Das Meer. Du.“
- ◇ „Wann genau soll ich lauschen?“ → **[Sæunn]** „Wenn die Sonne weg ist. Oder wenn der Nebel kommt. Dann bleib in der Mitte stehen.“ *(Hilfe: Bedingung `night` oder `fog` wird im Saga-Buch genannt)*

<a id="d20-5"></a>

### 20.5 1K.5 Das Lied der Fjordwiesen (Nebelbild)

*Nachts oder im Nebel, aktives Lauschen in der Mitte: Steinkreis-Fund (+8 Gespür-EP beim ersten Mal). Der türkise Ring läuft langsamer als sonst und bricht an jedem Stein, als würde er zurückgeworfen. Dann das Nebelbild.*

**Nebelbild 4: Hallbjörn am Kreis**
*Eine einzelne Gestalt, ein großer Mann mit einem Beutel. Er legt Seeglas auf jeden Stein, einen Splitter nach dem anderen. Am letzten Stein zögert er.*
**[Hallbjörn, verweht]** „…nicht hier. Sie hören es hier… bis zum Meer…“
*Er nimmt den letzten Splitter wieder an sich. Er dreht sich nach Süden, zur Küste, und zeigt auf etwas, das man nicht sieht.*
**[Hallbjörn, verweht]** „…wo das Meer zweimal am Tag den Weg freigibt… dort hört mich niemand…“
*Er geht nach Süden. Der Nebel verweht.*

**Gedanke:** „Er hatte Angst, gehört zu werden. Nicht von den Steinen. Von Menschen.“

**Lied-Strophe** (ins Buch, `item_saga_fragment_fjord_1`, vom Kreis selbst „gesungen“, liest der Erzähler):
> *Sieben stehen am Wasser, das aus dem Berg kommt,*
> *sieben halten den Ton, den das Meer verlor.*
> *Wer ihn ruft, wird gehört, nah und fern.*
> *Wer ihn hält, wird nicht gefunden.*

**Saga-Buch** (Kapitelabschluss):
> *Am Kreis der Fjordwiesen sahen sie Hallbjörn im Nebel. Er hatte das Glas nicht gelegt, weil er fürchtete, wer es hören würde. Dann ging er nach Süden, in die Marsch, wo das Meer den Weg zweimal am Tag freigibt.*

**Offene Frage im Saga-Buch:** *„Vor wem hatte Hallbjörn Angst?“*

<a id="d20-6"></a>

### 20.6 1K.6 Abschied am Lager

**[Sæunn]** „Du gehst ihm nach.“ *Keine Frage.* „Dann nimm das mit.“ *Sie gibt einen kleinen Lederbeutel mit drei Seeglas-Splittern (normales Seeglas).* „Er hat sie mir gegeben, bevor er ging. Ich habe sie nie gelegt.“

**Kleine Entscheidung (Nebenstrang-Gewicht):**
- **Annehmen:** Seeglas (+1 je Stück) und **[Sæunn]** „Leg sie nicht leichtfertig.“ Sæunn bleibt am Kreis.
- **Zurückgeben, „Behalte sie für den Kreis“:** **[Sæunn]** *(lange Pause)* „Dann bleibe ich bei ihm.“ Sæunn bleibt ebenfalls am Kreis. Später öffnet sich in „Die Stimme im Seeglas“ ein zusätzliches Gespräch: Sie vertraut dir früher und kommt eher ins Dorf.

**Übergang:** Startet den Nebenstrang „Die Stimme im Seeglas“ (Sæunn als künftige Dorf-Seherin) und Kapitel 2K.

---

## 1H: Rauch am Fjord (Weg des Hofs)

Voraussetzung: `choice:choice_empty_stone=name`. Ziel-Gespür: Stufe 1.

<a id="d20-7"></a>

### 20.7 1H.1 Die Rauchsäule

*Die dünne Rauchsäule im Osten steht bei jedem Wetter, außer bei Sturm. Sie kommt aus einer Bucht, die man vom Strand aus nicht sieht. Der Weg führt am Fjordufer entlang, über Felsen, durch Birken.*

**Gedanke** (beim ersten Blick auf den Rauch vom Hügel): „Der war vorher nicht da. Oder ich habe nicht hingesehen.“
**Hinweis** (einmalig): „Rauch heißt Feuer. Feuer heißt Menschen.“

<a id="d20-8"></a>

### 20.8 1H.2 Das versteckte Lager

*Eine schmale Bucht, von außen durch eine Felsnase verdeckt. Zwei Hütten aus Treibholz, ein Trockengestell mit wenig Fisch, ein Boot an Land, der Rumpf eingedrückt. Sieben Menschen. Ein alter Mann (Arnfinn) arbeitet an dem Boot, ohne aufzusehen. Eine Frau um die dreißig (Tove) steht auf, ein Beil in der Hand.*

**[Tove]** „Halt. Wer schickt dich?“

**Antworten:**
- „Niemand. Ich habe euren Rauch gesehen.“ → **[Tove]** „Dann haben ihn andere auch gesehen.“ *Sie sieht kurz zum Feuer, dann zurück.*
- „Ich komme vom Hügel. Von den Gräbern.“ → **[Tove]** *(das Beil sinkt ein Stück)* „…Von den Gräbern.“

**[Tove]** „Wir haben letzte Woche gesehen, dass jemand dort oben war. Das Moos war weg.“ *Pause.* „Und auf dem fünften Stein steht ein Name. Den hat dort niemand von uns hingeschrieben.“

**Antworten:**
- „Ich habe ihn geritzt. Hallbjörn.“ → **[Tove]** „Warum?“
  - „Weil der Stein auf ihn gewartet hat.“ → **[Tove]** *(lange Pause)* „Meine Mutter hat ihn gegraben. Asgerd. Sie hat gesagt, er sei schon tot, als er noch vor ihr stand.“ *Sie legt das Beil auf den Boden.* „Komm ans Feuer. Es ist nicht viel, aber es ist warm.“
  - „Weil ich wollte, dass es zu Ende ist.“ → **[Tove]** „…Für sie war es nie zu Ende.“ *Sie legt das Beil auf den Boden.* „Komm ans Feuer.“

**Gedanke** (am Feuer): „Sieben. Asgerd hat siebzehn geschrieben.“

**Saga-Buch:**
> *In einer verborgenen Bucht fanden sie Tove, Asgerds Tochter, und sechs, die mit ihr überlebt hatten. Asgerd selbst lebte nicht mehr.*

<a id="d20-9"></a>

### 20.9 1H.3 Der Winter, der kommt

**[Tove]** „Wir sind hier, weil uns hier niemand sieht. Aber hier wächst nichts. Wir hatten eine Wiese oben am Bach, mit Flachs und Rüben. Ein Keiler hat sie umgegraben. Der Alte, sagen die Kinder. Er kommt nachts.“

**[Arnfinn]** *(vom Boot aus, ohne sich umzudrehen)* „Und das Boot fährt nicht mehr. Also bleiben wir. Also brauchen wir Essen. So einfach ist das.“

**Antworten:**
- „Was braucht ihr?“ → **[Tove]** „Vorräte für den Winter. Etwas, das sich hält. Und Wasser, das nicht nach Salz schmeckt.“
- ◇ „Wo ist die Wiese?“ → **[Tove]** „Den Bach hinauf, bis zu den zwei Birken, die zusammengewachsen sind. Er kommt, wenn es dunkel ist.“ *(Hilfe: Wiese auf der Karte eingegrenzt)*

**Aufgaben (Reihenfolge frei, keine Frist):**
- **Vorräte:** 10 haltbare Gerichte oder Lebensmittel bringen (z. B. Trockenfisch, Haselnüsse, gekochte Gerichte; Verderb zählt). Was man bringt, legt man in ihren Vorratskorb.
- **Wasser:** einen Regenfänger im Lager bauen (Rezept bekannt, bestätigt: 1 Baumstamm, 1 großer Stein, 3 kleine Steine, 5 Äste).
- **Die Wiese:** den Alten Keiler erlegen.

**Gedanke** (wenn ihr Winter kommt, bevor alles erledigt ist): „Sie werden es schaffen, irgendwie. Aber nicht gut.“ *(kein Scheitern, die Aufgaben bleiben offen)*

<a id="d20-10"></a>

### 20.10 1H.4 Der Alte Keiler

*Die Wiese am Bach: umgewühlte Erde, ein zerbrochener Zaun, Rübenreste. Nachts kommt der Keiler, derselbe Boss wie im Weg des Kreises.*

**Bosskampf:** wie in 1K.3, aber auf offener Wiese. Mehr Platz, keine Steine als Deckung, dafür der Bach als Hindernis für den Keiler.

**[Tove]** *(nach dem Sieg, am nächsten Morgen in der Bucht)* „Du hast ihn erlegt. Allein?“
- Solo: „Allein.“ → **[Tove]** „Meine Mutter hätte dich gemocht. Sie mochte Leute, die zu dumm waren, um Angst zu haben.“
- Koop: „Nicht allein.“ → **[Tove]** „Dann seid ihr mehr, als ich dachte. Gut.“

<a id="d20-11"></a>

### 20.11 1H.5 Asgerds Kamm

*Wenn Vorräte, Wasser und Wiese erledigt sind: Tove holt etwas aus der Hütte, ein Kamm aus Knochen, mit eingeritztem Zeichen (der offene Ring mit drei Kerben).*

**[Tove]** „Er gehörte meiner Mutter. Sie hat dieses Zeichen überall hingeritzt, auch wenn sie es gehasst hat. Sie sagte, man soll wissen, wovor man sich hütet.“ *Sie gibt ihn der Figur.* „Behalte ihn. Wer auf dem Hügel einen Namen schreibt, hat ein Andenken verdient.“

**Fund: Asgerds Kamm** (ins Buch, Erbstück)
> Knochen, abgegriffen, drei Zinken fehlen. In den Griff ist der offene Ring mit drei Kerben geritzt, so tief, dass er fast durchgeht.

**[Arnfinn]** *(zum ersten Mal mit der Figur)* „Und jetzt das Boot.“ *Er klopft auf den Rumpf.* „Nicht dieses. Das andere. Unser erstes. Die Plünderer haben es mitgenommen, als sie kamen. Darin war Asgerds Saatkiste: Gerste aus der Heimat, Flachs, alles. Das Boot liegt draußen in der Marsch, hab ich gehört. Auf einem Wrack. Bei einer, die sie die Schlickmutter nennen.“

**[Tove]** „Arnfinn.“
**[Arnfinn]** „Was? Sie hat gefragt, was wir brauchen. Das brauchen wir.“

<a id="d20-12"></a>

### 20.12 1H.6 Die Frage des Ortes

**Kleine Entscheidung (Nebenstrang-Gewicht, bestätigtes Muster aus dem Kapitelplan):**
- **„Kommt mit in meine Siedlung.“** → **[Tove]** „…Nicht jetzt. Erst, wenn wir wissen, dass uns dort keiner findet. Wenn das Boot zurück ist, reden wir weiter.“ Sie ziehen nach Kapitel 3 als Bewohner ein.
- **„Ich helfe euch, hier zu bleiben.“** → **[Tove]** „Dann bleiben wir. Und du kommst, wann du willst. Wir haben Fisch und Leinen, mehr als wir brauchen.“ Die Bucht wird ein Handelsposten.

**Saga-Buch** (Kapitelabschluss):
> *Sie halfen Toves Leuten über den Winter und erlegten den Keiler auf ihrer Wiese. Tove gab ihnen den Kamm ihrer Mutter. Und Arnfinn sprach von einem Boot draußen in der Marsch, auf dem Wrack der Schlickmutter, mit Asgerds Saat darin.*

**Offene Frage im Saga-Buch:** *„Siebzehn kamen. Wer fehlt noch?“* (4 auf dem Hügel, 4 mit Hallbjörn, Asgerd tot, 7 bei Tove = 16; siehe Anmerkung)

**Übergang:** Startet „Die Stimme im Seeglas“ (Tove als künftige Dorf-Seherin) und Kapitel 2H.

---

<a id="d20-13"></a>

### 20.13 Anmerkungen für das Team

- **Rechenfehler im Prolog:** Die Kapitelzeile in `docs/15_probe_spur_der_ersten.md` sagt „Acht sind noch irgendwo“. Richtig ist **neun** (17 − 4 auf dem Hügel − 4 mit Hallbjörn). Korrektur dort: „Neun sind noch irgendwo“.
- **Zählung der Ersten (bestätigt 2026-11-21, der Siebzehnte ist der Lotse):** 17 gesamt = 4 auf dem Gräberhügel + Hallbjörn mit 3 Gefährten (Sæunn, Orm, Kolla) + Asgerd (tot, nach dem Prolog) + Tove mit 6 = 16. **Eine Person fehlt.** Vorschlag: Das ist der „Lotse“, der die Plünderer zur Bucht führte. Er taucht in Kapitel 2 bei der Schlickmutter auf (Verräter oder Gefangener, je nach Weg). Damit wäre auch die offene Frage im Saga-Buch eine echte Spur.
- **Gemeinsame Orte:** Beide Wege nutzen denselben Keiler und dieselbe Bossarena-Logik, aber andere Orte (Kreis bzw. Wiese). Gespräche und Belohnungen sind getrennt.
- **Koop:** Haben zwei Spieler verschiedene Wege, sieht jeder seine eigenen Gespräche. Das Lager bei Tove und das Lager am Kreis existieren beide in der Welt (Folgen addieren sich), sprechen aber nur mit dem, dessen Weg dazu passt; die anderen bekommen eine kurze, freundliche Abfuhr.
- **Länge:** etwa 90–150 Sprechzeilen je Weg und Sprache, Spielzeit 1–2 Stunden je Weg (mit Sammeln, Bauen, Jagd).
- **Keine Etappe braucht Horden;** mit Weltoption Horden kann der Rauch der Bucht nachts Plünderer-Kundschafter anlocken (nur Atmosphäre, kein Angriff auf die eigene Siedlung in Kapitel 1).


[↑ zum Inhaltsverzeichnis](#inhaltsverzeichnis)

<a id="d21"></a>

## 21. Kapitel 2: Spieltext (Entwurf)

> Quelle: `docs/19_kapitel_2_text.md`

Stand 2026-11-21. **Spieltext von Claude** (KI-generiert, Eintrag in `design/asset_log.md`). Grundlage: `docs/17_hauptstory_kapitel.md` (Kapitel 2K und 2H, bestätigter Kern), Kapitel 1 in `docs/18_kapitel_1_text.md`.
Schreibweise wie in Kapitel 1:
- `[Sprecher]` = vertonte Zeile;
- **Gedanke** = Stimme der Spielerfigur;
- **Saga-Buch** = Erzähler;
- ◇ = Hilfe-Frage, nur mit Questhilfe.

Das Kapitel braucht das **Kampfsystem (Phase 3)**, außerdem **Ebbe und Flut** (bestätigt: zweimal pro Spieltag, je 15 Minuten wechselnd).

**Neue Figuren (Vorschlag):**
- **Die Schlickmutter**, eigentlich **Hrefna**: Plündererin des Seekönigs, Herrin eines gestrandeten Wracks in der Marsch.
- **Eyvind, der Lotse**: der Siebzehnte der Ersten (bestätigt 2026-11-21). Er führte die Plünderer zur Bucht. Jetzt dient er Hrefna, angekettet bei Nacht, als Lotse durch die Priele bei Tag.

**Die Marsch, für beide Wege gleich:**
- **Flut:** Priele laufen voll, Wege verschwinden. Wer im Wasser steht, wird langsamer und kühlt aus (Kälte). Es gibt keinen Tod durch Ertrinken, solange man nicht taucht: Die Figur schwimmt zum nächsten Steg (Vorschlag).
- **Ebbe:** Schlick, Muschelbänke, das Wrack und der Steinkreis im Watt liegen frei. Im Schlick ist man langsamer, und die Ausdauer erholt sich schlechter (keine Sperre).
- **Nebel** liegt oft am Morgen über der Marsch. Das Lauschen wird stärker (+1 Stufe, bestätigt).
- **Fällt man im Schlick**, liegt das Grab dort. Bei Flut ist es unter Wasser, aber nicht verloren: Bei Ebbe kann man es bergen, Mitspieler nach 120 s.

---

## 2K: Was die Ebbe freigibt (Weg des Kreises)

Voraussetzung: `quest_done:quest_fo_1k_end`. Ziel-Gespür: Stufe 2.

<a id="d21-1"></a>

### 21.1 2K.1 Am Rand der Marsch

*Hallbjörns Ring zieht nach Süden, über die Fjordwiesen hinaus, bis das Land flach wird. Schilf, Weiden, Priele, die im Licht glänzen. Bei Flut sieht man nur Wasser und Schilfinseln.*

**Gedanke** (bei Flut am Marschrand): „Zweimal am Tag gibt das Meer den Weg frei. Also warte ich.“
**Hinweis** (einmalig): „Bei Ebbe kommt man weiter. Die Flut kommt wieder, aber sie kommt auch wieder weg.“

*Bei Ebbe, im Schlick: Pfahlreste eines alten Stegs, Muschelschalen, Fußspuren, die zu alt sind, um noch da zu sein, und trotzdem da sind (Schlick hält Spuren).*

<a id="d21-2"></a>

### 21.2 2K.2 Der Kreis im Watt

*Weit draußen, nur bei Ebbe: sieben flache Steine, halb im Schlick, mit Seepocken bewachsen. Auf einem davon liegen noch drei Splitter Seeglas, eingewachsen in Muschelkalk. Das Summen ist hier anders als auf der Anhöhe: tiefer, langsamer, wie Wasser, das zurückläuft.*

**Steinkreis-Fund** beim ersten Lauschen: +8 Gespür-EP.

**Nebelbild 5: Das Licht** (*bei Ebbe und Nacht oder Nebel; sonst bleibt es bei der Inschrift*)
*Hallbjörn kniet im Schlick, allein. Er legt Splitter auf die Steine, einen nach dem anderen, und diesmal legt er auch den letzten. Der Kreis leuchtet auf, nicht kurz, sondern lange, ein ruhiges, türkises Licht, das sich auf dem nassen Watt spiegelt bis zum Horizont.*
**[Hallbjörn, verweht]** „…hier hört es niemand… nur das Meer…“
*Er lacht. Dann hört er auf zu lachen. Weit draußen, auf dem Wasser, tauchen Umrisse auf: zwei, drei Segel, die näher kommen. Hallbjörn steht auf, reißt die Splitter von den Steinen, einen nach dem anderen, zu spät. Das Licht verlischt. Die Segel bleiben.*
**[Hallbjörn, verweht]** „…nein… nein, nicht hierher… nicht zu ihnen…“

**Gedanke:** „Er hat sie gerufen. Nicht die Steine. Die Schiffe.“

**Saga-Buch:**
> *Im Watt fanden sie den Kreis, an dem Hallbjörn das Glas gelegt hatte, weil er glaubte, nur das Meer würde ihn hören. Das Meer war nicht allein.*

<a id="d21-3"></a>

### 21.3 2K.3 Große Entscheidung: Den Kreis rufen lassen?

*Die drei eingewachsenen Splitter lassen sich lösen (Spitzhacke, vorsichtig). Danach kann man sie mitnehmen oder auf die drei leeren Steine legen.*

**Gedanke** (beim Lösen des ersten Splitters): „Wenn ich sie lege, singt der Kreis. Wenn ich sie lege, sieht man es.“

| Wahl | Folge |
|---|---|
| **Die Splitter legen, den Kreis rufen lassen** | Der Kreis leuchtet in jeder Ebbe-Nacht. Das **Lied der Marsch** wird vollständig (zwei Strophen mehr). Es gibt deutlich mehr Gespür-EP: Der Kreis zählt als Ereignis, +15 einmal (bestätigte Quelle „Gezeitenhall-Ereignis“). Nebelbild 6 wird möglich (Hallbjörns Weg nach Norden, er hilft in Kapitel 5). **Aber:** Das Licht sieht man vom Meer aus. Hrefnas Leute am Wrack sind gewarnt und zahlreicher. Mit Weltoption Horden kommt einmal ein Plünderertrupp an die Küste nahe der eigenen Siedlung (Angriff); ohne Horden nur mehr Wachen am Wrack |
| **Die Splitter mitnehmen, den Kreis stumm lassen** | Sicher: kein Licht, das Wrack ist schwächer bewacht. Man hat 3 Seeglas mehr (normal nutzbar). Das Lied der Marsch bleibt lückenhaft (eine Strophe), Nebelbild 6 kommt nicht. Hallbjörns Spur wird kälter: In Kapitel 5 ist die Suche nach ihm länger (ein zusätzlicher Ort im Fjell) |

**Saga-Buch** (je nach Wahl):
> *{Spieler} legte das Glas zurück auf die Steine. Der Kreis im Watt sang, und sein Licht lag bis zum Horizont.*
> *{Spieler} nahm das Glas von den Steinen. Der Kreis im Watt blieb stumm, und niemand auf dem Meer sah etwas.*

**Lied der Marsch** (Strophe 1 immer, 2 und 3 nur beim Rufen):
> *Was das Meer nimmt, gibt es zweimal am Tag zurück,*
> *nur nicht die Stimme, die es einmal verlor.*
> *(2) Wer sie ihm leiht, leiht sie allen,*
> *(3) und alle, die hören, kommen.*

<a id="d21-4"></a>

### 21.4 2K.4 Das Wrack der Schlickmutter

*Draußen, wo Marsch und Meer sich mischen: ein großes Schiff auf der Seite, im Schlick festgefahren, mit Plattformen und Stegen ausgebaut zu einem Lager. Rauch, Netze, Fässer. Bei Flut ist es eine Insel, bei Ebbe erreicht man es über die Muschelbänke. Auf dem Deck, unter einem Segeltuch: Hrefna, die Schlickmutter, eine breite Frau mit grauem Zopf, Schlamm an den Stiefeln bis zum Knie. Neben dem Mast, an einer Kette am Knöchel: ein hagerer Mann, Eyvind.*

*Man kann sich anschleichen (Nebel hilft) oder offen kommen. Hrefna spricht, wenn man offen kommt, bevor gekämpft wird.*

**[Hrefna]** „Noch einer aus dem Norden. Ihr kommt immer bei Ebbe. Das Meer schickt euch mir wie Treibholz.“ *Sie sieht den Ring.* „Ah. Du trägst sein Ding. Dann weißt du, wer uns gerufen hat.“

**Antworten:**
- „Er hat euch nicht gerufen. Er hat das Meer gerufen.“ → **[Hrefna]** „Das Meer und wir sind dasselbe, Kind. Frag den Lotsen.“ *Sie nickt zu Eyvind.*
- „Wo ist Hallbjörn?“ → **[Hrefna]** „Weg. Er ist gerannt, als er die Segel sah. Nach Norden. Wir haben die anderen genommen. Die, die nicht schnell genug waren.“
- ◇ „Was willst du?“ → **[Hrefna]** „Leute, die das Glas zum Singen bringen. Für die gibt es Silber, im Süden. Viel Silber.“ *(Hilfe: Saga-Buch vermerkt „Wer zahlt für die, die rufen können?“)*

**[Hrefna]** „Und jetzt gibst du mir den Ring, oder du gehst wieder ins Wasser.“

**Bosskampf:** Die Schlickmutter, Stufe T3 (Mensch, Kampfsystem Phase 3). Bei Ebbe auf Deck und Muschelbank; **kommt während des Kampfes die Flut**, läuft die Muschelbank voll und der Kampf zieht sich auf das Deck zurück (kleinere Fläche, kein Scheitern). Hat man den Kreis rufen lassen, kämpfen zwei Wachen mehr mit.

<a id="d21-5"></a>

### 21.5 2K.5 Der Lotse

*Nach dem Sieg (Hrefna fällt oder flieht über Bord; siehe Anmerkungen zum Nachspawnen): Eyvind an der Kette.*

**[Eyvind]** „Bist du von ihnen? Vom Hügel? Du trägst Hallbjörns…“ *Er bricht ab.* „Dann weißt du, was ich bin.“

**Antworten:**
- „Was bist du?“ → **[Eyvind]** „Der, der sie in die Bucht geführt hat. Ich kannte die Priele. Sie hatten ein Messer an Asgerds Hals, auf dem Boot. Ich habe gesagt: Ich zeige euch den Weg, wenn ihr sie loslasst. Sie haben sie losgelassen. Und dann habe ich ihnen den Weg gezeigt.“
- „Einer der Ersten.“ → **[Eyvind]** „Der Siebzehnte. Der, den keiner mitzählt.“

**Kleine Entscheidung (Nebenstrang-Gewicht):**
- **Kette lösen:** **[Eyvind]** „…Ich weiß nicht, wohin ich gehen soll.“ Er geht in die eigene Siedlung und wartet am Rand. In Kapitel 3 steht er am Thingplatz.
- **Ihn lassen, wo er ist:** **[Eyvind]** „Dann bleibe ich beim Meer. Das hat mich wenigstens nie belogen.“ Er bleibt auf dem Wrack. In Kapitel 3 kommt er nicht; seine Seite des Logbuchs fehlt (eine Lücke im Buch).

<a id="d21-6"></a>

### 21.6 2K.6 Das Logbuch

*In Hrefnas Kiste unter Deck: Hallbjörns Seeglas-Beutel (noch 5 Splitter, normales Seeglas) und ein in Wachstuch gewickeltes Buch, nass, die Seiten verklebt.*

**Fund: Logbuch der Ersten, zweiter Teil** (`item_logbook`, ins Buch; **noch nicht lesbar**)
> Die Seiten sind vom Salzwasser verklebt. Auf dem Umschlag, mit Kohle: *A.* Man müsste es trocknen, langsam, an einem Feuer, das nicht zu heiß ist.

**Gedanke:** „Asgerds zweites Buch. Und ich kann es nicht lesen.“

**Saga-Buch** (Kapitelabschluss):
> *Auf dem Wrack der Schlickmutter fanden sie den Siebzehnten der Ersten, Eyvind den Lotsen, und Asgerds zweites Logbuch, nass und verklebt. Was darin stand, musste warten, bis es trocken war.*

**Übergang:** Kapitel 3 beginnt, sobald das Logbuch in der eigenen Siedlung an einem Feuer getrocknet wird (Lagerfeuer oder Herdstelle, E, Arbeitsbalken; Dauer etwa ein halber Spieltag, ohne Frist).

---

## 2H: Das Boot der Ersten (Weg des Hofs)

Voraussetzung: `quest_done:quest_fo_1h_end`. Ziel-Gespür: Stufe 1–2.

<a id="d21-7"></a>

### 21.7 2H.1 Arnfinns Karte

*In der Bucht: Arnfinn hat mit Kohle auf ein Brett gezeichnet, wie die Priele laufen. Er zeigt sie nur einmal.*

**[Arnfinn]** „Bei Ebbe über die Muschelbänke, dann rechts vom großen Pfahl, nie links, links ist der Schlick so tief, dass er dir die Stiefel nimmt. Und wenn das Wasser kommt, geh nicht zurück. Geh hoch. Auf alles, was höher ist als du.“

**Antworten:**
- „Kommst du mit?“ → **[Arnfinn]** „Mit den Knien? Ich komme mit, wenn ihr das Boot habt. Dann segle ich es heim.“
- ◇ „Wo genau liegt das Wrack?“ → **[Arnfinn]** „Da, wo der Rauch aus dem Meer kommt. Du siehst es bei Ebbe vom letzten Weidenbusch aus.“ *(Hilfe: Wrack auf der Karte eingegrenzt)*

**[Tove]** *(leise, als Arnfinn weg ist)* „Er war es, der das Boot gebaut hat. Daheim. Bevor wir losfuhren. Er redet nicht darüber.“

**Fund: Arnfinns Brett** (normale Karte; die Priele erscheinen auf der eigenen Karte)

<a id="d21-8"></a>

### 21.8 2H.2 Durch die Marsch

*Wie in 2K.1, aber man folgt Arnfinns Weg. Wer links vom großen Pfahl geht, sinkt tiefer ein (stark verlangsamt, kein Tod).*

**Gedanke** (beim Pfahl, wenn man links gegangen ist): „Arnfinn hatte recht. Natürlich hatte er recht.“

*Unterwegs, bei einer zerbrochenen Pfahlhütte: der verlassene Torfstich der Plünderer. Getrockneter Torf, gut als Brennstoff (Nebenfund).*

<a id="d21-9"></a>

### 21.9 2H.3 Das Wrack und das Boot

*Wie in 2K.4: das Wrack, Hrefna, Eyvind an der Kette. Daneben, auf Pfählen hochgebockt, ein kleineres Boot, sorgfältig gebaut, mit einem geschnitzten Vordersteven: der offene Ring mit drei Kerben.*

**Gedanke:** „Das ist es. Das ist ihr Boot.“

**[Hrefna]** „Noch einer aus dem Norden.“ *Sie folgt dem Blick der Figur zum Boot.* „Ah. Wegen dem da. Ein gutes Boot. Zu gut für die, die es gebaut haben. Willst du es? Dann kauf es. Oder hol es.“

**Antworten:**
- „Es gehört Asgerds Leuten.“ → **[Hrefna]** „Asgerd ist tot, hab ich gehört. Tote besitzen nichts.“
- „Was willst du dafür?“ → **[Hrefna]** „Leute, die das Glas zum Singen bringen. Kennst du welche? Für die zahlt man im Süden in Silber.“
- ◇ „Wer zahlt dafür?“ → **[Hrefna]** „Leute mit Asche im Gesicht. Frag nicht weiter, Kind.“ *(Hilfe: Saga-Buch vermerkt „Wer sind die mit Asche im Gesicht?“)*

**Bosskampf:** wie in 2K.4 (ohne Zusatzwachen).

<a id="d21-10"></a>

### 21.10 2H.4 Große Entscheidung: Die Schlickmutter

*Fällt Hrefnas Leben auf ein Viertel, bricht ihre Haltung (Kampfsystem: Haltungsleiste). Sie geht in die Knie, das Beil im Schlick.*

**[Hrefna]** „Na los. Oder hast du keinen Magen dafür?“

| Wahl | Folge |
|---|---|
| **Sie töten** | Volle Bossbeute (erster Sieg: Bossmaterial T3). Das Lager zerfällt, ihre Leute fliehen. Toves Leute sind erleichtert. In Kapitel 7 fehlt eine Lotsin zum Eisrand: Man findet den Weg selbst, über einen längeren Weg |
| **Sie gefangen nehmen** | Hrefna redet: Der **Seekönig** sammelt für den **Aschebund** Menschen, „die rufen können“. Das bringt einen frühen, klaren Hinweis auf Kapitel 4–7 (Saga-Buch). Sie wird Gefangene in der eigenen Siedlung (eine Figur am Pranger oder in einer Hütte, mit Gespräch) und kann in Kapitel 7 als Lotsin zum Eisrand dienen. **Aber:** Tove und ihre Leute sind entsetzt. „Du bringst sie dorthin, wo wir wohnen sollen?“ Ihr Zuzug verzögert sich um ein Kapitel. Wurde der Handelsposten gewählt, ist der Handel bis Kapitel 4 teurer |

**[Hrefna]** *(gefangen, auf dem Weg)* „Weißt du, warum der Norden so voll ist von Leuten wie mir? Weil im Süden einer bezahlt. Und der bezahlt gut.“

**Saga-Buch** (je nach Wahl):
> *Auf dem Wrack in der Marsch fiel die Schlickmutter durch die Hand von {Spieler}.*
> *Auf dem Wrack in der Marsch zwang {Spieler} die Schlickmutter in die Knie und nahm sie gefangen. Was sie erzählte, ließ Tove nicht mehr schlafen.*

<a id="d21-11"></a>

### 21.11 2H.5 Eyvind

*Wie in 2K.5, aber Eyvind sieht nicht auf den Ring, sondern auf den Kamm, wenn man Asgerds Kamm trägt (im Inventar oder im Buch).*

**[Eyvind]** „Woher hast du den?“ *Pause.* „Tove. Natürlich. Lebt sie?“

**Antworten:**
- „Sie lebt. Sie hat sieben Leute durch einen Winter gebracht.“ → **[Eyvind]** *(schließt die Augen)* „Sag ihr nicht, dass du mich gesehen hast.“
- „Sie lebt. Und sie weiß, wer die Plünderer gebracht hat.“ → **[Eyvind]** „Dann weiß sie mehr als ich. Ich weiß nur noch, dass ich es getan habe.“

**Kleine Entscheidung (Nebenstrang-Gewicht):** wie in 2K.5 (Kette lösen oder ihn lassen). Wird er gelöst, folgt er nicht in die Bucht, sondern wartet am Rand der eigenen Siedlung: **[Eyvind]** „Zu Tove kann ich nicht. Noch nicht.“

<a id="d21-12"></a>

### 21.12 2H.6 Das Boot kommt heim

*Bei der nächsten Flut schwimmt das Boot auf. Arnfinn kommt mit zwei Leuten von Tove (wenn das Wrack frei ist, ohne Frist; er wartet auf die Flut). Er legt die Hand auf den Vordersteven, sagt nichts, lange.*

**[Arnfinn]** „Na also.“ *Mehr sagt er nicht.*

*Im Boot, unter den Ruderbänken, verschnürt: Asgerds Saatkiste. Darin, in Wachstuch, ein zweites Buch.*

**Fund: Asgerds Saatkiste** (normaler Gegenstand in das eigene Lager; Ursaat: Gerste, Flachs, Möhren mit höherem Ertrag im Feldbau, Vorschlag +25 %)
**Fund: Logbuch der Ersten, zweiter Teil** (wie in 2K.6: nass, verklebt, muss getrocknet werden)

**[Tove]** *(in der Bucht, als das Boot einläuft)* „Das ist ihre Schrift.“ *Sie berührt den Umschlag, nicht die Seiten.* „Trockne es. Aber nicht hier. Ich will nicht dabei sein, wenn es aufgeht.“

**Saga-Buch** (Kapitelabschluss):
> *Arnfinn segelte das Boot der Ersten zurück in die Bucht. Unter den Bänken lag Asgerds Saat, und in Wachstuch ihr zweites Buch, nass vom Meer. Tove wollte es nicht öffnen.*

**Übergang:** wie in 2K.6. Kapitel 3 beginnt mit dem Trocknen des Logbuchs in der eigenen Siedlung.

---

<a id="d21-13"></a>

### 21.13 Anmerkungen für das Team

- **Menschliche Bosse und „alles spawnt nach“** (bestätigt): Eine Figur wie Hrefna kann nach dem Tod nicht einfach wieder auftauchen. Vorschlag: Nach `boss_respawn_days` (3) übernimmt **eine neue Kapitänin** das Wrack (andere Figur, gleiche Boss-Werte, Name „Die Schlickmutter“ als Titel). Wurde Hrefna gefangen genommen, gilt dasselbe. So bleibt Hrefna als Person einmalig, der Boss aber wiederholbar. Gilt sinngemäß für alle menschlichen Bosse. **Bestätigt 2026-11-21.**
- **Koop mit beiden Wegen:** Wrack, Hrefna und Eyvind gibt es einmal in der Welt. Hrefnas Gespräch hängt vom Weg des Sprechenden ab (Ring oder Kamm). Geht sie in die Knie und stehen mehrere Spieler dabei, öffnet sich ein **gemeinsames Gespräch** für alle in der Nähe: Jeder wählt, es läuft keine Uhr. Sind sich alle einig, gilt das. Sind sie uneinig, gilt „gefangen nehmen“, und töten bleibt später in der Siedlung möglich (bestätigt 2026-11-21).
- **Logbuch trocknen** verbindet Kapitel 2 mit der eigenen Siedlung: Man muss heim, statt draußen weiterzuziehen. Das passt zu „Siedlung als Anker“, ist aber keine Frist.
- **Länge:** etwa 120–160 Sprechzeilen je Weg und Sprache.
- **Horden:** nur beim Rufen des Kreises (2K) und nur mit Weltoption ein einmaliger Plünderertrupp an der Küste.


[↑ zum Inhaltsverzeichnis](#inhaltsverzeichnis)

<a id="d22"></a>

## 22. Kapitel 3: Das zweite Logbuch (Spieltext, Entwurf)

> Quelle: `docs/20_kapitel_3_text.md`

Stand 2026-11-21. **Spieltext von Claude** (KI-generiert, Eintrag in `design/asset_log.md`). Grundlage: `docs/17_hauptstory_kapitel.md` (Kapitel 3, bestätigter Kern), Kapitel 1 und 2 in `docs/18_kapitel_1_text.md` und `docs/19_kapitel_2_text.md`.
Die Schreibweise ist dieselbe wie dort:
- `[Sprecher]` = vertonte Zeile;
- **Gedanke** = Stimme der Spielerfigur;
- **Saga-Buch** = Erzähler;
- ◇ = Hilfe-Frage.

Beide Wege laufen hier wieder zusammen (bestätigt). Kapitel 3 spielt **in der eigenen Siedlung**. Es gibt keinen Boss und keinen Kampf. Die große Entscheidung ist die **Haltung „Hüter oder Rufer“** (bestätigt 2026-11-16). Jeder Spieler wählt für sich. Am Ende leuchtet die **erste Kerbe**.

**Wer in der Welt auftaucht, hängt davon ab, was in dieser Welt gewählt wurde.** Die Folgen addieren sich:
- **Sæunn, Orm, Kolla** kommen, wenn mindestens ein Spieler den Weg des Kreises gegangen ist.
- **Tove und Arnfinn** kommen, wenn mindestens ein Spieler den Weg des Hofs gegangen ist. Hat jemand Hrefna gefangen genommen, kommen sie erst nach diesem Kapitel; dann vertritt sie Arnfinn allein.
- **Eyvind** kommt, wenn jemand seine Kette gelöst hat.
- **Hrefna** ist da, wenn jemand sie gefangen genommen hat.

---

<a id="d22-1"></a>

### 22.1 3.1 Das Buch am Feuer

*Das nasse Logbuch wird an ein Feuer in der eigenen Siedlung gelegt (Lagerfeuer oder Herdstelle, E, Arbeitsbalken). Es trocknet etwa einen halben Spieltag lang, ohne Frist: Man kann weggehen und wiederkommen. Am Feuer liegt es sichtbar auf einem flachen Stein, die Seiten wellen sich langsam.*

**Gedanke** (beim Hinlegen): „Langsam. Sie hat lange genug gewartet.“

*Ist es trocken, lassen sich die Seiten lösen. Man liest sie am Feuer. Asgerd spricht jede Seite (`spk_asgerd`), leise, mit Pausen. Im Koop hören alle am Feuer mit (Mitspieler in der Nähe, bestätigt).*

<a id="d22-2"></a>

### 22.2 3.2 Die Seiten

**Logbuch der Ersten, zweiter Teil, Seite 1** (`lore_asgerd2_01`)
> *Neununddreißigster Tag. Die Nacht, in der das Meer hell war.*
> Wir haben das Licht von der Bucht aus gesehen. Grün wie das Glas, flach über dem Wasser, bis an den Rand der Welt. Die Kinder haben gejubelt. Ich habe sie ins Haus geschickt und die Tür verriegelt, und ich weiß bis heute nicht, ob ich es gewusst habe oder nur Angst hatte.
> Am dritten Morgen danach waren Segel in der Marsch.

**Seite 2** (`lore_asgerd2_02`)
> Sie kamen bei Ebbe, zu Fuß, durch die Priele, als wären sie hier geboren. Eyvind ging vorn. Sie hatten ein Messer an meinem Hals, damit er geht, und er ist gegangen. Ich habe ihn angeschrien. Er hat sich nicht umgedreht.
> Ich schreibe das auf, damit es stimmt: Er hat mein Leben gegen das der anderen getauscht. Ich weiß nicht, ob ich ihm das verzeihen soll. Ich weiß nicht, ob ich mir verzeihen soll, dass ich noch lebe.

**Seite 3** (`lore_asgerd2_03`)
> Sie haben nach dem gefragt, der das Meer zum Leuchten bringt. Immer wieder. Nicht nach Silber, nicht nach Vieh. Nach ihm.
> Zwei von ihnen trugen Asche auf den Wangen, in Streifen, wie Krieger Farbe tragen. Sie haben nicht geplündert. Sie haben zugesehen. Als sie Hallbjörn nicht fanden, haben sie Ulf und Gyda mitgenommen, um zu sehen, ob sie es auch können. Am nächsten Tag lagen beide am Strand.

**Seite 4** (`lore_asgerd2_04`)
> *Erster Winter.*
> Hallbjörn kam in der Nacht, als alles vorbei war. Er wollte zu den Steinen im Norden, weiter, als sie ihn je suchen würden. Er sagte, er werde nie wieder rufen. Ich sagte, dann bist du schon tot. Ich habe ihm das Grab gegraben, während er zusah.
> Er hat seinen Ring daraufgelegt. Ich hätte ihn zerschlagen sollen. Ich habe es nicht gekonnt.

**Seite 5** (`lore_asgerd2_05`, letzte Seite, die Schrift wird zittriger)
> *Zweiter Winter.*
> Tove, wenn du das liest: Ihr seid in der Bucht sicher, solange kein Licht über dem Wasser ist. Bleibt dort.
> Und wer immer das sonst liest, weil ich es nicht mehr kann:
> Das Glas antwortet. Das ist nicht seine Schuld und nicht unsere. Es ist, was es ist.
> Leg es nicht. Oder leg es besser als er.
> *– A.*

**Gedanke** (nach der letzten Seite, kurze Pause, dann): „‚Leg es besser als er.‘ Was heißt besser?“

**Saga-Buch:**
> *Am Feuer trockneten sie Asgerds zweites Buch. Darin stand, was die Ersten auseinanderriss: Hallbjörns Licht über dem Meer, die Segel in der Marsch, Eyvind, der vorausging, und Männer mit Asche auf den Wangen, die nicht plünderten, sondern zusahen.*

**Offene Frage im Saga-Buch:** *„Wer sind die mit Asche im Gesicht?“*

<a id="d22-3"></a>

### 22.3 3.3 Der Thingplatz

*Nach dem Lesen erscheint ein Hinweis, und in der Siedlung kommen die ersten Leute an den Rand.*

**Hinweis** (einmalig): „Was die Ersten getrennt hat, kann man nicht am Feuer bereden. Dafür gibt es den Thing.“

**Aufgabe:** **Thingplatz** bauen (`building_thing_site`: Steinkreis aus Sitzsteinen, über das Baumenü; Kosten Vorschlag: 8 große Steine, 6 Baumstämme). Das Bauen schaltet das Kapitel frei, es gibt keine Frist.

**Gedanke** (beim ersten Sitzstein): „Ein Kreis aus Steinen. Für Menschen, diesmal.“

*Ist der Thingplatz fertig, versammeln sich dort am nächsten Abend alle, die gekommen sind (siehe oben). Es brennt ein Feuer in der Mitte. Der Abend wartet, bis ein Spieler sich in den Kreis setzt (E an einem Sitzstein).*

<a id="d22-4"></a>

### 22.4 3.4 Der Thing

*Der Ablauf passt sich an, je nachdem, wer da ist. Unten steht die volle Fassung mit beiden Gruppen. Ist nur eine Gruppe da, entfallen die Zeilen der anderen, und der Streit wird zu einem Gespräch mit der Figur.*

**[Arnfinn]** *(steht als Erster, mit Blick auf Sæunn)* „Ihr seid von seinen Leuten.“
**[Orm]** „Und ihr von ihren. Und?“
**[Tove]** „Und? Seinetwegen liegen Ulf und Gyda am Strand.“
**[Sæunn]** *(ruhig)* „Seinetwegen hat er sich selbst begraben lassen. Reicht dir das nicht?“
*Stille. Kolla sieht ins Feuer. Arnfinn setzt sich wieder.*

*Wenn Eyvind da ist: Er steht am Rand, außerhalb des Kreises. Tove sieht ihn erst jetzt.*
**[Tove]** „…Du.“
**[Eyvind]** „Ich weiß.“
**[Tove]** *(lange Pause)* „Sie hat geschrieben, du hast ihr Leben gegen unseres getauscht.“
**[Eyvind]** „Ja.“
**[Tove]** „Setz dich. Aber nicht neben mich.“
*Er setzt sich, an den äußersten Stein.*

*Wenn Hrefna gefangen ist: Sie wird von zwei Bewohnern an den Rand gebracht, gefesselt.*
**[Hrefna]** „Ein Thing. Wie rührend. Wisst ihr, was im Süden mit Leuten passiert, die sich in Kreise setzen und reden? Sie werden verkauft. Einzeln.“

*Dann wenden sich alle an die Figur (im Koop an alle Spieler am Thing; jeder hört dieselben Zeilen).*

**[Sæunn]** „Du hast die Steine wieder aufgerichtet. Das Glas antwortet dir.“ *(nur, wenn ein Spieler den Weg des Kreises ging)*
**[Tove]** „Du hast das Boot heimgeholt. Und Asgerds Buch.“ *(nur, wenn ein Spieler den Weg des Hofs ging)*
**[Orm oder Arnfinn]** „Also sag du es: Was machen wir mit dem, was in dem Buch steht?“

**Antworten** (alle Spieler können nacheinander sprechen; es sind nur Auskünfte, noch keine Entscheidung):
- „Die mit der Asche suchen Leute, die rufen können. Sie werden wiederkommen.“ → **[Sæunn]** „Dann suchen sie bald nach dir.“
- „Asgerd hat geschrieben: Leg es nicht. Oder leg es besser als er.“ → **[Tove]** „Sie hat nie gesagt, was besser heißt.“ **[Sæunn]** „Weil sie es nicht wusste. Keiner von uns weiß es.“
- ◇ „Was weiß Hrefna über die mit der Asche?“ *(nur, wenn Hrefna gefangen ist)* → **[Hrefna]** „Dass sie Aschebund heißen. Dass sie an der Glutküste sitzen. Und dass der Seekönig für sie fährt.“ *(Hilfe: Saga-Buch nennt Aschebund und Glutküste)*

<a id="d22-5"></a>

### 22.5 3.5 Große Entscheidung: Hüter oder Rufer

*Am Ende des Things legt Sæunn, Tove oder (wenn keine der beiden da ist) Arnfinn einen einzelnen Splitter Seeglas auf den Stein vor der Figur. Jeder Spieler bekommt seinen eigenen Splitter, und jeder entscheidet für sich, was er damit macht (eigener Queststand, bestätigt). Das ist **keine** gemeinsame Entscheidung: Die Welt verändert sich hier nicht auf nur eine Art.*

**[Sæunn / Tove / Arnfinn]** „Asgerd hätte gesagt, wirf es ins Meer. Hallbjörn hätte gesagt, leg es. Was sagst du?“

| Wahl | Handlung | Text | Folgen (Kapitel 4–6, groß) |
|---|---|---|---|
| **Hüter** | Den Splitter in das Feuer der Thingmitte legen, wo er trüb wird und nicht mehr summt | *„Keiner soll mehr wegen eines Lichts sterben. Wir halten es still, bis wir wissen, wie man den Ring heilt.“* | `stance:hueter`. Asgerds Leute und alle, die das Licht fürchten, trauen dir mehr: Handel und Zuzug leichter. Der Aschebund jagt dich als Feind (Späher, später Angriffe nur mit Weltoption Horden). In Kapitel 5 und 6 führen die Gespräche zu dem Wissen, wie man den Ring schließt. Gespür wächst normal weiter |
| **Rufer** | Den Splitter an die eigene Brust nehmen, an Hallbjörns Ring oder an das Amulett | *„Das Glas antwortet. Dann sollten wir lernen, was es sagt, bevor es jemand anderes tut.“* | `stance:rufer`. Hallbjörns Leute und alle, die die Kraft suchen, trauen dir mehr. Der Aschebund **umwirbt** dich statt dich zu jagen (Boten, Angebote, Gefahr des Verrats). Steinkreise antworten dir stärker (Gespür-EP an Steinkreisen +25 %, bestätigt). In Kapitel 6 kann man der Aschezunge folgen |

**Gedanke** (nach der Wahl, Hüter): „Es ist still. Das ist das Schwerste daran.“
**Gedanke** (nach der Wahl, Rufer): „Es summt. Leise. Wie etwas, das meinen Namen kennt.“

**Reaktionen** (je nach Anwesenden):
- Hüter: **[Tove]** „Danke.“ **[Sæunn]** *(leise)* „Dann wirst du nie hören, was er gehört hat.“
- Rufer: **[Sæunn]** „Dann hör gut zu. Er hat es nicht getan.“ **[Tove]** „Asgerd hat dir geschrieben, du sollst es besser machen. Mach es besser.“

**Saga-Buch** (persönliche Tat, je nach Wahl):
> *Am ersten Thing der Siedlung legte {Spieler} das Glas ins Feuer und wählte, Hüter zu sein.*
> *Am ersten Thing der Siedlung nahm {Spieler} das Glas an sich und wählte, ein Rufer zu sein.*
(Vertonung: Der Erzähler sagt „eine der Euren“ statt des Namens, bestätigt.)

<a id="d22-6"></a>

### 22.6 3.6 Was danach im Dorf geschieht

**Folgen für die Welt** (sie addieren sich, je nach dem, was in der Welt gewählt wurde):
- **Toves Leute ziehen ein**, wenn ein Spieler sie in Kapitel 1 eingeladen hat. Sind sie wegen Hrefna verärgert, kommen sie erst mit dem nächsten Kapitel. Mit ihnen kommen Arnfinn (Beruf Bootsbauer, später) und Asgerds Saatkiste ins Lager.
- **Sæunn** kommt ins Dorf, sobald „Die Stimme im Seeglas“ abgeschlossen ist (Seherhütte). Bis dahin bleibt sie am Kreis.
- **Eyvind** bleibt am Rand der Siedlung und wird Bewohner, wenn ihm ein Bett zugewiesen wird. Er kennt die Priele. Vorschlag: späterer Beruf Fischer oder Lotse.
- **Hrefna**, wenn gefangen: Sie bleibt in einer Hütte und spricht jeden Spieltag einmal mit Spielern. Später kann die Siedlung über sie entscheiden: hinrichten, freilassen oder als Lotsin mitnehmen. Das ist eine gemeinsame Entscheidung (bestätigtes Muster).

**Der Skalde:** Mit dem Thingplatz kommt ein Skalde (`npc_skald`) in die Siedlung. Er liest bei Festen aus der Siedlungs-Chronik vor (Taten, Gefallene, Gedenksteine).
**[Skalde]** *(bei der ersten Begegnung)* „Ich habe gehört, hier schreibt jemand eine Saga. Ich bin gekommen, um sie zu singen, bevor ihr sie vergesst.“

<a id="d22-7"></a>

### 22.7 3.7 Die erste Kerbe

*In der Nacht nach dem Thing, wenn man das Saga-Buch öffnet: Auf dem Deckel leuchtet die erste der drei Kerben in Gezeitenlicht. Es gibt keine Meldung und keinen Ton außer einem tiefen, kurzen Puls, wie beim Lauschen.*

**Saga-Buch** (Kapitel- und Aktabschluss, gelesen vom Skalden, wenn er da ist, sonst vom Erzähler):
> *So endete die Spur der Ersten, und so begann die eure. Zwei Häuser, die sich nicht verziehen hatten, saßen an einem Feuer. Ein Verräter saß am äußersten Stein. Und im Süden wartete jemand mit Asche im Gesicht auf ein Licht über dem Meer.*

**Offene Fragen im Saga-Buch:** *„Wo ist Hallbjörn jetzt?“*, *„Wer ist der Aschebund?“*

**Übergang:** Kapitel 3 schließt den Kern ab. Die Fortsetzung (Kapitel 4, Hügelgrab-Heide) ist „später“ (bestätigt). Bis dahin bleibt die Welt offen: Nebenstränge, Bosse, die nachspawnen, Lieder, verborgene Strophen, Siedlungsbau.

---

<a id="d22-8"></a>

### 22.8 Anmerkungen für das Team

- **Kein Boss, kein Kampf:** Kapitel 3 ist ein Ruhepunkt in der eigenen Siedlung, mit Bauen (Thingplatz), Lesen und einem Gespräch. Es passt zu „Siedlung als Anker“ und braucht kein Kampfsystem.
- **Haltung im Koop:** Jeder entscheidet für sich. Eine Siedlung kann Hüter und Rufer zugleich haben. Der Aschebund jagt die einen und umwirbt die anderen, in derselben Welt. Das ist gewollt (Folgen addieren sich), braucht aber in Kapitel 4–6 Gespräche, die erkennen, wer spricht.
- **Rufer-Bonus +25 % an Steinkreisen:** bestätigt 2026-11-21. Er verändert die Gespür-Rechnung aus `listen.json` leicht.
- **Hrefnas Schicksal:** bestätigt 2026-11-21 (hinrichten, freilassen, Lotsin; gemeinsames Gespräch). Uneinig: Sie bleibt vorerst gefangen (Vorschlag).
- **Länge:** etwa 100–140 Sprechzeilen je Sprache (das Logbuch hat 5 Seiten in Asgerds Stimme). Spielzeit etwa 1 Stunde, mit dem Bau des Thingplatzes.
- **Vollständige Zählung der Ersten nach Kapitel 3:**
  - 4 auf dem Gräberhügel: Ulf und Gyda (laut Logbuch nach dem Überfall am Strand gefunden), Ketil der Junge, Ragna;
  - Asgerd: tot;
  - Hallbjörn: lebt, im Fjell;
  - Sæunn, Orm, Kolla;
  - Tove und 6;
  - Eyvind.

  Das sind 17.


[↑ zum Inhaltsverzeichnis](#inhaltsverzeichnis)


---

<a id="teil-5"></a>

# Teil V – Kreaturen, Level und Kampf

<a id="d23"></a>

## 23. Kreaturen, Level und Kampfwerte (bereinigte Fassung „Küstenasche“)

> Quelle: `docs/21_kreaturen_level_kampf.md`

Stand 2026-11-22. Grundlage ist Geminis „Küstenasche – Gesamtdokumentation“ (Kopie im Projekt: `design/referenz/gemini_kuestenasche_gesamtdokumentation.md`), angepasst an die Entscheidungen vom 2026-11-22 (`docs/01_entscheidungen.md`, acht Zeilen ab „Gegnerarten: Fantasy-Kreaturen (Antwort)“).

- **Dieses Dokument:** Regeln, Formeln, Begründungen, Tiere, Bosse, Herstellen, Technik.
- **`docs/22_kreaturen_daten.md`:** alle Tabellen zu den 48 Kreaturen (Einordnung, Werte je Rang, Spawn je Biom, bereinigte Beute, Gegenstands-Katalog). Es wird von `tools/data/creatures_from_gemini.py` erzeugt und nicht von Hand geändert.
- **Nicht hier:** Story und Boss-Texte (eigene Sitzung „Sagen-Bosse“), Modelle und Animationen (Asset-Plan).

Kennzeichen wie im Entscheidungslog: **bestätigt** = vom Nutzer entschieden · **Vorschlag** = von Claude · **Gemini** = aus dem Gemini-Dokument übernommen, nicht einzeln bestätigt. **Alle Zahlen sind Startwerte.** Einstellen kann man sie erst, wenn man kämpfen kann (Phase 3).

---

<a id="d23-1"></a>

### 23.1 1. Was feststeht

| Punkt | Inhalt | Status |
|---|---|---|
| Gegner | 48 Kreaturen aus dem Gemini-Dokument, dazu Tiere und Menschen; keine Wasserwesen | bestätigt (2026-11-22) |
| Bosse | Sagen-Fassung als Standard (Moorhexe, Draugr-Fürst, Troll, Lindwurm, Frostriese); der Alte Keiler bleibt realistisch; Weltoption „Sagenwelt“ entfällt | bestätigt (2026-11-22) |
| Fortschritt | Spielerlevel 1–50; Rezepte haben eine Level-Anforderung | bestätigt (2026-11-22) |
| Erfahrung | aus allem (Kampf, Bosse, Quests, Entdecken, Herstellen, Bauen, Sammeln); Mitspieler in der Nähe bekommen einen Anteil; Aufstieg gibt Fähigkeitspunkte; Werte steigen weiter durch Tun | bestätigt (2026-11-22) |
| Gebiete | die 6 Biome bleiben, Geminis 5 Gebiete werden zugeordnet | bestätigt (2026-11-22) |
| Beute | Kreaturen fein wie bei Gemini (Sammelbegriffe werden konkret), Tiere schlank | bestätigt (2026-11-22) |
| Herstellen | Metall-Leiter T1–T6 bleibt das Rückgrat, Kreaturenteile kommen als Zusatz und für Spezialsets dazu | bestätigt (2026-11-22) |
| Reihenfolge | alle 48 Kreaturen zuerst als Daten, Modelle gestaffelt, zuerst der Kern | bestätigt (2026-11-22) |
| Kampf | Richtungshiebe (links, rechts, oben) plus Stoß, Zielerfassung, Ausweichschritt, Trefferzonen Kopf/Körper/Glieder | bestätigt (2026-11-13) |
| Ausdauer | leere Ausdauer macht nur langsamer und schwächer, sperrt nie (Regel 5) | bestätigt (2026-11-16) |
| Ausrüstungs-Slots | Helm, Schulter, Oberkörper, Hände, 2 × Ring, Beine, Füße, Umhang | bestätigt (2026-11-12/13) |
| Bauraster | 300 cm (Gemini: 200 cm) | festgestellt (so gebaut, `GamerBuildSnapping.h`) |

---

<a id="d23-2"></a>

### 23.2 2. Level, Stufen und Biome

#### 2.1 Zuordnung (Vorschlag)

Gemini teilt Level 1–50 in fünf Phasen. Wir haben sechs Stufen und sechs Biome. Darum ordne ich die Level so zu, dass jede Stufe etwa 8 Level umfasst, nah an Geminis Rang-Spalte:

| Stufe | Biom | Level | Biom-Rang | Gefährliche Orte (Rang + 1) | Metall | Boss (Sagen-Fassung) → Belohnung |
|---|---|---|---|---|---|---|
| T1 | Fjordwiesen | 1–8 | 1 | Banditenlager, Wolfshöhlen (2) | Rohstufe (Stein, Holz, Knochen) | Der Alte Keiler (realistisch) → Bauplan Rennofen |
| T2 | Gezeitenmarsch | 9–16 | 2 | Wracks, tiefe Priele (3) | Raseneisen | Schlickmutter als Moorhexe → Bossmaterial T3 |
| T3 | Hügelgrab-Heide | 17–25 | 3 | Grabhügel, Gewölbe, Grabfestung (4) | Schmiedeeisen | Hügelkönig als Draugr-Fürst → T4 |
| T4 | Basaltfjell | 26–34 | 4 | Gipfel, Eishöhlen (5) | Stahl | Felsenvater als Troll → T5 |
| T5 | Glutküste | 35–42 | 5 | Lavafelder, Kultstätten (6) | Damast | Aschezunge als Lindwurm → Relikt (T6) |
| T6 | Eisrand | 43–50 | 6 | Gletscherspalten (6, mehr Anführer) | Relikt | Sturmkoloss als Frostriese → Endsieg |

Die Belohnungen stammen aus `design/ASSET_LIST_GESAMT.md`. **Eine Stufe öffnet sich doppelt:** Das Level muss erreicht sein (bestätigt: Rezepte mit Level-Anforderung), und das Material bzw. der Bauplan kommt vom Boss der vorigen Stufe. Im Spielmodus „Freies Spiel“ findet man die Bosse durch Erkunden.

#### 2.2 Ausrüstungswerte je Stufe

Die Werte sind aus Geminis Level-Tabelle (Abschnitt 2.2 dort) auf die Level-Bereiche umgerechnet, mit linearer Einteilung innerhalb von Geminis Fünfer-Schritten. Sie gelten für eine Standard-Einhandwaffe und ein volles, mittelschweres Rüstungsset.

| Stufe | Level | Waffen-Schaden | Haltungsschaden | Rüstung (Set) | Haltungswiderstand (Set) | Resistenz je Art höchstens |
|---|---|---|---|---|---|---|
| T1 | 1–8 | 14–24 | 12–19 | 15–37 | 20–35 | 8 % |
| T2 | 9–16 | 26–39 | 21–30 | 41–66 | 37–53 | 16 % |
| T3 | 17–25 | 41–60 | 32–45 | 71–110 | 56–80 | 26 % |
| T4 | 26–34 | 61–86 | 46–62 | 111–156 | 81–108 | 36 % |
| T5 | 35–42 | 89–111 | 64–79 | 162–199 | 112–136 | 46 % |
| T6 | 43–50 | 115–145 | 83–105 | 208–260 | 142–175 | 55 % |

**Innerhalb einer Stufe (Vorschlag):** Das Grundrezept am Stufenanfang gibt den unteren Wert. Verbesserte Fassungen mit Kreaturenteilen (höhere Level-Anforderung) heben den Wert zur Mitte, ein Meisterstück am Stufenende bis zum oberen Wert. So ersetzt die Zusatz-Regel aus Abschnitt 11 Geminis stufenlose Kurve.

#### 2.3 Archetypen (Gemini, unverändert)

| Archetyp | Schaden | Haltungsschaden | Rüstung | Haltungswiderstand |
|---|---|---|---|---|
| leichte Waffe (Dolch, Sax) | × 0,80 | × 0,65 | – | – |
| Standard (Schwert, Axt, Speer) | × 1,00 | × 1,00 | – | – |
| schwer (Zweihänder, Hammer, Breitaxt) | × 1,25–1,40 | × 1,35–1,60 | – | – |
| leichte Rüstung | – | – | × 0,75–0,85 | × 0,70–0,80 |
| schwere Rüstung | – | – | × 1,15–1,30 | × 1,20–1,35 |

Leichte Rüstung bringt dafür schnellere Ausdauer-Erholung und Beweglichkeit. Nach Regel 5 heißt das nur „schneller“, nie „schwere Rüstung sperrt etwas“.

---

<a id="d23-3"></a>

### 23.3 3. Erfahrung und Fähigkeitspunkte

Bestätigt ist: Erfahrung kommt aus allem, Mitspieler in der Nähe bekommen einen Anteil, jeder Aufstieg gibt Fähigkeitspunkte. Gemini nennt dazu keine Zahlen. Die folgenden Startwerte sind ein **Vorschlag**:

| Quelle | Erfahrung (Stufe = Stufe der Quelle) |
|---|---|
| Gegner besiegen | 10 × Rang; Anführer-Stufen × 1,5 / × 2 / × 3 (Abschnitt 6.4) |
| Boss, erster Sieg pro Spieler | 300 × Stufe |
| Quest-Etappe | 50 × Stufe |
| neuer Ort, Steinkreis, Biom entdeckt | 25 × Stufe |
| Rezept zum ersten Mal hergestellt | 20 × Stufe; danach 1 × Stufe je Stück |
| Bauteil fertig gebaut | 2 × Stufe |
| Sammeln, Abbauen, Ernten | 0,5 × Stufe je Stück |

- **Bedarf je Level:** steigt mit Level hoch 1,5 (Vorschlag: `100 × Level^1,5`). Das Zieltempo legen wir mit dem ersten Kampf-Spieltest fest.
- **Anteil im Koop:** Mitspieler im Umkreis von 50 m (wie die Saga-Gutschrift) bekommen bei Kampf, Bossen, Quests und Entdecken den vollen Wert, bei Herstellen, Bauen und Sammeln 25 %.
- **Bremse gegen Farmen:** Gegner unter der eigenen Stufe geben weniger: eine Stufe darunter 75 %, zwei 25 %, drei oder mehr 10 %. Die eigene Stufe ergibt sich aus dem Level nach Abschnitt 2.1.
- **Fähigkeitspunkte:** 1 je Aufstieg, bei Level 5, 10, … 50 je einer extra, also 59 Punkte bis Level 50. Der Skilltree selbst fehlt noch.
- **Werte (Stärke, Geschick, Zähigkeit, Handwerk, Wille)** steigen weiter durch Tun (`docs/11_charaktere.md`). Das Level erhöht sie nicht direkt.

---

<a id="d23-4"></a>

### 23.4 4. Lebenspunkte und Haltung des Spielers

Gemini legt keine Lebenspunkte für den Spieler fest. **Vorschlag:**

- **Lebenspunkte = 100 + 5 × (Zähigkeit − 5) + Mahlzeiten-Buff „Gesundheit“.** Die 100 stehen schon in `health.json`. Kein Zuwachs pro Level: Das hält Koop-Gruppen mit unterschiedlichem Level fair, ein neuer Mitspieler wird über Ausrüstung mitgezogen.
- **Haltung des Spielers:** Leiste 100. Bei Bruch taumelt man 0,8 s. Nach 2 s ohne Treffer erholt sie sich um 30 pro Sekunde. Keine Verbindung zur Ausdauer.

---

<a id="d23-5"></a>

### 23.5 5. Kampfformeln

#### 5.1 Rüstung pro Trefferzone (Vorschlag auf Basis der Gemini-Formel)

Geminis Formel bleibt: **Reduktion = min(0,75 ; R / (R + 160))** und **Schaden = max(1 ; Roh × Zonenfaktor × (1 − Reduktion))**. Neu ist, dass R je Trefferzone aus den Slots berechnet wird (bestätigt sind die drei Zonen und der Schutz durch die passenden Slots):

| Slot | Anteil am Set | Zone |
|---|---|---|
| Helm | 20 % | Kopf: R = Helm × 5 |
| Schulter | 10 % | Körper: R = (Schulter + Oberkörper) × 2 |
| Oberkörper | 40 % | |
| Hände | 10 % | Glieder: R = (Hände + Beine + Füße) × 10/3 |
| Beine | 15 % | |
| Füße | 5 % | |
| Umhang, Ringe | – | keine Rüstung; Resistenzen bzw. Wirkungen |

Mit vollem Set bekommt jede Zone den Set-Wert aus Abschnitt 2.2. Fehlt der Helm, ist der Kopf ungeschützt. **Zonenfaktoren (Vorschlag):** Kopf × 1,5 · Körper × 1,0 · Glieder × 0,7.

**Kreaturen** tragen keine Rüstung. Sie bekommen Trefferzonen mit Faktoren in den Daten: Schwachpunkt × 1,5, Panzer × 0,5. Beispiele aus Geminis Beschreibungen: Slagbrute mit Frontkruste (Panzer vorne), Cindercolossus mit Schwachstellen in den Gelenken, Ashweaver mit glimmenden Schwachpunkten.

#### 5.2 Resistenzen und Umweltgefahren

Formel (Gemini, unverändert): **wirksame Resistenz = Basis + Ausrüstung + Buffs, begrenzt auf −25 % bis 60 %**; **Wirkung = Roh × (1 − Resistenz)**.

| Gefahr | Biom | Wirkung ohne Schutz (Vorschlag, Regel 5: nur langsamer oder schwächer) | Buff-Art |
|---|---|---|---|
| Hitze | Glutküste | Durst steigt doppelt so schnell; nahe an Lava 2 LP/s | `heat_resist` (neu) |
| Frost | Basaltfjell, Eisrand | Ausdauer-Erholung −40 %; Kälte wirkt stärker | `cold_resist` (gibt es schon) |
| Sturm | Eisrand | Böen machen 10 Haltungsschaden/s; gegen den Wind −20 % Tempo | `storm_resist` (neu) |
| Verfall/Sporen | Gezeitenmarsch, Hügelgrab-Heide | keine LP-Regeneration; in Sporenwolken 3 LP/s | `poison_resist` (gibt es schon) |

Hitze und Sturm brauchen Durst und Wetter. Beides kommt mit Tag/Nacht und Wetter.

#### 5.3 Haltung (Poise)

Gemini, ergänzt um den Abbau, der dort fehlt:

- **Gegner:** Haltung += Waffen-Haltungsschaden × Angriffsfaktor × Zonenfaktor. Ist die Leiste voll, kommt der **Haltungsbruch**: Der Gegner taumelt 2,5 s, Treffer in dieser Zeit machen × 1,5 Schaden, danach ist die Leiste leer. Bosse taumeln 3 s. **Abbau (neu):** nach 3 s ohne Treffer 25 % der Leiste pro Sekunde.
- **Spieler:** eingehender Haltungsschaden × 90 / (Haltungswiderstand + 90) (Gemini). Beispiel: 150 Haltungswiderstand dämpfen um 62,5 %.

**Angriffsfaktoren (Vorschlag, passend zum bestätigten Richtungskampf):**

| Aktion | Schaden | Haltungsschaden |
|---|---|---|
| leichter Hieb (links, rechts, oben) | × 1,0 | × 1,0 |
| Stoß | × 0,9 | × 0,8, an Schwachpunkten × 1,5 |
| schwerer (aufgeladener) Hieb | × 1,4 | × 1,6 |
| Schildstoß | 0 | × 2,0 |
| Gegenschlag nach perfektem Parieren | × 1,2 | × 1,5 |

#### 5.4 Wie lange dauert ein Kampf? (Prüfrechnung)

Mit Geminis Werten, Waffe und Rüstung aus der Stufenmitte und ohne Parieren:

| Gegner | Treffer bis zum Sieg (Körper / Kopf) | Schwere Treffer, die der Spieler aushält |
|---|---|---|
| Bramblefang, Rang 2 (410 LP), T2-Waffe ~32 | 13 / 9 | 8 (13,6 Schaden je Treffer, 100 LP) |
| Kraggar, Rang 4 (1.120 LP), T4-Waffe ~74 | 16 / 11 | 6 (19 je Treffer, 100 LP) |
| Galewyrm, Rang 6 (2.900 LP), T6-Waffe ~130 | 23 / 15 | 6 (30 je Treffer, 160 LP) |
| Galewyrm als Anführer mit Streuung 115 % | 35 / 23 | 4 (43 je Treffer, 160 LP) |

**Einschätzung:** Für einen KCD-artigen Kampf sind Geminis Lebenspunkte eher hoch. Kopftreffer und Haltungsbrüche verkürzen das zwar, trotzdem schlage ich einen **Regler `creature_hp_scale`** in den Daten vor (Start 1,0). Mit ihm lassen sich nach dem ersten Kampftest alle Kreaturen auf einmal anpassen, statt 48 Zeilen einzeln zu ändern.

---

<a id="d23-6"></a>

### 23.6 6. Gegner-Rang, Streuung und Anführer-Stufen

#### 6.1 Rang im Biom

- **Biom-Rang = Stufe des Bioms.** An **gefährlichen Orten** gilt Rang + 1, höchstens 6 (Tabelle 2.1). So bleibt etwas von Geminis Gebieten mit mehreren Rängen erhalten.
- **Geminis Ränge 2–6 bleiben**, Rang 1 sind Tiere und Menschen. Gemini hatte für Level 1–5 Rang 1, aber keine Kreatur mit Rang 1. Diese Lücke ist damit geschlossen.
- **Ränge über 6 gibt es nicht.** An die Stelle von Geminis Captain-Rängen 7–25 treten die Anführer-Stufen (6.4).
- Eine Kreatur kommt nur in ihrem **Heimat-Biom und in späteren Biomen** vor, nie früher. Sonst bräuchte man ihr Modell vor ihrer Welle (Abschnitt 7).

#### 6.2 Mitwachsen und Nachzügler (Gemini, übertragen)

Geminis Felder bleiben und heißen in den Daten `scale_to_zone`, `scale_chance`, `max_rank` und `fixed_rank`:

- **wächst mit (z. B. 85 %):** 85 % der Exemplare bekommen den Rang des Ortes, der Rest einen Rang darunter, mindestens den MinRang. Geminis Text ließ offen, was mit dem Rest passiert. Das ist die Festlegung dafür (Vorschlag).
- **wächst mit bis Rang X:** wie oben, aber höchstens Rang X.
- **fest Rang X (Nachzügler):** bleibt auf Rang X. Das sind die schwachen Kontrast-Gegner in starken Gebieten, eine Idee von Gemini.

Die Spawn-Tabellen je Biom stehen in `docs/22_kreaturen_daten.md`, Abschnitt 5.

#### 6.3 Streuung (Vorschlag)

Gemini: jedes Exemplar zufällig 85–115 % auf LP, Haltung und Schaden. Weil das alle drei Werte trifft, ist ein starkes Exemplar fast doppelt so gefährlich wie ein schwaches (0,85² gegen 1,15²), ohne dass man es sieht. **Lösung:** ein einziger Wurf für alle drei Werte, und **die Größe folgt ihm**: Größe × (1 + 0,5 × (Wurf − 1)), also ±7,5 %. Größere Tiere sind stärker, das kann man lesen. Wirkt es im Test trotzdem unfair, die Spanne auf 90–110 % verkleinern.

#### 6.4 Anführer-Stufen statt Captain-Rang 25 (Vorschlag)

Geminis Kurve von Rang 6 bis 25 bringt am Ende nur + 35 % LP, + 20 % Haltung und + 25 % Schaden. Rang 7 ist gerade + 5 % stärker als Rang 6, sichtbar ist das nicht. Ich fasse das in **drei sichtbare Stufen** mit Geminis eigenen Kurvenwerten bei Rang 10, 17 und 25:

| Stufe | entspricht bei Gemini | LP | Haltung | Schaden | sichtbar | Beute | wo und wie oft |
|---|---|---|---|---|---|---|---|
| Erfahren | Captain-Rang ~10 | × 1,13 | × 1,07 | × 1,09 | 5 % größer, Narben | + 1 Wurf Grundbeute | ab Rang 2, 5 % (Orte 10 %) |
| Veteran | ~17 | × 1,25 | × 1,14 | × 1,18 | 10 % größer, Name „Alter …“ | + 1 Wurf Grund- und Rangbeute | ab Rang 4, 3 % (Orte 6 %) |
| Anführer | 25 (Geminis Höchstwert) | × 1,35 | × 1,20 | × 1,25 | 15 % größer, eigener Name | Rangbeute doppelt | nur Rang 6, 2 % (Orte 4 %) |

Höchstens ein Exemplar mit Stufe pro Gruppe. Sollen sich Anführer stärker anfühlen, die Faktoren nach dem Test erhöhen. Geminis Werte sind bewusst vorsichtig.

---

<a id="d23-7"></a>

### 23.7 7. Kreaturen

Alle 48 Kreaturen mit ID, Rolle, Heimat, Rängen und Werten stehen in `docs/22_kreaturen_daten.md`. Kurzfassung der Modell-Wellen (bestätigt: Daten alle, Modelle gestaffelt):

| Welle | Heimat-Biom | Kreaturen |
|---|---|---|
| Kern | Fjordwiesen | keine Kreaturen: Tiere, Waldbanditen, Alter Keiler |
| Kern (1) | Gezeitenmarsch | 4: Bramblefang, Siltstrider, Mirecrawler, Thornmaw (+ Irrlichter als Boss-Begleiter) |
| 2 | Hügelgrab-Heide | 13 |
| 3 | Basaltfjell | 14 |
| 4 | Glutküste | 10 |
| 5 | Eisrand | 7 |

Das passt zum Stil „Alltag historisch, Ränder mythisch“: Je weiter vom Start, desto fantastischer. **Namen:** Geminis Namen sind Arbeitsnamen. IDs sind `creature_<name>` auf Englisch (Riese = `creature_giant`, Wiedergänger = `creature_revenant`). Einige Namen erinnern an bestehende Marken, etwa „Ironhide“ (Transformers) oder „Cragmaw“ (Dungeons & Dragons). Vor dem Release prüfen.

---

<a id="d23-8"></a>

### 23.8 8. Tiere und Menschen

#### 8.1 Tiere: Werte (Vorschlag)

Gemini hat keine Tiere. Die Werte gelten im Heimat-Biom des Tiers. In höheren Biomen pro Rang LP × 1,4, Haltung × 1,25, Schaden × 1,25. Das entspricht dem Wachstum einzelner Gemini-Kreaturen von Rang zu Rang.

| Tier (`animal_…`) | Biom (Rang) | LP | Haltung | Schaden | Verhalten (Asset-Liste) |
|---|---|---|---|---|---|
| Hase | Fjordwiesen, Heide (1) | 30 | 5 | – | flieht schnell |
| Fuchs | Fjordwiesen (1) | 50 | 8 | 4 | scheu |
| Rothirsch | Fjordwiesen, Heide (1) | 140 | 20 | 7 | flieht; stößt, wenn gestellt |
| Wolf | Fjordwiesen (1), Heide, Fjell | 140 | 18 | 8 | Rudel, umkreist zuerst |
| Wildschwein | Fjordwiesen (1) | 220 | 35 | 11 | greift an, wenn bedrängt |
| Robbe | Gezeitenmarsch (2) | 200 | 25 | 6 | flieht ins Wasser |
| Seevögel | Küste (2) | 15 | – | – | Vogelfelsen |
| Elch | Hügelgrab-Heide (3) | 650 | 80 | 20 | gefährlich, wenn bedrängt |
| Bär | Heide (3), Fjell | 800 | 95 | 24 | verteidigt seine Höhle |
| Luchs | Basaltfjell (4) | 600 | 60 | 22 | schleicht, greift Einzelne an |
| Rentier | Basaltfjell (4), Eisrand | 700 | 70 | 18 | Herden, scheu |
| Polarfuchs | Eisrand (6) | 300 | 20 | 12 | scheu |

Zum Vergleich die Mittelwerte der Kreaturen: Rang 2 = 379 / 40 / 12, Rang 3 = 607 / 63 / 18, Rang 4 = 910 / 93 / 24 (Dokument 22, Abschnitt 4).

#### 8.2 Tiere: Beute (schlank, bestätigt; Teile = Vorschlag)

Jedes Säugetier gibt zusätzlich **Sehne und Knochen** in der Stufe seines Biom-Rangs (`item_sinew_t1…t6`, `item_bone_t1…t6`, dieselben Gegenstände wie bei den Kreaturen). Wildschwein, Rothirsch und Elch geben außerdem **Haut** (`item_hide_t…`).

| Tier | typische Teile | Fleisch |
|---|---|---|
| Wildschwein | Keilerhauer (`item_boar_tusk`) | ja |
| Rothirsch | Hirschgeweih (`item_deer_antler`) | ja |
| Hase | Hasenfell (`item_hare_pelt`) | ja |
| Fuchs | Fuchsfell (`item_fox_pelt`) | – |
| Wolf | Wolfsfell (`item_wolf_pelt`), Wolfszahn (`item_wolf_fang`) | – |
| Robbe | Robbenfell (`item_seal_pelt`), Tran (`item_seal_blubber`) | ja |
| Seevögel, Seeadler | Federn (`item_feather`), Seevogel-Ei (`item_seabird_egg`) | – |
| Elch | Elchschaufel (`item_elk_antler`) | ja |
| Bär | Bärenfell (`item_bear_pelt`), Bärenfett (`item_bear_fat`) | ja |
| Luchs | Luchsfell (`item_lynx_pelt`) | – |
| Rentier | Rentierfell (`item_reindeer_pelt`), Rentiergeweih (`item_reindeer_antler`) | ja |
| Polarfuchs | Polarfuchsfell (`item_arctic_fox_pelt`) | – |

**Fleisch** ist ein einziger Gegenstand, „Rohes Fleisch“ (`item_meat_raw`). Die Taverne-Gerichte, die noch auf Fleisch warten (Trockenfleisch, Wildeintopf, Braten), brauchen nur ihn.

#### 8.3 Menschen (Vorschlag)

Banditen, Plünderer, Eidbrecher, Kultisten und Söldner (Gruppen aus der Asset-Liste) folgen **denselben Regeln wie Spieler**: 100 LP, Rüstung und Waffe ihrer Stufe, Haltung 100, Trefferzonen wie beim Spieler. Das passt zum KCD-Kampf: Ein Mensch fällt nach wenigen guten Treffern, wenn er schlecht gerüstet ist. Ihr Hauptmann bekommt eine Anführer-Stufe. Beute: beschädigte Ausrüstung ihrer Stufe und Vorräte. Details kommen mit dem Kampfsystem.

---

<a id="d23-9"></a>

### 23.9 9. Bosse (Sagen-Fassung)

Story, Ablauf und Phasen der Bosse klärt die eigene Sitzung „Sagen-Bosse“. Hier stehen nur die Kampfwerte (**Vorschlag**): **Boss-Rang = Biom-Rang + 1** (höchstens 6). Die Werte kommen von einer passenden Kreatur dieses Rangs, dann **LP × 4, Haltung × 2, Schaden × 1,3**.

| Boss | Biom | Grundlage | LP | Haltung | Schaden |
|---|---|---|---|---|---|
| Der Alte Keiler (realistisch) | Fjordwiesen | Wildschwein auf Rang 2 | 1.230 | 90 | 18 |
| Schlickmutter als Moorhexe | Gezeitenmarsch | Mittelwert Rang 3 | 2.430 | 125 | 23 |
| Hügelkönig als Draugr-Fürst | Hügelgrab-Heide | Wiedergänger Rang 4 | 3.520 | 190 | 31 |
| Felsenvater als Troll | Basaltfjell | Troll Rang 5 | 6.240 | 330 | 44 |
| Aschezunge als Lindwurm | Glutküste | Cinderwyrm Rang 6 | 11.000 | 430 | 62 |
| Sturmkoloss als Frostriese | Eisrand | Riese Rang 6 + Anführer-Stufe | 13.230 | 530 | 75 |

Die Irrlichter der Moorhexe (`creature_will_o_wisp` aus der Asset-Liste) sind Begleiter mit Rang-2-Werten. Bossbeute bei Wiederholung folgt der bestätigten Regel vom 2026-11-15 (erster Sieg volle Beute, danach Bossmaterial mit Chance je Stufe).

---

<a id="d23-10"></a>

### 23.10 10. Beute-Regeln

#### 10.1 Würfeln (Gemini, bereinigt)

1. **Rang-Sperre (unverändert):** Ein Eintrag wird nur gewürfelt, wenn der Rang des Exemplars mindestens „ab Rang“ ist.
2. **Mengenbonus:** + 1 Stück je Rang über „ab Rang“, **höchstens + 2**. Bei Gemini war der Bonus unbegrenzt. Ein Rang-25-Captain hätte bei einem Rang-6-Eintrag 19 Stück extra bekommen.
3. **Nachzügler-Dämpfung nur für Biom-Zusatzbeute:** Liegt das Exemplar unter dem Rang des Ortes (Differenz 1 / 2 / ab 3), sinkt die Chance der Biom-Zusatzbeute auf × 0,85 / × 0,70 / × 0,55. Grund- und Rangbeute bleiben ungedämpft, denn ein Nachzügler gibt ohnehin nur die Beute seines niedrigen Rangs. Bei Gemini wurde doppelt bestraft. Geminis „Kerndrop-Schutz“ ist damit überflüssig.
4. **Anführer-Stufen** geben zusätzliche Würfe (Tabelle 6.4).
5. **Koop:** Gegner-Beute fällt einmal als Gegenstände auf den Boden (wie beim Grab). Bossbeute gibt es pro Spieler (bestätigt 2026-11-15).
6. **Schwierigkeit:** Der bestehende Faktor `yield` aus `difficulty.json` wirkt auf die Mengen.
7. **Der Server würfelt** (Regel 1), mit einem `FRandomStream` wie in Geminis Code.

#### 10.2 Bereinigung von Geminis Beute-Namen (Vorschlag)

Geminis Tabellen enthalten **361 Einträge mit 279 verschiedenen Namen**. Nach diesen Regeln werden daraus **197 Gegenstände** (Zählung und alle Zuordnungen: Dokument 22):

1. **Gleicher Name = gleicher Gegenstand** (z. B. „Verfall-Relikt“ bei neun Kreaturen).
2. **Schlichte Synonyme werden zu allgemeinen Teilen je Stufe:** „Dünne / Zähe / Lange / Schwere Sehnen“ usw. werden zu `item_sinew_t<Rang>`, ebenso Knochen, Haut, Horn und Drachensehne. Die Stufe ist der Rang beim Tod. Kreaturentypische Teile bleiben eigene Gegenstände (Dornenbalg, Klammer-Chitin, Glutschuppe, Frostsehne …). Das ist die einzige Stelle, an der die feine Beute etwas zusammengelegt wird.
3. **„A & B“ wird geteilt:** zwei Einträge mit derselben Chance; A behält die Menge, B fällt einmal (15 Fälle).
4. **„A/B“ wird zum ersten Begriff** („Asche-Balg/Haut“ → Aschebalg).
5. **Sammelbegriffe werden konkrete Teile** der jeweiligen Kreatur, z. B. „Seltene Hochrang-Materialien“ (Dravik) → Dravik-Hornkern, „Spitzen-Komponenten“ (Cinderwyrm) → Glutdrachenherz, „Seltene Rohstoffe“ → Kammkern (Cragmaw) bzw. Riesenzahn (Riese), „Seltenere Relikte“ (Wiedergänger) → Grabbeigabe.
6. **Biom-Zusatzbeute** gilt nur in Biomen, in denen die Kreatur vorkommt. 33 Einträge entfallen, sie wären nie gefallen. Die Biom-Begriffe werden zu 18 Biom-Materialien, die man meist auch sammeln oder abbauen kann (Moosfaser, Verfall-Relikt, Basaltbrocken, Glutschuppe, Sturmfaser …; eines davon, Pflanzenfasern, gibt es schon). Zusammen mit dem Knorpel sind das die 19 Gegenstände mit fester Stufe.
7. **Rang-Beute über dem erreichbaren Rang rückt vor:** In unserer Biom-Ordnung erreichen manche Kreaturen Geminis Rang 5 oder 6 nie. Damit ihre seltene Beute nicht verloren geht, rückt sie auf den höchsten Rang, den die Kreatur erreicht, mit Geminis Chance (19 Fälle, in Dokument 22 als „vorgezogen“ markiert).

**Aufwand, ehrlich:** 197 Gegenstände heißt 197 Einträge in `items.json` und 197 Icons, dazu Tierteile und Metall. Ich hatte vorher „rund 150“ geschätzt, die echte Zahl bei Gemini war höher (279 Namen).

---

<a id="d23-11"></a>

### 23.11 11. Herstellen: Metall plus Kreaturenteile

Bestätigt: Die Metall-Leiter bleibt das Rückgrat, Kreaturenteile kommen dazu. **Was die Teile bringen (Vorschlag):**

| Art (Katalog in Dokument 22) | wofür | Wirkung |
|---|---|---|
| Waffe/Werkzeug: Zähne, Klauen, Hörner, Knochen | Klingen, Spitzen, Köpfe | Schaden und Haltungsschaden zur Mitte bzw. zum oberen Ende der Stufe |
| Rüstung: Häute, Schuppen, Platten, Chitin | Rüstungsteile | Rüstung und Haltungswiderstand; Element-Teile geben Resistenz |
| Bindung: Sehnen, Fasern, Gewebe | Griffe, Bögen, Nähte | Haltbarkeit; bei Bögen Zugkraft |
| Veredelung: Kerne, Glut, Frost, Sporen | Veredelung, Tränke | Resistenzen, später Magie |
| Essen: Knorpel, Fett, Fleisch | Taverne | Brühen und Gerichte nach den Taverne-Regeln |
| Bau/Relikt: Platten, Relikte, Altmetall | Bauteile, Quests, Einschmelzen | Altes Eisenstück, Waffenbruchstück, Rüstungsschrott → Metall |

**Beispiele (Vorschlag; Geminis Rezepte an die Metall-Leiter angepasst):**

| Rezept | Stufe, ab Level | Zutaten | Ergebnis |
|---|---|---|---|
| Raseneisen-Schwert (Grundrezept) | T2, 9 | 2× Raseneisenbarren, 1× Zähe Sehne, 1× Holzgriff | 26 / 21 |
| Dornzahn-Klinge (Gemini) | T2, 12 | 2× Raseneisenbarren, 3× Fangzahn (Bramblefang), 2× Zähe Sehne, 1× Dornholz | ~31 / ~24 |
| Stahlschwert mit Felsklaue | T4, 30 | 3× Stahlbarren, 1× Felsklaue (Kraggar), 2× Schwere Sehne | ~74 / ~54 |
| Schlackekiefer-Brecher (Gemini, schwere Waffe) | T4, 29 | 3× Stahlbarren, 2× Gehärteter Kiefer (Charmaw), 1× Schlackeplatte, 2× Schwere Sehne | Grundwert ~71 / ~52, schwer × 1,3 / × 1,5 → ~92 / ~78 |
| Sturmwyrm-Speer (Gemini) | T6, 47 | 2× Reliktmetall, 2× Sturmwyrm-Horn, 2× Sturmwyrm-Schwingenhaut, 2× Massive Drachensehne, 1× Sturmwyrm-Kern | ~130 / ~94 |
| Dornenbalg-Wams (Oberkörper) | T2, 12 | 3× Dornenbalg, 2× Zähe Sehne, 1× Raseneisen-Nieten | Rüstung ~20, Haltungswiderstand ~18 (40 % des Sets) |
| Frost-Schuppenrüstung (Spezialset, Gemini) | T5, 36 | Oberkörper: 3× Frostschuppenbrocken (Frostmauler), 2× Damastringe, 2× Dichtes Hochlagenfell (Rimehound), 2× Gehärtete Sehne | Set ~165 / ~115, Frost-Resistenz + 36 % |
| Knorpelbrühe (Gemini: Knorpel- & Laufbrühe) | T2, Taverne | 2× Knorpel, 1× Knochen (T2) | Gesundheit + 12, Regeneration + 0,4; Geminis „+ 20 % Ausdauer-Erholung“ braucht eine neue Buff-Art `stamina_regen` |
| Sporen-Tonikum (Gemini) | T3 | 2× Sporenkapsel (Sporeback), 1× Verfall-Relikt, 1× Moosfaser | Verfall/Sporen-Resistenz + 15 % (`poison_resist`) |
| Glut- & Sturm-Elixier (Gemini) | T6 | 1× Stabile Glutkapsel, 1× Geladener Kernrest, 2× Glutknoten | Hitze und Sturm je + 20 % (neue Buff-Arten) |
| Dornen-Palisade (300 cm) | T2 | 4× Dornholz, 2× Dornenbalg, 2× Zähe Sehne | Verteidigungsbau (Weltoption Angriffe) |
| Gewölbe-Bollwerk mit Tor (300 cm, Gemini) | T4 | 4× Gewölbeplatte (Vaultcrawler), 3× Altes Eisenstück, 3× großer Stein, 2× Wurzelbast (Mossgolem) | schwere Mauer mit Tor |

Die Zahlen bei Waffen bedeuten Schaden / Haltungsschaden. Metall-Gegenstände (Erze, Barren, Nieten, Ringe) fehlen noch in `items.json`. Bauteile bekommen dieselbe Level-Anforderung wie Rezepte (Vorschlag).

---

<a id="d23-12"></a>

### 23.12 12. Technik

**Neue Tabellen in `game/Content/Data/`** (Format wie `docs/06_datenformat.md`: `{ "schema": 1, "rows": [ … ] }`, IDs `snake_case`). Sie kommen mit Phase 3, zusammen mit dem Lader:

| Datei | Inhalt |
|---|---|
| `creatures.json` | ID, Name, MinRang, Werte je Rang, Trefferzonen, Welle |
| `animals.json` | Tiere wie in Abschnitt 8 (oder als Art in `creatures.json`) |
| `spawns.json` | je Biom: Kreatur, Gewicht, Rang-Verhalten, nur an Orten |
| `creature_loot.json` | je Kreatur: Grund-, Rang- und Biom-Beute mit Gegenstands-IDs |
| `progression.json` | Level-Kurve, Erfahrungsquellen, Stufen-Grenzen, Ausrüstungswerte je Stufe |
| `combat.json` | Konstanten (Rüstung K = 160 und Deckel 0,75, Resistenz −25 % bis 60 %, Haltung K = 90), Zonenfaktoren, Angriffsfaktoren, Streuung, Anführer-Stufen, `creature_hp_scale` |

Dazu kommen rund 200 neue Einträge in `items.json` für die Kreaturenbeute, die Tierteile aus Abschnitt 8.2 und Metall-Gegenstände. Rezepte bekommen das Feld `min_level`.

**Zu Geminis C++-Beispielen:**
- Die Formeln für Rüstung, Resistenz und Haltung (Abschnitt 9.2 dort) sind sauber und können fast unverändert in einen Namensraum wie `GamerCombatMath` übernommen werden.
- Den Beute-Würfler (9.3) muss man neu auf die JSON-Strukturen bauen. Die Typen, auf die er sich stützt, fehlen im Dokument: `CreatureMasterTypes.h`, `FLootEntry`, `FLootBiomeOverride`, `FCreatureLootRow`, `FAdjustedLootRoll`, `EvaluateFairDownscaledLoot`. So kompiliert er nicht.
- Gemini benutzt Unreal-DataTables. Unser Projekt liest JSON über `GamerDataLoader` (bestätigt 2026-11-13).
- Die Herstell-Bibliothek (9.4) wird nicht gebraucht: Herstellen gibt es schon (`recipes.json`), es fehlt nur `min_level`.
- **Prüfungen** wie bei `GamerDataTests`: Jeder Gegenstand in der Beute existiert, jede Kreatur spawnt irgendwo, kein Spawn unter dem MinRang, Heimat-Regel (nie vor der eigenen Welle), keine Drachensehne unter Rang 5.
- Spawn-Würfe, Streuung und Beute laufen auf dem Server und werden repliziert (Regel 1, gilt auch für Dedicated Server).

---

<a id="d23-13"></a>

### 23.13 13. Was sich gegenüber Gemini ändert

| Gemini | bereinigt | Grund |
|---|---|---|
| 5 eigene Gebiete | 6 Biome, Gebiete zugeordnet | Entscheidung 2026-11-22 |
| Rang 1 (Level 1–5) ohne Kreaturen | Tiere und Menschen | Lücke |
| Captain-Ränge 7–25 | 3 sichtbare Anführer-Stufen mit denselben Höchstwerten | 19 kaum spürbare Ränge |
| Mengenbonus unbegrenzt | höchstens + 2 | Fehler: bei Rang 25 bis zu + 19 Stück |
| Nachzügler-Dämpfung auf alle Beute, dazu Kerndrop-Schutz | nur auf Biom-Zusatzbeute | doppelte Bestrafung |
| Rang-Beute oberhalb des erreichbaren Rangs | rückt auf den höchsten erreichbaren Rang | sonst nie erreichbar |
| Biom-Zusatzbeute in Biomen ohne die Kreatur | entfällt | tote Daten |
| 279 Beute-Namen | 197 Gegenstände | Doppelungen, Kombinationen, Sammelbegriffe |
| ein Rüstungswert | Rüstung pro Trefferzone aus den Slots | bestätigte Trefferzonen |
| Ausrüstung aus Kreaturenteilen | Metall-Leiter plus Kreaturenteile | Entscheidung 2026-11-22 |
| Streuung unsichtbar | ein Wurf, Größe folgt | Lesbarkeit |
| Haltung ohne Abbau | Abbau nach 3 s | sonst bricht jede Leiste irgendwann |
| keine Spieler-LP, keine Erfahrung | Abschnitte 3 und 4 | fehlte |
| „Ausdauer-Druck“, Ausdauer-Kosten | nur langsamer und schwächer | Regel 5 |
| 200-cm-Raster | 300 cm | so gebaut |
| Praxis-Beispiel 3.1: „86,25 Rohschaden“ | 50 × 1,15 × 1,4 = 80,5 | Rechenfehler bei Gemini (mit 1,5 statt 1,4 gerechnet) |
| JSON-Beispiele 6.5 mit CamelCase-IDs, teils andere Mengen als in den Tabellen | Tabellen gelten, IDs `snake_case` | Widerspruch im Dokument |
| „Delta == 0“ unter der Überschrift „Delta > 0“ | aufgelöst in 10.1 | Unklarheit |
| C++ mit DataTables und fehlenden Typen | JSON, Typen neu | Projektformat |

Unverändert übernommen: die drei Formeln (Rüstung, Resistenz, Haltung), die Archetyp-Faktoren, die Werte der 48 Kreaturen, die Spawn-Idee mit Mitwachsen und Nachzüglern, die Biom-Zusatzbeute als Idee und die Beute-Chancen.

---

<a id="d23-14"></a>

### 23.14 14. Offen

- **Zahlen nach dem ersten Kampftest:** Kreaturen-LP (Regler `creature_hp_scale`), Anführer-Faktoren, Streuung, Erfahrungskurve.
- **Skilltree:** wofür die 59 Fähigkeitspunkte ausgegeben werden.
- **Metall-Gegenstände** (Erze, Barren, Nieten, Ringe) und ihre Rezepte fehlen in `items.json`.
- **Icons** für rund 200 neue Gegenstände: größter Einzelposten der Beute.
- **Umweltgefahren** Hitze und Sturm brauchen Durst und Wetter.
- **Bosse:** Phasen und Mechaniken mit der Story-Sitzung „Sagen-Bosse“ abstimmen.
- **Namen der Kreaturen:** Arbeitsnamen behalten oder nordische Anzeigenamen? Markenprüfung vor dem Release.
- **Nächster Schritt „Daten alle“:** die JSON-Tabellen aus Abschnitt 12 aus `tools/data/creatures_from_gemini.py` heraus erzeugen, sobald der Lader dafür steht.


[↑ zum Inhaltsverzeichnis](#inhaltsverzeichnis)

<a id="d24"></a>

## 24. Kreaturen-Daten (erzeugt)

> Quelle: `docs/22_kreaturen_daten.md`

> **Erzeugt** am 2026-11-22 von `tools/data/creatures_from_gemini.py` aus `design/referenz/gemini_kuestenasche_gesamtdokumentation.md`. **Nicht von Hand ändern:** Zuordnungen im Skript anpassen und neu erzeugen. Regeln und Begründungen: `docs/21_kreaturen_level_kampf.md`.
>
> Status: Einordnung, Spawn und Beute-Zuordnung sind **Vorschläge von Claude** (2026-11-22). Die Werte (LP / Haltung / Schaden), Spawn-Gewichte für Nr. 31–48 und die Beute-Chancen sind **Geminis Startwerte**.

<a id="d24-1"></a>

### 24.1 1. Überblick

| Was | Anzahl |
|---|---|
| Kreaturen | 48 |
| Beute-Einträge bei Gemini | 361 |
| verschiedene Beute-Namen bei Gemini | 279 |
| davon „A & B“-Einträge, die in zwei Gegenstände geteilt werden | 15 |
| Biom-Zusatzbeute, die entfällt (Kreatur kommt in dem Biom nicht vor) | 33 |
| Rang-Beute, die auf den höchsten erreichbaren Rang vorgezogen wird | 19 |
| **Gegenstände nach der Bereinigung** | **197** |
| davon kreaturentypische Teile | 158 |
| davon Biom-Materialien und Knorpel (feste Stufe) | 19 |
| davon allgemeine Teile je Stufe, die Kreaturen geben (Sehne, Knochen, Haut, Horn, Drachensehne; Stufe 1 kommt mit den Tieren dazu) | 20 |

Für jeden dieser Gegenstände braucht es später einen Eintrag in `items.json` und ein Icon.

<a id="d24-2"></a>

### 24.2 2. Kreaturen nach Modell-Welle

Eine Kreatur kommt nur in ihrem Heimat-Biom und in späteren Biomen vor. Ihr Modell wird mit der Welle ihres Heimat-Bioms gebaut (Entscheidung „Kreaturen: Reihenfolge“, 2026-11-22).

| Welle | Heimat-Biom | Anzahl | Kreaturen |
|---|---|---|---|
| Kern | Fjordwiesen | 0 | keine (Tiere, Waldbanditen, Alter Keiler) |
| Kern (1) | Gezeitenmarsch | 4 | Thornmaw, Mirecrawler, Bramblefang, Siltstrider |
| 2 | Hügelgrab-Heide | 13 | Osskarr, Morrowear, Dustmote, Graveclaw, Hollowback, Shroudwalker, Wiedergänger, Sporeback, Skitterclaw, Gloomstalker, Charmaw, Vaultcrawler, Mossgolem |
| 3 | Basaltfjell | 14 | Vaelgorn, Murghul, Kraggar, Rimestalker, Ironhide, Cragmaw, Troll, Riese, Cragdrake, Rimehound, Slagbrute, Cragstalker, Frostmauler, Stormwyrmling |
| 4 | Glutküste | 10 | Cinderloom, Pyroclast, Dravik, Ashstalker, Cinderhound, Cinderwyrm, Ashwing, Cinderling, Ashweaver, Cindercolossus |
| 5 | Eisrand | 7 | Galeborn, Dreadcolossus, Stormmaw, Frostdrake, Dreadwyrm, Galehound, Galewyrm |

<a id="d24-3"></a>

### 24.3 3. Einordnung aller 48 Kreaturen

| Nr. | ID | Name | Rolle | MinRang | Heimat | auch in | erreichte Ränge | Welle |
|---|---|---|---|---|---|---|---|---|
| 1 | `creature_vaelgorn` | Vaelgorn | Krustenwesen mit Mineralplatten | 3 | Basaltfjell | Glutküste | 3–6 | 3 |
| 2 | `creature_cinderloom` | Cinderloom | glimmender Faserweber mit Glutknoten | 4 | Glutküste | – | 4–6 | 4 |
| 3 | `creature_osskarr` | Osskarr | Knochenwesen mit Gelenkpanzer und Stofffetzen | 3 | Hügelgrab-Heide | Basaltfjell | 3–5 | 2 |
| 4 | `creature_morrowear` | Morrowear | Lauscher mit großem Schwingungsorgan | 3 | Hügelgrab-Heide | Basaltfjell | 3–5 | 2 |
| 5 | `creature_pyroclast` | Pyroclast | grabender Schlacke-Panzerträger | 4 | Glutküste | – | 4–6 | 4 |
| 6 | `creature_dravik` | Dravik | schwer gepanzerter Gräber mit Schuppenplatten | 5 | Glutküste | – | 5–6 | 4 |
| 7 | `creature_murghul` | Murghul | Chitin-Jäger mit Sichelklauen | 3 | Basaltfjell | – | 3–5 | 3 |
| 8 | `creature_kraggar` | Kraggar | Fels-Raubtier mit Granithaut | 4 | Basaltfjell | Eisrand | 4–6 | 3 |
| 9 | `creature_galeborn` | Galeborn | Sturmwesen aus Faserbändern mit geladenen Kernen | 5 | Eisrand | – | 5–6 | 5 |
| 10 | `creature_ashstalker` | Ashstalker | Asche-Pirscher mit leichtem Balg | 2 | Glutküste | – | 4–6 | 4 |
| 11 | `creature_thornmaw` | Thornmaw | Dornenmaul mit verholzter Haut | 2 | Gezeitenmarsch | Hügelgrab-Heide | 2–3 | 1 |
| 12 | `creature_dustmote` | Dustmote | kleiner, flinker Staubflatterer (Schwarm) | 2 | Hügelgrab-Heide | Glutküste | 2–4 | 2 |
| 13 | `creature_mirecrawler` | Mirecrawler | Schlick-Kriecher mit breiten Greifpratzen | 2 | Gezeitenmarsch | – | 2–3 | 1 |
| 14 | `creature_cinderhound` | Cinderhound | Glut-Jagdhund im Rudel | 3 | Glutküste | – | 4–6 | 4 |
| 15 | `creature_graveclaw` | Graveclaw | Grabkrallen-Wühler mit Schutt-Schultern | 3 | Hügelgrab-Heide | – | 3–4 | 2 |
| 16 | `creature_rimestalker` | Rimestalker | kaltgrauer Frost-Pirscher mit Hornspitzen | 4 | Basaltfjell | Eisrand | 4–6 | 3 |
| 17 | `creature_hollowback` | Hollowback | Hohlrücken, stößt Staub- und Sporenwolken aus | 4 | Hügelgrab-Heide | Glutküste | 4–6 | 2 |
| 18 | `creature_ironhide` | Ironhide | schwerer Eisenkrusten-Panzer | 4 | Basaltfjell | Glutküste | 4–6 | 3 |
| 19 | `creature_shroudwalker` | Shroudwalker | Schattenweber der Gewölbe mit glimmendem Kern | 4 | Hügelgrab-Heide | – | 4 | 2 |
| 20 | `creature_cragmaw` | Cragmaw | Felskiefer-Räuber mit Steinkamm | 5 | Basaltfjell | Eisrand | 5–6 | 3 |
| 21 | `creature_dreadcolossus` | Dreadcolossus | uralter Koloss aus Stein und Relikten, sehr selten | 6 | Eisrand | – | 6 | 5 |
| 22 | `creature_troll` | Troll | Troll mit dicker Haut | 4 | Basaltfjell | – | 4–5 | 3 |
| 23 | `creature_giant` | Riese | Riese mit Rüstungsresten | 5 | Basaltfjell | Eisrand | 5–6 | 3 |
| 24 | `creature_revenant` | Wiedergänger | Untoter aus den Grabhügeln | 3 | Hügelgrab-Heide | – | 3–4 | 2 |
| 25 | `creature_cinderwyrm` | Cinderwyrm | Glut-Drache | 5 | Glutküste | – | 5–6 | 4 |
| 26 | `creature_cragdrake` | Cragdrake | Fels-Drache der Gipfel | 5 | Basaltfjell | Eisrand | 5–6 | 3 |
| 27 | `creature_stormmaw` | Stormmaw | großer Sturm-Drache | 6 | Eisrand | – | 6 | 5 |
| 28 | `creature_ashwing` | Ashwing | Asche-Gleitdrache | 5 | Glutküste | – | 5–6 | 4 |
| 29 | `creature_frostdrake` | Frostdrake | Frost-Drache | 5 | Eisrand | – | 5–6 | 5 |
| 30 | `creature_dreadwyrm` | Dreadwyrm | Spitzen-Drache, sehr selten | 6 | Eisrand | – | 6 | 5 |
| 31 | `creature_bramblefang` | Bramblefang | Dornen-Raubtier, schnelle Schnappangriffe, leicht aus dem Tritt zu bringen | 2 | Gezeitenmarsch | Hügelgrab-Heide | 2–3 | 1 |
| 32 | `creature_siltstrider` | Siltstrider | langbeiniger Senkenläufer, stolpert bei Flankentreffern | 2 | Gezeitenmarsch | Hügelgrab-Heide, Glutküste | 2–3 | 1 |
| 33 | `creature_cinderling` | Cinderling | kleiner Funkenläufer, flankiert in Gruppen | 2 | Glutküste | – | 2–3 | 4 |
| 34 | `creature_sporeback` | Sporeback | buckliger Schleicher, platzt bei Haltungsbruch zur Sporenwolke | 3 | Hügelgrab-Heide | – | 3–4 | 2 |
| 35 | `creature_skitterclaw` | Skitterclaw | vielbeiniger Mauer- und Felskrabbler | 3 | Hügelgrab-Heide | Basaltfjell | 3–4 | 2 |
| 36 | `creature_gloomstalker` | Gloomstalker | Schattenpirscher, lauert in dunklen Ecken | 3 | Hügelgrab-Heide | – | 3–4 | 2 |
| 37 | `creature_charmaw` | Charmaw | Glut-Beißer, beißt sich fest | 3 | Hügelgrab-Heide | Glutküste | 3–6 | 2 |
| 38 | `creature_rimehound` | Rimehound | Frost-Rudeljäger, bremst die Ausdauer-Erholung | 4 | Basaltfjell | Eisrand | 4–5 | 3 |
| 39 | `creature_slagbrute` | Slagbrute | Schlacke-Schläger mit dicker Frontkruste | 4 | Basaltfjell | Glutküste | 4–6 | 3 |
| 40 | `creature_vaultcrawler` | Vaultcrawler | gepanzerter Gewölbekletterer mit Greifarmen | 4 | Hügelgrab-Heide | – | 4 | 2 |
| 41 | `creature_mossgolem` | Mossgolem | Wächter aus Moos, Wurzeln und altem Gestein | 4 | Hügelgrab-Heide | Basaltfjell | 4–5 | 2 |
| 42 | `creature_galehound` | Galehound | Sturm-Läufer, lädt vor Stößen hörbar auf | 5 | Eisrand | – | 5–6 | 5 |
| 43 | `creature_cragstalker` | Cragstalker | Grat-Jäger mit weiten Sprüngen | 5 | Basaltfjell | Eisrand | 5–6 | 3 |
| 44 | `creature_ashweaver` | Ashweaver | Asche-Weber mit Netzen und glimmenden Schwachpunkten | 5 | Glutküste | – | 5–6 | 4 |
| 45 | `creature_frostmauler` | Frostmauler | massiger Frost-Brecher mit Bodenstößen | 5 | Basaltfjell | Eisrand | 5–6 | 3 |
| 46 | `creature_stormwyrmling` | Stormwyrmling | junger Sturm-Drache, Vorbote der Sturm-Spitzen | 5 | Basaltfjell | Eisrand | 5–6 | 3 |
| 47 | `creature_cindercolossus` | Cindercolossus | glühender Schlacke-Koloss, Schwachstellen in den Gelenken | 6 | Glutküste | – | 6 | 4 |
| 48 | `creature_galewyrm` | Galewyrm | Spitzen-Sturm-Drache mit Schwingenböen | 6 | Eisrand | – | 6 | 5 |

<a id="d24-4"></a>

### 24.4 4. Werte je Rang (LP / Haltung / Schaden)

Geminis Basiswerte vor Streuung (85–115 %) und vor Anführer-Stufen. Werte für Ränge, die die Kreatur in unserer Biom-Ordnung nie erreicht, stehen in Klammern.

| Nr. | Name | Rang 2 | Rang 3 | Rang 4 | Rang 5 | Rang 6 |
|---|---|---|---|---|---|---|
| 1 | Vaelgorn | – | 640 / 70 / 19 | 890 / 92 / 24 | 1.220 / 118 / 30 | 1.650 / 145 / 38 |
| 2 | Cinderloom | – | – | 940 / 88 / 25 | 1.280 / 112 / 31 | 1.720 / 138 / 39 |
| 3 | Osskarr | – | 620 / 68 / 19 | 860 / 90 / 24 | 1.180 / 114 / 30 | (1.580 / 140 / 37) |
| 4 | Morrowear | – | 660 / 75 / 18 | 910 / 96 / 23 | 1.240 / 120 / 29 | (1.660 / 148 / 36) |
| 5 | Pyroclast | – | – | 1.080 / 122 / 26 | 1.480 / 152 / 33 | 1.980 / 185 / 41 |
| 6 | Dravik | – | – | – | 1.560 / 155 / 34 | 2.120 / 192 / 43 |
| 7 | Murghul | – | 580 / 58 / 19 | 810 / 76 / 24 | 1.100 / 96 / 30 | (1.480 / 118 / 37) |
| 8 | Kraggar | – | – | 1.120 / 130 / 25 | 1.520 / 160 / 32 | 2.050 / 195 / 40 |
| 9 | Galeborn | – | – | – | 1.420 / 132 / 34 | 1.920 / 165 / 43 |
| 10 | Ashstalker | (390 / 40 / 13) | (580 / 56 / 18) | 800 / 74 / 23 | 1.080 / 94 / 29 | 1.440 / 115 / 36 |
| 11 | Thornmaw | 420 / 45 / 13 | 620 / 62 / 18 | (860 / 82 / 23) | (1.150 / 104 / 29) | (1.540 / 128 / 36) |
| 12 | Dustmote | 260 / 25 / 10 | 390 / 35 / 14 | 550 / 48 / 19 | (750 / 64 / 24) | (1.000 / 78 / 30) |
| 13 | Mirecrawler | 450 / 50 / 12 | 660 / 68 / 17 | (910 / 88 / 22) | (1.220 / 110 / 28) | (1.620 / 135 / 35) |
| 14 | Cinderhound | – | (610 / 60 / 19) | 840 / 78 / 24 | 1.140 / 98 / 30 | 1.520 / 120 / 37 |
| 15 | Graveclaw | – | 680 / 76 / 19 | 940 / 98 / 24 | (1.280 / 124 / 31) | (1.700 / 152 / 38) |
| 16 | Rimestalker | – | – | 890 / 82 / 25 | 1.220 / 104 / 32 | 1.640 / 128 / 40 |
| 17 | Hollowback | – | – | 960 / 96 / 24 | 1.320 / 120 / 30 | 1.780 / 148 / 38 |
| 18 | Ironhide | – | – | 1.150 / 135 / 26 | 1.580 / 168 / 33 | 2.150 / 205 / 41 |
| 19 | Shroudwalker | – | – | 880 / 80 / 25 | (1.200 / 102 / 32) | (1.620 / 126 / 40) |
| 20 | Cragmaw | – | – | – | 1.500 / 148 / 34 | 2.020 / 182 / 42 |
| 21 | Dreadcolossus | – | – | – | – | 3.200 / 255 / 54 |
| 22 | Troll | – | – | 1.140 / 132 / 27 | 1.560 / 164 / 34 | (2.100 / 200 / 42) |
| 23 | Riese | – | – | – | 1.780 / 180 / 37 | 2.450 / 222 / 46 |
| 24 | Wiedergänger | – | 640 / 72 / 19 | 880 / 94 / 24 | (1.200 / 118 / 30) | (1.600 / 145 / 37) |
| 25 | Cinderwyrm | – | – | – | 1.950 / 168 / 38 | 2.750 / 215 / 48 |
| 26 | Cragdrake | – | – | – | 2.050 / 182 / 37 | 2.850 / 230 / 47 |
| 27 | Stormmaw | – | – | – | – | 3.100 / 235 / 52 |
| 28 | Ashwing | – | – | – | 1.880 / 155 / 37 | 2.650 / 198 / 46 |
| 29 | Frostdrake | – | – | – | 2.000 / 175 / 37 | 2.800 / 220 / 47 |
| 30 | Dreadwyrm | – | – | – | – | 3.400 / 265 / 55 |
| 31 | Bramblefang | 410 / 42 / 13 | 610 / 60 / 18 | (840 / 78 / 23) | (1.120 / 98 / 29) | (1.480 / 120 / 36) |
| 32 | Siltstrider | 440 / 48 / 12 | 650 / 68 / 17 | (890 / 88 / 22) | (1.190 / 110 / 28) | (1.560 / 135 / 35) |
| 33 | Cinderling | 280 / 28 / 11 | 410 / 38 / 15 | (580 / 52 / 20) | (790 / 68 / 25) | (1.060 / 85 / 31) |
| 34 | Sporeback | – | 690 / 72 / 19 | 940 / 92 / 24 | (1.260 / 115 / 30) | (1.680 / 142 / 37) |
| 35 | Skitterclaw | – | 560 / 55 / 18 | 780 / 72 / 23 | (1.060 / 90 / 29) | (1.420 / 112 / 36) |
| 36 | Gloomstalker | – | 600 / 62 / 19 | 830 / 80 / 24 | (1.120 / 100 / 30) | (1.500 / 122 / 37) |
| 37 | Charmaw | – | 720 / 82 / 20 | 980 / 102 / 26 | 1.310 / 126 / 32 | 1.740 / 155 / 39 |
| 38 | Rimehound | – | – | 860 / 78 / 25 | 1.180 / 98 / 32 | (1.580 / 120 / 40) |
| 39 | Slagbrute | – | – | 1.100 / 128 / 26 | 1.500 / 158 / 33 | 2.020 / 192 / 41 |
| 40 | Vaultcrawler | – | – | 930 / 95 / 24 | (1.270 / 118 / 30) | (1.700 / 145 / 37) |
| 41 | Mossgolem | – | – | 1.160 / 135 / 25 | 1.580 / 165 / 32 | (2.120 / 200 / 40) |
| 42 | Galehound | – | – | – | 1.220 / 102 / 31 | 1.660 / 126 / 39 |
| 43 | Cragstalker | – | – | – | 1.480 / 145 / 33 | 1.980 / 178 / 41 |
| 44 | Ashweaver | – | – | – | 1.340 / 115 / 31 | 1.800 / 142 / 38 |
| 45 | Frostmauler | – | – | – | 1.650 / 170 / 35 | 2.250 / 210 / 44 |
| 46 | Stormwyrmling | – | – | – | 1.900 / 160 / 36 | 2.620 / 202 / 45 |
| 47 | Cindercolossus | – | – | – | – | 2.800 / 245 / 50 |
| 48 | Galewyrm | – | – | – | – | 2.900 / 215 / 53 |

**Mittelwerte je Rang** (über alle Kreaturen mit Werten für diesen Rang; Grundlage für Tiere und Bosse):

| Rang | LP | Haltung | Schaden | Kreaturen |
|---|---|---|---|---|
| 2 | 379 | 40 | 12 | 7 |
| 3 | 607 | 63 | 18 | 18 |
| 4 | 910 | 93 | 24 | 30 |
| 5 | 1.368 | 127 | 32 | 43 |
| 6 | 1.978 | 166 | 41 | 48 |

<a id="d24-5"></a>

### 24.5 5. Spawn-Tabellen je Biom

Biom-Rang = Stufe des Bioms; an gefährlichen Orten gilt Rang + 1 (höchstens 6). „Wächst mit (85 %)“: 85 % erreichen den Rang des Ortes, der Rest einen Rang darunter (mindestens MinRang).

#### Fjordwiesen (`biome_fjord_meadow`, Rang 1; gefährliche Orte Rang 2: Banditenlager, Wolfshöhlen)

Keine Kreaturen (bestätigt). Tiere: Wildschwein, Rothirsch, Hase, Fuchs, Wolf. Menschen: Waldbanditen. Boss: Der Alte Keiler (realistisch).

#### Gezeitenmarsch (`biome_tidal_marsh`, Rang 2; gefährliche Orte Rang 3: Wracks, tiefe Priele)

Dazu Robbe und Seevögel (friedlich), Strandplünderer. Boss: Die Schlickmutter als Moorhexe mit Irrlichtern (`creature_will_o_wisp`).

| Kreatur | Gewicht | Rang-Verhalten | wo | Ränge hier | Welle |
|---|---|---|---|---|---|
| Bramblefang | 1,25 | wächst mit (immer) | überall | 2–3 | 1 |
| Thornmaw | 1,00 | wächst mit (85 %) | überall | 2–3 | 1 |
| Mirecrawler | 1,10 | wächst mit (85 %) | überall | 2–3 | 1 |
| Siltstrider | 0,90 | wächst mit (85 %) | überall | 2–3 | 1 |

#### Hügelgrab-Heide (`biome_barrow_heath`, Rang 3; gefährliche Orte Rang 4: Grabhügel, Gewölbe, Grabfestung)

Dazu Rothirsch, Hase, Wolf, Bär, Elch; Waldbanditen, Eidbrecher. Boss: Der Hügelkönig als Draugr-Fürst (die Wiedergänger sind sein Gefolge).

| Kreatur | Gewicht | Rang-Verhalten | wo | Ränge hier | Welle |
|---|---|---|---|---|---|
| Osskarr | 1,00 | wächst mit (85 %) | überall | 3–4 | 2 |
| Morrowear | 0,80 | wächst mit (85 %) | überall | 3–4 | 2 |
| Graveclaw | 1,00 | wächst mit (85 %) | überall | 3–4 | 2 |
| Wiedergänger | 1,10 | wächst mit (85 %) | überall | 3–4 | 2 |
| Dustmote | 1,00 | wächst mit (85 %) | überall | 2–4 | 2 |
| Sporeback | 1,05 | wächst mit (85 %) | überall | 3–4 | 2 |
| Skitterclaw | 1,15 | wächst mit (85 %) | überall | 3–4 | 2 |
| Gloomstalker | 0,95 | wächst mit (80 %) | überall | 3–4 | 2 |
| Charmaw | 0,70 | wächst mit bis Rang 4 (60 %) | überall | 3–4 | 2 |
| Bramblefang | 0,60 | fest Rang 2 (Nachzügler) | überall | 2 | 1 |
| Siltstrider | 0,50 | fest Rang 2 (Nachzügler) | überall | 2 | 1 |
| Thornmaw | 0,50 | fest Rang 2 (Nachzügler) | überall | 2 | 1 |
| Hollowback | 0,80 | wächst mit (85 %) | nur gefährliche Orte | 4 | 2 |
| Shroudwalker | 0,80 | wächst mit (85 %) | nur gefährliche Orte | 4 | 2 |
| Vaultcrawler | 0,90 | wächst mit (85 %) | nur gefährliche Orte | 4 | 2 |
| Mossgolem | 0,60 | wächst mit (70 %) | nur gefährliche Orte | 4 | 2 |

#### Basaltfjell (`biome_basalt_fell`, Rang 4; gefährliche Orte Rang 5: Gipfel, Eishöhlen)

Dazu Wolf, Bär, Rentier, Luchs; Eidbrecher. Boss: Der Felsenvater als Troll (der größte und älteste Troll).

| Kreatur | Gewicht | Rang-Verhalten | wo | Ränge hier | Welle |
|---|---|---|---|---|---|
| Vaelgorn | 0,90 | wächst mit (85 %) | überall | 3–5 | 3 |
| Murghul | 0,90 | wächst mit (85 %) | überall | 3–5 | 3 |
| Kraggar | 1,00 | wächst mit (85 %) | überall | 4–5 | 3 |
| Rimestalker | 0,90 | wächst mit (85 %) | überall | 4–5 | 3 |
| Ironhide | 0,80 | wächst mit (85 %) | überall | 4–5 | 3 |
| Troll | 0,50 | wächst mit (85 %) | überall | 4–5 | 3 |
| Rimehound | 1,05 | wächst mit (85 %) | überall | 4–5 | 3 |
| Slagbrute | 0,70 | wächst mit bis Rang 5 (65 %) | überall | 4–5 | 3 |
| Mossgolem | 0,85 | wächst mit (85 %) | überall | 4–5 | 2 |
| Osskarr | 0,60 | wächst mit (85 %) | überall | 3–5 | 2 |
| Morrowear | 0,50 | wächst mit (85 %) | überall | 3–5 | 2 |
| Skitterclaw | 0,70 | fest Rang 3 (Nachzügler) | überall | 3 | 2 |
| Cragmaw | 0,80 | wächst mit (85 %) | nur gefährliche Orte | 5 | 3 |
| Cragdrake | 0,30 | wächst mit (85 %) | nur gefährliche Orte | 5 | 3 |
| Cragstalker | 0,85 | wächst mit (85 %) | nur gefährliche Orte | 5 | 3 |
| Riese | 0,35 | wächst mit (85 %) | nur gefährliche Orte | 5 | 3 |
| Frostmauler | 0,75 | wächst mit (85 %) | nur gefährliche Orte | 5 | 3 |
| Stormwyrmling | 0,45 | wächst mit (80 %) | nur gefährliche Orte | 5 | 3 |

#### Glutküste (`biome_ember_coast`, Rang 5; gefährliche Orte Rang 6: Lavafelder, Kultstätten)

Keine Tiere; Aschebund-Kult. Boss: Die Aschezunge als Lindwurm.

| Kreatur | Gewicht | Rang-Verhalten | wo | Ränge hier | Welle |
|---|---|---|---|---|---|
| Cinderloom | 0,90 | wächst mit (85 %) | überall | 4–6 | 4 |
| Pyroclast | 0,90 | wächst mit (85 %) | überall | 4–6 | 4 |
| Dravik | 0,80 | wächst mit (85 %) | überall | 5–6 | 4 |
| Ashstalker | 1,10 | wächst mit (85 %) | überall | 4–6 | 4 |
| Cinderhound | 1,00 | wächst mit (85 %) | überall | 4–6 | 4 |
| Charmaw | 1,00 | wächst mit (85 %) | überall | 4–6 | 2 |
| Slagbrute | 1,05 | wächst mit (85 %) | überall | 4–6 | 3 |
| Ashweaver | 0,75 | wächst mit (85 %) | überall | 5–6 | 4 |
| Cinderwyrm | 0,35 | wächst mit (85 %) | überall | 5–6 | 4 |
| Ashwing | 0,40 | wächst mit (85 %) | überall | 5–6 | 4 |
| Vaelgorn | 0,60 | wächst mit (85 %) | überall | 4–6 | 3 |
| Ironhide | 0,60 | wächst mit (85 %) | überall | 4–6 | 3 |
| Hollowback | 0,50 | wächst mit (85 %) | überall | 4–6 | 2 |
| Cinderling | 1,30 | wächst mit bis Rang 3 (85 %) | überall | 2–3 | 4 |
| Siltstrider | 0,60 | fest Rang 2 (Nachzügler) | überall | 2 | 1 |
| Dustmote | 0,60 | fest Rang 2 (Nachzügler) | überall | 2 | 2 |
| Cindercolossus | 0,35 | wächst mit (immer) | nur gefährliche Orte | 6 | 4 |

#### Eisrand (`biome_ice_rim`, Rang 6; gefährliche Orte Rang 6: Gletscherspalten, gefrorene Wracks)

Dazu Rentier, Polarfuchs; Menschen laut Story. Boss: Der Sturmkoloss als Frostriese. Anführer-Stufen hier am häufigsten.

| Kreatur | Gewicht | Rang-Verhalten | wo | Ränge hier | Welle |
|---|---|---|---|---|---|
| Galeborn | 0,90 | wächst mit (85 %) | überall | 5–6 | 5 |
| Galehound | 1,05 | wächst mit (90 %) | überall | 5–6 | 5 |
| Cragstalker | 0,65 | wächst mit (80 %) | überall | 5–6 | 3 |
| Frostmauler | 0,60 | wächst mit (80 %) | überall | 5–6 | 3 |
| Stormwyrmling | 0,55 | wächst mit (85 %) | überall | 5–6 | 3 |
| Frostdrake | 0,35 | wächst mit (85 %) | überall | 5–6 | 5 |
| Stormmaw | 0,20 | wächst mit (immer) | überall | 6 | 5 |
| Galewyrm | 0,35 | wächst mit (immer) | überall | 6 | 5 |
| Dreadcolossus | 0,15 | wächst mit (immer) | überall | 6 | 5 |
| Dreadwyrm | 0,10 | wächst mit (immer) | überall | 6 | 5 |
| Riese | 0,50 | wächst mit (85 %) | überall | 5–6 | 3 |
| Kraggar | 0,50 | wächst mit (85 %) | überall | 5–6 | 3 |
| Rimestalker | 0,60 | wächst mit (85 %) | überall | 5–6 | 3 |
| Cragmaw | 0,40 | wächst mit (85 %) | überall | 5–6 | 3 |
| Cragdrake | 0,25 | wächst mit (85 %) | überall | 5–6 | 3 |
| Rimehound | 0,75 | fest Rang 4 (Nachzügler) | überall | 4 | 3 |

<a id="d24-6"></a>

### 24.6 6. Beute je Kreatur (bereinigt)

Spalten: **Menge** und **Chance** wie bei Gemini; **ab Rang** = Rang-Sperre; **+Menge** = Mengenbonus (+1 je Rang über „ab Rang“, höchstens +2). Allgemeine Teile (Sehne, Knochen, Haut, Horn, Drachensehne) bekommen die Stufe des Rangs, den die Kreatur beim Tod hat. Biom-Zusatzbeute fällt nur im genannten Biom und wird bei Nachzüglern gedämpft (siehe `docs/21_kreaturen_level_kampf.md`, Abschnitt 10).

#### 1. Vaelgorn (`creature_vaelgorn`, MinRang 3, erreicht 3–6)

| Art | Gegenstand | ID | Menge | Chance | ab Rang | +Menge | Gemini-Name |
|---|---|---|---|---|---|---|---|
| Grund | Mineral-Krustensplitter | `item_vaelgorn_crust_shard` | 1–2 | 0,90 | 3 | – | Mineral-Krustensplitter |
| Grund | Sehne (Stufe = Rang) | `item_sinew_t<Rang>` | 1–2 | 0,75 | 3 | – | Sehnenfasern |
| Grund | Knochen (Stufe = Rang) | `item_bone_t<Rang>` | 1–2 | 0,65 | 3 | – | Knochensplitter |
| Rang | Dichte Krustenplatte | `item_vaelgorn_crust_plate` | 1–2 | 0,60 | 4 | Ja | Dichtere Krustenplatten |
| Rang | Sehne (Stufe = Rang) | `item_sinew_t<Rang>` | 1–2 | 0,50 | 5 | Ja | Verstärkte Sehnenstränge |
| Biom: Glutküste | Asche-Mineral | `item_ash_mineral` | 1–2 | 0,50 | – | – | Asche-Mineral |
| Biom: Basaltfjell | Basaltbrocken | `item_basalt_chunk` | 1–2 | 0,45 | – | – | Gesteinssplitter |

Entfällt (Kreatur kommt dort nicht vor): Moosgeäst: Faser-/Harzreste.

#### 2. Cinderloom (`creature_cinderloom`, MinRang 4, erreicht 4–6)

| Art | Gegenstand | ID | Menge | Chance | ab Rang | +Menge | Gemini-Name |
|---|---|---|---|---|---|---|---|
| Grund | Verkohlte Faser | `item_charred_fiber` | 1–2 | 0,85 | 4 | – | Verkohlte Faserstränge |
| Grund | Glutknoten | `item_ember_knot` | 1–2 | 0,75 | 4 | – | Kleine Glutknoten |
| Grund | Asche-Mineral | `item_ash_mineral` | 1–2 | 0,65 | 4 | – | Ruinen-/Asche-Reste |
| Rang | Hitzefaser | `item_heat_fiber` | 1–2 | 0,55 | 5 | Ja | Zähe Hitze-Fasern |
| Rang | Stabile Glutkapsel | `item_ember_capsule_stable` | 1 | 0,35 | 6 | Ja | Stabile Glutkapseln |
| Biom: Glutküste | Schlackekern | `item_slag_core` | 1 | 0,50 | – | – | Glut-Schlacke |

Entfällt (Kreatur kommt dort nicht vor): Bruchstein: Alte Stoff-/Reliktfäden; Moosgeäst: Verkohltes Holz-/Faserstück.

#### 3. Osskarr (`creature_osskarr`, MinRang 3, erreicht 3–5)

| Art | Gegenstand | ID | Menge | Chance | ab Rang | +Menge | Gemini-Name |
|---|---|---|---|---|---|---|---|
| Grund | Knochengelenk | `item_osskarr_bone_joint` | 1–2 | 0,85 | 3 | – | Knochengelenke |
| Grund | Sehne (Stufe = Rang) | `item_sinew_t<Rang>` | 1–2 | 0,75 | 3 | – | Trockene Sehnenbänder |
| Grund | Stofffetzen | `item_cloth_scrap` | 1–2 | 0,65 | 3 | – | Grobe Gewebe-/Stofffetzen |
| Rang | Knochenstrebe | `item_osskarr_bone_strut` | 1–2 | 0,60 | 4 | Ja | Gehärtete Knochenstreben |
| Rang | Sehne (Stufe = Rang) | `item_sinew_t<Rang>` | 1–2 | 0,45 | 5 | Ja | Lange Sehnen |
| Biom: Hügelgrab-Heide | Verfall-Relikt | `item_decay_relic` | 1 | 0,45 | – | – | Verfall-Relikt |
| Biom: Basaltfjell | Basaltbrocken | `item_basalt_chunk` | 1–2 | 0,40 | – | – | Felsstaub-/Splitterreste |

Entfällt (Kreatur kommt dort nicht vor): Moosgeäst: Waldfaser.

#### 4. Morrowear (`creature_morrowear`, MinRang 3, erreicht 3–5)

| Art | Gegenstand | ID | Menge | Chance | ab Rang | +Menge | Gemini-Name |
|---|---|---|---|---|---|---|---|
| Grund | Schwingungsorgan | `item_morrowear_ear` | 1 | 0,80 | 3 | – | Hör-/Schwingungsorgan |
| Grund | Haut (Stufe = Rang) | `item_hide_t<Rang>` | 1–2 | 0,80 | 3 | – | Dicke stumpfe Haut |
| Grund | Knorpel | `item_cartilage` | 1–2 | 0,65 | 3 | – | Knorpelreste |
| Rang | Feines Schwingungsorgan | `item_morrowear_ear_fine` | 1–2 | 0,55 | 4 | Ja | Ausgeprägtere Schwingungsorgane |
| Rang | Haut (Stufe = Rang) | `item_hide_t<Rang>` | 1–2 | 0,45 | 5 | Ja | Zähere Häute (vorgezogen von R6) |
| Biom: Hügelgrab-Heide | Ruinenstaub | `item_ruin_dust` | 1–2 | 0,45 | – | – | Staub-/Asche-Korn |
| Biom: Basaltfjell | Knorpel | `item_cartilage` | 1 | 0,40 | – | – | Gebirgs-Knorpel |

Entfällt (Kreatur kommt dort nicht vor): Moosgeäst: Feuchtes Balg-/Lederstück.

#### 5. Pyroclast (`creature_pyroclast`, MinRang 4, erreicht 4–6)

| Art | Gegenstand | ID | Menge | Chance | ab Rang | +Menge | Gemini-Name |
|---|---|---|---|---|---|---|---|
| Grund | Schlackeplatte | `item_slag_plate` | 1–2 | 0,85 | 4 | – | Schlacke-Panzerstücke |
| Grund | Glühgestein | `item_pyroclast_glowstone` | 1–2 | 0,75 | 4 | – | Glühendes Gestein |
| Grund | Schwere Grabklaue | `item_digging_claw_heavy` | 1–2 | 0,65 | 4 | – | Schwere Grabklauen |
| Rang | Dichte Schlackeplatte | `item_slag_plate_dense` | 1–2 | 0,55 | 5 | Ja | Dichte Schlackeplatten |
| Rang | Hitzekern | `item_heat_core` | 1 | 0,35 | 6 | Ja | Hitzebeständige Kerne/Brocken |
| Biom: Glutküste | Schlackekern | `item_slag_core` | 1 | 0,50 | – | – | Schlacke-Kern |

Entfällt (Kreatur kommt dort nicht vor): Bruchstein: Ruinen-Gestein; Schroffen-Grate: Geröll-Erzrest.

#### 6. Dravik (`creature_dravik`, MinRang 5, erreicht 5–6)

| Art | Gegenstand | ID | Menge | Chance | ab Rang | +Menge | Gemini-Name |
|---|---|---|---|---|---|---|---|
| Grund | Dravik-Schuppenplatte | `item_dravik_scale_plate` | 1–3 | 0,90 | 5 | – | Dicke Horn-/Schuppenplatten |
| Grund | Grabklaue | `item_digging_claw` | 1–2 | 0,75 | 5 | – | Grabklauen |
| Grund | Sehne (Stufe = Rang) | `item_sinew_t<Rang>` | 1–2 | 0,70 | 5 | – | Zähe Dravik-Sehnen |
| Rang | Schwere Dravik-Schuppe | `item_dravik_scale_heavy` | 1–2 | 0,50 | 6 | Ja | Schwere Spitzen-Schuppen |
| Rang | Dravik-Hornkern | `item_dravik_horn_core` | 1 | 0,35 | 6 | Ja | Seltene Hochrang-Materialien |
| Biom: Glutküste | Horn (Stufe = Rang) | `item_horn_t<Rang>` | 1 | 0,50 | – | – | Asche-Horn |

Entfällt (Kreatur kommt dort nicht vor): Bruchstein / Schroffen-Grate: Panzer-Schutt.

#### 7. Murghul (`creature_murghul`, MinRang 3, erreicht 3–5)

| Art | Gegenstand | ID | Menge | Chance | ab Rang | +Menge | Gemini-Name |
|---|---|---|---|---|---|---|---|
| Grund | Sichelklaue | `item_murghul_sickle_claw` | 1–2 | 0,85 | 3 | – | Sichel-Greifklauen |
| Grund | Chitin-Plattenstück | `item_murghul_chitin` | 1–2 | 0,75 | 3 | – | Chitin-Plattenstücke |
| Grund | Horn (Stufe = Rang) | `item_horn_t<Rang>` | 1–2 | 0,65 | 3 | – | Hornstacheln |
| Rang | Gehärtete Sichelklaue | `item_murghul_sickle_claw_hard` | 1–2 | 0,60 | 4 | Ja | Gehärtete Sichelklauen |
| Rang | Dicker Chitinpanzer | `item_murghul_chitin_thick` | 1–2 | 0,45 | 5 | Ja | Dickere Chitinpanzer (vorgezogen von R6) |
| Biom: Basaltfjell | Horn (Stufe = Rang) | `item_horn_t<Rang>` | 1 | 0,45 | – | – | Fels-Stachel |

Entfällt (Kreatur kommt dort nicht vor): Bruchstein: Ruinen-Chitin.

#### 8. Kraggar (`creature_kraggar`, MinRang 4, erreicht 4–6)

| Art | Gegenstand | ID | Menge | Chance | ab Rang | +Menge | Gemini-Name |
|---|---|---|---|---|---|---|---|
| Grund | Felsklaue | `item_kraggar_rock_claw` | 1–2 | 0,85 | 4 | – | Fels-Klauen |
| Grund | Granithaut | `item_kraggar_granite_hide` | 1–2 | 0,80 | 4 | – | Granit-/Schiefer-Hautstücke |
| Grund | Horn (Stufe = Rang) | `item_horn_t<Rang>` | 1–2 | 0,65 | 4 | – | Hornkamm-Splitter |
| Rang | Massive Felsklaue | `item_kraggar_rock_claw_massive` | 1–2 | 0,55 | 5 | Ja | Massive Felsklauen |
| Rang | Erzader-Brocken | `item_kraggar_ore_vein` | 1 | 0,35 | 6 | Ja | Gebirgs-/Schmiede-Reste |
| Biom: Basaltfjell | Basaltbrocken | `item_basalt_chunk` | 1–2 | 0,55 | – | – | Hochgebirgs-Stein |
| Biom: Basaltfjell, Eisrand | Horn (Stufe = Rang) | `item_horn_t<Rang>` | 1 | 0,45 | – | – | Kanten-Horn |

#### 9. Galeborn (`creature_galeborn`, MinRang 5, erreicht 5–6)

| Art | Gegenstand | ID | Menge | Chance | ab Rang | +Menge | Gemini-Name |
|---|---|---|---|---|---|---|---|
| Grund | Sturmfaser | `item_storm_fiber` | 1–3 | 0,90 | 5 | – | Sturm-Faserbänder |
| Grund | Geladener Kernrest | `item_charged_core` | 1–2 | 0,70 | 5 | – | Geladene Kernreste |
| Grund | Hornspange | `item_galeborn_horn_clasp` | 1–2 | 0,70 | 5 | – | Hornspangen |
| Rang | Dichte Sturmfaser | `item_storm_fiber_dense` | 1–2 | 0,50 | 6 | Ja | Dichte Sturm-Fasern |
| Rang | Sturmdrüse | `item_galeborn_storm_gland` | 1 | 0,35 | 6 | Ja | Hochlagen-Komponenten |
| Biom: Eisrand | Sturmfaser | `item_storm_fiber` | 1–2 | 0,55 | – | – | Sturm-Faser |
| Biom: Eisrand | Hornspange | `item_galeborn_horn_clasp` | 1 | 0,40 | – | – | Ruinen-Spange |

#### 10. Ashstalker (`creature_ashstalker`, MinRang 2, erreicht 4–6)

| Art | Gegenstand | ID | Menge | Chance | ab Rang | +Menge | Gemini-Name |
|---|---|---|---|---|---|---|---|
| Grund | Aschebalg | `item_ashstalker_pelt` | 1–2 | 0,85 | 2 | – | Asche-Balg/Haut |
| Grund | Sehne (Stufe = Rang) | `item_sinew_t<Rang>` | 1–2 | 0,75 | 2 | – | Dünne Sehnen |
| Grund | Knochen (Stufe = Rang) | `item_bone_t<Rang>` | 1 | 0,65 | 2 | – | Leichte Knochenreste |
| Rang | Haut (Stufe = Rang) | `item_hide_t<Rang>` | 1–2 | 0,65 | 3 | Ja | Zähere Häute |
| Rang | Sehne (Stufe = Rang) | `item_sinew_t<Rang>` | 1–2 | 0,45 | 5 | Ja | Stärkere Sehnen |
| Biom: Glutküste | Verkohlte Faser | `item_charred_fiber` | 1–2 | 0,50 | – | – | Asche-Faser |

Entfällt (Kreatur kommt dort nicht vor): Moosgeäst: Unterholz-Reste; Bruchstein: Trümmer-Staub.

#### 11. Thornmaw (`creature_thornmaw`, MinRang 2, erreicht 2–3)

| Art | Gegenstand | ID | Menge | Chance | ab Rang | +Menge | Gemini-Name |
|---|---|---|---|---|---|---|---|
| Grund | Dornsplitter | `item_thornmaw_thorn` | 1–2 | 0,85 | 2 | – | Dornen-/Horn-Splitter |
| Grund | Verholzte Haut | `item_thornmaw_bark_hide` | 1–2 | 0,75 | 2 | – | Verholzte Hautstücke |
| Grund | Knorpel | `item_cartilage` | 1 | 0,65 | 2 | – | Kiefer-/Knorpelreste |
| Rang | Dicker Dorn | `item_thornmaw_thorn_thick` | 1–2 | 0,65 | 3 | Ja | Dickere Dorn-/Horn-Teile |
| Rang | Haut (Stufe = Rang) | `item_hide_t<Rang>` | 1–2 | 0,45 | 3 | Ja | Zähere Häute (vorgezogen von R5) |
| Biom: Gezeitenmarsch | Dornholz | `item_thorn_wood` | 1–2 | 0,50 | – | – | Dornen-Holz |

Entfällt (Kreatur kommt dort nicht vor): Schroffen-Grate: Gesteins-Horn.

#### 12. Dustmote (`creature_dustmote`, MinRang 2, erreicht 2–4)

| Art | Gegenstand | ID | Menge | Chance | ab Rang | +Menge | Gemini-Name |
|---|---|---|---|---|---|---|---|
| Grund | Staubhautlappen | `item_dustmote_skin` | 1–2 | 0,85 | 2 | – | Staubige Hautlappen |
| Grund | Fühlerfaser | `item_dustmote_feeler` | 1–2 | 0,70 | 2 | – | Feine Fühler-/Faserreste |
| Rang | Dichte Fühlerfaser | `item_dustmote_feeler_dense` | 1–3 | 0,60 | 3 | Ja | Dichtere Faser-/Staub-Teile |
| Biom: Hügelgrab-Heide, Glutküste | Ruinenstaub | `item_ruin_dust` | 1–2 | 0,45 | – | – | Feiner Staub-/Asche-Rest |

#### 13. Mirecrawler (`creature_mirecrawler`, MinRang 2, erreicht 2–3)

| Art | Gegenstand | ID | Menge | Chance | ab Rang | +Menge | Gemini-Name |
|---|---|---|---|---|---|---|---|
| Grund | Feuchthaut | `item_mirecrawler_hide` | 1–2 | 0,85 | 2 | – | Zähe Feuchthaut |
| Grund | Greifpratze | `item_mirecrawler_paw` | 1–2 | 0,75 | 2 | – | Breite Greifpratzen |
| Grund | Knorpel | `item_cartilage` | 1 | 0,65 | 2 | – | Knorpelreste |
| Rang | Haut (Stufe = Rang) | `item_hide_t<Rang>` | 1–2 | 0,65 | 3 | Ja | Dickere Häute |
| Rang | Kräftige Greifpratze | `item_mirecrawler_paw_strong` | 1–2 | 0,45 | 3 | Ja | Kräftigere Pratzen (vorgezogen von R5) |
| Biom: Gezeitenmarsch | Feuchtleder | `item_damp_leather` | 1–2 | 0,50 | – | – | Feucht-Leder/Schlammrest |

#### 14. Cinderhound (`creature_cinderhound`, MinRang 3, erreicht 4–6)

| Art | Gegenstand | ID | Menge | Chance | ab Rang | +Menge | Gemini-Name |
|---|---|---|---|---|---|---|---|
| Grund | Brandfell | `item_cinderhound_pelt` | 1–2 | 0,85 | 3 | – | Verkohlte Faser-/Fellreste |
| Grund | Funkenasche | `item_spark_ash` | 1–2 | 0,70 | 3 | – | Glimmende Funken-/Glut-Reste |
| Grund | Sehne (Stufe = Rang) | `item_sinew_t<Rang>` | 1–2 | 0,70 | 3 | – | Sehnen |
| Rang | Zähes Brandfell | `item_cinderhound_pelt_tough` | 1–2 | 0,55 | 4 | Ja | Zähere Brand-Häute |
| Rang | Sehne (Stufe = Rang) | `item_sinew_t<Rang>` | 1–2 | 0,45 | 6 | Ja | Stärkere Sehnen & Glut-Reste (geteilt) |
| Rang | Funkenasche | `item_spark_ash` | 1 | 0,45 | 6 | Ja | Stärkere Sehnen & Glut-Reste (geteilt) |
| Biom: Glutküste | Funkenasche | `item_spark_ash` | 1–2 | 0,50 | – | – | Glut-Funke |
| Biom: Glutküste | Sehne (Stufe = Rang) | `item_sinew_t<Rang>` | 1 | 0,40 | – | – | Asche-Sehne |

#### 15. Graveclaw (`creature_graveclaw`, MinRang 3, erreicht 3–4)

| Art | Gegenstand | ID | Menge | Chance | ab Rang | +Menge | Gemini-Name |
|---|---|---|---|---|---|---|---|
| Grund | Grabklaue | `item_digging_claw` | 1–2 | 0,85 | 3 | – | Grabklauen |
| Grund | Schutt-Schulterplatte | `item_graveclaw_shoulder` | 1–2 | 0,75 | 3 | – | Erd-/Schutt-Schulterstücke |
| Grund | Knochen (Stufe = Rang) | `item_bone_t<Rang>` | 1–2 | 0,65 | 3 | – | Knochenreste |
| Rang | Schwere Grabklaue | `item_digging_claw_heavy` | 1–2 | 0,55 | 4 | Ja | Schwere Grabklauen |
| Rang | Gehärtete Schulterplatte | `item_graveclaw_shoulder_hard` | 1–2 | 0,45 | 4 | Ja | Gehärtete Schulterplatten (vorgezogen von R6) |
| Biom: Hügelgrab-Heide | Eisenerz | `item_iron_ore` | 1–2 | 0,45 | – | – | Grab-Erz/Schuttrest |

#### 16. Rimestalker (`creature_rimestalker`, MinRang 4, erreicht 4–6)

| Art | Gegenstand | ID | Menge | Chance | ab Rang | +Menge | Gemini-Name |
|---|---|---|---|---|---|---|---|
| Grund | Kaltgraue Haut | `item_rimestalker_hide` | 1–2 | 0,85 | 4 | – | Kalt-graue Hautstücke |
| Grund | Hornspitze | `item_rimestalker_spike` | 1–2 | 0,75 | 4 | – | Scharfe Glieder-/Hornspitzen |
| Grund | Sehne (Stufe = Rang) | `item_sinew_t<Rang>` | 1–2 | 0,70 | 4 | – | Sehnen |
| Rang | Dichte Kältehaut | `item_rimestalker_hide_dense` | 1–2 | 0,55 | 5 | Ja | Dichte Kälte-Häute |
| Rang | Frostdorn | `item_rimestalker_frost_spike` | 1 | 0,35 | 6 | Ja | Hochlagen-Teile |
| Biom: Basaltfjell | Frostsplitter | `item_frost_shard` | 1–2 | 0,55 | – | – | Frost-Splitter |

#### 17. Hollowback (`creature_hollowback`, MinRang 4, erreicht 4–6)

| Art | Gegenstand | ID | Menge | Chance | ab Rang | +Menge | Gemini-Name |
|---|---|---|---|---|---|---|---|
| Grund | Hohlrückenschale | `item_hollowback_shell` | 1–2 | 0,85 | 4 | – | Ausgehöhlte Rücken-/Rumpfstücke |
| Grund | Sporenstaub | `item_hollowback_spore_dust` | 1–2 | 0,75 | 4 | – | Staub-/Sporen-/Ruß-Reste |
| Grund | Sehne (Stufe = Rang) | `item_sinew_t<Rang>` | 1–2 | 0,65 | 4 | – | Sehnen |
| Rang | Gehärtete Rumpfschale | `item_hollowback_shell_hard` | 1–2 | 0,55 | 5 | Ja | Gehärtete Rumpfstrukturen |
| Rang | Sporensack | `item_hollowback_spore_sac` | 1–2 | 0,40 | 6 | Ja | Dichtere Wolken-/Materialreste |
| Biom: Hügelgrab-Heide, Glutküste | Ruinenstaub | `item_ruin_dust` | 1 | 0,45 | – | – | Umgebungs-Kapsel/Rest |

#### 18. Ironhide (`creature_ironhide`, MinRang 4, erreicht 4–6)

| Art | Gegenstand | ID | Menge | Chance | ab Rang | +Menge | Gemini-Name |
|---|---|---|---|---|---|---|---|
| Grund | Eisenkrusten-Platte | `item_ironhide_plate` | 1–2 | 0,85 | 4 | – | Eisen-/Krusten-Panzerplatten |
| Grund | Horn (Stufe = Rang) | `item_horn_t<Rang>` | 1–2 | 0,75 | 4 | – | Schwere Hornreste |
| Grund | Sehne (Stufe = Rang) | `item_sinew_t<Rang>` | 1–2 | 0,65 | 4 | – | Dicke Sehnen |
| Rang | Massive Panzerplatte | `item_ironhide_plate_massive` | 1–2 | 0,55 | 5 | Ja | Massive Panzerplatten |
| Rang | Erzkruste | `item_ironhide_ore_crust` | 1–2 | 0,35 | 6 | Ja | Metall-/Mineral-Reste |
| Biom: Basaltfjell, Glutküste | Eisenerz | `item_iron_ore` | 1–2 | 0,45 | – | – | Erz-/Eisen-Kruste |

#### 19. Shroudwalker (`creature_shroudwalker`, MinRang 4, erreicht 4)

| Art | Gegenstand | ID | Menge | Chance | ab Rang | +Menge | Gemini-Name |
|---|---|---|---|---|---|---|---|
| Grund | Schattengewebe | `item_shadow_weave` | 1–2 | 0,85 | 4 | – | Dunkle Faser-/Gewebebahnen |
| Grund | Schattenkern-Splitter | `item_shadow_core_shard` | 1 | 0,70 | 4 | – | Glimmende Kernreste |
| Grund | Sehne (Stufe = Rang) | `item_sinew_t<Rang>` | 1–2 | 0,70 | 4 | – | Lange Sehnen |
| Rang | Dichtes Schattengewebe | `item_shadow_weave_dense` | 1–2 | 0,55 | 4 | Ja | Dichtere Schatten-/Faserbahnen (vorgezogen von R5) |
| Rang | Schleier-Relikt | `item_shroud_relic` | 1 | 0,35 | 4 | Ja | Relikt-Teile (vorgezogen von R6) |
| Biom: Hügelgrab-Heide | Verfall-Relikt | `item_decay_relic` | 1–2 | 0,50 | – | – | Relikt-Faser |

#### 20. Cragmaw (`creature_cragmaw`, MinRang 5, erreicht 5–6)

| Art | Gegenstand | ID | Menge | Chance | ab Rang | +Menge | Gemini-Name |
|---|---|---|---|---|---|---|---|
| Grund | Zackenkiefer | `item_cragmaw_jaw` | 1–2 | 0,85 | 5 | – | Zackige Kiefer-/Horn-Teile |
| Grund | Steinkamm | `item_cragmaw_crest` | 1–2 | 0,80 | 5 | – | Steinkamm-Stücke |
| Grund | Knochen (Stufe = Rang) | `item_bone_t<Rang>` | 1–2 | 0,70 | 5 | – | Schwere Vorderlauf-Reste |
| Rang | Kiefer-Trophäe | `item_cragmaw_trophy` | 1–2 | 0,50 | 6 | Ja | Große Kiefer-/Horn-Trophäen |
| Rang | Kammkern | `item_cragmaw_crest_core` | 1 | 0,35 | 6 | Ja | Seltene Rohstoffe |
| Biom: Basaltfjell | Knochen (Stufe = Rang) | `item_bone_t<Rang>` | 1–2 | 0,50 | – | – | Fels-Kieferrest |

#### 21. Dreadcolossus (`creature_dreadcolossus`, MinRang 6, erreicht 6)

| Art | Gegenstand | ID | Menge | Chance | ab Rang | +Menge | Gemini-Name |
|---|---|---|---|---|---|---|---|
| Grund | Koloss-Gestein | `item_colossus_stone` | 2–3 | 0,95 | 6 | – | Massive Stein-/Mineral-/Relikt-Teile |
| Grund | Koloss-Knochen | `item_colossus_bone` | 2–3 | 0,85 | 6 | – | Koloss-Knochen/Sehnen |
| Grund | Koloss-Herzstein | `item_colossus_heartstone` | 1–2 | 0,70 | 6 | – | Spitzen-Materialien |
| Rang | Koloss-Relikt | `item_colossus_relic` | 1–2 | 0,45 | 6 | Ja | Seltener Koloss-Spitzenrest |
| Biom: Eisrand | Koloss-Kern | `item_colossus_core` | 1–2 | 0,55 | – | – | Seltener Koloss-Kern/Rest |

#### 22. Troll (`creature_troll`, MinRang 4, erreicht 4–5)

| Art | Gegenstand | ID | Menge | Chance | ab Rang | +Menge | Gemini-Name |
|---|---|---|---|---|---|---|---|
| Grund | Trollhaut | `item_troll_hide` | 1–2 | 0,90 | 4 | – | Dicke Trollhaut |
| Grund | Knochen (Stufe = Rang) | `item_bone_t<Rang>` | 1–2 | 0,80 | 4 | – | Grobe Knochen |
| Grund | Sehne (Stufe = Rang) | `item_sinew_t<Rang>` | 1–2 | 0,70 | 4 | – | Zähe Sehnen |
| Rang | Dicke Trollhaut | `item_troll_hide_thick` | 1–2 | 0,55 | 5 | Ja | Besonders dicke Häute |
| Rang | Knochen (Stufe = Rang) | `item_bone_t<Rang>` | 1–2 | 0,40 | 5 | Ja | Schwere Knochen (vorgezogen von R6) |

Entfällt (Kreatur kommt dort nicht vor): Moosgeäst: Moos-/Erd-Balg; Glut-Fluren / Bruchstein: Grobe Schlacke-/Steinreste.

#### 23. Riese (`creature_giant`, MinRang 5, erreicht 5–6)

| Art | Gegenstand | ID | Menge | Chance | ab Rang | +Menge | Gemini-Name |
|---|---|---|---|---|---|---|---|
| Grund | Riesenknochen | `item_giant_bone` | 2–3 | 0,90 | 5 | – | Große Knochen-/Sehnenstücke |
| Grund | Riesen-Rüstungsschrott | `item_giant_armor_scrap` | 1–2 | 0,75 | 5 | – | Schwere Rüstungs-/Reliktreste |
| Grund | Riesenzahn | `item_giant_tooth` | 1 | 0,60 | 5 | – | Seltene Rohstoffe |
| Rang | Kolossaler Riesenknochen | `item_giant_bone_massive` | 1–2 | 0,50 | 6 | Ja | Kolossale Knochen-/Sehnen-Teile |
| Rang | Riesen-Relikt | `item_giant_relic` | 1 | 0,35 | 6 | Ja | Spitzen-Relikte |
| Biom: Basaltfjell, Eisrand | Basaltbrocken | `item_basalt_chunk` | 1–2 | 0,50 | – | – | Uralter Fels-/Mineralrest |

Entfällt (Kreatur kommt dort nicht vor): Bruchstein: Altes Metall-/Reliktsück.

#### 24. Wiedergänger (`creature_revenant`, MinRang 3, erreicht 3–4)

| Art | Gegenstand | ID | Menge | Chance | ab Rang | +Menge | Gemini-Name |
|---|---|---|---|---|---|---|---|
| Grund | Altes Eisenstück | `item_old_iron` | 1–2 | 0,85 | 3 | – | Alte Ausrüstungs-/Metallreste |
| Grund | Knochen (Stufe = Rang) | `item_bone_t<Rang>` | 1–2 | 0,75 | 3 | – | Knochen-/Sehnenmaterial |
| Grund | Verfall-Relikt | `item_decay_relic` | 1–2 | 0,65 | 3 | – | Verfall-Reste |
| Rang | Waffenbruchstück | `item_revenant_weapon_fragment` | 1–2 | 0,55 | 4 | Ja | Bessere Metall-/Waffen-/Rüstungsfragmente |
| Rang | Grabbeigabe | `item_revenant_grave_goods` | 1 | 0,35 | 4 | Ja | Seltenere Relikte (vorgezogen von R6) |
| Biom: Hügelgrab-Heide | Altes Eisenstück | `item_old_iron` | 1–2 | 0,50 | – | – | Altes Eisenstück |

Entfällt (Kreatur kommt dort nicht vor): Glut-Fluren: Asche-Relikt; Moosgeäst: Modriges Gewebe-/Relikt.

#### 25. Cinderwyrm (`creature_cinderwyrm`, MinRang 5, erreicht 5–6)

| Art | Gegenstand | ID | Menge | Chance | ab Rang | +Menge | Gemini-Name |
|---|---|---|---|---|---|---|---|
| Grund | Glutschuppe | `item_ember_scale` | 1–3 | 0,90 | 5 | – | Glut-Schuppen |
| Grund | Dichte Schlackeplatte | `item_slag_plate_dense` | 1–2 | 0,80 | 5 | – | Schlacke-Platten |
| Grund | Drachensehne (Stufe = Rang) | `item_dragon_sinew_t<Rang>` | 1–2 | 0,75 | 5 | – | Zähe Drachensehnen |
| Grund | Glutdrachenhorn | `item_cinderwyrm_horn` | 1–2 | 0,65 | 5 | – | Hitzebeständige Knochen/Hörner |
| Rang | Edle Glutschuppe | `item_ember_scale_fine` | 1–2 | 0,50 | 6 | Ja | Hochwertige Glut-Schuppen |
| Rang | Glutdrachenherz | `item_cinderwyrm_heart` | 1 | 0,35 | 6 | Ja | Spitzen-Komponenten |
| Biom: Glutküste | Glutschuppe | `item_ember_scale` | 1–2 | 0,55 | – | – | Glut-Schuppe |

Entfällt (Kreatur kommt dort nicht vor): Bruchstein / Schroffen-Grate: Hitze-Horn.

#### 26. Cragdrake (`creature_cragdrake`, MinRang 5, erreicht 5–6)

| Art | Gegenstand | ID | Menge | Chance | ab Rang | +Menge | Gemini-Name |
|---|---|---|---|---|---|---|---|
| Grund | Felsdrachenschuppe | `item_cragdrake_scale` | 1–3 | 0,90 | 5 | – | Granit-/Fels-Schuppen |
| Grund | Klammerkralle | `item_cragdrake_claw` | 1–2 | 0,80 | 5 | – | Klammerkrallen |
| Grund | Drachensehne (Stufe = Rang) | `item_dragon_sinew_t<Rang>` | 1–2 | 0,75 | 5 | – | Dicker Drachenknorpel/Sehnen |
| Rang | Schwere Bergschuppe | `item_cragdrake_scale_heavy` | 1–2 | 0,50 | 6 | Ja | Schwere Berg-Schuppen |
| Rang | Felsdrachenherz | `item_cragdrake_heart` | 1 | 0,35 | 6 | Ja | Hochgebirgs-/Drachen-Werkstoffe |
| Biom: Basaltfjell, Eisrand | Granitschuppe | `item_granite_scale` | 1–2 | 0,55 | – | – | Granit-Schuppe |
| Biom: Basaltfjell | Klammerkralle | `item_cragdrake_claw` | 1 | 0,45 | – | – | Fels-Klaue |

#### 27. Stormmaw (`creature_stormmaw`, MinRang 6, erreicht 6)

| Art | Gegenstand | ID | Menge | Chance | ab Rang | +Menge | Gemini-Name |
|---|---|---|---|---|---|---|---|
| Grund | Sturm-Schwingenhaut | `item_storm_wing_hide` | 2–3 | 0,95 | 6 | – | Sturm-Schwingenhaut |
| Grund | Drachensehne (Stufe = Rang) | `item_dragon_sinew_t<Rang>` | 2–3 | 0,85 | 6 | – | Zähe Drachensehnen |
| Grund | Sturmdrachenhorn | `item_stormmaw_horn` | 1–2 | 0,75 | 6 | – | Gebogene Hörner/Zähne |
| Rang | Sturmdrachenherz | `item_stormmaw_heart` | 1–2 | 0,45 | 6 | Ja | Seltene Sturm-/Relikt-Materialien |
| Biom: Eisrand | Sturm-Schwingenhaut | `item_storm_wing_hide` | 1–2 | 0,60 | – | – | Sturm-Schwingenhaut |
| Biom: Eisrand | Sturmdrachenhorn | `item_stormmaw_horn` | 1 | 0,45 | – | – | Sturm-Horn |

#### 28. Ashwing (`creature_ashwing`, MinRang 5, erreicht 5–6)

| Art | Gegenstand | ID | Menge | Chance | ab Rang | +Menge | Gemini-Name |
|---|---|---|---|---|---|---|---|
| Grund | Asche-Schwingenhaut | `item_ashwing_wing_hide` | 1–3 | 0,90 | 5 | – | Asche-Schwingenhaut |
| Grund | Drachensehne (Stufe = Rang) | `item_dragon_sinew_t<Rang>` | 1–2 | 0,75 | 5 | – | Zähe Drachensehnen |
| Grund | Horn (Stufe = Rang) | `item_horn_t<Rang>` | 1–2 | 0,70 | 5 | – | Staubige Horn-/Knochenreste |
| Rang | Gleit-Schwingenhaut | `item_ashwing_wing_hide_fine` | 1–2 | 0,50 | 6 | Ja | Hochwertige Gleit-Schwingenhäute |
| Rang | Aschedrachenherz | `item_ashwing_heart` | 1 | 0,35 | 6 | Ja | Asche-/Drachen-Teile |
| Biom: Glutküste | Asche-Schwingenhaut | `item_ashwing_wing_hide` | 1–2 | 0,55 | – | – | Asche-Haut |
| Biom: Glutküste | Horn (Stufe = Rang) | `item_horn_t<Rang>` | 1 | 0,45 | – | – | Staub-Klaue |

#### 29. Frostdrake (`creature_frostdrake`, MinRang 5, erreicht 5–6)

| Art | Gegenstand | ID | Menge | Chance | ab Rang | +Menge | Gemini-Name |
|---|---|---|---|---|---|---|---|
| Grund | Frostdrachenschuppe | `item_frostdrake_scale` | 1–3 | 0,90 | 5 | – | Frost-/Schiefer-Schuppen |
| Grund | Drachensehne (Stufe = Rang) | `item_dragon_sinew_t<Rang>` | 1–2 | 0,75 | 5 | – | Drachensehnen |
| Grund | Horn (Stufe = Rang) | `item_horn_t<Rang>` | 1–2 | 0,70 | 5 | – | Kalte Horn-/Knochenreste |
| Rang | Gehärtete Frostschuppe | `item_frostdrake_scale_hard` | 1–2 | 0,50 | 6 | Ja | Gehärtete Frost-Schuppen |
| Rang | Frostdrachenherz | `item_frostdrake_heart` | 1 | 0,35 | 6 | Ja | Hochlagen-Komponenten |
| Biom: Eisrand | Frostdrachenschuppe | `item_frostdrake_scale` | 1–2 | 0,55 | – | – | Frost-Schuppe |

Entfällt (Kreatur kommt dort nicht vor): Schroffen-Grate / Bruchstein: Kalt-Horn.

#### 30. Dreadwyrm (`creature_dreadwyrm`, MinRang 6, erreicht 6)

| Art | Gegenstand | ID | Menge | Chance | ab Rang | +Menge | Gemini-Name |
|---|---|---|---|---|---|---|---|
| Grund | Wyrmschuppe | `item_dreadwyrm_scale` | 2–3 | 0,95 | 6 | – | Spitzen-Drachenschuppen |
| Grund | Drachensehne (Stufe = Rang) | `item_dragon_sinew_t<Rang>` | 2–3 | 0,85 | 6 | – | Massive Sehnen |
| Grund | Uraltes Wyrmhorn | `item_dreadwyrm_horn` | 1–2 | 0,80 | 6 | – | Uralte Knochen-/Horn- & Relikt-Teile (geteilt) |
| Grund | Wyrm-Relikt | `item_wyrm_relic` | 1 | 0,80 | 6 | – | Uralte Knochen-/Horn- & Relikt-Teile (geteilt) |
| Rang | Wyrmherz | `item_dreadwyrm_heart` | 1–2 | 0,45 | 6 | Ja | Seltene Wyrm-Spitzenrelikte |
| Biom: Eisrand | Wyrm-Relikt | `item_wyrm_relic` | 1–2 | 0,60 | – | – | Uralte Drachen-Reliktreste |

#### 31. Bramblefang (`creature_bramblefang`, MinRang 2, erreicht 2–3)

| Art | Gegenstand | ID | Menge | Chance | ab Rang | +Menge | Gemini-Name |
|---|---|---|---|---|---|---|---|
| Grund | Dornenbalg | `item_bramblefang_pelt` | 1–2 | 0,90 | 2 | – | Dornen-Balg |
| Grund | Fangzahn | `item_bramblefang_fang` | 1–2 | 0,75 | 2 | – | Scharfe Fangzähne |
| Grund | Sehne (Stufe = Rang) | `item_sinew_t<Rang>` | 1–2 | 0,70 | 2 | – | Dünne Sehnen |
| Rang | Gehärteter Dornenbalg | `item_bramblefang_pelt_hard` | 1–2 | 0,65 | 3 | Ja | Gehärteter Dornen-Balg |
| Rang | Verstärkter Fangzahn | `item_bramblefang_fang_strong` | 1–2 | 0,45 | 3 | Ja | Verstärkte Fangzähne (vorgezogen von R5) |
| Biom: Gezeitenmarsch | Moosfaser | `item_moss_fiber` | 1–2 | 0,55 | – | – | Moosfaser |
| Biom: Gezeitenmarsch | Dornholz | `item_thorn_wood` | 1 | 0,45 | – | – | Dornen-Holz |
| Biom: Hügelgrab-Heide | Verfall-Relikt | `item_decay_relic` | 1 | 0,35 | – | – | Verfall-Relikt |
| Biom: Hügelgrab-Heide | Ruinenstaub | `item_ruin_dust` | 1–2 | 0,40 | – | – | Ruinen-Staubrest |

#### 32. Siltstrider (`creature_siltstrider`, MinRang 2, erreicht 2–3)

| Art | Gegenstand | ID | Menge | Chance | ab Rang | +Menge | Gemini-Name |
|---|---|---|---|---|---|---|---|
| Grund | Laufhaut | `item_siltstrider_hide` | 1–2 | 0,85 | 2 | – | Trockene Laufhaut |
| Grund | Lange Laufsehne | `item_siltstrider_sinew` | 1–2 | 0,75 | 2 | – | Lange Laufsehnen |
| Grund | Knochen (Stufe = Rang) | `item_bone_t<Rang>` | 1 | 0,65 | 2 | – | Leichte Knochenreste |
| Rang | Zähe Laufhaut | `item_siltstrider_hide_tough` | 1–2 | 0,65 | 3 | Ja | Zähere Laufhaut & längere Sehnen (geteilt) |
| Rang | Lange Laufsehne | `item_siltstrider_sinew` | 1 | 0,65 | 3 | Ja | Zähere Laufhaut & längere Sehnen (geteilt) |
| Rang | Knochen (Stufe = Rang) | `item_bone_t<Rang>` | 1–2 | 0,45 | 3 | Ja | Gehärtete Laufknochen & dichtere Fasern (geteilt) (vorgezogen von R5) |
| Rang | Lange Laufsehne | `item_siltstrider_sinew` | 1 | 0,45 | 3 | Ja | Gehärtete Laufknochen & dichtere Fasern (geteilt) (vorgezogen von R5) |
| Biom: Gezeitenmarsch | Feuchtleder | `item_damp_leather` | 1 | 0,50 | – | – | Feucht-Leder |
| Biom: Gezeitenmarsch | Pflanzenfasern | `item_fiber` | 1–2 | 0,45 | – | – | Unterholz-Faser |
| Biom: Glutküste | Asche-Mineral | `item_ash_mineral` | 1 | 0,45 | – | – | Asche-Mineral |
| Biom: Glutküste | Haut (Stufe = Rang) | `item_hide_t<Rang>` | 1–2 | 0,40 | – | – | Ruß-Balgrest |

#### 33. Cinderling (`creature_cinderling`, MinRang 2, erreicht 2–3)

| Art | Gegenstand | ID | Menge | Chance | ab Rang | +Menge | Gemini-Name |
|---|---|---|---|---|---|---|---|
| Grund | Funkenasche | `item_spark_ash` | 1–3 | 0,90 | 2 | – | Funken-Asche |
| Grund | Verkohlte Faser | `item_charred_fiber` | 1–2 | 0,70 | 2 | – | Verkohlte Faser |
| Grund | Glimmsplitter | `item_cinderling_ember_shard` | 1 | 0,45 | 2 | – | Glimmender Splitter |
| Rang | Funkenasche | `item_spark_ash` | 1–2 | 0,65 | 3 | Ja | Dichtere Funken-Asche & zähere Fasern (geteilt) |
| Rang | Verkohlte Faser | `item_charred_fiber` | 1 | 0,65 | 3 | Ja | Dichtere Funken-Asche & zähere Fasern (geteilt) |
| Rang | Glutkapsel | `item_ember_capsule` | 1 | 0,40 | 3 | Ja | Glimmende Glutkapsel (vorgezogen von R5) |
| Biom: Glutküste | Schlackekern | `item_slag_core` | 1 | 0,40 | – | – | Schlacke-Kern |
| Biom: Glutküste | Glutschuppe | `item_ember_scale` | 1 | 0,30 | – | – | Glut-Schuppe |

Entfällt (Kreatur kommt dort nicht vor): Bruchstein: Altes Eisenstück; Bruchstein: Ruinen-Rußrest.

#### 34. Sporeback (`creature_sporeback`, MinRang 3, erreicht 3–4)

| Art | Gegenstand | ID | Menge | Chance | ab Rang | +Menge | Gemini-Name |
|---|---|---|---|---|---|---|---|
| Grund | Sporenkappe | `item_sporeback_cap` | 1–2 | 0,85 | 3 | – | Sporenkappen-Rest |
| Grund | Buckelhaut | `item_sporeback_hide` | 1–2 | 0,75 | 3 | – | Buckelige Rumpfhaut |
| Grund | Sehne (Stufe = Rang) | `item_sinew_t<Rang>` | 1 | 0,65 | 3 | – | Zähe Sehnen |
| Rang | Sporenkapsel | `item_sporeback_spore_capsule` | 1–2 | 0,50 | 4 | Ja | Konzentrierte Sporenkapsel & gehärtete Rumpfhaut (geteilt) |
| Rang | Gehärtete Buckelhaut | `item_sporeback_hide_hard` | 1 | 0,50 | 4 | Ja | Konzentrierte Sporenkapsel & gehärtete Rumpfhaut (geteilt) |
| Rang | Sporen-Essenz | `item_spore_essence` | 1 | 0,30 | 4 | Ja | Seltene Sporen-Essenz (vorgezogen von R6) |
| Biom: Hügelgrab-Heide | Verfall-Relikt | `item_decay_relic` | 1 | 0,40 | – | – | Verfall-Relikt |
| Biom: Hügelgrab-Heide | Ruinenstaub | `item_ruin_dust` | 1 | 0,35 | – | – | Kalk-/Mauerstaub-Kapsel |

Entfällt (Kreatur kommt dort nicht vor): Moosgeäst: Moosfaser; Moosgeäst: Feucht-Leder.

#### 35. Skitterclaw (`creature_skitterclaw`, MinRang 3, erreicht 3–4)

| Art | Gegenstand | ID | Menge | Chance | ab Rang | +Menge | Gemini-Name |
|---|---|---|---|---|---|---|---|
| Grund | Klammer-Chitin | `item_skitterclaw_chitin` | 1–2 | 0,85 | 3 | – | Klammer-Chitin |
| Grund | Krallenspitze | `item_skitterclaw_claw` | 1–2 | 0,75 | 3 | – | Scharfe Krallenspitzen |
| Grund | Sehne (Stufe = Rang) | `item_sinew_t<Rang>` | 1–2 | 0,65 | 3 | – | Dünne Gliedsehnen |
| Rang | Gehärtetes Klammer-Chitin | `item_skitterclaw_chitin_hard` | 1–2 | 0,60 | 4 | Ja | Gehärtetes Klammer-Chitin & dickere Krallen (geteilt) |
| Rang | Krallenspitze | `item_skitterclaw_claw` | 1 | 0,60 | 4 | Ja | Gehärtetes Klammer-Chitin & dickere Krallen (geteilt) |
| Rang | Seltene Chitinplatte | `item_skitterclaw_chitin_plate` | 1 | 0,35 | 4 | Ja | Seltene Chitin-Platte (vorgezogen von R6) |
| Biom: Hügelgrab-Heide | Ruinen-Chitin | `item_ruin_chitin` | 1–2 | 0,50 | – | – | Ruinen-Chitin |
| Biom: Hügelgrab-Heide | Altes Eisenstück | `item_old_iron` | 1 | 0,35 | – | – | Altes Eisenstück |
| Biom: Basaltfjell | Basaltbrocken | `item_basalt_chunk` | 1 | 0,45 | – | – | Hochgebirgs-Stein |
| Biom: Basaltfjell | Granitschuppe | `item_granite_scale` | 1 | 0,30 | – | – | Granit-Schuppe |

#### 36. Gloomstalker (`creature_gloomstalker`, MinRang 3, erreicht 3–4)

| Art | Gegenstand | ID | Menge | Chance | ab Rang | +Menge | Gemini-Name |
|---|---|---|---|---|---|---|---|
| Grund | Schattengewebe | `item_shadow_weave` | 1–2 | 0,85 | 3 | – | Schatten-Gewebefetzen |
| Grund | Sehne (Stufe = Rang) | `item_sinew_t<Rang>` | 1–2 | 0,70 | 3 | – | Gedämpfte Sehnen |
| Grund | Knochen (Stufe = Rang) | `item_bone_t<Rang>` | 1 | 0,60 | 3 | – | Dunkle Knochensplitter |
| Rang | Dichtes Schattengewebe | `item_shadow_weave_dense` | 1–2 | 0,60 | 4 | Ja | Dichtere Schatten-Bahnen & stärkere Sehnen (geteilt) |
| Rang | Sehne (Stufe = Rang) | `item_sinew_t<Rang>` | 1 | 0,60 | 4 | Ja | Dichtere Schatten-Bahnen & stärkere Sehnen (geteilt) |
| Rang | Schattenkern | `item_shadow_core` | 1 | 0,30 | 4 | Ja | Glimmender Schattenkern (vorgezogen von R6) |
| Biom: Hügelgrab-Heide | Verfall-Relikt | `item_decay_relic` | 1 | 0,45 | – | – | Verfall-Relikt |
| Biom: Hügelgrab-Heide | Ruinen-Chitin | `item_ruin_chitin` | 1 | 0,35 | – | – | Ruinen-Chitin |

Entfällt (Kreatur kommt dort nicht vor): Moosgeäst: Moosfaser; Moosgeäst: Dornen-Holz.

#### 37. Charmaw (`creature_charmaw`, MinRang 3, erreicht 3–6)

| Art | Gegenstand | ID | Menge | Chance | ab Rang | +Menge | Gemini-Name |
|---|---|---|---|---|---|---|---|
| Grund | Verkohlter Kiefer | `item_charmaw_jaw` | 1 | 0,80 | 3 | – | Verkohlter Kieferknochen |
| Grund | Brandsehne | `item_charmaw_burn_sinew` | 1–2 | 0,75 | 3 | – | Brand-Sehnen |
| Grund | Rußzahn | `item_charmaw_tooth` | 1–2 | 0,70 | 3 | – | Rußige Kieferzähne |
| Rang | Gehärteter Kiefer | `item_charmaw_jaw_hard` | 1–2 | 0,55 | 4 | Ja | Gehärteter Kieferknochen & glimmende Zahnreste (geteilt) |
| Rang | Rußzahn | `item_charmaw_tooth` | 1 | 0,55 | 4 | Ja | Gehärteter Kieferknochen & glimmende Zahnreste (geteilt) |
| Rang | Glutkiefer | `item_charmaw_ember_jaw` | 1 | 0,35 | 6 | Ja | Schwerer Glut-Kiefer |
| Biom: Glutküste | Glutschuppe | `item_ember_scale` | 1 | 0,45 | – | – | Glut-Schuppe |
| Biom: Glutküste | Asche-Mineral | `item_ash_mineral` | 1–2 | 0,50 | – | – | Asche-Mineral |
| Biom: Hügelgrab-Heide | Altes Eisenstück | `item_old_iron` | 1 | 0,40 | – | – | Altes Eisenstück |
| Biom: Hügelgrab-Heide | Verfall-Relikt | `item_decay_relic` | 1 | 0,35 | – | – | Verfall-Relikt |

#### 38. Rimehound (`creature_rimehound`, MinRang 4, erreicht 4–5)

| Art | Gegenstand | ID | Menge | Chance | ab Rang | +Menge | Gemini-Name |
|---|---|---|---|---|---|---|---|
| Grund | Kaltborstenfell | `item_rimehound_fur` | 1–2 | 0,85 | 4 | – | Kalt-borstiges Fell |
| Grund | Frostsehne | `item_frost_sinew` | 1–2 | 0,75 | 4 | – | Frost-Sehnen |
| Grund | Raufangzahn | `item_rimehound_fang` | 1–2 | 0,65 | 4 | – | Raue Fangzähne |
| Rang | Dichtes Hochlagenfell | `item_rimehound_fur_dense` | 1–2 | 0,55 | 5 | Ja | Dichtes Hochlagen-Fell & kristalline Sehnen (geteilt) |
| Rang | Frostsehne | `item_frost_sinew` | 1 | 0,55 | 5 | Ja | Dichtes Hochlagen-Fell & kristalline Sehnen (geteilt) |
| Rang | Frostkern | `item_frost_core` | 1 | 0,35 | 5 | Ja | Frost-Kernstück (vorgezogen von R6) |
| Biom: Basaltfjell | Frostsplitter | `item_frost_shard` | 1–2 | 0,55 | – | – | Frost-Splitter |
| Biom: Basaltfjell | Granitschuppe | `item_granite_scale` | 1 | 0,40 | – | – | Granit-Schuppe |
| Biom: Eisrand | Sturmfaser | `item_storm_fiber` | 1 | 0,45 | – | – | Sturm-Faser |
| Biom: Eisrand | Frostsplitter | `item_frost_shard` | 1 | 0,40 | – | – | Frost-Splitter |

#### 39. Slagbrute (`creature_slagbrute`, MinRang 4, erreicht 4–6)

| Art | Gegenstand | ID | Menge | Chance | ab Rang | +Menge | Gemini-Name |
|---|---|---|---|---|---|---|---|
| Grund | Schlackeplatte | `item_slag_plate` | 1–2 | 0,85 | 4 | – | Schlacke-Frontkruste |
| Grund | Basaltbrocken | `item_basalt_chunk` | 1–3 | 0,80 | 4 | – | Grobe Gesteinsbrocken |
| Grund | Sehne (Stufe = Rang) | `item_sinew_t<Rang>` | 1–2 | 0,65 | 4 | – | Schwere Sehnen |
| Rang | Dichte Schlackeplatte | `item_slag_plate_dense` | 1–2 | 0,50 | 5 | Ja | Dichte Schlacke-Platte & gehärtete Gesteinsbrocken (geteilt) |
| Rang | Basaltbrocken | `item_basalt_chunk` | 1 | 0,50 | 5 | Ja | Dichte Schlacke-Platte & gehärtete Gesteinsbrocken (geteilt) |
| Rang | Hitzekern | `item_heat_core` | 1 | 0,35 | 6 | Ja | Glühender Schlacke-Kern |
| Biom: Glutküste | Schlackekern | `item_slag_core` | 1 | 0,50 | – | – | Schlacke-Kern |
| Biom: Glutküste | Asche-Mineral | `item_ash_mineral` | 1–2 | 0,50 | – | – | Asche-Mineral |
| Biom: Basaltfjell | Basaltbrocken | `item_basalt_chunk` | 1–2 | 0,50 | – | – | Hochgebirgs-Stein |
| Biom: Basaltfjell | Granitschuppe | `item_granite_scale` | 1 | 0,35 | – | – | Granit-Schuppe |

#### 40. Vaultcrawler (`creature_vaultcrawler`, MinRang 4, erreicht 4)

| Art | Gegenstand | ID | Menge | Chance | ab Rang | +Menge | Gemini-Name |
|---|---|---|---|---|---|---|---|
| Grund | Gewölbeplatte | `item_vaultcrawler_plate` | 1–2 | 0,85 | 4 | – | Gewölbe-Panzerplatten |
| Grund | Greifarm-Gelenk | `item_vaultcrawler_joint` | 1–2 | 0,75 | 4 | – | Greifarm-Gelenke |
| Grund | Sehne (Stufe = Rang) | `item_sinew_t<Rang>` | 1–2 | 0,65 | 4 | – | Zähe Sehnen |
| Rang | Verstärkte Gewölbeplatte | `item_vaultcrawler_plate_strong` | 1–2 | 0,55 | 4 | Ja | Verstärkte Gewölbe-Platten & gehärtete Greifgelenke (geteilt) (vorgezogen von R5) |
| Rang | Greifarm-Gelenk | `item_vaultcrawler_joint` | 1 | 0,55 | 4 | Ja | Verstärkte Gewölbe-Platten & gehärtete Greifgelenke (geteilt) (vorgezogen von R5) |
| Rang | Gewölbe-Reliktplatte | `item_vaultcrawler_relic_plate` | 1 | 0,35 | 4 | Ja | Seltene Gewölbe-Reliktplatte (vorgezogen von R6) |
| Biom: Hügelgrab-Heide | Ruinen-Chitin | `item_ruin_chitin` | 1–2 | 0,55 | – | – | Ruinen-Chitin |
| Biom: Hügelgrab-Heide | Altes Eisenstück | `item_old_iron` | 1 | 0,45 | – | – | Altes Eisenstück |
| Biom: Hügelgrab-Heide | Verfall-Relikt | `item_decay_relic` | 1 | 0,40 | – | – | Verfall-Relikt |

#### 41. Mossgolem (`creature_mossgolem`, MinRang 4, erreicht 4–5)

| Art | Gegenstand | ID | Menge | Chance | ab Rang | +Menge | Gemini-Name |
|---|---|---|---|---|---|---|---|
| Grund | Wurzelstein | `item_mossgolem_rootstone` | 1–3 | 0,90 | 4 | – | Verwachsene Wurzel-/Steinbrocken |
| Grund | Moosrinde | `item_mossgolem_bark` | 1–2 | 0,80 | 4 | – | Moosdurchsetzte Rinde |
| Grund | Wurzelbast | `item_mossgolem_bast` | 1–2 | 0,65 | 4 | – | Zähes Bindematerial |
| Rang | Uralter Wurzelkern | `item_mossgolem_root_core` | 1–2 | 0,45 | 5 | Ja | Uralte Wurzel-/Steinkerne & dichtere Rindenplatten (geteilt) |
| Rang | Moosrinde | `item_mossgolem_bark` | 1 | 0,45 | 5 | Ja | Uralte Wurzel-/Steinkerne & dichtere Rindenplatten (geteilt) |
| Rang | Wächterkern | `item_mossgolem_warden_core` | 1 | 0,30 | 5 | Ja | Seltener Wächter-Kern (vorgezogen von R6) |
| Biom: Hügelgrab-Heide | Verfall-Relikt | `item_decay_relic` | 1 | 0,40 | – | – | Verfall-Relikt |
| Biom: Hügelgrab-Heide | Ruinen-Chitin | `item_ruin_chitin` | 1 | 0,35 | – | – | Ruinen-Chitin |

Entfällt (Kreatur kommt dort nicht vor): Moosgeäst: Moosfaser; Moosgeäst: Dornen-Holz.

#### 42. Galehound (`creature_galehound`, MinRang 5, erreicht 5–6)

| Art | Gegenstand | ID | Menge | Chance | ab Rang | +Menge | Gemini-Name |
|---|---|---|---|---|---|---|---|
| Grund | Sturmborstenfell | `item_galehound_fur` | 1–2 | 0,85 | 5 | – | Sturm-Borstenfell |
| Grund | Böensehne | `item_galehound_sinew` | 1–2 | 0,75 | 5 | – | Gespannte Böen-Sehnen |
| Grund | Geladener Kernrest | `item_charged_core` | 1 | 0,55 | 5 | – | Flackernde Funken-/Spannungsreste |
| Rang | Geladener Kernrest | `item_charged_core` | 1–2 | 0,45 | 6 | Ja | Geladener Kernrest & hochgespannte Sehnen (geteilt) |
| Rang | Böensehne | `item_galehound_sinew` | 1 | 0,45 | 6 | Ja | Geladener Kernrest & hochgespannte Sehnen (geteilt) |
| Biom: Eisrand | Sturmfaser | `item_storm_fiber` | 1–2 | 0,55 | – | – | Sturm-Faser |
| Biom: Eisrand | Geladener Kernrest | `item_charged_core` | 1 | 0,40 | – | – | Geladener Kernrest |

Entfällt (Kreatur kommt dort nicht vor): Schroffen-Grate: Frost-Splitter; Schroffen-Grate: Hochgebirgs-Stein.

#### 43. Cragstalker (`creature_cragstalker`, MinRang 5, erreicht 5–6)

| Art | Gegenstand | ID | Menge | Chance | ab Rang | +Menge | Gemini-Name |
|---|---|---|---|---|---|---|---|
| Grund | Gratschuppenhaut | `item_cragstalker_hide` | 1–2 | 0,85 | 5 | – | Grat-Schuppenhaut |
| Grund | Sprungsehne | `item_cragstalker_sinew` | 1–2 | 0,75 | 5 | – | Kräftige Sprungsehnen |
| Grund | Kammhorn | `item_cragstalker_horn` | 1–2 | 0,65 | 5 | – | Gebogene Kammhörner |
| Rang | Gehärtete Gratschuppe | `item_cragstalker_hide_hard` | 1–2 | 0,35 | 6 | Ja | Gehärtete Grat-Schuppen & seltene Hochgebirgs-Trophäe (geteilt) |
| Rang | Grat-Trophäe | `item_cragstalker_trophy` | 1 | 0,35 | 6 | Ja | Gehärtete Grat-Schuppen & seltene Hochgebirgs-Trophäe (geteilt) |
| Biom: Basaltfjell | Granitschuppe | `item_granite_scale` | 1–2 | 0,55 | – | – | Granit-Schuppe |
| Biom: Basaltfjell | Basaltbrocken | `item_basalt_chunk` | 1–2 | 0,50 | – | – | Hochgebirgs-Stein |
| Biom: Eisrand | Sturmfaser | `item_storm_fiber` | 1 | 0,45 | – | – | Sturm-Faser |
| Biom: Eisrand | Granitschuppe | `item_granite_scale` | 1 | 0,40 | – | – | Granit-Schuppe |

#### 44. Ashweaver (`creature_ashweaver`, MinRang 5, erreicht 5–6)

| Art | Gegenstand | ID | Menge | Chance | ab Rang | +Menge | Gemini-Name |
|---|---|---|---|---|---|---|---|
| Grund | Hitzefaser | `item_heat_fiber` | 2–3 | 0,90 | 5 | – | Zähe Asche-Faserstränge |
| Grund | Glutknoten | `item_ember_knot` | 1–2 | 0,70 | 5 | – | Glimmende Glutknoten |
| Grund | Verkohltes Chitin | `item_ashweaver_chitin` | 1–2 | 0,65 | 5 | – | Verkohlte Chitin-/Faserreste |
| Rang | Hochfester Glut-Faserstrang | `item_heat_fiber_fine` | 1–2 | 0,50 | 6 | Ja | Hochfester Glut-Faserstrang |
| Rang | Stabile Glutkapsel | `item_ember_capsule_stable` | 1 | 0,35 | 6 | Ja | Seltene Glutkapsel |
| Biom: Glutküste | Glutschuppe | `item_ember_scale` | 1 | 0,45 | – | – | Glut-Schuppe |
| Biom: Glutküste | Schlackekern | `item_slag_core` | 1 | 0,45 | – | – | Schlacke-Kern |
| Biom: Glutküste | Asche-Mineral | `item_ash_mineral` | 1–2 | 0,50 | – | – | Asche-Mineral |

Entfällt (Kreatur kommt dort nicht vor): Bruchstein: Verfall-Relikt; Bruchstein: Ruinen-Chitin.

#### 45. Frostmauler (`creature_frostmauler`, MinRang 5, erreicht 5–6)

| Art | Gegenstand | ID | Menge | Chance | ab Rang | +Menge | Gemini-Name |
|---|---|---|---|---|---|---|---|
| Grund | Frostschuppenbrocken | `item_frostmauler_scale` | 1–2 | 0,85 | 5 | – | Dicke Frost-Schuppenbrocken |
| Grund | Prankenknochen | `item_frostmauler_paw_bone` | 1–2 | 0,75 | 5 | – | Massive Prankenknochen |
| Grund | Sehne (Stufe = Rang) | `item_sinew_t<Rang>` | 1–2 | 0,65 | 5 | – | Zähe Sehnen |
| Rang | Gehärteter Frostpanzer | `item_frostmauler_armor` | 1–2 | 0,45 | 6 | Ja | Gehärteter Frost-Panzer |
| Rang | Hochlagen-Knochen | `item_frostmauler_rare_bone` | 1 | 0,35 | 6 | Ja | Seltener Hochlagen-Knochen |
| Biom: Basaltfjell | Frostsplitter | `item_frost_shard` | 1–2 | 0,60 | – | – | Frost-Splitter |
| Biom: Basaltfjell | Granitschuppe | `item_granite_scale` | 1–2 | 0,50 | – | – | Granit-Schuppe |
| Biom: Eisrand | Frostsplitter | `item_frost_shard` | 1 | 0,45 | – | – | Frost-Splitter |
| Biom: Eisrand | Sturmfaser | `item_storm_fiber` | 1 | 0,40 | – | – | Sturm-Faser |

#### 46. Stormwyrmling (`creature_stormwyrmling`, MinRang 5, erreicht 5–6)

| Art | Gegenstand | ID | Menge | Chance | ab Rang | +Menge | Gemini-Name |
|---|---|---|---|---|---|---|---|
| Grund | Junge Sturm-Schwingenhaut | `item_stormwyrmling_wing_hide` | 1–2 | 0,85 | 5 | – | Junge Sturm-Schwingenhaut |
| Grund | Drachensehne (Stufe = Rang) | `item_dragon_sinew_t<Rang>` | 1–2 | 0,80 | 5 | – | Elastische Drachensehnen |
| Grund | Schmales Drachenhorn | `item_stormwyrmling_horn` | 1–2 | 0,65 | 5 | – | Schmale Drachenhörner |
| Rang | Sturm-Schwingenhaut | `item_storm_wing_hide` | 1–2 | 0,50 | 6 | Ja | Verdichtete Sturm-Schwingenhaut |
| Rang | Geladener Drachenkern | `item_charged_dragon_core` | 1 | 0,35 | 6 | Ja | Geladener Drachenkernrest |
| Biom: Eisrand | Sturm-Schwingenhaut | `item_storm_wing_hide` | 1 | 0,50 | – | – | Sturm-Schwingenhaut |
| Biom: Eisrand | Sturmfaser | `item_storm_fiber` | 1–2 | 0,50 | – | – | Sturm-Faser |
| Biom: Eisrand | Geladener Kernrest | `item_charged_core` | 1 | 0,35 | – | – | Geladener Kernrest |
| Biom: Basaltfjell | Granitschuppe | `item_granite_scale` | 1 | 0,40 | – | – | Granit-Schuppe |
| Biom: Basaltfjell | Frostsplitter | `item_frost_shard` | 1 | 0,40 | – | – | Frost-Splitter |

#### 47. Cindercolossus (`creature_cindercolossus`, MinRang 6, erreicht 6)

| Art | Gegenstand | ID | Menge | Chance | ab Rang | +Menge | Gemini-Name |
|---|---|---|---|---|---|---|---|
| Grund | Schlacke-Kolossplatte | `item_colossus_slag_plate` | 2–3 | 0,95 | 6 | – | Massive Schlacke-Kolossplatte |
| Grund | Glühender Kolosskern | `item_colossus_core_glowing` | 1–2 | 0,75 | 6 | – | Glühender Koloss-Kern |
| Grund | Koloss-Gestein | `item_colossus_stone` | 2–3 | 0,80 | 6 | – | Schwere Gesteins-/Knochenmasse |
| Rang | Schlacke-Relikt | `item_colossus_slag_relic` | 1–2 | 0,45 | 6 | Ja | Seltener Spitzen-Schlacke-Relikt |
| Biom: Glutküste | Schlackekern | `item_slag_core` | 1–2 | 0,60 | – | – | Schlacke-Kern |
| Biom: Glutküste | Glutschuppe | `item_ember_scale` | 1–2 | 0,55 | – | – | Glut-Schuppe |

Entfällt (Kreatur kommt dort nicht vor): Bruchstein: Verfall-Relikt; Bruchstein: Altes Eisenstück.

#### 48. Galewyrm (`creature_galewyrm`, MinRang 6, erreicht 6)

| Art | Gegenstand | ID | Menge | Chance | ab Rang | +Menge | Gemini-Name |
|---|---|---|---|---|---|---|---|
| Grund | Sturmwyrm-Schwingenhaut | `item_galewyrm_wing_hide` | 2–3 | 0,95 | 6 | – | Spitzen-Sturm-Schwingenhaut |
| Grund | Drachensehne (Stufe = Rang) | `item_dragon_sinew_t<Rang>` | 2–3 | 0,85 | 6 | – | Massive Drachensehnen |
| Grund | Sturmwyrm-Horn | `item_galewyrm_horn` | 1–2 | 0,75 | 6 | – | Gebogene Sturm-Drachenhörner |
| Rang | Sturmwyrm-Kern | `item_galewyrm_core` | 1–2 | 0,45 | 6 | Ja | Seltener Sturm-Wyrm-Kern |
| Biom: Eisrand | Sturm-Schwingenhaut | `item_storm_wing_hide` | 1–2 | 0,65 | – | – | Sturm-Schwingenhaut |
| Biom: Eisrand | Geladener Kernrest | `item_charged_core` | 1–2 | 0,55 | – | – | Geladener Kernrest |
| Biom: Eisrand | Sturmfaser | `item_storm_fiber` | 2–3 | 0,60 | – | – | Sturm-Faser |

<a id="d24-7"></a>

### 24.7 7. Gegenstands-Katalog

#### 7.1 Allgemeine Teile je Stufe

Stufe = Rang der Kreatur bzw. des Tiers beim Tod. Auch Tiere geben diese Teile (Abschnitt 8 in Dokument 21).

| Teil | IDs | Namen (Stufe 1–6) | Art |
|---|---|---|---|
| Sehne | `item_sinew_t1`, `item_sinew_t2`, `item_sinew_t3`, `item_sinew_t4`, `item_sinew_t5`, `item_sinew_t6` | Dünne Sehne · Zähe Sehne · Kräftige Sehne · Schwere Sehne · Gehärtete Sehne · Uralte Sehne | Bindung |
| Knochen | `item_bone_t1`, `item_bone_t2`, `item_bone_t3`, `item_bone_t4`, `item_bone_t5`, `item_bone_t6` | Leichte Knochen · Knochen · Kräftige Knochen · Schwere Knochen · Gehärtete Knochen · Uralte Knochen | Waffe/Werkzeug |
| Haut | `item_hide_t1`, `item_hide_t2`, `item_hide_t3`, `item_hide_t4`, `item_hide_t5`, `item_hide_t6` | Dünne Haut · Zähe Haut · Dicke Haut · Schwere Haut · Gehärtete Haut · Uralte Haut | Rüstung |
| Horn | `item_horn_t1`, `item_horn_t2`, `item_horn_t3`, `item_horn_t4`, `item_horn_t5`, `item_horn_t6` | Hornsplitter · Horn · Kräftiges Horn · Schweres Horn · Gehärtetes Horn · Uraltes Horn | Waffe/Werkzeug |
| Drachensehne | `item_dragon_sinew_t5`, `item_dragon_sinew_t6` | Drachensehne · Massive Drachensehne | Bindung |

#### 7.2 Biom-Materialien und Knorpel (feste Stufe)

| ID | Name | Stufe | Art | Herkunft | Kreaturen |
|---|---|---|---|---|---|
| `item_ash_mineral` | Asche-Mineral | 5 | Veredelung | Glutküste, auch abbaubar | Vaelgorn, Cinderloom, Siltstrider, Charmaw, Slagbrute, Ashweaver |
| `item_basalt_chunk` | Basaltbrocken | 4 | Bau/Relikt | Basaltfjell, auch abbaubar | Vaelgorn, Osskarr, Kraggar, Riese, Skitterclaw, Slagbrute, Cragstalker |
| `item_cartilage` | Knorpel | 1 | Essen | Zutat für Brühen | Morrowear, Thornmaw, Mirecrawler |
| `item_charged_core` | Geladener Kernrest | 6 | Veredelung | Eisrand | Galeborn, Galehound, Stormwyrmling, Galewyrm |
| `item_damp_leather` | Feuchtleder | 2 | Rüstung | Gezeitenmarsch | Mirecrawler, Siltstrider |
| `item_decay_relic` | Verfall-Relikt | 3 | Bau/Relikt | Hügelgrab-Heide, auch in Ruinen | Osskarr, Shroudwalker, Wiedergänger, Bramblefang, Sporeback, Gloomstalker, Charmaw, Vaultcrawler, Mossgolem |
| `item_ember_scale` | Glutschuppe | 5 | Rüstung | Glutküste | Cinderwyrm, Cinderling, Charmaw, Ashweaver, Cindercolossus |
| `item_fiber` | Pflanzenfasern | 1 | Bindung | gibt es schon in `items.json` | Siltstrider |
| `item_frost_shard` | Frostsplitter | 4 | Veredelung | Basaltfjell und Eisrand | Rimestalker, Rimehound, Frostmauler, Stormwyrmling |
| `item_granite_scale` | Granitschuppe | 4 | Rüstung | Basaltfjell, auch sammelbar | Cragdrake, Skitterclaw, Rimehound, Slagbrute, Cragstalker, Frostmauler, Stormwyrmling |
| `item_iron_ore` | Eisenerz | 3 | Bau/Relikt | Metall-Leiter, auch abbaubar | Graveclaw, Ironhide |
| `item_moss_fiber` | Moosfaser | 2 | Bindung | Gezeitenmarsch, auch sammelbar | Bramblefang |
| `item_old_iron` | Altes Eisenstück | 3 | Bau/Relikt | Hügelgrab-Heide, einschmelzbar | Wiedergänger, Skitterclaw, Charmaw, Vaultcrawler |
| `item_ruin_chitin` | Ruinen-Chitin | 3 | Rüstung | Hügelgrab-Heide | Skitterclaw, Gloomstalker, Vaultcrawler, Mossgolem |
| `item_ruin_dust` | Ruinenstaub | 3 | Bau/Relikt | Hügelgrab-Heide, für Mörtel und Alchemie | Morrowear, Dustmote, Hollowback, Bramblefang, Sporeback |
| `item_slag_core` | Schlackekern | 5 | Veredelung | Glutküste | Cinderloom, Pyroclast, Cinderling, Slagbrute, Ashweaver, Cindercolossus |
| `item_storm_fiber` | Sturmfaser | 6 | Bindung | Eisrand | Galeborn, Rimehound, Galehound, Cragstalker, Frostmauler, Stormwyrmling, Galewyrm |
| `item_storm_wing_hide` | Sturm-Schwingenhaut | 6 | Rüstung | Eisrand | Stormmaw, Stormwyrmling, Galewyrm |
| `item_thorn_wood` | Dornholz | 2 | Bau/Relikt | Gezeitenmarsch, auch sammelbar | Thornmaw, Bramblefang |

#### 7.3 Kreaturentypische Teile

Stufe = niedrigster Rang, ab dem der Teil fallen kann.

| ID | Name | Stufe | Art | Kreaturen |
|---|---|---|---|---|
| `item_ashstalker_pelt` | Aschebalg | 2 | Rüstung | Ashstalker |
| `item_bramblefang_fang` | Fangzahn | 2 | Waffe/Werkzeug | Bramblefang |
| `item_bramblefang_pelt` | Dornenbalg | 2 | Rüstung | Bramblefang |
| `item_charred_fiber` | Verkohlte Faser | 2 | Bindung | Cinderloom, Ashstalker, Cinderling |
| `item_cinderling_ember_shard` | Glimmsplitter | 2 | Veredelung | Cinderling |
| `item_dustmote_feeler` | Fühlerfaser | 2 | Bindung | Dustmote |
| `item_dustmote_skin` | Staubhautlappen | 2 | Rüstung | Dustmote |
| `item_mirecrawler_hide` | Feuchthaut | 2 | Rüstung | Mirecrawler |
| `item_mirecrawler_paw` | Greifpratze | 2 | Waffe/Werkzeug | Mirecrawler |
| `item_siltstrider_hide` | Laufhaut | 2 | Rüstung | Siltstrider |
| `item_siltstrider_sinew` | Lange Laufsehne | 2 | Bindung | Siltstrider |
| `item_spark_ash` | Funkenasche | 2 | Veredelung | Cinderhound, Cinderling |
| `item_thornmaw_bark_hide` | Verholzte Haut | 2 | Rüstung | Thornmaw |
| `item_thornmaw_thorn` | Dornsplitter | 2 | Waffe/Werkzeug | Thornmaw |
| `item_bramblefang_fang_strong` | Verstärkter Fangzahn | 3 | Waffe/Werkzeug | Bramblefang |
| `item_bramblefang_pelt_hard` | Gehärteter Dornenbalg | 3 | Rüstung | Bramblefang |
| `item_charmaw_burn_sinew` | Brandsehne | 3 | Bindung | Charmaw |
| `item_charmaw_jaw` | Verkohlter Kiefer | 3 | Waffe/Werkzeug | Charmaw |
| `item_charmaw_tooth` | Rußzahn | 3 | Waffe/Werkzeug | Charmaw |
| `item_cinderhound_pelt` | Brandfell | 3 | Rüstung | Cinderhound |
| `item_cloth_scrap` | Stofffetzen | 3 | Bindung | Osskarr |
| `item_digging_claw` | Grabklaue | 3 | Waffe/Werkzeug | Dravik, Graveclaw |
| `item_dustmote_feeler_dense` | Dichte Fühlerfaser | 3 | Bindung | Dustmote |
| `item_ember_capsule` | Glutkapsel | 3 | Veredelung | Cinderling |
| `item_graveclaw_shoulder` | Schutt-Schulterplatte | 3 | Rüstung | Graveclaw |
| `item_mirecrawler_paw_strong` | Kräftige Greifpratze | 3 | Waffe/Werkzeug | Mirecrawler |
| `item_morrowear_ear` | Schwingungsorgan | 3 | Veredelung | Morrowear |
| `item_murghul_chitin` | Chitin-Plattenstück | 3 | Rüstung | Murghul |
| `item_murghul_sickle_claw` | Sichelklaue | 3 | Waffe/Werkzeug | Murghul |
| `item_osskarr_bone_joint` | Knochengelenk | 3 | Waffe/Werkzeug | Osskarr |
| `item_shadow_weave` | Schattengewebe | 3 | Rüstung | Shroudwalker, Gloomstalker |
| `item_siltstrider_hide_tough` | Zähe Laufhaut | 3 | Rüstung | Siltstrider |
| `item_skitterclaw_chitin` | Klammer-Chitin | 3 | Rüstung | Skitterclaw |
| `item_skitterclaw_claw` | Krallenspitze | 3 | Waffe/Werkzeug | Skitterclaw |
| `item_sporeback_cap` | Sporenkappe | 3 | Veredelung | Sporeback |
| `item_sporeback_hide` | Buckelhaut | 3 | Rüstung | Sporeback |
| `item_thornmaw_thorn_thick` | Dicker Dorn | 3 | Waffe/Werkzeug | Thornmaw |
| `item_vaelgorn_crust_shard` | Mineral-Krustensplitter | 3 | Rüstung | Vaelgorn |
| `item_charmaw_jaw_hard` | Gehärteter Kiefer | 4 | Waffe/Werkzeug | Charmaw |
| `item_cinderhound_pelt_tough` | Zähes Brandfell | 4 | Rüstung | Cinderhound |
| `item_cragdrake_claw` | Klammerkralle | 4 | Waffe/Werkzeug | Cragdrake |
| `item_digging_claw_heavy` | Schwere Grabklaue | 4 | Waffe/Werkzeug | Pyroclast, Graveclaw |
| `item_ember_knot` | Glutknoten | 4 | Veredelung | Cinderloom, Ashweaver |
| `item_frost_sinew` | Frostsehne | 4 | Bindung | Rimehound |
| `item_graveclaw_shoulder_hard` | Gehärtete Schulterplatte | 4 | Rüstung | Graveclaw |
| `item_hollowback_shell` | Hohlrückenschale | 4 | Rüstung | Hollowback |
| `item_hollowback_spore_dust` | Sporenstaub | 4 | Veredelung | Hollowback |
| `item_ironhide_plate` | Eisenkrusten-Platte | 4 | Rüstung | Ironhide |
| `item_kraggar_granite_hide` | Granithaut | 4 | Rüstung | Kraggar |
| `item_kraggar_rock_claw` | Felsklaue | 4 | Waffe/Werkzeug | Kraggar |
| `item_morrowear_ear_fine` | Feines Schwingungsorgan | 4 | Veredelung | Morrowear |
| `item_mossgolem_bark` | Moosrinde | 4 | Rüstung | Mossgolem |
| `item_mossgolem_bast` | Wurzelbast | 4 | Bindung | Mossgolem |
| `item_mossgolem_rootstone` | Wurzelstein | 4 | Bau/Relikt | Mossgolem |
| `item_murghul_sickle_claw_hard` | Gehärtete Sichelklaue | 4 | Waffe/Werkzeug | Murghul |
| `item_osskarr_bone_strut` | Knochenstrebe | 4 | Waffe/Werkzeug | Osskarr |
| `item_pyroclast_glowstone` | Glühgestein | 4 | Veredelung | Pyroclast |
| `item_revenant_grave_goods` | Grabbeigabe | 4 | Bau/Relikt | Wiedergänger |
| `item_revenant_weapon_fragment` | Waffenbruchstück | 4 | Bau/Relikt | Wiedergänger |
| `item_rimehound_fang` | Raufangzahn | 4 | Waffe/Werkzeug | Rimehound |
| `item_rimehound_fur` | Kaltborstenfell | 4 | Rüstung | Rimehound |
| `item_rimestalker_hide` | Kaltgraue Haut | 4 | Rüstung | Rimestalker |
| `item_rimestalker_spike` | Hornspitze | 4 | Waffe/Werkzeug | Rimestalker |
| `item_shadow_core` | Schattenkern | 4 | Veredelung | Gloomstalker |
| `item_shadow_core_shard` | Schattenkern-Splitter | 4 | Veredelung | Shroudwalker |
| `item_shadow_weave_dense` | Dichtes Schattengewebe | 4 | Rüstung | Shroudwalker, Gloomstalker |
| `item_shroud_relic` | Schleier-Relikt | 4 | Bau/Relikt | Shroudwalker |
| `item_skitterclaw_chitin_hard` | Gehärtetes Klammer-Chitin | 4 | Rüstung | Skitterclaw |
| `item_skitterclaw_chitin_plate` | Seltene Chitinplatte | 4 | Rüstung | Skitterclaw |
| `item_slag_plate` | Schlackeplatte | 4 | Rüstung | Pyroclast, Slagbrute |
| `item_spore_essence` | Sporen-Essenz | 4 | Veredelung | Sporeback |
| `item_sporeback_hide_hard` | Gehärtete Buckelhaut | 4 | Rüstung | Sporeback |
| `item_sporeback_spore_capsule` | Sporenkapsel | 4 | Veredelung | Sporeback |
| `item_troll_hide` | Trollhaut | 4 | Rüstung | Troll |
| `item_vaelgorn_crust_plate` | Dichte Krustenplatte | 4 | Rüstung | Vaelgorn |
| `item_vaultcrawler_joint` | Greifarm-Gelenk | 4 | Waffe/Werkzeug | Vaultcrawler |
| `item_vaultcrawler_plate` | Gewölbeplatte | 4 | Rüstung | Vaultcrawler |
| `item_vaultcrawler_plate_strong` | Verstärkte Gewölbeplatte | 4 | Rüstung | Vaultcrawler |
| `item_vaultcrawler_relic_plate` | Gewölbe-Reliktplatte | 4 | Bau/Relikt | Vaultcrawler |
| `item_ashweaver_chitin` | Verkohltes Chitin | 5 | Rüstung | Ashweaver |
| `item_ashwing_wing_hide` | Asche-Schwingenhaut | 5 | Rüstung | Ashwing |
| `item_cinderwyrm_horn` | Glutdrachenhorn | 5 | Waffe/Werkzeug | Cinderwyrm |
| `item_cragdrake_scale` | Felsdrachenschuppe | 5 | Rüstung | Cragdrake |
| `item_cragmaw_crest` | Steinkamm | 5 | Rüstung | Cragmaw |
| `item_cragmaw_jaw` | Zackenkiefer | 5 | Waffe/Werkzeug | Cragmaw |
| `item_cragstalker_hide` | Gratschuppenhaut | 5 | Rüstung | Cragstalker |
| `item_cragstalker_horn` | Kammhorn | 5 | Waffe/Werkzeug | Cragstalker |
| `item_cragstalker_sinew` | Sprungsehne | 5 | Bindung | Cragstalker |
| `item_dravik_scale_plate` | Dravik-Schuppenplatte | 5 | Rüstung | Dravik |
| `item_frost_core` | Frostkern | 5 | Veredelung | Rimehound |
| `item_frostdrake_scale` | Frostdrachenschuppe | 5 | Rüstung | Frostdrake |
| `item_frostmauler_paw_bone` | Prankenknochen | 5 | Waffe/Werkzeug | Frostmauler |
| `item_frostmauler_scale` | Frostschuppenbrocken | 5 | Rüstung | Frostmauler |
| `item_galeborn_horn_clasp` | Hornspange | 5 | Waffe/Werkzeug | Galeborn |
| `item_galehound_fur` | Sturmborstenfell | 5 | Rüstung | Galehound |
| `item_galehound_sinew` | Böensehne | 5 | Bindung | Galehound |
| `item_giant_armor_scrap` | Riesen-Rüstungsschrott | 5 | Bau/Relikt | Riese |
| `item_giant_bone` | Riesenknochen | 5 | Waffe/Werkzeug | Riese |
| `item_giant_tooth` | Riesenzahn | 5 | Waffe/Werkzeug | Riese |
| `item_heat_fiber` | Hitzefaser | 5 | Bindung | Cinderloom, Ashweaver |
| `item_hollowback_shell_hard` | Gehärtete Rumpfschale | 5 | Rüstung | Hollowback |
| `item_ironhide_plate_massive` | Massive Panzerplatte | 5 | Rüstung | Ironhide |
| `item_kraggar_rock_claw_massive` | Massive Felsklaue | 5 | Waffe/Werkzeug | Kraggar |
| `item_mossgolem_root_core` | Uralter Wurzelkern | 5 | Veredelung | Mossgolem |
| `item_mossgolem_warden_core` | Wächterkern | 5 | Veredelung | Mossgolem |
| `item_murghul_chitin_thick` | Dicker Chitinpanzer | 5 | Rüstung | Murghul |
| `item_rimehound_fur_dense` | Dichtes Hochlagenfell | 5 | Rüstung | Rimehound |
| `item_rimestalker_hide_dense` | Dichte Kältehaut | 5 | Rüstung | Rimestalker |
| `item_slag_plate_dense` | Dichte Schlackeplatte | 5 | Rüstung | Pyroclast, Cinderwyrm, Slagbrute |
| `item_stormwyrmling_horn` | Schmales Drachenhorn | 5 | Waffe/Werkzeug | Stormwyrmling |
| `item_stormwyrmling_wing_hide` | Junge Sturm-Schwingenhaut | 5 | Rüstung | Stormwyrmling |
| `item_troll_hide_thick` | Dicke Trollhaut | 5 | Rüstung | Troll |
| `item_ashwing_heart` | Aschedrachenherz | 6 | Veredelung | Ashwing |
| `item_ashwing_wing_hide_fine` | Gleit-Schwingenhaut | 6 | Rüstung | Ashwing |
| `item_charged_dragon_core` | Geladener Drachenkern | 6 | Veredelung | Stormwyrmling |
| `item_charmaw_ember_jaw` | Glutkiefer | 6 | Waffe/Werkzeug | Charmaw |
| `item_cinderwyrm_heart` | Glutdrachenherz | 6 | Veredelung | Cinderwyrm |
| `item_colossus_bone` | Koloss-Knochen | 6 | Waffe/Werkzeug | Dreadcolossus |
| `item_colossus_core` | Koloss-Kern | 6 | Veredelung | Dreadcolossus |
| `item_colossus_core_glowing` | Glühender Kolosskern | 6 | Veredelung | Cindercolossus |
| `item_colossus_heartstone` | Koloss-Herzstein | 6 | Veredelung | Dreadcolossus |
| `item_colossus_relic` | Koloss-Relikt | 6 | Bau/Relikt | Dreadcolossus |
| `item_colossus_slag_plate` | Schlacke-Kolossplatte | 6 | Rüstung | Cindercolossus |
| `item_colossus_slag_relic` | Schlacke-Relikt | 6 | Bau/Relikt | Cindercolossus |
| `item_colossus_stone` | Koloss-Gestein | 6 | Bau/Relikt | Dreadcolossus, Cindercolossus |
| `item_cragdrake_heart` | Felsdrachenherz | 6 | Veredelung | Cragdrake |
| `item_cragdrake_scale_heavy` | Schwere Bergschuppe | 6 | Rüstung | Cragdrake |
| `item_cragmaw_crest_core` | Kammkern | 6 | Veredelung | Cragmaw |
| `item_cragmaw_trophy` | Kiefer-Trophäe | 6 | Bau/Relikt | Cragmaw |
| `item_cragstalker_hide_hard` | Gehärtete Gratschuppe | 6 | Rüstung | Cragstalker |
| `item_cragstalker_trophy` | Grat-Trophäe | 6 | Bau/Relikt | Cragstalker |
| `item_dravik_horn_core` | Dravik-Hornkern | 6 | Veredelung | Dravik |
| `item_dravik_scale_heavy` | Schwere Dravik-Schuppe | 6 | Rüstung | Dravik |
| `item_dreadwyrm_heart` | Wyrmherz | 6 | Veredelung | Dreadwyrm |
| `item_dreadwyrm_horn` | Uraltes Wyrmhorn | 6 | Waffe/Werkzeug | Dreadwyrm |
| `item_dreadwyrm_scale` | Wyrmschuppe | 6 | Rüstung | Dreadwyrm |
| `item_ember_capsule_stable` | Stabile Glutkapsel | 6 | Veredelung | Cinderloom, Ashweaver |
| `item_ember_scale_fine` | Edle Glutschuppe | 6 | Rüstung | Cinderwyrm |
| `item_frostdrake_heart` | Frostdrachenherz | 6 | Veredelung | Frostdrake |
| `item_frostdrake_scale_hard` | Gehärtete Frostschuppe | 6 | Rüstung | Frostdrake |
| `item_frostmauler_armor` | Gehärteter Frostpanzer | 6 | Rüstung | Frostmauler |
| `item_frostmauler_rare_bone` | Hochlagen-Knochen | 6 | Waffe/Werkzeug | Frostmauler |
| `item_galeborn_storm_gland` | Sturmdrüse | 6 | Veredelung | Galeborn |
| `item_galewyrm_core` | Sturmwyrm-Kern | 6 | Veredelung | Galewyrm |
| `item_galewyrm_horn` | Sturmwyrm-Horn | 6 | Waffe/Werkzeug | Galewyrm |
| `item_galewyrm_wing_hide` | Sturmwyrm-Schwingenhaut | 6 | Rüstung | Galewyrm |
| `item_giant_bone_massive` | Kolossaler Riesenknochen | 6 | Waffe/Werkzeug | Riese |
| `item_giant_relic` | Riesen-Relikt | 6 | Bau/Relikt | Riese |
| `item_heat_core` | Hitzekern | 6 | Veredelung | Pyroclast, Slagbrute |
| `item_heat_fiber_fine` | Hochfester Glut-Faserstrang | 6 | Bindung | Ashweaver |
| `item_hollowback_spore_sac` | Sporensack | 6 | Veredelung | Hollowback |
| `item_ironhide_ore_crust` | Erzkruste | 6 | Bau/Relikt | Ironhide |
| `item_kraggar_ore_vein` | Erzader-Brocken | 6 | Bau/Relikt | Kraggar |
| `item_rimestalker_frost_spike` | Frostdorn | 6 | Waffe/Werkzeug | Rimestalker |
| `item_storm_fiber_dense` | Dichte Sturmfaser | 6 | Bindung | Galeborn |
| `item_stormmaw_heart` | Sturmdrachenherz | 6 | Veredelung | Stormmaw |
| `item_stormmaw_horn` | Sturmdrachenhorn | 6 | Waffe/Werkzeug | Stormmaw |
| `item_wyrm_relic` | Wyrm-Relikt | 6 | Bau/Relikt | Dreadwyrm |



[↑ zum Inhaltsverzeichnis](#inhaltsverzeichnis)


---

<a id="teil-6"></a>

# Teil VI – Art Direction, Stil und Figuren

<a id="d25"></a>

## 25. Stilidee „Gelebter Norden“ (Vorschlag)

> Quelle: `design/stil_gelebter_norden.md`

> **Abgelöst (2026-11-15):** Geltende Stilvorgabe ist `design/stil_norden_gezeitenhall.md` (Mischung mit „Küstenasche“). Dieses Dokument bleibt als Grundlage zum Nachlesen.

Stand 2026-11-14. Vorschlag von Claude auf Wunsch des Nutzers: eigene Stilidee, angelehnt an **Aska**, **Medieval Dynasty** und **Kingdom Come: Deliverance II**. Nicht bestätigt. Es wird nichts aus diesen Spielen übernommen (keine Assets, Namen, Motive); sie dienen nur als Richtung.

<a id="d25-1"></a>

### 25.1 Was wir von wem lernen

| Vorbild | Was dort überzeugt | Was wir übernehmen | Was wir anders machen |
|---|---|---|---|
| Aska | Nordische Siedlung, klare Formen, starke Lichtstimmung (warmes Feuer gegen kalte Nacht), Siedlung von oben gut lesbar | lesbare Silhouetten, Stimmungslicht, Vogelperspektive als Teil des Spiels | realistischere Materialien und Gesichter |
| Medieval Dynasty | Alltag, Jahreszeiten, Feldbau, Holzbau, das Dorf wächst sichtbar | Spuren des Lebens im Dorf, Jahreszeiten als Bildwechsel, Handwerk sichtbar | Skandinavien statt Mitteleuropa; dichtere Atmosphäre |
| Kingdom Come II | historische Glaubwürdigkeit, Materialien mit Schmutz und Abnutzung, natürliches Licht, kaum aufdringliche Anzeige | Abnutzung, Schlamm, glaubwürdige Kleidung und Werkzeuge, zurückhaltende Anzeige | kein Simulations-Zwang; dazu eine leise mythische Ebene (nordische Magie) |

<a id="d25-2"></a>

### 25.2 Kernidee in einem Satz

**Ein glaubwürdiger, gelebter Norden, in dem man jede Jahreszeit spürt und in dem die Mythen leise durchscheinen: realistisch im Material, klar in der Form, warm im Feuer, kalt im Wind.**

<a id="d25-3"></a>

### 25.3 Sechs Stilregeln

1. **Glaubwürdig, nicht museal.** Häuser, Werkzeuge und Kleidung könnten so im 9.–11. Jh. im Norden existiert haben, sind aber für das Spiel vereinfacht (klare Bauteile, gut greifbare Werkzeuge). Keine Hörnerhelme, keine Plattenrüstung.
2. **Klare Silhouetten, auch von oben.** Jedes Spielelement ist an seiner Form erkennbar: Bäume nach Art, erntbare Pflanzen an Früchten (Früchte dürfen leicht übertrieben groß und farbig sein), Gebäude an der Dachform. Aus der Vogelperspektive muss man das Dorf „lesen“ können.
3. **Spuren des Lebens.** Wege treten sich dort aus, wo Bewohner und Spieler oft laufen; vor Werkstätten liegen Späne, am Lagerfeuer Asche, an der Schmiede Ruß. Neue Bauten sind hell und sauber, alte vergraut, bemoost, ausgebessert.
4. **Die Jahreszeit bestimmt die Farbe.** Frühling frisch und hell, Sommer satt und golden, Herbst rostrot und neblig, Winter blau-weiß und leise. Die Farbsättigung bleibt insgesamt gedämpft; kräftige Farben gibt es nur an Feuer, Früchten, Färbestoffen und Magie.
5. **Warm gegen kalt.** Licht erzählt Sicherheit: Feuer, Herd und Fackeln warm (orange, ca. 2000–2700 K), Nacht, Nebel und Wildnis kalt (blau-grün). Bodennebel in Senken und über Wasser (siehe Methoden-Doku).
6. **Mythos leise.** Magie (nordisch, Runen, Seidr) ist selten und gedämpft: Runen glimmen schwach, Seidr zeigt sich als Nebel, Schatten, Visionen. Keine bunten Feuerbälle. Farben der Magie: kaltes Türkis und Glut-Orange.

<a id="d25-4"></a>

### 25.4 Material- und Farbpalette

- **Baustoffe:** Kiefern-/Fichtenholz (frisch honigfarben → vergraut silbergrau), Torf-/Grassodendach und Reet, Feldstein, Lehmflechtwerk, Birkenrinde.
- **Kleidung:** Wolle und Leinen in Naturtönen (Grau, Braun, Ungefärbt), Leder, Fell; Farbe als Wohlstand: Krapprot, Färberwaid-Blau, Ocker, selten Grün.
- **Metall:** dunkles, geschmiedetes Eisen mit Rost, wenig Bronze und Silber (Schmuck).
- **Regeln für Texturen:** Foto-/Scan-Materialien (CC0 oder eigene Fotos), einheitliche Texeldichte ca. 512 px pro Meter in der Nähe, Rauheit eher hoch (wenig Glanz), keine reinen Farben.

<a id="d25-5"></a>

### 25.5 Menschen

- Realistische Proportionen und Gesichter (MetaHuman laut Entscheidung), wettergegerbte Haut, Zöpfe, Bärte, Tätowierungen/Bemalung als Wahl im Editor.
- Kleidung zeigt Tätigkeit und Zustand: Ruß beim Schmied, Erde beim Bauern, Nässe nach Regen, Schnee auf den Schultern im Winter.

<a id="d25-6"></a>

### 25.6 Anzeige (UI)

- So wenig wie nötig, gut lesbar (Entscheidung zur Lesbarkeit gilt). Rahmen und Symbole wie geschnitztes Holz, gestanzte Lederstücke, Runenkanten, aber schlicht.
- Möglichst im Bild statt auf dem Bild: z. B. Fäulnis sieht man am Apfel selbst, Hunger an der Haltung der Figur; die Leiste bleibt trotzdem für Klarheit.

<a id="d25-7"></a>

### 25.7 Klang (passend zur vorhandenen Musik)

- Natur und Handwerk im Vordergrund (Wind, Holzhacken, Schmiede, Tiere), Musik selten und leise: Viking-Folk am Feuer, Ambient in der Wildnis.

<a id="d25-8"></a>

### 25.8 Technik-Leitplanken

- 8-GB-Grafikkarte als Ziel: Nanite für Felsen und Gebäude, Pflanzen als Karten-Modelle mit LOD, Licht dynamisch (Lumen auf Hoch, einfachere Lösung auf Niedrig).
- Gesamtgröße ≤ 20 GB: lieber wenige, gut variierte Materialien als viele Einzeltexturen.

<a id="d25-9"></a>

### 25.9 Was das für die laufenden Arbeiten heißt

- Stil-Probe (Gebäude aus Highlands und Medieval Village, umtexturiert): prüft Regel 1, 3 und die Materialpalette.
- Grassodendach als typisch nordisches Dach ergänzen (Material fehlt noch).
- Früchte und erntbare Pflanzen leicht übertreiben (Regel 2) – passt zum offenen Punkt „Haselnüsse nicht erkennbar“.
- Abnutzung und Moos über ein gemeinsames Material statt pro Modell.
- Nächster Schritt Design: Moodboards je Jahreszeit und für Dorf/Wildnis/Nacht (ChatGPT, siehe Rollen in `CLAUDE.md`).


[↑ zum Inhaltsverzeichnis](#inhaltsverzeichnis)

<a id="d26"></a>

## 26. Stil „Gelebter Norden“ mit Gezeitenhall (Mischung)

> Quelle: `design/stil_norden_gezeitenhall.md`

Stand 2026-11-15. Auf Wunsch des Nutzers („erstelle eine passende Mischung“) zusammengeführt aus
`design/stil_gelebter_norden.md` (Grundlage) und `design/inbox/08_stilbibel_kuestenasche.md` (Magie, Zeichen, Kreaturen, Palette).
**Festgelegt vom Nutzer (2026-11-15: „Ja, festlegen“).** Dieses Dokument ist die Stilvorgabe und ersetzt beide Quellen; die Quellen bleiben zum Nachlesen.

<a id="d26-1"></a>

### 26.1 Kernidee in einem Satz

**Ein glaubwürdiger, gelebter Norden, in dem man jede Jahreszeit spürt – und an seinen Rändern eine alte Kraft, der Gezeitenhall, die sich als türkises Seeglas, gebrochene Ringe und eine antwortende Natur zeigt.**

Kurz: **Alltag historisch, Ränder mythisch.** Dorf, Werkzeuge, Kleidung und Arbeit bleiben glaubwürdig (9.–11. Jh., vereinfacht). Das Fantastische gehört zu besonderen Orten, Kreaturen und Fähigkeiten, nie zum Alltagsgegenstand.

<a id="d26-2"></a>

### 26.2 Was woher kommt

| Bereich | aus „Gelebter Norden“ | aus „Küstenasche“ |
|---|---|---|
| Grundgefühl | glaubwürdig, nicht museal; Spuren des Lebens; warm gegen kalt | Stimmungswörter (rau, kühl, windgeformt, handgemacht, geheimnisvoll, alt, widerständig) |
| Farbe | Jahreszeit bestimmt die Farbe | feste Grundpalette mit Farbwerten (die Jahreszeit verschiebt sie) |
| Magie | Runen und Seidr als kulturelle Form, selten und leise | Gezeitenhall als Quelle: Seeglas, Lichtadern, gebrochene Ringe, Rhythmus sammeln – ausbreiten – abklingen |
| Zeichen | – | offener Ring mit drei schrägen Kerben |
| Kreaturen | – | magische Merkmale wachsen aus Körper und Umgebung |
| Menschen, UI, Klang, Technik | vollständig übernommen | Kleidung „mehrere Schichten, geflickt“ ergänzt |

<a id="d26-3"></a>

### 26.3 Sieben Stilregeln

1. **Glaubwürdig, nicht museal.** Häuser, Werkzeuge und Kleidung könnten so im 9.–11. Jh. im Norden existiert haben, vereinfacht für das Spiel. Keine Hörnerhelme, keine Plattenrüstung, keine leuchtenden Alltagsgegenstände.
2. **Klare Silhouetten, auch von oben.** Bäume nach Art, erntbare Pflanzen an (leicht übertriebenen) Früchten, Gebäude an der Dachform, Kreaturen an ihrem Umriss. Das Dorf muss aus der Vogelperspektive lesbar sein.
3. **Spuren des Lebens.** Ausgetretene Wege, Späne vor Werkstätten, Asche am Feuer, Ruß an der Schmiede. Neue Bauten hell und sauber, alte vergraut, bemoost, ausgebessert, erweitert.
4. **Die Jahreszeit verschiebt die Grundpalette.** Die Grundpalette (unten) gilt das ganze Jahr; die Jahreszeit färbt sie: Frühling heller und grüner, Sommer wärmer und goldener, Herbst rostiger und nebliger, Winter blau-weiß und leise. Sättigung bleibt gedämpft.
5. **Warm gegen kalt.** Herdfeuer und Fackeln warm (ca. 2000–2700 K) bedeuten Sicherheit; Nacht, Nebel und Wildnis kühl. Aktiver Gezeitenhall ist die einzige zweite Lichtfarbe: blasses Türkis.
6. **Mythos an den Rändern.** Der Gezeitenhall ist selten. Im Ruhezustand: milchiges Seeglas, das kaum oder gar nicht leuchtet. Beim Einsatz: kurzer Puls, gebrochene Ringe, die Umgebung reagiert, dann klingt er ab. Keine bunten Feuerbälle, keine Dauerleuchten.
7. **Ein Zeichen, sparsam.** Der offene Ring mit drei schrägen Kerben erscheint als Schnitzerei, Schmiedemarke, Felsader oder Form eines Effekts – klein und selten, damit er wiedererkennbar bleibt.

<a id="d26-4"></a>

### 26.4 Grundpalette

| Farbe | Wert | Verwendung |
|---|---|---|
| Nebelgrau | `#9AA8A5` | Himmel, Dunst, kaltes Tageslicht |
| Treibholz | `#A99B80` | helles, vergrautes Holz, Leder, trockene Fasern |
| Aschebraun | `#716957` | dunkles Holz, Erde, Alltagskleidung |
| Basaltschwarz | `#303735` | Eisen, Fels, Schatten |
| Moosgrün | `#56654F` | Wald, Flechten, Moos |
| Rostrot | `#904C3E` | Krapprot-Stoffe, Wicklungen, Nähte, Kulturakzente |
| Seeglas | `#5D9990` | schlummernder Gezeitenhall, Mineral, seltene Details |
| Gezeitenlicht | `#A9E9DC` | aktiver Gezeitenhall, sparsame helle Akzente |
| Herdfeuer | `#DB9345` | Feuer, Fackeln, Fensterlicht |

Kräftige Farben gibt es nur an Feuer, Früchten, Färbestoffen und aktivem Gezeitenhall.

<a id="d26-5"></a>

### 26.5 Kleidung und Kleidungsfarben (Charaktereditor)

Mehrere Schichten aus Wolle, Leinen, Leder und Fell, gemacht für Arbeit, Kälte und Reisen. Getragen: geflickt, ausgeblichen, an beanspruchten Stellen dunkler; Tätigkeit sichtbar (Ruß, Erde, Nässe, Schnee). Naturtöne als Alltag, Farbe als Wohlstand. Die Editor-Optionen bleiben, ihre Werte folgen der Palette:

| Option (`look_cloth_color`) | Name | Wert |
|---|---|---|
| `cloth_natural` | ungefärbt | `#A99B80` (Treibholz) |
| `cloth_grey` | Wollgrau | `#8A8C86` |
| `cloth_brown` | braun | `#716957` (Aschebraun) |
| `cloth_ochre` | Ocker | `#A9844A` |
| `cloth_red` | Krapprot | `#904C3E` (Rostrot) |
| `cloth_blue` | Waidblau | `#4E6475` |
| `cloth_green` | Moosgrün | `#56654F` |

Kein Türkis/Seeglas als Kleidungsfarbe: das bleibt dem Gezeitenhall vorbehalten (höchstens ein kleiner Seeglas-Einsatz an Schmuck oder Herkunftszeichen, z. B. bei der Seherin).

<a id="d26-6"></a>

### 26.6 Menschen

- Realistische Proportionen und Gesichter (MetaHuman), wettergegerbte Haut, Zöpfe, Bärte, Tätowierungen/Bemalung als Wahl im Editor.
- Bemalung „Runen“ und Herkunft Seherin dürfen das Kerben-Zeichen tragen; Runen glimmen nur beim Einsatz von Gezeitenhall-Fähigkeiten.

<a id="d26-7"></a>

### 26.7 Materialien

- **Holz:** Kiefer/Fichte, frisch honigfarben, vergraut zu Treibholz-Grau; Maserung, Kerben, Reparaturen.
- **Dächer:** Torf-/Grassoden, Reet; Wände aus Holz, Lehmflechtwerk; Feldstein-Sockel; Birkenrinde.
- **Stein:** grauer Fels und dunkler Basalt mit Moos, Flechten, an der Küste Salzränder.
- **Eisen:** dunkel, matt, Schmiedespuren, Rost an beanspruchten Stellen; wenig Bronze und Silber (Schmuck).
- **Seeglas:** milchig, leicht durchscheinend, trübe innere Schlieren; im Ruhezustand ohne eigenes Leuchten. Je seltener ein Gegenstand, desto klarer das Glas und desto eher erscheinen Lichtadern beim Gebrauch.
- **Texturen:** Foto-/Scan-Materialien (CC0 oder eigene), ca. 512 px pro Meter in der Nähe, Rauheit eher hoch, keine reinen Farben.

<a id="d26-8"></a>

### 26.8 Welt

Küsten, Fjorde, dichte Wälder, Hochmoore, felsige Hochebenen. Wetter prägt die Stimmung: Nebel schluckt die Ferne, Regen dunkelt Holz und Stein, Schnee vereinfacht zu hell und dunkel. Siedlungen wachsen und werden ausgebessert. Alte Steinkreise, überwucherte Wege und halb versunkene Mauern sind älter als die Siedler – dort tritt der Gezeitenhall zutage: türkise Adern in Fels und Wurzeln, Ringmuster auf Wasser, Nebel, der sich kurz in Bögen bewegt.

<a id="d26-9"></a>

### 26.9 Kreaturen

Klare Silhouette zuerst. Gewöhnliche Tiere ohne sichtbare Magie. „Gezeichnete“ Kreaturen mit ein bis zwei Merkmalen, die aus Körper oder Umgebung wachsen (Seeglas-Platten, durchscheinende Geweihe, Mineraladern im Fell, Nebelatem). Alte Wesen verschmelzen mit Wurzeln, Fels, Wasser oder Nebel. Die Magie reagiert auf ihr Verhalten (Adern leuchten beim Angriff, Nebel beim Einatmen).

<a id="d26-10"></a>

### 26.10 Gezeitenhall: Fähigkeiten und Effekte

Ein Rhythmus für alles: **sammeln – in gebrochenen Ringen ausbreiten – abklingen.**
1. Kurzer Lichtimpuls an Hand, Stab, Waffe, Kreatur oder Boden.
2. Unterbrochener türkiser Ring breitet sich aus (gern mit den drei Kerben).
3. Die Umgebung reagiert: Wasser kräuselt sich, Gras legt sich, Staub hebt sich, Nebel wird auseinandergezogen.
4. Das Licht zieht sich zurück, kurze schimmernde Spuren bleiben.

Runen und Seidr sind die menschlichen Wege, den Gezeitenhall zu nutzen: Runen werden geritzt (Seherin: „Rune des Schutzes“ als kurzer Schutzkreis), Seidr zeigt sich als Nebel und Visionen. Nicht kämpferische Fähigkeiten sind ausdrücklich gewollt: Spuren sichtbar machen, Verborgenes enthüllen, Pflanzen bewegen.

<a id="d26-11"></a>

### 26.11 Licht und Atmosphäre

Tag: kühles, weiches Licht, oft bewölkt. Innen und am Feuer warm. Nacht: Mond, Sterne, Feuer; aktiver Gezeitenhall ergänzt blasses Türkis und färbt nahe Flächen sanft, ohne zu überstrahlen. Bodennebel in Senken und über Wasser. Figuren und wichtige Gegenstände bleiben bei Nebel, Regen und Schnee erkennbar.

<a id="d26-12"></a>

### 26.12 Anzeige (UI) und Klang

- UI: so wenig wie nötig, gut lesbar; Rahmen wie geschnitztes Holz, gestanzte Lederstücke, schlichte Runenkanten. Möglichst im Bild statt auf dem Bild. Hervorhebungen und der Umgebungs-Scan dürfen die Ring-Sprache des Gezeitenhalls nutzen (die Scan-Welle als gebrochener Ring), Farbe Gezeitenlicht.
- Klang: Natur und Handwerk im Vordergrund, Musik selten und leise (Viking-Folk am Feuer, Ambient in der Wildnis); Gezeitenhall mit tiefem, hallendem Puls.

<a id="d26-13"></a>

### 26.13 Regeln für einheitliche Assets

- An der Silhouette erkennbar, auch klein und von oben.
- Pro Objekt höchstens ein markantes Seeglas-Detail und das Zeichen – und nur, wenn das Objekt selten oder magisch ist.
- Alltagsgegenstände matt und natürlich; aktiver Gezeitenhall hebt sich klar davon ab.
- Abnutzung, Moos und Vergrauung über gemeinsame Materialien statt pro Modell.

<a id="d26-14"></a>

### 26.14 Technik-Leitplanken

- 8-GB-Grafikkarte: Nanite für Fels und Gebäude, Pflanzen als Karten-Modelle mit LOD, Lumen auf Hoch, einfacher auf Niedrig.
- Gesamtgröße ≤ 20 GB: wenige, gut variierte Materialien.
- Gezeitenhall-Effekte als wenige gemeinsame Niagara-Systeme (Puls, Ring, Spuren) mit Parametern statt vieler Einzeleffekte.

<a id="d26-15"></a>

### 26.15 Gemeinsamer Stilprompt (für Bild- und 3D-Werkzeuge)

```text
Glaubwürdiger, gelebter nordischer Alltag im 9.–11. Jahrhundert, semi-realistisch, mit einer leisen mythischen Ebene an den Rändern der Welt. Raue, bewohnte Landschaften mit Küsten, Wäldern, Mooren und Felsen; Siedlungen aus vergrautem Holz, Grassoden- und Reetdächern, ausgebessert und gewachsen. Glaubwürdige Proportionen, handgemachte, verwitterte Materialien: Treibholz-graues Holz, dunkles Eisen mit Rost, Basalt, Moos, grobe Wolle und Leinen, Leder, Fell. Gedämpfte Farben aus Nebelgrau, Treibholz, Aschebraun, Basaltschwarz und Moosgrün; Rostrot und Ocker als sparsame Stoffakzente; warmes Herdfeuer gegen kühles Tageslicht. Die Jahreszeit färbt die Szene. Die alte Kraft heißt „Gezeitenhall“: milchig-türkises Seeglas, feine Lichtadern, gebrochene, wandernde Ringe, sichtbar nur an alten Orten, bei gezeichneten Kreaturen und beim Einsatz von Fähigkeiten; im Ruhezustand zurückhaltend. Wiederkehrendes Zeichen: ein offener Ring mit drei schrägen Kerben, klein und selten.
```


[↑ zum Inhaltsverzeichnis](#inhaltsverzeichnis)

<a id="d27"></a>

## 27. Briefing für das Design (ChatGPT): Fotorealistische Vegetation

> Quelle: `design/briefing_fotorealismus.md`

**Stand:** 2026-11-13 · **Von:** Claude Code (Umsetzung) · **Für:** ChatGPT (Design), zur Weitergabe durch den Nutzer

<a id="d27-1"></a>

### 27.1 Was sich geändert hat
Der Nutzer hat am 2026-11-13 entschieden: **Die Vegetation wird fotorealistisch** (Fotos als Vorlage, Texturen aus frei nutzbaren Fotos). Gleichzeitig gilt: **kein Geld für Asset-Dienste** (kein Meshy, keine Kaufpakete). Siehe `docs/01_entscheidungen.md`, Zeilen „Vegetation: kostenlos selbst erzeugen“ und „Stil der Vegetation“.

Das weicht von der gewählten Art Direction **Klarform-Saga** ab (`design/inbox/01_art_direction.md`: stilisiert, klare Formen, kräftige Farben, Detailgrad niedrig bis mittel).

<a id="d27-2"></a>

### 27.2 Bitte um Vorschlag
1. **Gesamtstil:** Wie passen fotorealistische Natur und Siedlung zusammen? Möglichkeiten: (a) alles realistischer (Gebäude, Figuren, Werkzeuge, UI-Akzente), (b) realistische Natur mit leicht stilisierter Siedlung und klaren Farbakzenten, (c) andere Lösung. Mit Vor- und Nachteilen für Aufwand, Lesbarkeit in der Vogelperspektive und Leistung.
2. **Regeln für die Vegetation:** Farbstimmung (nordisch: Fjordwiesen und Birkenhaine, `biome_fjord_meadow`), Lesbarkeit der sammelbaren Pflanzen (Beeren, Äpfel, Haselnüsse, Pilze müssen auch aus einigen Metern erkennbar sein), Zustände voll / abgeerntet / Stumpf / Setzling.
3. **Holzbaukasten:** Reichen neue, realistischere Texturen auf den vorhandenen 10 Bauteilen, oder braucht es neue Formen?

<a id="d27-3"></a>

### 27.3 Rahmenbedingungen
- Budgets (`CLAUDE.md`): fertiges Spiel höchstens ca. 20 GB, läuft mit mindestens 8 GB RAM; Entwicklungs-PC RTX 3070 (8 GB Grafikspeicher). Realistisches Laub (viele durchsichtige Blätter) kostet deutlich mehr Leistung als stilisiertes.
- Quellen nur frei nutzbar und dokumentiert: eigene Fotos des Nutzers oder CC0 (z. B. Poly Haven, ambientCG). Alles KI-Erzeugte ins `design/asset_log.md`.
- Umsetzung (Claude): Bäume per eigenem Generator in Blender mit Fototexturen, fotogescannte CC0-Modelle (Felsen, Stümpfe, Gras), sichtbares Nachwachsen im Spiel.

<a id="d27-4"></a>

### 27.4 Ergebnis bitte nach
`design/inbox/08_art_direction_fotorealismus.md`


[↑ zum Inhaltsverzeichnis](#inhaltsverzeichnis)

<a id="d28"></a>

## 28. Briefing für ChatGPT – Design-Lead

> Quelle: `design/CHATGPT_BRIEFING.md`

*Dieses Dokument ist für ChatGPT geschrieben. Der Nutzer kopiert es (bzw. den Abschnitt „Erster Auftrag“) in den Chat.*

<a id="d28-1"></a>

### 28.1 Deine Rolle
Du bist Design-Lead für ein PC-Spiel (Unreal Engine 5, Koop, Wikinger/Mittelalter). Programmierung und Umsetzung macht Claude Code.
Du lieferst Design: Art Direction, Siedlungs- und UI-Konzepte, Looks, Stimmung. Du schreibst keinen Spielcode.

<a id="d28-2"></a>

### 28.2 Das Spiel in Kürze
- Leitspiel **Aska**: Der Spieler führt eine Wikinger-Siedlung, Dorfbewohner erledigen Routinearbeit (Holz, Bau, Kochen, Schmieden …), der Spieler erkundet, kämpft, baut.
- Koop (mehrere Spieler), auch solo spielbar. Open World mit mehreren Questlinien, Biomen und Bossen.
- Basisbau mit Bauteilen, Ausrüstung mit Slots (levelbasiert, schmiedbar), Skilltree, späte Portale zwischen Siedlungslagern.
- Hauptkamera Third-Person; zusätzlich eine Vogelperspektive als Bau- und Dorf-Kommandoansicht.
- Weltoptionen im Menü, kombinierbar: freies Bauen, Horden, „Survivor“ (Bedeutung noch offen).

<a id="d28-3"></a>

### 28.3 Harte Vorgaben
- **Keine Ausdauerbalken**, die Laufen, Ducken oder Waffengebrauch blockieren.
- **Kein Zeitdruck** in der UI (keine Countdown-Schleifen als Kernelement).
- **Lesbarkeit der Dorf-UI hat Vorrang.** Askas Schwächen sind klobige Lager-/Inventar-Menüs und zu viel Babysitting: Dorfbewohner-Zustand, Jobs und Lager müssen auf einen Blick lesbar und mit wenigen Klicks steuerbar sein.
- **Keine Nachahmung geschützter Designs** (Aska, Valheim, Kingdom Come, Manor Lords, Medieval Dynasty). Die Spiele sind Vorbild für Prinzipien, nicht für Optik, Figuren oder Logos.
- **Umfang bremsen:** Gib bei jedem Vorschlag eine **MVP-Stufe** (klein, zuerst) und eine **Ausbaustufe** an.

<a id="d28-4"></a>

### 28.4 Was du liefern sollst (in dieser Reihenfolge)
1. **Art Direction:** 2–3 Stilvarianten zur Auswahl (Formensprache, Farbpalette, Licht/Stimmung, Detailgrad), jeweils mit kurzer Begründung und Aufwandseinschätzung.
2. **Siedlungs-Look:** Gebäudetypen, Bauteil-Set und Materialstufen (Holz → Stein → Eisen …).
3. **Dorfbewohner:** Berufe/Rollen, Look-Varianten, Anzeige von Bedürfnissen und Zufriedenheit.
4. **UI/HUD:** Dorf-Kommandoansicht, Lager/Inventar, Baumenü, Schmiede, Skilltree, Weltoptionen-Menü.
5. **Ausrüstung:** Slots (wie in Kingdom Come), Stufenstaffelung, visuelle Unterschiede pro Stufe.
6. **Biome und Bosse:** Stimmung je Biom, Gegner- und Bossdesign.
7. *(optional)* Audio- und Musikrichtung.

<a id="d28-5"></a>

### 28.5 Technische Budgets (gelten auch fürs Design)
- **Gesamtgröße des fertigen Spiels: höchstens ca. 20 GB.** Plane sparsam: wenige, wiederverwendbare Materialien, keine 4K-Texturen für Nebensächliches, Variation über Farbe und Kombination statt über neue Assets.
- **Mindest-PC mit 8 GB RAM, empfohlen 16 GB.** Der Look muss auch in niedrigen Qualitätsstufen funktionieren.
- **Bis zu 8 Spieler (seit 2026-11-13, vorher 16) und viele Dorfbewohner gleichzeitig im Bild:** Figuren müssen günstig sein (wenige Varianten, die sich kombinieren lassen). Die UI muss mit bis zu 8 Spielern lesbar bleiben (Spielerliste).

<a id="d28-6"></a>

### 28.6 Format der Ergebnisse
- Markdown-Dateien, benannt `NN_thema.md` (z. B. `01_art_direction.md`). Bilder als PNG mit Beschreibung dazu.
- Alles, was später in Daten übergehen soll (Gebäude, Berufe, Ausrüstungsstufen, UI-Elemente), als **Tabelle mit stabilen IDs** in `snake_case` (z. B. `building_longhouse`, `job_woodcutter`). So kann Claude es direkt in Datentabellen überführen.
- Am Ende jeder Lieferung ein Abschnitt **„Entscheidungen, die der Nutzer treffen muss“**.
- **Prompts für KI-Generatoren** (Konzeptbilder, Texturen, Icons, 3D) immer als vollständiges Prompt-Paket nach `design/PROMPT_STANDARD.md`: alle Pflichtfelder, Stil-, Licht-, Hintergrund- und Ausschlussblock wörtlich, Proportionen als Verhältnisse, Farben als Hex-Werte aus der Palette, messbare Abnahme-Kriterien. Ein Prompt pro Objekt. Beispiele: `design/prompts/`.
- Alles KI-Generierte (Bild, Text, Audio) im Format `Datei | Werkzeug | Datum | Lizenz/Quelle` melden; der Nutzer trägt es in `design/asset_log.md` ein.

<a id="d28-7"></a>

### 28.7 Ablauf
Der Nutzer legt deine Ergebnisse in `design/inbox/` ab. Claude liest sie, prüft die Machbarkeit (Umfang!) und meldet Rückfragen zurück.

<a id="d28-8"></a>

### 28.8 Erster Auftrag an ChatGPT
Starte mit **Punkt 1 (Art Direction)** und **Punkt 2 (Siedlungs-Look)**. Liefere zuerst die Art-Direction-Varianten und halte an, bis der Nutzer eine Variante gewählt hat.
Stelle vorab höchstens drei Rückfragen, falls dir etwas Wesentliches fehlt.


[↑ zum Inhaltsverzeichnis](#inhaltsverzeichnis)

<a id="d29"></a>

## 29. Leitfaden: Blender- und Unreal-Assets, Gesichter, Animation

> Quelle: `design/leitfaden_blender_unreal_figuren.md`

Stand 2026-11-21. Verfasst von Claude Code auf Grundlage der Referenzordner und einer kurzen Web-Recherche.
Ergänzt `design/inbox/Referenzmaterialien_2026-11-21/01_Bauweise_und_Materialien.md` (Gegenstände, Materialkacheln) – das dort Gesagte wird hier nicht wiederholt.

**Ehrlicher Rahmen:** Das Durchsehen der Bilder „trainiert“ das KI-Modell nicht dauerhaft. Was bleibt, ist dieses Dokument (plus ein Memory-Eintrag), das bei künftigen Modellier-Aufgaben gelesen wird. Regeln und Zahlen hier sind Arbeitswerte und Vorschläge von Claude, keine bestätigten Entscheidungen.

<a id="d29-1"></a>

### 29.1 1. Was ausgewertet wurde

| Quelle | Inhalt | Brauchbar als |
|---|---|---|
| `D:\gamer\referensbilder - Modell\*.jpg/png` (47) | 38 Dreamstime-Stockvorschauen (Waffen, Rüstung, Fantasy-Figuren, Metall-/Schuppenkacheln, Keltenknoten), 7 Gemini-KI-Bilder, 1 Rüstungsübersicht, 1 weiteres | nur Anschauung |
| `design/referenz/figuren`, `gesichter` (7) | dieselben 7 Gemini-Bilder (Dateien identisch) | Stil- und Typenvorlage für Figuren |
| `…\3d mshes\*.zip/rar` (23) | Bäume, Weizen, Gras, Schaf, Hirsch, Lederrüstung, Ritterfigur, Axt, Bögen, Schwerter, Hut, Löwenkrieger | Technik-Anschauung (Poly-Zahlen, UV, Texturaufbau) |

Inventar und Prüfskript: `design/inbox/Referenzanalyse_2026-11-21/` (`reference_inventory.json`, `audit_summary.txt`).

<a id="d29-2"></a>

### 29.2 2. Kritische Befunde (vor allem anderen lesen)

1. **Stockbilder sind keine Texturen.** Alle Dreamstime-Dateien tragen Wasserzeichen und sind urheberrechtlich geschützte Vorschauen. Weder Pixel noch Muster (Keltenknoten, Tapete, Schuppen) direkt übernehmen; nur Form und Bauweise beobachten.
2. **Stilbruch.** Gut die Hälfte der Stockbilder zeigt Hochmittelalter/Fantasy (Plattenrüstung, Visierhelme, verzierte Zweihänder, Elfen). Das Projekt ist Wikingerzeit („gelebter Norden“). Passend sind: Rundschild, Bartaxt, Brillenhelm (`old-helmet`), Lamellen-/Schuppenstudien, die Gemini-Trachten. Keltenknoten sind insular-keltisch; für nordische Ornamentik eher Borre-, Jelling-, Mammen-, Ringerike- oder Urnes-Stil als eigene Motive zeichnen.
3. **3D-Archive ohne Lizenz.** In keinem Archiv liegt eine Lizenzdatei (Skript hat danach gesucht). Die `open3dmodel.com`-Pakete haben keine nachgewiesene Herkunft → laut `CLAUDE.md` nicht ins Spiel. Ordner `lizenzbeachtet` heißt nur, dass der Nutzer die Lizenz beachtet hat; die Lizenztexte sind nicht beigelegt und müssten in `design/asset_log.md` mit Quelle stehen, bevor etwas davon eingebaut wird.
4. **Keine einzige Rig- oder Animationsvorlage.** Alle GLB-Dateien haben 0 Skins, 0 Gelenke, 0 Animationen, 0 Morph Targets. Für Rigging und Gesichtsanimation liefert der Ordner nichts; dafür gilt Abschnitt 5–6 (Recherche).
5. **Die Gemini-Bilder** sind laut Asset-Log bereits als „nicht im Spiel verwenden, nur Maßstab“ eingetragen. Das bleibt so.

<a id="d29-3"></a>

### 29.3 3. Lehren aus den 3D-Archiven (was man richtig und falsch sieht)

| Beispiel | Messwert | Lehre |
|---|---|---|
| `3715` Baum | 1,2 Mio. Vertices, nur 12 UVs | Renderbaum für Standbilder, für Spiele unbrauchbar. Spielbaum: Stamm 2–8 k Tris + Blattkarten (Alpha-Ebenen), LODs, in UE Nanite-Foliage prüfen. |
| `107090` Baum | 400 k Vertices | dito; Faustregel: was nicht in einer Spielszene 100-fach stehen kann, ist kein Vegetations-Asset. |
| `5291` „trees lo-poly“ | 153 k Vertices für mehrere Bäume, 17 Materialien | Viele Materialslots = viele Draw Calls. Ziel: 1–2 Materialien je Baum (Rinde, Blattatlas). |
| `79822` Gras | 130 Vertices, 2 Texturen | Richtig: Gras als wenige gekreuzte Karten mit Alphatextur. |
| `107698` Weizen | 21 k Vertices, ein Atlas mit Halmen | Gutes Muster: alle Halme/Ähren in einen Atlas, Karten instanziieren. |
| `armored-warrior` | Base color / Metallic / Roughness / Mixed_AO / Normal_OpenGL / Opacity je Bauteil; Haare als **Haarkarten** (Strähnen-Atlas + Alpha) | Saubere Metal/Rough-Kette wie Substance Painter. **Achtung:** Normalmaps OpenGL → in Unreal Grünkanal umkehren (UE erwartet DirectX). Haarkarten statt Strands spart VRAM (8-GB-Ziel). |
| `78271` Lederrüstung | Diffuse + Normal + specularGlossiness | Alte Spec/Gloss-Kette. Umrechnen: Roughness = 1 − Glossiness, Metallic aus Material ableiten (Leder 0, Eisen 1), Spec-Farbe verwerfen. |
| `27992` Hirsch | stag_diff/stag_Spec/stag_hair.bmp | 12-MB-BMPs, Spec-Kette, Fell als Textur. Veraltet; als Format nie BMP liefern. |
| `bow.zip` | eine 8k-Textur, Inseln füllen ca. 15 % | Verschwendung: 8k für einen Bogen. Waffe in Hand: 1–2k, Inseln dicht packen, gleiche Texeldichte. |
| `recurve-bow` | 7 Texturen, Inseln winzig auf großer Fläche | Schlechte Texeldichte; besser ein Atlas mit gestapelten Spiegelhälften. |
| `medival-sword` | nur Kachelmaterialien (Metal03/07, Leder) | Kachelmaterial geht für Prototyp; für Nahansicht unikat backen (Kantenabrieb, Griffwicklung). |
| `plhat.glb` Hut | 25,8 k Vertices / ~39 k Tris | Für einen Hut viel zu dicht (Ziel 1–3 k Tris + Normalmap). Typisch für unbearbeitete KI-/Scan-Modelle. |
| `lion-warrior.glb` | 27 k Vertices, nur Basecolor, zerstückelter Atlas | Typisch KI-generiert: kein Normal/Rough, fragmentierte UVs → nicht riggbar ohne Retopo. |

**Daraus abgeleitete Budgets (Vorschlag, für 8 GB VRAM / 20 GB Spiel):**

| Asset | Tris LOD0 | Textur |
|---|---|---|
| Handwerkzeug, Waffe | 1,5–5 k | 1k–2k, ein Material |
| Schild, Helm | 2–6 k | 2k |
| Kleidungsteil (Figur) | 3–10 k | 2k, Texturen teilen |
| Bewohner komplett (nicht MetaHuman) | 15–35 k | 2–3 × 2k |
| Gras/Halm-Büschel | 50–300 | Atlas 1–2k |
| Baum | 3–15 k + Blattkarten, ≥3 LODs oder Nanite | Rinde 2k kachelnd, Blattatlas 2k |

Texeldichte (Vorschlag): Figuren/Ausrüstung ca. 10,24 px/cm (1k auf 1 m), Spielergegenstände in Ego-Nähe bis 20 px/cm.

<a id="d29-4"></a>

### 29.4 4. Pipeline Blender → Unreal (Einstellungen, die Fehler vermeiden)

**Szene:** Einheit Meter, Unit Scale 1,0 (oder 0,01 – nur einheitlich bleiben). +Y ist in Blender vorn; Figur blickt −Y. Transformationen vor dem Export anwenden (Strg+A → All Transforms). Ursprung: Fußpunkt bei Figuren und stehenden Objekten, Griffpunkt bei Waffen (wichtig für Sockets).

**Statische Meshes (FBX):** Selected Objects; Apply Scalings „FBX All“; Forward −Y, Up Z; Smoothing „Face“; Apply Modifiers an; Tangent Space an. Kollision als eigenes Objekt `UCX_<MeshName>_00` (konvex, wenig Polys). LOD in UE generieren oder `_LOD1` usw.

**Skelettmeshes:** Armature + Mesh exportieren, *Add Leaf Bones aus*, *Only Deform Bones an*, Primary/Secondary Bone Axis Y/X. Erprobte Exporteinstellungen: `design/inbox/Testkopf_2026-11-21/export_testkopf.py` (Rückimport in Blender geprüft, Unreal noch nicht). Häufigster Fehler laut Foren: Skelett kommt in UE mit Skalierung 100 bzw. 0,01 an → Armature-Objektskala vor dem Export 1,0, angewendet; im Zweifel kleines Testmodell exportieren und in UE die Root-Bone-Skala prüfen. Animationen getrennt als „nur Armature, Bake Animation“ exportieren und auf das vorhandene Skeleton importieren.

**Projektspezifisch:** Das Projekt nutzt das UE-Standardskelett (Manny/Quinn) und MetaHumans mit Retargeting (Entscheidung 2026-11-15). Eigene Figuren daher **direkt auf dem Manny-Skelett aufbauen** (Manny als FBX nach Blender holen, Mesh darauf skinnen, nur das Mesh zurück) statt ein eigenes Rig zu erfinden – dann laufen alle Animationen (Kevin Iglesias, Quaternius UAL2) ohne Retargeting.

**Rigify:** nur wenn eigene Animationen in Blender entstehen sollen. Rigify ist nicht direkt spieltauglich (Hierarchie, Steuerknochen); vor Export auf die DEF-Knochen reduzieren (Add-ons wie Uefy/Rig2U5 tun das, Lizenz vorher prüfen). In UE dann IK Rig + IK Retargeter; Foren melden gute Ergebnisse mit Rotation „Interpolated“ und Translation „Absolute“ an den IK-Ketten.

**Texturen nach UE:** BaseColor sRGB; ORM (R AO, G Rough, B Metal) linear, Kompression „Masks“; Normal **DirectX** (bei OpenGL-Quellen „Flip Green Channel“). PNG/TGA, keine BMP/JPEG für Normalmaps.

**Mit dem Blender-MCP prüfen:** nach jedem Bau `look` (Ansicht vorn/seitlich/3/4), Polycount und Maße per Skript ausgeben; erst dann „fertig“ melden (Regel 2).

<a id="d29-5"></a>

### 29.5 5. Gesichter

#### 5.1 Was die Referenzbilder vorgeben
- Wettergegerbte, asymmetrische Gesichter; Altersspanne von jung bis sehr alt mit klaren Falten (Stirn, Krähenfüße, Nasolabial, Marionettenfalten).
- Narben (Augenbraue, Wange, Nasenrücken), Sommersprossen, Hautrötungen, Gesichtsbemalung/Tätowierung als Runenlinien (Stirn, Kinn).
- Bärte oft geflochten mit Perlen/Ringen, Haare in Zöpfen, rasierte Seiten. Breite Mischung an Hauttönen.
- Kleidung: Hängerock mit Ovalfibeln und Perlenschnüren, Tunika mit Brettchenborte, Wickelgamaschen, Wendeschuhe, Fell- und Karo-Umhänge, Gürteltaschen. Farbpalette gedeckt: Waidblau, Krapprot, Moosgrün, Ocker, Naturbraun.
- KI-typische Fehler in den Bildern: Fibeln und Borten nicht symmetrisch, Gürtelschnallen „schmelzen“, Hände ungenau. Für Details historische Funde statt dieser Bilder nehmen.

#### 5.2 Weg im Projekt: MetaHuman
Die Spielerfiguren sind als MetaHuman entschieden. Ein selbst modellierter Kopf wird damit nicht 1:1 übernommen:
- **From Custom Mesh / Mesh to MetaHuman** (UE 5.6+ im Editor): beliebige Topologie, auch Scans und KI-Meshes. Laut Epic-Doku und Foren löst der Solver MetaHuman-Parameter, die die Vorlage annähern, und baut den Standardkopf daraus neu. Feine Merkmale (kleines Kinn, besondere Nase) gehen dabei teils verloren.
- **From Template Mesh:** genauer, verlangt aber exakt die MetaHuman-Vorlagentopologie.
- Folgerung: Charakterköpfe im MetaHuman Creator über dessen Regler formen (Referenzbilder 5.1 als Zielbilder), Narben/Bemalung als Hautdetail-Textur oder Decal, Bärte/Zöpfe über MetaHuman-Groom oder eigene Haarkarten. Frontale Porträts **nicht** auf Köpfe projizieren.

#### 5.3 Wenn ein Kopf selbst in Blender entsteht (Bewohner, Gegner, Tiere)
**Topologie (Kantenschleifen):**
- Geschlossene Ringe um Augen (5–7 Schleifen inkl. Lidkante, Lidfalte, Augenhöhle) und um den Mund (6–8 Ringe bis über die Lippenkante hinaus) – das sind die beweglichen „Muskelringe“.
- Eine Schleife von Nasenflügel über die Nasolabialfalte bis unter das Kinn, damit die Falte beim Lächeln knickt statt zu dehnen.
- Pole (5er-/3er-Knoten) in ruhige Zonen legen: Schläfe, Wange seitlich, nicht in Mundwinkel oder Lidwinkel.
- Ober- und Unterlippe getrennt (sonst kann der Mund nicht öffnen), innen Mundsack modellieren; Augäpfel, Zähne, Zunge als eigene Objekte.
- Reine Quads, gleichmäßige Dichte; Spielkopf ca. 4–8 k Tris für Bewohner, Details per Normalmap aus einem Sculpt backen.

**Gesichtsausdrücke:** Industriestandard sind die **52 ARKit-Blendshapes** (Namen wie `eyeBlinkLeft`, `jawOpen`, `mouthSmileLeft` …). Vorteil: kompatibel mit Live Link Face, Audio-zu-Gesicht-Werkzeugen und MetaHuman-Animator-Ausgaben. In Blender als Shape Keys mit exakt diesen Namen anlegen; Hilfs-Add-ons (FaceKeys, ARKit Blendshape Tools, ARKitBlendshapeHelper) erzeugen Rohformen, die danach von Hand nachgearbeitet werden müssen. Lizenz jedes Add-ons vor Nutzung prüfen. Export: FBX mit Shape Keys → in UE Morph Targets.

**Prüfliste Ausdruck:** Blinzeln schließt das Lid vollständig, ohne durch den Augapfel zu stoßen · `jawOpen` ohne Lippenriss · Lächeln hebt Wange und schiebt das Unterlid hoch · Asymmetrie links/rechts getrennt · Kombination `jawOpen`+`mouthSmile` bleibt sauber.

#### 5.4 Praxistest „Testkopf“ (2026-11-21)

Skripte, FBX und `.blend` liegen in `design/inbox/Testkopf_2026-11-21/` (reproduzierbar: `build_testkopf.py` → `shape_keys_testkopf.py` → `export_testkopf.py`). Nur Pipeline-Test, kein Spiel-Asset (Ei-Form, keine Ohren, keine Haare, keine UVs/Texturen).

**Geprüft und erfüllt:** 2.776 Quads ≈ 5.550 Dreiecke (Budget 4–8 k); 3 Kantenringe um Augen und Mund; Ober- und Unterlippe getrennt, Mundsack geschlossen; keine Kanten mit >2 Flächen, keine losen Kanten; Lidöffnung 8,5 × 23,4 mm. Shape-Keys `eyeBlinkLeft/Right`, `jawOpen`, `mouthSmileLeft/Right`: Blinzeln verdeckt das richtige Auge vollständig (Strahltest 0/20 statt 14/20 sichtbar), das andere bleibt offen; Kiefer öffnet ca. 18 mm. FBX-Rückimport in Blender: Maße und Skala 1,0 unverändert, Shape-Keys verformen identisch. **Nicht geprüft:** Import in Unreal (Editor nicht gestartet).

**Lehren (gelten für jedes künftige Modell):**
1. **Ringstärke × Anzahl < halbe Lochhöhe.** Sonst kippt die Innenfläche beim Einsetzen, und Ringe falten sich über die Öffnung. Das Auge war deshalb zuerst zu, obwohl die Randmessung „offen“ ergab. Prüfen mit einem **Strahltest von vorn** statt nur mit Randmaßen.
2. **Öffnungen nicht nachträglich stark aufweiten.** Wenn der Rand weiter wandert, als der Ringabstand beträgt, kreuzt er den ersten Ring. Lieber die Ausgangsfläche größer anlegen.
3. **Lidkante zur Augapfelmitte einziehen, nicht stur nach +Y.** Am äußeren Augenwinkel zeigt die Fläche zur Seite, ein +Y-Einzug verdreht sie. Den Augapfel danach hinter jeden Randpunkt setzen (Abstand ≥ Radius + 0,7 mm).
4. **`bmesh.ops.extrude_face_region` lässt die Ausgangsflächen stehen.** Sie müssen selbst gelöscht werden, sonst entstehen innere Flächen und dunkle Schattierungsflecken.
5. **`mesh.materials.clear()` setzt die Materialindizes zurück.** Indizes erst nach dem Anlegen der Slots setzen.
6. **`grid_fill` liefert bei flachen Schlitzen still nichts.** Den Mundsack durch Verschweißen schließen (erzeugt 4 Dreiecke an den Mundwinkeln, vertretbar).
7. **Mess- statt Schätzwerte für Merkmale:** Die Mundlinie lag 6 mm höher als angenommen, Lippenwulst und Kiefer-Abklingen saßen falsch. Positionen aus dem Modell messen und weitergeben.
8. **Nach dem Setzen von Positionen `view_layer.update()`**, sonst ist `matrix_world` noch alt (Shape-Key-Skript fand die Augen bei 0,0,0).
9. **Morph Targets brauchen in Unreal ein Skelett-Mesh.** Ein Kopf mit Shape-Keys bekommt mindestens root → neck → head; die Augäpfel hängen per Gewicht am Knochen head. Knochenlängen kommen beim FBX-Rückimport anders an (FBX speichert keine Länge), das ist normal.
10. **Seiten:** ARKit „Left“ ist die linke Seite der Figur, bei Blick nach −Y also **+X**. Mit Strahltest pro Seite bestätigt.

<a id="d29-6"></a>

### 29.6 6. Animation

- **Vorhandene Bibliotheken zuerst:** Kevin Iglesias, Quaternius UAL2 (CC0) – passen aufs Manny-Skelett. Eigene Animationen nur für Lücken (Hinhocken beim Aufsammeln, Herstellen der Anfangswerkzeuge, siehe Entscheidungen 2026-11-15).
- **Regeln für eigene Animationen:** 30 fps; In-Place für Lauf-Zyklen (Bewegung kommt aus dem Code), Root Motion nur für Aktionen mit festem Weg (Ausweichrolle, Schlag mit Ausfallschritt); Zyklen mit identischem erstem und letztem Frame; Kontaktposen zuerst blocken (Key Poses → Breakdowns → Spline), dann Überlappung (Zöpfe, Umhang besser als Physik/Cloth in UE).
- **Arbeitsaktionen:** Schlagbewegung mit klarer Ausholpose und einem Treffer-Frame; diesen Frame in UE als Anim Notify markieren (Server wertet den Treffer aus, Regel 1).
- **Gesichtsanimation/Lippen:** entschieden ist MetaHuman aus Audio. Für Nicht-MetaHuman-Köpfe mit ARKit-Shapes funktionieren dieselben Kurvennamen.

<a id="d29-7"></a>

### 29.7 7. Offene Fragen an den Nutzer
1. Sollen Bewohner ebenfalls MetaHumans werden oder eigene, leichtere Köpfe nach 5.3? (Budget 8 GB VRAM spricht für eigene.)
2. Lizenztexte der Archive im Ordner `lizenzbeachtet` nachreichen, falls etwas davon ins Spiel soll.
3. Ornamentik: nordische Stile (Urnes, Mammen …) statt Keltenknoten – einverstanden?

<a id="d29-8"></a>

### 29.8 Quellen (Recherche 2026-11-21)
- Epic: [MetaHuman from mesh](https://dev.epicgames.com/documentation/en-us/metahuman/from-mesh), [from template mesh](https://dev.epicgames.com/documentation/en-us/metahuman/from-template-mesh), [From Custom Mesh tool](https://dev.epicgames.com/documentation/metahuman/metahuman-creator-from-custom-mesh-tool-in-unreal-engine)
- Forum: [Mesh to MetaHuman verändert das Gesicht](https://forums.unrealengine.com/t/metahuman-creator-changes-my-custom-face-mesh-after-mesh-to-metahuman-is-this-normal/2844122), [Root bone scaled x100](https://forums.unrealengine.com/t/root-bone-is-scaled-by-x100/484622), [Blender→UE Skalierung](https://forums.unrealengine.com/t/blender-to-unreal-fbx-always-scales-wrong-with-skeletal-mesh-imports/2657998)
- [Blender to UE5: complete export pipeline](https://sarahhyperdense.substack.com/p/blender-to-ue5-the-complete-export)
- Apple: [ARKit BlendShapeLocation](https://developer.apple.com/documentation/arkit/arfaceanchor/blendshapelocation)
- Add-ons (nur Hinweis, Lizenz ungeprüft): [FaceKeys](https://superhivemarket.com/products/facekeys/docs), [ARKit Blendshape Tools](https://extensions.blender.org/add-ons/arkit-blendshape-tools/), [Rig2U5](https://superhivemarket.com/products/rig2u5), [Uefy](https://forums.unrealengine.com/t/uefy-2-0-pro-edition-make-blenders-rigify-rigs-compatible-with-ue5-also-has-a-free-version/120782)
- [Blender Studio: Stylized Character Workflow (Gesichtsschleifen)](https://studio.blender.org/training/stylized-character-workflow/5e5408388faf011a381510de)


[↑ zum Inhaltsverzeichnis](#inhaltsverzeichnis)

<a id="d30"></a>

## 30. Kleidung für die Figuren: kostenlose Kandidaten (Fab, geprüft 2026-11-16)

> Quelle: `design/kleidung_kandidaten_kostenlos.md`

Vorgabe des Nutzers: eigene Umsetzung, angelehnt an die Figuren von Aska und Enshrouded (mehrere Schichten Wolle, Leinen, Leder, Fell; nichts davon nachbauen), **nur kostenlose Assets**. Stilfarben: `design/stil_norden_gezeitenhall.md`.
Noch nichts heruntergeladen (Download und Fab-Konto: nur durch den Nutzer bzw. nach ausdrücklicher Freigabe). Lizenzen vor Einbau in `design/asset_log.md` eintragen.

| Asset | Was | Passt zu MetaHuman? | Lizenz | Bewertung |
|---|---|---|---|---|
| [Poor man Outfit](https://www.fab.com/listings/ebeea391-8264-44ef-8cb0-f4801ffea4a6) (NOETA Marketstore) | ganzes Outfit, zerschlissen, Schichten | **ja**, parametrisch (.mhpkg, `WI_PoormanOutfit` im Creator) | Fab Standardlizenz, Stufe „kostenlos“ wählbar (bis 22,64 € für Profi-Stufe) | **bester Kandidat** für Bewohner: geht direkt in den Creator, passt sich kräftig/schmal an. Wüsten-Look, Farben lassen sich evtl. anpassen. Bewertung 3,5/5. |
| [Medieval Shirt Lace Up](https://www.fab.com/listings/552462f7-e15e-463f-b305-459c733bba8b) (Clothcraft Studio 3D) | Leinenhemd mit Schnürung | nein, nur Mesh (fbx/obj/Maya), nicht geriggt | Fab Standardlizenz, kostenlos | gute Grundlage für eigene Tunika in Blender (an MetaHuman-Körper anpassen, riggen) |
| [Medieval Hooded Cloak](https://www.fab.com/listings/4bef3f38-bfdc-4623-b081-5e6fd9eb41ee) (Paradoox) | Kapuzenumhang, 4K PBR | nein, nur Mesh | Fab Standardlizenz, kostenlos | Umhang für Seherin/Jäger, in Blender anpassen |
| [Viking Outfit](https://www.fab.com/listings/36ffca37-c25b-4d81-bbae-d09c98915c1a) / [Viking Man Outfit](https://www.fab.com/listings/aa53f092-c884-48ab-b162-018c17b6570b) (a9908244) | Wikinger-Kleidung mit Fell | nein, nur glb | **CC-BY** (Namensnennung nötig; Felltexturen von einer DeviantArt-Quelle, Lizenz dort extra prüfen) | Optik am nächsten an Wikinger-Tracht; Qualität/Topologie unbekannt, erst ansehen |
| [Viking](https://www.fab.com/listings/ca4ba583-8d90-4069-b51f-50e694530b2f) (Art.Hiraeth) | fertige Figur mit eigenem Skelett | nein | keine Lizenzangabe auf der Seite | **nicht geeignet** (eigene Figur, Lizenz unklar) |
| MetaHuman Techwear, Jeans, Hoodie, Survivor (Epic, polycorn) | moderne Kleidung | ja | kostenlos | passen nicht zum Setting |

Kostenpflichtig gesehen (nur zur Information, nicht gewählt): Medieval Commoner Outfit (38,84–49,63 €), Parametric Viking Warrior Outfit Pack Vol 1 (37,75–64,74 €, UE 5.6–5.7).

<a id="d30-1"></a>

### 30.1 Referenzbilder des Nutzers (2026-11-16)

Erstes Bild (Figurenauswahl 3 × 8) nur im Chat; vier weitere liegen in `design/referenz/figuren/figuren_1..4.jpg` (KI-generierte Vorlagen des Nutzers, nur Referenz, nicht im Spiel verwendet). Ergänzungen aus diesen: viele Hauttöne (hell bis dunkel, ostasiatisch), Zöpfe mit Perlen, Cornrows, Kranzzopf bei älteren Frauen, geflochtene Bärte mit Perlen, Stirnbänder, Fellkragen, Karo-Umhänge, Schürzenkleider in Rot/Ocker. Daran ausrichten:
- **Frauen:** knöchellange Wollkleider (Moosgrün, Waidblau, Rostbraun) mit helleren Leinen-Unterkleidern, Schürzenkleid mit zwei ovalen Fibeln und Perlenketten dazwischen, Ledergürtel mit Tasche; Haare in langen Zöpfen (blond, rot).
- **Männer:** Tuniken mit gewebter Borte am Halsausschnitt (Grün, Blau, Rostrot), Lederweste darüber, breite Ledergürtel, Thorshammer-Anhänger; einzelne mit Fellumhang, Kapuzenmantel mit Fibel oder Kettenhemd/Lederrüstung (Krieger-Herkunft). Haare zurückgebunden, Dutt, Seitenschnitt oder lang; Vollbärte, teils geflochten; ein grauhaariger Älterer.
- Haut meist hell und wettergegerbt, einzelne dunklere Figuren.
- Farben passen zur Stilpalette (keine Türkis-Kleidung).
Haare/Bärte/Haut sind danach in `tools/unreal/metahuman_looks.py` gesetzt; die Kleidung ist der Maßstab für die eigene Tracht.

<a id="d30-2"></a>

### 30.2 Vorschlag

1. **Sofort:** „Poor man Outfit“ (kostenlose Stufe) als Bewohner-Grundkleidung im Creator; Farben auf die Stilpalette (Wollgrau, Aschebraun, Ocker, Waidblau, Moosgrün) stellen, sofern das Outfit Farbparameter hat.
2. **Danach, eigene Umsetzung:** Hemd (Lace Up), Umhang (Hooded Cloak) und Wikinger-Teile (CC-BY) in Blender als Ausgangsbasis, an den MetaHuman-Körper anpassen, Schichten ergänzen (Wollkittel, Gürtel, Fellkragen, Wickelgamaschen), riggen, als eigene Kleidungsstücke einbinden. Größere Aufgabe, eigene Sitzung.


[↑ zum Inhaltsverzeichnis](#inhaltsverzeichnis)

<a id="d31"></a>

## 31. Review 01 – Art Direction, Holzbaukasten, Arbeitsstätten (2026-11-12)

> Quelle: `design/reviews/01_review_baukasten_arbeitsstaetten.md`

*Von Claude Code. Geprüft: `design/inbox/01_art_direction.md`, `03_assets_baukasten.md`, `04_assets_arbeitsstaetten.md` gegen `design/ASSET_PLAN.md`.*
*Für ChatGPT: Bitte Punkte 1–5 in einer überarbeiteten Fassung von 03 und 04 berücksichtigen. Entscheidungen des Nutzers stehen am Ende.*

<a id="d31-1"></a>

### 31.1 Gesamturteil
Gute Grundlage. Format, IDs, Budgets, Zustände, MVP/Ausbau und KI-Nachweis entsprechen dem Asset-Plan; alle Priorität-A-Assets aus 5.3 und 5.4 sind abgedeckt. Die Art Direction ist als Entscheidung eingetragen (`docs/01_entscheidungen.md`).
Es gibt **einen Rechenfehler beim Lager** und **drei technische Lücken**, die vor dem Modellieren geklärt sein müssen.

<a id="d31-2"></a>

### 31.2 1. Lager passt nicht zum Dach-Baukasten (muss geklärt werden)
Die Dachmodule decken bei 45° eine **horizontale Spannweite von 150 cm je Seite** ab (Dachfläche 212 cm, First 150 cm über der Wand). Damit lassen sich nur Gebäude mit **300 cm Breite quer zum First** decken.
Das Lager ist mit **600 × 600 cm** und **ca. 450 cm bis zum First** angegeben. Bei 600 cm Breite und 45° läge der First aber **300 cm** über der Wand (gesamt 600 cm), und dafür fehlen Module (doppelte Dachflächen, größerer Giebel). 450 cm würden eine flachere Neigung (ca. 27°) und einen eigenen Dachsatz verlangen.

Möglichkeiten:
- **A (Empfehlung): Lager 300 × 600 cm**, First in Längsrichtung. Passt ohne neue Module zum Baukasten; die 450 cm bis zum First stimmen dann. Die „zweigeteilte“ Idee bleibt: 300 × 300 geschlossener Raum + 300 × 300 offene Regalbucht.
- **B: 600 × 600 cm beibehalten** und den Baukasten erweitern: zwei Dachflächen je Seite plus ein halber Giebel (rechtwinkliges Dreieck 300 × 300 cm). First dann bei 600 cm. Mehr Module, höheres Gebäude.
- **C: eigener, flacherer Dachsatz** für große Gebäude. Am teuersten.

<a id="d31-3"></a>

### 31.3 2. Dreiecksbudgets: Ziel- statt Höchstwerte für Module
Jedes Bauteil hat „max. 2.000 Dreiecke“. Ein Gebäude aus 20–30 Modulen läge dann bei 40.000–60.000 Dreiecken, viel zu viel für eine Siedlung mit vielen Gebäuden und bis zu 16 Spielern.
Bitte **Zielwerte** ergänzen, 2.000 bleibt die harte Obergrenze. Vorschlag:

| Modul | Ziel |
|---|---:|
| Wand, Türwand | 800 |
| Fundament, Boden | 500 |
| Dachfläche | 600 |
| Giebel | 400 |
| First, Pfosten, Balken | 200 |
| Türblatt | 400 |

**Lager:** als **eigenes, optimiertes Mesh im Stil des Baukastens** planen (gleiche Trim-Sheets, keine verdeckten Innenflächen), nicht wörtlich aus Modulen zusammengesetzt. Das 8.000-Ziel ist dann realistisch. Die Füllzustände setzt Claude als Blueprint um (`BP_Building_Storage`) mit wiederverwendeten Item-Meshes.

<a id="d31-4"></a>

### 31.4 3. Fundament auf unebenem Gelände
20 cm Höhe reichen nur auf flachem Boden. Auf Hängen schwebt das Fundament oder versinkt.
Bitte das Fundament mit einem **Sockel nach unten** beschreiben: 20 cm sichtbar über der Bauebene, **mindestens 100 cm** darunter (geschlossene Seiten, gleiche Trim-Sheet-Fläche). Später optional ein Stelzen-Modul für steile Hänge.

<a id="d31-5"></a>

### 31.5 4. Drehpunkte (Pivots) und Ecken
Für das Einrasten in Unreal braucht jedes Modul einen festen Ursprung. Vorgabe von Claude:

| Modul | Ursprung |
|---|---|
| Fundament, Boden | Mitte der Oberseite |
| Wand, Türwand, Giebel | unten, Mitte, auf der Rasterlinie (Wandmitte) |
| Pfosten | unten, Mitte |
| Balken, First | Mitte der Längsachse |
| Dachfläche | Traufkante, unten, Mitte |
| Türblatt | Scharnierkante, unten |

**Ecken:** Wände stehen mittig auf der Rasterlinie (20 cm stark). An Außenecken überlappen sich zwei Wände. Bitte festlegen, dass **Eckpfosten** (`building_pillar_wood`, 25 × 25) die Wandenden verdecken. Die Randpfosten in der Wand dürfen dann höchstens 12,5 cm vom Wandende nach innen reichen, damit sie im Eckpfosten verschwinden.

<a id="d31-6"></a>

### 31.6 5. Turf-Material
Terrain-Materialien (Landscape-Schichten) lassen sich nicht direkt auf Meshes legen. Die Turf-Kante bekommt eine **eigene Materialinstanz, die die Gras-Texturen wiederverwendet**. Das ist in Ordnung, kostet aber einen zweiten Material-Slot pro Dachmodul (ein zusätzlicher Draw-Call je Modul). Bei vielen Dächern ist das spürbar, deshalb die Empfehlung unten.

<a id="d31-7"></a>

### 31.7 Kleinigkeiten (optional)
- Ansichten: Der Asset-Plan wünscht bei Gebäuden Vorder- und Seitenansicht. Für die Übergabe an einen 3D-Artist hilft je Modul eine einfache Skizze (vorne/seitlich/oben). KI-Bilder bitte mit Nachweis.
- Türwand: Öffnung 130 × 230 cm und Türblatt 126 × 226 cm passen. Die Tür muss nach innen aufgehen können, ohne einen 120 cm breiten Durchgang zu versperren.

<a id="d31-8"></a>

### 31.8 Nachprüfung der überarbeiteten Fassung (2026-11-12)
Überarbeitete `03_assets_baukasten.md` und `04_assets_arbeitsstaetten.md` geprüft und in `design/inbox/` übernommen.

| Punkt | Ergebnis |
|---|---|
| 1 Lager/Dach | erledigt: 300 × 600 cm, Wand 300, First 450, nutzt das 45°-Dachmodul |
| 2 Budgets | erledigt: Zielwerte wie vorgeschlagen, 2.000 als harte Obergrenze; Lager als eigene optimierte Hülle (Ziel 8.000) |
| 3 Fundament | erledigt: 20 cm sichtbar, mind. 100 cm Sockel; Stelzenmodul als späterer Ausbau |
| 4 Pivots/Ecken | erledigt: Tabelle übernommen, Eckpfosten-Regel eingearbeitet |
| 5 Turf | erledigt: eigene geteilte Materialinstanz, Draw-Call-Kosten als Messpunkt notiert |

**Offen, klein:** Das Lager hat seinen Ursprung „auf Geländehöhe“, beschreibt aber keinen Sockel nach unten wie das Fundament. Auf Hängen würde es schweben. Beim Modellieren gleiche Sockelregel anwenden (mind. 100 cm unter Gelände). Kein neuer Durchlauf nötig.

<a id="d31-9"></a>

### 31.9 Entscheidungen des Nutzers
**Am 2026-11-12 entschieden: alle drei Empfehlungen übernommen** (Holzdach mit Turf-Kante, Lager 300 × 600 cm, Lagerfeuer). Eingetragen in `docs/01_entscheidungen.md`.
Für ChatGPT: Bitte 03 und 04 entsprechend überarbeiten (Punkte 1–5 oben, Lager auf 300 × 600 cm umplanen).

| # | Frage | Empfehlung von Claude (übernommen) |
|---|---|---|
| 1 | **Dachdeckung:** Holz mit Turf-Kante oder ganzes Turf-Dach? | **Holz mit schmaler Turf-Kante.** Ein ganzes Turf-Dach verschmilzt in der Vogelperspektive mit der Graslandschaft, die Gebäude wären schlechter zu erkennen. Wer sparen will: im MVP nur Holz, Turf-Kante später. |
| 2 | **Lagergröße** | **300 × 600 cm** (Variante A in Punkt 1), passt ohne neue Dachmodule. |
| 3 | **Kochstelle:** offenes Lagerfeuer mit Holzgalgen und Kessel? | **Ja.** Günstig (2.000 Dreiecke), gut lesbar, passt zum Slice. |


[↑ zum Inhaltsverzeichnis](#inhaltsverzeichnis)

<a id="d32"></a>

## 32. Review 02 – Arbeitsstätten (3. Fassung), Armbrust, Rüstung, Entscheidungsdelta (2026-11-12)

> Quelle: `design/reviews/02_review_armbrust_ruestung_entscheidungen.md`

*Von Claude Code. Geprüft: `design/inbox/04`–`07` gegen `docs/01_entscheidungen.md` und die Asset-Listen in `design/`.*
*Für ChatGPT: Bitte Punkt 3 (Slots) in einer überarbeiteten `06` umsetzen. Die offenen Fragen klärt der Nutzer.*

<a id="d32-1"></a>

### 32.1 1. `04_assets_arbeitsstaetten.md` (3. Fassung) – passt
Beeren und Mahlzeiten liegen jetzt in `prop_basket` und `prop_sack`, wie in `design/ASSET_LIST_GEGENSTAENDE.md` vorgeschlagen. Keine weiteren Anmerkungen. Der kleine Punkt aus Review 01 (Sockel des Lagers am Hang) gilt weiter beim Modellieren.

<a id="d32-2"></a>

### 32.2 2. `05_assets_armbrust.md` – passt, eine offene Stufe
- Teile-Strategie, Budgets (Ziel 2.000 / max. 4.000) und Benennung passen zur Waffenliste.
- **Offen:** Codex schlägt **ab T4** (seltene Import-/Schwarzmarktware) vor, Claude **ab T3**. Entscheidet der Nutzer. T4 passt gut zum bestätigten Nacht-Händler.
- **Neu bestätigt vom Nutzer:** Freischaltung über bestimmte Attribute plus eine Fähigkeit im Skilltree. Das sollte in 05 ergänzt werden.

<a id="d32-3"></a>

### 32.3 3. `06_assets_ruestung_kleidung.md` – Slots müssen angepasst werden
Der Nutzer hat am 2026-11-12 diese Slots festgelegt: **Helm, Schulter, Hände, 2 × Ring, Beine, Füße, Umhang** (Umhang nach dem ersten Winter freigeschaltet).

| Punkt | 06 (Codex) | Nutzer-Entscheidung | Änderung |
|---|---|---|---|
| Schulter | fehlt | eigener Slot | ergänzen (`slot_shoulders`) |
| Ringe | kein Schmuck | 2 × Ring | ergänzen, nur Icons; Verhältnis zu „kein Schmuck-Slot“ ist offen |
| Oberkörper | `slot_torso` | nicht genannt | **offen**, vermutlich vergessen – bis zur Klärung beibehalten |
| Umhang | optional kosmetisch | Slot, Freischaltung nach dem ersten Winter | anpassen; Kälteschutz als offene Frage |
| Haltbarkeit Rüstung | offen | **ja** | übernehmen |

Gute Punkte aus 06, die Claude übernommen hat: ein Oberkörper-Slot statt zwei Lagen, Name `T_Clothing_Atlas`, einfache Umhang-Physik, `fresh`/`worn` als einzige optische Zustände. Abgeglichene Liste: `design/ASSET_LIST_RUESTUNG.md`.

<a id="d32-4"></a>

### 32.4 4. `07_assetentscheidungen_2026-11-12.md` – zwei Abweichungen
- **Tierhaltung:** 07 schreibt „später“. Der Nutzer hatte „ja, einfach gehalten“ gesagt, ohne Zeitpunkt. Eingetragen ist „ja“.
- Die Antworten des Nutzers auf die offenen Fragen in 07 stehen nicht in der Datei, sondern in `docs/01_entscheidungen.md`.

<a id="d32-5"></a>

### 32.5 5. Neue Entscheidungen des Nutzers (eingetragen in `docs/01_entscheidungen.md`)
- Feldbau ab Phase 2.
- Armbrust: Freischaltung über Attribute plus eine Fähigkeit.
- Haltbarkeit auch für Rüstung.
- Slots wie in Punkt 3.
- Es gibt **Jahreszeiten mit Winter**; der erste Winter ist nicht hart.
- Verderben: Fleisch 4 Tage, Obst und Gemüse 2,5 Tage, wenn nicht trocken gelagert; sonstige Lebensmittel 3 Tage.

<a id="d32-6"></a>

### 32.6 6. Offene Fragen an den Nutzer
1. Armbrust ab **T3 oder T4**?
2. **Oberkörper-Slot** – vergessen oder bewusst weggelassen?
3. **Ringe vs. „kein Schmuck-Slot“** – sind Ringe die Ausnahme?
4. **Verderben:** Spieltage? Hält trockene Lagerung unbegrenzt? Verderben auch gekochte Speisen?
5. **Umhang:** auch Kälteschutz im Winter?
6. **Jahreszeiten** sind ein neues System (Wetter, Schnee, Feldbau, Kälte). In welcher Phase? Vorschlag: Phase 2 zusammen mit dem Feldbau, der Slice bleibt ohne Jahreszeiten.


[↑ zum Inhaltsverzeichnis](#inhaltsverzeichnis)


---

<a id="teil-7"></a>

# Teil VII – Assets, Asset-Listen und Prompts

<a id="d33"></a>

## 33. Asset-Plan für ChatGPT (Stand 2026-11-12)

> Quelle: `design/ASSET_PLAN.md`

*Für ChatGPT geschrieben, ergänzt `design/CHATGPT_BRIEFING.md`. Der Nutzer kopiert dieses Dokument in den Chat.*
*Alle Zahlen sind Vorschläge von Claude und gelten, bis der Nutzer sie ändert. Status der Entscheidungen: `docs/01_entscheidungen.md`.*

<a id="d33-1"></a>

### 33.1 1. Zweck
Dieser Plan legt fest, **welche Assets das Spiel zuerst braucht, in welcher Reihenfolge, mit welchen technischen Grenzen und in welchem Format du sie beschreibst.**
Zuerst wird nur der **Vertical Slice (Phase 1, „Siedlung“)** ausgestattet: Spielfigur (Koop), Bauteile, Ressourcen, 3–4 Dorfbewohner-Jobs, 3 Bedürfnisse, ein Lager. Alles andere steht in Abschnitt 8 nur als Ausblick.

<a id="d33-2"></a>

### 33.2 2. Was du lieferst – und was nicht
| Du lieferst | Du lieferst nicht |
|---|---|
| Konzeptblätter pro Asset (Bild + Beschreibung, Maße, Materialien, Farbwerte) | fertige 3D-Modelle, Rigs, Animationen, Unreal-Dateien |
| 2D-UI-Elemente und Icons (Entwurf; Endfassung siehe Abschnitt 7) | Code, Spielwerte, Datentabellen in `data/` |
| Stilregeln: Formensprache, Palette, Materialliste, Abnutzungsgrad | Nachbauten geschützter Designs (Aska, Valheim, KCD, Manor Lords, Medieval Dynasty) |
| Audio-Richtung (Beschreibung, Referenzbegriffe) | Musik/Sounds als fertige Dateien |

**Wichtig:** Woher die fertigen 3D-Modelle kommen, ist **noch nicht entschieden** (siehe Abschnitt 9). Deine Konzeptblätter müssen deshalb so genau sein, dass ein 3D-Artist, ein gekauftes Asset-Pack oder ein Werkzeug danach arbeiten kann.

<a id="d33-3"></a>

### 33.3 3. Voraussetzung
Die **Art Direction ist noch offen** (Briefing, Auftrag 1). Dieser Asset-Plan wird erst nach der Stilwahl des Nutzers bearbeitet. Beschreibe alle Assets im gewählten Stil.

<a id="d33-4"></a>

### 33.4 4. Technische Vorgaben (für jedes Konzeptblatt)

#### 4.1 Maßstab und Raster
- 1 Unreal-Einheit = 1 cm. Echte Größen verwenden. Erwachsene Figur ca. 180 cm.
- **Bauraster: 300 cm.** Wandmodul 300 × 300 cm, Fundament 300 × 300 cm, Dachneigung 45°. Alle Bauteile passen auf dieses Raster (Snapping).
- Türöffnung mindestens 120 × 220 cm (Figuren mit Traglast und Koop-Gedränge).

#### 4.2 Größen- und Polygon-Budgets
Ziel: Das fertige Spiel bleibt **unter ca. 20 GB** und läuft mit **8 GB RAM** auf niedriger Stufe. Lumen/Nanite sind noch nicht entschieden, deshalb gelten klassische Budgets mit LODs.

| Asset-Art | Dreiecke LOD0 (max.) | Texturen (max.) | Hinweis |
|---|---|---|---|
| Spielerfigur inkl. Kleidung | 40.000 | 2048² | modular: Körper, Kopf, Haare, Oberteil, Hose, Schuhe |
| Dorfbewohner | 20.000 | 2048², geteilt mit anderen | viele gleichzeitig im Bild → wenige Teile, Variation über Farbe |
| Bauteil (Wand, Boden, Dach …) | 2.000 | **Trim-Sheet** 2048² pro Materialstufe | keine eigene Textur pro Bauteil |
| Gebäude-Unikat (z. B. Lager) | 15.000 | Trim-Sheet + max. 1 × 1024² | möglichst aus Bauteilen zusammensetzen |
| Baum | 15.000 | 2048² Rinde + Blatt-Atlas | gefällter Stamm und Stumpf als eigene Teile |
| Fels, Busch | 3.000 | 1024² | 3 Größen aus einer Textur |
| Kleines Objekt (Werkzeug, Item) | 1.000 | 512²–1024², Atlas bevorzugt | |
| Terrain-Schicht | – | 2048², kachelbar | höchstens 6 Schichten |
| UI-Icon | – | 256² im Spiel (Entwurf 512²) | gleicher Rahmen und Blickwinkel |

Keine 4K-Texturen. Variation über **Farbe, Kombination und Abnutzung** statt über neue Assets.

#### 4.3 Vorläufige Aufteilung der 20 GB
| Bereich | Budget |
|---|---|
| Texturen | 8 GB |
| Welt- und Kartendaten | 3 GB |
| Meshes | 3 GB |
| Audio (komprimiert) | 2 GB |
| Animationen | 1 GB |
| Materialien, Shader, Effekte | 1 GB |
| Engine, Code, Rest | 1,5 GB |
| Reserve | 0,5 GB |

Claude misst die Paketgröße laufend (`tools/build.ps1 Package`). Heute: 0,96 GB ohne eigene Inhalte.

#### 4.4 Figuren und Animation
- Proportionen kompatibel zum **Unreal-5-Standardskelett** (Mannequin). So lassen sich vorhandene Animationen übertragen, statt jede neu zu erstellen.
- Spieler und Dorfbewohner nutzen **dasselbe Skelett**.
- Ausrüstung später als austauschbare Teile über diesem Skelett (KCD-ähnliche Slots).

#### 4.5 Benennung
Jedes Asset hat eine **stabile ID in `snake_case`** (wird nie wiederverwendet) und einen Unreal-Namen nach Typ:

| Typ | Präfix | Beispiel-ID → Unreal-Name |
|---|---|---|
| Statisches Mesh | `SM_` | `building_wall_wood` → `SM_Building_Wall_Wood` |
| Skelett-Mesh | `SK_` | `char_villager_base` → `SK_Char_Villager_Base` |
| Textur | `T_` + Suffix `_BC` / `_N` / `_ORM` | `T_Trim_Wood_BC` |
| Material / Instanz | `M_` / `MI_` | `MI_Trim_Wood_Weathered` |
| Animation | `A_` | `A_Villager_Chop_Loop` |
| Effekt (Niagara) | `NS_` | `NS_Campfire` |
| Sound | `SW_` / `MS_` (MetaSound) | `SW_Axe_Chop_01` |
| UI-Widget / Icon | `WBP_` / `T_Icon_` | `T_Icon_Res_Wood` |

<a id="d33-5"></a>

### 33.5 5. Asset-Liste Phase 1 (Vertical Slice)
Priorität: **A** = ohne geht der Slice nicht · **B** = wichtig für Lesbarkeit/Stimmung · **C** = schön, notfalls Platzhalter.
Jobs und Bedürfnisse sind Vorschläge; endgültig legt sie der Nutzer fest.

#### 5.1 Figuren
| ID | Beschreibung | Prio |
|---|---|---|
| `char_player_base` | Spielerfigur, modular (2 Körpertypen, 3 Köpfe, 3 Frisuren, 2 Grund-Outfits), Farbe pro Koop-Spieler unterscheidbar | A |
| `char_villager_base` | Dorfbewohner, gleiche Basis, schlichter; Beruf an **Kleidungsfarbe + Werkzeug** erkennbar | A |
| `char_villager_variants` | 6 Kombinationen aus Kopf/Haar/Kleidung, damit 10–20 Bewohner nicht gleich aussehen | B |

#### 5.2 Animationen (Liste für Abschnitt 9 – Quelle offen)
| ID | Beschreibung | Prio |
|---|---|---|
| `anim_locomotion` | Stehen, Gehen, Laufen, Ducken, Springen (ohne Ausdauer-Erschöpfung!) | A |
| `anim_chop` | Holz hacken (Schleife) | A |
| `anim_mine` | Stein schlagen (Schleife) | A |
| `anim_carry` | Tragen (Gehen mit Last) | A |
| `anim_build` | Hämmern/Bauen | A |
| `anim_cook` | Kochen am Feuer | B |
| `anim_eat` / `anim_sleep` / `anim_sit` | Bedürfnisse erfüllen | B |
| `anim_idle_variants` | 2–3 Leerlauf-Varianten | C |

#### 5.3 Baukasten (Materialstufe Holz)
| ID | Beschreibung | Prio |
|---|---|---|
| `building_foundation_wood` | Fundament 300 × 300 | A |
| `building_floor_wood` | Boden 300 × 300 | A |
| `building_wall_wood` | Wand 300 × 300 | A |
| `building_wall_door_wood` | Wand mit Türöffnung | A |
| `building_wall_window_wood` | Wand mit Fenster | B |
| `building_door_wood` | Tür (öffnet) | A |
| `building_pillar_wood` / `building_beam_wood` | Pfosten, Balken | A |
| `building_roof_wood` / `building_roof_ridge_wood` / `building_roof_gable_wood` | Dachfläche, First, Giebel | A |
| `building_stairs_wood` | Treppe | B |
| `building_fence_wood` / `building_gate_wood` | Zaun, Tor | C |
| `trim_wood` | **Trim-Sheet** Holz (neu, verwittert) für alle Bauteile | A |

#### 5.4 Arbeitsstätten und Siedlung
| ID | Beschreibung | Prio |
|---|---|---|
| `building_storage` | **Lager** der Siedlung (ein Lager pro Siedlung), gefüllt/leer sichtbar unterscheidbar | A |
| `building_woodcutter` | Holzfäller-Platz (Hackklotz, Holzstapel) | A |
| `building_stonecutter` | Steinmetz-Platz | A |
| `building_campfire` | Lagerfeuer = Kochstelle im Slice | A |
| `building_bed` | Schlafplatz (Strohlager) | A |
| `building_workbench` | Werkbank für den Spieler | B |
| `building_totem` | Siedlungszentrum/Markierung (Anführer-Symbol, eigenes Design!) | C |

#### 5.5 Ressourcen und Gegenstände
| ID | Beschreibung | Prio |
|---|---|---|
| `res_tree_pine`, `res_tree_birch` | 2 Baumarten, je stehend / gefällt / Stumpf | A |
| `res_rock_small/medium/large` | Felsen zum Abbauen, sichtbar abgebaut | A |
| `res_berry_bush` | Beerenstrauch (voll/leer) | B |
| `item_log`, `item_stone`, `item_plank`, `item_berries`, `item_meal` | Gegenstände im Lager und beim Tragen | A |
| `tool_axe`, `tool_pickaxe`, `tool_hammer` | Werkzeuge (Spieler und Dorfbewohner) | A |

#### 5.6 Umgebung
| ID | Beschreibung | Prio |
|---|---|---|
| `terrain_grass`, `terrain_dirt`, `terrain_rock`, `terrain_path` | Terrain-Schichten | A |
| `foliage_grass`, `foliage_bush`, `foliage_flowers` | Bodenbewuchs, günstig | B |
| `env_sky_mood` | Licht-/Himmelsstimmung (Beschreibung; Umsetzung in der Engine) | B |

#### 5.7 Effekte
| ID | Beschreibung | Prio |
|---|---|---|
| `vfx_campfire` | Feuer + Rauch | A |
| `vfx_chop_chips`, `vfx_rock_dust` | Treffer-Effekte | B |
| `vfx_build_place` | Effekt beim Platzieren eines Bauteils | C |

#### 5.8 UI (Lesbarkeit hat Vorrang)
| ID | Beschreibung | Prio |
|---|---|---|
| `ui_icons_resources` | Icons für alle Items aus 5.5 | A |
| `ui_icons_jobs` | Icons für die 3–4 Jobs (Vorschlag: Holzfäller, Steinmetz, Koch, Träger) | A |
| `ui_icons_needs` | Icons für 3 Bedürfnisse (Vorschlag: Hunger, Schlaf, Wärme) mit 3 Zuständen (gut/mittel/kritisch) | A |
| `ui_hud` | HUD: Gesundheit, Schnellleiste, Koop-Spielerliste (bis 8 Namen) – **kein Ausdauerbalken** | A |
| `ui_build_menu` | Baumenü mit Kategorien | A |
| `ui_storage` | Lageransicht: Bestand auf einen Blick, wenige Klicks | A |
| `ui_villager_panel` | Dorfbewohner-Übersicht: Job, Bedürfnisse, Priorität ändern | A |
| `ui_world_marker` | Markierung über Dorfbewohnern (Job + kritisches Bedürfnis) | B |
| `ui_font` | Schrift (lizenzfrei für Spiele, z. B. SIL Open Font License) | A |

#### 5.9 Audio (Richtung, keine Dateien)
| ID | Beschreibung | Prio |
|---|---|---|
| `sfx_footsteps` | Schritte auf Gras, Erde, Holz | B |
| `sfx_chop`, `sfx_mine`, `sfx_build` | Arbeitsgeräusche | B |
| `sfx_fire`, `amb_settlement`, `amb_forest` | Feuer, Siedlung, Wald | B |
| `sfx_ui` | Klick, Bestätigen, Fehler | B |
| `music_settlement` | ruhige Siedlungsmusik (kein Zeitdruck!) | C |

<a id="d33-6"></a>

### 33.6 6. Format deiner Lieferung
Pro Asset (oder Gruppe) **ein Abschnitt** in einer Markdown-Datei `NN_assets_<gruppe>.md` (z. B. `03_assets_baukasten.md`) mit:

| Feld | Inhalt |
|---|---|
| ID | aus Abschnitt 5 (neue Assets: neue ID vorschlagen, `snake_case`) |
| Zweck | wofür im Spiel, wer sieht es wie oft |
| Konzept | Bild (PNG) oder genaue Beschreibung, Vorder- und Seitenansicht bei Figuren und Gebäuden |
| Maße | in cm, passend zum 300-cm-Raster |
| Materialien | welche Trim-Sheets / Atlanten, Farbwerte (Hex) |
| Zustände | z. B. voll/leer, neu/verwittert, stehend/gefällt |
| MVP / Ausbau | was zuerst reicht, was später dazukommt |
| Budget-Check | geschätzte Dreiecke und Texturgröße laut 4.2 |
| KI-Hinweis | ob Bild/Text KI-generiert ist, als Eintrag: Datei / Werkzeug / Datum / Lizenz oder Quelle |
| Prompt-Paket | **Pflicht, sobald ein Asset per KI erzeugt werden soll:** vollständiges Paket nach `design/PROMPT_STANDARD.md` (alle Pflichtfelder, feste Blöcke S1/L1/H1/N1 wörtlich, Abnahme-Kriterien). Beispiele in `design/prompts/` |

Am Ende jeder Datei: **„Entscheidungen, die der Nutzer treffen muss“**.

<a id="d33-7"></a>

### 33.7 7. Regeln
- **Keine Nachahmung** von Figuren, Gebäuden, Logos oder Optik der Vorbilder. Prinzipien ja, Aussehen nein.
- **Keine Ausdauerbalken** und keine Countdown-Elemente in der UI.
- **Alles KI-Generierte** wird im Asset-Log eingetragen (Steam verlangt Angaben). KI-Bilder sind zunächst **Konzepte**; ob sie ins fertige Spiel dürfen (z. B. Icons), entscheidet der Nutzer.
- Fremde Assets (Packs, Schriften, Sounds) nur mit **Quelle und Lizenz**; ohne Lizenzangabe kein Einsatz.
- **Umfang bremsen:** Wenn ein Asset das Budget sprengt oder für den Slice nicht nötig ist, sag es und schlage eine kleinere Variante vor.

<a id="d33-8"></a>

### 33.8 8. Ausblick (noch nicht bearbeiten)
Vorbereitete Detail-Listen: Waffen mit Tier-Stufen in `design/ASSET_LIST_WAFFEN.md`, Gegenstände, Werkzeuge und Behälter in `design/ASSET_LIST_GEGENSTAENDE.md`, Rüstung und Kleidung in `design/ASSET_LIST_RUESTUNG.md`.

| Phase | Assets |
|---|---|
| 2 Siedlung ausbauen | Materialstufe Stein, ca. 10 Jobs und deren Arbeitsstätten, Kommandoansicht-UI, Miliz-Ausrüstung |
| 3 Kampf/Ausrüstung | Waffen, Rüstungs-Slots mit Stufen, Schmiede, Skilltree-UI, erste Gegner, Kampf-Animationen (Parieren, Haltungsleiste statt Ausdauer) |
| 4 Welt | weitere Biome (Terrain, Vegetation, Stimmung), Bosse, Quest-Figuren |
| 5 Spätspiel | Portale, Angriffswellen-Gegner, Weltoptionen-Menü |

<a id="d33-9"></a>

### 33.9 9. Offene Entscheidungen (Nutzer)
1. **Art Direction** (Auftrag 1 im Briefing) – Voraussetzung für alles hier.
2. **Herkunft der 3D-Assets:** gekaufte/freie Packs (z. B. Fab; Lizenz je Asset prüfen) · eigene Modelle in Blender · Freelancer · KI-3D-Werkzeuge (Qualität und Lizenz unsicher, Steam-Angabe nötig) · Mischung.
3. **Herkunft der Animationen:** vorhandene Unreal-Animationen übertragen · gekaufte Packs · Motion Capture · selbst.
4. **Jobs und Bedürfnisse des Slice:** Vorschlag Holzfäller, Steinmetz, Koch, Träger / Hunger, Schlaf, Wärme.
5. **Lumen/Nanite** ja oder nein (beeinflusst Budgets, siehe `docs/04_technische_ziele.md`).
6. Ob KI-generierte 2D-Inhalte (Icons, Texturen) **ins fertige Spiel** dürfen.

<a id="d33-10"></a>

### 33.10 Erster Auftrag an ChatGPT (nach der Stilwahl)
Liefere **Abschnitt 5.3 (Baukasten Holz)** und **5.4 (Arbeitsstätten)** im Format aus Abschnitt 6, zuerst nur Priorität A.
Danach anhalten und auf Rückmeldung warten. Stelle vorab höchstens drei Rückfragen.


[↑ zum Inhaltsverzeichnis](#inhaltsverzeichnis)

<a id="d34"></a>

## 34. Gesamtliste: Gegenstände, Assets und Inhalte (Stand 2026-11-13)

> Quelle: `design/ASSET_LIST_GESAMT.md`

*Sammelliste über das ganze Spiel. Für den Nutzer, ChatGPT (Design) und Claude (Umsetzung).*
*Details stehen in den Einzellisten; diese Liste verweist darauf, statt sie zu wiederholen:*
`design/ASSET_PLAN.md` (Phase-1-Slice) · `design/ASSET_LIST_GEGENSTAENDE.md` · `design/ASSET_LIST_WAFFEN.md` · `design/ASSET_LIST_RUESTUNG.md` · Werte: `game/Content/Data/*.json`

<a id="d34-1"></a>

### 34.1 0. Legende
**Status:** ✔ bestätigt (steht in `docs/01_entscheidungen.md`) · ◐ im Design-Register vorgeschlagen, noch nicht bestätigt · ○ neuer Vorschlag aus dieser Liste
**Phase:** ab wann gebraucht (1 Vertical Slice · 2 Siedlung · 3 Kampf · 4 Welt · 5 Spätspiel)
**Art:** M = 3D-Modell · F = Figur mit Skelett · A = Animation · I = nur Icon · V = Effekt · S = Ton · U = Oberfläche
**Herkunft (Plan):** Baukasten per Blender-Skript · organische Objekte per KI-3D + Blender · Texturen/Icons per KI-Bild (vom Nutzer geprüft) · Figuren und Bosse: offen (KI, gekaufte Basis oder Freelancer). Alles KI-Erzeugte ins `design/asset_log.md`.

**ID-Präfixe** (bestehend): `item_` `tool_` `weapon_` `shield_` `ammo_` `cloth_` `res_` `building_` `prop_` `char_` `anim_` `vfx_` `ui_` `terrain_` `foliage_` `biome_` `poi_` `animal_` `job_`
**ID-Präfixe** (○ neu vorgeschlagen): `npc_` (Personen mit Rolle) · `enemy_` (menschliche Gegner) · `creature_` (Sagenwesen) · `boss_` · `deco_` (Möbel, Schmuck der Siedlung) · `sfx_` (Geräusch) · `amb_` (Umgebungston) · `music_` · `decal_` · `mat_` (Master-Material)

<a id="d34-2"></a>

### 34.2 1. Überblick: Mengen (grobe Schätzung)
| Bereich | Phase 1 | bis Phase 3 | ganzes Spiel |
|---|--:|--:|--:|
| Spieler- und Bewohner-Figuren (Teile) | 2 Grundkörper | ca. 30 Teile | ca. 60 Teile |
| Tiere (wild und Nutztiere) | 0 | 6 | ca. 16 |
| Menschliche Gegner (Ausrüstungs-Sätze) | 0 | 8 | ca. 20 |
| Bosse | 0 | 1–2 | 6 (Sagen-Fassung: +5) |
| Biome (Landschaftssätze) | 1 Testgelände | 2 | 6 |
| Ressourcen-Vorkommen | 6 | ca. 20 | ca. 40 |
| Orte (POIs) | 0 | 6 | ca. 20 |
| Bauteile und Gebäude | 16 | ca. 60 | ca. 120 |
| Möbel, Requisiten, Deko | 4 | ca. 40 | ca. 90 |
| Gegenstände (Icons) | 8 | ca. 60 | ca. 110 |
| Waffen und Rüstung | 0 | ca. 30 Teile | ca. 70 Teile |
| Animationen | ca. 12 | ca. 90 | ca. 200 |
| Effekte | 3 | ca. 25 | ca. 50 |
| Geräusche und Musik | ca. 20 | ca. 150 | ca. 400 |
| Oberflächen (Bildschirme, Fenster) | 4 | ca. 15 | ca. 25 |
| Texturen (Material-, Deko-, Terrain-, UI-Texturen) | ca. 10 | ca. 80 | ca. 200 |
| Deko-Kleinteile (Schnitzwerk, Schmuck, Streudeko) | 0 | ca. 40 | ca. 120 |

Der teuerste Teil sind **Figuren, Tiere und Bosse** (Skelett, Animation). Der umfangreichste Teil sind **Icons und Geräusche**.

<a id="d34-3"></a>

### 34.3 2. Figuren
#### 2.1 Spielerfigur ✔ (Phase 1 Platzhalter, Phase 2 echt)
| ID | Was | Art | Status |
|---|---|:-:|:-:|
| `char_player_base` | 2 Körpertypen auf dem UE5-Standardskelett | F | ✔ |
| `char_head_*` | 3–6 Köpfe | M | ✔ (3) / ○ (mehr) |
| `char_hair_*`, `char_beard_*` | 3–6 Frisuren, 4 Bärte, Zöpfe | M | ✔ / ○ |
| `char_skin_*` | Hauttöne als Materialvarianten | – | ○ |
| `char_paint_*` | Gesichtsbemalung, Narben, Tätowierungen (Charakter-Editor) | Textur | ○ |
| Kleidung, Rüstung | siehe `ASSET_LIST_RUESTUNG.md` (Slots ✔) | M | ✔ |

#### 2.2 Dorfbewohner ✔
| ID | Was | Status |
|---|---|:-:|
| `char_villager_base` | gleiche Basis wie Spieler, schlichter | ✔ |
| `char_villager_variants` | 6+ Kombinationen Kopf/Haar/Kleidung | ✔ |
| Berufskleidung | Beruf erkennbar an **Farbe + Werkzeug** (Schürze Schmied, Fellumhang Jäger …) | ✔ |
| `char_elder` | ältere Bewohner (graue Haare, gebeugte Haltung), später als günstige Variante | ✔ |
| Kinder | **keine im ersten Wurf** | ✔ |

#### 2.3 Personen mit besonderer Rolle (`npc_`)
| ID | Wer | Wo | Status |
|---|---|---|:-:|
| `npc_coastal_merchant` | Küstenkaufmann (kommt mit Schiff, kauft saisonal) | Handelsposten, Fischerdorf | ◐ |
| `npc_blackmarket_trader` | Schwarzmarkt-Händler, nur nachts, Zeichen am Wegstein | einsam im Wald | ✔ (Händler bestätigt) |
| `npc_fishing_villager` | Bewohner des Fischerdorfs (Quests, Ruf) | Küste | ◐ |
| `npc_lawspeaker` | Gesetzessprecher beim Thing | Thing-Platz | ○ |
| `npc_skald` | Skalde, trägt die Saga vor (Feste, Saga-Einträge) | Siedlung | ○ |
| `npc_seeress` | Seherin (Hinweise auf Questlinien, Weltoption Sagenwelt) | Opferstein | ○ |
| `npc_prisoner` | Gefangene in Banditenlagern → werden Bewohner | Banditenlager | ◐ |
| `npc_mercenary` | anheuerbare Söldner (Ausbau) | überall | ◐ |

<a id="d34-4"></a>

### 34.4 3. Tiere (`animal_`)
| ID | Tier | Verhalten | liefert | Biome | Status |
|---|---|---|---|---|:-:|
| `animal_boar` | Wildschwein | greift an, wenn bedrängt | Fleisch, Haut, Hauer | Wiesen | ○ (Grundlage für Boss 1) |
| `animal_deer` | Rothirsch | flieht | Fleisch, Haut, Geweih | Wiesen, Heide | ○ |
| `animal_hare` | Hase | flieht schnell, Fallen | Fleisch, kleines Fell | Wiesen, Heide | ○ |
| `animal_fox` | Rotfuchs | scheu, stiehlt Eier | Fell | Wiesen | ○ |
| `animal_wolf` | Wolf | Rudel, umkreist zuerst | Fell | Wiesen, Heide, Fjell | ◐ |
| `animal_bear` | Braunbär | verteidigt seine Höhle | Fleisch, großes Fell, Fett | Heide, Fjell | ◐ |
| `animal_elk` | Elch | friedlich, gefährlich wenn bedrängt | Fleisch, Haut, Geweih | Heide | ◐ |
| `animal_reindeer` | Rentier | Herden, scheu | Fleisch, Fell, Geweih | Fjell, Eisrand | ◐ |
| `animal_lynx` | Luchs | schleicht, greift Einzelne an | Fell | Fjell | ◐ |
| `animal_seal` | Robbe | flieht ins Wasser | Fleisch, Tran, Fell | Marsch, Küste | ◐ |
| `animal_seabird` | Seevögel | Vogelfelsen | Eier, Federn | Küste | ◐ |
| `animal_arctic_fox` | Polarfuchs | scheu | Fell | Eisrand | ◐ |
| `animal_raven` | Raben | Umgebung, kündigen Ereignisse an (Saga) | – | alle | ○ |
| `animal_fish_*` | Lachs, Hering, Dorsch (Angeln, Reusen) | Schwärme | Fisch | Bäche, Küste | ○ |
| `animal_eagle` | Seeadler | Umgebung, Vogelfelsen | Federn (selten) | Küste, Fjell | ○ |
| Nutztiere | Huhn, Schaf, Ziege, Rind | in der Siedlung, einfach gehalten | Eier, Wolle, Milch, Leder | Siedlung | ✔ |
| `animal_dog` | Hofhund (warnt, begleitet Jäger) | Siedlung | – | Siedlung | ○ |
| `animal_horse` | Pferd oder Ochse als Zugtier für Karren, gesteuert von Bewohnern (später); **kein Reiten im ersten Wurf** ✔ | Siedlung | Zugkraft | Siedlung | ○ |

Jedes Tier braucht mindestens: Modell, Skelett, 6–10 Animationen (Stehen, Gehen, Laufen, Angriff/Flucht, Treffer, Tod, Fressen), 3–5 Geräusche, Kadaver-Zustand.

<a id="d34-5"></a>

### 34.5 4. Menschliche Gegner (`enemy_`) ◐
**Gruppen** (Register): Waldbanditen (Wiesen, Heide; halten Gefangene) · Strandplünderer (Marsch, Küste; kommen bei Ebbe/übers Wasser) · Eidbrecher (Heide, Fjell; gute Ausrüstung, Formation) · Aschebund-Kult (Glutküste; Fackeln, Brandpfeile) · Söldner (überall).
**Rollen** (Register): Plänkler (Sax) · Schildträger (Rundschild, Speer; Schildwand) · Speerträger · Schläger (Breitaxt).
○ Ergänzt: Bogenschütze · Anführer/Hauptmann (stärker, eigene Optik) · Fackelträger (Aschebund, setzt Gebäude in Brand – nur mit Weltoption Angriffe).

Assets je Gruppe: 1 Kleidungs-/Rüstungssatz in Gruppenfarbe (aus den Rüstungs-Teilen kombiniert), Banner/Erkennungszeichen, 1 Lager-Set (Zelte, Feuerstelle, Palisade, Beutekiste), 5–8 Ruf-/Kampfrufe.

<a id="d34-6"></a>

### 34.6 5. Sagenwesen (`creature_`) – zurückgestellt
**Entschieden am 2026-11-13:** zuerst nur Menschen und Tiere, Bosse in der realistischen Fassung. Sagenwesen höchstens später als Weltoption „Sagenwelt“. Die Tabelle bleibt als Vorrat für diesen Fall.
| ID | Wesen | Biom | Rolle |
|---|---|---|---|
| `creature_draugr` | Draugr (Untote aus Grabhügeln) | Hügelgrab-Heide | Gegner, nachts aus Grabhügeln |
| `creature_will_o_wisp` | Irrlichter | Gezeitenmarsch | führen in die Irre, Boss-Begleiter |
| `creature_troll` | Troll | Basaltfjell | schwerer Gegner, Boss-Fassung |
| `creature_frost_giant` | Frostriese | Eisrand | Boss-Fassung |
| `creature_lindworm` | Lindwurm | Glutküste | Boss-Fassung |

Aufwand: verdoppelt die Boss-Arbeit. Gemeinfreie Sagenfiguren, aber eigenes Aussehen (keine Anlehnung an bekannte Spiele).

<a id="d34-7"></a>

### 34.7 6. Bosse (`boss_`) ◐ – ein Boss pro Biom
Gebaut wird zuerst die **realistische Fassung** ✔ (2026-11-13); die Sagen-Fassung nur, falls die Weltoption „Sagenwelt“ später kommt.
| ID | Boss | Biom | Realistische Fassung | Sagen-Fassung | Belohnung |
|---|---|---|---|---|---|
| `boss_old_boar` | Der Alte Keiler | Fjordwiesen | riesiger, narbiger Keiler | (immer realistisch) | Hauer → Bauplan Rennofen (T2) |
| `boss_mud_mother` | Die Schlickmutter | Gezeitenmarsch | Plündererin auf einem Wrack | Moorhexe mit Irrlichtern | Bossmaterial → T3 |
| `boss_barrow_king` | Der Hügelkönig | Hügelgrab-Heide | Eidbrecher-Jarl in Grabfestung | Draugr-Fürst | → T4 |
| `boss_rock_father` | Der Felsenvater | Basaltfjell | Bärenkönig in Eishöhle | Troll | → T5 |
| `boss_ash_tongue` | Die Aschezunge | Glutküste | Hohepriester des Aschebunds | Lindwurm | Relikt-Material (T6) |
| `boss_storm_colossus` | Der Sturmkoloss | Eisrand | Seekönig der Plünderer mit Flotte | Frostriese | Endsieg, Saga-Finale |

Je Boss: Figur, 15–25 Animationen (3 Phasen), Arena (`poi_boss_arena`, fester Ort), Leibwache oder Begleiter, Trophäe (`item_trophy_boss_*`, platzierbar ✔), Bossmaterial-Icon, eigene Musik, Phasen-Effekte (Staubwolke, Flut, Nebel, Lawine, Feuerwalze, Eisbruch), Saga-Bild.

<a id="d34-8"></a>

### 34.8 7. Landschaften (`biome_`) ◐
| ID | Biom | Stufe | Stimmung | Typische Assets |
|---|---|:-:|---|---|
| `biome_fjord_meadow` | Fjordwiesen und Birkenhaine | T1 | hell, Bäche, Startfjord | Birke, Kiefer, Erle, Wiesenblumen, Bachkiesel, Findlinge, Fjordufer |
| `biome_tidal_marsh` | Gezeitenmarsch | T2 | Watt, Priele, Morgennebel | Schilf, Weiden, Seegras, Schlick, Priele, Wracks, Pfahlstege, Torfstiche |
| `biome_barrow_heath` | Hügelgrab-Heide | T3 | Heide, Steinsetzungen | Heidekraut, Wacholder, krumme Kiefern, Grabhügel, Schiffssetzungen aus Stein, Bautasteine |
| `biome_basalt_fell` | Basaltfjell | T4 | Säulenfelsen, Wasserfälle, Schnee | Basaltsäulen, Fichten, Zwergbirken, Moos, Flechten, Wasserfälle, Eishöhlen, Schneefelder |
| `biome_ember_coast` | Glutküste | T5 | Lavafelder, schwarzer Sand | Lavagestein, verkohlte Stämme, Farne an Quellen, Dampfspalten, heiße Quellen, Obsidian |
| `biome_ice_rim` | Eisrand | T6 | Gletscher, Sturm | Gletschereis, Eisbrocken, Schneewehen, gefrorene Wracks, Eishöhlen |

Je Biom: 4–6 Terrain-Schichten (`terrain_*`), 3–5 Bäume/Großpflanzen mit LOD und Fernansicht, 6–10 Bodenbewuchs (`foliage_*`), 5–8 Felsen, 1 Wasser-Material, Himmels- und Lichtstimmung (`env_sky_*`), Wetter, Umgebungston, Musik.

<a id="d34-9"></a>

### 34.9 8. Ressourcen-Vorkommen (`res_`)
| ID | Vorkommen | liefert | Biom | Status |
|---|---|---|---|:-:|
| `res_tree_pine`, `res_tree_birch` | Kiefer, Birke (stehend/gefällt/Stumpf) | Stämme | Wiesen | ✔ Slice |
| `res_tree_alder`, `res_tree_spruce`, `res_tree_oak` | Erle, Fichte, Eiche (Eiche = hartes Holz ab T2) | Stämme, Rinde | je Biom | ○ |
| `res_rock_small/medium/large` | Felsen | Stein | alle | ✔ Slice |
| `res_flint` | Feuersteinknollen am Bach | Feuerstein | Wiesen | ○ |
| `res_berry_bush` | Beerenstrauch (voll/leer) | Beeren | Wiesen | ✔ Slice |
| `res_tree_apple` | Wildapfelbaum (Holzapfel): knorrig, kleine Krone, kleine gelb-rote Äpfel sichtbar am Baum und im Gras darunter; Zustände voll / abgeerntet / Winter kahl | Wildäpfel | Wiesen, Waldrand | ✔ (2026-11-13) |
| `res_berry_bush` (Ausbau) | Beerensträucher als echte Arten: Himbeere, Heidelbeere (niedrig), Preiselbeere (Heide), Moltebeere (Moor) – je eigenes Modell oder Variante | Beeren | je Biom | ✔ / ○ Arten |
| `res_mushroom_patch` | Pilzstelle: Steinpilze, Pfifferlinge an Baumstümpfen und im Moos; Fliegenpilz nur als giftige Deko | Pilze | Wald | ✔ (2026-11-13) |
| `res_hazel_bush` | Haselstrauch: vielstämmig, Nüsse in Büscheln sichtbar | Haselnüsse | Waldrand | ✔ (2026-11-13) |
| `res_herbs` | Kräuter (Schafgarbe, Johanniskraut, Bärlauch im Frühling) für Heilung und Färben | Kräuter | Wald, Heide | ○ |
| `res_loose_stone`, `res_fallen_branch`, `res_fiber_plant` | lose Steine, herabgefallene Äste, Faserpflanze (Brennnessel) – Spielstart, von Hand | Steine, Äste, Fasern | alle | ✔ (2026-11-13) |
| `res_reeds` | Schilf | Stroh/Schilf (Dach) | Marsch | ◐ |
| `res_peat` | Torfstich | Torf (Brennstoff) | Marsch, Heide | ◐ |
| `res_bog_ore` | Raseneisen im Moor | Raseneisenerz | Marsch | ◐ |
| `res_clay_pit` | Tongrube | Ton | Bachufer | ○ |
| `res_mussel_bank` | Muschelbank (nur bei Ebbe) | Muscheln | Marsch | ◐ |
| `res_driftwood` | Treibholz | Holz, selten Fundstücke | Küste | ○ |
| `res_amber` | Bernstein am Strand | Bernstein | Küste | ○ |
| `res_iron_vein` | Eisenerz-Ader | Eisenerz | Fjell | ◐ |
| `res_crystal` | Kristalle | Kristall (Schmuck, Relikte) | Fjell | ◐ |
| `res_sulfur`, `res_obsidian` | Schwefel, Obsidian | Flussmittel für Damast | Glutküste | ◐ |
| `res_relic_ice` | eingefrorene Relikte, alte Wracks | Relikt-Material, Eisen | Eisrand | ◐ |
| `res_flax_wild`, `res_wild_grain` | wilder Flachs, Wildgerste (Saatgut-Quelle) | Saatgut | Wiesen | ○ |
| `res_beehive_wild` | wildes Bienennest | Honig, Wachs | Wald | ○ |
| `res_fish_spot` | Fischgrund (Angeln, Reuse) | Fisch | Bach, Küste | ○ |

Jedes Vorkommen: Zustände voll/abgeerntet/Rest, Treffer-Effekt, 2–3 Geräusche, Icon des Ertrags.

<a id="d34-10"></a>

### 34.10 9. Orte (`poi_`) ◐
Langhaus-Ruine · Grabhügel (kleiner Dungeon) · Banditenlager · verlassener Hof · Fischerdorf · Handelsposten · Opferstein · Höhle/Bärenhöhle · Turmruine (deckt Karte auf) · Schiffswrack (nur bei Ebbe) · heiße Quelle · alte Mine · Schwarzmarkt-Zeichen · Boss-Arena.
○ Ergänzt: Runensteinfeld (Saga-Fragmente) · Opferhain mit heiligem Baum · Furt und Holzbrücke · Seezeichen/Leuchtfeuer · Thingplatz einer fremden Siedlung · Köhlerlichtung (verlassen) · Fallgrube/Jagdstand · Robbenfelsen · Eishöhle mit gefrorenem Schiff.
Je Ort: eigenes Modul-Set oder Kombination aus Baukasten, Beutebehälter, 1–2 besondere Requisiten, Kartensymbol.

<a id="d34-11"></a>

### 34.11 10. Siedlung: Baukasten (`building_`)
| Satz | Inhalt | Phase | Status |
|---|---|:-:|:-:|
| Holz (T1) | Fundament, Boden, Wand, Wand mit Tür/Fenster, Tür, Pfosten, Balken, Dach, First, Giebel, Treppe, Zaun, Tor | 1 | ✔ (10 Modelle in `Assets/`) |
| Flechtwerk und Lehm | Wand, Wand mit Tür, Ausbesserung | 2 | ○ |
| Grassoden (Turf) | Dachfläche, First, Kante (Turf-Kante ✔ beim Holzdach) | 2 | ✔ / ○ |
| Stein (Materialstufe 2) | Steinfundament, Trockenmauer, Herd, Stufen | 2 | ◐ |
| Palisade | Pfahlwand, Tor, Wehrgang, Wachturm | 2 | ○ |
| Hafen | Steg, Pfahl, Bootsrampe, Bootshaus | 4 | ○ |
| Langhaus-Teile | geschwungene Wand, Langdach, Firstschmuck, Türrahmen mit Schnitzwerk | 2 | ○ |

<a id="d34-12"></a>

### 34.12 11. Siedlung: Gebäude und Arbeitsstätten
| ID | Gebäude | Beruf (`job_`) | Phase | Status |
|---|---|---|:-:|:-:|
| `building_storage` | Lager (eins pro Siedlung, voll/leer sichtbar) | Träger | 1 | ✔ |
| `building_woodcutter` | Holzfällerplatz | Holzfäller | 1 | ✔ |
| `building_stonecutter` | Steinmetzplatz | Steinmetz | 1 | ✔ |
| `building_campfire` | Lagerfeuer = Kochstelle | Koch | 1 | ✔ |
| `building_bed` | Strohlager | – | 1 | ✔ |
| `building_workbench` | Werkbank | Spieler | 1 | ✔ |
| `building_totem` | Siedlungszentrum (Anführer-Symbol) | – | 1 | ✔ |
| `building_sawpit` | Sägegrube | Säger | 2 | ◐ |
| `building_forager_hut` | Sammlerhütte | Sammler | 2 | ◐ |
| `building_builder_yard` | Bauhof | Baumeister | 2 | ◐ |
| `building_hunter_hut` | Jägerhütte, Trockengestell | Jäger | 2 | ◐ |
| `building_fisher_hut` | Fischerhütte, Netze, Reusen | Fischer | 2 | ◐ |
| `building_field` | Feld (Gerste, Flachs ✔), Saat-/Wachs-/Erntezustand | Bauer | 2 | ✔ |
| `building_coop`, `building_pen`, `building_byre` | Hühnerstall, Pferch, Stall | Hirte | 2–3 | ✔ |
| `building_mill` | Handmühle, Backofen | Müller/Bäcker | 3 | ◐ |
| `building_tannery` | Gerberei (Bottiche, Gestelle) | Gerber | 2 | ◐ |
| `building_weaver` | Webhaus (Webstuhl, Spinnrocken) | Weber | 3 | ◐ |
| `building_healer` | Heilerhütte (Kräuterbündel, Liegen) | Heiler | 3 | ◐ |
| `building_guardhouse` | Wachhaus, Waffenständer | Wache/Miliz | 2 | ◐ |
| `building_charcoal_kiln` | Kohlenmeiler | Köhler | 3 | ◐ |
| `building_bloomery` | Rennofen (Bauplan vom Alten Keiler) | Schmelzer | 3 | ◐ |
| `building_smithy` | Schmiede (Esse, Amboss, Blasebalg, Löschtrog), 3 Ausbaustufen | Schmied | 3 | ◐ |
| `building_longhouse` | Langhaus (Wohnen, Feste) | – | 2 | ○ |
| `building_pit_house` | Grubenhaus (einfache Werkstatt/Wohnung) | – | 2 | ○ |
| `building_smokehouse` | Räucherhaus (Fleisch, Fisch haltbar) | Koch | 2 | ○ |
| `building_well` | Brunnen | – | 2 | ○ |
| `building_bathhouse` | Badehaus (Zufriedenheit) | – | 3 | ○ |
| `building_brewery` | Brauhaus (Met, Bier) | Brauer | 3 | ○ |
| `building_apiary` | Bienenkörbe | Imker | 3 | ○ |
| `building_pottery` | Töpferei, Brennofen | Töpfer | 3 | ○ |
| `building_thing_site` | Thingplatz (Steinkreis, Sitzsteine) | – | 2 | ◐ |
| `building_jarl_hall` | Jarlssitz (Belohnung Saga-Finale) | – | 5 | ◐ |
| `building_runestone` | Runenstein, Erinnerungsstein (Prestige) | – | 4 | ◐ |
| `building_portal` | Portal (Spätspiel, verbindet Lager) | – | 5 | ✔ |
| `building_shipyard` | Bootsbauplatz | Bootsbauer | 4 | ○ offen |

<a id="d34-13"></a>

### 34.13 12. Möbel, Requisiten, Deko (`prop_`, `deco_`)
- **Behälter ✔:** Sack, Korb, Fass, Kiste, Bündel für abgelegte Gegenstände, Trockengestell.
- **Werkstatt-Requisiten:** Hackklotz, Sägebock, Steinmetz-Werkbank, Amboss, Esse, Blasebalg, Löschtrog, Gerbbottich, Spannrahmen, Webstuhl, Spinnrocken, Mahlstein, Backofen, Kessel mit Galgen ✔, Bratspieß, Netz, Reuse, Fischgestell, Bienenkorb, Töpferscheibe.
- **Wohnen:** Bettgestell, Strohlager ✔, Bank, Tisch, Hocker, Truhe (Deko – Lagerung bleibt im Lager ✔), Herdstelle im Langhaus, Kochgeschirr, Trinkhorn, Tranlampe, Fackelhalter, Fellteppich, Wandbehang, Webteppich, Kräuterbündel, Schilde an der Wand.
- **Siedlung:** Wegsteine, Zaunarten, Brennholzstapel, Holzstapel ✔ (Füllstand), Steinhaufen, Erntegarben, Heuhaufen, Wassertrog, Futterkrippe, Vogelscheuche, Banner der Siedlung (Lager-Editor ✔), Festtafel, Feuerkorb, Opferschale.
- **Trophäen ✔:** je Boss eine platzierbare Trophäe, dazu Tierfelle und Geweihe als Wanddeko.

<a id="d34-14"></a>

### 34.14 13. Gegenstände
Vollständig in `ASSET_LIST_GEGENSTAENDE.md` (Rohstoffe, Zwischenprodukte, Nahrung, Werkzeuge, Verbrauchsgüter, Handel, Behälter). Ergänzungen aus den Biomen (○ bzw. ◐ aus dem Register):
| ID | Gegenstand | Herkunft | Status |
|---|---|---|:-:|
| `item_peat` | Torf (Brennstoff, Köhler) | Marsch, Heide | ◐ |
| `item_mussels` | Muscheln (Nahrung, Köder) | Marsch | ◐ |
| `item_herbs` | Kräuter (Salben, Färben) | Heide | ◐ |
| `item_fur`, `item_fur_large` | Fell, großes Fell | Wolf, Bär, Luchs … | ◐ |
| `item_fat`, `item_train_oil` | Fett, Tran (Lampen, Leder) | Bär, Robbe | ◐ |
| `item_tusk_boar` | Hauer (Bauplan Rennofen) | Alter Keiler | ◐ |
| `item_crystal` | Kristall | Fjell | ◐ |
| `item_sulfur`, `item_obsidian`, `item_flux` | Schwefel, Obsidian, Flussmittel | Glutküste | ◐ |
| `item_relic_material` | Relikt-Material | Aschezunge, Eisrand | ◐ |
| `item_seed_barley`, `item_seed_flax` | Saatgut | Händler, verlassener Hof | ◐ |
| `item_wax`, `item_honey` | Wachs, Honig | Bienen | ○ / Liste |
| `item_blueprint_*` | Baupläne (Rennofen, Armbrust T4 …) | Ruinen, Bosse, Schwarzmarkt | ◐ |
| `item_saga_fragment_*` | Saga-Fragmente (Sammelobjekte) | Ruinen, Runensteine | ◐ |
| `item_logbook` | Logbuch (Questgegenstand, Schlickmutter) | Boss 2 | ◐ |
| `item_bridge_runes` | Brückenrunen (Questgegenstand) | Geschichte | ◐ |
| `item_trap_kit` | Fallen-Bausatz (Grubenfalle für den Keiler) | Werkbank | ◐ |
| `item_gift_*` | Gastgeschenke (Siedler anwerben) | hergestellt | ◐ |
| `item_beer`, `item_ale_mead` | Bier, Met | Brauhaus | ○ / Liste |
| `item_pottery` | Tonkrug, Schale (Lagerung, Handel) | Töpferei | ○ |

<a id="d34-15"></a>

### 34.15 14. Waffen, Schilde, Rüstung, Kleidung
- **Waffen ✔/◐:** `ASSET_LIST_WAFFEN.md` – Bartaxt, Sax, Speer, Schwert (ab T3), Rundschild, Bogen, Pfeile (MVP ✔); Keule, Breitaxt, Wurfspeer, Schleuder, Armbrust und Bolzen; Teile-Strategie (Kopf je Material, geteilter Stiel, Stufen über Materialinstanzen).
- **Rüstung ✔:** Slots Helm, Schulter, Oberkörper (seit 2026-11-13), Hände, 2 × Ring (Ringe mit Wirkung, die einzige Ausnahme vom „kein Schmuck-Slot“), Beine, Füße, Umhang – `ASSET_LIST_RUESTUNG.md`.
- **Kleidung:** Tunika, Hose, Schuhe, Kapuze, Schürze, Kleid; ○ ergänzt: Wickelgamaschen, Fellmantel, Gürtel mit Taschen, Wollmütze, Fäustlinge (Kälte im Fjell).
- **Rucksack ◐:** Formen je Ausbaustufe (Tragegestell, Lederranzen, Fellsack), sichtbar am Rücken.
- **Qualität ◐:** grob · ordentlich · gut · meisterhaft · Meisterwerk mit eigenem Namen – sichtbar über Materialinstanz (Glanz, Gravur), kein eigenes Modell.

<a id="d34-16"></a>

### 34.16 15. Transport und Fahrzeuge ○ (offen, nur Vorschlag)
Handkarren (Träger, Phase 2) · Schlitten (Winter, Phase 3) · Ruderboot (Küste, Phase 4) · Karve als Kulisse beim Küstenkaufmann (Phase 4). Ein fahrbares Schiff wäre ein großes Zusatzsystem und gehört nicht in den ersten Wurf.

<a id="d34-17"></a>

### 34.17 16. Animationen (`anim_`)
| Gruppe | Inhalt | Phase |
|---|---|:-:|
| Fortbewegung ✔ | Stehen, Gehen, Laufen, Ducken, Springen, Schwimmen (○), Klettern niedrig (○) – **ohne Erschöpfung** | 1–2 |
| Arbeit ✔ | Hacken, Abbauen, Tragen, Bauen/Hämmern, Kochen; ○ Sägen, Graben, Säen, Ernten, Melken, Füttern, Schmieden, Weben, Gerben, Angeln, Netz werfen, Mahlen | 1–3 |
| Bedürfnisse ✔ | Essen, Schlafen, Sitzen, Wärmen am Feuer (○), Waschen (○) | 1–2 |
| Kampf ◐ | 3 Schlagrichtungen + Stoß je Waffengruppe, Block, Parade, Ausweichen, Schildstoß, Bogen spannen/schießen, Armbrust laden, Treffer, Haltungsbruch (Taumeln), Tod | 3 |
| Sozial ○ | Reden, Zustimmen/Ablehnen (Thing), Jubeln, Trinken, Tanzen am Fest, Trauern, Winken | 2–4 |
| Tiere | je Tier 6–10 (siehe Abschnitt 3) | 2–4 |
| Bosse | je Boss 15–25 | 3–5 |

Quelle: Skelett kompatibel zum UE5-Standardskelett ✔, damit gekaufte oder übertragene Animationen passen (Lizenz prüfen).

<a id="d34-18"></a>

### 34.18 17. Effekte (`vfx_`)
- ✔ Phase 1: Lagerfeuer (Feuer, Rauch), Holzspäne, Steinstaub, Platzieren eines Bauteils.
- Arbeit: Funken an der Esse, Glut beim Schmieden („Glut und Takt“ ◐), Dampf im Löschtrog, Mehlstaub, Wasserspritzer beim Fischen.
- Kampf: Treffer auf Holz/Metall/Fleisch, Funken beim Parieren, Blut ✔ und abtrennbare Körperteile ✔ (beides schaltbar), Haltungsbruch-Hinweis.
- Welt: Regen, Schnee, Nebel (Morgennebel in der Marsch, Nachtnebel in der Heide), Sturm mit Sichtgrenze, Wasserfall-Gischt, Brandung, Gezeiten-Übergang, Lava-Glühen, Dampf heißer Quellen, Fußspuren im Schnee, Staub in Höhlen, Glühwürmchen (Wiesen, Sommer).
- Sagenwelt ◐: Irrlichter, Draugr-Nebel, Troll wird zu Stein (Sonne).
- Siedlung: Rauch aus Dächern, Feuer an Gebäuden (Weltoption Angriffe), Fest-Feuerwerk aus Funken, Saga-Ereignis-Hinweis.

<a id="d34-19"></a>

### 34.19 18. Ton und Musik
- **Musik (`music_`):** Thema je Biom (6), Tag/Nacht-Variante, Kampf (leicht/schwer), je Boss ein Thema (6), Siedlung im Frieden, Fest, Thing, Saga-Finale. Instrumente passend: Leier, Knochenflöte, Trommeln, Kehlgesang. **Kein Gesang mit geschützten Texten.**
- **Umgebung (`amb_`):** je Biom Tag/Nacht/Wetter, Siedlung mit Arbeitsgeräuschen, Innenraum Langhaus, Höhle, Brandung, Wind am Gletscher.
- **Geräusche (`sfx_`):** Schritte je Untergrund (Gras, Erde, Stein, Holz, Schlick, Schnee, Kies), Arbeit je Beruf, Bauen (Platzieren, Abreißen, Einsturz), Werkzeuge, Waffen (Schwung, Treffer je Material, Parade, Schild), Bogen/Armbrust, Tierlaute, Bewohner-Rufe (kurze Laute in einer erfundenen, altnordisch klingenden Sprache ○), Oberfläche (Klick, Fehler, Saga-Eintrag, Stufenaufstieg).

<a id="d34-20"></a>

### 34.20 19. Oberflächen (`ui_`)
| ID | Bildschirm/Fenster | Phase | Status |
|---|---|:-:|:-:|
| `ui_hud` | Gesundheit, Schnellleiste, Koop-Liste (bis 8), **kein Ausdauerbalken** | 1 | ✔ |
| `ui_build_menu` | Baumenü mit Kategorien | 1 | ✔ |
| `ui_inventory` | Inventar und Rucksack, Traglast | 1 | ◐ |
| `ui_crafting` | Herstellen je Station, Qualitätsanzeige | 1–3 | ◐ |
| `ui_settlement` | Bewohner, Berufe, Prioritäten, Bedürfnisse, Vorratsziele („Halte 40 Bretter vorrätig“) | 2 | ◐ |
| `ui_command_view` | Kommandoansicht (Vogelperspektive) | 2 | ✔ |
| `ui_map` | Karte mit Nebel, Orten, Markierungen | 2 | ○ |
| `ui_saga` | Saga-Buch (Einträge, Relikte, Taten) | 2 | ◐ |
| `ui_thing` | Thing-Versammlung, Abstimmungen | 2 | ◐ |
| `ui_trade` | Handel mit Hacksilber | 4 | ✔ |
| `ui_character_editor` | Charakter-Editor (Körper, Kopf, Farben) | 2 | ✔ |
| `ui_settlement_editor` | Lager-Editor (Farben, Wappen, Banner) | 2 | ✔ |
| `ui_skills` | Fähigkeitenbaum, Attribute | 3 | ✔ |
| `ui_world_setup` | Welt erstellen: Seed, Größe, Weltoptionen | 2 | ✔ |
| `ui_multiplayer` | Server beitreten/erstellen, Spielerliste | 1–2 | ○ |
| `ui_settings` | Einstellungen inkl. Blut/Körperteile schaltbar | 2 | ✔ |
| `ui_icons_*` | Icons: Gegenstände, Berufe, Bedürfnisse, Buffs, Stufenrahmen T1–T6, Kartensymbole | 1+ | ✔ |
| `ui_font_*` | Zierschrift (runenartig, nur Überschriften) + gut lesbare Fließtextschrift | 1 | ○ |
| `ui_loading` | Ladebildschirme mit Saga-Bildern | 4 | ○ |

<a id="d34-21"></a>

### 34.21 20. Gestaltung im Charakter- und Lager-Editor ✔
Farbstoffe (Waid blau, Krapp rot, Wau gelb ✔) · Wappenzeichen ○ (Rabe, Wolf, Schiff, Drachenkopf, Knotenmuster, Axt, Sonne, Baum) · Schildmuster (geteilt, geviertelt, Spirale, Ring) · Bannerformen (Rechteck, Schwalbenschwanz, Dreieck) · Muster für Kleidungsborten.

<a id="d34-22"></a>

### 34.22 21. Technik und Grundlagen (für alle Assets)
- **Master-Materialien (`mat_`):** Undurchsichtig, Bewuchs (mit Wind), Terrain, Wasser (Bach, Meer, Gezeiten), Figur/Haut, Haar, Metall (Stufen-Instanzen ✔), Glas/Eis, Effekte.
- **Trim-Sheets:** Holz ✔ (neu, verwittert), Stein, Metall, Leder, Stoff, Turf.
- **Decals (`decal_`):** Schmutz, Moos, Blut (schaltbar), Fußspuren, Brandflecken, Schnee auf Dächern, Runen-Gravuren.
- **Detailstufen:** LODs für alles Größere, Fernansicht für Bäume (Impostor), HLOD für die Siedlung; Budget ✔: Spiel höchstens ca. 20 GB, läuft ab 8 GB RAM.
- **Platzhalter:** einfache Formen für alles, was noch fehlt (wie die Zylinder-Spielfigur jetzt).

<a id="d34-23"></a>

### 34.23 22. Texturen ○ (Vorschlag, ergänzt am 2026-11-13)
Texturen sind die Bilder, die auf Modellen liegen. Damit das Spiel unter ca. 20 GB bleibt, gilt: **lieber wenige, kachelbare Texturen und Trim-Sheets, die viele Modelle teilen**, als für jedes Modell eigene. Format wie beim Holzbaukasten ✔: Farbe (`_BC`), Normalen (`_N`), AO/Rauheit/Metall (`_ORM`).

#### 22.1 Material-Texturen (kachelbar, `T_Mat_*`)
| Gruppe | Varianten |
|---|---|
| Holz | Kiefer, Birke, Eiche, Treibholz (grau, ausgewaschen), verkohlt, frisch geschnitten (Stirnholz mit Jahresringen), bemalt |
| Rinde | Kiefer, Birke (weiß mit Flecken), Fichte, Erle |
| Stein | Granit, Feldstein, Sandstein, Basalt (Säulenstruktur), Lavagestein, Schiefer, behauener Stein, Trockenmauer |
| Erde und Boden | Erde, Lehm, Schlick (nass glänzend), Torf, Kies, Sand, schwarzer Sand, Asche |
| Dach | Stroh/Schilf, Grassoden, Holzschindeln, Birkenrinde unter Soden |
| Wand | Flechtwerk, Lehmputz (glatt und rissig), gekalkt |
| Metall | Eisen roh, Eisen geschmiedet, Stahl, Damast-Maske, Bronze, Silber/Hacksilber, Rost, Hammerschläge |
| Stoff | Wolle grob und fein, Leinen, Filz, Fischgrät- und Rautenköper, Brettchenborten |
| Leder und Fell | Rohhaut, Leder hell/dunkel, genietet, Fell (Wolf, Bär, Schaf, Rentier, Robbe), Pelzrand |
| Natur | Eis (klar, trüb, Gletscherblau), Schnee (frisch, verharscht), Moos, Flechten, Laubstreu, Nadelstreu |

#### 22.2 Terrain-Texturen (`T_Terrain_*`)
Je Biom 4–6 Schichten (Abschnitt 7): Gras kurz/lang, Waldboden, Erde, Fels, Weg (festgetreten), Bachkiesel, Schlick, Heideboden, Schnee, Lava, schwarzer Sand. Dazu **Jahreszeiten-Varianten** über Farbverschiebung statt neuer Texturen: Frühling frisch, Sommer satt, Herbst braun-gold, Winter mit Schneedecke.

#### 22.3 Deko-Texturen und Muster (`T_Deco_*`)
| Gruppe | Beispiele | Wo benutzt |
|---|---|---|
| Schnitzmuster | Flechtband, Tierornamente in frühmittelalterlichen Stilen (Borre, Jellinge, Mammen, Urnes – historisch, gemeinfrei, eigene Umsetzung), Drachenköpfe, Ranken | Türrahmen, Firstbalken, Pfosten, Truhen, Waffengriffe |
| Runen | Runeninschriften (jüngeres Futhark), Runenstein-Bänder mit Schlangenleib, eingeritzte Zeichen | Runensteine, Gravuren, Meisterwerk-Waffen |
| Webmuster | Wandbehänge mit Szenen (Schiffe, Reiter, Jagd), Teppichmuster, Brettchenborten | Langhaus, Kleidung, Festtafel |
| Malerei | Schildbemalungen, bemalte Balken, Bannermotive, Wappenzeichen (Abschnitt 20) | Schilde, Banner, Gebäude |
| Abnutzung | Ruß, Wasserflecken, Kerben, Moosbewuchs, ausgeblichene Farbe | über Masken auf allen Bauteilen |
| Zustand Siedlung | gepflegt / vernachlässigt / beschädigt (Angriffe) | Bauteile, Zäune |

#### 22.4 Decals und Masken
Siehe auch Abschnitt 21. Zusätzlich: Brandspuren, Pfotenabdrücke im Schnee, Wagenspuren, Blätter auf Wegen (Herbst), Pfützen nach Regen, Lagerfeuer-Asche, Teppichkanten, Blutspuren (schaltbar ✔), Schneeauflage oben auf Modellen (eine Maske für alles statt eigener Winter-Modelle).

#### 22.5 Himmel, Wasser, Effekte
Wolkenschichten (Schönwetter, Regen, Sturm, Polarlicht im Winter ○), Sternhimmel, Mond; Wasser-Normalen (Bach, See, Meer, Brandung), Schaum, Gischt; Rauch-, Funken- und Nebel-Texturen für Effekte; Rauschen und Verlaufs-Masken (technisch, selbst erzeugt).

#### 22.6 Oberflächen-Texturen (`T_UI_*`)
Pergament- oder Leinen-Hintergrund, Holzrahmen mit Schnitzkante, Eisenbeschläge an Fensterecken, Stufenrahmen T1–T6 für Icons ✔, Kartenpapier mit Tusche-Stil, Saga-Buch-Seiten, Runen-Zierleisten, Schaltflächen (normal/aktiv/gesperrt). Lesbarkeit hat Vorrang ✔: Zierde nur am Rand.

<a id="d34-24"></a>

### 34.24 23. Deko-Modelle und Kleinkram ○ (ergänzt am 2026-11-13)
Kleine Modelle, die Orte lebendig machen. Ohne Spielfunktion, aber wichtig für Stimmung und Wiedererkennung.
- **Siedlungs-Kleinkram:** Eimer, Schöpfkelle, Holzschalen, Löffel, Seilrollen, aufgehängte Netze, Werkzeuge an der Wand, Späne-Haufen, Sägemehl, Brennholzkorb, Wassereimer am Brunnen, Spinnwirtel, Webgewichte, Mahlsteine, Kesselhaken, Räucherfisch an Stangen.
- **Schnitzwerk als Modell:** Drachenkopf am Giebel, geschnitzte Türpfosten, Hochsitzpfeiler im Langhaus, Wetterfahne, Stuhllehnen, Truhenbeschläge.
- **Fest- und Jahreszeiten-Deko:** Erntegarben, Kränze, Laternen, Festbanner, Julfeuer, Tannengrün im Winter, Fackelreihen beim Thing, Opfergaben am Opferstein.
- **Natur-Streudeko:** Äste, Zapfen, Pilzgruppen, Farne, Steinchen, Muscheln am Strand, Tierknochen, Geweihabwürfe, umgestürzte Stämme, Wurzelteller, Bachsteine mit Moos, Eiszapfen.
- **Ruinen-Teile:** eingestürzte Wände, verkohlte Balken, umgestürzte Runensteine, zerbrochene Schilde, rostige Klingen, Knochenhaufen (für Grabhügel und Banditenlager).
- **Licht als Deko:** Fackeln, Feuerkörbe, Tranlampen, Talglichter, Glut in Herdstellen – mit Effekt und Licht, sparsam wegen Leistung.
- **Bewegliche Deko:** Wäsche und Felle auf Leinen, Banner im Wind, Rauchfahnen, Windspiel aus Knochen – bewegen sich über Material (Wind), nicht über Animation.

<a id="d34-25"></a>

### 34.25 24. Was als Nächstes sinnvoll ist
1. **Biome bestätigen** (Abschnitt 7) – davon hängen Bäume, Felsen, Tiere, Gegner und Bosse ab.
2. ~~Sagenwelt ja/nein~~ – entschieden am 2026-11-13: zuerst nein, später höchstens Weltoption.
3. **Herkunft der Figuren entscheiden** (KI, gekaufte Basis, Freelancer) – größter Kostenblock.
4. **Icon-Raster festlegen** (Blickwinkel, Licht, Rahmen, Stufenfarbe) – größter Mengenblock.
5. Die ○-Vorschläge dieser Liste durchgehen: übernehmen, streichen oder später.

Figuren- und Kampf-Entscheidungen vom 2026-11-13 (Körperbau frei kombinierbar, keine Sprachausgabe, Namensschild + Spielerfarbe, Richtungsangriffe, Zielerfassung, Ausweichschritt, Trefferzonen u. a.) stehen in `docs/01_entscheidungen.md`.


[↑ zum Inhaltsverzeichnis](#inhaltsverzeichnis)

<a id="d35"></a>

## 35. Asset-Liste Waffen mit Tier-Stufen (Planung für Phase 3, Stand 2026-11-12)

> Quelle: `design/ASSET_LIST_WAFFEN.md`

*Ergänzt `design/ASSET_PLAN.md` (Abschnitt 8, Phase 3). Für ChatGPT und den Nutzer.*
*Vorbereitung, keine Produktion: Waffen werden erst in Phase 3 („Kampf und Ausrüstung“) gebaut, wenn der Vertical Slice stabil läuft (Architekturregel 9).*
*Alle Werte sind Vorschläge von Claude. Spielwerte (Schaden, Tempo, Haltbarkeit) gehören nicht hierher, sondern später in `data/`.*

<a id="d35-1"></a>

### 35.1 1. Grundidee
- **Ausrüstung wie in Kingdom Come:** Slots, levelbasiert, schmiedbar (bestätigt).
- **Kampf ohne Ausdauer-Sperre:** Parierfenster, Haltungsleiste der Gegner, Abklingzeiten. Waffen unterscheiden sich in Reichweite, Tempo und Haltungsschaden, nicht in Ausdauerverbrauch.
- **Wenige Meshes, viele Stufen:** Pro Waffentyp gibt es nur **2–3 Kopf- oder Klingen-Meshes**. Die Stufen unterscheiden sich vor allem über **Material, Farbe und kleine Zusatzteile**. Das schützt das 20-GB-Budget.
- **Lesbarkeit (Klarform-Saga):** Die Stufe muss auch auf Distanz und im Koop-Gewühl erkennbar sein: über Metallfarbe, eine helle Schneide ab Stufe 4 und ein Muster bei Stufe 5.

<a id="d35-2"></a>

### 35.2 2. Tier-Stufen
Vorschlag mit nordisch-historischer Abfolge statt der üblichen Bronze/Eisen/Silber-Leiter. Das grenzt das Spiel zugleich von den Vorbildern ab.

| ID | Name | Material-Look | Freischaltung (Vorschlag) | Erkennungszeichen |
|---|---|---|---|---|
| `tier_1` | Rohstufe | Holz, Feuerstein, Knochen, Sehnen | Spielstart | kein Metall, helle Knochen- und Steinfarben |
| `tier_2` | Raseneisen | grobes, dunkles Eisen aus Sumpferz, sichtbare Schmiedespuren | erstes Biom, Rennofen | dunkelgrau-rostbraun, raue Oberfläche |
| `tier_3` | Schmiedeeisen | sauber geschmiedetes Eisen | Boss 1 / Schmiede Stufe 2 | mittelgrau, glattere Flächen |
| `tier_4` | Stahl | aufgekohlte, gehärtete Schneide | Boss 2 / Schmiede Stufe 3 | **helle Schneidenkante** als Band |
| `tier_5` | Damast (Schweißverbund) | sichtbares Wellenmuster im Stahl | Boss 3 | **Wellenmuster** über Maske, Messing-Akzente |
| `tier_6` | Relikt *(bestätigt, Ausbau)* | Bossmaterialien, eigenständige Formen | späte Bosse | eigene Akzentfarbe, eigene Silhouette |

**Stufenfarben für UI und Icons** (Rahmen, nicht die Waffe selbst): T1 Knochenweiß `#E5D8BC`, T2 Rostbraun `#8A5A3C`, T3 Schieferblau `#607C8B`, T4 Fjordgrün `#3E6655`, T5 Ocker `#D2A64F`, T6 Ziegelrot `#A95D48`.

<a id="d35-3"></a>

### 35.3 3. Waffentypen und Verfügbarkeit
✓ = gibt es in dieser Stufe · – = nicht vorgesehen. MVP = erste Kampf-Version in Phase 3.

| ID | Waffe | Slot | MVP | T1 | T2 | T3 | T4 | T5 | T6 |
|---|---|---|:-:|:-:|:-:|:-:|:-:|:-:|:-:|
| `weapon_axe_bearded` | Bartaxt (einhändig) | Haupthand | ✓ | ✓ | ✓ | ✓ | ✓ | ✓ | ✓ |
| `weapon_seax` | Sax (Kurzklinge) | Haupt-/Nebenhand | ✓ | ✓ | ✓ | ✓ | ✓ | ✓ | – |
| `weapon_spear` | Speer | Haupthand (mit Schild) oder beidhändig | ✓ | ✓ | ✓ | ✓ | ✓ | ✓ | ✓ |
| `weapon_sword` | Schwert (einhändig) | Haupthand | ✓ | – | – | ✓ | ✓ | ✓ | ✓ |
| `shield_round` | Rundschild | Nebenhand | ✓ | ✓ | ✓ | ✓ | ✓ | ✓ | ✓ |
| `weapon_bow` | Bogen | Fernkampf | ✓ | ✓ | ✓ | ✓ | ✓ | ✓ | ✓ |
| `ammo_arrow` | Pfeile | Munition | ✓ | ✓ | ✓ | ✓ | ✓ | ✓ | ✓ |
| `weapon_club` | Keule | Haupthand | | ✓ | – | – | – | – | – |
| `weapon_axe_dane` | Breitaxt (zweihändig) | beidhändig | | – | ✓ | ✓ | ✓ | ✓ | ✓ |
| `weapon_spear_throw` | Wurfspeer | Fernkampf | | ✓ | ✓ | ✓ | ✓ | – | – |
| `weapon_sling` | Schleuder | Fernkampf | | ✓ | – | – | – | – | – |
| `weapon_crossbow` | Armbrust | Fernkampf | | – | – | ✓ | ✓ | ✓ | ✓ |
| `ammo_bolt` | Bolzen | Munition | | – | – | ✓ | ✓ | ✓ | ✓ |

Schwert erst ab T3: Schwerter waren teuer und selten. Das gibt dem Fortschritt ein Ziel.
Armbrust: **Freischaltung über bestimmte Attribute plus eine Fähigkeit im Skilltree** (bestätigt). Ab welcher Stufe ist offen: Claude schlägt T3 vor (Mechanik braucht Schmiedeeisen), ChatGPT/Codex T4 als seltene Import- oder Schwarzmarktware (`design/inbox/05_assets_armbrust.md`). Die T4-Variante passt gut zum bestätigten Nacht-Händler. Langsamer als der Bogen, dafür mehr Haltungsschaden. Ausgestaltung der Teile siehe 05.
Werkzeuge aus Phase 1 (`tool_axe`, `tool_pickaxe`) bleiben eigene Assets. Die Kampfaxt darf deren Stiel-Mesh wiederverwenden.

<a id="d35-4"></a>

### 35.4 4. Mesh-Strategie pro Typ
Jede Waffe besteht aus **Teilen**, die kombiniert werden. Material-Instanzen erzeugen die Stufen.

| Typ | Teile | Kopf-/Klingen-Meshes |
|---|---|---|
| Bartaxt | Kopf, Stiel, Wicklung | T1 Stein-Kopf · T2–T5 Eisen-Kopf (ein Mesh) · T6 eigener Kopf |
| Sax | Klinge, Griff, Knauf | T1 Knochen/Feuerstein · T2–T5 Metall · – |
| Speer | Spitze, Schaft, Bindung | T1 Feuerstein/Knochen · T2–T5 Metall (T4/T5 mit Flügeln als Zusatzteil) · T6 eigen |
| Schwert | Klinge, Parierstange, Griff, Knauf | T3–T5 eine Klinge, **2 Knauf-Varianten** (einfach / gestuft) · T6 eigen |
| Rundschild | Scheibe, Buckel, Rand | T1 ohne Metallbuckel (Holzbuckel) · T2–T5 Metallbuckel und Randbeschlag ab T3 · T6 eigen |
| Bogen | Bogen, Sehne, Griffwicklung | T1 einfacher Holzbogen · T2–T5 verstärkter Bogen (Hornbeläge als Zusatzteil ab T4) · T6 eigen |
| Pfeil | Schaft, Spitze, Federn | 1 Schaft, 3 Spitzen (Stein, Eisen, Stahl/Damast) |
| Breitaxt | Kopf, langer Stiel | T2–T5 ein Kopf · T6 eigen |
| Armbrust | Säule, Bogen, Sehne, Abzug | T3–T5 eine Säule und ein Bogen (Horn-Komposit als Zusatzteil ab T4) · T6 eigen |
| Bolzen | Schaft, Spitze | 1 Schaft, 2 Spitzen (Eisen, Stahl/Damast) |

**Schild-Gestaltung (bestätigt):** individuell gestaltbar. Spieler gestalten ihren Schild im **Charakter-Editor**, die Siedlung bekommt im **Lager-Editor** eigene Farben und ein Wappen (Deutung von Claude). Technik: 2–3 Farben als Material-Parameter plus **8–12 eigene Muster-Masken** und ein Wappen-Feld; keine historischen Wappen nachbilden. Dieselben Farben können Umhang und Banner nutzen.

<a id="d35-5"></a>

### 35.5 5. Budgets
| Asset-Art | Dreiecke Ziel | Obergrenze | Texturen |
|---|---:|---:|---|
| Einhandwaffe (Axt, Sax, Schwert, Keule) | 1.500 | 3.000 | geteiltes Waffen-Atlas |
| Zweihand- und Stangenwaffe (Breitaxt, Speer) | 2.000 | 4.000 | geteiltes Waffen-Atlas |
| Rundschild | 1.200 | 2.500 | Atlas + Muster-Masken 512² |
| Bogen | 1.200 | 2.500 | Atlas |
| Pfeil | 150 | 300 | Atlas |

- **Waffen-Atlas** `T_Weapon_Atlas_BC/_N/_ORM`, 2048²: Holz, Leder, Knochen, Stein, Sehne, Federn.
- **Metall-Set** `T_Weapon_Metal_BC/_N/_ORM`, 1024², kachelbar; Stufen T2–T5 über Material-Instanzen (Farbton, Rauheit, Schneidenband).
- **Damast-Maske** `T_Weapon_Damask_Mask`, 512², kachelbar.
- Je Waffe 3 LODs. Keine 4K-Texturen.

<a id="d35-6"></a>

### 35.6 6. Benennung
- Basistyp: `weapon_axe_bearded` · Stufe: `tier_3` · Item im Spiel: `weapon_axe_bearded_t3`.
- Unreal: `SM_Weapon_AxeBearded_Head_T1`, `SM_Weapon_AxeBearded_Head_Metal`, `SM_Weapon_AxeBearded_Haft`, `MI_Weapon_Metal_T4`, `T_Icon_Weapon_AxeBearded`.

<a id="d35-7"></a>

### 35.7 7. Weitere Assets, die an Waffen hängen
| Bereich | Inhalt | Prio |
|---|---|---|
| Animationen | Sets für einhändig, einhändig + Schild, zweihändig, Speer, Bogen; je leichter/schwerer Angriff, Kombo, **Parieren**, Block, Schildstoß, Haltungsbruch-Reaktion. **Keine Erschöpfungs-Animationen.** | A |
| Effekte | Funken beim Parieren, Treffer auf Holz/Metall/Schild, Haltungsbruch-Signal | A |
| Audio | Schwung, Treffer je Material (Holz, Metall, Schild, Körper), Parieren, Bogen spannen/lösen | A |
| UI | **ein Icon pro Waffentyp** plus Stufen-Rahmen in der Stufenfarbe (statt einem Icon pro Typ und Stufe), Haltungsleiste der Gegner, Ausrüstungs-Slots | A |
| Schmiede | Werkstücke (Rohling, Klinge), Amboss, Esse – gehört zur Schmiede-Station | B |
| Köcher, Scheide, Schildgurt | am Körper getragen | B |
| Editor | Muster-Masken (8–12), Wappen-Bausteine, Farbpaletten für Charakter- und Lager-Editor | B |

#### Gewaltdarstellung (bestätigt: einstellbar)
Vorschlag für die Menü-Stufen: **Aus** (keine Blutspuren) · **Blut** · **Blut + abtrennbare Körperteile**. Jede Stufe braucht eigene Assets:

| Stufe | Assets | Aufwand |
|---|---|---|
| Blut | Blut-Effekt bei Treffern (Niagara), 4–6 Wund-Decals, Blutspuren am Boden | klein |
| Abtrennbare Körperteile | Schnittstellen an Figuren (Kopf, Arme, Beine), Stumpf-Kappen je Schnittstelle, abgetrennte Teile aus den vorhandenen Modulteilen der Figur, eigener Effekt | **groß**: betrifft jede Figur und jeden Gegnertyp |

Hinweise von Claude:
- Abtrennbare Körperteile sollten ein **Ausbau nach der ersten Kampf-Version** sein. Die modularen Figuren aus dem Asset-Plan helfen, weil Körperteile schon getrennt sind.
- **Altersfreigabe:** Nach meinem Wissen richtet sich die Einstufung nach dem stärksten einstellbaren Inhalt, nicht nach der Voreinstellung. Das sollte vor der Veröffentlichung für das Zielland geprüft werden (für Deutschland USK).

<a id="d35-8"></a>

### 35.8 8. Mengenabschätzung (MVP Phase 3)
7 Typen im MVP · rund **25 Meshes** (Köpfe, Klingen, Stiele, Zusatzteile) · **3 Textursätze** + Masken · **7 Icons** + 6 Stufenrahmen.
Zum Vergleich: ohne Teile-Strategie wären es 7 Typen × 6 Stufen = 42 Einzelwaffen mit eigenen Texturen.

<a id="d35-9"></a>

### 35.9 9. Entscheidungen
**Beantwortet am 2026-11-12** (eingetragen in `docs/01_entscheidungen.md`):
1. Stufen-Leiter: nordisch-historisch wie vorgeschlagen.
2. Relikt-Stufe T6: ja.
3. Waffentypen im MVP: die 7 Typen (Antwort „ja“, von Claude als „passend“ gedeutet).
4. Gewaltdarstellung: einstellbar, auch ob Körperteile abgetrennt werden.
5. Schild: individuell gestaltbar im Charakter- und Lager-Editor.
6. Fernkampf: Bogen und Armbrust.

**Nachtrag 2026-11-12:** Armbrust wird über Attribute plus eine Fähigkeit freigeschaltet.

**Noch offen:**
1. Armbrust ab **T3 oder T4** (siehe Abschnitt 3)?
2. Ist „Lager-Editor“ der Editor für Farben und Wappen der **Siedlung**?
3. Abtrennbare Körperteile schon in der ersten Kampf-Version oder als Ausbau danach (Empfehlung: Ausbau)?
4. Ziel-Altersfreigabe (z. B. USK 16 oder 18)?


[↑ zum Inhaltsverzeichnis](#inhaltsverzeichnis)

<a id="d36"></a>

## 36. Asset-Liste Rüstung und Kleidung mit Tier-Stufen (Planung, Stand 2026-11-12, überarbeitet)

> Quelle: `design/ASSET_LIST_RUESTUNG.md`

*Ergänzt `design/ASSET_PLAN.md`, `design/ASSET_LIST_WAFFEN.md` und `design/ASSET_LIST_GEGENSTAENDE.md`. Für ChatGPT und den Nutzer.*
*Überarbeitet nach der Slot-Entscheidung des Nutzers (2026-11-12) und abgeglichen mit `design/inbox/06_assets_ruestung_kleidung.md` (ChatGPT/Codex).*
*Grundkleidung der Figuren gehört zu Phase 1 (`char_player_base`, `char_villager_base`), Miliz-Ausrüstung zu Phase 2, Rüstung zu Phase 3.*
*Alle Werte sind Vorschläge von Claude. Schutzwerte, Gewicht und Haltbarkeit gehören später in `data/`.*

<a id="d36-1"></a>

### 36.1 1. Grundidee
- **Ausrüstung wie in Kingdom Come (bestätigt), mit den vom Nutzer festgelegten Slots** (Abschnitt 2).
- **Gleiche Stufen-Leiter wie bei den Waffen (bestätigt):** T1 Rohstufe · T2 Raseneisen · T3 Schmiedeeisen · T4 Stahl · T5 Damast · T6 Relikt.
- **Formfamilien statt Einzelteile:** Pro Slot gibt es nur 3 Grundformen (leicht, mittel, schwer) plus T6. Die Stufen unterscheiden sich über Material, Farbe und kleine Zusatzteile.
- **Individuelle Gestaltung (bestätigt):** Farben im **Charakter-Editor**, Siedlungsfarben und Wappen im **Lager-Editor**. Keine automatische Bindung an die Koop-Spielerfarbe. Technik: 3 Farbkanäle als Masken pro Teil.
- **Haltbarkeit (bestätigt, auch für Rüstung):** einfach gehalten, ein Wert pro Teil, Reparatur an Werkbank oder Schmiede. Optisch nur `fresh` und `worn` über Materialinstanz.
- **Stil:** Klarform-Saga, nordisch-frühmittelalterlich. **Keine Hörnerhelme.** Kettenhemd und Helm sind seltene Fortschritts- und Statusausrüstung, nicht Standardkleidung (so auch 06).

<a id="d36-2"></a>

### 36.2 2. Slots (bestätigt am 2026-11-12)
| ID | Slot | Mesh | Beispiele | Hinweis |
|---|---|:-:|---|---|
| `slot_head` | Helm | ✓ | Fellmütze, Lederhaube, Spangenhelm, Brillenhelm | |
| `slot_shoulders` | Schulter | ✓ | Fellkragen, Leder-Schulterstücke, Lamellen | neu gegenüber der ersten Fassung |
| `slot_torso` | Oberkörper | ✓ | Tunika, Gambeson, Lederpanzer, Kettenhemd | **bestätigt am 2026-11-13:** ein Slot, Unterkleidung gehört zur Grundfigur (wie in 06); schützt die Trefferzone „Körper“ |
| `slot_hands` | Hände | ✓ | Handschuhe, Armschienen | ein Paar-Mesh |
| `slot_ring_1`, `slot_ring_2` | 2 × Ring | – | Ringe mit Wirkung | nur Icon; **geklärt am 2026-11-13:** Ringe sind die Ausnahme vom „kein Schmuck-Slot“, anderer Schmuck bleibt Handelsware |
| `slot_legs` | Beine | ✓ | Hose, Wickelgamaschen, Beinschienen | Basishose gehört zur Grundfigur |
| `slot_feet` | Füße | ✓ | Bundschuhe, Stiefel | ein Paar-Mesh |
| `slot_cloak` | Umhang | ✓ | Wollumhang, Fellumhang | **wird nach dem ersten Winter freigeschaltet** |

**Umhang und Winter:** Es gibt Jahreszeiten mit Winter; der erste Winter ist nicht hart (bestätigt). Vorschlag: Der Umhang gibt Kälteschutz (passt zum Bedürfnis „Wärme“ aus dem Slice) und ist die Belohnung für den überstandenen ersten Winter.

**Lagen ohne Durchragen:** Schulter- und Oberkörperteile werden aufeinander abgestimmt modelliert. Verdeckte Körperteile werden ausgeblendet.

<a id="d36-3"></a>

### 36.3 3. Ausrüstung nach Stufe
| Slot | T1 Rohstufe | T2 Raseneisen | T3 Schmiedeeisen | T4 Stahl | T5 Damast | T6 Relikt |
|---|---|---|---|---|---|---|
| Helm | Fellmütze, Stoffhaube | Lederhaube mit groben Eisenbändern | Spangenhelm mit Nasal | Helm mit Brillenvisier, helle Stirnkante | verzierter Helm, Messingpunkte | eigene Form |
| Schulter | Fellkragen | Leder-Schulterstücke mit Nieten | Leder mit Eisenbeschlag | Stahl-Lamellen | Lamellen mit Messingsaum | eigene Form |
| Oberkörper | wattierte Tunika | Gambeson mit Lederverstärkung | kurzes Kettenhemd über Gambeson | langes Kettenhemd, helle Kanten | Kettenhemd mit Damast-Segmenten | eigene Form |
| Hände | Stoffwickel, Fellhandschuhe | Leder-Armschienen mit Nieten | Armschienen mit Eisen | Stahl-Armschienen | Armschienen mit Damastfläche | eigene Form |
| Beine | Wickelgamaschen | Leder an Knie und Schienbein | Eisenbeschläge | Stahl-Beinschienen | Damast-Akzent | eigene Form |
| Füße | Bundschuhe | geschnürte Lederstiefel | Stiefel | verstärkte Stiefel | verzierte Stiefel | eigene Form |
| Umhang | Fellumhang | Wollumhang | Wollumhang mit Fibel | gefütterter Umhang | Umhang mit Borte | eigene Form |
| Ring | Knochenring | Eisenring | Eisenring | Stahlring | Damast-/Messingring | Reliktring |

**Formfamilien:** leicht = T1 · mittel = T2–T3 · schwer = T4–T5 · T6 eigen.

<a id="d36-4"></a>

### 36.4 4. Kleidung (ohne Schutzwert)
| ID | Teil | Phase | Hinweis |
|---|---|:-:|---|
| `cloth_tunic` | Tunika | 1 | Grund-Outfit von Spieler und Dorfbewohnern |
| `cloth_trousers` | Hose | 1 | |
| `cloth_shoes` | Bundschuhe | 1 | |
| `cloth_hood` | Kapuze | 2 | |
| `cloth_apron` | Schürze | 2 | Erkennungsteil für Handwerker-Jobs |
| `cloth_dress` | Kleid/Überkleid | 2 | zweiter Körpertyp |

Berufe der Dorfbewohner erkennt man an **Farbe + Werkzeug + höchstens einem Kleidungsteil**, wie im Asset-Plan vorgesehen.

<a id="d36-5"></a>

### 36.5 5. Miliz (Phase 2)
Dorfbewohner als Verteidiger tragen **vereinfachte Fassungen** der T1–T3-Ausrüstung: eigene LODs mit weniger Dreiecken, gleiche Texturen, Siedlungsfarben auf Umhang und Schild.

<a id="d36-6"></a>

### 36.6 6. Budgets
Gesamtbudget aus dem Asset-Plan: Spielerfigur **40.000 Dreiecke** mit voller Ausrüstung (06 nennt 32.000–36.000 als Ziel – passt), Dorfbewohner **20.000**.

| Teil | Spieler Ziel | Dorfbewohner/Miliz Ziel |
|---|---:|---:|
| Körper, Kopf, Haare, Grundkleidung | 14.000 | 9.000 |
| Helm | 2.000 | 1.000 |
| Schulter | 2.000 | 1.000 |
| Oberkörper | 6.000 | 3.500 |
| Hände | 1.500 | 500 |
| Beine | 3.500 | 2.000 |
| Füße | 1.500 | 1.000 |
| Umhang | 3.000 | 1.500 |
| Ringe | 0 (nur Icon) | 0 |
| **Summe** | **33.500** | **19.500** |

- **Kleidungs-Atlas** `T_Clothing_Atlas_BC/_N/_ORM`, 2048² (Name aus 06 übernommen): Stoff, Wolle, Leder, Fell, Borten.
- **Kettengeflecht** `T_Mail_N`, 512², kachelbar (nur Normal Map, Farbe aus dem Metall-Set der Waffen).
- **Metall-Set** und **Damast-Maske** aus `design/ASSET_LIST_WAFFEN.md` wiederverwenden.
- **Farbmasken:** 3 Kanäle pro Teil für die Editoren.
- Je Teil 3 LODs. Keine 4K-Texturen.

<a id="d36-7"></a>

### 36.7 7. Weitere Assets
| Bereich | Inhalt | Prio |
|---|---|---|
| UI | Icon pro Slot-Typ und Formfamilie + Stufen-Rahmen; Ausrüstungsansicht mit Figur; Haltbarkeitsanzeige | A |
| Charakter-Editor | Körpertyp, Kopf, Haare, Farben, Schild-Muster und -Wappen | A |
| Lager-Editor | Siedlungsfarben, Wappen-Bausteine, Banner | B |
| Props | `prop_banner` (Banner der Siedlung), `prop_armor_stand` (Rüstungsständer im Haus) | B |
| Physik | Umhang mit einfacher Physik, keine Stoffsimulation im MVP (wie in 06) | A |
| Audio | Schritte je Rüstungsklasse (Stoff, Leder, Kette) | B |

<a id="d36-8"></a>

### 36.8 8. Mengenabschätzung
- 7 Slots mit Mesh × 3 Formfamilien = **21 Meshes** + 7 Relikt-Meshes + Zusatzteile (ca. 8) = rund **36**.
- Ringe: nur Icons (je Stufe eins, rund 6).
- Kleidung: rund **6** Meshes.
- **Zweiter Körpertyp:** Mit eigenen Meshes pro Körpertyp verdoppelt sich die Zahl auf rund 84. Mit **einem Mesh pro Teil plus Formanpassung (Morph Target)** bleibt es bei rund 42.

<a id="d36-9"></a>

### 36.9 9. Entscheidungen
**Beantwortet am 2026-11-12:** Slots (Helm, Schulter, Hände, 2 × Ring, Beine, Füße, Umhang nach dem ersten Winter) · Haltbarkeit auch für Rüstung · keine automatische Spielerfarbe (die Koop-Spielerfarbe vom 2026-11-13 gilt nur für Namensschild, Karte und Markierung, nicht für die Kleidung).

**Beantwortet am 2026-11-13:** Oberkörper-Slot ja, ein Slot · Ringe sind die Ausnahme vom „kein Schmuck-Slot“, Fibeln/Armringe bleiben Handelsware.

**Noch offen:**
1. **Umhang:** nur Optik oder auch Kälteschutz für den Winter?
2. **Zweiter Körpertyp:** ein Mesh mit Formanpassung (Empfehlung) oder eigene Meshes? (Dass es zwei frei kombinierbare Körperbauten gibt, ist seit 2026-11-13 entschieden; offen ist nur die technische Umsetzung.)
3. **Helm ausblendbar** (Option im Menü)?
4. **Schutzwerte nach Schadensart** (Hieb, Stich, Wucht) wie in Kingdom Come, oder ein einziger Rüstungswert?


[↑ zum Inhaltsverzeichnis](#inhaltsverzeichnis)

<a id="d37"></a>

## 37. Asset-Liste Gegenstände mit Tier-Stufen (Planung, Stand 2026-11-12)

> Quelle: `design/ASSET_LIST_GEGENSTAENDE.md`

*Ergänzt `design/ASSET_PLAN.md` und `design/ASSET_LIST_WAFFEN.md`. Für ChatGPT und den Nutzer.*
*Phase-1-Gegenstände (Spalte „Phase“ = 1) gehören zum Vertical Slice, alles andere ist Vorbereitung für spätere Phasen.*
*Alle Werte sind Vorschläge von Claude. Spielwerte (Stapelgröße, Gewicht, Buff-Stärke) gehören später in `data/`.*

<a id="d37-1"></a>

### 37.1 1. Grundidee
- **Nicht jeder Gegenstand braucht ein 3D-Modell.** Die meisten Gegenstände sieht man nur als **Icon** im Inventar. Ein Mesh bekommt nur, was sichtbar **getragen, gestapelt oder abgelegt** wird.
- **Sammelbehälter statt Einzelmodelle:** Im Lager und beim Tragen stehen Nahrung und Kleinkram in **Sack, Korb, Fass oder Kiste**. Fallengelassene Gegenstände erscheinen als **ein gemeinsames Bündel-Mesh** mit Icon darüber.
- **Stufen wie bei den Waffen:** T1 Rohstufe · T2 Raseneisen · T3 Schmiedeeisen · T4 Stahl · T5 Damast · T6 Relikt (bestätigt, Ausbau). Die Rohstoffe einer Stufe schalten die Waffen und Werkzeuge dieser Stufe frei.
- **IDs:** Alle Inventar-Gegenstände beginnen mit `item_`, Werkzeuge mit `tool_`, Behälter-Props mit `prop_`. Die Phase-1-IDs aus dem Asset-Plan bleiben unverändert.

Spalten: **Mesh** = eigenes 3D-Modell nötig (✓) oder nur Icon (–) bzw. Behälter (Sack/Korb/Fass/Kiste) · **Phase** = ab wann gebraucht.

<a id="d37-2"></a>

### 37.2 2. Rohstoffe
| ID | Gegenstand | Stufe | Mesh | Phase | Hinweis |
|---|---|:-:|:-:|:-:|---|
| `item_log` | Baumstamm | T1 | ✓ | 1 | wird getragen und gestapelt |
| `item_stone` | Stein | T1 | ✓ | 1 | |
| `item_stick` | Äste | T1 | – | 2 | Bündel im Lager |
| `item_fiber` | Pflanzenfasern | T1 | – | 2 | Korb |
| `item_flint` | Feuerstein | T1 | – | 2 | für T1-Waffen und -Werkzeuge |
| `item_bone` | Knochen | T1 | – | 2 | Kiste |
| `item_hide_raw` | Rohhaut | T1 | ✓ | 2 | hängt am Trockengestell |
| `item_resin` | Harz | T1 | – | 2 | Korb |
| `item_clay` | Ton | T1 | – | 2 | Korb |
| `item_thatch` | Stroh/Schilf | T1 | ✓ | 2 | Garbe; Dach und Betten |
| `item_turf` | Grassoden | T1 | ✓ | 2 | Stapel; für Turf-Dächer |
| `item_bog_ore` | Raseneisenerz | T2 | ✓ | 3 | Haufen im Lager |
| `item_charcoal` | Holzkohle | T2 | – | 3 | Sack |
| `item_wool` | Wolle | T2 | – | 2 | Korb; von Schafen (Tierhaltung bestätigt) |
| `item_feather` | Federn | T1 | – | 2 | Hühner und Jagd; für Pfeile |
| `item_egg` | Eier | T1 | – | 2 | Hühner; Zutat |
| `item_milk` | Milch | T2 | – | 3 | Ziegen; nur Zutat (verdirbt nicht, gilt als verarbeitet) |
| `item_dye_*` | Färbemittel (Waid blau, Krapp rot, Wau gelb) | T2 | – | 3 | für Charakter- und Lager-Editor (Kleidung, Schild, Banner) |
| `item_flax` | Flachs | T2 | – | 2 | Bündel |
| `item_iron_ore` | Eisenerz (Fels) | T4 | ✓ | 4 | Biom 2; Grundlage für Stahl |
| `item_antler` | Geweih | T3 | – | 3 | Griffe, Bogenbeläge |
| `item_amber` | Bernstein | T4 | – | 4 | Schmuck, Handel |
| `item_boss_material_*` | Bossmaterial je Boss | T3–T6 | – | 4 | einzigartige Icons, Freischaltung der nächsten Stufe |

<a id="d37-3"></a>

### 37.3 3. Zwischenprodukte (hergestellt)
| ID | Gegenstand | Stufe | Mesh | Phase | Hinweis |
|---|---|:-:|:-:|:-:|---|
| `item_plank` | Bretter | T1 | ✓ | 1 | getragen und gestapelt |
| `item_stone_block` | behauener Stein | T1 | ✓ | 2 | für die Materialstufe Stein |
| `item_rope` | Seil | T1 | – | 2 | Korb |
| `item_leather` | Leder | T2 | – | 2 | Kiste |
| `item_linen` | Leinen | T2 | – | 3 | Kiste |
| `item_cloth_wool` | Wollstoff | T2 | – | 3 | Kiste |
| `item_tar` | Teer | T2 | – | 4 | Fass |
| `item_iron_bar_bog` | Raseneisen-Barren | T2 | ✓ | 3 | Barren-Stapel, Farbe je Stufe |
| `item_iron_bar` | Schmiedeeisen-Barren | T3 | ✓ | 3 | aus Raseneisenerz in besserer Esse; gleiches Mesh, andere Materialinstanz |
| `item_steel_bar` | Stahl-Barren | T4 | ✓ | 4 | gleiches Mesh |
| `item_damask_billet` | Damast-Paket | T5 | ✓ | 5 | gleiches Mesh, Damast-Maske |
| `item_rivets` | Nieten und Nägel | T2 | – | 3 | Kiste; Schildbeschläge, Türen |

Die vier Barren teilen sich **ein Mesh** und unterscheiden sich nur über die Metall-Materialinstanzen aus der Waffenliste.

<a id="d37-4"></a>

### 37.4 4. Nahrung (Buff-System, kein Ausdauer-Ersatz)
Essen gibt Buffs auf **Gesundheit, Regeneration, Traglast, Resistenzen oder Schaden** (bestätigt). Rohes Essen gibt wenig, Gekochtes mehr, Kombinationen am meisten.

| ID | Gegenstand | Stufe | Mesh | Phase | Buff-Schwerpunkt |
|---|---|:-:|:-:|:-:|---|
| `item_berries` | Beeren | T1 | – | 1 | Regeneration (klein) |
| `item_meal` | einfache Mahlzeit | T1 | – | 1 | Gesundheit; Platzhalter für den Slice |
| `item_mushrooms` | Pilze | T1 | – | 2 | Regeneration |
| `item_meat_raw` | rohes Fleisch | T1 | – | 2 | Sack/Fass im Lager |
| `item_meat_cooked` | gebratenes Fleisch | T1 | – | 2 | Gesundheit |
| `item_fish_raw` / `item_fish_cooked` | Fisch | T1 | – | 2 | Regeneration |
| `item_grain` | Gerste | T2 | – | 2 | Sack; Feldbau |
| `item_flour` | Mehl | T2 | – | 3 | Sack |
| `item_porridge` | Grütze | T2 | – | 2 | Traglast |
| `item_bread` | Fladenbrot | T2 | – | 3 | Gesundheit + Traglast |
| `item_stew` | Eintopf | T3 | – | 3 | Gesundheit + Resistenz |
| `item_honey` | Honig | T3 | – | 3 | Zutat |
| `item_mead` | Met | T3 | – | 3 | Schaden (kurz) |
| `item_feast_*` | Festmahl je Biom | T4–T5 | – | 4 | mehrere Buffs |

| `item_food_spoiled` | verdorbenes Essen | T1 | – | 2 | kein Buff; Dünger für den Feldbau (falls es Feldbau gibt) |

| `item_meat_dried` / `item_fish_dried` | Trockenfleisch, Stockfisch | T2 | – | 2 | am Trockengestell hergestellt; verdirbt nicht (Vorschlag) |

**Verderben (bestätigt, präzisiert am 2026-11-12):** Fleisch **4 Tage**, Obst und Gemüse **2,5 Tage**, wenn sie nicht trocken gelagert werden; sonstige Lebensmittel **3 Tage**.
Vorschläge von Claude dazu (offen, siehe `docs/01_entscheidungen.md`):
- Gemeint sind **Spieltage**.
- **Trockene Lagerung** = der geschlossene Raum des Lagers (`building_storage`) bzw. später eine Vorratskammer. Dort verdirbt nichts oder deutlich langsamer.
- **Trocknen und Räuchern** (Trockengestell) machen Fleisch und Fisch haltbar. Das passt zum nordischen Setting.
- **Erst ab Phase 2.** Im Vertical Slice verdirbt nichts, damit die Dorfbewohner-KI zuerst ohne diese Zusatzregel stabil läuft.
- Spannung zur Regel „kein Zeitdruck“: 2,5 bis 4 Tage sind spürbar. Trockene Lagerung und Trocknen sollen den Druck herausnehmen.

<a id="d37-5"></a>

### 37.5 5. Werkzeuge mit Stufen
Gleiche Teile-Strategie wie bei den Waffen: Kopf-Mesh je Material (T1 Stein, ab T2 Metall), Stiel geteilt, Stufen über Materialinstanzen.

| ID | Werkzeug | T1 | T2 | T3 | T4 | T5 | Mesh | Phase | Wirkung der Stufe |
|---|---|:-:|:-:|:-:|:-:|:-:|:-:|:-:|---|
| `tool_axe` | Axt | ✓ | ✓ | ✓ | ✓ | ✓ | ✓ | 1 | schneller fällen, härtere Bäume |
| `tool_pickaxe` | Spitzhacke | ✓ | ✓ | ✓ | ✓ | ✓ | ✓ | 1 | schneller abbauen, neue Erze |
| `tool_hammer` | Bauhammer | ✓ | ✓ | ✓ | – | – | ✓ | 1 | schaltet Baustufen frei |
| `tool_knife` | Messer | ✓ | ✓ | ✓ | ✓ | – | ✓ | 2 | häuten, mehr Ausbeute |
| `tool_sickle` | Sichel | – | ✓ | ✓ | ✓ | – | ✓ | 2 | Getreide, Fasern |
| `tool_hoe` | Hacke | ✓ | ✓ | ✓ | – | – | ✓ | 2 | Feldbau |
| `tool_fishing_rod` | Angel | ✓ | ✓ | – | – | – | ✓ | 2 | Fischen |
| `tool_torch` | Fackel | ✓ | – | – | – | – | ✓ | 2 | Licht (verbraucht sich langsam, kein Zeitdruck) |
| `tool_tongs` | Schmiedezange | – | ✓ | ✓ | – | – | ✓ | 3 | gehört zur Schmiede |

Dorfbewohner nutzen dieselben Werkzeug-Meshes. Ihre Arbeitsgeschwindigkeit hängt vom Werkzeug im Lager ab (Vorschlag).

<a id="d37-6"></a>

### 37.6 6. Verbrauchsgüter
| ID | Gegenstand | Stufe | Mesh | Phase | Hinweis |
|---|---|:-:|:-:|:-:|---|
| `item_bandage` | Verband | T1 | – | 3 | Heilung über Zeit |
| `item_salve` | Kräutersalbe | T2 | – | 3 | stärkere Heilung |
| `item_antidote` | Gegengift | T3 | – | 4 | Biom mit Gift |
| `item_firestarter` | Zunder | T1 | – | 2 | Lagerfeuer entzünden |
| `item_repair_kit` | Flickzeug | T2 | – | 3 | Ausrüstung unterwegs reparieren. Haltbarkeit bestätigt, **einfach gehalten** (Vorschlag: ein Wert pro Gegenstand, Reparatur an Werkbank/Schmiede oder mit Flickzeug, kein Totalverlust) |

<a id="d37-7"></a>

### 37.7 7. Wertgegenstände, Handel, Quest
| ID | Gegenstand | Mesh | Phase | Hinweis |
|---|---|:-:|:-:|---|
| `item_hacksilver` | Hacksilber (Währung) | – | 4 | Handel bestätigt: Händler, nachts zusätzlich ein Schwarzmarkt-Händler |
| `item_contraband_*` | Schwarzmarkt-Ware (seltene Bauteile, Rezepte, Bossmaterial-Splitter) | – | 4 | nur beim Nacht-Händler; Inhalte offen |
| `item_jewelry_*` | Schmuck (Fibel, Armring) | – | 4 | nur Handelsware, **kein Ausrüstungs-Slot** (bestätigt) |
| `item_ring_*` | Ringe für die 2 Ring-Slots | – | 3 | Ring-Slots am 2026-11-12 bestätigt; am 2026-11-13 geklärt: Ringe sind die Ausnahme vom „kein Schmuck-Slot“. Nur Icon, kein Mesh |
| `item_trophy_boss_*` | Trophäe je Boss | ✓ | 4 | **eigenes Design**, platzierbar in der Siedlung |
| `item_quest_*` | Quest-Gegenstände | – | 4 | pro Questlinie |
| `item_portal_key_*` | Portal-Fokus | ✓ | 5 | für das Portalsystem im Spätspiel |

<a id="d37-8"></a>

### 37.8 8. Behälter- und Ablage-Props
| ID | Prop | Mesh | Phase | Verwendung |
|---|---|:-:|:-:|---|
| `prop_sack` | Sack | ✓ | 1 | Getreide, Mehl, Holzkohle, Fleisch |
| `prop_basket` | Korb | ✓ | 1 | Beeren, Pilze, Fasern, Harz |
| `prop_barrel` | Fass | ✓ | 2 | Met, Teer, Fisch |
| `prop_crate` | Kiste | ✓ | 2 | Leder, Stoffe, Knochen, Nieten |
| `prop_item_bundle` | Bündel für abgelegte Gegenstände | ✓ | 2 | eins für alles, Icon schwebt darüber |
| `prop_drying_rack` | Trockengestell | ✓ | 2 | Häute, Fisch |

<a id="d37-9"></a>

### 37.9 8a. Tierhaltung (bestätigt, einfach gehalten)
Tierarten passend zu den Tier-Stufen (Deutung von Claude). Die Tiere selbst sind Figuren-Assets und kommen in eine spätere Liste „Tiere und Gegner“. Hier nur, was sie für Gegenstände bedeuten:

| Stufe | Tier (Vorschlag) | liefert | Gebäude/Prop |
|---|---|---|---|
| T1 | Hühner | `item_egg`, `item_feather` | `building_coop` (Hühnerstall) |
| T2 | Schafe, Ziegen | `item_wool`, `item_milk`, `item_hide_raw` | `building_pen` (Pferch) |
| T3 | Rinder | `item_milk`, `item_hide_raw`, Zugkraft (offen) | `building_byre` (Stall) |

Einfach gehalten heißt (Vorschlag): Tiere brauchen nur Futter und einen Stall, keine Zucht-Genetik, kein Tierwohl-Zeitdruck. Ein Dorfbewohner-Job „Hirte“ kümmert sich darum.

<a id="d37-10"></a>

### 37.10 9. Budgets
| Asset-Art | Dreiecke Ziel | Obergrenze | Textur |
|---|---:|---:|---|
| Gegenstand mit Mesh (Stamm, Barren, Stein …) | 300 | 1.000 | geteiltes Item-Atlas 2048² |
| Behälter-Prop | 500 | 1.500 | Item-Atlas |
| Werkzeug | 800 | 2.000 | Waffen-Atlas + Metall-Set (aus der Waffenliste) |
| Trophäe | 2.000 | 4.000 | max. 1024² eigene Textur |
| Icon | – | – | 256² im Spiel, Entwurf 512², **einheitlicher Blickwinkel und Rahmen**, Stufenfarbe im Rahmen |

<a id="d37-11"></a>

### 37.11 10. Mengenabschätzung
| | Phase 1 | bis Phase 3 | gesamt (Plan) |
|---|---:|---:|---:|
| Gegenstände (Icons) | 5 | ca. 55 | ca. 75 |
| Gegenstände mit eigenem Mesh | 3 | ca. 12 | ca. 16 |
| Werkzeug-Meshes (Köpfe + Stiele) | 4 | ca. 16 | ca. 18 |
| Behälter-Props | 2 | 6 | 6 |

**Abgleich mit `design/inbox/04_assets_arbeitsstaetten.md`:** Dort zeigt das gefüllte Lager Beeren und Mahlzeiten als eigene Item-Meshes. Nach dieser Liste haben `item_berries` und `item_meal` **kein eigenes Mesh**. Vorschlag: Im Lager stehen sie in `prop_basket` und `prop_sack`. Deshalb sind diese beiden Behälter schon Teil von Phase 1. Das ist der einzige Zuwachs gegenüber dem Slice im Asset-Plan.

Der größte Aufwand sind die **Icons**, nicht die Modelle. Deshalb lohnt sich früh ein festes Icon-Raster (Blickwinkel, Licht, Rahmen, Stufenfarbe).

<a id="d37-12"></a>

### 37.12 11. Entscheidungen
**Beantwortet am 2026-11-12** (eingetragen in `docs/01_entscheidungen.md`):
1. Verderben: nur Rohes.
2. Haltbarkeit: ja, einfach gehalten.
3. Handel: Händler und nachts ein Schwarzmarkt-Händler.
4. Tierhaltung: ja, einfach gehalten, passend zu den Tier-Stufen.
5. Schmuck-Slot: nein.
6. Rüstung und Kleidung: eigene Liste → `design/ASSET_LIST_RUESTUNG.md`.

**Nachtrag 2026-11-12:** Feldbau ab Phase 2 (bestätigt). Verderben präzisiert (Abschnitt 4).

**Noch offen:**
1. **Verderben – Auslegung:** Spieltage? Hält trockene Lagerung unbegrenzt? Verderben auch gekochte Speisen („sonstige Lebensmittel 3 Tage“)?
2. Was verkauft der **Schwarzmarkt-Händler**, und gibt es dafür ein Risiko (z. B. Ruf in der Siedlung)?
3. Sind die Tierarten pro Stufe (Hühner → Schafe/Ziegen → Rinder) so richtig gedeutet?


[↑ zum Inhaltsverzeichnis](#inhaltsverzeichnis)

<a id="d38"></a>

## 38. Prompt-Standard für KI-erzeugte Assets (Stand 2026-11-12)

> Quelle: `design/PROMPT_STANDARD.md`

*Verbindlich für alle Prompts, mit denen Konzeptbilder, Texturen, Icons und 3D-Modelle erzeugt werden – egal ob ChatGPT, Codex, Claude oder der Nutzer sie schreibt.*
*Ziel des Nutzers: Prompts so genau, dass das Ergebnis der Vorgabe so nahe wie möglich kommt.*

<a id="d38-1"></a>

### 38.1 1. Grundsatz und ehrliche Grenze
Ein perfekter Prompt garantiert **kein** perfektes Ergebnis. KI-Generatoren halten sich nur ungefähr an Zahlen, Maße und Pixelpositionen. Genauigkeit entsteht deshalb aus **vier Stufen zusammen**:

1. **Präziser Prompt** nach diesem Standard (alle Pflichtfelder, feste Stil- und Ausschlussblöcke).
2. **Referenzbild statt reinem Text:** Für 3D immer zuerst ein orthografisches Konzeptblatt erzeugen und daraus das Modell (Bild-zu-3D). Text-zu-3D nur als Notlösung.
3. **Automatische Nachbearbeitung** in Blender per Skript: Maßstab, Ursprung, Dreiecke, LODs, UVs. Was die KI ungenau macht, korrigiert das Skript exakt.
4. **Messbare Abnahme** (Abschnitt 7): Nur was alle Kriterien erfüllt, kommt ins Projekt.

**Sprache:** Die Prompts selbst schreiben wir auf **Englisch**, weil die Generatoren Englisch am zuverlässigsten umsetzen. Steckbrief, Begründung und Abnahme bleiben auf Deutsch.

**Ein Prompt, ein Objekt:** Nie mehrere Assets in einem Prompt. Varianten (Stufen, Zustände) bekommen eigene Prompts, die sich nur im Variantenfeld unterscheiden.

<a id="d38-2"></a>

### 38.2 2. Pflichtfelder jedes Prompt-Pakets
Jedes Asset bekommt ein **Prompt-Paket** mit diesen Feldern. Fehlt eins, ist das Paket unvollständig.

| Feld | Inhalt | Beispiel |
|---|---|---|
| ID | Asset-ID aus den Listen | `item_log` |
| Ziel-Werkzeug | Bildgenerator, Bild-zu-3D, Texturgenerator | Bild-zu-3D |
| Zweck | wo und wie oft im Spiel sichtbar, aus welcher Distanz | wird getragen und im Lager gestapelt, Nah- und Vogelperspektive |
| Stilblock | **wörtlich** Block S1 aus Abschnitt 3 | – |
| Objekt | was genau, in einem Satz | a single felled pine log |
| Form und Silhouette | Grundform, markante Merkmale, Symmetrie | straight cylinder, slight taper, two clean saw cuts |
| Proportionen | **Verhältnisse statt Zentimeter** (Generatoren verstehen keine cm) | length is 6.5 times the diameter |
| Maße (Soll) | echte Maße in cm, nur für die Nachbearbeitung | 200 × Ø 30 cm |
| Materialien | Material je Bereich mit Hex-Farbe aus der Palette | bark #5F4833, cut face #B08A5C |
| Oberfläche und Abnutzung | Grad der Abnutzung auf einer Skala 0–3 | wear level 1: subtle |
| Stufe/Zustand | T1–T6, `fresh`/`worn`, voll/leer | – |
| Ansicht und Kamera | Blickwinkel, Brennweite, Ausrichtung | orthographic front, side, top |
| Licht | immer gleich: Block L1 | – |
| Hintergrund | immer gleich: Block H1 | – |
| Ausschlüsse | **wörtlich** Block N1 plus objektbezogene Ausschlüsse | no branches, no moss |
| Varianten | Anzahl der Durchläufe, Seed, falls das Werkzeug ihn anbietet | 4 Durchläufe, Seed notieren |
| Abnahme | Kriterien aus Abschnitt 7, ausgefüllt | – |
| Nachbearbeitung | welches Skript, welche Zielwerte | `normalize_import.py`, 200 cm, Ziel 300 Dreiecke |
| KI-Nachweis | Werkzeug, Version, Datum, Lizenzstufe des Kontos | für `design/asset_log.md` |

<a id="d38-3"></a>

### 38.3 3. Feste Textblöcke (wörtlich verwenden, nie umformulieren)

#### S1 – Stilblock Klarform-Saga
```
Stylized Norse early-medieval game asset in the "Klarform Saga" art direction:
broad, calm primary shapes with only a few strong details, a clean and instantly
readable silhouette, hand-crafted feel with subtle natural wear, matte surfaces.
Limited color palette: fjord green #3E6655, slate blue #607C8B, bone white #E5D8BC,
ochre #D2A64F, brick red #A95D48, fresh wood #8E6B46, aged wood #5F4833,
linen #D8C9A8. Must stay readable from a top-down view and at low graphics settings.
```

#### L1 – Licht
```
Soft, even, neutral daylight from the upper left, no colored light, no rim light,
no dramatic shadows, no baked ambient occlusion darker than 20 percent.
```

#### H1 – Hintergrund
```
Isolated on a plain flat neutral grey background (#808080), no ground plane,
no cast shadow on the background, object fully inside the frame with margin.
```

#### N1 – Ausschlüsse (Negativ-Prompt)
```
text, letters, numbers, runes used as decoration, watermark, logo, signature,
frame, border, UI elements, multiple objects, cropped object, background scenery,
ground, grass, horns on helmets, glowing effects, magic particles, modern objects,
screws, plastic look, photoscan look, high-frequency noise, heavy grime, blood,
resemblance to existing video game assets or characters
```
Werkzeuge ohne eigenes Negativ-Feld: Block N1 mit „Avoid:“ an das Ende des Prompts setzen.

#### Abnutzungsskala
| Stufe | Bedeutung | Englisch im Prompt |
|---|---|---|
| 0 | neu, sauber | wear level 0: brand new, clean edges |
| 1 | leicht benutzt (Standard) | wear level 1: subtle edge wear, slightly weathered |
| 2 | deutlich benutzt (`worn`) | wear level 2: visible wear, scratches, faded color |
| 3 | stark beschädigt | wear level 3: heavily damaged (nur auf ausdrückliche Anweisung) |

<a id="d38-4"></a>

### 38.4 4. Vorlagen nach Asset-Typ

#### V1 – Konzeptblatt (Grundlage für jedes 3D-Modell)
```
[S1]
Orthographic reference sheet of [OBJEKT], shown three times side by side at
identical scale: front view, side view, top view. No perspective distortion.
Shape: [FORM UND SILHOUETTE]. Proportions: [PROPORTIONEN].
Materials: [MATERIALIEN MIT HEX].
Surface: [ABNUTZUNGSSTUFE]. Variant: [STUFE/ZUSTAND].
[L1] [H1]
Avoid: [N1], [OBJEKTBEZOGENE AUSSCHLÜSSE]
```

#### V2 – Bild-zu-3D
Eingabe: das abgenommene Konzeptblatt, nur die **Vorderansicht** oder alle drei Ansichten, je nach Werkzeug.
Einstellungen (werkzeugneutral):
```
Single watertight object, no base or pedestal, centered, real-world proportions
as in the reference, PBR textures (base color, normal, roughness), no baked
lighting in the base color, symmetry: [ja/nein], target polycount: [ZIEL × 3]
(the script reduces it afterwards).
```

#### V3 – Kachelbare Textur (je Trim-Streifen ein Prompt)
Generatoren treffen keine exakten Pixelzeilen. Deshalb wird **jeder Streifen des Trim-Sheets einzeln** als nahtlos kachelbare Textur erzeugt. Ein Skript setzt die Streifen danach pixelgenau zum Atlas zusammen.
```
[S1]
Seamless tileable texture, flat orthographic top-down scan of [MATERIAL],
[AUSRICHTUNG, z. B. boards running horizontally], exactly [ANZAHL] [ELEMENTE]
stacked vertically filling the full height, each the same height, [FARBEN MIT HEX],
[ABNUTZUNGSSTUFE], evenly lit, no perspective, no vignette, no shadows,
tiles seamlessly on the left and right edges.
Avoid: [N1], visible seams, repeating stains
```

#### V4 – Icon
Alle Icons gleich, damit das Inventar ruhig wirkt.
```
[S1]
Game inventory icon of [OBJEKT], three-quarter view from the front left,
camera elevation 30 degrees, object rotated 35 degrees, object fills 80 percent
of the canvas, centered, slight dark outline (#2E2B28, 2 px at 512 px),
[MATERIALIEN MIT HEX], [STUFE].
[L1] Transparent or plain #808080 background.
Avoid: [N1], frame, tier color, shadow outside the object
```
Der **Stufen-Rahmen wird nicht generiert**, sondern in der Engine darübergelegt.

<a id="d38-5"></a>

### 38.5 5. Ablauf
1. Claude oder ChatGPT schreibt das Prompt-Paket nach Abschnitt 2.
2. Der Nutzer prüft es (oder gibt es frei), dann wird erzeugt: zuerst V1 (Konzeptblatt).
3. Abnahme des Konzeptblatts (Abschnitt 7, Teil A). Erst dann V2/V3/V4.
4. Claude normalisiert das Ergebnis per Skript und prüft Teil B.
5. Eintrag ins Asset-Log mit Werkzeug, Version, Datum und Lizenzstufe.

<a id="d38-6"></a>

### 38.6 6. Ablage
- Prompt-Pakete: `design/prompts/<id>.md`
- Rohausgaben der KI: `Assets/_Raw/<id>/` (nicht ins Spiel)
- Abgenommene, normalisierte Assets: `Assets/<Gruppe>/` (Struktur und `manifest.json` wie von `tools/blender/build_assets.py` erzeugt)

<a id="d38-7"></a>

### 38.7 7. Abnahme-Kriterien (messbar)
**Teil A – Bild (Konzeptblatt, Textur, Icon):**
| Kriterium | Grenze |
|---|---|
| Silhouette erkennbar | bei 64 × 64 px noch eindeutig |
| Farben | Hauptflächen innerhalb der Palette (Farbabstand ΔE ≤ 10 zum nächsten Palettenwert) |
| Ansichten (V1) | drei Ansichten, gleicher Maßstab, keine Perspektive |
| Kachelbarkeit (V3) | keine sichtbare Naht bei 3 × 3-Kachelung |
| Ausschlüsse | kein Element aus N1 sichtbar |

**Teil B – 3D nach der Nachbearbeitung:**
| Kriterium | Grenze |
|---|---|
| Maße | ±2 % zum Soll |
| Ursprung | wie im Steckbrief |
| Dreiecke | ≤ Ziel, sicher ≤ Obergrenze |
| Geometrie | geschlossen, keine schwebenden Teile, keine umgedrehten Normalen |
| Texturen | Größe laut Budget, kein eingebranntes Licht |
| LODs | 3 Stufen erzeugt |

<a id="d38-8"></a>

### 38.8 8. Ausgearbeitete Beispiele
- `design/prompts/item_log.md` – 3D über Konzeptblatt und Bild-zu-3D
- `design/prompts/trim_wood.md` – Trim-Sheet aus fünf Streifen
- `design/prompts/ui_icon_res_wood.md` – Icon


[↑ zum Inhaltsverzeichnis](#inhaltsverzeichnis)

<a id="d39"></a>

## 39. Prompt-Paket `item_log` – Baumstamm (Phase 1, Prio A)

> Quelle: `design/prompts/item_log.md`

*Nach `design/PROMPT_STANDARD.md`. Status: Prompt nicht verwendet. Das Asset wurde am 2026-11-12 **prozedural in Blender** erzeugt (`Assets/Items/SM_Item_Log.fbx`, 70 Dreiecke, 200 × Ø 30 cm). Der Prompt bleibt für eine spätere KI-Fassung erhalten.*

| Feld | Inhalt |
|---|---|
| ID | `item_log` → `SM_Item_Log` |
| Ziel-Werkzeug | 1. Bildgenerator (Konzeptblatt V1) · 2. Bild-zu-3D (V2) |
| Zweck | Wird von Dorfbewohnern getragen (Schulter), liegt am Holzfäller-Platz und im Lager gestapelt (3–12 Stück). Sichtbar aus 2–30 m und in der Vogelperspektive. |
| Objekt | a single felled and cleanly cut pine log, bark on |
| Form und Silhouette | straight cylinder, very slight taper toward one end, both ends cut flat and perpendicular with a saw, two small knot bumps where branches were removed, no branches |
| Proportionen | length is 6.5 times the thick-end diameter; the thin end diameter is 85 percent of the thick end |
| Maße (Soll) | 200 cm lang, Ø 30 cm (dickes Ende), Ø 25,5 cm (dünnes Ende) – **Vorschlag von Claude**, im Asset-Plan bisher nicht festgelegt |
| Materialien | bark aged wood #5F4833 with slightly lighter ridges #79664C; cut faces end grain #B08A5C with 8–10 darker growth rings #8E6B46 and a small darker heart; knots #4A3726 |
| Oberfläche und Abnutzung | wear level 1: subtle (bark slightly rough, cut faces clean) |
| Stufe/Zustand | T1, `fresh` |
| Ansicht und Kamera | V1: orthographic front (log lying horizontally, length left to right), side (looking at the thick cut face), top |
| Licht / Hintergrund | L1 / H1 |
| Ausschlüsse | N1 + `branches, leaves, moss, axe marks, split wood, resin drops, insects` |
| Varianten | 4 Durchläufe für V1, bestes Blatt auswählen; Seed notieren |
| Nachbearbeitung | Skript `tools/blender/normalize_import.py` (noch zu schreiben): Länge auf 200 cm skalieren, Ursprung **Mitte des Stamms**, Längsachse X, Dreiecke auf Ziel 300 reduzieren (Obergrenze 1.000), 3 LODs, Textur auf 1024² ins Item-Atlas |
| KI-Nachweis | Werkzeug, Version, Datum, Lizenzstufe → `design/asset_log.md` |

<a id="d39-1"></a>

### 39.1 Prompt 1 – Konzeptblatt (V1)
```
Stylized Norse early-medieval game asset in the "Klarform Saga" art direction:
broad, calm primary shapes with only a few strong details, a clean and instantly
readable silhouette, hand-crafted feel with subtle natural wear, matte surfaces.
Limited color palette: fjord green #3E6655, slate blue #607C8B, bone white #E5D8BC,
ochre #D2A64F, brick red #A95D48, fresh wood #8E6B46, aged wood #5F4833,
linen #D8C9A8. Must stay readable from a top-down view and at low graphics settings.

Orthographic reference sheet of a single felled and cleanly cut pine log, bark on,
shown three times side by side at identical scale: front view with the log lying
horizontally from left to right, side view looking straight at the thick cut face,
top view. No perspective distortion.
Shape: straight cylinder, very slight taper toward the right end, both ends cut flat
and perpendicular with a saw, two small knot bumps where branches were removed,
no branches.
Proportions: length is 6.5 times the diameter of the thick end; the thin end
diameter is 85 percent of the thick end.
Materials: bark in aged wood #5F4833 with slightly lighter ridges #79664C;
cut faces show end grain #B08A5C with 8 to 10 darker growth rings #8E6B46 and a
small darker heart; knots #4A3726.
Surface: wear level 1: subtle edge wear, slightly weathered. Variant: fresh.

Soft, even, neutral daylight from the upper left, no colored light, no rim light,
no dramatic shadows, no baked ambient occlusion darker than 20 percent.
Isolated on a plain flat neutral grey background (#808080), no ground plane,
no cast shadow on the background, object fully inside the frame with margin.

Avoid: text, letters, numbers, runes used as decoration, watermark, logo, signature,
frame, border, UI elements, multiple objects, cropped object, background scenery,
ground, grass, horns on helmets, glowing effects, magic particles, modern objects,
screws, plastic look, photoscan look, high-frequency noise, heavy grime, blood,
resemblance to existing video game assets or characters, branches, leaves, moss,
axe marks, split wood, resin drops, insects
```

<a id="d39-2"></a>

### 39.2 Prompt 2 – Bild-zu-3D (V2)
Eingabe: Vorderansicht aus dem abgenommenen Konzeptblatt (oder alle drei Ansichten, falls das Werkzeug Mehrfachansichten annimmt).
```
Single watertight object, no base or pedestal, centered, real-world proportions
as in the reference, PBR textures (base color, normal, roughness), no baked
lighting in the base color, symmetry: no, target polycount: 900.
```

<a id="d39-3"></a>

### 39.3 Abnahme
| Teil | Kriterium | Ergebnis |
|---|---|---|
| A | bei 64 px als Stamm erkennbar | offen |
| A | Rinde und Schnittfläche innerhalb ΔE ≤ 10 der Palette | offen |
| A | drei Ansichten, gleicher Maßstab, keine Perspektive | offen |
| A | keine Äste, kein Moos, kein Text | offen |
| B | 200 cm ± 2 %, Ø 30 cm ± 2 % | offen |
| B | ≤ 300 Dreiecke (max. 1.000), 3 LODs | offen |
| B | geschlossen, Ursprung Mitte, Längsachse X | offen |


[↑ zum Inhaltsverzeichnis](#inhaltsverzeichnis)

<a id="d40"></a>

## 40. Prompt-Paket `trim_wood` – Holz-Trim-Sheet (Phase 1, Prio A)

> Quelle: `design/prompts/trim_wood.md`

*Nach `design/PROMPT_STANDARD.md`. Status: Prompts nicht verwendet. Das Trim-Sheet wurde am 2026-11-12 **prozedural in Blender** erzeugt (`Assets/Textures/T_Trim_Wood_BC/_N/_ORM.png`, Layout exakt wie unten). Die Prompts bleiben für eine spätere KI-Fassung erhalten.*
*Layout verbindlich (umgesetzt in `tools/blender/build_wood_kit.py`, `STRIPS`): 2048 × 2048 px, 1 UV = 200 cm, u kachelt waagerecht.*

| Feld | Inhalt |
|---|---|
| ID | `trim_wood` → `T_Trim_Wood_BC`, `T_Trim_Wood_N`, `T_Trim_Wood_ORM` |
| Ziel-Werkzeug | Bild- oder Texturgenerator (V3), **fünf getrennte Prompts**, einer je Streifen |
| Zweck | Oberflächen aller Holz-Bauteile (Wand, Boden, Dach, Pfosten, Tür …). Größte sichtbare Fläche der Siedlung, aus 1–50 m. |
| Stufe/Zustand | T1. `fresh` und `worn` entstehen später über Materialinstanzen, **nicht** über eigene Texturen |
| Licht / Hintergrund | gleichmäßig, ohne Schatten (in V3 enthalten); kein Hintergrund (Textur füllt alles) |
| Ausschlüsse | N1 + `visible seams, repeating stains, nails, paint, carvings, knots larger than 4 cm` |
| Varianten | je Streifen 4 Durchläufe, beste Kachel wählen |
| Nachbearbeitung | Skript setzt die Streifen zusammen: jeden auf 2048 px Breite skalieren, auf die exakte Streifenhöhe bringen (Stauchung ≤ 12 % erlaubt), untereinander anordnen. **Normal und ORM:** aus dem Werkzeug, wenn es PBR-Karten liefert; sonst erzeugt Claude sie per Skript aus der Helligkeit (Näherung, Qualität prüfen) |
| KI-Nachweis | je Streifen ein Eintrag in `design/asset_log.md` |

<a id="d40-1"></a>

### 40.1 Streifen-Layout (Zielpositionen im fertigen Atlas)
| Streifen | v-Bereich | Pixelzeilen von oben (2048 px) | reale Höhe | Inhalt | Seitenverhältnis beim Erzeugen |
|---|---|---|---|---|---|
| plank | 0,70–1,00 | 0–614 | 60 cm | 4 Bretter à 15 cm, waagerecht | 10 : 3 (notfalls 3 : 1) |
| timber | 0,45–0,70 | 614–1126 | 50 cm | Balkenfläche, Maserung waagerecht | 4 : 1 |
| end | 0,35–0,45 | 1126–1331 | 20 cm | Hirnholz, Jahresringe | 10 : 1 (notfalls 8 : 1) |
| dark | 0,10–0,35 | 1331–1843 | 50 cm | 4 dunkle Bretter à 12,5 cm | 4 : 1 |
| metal | 0,00–0,10 | 1843–2048 | 20 cm | dunkles, geschmiedetes Eisen | 10 : 1 (notfalls 8 : 1) |

<a id="d40-2"></a>

### 40.2 Prompt 1 – plank
```
Stylized Norse early-medieval game asset in the "Klarform Saga" art direction:
broad, calm primary shapes with only a few strong details, a clean and instantly
readable silhouette, hand-crafted feel with subtle natural wear, matte surfaces.
Limited color palette: fjord green #3E6655, slate blue #607C8B, bone white #E5D8BC,
ochre #D2A64F, brick red #A95D48, fresh wood #8E6B46, aged wood #5F4833,
linen #D8C9A8. Must stay readable from a top-down view and at low graphics settings.

Seamless tileable texture, flat orthographic top-down scan of hand-split pine
planks, boards running horizontally, exactly 4 boards stacked vertically filling
the full height, each the same height, separated by thin dark gaps #4A3726,
board color fresh wood #8E6B46 with gentle variation between boards of at most
10 percent brightness, soft straight wood grain along the boards, two or three
small knots in total, wear level 1: subtle edge wear, slightly weathered,
evenly lit, no perspective, no vignette, no shadows, tiles seamlessly on the
left and right edges. Aspect ratio 10:3.

Avoid: text, letters, numbers, runes used as decoration, watermark, logo, signature,
frame, border, UI elements, multiple objects, cropped object, background scenery,
ground, grass, horns on helmets, glowing effects, magic particles, modern objects,
screws, plastic look, photoscan look, high-frequency noise, heavy grime, blood,
resemblance to existing video game assets or characters, visible seams,
repeating stains, nails, paint, carvings, knots larger than 4 cm
```

<a id="d40-3"></a>

### 40.3 Prompt 2 – timber
Wie Prompt 1, aber Objektteil ersetzen durch:
```
Seamless tileable texture, flat orthographic scan of the side face of a single
hewn pine beam, grain running horizontally, faint adze marks every 30 cm,
color #7A5A3A with slightly darker streaks #5F4833, no gaps, no board edges,
wear level 1: subtle edge wear, slightly weathered, evenly lit, no perspective,
no vignette, no shadows, tiles seamlessly on the left and right edges.
Aspect ratio 4:1.
```

<a id="d40-4"></a>

### 40.4 Prompt 3 – end
```
Seamless tileable texture, flat orthographic scan of a row of sawn pine end grain,
clearly visible concentric growth rings, base #B08A5C, rings #8E6B46,
fine radial cracks, evenly lit, no perspective, no vignette, no shadows,
tiles seamlessly on the left and right edges. Aspect ratio 10:1.
```

<a id="d40-5"></a>

### 40.5 Prompt 4 – dark
```
Seamless tileable texture, flat orthographic top-down scan of old weathered pine
planks, boards running horizontally, exactly 4 boards stacked vertically filling
the full height, each the same height, separated by thin very dark gaps #33261A,
board color aged wood #5F4833, slightly greyed, wear level 2: visible wear,
scratches, faded color, evenly lit, no perspective, no vignette, no shadows,
tiles seamlessly on the left and right edges. Aspect ratio 4:1.
```

<a id="d40-6"></a>

### 40.6 Prompt 5 – metal
```
Seamless tileable texture, flat orthographic scan of hand-forged dark iron strap,
hammer-dimpled surface, color #2E2B28 with slightly lighter worn high points
#4A4744, faint rust tint only in small spots #6B4A33, matte, evenly lit,
no perspective, no vignette, no shadows, tiles seamlessly on the left and
right edges. Aspect ratio 10:1.
```
(Für Prompt 2–5 jeweils Stilblock S1 davor und Ausschlussblock wie in Prompt 1 danach setzen.)

<a id="d40-7"></a>

### 40.7 Abnahme
| Teil | Kriterium | Ergebnis |
|---|---|---|
| A | jede Kachel nahtlos bei 3 × 3-Kachelung | offen |
| A | plank und dark: genau 4 Bretter, gleich hoch (± 5 %) | offen |
| A | Farben innerhalb ΔE ≤ 10 der Sollwerte | offen |
| A | keine Nägel, keine Farbe, keine Schnitzereien, kein Text | offen |
| B | fertiger Atlas 2048 × 2048, Streifengrenzen exakt laut Tabelle | offen |
| B | auf den Bauteilen aus `Assets/Buildings/WoodKit/` getestet: Bretter laufen richtig, keine Verzerrung | offen |


[↑ zum Inhaltsverzeichnis](#inhaltsverzeichnis)

<a id="d41"></a>

## 41. Prompt-Paket `ui_icon_res_wood` – Icon Baumstamm (Phase 1, Prio A)

> Quelle: `design/prompts/ui_icon_res_wood.md`

*Nach `design/PROMPT_STANDARD.md`. Status: Prompt nicht verwendet. Das Icon wurde am 2026-11-12 **in Blender gerendert** (`Assets/UI/Icons/T_Icon_Res_Wood.png`, Kamera und Kontur wie unten, Füllgrad 0,80). Der Prompt bleibt für eine spätere KI-Fassung erhalten.*
*Dieses Icon legt das **Icon-Raster** fest. Alle weiteren Icons verwenden dieselbe Kamera, dasselbe Licht und dieselbe Kontur.*

| Feld | Inhalt |
|---|---|
| ID | Icon zu `item_log` → `T_Icon_Res_Wood` |
| Ziel-Werkzeug | Bildgenerator (V4) |
| Zweck | Inventar, Lager-UI, Bauanforderungen. Angezeigt mit 48–96 px, muss bei 48 px eindeutig sein. |
| Objekt | three stacked pine logs (two at the bottom, one on top) |
| Form und Silhouette | compact triangular stack, cut faces turned toward the viewer, logs identical in size |
| Proportionen | each log is 4 times as long as its diameter; the stack is as wide as it is tall |
| Materialien | bark #5F4833 with ridges #79664C; cut faces #B08A5C with rings #8E6B46 |
| Oberfläche und Abnutzung | wear level 1 |
| Stufe/Zustand | T1 (Rahmen in Stufenfarbe **nicht** im Bild, kommt aus der Engine) |
| Ansicht und Kamera | three-quarter view from the front left, elevation 30°, object rotated 35°, fills 80 % of the canvas |
| Licht / Hintergrund | L1 / transparent oder #808080 |
| Ausschlüsse | N1 + `frame, tier color, shadow outside the object, axe, branches, moss` |
| Varianten | 4 Durchläufe, 512 × 512 px |
| Nachbearbeitung | Hintergrund freistellen, auf 256 × 256 px verkleinern, Kontur prüfen, als `T_Icon_Res_Wood` (UI-Textur, keine Mipmaps) |
| KI-Nachweis | Eintrag in `design/asset_log.md` |

<a id="d41-1"></a>

### 41.1 Prompt (V4)
```
Stylized Norse early-medieval game asset in the "Klarform Saga" art direction:
broad, calm primary shapes with only a few strong details, a clean and instantly
readable silhouette, hand-crafted feel with subtle natural wear, matte surfaces.
Limited color palette: fjord green #3E6655, slate blue #607C8B, bone white #E5D8BC,
ochre #D2A64F, brick red #A95D48, fresh wood #8E6B46, aged wood #5F4833,
linen #D8C9A8. Must stay readable from a top-down view and at low graphics settings.

Game inventory icon of three stacked pine logs, two at the bottom and one on top,
forming a compact triangular stack, cut faces turned toward the viewer, all logs
identical in size, each log 4 times as long as its diameter, the stack as wide as
it is tall. Three-quarter view from the front left, camera elevation 30 degrees,
object rotated 35 degrees, object fills 80 percent of the canvas, centered,
slight dark outline #2E2B28, 2 px at 512 px.
Materials: bark in aged wood #5F4833 with lighter ridges #79664C, cut faces end
grain #B08A5C with darker growth rings #8E6B46. Surface: wear level 1: subtle edge
wear, slightly weathered.
Soft, even, neutral daylight from the upper left, no colored light, no rim light,
no dramatic shadows, no baked ambient occlusion darker than 20 percent.
Transparent or plain #808080 background. Square 1:1, 512 x 512 px.

Avoid: text, letters, numbers, runes used as decoration, watermark, logo, signature,
frame, border, UI elements, multiple objects, cropped object, background scenery,
ground, grass, horns on helmets, glowing effects, magic particles, modern objects,
screws, plastic look, photoscan look, high-frequency noise, heavy grime, blood,
resemblance to existing video game assets or characters, frame, tier color,
shadow outside the object, axe, branches, moss
```

<a id="d41-2"></a>

### 41.2 Abnahme
| Kriterium | Ergebnis |
|---|---|
| bei 48 px als Holz erkennbar und vom Bretter-Icon unterscheidbar | offen |
| Kamera 3/4 von vorne links, ca. 30° von oben | offen |
| Objekt füllt ca. 80 %, mittig | offen |
| Kontur vorhanden und gleichmäßig | offen |
| kein Rahmen, kein Text, kein Hintergrundmotiv | offen |
| Farben innerhalb ΔE ≤ 10 der Palette | offen |


[↑ zum Inhaltsverzeichnis](#inhaltsverzeichnis)

<a id="d42"></a>

## 42. Asset-Log

> Quelle: `design/asset_log.md`

Jedes KI-generierte oder fremde Asset hier eintragen (Steam verlangt Angaben zu KI-Inhalten; Lizenzen sind später nicht mehr rekonstruierbar).

| Datei | Typ | Werkzeug/Quelle | Datum | Lizenz | KI-generiert? | Notiz |
|---|---|---|---|---|---|---|
| `design/inbox/01_art_direction.md` | Konzepttext (Art Direction) | OpenAI Codex | 2026-11-12 | keine externen Assets (laut Lieferung) | ja | Stilwahl Klarform-Saga |
| `design/inbox/03_assets_baukasten.md` | Konzepttext (Asset-Steckbriefe) | OpenAI Codex | 2026-11-12 | keine externen Assets (laut Lieferung) | ja | Holzbaukasten, Prio A; überarbeitet nach Review 01 (2026-11-12) |
| `design/inbox/04_assets_arbeitsstaetten.md` | Konzepttext (Asset-Steckbriefe) | OpenAI Codex | 2026-11-12 | keine externen Assets (laut Lieferung) | ja | Arbeitsstätten, Prio A; überarbeitet nach Review 01, 2. Überarbeitung: Lager-Füllung über `prop_basket`/`prop_sack` (2026-11-12) |
| `design/inbox/05_assets_armbrust.md` | Konzepttext (Waffen-Ergänzung) | OpenAI Codex | 2026-11-12 | keine externen Assets; verlinkte Museumsquellen nur als Kontext | ja | Armbrust, Bolzen |
| `design/inbox/06_assets_ruestung_kleidung.md` | Konzepttext (Rüstung/Kleidung) | OpenAI Codex | 2026-11-12 | keine externen Assets; verlinkte Museumsquellen nur als Kontext | ja | Slots weichen von der Nutzer-Entscheidung ab, siehe Review 02 |
| `design/inbox/07_assetentscheidungen_2026-11-12.md` | Zusammenfassung | OpenAI Codex | 2026-11-12 | – | ja | Antworten des Nutzers stehen in `docs/01_entscheidungen.md` |
| `art/wood_kit/SM_Building_*.fbx` (10 Module) | 3D-Mesh, prozedural | Blender 5.3 + Skript `tools/blender/build_wood_kit.py` (Skript von Claude Code geschrieben) | 2026-11-12 | eigenes Werk, keine Fremd-Assets | indirekt (KI-geschriebenes Skript) | Platzhalter-Geometrie in Endmaßen |
| `art/wood_kit/T_Trim_Wood_Placeholder.png`, `T_Turf_Placeholder.png` | Textur, prozedural | dasselbe Skript (Rauschen + Linien) | 2026-11-12 | eigenes Werk | indirekt (KI-geschriebenes Skript) | nur Platzhalter, wird ersetzt |
| `art/wood_kit/preview_*.png` | Vorschaubild (Render) | Blender Workbench | 2026-11-12 | eigenes Werk | indirekt | nur Doku |
| `Assets/` (alle Dateien, siehe `Assets/manifest.json`) | Meshes, Texturen (BC/N/ORM), Icon, .blend, Vorschauen – prozedural | Blender 5.3 + `tools/blender/build_assets.py` (Skript von Claude Code geschrieben) | 2026-11-12 | eigenes Werk, keine Fremd-Assets, kein Bildgenerator | indirekt (KI-geschriebenes Skript) | ersetzt die Platzhalter aus `art/wood_kit/` (dort am 2026-11-12 gelöscht, in der Git-Historie erhalten) |
| `Assets/Nature/Rocks/*.fbx`, `Assets/Nature/Plants/SM_Branches.fbx`, `SM_Nettle.fbx`, `SM_NettleCut.fbx`, Texturen `T_RockMoss_*`, `T_DryBranches_*`, `T_Nettle_*` | 3D-Scans (Fotogrammetrie), zusammengesetzt, verkleinert, Texturen umgerechnet | Poly Haven: `rock_moss_set_01`, `dry_branches_medium_01`, `nettle_plant` (glTF 1k); Aufbereitung `tools/blender/process_nature.py` (Skript von Claude Code) | 2026-11-13 | CC0 1.0 (polyhaven.com/license) | nein (Fotos/Scans); Aufbereitung per KI-geschriebenem Skript | Download und Quellenliste: `tools/cc0/fetch_cc0.py`, `D:/gamer/cc0_quellen/sources.json` (außerhalb des Repos) |
| `Assets/Nature/Textures/T_BarkPine_*`, `T_BarkBrown_*` | Fototexturen Rinde | Poly Haven `pine_bark`, `bark_brown_02` (2k) | 2026-11-13 | CC0 1.0 | nein | Normal-Karte von OpenGL auf DirectX umgerechnet |
| `Assets/Nature/Trees/*.fbx`, `Assets/Nature/Plants/SM_Bush*.fbx`, Texturen `T_Leaves*`, `T_NeedlesPine_*`, `T_BarkBirch_*`, `T_EndGrain_*`, `T_Fruit_*` | Bäume, Sträucher, Setzlinge, Stümpfe, Blatt-/Nadelbüschel – prozedural | Blender 5.3 + `tools/blender/build_trees.py` (Skript von Claude Code); Blattoberfläche aus ambientCG `LeafSet010` (CC0); Kiefernnadel-Triebe aus dem Zweig-Atlas von Poly Haven `pine_tree_01` (nur Bilder twig_diff/alpha/nor_dx/arm 2k, CC0 1.0, polyhaven.com/license), neu zu Büscheln zusammengesetzt (2026-11-14) | 2026-11-13 | eigenes Werk + CC0 1.0 (docs.ambientcg.com/license) | indirekt (KI-geschriebenes Skript), kein Bild- oder 3D-Generator | Birkenrinde und Blattformen erzeugt; heruntergeladen, aber nicht verwendet: Poly Haven `rock_09`, ambientCG `LeafSet019` (Thuja statt Kiefer); `Foliage006` inzwischen für die Grasbüschel verwendet (siehe Zeile Grasbüschel) |
| `Assets/Nature/Grass/SM_GrassClump_*.fbx` (Short, Medium, Tall, Sparse), Texturen `T_Grass_BC`, `T_Grass_N`, `T_Grass_ORM` | Grasbüschel aus ausgeschnittenen Foto-Halmen (Karten), Halm-Atlas neu gepackt (512 x 2048) | ambientCG `Foliage006` (2K-JPG: Color, Opacity, NormalDX, Roughness); Aufbereitung Blender 5.3 + `tools/blender/build_grass.py` (Skript von Claude Code) | 2026-11-13 | CC0 1.0 (docs.ambientcg.com/license) | nein (Foto); Aufbereitung per KI-geschriebenem Skript, kein Bild- oder 3D-Generator | AO-Verlauf zum Halmansatz erzeugt; die zwei Stängel mit Seitenblättern aus dem Atlas nicht verwendet |
| `Assets/Nature/Ground/T_GroundForest_*`, `T_GroundNeedleSand_*`, `T_GroundSand_*`, `T_GroundMoss_*`, `T_GroundDirt_*` (je BC, N, ORM; 2048 px) | Fototexturen Boden (kachelbar), ORM mit Höhenkarte im Alphakanal | Poly Haven `forrest_ground_03`, `forrest_sand_01`, `forest_leaves_02`, `dirt_floor` (2k); ambientCG `Ground033` (2K-JPG); Aufbereitung `tools/blender/process_nature.py` („ground“ in `tools/cc0/nature.json`, Skript von Claude Code) | 2026-11-13 | CC0 1.0 (polyhaven.com/license, docs.ambientcg.com/license) | nein (Fotos/Scans); Aufbereitung per KI-geschriebenem Skript | für `M_Gamer_Ground` (`tools/unreal/build_ground_material.py`); Normal-Karten schon DirectX; .blend-Dateien der Quellen nicht geöffnet |
| `Assets/Nature/Trees/SM_DeadStump.fbx`, `SM_DeadTrunk.fbx`, `Assets/Nature/Plants/SM_Fern_*.fbx` (Large, Medium, Small), `SM_ShrubWillow*.fbx`, `SM_ShrubSprig.fbx`, `Assets/Nature/Deco/SM_SandstoneCliff.fbx`, Texturen `T_DeadStump_*`, `T_DeadTrunk_*`, `T_Fern02_*`, `T_LeavesShrub02_*`, `T_LeavesShrub04_*`, `T_Sandstone_*` | 3D-Scans (Fotogrammetrie), skaliert, Pivot am Boden, verkleinert, Kollision (UCX-Hüllen) erzeugt, Texturen umgerechnet (Normal OpenGL → DirectX, ORM) | Poly Haven `tree_stump_01`, `dead_tree_trunk`, `fern_02`, `shrub_02`, `shrub_04`, `rock_face_02` (glTF 2k); Aufbereitung `tools/blender/process_nature.py` (Skript von Claude Code) | 2026-11-13 | CC0 1.0 (polyhaven.com/license) | nein (Fotos/Scans); Aufbereitung per KI-geschriebenem Skript | Stumpf und Totholz als künftige Vorkommen (nicht per PCG verteilt); Sträucher importiert, aber nicht verteilt (wirken wie lose Zweige) |
| `Assets/Buildings/StyleTest/SM_Hut*.fbx`, `SM_WatchTower*.fbx`, Texturen `T_BldOrigHut_*`, `T_BldOrigWatchTower_*` (Stil-Probe) | 3D-Modelle (Gebäude, stilisiert) mit UCX-Kollision; Original skaliert (Tür ca. 195 cm), Texturen von 4k auf 2k verkleinert; umgebaute Fassung mit neuen Materialien, neuen UVs und gefasten Kanten | „Highlands – Fantasy 3D Buildings Pack“ von Visions Mind (visionsmind.itch.io/highlands-fantasy-pack-3d), nur FBX und PNG aus `Hut/` und `WatchTower/`; Aufbereitung Blender 5.3 + `tools/blender/restyle_buildings.py` (Skript von Claude Code) | 2026-11-14 | CC0 laut Seite (Nennung freiwillig) | nein (Fremd-Asset); Aufbereitung per KI-geschriebenem Skript | nur Stil-Probe; entpackt nach `D:/gamer/cc0_quellen/extern/highlands` (außerhalb des Repos) |
| `Assets/Buildings/StyleTest/SM_VillageHouse*.fbx`, Texturen `T_BldOrigKit*` (Stil-Probe) | 3D-Modell, aus Bauteilen zusammengesetzt (Wände, Tür, Fensterläden, Ecken, Boden, Ziegeldach als Original); umgebaute Fassung mit eigenem Reetdach, Giebelbrettern, Windbrettern und Steinsockel (Skript-Geometrie) | „Medieval Village MegaKit [Standard]“ von Quaternius, nur FBX und PNG; Aufbereitung Blender 5.3 + `tools/blender/restyle_buildings.py` (Skript von Claude Code) | 2026-11-14 | CC0 1.0 (`License_Standard.txt` im Paket) | nein (Fremd-Asset); Aufbereitung per KI-geschriebenem Skript | nur Stil-Probe; entpackt nach `D:/gamer/cc0_quellen/extern/medieval_village_megakit` |
| `Assets/Buildings/StyleTest/Textures/T_BldThatch_*`, `T_BldPlanks_*`, `T_BldWood_*`, `T_BldStone_*`, `T_BldPlaster_*`, `T_BldCloth_*` (je BC, N, ORM) | Fototexturen Gebäude (kachelbar): Farbe gedämpft, Rauheit angehoben; Stoff = Lehmputz-Textur blau eingefärbt | Poly Haven `thatch_roof_angled`, `weathered_planks`, `wood_planks`, `stone_wall`, `clay_plaster` (2k: diff, nor_dx, arm); Aufbereitung `tools/blender/restyle_buildings.py` (Skript von Claude Code) | 2026-11-14 | CC0 1.0 (polyhaven.com/license) | nein (Fotos/Scans); Aufbereitung per KI-geschriebenem Skript | Moos, Feuchte und Vergrauung kommen aus dem Material `M_Gamer_Building` (`tools/unreal/import_assets.py`), nicht aus Texturen |
| `game/Content/Characters/Mannequins/` (SKM_Manny_Simple, SKM_Quinn_Simple, SK_Mannequin, Materialien, Texturen, Rigs, `Anims/Unarmed`: MM_Idle, Walk, Jog, Jump, Fall, Land) | Spielfigur (Zwischenfigur bis MetaHuman), Skelett, Animationen | Epic Games, UE5-Mannequin aus den Engine-Vorlagen (`E:/UE58/Templates/TemplateResources/High/Characters/Content/Mannequins`), Kopie per `tools/import_characters.ps1` | 2026-11-15 | Unreal Engine EULA (Epic-Inhalt, nur in Unreal-Projekten) | nein | Ersatz für den Platzhalter-Zylinder; Waffen-Animationen (Pistol/Rifle) und ABP nicht kopiert |
| `/Game/Gamer/Characters/Anims/A_Sprint`, `A_Gather`, `A_Mine`, `A_MineGround`, `A_Hammer`; Quellen `/Game/Gamer/Characters/Source/Kevin/*` | Animationen, per IK Retargeter auf SK_Mannequin übertragen | Kevin Iglesias „Human Basic Motions FREE“ 2.4 und „Human Crafting Animations FREE“ 2.2 (FBX, männliche Fassung); entpackt nach `D:/gamer/cc0_quellen/extern/kevin_iglesias_*`; Retargeting `tools/unreal/import_characters.py` (Skript von Claude Code) | 2026-11-15 | **unklar belegt:** Asset-Store-EULA laut Auftrag (kommerziell erlaubt, kein Weiterverkauf); keviniglesias.com: Abschnitt „Lizenz“ leer (Stand 2026-11-15), Startseite: „royalty-free … commercial projects … crediting … not required“; ob die Nutzung außerhalb von Unity erlaubt ist, steht nirgends. Nutzer entschied 2026-11-15 „Kevin voll nutzen“. Klärung: support@keviniglesias.com | nein (Motion-Capture/Handanimation des Autors); Retargeting per KI-geschriebenem Skript | .blend-Dateien der Pakete nicht entpackt |
| `/Game/Gamer/Characters/Anims/A_Chop`, `A_PlantSeed`, `A_Harvest`, `A_Water`, `A_Consume`; Quellen `/Game/Gamer/Characters/Source/Quaternius/*` | Animationen, per IK Retargeter auf SK_Mannequin übertragen | Quaternius „Universal Animation Library 2 [Standard]“ (`Unreal-Godot/UAL2_Standard.glb`); entpackt nach `D:/gamer/cc0_quellen/extern/quaternius_universal_animation_library_2` | 2026-11-15 | CC0 1.0 (`License.txt` im Paket) | nein; Retargeting per KI-geschriebenem Skript | .blend nicht entpackt |
| `design/inbox/08_stilbibel_kuestenasche.md` | Konzepttext (Stilbibel „Küstenasche“, Magie „Gezeitenhall“) | vom Nutzer im Chat eingefügt, vermutlich ChatGPT | 2026-11-15 | keine externen Assets; verlinkt nur die Steam-Seite von ASKA als Stimmungsanker | vermutlich ja | Vorschlag, nicht entschieden; Verhältnis zu `design/stil_gelebter_norden.md` offen |
| `docs/15_probe_spur_der_ersten.md` | Spieltext (Probe Prolog, Logbuch, Saga-Einträge) | Claude Code (Claude Opus 5.5) | 2026-11-15 | eigenes Werk, keine fremden Texte | ja | nur Probe zur Entscheidung; vor Übernahme ins Spiel prüfen |
| `design/referenz/figuren/figuren_1..4.jpg` (+ ein weiteres Bild nur im Chat) | Referenzbilder Figuren (Haare, Bärte, Hauttöne, Kleidung) | vom Nutzer im Chat geschickt, Herkunft vermutlich KI-Bildgenerator | 2026-11-16 | unklar, nur interne Vorlage | vermutlich ja | **nicht im Spiel verwenden**, nur Maßstab für `tools/unreal/metahuman_looks.py` und die eigene Tracht |
| `/Game/Gamer/Characters/MetaHuman/*` (Kraeftig, Schmal, Bewohner_K/S*, Seherin; High/Low) | Spielfiguren (MetaHuman) mit Haaren/Bärten aus der MetaHuman-Grundbibliothek | MetaHuman Creator in UE 5.8 (Epic), Werte per `tools/unreal/metahuman_prepare.py`, `metahuman_looks.py` (Skripte von Claude Code); Auto-Rig und Texturen über Epics MetaHuman-Dienst | 2026-11-16 | MetaHuman-Lizenz von Epic (Nutzung in Unreal-Projekten) | Gesichter/Rig vom MetaHuman-Dienst (modellbasiert), Werte per KI-Skript | Kleidung noch keine (nur Unterwäsche des Creators) |
| `design/inbox/Blender_Kuestenasche/scripts/*.py` | Prozedurale Blender-Mesh-Generatoren (sechs Charakter-/Rüstungsassets und gemeinsame PBR-/Rig-Helfer) | OpenAI Codex, basierend auf Nutzerbriefing und Chat-Referenzen | 2026-11-18 | eigene Skripte; Referenzbilder nur interne Vorlage, Herkunft/Lizenz unklar | ja (Skripte) | Generatoren für Zweihänder, Helm, Cuirass, Schulter-/Armschutz, Tassets/Waffenrock sowie Basisfigur mit Haaren und A-Pose-Rig; noch keine exportierten Assets |
| `design/inbox/Blender_Kuestenasche/09_armor_variant_architecture.md`, `scripts/variant_common.py`, `scripts/HEAD_*.py`, `scripts/TORSO_*.py` | Konzeptnotiz und prozedurale Blender-Generatoren für sechs Kopf-/Torso-Varianten T1–T3 | OpenAI Codex, basierend auf Nutzerbriefing, Projektmaßen und Chat-Referenzen | 2026-11-18 | eigene Skripte, keine externen Meshes oder Texturen; Referenzbilder nur interne Vorlage, Herkunft/Lizenz unklar | ja | Quad-Schalen/Details, UV0, PBR-Nodes und A-Pose-Rig; Generator-Skripte noch nicht in Blender ausgeführt |
| `design/inbox/Blender_Kuestenasche/09_armor_variant_architecture.md` | Ergänzte Architektur-Spezifikation der Slots Schulter, Arme, Hände, Beine, Waffenrock und Umhang/Fell | OpenAI Codex, basierend auf Nutzerbriefing und Projekt-Rig | 2026-11-18 | keine externen Assets | ja | Slot-IDs, Formfamilien, anatomische Ausrichtung, Vertex-Group-/Collision-Verträge und Cloth-/Finger-Rig-Abhängigkeiten |
| `design/inbox/Blender_Kuestenasche/scripts/slot_variants_03_08.py`, `scripts/*_T[1-3]_*.py` | 18 prozedurale Blender-Generatoren für Schulter, Arme, Hände, Beine, Surkot und Umhang | OpenAI Codex, basierend auf Nutzerbriefing und `KA_1.75m_A_Pose_ZUp_front_-Y` | 2026-11-18 | eigene Skripte, keine externen Meshes/Texturen | ja | quadbasierte Lofts/Panels, UV0, PBR, Rig-, Joint-, Handsegment- und Cloth-Maskengruppen; noch nicht in Blender ausgeführt |
| `design/inbox/Blender_Kuestenasche/outputs/KA_Complete_Character_Armor_Set.blend` | Zusammengeführte Blender-Szene mit sechs ursprünglichen Assets, sechs Kopf-/Torso-Varianten und 18 Varianten der Slots Schulter bis Umhang; alle 30 Generatoren einzeln aufgerufen | OpenAI Codex-Generatoren, ausgeführt mit Blender 5.3.0 am 2026-11-18 | 2026-11-18 | eigene prozedurale Meshes und Shader; keine externen Meshes/Texturen | ja (prozedurale Blender-Materialknoten) | erfolgreich gespeichert; Variantenkollektionen zunächst ausgeblendet. Keine gebackenen Bild-Texturen, FBX-Exporte oder UE5-Importprüfung enthalten. |
| `design/inbox/Blender_Kuestenasche_Rebuild_2026-11-18/scripts/`, `README.md`, `outputs/SK_Character_Base.blend`, `outputs/SM_Weapon_Zweihander.blend/.fbx`, `outputs/SM_Armor_Helmet.blend/.fbx` | Blender-Generatoren, Fit-Charakter und zwei separate Mesh-Assets; Geometrie/UVs/Shader/Exports prozedural erzeugt | OpenAI Codex, Blender 5.3.0; Charakter-Basis mit MPFB 2.0.18 (lokal geladen), PBR-Bildkarten von AmbientCG | Eigene Generatoren; MPFB-Daten gemäß MakeHuman-Lizenzseite CC0 für Kerndaten, MPFB-Code GPL; AmbientCG CC0 1.0 (`https://docs.ambientcg.com/license/`); keine Referenzbilder in Modell oder Textur übernommen | indirekt (KI-generierte Skripte) | Neubau als einzelne `.blend`- und `.fbx`-Dateien; UV0 und prozedurale Roughness/Bump-Knoten; Reihenfolge und restliche Slots werden fortgesetzt |
| `design/inbox/Blender_Kuestenasche_Rebuild_2026-11-18/sources/makehuman/skins01_extracted/skins/bobby_03_young_female_hairless/Young_Female_Hairless.png` | Haut-Diffuse-Textur der Fit-Figur | MakeHuman Community `skins01` CC0 pack | CC0 1.0 laut Paket/Asset-Seite (`https://static.makehumancommunity.org/assets/assetpacks/skins01.html`) | nein (Fototextur) | nur im Charakter-Basis-Material; internes Projekt-Asset |
| `design/inbox/Blender_Kuestenasche_Rebuild_2026-11-18/sources/pbr/extracted/{Metal054B_2K-PNG,Metal008_1K-PNG,Leather007_1K-PNG,Wood014_1K-PNG,Fabric019_1K-PNG}/` | PBR-Farb-, Metall-, Rauheits-, Normal- und Höhenkarten für dunklen Stahl, Messing, Leder, Holz und Stoff | ambientCG Material packs | CC0 1.0 (`https://docs.ambientcg.com/license/`) | nein (Fototexturen) | NormalGL für Blender; NormalDX zusätzlich für Unreal; Bilder lokal referenziert und im Blend gepackt |
| `design/inbox/Blender_Kuestenasche_Rebuild_2026-11-18/sources/mpfb/package/` | Projektlokaler MPFB-Generatorcode für die 1,75-m-Fit-Figur und ihr Rig | MPFB 2.0.18, offizielles MakeHuman-Community-Repository/Extension | MPFB-Code GPL; MakeHuman-Kerndaten CC0 nach Lizenzseite (`https://static.makehumancommunity.org/about/license.html`) | nein (Software/Generator) | nur lokale Projektkopie, kein globales Add-on installiert |
| `design/inbox/teams/` | Teampläne, Übergabeablauf und Aufgabenvorlage | OpenAI Codex | 2026-11-19 | keine externen Assets | ja | interne Projektdokumentation |
| `design/inbox/teams/gemini-auswertung_2026-11-19.md`, `design/inbox/teams/aufgaben/G-001*` bis `G-006*` | Quellenbewertung und Teamaufgaben aus vier Gemini-Gesprächen | OpenAI Codex, auf Basis der vom Nutzer bereitgestellten Gemini-Texte | 2026-11-19 | keine externen Assets; Gemini-Texte nur ausgewertet, nicht vollständig übernommen | ja | widersprüchliche oder ungeprüfte Inhalte als solche gekennzeichnet |
| `Assets/Source/KA_T01_Tol_BuildHammer.blend`, `Assets/Items/KA_T01_Tol_BuildHammer.fbx`, `tools/blender/create_build_hammer.py` | Tier-1-Bauhammer, prozedurales 3D-Spielmesh mit UV0, drei PBR-Materialien und zwei UCX-Kollisionen | OpenAI Codex, live in Blender 5.3 nach `assets plan.docx` des Nutzers erzeugt | 2026-11-19 | eigenes Werk, keine Fremd-Assets oder externen Texturen | indirekt (KI-geschriebenes Blender-Skript) | 2.258 Dreiecke in der aktuell geprüften Datei; 46-cm-Schaft, Schlagfläche, gespaltene Nagelklaue, Eisenband und vier Nieten; fehlende UCX-Körper nach Gesamtprüfung ergänzt |
| `Assets/Source/KA_T01_{Tol_Sickle,Tol_Shovel,Wep_Mace,Wep_Warhammer,Wep_Spear,Wep_Shield_Round,Wep_Seax,Wep_Axe}.blend`, entsprechende `Assets/Items/*.fbx`, `tools/blender/create_remaining_plan_assets.py` | Acht einzelne Tier-1-Werkzeug-/Waffen-Assets mit UV0, je drei PBR-Materialien und individuellen UCX-Kollisionen | OpenAI Codex, mit Blender 5.3 nach `assets plan.docx` des Nutzers erzeugt | 2026-11-19 | eigenes Werk, keine Fremd-Assets oder externen Texturen | indirekt (englisch geschriebenes Blender-Skript) | Sichel 1.540, Spaten 1.840, Streitkolben 2.172, Kriegshammer 2.028, Speer 1.548, Rundschild 2.824, Seax 1.532, Axt 2.328 Dreiecke; zweisprachige Einzelanleitungen unter `design/assets/tier1/` |
| `Assets/Items/KA_T01_Tol_{Pickaxe,Saw}.fbx` | FBX-Exporte der bereits vorhandenen Spitzhacke und Säge inklusive UCX-Kollisionen | OpenAI Codex, vorhandene Blender-Dateien technisch geprüft und unverändert exportiert | 2026-11-19 | eigenes Projektmaterial, keine neuen Fremd-Assets | nein; Exportskript KI-geschrieben | Spitzhacke 2.400, Säge 2.268 Dreiecke; UV0, drei Materialien und je zwei UCX-Körper geprüft |
| `Assets/Source/KA_T01_Tol_{WoodAxe,Torch,BuildTool,Hammer,Rake,Hoe,SmithHammer,Tongs}.blend`, entsprechende `Assets/Items/*.fbx`, `tools/blender/create_missing_tier1_tools.py` | Acht weitere einzelne Tier-1-Werkzeuge mit UV0, je drei Materialien und individuellen UCX-Kollisionen | OpenAI Codex, mit Blender 5.3 nach der Werkzeugliste aus `assets plan.docx` erzeugt | 2026-11-19 | eigenes Werk, keine Fremd-Assets oder externen Texturen | indirekt (englisch geschriebenes Blender-Skript) | Detaillierte Geometrie nach Qualitätsregel beibehalten; Handbeil, Wetzstein und Meißel auf Nutzerwunsch ausdrücklich nicht erstellt; zweisprachige Einzelanleitungen unter `design/assets/tier1/` |
| `Assets/Previews/Tier1/*.png`, `tools/blender/prepare_asset_preview.py`, `tools/blender/render_asset_previews.py` | Einheitliche Qualitätsvorschauen für alle 19 Tier-1-Assets | OpenAI Codex, Blender 5.3 | 2026-11-20 | eigenes Werk, keine Fremd-Assets | indirekt (englisch geschriebene Blender-Skripte) | Je Datei nicht exportierte Kamera und Drei-Punkt-Licht in `PREVIEW_RIG`; technische UCX-Körper standardmäßig verborgen. Visuelle Prüfung fand und korrigierte getrennte Bauteile bei Bauwerkzeug, Hammer, Hacke, Spaten, Schmiedehammer, Holzfälleraxt, Standardaxt und Kriegshammer; Schildkamera auf Vorderseite korrigiert. |
| `Assets/Previews/Tier1/*.png` und `PREVIEW_RIG` in allen `Assets/Source/KA_T01_*.blend` | Korrigierte, größenabhängige Studiobeleuchtung | OpenAI Codex, Blender 5.3 | 2026-11-20 | eigenes Werk | indirekt (KI-geschriebenes Skript) | Überbelichtung beseitigt: Lichtleistung skaliert quadratisch mit der Assetgröße, AgX Medium High Contrast, Exposure -0.55, einheitlicher dunkler World-Hintergrund; Kamera/Licht bleiben vom FBX-Export ausgeschlossen. |
| `Assets/Source/KA_T01_Tol_{Hoe,Rake,Saw,Shovel,Sickle,Tongs,BuildHammer,Pickaxe,Torch,WoodAxe}.blend`, `Assets/Source/KA_T01_Wep_{Spear,Warhammer}.blend`, entsprechende FBX- und Vorschau-Dateien | Visuelle Überarbeitung von zwölf Tier-1-Assets nach Nutzerprüfung: eindeutige Silhouetten, verbundene Bauteile, offene Zange, Sägezähne, Fackelkorb mit Flamme, scharfe Speerspitze und eigenständiger Kriegshammer | OpenAI Codex, Blender 5.3 | 2026-11-20 | eigenes Werk, keine Fremd-Assets oder externen Texturen | indirekt (KI-geschriebene Blender-Skripte) | Dreiecksbudget bei sichtbarem Qualitätsgewinn bewusst überschritten; Bauhammerholz auf verwittertes Dunkelbraun geändert; Dachmoos-Parameter und gemeinsames Gebäudematerial auf kleinere, strukturierte Flecken reduziert. |
| `Assets/Source/KA_T01_Wep_BeardedAxe_Reference.blend`, `Assets/Items/KA_T01_Wep_BeardedAxe_Reference.fbx`, `Assets/Previews/Tier1/KA_T01_Wep_BeardedAxe_Reference.png` | Detaillierte Bartaxt mit gebogenem Eschenschaft, sichtbarem Keil, geschmiedetem Kopf, polierter Schneide und geometrischer Lederwicklung | OpenAI Codex, live in Blender 5.3 anhand der vier vom Nutzer bereitgestellten Referenzbilder | 2026-11-20 | eigenes prozedurales Modell; Referenzbilder nur als visuelle Vorlage, nicht als eingebettete Texturen | indirekt (KI-geschriebenes Blender-Skript und prozedurale Materialien) | 10.460 Dreiecke, vier Materialien, UV0 und zwei UCX-Kollisionen; `.blend`, `.fbx` und Kontrollrender erzeugt. |
| `Assets/Source/KA_T01_Wep_LongSword_BronzeGuard.blend`, `Assets/Items/KA_T01_Wep_LongSword_BronzeGuard.fbx`, `Assets/Previews/Tier1/KA_T01_Wep_LongSword_BronzeGuard.png` | Eigenständiges langes Fantasieschwert mit dunkler Diamantklinge, polierten Schneiden, gealtertem Bronzeparierstück, schwarzem Griff und gestuftem Knauf | OpenAI Codex, live in Blender 5.3 anhand der vier vom Nutzer bereitgestellten Schwertreferenzen | 2026-11-20 | eigenes prozedurales Modell; Referenzen nur für Proportionen und Materialrichtung, konkrete Witcher-Verzierung nicht kopiert | indirekt (KI-geschriebenes Blender-Skript und prozedurale Materialien) | 9.448 Dreiecke, vier Materialien, UV0 und zwei UCX-Kollisionen; vollständige Kameraansicht nach Kontrollrender korrigiert. |
| `Assets/Source/KA_T01_Wep_ShortSword_Normal.blend`, `Assets/Items/KA_T01_Wep_ShortSword_Normal.fbx`, `Assets/Previews/Tier1/KA_T01_Wep_ShortSword_Normal.png` | Normales einhändiges Kurzschwert mit breiter zweischneidiger Klinge, flachem Mittelbereich, gerader Stahlparierstange, Ledergriff und kompaktem Scheibenknauf | OpenAI Codex, live in Blender 5.3 | 2026-11-20 | eigenes prozedurales Modell, keine Fremd-Assets oder externen Texturen | indirekt (KI-geschriebenes Blender-Skript und prozedurale Materialien) | 7.380 Dreiecke, vier Materialien, UV0 und zwei UCX-Kollisionen; `.blend`, `.fbx` und Kontrollrender erzeugt. |
| `design/inbox/Blender_Kuestenasche_Rebuild_2026-11-18/GEZEITENHALL_ARMOR_ART_DIRECTION.md` | Aktualisierte Art-Direction und Produktionsstandard für Gezeitenhall-Rüstungen | OpenAI Codex, auf Grundlage der aktuellen Nutzervorgabe, vorheriger Küstenasche-Chats und Museums-/Epic-/Blender-Primärquellen | 2026-11-21 | eigenes Konzeptdokument; externe Seiten nur als Referenz verlinkt | nein | Explizit setzt die aktuelle Nutzeranweisung ältere Projekttexte zu „keine Plattenrüstung“ außer Kraft. |
| `design/inbox/Blender_Kuestenasche_Rebuild_2026-11-18/scripts/build_05_sk_armor_pauldrons_gezeitenhall.py`, `outputs/SK_Armor_Pauldrons_T06_Gezeitenhall.blend/.fbx`, `outputs/previews/SK_Armor_Pauldrons_T06_Gezeitenhall.png` | Eigenständiger Gezeitenhall-Schulterprototyp mit gewölbten Quad-Lames, Rig-Gewichten, Messingrand, Seeglas und Relief | OpenAI Codex, Blender 5.3, Passform am lokal eingebundenen MPFB-Fit-Körper geprüft | 2026-11-21 | prozedurale Geometrie; lokale AmbientCG-PBR-Quellen CC0 1.0; Nutzerbilder nur als Stilreferenz | indirekt (KI-geschriebenes Blender-Skript) | **WIP, nicht abgenommen:** Dreiviertelpreview ist im Projekt gespeichert; nächste Modellierpassage muss Kugelform reduzieren und die Plattenkonstruktion deutlicher machen. |
| `design/inbox/Blender_Kuestenasche_Rebuild_2026-11-18/outputs/motion/`, `outputs/SK_Armor_Pauldrons_T06_Gezeitenhall_MotionRefit.blend/.fbx`, `ARMOR_MOTION_HANDOFF.md` | Drei abspielbare Bewegungsstudien, quadratische Prüfbilder und überarbeitete Schultergeometrie | OpenAI Codex, Blender 5.3, gemeinsamer MPFB-Anprobekörper | 2026-11-21 | Eigene Keyframes und Mesh-Überarbeitung. Bestehende MPFB- und AmbientCG-Grunddaten gemäß den Einträgen oben. | indirekt (KI-geschriebene Blender-Skripte) | WIP. Kampfauslage, drei Schleichschritte mit 54 cm Weg und Bücken. Der Variantenvergleich erfasst Mesh-Kontakte in Blender. Die Aufhängung mit zwei zusätzlichen Schulterknochen ist eine Blender-Studie. |
| `design/inbox/Referenzmaterialien_2026-11-21/` | Referenzanalyse, 8 prozedurale PBR-Materialstudien mit 40 PNG-Karten (1K), Texturuebersicht und Generatoren | OpenAI Codex; NumPy | 2026-11-21 | Eigene prozedurale Strukturen; 47 Nutzerbilder nur visuell ausgewertet, keine Referenzpixel eingebettet | indirekt (KI-generierter Code/Text) | PNGs erzeugt und Dimensionen geprueft; keine neuen Gegenstandsmeshes. Blender-Start scheitert mit 0xC0000022; Blend-Generator nur syntaktisch geprueft, UE-Import nicht getestet. |
| `docs/18_kapitel_1_text.md` | Spieltext (Kapitel 1, beide Wege: Gespräche, Nebelbild, Saga-Einträge, Lied-Strophe) | Claude Code (Claude Opus 5.5) | 2026-11-21 | eigenes Werk, keine fremden Texte | ja | Entwurf; vor Übernahme ins Spiel prüfen |
| `docs/19_kapitel_2_text.md` | Spieltext (Kapitel 2, beide Wege: Gespräche, Nebelbild, Saga-Einträge, Lied-Strophen) | Claude Code (Claude Opus 5.5) | 2026-11-21 | eigenes Werk, keine fremden Texte | ja | Entwurf; vor Übernahme ins Spiel prüfen |
| `docs/20_kapitel_3_text.md` | Spieltext (Kapitel 3: Logbuch Teil 2, Thing, Haltung, Saga-Einträge) | Claude Code (Claude Opus 5.5) | 2026-11-21 | eigenes Werk, keine fremden Texte | ja | Entwurf; vor Übernahme ins Spiel prüfen |
| `tools/blender/build_stations.py`, `Assets/Buildings/Stations/SM_Station_*.fbx` (15 Modelle: Werkbank, Lagerfeuer, Strohlager, Lager, Holzfällerplatz, Steinmetzplatz, Hof, Feld-Kachel, Komposter, Regenfänger, Taverne, Herdstelle, Lehmbackofen, Braukessel, Festtafel) | Einfache Modelle für Stationen, Farm- und Tavernen-Bauteile anstelle der Platzhalter-Formen | Claude Code (Claude Opus 5.5), Blender-Skript aus Grundformen | 2026-11-21 | eigene Geometrie; Texturen: vorhandene Gebäudeflächen aus `restyle_buildings.py` (Poly Haven CC0) und neu `T_BldSoil/Sod/Ash/Water_*` aus Poly Haven `dirt_floor`, `forrest_ground_03`, `clay_plaster` (CC0, schon in `D:\gamer\cc0_quellen`) | indirekt (KI-geschriebenes Blender-Skript) | 200–600 Dreiecke je Modell, UCX-Boxen; Moos/Feuchte/Vergrauung über `M_Gamer_Building` |
| `Assets/UI/Icons/T_Icon_building_*.png` (17 neu) | Baumenü-Icons der Stationen, Farm- und Tavernen-Bauteile sowie „Baum pflanzen“ (Setzling) | Claude Code, `render_icons.py` (gleiche Kamera/Licht wie die übrigen Icons) | 2026-11-21 | aus den Modellen oben gerendert | indirekt | keine KI-Bilder |
| `/Game/Gamer/Characters/Clothing/*` (SKM_Cloth_<Kraeftig/Schmal>_<Tunika/Kleid>, M_Cloth, MI_Cloth_*) | Kleidung der MetaHuman-Figuren | eigene Umsetzung: per Skript aus der MetaHuman-Körperoberfläche erzeugt (`tools/blender/build_clothing.py`, `tools/unreal/clothing_import.py`, Skripte von Claude Code) | 2026-11-21 | eigenes Werk; Grundform aus dem MetaHuman-Körper (MetaHuman-Lizenz von Epic) | Geometrie per KI-geschriebenem Skript | keine fremden Assets; Referenz `design/referenz/figuren` nur als Vorlage |
| `design/inbox/Testkopf_2026-11-21/` (`build_testkopf.py`, `shape_keys_testkopf.py`, `export_testkopf.py`, `SK_Testkopf.fbx`, `Testkopf.blend`) | Test-Kopfmodell mit 5 ARKit-Shape-Keys, Skript-generiert | Claude Code (Skripte), Blender 5.3 | 2026-11-21 | keine externen Assets | ja | nur Pipeline-/Leitfaden-Test, **nicht im Spiel verwenden**; `design/leitfaden_blender_unreal_figuren.md` |
| `design/leitfaden_blender_unreal_figuren.md` | Leitfaden (Text) | Claude Code, Web-Recherche | 2026-11-21 | Quellen im Dokument verlinkt | ja | Auswertung `D:\gamer\referensbilder - Modell` |
| `design/referenz/gemini_kuestenasche_gesamtdokumentation.md` | Konzepttext (Kreaturen, Werte, Beute, Rezepte, C++-Beispiele) | Google Gemini, vom Nutzer erarbeitet; Original `D:\gamer\heruntergeladenes\k_stenasche_gesamtdokumentation.md` | 2026-11-22 | keine externen Assets erkennbar | ja | Namen, Beschreibungen und Werte der 48 Kreaturen stammen von Gemini; Grundlage für `docs/21_kreaturen_level_kampf.md` und `docs/22_kreaturen_daten.md` |
| `docs/21_kreaturen_level_kampf.md` | Konzepttext (bereinigte Fassung) | Claude Code auf Basis des Gemini-Dokuments | 2026-11-22 | – | ja | Entscheidungen vom 2026-11-22 eingearbeitet; Zahlen = Startwerte |
| `docs/22_kreaturen_daten.md`, `tools/data/creatures_from_gemini.py` | Datentabellen (erzeugt) und Erzeuger-Skript | Claude Code; Werte, Namen und Beute-Chancen aus dem Gemini-Dokument | 2026-11-22 | – | ja | Einordnung, Spawn und Beute-Zuordnung = Vorschlag von Claude |


[↑ zum Inhaltsverzeichnis](#inhaltsverzeichnis)


---

<a id="teil-8"></a>

# Teil VIII – Anhang: Tier-1-Asset-Karten

<a id="d43"></a>

## 43. Tier-1 Asset-Status / Tier-1 Asset Status

> Quelle: `design/assets/tier1/ASSET_STATUS.md`

<a id="d43-1"></a>

### 43.1 Deutsch

Erstellt und als einzelne `.blend`- und `.fbx`-Dateien vorhanden: Bauhammer, Spitzhacke, Säge, Spaten, Sichel, Holzfälleraxt, Fackel, Bauwerkzeug, Hammer, Rechen, Hacke, Schmiedehammer, Schmiedezange, Standardaxt, Streitkolben, Kriegshammer, Speer, Rundschild und Seax.

Nicht benötigt und bewusst nicht erstellt: Handbeil, Wetzstein und Meißel.

Qualitätsregel: 1.500–3.000 Dreiecke bleiben der Richtwert. Eine Überschreitung ist zulässig, wenn sie die sichtbare Silhouette, Rundungen, Funktionsdetails oder handwerkliche Qualität verbessert.

<a id="d43-2"></a>

### 43.2 English

Created and available as individual `.blend` and `.fbx` files: Build Hammer, Pickaxe, Saw, Shovel, Sickle, Wood Axe, Torch, Build Tool, Hammer, Rake, Hoe, Smithing Hammer, Tongs, Standard Axe, Mace, Warhammer, Spear, Round Shield, and Seax.

Not required and intentionally excluded: Hatchet, Whetstone, and Chisel.

Quality rule: 1,500–3,000 triangles remains the guideline. Exceeding it is allowed when it improves the visible silhouette, curvature, functional details, or crafted quality.


[↑ zum Inhaltsverzeichnis](#inhaltsverzeichnis)

<a id="d44"></a>

## 44. KA_T01_Tol_BuildHammer — Bauhammer / Build Hammer

> Quelle: `design/assets/tier1/KA_T01_Tol_BuildHammer.md`

<a id="d44-1"></a>

### 44.1 Deutsch

Baue einen 46-cm-Holzschaft mit Griffknauf, einen Eisenkopf mit Schlagfläche und gespaltener Nagelklaue sowie ein Eisenband mit vier Nieten. Ursprung: Griffmitte. Erzeuge UV0, drei Material-Slots und zwei UCX-Körper. Ziel: 1.500–3.000 Dreiecke.

<a id="d44-2"></a>

### 44.2 English (script source)

Build a 46 cm wooden shaft with a grip pommel, an iron head with a striking face and split nail claw, plus an iron collar with four rivets. Origin: grip center. Generate UV0, three material slots, and two UCX hulls. Target: 1,500–3,000 triangles.


[↑ zum Inhaltsverzeichnis](#inhaltsverzeichnis)

<a id="d45"></a>

## 45. KA_T01_Tol_BuildTool — Bauwerkzeug / Build Tool

> Quelle: `design/assets/tier1/KA_T01_Tol_BuildTool.md`

<a id="d45-1"></a>

### 45.1 Deutsch

Baue einen 48-cm-Holzgriff, einen rechtwinkligen Eisenkopf, einen Ausrichtungs-/Hebehaken und ein vernietetes Messband. Ursprung: Griffmitte. Erzeuge UV0, drei Material-Slots und zwei UCX-Körper. Ziel: 1.500–3.000 Dreiecke.

<a id="d45-2"></a>

### 45.2 English (script source)

Build a 48 cm wooden handle, a square iron head, an alignment/lifting hook, and a riveted measuring collar. Origin: grip center. Generate UV0, three material slots, and two UCX hulls. Target: 1,500–3,000 triangles.


[↑ zum Inhaltsverzeichnis](#inhaltsverzeichnis)

<a id="d46"></a>

## 46. KA_T01_Tol_Hammer — Hammer / Hammer

> Quelle: `design/assets/tier1/KA_T01_Tol_Hammer.md`

<a id="d46-1"></a>

### 46.1 Deutsch

Baue einen 46-cm-Holzgriff, einen Eisenkopf mit flacher Schlagbahn und Querfinne sowie ein Eisenband mit vier Nieten. Ursprung: Griffmitte. Erzeuge UV0, drei Material-Slots und zwei UCX-Körper. Ziel: 1.500–3.000 Dreiecke.

<a id="d46-2"></a>

### 46.2 English (script source)

Build a 46 cm wooden handle, an iron head with a flat striking face and cross peen, plus an iron collar with four rivets. Origin: grip center. Generate UV0, three material slots, and two UCX hulls. Target: 1,500–3,000 triangles.


[↑ zum Inhaltsverzeichnis](#inhaltsverzeichnis)

<a id="d47"></a>

## 47. KA_T01_Tol_Hoe — Hacke / Hoe

> Quelle: `design/assets/tier1/KA_T01_Tol_Hoe.md`

<a id="d47-1"></a>

### 47.1 Deutsch

Baue einen 1,34-m-Holzschaft, eine stabile Eisenfassung, ein breites Bodenblatt und ein Eisenband mit vier Nieten. Ursprung: Griffmitte. Erzeuge UV0, drei Material-Slots und zwei UCX-Körper. Ziel: 1.500–3.000 Dreiecke.

<a id="d47-2"></a>

### 47.2 English (script source)

Build a 1.34 m wooden shaft, a strong iron socket, a broad soil blade, and an iron collar with four rivets. Origin: grip center. Generate UV0, three material slots, and two UCX hulls. Target: 1,500–3,000 triangles.


[↑ zum Inhaltsverzeichnis](#inhaltsverzeichnis)

<a id="d48"></a>

## 48. KA_T01_Tol_Pickaxe — Spitzhacke / Pickaxe

> Quelle: `design/assets/tier1/KA_T01_Tol_Pickaxe.md`

<a id="d48-1"></a>

### 48.1 Deutsch

Verwende einen langen Holzschaft, einen symmetrischen Eisenkopf mit Spitze und Meißelende sowie ein vernietetes Eisenband. Ursprung: Griffmitte. Prüfe UV0, drei Material-Slots, zwei UCX-Körper und den Bereich von 1.500–3.000 Dreiecken. Exportiere `.blend` und `.fbx` separat.

<a id="d48-2"></a>

### 48.2 English (script source)

Use a long wooden shaft, a symmetrical iron head with a point and chisel end, and a riveted iron collar. Origin: grip center. Verify UV0, three material slots, two UCX hulls, and the 1,500–3,000 triangle range. Export separate `.blend` and `.fbx` files.


[↑ zum Inhaltsverzeichnis](#inhaltsverzeichnis)

<a id="d49"></a>

## 49. KA_T01_Tol_Rake — Rechen / Rake

> Quelle: `design/assets/tier1/KA_T01_Tol_Rake.md`

<a id="d49-1"></a>

### 49.1 Deutsch

Baue einen 1,48-m-Holzschaft, eine 46-cm-Querleiste, neun Eisenzinken und eine vernietete Verstärkungsplatte. Ursprung: Griffmitte. Erzeuge UV0, drei Material-Slots und zwei UCX-Körper. Ziel: 1.500–3.000 Dreiecke.

<a id="d49-2"></a>

### 49.2 English (script source)

Build a 1.48 m wooden shaft, a 46 cm crossbar, nine iron tines, and a riveted reinforcement plate. Origin: grip center. Generate UV0, three material slots, and two UCX hulls. Target: 1,500–3,000 triangles.


[↑ zum Inhaltsverzeichnis](#inhaltsverzeichnis)

<a id="d50"></a>

## 50. KA_T01_Tol_Saw — Säge / Saw

> Quelle: `design/assets/tier1/KA_T01_Tol_Saw.md`

<a id="d50-1"></a>

### 50.1 Deutsch

Baue einen ergonomischen Holzgriff, ein vernietetes Eisenblatt und eine deutlich gezahnte Schneidkante. Ursprung: Griffmitte. Erzeuge UV0, drei Material-Slots und zwei UCX-Körper. Ziel: 1.500–3.000 Dreiecke; separat als `.blend` und `.fbx` speichern.

<a id="d50-2"></a>

### 50.2 English (script source)

Build an ergonomic wooden handle, a riveted iron blade, and a clearly toothed cutting edge. Origin: handle center. Generate UV0, three material slots, and two UCX hulls. Target: 1,500–3,000 triangles; save separate `.blend` and `.fbx` files.


[↑ zum Inhaltsverzeichnis](#inhaltsverzeichnis)

<a id="d51"></a>

## 51. KA_T01_Tol_Shovel — Spaten / Shovel

> Quelle: `design/assets/tier1/KA_T01_Tol_Shovel.md`

<a id="d51-1"></a>

### 51.1 Deutsch

Baue ein 26-cm-Eisenblatt, einen 88-cm-Holzstiel, einen 13-cm-T-Griff und ein Eisenband mit vier Nieten. Setze den Ursprung bei der Griffmitte des Plans, erzeuge UV0, drei Material-Slots und zwei UCX-Körper. Ziel: 1.500–3.000 Dreiecke. Speichere und exportiere das Asset separat.

<a id="d51-2"></a>

### 51.2 English (script source)

Build a 26 cm iron blade, an 88 cm wooden shaft, a 13 cm T-handle, and an iron collar with four rivets. Place the origin at the specified grip center, generate UV0, three material slots, and two UCX hulls. Target 1,500–3,000 triangles. Save and export the asset separately.


[↑ zum Inhaltsverzeichnis](#inhaltsverzeichnis)

<a id="d52"></a>

## 52. KA_T01_Tol_Sickle — Sichel / Sickle

> Quelle: `design/assets/tier1/KA_T01_Tol_Sickle.md`

<a id="d52-1"></a>

### 52.1 Deutsch

Baue einen 15-cm-Holzgriff entlang Y, eine 16-mm-Eisenzwinge mit vier Nieten und eine nach +Y/+Z gebogene, spitz zulaufende Klinge. Setze den Ursprung in die Griffmitte, erzeuge UV0 mit 0,02 Inselabstand, nutze die drei vorgeschriebenen Material-Slots und zwei einfache UCX-Körper. Ziel: 1.500–3.000 Dreiecke. Exportiere `.blend` und `.fbx` einzeln.

<a id="d52-2"></a>

### 52.2 English (script source)

Build a 15 cm wooden handle along Y, a 16 mm iron ferrule with four rivets, and a pointed blade curving toward +Y/+Z. Place the origin at the handle center, generate UV0 with a 0.02 island margin, use the three required material slots, and add two simple UCX hulls. Target 1,500–3,000 triangles. Export separate `.blend` and `.fbx` files.


[↑ zum Inhaltsverzeichnis](#inhaltsverzeichnis)

<a id="d53"></a>

## 53. KA_T01_Tol_SmithHammer — Schmiedehammer / Smithing Hammer

> Quelle: `design/assets/tier1/KA_T01_Tol_SmithHammer.md`

<a id="d53-1"></a>

### 53.1 Deutsch

Baue einen 50-cm-Holzgriff, einen schweren Eisenkopf mit großer Schlagbahn und Schmiedefinne sowie ein vernietetes Eisenband. Ursprung: Griffmitte. Erzeuge UV0, drei Material-Slots und zwei UCX-Körper. Ziel: 1.500–3.000 Dreiecke.

<a id="d53-2"></a>

### 53.2 English (script source)

Build a 50 cm wooden handle, a heavy iron head with a large striking face and forging peen, plus a riveted iron collar. Origin: grip center. Generate UV0, three material slots, and two UCX hulls. Target: 1,500–3,000 triangles.


[↑ zum Inhaltsverzeichnis](#inhaltsverzeichnis)

<a id="d54"></a>

## 54. KA_T01_Tol_Tongs — Schmiedezange / Smithing Tongs

> Quelle: `design/assets/tier1/KA_T01_Tol_Tongs.md`

<a id="d54-1"></a>

### 54.1 Deutsch

Baue zwei lange Eisenarme, gegenüberliegende Greifbacken, lederumwickelte Griffe und einen stabilen Drehpunkt mit Nieten. Ursprung: Griff-/Drehpunktbereich. Erzeuge UV0, drei Material-Slots und zwei UCX-Körper. Ziel: 1.500–3.000 Dreiecke.

<a id="d54-2"></a>

### 54.2 English (script source)

Build two long iron arms, opposing gripping jaws, leather-wrapped handles, and a strong riveted pivot. Origin: grip/pivot area. Generate UV0, three material slots, and two UCX hulls. Target: 1,500–3,000 triangles.


[↑ zum Inhaltsverzeichnis](#inhaltsverzeichnis)

<a id="d55"></a>

## 55. KA_T01_Tol_Torch — Fackel / Torch

> Quelle: `design/assets/tier1/KA_T01_Tol_Torch.md`

<a id="d55-1"></a>

### 55.1 Deutsch

Baue einen 64-cm-Holzschaft, mehrere überlappende Pechstoff-Wicklungen, eine Glutkappe und einen Eisenhaltering. Ursprung: Griffmitte. Erzeuge UV0, drei Material-Slots und zwei UCX-Körper. Ziel: 1.500–3.000 Dreiecke.

<a id="d55-2"></a>

### 55.2 English (script source)

Build a 64 cm wooden shaft, several overlapping pitch-cloth wraps, an ember cap, and an iron retaining ring. Origin: grip center. Generate UV0, three material slots, and two UCX hulls. Target: 1,500–3,000 triangles.


[↑ zum Inhaltsverzeichnis](#inhaltsverzeichnis)

<a id="d56"></a>

## 56. KA_T01_Tol_WoodAxe — Holzfälleraxt / Wood Axe

> Quelle: `design/assets/tier1/KA_T01_Tol_WoodAxe.md`

<a id="d56-1"></a>

### 56.1 Deutsch

Baue einen 78-cm-Eschenstiel, einen breiten keilförmigen Fällkopf, ein sichtbares Axtauge und ein Eisenband mit vier Nieten. Ursprung: Griffmitte. Erzeuge UV0, drei Material-Slots und zwei UCX-Körper. Ziel: 1.500–3.000 Dreiecke.

<a id="d56-2"></a>

### 56.2 English (script source)

Build a 78 cm ash shaft, a broad wedge-shaped felling head, a visible axe eye, and an iron collar with four rivets. Origin: grip center. Generate UV0, three material slots, and two UCX hulls. Target: 1,500–3,000 triangles.


[↑ zum Inhaltsverzeichnis](#inhaltsverzeichnis)

<a id="d57"></a>

## 57. KA_T01_Wep_Axe — Standardaxt / Standard Axe

> Quelle: `design/assets/tier1/KA_T01_Wep_Axe.md`

<a id="d57-1"></a>

### 57.1 Deutsch

Baue einen 68-cm-Holzstiel, einen keilförmigen Eisenkopf mit sichtbar offenem Axtauge und ein ovales Eisenband mit vier Nieten. Setze den Ursprung in die Griffmitte, erzeuge UV0, drei Material-Slots und zwei UCX-Körper. Ziel: 1.500–3.000 Dreiecke. Speichere `.blend` und exportiere `.fbx` separat.

<a id="d57-2"></a>

### 57.2 English (script source)

Build a 68 cm wooden shaft, a wedge-shaped iron head with a visibly open axe eye, and an oval iron collar with four rivets. Place the origin at the grip center, generate UV0, three material slots, and two UCX hulls. Target 1,500–3,000 triangles. Save `.blend` and export `.fbx` separately.


[↑ zum Inhaltsverzeichnis](#inhaltsverzeichnis)

<a id="d58"></a>

## 58. KA_T01_Wep_Mace — Streitkolben / Mace

> Quelle: `design/assets/tier1/KA_T01_Wep_Mace.md`

<a id="d58-1"></a>

### 58.1 Deutsch

Baue einen 66-cm-Holzschaft, einen tonnenförmigen Eisenkopf mit sechs Schlagrippen sowie ein Eisenband mit vier Nieten. Ursprung: Griffmitte. Erzeuge UV0, drei Material-Slots und zwei UCX-Körper. Ziel: 1.500–3.000 Dreiecke; separater `.blend`- und `.fbx`-Export.

<a id="d58-2"></a>

### 58.2 English (script source)

Build a 66 cm wooden shaft, a barrel-shaped iron head with six striking ribs, and an iron collar with four rivets. Origin: grip center. Generate UV0, three material slots, and two UCX hulls. Target 1,500–3,000 triangles; export separate `.blend` and `.fbx` files.


[↑ zum Inhaltsverzeichnis](#inhaltsverzeichnis)

<a id="d59"></a>

## 59. KA_T01_Wep_Seax — Seax

> Quelle: `design/assets/tier1/KA_T01_Wep_Seax.md`

<a id="d59-1"></a>

### 59.1 Deutsch

Baue einen 14-cm-Holzgriff, eine 14-mm-Eisenzwinge mit vier Nieten und eine 26,6-cm-keilförmige Seax-Klinge mit markanter Spitze. Ursprung: Griffmitte. Erzeuge UV0, drei Material-Slots und zwei UCX-Körper. Ziel: 1.500–3.000 Dreiecke; separat speichern und exportieren.

<a id="d59-2"></a>

### 59.2 English (script source)

Build a 14 cm wooden handle, a 14 mm iron ferrule with four rivets, and a 26.6 cm wedge-shaped Seax blade with a distinct point. Origin: handle center. Generate UV0, three material slots, and two UCX hulls. Target 1,500–3,000 triangles; save and export separately.


[↑ zum Inhaltsverzeichnis](#inhaltsverzeichnis)

<a id="d60"></a>

## 60. KA_T01_Wep_Shield_Round — Rundschild / Round Shield

> Quelle: `design/assets/tier1/KA_T01_Wep_Shield_Round.md`

<a id="d60-1"></a>

### 60.1 Deutsch

Baue einen Rundschild mit 78 cm Durchmesser, Holzfläche, Eisenrand, mittigem Schildbuckel, Rückgriff und vier Nieten. Die Vorderseite zeigt nach +Y; der Ursprung liegt in der rückseitigen Griffmitte. Erzeuge UV0, drei Material-Slots und einen UCX-Körper. Ziel: 1.500–3.000 Dreiecke; separat exportieren.

<a id="d60-2"></a>

### 60.2 English (script source)

Build a 78 cm round shield with a wooden face, iron rim, central boss, rear grip, and four rivets. The front faces +Y; place the origin at the rear grip center. Generate UV0, three material slots, and one UCX hull. Target 1,500–3,000 triangles; export separately.


[↑ zum Inhaltsverzeichnis](#inhaltsverzeichnis)

<a id="d61"></a>

## 61. KA_T01_Wep_Spear — Speer / Spear

> Quelle: `design/assets/tier1/KA_T01_Wep_Spear.md`

<a id="d61-1"></a>

### 61.1 Deutsch

Baue einen 1,68-m-Holzschaft, eine 7-cm-Eisentülle mit vier Nieten und eine 23-cm-blattförmige Speerspitze mit Mittelgrat. Ursprung: vorgeschriebene Griffmitte. Erzeuge UV0, drei Material-Slots und zwei UCX-Körper. Ziel: 1.500–3.000 Dreiecke; separater Export.

<a id="d61-2"></a>

### 61.2 English (script source)

Build a 1.68 m wooden shaft, a 7 cm iron socket with four rivets, and a 23 cm leaf-shaped spearhead with a central ridge. Origin: specified grip center. Generate UV0, three material slots, and two UCX hulls. Target 1,500–3,000 triangles; export separately.


[↑ zum Inhaltsverzeichnis](#inhaltsverzeichnis)

<a id="d62"></a>

## 62. KA_T01_Wep_Warhammer — Kriegshammer / Warhammer

> Quelle: `design/assets/tier1/KA_T01_Wep_Warhammer.md`

<a id="d62-1"></a>

### 62.1 Deutsch

Baue einen 62-cm-Holzschaft, einen Eisenkopf mit Schlagfläche in +Y und Keildorn in -Y sowie ein Eisenband mit vier Nieten. Ursprung: Griffmitte. Erzeuge UV0, drei Material-Slots und zwei UCX-Körper. Ziel: 1.500–3.000 Dreiecke; einzeln speichern und exportieren.

<a id="d62-2"></a>

### 62.2 English (script source)

Build a 62 cm wooden shaft, an iron head with a +Y striking face and a -Y wedge spike, plus an iron collar with four rivets. Origin: grip center. Generate UV0, three material slots, and two UCX hulls. Target 1,500–3,000 triangles; save and export separately.


[↑ zum Inhaltsverzeichnis](#inhaltsverzeichnis)
