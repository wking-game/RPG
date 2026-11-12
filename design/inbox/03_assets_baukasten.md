# 03 – Assets: Holzbaukasten (Phase 1)

**Stil:** Klarform-Saga im mittelalterlich-nordischen Wikinger-Kontext; klare, kräftige Formen mit zurückhaltender natürlicher Abnutzung.  
**Umfang:** Abschnitt 5.3, nur Priorität A. Fensterwand, Treppe, Zaun und Tor bleiben außerhalb dieses MVP-Auftrags.

## Gemeinsame Vorgaben

- Raster: 300 cm; Wand- und Bodenmodule belegen jeweils 300 × 300 cm.
- Erwachsene Figur: ungefähr 180 cm. Alle Türöffnungen sind mindestens 120 × 220 cm frei.
- Jedes Bauteil verwendet dieselbe Holz-Trim-Sheet-Familie und Materialinstanzen. Farbvarianten und Abnutzung ersetzen individuelle Texturen.
- Formen sind stilisiert und aus der Vogelperspektive unterscheidbar. Balken, Dachkante und einfache geometrische Kerben ergeben eine eigene Siedlungsidentität; Runen werden nicht flächig dekorativ eingesetzt.
- Keine 4K-Texturen. Alle Modulmeshes zielen auf höchstens 2.000 Dreiecke pro Bauteil.

### Material- und Farbregeln

| Material-/Farbrolle | Farbwert | Einsatz |
|---|---:|---|
| Helles, frisch bearbeitetes Holz | `#8E6B46` | Brettflächen und Schnittkanten |
| Gealtertes Holz | `#5F4833` | Schatten, Reparaturen und ältere Bauteile |
| Leinen und helle Fasern | `#D8C9A8` | kleine Seile oder Abdeckungen, wenn vorhanden |
| Fjordgrün | `#3E6655` | sehr sparsame Erkennungsakzente, kein großflächiger Holzfarbton |
| Glut | `#D78342` | Licht- und Interaktionsakzent, nicht als Bauteilfarbe |

`trim_wood` liefert die Holzoberflächen. Stein, Eisen und Feuer werden nur dort ergänzt, wo sie für das Bauteil gebraucht werden; die Holzbauteile selbst erhalten keine eigenen Materialsets.

## Bauteilregister – Priorität A

| ID | Unreal-Name | Maße / Raster | Texturregel | Dreiecksbudget |
|---|---|---|---|---:|
| `building_foundation_wood` | `SM_Building_Foundation_Wood` | 300 × 300 cm; ca. 20 cm hoch | `trim_wood` | max. 2.000 |
| `building_floor_wood` | `SM_Building_Floor_Wood` | 300 × 300 cm; ca. 15 cm stark | `trim_wood` | max. 2.000 |
| `building_wall_wood` | `SM_Building_Wall_Wood` | 300 × 300 cm; ca. 20 cm stark | `trim_wood` | max. 2.000 |
| `building_wall_door_wood` | `SM_Building_Wall_Door_Wood` | 300 × 300 cm; Öffnung 130 × 230 cm | `trim_wood` | max. 2.000 |
| `building_door_wood` | `SM_Building_Door_Wood` | Türblatt ca. 126 × 226 cm; für obige Öffnung | `trim_wood` | max. 2.000 |
| `building_pillar_wood` | `SM_Building_Pillar_Wood` | 25 × 25 × 300 cm | `trim_wood` | max. 2.000 |
| `building_beam_wood` | `SM_Building_Beam_Wood` | 300 cm Länge; ca. 20 × 25 cm Querschnitt | `trim_wood` | max. 2.000 |
| `building_roof_wood` | `SM_Building_Roof_Wood` | 300 cm entlang First × ca. 212 cm Dachfläche; 45° Neigung | `trim_wood`; optional vorhandene Grasschicht für Turf-Akzent | max. 2.000 |
| `building_roof_ridge_wood` | `SM_Building_Roof_Ridge_Wood` | 300 cm Firstlänge; ca. 30 cm breit/hoch | `trim_wood` | max. 2.000 |
| `building_roof_gable_wood` | `SM_Building_Roof_Gable_Wood` | 300 cm breit × 150 cm Giebelhöhe | `trim_wood` | max. 2.000 |
| `trim_wood` | `T_Trim_Wood_BC`, `T_Trim_Wood_N`, `T_Trim_Wood_ORM` | je 2048 × 2048 px | eine gemeinsame Trim-Sheet-Familie für die Holzstufe | entfällt |

