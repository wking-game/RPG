# Prompt-Standard für KI-erzeugte Assets (Stand 2026-11-12)

*Verbindlich für alle Prompts, mit denen Konzeptbilder, Texturen, Icons und 3D-Modelle erzeugt werden – egal ob ChatGPT, Codex, Claude oder der Nutzer sie schreibt.*
*Ziel des Nutzers: Prompts so genau, dass das Ergebnis der Vorgabe so nahe wie möglich kommt.*

## 1. Grundsatz und ehrliche Grenze
Ein perfekter Prompt garantiert **kein** perfektes Ergebnis. KI-Generatoren halten sich nur ungefähr an Zahlen, Maße und Pixelpositionen. Genauigkeit entsteht deshalb aus **vier Stufen zusammen**:

1. **Präziser Prompt** nach diesem Standard (alle Pflichtfelder, feste Stil- und Ausschlussblöcke).
2. **Referenzbild statt reinem Text:** Für 3D immer zuerst ein orthografisches Konzeptblatt erzeugen und daraus das Modell (Bild-zu-3D). Text-zu-3D nur als Notlösung.
3. **Automatische Nachbearbeitung** in Blender per Skript: Maßstab, Ursprung, Dreiecke, LODs, UVs. Was die KI ungenau macht, korrigiert das Skript exakt.
4. **Messbare Abnahme** (Abschnitt 7): Nur was alle Kriterien erfüllt, kommt ins Projekt.

**Sprache:** Die Prompts selbst schreiben wir auf **Englisch**, weil die Generatoren Englisch am zuverlässigsten umsetzen. Steckbrief, Begründung und Abnahme bleiben auf Deutsch.

**Ein Prompt, ein Objekt:** Nie mehrere Assets in einem Prompt. Varianten (Stufen, Zustände) bekommen eigene Prompts, die sich nur im Variantenfeld unterscheiden.

## 2. Pflichtfelder jedes Prompt-Pakets
Jedes Asset bekommt ein **Prompt-Paket** mit diesen Feldern. Fehlt eins, ist das Paket unvollständig.

| Feld | Inhalt | Beispiel |
|---|---|---|
| ID | Asset-ID aus den Listen | `item_log` |
| Ziel-Werkzeug | Bildgenerator, Bild-zu-3D, Texturgenerator | Bild-zu-3D |
| Zweck | wo und wie oft im Spiel sichtbar, aus welcher Distanz | wird getragen und im Lager gestapelt, Nah- und Vogelperspektive |
| Stilblock | **wörtlich** Block S1 aus Abschnitt 3 | – |
| Objekt | was genau, in einem Satz | a single felled pine log |
| Form und Silhouette | Grundform, markante Merkmale, Symmetrie | straight cylinder, slight taper, two clean saw cuts |
| Proportionen | **Verhältnisse statt Zentimeter** (Generatoren verstehen keine cm) | length is 6.5 times the diameter |
| Maße (Soll) | echte Maße in cm, nur für die Nachbearbeitung | 200 × Ø 30 cm |
| Materialien | Material je Bereich mit Hex-Farbe aus der Palette | bark #5F4833, cut face #B08A5C |
| Oberfläche und Abnutzung | Grad der Abnutzung auf einer Skala 0–3 | wear level 1: subtle |
| Stufe/Zustand | T1–T6, `fresh`/`worn`, voll/leer | – |
| Ansicht und Kamera | Blickwinkel, Brennweite, Ausrichtung | orthographic front, side, top |
| Licht | immer gleich: Block L1 | – |
| Hintergrund | immer gleich: Block H1 | – |
| Ausschlüsse | **wörtlich** Block N1 plus objektbezogene Ausschlüsse | no branches, no moss |
| Varianten | Anzahl der Durchläufe, Seed, falls das Werkzeug ihn anbietet | 4 Durchläufe, Seed notieren |
| Abnahme | Kriterien aus Abschnitt 7, ausgefüllt | – |
| Nachbearbeitung | welches Skript, welche Zielwerte | `normalize_import.py`, 200 cm, Ziel 300 Dreiecke |
| KI-Nachweis | Werkzeug, Version, Datum, Lizenzstufe des Kontos | für `design/asset_log.md` |

## 3. Feste Textblöcke (wörtlich verwenden, nie umformulieren)

### S1 – Stilblock Klarform-Saga
```
Stylized Norse early-medieval game asset in the "Klarform Saga" art direction:
broad, calm primary shapes with only a few strong details, a clean and instantly
readable silhouette, hand-crafted feel with subtle natural wear, matte surfaces.
Limited color palette: fjord green #3E6655, slate blue #607C8B, bone white #E5D8BC,
ochre #D2A64F, brick red #A95D48, fresh wood #8E6B46, aged wood #5F4833,
linen #D8C9A8. Must stay readable from a top-down view and at low graphics settings.
```

### L1 – Licht
```
Soft, even, neutral daylight from the upper left, no colored light, no rim light,
no dramatic shadows, no baked ambient occlusion darker than 20 percent.
```

### H1 – Hintergrund
```
Isolated on a plain flat neutral grey background (#808080), no ground plane,
no cast shadow on the background, object fully inside the frame with margin.
```

### N1 – Ausschlüsse (Negativ-Prompt)
```
text, letters, numbers, runes used as decoration, watermark, logo, signature,
frame, border, UI elements, multiple objects, cropped object, background scenery,
ground, grass, horns on helmets, glowing effects, magic particles, modern objects,
screws, plastic look, photoscan look, high-frequency noise, heavy grime, blood,
resemblance to existing video game assets or characters
```
Werkzeuge ohne eigenes Negativ-Feld: Block N1 mit „Avoid:“ an das Ende des Prompts setzen.

