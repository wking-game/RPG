# Review 01 – Art Direction, Holzbaukasten, Arbeitsstätten (2026-11-12)

*Von Claude Code. Geprüft: `design/inbox/01_art_direction.md`, `03_assets_baukasten.md`, `04_assets_arbeitsstaetten.md` gegen `design/ASSET_PLAN.md`.*
*Für ChatGPT: Bitte Punkte 1–5 in einer überarbeiteten Fassung von 03 und 04 berücksichtigen. Entscheidungen des Nutzers stehen am Ende.*

## Gesamturteil
Gute Grundlage. Format, IDs, Budgets, Zustände, MVP/Ausbau und KI-Nachweis entsprechen dem Asset-Plan; alle Priorität-A-Assets aus 5.3 und 5.4 sind abgedeckt. Die Art Direction ist als Entscheidung eingetragen (`docs/01_entscheidungen.md`).
Es gibt **einen Rechenfehler beim Lager** und **drei technische Lücken**, die vor dem Modellieren geklärt sein müssen.

## 1. Lager passt nicht zum Dach-Baukasten (muss geklärt werden)
Die Dachmodule decken bei 45° eine **horizontale Spannweite von 150 cm je Seite** ab (Dachfläche 212 cm, First 150 cm über der Wand). Damit lassen sich nur Gebäude mit **300 cm Breite quer zum First** decken.
Das Lager ist mit **600 × 600 cm** und **ca. 450 cm bis zum First** angegeben. Bei 600 cm Breite und 45° läge der First aber **300 cm** über der Wand (gesamt 600 cm), und dafür fehlen Module (doppelte Dachflächen, größerer Giebel). 450 cm würden eine flachere Neigung (ca. 27°) und einen eigenen Dachsatz verlangen.

Möglichkeiten:
- **A (Empfehlung): Lager 300 × 600 cm**, First in Längsrichtung. Passt ohne neue Module zum Baukasten; die 450 cm bis zum First stimmen dann. Die „zweigeteilte“ Idee bleibt: 300 × 300 geschlossener Raum + 300 × 300 offene Regalbucht.
- **B: 600 × 600 cm beibehalten** und den Baukasten erweitern: zwei Dachflächen je Seite plus ein halber Giebel (rechtwinkliges Dreieck 300 × 300 cm). First dann bei 600 cm. Mehr Module, höheres Gebäude.
- **C: eigener, flacherer Dachsatz** für große Gebäude. Am teuersten.

## 2. Dreiecksbudgets: Ziel- statt Höchstwerte für Module
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

## 3. Fundament auf unebenem Gelände
20 cm Höhe reichen nur auf flachem Boden. Auf Hängen schwebt das Fundament oder versinkt.
Bitte das Fundament mit einem **Sockel nach unten** beschreiben: 20 cm sichtbar über der Bauebene, **mindestens 100 cm** darunter (geschlossene Seiten, gleiche Trim-Sheet-Fläche). Später optional ein Stelzen-Modul für steile Hänge.

## 4. Drehpunkte (Pivots) und Ecken
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

## 5. Turf-Material
Terrain-Materialien (Landscape-Schichten) lassen sich nicht direkt auf Meshes legen. Die Turf-Kante bekommt eine **eigene Materialinstanz, die die Gras-Texturen wiederverwendet**. Das ist in Ordnung, kostet aber einen zweiten Material-Slot pro Dachmodul (ein zusätzlicher Draw-Call je Modul). Bei vielen Dächern ist das spürbar, deshalb die Empfehlung unten.

## Kleinigkeiten (optional)
- Ansichten: Der Asset-Plan wünscht bei Gebäuden Vorder- und Seitenansicht. Für die Übergabe an einen 3D-Artist hilft je Modul eine einfache Skizze (vorne/seitlich/oben). KI-Bilder bitte mit Nachweis.
- Türwand: Öffnung 130 × 230 cm und Türblatt 126 × 226 cm passen. Die Tür muss nach innen aufgehen können, ohne einen 120 cm breiten Durchgang zu versperren.

## Nachprüfung der überarbeiteten Fassung (2026-11-12)
Überarbeitete `03_assets_baukasten.md` und `04_assets_arbeitsstaetten.md` geprüft und in `design/inbox/` übernommen.

| Punkt | Ergebnis |
|---|---|
| 1 Lager/Dach | erledigt: 300 × 600 cm, Wand 300, First 450, nutzt das 45°-Dachmodul |
| 2 Budgets | erledigt: Zielwerte wie vorgeschlagen, 2.000 als harte Obergrenze; Lager als eigene optimierte Hülle (Ziel 8.000) |
| 3 Fundament | erledigt: 20 cm sichtbar, mind. 100 cm Sockel; Stelzenmodul als späterer Ausbau |
| 4 Pivots/Ecken | erledigt: Tabelle übernommen, Eckpfosten-Regel eingearbeitet |
| 5 Turf | erledigt: eigene geteilte Materialinstanz, Draw-Call-Kosten als Messpunkt notiert |

**Offen, klein:** Das Lager hat seinen Ursprung „auf Geländehöhe“, beschreibt aber keinen Sockel nach unten wie das Fundament. Auf Hängen würde es schweben. Beim Modellieren gleiche Sockelregel anwenden (mind. 100 cm unter Gelände). Kein neuer Durchlauf nötig.

## Entscheidungen des Nutzers
**Am 2026-11-12 entschieden: alle drei Empfehlungen übernommen** (Holzdach mit Turf-Kante, Lager 300 × 600 cm, Lagerfeuer). Eingetragen in `docs/01_entscheidungen.md`.
Für ChatGPT: Bitte 03 und 04 entsprechend überarbeiten (Punkte 1–5 oben, Lager auf 300 × 600 cm umplanen).

| # | Frage | Empfehlung von Claude (übernommen) |
|---|---|---|
| 1 | **Dachdeckung:** Holz mit Turf-Kante oder ganzes Turf-Dach? | **Holz mit schmaler Turf-Kante.** Ein ganzes Turf-Dach verschmilzt in der Vogelperspektive mit der Graslandschaft, die Gebäude wären schlechter zu erkennen. Wer sparen will: im MVP nur Holz, Turf-Kante später. |
| 2 | **Lagergröße** | **300 × 600 cm** (Variante A in Punkt 1), passt ohne neue Dachmodule. |
| 3 | **Kochstelle:** offenes Lagerfeuer mit Holzgalgen und Kessel? | **Ja.** Günstig (2.000 Dreiecke), gut lesbar, passt zum Slice. |
