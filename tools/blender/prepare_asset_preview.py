import bpy
import math
import os
import sys
from mathutils import Vector


def ensure_collection(name):
    collection = bpy.data.collections.get(name)
    if collection is None:
        collection = bpy.data.collections.new(name)
        bpy.context.scene.collection.children.link(collection)
    return collection


def move_to_collection(obj, destination):
    for source in list(obj.users_collection):
        source.objects.unlink(obj)
    destination.objects.link(obj)


def look_at(obj, target):
    direction = Vector(target) - obj.location
    obj.rotation_euler = direction.to_track_quat("-Z", "Y").to_euler()


def add_area_light(collection, name, location, energy, size, color, target):
    data = bpy.data.lights.new(name=name, type="AREA")
    data.energy = energy
    data.shape = "DISK"
    data.size = size
    data.color = color
    light = bpy.data.objects.new(name, data)
    collection.objects.link(light)
    light.location = location
    look_at(light, target)
    return light


def asset_bounds(asset):
    corners = [asset.matrix_world @ Vector(corner) for corner in asset.bound_box]
    minimum = Vector((min(p.x for p in corners), min(p.y for p in corners), min(p.z for p in corners)))
    maximum = Vector((max(p.x for p in corners), max(p.y for p in corners), max(p.z for p in corners)))
    return (minimum + maximum) * 0.5, maximum - minimum


def prepare(blend_path):
    bpy.ops.wm.open_mainfile(filepath=blend_path)
    asset_name = os.path.splitext(os.path.basename(blend_path))[0]
    asset = bpy.data.objects.get(asset_name)
    if asset is None:
        raise RuntimeError(f"Main asset object not found: {asset_name}")

    for obj in list(bpy.data.objects):
        if obj.type in {"CAMERA", "LIGHT"}:
            bpy.data.objects.remove(obj, do_unlink=True)

    preview_collection = ensure_collection("PREVIEW_RIG")
    collision_collection = ensure_collection("COLLISION")
    for obj in bpy.data.objects:
        if obj.name.startswith("UCX_"):
            move_to_collection(obj, collision_collection)
            obj.hide_render = True
            obj.hide_set(True)
            obj.display_type = "WIRE"
    collision_collection.hide_render = True
    collision_collection.hide_viewport = True

    center, dimensions = asset_bounds(asset)
    span = max(dimensions.x, dimensions.y, dimensions.z, 0.25)
    camera_distance = span * 2.15
    # Scale light power with the square of asset size so small props are not overexposed.
    light_scale = span * span
    camera_data = bpy.data.cameras.new("Preview_Camera")
    camera_data.lens = 58
    camera_data.sensor_width = 36
    camera = bpy.data.objects.new("Preview_Camera", camera_data)
    preview_collection.objects.link(camera)
    front_sign = 1.0 if "Shield" in asset_name else -1.0
    camera.location = center + Vector((camera_distance * 0.72, front_sign * camera_distance, camera_distance * 0.48))
    look_at(camera, center)
    bpy.context.scene.camera = camera

    light_size = span * 0.75
    add_area_light(
        preview_collection, "Preview_Key", center + Vector((span * 1.2, front_sign * span * 1.5, span * 1.35)),
        120.0 * light_scale, light_size, (1.0, 0.78, 0.60), center,
    )
    add_area_light(
        preview_collection, "Preview_Fill", center + Vector((-span * 1.15, front_sign * span * 0.65, span * 0.45)),
        42.0 * light_scale, light_size * 1.15, (0.55, 0.68, 1.0), center,
    )
    add_area_light(
        preview_collection, "Preview_Rim", center + Vector((0, span * 1.2, span * 1.0)),
        72.0 * light_scale, light_size * 0.85, (0.72, 0.84, 1.0), center,
    )

    scene = bpy.context.scene
    scene.render.engine = "BLENDER_EEVEE"
    scene.render.resolution_x = 720
    scene.render.resolution_y = 720
    scene.render.resolution_percentage = 100
    scene.render.image_settings.file_format = "PNG"
    scene.world.use_nodes = True
    world_background = scene.world.node_tree.nodes.get("Background")
    world_background.inputs["Color"].default_value = (0.012, 0.016, 0.022, 1.0)
    world_background.inputs["Strength"].default_value = 0.18
    scene.view_settings.look = "AgX - Medium High Contrast"
    scene.view_settings.exposure = -0.55

    for obj in scene.objects:
        obj.select_set(False)
    asset.hide_set(False)
    asset.select_set(True)
    bpy.context.view_layer.objects.active = asset
    bpy.ops.wm.save_as_mainfile(filepath=blend_path)
    print(f"PREVIEW_RIG_READY {asset_name}")


if __name__ == "__main__":
    paths = [argument for argument in sys.argv[sys.argv.index("--") + 1:] if argument.lower().endswith(".blend")]
    for path in paths:
        prepare(os.path.abspath(path))
