# Stilidee „Gelebter Norden“ (Vorschlag)

> **Abgelöst (2026-11-15):** Geltende Stilvorgabe ist `design/stil_norden_gezeitenhall.md` (Mischung mit „Küstenasche“). Dieses Dokument bleibt als Grundlage zum Nachlesen.

Stand 2026-11-14. Vorschlag von Claude auf Wunsch des Nutzers: eigene Stilidee, angelehnt an **Aska**, **Medieval Dynasty** und **Kingdom Come: Deliverance II**. Nicht bestätigt. Es wird nichts aus diesen Spielen übernommen (keine Assets, Namen, Motive); sie dienen nur als Richtung.

## Was wir von wem lernen

| Vorbild | Was dort überzeugt | Was wir übernehmen | Was wir anders machen |
|---|---|---|---|
| Aska | Nordische Siedlung, klare Formen, starke Lichtstimmung (warmes Feuer gegen kalte Nacht), Siedlung von oben gut lesbar | lesbare Silhouetten, Stimmungslicht, Vogelperspektive als Teil des Spiels | realistischere Materialien und Gesichter |
| Medieval Dynasty | Alltag, Jahreszeiten, Feldbau, Holzbau, das Dorf wächst sichtbar | Spuren des Lebens im Dorf, Jahreszeiten als Bildwechsel, Handwerk sichtbar | Skandinavien statt Mitteleuropa; dichtere Atmosphäre |
| Kingdom Come II | historische Glaubwürdigkeit, Materialien mit Schmutz und Abnutzung, natürliches Licht, kaum aufdringliche Anzeige | Abnutzung, Schlamm, glaubwürdige Kleidung und Werkzeuge, zurückhaltende Anzeige | kein Simulations-Zwang; dazu eine leise mythische Ebene (nordische Magie) |

## Kernidee in einem Satz

**Ein glaubwürdiger, gelebter Norden, in dem man jede Jahreszeit spürt und in dem die Mythen leise durchscheinen: realistisch im Material, klar in der Form, warm im Feuer, kalt im Wind.**

## Sechs Stilregeln

1. **Glaubwürdig, nicht museal.** Häuser, Werkzeuge und Kleidung könnten so im 9.–11. Jh. im Norden existiert haben, sind aber für das Spiel vereinfacht (klare Bauteile, gut greifbare Werkzeuge). Keine Hörnerhelme, keine Plattenrüstung.
2. **Klare Silhouetten, auch von oben.** Jedes Spielelement ist an seiner Form erkennbar: Bäume nach Art, erntbare Pflanzen an Früchten (Früchte dürfen leicht übertrieben groß und farbig sein), Gebäude an der Dachform. Aus der Vogelperspektive muss man das Dorf „lesen“ können.
3. **Spuren des Lebens.** Wege treten sich dort aus, wo Bewohner und Spieler oft laufen; vor Werkstätten liegen Späne, am Lagerfeuer Asche, an der Schmiede Ruß. Neue Bauten sind hell und sauber, alte vergraut, bemoost, ausgebessert.
4. **Die Jahreszeit bestimmt die Farbe.** Frühling frisch und hell, Sommer satt und golden, Herbst rostrot und neblig, Winter blau-weiß und leise. Die Farbsättigung bleibt insgesamt gedämpft; kräftige Farben gibt es nur an Feuer, Früchten, Färbestoffen und Magie.
5. **Warm gegen kalt.** Licht erzählt Sicherheit: Feuer, Herd und Fackeln warm (orange, ca. 2000–2700 K), Nacht, Nebel und Wildnis kalt (blau-grün). Bodennebel in Senken und über Wasser (siehe Methoden-Doku).
6. **Mythos leise.** Magie (nordisch, Runen, Seidr) ist selten und gedämpft: Runen glimmen schwach, Seidr zeigt sich als Nebel, Schatten, Visionen. Keine bunten Feuerbälle. Farben der Magie: kaltes Türkis und Glut-Orange.

## Material- und Farbpalette

- **Baustoffe:** Kiefern-/Fichtenholz (frisch honigfarben → vergraut silbergrau), Torf-/Grassodendach und Reet, Feldstein, Lehmflechtwerk, Birkenrinde.
- **Kleidung:** Wolle und Leinen in Naturtönen (Grau, Braun, Ungefärbt), Leder, Fell; Farbe als Wohlstand: Krapprot, Färberwaid-Blau, Ocker, selten Grün.
- **Metall:** dunkles, geschmiedetes Eisen mit Rost, wenig Bronze und Silber (Schmuck).
- **Regeln für Texturen:** Foto-/Scan-Materialien (CC0 oder eigene Fotos), einheitliche Texeldichte ca. 512 px pro Meter in der Nähe, Rauheit eher hoch (wenig Glanz), keine reinen Farben.

## Menschen

- Realistische Proportionen und Gesichter (MetaHuman laut Entscheidung), wettergegerbte Haut, Zöpfe, Bärte, Tätowierungen/Bemalung als Wahl im Editor.
- Kleidung zeigt Tätigkeit und Zustand: Ruß beim Schmied, Erde beim Bauern, Nässe nach Regen, Schnee auf den Schultern im Winter.

## Anzeige (UI)

- So wenig wie nötig, gut lesbar (Entscheidung zur Lesbarkeit gilt). Rahmen und Symbole wie geschnitztes Holz, gestanzte Lederstücke, Runenkanten, aber schlicht.
- Möglichst im Bild statt auf dem Bild: z. B. Fäulnis sieht man am Apfel selbst, Hunger an der Haltung der Figur; die Leiste bleibt trotzdem für Klarheit.

## Klang (passend zur vorhandenen Musik)

- Natur und Handwerk im Vordergrund (Wind, Holzhacken, Schmiede, Tiere), Musik selten und leise: Viking-Folk am Feuer, Ambient in der Wildnis.

## Technik-Leitplanken

- 8-GB-Grafikkarte als Ziel: Nanite für Felsen und Gebäude, Pflanzen als Karten-Modelle mit LOD, Licht dynamisch (Lumen auf Hoch, einfachere Lösung auf Niedrig).
- Gesamtgröße ≤ 20 GB: lieber wenige, gut variierte Materialien als viele Einzeltexturen.

## Was das für die laufenden Arbeiten heißt

- Stil-Probe (Gebäude aus Highlands und Medieval Village, umtexturiert): prüft Regel 1, 3 und die Materialpalette.
- Grassodendach als typisch nordisches Dach ergänzen (Material fehlt noch).
- Früchte und erntbare Pflanzen leicht übertreiben (Regel 2) – passt zum offenen Punkt „Haselnüsse nicht erkennbar“.
- Abnutzung und Moos über ein gemeinsames Material statt pro Modell.
- Nächster Schritt Design: Moodboards je Jahreszeit und für Dorf/Wildnis/Nacht (ChatGPT, siehe Rollen in `CLAUDE.md`).
