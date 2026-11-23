# Küstenasche – Architektur der modularen Ausrüstung (8 Slots)

**Status:** 24 Generator-Einstiege für je T1–T3 in allen acht Slots; weitere Tierformen sind Ausbauumfang  
**Einheiten:** Blender Metric, 1 BU = 1 m, Z-Up, Figurenfront = −Y  
**Ausrichtung:** Projektbasis `KA_1.75m_A_Pose_ZUp_front_-Y`  
**Stilreferenz:** Die im Chat gesendeten Bilder dienen als Look-Referenz: dunkle, beanspruchte Platten, matte Messingkanten, zurückhaltende Reliefs und eine realistische, wenig glänzende Materialwirkung. Die Bilder enthalten keine zusätzlichen Produktionsanweisungen.

## 1. Ordner- und Collection-Struktur

```text
design/inbox/Blender_Kuestenasche/
├── 09_armor_variant_architecture.md
└── scripts/
    ├── variant_common.py
    ├── HEAD_T1_LeatherCap.py
    ├── HEAD_T2_Spangenhelm.py
    ├── HEAD_T3_PotHelm.py
    ├── TORSO_T1_WoolTunic.py
    ├── TORSO_T2_Gambeson.py
    ├── TORSO_T3_CuirBouilli.py
    ├── slot_variants_03_08.py
    ├── slot_variant_entry.py
    ├── SHOULDERS_T*_*.py / ARMS_T*_*.py / HANDS_T*_*.py
    ├── LEGS_T*_*.py / SURCOAT_T*_*.py / CLOAK_T*_*.py
    └── variant_common.py
```

Jedes Einstiegsskript baut genau ein Asset. Das Hilfsmodul enthält die Quad-Geometrie-, Material-, UV- und Rig-Funktionen. Ein Lauf ersetzt nur seine eigene Generator-Collection, sodass vorhandene Szenenobjekte und die anderen Varianten erhalten bleiben.

```text
KA_Armor_Variants
├── slot_head
│   ├── SK_Armor_Head_LeatherCap_T1
│   │   └── RIG_SK_Armor_Head_LeatherCap_T1 / ARM_SK_Armor_Head_LeatherCap_T1
│   ├── SK_Armor_Head_Spangenhelm_T2
│   └── SK_Armor_Head_PotHelm_T3
├── slot_torso
    ├── SK_Armor_Torso_WoolTunic_T1
    ├── SK_Armor_Torso_PaddedGambeson_T2
    └── SK_Armor_Torso_CuirBouilli_T3
├── slot_shoulders / slot_arms / slot_hands
└── slot_legs / slot_surcoat / slot_cloak
```

Jedes Asset wird als ein benanntes Mesh mit mehreren Material-Slots erstellt. Nähte, Nieten, Platten, Einlagen und Kanten sind echte Mesh-Inseln innerhalb dieses Mesh-Objekts; sie werden nicht nur aufgemalt. Ein benanntes Rig-Untercollection enthält das vollständige A-Pose-Skelett für den Skeletal-Mesh-Export.

## 2. 8-Slot- und 15-Varianten-Plan

Stabile Slot-IDs für das modulare System:

| Slot-ID | Inhalt |
|---|---|
| `slot_head` | Hauben, Kappen und Helme |
| `slot_torso` | Tuniken, wattierte Lagen und Rumpfpanzer |
| `slot_shoulders` | Schulterplatten und Schulterkragen |
| `slot_arms` | Oberarm- und Unterarmschutz |
| `slot_hands` | Handschuhe, Wickel und Handplatten |
| `slot_legs` | Beinwickel, Beinlinge, Beinschienen und Tassets |
| `slot_surcoat` | Waffenrock, Tabard und geschlitzte Brigandinen-Schürze |
| `slot_cloak` | Umhang, Fell und rückwärtige Stofflage |

Vorschlag für 15 eigenständige Silhouetten je Slot: 3 Varianten für T1, 3 für T2, 3 für T3, 2 für T4, 2 für T5 und 2 für T6. Jede Variante erhält eine eigene, klar unterscheidbare Grundform und physische Details; Tier-Materialien und Verzierungen ergänzen diese Form. Die sechs hier erzeugten Teile belegen je Slot die erste Form der Stufen T1, T2 und T3. Die übrigen neun Formen pro Slot sind nicht Bestandteil dieses Pakets.

