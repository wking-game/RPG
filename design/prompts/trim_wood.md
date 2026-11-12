# Prompt-Paket `trim_wood` – Holz-Trim-Sheet (Phase 1, Prio A)

*Nach `design/PROMPT_STANDARD.md`. Status: Entwurf von Claude, noch nicht erzeugt.*
*Layout verbindlich aus `art/wood_kit/README.md`: 2048 × 2048 px, 1 UV = 200 cm, u kachelt waagerecht.*

| Feld | Inhalt |
|---|---|
| ID | `trim_wood` → `T_Trim_Wood_BC`, `T_Trim_Wood_N`, `T_Trim_Wood_ORM` |
| Ziel-Werkzeug | Bild- oder Texturgenerator (V3), **fünf getrennte Prompts**, einer je Streifen |
| Zweck | Oberflächen aller Holz-Bauteile (Wand, Boden, Dach, Pfosten, Tür …). Größte sichtbare Fläche der Siedlung, aus 1–50 m. |
| Stufe/Zustand | T1. `fresh` und `worn` entstehen später über Materialinstanzen, **nicht** über eigene Texturen |
| Licht / Hintergrund | gleichmäßig, ohne Schatten (in V3 enthalten); kein Hintergrund (Textur füllt alles) |
| Ausschlüsse | N1 + `visible seams, repeating stains, nails, paint, carvings, knots larger than 4 cm` |
| Varianten | je Streifen 4 Durchläufe, beste Kachel wählen |
| Nachbearbeitung | Skript setzt die Streifen zusammen: jeden auf 2048 px Breite skalieren, auf die exakte Streifenhöhe bringen (Stauchung ≤ 12 % erlaubt), untereinander anordnen. **Normal und ORM:** aus dem Werkzeug, wenn es PBR-Karten liefert; sonst erzeugt Claude sie per Skript aus der Helligkeit (Näherung, Qualität prüfen) |
| KI-Nachweis | je Streifen ein Eintrag in `design/asset_log.md` |

## Streifen-Layout (Zielpositionen im fertigen Atlas)
| Streifen | v-Bereich | Pixelzeilen von oben (2048 px) | reale Höhe | Inhalt | Seitenverhältnis beim Erzeugen |
|---|---|---|---|---|---|
| plank | 0,70–1,00 | 0–614 | 60 cm | 4 Bretter à 15 cm, waagerecht | 10 : 3 (notfalls 3 : 1) |
| timber | 0,45–0,70 | 614–1126 | 50 cm | Balkenfläche, Maserung waagerecht | 4 : 1 |
| end | 0,35–0,45 | 1126–1331 | 20 cm | Hirnholz, Jahresringe | 10 : 1 (notfalls 8 : 1) |
| dark | 0,10–0,35 | 1331–1843 | 50 cm | 4 dunkle Bretter à 12,5 cm | 4 : 1 |
| metal | 0,00–0,10 | 1843–2048 | 20 cm | dunkles, geschmiedetes Eisen | 10 : 1 (notfalls 8 : 1) |

## Prompt 1 – plank
```
Stylized Norse early-medieval game asset in the "Klarform Saga" art direction:
broad, calm primary shapes with only a few strong details, a clean and instantly
readable silhouette, hand-crafted feel with subtle natural wear, matte surfaces.
Limited color palette: fjord green #3E6655, slate blue #607C8B, bone white #E5D8BC,
ochre #D2A64F, brick red #A95D48, fresh wood #8E6B46, aged wood #5F4833,
linen #D8C9A8. Must stay readable from a top-down view and at low graphics settings.

Seamless tileable texture, flat orthographic top-down scan of hand-split pine
planks, boards running horizontally, exactly 4 boards stacked vertically filling
the full height, each the same height, separated by thin dark gaps #4A3726,
board color fresh wood #8E6B46 with gentle variation between boards of at most
10 percent brightness, soft straight wood grain along the boards, two or three
small knots in total, wear level 1: subtle edge wear, slightly weathered,
evenly lit, no perspective, no vignette, no shadows, tiles seamlessly on the
left and right edges. Aspect ratio 10:3.

Avoid: text, letters, numbers, runes used as decoration, watermark, logo, signature,
frame, border, UI elements, multiple objects, cropped object, background scenery,
ground, grass, horns on helmets, glowing effects, magic particles, modern objects,
screws, plastic look, photoscan look, high-frequency noise, heavy grime, blood,
resemblance to existing video game assets or characters, visible seams,
repeating stains, nails, paint, carvings, knots larger than 4 cm
```

## Prompt 2 – timber
Wie Prompt 1, aber Objektteil ersetzen durch:
```
Seamless tileable texture, flat orthographic scan of the side face of a single
hewn pine beam, grain running horizontally, faint adze marks every 30 cm,
color #7A5A3A with slightly darker streaks #5F4833, no gaps, no board edges,
wear level 1: subtle edge wear, slightly weathered, evenly lit, no perspective,
no vignette, no shadows, tiles seamlessly on the left and right edges.
Aspect ratio 4:1.
```

## Prompt 3 – end
```
Seamless tileable texture, flat orthographic scan of a row of sawn pine end grain,
clearly visible concentric growth rings, base #B08A5C, rings #8E6B46,
fine radial cracks, evenly lit, no perspective, no vignette, no shadows,
tiles seamlessly on the left and right edges. Aspect ratio 10:1.
```

## Prompt 4 – dark
```
Seamless tileable texture, flat orthographic top-down scan of old weathered pine
planks, boards running horizontally, exactly 4 boards stacked vertically filling
the full height, each the same height, separated by thin very dark gaps #33261A,
board color aged wood #5F4833, slightly greyed, wear level 2: visible wear,
scratches, faded color, evenly lit, no perspective, no vignette, no shadows,
tiles seamlessly on the left and right edges. Aspect ratio 4:1.
```

## Prompt 5 – metal
```
Seamless tileable texture, flat orthographic scan of hand-forged dark iron strap,
hammer-dimpled surface, color #2E2B28 with slightly lighter worn high points
#4A4744, faint rust tint only in small spots #6B4A33, matte, evenly lit,
no perspective, no vignette, no shadows, tiles seamlessly on the left and
right edges. Aspect ratio 10:1.
```
(Für Prompt 2–5 jeweils Stilblock S1 davor und Ausschlussblock wie in Prompt 1 danach setzen.)

## Abnahme
| Teil | Kriterium | Ergebnis |
|---|---|---|
| A | jede Kachel nahtlos bei 3 × 3-Kachelung | offen |
| A | plank und dark: genau 4 Bretter, gleich hoch (± 5 %) | offen |
| A | Farben innerhalb ΔE ≤ 10 der Sollwerte | offen |
| A | keine Nägel, keine Farbe, keine Schnitzereien, kein Text | offen |
| B | fertiger Atlas 2048 × 2048, Streifengrenzen exakt laut Tabelle | offen |
| B | auf den Bauteilen aus `art/wood_kit/` getestet: Bretter laufen richtig, keine Verzerrung | offen |
