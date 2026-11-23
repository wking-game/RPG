# Gemini-Bestandsaufnahme · 2026-11-19

## Zweck und Bewertungsregel

Vier bereitgestellte Gemini-Gespräche wurden mit den aktuellen Projektregeln, Entscheidungen und vorhandenen Entwürfen abgeglichen. Maßgeblich sind bestätigte Einträge in [01_entscheidungen.md](../../../docs/01_entscheidungen.md), danach die Projektregeln und der aktuelle Phasenplan. Gemini-Inhalte sind Ideenquellen. Sie ändern keine Entscheidung von selbst.

Die umfangreichen Rohgespräche werden zur Token-Ersparnis nicht ins Projekt kopiert. Aufgaben laden nur diese Auswertung und die jeweils verlinkten Projektabschnitte. Anhang-ID, lokaler Pfad, Zeilenbereich und Hash-Präfix machen die Quelle bei einer späteren Detailprüfung auffindbar.

| Quelle | Inhalt | Kurzurteil |
|---|---|---|
| S1 · Anhang `9298ce0d…` · SHA-256 `A5915099…` | Outfits, Figuren, Blender, Waffen und Großkataloge | Stil und Modularität brauchbar; Mengen, Slots und technische Versprechen widersprüchlich |
| S2 · Anhang `d94b6b8e…` · SHA-256 `0CDC0B01…` | Gesamtvision, Quests, Seed-Welt, Ausrüstung | Grundidee teils aktuell; frühere Solo-, Ausdauer- und Umfangsannahmen überholt |
| S3 · Anhang `c8d0a05f…` · SHA-256 `65B08043…` | Regie-Regelwerk für In-Engine-Momente | Sensorische Regie brauchbar; Kontrollentzug, Ego-Perspektive und feste Masseneffekte ungeeignet |
| S4 · Anhang `5da6d97d…` · SHA-256 `18BAF15E…` | Prolog, Kapitelideen, Vertonung und Regie | Prolog und Hauptpfade großteils bereits in aktuellen Story-Dokumenten enthalten |

### Lokale Rohquellen

- S1: `C:/Users/Emanu/.codex/attachments/9298ce0d-620f-4b37-be64-c446cb4f6039/Eingefügter Text.txt`
- S2: `C:/Users/Emanu/.codex/attachments/d94b6b8e-b700-4fa6-bef6-f7c0c1cb6a5d/Eingefügter Text.txt`
- S3: `C:/Users/Emanu/.codex/attachments/c8d0a05f-6bcb-4098-8f30-33d8fc0931dc/Eingefügter Text.txt`
- S4: `C:/Users/Emanu/.codex/attachments/5da6d97d-0192-4324-9ab1-d57193911d6a/Eingefügter Text.txt`

## Übernommen oder bereits bestätigt

Diese Punkte standen bereits in den Projektunterlagen. Die Gemini-Texte bestätigen oder erweitern dieselbe Richtung, sind aber nicht die maßgebliche Quelle.

- **Stil:** gelebter, verwitterter Norden mit zurückhaltendem Gezeitenhall, Seeglas und dem offenen Ring mit drei Kerben. Bindend bleibt die [Stilvorgabe](../../stil_norden_gezeitenhall.md).
- **Storyform:** offene Erkundung, sichtbare Spuren, kurze vertonte Reaktionen, Saga-Buch als Gedächtnis und Entscheidungen mit Folgen. Das steckt bereits im [Prolog](../../../docs/15_probe_spur_der_ersten.md) und [Kapitelplan](../../../docs/17_hauptstory_kapitel.md).
- **Progression:** sechs Materialstufen, modulare Waffenbestandteile und wiederverwendete Materialien sind bereits in der [Waffenliste](../../ASSET_LIST_WAFFEN.md) enthalten.
- **Vertonung:** Deutsch und Englisch, Sprecher-IDs, Untertitel, freie Namen nur im Text und MetaHuman-Gesichtsbewegung aus fertigem Audio sind im [Entscheidungslog](../../../docs/01_entscheidungen.md) bestätigt.
- **Open-World-Ende:** Nach dem Finale bleibt die Welt spielbar. Bosse, Nebenstränge und Siedlungsausbau laufen laut [Kapitelplan](../../../docs/17_hauptstory_kapitel.md) weiter.

