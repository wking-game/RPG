"""Build every previously authored Küstenasche character/armor asset separately.

Run in Blender 5.x with:
  blender --background --factory-startup --python build_complete_character_set.py

The six original assets are generated first. Then the six HEAD/TORSO T1-T3
variants and all 18 SHOULDER/ARMS/HANDS/LEGS/SURCOAT/CLOAK T1-T3 variants are
generated one by one. Variant collections are retained but hidden initially so
they do not z-fight or cover the original assembled reference set.
"""
import os
import sys

import bpy

sys.dont_write_bytecode = True
SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))
if SCRIPT_DIR not in sys.path:
    sys.path.insert(0, SCRIPT_DIR)

import SK_Character_Base
import SK_Armor_Cuirass
import SK_Armor_Pauldrons_Arms
import SK_Armor_Tassets_Skirt
import SM_Armor_Helmet
import SM_Weapon_Zweihander
from variant_common import generate_variant as generate_head_torso
from slot_variants_03_08 import BUILDERS as SLOT_BUILDERS


def remove_factory_objects():
    # This builder runs with --factory-startup; remove only those default scene objects.
    for obj in list(bpy.data.objects):
        bpy.data.objects.remove(obj, do_unlink=True)


def build_everything():
    remove_factory_objects()
    scene = bpy.context.scene
    scene.unit_settings.system = "METRIC"
    scene.unit_settings.scale_length = 1.0
    scene.unit_settings.length_unit = "METERS"

    original_builders = (
        ("SK_Character_Base", SK_Character_Base.build),
        ("SK_Armor_Cuirass", SK_Armor_Cuirass.build),
        ("SK_Armor_Pauldrons_Arms", SK_Armor_Pauldrons_Arms.build),
        ("SK_Armor_Tassets_Skirt", SK_Armor_Tassets_Skirt.build),
        ("SM_Armor_Helmet", SM_Armor_Helmet.build),
        ("SM_Weapon_Zweihander", SM_Weapon_Zweihander.build),
    )
    completed = []
    for asset_id, builder in original_builders:
        print("[KA_BUILD_BEGIN] " + asset_id, flush=True)
        builder()
        completed.append(asset_id)
        print("[KA_BUILD_DONE] " + asset_id, flush=True)

    head_torso_ids = ("HEAD_T1", "HEAD_T2", "HEAD_T3",
                      "TORSO_T1", "TORSO_T2", "TORSO_T3")
    for variant_id in head_torso_ids:
        print("[KA_BUILD_BEGIN] " + variant_id, flush=True)
        obj = generate_head_torso(variant_id)
        for collection in obj.users_collection:
            collection.hide_viewport = True
            collection.hide_render = True
        completed.append(variant_id)
        print("[KA_BUILD_DONE] " + variant_id, flush=True)

    for variant_id, builder in SLOT_BUILDERS.items():
        print("[KA_BUILD_BEGIN] " + variant_id, flush=True)
        obj = builder()
        for collection in obj.users_collection:
            collection.hide_viewport = True
            collection.hide_render = True
        completed.append(variant_id)
        print("[KA_BUILD_DONE] " + variant_id, flush=True)

    root = bpy.data.collections.get("KA_Armor_Variants")
    if root:
        root["generated_variant_count"] = 24
        root["variant_visibility_note"] = "All 24 variant collections retained and initially hidden to avoid overlap."
    scene["generated_asset_script_count"] = len(completed)
    scene["generated_asset_ids"] = ",".join(completed)

    output_dir = os.path.abspath(os.path.join(SCRIPT_DIR, "..", "outputs"))
    os.makedirs(output_dir, exist_ok=True)
    output_file = os.path.join(output_dir, "KA_Complete_Character_Armor_Set.blend")
    bpy.ops.wm.save_as_mainfile(filepath=output_file, compress=True)
    print("[KA_BUILD_SUMMARY] scripts=%d variants=24 file=%s" % (len(completed), output_file), flush=True)
    return output_file


if __name__ == "__main__":
    build_everything()