| Variante | Tier | Form und Materiallesbarkeit |
|---|---:|---|
| `HEAD_T1` | 1 | Eng sitzende Wollkappe mit gefettetem Lederabschluss, Sehnennähten und Ohr-/Wangenlaschen |
| `HEAD_T2` | 2 | Dunkler Raseneisen-Spangenhelm mit vier Rippen, Naseneisen, Wangenklappen und Messingnieten |
| `HEAD_T3` | 3 | Geschlossener Topfhelm mit ausgeschnittener Sehöffnung, dunkler Einlage, Kinnschutz und Verstärkungsrippen |
| `TORSO_T1` | 1 | Grobe Wolltunika mit Leinen-Schulterzwickeln, sichtbaren Nähten und angepasstem Gürtel |
| `TORSO_T2` | 2 | Dick wattierter Gambeson mit physisch erhabenen Steppkanälen, Rollkragen und Schulterkappen |
| `TORSO_T3` | 3 | Gehärteter Lederharnisch mit überlappenden Brustfeldern, hohem Kragen, Nieten und kleinem offenem-Ring-Zeichen |

Die Reihenfolge folgt dem aktuellen Auftrag. Kettenhemd, Brigandine und Basaltplatte mit Seeglas sind spätere eigenständige Torsoformen, nicht bloße Texturvarianten dieser drei Stücke.

## 2a. Slots 3–8: Bauprinzipien und Rig-Verträge

Die nachfolgenden sechs Beschreibungen erweitern die Slotarchitektur. Pro Slot gelten weiterhin 15 eigenständige Formen gemäß Tierverteilung oben. Für Slots 3–8 liegen nun je drei Generatoren für T1, T2 und T3 vor. Die zwölf weiteren Formen je Slot bleiben Ausbauumfang.

