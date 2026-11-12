# 03 – Assets: Holzbaukasten (Phase 1, überarbeitet)

**Stil:** Klarform-Saga im mittelalterlich-nordischen Wikinger-Kontext.  
**Überarbeitet nach:** Review 01 vom 2026-11-12.  
**Umfang:** Abschnitt 5.3, ausschließlich Priorität A. Wandfenster, Treppe, Zaun und Tor bleiben außerhalb dieses MVP-Auftrags.

## Gemeinsame Vorgaben

- Raster: 300 cm. Wand, Boden und Fundament belegen jeweils 300 × 300 cm.
- Figurenmaßstab: erwachsene Figur ca. 180 cm. Türöffnung mindestens 120 × 220 cm; die gewählte Öffnung ist breiter.
- Holzbauteile verwenden `trim_wood`. Farbvarianten und Abnutzung kommen über Materialinstanzen, nicht über eigene Texturen je Mesh.
- Dreiecksangaben sind **Zielwerte**. Für jedes einzelne Rastermodul gilt eine harte Obergrenze von 2.000 Dreiecken.
- Wandmodule sind 20 cm stark und stehen mittig auf der Rasterlinie. An Außenecken deckt `building_pillar_wood` die Wandenden ab.
- Die Materialoberflächen und Silhouetten bleiben aus der Vogelperspektive gut unterscheidbar.

### Farb- und Materialregeln

| Material-/Farbrolle | Farbwert | Einsatz |
|---|---:|---|
| Helles, frisch bearbeitetes Holz | `#8E6B46` | Brettflächen und Schnittkanten |
| Gealtertes Holz | `#5F4833` | Schatten, Reparaturen und ältere Bauteile |
| Leinen und helle Fasern | `#D8C9A8` | sparsame Seile oder Abdeckungen |
| Fjordgrün | `#3E6655` | sehr sparsame Akzente |
| Glut | `#D78342` | Feuer- und Interaktionsakzent |

`trim_wood` ist eine gemeinsame Trim-Sheet-Familie mit Base Color, Normal und ORM. Die Holzbauteile erhalten keine individuellen Texturen. Für die schmale Turf-Kante am Dach wird eine eigene Materialinstanz aus wiederverwendeten Grastexturen angelegt; Landscape-Materialien werden nicht direkt auf die Dachmeshes gelegt.

## Bauteilregister – Priorität A

| ID | Unreal-Name | Maße / Raster | Dreiecks-Ziel | Harte Obergrenze |
|---|---|---|---:|---:|
| `building_foundation_wood` | `SM_Building_Foundation_Wood` | 300 × 300 cm; Sockel 20 cm über bis mind. 100 cm unter Gelände | 500 | 2.000 |
| `building_floor_wood` | `SM_Building_Floor_Wood` | 300 × 300 cm; ca. 15 cm stark | 500 | 2.000 |
| `building_wall_wood` | `SM_Building_Wall_Wood` | 300 × 300 × 20 cm | 800 | 2.000 |
| `building_wall_door_wood` | `SM_Building_Wall_Door_Wood` | 300 × 300 × 20 cm; Öffnung 130 × 230 cm | 800 | 2.000 |
| `building_door_wood` | `SM_Building_Door_Wood` | Türblatt 126 × 226 × 6 cm | 400 | 2.000 |
| `building_pillar_wood` | `SM_Building_Pillar_Wood` | 25 × 25 × 300 cm | 200 | 2.000 |
| `building_beam_wood` | `SM_Building_Beam_Wood` | 300 cm lang; ca. 20 × 25 cm Querschnitt | 200 | 2.000 |
| `building_roof_wood` | `SM_Building_Roof_Wood` | 300 cm Firstlänge × ca. 212 cm Dachfläche; 45° Neigung | 600 | 2.000 |
| `building_roof_ridge_wood` | `SM_Building_Roof_Ridge_Wood` | 300 cm Länge | 200 | 2.000 |
| `building_roof_gable_wood` | `SM_Building_Roof_Gable_Wood` | 300 cm breit × 150 cm hoch | 400 | 2.000 |
| `trim_wood` | `T_Trim_Wood_BC`, `T_Trim_Wood_N`, `T_Trim_Wood_ORM` | je 2048 × 2048 px | entfällt | entfällt |

## Rastermodule

### `building_foundation_wood` – Holzfundament

- **Zweck:** Eine klare, wiederholbare Basis je 300-cm-Baufeld, auch auf leicht unebenem Gelände.
- **Konzept:** Quadratischer Holzrahmen mit wenigen kräftigen Querträgern. Ein geschlossener Sockelbereich reicht 20 cm sichtbar über den Gelände-/Baubezug und mindestens 100 cm darunter. Die Seitenflächen sind geschlossen und verwenden dieselbe Holz-Trim-Sheet-Fläche; dadurch bleiben sie bei schrägem Gelände geschlossen, ohne eine eigene Textur zu benötigen.
- **Zustände:** Neu/verwittert über Materialinstanz. Im MVP keine Bruchgeometrie.
- **MVP / Ausbau:** Ein Fundamentmodul für ebenes und leicht geneigtes Gelände / später optional `building_foundation_stilt_wood` als separates Stelzenmodul für steile Hänge.
- **Budget-Check:** Ziel 500, harte Obergrenze 2.000 Dreiecke; Holz-Trim-Sheet.
- **Ursprung/Pivot:** Mitte der Oberseite. Die Oberseite liegt 20 cm über dem Gelände-Bezug; der Sockel reicht mindestens 100 cm darunter.