### `building_foundation_wood` – Holzfundament

- **Zweck:** Gibt jedem 300-cm-Baufeld eine klare, leicht erhöhte Basis.
- **Konzept:** Vier kräftige Randhölzer und wenige Querträger. Die Balkenenden sind einfach eingekerbt; keine kleinteiligen Nägel oder filigranen Verzierungen. Das Fundament bleibt visuell offen genug, damit Boden und Anschluss zum Gelände verständlich bleiben.
- **Zustände:** Neu und verwittert über Materialinstanz; keine separate Schadensgeometrie im MVP.
- **MVP / Ausbau:** Ein neutrales Modul für alle Holzgebäude / später Reparatur- und beschädigte Varianten.
- **Budget-Check:** max. 2.000 Dreiecke; ausschließlich Holz-Trim-Sheet.

### `building_floor_wood` – Boden

- **Zweck:** Begehbare 300-cm-Baufläche für Haus- und Arbeitsstättenmodule.
- **Konzept:** Breite, gut lesbare Dielen mit klarer Laufrichtung. Zwei bis drei breitere Träger zeigen die Unterseite an den Rändern. Kleine Spalten geben Struktur, lassen aber keine sichtbaren Löcher entstehen.
- **Zustände:** Neu und verwittert über Materialinstanz.
- **MVP / Ausbau:** Ein Bodenmodul / später einzelne Reparaturdielen oder dekorative Randstücke.
- **Budget-Check:** max. 2.000 Dreiecke; Holz-Trim-Sheet, keine eigene Textur.

### `building_wall_wood` – geschlossene Wand

- **Zweck:** Grundmodul für Wände und modulare Gebäudefassaden.
- **Konzept:** Zwei kräftige Randpfosten, obere und untere Querbalken sowie eine Fläche aus breiten vertikalen Brettern. Ein einzelner diagonaler Verstrebungsbalken gibt jeder Wand ein leicht wiedererkennbares Muster. Die Rückseite bleibt ebenfalls verwendbar.
- **Zustände:** Neu und verwittert über Materialinstanz.
- **MVP / Ausbau:** Ein Wandmodul mit beidseitig brauchbarer Darstellung / später Varianten mit stärkerer Reparatur oder Wandfüllung.
- **Budget-Check:** max. 2.000 Dreiecke; Holz-Trim-Sheet.

### `building_wall_door_wood` – Türwand

- **Zweck:** Verbindet den 300-cm-Raster mit einer breiten, im Koop gut passierbaren Öffnung.
- **Konzept:** Wandaufbau wie beim Grundmodul. Die mittige Öffnung ist 130 × 230 cm frei; ein klar sichtbarer Sturz und verstärkte Pfosten rahmen sie. Die Türöffnung hebt sich in der Vogelperspektive durch eine dunklere Schwelle ab.
- **Zustände:** Wandmaterial neu/verwittert. Die Öffnung bleibt frei; das Türblatt ist ein separates Mesh.
- **MVP / Ausbau:** Eine Öffnung mittig im Modul / später alternative Positionen oder Doppeltür, falls der Bauablauf es braucht.
- **Budget-Check:** max. 2.000 Dreiecke; Holz-Trim-Sheet.

### `building_door_wood` – Türblatt

- **Zweck:** Schließt die Türöffnung, bleibt im offenen Zustand gut erkennbar und darf den Durchgang nicht einengen.
- **Konzept:** Ein einfaches Türblatt aus breiten Brettern mit zwei Querstreben und einer diagonalen Verstärkung. Eine Seite trägt gut sichtbare, stilisierte Holzscharniere; die Griffseite erhält eine dunkle Holzlasche. Das Design nutzt keine überfeinen Metallteile.
- **Maße:** ca. 126 × 226 × 6 cm; zur 130 × 230 cm Öffnung mit kleinem umlaufendem Spiel.
- **Zustände:** geschlossen/offen durch Drehung um die Scharnierkante; neu/verwittert über Materialinstanz.
- **MVP / Ausbau:** ein Türblatt / später Verstärkungs- und Materialstufen als Varianten desselben Grunddesigns.
- **Budget-Check:** max. 2.000 Dreiecke; Holz-Trim-Sheet.

### `building_pillar_wood` und `building_beam_wood` – Stützen und Balken

