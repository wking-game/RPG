# Vegetations-Material und Jahreszeiten

Stand 2026-11-14. Gilt für das gemeinsame Blatt-/Pflanzenmaterial `/Game/Gamer/Materials/M_Gamer_Foliage` (Laub, Nadeln, Gras, Kräuter).
Gebaut wird es per Python in `tools/unreal/import_assets.py` (Aufruf: `tools/import_assets.ps1`). Änderungen im Editor gehen beim nächsten Import verloren.

## Ursache der „verbrannten“ grauen Grasbüschel (2026-11-13)

Nicht die Normalen oder der Glanz: Das Client-Log meldete
`Material M_Nature_Grass missing usage flag InstancedStaticMeshes! Default Material will be used in game.`
PCG setzt das Gras als Instanzen (ISM). Ohne das Nutzungs-Flag „Used with Instanced Static Meshes“ zeigt das Spiel (`-game`, ohne Editor)
das graue Standard-Gittermaterial. Das betraf auch die PCG-Kiesel und -Zweige (`M_Gamer_Base`). Beide Eltern-Materialien setzen das Flag jetzt.

## Shading

| Einstellung | Wert | Grund |
|---|---|---|
| Shading Model | Two Sided Foliage | Licht scheint durch Halme und Blätter |
| Blend Mode | Masked, zweiseitig | Alpha der BaseColor-Textur |
| Subsurface Color | Jahreszeiten-Farbe × 0,4 | gedämpftes Durchlicht |
| Specular | 0,25 | weniger weißlicher Himmelsglanz |
| Roughness | 0,6 + 0,35 × ORM.G (0,6–0,95) | nie glänzend |
| Normal | Normal-Map × TwoSidedSign | Rückseiten behalten die Normale der Vorderseite. Die Engine dreht sie sonst um, und weil Gras und Baumkronen nach oben bzw. außen gebogene Normalen haben, würden Rückseiten dunkel. |
| Ambient Occlusion | ORM.R | wie bisher |

## Jahreszeit: `MPC_GamerWorld.SeasonPhase`

Material Parameter Collection `/Game/Gamer/Materials/MPC_GamerWorld`, Skalar **`SeasonPhase`** (Standard 1,3 = Sommer). Das Spiel (C++, `AGamerGameState`) setzt den Wert zur Laufzeit.
Name und Pfad nicht ändern. Der Import legt den Parameter nur an, wenn er fehlt (Materialien verweisen über seine ID darauf), und setzt den Standardwert jedes Mal auf 1,3.

| Phase | Zeitpunkt (Nordeuropa) |
|---|---|
| 0 | Frühlingsanfang (~20. März) |
| 1 | Sommeranfang (~21. Juni) |
| 2 | Herbstanfang (~22. September) |
| 3 | Winteranfang (~21. Dezember) |
| 4 | = 0, zyklisch |

Testen im Spiel: Testschritt `season:<phase>`, z. B. `-GamerTestScript=season:2.5;wait:3;shot`.

## Parameter je Material-Instanz

| Parameter | Standard | Bedeutung |
|---|---|---|
| `Evergreen` | 0 | 1 = immergrün (Nadeln): im Winter nur dunkler, olivgrüner |
| `LeafDrop` | 0 | 1 = Laub fällt ab: Herbstfarben, im Spätherbst ausblenden, im Frühling austreiben. Hat Vorrang vor `Evergreen`. |
| beide 0 | | Gras, Kräuter: Farbwechsel ohne Ausblenden |

Werte: 0 oder 1 (Schwelle 0,5). `import_assets.py` setzt sie nach Namensregeln (Tabelle `SEASON_RULES`, erster Treffer gilt, Groß-/Kleinschreibung egal).
Die Regeln stehen bewusst nicht im Manifest, weil die Blender-Skripte ihre Manifest-Einträge neu schreiben.

| Name enthält | Verhalten | Beispiele |
|---|---|---|
| `Fern` | deciduous (`LeafDrop` 1) | M_Nature_Fern02 (Farn fern_02) |
| `Shrub` | grass | M_Nature_LeavesShrub02, …Shrub04 (Sträucher; Regel steht vor `Leaves`) |
| `Needles` | evergreen (`Evergreen` 1) | M_Nature_NeedlesPine |
| `Leaves` | deciduous (`LeafDrop` 1) | M_Nature_LeavesBirch, …Apple, …Hazel, …Raspberry |
| sonst (maskiert) | grass | M_Nature_Grass, M_Nature_Nettle |

