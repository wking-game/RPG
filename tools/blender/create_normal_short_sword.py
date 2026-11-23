import bpy
import math
import os
import sys

sys.path.insert(0, r"D:\gamer\gamer\tools\blender")
import create_reference_long_sword as common

ROOT = r"D:\gamer\gamer"
NAME = "KA_T01_Wep_ShortSword_Normal"
common.NAME = NAME
common.BLEND = os.path.join(ROOT, "Assets", "Source", NAME + ".blend")
common.FBX = os.path.join(ROOT, "Assets", "Items", NAME + ".fbx")
common.PREVIEW = os.path.join(ROOT, "Assets", "Previews", "Tier1", NAME + ".png")


def build():
    common.clear()
    scene = bpy.context.scene
    scene.unit_settings.system = "METRIC"
    scene.unit_settings.scale_length = 1.0

    steel = common.material("Mat_ShortSword_Steel", (0.025, 0.030, 0.032), (0.24, 0.27, 0.28), 0.93, 0.28, 18, 0.22)
    edge = common.material("Mat_ShortSword_Edge", (0.16, 0.18, 0.19), (0.48, 0.52, 0.53), 0.98, 0.15, 34, 0.08)
    leather = common.material("Mat_ShortSword_Leather", (0.012, 0.003, 0.001), (0.085, 0.018, 0.006), 0.0, 0.70, 26, 0.28)
    bronze = common.material("Mat_ShortSword_Bronze", (0.035, 0.014, 0.002), (0.20, 0.072, 0.010), 0.84, 0.36, 15, 0.25)

    parts = []
    blade_outline = [(-0.38, 0.055), (0.20, 0.050), (0.35, 0.025), (0.44, 0.0),
                     (0.35, -0.025), (0.20, -0.050), (-0.38, -0.055)]
    parts.append(common.prism("Broad Short Sword Blade", blade_outline, 0.024, steel, 0.0015))
    upper_edge = [(-0.365, 0.055), (0.20, 0.050), (0.35, 0.025), (0.44, 0.0),
                  (0.35, 0.015), (0.19, 0.038), (-0.365, 0.041)]
    lower_edge = [(x, -z) for x, z in upper_edge]
    parts.append(common.prism("Upper Sharpened Edge", upper_edge, 0.027, edge, 0.0007))
    parts.append(common.prism("Lower Sharpened Edge", lower_edge, 0.027, edge, 0.0007))
    fuller = [(-0.34, 0.018), (0.25, 0.015), (0.34, 0.0), (0.25, -0.015), (-0.34, -0.018)]
    parts.append(common.prism("Shallow Central Fuller", fuller, 0.028, steel, 0.001))

    parts.append(common.cylinder("Steel Guard Center", (-0.415, 0, 0), 0.045, 0.036, steel, 32))
    guard_points = [(-0.435, 0.16), (-0.405, 0.17), (-0.390, 0.045),
                    (-0.390, -0.045), (-0.405, -0.17), (-0.435, -0.16)]
    parts.append(common.prism("Straight Practical Crossguard", guard_points, 0.030, steel, 0.003))
    parts.append(common.cylinder("Bronze Guard Collar", (-0.442, 0, 0), 0.052, 0.014, bronze, 36))

    parts.append(common.cylinder("Wooden Grip Core", (-0.575, 0, 0), 0.031, 0.25, leather, 32, scale=(1, 0.86, 1.08)))
    for index in range(8):
        x = -0.475 - index * 0.029
        bpy.ops.mesh.primitive_torus_add(major_radius=0.033, minor_radius=0.0028, major_segments=28, minor_segments=7,
                                        location=(x, 0, 0), rotation=(0, math.pi / 2, 0))
        wrap = bpy.context.object
        wrap.name = "Leather Grip Wrap"
        wrap.scale.z = 1.09
        bpy.ops.object.transform_apply(location=False, rotation=False, scale=True)
        wrap.data.materials.append(leather)
        parts.append(wrap)

    parts.append(common.cylinder("Pommel Neck", (-0.715, 0, 0), 0.031, 0.026, steel, 32))
    parts.append(common.cylinder("Compact Disc Pommel", (-0.755, 0, 0), 0.060, 0.055, steel, 40, scale=(1, 0.82, 1.0)))
    parts.append(common.cylinder("Bronze Pommel Inlay", (-0.785, 0, 0), 0.038, 0.010, bronze, 32))

    asset = common.join(parts, (steel, edge, leather, bronze))
    collisions = [
        common.collision("UCX_" + NAME + "_00", (0.02, 0, 0), (0.86, 0.06, 0.13)),
        common.collision("UCX_" + NAME + "_01", (-0.59, 0, 0), (0.40, 0.10, 0.36)),
    ]
    common.preview()
    camera = bpy.data.objects.get("Preview_Camera")
    camera.data.lens = 57
    camera.location = (-0.20, -2.50, 0.62)
    common.look_at(camera, (-0.18, 0, 0))

    bpy.ops.object.select_all(action="DESELECT")
    for obj in [asset] + collisions:
        obj.select_set(True)
    bpy.context.view_layer.objects.active = asset
    bpy.ops.wm.save_as_mainfile(filepath=common.BLEND)
    bpy.ops.export_scene.fbx(filepath=common.FBX, use_selection=True, object_types={"MESH"}, apply_unit_scale=True,
                             apply_scale_options="FBX_SCALE_ALL", axis_forward="-Y", axis_up="Z",
                             mesh_smooth_type="FACE", add_leaf_bones=False, bake_anim=False)
    bpy.context.scene.render.filepath = common.PREVIEW
    bpy.ops.render.render(write_still=True)
    triangles = sum(len(face.vertices) - 2 for face in asset.data.polygons)
    print(f"NORMAL_SHORT_SWORD_COMPLETE {NAME} triangles={triangles} materials={len(asset.data.materials)}")


if __name__ == "__main__":
    build()
