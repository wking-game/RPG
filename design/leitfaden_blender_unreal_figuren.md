# Leitfaden: Blender- und Unreal-Assets, Gesichter, Animation

Stand 2026-11-21. Verfasst von Claude Code auf Grundlage der Referenzordner und einer kurzen Web-Recherche.
Ergänzt `design/inbox/Referenzmaterialien_2026-11-21/01_Bauweise_und_Materialien.md` (Gegenstände, Materialkacheln) – das dort Gesagte wird hier nicht wiederholt.

**Ehrlicher Rahmen:** Das Durchsehen der Bilder „trainiert“ das KI-Modell nicht dauerhaft. Was bleibt, ist dieses Dokument (plus ein Memory-Eintrag), das bei künftigen Modellier-Aufgaben gelesen wird. Regeln und Zahlen hier sind Arbeitswerte und Vorschläge von Claude, keine bestätigten Entscheidungen.

## 1. Was ausgewertet wurde

| Quelle | Inhalt | Brauchbar als |
|---|---|---|
| `D:\gamer\referensbilder - Modell\*.jpg/png` (47) | 38 Dreamstime-Stockvorschauen (Waffen, Rüstung, Fantasy-Figuren, Metall-/Schuppenkacheln, Keltenknoten), 7 Gemini-KI-Bilder, 1 Rüstungsübersicht, 1 weiteres | nur Anschauung |
| `design/referenz/figuren`, `gesichter` (7) | dieselben 7 Gemini-Bilder (Dateien identisch) | Stil- und Typenvorlage für Figuren |
| `…\3d mshes\*.zip/rar` (23) | Bäume, Weizen, Gras, Schaf, Hirsch, Lederrüstung, Ritterfigur, Axt, Bögen, Schwerter, Hut, Löwenkrieger | Technik-Anschauung (Poly-Zahlen, UV, Texturaufbau) |

Inventar und Prüfskript: `design/inbox/Referenzanalyse_2026-11-21/` (`reference_inventory.json`, `audit_summary.txt`).

## 2. Kritische Befunde (vor allem anderen lesen)

1. **Stockbilder sind keine Texturen.** Alle Dreamstime-Dateien tragen Wasserzeichen und sind urheberrechtlich geschützte Vorschauen. Weder Pixel noch Muster (Keltenknoten, Tapete, Schuppen) direkt übernehmen; nur Form und Bauweise beobachten.
2. **Stilbruch.** Gut die Hälfte der Stockbilder zeigt Hochmittelalter/Fantasy (Plattenrüstung, Visierhelme, verzierte Zweihänder, Elfen). Das Projekt ist Wikingerzeit („gelebter Norden“). Passend sind: Rundschild, Bartaxt, Brillenhelm (`old-helmet`), Lamellen-/Schuppenstudien, die Gemini-Trachten. Keltenknoten sind insular-keltisch; für nordische Ornamentik eher Borre-, Jelling-, Mammen-, Ringerike- oder Urnes-Stil als eigene Motive zeichnen.
3. **3D-Archive ohne Lizenz.** In keinem Archiv liegt eine Lizenzdatei (Skript hat danach gesucht). Die `open3dmodel.com`-Pakete haben keine nachgewiesene Herkunft → laut `CLAUDE.md` nicht ins Spiel. Ordner `lizenzbeachtet` heißt nur, dass der Nutzer die Lizenz beachtet hat; die Lizenztexte sind nicht beigelegt und müssten in `design/asset_log.md` mit Quelle stehen, bevor etwas davon eingebaut wird.
4. **Keine einzige Rig- oder Animationsvorlage.** Alle GLB-Dateien haben 0 Skins, 0 Gelenke, 0 Animationen, 0 Morph Targets. Für Rigging und Gesichtsanimation liefert der Ordner nichts; dafür gilt Abschnitt 5–6 (Recherche).
5. **Die Gemini-Bilder** sind laut Asset-Log bereits als „nicht im Spiel verwenden, nur Maßstab“ eingetragen. Das bleibt so.

## 3. Lehren aus den 3D-Archiven (was man richtig und falsch sieht)

