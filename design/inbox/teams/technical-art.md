# Technical Art & Pipeline Team

[Teamablauf](README.md) · `TA`

## Auftrag

Assets reproduzierbar von Blender bis zum Unreal-Import aufbereiten. Claude Code setzt Skripte und Projektintegration um.

## Ablauf

1. ART-Entwurf, Asset-ID, Herkunft/Lizenz und Budget prüfen.
2. Zentimeter, Z oben, Pivot, UVs, Materialslots, Kollision und Detailstufen nach Assettyp prüfen. Texturen nach den Asset-Konventionen ausgeben.
3. Generierte Dateien aus Quellen oder Skripten neu erzeugen. Manifest aktualisieren und Reimport mit derselben ID prüfen.

## Übergaben

- ENG und QA erhalten Asset-ID, Quell- und Zielpfad, Manifest, Importnachweis und offene Abweichungen.
- ANIM erhält bei Figuren Skelett, Referenzpose, Socket-Namen und Rig-Version.
- ART erhält Umsetzungsgrenzen und notwendige Entwurfsänderungen.

## Abnahme

Reimport liefert dieselbe Zuordnung und korrekte Maße, Materialien sowie Kollision. Budgetprüfung und Quellenbeleg liegen vor.

## Kontext

[Asset-Konventionen](../../../Assets/README.md) · [Importskript](../../../tools/unreal/import_assets.py)
