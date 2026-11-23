# Blender-Generatoren: Küstenasche-Rüstung

Prozedurale Startskripte für das Charakter- und Rüstungsset. Die Referenzen aus dem Chat bestimmen Silhouette und Verzierungen; Oberflächen bleiben gemäß Vorgabe dunkel, matt und gebraucht.

## Ordnerstruktur

```text
Blender_Kuestenasche/
├── README.md
├── scripts/
│   ├── kustenasche_common.py
│   ├── SM_Weapon_Zweihander.py
│   ├── SM_Armor_Helmet.py
│   ├── SK_Armor_Cuirass.py
│   ├── SK_Armor_Pauldrons_Arms.py
│   ├── SK_Armor_Tassets_Skirt.py
│   ├── SK_Character_Base.py
│   └── build_character_set.py
├── exports/
│   ├── fbx/
│   └── textures/
└── references/
```

## Blender-Collections

```text
KA_CharacterSet
├── 01_SM_Weapon_Zweihander
├── 02_SM_Armor_Helmet
├── 03_SK_Armor_Cuirass
│   ├── GEO_Cuirass
│   └── RIG_Cuirass
├── 04_SK_Armor_Pauldrons_Arms
│   ├── GEO_Pauldrons_Arms
│   └── RIG_Pauldrons_Arms
├── 05_SK_Armor_Tassets_Skirt
│   ├── GEO_Tassets_Skirt
│   └── RIG_Tassets_Skirt
└── 06_SK_Character_Base
    ├── GEO_Character_Base
    ├── HAIR_Character_Base
    └── RIG_Character_Base
```

Jedes Skript erzeugt nur seine eigene Asset-Collection. Das Schwert und der Helm enthalten jeweils ein zusammengefügtes Mesh. Cuirass, Arme und Tassets sind zusammengefügte Skeletal-Mesh-Kandidaten mit je einem A-Pose-Armature-Objekt; der Basiskörper hat ein eigenes Körper- und Haar-Mesh sowie das Charakter-Rig. Alle Rig-Generatoren verwenden dieselben Knochen-Namen und dieselbe 1,75-m-Restpose. Das Schwert liegt entlang +Z, seine breite Fläche zeigt nach -Y; sein Objektursprung ist das Griffzentrum. Der Helm zeigt nach -Y und hat den Ursprung an der Halsbasis, deckungsgleich mit `neck_01` des Rigs. Die Szeneneinheiten stehen auf Meter und 1 Blender Unit = 1 Meter.

## Ausführen

Lege `scripts/` als gemeinsames Verzeichnis ab und führe `build_character_set.py` in Blender 5.3 über **Scripting → Run Script** aus, um das ganze Set zu generieren. Jedes Asset-Skript kann auch einzeln ausgeführt werden. Alternativ kann Blender mit `--python` und dem vollständigen Pfad gestartet werden. Die Skripte importieren `kustenasche_common.py` aus demselben Verzeichnis. Ein erneuter Lauf ersetzt nur die jeweilige Asset-Collection.

## Mesh- und Materialhinweise

- Die großen Flächen und Profile sind aus Quad-Loops aufgebaut. Im Blender-5.3-Lauf waren je nach Asset 98,6–99,97 % der Polygone Quads; der Rest sind überwiegend kleine N-Gon-Abschlüsse. Die Helmschlitze besitzen offene Randloops.
- Jedes erzeugte Asset bekommt ein Smart-UV-Layout und Material-Slots für dunklen Stahl, patiniertes Messing, Holz, Leder, Leinen und Haut.
- Die drei `SK_Armor_*`-Skripte erzeugen passend zum Basisrig eigene Armature-Objekte. Beim UE-Import können sie dem gemeinsamen Character-Skeleton zugeordnet werden; die prozeduralen Gewichte sind Startgewichte und müssen für Animationen überprüft werden.
- Noise-/Bump-Knoten erzeugen Schmiedespuren und Rauheitsvariation; Cycles-Pointiness hellt Kanten leicht auf. FBX überträgt diese Nodes nicht als Unreal-Material; für UE5 müssen daraus Texturen gebacken oder die Materialparameter in Unreal nachgebaut werden.
- Körper, Gesicht, Haare, Maße, Proportionen und Reliefs sind prozedurale Startmodelle. Vor Spielintegration sind Sichtprüfung, Gewichtsfeinschliff, Retopologie/LOD, Kollisionskörper und ein UE-Importtest nötig.
- Die Bilder zeigen eine deutlich glänzende Konzeptdarstellung. Diese wurde als Formreferenz gelesen; die Shader behalten die gewünschte matte Küstenasche-Oberfläche.

## Umfang

Alle sechs Generator-Skripte sind enthalten. Sie erzeugen prozedurale, quad-dominante Blockout-Meshes. Die Basisfigur steht in A-Pose; Brustpanzer, Schulter-/Armschutz und Tassets verwenden dieselbe Restpose für den späteren UE-Skeleton-Import.

## Laufzeitprüfung

Alle sechs Generatoren wurden in Blender 5.3 gemeinsam ausgeführt. Meshes, UV-Layer, Material-Slots, Bump-Nodes und Rig-Bindungen wurden geprüft. Die vier Skeletal-Mesh-Rigs besitzen dieselben 21 Knochen; ein zweiter Lauf hinterließ keine doppelten Asset-Collections oder verwaisten Mesh-Daten.

## Kopf-/Torso-Varianten T1–T3

Die Architektur und Maße für sechs zusätzliche Varianten stehen in `09_armor_variant_architecture.md`. Für diese Variante muss jeweils eines der neuen Einstiegsskripte `HEAD_T1_LeatherCap.py`, `HEAD_T2_Spangenhelm.py`, `HEAD_T3_PotHelm.py`, `TORSO_T1_WoolTunic.py`, `TORSO_T2_Gambeson.py` oder `TORSO_T3_CuirBouilli.py` einzeln ausgeführt werden. Sie verwenden `variant_common.py` für Quad-Lofts, Material-Nodes, UV0 und das gemeinsame A-Pose-Skelett. Kopf-Pivots liegen an der Halsbasis, Torso-Pivots am Becken. Diese sechs neuen Skripte sind als Generatorentwurf abgelegt und wurden noch nicht in Blender ausgeführt.

Die 8-Slot-Architektur ist in `09_armor_variant_architecture.md` aktualisiert: Kopf, Torso, Schulter, Arme/Armschienen, Hände/Handschuhe, Beine/Beinschienen, Waffenrock/Surkot sowie Umhang/Fell. `slot_variants_03_08.py` enthält die 18 Generatoren für T1–T3 dieser sechs Slots; gleichnamige Einstiegsskripte rufen jeweils genau eine Variante auf. Diese neuen Skripte sind noch nicht in Blender ausgeführt.