| Slot | Formfamilie und sichtbare Staffelung | Mesh-/Rig-Regel |
|---|---|---|
| `slot_shoulders` | 15 Formen von Wolf-/Bärenfell, Knotenschlaufen und asymmetrischen Eisenauflagen bis zu drachengeprägten, flügelartig aufgestellten Plattenschultern mit mattem Messing und einzelnen Seeglasfugen. | Schalenkrümmung folgt der Schlüsselbeinlinie vom Halsansatz `(0,0,1.455)` über Schulter `(±0.185,0,1.445)` zum Schultervolumen `(±0.195,0,1.430)`. Vertex-Gruppen `VG_Clavicle_L/R`, `VG_UpperArm_L/R` für Gewichte; `VG_Collision_Shoulder_L/R` markieren getrennt die Kollisionsbereiche. Kollisionsgruppen sind Masken: pro UE-Physics-Asset wird zusätzlich eine passende Collision-Hülle benötigt. |
| `slot_arms` | 15 Formen von Leder-Bracern mit gekreuzter Sehne und Schnallen bis zu genieteten Stahlmanschetten, Ellenbogenkapseln und gravierten Gezeitenhall-Platten. | Oberarm- und Unterarmschalen folgen den Segmenten Schulter→Ellenbogen `(±0.385,0,1.285)`→Handgelenk `(±0.535,−0.005,1.135)`. „Zylinderbasiert“ bedeutet hier ein anatomisch verjüngter, an der Innenseite abgeflachter Ring-Loft mit nichtuniformen Querschnitten, keinen Grundkörper-Zylinder. Mindestens drei enge Support-Loops um das Ellenbogengelenk; Oberarm und Unterarm bleiben getrennte Mesh-Sections oder getrennte Variant-Objekte. |
| `slot_hands` | 15 Formen von gefütterten Wollfäustlingen und Lederhandschuhen bis zu Mail-Mittens und stahlbeschichteten Gauntlets. | Handfläche und Daumen, Zeigefinger sowie die übrigen Finger als gemeinsamer Block sind getrennte, über Vertex-Gruppen adressierbare Segmente. Die rechte Handschuhgeometrie sitzt in der Restpose auf `hand_r` bei `(−0.535,−0.005,1.135)`; das Paar-Mesh bleibt am Charakterrig gebunden und nutzt den Becken-Root-Pivot. Das vorhandene 21-Bone-Rig hat keine Fingerknochen, daher sind die Fingersegmente modular, aber noch nicht einzeln animierbar. |
| `slot_legs` | 15 Formen von Wollwickeln und gefütterten Beinlingen über Leder-Tassets an Ketten bis zu geschmiedeten Beinschienen und Basaltplatten. | Oberschenkel/Knie/Wade folgen `thigh`, `calf`, `foot`; Kniezentrum ist `(±0.098,0,0.535)`. Unterhalb und oberhalb der Kniescheibe liegen Support-Loops, das Knie selbst erhält eine freie Kompressionszone. Tassets bleiben physisch überlappend, mit getrennten Gewichten für `pelvis`/`thigh_l/r`; Bewegungs- und Hocktests bestimmen die finalen Abstände. |
| `slot_surcoat` | 15 Formen von ausgewaschenem Leinen und Sippen-Tabards bis zu schweren, geschlitzten Waffenröcken mit gewebtem oder aufgedrucktem offenem Ring und drei Kerben. | Doppelseitige Cloth-Flächen erhalten vertikale Falten-Loops. Vorder- und Rückenschlitze enden in eigenständigen Stoffbahnen, nicht in Textur-Schnittlinien. Eine Pin-Gewichtsgruppe sitzt an Gürtel/Schulteransatz; Saum und freie Bahnen bekommen graduell zunehmende Simulationsgewichte. Das UE-Chaos-Cloth-Setup und Naht-/Saumkollisionen werden nach dem Mesh-Import konfiguriert. |
| `slot_cloak` | 15 Formen von ungeschorenem Schaffell und kurzen Wolfs-/Bärenfell-Überwürfen bis zum langen Reliktumhang mit matter Runenbordüre. Köpfe und Pranken sind optionale physische Silhouettendetails einzelner Varianten. | Nacken-/Schulteransatz ist die fixierte Zone, dort liegen zusätzliche Quads für Übergang und Gewichtsgradient. Freie Fläche wird nach unten gröber; Rand und Fellbüschel erhalten kontrollierte Zusatzdichte. Pin-Gruppe am Kragen, simulierte Gewichte zum Saum hin; Wind, Selbstkollision und Fell-/Rüstungskollision folgen später in UE5. |

Alle Schulter-, Arm-, Hand- und Beinpanzer bleiben in derselben A-Pose-Ausrichtung wie das bestehende Rig. Das Rig ist asymmetrisch als `l = +X` und `r = −X` angelegt. Für Kollisionshüllen gilt: Vertex-Gruppen kennzeichnen Bereiche, ersetzen aber keine UE-PhysicsAsset-Körper oder passende Kollisions-Meshes. Für Cloth sind Pin-/Weight-Gruppen eine Eingabe für das UE-Chaos-Setup, noch keine fertige Laufzeit-Simulation.

| Variante | Tier | Form |
|---|---:|---|
| `SHOULDERS_T1/T2/T3` | 1–3 | Wolfsfell mit Sehnenknoten; asymmetrische Eisenpauldrons; flügelgelippte Drake-Schultern mit Messing und Seeglas |
| `ARMS_T1/T2/T3` | 1–3 | gekreuzte Leder-Bracers; genietete Eisenarmschienen mit Ellenbogenkappen; gravierte Gezeitenhall-Armplatten |
| `HANDS_T1/T2/T3` | 1–3 | Wollfäustlinge; Leder-Kampfhandschuhe mit Knöchelplatten; Mail-Mittens mit Stahlsegmenten |
| `LEGS_T1/T2/T3` | 1–3 | Wollwickel; Ketten-Tassets über Beinlagen; Stahlbeinschienen mit Kniekappen |
| `SURCOAT_T1/T2/T3` | 1–3 | Leinenüberwurf; Sippen-Tabard; geschlitzter Brigandinen-Waffenrock |
| `CLOAK_T1/T2/T3` | 1–3 | Schaffell; mittellanger Wolfsfellumhang; mittellanger Gezeitenhall-Mantel. Bodentiefes Artefaktdesign bleibt für die oberen Stufen. |