| Beispiel | Messwert | Lehre |
|---|---|---|
| `3715` Baum | 1,2 Mio. Vertices, nur 12 UVs | Renderbaum für Standbilder, für Spiele unbrauchbar. Spielbaum: Stamm 2–8 k Tris + Blattkarten (Alpha-Ebenen), LODs, in UE Nanite-Foliage prüfen. |
| `107090` Baum | 400 k Vertices | dito; Faustregel: was nicht in einer Spielszene 100-fach stehen kann, ist kein Vegetations-Asset. |
| `5291` „trees lo-poly“ | 153 k Vertices für mehrere Bäume, 17 Materialien | Viele Materialslots = viele Draw Calls. Ziel: 1–2 Materialien je Baum (Rinde, Blattatlas). |
| `79822` Gras | 130 Vertices, 2 Texturen | Richtig: Gras als wenige gekreuzte Karten mit Alphatextur. |
| `107698` Weizen | 21 k Vertices, ein Atlas mit Halmen | Gutes Muster: alle Halme/Ähren in einen Atlas, Karten instanziieren. |
| `armored-warrior` | Base color / Metallic / Roughness / Mixed_AO / Normal_OpenGL / Opacity je Bauteil; Haare als **Haarkarten** (Strähnen-Atlas + Alpha) | Saubere Metal/Rough-Kette wie Substance Painter. **Achtung:** Normalmaps OpenGL → in Unreal Grünkanal umkehren (UE erwartet DirectX). Haarkarten statt Strands spart VRAM (8-GB-Ziel). |
| `78271` Lederrüstung | Diffuse + Normal + specularGlossiness | Alte Spec/Gloss-Kette. Umrechnen: Roughness = 1 − Glossiness, Metallic aus Material ableiten (Leder 0, Eisen 1), Spec-Farbe verwerfen. |
| `27992` Hirsch | stag_diff/stag_Spec/stag_hair.bmp | 12-MB-BMPs, Spec-Kette, Fell als Textur. Veraltet; als Format nie BMP liefern. |
| `bow.zip` | eine 8k-Textur, Inseln füllen ca. 15 % | Verschwendung: 8k für einen Bogen. Waffe in Hand: 1–2k, Inseln dicht packen, gleiche Texeldichte. |
| `recurve-bow` | 7 Texturen, Inseln winzig auf großer Fläche | Schlechte Texeldichte; besser ein Atlas mit gestapelten Spiegelhälften. |
| `medival-sword` | nur Kachelmaterialien (Metal03/07, Leder) | Kachelmaterial geht für Prototyp; für Nahansicht unikat backen (Kantenabrieb, Griffwicklung). |
| `plhat.glb` Hut | 25,8 k Vertices / ~39 k Tris | Für einen Hut viel zu dicht (Ziel 1–3 k Tris + Normalmap). Typisch für unbearbeitete KI-/Scan-Modelle. |
| `lion-warrior.glb` | 27 k Vertices, nur Basecolor, zerstückelter Atlas | Typisch KI-generiert: kein Normal/Rough, fragmentierte UVs → nicht riggbar ohne Retopo. |

**Daraus abgeleitete Budgets (Vorschlag, für 8 GB VRAM / 20 GB Spiel):**

| Asset | Tris LOD0 | Textur |
|---|---|---|
| Handwerkzeug, Waffe | 1,5–5 k | 1k–2k, ein Material |
| Schild, Helm | 2–6 k | 2k |
| Kleidungsteil (Figur) | 3–10 k | 2k, Texturen teilen |
| Bewohner komplett (nicht MetaHuman) | 15–35 k | 2–3 × 2k |
| Gras/Halm-Büschel | 50–300 | Atlas 1–2k |
| Baum | 3–15 k + Blattkarten, ≥3 LODs oder Nanite | Rinde 2k kachelnd, Blattatlas 2k |

Texeldichte (Vorschlag): Figuren/Ausrüstung ca. 10,24 px/cm (1k auf 1 m), Spielergegenstände in Ego-Nähe bis 20 px/cm.

## 4. Pipeline Blender → Unreal (Einstellungen, die Fehler vermeiden)

**Szene:** Einheit Meter, Unit Scale 1,0 (oder 0,01 – nur einheitlich bleiben). +Y ist in Blender vorn; Figur blickt −Y. Transformationen vor dem Export anwenden (Strg+A → All Transforms). Ursprung: Fußpunkt bei Figuren und stehenden Objekten, Griffpunkt bei Waffen (wichtig für Sockets).

**Statische Meshes (FBX):** Selected Objects; Apply Scalings „FBX All“; Forward −Y, Up Z; Smoothing „Face“; Apply Modifiers an; Tangent Space an. Kollision als eigenes Objekt `UCX_<MeshName>_00` (konvex, wenig Polys). LOD in UE generieren oder `_LOD1` usw.

