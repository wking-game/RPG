import bpy
import os
import sys


def render_preview(blend_path, output_directory):
    bpy.ops.wm.open_mainfile(filepath=blend_path)
    asset_name = os.path.splitext(os.path.basename(blend_path))[0]
    if bpy.context.scene.camera is None:
        raise RuntimeError(f"No preview camera found in {asset_name}")
    os.makedirs(output_directory, exist_ok=True)
    output_path = os.path.join(output_directory, asset_name + ".png")
    scene = bpy.context.scene
    scene.render.filepath = output_path
    scene.render.resolution_x = 640
    scene.render.resolution_y = 640
    scene.render.resolution_percentage = 100
    scene.render.image_settings.file_format = "PNG"
    bpy.ops.render.render(write_still=True)
    print(f"PREVIEW_RENDERED {asset_name} -> {output_path}")


if __name__ == "__main__":
    arguments = sys.argv[sys.argv.index("--") + 1:]
    destination = os.path.abspath(arguments[0])
    for source in arguments[1:]:
        render_preview(os.path.abspath(source), destination)