Die Generatoren ergänzen für Schulterstücke `VG_Weight_Shoulder_L/R`, `VG_Collision_Shoulder_L/R` sowie die Rig-Knochengewichte `clavicle_l/r` und `upperarm_l/r`; für Arme/Knie `VG_Elbow_Flex_L/R` und `VG_Knee_Compression_L/R`; für Handschuhe modulare Gruppen `VG_Hand_Thumb_*`, `VG_Hand_Index_*` und `VG_Hand_FingersBlock_*`. Surkot und Umhang erhalten `VG_Cloth_Pin`/`VG_Cloth_Free`. Diese Gruppen müssen vor Spielintegration mit Animation, Physics Asset und UE-Chaos-Simulation geprüft werden.

## 3. Anatomische Maße und Pivot-Mathematik

Alle Profile sind in Metern und als Querschnittsfolgen `z, rx, ry` festgelegt. `rx` und `ry` sind die halbe Breite beziehungsweise Tiefe eines Querschnitts. Zwischen den Stützringen wird die Form abschnittsweise linear interpoliert. Torso-Hauptschalen verwenden 64–72 Umfangssegmente; Arm-/Beinlofts 20–32; kleine Finger-, Rivet- und Kettendetails 4–16. So folgt die Dichte dem Formmaßstab, während zusätzliche Längsringe Taille, Gelenke und Kragen kontrollieren.

Die Querschnittskoordinaten lauten:

```text
x(θ,z) = rx(z) · cos(θ)
y(θ,z) = ry(z) · sin(θ) − 0,003 · cos²(θ)
```

Damit liegt die Figurenfront bei `y < 0`. Rumpfprofile folgen dem gemeinsamen A-Pose-Körper zwischen Becken `z=0,88 m`, Taille ungefähr `z=1,00 m`, Brust `z=1,24–1,40 m`, Schulteransatz `z=1,445 m` und Halsbasis `z=1,47 m`. Typische halbe Brustbreite liegt bei `0,16–0,19 m`, die halbe Tiefe bei `0,09–0,11 m`. Die Torsoschale lässt das Halsloch am oberen Profil offen und schneidet seitliche Armöffnungen zwischen `z=1,235–1,445 m` um `θ=0` und `θ=π` aus. Innenhaut und quad-basierte Randbrücken geben den offenen Konturen etwa 3–5 mm Materialstärke.

Der Kopfslot verwendet die Halsbasis als Mesh-Pivot. Profile liegen relativ zur Halsbasis und reichen je nach Helm etwa `0,31–0,34 m` nach oben; der Helm sitzt über der Kopfhaut der 1,75-m-Basis und erhält etwas Bewegungs-/Haarfreiheit. Kopfstücke sind vollständig dem Knochen `head` gewichtet, damit sie bei Animationen nicht über Augen, Kiefer oder Sehschlitz verformen.

Der Torsoslot verwendet den Becken-Pivot `z=0,88 m`. Im Blender-Viewport bleibt das Stück auf Körperhöhe positioniert; die lokalen Meshkoordinaten werden um diese Höhe verschoben und der Objektursprung sitzt an der Beckenbasis. Torsos werden mit maximal drei Einflüssen aus `pelvis`, `spine_01..03`, `neck_01` und `clavicle_l/r` gewichtet. Die geöffneten Armlochbereiche halten den Oberarm-Schwenk frei.

## 4. Geometrie- und Exportkonventionen