**Skelettmeshes:** Armature + Mesh exportieren, *Add Leaf Bones aus*, *Only Deform Bones an*, Primary/Secondary Bone Axis Y/X. Erprobte Exporteinstellungen: `design/inbox/Testkopf_2026-11-21/export_testkopf.py` (Rückimport in Blender geprüft, Unreal noch nicht). Häufigster Fehler laut Foren: Skelett kommt in UE mit Skalierung 100 bzw. 0,01 an → Armature-Objektskala vor dem Export 1,0, angewendet; im Zweifel kleines Testmodell exportieren und in UE die Root-Bone-Skala prüfen. Animationen getrennt als „nur Armature, Bake Animation“ exportieren und auf das vorhandene Skeleton importieren.

**Projektspezifisch:** Das Projekt nutzt das UE-Standardskelett (Manny/Quinn) und MetaHumans mit Retargeting (Entscheidung 2026-11-15). Eigene Figuren daher **direkt auf dem Manny-Skelett aufbauen** (Manny als FBX nach Blender holen, Mesh darauf skinnen, nur das Mesh zurück) statt ein eigenes Rig zu erfinden – dann laufen alle Animationen (Kevin Iglesias, Quaternius UAL2) ohne Retargeting.

**Rigify:** nur wenn eigene Animationen in Blender entstehen sollen. Rigify ist nicht direkt spieltauglich (Hierarchie, Steuerknochen); vor Export auf die DEF-Knochen reduzieren (Add-ons wie Uefy/Rig2U5 tun das, Lizenz vorher prüfen). In UE dann IK Rig + IK Retargeter; Foren melden gute Ergebnisse mit Rotation „Interpolated“ und Translation „Absolute“ an den IK-Ketten.

**Texturen nach UE:** BaseColor sRGB; ORM (R AO, G Rough, B Metal) linear, Kompression „Masks“; Normal **DirectX** (bei OpenGL-Quellen „Flip Green Channel“). PNG/TGA, keine BMP/JPEG für Normalmaps.

**Mit dem Blender-MCP prüfen:** nach jedem Bau `look` (Ansicht vorn/seitlich/3/4), Polycount und Maße per Skript ausgeben; erst dann „fertig“ melden (Regel 2).

## 5. Gesichter

### 5.1 Was die Referenzbilder vorgeben
- Wettergegerbte, asymmetrische Gesichter; Altersspanne von jung bis sehr alt mit klaren Falten (Stirn, Krähenfüße, Nasolabial, Marionettenfalten).
- Narben (Augenbraue, Wange, Nasenrücken), Sommersprossen, Hautrötungen, Gesichtsbemalung/Tätowierung als Runenlinien (Stirn, Kinn).
- Bärte oft geflochten mit Perlen/Ringen, Haare in Zöpfen, rasierte Seiten. Breite Mischung an Hauttönen.
- Kleidung: Hängerock mit Ovalfibeln und Perlenschnüren, Tunika mit Brettchenborte, Wickelgamaschen, Wendeschuhe, Fell- und Karo-Umhänge, Gürteltaschen. Farbpalette gedeckt: Waidblau, Krapprot, Moosgrün, Ocker, Naturbraun.
- KI-typische Fehler in den Bildern: Fibeln und Borten nicht symmetrisch, Gürtelschnallen „schmelzen“, Hände ungenau. Für Details historische Funde statt dieser Bilder nehmen.

### 5.2 Weg im Projekt: MetaHuman
Die Spielerfiguren sind als MetaHuman entschieden. Ein selbst modellierter Kopf wird damit nicht 1:1 übernommen:
- **From Custom Mesh / Mesh to MetaHuman** (UE 5.6+ im Editor): beliebige Topologie, auch Scans und KI-Meshes. Laut Epic-Doku und Foren löst der Solver MetaHuman-Parameter, die die Vorlage annähern, und baut den Standardkopf daraus neu. Feine Merkmale (kleines Kinn, besondere Nase) gehen dabei teils verloren.
- **From Template Mesh:** genauer, verlangt aber exakt die MetaHuman-Vorlagentopologie.
- Folgerung: Charakterköpfe im MetaHuman Creator über dessen Regler formen (Referenzbilder 5.1 als Zielbilder), Narben/Bemalung als Hautdetail-Textur oder Decal, Bärte/Zöpfe über MetaHuman-Groom oder eigene Haarkarten. Frontale Porträts **nicht** auf Köpfe projizieren.

