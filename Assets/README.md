# Assets

Von Blender erzeugt mit `tools/blender/build_assets.py`. **Nicht von Hand bearbeiten** – Änderungen im Skript machen und neu erzeugen:

```
E:\Blender\blender.exe --background --factory-startup --python tools/blender/build_assets.py -- Assets
```

## Konventionen

- Einheit: 1 = 1 cm, Z oben. FBX mit Unit-Scale cm.
- Normal Maps im **DirectX-Format** (Unreal-Standard, kein Grün-Umdrehen nötig).
- `_ORM`: R = Ambient Occlusion, G = Roughness, B = Metallic. Normal und ORM sind **Non-Color** (in Unreal sRGB aus).
- Ordner spiegeln die spätere Ablage in Unreal: `Assets/<Gruppe>/…` → `/Game/Gamer/<Gruppe>/…`.
- `manifest.json` listet jedes Asset mit Datei, Dreiecken, Maßen, Ursprung und Materialien – für automatische Weiterverarbeitung (z. B. Import-Skript in Unreal).

## Inhalt

| Ordner | Inhalt |
|---|---|
| `Buildings/WoodKit/` | 10 Holz-Bauteile (FBX) |
| `Items/` | `SM_Item_Log.fbx` |
| `Textures/` | Trim-Sheet Holz (BC/N/ORM, 2048²), Turf (BC/N, 512²), Baumstamm (BC/N/ORM, 1024²) |
| `UI/Icons/` | `T_Icon_Res_Wood.png` (256², freigestellt) |
| `Source/` | `Gamer_Assets.blend` (alle Objekte), Icon in 512² |
| `Previews/` | Vorschaubilder |

## Meshes

| Asset | Datei | Dreiecke | Ziel |
|---|---|---:|---:|
| `SM_Building_Foundation_Wood` | `Buildings/WoodKit/SM_Building_Foundation_Wood.fbx` | 58 | 500 |
| `SM_Building_Floor_Wood` | `Buildings/WoodKit/SM_Building_Floor_Wood.fbx` | 50 | 500 |
| `SM_Building_Wall_Wood` | `Buildings/WoodKit/SM_Building_Wall_Wood.fbx` | 70 | 800 |
| `SM_Building_Wall_Door_Wood` | `Buildings/WoodKit/SM_Building_Wall_Door_Wood.fbx` | 106 | 800 |
| `SM_Building_Door_Wood` | `Buildings/WoodKit/SM_Building_Door_Wood.fbx` | 80 | 400 |
| `SM_Building_Pillar_Wood` | `Buildings/WoodKit/SM_Building_Pillar_Wood.fbx` | 28 | 200 |
| `SM_Building_Beam_Wood` | `Buildings/WoodKit/SM_Building_Beam_Wood.fbx` | 28 | 200 |
| `SM_Building_Roof_Wood` | `Buildings/WoodKit/SM_Building_Roof_Wood.fbx` | 78 | 600 |
| `SM_Building_Roof_Ridge_Wood` | `Buildings/WoodKit/SM_Building_Roof_Ridge_Wood.fbx` | 20 | 200 |
| `SM_Building_Roof_Gable_Wood` | `Buildings/WoodKit/SM_Building_Roof_Gable_Wood.fbx` | 34 | 400 |
| `SM_Item_Log` | `Items/SM_Item_Log.fbx` | 70 | 300 |

Alle Texturen und Meshes sind prozedural von einem Skript erzeugt, das Claude Code geschrieben hat (Eintrag in `design/asset_log.md`).