- Hauptschalen entstehen aus anatomisch kuratierten Ringprofilen mit Quad-Loops. Die Helfer verwenden keine `bpy.ops.mesh.primitive_*`-Grundkörper.
- Details sind quadbasierte Kurvenzüge, gerundete Rivet-Domes oder geschlossene, gekrümmte Grid-Panels. Sehschlitze werden aus der Helmschale ausgespart und mit einer zurückgesetzten Schattenfläche hinterlegt.
- Jede generierte Fläche hat vier Ecken. Die Schalen besitzen Innenflächen und quadbasierte Randbrücken; bei separat überlappten Reliefs bleiben die Mesh-Inseln getrennt, damit Materialien und Silhouette sauber kontrollierbar sind.
- Smart UV wird als `UV0` auf das fertige Mesh gelegt. Die prozeduralen Blender-Nodes liefern Grundfarbe, Metallic, Roughness-Variation und Bump. Für ein endgültiges UE5-Paket müssen diese Node-Materialien gebacken oder durch UE-Materialien mit passenden Texturen ersetzt werden.
- Textur-Bake-Ziele: gemeinsame 2K-Kleidungsfamilie `BC/N/ORM`, Metall 1K bis 2K; Texturen nicht pro Einzelteil duplizieren. Blender-Noise wird nicht als importierbare UE-Prozedur behauptet.
- Export als Skeletal Mesh: jeweils Mesh und sein `ARM_*`-Objekt gemeinsam als FBX exportieren, `-Y Forward`, `Z Up`, Apply Unit, keine zusätzliche 90°-Rotation. Die identischen Bone-Namen und Restpose-Positionen erlauben später die gemeinsame UE-Skeleton-Zuordnung. Nur die Generator-Collections für den Export auswählen.
- Für Laufzeit-Budgets sind diese Meshes ein hochaufgelöster Form-/Look-Entwurf. LOD0, LOD1/2, Kollisionshüllen, Cage-Bakes, Texturbakes und Engine-Import sind noch nicht erzeugt.

## 5. Materialwerte

Die PBR-Startwerte liegen bewusst auf matter Seite. Node-Rauschen speist prozedurale Metallic- und Roughness-Rampen sowie die Grundfarbe; die Bump-Stärke ist für mikroskopische Schmiede-, Faser- oder Narbenstruktur gedacht und erzeugt keine großen Beulen.

| Materialfamilie | Metallic | Basis-Roughness | Farbcharakter |
|---|---:|---:|---|
| Wolle/Leinen | 0,00 | 0,86–0,91 | Salz-Aschgrau und Naturleinen |
| Gefettetes/Hartleder | 0,01–0,06 | 0,62–0,78 | dunkles Aschebraun, gebrochene Kanten |
| Raseneisen/Schmiedeeisen | 0,78–0,86 | 0,54–0,69 | angelaufenes Basaltschwarz, kein Chromglanz |
| Patiniertes Messing | 0,66–0,70 | 0,71–0,75 | stumpfes Altgold, nur an Nieten und Linien |
| Visier-Schatten | 0,25 | 0,88 | dunkler Innenraum, kein emissives Leuchten |

## 6. Laufreihenfolge

In Blender 5.x eines der sechs Einstiegsskripte aus `scripts/` ausführen. Das gemeinsame Modul `variant_common.py` und `kustenasche_common.py` müssen im selben Skriptordner vorhanden sein. Für alle sechs Teile die Skripte einzeln ausführen; die Collections erscheinen dann gesammelt unter `KA_Armor_Variants`. Vor einem UE-Export in Blender nicht benötigte Varianten ausblenden oder nur die gewünschte Asset-Collection mit ihrem Rig auswählen.

## 7. Abgrenzung und nächste Ausbaustufe

Die Meshes sind geformte, quadbasierte Startformen und besitzen UV0, Material-Nodes, Rumpföffnungen, Pivot und Standard-Rig-Gewichte. Sie ersetzen keine finale Art-Review: vor Veröffentlichung sind Passform gegen die tatsächlich verwendete MetaHuman-/Spieler-Körperform, Gewichtsprüfung in Extremposen, Naht-/Rivet-Bereinigung, LOD-Reduktion, Texturbake und UE5-Import zu prüfen. Gesicht, Haare und darunterliegende Körperpolygone werden durch diese Slots nicht erzeugt oder automatisch ausgeblendet.

## KI-Inhaltsnachweis

| Datei | Werkzeug | Datum | Quelle |
|---|---|---|---|
| `scripts/variant_common.py`, `scripts/HEAD_*.py`, `scripts/TORSO_*.py` | OpenAI Codex | 2026-11-18 | KI-generierte Blender-Python-Generatoren; Geometrie aus Nutzerbriefing, keine externen Meshes/Texturen |
| `09_armor_variant_architecture.md` | OpenAI Codex | 2026-11-18 | KI-generierte technische Konzeptnotiz; Projektmaße und Nutzerbriefing |