### 5.3 Wenn ein Kopf selbst in Blender entsteht (Bewohner, Gegner, Tiere)
**Topologie (Kantenschleifen):**
- Geschlossene Ringe um Augen (5–7 Schleifen inkl. Lidkante, Lidfalte, Augenhöhle) und um den Mund (6–8 Ringe bis über die Lippenkante hinaus) – das sind die beweglichen „Muskelringe“.
- Eine Schleife von Nasenflügel über die Nasolabialfalte bis unter das Kinn, damit die Falte beim Lächeln knickt statt zu dehnen.
- Pole (5er-/3er-Knoten) in ruhige Zonen legen: Schläfe, Wange seitlich, nicht in Mundwinkel oder Lidwinkel.
- Ober- und Unterlippe getrennt (sonst kann der Mund nicht öffnen), innen Mundsack modellieren; Augäpfel, Zähne, Zunge als eigene Objekte.
- Reine Quads, gleichmäßige Dichte; Spielkopf ca. 4–8 k Tris für Bewohner, Details per Normalmap aus einem Sculpt backen.

**Gesichtsausdrücke:** Industriestandard sind die **52 ARKit-Blendshapes** (Namen wie `eyeBlinkLeft`, `jawOpen`, `mouthSmileLeft` …). Vorteil: kompatibel mit Live Link Face, Audio-zu-Gesicht-Werkzeugen und MetaHuman-Animator-Ausgaben. In Blender als Shape Keys mit exakt diesen Namen anlegen; Hilfs-Add-ons (FaceKeys, ARKit Blendshape Tools, ARKitBlendshapeHelper) erzeugen Rohformen, die danach von Hand nachgearbeitet werden müssen. Lizenz jedes Add-ons vor Nutzung prüfen. Export: FBX mit Shape Keys → in UE Morph Targets.

**Prüfliste Ausdruck:** Blinzeln schließt das Lid vollständig, ohne durch den Augapfel zu stoßen · `jawOpen` ohne Lippenriss · Lächeln hebt Wange und schiebt das Unterlid hoch · Asymmetrie links/rechts getrennt · Kombination `jawOpen`+`mouthSmile` bleibt sauber.

### 5.4 Praxistest „Testkopf“ (2026-11-21)

Skripte, FBX und `.blend` liegen in `design/inbox/Testkopf_2026-11-21/` (reproduzierbar: `build_testkopf.py` → `shape_keys_testkopf.py` → `export_testkopf.py`). Nur Pipeline-Test, kein Spiel-Asset (Ei-Form, keine Ohren, keine Haare, keine UVs/Texturen).

**Geprüft und erfüllt:** 2.776 Quads ≈ 5.550 Dreiecke (Budget 4–8 k); 3 Kantenringe um Augen und Mund; Ober- und Unterlippe getrennt, Mundsack geschlossen; keine Kanten mit >2 Flächen, keine losen Kanten; Lidöffnung 8,5 × 23,4 mm. Shape-Keys `eyeBlinkLeft/Right`, `jawOpen`, `mouthSmileLeft/Right`: Blinzeln verdeckt das richtige Auge vollständig (Strahltest 0/20 statt 14/20 sichtbar), das andere bleibt offen; Kiefer öffnet ca. 18 mm. FBX-Rückimport in Blender: Maße und Skala 1,0 unverändert, Shape-Keys verformen identisch. **Nicht geprüft:** Import in Unreal (Editor nicht gestartet).

**Lehren (gelten für jedes künftige Modell):**
1. **Ringstärke × Anzahl < halbe Lochhöhe.** Sonst kippt die Innenfläche beim Einsetzen, und Ringe falten sich über die Öffnung. Das Auge war deshalb zuerst zu, obwohl die Randmessung „offen“ ergab. Prüfen mit einem **Strahltest von vorn** statt nur mit Randmaßen.
2. **Öffnungen nicht nachträglich stark aufweiten.** Wenn der Rand weiter wandert, als der Ringabstand beträgt, kreuzt er den ersten Ring. Lieber die Ausgangsfläche größer anlegen.
3. **Lidkante zur Augapfelmitte einziehen, nicht stur nach +Y.** Am äußeren Augenwinkel zeigt die Fläche zur Seite, ein +Y-Einzug verdreht sie. Den Augapfel danach hinter jeden Randpunkt setzen (Abstand ≥ Radius + 0,7 mm).
4. **`bmesh.ops.extrude_face_region` lässt die Ausgangsflächen stehen.** Sie müssen selbst gelöscht werden, sonst entstehen innere Flächen und dunkle Schattierungsflecken.
5. **`mesh.materials.clear()` setzt die Materialindizes zurück.** Indizes erst nach dem Anlegen der Slots setzen.
6. **`grid_fill` liefert bei flachen Schlitzen still nichts.** Den Mundsack durch Verschweißen schließen (erzeugt 4 Dreiecke an den Mundwinkeln, vertretbar).
7. **Mess- statt Schätzwerte für Merkmale:** Die Mundlinie lag 6 mm höher als angenommen, Lippenwulst und Kiefer-Abklingen saßen falsch. Positionen aus dem Modell messen und weitergeben.
8. **Nach dem Setzen von Positionen `view_layer.update()`**, sonst ist `matrix_world` noch alt (Shape-Key-Skript fand die Augen bei 0,0,0).
9. **Morph Targets brauchen in Unreal ein Skelett-Mesh.** Ein Kopf mit Shape-Keys bekommt mindestens root → neck → head; die Augäpfel hängen per Gewicht am Knochen head. Knochenlängen kommen beim FBX-Rückimport anders an (FBX speichert keine Länge), das ist normal.
10. **Seiten:** ARKit „Left“ ist die linke Seite der Figur, bei Blick nach −Y also **+X**. Mit Strahltest pro Seite bestätigt.

