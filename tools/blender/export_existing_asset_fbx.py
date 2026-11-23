import bpy
import os
import sys


def export_asset(blend_path, output_directory):
    bpy.ops.wm.open_mainfile(filepath=blend_path)
    asset_name = os.path.splitext(os.path.basename(blend_path))[0]
    export_objects = [
        obj for obj in bpy.data.objects
        if obj.type == "MESH" and (obj.name == asset_name or obj.name.startswith("UCX_" + asset_name))
    ]
    if not export_objects:
        raise RuntimeError(f"No exportable mesh objects found for {asset_name}")

    bpy.ops.object.select_all(action="DESELECT")
    for obj in export_objects:
        obj.select_set(True)
    bpy.context.view_layer.objects.active = next(obj for obj in export_objects if obj.name == asset_name)

    output_path = os.path.join(output_directory, asset_name + ".fbx")
    bpy.ops.export_scene.fbx(
        filepath=output_path,
        use_selection=True,
        object_types={"MESH"},
        apply_unit_scale=True,
        apply_scale_options="FBX_SCALE_ALL",
        axis_forward="-Y",
        axis_up="Z",
        mesh_smooth_type="FACE",
        add_leaf_bones=False,
        bake_anim=False,
    )
    print(f"EXPORTED_EXISTING_ASSET {asset_name} -> {output_path}")


if __name__ == "__main__":
    arguments = sys.argv[sys.argv.index("--") + 1:]
    destination = os.path.abspath(arguments[0])
    for source in arguments[1:]:
        export_asset(os.path.abspath(source), destination)