### `building_floor_wood` – Boden

- **Zweck:** Begehbare 300-cm-Baufläche.
- **Konzept:** Breite Dielen mit eindeutiger Laufrichtung und wenigen sichtbaren Querträgern. Kleine Spalten geben Struktur, bleiben aber geschlossen genug für eine durchgehende Spielfläche.
- **Zustände:** Neu/verwittert über Materialinstanz.
- **MVP / Ausbau:** Ein Bodenmodul / später einzelne Reparaturdielen, falls Bauzustände gebraucht werden.
- **Budget-Check:** Ziel 500, harte Obergrenze 2.000 Dreiecke; Holz-Trim-Sheet.
- **Ursprung/Pivot:** Mitte der Oberseite.

### `building_wall_wood` – geschlossene Wand

- **Zweck:** Grundmodul für Außen- und Innenwände.
- **Konzept:** 20 cm starke Wand mit breiten vertikalen Brettern, oberem und unterem Querbalken sowie einem diagonalen Verstrebungsbalken. Die Randpfosten liegen höchstens 12,5 cm vom jeweiligen Wandende nach innen; im Eck werden diese Enden von einem separaten 25 × 25 cm Eckpfosten verdeckt. Wandstärke und Stöße bleiben dadurch aus beiden Blickrichtungen sauber.
- **Zustände:** Neu/verwittert über Materialinstanz.
- **MVP / Ausbau:** Ein beidseitig verwendbares Wandmodul / später Reparaturvarianten.
- **Budget-Check:** Ziel 800, harte Obergrenze 2.000 Dreiecke; Holz-Trim-Sheet.
- **Ursprung/Pivot:** Untere Mitte auf der Rasterlinie.

### `building_wall_door_wood` – Türwand

- **Zweck:** Türdurchgang, der auch mit getragenen Gegenständen und mehreren Koop-Spielern benutzbar bleibt.
- **Konzept:** Aufbau wie `building_wall_wood`, mit mittiger Öffnung 130 × 230 cm. Ein breiter Sturz und verstärkte Pfosten rahmen die Öffnung; die Schwelle ist in der Vogelperspektive durch einen dunkleren Holzton sichtbar. Die separate Tür schwenkt nach innen und liegt geöffnet möglichst flach an der Innenwand, sodass der freie Durchgang 120 cm nicht unterschreitet.
- **Zustände:** Wand neu/verwittert; Türblatt separat offen/geschlossen.
- **MVP / Ausbau:** Eine mittige Öffnung / später alternative Öffnungsposition nur bei nachgewiesenem Bedarf.
- **Budget-Check:** Ziel 800, harte Obergrenze 2.000 Dreiecke; Holz-Trim-Sheet.
- **Ursprung/Pivot:** Untere Mitte auf der Rasterlinie.

### `building_door_wood` – Türblatt

- **Zweck:** Schließt die Türöffnung und bleibt im offenen Zustand sichtbar, ohne den Durchgang zu verengen.
- **Konzept:** Türblatt aus breiten Brettern, zwei Querstreben und einer diagonalen Verstärkung. Stilisierte Holzscharniere sind auf der Bandseite gut lesbar; kleine Metallteile werden nur als dunkle Akzente angedeutet.
- **Maße:** 126 × 226 × 6 cm für die Öffnung 130 × 230 cm.
- **Zustände:** Offen/geschlossen durch Drehung an der Scharnierkante; neu/verwittert über Materialinstanz.
- **MVP / Ausbau:** Ein Türblatt / später verstärkte oder reparierte Materialvarianten.
- **Budget-Check:** Ziel 400, harte Obergrenze 2.000 Dreiecke; Holz-Trim-Sheet.
- **Ursprung/Pivot:** Untere Scharnierkante.

### `building_pillar_wood` und `building_beam_wood` – Stützen und Balken

| ID | Konzept und Maße | Dreiecks-Ziel / Obergrenze |
|---|---|---:|
| `building_pillar_wood` | Vierkantstütze 25 × 25 × 300 cm, leicht gebrochene Kanten und einfache Anschlusskerbe. An Außenecken deckt sie die Enden zweier 20-cm-Wände ab. | 200 / 2.000 |
| `building_beam_wood` | Balken 300 cm lang, ca. 20 × 25 cm Querschnitt. Für längere Strecken werden Rastermodule wiederholt; die Stoßstelle ist als einfacher Überblattungsstoß lesbar. | 200 / 2.000 |