| ID | Konzept und Maße | Zustände / Ausbau | Budget |
|---|---|---|---:|
| `building_pillar_wood` | Vierkantstütze 25 × 25 × 300 cm, leicht gebrochene Kanten, einfache Kerbe am oberen Anschluss. | Neu/verwittert über Materialinstanz; später beschädigte Enden. | max. 2.000 Dreiecke |
| `building_beam_wood` | Balken 300 cm lang, ca. 20 × 25 cm Querschnitt. Zwei Module bilden längere Spannweiten; die Stoßstelle wird als einfacher Überblattungsstoß gestaltet. | Neu/verwittert über Materialinstanz; spätere Längenvarianten nur bei nachgewiesenem Bedarf. | max. 2.000 Dreiecke |

**Zweck:** Stützen und Balken machen Tragstruktur und Bauanschlüsse auf Distanz lesbar.  
**Material:** Holz-Trim-Sheet, mit etwas dunkleren Flächen an Kontaktstellen.  
**MVP:** je ein wiederverwendbares Modul; keine separaten Beschlag- oder Schadensassets.

### `building_roof_wood`, `building_roof_ridge_wood` und `building_roof_gable_wood` – Dachgruppe

- **Zweck:** Schließt ein 300-cm-Rastergebäude mit durchgängiger Dachform ab.
- **Konzept Dachfläche:** Eine klare 45°-Dachfläche mit breitem Traufbalken. Die sichtbare Unterseite zeigt wenige große Sparren. Als nordischer Akzent ist eine schmale Turf-Kante vorgesehen; die Hauptfläche nutzt eine einfache Holzdeckung. Turf verwendet nach Möglichkeit die vorhandene Terrain-Grass-Materialfamilie.
- **Konzept First:** 300-cm-Firststück aus zwei breiten Hölzern mit einfacher, wiederholbarer Überdeckung. Die Firstlinie bleibt in der Vogelperspektive deutlich sichtbar.
- **Konzept Giebel:** Dreieckiges Füllmodul 300 cm breit und 150 cm hoch, passend auf eine 300-cm-Wand. Ein massiver Randrahmen und wenige horizontale Bretter halten die Dreiecksform klar.
- **Maße:** Bei 45° Neigung beträgt die Dachfläche vom First bis zur Traufe ca. 212 cm; die Firsthöhe liegt bei einem 300-cm-Wandmodul etwa 150 cm über der Wandoberkante.
- **Zustände:** Neu/verwittert über Holzmaterialinstanz. Kein Schneefang, Moos oder Schaden im MVP.
- **MVP / Ausbau:** eine Dachfläche, ein Firststück und ein Giebel / später separate Trauf- oder Sturmschadenvarianten.
- **Budget-Check:** jedes Modul max. 2.000 Dreiecke; Holz-Trim-Sheet. Für Turf wird eine vorhandene Terrain-Grass-Materialfamilie genutzt, keine eigene 4K-Textur.

### `trim_wood` – gemeinsame Holz-Trim-Sheet-Familie

- **Zweck:** Einheitliche Oberflächen für alle Bauteile der Holzstufe.
- **Konzept:** Ein 2048²-Atlas mit wenigen großen, gut wiederverwendbaren Flächen: helles Brett, dunkle Brettkante, Balkenfläche, Schnittkante und sparsame Schad-/Kerbzone. Die Materialfamilie enthält Base Color, Normal und ORM. Helligkeit und Abnutzung werden über Materialinstanzen gesteuert.
- **Zustände:** `MI_Trim_Wood_New` und `MI_Trim_Wood_Weathered` als MVP-Varianten; die Geometrie bleibt gleich.
- **MVP / Ausbau:** eine Trim-Sheet-Familie und zwei Instanzen / später weitere Farbvarianten ohne neue Texturauflösung.
- **Budget-Check:** je Map 2048²; keine 4K-Textur und keine individuelle Bauteiltextur.

## Entscheidungen, die der Nutzer treffen muss

1. **Dachdeckung:** Für den MVP empfehle ich Holzdeckung mit schmaler Turf-Kante. Soll der Turf-Akzent bleiben oder soll das gesamte Dach als Turf-Dach ausgeführt werden?
2. Die Rastermaße und Budgets folgen dem Asset-Plan als Vorschlagswerte. Änderungen sind nur nötig, wenn du ein Bauteil bewusst größer, kleiner oder detailreicher möchtest.

## KI-Inhaltsnachweis

| Datei | Werkzeug | Datum | Lizenz/Quelle |
|---|---|---|---|
| `03_assets_baukasten.md` | OpenAI Codex | 2026-11-12 | KI-generierter Konzepttext; keine externen Assets verwendet |
