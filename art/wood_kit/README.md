# Holzbaukasten – erzeugte Module (Phase 1, Priorität A)

Erzeugt mit `tools/blender/build_wood_kit.py` (Blender, headless) nach `design/inbox/03_assets_baukasten.md`.
Geometrie ist **Platzhalter in endgültigen Maßen**: richtige Größen, Pivots und UVs, aber noch ohne endgültige Textur.
Einheit: 1 = 1 cm. Export: FBX, Z oben. **Import in Unreal noch nicht geprüft** (Maßstab, Ausrichtung).

| Modul | Dreiecke | Ziel | Bounding Box min → max (cm) | Ursprung |
|---|---:|---:|---|---|
| `SM_Building_Foundation_Wood` | 58 | 500 | (-150, -150, -120) → (150, 150, 0) | Mitte der Oberseite (Oberseite 20 cm ueber Gelaende, Sockel bis 120 cm darunter) |
| `SM_Building_Floor_Wood` | 50 | 500 | (-150, -150, -35) → (150, 150, 0) | Mitte der Oberseite |
| `SM_Building_Wall_Wood` | 70 | 800 | (-150, -10, 0) → (150, 10, 300) | unten mittig auf der Rasterlinie |
| `SM_Building_Wall_Door_Wood` | 106 | 800 | (-150, -10, 0) → (150, 10, 300) | unten mittig auf der Rasterlinie |
| `SM_Building_Door_Wood` | 80 | 400 | (0, -9, 0) → (126, 7, 226) | untere Scharnierkante (Tuer liegt bei +X) |
| `SM_Building_Pillar_Wood` | 28 | 200 | (-12.5, -12.5, 0) → (12.5, 12.5, 300) | unten mittig |
| `SM_Building_Beam_Wood` | 28 | 200 | (-150, -10, -12.5) → (150, 10, 12.5) | Mitte der Laengsachse |
| `SM_Building_Roof_Wood` | 78 | 600 | (-150, -14.1, -10.6) → (150, 160.6, 158.5) | Traufkante unten mittig (Dach steigt Richtung +Y) |
| `SM_Building_Roof_Ridge_Wood` | 20 | 200 | (-150, -21.2, -0.7) → (150, 21.2, 24) | Firstlinie der Dach-Unterseite, mittig |
| `SM_Building_Roof_Gable_Wood` | 34 | 400 | (-150, -10, -14.1) → (150, 10, 150) | unten mittig auf der Rasterlinie |

## Trim-Sheet-Layout (verbindlich für die echte Textur)

Eine Textur `T_Trim_Wood` (2048², BC/N/ORM). u kachelt waagerecht, v ist in Streifen geteilt. 1 UV = 200 cm.

| Streifen | v von–bis | Höhe real | Inhalt |
|---|---|---|---|
| plank | 0,70–1,00 | 60 cm | helle Bretter, laufen waagerecht (entlang u), 4 Bretter à 15 cm |
| timber | 0,45–0,70 | 50 cm | Balkenflächen, Maserung entlang u |
| end | 0,35–0,45 | 20 cm | Hirnholz, Schnittkanten |
| dark | 0,10–0,35 | 50 cm | dunkle, gealterte Bretter (Sockel, Unterseiten), à 12,5 cm |
| metal | 0,00–0,10 | 20 cm | dunkle Beschläge (Türbänder, Griff) |

Turf-Kante (nur Dach, Material-Slot 2): eigene kachelbare Grastextur, 1 UV = 200 cm.
Deutung: Die „schmale Turf-Kante“ liegt als 30 cm breiter, 8 cm hoher Streifen an der **Traufe** auf der Dachfläche.
