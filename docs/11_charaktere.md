# Charaktererstellung, Charakterauswahl, Charaktereditor

Stand 2026-11-14. Wunsch des Nutzers: Charaktererstellung bzw. -auswahl und ein Charaktereditor. Bei der Auswahl geht es vor allem um **freischaltbare Fähigkeiten, Werte und Anfangswaffen**; das Aussehen ist nur ein Vorschlag, den der Spieler im Editor ändern kann.

Alles hier ist ein **Vorschlag von Claude**, nicht bestätigt.

## Ablauf (Vorschlag)

1. **Auswahl:** fünf Herkünfte (Klassen) zur Wahl, jede mit Vorschlags-Aussehen, Startwerten, Startausrüstung und einer Startfähigkeit.
2. **Editor:** Aussehen frei ändern (Körper, Gesicht, Haare, Bart, Farben, Tätowierungen/Runenbemalung, Kleidung aus der Startausrüstung); die Werte der Herkunft bleiben.
3. **Name**, dann ins Spiel. Der Charakter gehört zum Spieler, nicht zur Welt (in Koop-Welten mitnehmbar) – offene Frage.

## Werte (fünf, keine Ausdauer-Sperre)

| Wert | Wirkt auf |
|---|---|
| Stärke | Nahkampfschaden, Traglast, Abbau-Tempo |
| Geschick | Fernkampf, Ausweichen, Sammel- und Ernte-Tempo |
| Zähigkeit | Gesundheit, Regeneration, Kälte- und Hungerresistenz |
| Handwerk | Herstell- und Bautempo, Haltbarkeit hergestellter Dinge, Reparatur |
| Wille | Magie (Runen, Seidr), Tränke, Wirkung auf Bewohner (Stimmung) |

Startwerte je Herkunft: Summe 25, jeder Wert 3–8. Später steigen Werte durch Tun und Fähigkeitspunkte.

## Fünf Herkünfte

| | Jägerin/Jäger | Krieger/in | Schmied/in | Bäuerin/Bauer | Seherin/Seher (Völva) |
|---|---|---|---|---|---|
| **Aussehen (Vorschlag)** | Leder, Fellumhang, Kapuze, schlank | Kettenhemd-Andeutung, Rundschild auf dem Rücken, Narben | kräftig, Lederschürze, Rußspuren, Zöpfe | Leinenkittel, Strohhut oder Kopftuch, Gürteltasche | dunkler Umhang, Stab mit Runen, Perlen, Bemalung im Gesicht |
| **Werte** S/G/Z/H/W | 4/8/5/4/4 | 8/5/7/3/2 | 7/3/6/7/2 | 4/5/6/6/4 | 2/5/4/4/8 |
| **Anfangswaffe** | Kurzbogen + 20 Pfeile, Dolch | Bartaxt + Rundschild | Schmiedehammer (Waffe und Werkzeug) | Sichel, Hackmesser | Zauberstab (Runenstab), Dolch |
| **Startfähigkeit** | Spuren lesen (Tiere und Vorkommen in der Nähe markiert) | Schildstoß (bricht die Haltung des Gegners) | Feldreparatur (Werkzeuge unterwegs reparieren) | Grüner Daumen (+1 Ertrag bei Feld und Wildpflanzen) | Rune des Schutzes (kleiner Schild auf sich oder Mitspieler) |
| **Freischaltbar (Beispiele)** | Leiser Gang, Fallen, Zielschuss, Häuten (mehr Fell/Fleisch) | Kampfschrei (Gruppe stärker), Haltungsbruch, Berserkerwut (Abklingzeit, keine Ausdauer) | bessere Waffen und Werkzeuge, Erz verhütten, Rüstung schmieden, Baumeister-Bonus | Saatgut sparen, längere Haltbarkeit, Tiere halten, Kompost-Meister | Seidr-Vision (Wetter/Jahreszeit vorhersehen), Heiltränke, Runen auf Waffen, Segen auf Felder |
| **Stärke im Koop** | Erkunden, Jagd | Schutz, Front | Ausrüstung für alle | Nahrung für Siedlung | Unterstützung, Heilung |

## Entscheidungen (2026-11-14)

- Fähigkeiten: jeder kann alles lernen, die Herkunft gibt Vorsprung und einen günstigeren eigenen Zweig.
- Charakter pro Spieler (in jede Welt mitnehmbar).
- Figurenmodell: MetaHuman (Nachteile siehe `docs/01_entscheidungen.md`).
- Umsetzung und Stand: `docs/02_phasenplan.md`, Abschnitt „Charakterauswahl …“. Werte-Korrektur (Seherin 3/5/5/4/8, Grenzen 2–8) ist ein Vorschlag von Claude.

## Offene Fragen (Stand vor der Entscheidung)

- Sind Fähigkeiten an die Herkunft gebunden oder kann später jeder alles lernen (Herkunft = Vorsprung)? Vorschlag: jeder kann alles lernen, die Herkunft gibt Startwerte, Startfähigkeit und einen günstigeren Zweig.
- Geschlecht/Körperform: frei im Editor, unabhängig von der Herkunft (Vorschlag).
- Charakter pro Spieler oder pro Welt?
- Technik: Figurenmodell und Gesichts-/Haar-Varianten fehlen noch (Platzhalter-Zylinder). MetaHuman ist kostenlos in Unreal, aber groß und nicht im Wikinger-Stil vorgefertigt; Alternative eigenes modulares Modell. Muss entschieden werden, bevor der Editor gebaut wird.
