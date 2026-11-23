import os, sys, bpy
sys.dont_write_bytecode = True
try: script_path = __file__
except NameError:
    text = getattr(getattr(bpy.context, "space_data", None), "text", None)
    script_path = getattr(text, "filepath", "") or bpy.data.filepath
if not script_path: raise RuntimeError("Bitte Skript speichern oder Blender --python verwenden.")
sys.path.insert(0, os.path.dirname(os.path.abspath(script_path)))
from slot_variant_entry import run
run("ARMS_T1")
