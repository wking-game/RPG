# 04 – Assets: Arbeitsstätten der Siedlung (Phase 1)

**Stil:** Klarform-Saga im mittelalterlich-nordischen Wikinger-Kontext.  
**Umfang:** Abschnitt 5.4, ausschließlich Arbeitsstätten mit Priorität A. Werkbank und Totem (Priorität B/C) bleiben außen vor. Alle Größen sind Vorschlagswerte für den Vertical Slice.

## Gemeinsame Regeln

- Holzdetails verwenden die gemeinsame Familie `trim_wood` aus `03_assets_baukasten.md`.
- Gebäude und Stationen verwenden nach Möglichkeit die 300-cm-Baumodule. Kleine Stationen dürfen innerhalb eines Baufelds kleiner sein.
- Aufgaben sind aus der Vogelperspektive über die Hauptform und ein bis zwei große Arbeitsrequisiten erkennbar. Kleine Props und Textdetails sind kein primärer Lesbarkeitsträger.
- Füllzustände des Lagers verwenden die bereits geplanten Ressourcen-Assets (`item_log`, `item_plank`, `item_stone`, `item_berries`, `item_meal`). Es entstehen dafür keine eigenen Lagergut-Modelle.
- Die Budgetwerte unten sind Ziele. Einzigartige Siedlungsgebäude bleiben unter dem Limit von 15.000 Dreiecken und verwenden höchstens eine zusätzliche 1024²-Textur; das MVP zielt auf gar keine eigene Gebäudetextur.

## Arbeitsstättenregister – Priorität A

| ID | Unreal-Name | Zweck | Empfohlene Maße | Zielbudget |
|---|---|---|---|---:|
| `building_storage` | `SM_Building_Storage` | Zentrales Siedlungslager | 600 × 600 cm Grundfläche; ca. 450 cm bis First | max. 8.000 Dreiecke; keine eigene Textur |
| `building_woodcutter` | `SM_Building_Woodcutter` | Arbeitsplatz Holzfällen/Holzbearbeitung | 300 × 300 cm Arbeitsfläche; ca. 100 cm Requisitenhöhe | max. 4.000 Dreiecke |
| `building_stonecutter` | `SM_Building_Stonecutter` | Arbeitsplatz Steine bearbeiten | 300 × 300 cm Arbeitsfläche; ca. 130 cm Requisitenhöhe | max. 5.000 Dreiecke |
| `building_campfire` | `SM_Building_Campfire` | Kochstelle und sozialer Treffpunkt | ca. 200 × 200 cm; ca. 170 cm inkl. Kochaufhängung | max. 2.000 Dreiecke |
| `building_bed` | `SM_Building_Bed` | Schlafplatz für eine Figur | 210 × 90 × 50 cm | max. 1.000 Dreiecke |

### `building_storage` – zentrales Siedlungslager

- **Zweck:** Ein einziges, leicht auffindbares Lager pro Siedlung. Der Bestand soll im Gebäude-Look sichtbar werden und zusätzlich in der Lager-UI auf einen Blick lesbar sein.
- **Konzept:** Kompakter, zweigeteilter Holzspeicher mit 600 × 600 cm Grundfläche. Die breite Front hat eine überdachte Abladezone, zwei kräftige Pfosten und eine klar lesbare Doppelnutzung: geschlossener Innenraum plus offene Regalbucht. Satteldach mit deutlich erkennbarem First; Wand- und Dachmodule nutzen den Holzbaukasten.
- **Füllzustände:**
  - **Leer:** offene Regalbucht und Ablageflächen sichtbar, höchstens ein einzelner Arbeitskorb.
  - **Gefüllt:** sichtbare Stapel aus den geplanten Ressourcen-Assets. Holz, Bretter und Stein erhalten getrennte Ablagezonen; Beeren und Mahlzeiten stehen geschützt auf einem Regal.
  - Im MVP reichen zwei eindeutige Zustände (leer/gefüllt). Feinere Abstufungen können später folgen.
- **Materialien:** Holz-Trim-Sheet; dunklere Holzinstanz an Dach und Boden; vorhandene Steinmaterialfamilie nur für einzelne Bodenanker. Keine eigene Textur vorgesehen.
- **MVP / Ausbau:** ein Lagergebäude aus vorhandenen Wand-, Boden-, Dach- und Fundamentteilen mit zwei Füllzuständen / später gestaffelte Füllstände, Reparaturzustand und Materialstufen.
- **Budget-Check:** Ziel max. 8.000 Dreiecke inklusive sichtbarer Regale und Dach. Harte Obergrenze 15.000. Gemeinsame Holz-Trim-Sheet-Familie; keine zusätzliche 1024²-Textur im MVP.

### `building_woodcutter` – Holzfäller-Arbeitsplatz

- **Zweck:** Zeigt aus der Distanz, wo Holzfäller arbeiten und wo gefällte Stämme gesammelt werden.
- **Konzept:** Offene Arbeitsfläche auf einem 300-cm-Feld. Ein breiter Hackklotz steht nahe an einem niedrigen Holzgestell mit zwei bis drei ordentlich gestapelten Stämmen. Eine Axt steckt sichtbar im Klotz oder liegt in einer gut erkennbaren Halterung. Die Requisiten bilden eine kompakte Silhouette und bleiben unter Augenhöhe der Dorfbewohner.
- **Materialien:** Holz-Trim-Sheet, gemeinsame Stamm-/Holzmaterialien aus den geplanten Baum- und Item-Assets. Keine eigene Textur.
- **Zustände:** unbesetzt/besetzt wird über Bewohner und UI angezeigt; die Station selbst hat im MVP einen festen ordentlichen Arbeitszustand.
- **MVP / Ausbau:** Hackklotz, Holzgestell und kleiner Vorrat / später deutlicherer Arbeitsfortschritt über veränderbare Holzstapel und Reparaturzustand.
- **Budget-Check:** Ziel max. 4.000 Dreiecke für alle Stationsteile zusammen. Wiederverwendung von Baumstamm- und Werkzeugformen, keine zusätzlichen hochauflösenden Texturen.

