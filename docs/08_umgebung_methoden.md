# Umgebung erstellen: Methoden aus den Tutorials und unsere Umsetzung

Stand 2026-11-13. Sammlung der Methoden aus den Videos, die der Nutzer geschickt hat, und wie wir sie **ohne Kosten** umsetzen.
Bezahlte Werkzeuge (QuickScatter, Ultra Dynamic Sky, Quixel-/Fab-Pakete) nutzen wir nicht. Wir übernehmen die Methode und bauen sie mit Engine-Bordmitteln (PCG, Höhennebel, Landscape) und CC0-Dateien nach.

## Quellen (Videos des Nutzers)

| Nr. | Thema | Kernidee |
|---|---|---|
| V1 | Straßen mit Landscape-Splines und PCG | Spline formt das Gelände und trägt Weg-Meshes, PCG spart den Weg aus |
| V2 | Natürliches Gras mit PCG | Saatpunkte, Punktraster je Saatpunkt, Größe nach Abstand zum Saatpunkt, ohne Kollision |
| V3 | Fotorealer Wald in 7 Minuten (PCG) | Schichten: Bodenplatten, Bäume, Sträucher, Kleinzeug; je Schicht Rauschen, Filter, Zufallsdrehung |
| V4 | Bodennebel (mit Ultra Dynamic Sky) | Volumetrischer Nebel, Bodennebel nur in bestimmten Zonen, dezent einstellen |
| V5 | Offene-Welt-Wald (mit QuickScatter) | „Beim Laden“ gegen „zur Laufzeit“, räumliches Rauschen für Klumpen, Biom-Karte, Spline-Masken |
| Bild | Referenz: lichter Kiefernwald auf Sand | Zutaten für das „Rezept“ (siehe unten) |

## Methoden

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

## Referenzbild als „Rezept“ (V5-Methode)

Hauptzutaten, die das Bild ausmachen:
1. **Waldkiefer** (Pinus sylvestris): hohe, gerade Stämme, Krone erst im oberen Drittel, **oben orange „Spiegelrinde“, unten graubraune Borke**. Passt zum Wikinger-Norden.
   Folge für uns: Kiefer in `build_trees.py` höher und schlanker, Rinde nach Höhe überblenden (unten Borke, oben orange). Noch nicht umgesetzt.
2. **Heller Sandboden**, der zwischen den Pflanzen durchscheint → Boden-Textur „Sand“.
3. **Niedrige grüne Sträucher** in Gruppen → Bodendecker-Schicht mit Klumpen-Rauschen.
4. **Totholz**: Stümpfe, liegende Äste → Deko-Schicht (kleiner als die erntbaren Äste, damit man sie nicht verwechselt).
5. **Fels** (geschichteter Sandstein) → Felsen-Vorkommen, evtl. weiterer Scan.

## Was wir nicht übernehmen

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
