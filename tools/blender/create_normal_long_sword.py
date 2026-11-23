import bpy
import os
import sys

sys.path.insert(0, r"D:\gamer\gamer\tools\blender")
import create_normal_short_sword as source

ROOT = r"D:\gamer\gamer"
NAME = "KA_T01_Wep_LongSword_Normal"
BLEND = os.path.join(ROOT, "Assets", "Source", NAME + ".blend")
FBX = os.path.join(ROOT, "Assets", "Items", NAME + ".fbx")
PREVIEW = os.path.join(ROOT, "Assets", "Previews", "Tier1", NAME + ".png")


def build():
    source.build()
    asset = bpy.data.objects.get("KA_T01_Wep_ShortSword_Normal")
    asset.name = NAME

    for vertex in asset.data.vertices:
        if vertex.co.x > -0.38:
            vertex.co.x = -0.38 + (vertex.co.x + 0.38) * 1.55
        elif vertex.co.x < -0.44:
            vertex.co.x = -0.44 + (vertex.co.x + 0.44) * 1.25

    for obj in bpy.data.objects:
        if obj.name.startswith("UCX_KA_T01_Wep_ShortSword_Normal"):
            obj.name = obj.name.replace("KA_T01_Wep_ShortSword_Normal", NAME)
    blade_collision = bpy.data.objects.get("UCX_" + NAME + "_00")
    blade_collision.location.x = 0.24
    blade_collision.dimensions.x = 1.33
    bpy.context.view_layer.objects.active = blade_collision
    bpy.ops.object.transform_apply(location=False, rotation=False, scale=True)
    grip_collision = bpy.data.objects.get("UCX_" + NAME + "_01")
    grip_collision.location.x = -0.66
    grip_collision.dimensions.x = 0.52
    bpy.context.view_layer.objects.active = grip_collision
    bpy.ops.object.transform_apply(location=False, rotation=False, scale=True)

    camera = bpy.data.objects.get("Preview_Camera")
    camera.data.lens = 54
    camera.location = (-0.18, -3.35, 0.76)
    source.common.look_at(camera, (-0.15, 0, 0))
    bpy.context.scene.render.filepath = PREVIEW

    bpy.ops.object.select_all(action="DESELECT")
    collisions = [blade_collision, grip_collision]
    for obj in [asset] + collisions:
        obj.select_set(True)
    bpy.context.view_layer.objects.active = asset
    bpy.ops.wm.save_as_mainfile(filepath=BLEND)
    bpy.ops.export_scene.fbx(filepath=FBX, use_selection=True, object_types={"MESH"}, apply_unit_scale=True,
                             apply_scale_options="FBX_SCALE_ALL", axis_forward="-Y", axis_up="Z",
                             mesh_smooth_type="FACE", add_leaf_bones=False, bake_anim=False)
    bpy.ops.render.render(write_still=True)
    triangles = sum(len(face.vertices) - 2 for face in asset.data.polygons)
    print(f"NORMAL_LONG_SWORD_COMPLETE {NAME} triangles={triangles} materials={len(asset.data.materials)}")


if __name__ == "__main__":
    build()