### `building_stonecutter` – Steinmetz-Arbeitsplatz

- **Zweck:** Kennzeichnet den Platz, an dem Dorfbewohner Steine bearbeiten und sammeln.
- **Konzept:** Ein niedriger, massiver Arbeitstisch aus zwei Holzstützen und einer großen, helleren Steinplatte. Daneben steht ein grob behauener Werkstein mit klar sichtbarer Schnitt- oder Spaltfläche; wenige größere Steinsplitter markieren den Arbeitsbereich. Picke und Hammer werden aus den geplanten Werkzeug-Assets wiederverwendet.
- **Materialien:** Holz-Trim-Sheet und gemeinsame Steinmaterialfamilie der Felsen/Ressourcen. Keine einzigartige Steinstation-Textur.
- **Zustände:** Ein fester Stationszustand im MVP. Besetzung und Priorität zeigt die Dorf-UI; feiner Staub und Trefferfunken gehören zu den später ausgearbeiteten Effekten.
- **MVP / Ausbau:** Tisch, Arbeitsstein und wenige Splitter / später zwei Zustände des Werksteins (roh/bearbeitet) und mehr sichtbarer Arbeitsfortschritt.
- **Budget-Check:** Ziel max. 5.000 Dreiecke für das gesamte Ensemble. Steintextur aus der gemeinsamen 1024²-Ressourcenfamilie; Holz aus `trim_wood`.

### `building_campfire` – Lagerfeuer und Kochstelle

- **Zweck:** Gemeinsame Kochstelle und warmer Sammelpunkt. Das Feuer ist als wichtiger Siedlungsort schon in der Vogelperspektive erkennbar.
- **Konzept:** Flacher Ring aus sechs bis acht wiederverwendbaren Steinen, ungefähr 150 cm Außendurchmesser. Drei große Feuerhölzer liegen sichtbar im Ring. Ein einfacher, klarer Holzgalgen trägt einen dunklen Kochkessel über der Mitte; keine filigranen Ketten. Das Feuer bleibt tief genug, um Gesichter und Requisiten sichtbar zu lassen.
- **Maße:** ca. 200 × 200 cm für den gesamten Arbeitsbereich; ca. 170 cm bis zur Kesselaufhängung.
- **Materialien:** gemeinsame Stein- und Holzmaterialien; Kessel als dunkles, einfaches Metallmaterial. Das Feuer-/Rauchsystem ist ein separates Asset `vfx_campfire` aus Abschnitt 5.7 und wird hier nur als Abhängigkeit notiert.
- **Zustände:** Feuer aus/an über `vfx_campfire`; Kessel bleibt als Teil der Station sichtbar. Kein Countdown oder Zeitdruck-Signal.
- **MVP / Ausbau:** ein Feuer- und Kochzustand / später Feuer aus, Glut und volles Feuer als getrennte visuelle Varianten sowie zusätzliche Kochgefäße.
- **Budget-Check:** Ziel max. 2.000 Dreiecke ohne Niagara-Effekt. Gemeinsame Stein-/Holzmaterialien, keine eigene Textur.

### `building_bed` – Strohlager

- **Zweck:** Einfacher Schlafplatz für eine Figur. Im Dorf muss er als nutzbares Bett erkennbar sein, ohne die Innenräume mit Details zu überladen.
- **Konzept:** Niedriger, vierbeiniger Rahmen aus breiten Holzbalken. Eine voluminöse Strohlage bildet eine klare helle Fläche; eine gefaltete, ungefärbte Wolldecke liegt am Fußende. Das Bett steht bodennah und kann in Reihen oder entlang einer Innenwand platziert werden.
- **Maße:** 210 × 90 × 50 cm; Länge bietet einer ca. 180-cm-Figur sichtbaren Spielraum.
- **Materialien:** Holz-Trim-Sheet; einfarbiges, mattes Strohmaterial; Wolldecke in Leinenfarbe. Für das MVP genügt bei Stroh und Decke ein einfaches gemeinsames Material ohne eigene hochauflösende Textur.
- **Zustände:** unbenutzt und benutzt über Material-/Farbvariation; keine dynamische Deckenanimation im MVP.
- **MVP / Ausbau:** ein Strohlager mit einer Farbvariante / später bessere Schlafplätze als Materialstufen und optionale Deckenfarben.
- **Budget-Check:** Ziel max. 1.000 Dreiecke; gemeinsame Holzmaterialien und einfache, kostengünstige Stroh-/Stoffoberfläche.

## Entscheidungen, die der Nutzer treffen muss

1. **Lagergröße:** Ist das vorgeschlagene Lager mit 600 × 600 cm passend für eine Siedlung? Es kann bei Bedarf auf ein 300-cm-Raster verkleinert werden.
2. **Kochstelle:** Passt ein offenes Lagerfeuer mit Holzgalgen und Kessel als Kochstelle für den ersten Slice?

## KI-Inhaltsnachweis

| Datei | Werkzeug | Datum | Lizenz/Quelle |
|---|---|---|---|
| `04_assets_arbeitsstaetten.md` | OpenAI Codex | 2026-11-12 | KI-generierter Konzepttext; keine externen Assets verwendet |