- **Material/Zustände:** Holz-Trim-Sheet; neu/verwittert über Materialinstanz.
- **MVP / Ausbau:** Je ein wiederverwendbares Modul / spätere Längen- oder Schadensvarianten nur bei Bedarf.
- **Ursprung/Pivot:** Stütze unten mittig; Balken in der Mitte seiner Längsachse.

### `building_roof_wood`, `building_roof_ridge_wood` und `building_roof_gable_wood` – Dachgruppe

- **Zweck:** Deckt ein Gebäude mit 300 cm Breite und 45° geneigtem Satteldach.
- **Dachgeometrie:** Pro Dachseite 150 cm horizontale Ausladung und 150 cm Höhenanstieg. Das ergibt rund 212 cm geneigte Dachfläche. Das Firstniveau liegt 150 cm über der 300-cm-Wandoberkante.
- **Dachfläche:** 300 cm entlang des Firsts × ca. 212 cm auf der Schräge. Breite Sparren sind von unten sichtbar; außen liegt eine klare, großflächige Holzdeckung.
- **Turf-Entscheidung:** Für das MVP ist eine schmale Turf-Kante am Holz-Dach gewählt. Sie nutzt eine eigene, gemeinsame Materialinstanz, die Grastexturen wiederverwendet; kein Landscape-Material direkt am Mesh. Diese Instanz belegt einen zweiten Material-Slot und kann daher einen zusätzlichen Draw-Call je Dachmodul verursachen. Die Turf-Fläche bleibt bewusst schmal und die Instanz wird über alle Module geteilt. Falls die Laufzeitmessung das Budget gefährdet, kann die Kante aus dem MVP entfallen.
- **First:** `building_roof_ridge_wood` ist ein 300-cm-Abschnitt. Ein 600-cm-First nutzt zwei Abschnitte.
- **Giebel:** `building_roof_gable_wood` ist ein Dreieck 300 cm breit × 150 cm hoch mit kräftigem Randrahmen und wenigen horizontalen Brettern.
- **Zustände:** Neu/verwittert über Holzmaterialinstanz; Turf bleibt ein gemeinsamer Materialakzent.
- **MVP / Ausbau:** Ein Dachmodul pro 300 cm Firstlänge plus First und Giebel / später beschädigte Dachdeckung oder optionale schneefreie/verschneite Varianten.
- **Budget-Check:** Ziele: Dachfläche 600, First 200, Giebel 400; jedes Modul harte Obergrenze 2.000 Dreiecke. Holz-Trim-Sheet plus ein geteilter Turf-Material-Slot.
- **Ursprung/Pivot:** Dachfläche an der unteren Traufkante mittig; First mittig auf der Längsachse; Giebel unten mittig auf der Rasterlinie.

## `trim_wood` – gemeinsame Holz-Trim-Sheet-Familie

- **Zweck:** Einheitliche Holzoberflächen für alle Bauteile der Holzstufe.
- **Konzept:** Eine 2048²-Trim-Sheet-Familie mit wenigen großen Feldern für Brettfläche, Balken, Schnittkante und sparsame Kerb-/Abnutzungsspuren. Base Color, Normal und ORM haben dieselbe Auflösung. Keine 4K-Texturen und keine Einzeltexturen für Bauteile.
- **Zustände:** `MI_Trim_Wood_New` und `MI_Trim_Wood_Weathered`.
- **MVP / Ausbau:** Eine gemeinsame Familie mit zwei Materialinstanzen / später zusätzliche Farbinstanzen ohne neue Auflösung.
- **Budget-Check:** 2048² je Map; keine Dreiecke.

## Pivot- und Anschlussübersicht

| Modul | Ursprung/Pivot |
|---|---|
| Fundament, Boden | Mitte der Oberseite |
| Wand, Türwand, Giebel | unten, mittig, auf der Rasterlinie |
| Pfosten | unten, mittig |
| Balken, First | Mitte der Längsachse |
| Dachfläche | Traufkante, unten, mittig |
| Türblatt | Scharnierkante, unten |
| Wandecken | Wand 20 cm stark und mittig auf der Rasterlinie; Eckpfosten 25 × 25 cm deckt die Enden ab; Randpfosten in der Wand höchstens 12,5 cm vom Wandende eingerückt |

## Entscheidungen, die der Nutzer treffen muss

Die in Review 01 bestätigten Richtungsentscheidungen sind eingearbeitet: Holzdeckung mit schmaler Turf-Kante. Die Dachkante bleibt im MVP bewusst klein; ihre Performance-Auswirkung ist als zu messender Punkt dokumentiert.

## KI-Inhaltsnachweis

| Datei | Werkzeug | Datum | Lizenz/Quelle |
|---|---|---|---|
| `03_assets_baukasten.md` | OpenAI Codex | 2026-11-12 | KI-generierter Konzepttext auf Basis des Nutzer-Briefings und des eingereichten Reviews; keine externen Assets verwendet |
