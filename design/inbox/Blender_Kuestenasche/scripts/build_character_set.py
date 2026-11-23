"""Baut das vollständige Küstenasche-Set; Asset-Skripte bleiben einzeln ausführbar."""

from pathlib import Path
import sys

sys.dont_write_bytecode = True
SCRIPT_DIR = Path(__file__).resolve().parent
if str(SCRIPT_DIR) not in sys.path:
    sys.path.insert(0, str(SCRIPT_DIR))

import SM_Weapon_Zweihander
import SM_Armor_Helmet
import SK_Armor_Cuirass
import SK_Armor_Pauldrons_Arms
import SK_Armor_Tassets_Skirt
import SK_Character_Base


def build_all():
    # Erst die Körperreferenz, dann die angepassten Rüstungsteile und Waffen.
    results = {}
    results["SK_Character_Base"] = SK_Character_Base.build()
    results["SK_Armor_Cuirass"] = SK_Armor_Cuirass.build()
    results["SK_Armor_Pauldrons_Arms"] = SK_Armor_Pauldrons_Arms.build()
    results["SK_Armor_Tassets_Skirt"] = SK_Armor_Tassets_Skirt.build()
    results["SM_Armor_Helmet"] = SM_Armor_Helmet.build()
    results["SM_Weapon_Zweihander"] = SM_Weapon_Zweihander.build()
    return results


if __name__ == "__main__":
    generated = build_all()
    print("Kuestenasche assets created:", ", ".join(generated.keys()))
