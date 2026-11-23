# Export-Test nach Leitfaden Abschnitt 4: minimales Skelett (Morph Targets brauchen in Unreal ein
# Skelett-Mesh), FBX schreiben, in eine leere Szene zurückimportieren und prüfen.
import bpy, os
from mathutils import Vector

OUT = r"D:\gamer\gamer\design\inbox\Testkopf_2026-11-21\SK_Testkopf.fbx"
sc = bpy.data.scenes["Testkopf"]; bpy.context.window.scene = sc
head = bpy.data.objects["SM_Testkopf"]
eyes = [bpy.data.objects["SM_Testkopf_Auge_L"], bpy.data.objects["SM_Testkopf_Auge_R"]]
if head.data.shape_keys and head.data.shape_keys.animation_data:
    head.data.shape_keys.animation_data_clear()           # Vorschau-Keys nicht mitexportieren
for k in head.data.shape_keys.key_blocks[1:]: k.value = 0.0

# Skelett: root (Boden unter dem Hals) -> neck -> head
arm_name = "SK_Testkopf"
if arm_name in bpy.data.objects: bpy.data.objects.remove(bpy.data.objects[arm_name])
ad = bpy.data.armatures.new(arm_name); arm = bpy.data.objects.new(arm_name, ad); sc.collection.objects.link(arm)
bpy.context.view_layer.objects.active = arm
for o in sc.objects: o.select_set(o == arm)
bpy.ops.object.mode_set(mode='EDIT')
zmin = min((head.matrix_world @ v.co).z for v in head.data.vertices)
b_root = ad.edit_bones.new("root"); b_root.head = (0, 0, zmin); b_root.tail = (0, 0.05, zmin)
b_neck = ad.edit_bones.new("neck"); b_neck.head = (0, 0.01, zmin); b_neck.tail = (0, 0.005, -0.09); b_neck.parent = b_root
b_head = ad.edit_bones.new("head"); b_head.head = b_neck.tail; b_head.tail = (0, 0.0, 0.12); b_head.parent = b_neck; b_head.use_connect = True
bpy.ops.object.mode_set(mode='OBJECT')

# Gewichte: Hals unten -> neck, Rest -> head (weich überblendet); Augen ganz an head
def bind(o, fn):
    o.parent = arm; o.matrix_parent_inverse = arm.matrix_world.inverted()
    for g in list(o.vertex_groups): o.vertex_groups.remove(g)
    gn = o.vertex_groups.new(name="neck"); gh = o.vertex_groups.new(name="head")
    for v in o.data.vertices:
        z = (o.matrix_world @ v.co).z; w = fn(z)
        if w < 1: gn.add([v.index], 1 - w, 'REPLACE')
        if w > 0: gh.add([v.index], w, 'REPLACE')
    m = next((m for m in o.modifiers if m.type == 'ARMATURE'), None) or o.modifiers.new("Armature", 'ARMATURE')
    m.object = arm
def head_w(z):
    t = max(0.0, min(1.0, (z + 0.12) / 0.04)); return t * t * (3 - 2 * t)
for o in [head] + eyes:
    o.parent = None
bind(head, head_w)
for e in eyes: bind(e, lambda z: 1.0)

# Export
for o in sc.objects: o.select_set(o in [arm, head] + eyes)
bpy.ops.export_scene.fbx(filepath=OUT, use_selection=True, object_types={'ARMATURE', 'MESH'},
    apply_unit_scale=True, apply_scale_options='FBX_SCALE_ALL', axis_forward='-Y', axis_up='Z',
    use_mesh_modifiers=False, mesh_smooth_type='FACE', use_tspace=True,
    add_leaf_bones=False, primary_bone_axis='Y', secondary_bone_axis='X', use_armature_deform_only=True,
    bake_anim=False)
print("geschrieben:", OUT, round(os.path.getsize(OUT) / 1024), "KB")

# Rückimport in leere Szene
chk = bpy.data.scenes.get("Testkopf_Reimport") or bpy.data.scenes.new("Testkopf_Reimport")
for o in list(chk.objects): bpy.data.objects.remove(o, do_unlink=True)
bpy.context.window.scene = chk
bpy.ops.import_scene.fbx(filepath=OUT)
bpy.context.view_layer.update()
for o in chk.objects:
    line = f"{o.name}: {o.type}, Skala {tuple(round(s, 4) for s in o.scale)}, Maße {tuple(round(d, 3) for d in o.dimensions)}"
    if o.type == 'MESH':
        sk = o.data.shape_keys
        line += f", Shape-Keys {[k.name for k in sk.key_blocks] if sk else None}, Materialien {[m.name for m in o.data.materials]}"
        line += f", Gruppen {[g.name for g in o.vertex_groups]}"
    if o.type == 'ARMATURE':
        line += f", Knochen {[(b.name, round(b.length, 3)) for b in o.data.bones]}"
    print(line)
bpy.context.window.scene = sc