## Angepasst und als Vorschlag eingeplant

Diese Punkte sind aus Gemini abgeleitete Vorschläge. Sie werden erst nach der in der Aufgabe genannten Freigabe zu Projektentscheidungen.

| Idee | Anpassung für Gamer | Aufgabe |
|---|---|---|
| Mindestens 20 Wikinger-Trachten | Als 20 kuratierte Looks aus wenigen geprüften MetaHuman-kompatiblen Modulen, Farben und Materialien planen. Herkunft, Lizenz, Speicher- und Leistungsbudget je Quelle belegen. | [G-002](aufgaben/G-002-modulare-tracht.md) |
| Seed-basierte Welt mit Questorten | Deterministische Teil-Seeds und stabile POI-IDs spezifizieren. Weltzustand, Queststand, Speichern/Laden und Server-Autorität gehören in denselben Vertrag. | [G-003](aufgaben/G-003-seed-welt-poi.md) |
| In-Engine-Regie ohne Filmsequenzen | Audio fokussieren, Welt sichtbar verändern und kurze Zustandsvarianten nutzen. Kamera und Eingaben bleiben beim Spieler. Intensität und Bewegungseffekte brauchen zugängliche Einstellungen. | [G-004](aufgaben/G-004-in-engine-regie.md) |
| Zustandsabhängige Stimmen | Nur wichtige Zeilen erhalten Varianten für Kälte, Erschöpfung oder Gezeitenhall. Erst Text und Ereignis-ID festlegen, dann Lizenz, Kosten, Qualität und Speicherbedarf prüfen. | [G-006](aufgaben/G-006-voice-sfx-pipeline.md) |
| Weitere Questideen | Nur Motive übernehmen, die neue Orte, Entscheidungen oder Folgen liefern. Doppelte Kapitel und generische Fantasy-Aufträge entfallen. | [G-005](aufgaben/G-005-storyideen.md) |

## Später prüfen

- **Zusätzliche Nebenquest „Das Schweigen unter dem Moos“ und Motive aus den 30 Questlines:** erst nach Freigabe des Hauptkapitelplans und nur als kurze Pitches. [G-005](aufgaben/G-005-storyideen.md)
- **ElevenLabs oder ein anderes Erzeugungswerkzeug für Stimmen, Musik und SFX:** aktuelle Rechte, Kosten, Qualität und Reproduzierbarkeit anhand weniger Zeilen prüfen. [G-006](aufgaben/G-006-voice-sfx-pipeline.md)
- **Weitere Waffenarten, Schwarzmarkt und zusätzliche Aufwertungsstufen:** als Ideenbank behalten, bis Kampfsystem und bestätigter Phase-3-Umfang stabil sind.

## Veraltet oder ungeeignet

- **Separater Singleplayer-first-Codepfad:** Aktuell gilt Solo als Koop mit einer Person und derselben serverautoritativen Logik.
- **Aktionen durch leere Ausdauer sperren:** Ausdauer darf Leistung senken. Laufen, Ducken und Waffen bleiben benutzbar.
- **Regelmäßige Pflicht-Raids:** Angriffe bleiben eine kombinierbare Weltoption.
- **MPFB, Mixamo oder Manny als neues Mastermodell:** MetaHuman ist bestätigt. Andere Körper dienen höchstens als interne Passform- oder Testhilfe.
- **Gemini-Slotlisten mit acht oder elf Rüstungszonen:** Sie widersprechen dem bestätigten Slotstand und den bereits erzeugten Blender-Slots. Das muss vor weiterer Integration geklärt werden. [G-001](aufgaben/G-001-ausruestungs-slotvertrag.md)
- **120, 171 oder 636 Einzelassets als Produktionsziel:** Diese Zahlen stammen aus wechselnden Gesprächsständen, missachten den Vertical Slice und widersprechen der modularen Budgetstrategie.
- **Alle Waffen bis Stufe 10, konkrete Schadenswerte und Dropchancen:** Nicht bestätigt. Aktuell gelten sechs Materialstufen und die bestätigten Boss-Wiederholungswerte.
- **Erzwungene Kamera, verlangsamte Bewegung, gesperrtes Inventar oder Ego-Perspektive:** Die Hauptkamera ist Third-Person. Storyregie darf die Spielkontrolle nicht ungefragt übernehmen.
- **Dutzende gleichzeitig gespawnte Raben, globales Wetterumschalten und vollständige Tonstille als Standard:** Zu teuer, spielerisch riskant und im Koop schwer eindeutig. Als skalierbare Einzelmomente neu spezifizieren.
- **Bernsteinfarbenes „Gezeitenlicht“:** Gezeitenlicht ist `#A9E9DC`. Herdfeuer ist warm.
- **Ungeprüfte Gratis-, Lizenz- oder Qualitätsversprechen sowie rohe Gemini-Skripte:** Vor Verwendung sind Quelle, Lizenz, Version, Import, Rig, Maße, Gewichte, LODs und Laufzeitkosten nachzuweisen.
- **„Küstenasche“ als feststehender Spieltitel:** Im Projekt ist der Arbeitstitel weiterhin offen.

