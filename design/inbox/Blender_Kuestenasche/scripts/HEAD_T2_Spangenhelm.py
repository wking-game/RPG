"""Generate the Tier 2 dark iron spangenhelm in Blender."""
import os
import sys
import bpy

sys.dont_write_bytecode = True
try:
    script_path = __file__
except NameError:
    editor = getattr(bpy.context, "space_data", None)
    text_block = getattr(editor, "text", None)
    script_path = getattr(text_block, "filepath", "") or bpy.data.filepath
if not script_path:
    raise RuntimeError("Bitte die Datei speichern oder per Blender --python ausführen.")
sys.path.insert(0, os.path.dirname(os.path.abspath(script_path)))
from variant_common import generate_variant

generate_variant("HEAD_T2")
