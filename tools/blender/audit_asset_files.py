import bpy
import json
import os
import sys


def triangle_count(mesh_object):
    return sum(len(polygon.vertices) - 2 for polygon in mesh_object.data.polygons)


def audit(blend_path):
    bpy.ops.wm.open_mainfile(filepath=blend_path)
    mesh_objects = [obj for obj in bpy.data.objects if obj.type == "MESH"]
    render_meshes = [obj for obj in mesh_objects if not obj.name.startswith("UCX_")]
    collision_meshes = [obj for obj in mesh_objects if obj.name.startswith("UCX_")]
    materials = sorted({slot.material.name for obj in render_meshes for slot in obj.material_slots if slot.material})
    uv_layers = {obj.name: len(obj.data.uv_layers) for obj in render_meshes}
    origins = {obj.name: [round(value, 6) for value in obj.location] for obj in render_meshes}
    return {
        "file": blend_path,
        "render_objects": [obj.name for obj in render_meshes],
        "collision_objects": [obj.name for obj in collision_meshes],
        "triangles": sum(triangle_count(obj) for obj in render_meshes),
        "materials": materials,
        "uv_layers": uv_layers,
        "origins": origins,
    }


if __name__ == "__main__":
    paths = [argument for argument in sys.argv[sys.argv.index("--") + 1:] if argument.lower().endswith(".blend")]
    print("ASSET_AUDIT=" + json.dumps([audit(os.path.abspath(path)) for path in paths], ensure_ascii=False))