Nur maskierte Materialien (Eltern `M_Gamer_Foliage`) haben Jahreszeiten. Rinde, Früchte (`M_Nature_Fruit`), Stümpfe, Felsen, Totholz bleiben unverändert.

## Farbgebung

Die Textur wird nicht ersetzt, sondern umgefärbt: Die Helligkeit jedes Texels bleibt (Details bleiben), der Farbton wird mit dem Anteil *A* zur Jahreszeitenfarbe gezogen und mit dem Faktor *B* aufgehellt oder abgedunkelt.
Zwischen den Stützstellen wird weich interpoliert (smoothstep), Phase 4 schließt nahtlos an 0 an.

### Farben (sRGB, im Code linear)

| Name | sRGB | Verwendung |
|---|---|---|
| BUD | 150 185 60 | Laubaustrieb, hell gelbgrün |
| FRESH | 140 185 45 | frisches Frühlingsgrün |
| SUMMER | 70 130 35 | sattes Sommergrün |
| OLIVE | 125 135 55 | Spätsommer, leicht trocken |
| DRY | 150 140 70 | trockene Flecken |
| LUSH | 52 112 40 | feuchte, sattgrüne Flecken |
| AUTGRASS | 160 130 60 | Herbstgras, gelbbraun |
| STRAW | 165 145 105 | Wintergras, strohig |
| YELLOW / ORANGE / RUST | 215 170 40 / 205 115 30 / 150 60 30 | Herbstlaub (je Objekt gemischt, etwa 45 % gelb, 30 % orange, 25 % rotbraun) |
| DEADLEAF | 140 90 45 | Laub kurz vor dem Fallen |
| NEEDLEWINTER | 75 82 42 | Nadeln im Winter (oliv) |

### Stützstellen (Phase: Farbe, Anteil *A*, Helligkeit *B*)

| Typ | Frühling | Sommer | Herbst | Winter |
|---|---|---|---|---|
| Gras/Kräuter | 0,0 STRAW .78/.95 → 0,4 FRESH .60/1.18 → 0,85 FRESH .35/1.08 | 1,15 SUMMER .35/.92 → 1,95 OLIVE .45/.97 | 2,5 AUTGRASS .72/1.03 | 3,1 STRAW .82/1.0 → 3,9 STRAW .78/.95 |
| Laub (`LeafDrop`) | kahl bis 0,30, Austrieb 0,30–0,62; 0,45 BUD .65/1.18 → 0,85 FRESH .35/1.08 | 1,15 SUMMER .30/.92 → 1,75 SUMMER .28/.90 → 2,0 OLIVE .30/.92 | 2,35 Herbstfarbe .90/1.0–1.35; Laubfall 2,36–2,86, dabei → DEADLEAF .92/.85 | kahl ab ~2,86 |
| Nadeln (`Evergreen`) | 0,5 SUMMER .10/.95 → 0,85 FRESH .22/1.05 (Maitriebe) | 1,3 SUMMER .12/.97 | 2,7 OLIVE .25/.88 | 3,1 NEEDLEWINTER .50/.75 |

Laubfall und Austrieb laufen über die Opacity-Mask: ein 3D-Rauschen (35 cm und 13 cm) entscheidet, welche Blattbereiche zuerst verschwinden; die Krone lichtet sich fleckig statt auf einmal.
Blattbereiche, die früher fallen, verfärben sich auch früher.

## Variation gegen Eintönigkeit

| Quelle | Wirkung |
|---|---|
| Zufall je Objekt: `PerInstanceRandom` (PCG-Instanzen) + Hash der Objektposition (Einzel-Actors wie Bäume, dort ist PerInstanceRandom 0) | Helligkeit ±11 %, Farbton wärmer/kühler (±6–10 % in Rot/Blau), Zeitversatz der Jahreszeit (Gras ±0,05, Nadeln ±0,06, Laub ±0,09 Phasen), Wahl der Herbstfarbe |
| Welt-Rauschen (Wertrauschen 28 m + 11 m) | trockene und satte Flecken von etwa 15–40 m; trockene Flecken sind gelblicher und schneller im Herbst, feuchte im Sommer satter und dunkler |

## Bekannte Grenzen

- Früchte (`M_Nature_Fruit`) bleiben im Winter sichtbar, auch wenn der Strauch kahl ist. Ob Beeren/Nüsse im Winter erntbar sein sollen, ist eine Spiel-Entscheidung.
- Kein Schnee. Winter = strohiges Gras, kahles Laub, dunklere Nadeln.
- Kosten: ein Custom-Node (HLSL) mit Hash-Rauschen; das 3D-Rauschen läuft nur für `LeafDrop`-Materialien (Verzweigung auf dem Parameter).
