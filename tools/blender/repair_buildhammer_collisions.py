import bpy
import os


BLEND_PATH = r"D:\gamer\gamer\Assets\Source\KA_T01_Tol_BuildHammer.blend"
FBX_PATH = r"D:\gamer\gamer\Assets\Items\KA_T01_Tol_BuildHammer.fbx"
ASSET_NAME = "KA_T01_Tol_BuildHammer"


def add_collision(name, location, dimensions):
    existing = bpy.data.objects.get(name)
    if existing:
        return existing
    x, y, z = (value / 2.0 for value in dimensions)
    vertices = [
        (-x, -y, -z), (x, -y, -z), (x, y, -z), (-x, y, -z),
        (-x, -y, z), (x, -y, z), (x, y, z), (-x, y, z),
    ]
    faces = [
        (0, 1, 2, 3), (4, 7, 6, 5), (0, 4, 5, 1),
        (1, 5, 6, 2), (2, 6, 7, 3), (4, 0, 3, 7),
    ]
    mesh = bpy.data.meshes.new(name + "_Mesh")
    mesh.from_pydata(vertices, [], faces)
    mesh.update()
    obj = bpy.data.objects.new(name, mesh)
    bpy.context.scene.collection.objects.link(obj)
    obj.location = location
    obj.display_type = "WIRE"
    obj.hide_render = True
    return obj


bpy.ops.wm.open_mainfile(filepath=BLEND_PATH)
asset = bpy.data.objects[ASSET_NAME]
shaft_collision = add_collision("UCX_KA_T01_Tol_BuildHammer_00", (0.0, 0.004, 0.050), (0.040, 0.050, 0.460))
head_collision = add_collision("UCX_KA_T01_Tol_BuildHammer_01", (0.0, 0.000, 0.240), (0.050, 0.155, 0.060))

for obj in bpy.context.scene.objects:
    obj.select_set(False)
for obj in (asset, shaft_collision, head_collision):
    obj.select_set(True)
bpy.context.view_layer.objects.active = asset
bpy.ops.wm.save_as_mainfile(filepath=BLEND_PATH)
bpy.ops.export_scene.fbx(
    filepath=FBX_PATH,
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
print("BUILD_HAMMER_COLLISIONS_REPAIRED")
