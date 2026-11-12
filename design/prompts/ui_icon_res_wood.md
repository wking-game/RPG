# Prompt-Paket `ui_icon_res_wood` – Icon Baumstamm (Phase 1, Prio A)

*Nach `design/PROMPT_STANDARD.md`. Status: Prompt nicht verwendet. Das Icon wurde am 2026-11-12 **in Blender gerendert** (`Assets/UI/Icons/T_Icon_Res_Wood.png`, Kamera und Kontur wie unten, Füllgrad 0,80). Der Prompt bleibt für eine spätere KI-Fassung erhalten.*
*Dieses Icon legt das **Icon-Raster** fest. Alle weiteren Icons verwenden dieselbe Kamera, dasselbe Licht und dieselbe Kontur.*

| Feld | Inhalt |
|---|---|
| ID | Icon zu `item_log` → `T_Icon_Res_Wood` |
| Ziel-Werkzeug | Bildgenerator (V4) |
| Zweck | Inventar, Lager-UI, Bauanforderungen. Angezeigt mit 48–96 px, muss bei 48 px eindeutig sein. |
| Objekt | three stacked pine logs (two at the bottom, one on top) |
| Form und Silhouette | compact triangular stack, cut faces turned toward the viewer, logs identical in size |
| Proportionen | each log is 4 times as long as its diameter; the stack is as wide as it is tall |
| Materialien | bark #5F4833 with ridges #79664C; cut faces #B08A5C with rings #8E6B46 |
| Oberfläche und Abnutzung | wear level 1 |
| Stufe/Zustand | T1 (Rahmen in Stufenfarbe **nicht** im Bild, kommt aus der Engine) |
| Ansicht und Kamera | three-quarter view from the front left, elevation 30°, object rotated 35°, fills 80 % of the canvas |
| Licht / Hintergrund | L1 / transparent oder #808080 |
| Ausschlüsse | N1 + `frame, tier color, shadow outside the object, axe, branches, moss` |
| Varianten | 4 Durchläufe, 512 × 512 px |
| Nachbearbeitung | Hintergrund freistellen, auf 256 × 256 px verkleinern, Kontur prüfen, als `T_Icon_Res_Wood` (UI-Textur, keine Mipmaps) |
| KI-Nachweis | Eintrag in `design/asset_log.md` |

## Prompt (V4)
```
Stylized Norse early-medieval game asset in the "Klarform Saga" art direction:
broad, calm primary shapes with only a few strong details, a clean and instantly
readable silhouette, hand-crafted feel with subtle natural wear, matte surfaces.
Limited color palette: fjord green #3E6655, slate blue #607C8B, bone white #E5D8BC,
ochre #D2A64F, brick red #A95D48, fresh wood #8E6B46, aged wood #5F4833,
linen #D8C9A8. Must stay readable from a top-down view and at low graphics settings.

Game inventory icon of three stacked pine logs, two at the bottom and one on top,
forming a compact triangular stack, cut faces turned toward the viewer, all logs
identical in size, each log 4 times as long as its diameter, the stack as wide as
it is tall. Three-quarter view from the front left, camera elevation 30 degrees,
object rotated 35 degrees, object fills 80 percent of the canvas, centered,
slight dark outline #2E2B28, 2 px at 512 px.
Materials: bark in aged wood #5F4833 with lighter ridges #79664C, cut faces end
grain #B08A5C with darker growth rings #8E6B46. Surface: wear level 1: subtle edge
wear, slightly weathered.
Soft, even, neutral daylight from the upper left, no colored light, no rim light,
no dramatic shadows, no baked ambient occlusion darker than 20 percent.
Transparent or plain #808080 background. Square 1:1, 512 x 512 px.

Avoid: text, letters, numbers, runes used as decoration, watermark, logo, signature,
frame, border, UI elements, multiple objects, cropped object, background scenery,
ground, grass, horns on helmets, glowing effects, magic particles, modern objects,
screws, plastic look, photoscan look, high-frequency noise, heavy grime, blood,
resemblance to existing video game assets or characters, frame, tier color,
shadow outside the object, axe, branches, moss
```

## Abnahme
| Kriterium | Ergebnis |
|---|---|
| bei 48 px als Holz erkennbar und vom Bretter-Icon unterscheidbar | offen |
| Kamera 3/4 von vorne links, ca. 30° von oben | offen |
| Objekt füllt ca. 80 %, mittig | offen |
| Kontur vorhanden und gleichmäßig | offen |
| kein Rahmen, kein Text, kein Hintergrundmotiv | offen |
| Farben innerhalb ΔE ≤ 10 der Palette | offen |