## Herkunftsmatrix

| Thema | Gemini-Fundstelle | Projektstatus |
|---|---|---|
| Mindestens 20 Outfits und modulare Teile | S1, Zeilen 3–51 | neuer, angepasster Vorschlag G-002 |
| Stil und Materialsprache | S1, ab Zeile 1542 | bereits durch die aktuelle Stilvorgabe geregelt |
| Seed-Welt und POI-Zustände | S2, ab Zeile 509 | neuer Vorschlag G-003; Weltgenerierung noch offen |
| 30 Questlines | S2, ab Zeile 188 | durch „Die Spur der Ersten“ ersetzt; Motive später prüfen |
| In-Engine-Regie | S3, Zeilen 1–67; S4, ab Zeile 792 | angepasst als G-004 |
| Prolog und zwei Storypfade | S4, Zeilen 1–97 und ab Zeile 98 | bereits in Prolog und Kapitelplan verarbeitet |
| Zustandsstimmen und ElevenLabs | S4, ab Zeile 258 und 1162 | Grundsatz Vertonung bestätigt; Werkzeug und Varianten offen, G-006 |
| 8/11 widersprüchliche Slotzonen | S2, Zeilen 1092 und 1451; S1, Zeile 3433 | nicht übernommen; Entscheidung G-001 |
| Großkataloge mit 120/171/636 Assets | S1, Zeilen 1957, 2479 und 2769; S2, Zeilen 1132–1138 | verworfen als Produktionsziel |

## Einbindung der Teams

| Team | Nutzung aus den Gesprächen | Übergabe |
|---|---|---|
| PROD | Konflikte, Phase, Umfang und Freigaben verwalten | bereitet G-001 für Klio vor und priorisiert G-002 bis G-006 |
| ART | Tracht-Looks, Materialsprache und lesbare Varianten | Konzept und IDs an TA/UX |
| TA | MetaHuman-Passform, Materialinstanzen, Body-Masking, Rig/LOD/Import | geprüfte Assetverträge an ENG/ANIM/QA |
| ANIM | Kontaktpunkte, Kleidung in Bewegung und Gesichtsanimation aus Audio | Clip- und Gesichtsbelege an ENG/QA |
| ENG | Freigegebene Datenverträge, Weltzustände und Ereignisse integrieren | Build und Logs an QA |
| SYS | Seed/POI/Persistenz, Eventzustände und Gameplay-Grenzen spezifizieren | prüfbare Regeln an ENG/UX/QA |
| NAR | vorhandene Story schützen, neue Motive kuratieren, Zeilen-IDs liefern | Questvertrag an SYS, Texte an UX/SFX |
| SFX | Stimmen, Natur, Fokus-Audio und Zustandsvarianten planen | Dateien, IDs, Zeitmarken und Lizenznachweis |
| UX | Saga-Buch, Untertitel, Intensitätsoptionen und Steuerungshinweise | Zustände und Referenzansichten an ENG/QA |
| QA | Koop, Speichern, Wiederholung, Performance, Lesbarkeit und Audio prüfen | Befunde an zuständiges Team und PROD |