## 6. Animation

- **Vorhandene Bibliotheken zuerst:** Kevin Iglesias, Quaternius UAL2 (CC0) – passen aufs Manny-Skelett. Eigene Animationen nur für Lücken (Hinhocken beim Aufsammeln, Herstellen der Anfangswerkzeuge, siehe Entscheidungen 2026-11-15).
- **Regeln für eigene Animationen:** 30 fps; In-Place für Lauf-Zyklen (Bewegung kommt aus dem Code), Root Motion nur für Aktionen mit festem Weg (Ausweichrolle, Schlag mit Ausfallschritt); Zyklen mit identischem erstem und letztem Frame; Kontaktposen zuerst blocken (Key Poses → Breakdowns → Spline), dann Überlappung (Zöpfe, Umhang besser als Physik/Cloth in UE).
- **Arbeitsaktionen:** Schlagbewegung mit klarer Ausholpose und einem Treffer-Frame; diesen Frame in UE als Anim Notify markieren (Server wertet den Treffer aus, Regel 1).
- **Gesichtsanimation/Lippen:** entschieden ist MetaHuman aus Audio. Für Nicht-MetaHuman-Köpfe mit ARKit-Shapes funktionieren dieselben Kurvennamen.

## 7. Offene Fragen an den Nutzer
1. Sollen Bewohner ebenfalls MetaHumans werden oder eigene, leichtere Köpfe nach 5.3? (Budget 8 GB VRAM spricht für eigene.)
2. Lizenztexte der Archive im Ordner `lizenzbeachtet` nachreichen, falls etwas davon ins Spiel soll.
3. Ornamentik: nordische Stile (Urnes, Mammen …) statt Keltenknoten – einverstanden?

## Quellen (Recherche 2026-11-21)
- Epic: [MetaHuman from mesh](https://dev.epicgames.com/documentation/en-us/metahuman/from-mesh), [from template mesh](https://dev.epicgames.com/documentation/en-us/metahuman/from-template-mesh), [From Custom Mesh tool](https://dev.epicgames.com/documentation/metahuman/metahuman-creator-from-custom-mesh-tool-in-unreal-engine)
- Forum: [Mesh to MetaHuman verändert das Gesicht](https://forums.unrealengine.com/t/metahuman-creator-changes-my-custom-face-mesh-after-mesh-to-metahuman-is-this-normal/2844122), [Root bone scaled x100](https://forums.unrealengine.com/t/root-bone-is-scaled-by-x100/484622), [Blender→UE Skalierung](https://forums.unrealengine.com/t/blender-to-unreal-fbx-always-scales-wrong-with-skeletal-mesh-imports/2657998)
- [Blender to UE5: complete export pipeline](https://sarahhyperdense.substack.com/p/blender-to-ue5-the-complete-export)
- Apple: [ARKit BlendShapeLocation](https://developer.apple.com/documentation/arkit/arfaceanchor/blendshapelocation)
- Add-ons (nur Hinweis, Lizenz ungeprüft): [FaceKeys](https://superhivemarket.com/products/facekeys/docs), [ARKit Blendshape Tools](https://extensions.blender.org/add-ons/arkit-blendshape-tools/), [Rig2U5](https://superhivemarket.com/products/rig2u5), [Uefy](https://forums.unrealengine.com/t/uefy-2-0-pro-edition-make-blenders-rigify-rigs-compatible-with-ue5-also-has-a-free-version/120782)
- [Blender Studio: Stylized Character Workflow (Gesichtsschleifen)](https://studio.blender.org/training/stylized-character-workflow/5e5408388faf011a381510de)