### Abnutzungsskala
| Stufe | Bedeutung | Englisch im Prompt |
|---|---|---|
| 0 | neu, sauber | wear level 0: brand new, clean edges |
| 1 | leicht benutzt (Standard) | wear level 1: subtle edge wear, slightly weathered |
| 2 | deutlich benutzt (`worn`) | wear level 2: visible wear, scratches, faded color |
| 3 | stark beschädigt | wear level 3: heavily damaged (nur auf ausdrückliche Anweisung) |

## 4. Vorlagen nach Asset-Typ

### V1 – Konzeptblatt (Grundlage für jedes 3D-Modell)
```
[S1]
Orthographic reference sheet of [OBJEKT], shown three times side by side at
identical scale: front view, side view, top view. No perspective distortion.
Shape: [FORM UND SILHOUETTE]. Proportions: [PROPORTIONEN].
Materials: [MATERIALIEN MIT HEX].
Surface: [ABNUTZUNGSSTUFE]. Variant: [STUFE/ZUSTAND].
[L1] [H1]
Avoid: [N1], [OBJEKTBEZOGENE AUSSCHLÜSSE]
```

### V2 – Bild-zu-3D
Eingabe: das abgenommene Konzeptblatt, nur die **Vorderansicht** oder alle drei Ansichten, je nach Werkzeug.
Einstellungen (werkzeugneutral):
```
Single watertight object, no base or pedestal, centered, real-world proportions
as in the reference, PBR textures (base color, normal, roughness), no baked
lighting in the base color, symmetry: [ja/nein], target polycount: [ZIEL × 3]
(the script reduces it afterwards).
```

### V3 – Kachelbare Textur (je Trim-Streifen ein Prompt)
Generatoren treffen keine exakten Pixelzeilen. Deshalb wird **jeder Streifen des Trim-Sheets einzeln** als nahtlos kachelbare Textur erzeugt. Ein Skript setzt die Streifen danach pixelgenau zum Atlas zusammen.
```
[S1]
Seamless tileable texture, flat orthographic top-down scan of [MATERIAL],
[AUSRICHTUNG, z. B. boards running horizontally], exactly [ANZAHL] [ELEMENTE]
stacked vertically filling the full height, each the same height, [FARBEN MIT HEX],
[ABNUTZUNGSSTUFE], evenly lit, no perspective, no vignette, no shadows,
tiles seamlessly on the left and right edges.
Avoid: [N1], visible seams, repeating stains
```

### V4 – Icon
Alle Icons gleich, damit das Inventar ruhig wirkt.
```
[S1]
Game inventory icon of [OBJEKT], three-quarter view from the front left,
camera elevation 30 degrees, object rotated 35 degrees, object fills 80 percent
of the canvas, centered, slight dark outline (#2E2B28, 2 px at 512 px),
[MATERIALIEN MIT HEX], [STUFE].
[L1] Transparent or plain #808080 background.
Avoid: [N1], frame, tier color, shadow outside the object
```
Der **Stufen-Rahmen wird nicht generiert**, sondern in der Engine darübergelegt.

## 5. Ablauf
1. Claude oder ChatGPT schreibt das Prompt-Paket nach Abschnitt 2.
2. Der Nutzer prüft es (oder gibt es frei), dann wird erzeugt: zuerst V1 (Konzeptblatt).
3. Abnahme des Konzeptblatts (Abschnitt 7, Teil A). Erst dann V2/V3/V4.
4. Claude normalisiert das Ergebnis per Skript und prüft Teil B.
5. Eintrag ins Asset-Log mit Werkzeug, Version, Datum und Lizenzstufe.

## 6. Ablage
- Prompt-Pakete: `design/prompts/<id>.md`
- Rohausgaben der KI: `Assets/_Raw/<id>/` (nicht ins Spiel)
- Abgenommene, normalisierte Assets: `Assets/<Gruppe>/` (Struktur und `manifest.json` wie von `tools/blender/build_assets.py` erzeugt)

## 7. Abnahme-Kriterien (messbar)
**Teil A – Bild (Konzeptblatt, Textur, Icon):**
| Kriterium | Grenze |
|---|---|
| Silhouette erkennbar | bei 64 × 64 px noch eindeutig |
| Farben | Hauptflächen innerhalb der Palette (Farbabstand ΔE ≤ 10 zum nächsten Palettenwert) |
| Ansichten (V1) | drei Ansichten, gleicher Maßstab, keine Perspektive |
| Kachelbarkeit (V3) | keine sichtbare Naht bei 3 × 3-Kachelung |
| Ausschlüsse | kein Element aus N1 sichtbar |

**Teil B – 3D nach der Nachbearbeitung:**
| Kriterium | Grenze |
|---|---|
| Maße | ±2 % zum Soll |
| Ursprung | wie im Steckbrief |
| Dreiecke | ≤ Ziel, sicher ≤ Obergrenze |
| Geometrie | geschlossen, keine schwebenden Teile, keine umgedrehten Normalen |
| Texturen | Größe laut Budget, kein eingebranntes Licht |
| LODs | 3 Stufen erzeugt |

## 8. Ausgearbeitete Beispiele
- `design/prompts/item_log.md` – 3D über Konzeptblatt und Bild-zu-3D
- `design/prompts/trim_wood.md` – Trim-Sheet aus fünf Streifen
- `design/prompts/ui_icon_res_wood.md` – Icon
