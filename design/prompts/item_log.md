# Prompt-Paket `item_log` – Baumstamm (Phase 1, Prio A)

*Nach `design/PROMPT_STANDARD.md`. Status: Entwurf von Claude, noch nicht erzeugt.*

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

## Prompt 1 – Konzeptblatt (V1)
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

## Prompt 2 – Bild-zu-3D (V2)
Eingabe: Vorderansicht aus dem abgenommenen Konzeptblatt (oder alle drei Ansichten, falls das Werkzeug Mehrfachansichten annimmt).
```
Single watertight object, no base or pedestal, centered, real-world proportions
as in the reference, PBR textures (base color, normal, roughness), no baked
lighting in the base color, symmetry: no, target polycount: 900.
```

## Abnahme
| Teil | Kriterium | Ergebnis |
|---|---|---|
| A | bei 64 px als Stamm erkennbar | offen |
| A | Rinde und Schnittfläche innerhalb ΔE ≤ 10 der Palette | offen |
| A | drei Ansichten, gleicher Maßstab, keine Perspektive | offen |
| A | keine Äste, kein Moos, kein Text | offen |
| B | 200 cm ± 2 %, Ø 30 cm ± 2 % | offen |
| B | ≤ 300 Dreiecke (max. 1.000), 3 LODs | offen |
| B | geschlossen, Ursprung Mitte, Längsachse X | offen |
