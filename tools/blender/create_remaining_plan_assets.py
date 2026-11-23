import bpy
import math
import os
import sys
from mathutils import Vector

ROOT = r"D:\gamer\gamer"
SRC = os.path.join(ROOT, "Assets", "Source")
OUT = os.path.join(ROOT, "Assets", "Items")

WOOD = (0.28, 0.095, 0.028)
IRON = (0.105, 0.135, 0.145)
RIVET = (0.19, 0.21, 0.21)


def clear():
    bpy.ops.object.select_all(action="SELECT")
    bpy.ops.object.delete(use_global=False)
    for blocks in (bpy.data.meshes, bpy.data.curves, bpy.data.materials):
        for block in list(blocks):
            if block.users == 0:
                blocks.remove(block)


def mat(name, color, metallic=0.0, rough=0.55):
    m = bpy.data.materials.new(name)
    m.diffuse_color = (*color, 1)
    m.use_nodes = True
    p = m.node_tree.nodes.get("Principled BSDF")
    p.inputs["Base Color"].default_value = (*color, 1)
    p.inputs["Metallic"].default_value = metallic
    p.inputs["Roughness"].default_value = rough
    n = m.node_tree.nodes.new("ShaderNodeTexNoise")
    n.inputs["Scale"].default_value = 5.0
    n.inputs["Detail"].default_value = 2.4
    r = m.node_tree.nodes.new("ShaderNodeValToRGB")
    r.color_ramp.elements[0].color = (*(c * .62 for c in color), 1)
    r.color_ramp.elements[1].color = (*(min(1, c * 1.3) for c in color), 1)
    m.node_tree.links.new(n.outputs["Fac"], r.inputs["Fac"])
    m.node_tree.links.new(r.outputs["Color"], p.inputs["Base Color"])
    return m


def cube(name, loc, dims, material, bevel=.002, seg=3, rot=(0, 0, 0)):
    bpy.ops.mesh.primitive_cube_add(location=loc, rotation=rot)
    o = bpy.context.object; o.name = name; o.dimensions = dims
    bpy.ops.object.transform_apply(location=False, rotation=False, scale=True)
    o.data.materials.append(material)
    b = o.modifiers.new("Hand-Forged Edges", "BEVEL"); b.width = bevel; b.segments = seg
    bpy.context.view_layer.objects.active = o; bpy.ops.object.modifier_apply(modifier=b.name)
    return o


def cyl(name, loc, radius, depth, material, vertices=24, rot=(0, 0, 0), scale=(1, 1, 1)):
    bpy.ops.mesh.primitive_cylinder_add(vertices=vertices, radius=radius, depth=depth, location=loc, rotation=rot)
    o = bpy.context.object; o.name = name; o.scale = scale
    bpy.ops.object.transform_apply(location=False, rotation=False, scale=True)
    o.data.materials.append(material)
    b = o.modifiers.new("Worn Edges", "BEVEL"); b.width = min(.002, radius * .12); b.segments = 2
    bpy.context.view_layer.objects.active = o; bpy.ops.object.modifier_apply(modifier=b.name)
    return o


def sphere(name, loc, material, radius=.0035, scale=(1, 1, .58)):
    bpy.ops.mesh.primitive_uv_sphere_add(segments=12, ring_count=8, radius=radius, location=loc)
    o = bpy.context.object; o.name = name; o.scale = scale
    bpy.ops.object.transform_apply(location=False, rotation=False, scale=True); o.data.materials.append(material)
    return o


def poly_prism(name, points_yz, thickness, material):
    verts = [(-thickness / 2, y, z) for y, z in points_yz] + [(thickness / 2, y, z) for y, z in points_yz]
    n = len(points_yz); faces = [tuple(range(n - 1, -1, -1)), tuple(range(n, 2 * n))]
    for i in range(n): faces.append((i, (i + 1) % n, n + (i + 1) % n, n + i))
    me = bpy.data.meshes.new(name + "_Mesh"); me.from_pydata(verts, [], faces); me.update()
    o = bpy.data.objects.new(name, me); bpy.context.collection.objects.link(o); o.data.materials.append(material)
    b = o.modifiers.new("Forged Bevel", "BEVEL"); b.width = .0015; b.segments = 3
    bpy.context.view_layer.objects.active = o; bpy.ops.object.modifier_apply(modifier=b.name)
    return o


def poly_prism_xz(name, points_xz, thickness_y, material):
    vertices = [(x, -thickness_y / 2, z) for x, z in points_xz]
    vertices += [(x, thickness_y / 2, z) for x, z in points_xz]
    count = len(points_xz)
    faces = [tuple(range(count - 1, -1, -1)), tuple(range(count, count * 2))]
    for index in range(count):
        next_index = (index + 1) % count
        faces.append((index, next_index, count + next_index, count + index))
    mesh = bpy.data.meshes.new(name + "_Mesh")
    mesh.from_pydata(vertices, [], faces)
    mesh.update()
    obj = bpy.data.objects.new(name, mesh)
    bpy.context.collection.objects.link(obj)
    obj.data.materials.append(material)
    bevel = obj.modifiers.new("Forged Bevel", "BEVEL")
    bevel.width = 0.002
    bevel.segments = 4
    bpy.context.view_layer.objects.active = obj
    bpy.ops.object.modifier_apply(modifier=bevel.name)
    return obj


def join(parts, name, materials):
    bpy.ops.object.select_all(action="DESELECT")
    for p in parts: p.select_set(True)
    bpy.context.view_layer.objects.active = parts[0]; bpy.ops.object.join(); o = bpy.context.object; o.name = name
    for m in materials:
        if m.name not in [x.name for x in o.data.materials]: o.data.materials.append(m)
    bpy.ops.object.transform_apply(location=True, rotation=True, scale=True)
    bpy.ops.object.mode_set(mode="EDIT"); bpy.ops.mesh.select_all(action="SELECT")
    bpy.ops.uv.smart_project(angle_limit=math.radians(66), island_margin=.02)
    bpy.ops.mesh.normals_make_consistent(inside=False); bpy.ops.object.mode_set(mode="OBJECT")
    for p in o.data.polygons: p.use_smooth = True
    return o


def collision(name, loc, dims, rot=(0, 0, 0)):
    bpy.ops.mesh.primitive_cube_add(location=loc, rotation=rot)
    o = bpy.context.object; o.name = name; o.dimensions = dims
    bpy.ops.object.transform_apply(location=False, rotation=False, scale=True); o.display_type = "WIRE"; o.hide_render = True
    return o


def finish(asset, cols):
    bpy.ops.object.select_all(action="DESELECT")
    for o in [asset] + cols: o.select_set(True)
    bpy.context.view_layer.objects.active = asset
    blend = os.path.join(SRC, asset.name + ".blend"); fbx = os.path.join(OUT, asset.name + ".fbx")
    bpy.ops.wm.save_as_mainfile(filepath=blend)
    bpy.ops.export_scene.fbx(filepath=fbx, use_selection=True, object_types={"MESH"}, apply_unit_scale=True,
        apply_scale_options="FBX_SCALE_ALL", axis_forward="-Y", axis_up="Z", mesh_smooth_type="FACE",
        add_leaf_bones=False, bake_anim=False)
    tris = sum(len(p.vertices) - 2 for p in asset.data.polygons)
    print(f"ASSET_OK {asset.name} triangles={tris} materials={len(asset.data.materials)}")


def setup(names):
    clear(); s = bpy.context.scene; s.unit_settings.system = "METRIC"; s.unit_settings.scale_length = 1
    s.render.engine = "BLENDER_EEVEE"; s.world.color = (.018, .022, .026)
    return (mat(names[0], WOOD, 0, .58), mat(names[1], IRON, .82, .32), mat(names[2], RIVET, .9, .25))


def rivets4(parts, rv, locs, radius=.0035):
    for i, loc in enumerate(locs): parts.append(sphere(f"Niete_{i+1}", loc, rv, radius))


def sickle():
    name="KA_T01_Tol_Sickle"; wood,iron,rv=setup(("Mat_T01_Sickle_Wood","Mat_T01_Sickle_Iron","Mat_T01_Sickle_Rivets")); p=[]
    p.append(cyl("Holzgriff",(0,0,0),.017,.15,wood,28,(math.pi/2,0,0), (1,1.16,1)))
    p.append(cyl("Zwinge",(0,.083,0),.020,.016,iron,24,(math.pi/2,0,0),(1,1.16,1)))
    pts=[(.088,-.025),(.14,-.024),(.205,-.005),(.265,.030),(.315,.080),(.335,.105),(.320,.057),(.280,.016),(.220,-.018),(.150,-.040)]
    p.append(poly_prism("Broad Curved Sickle Blade",pts,.010,iron))
    p.append(poly_prism("Reinforced Blade Spine",[(.10,-.019),(.18,-.002),(.255,.032),(.300,.065),(.276,.025),(.205,-.014)],.016,iron))
    rivets4(p,rv,[(.020,.083,0),(-.020,.083,0),(0,.083,.023),(0,.083,-.023)])
    a=join(p,name,(wood,iron,rv)); finish(a,[collision("UCX_"+name+"_00",(0,0,0),(.045,.15,.05)),collision("UCX_"+name+"_01",(0,.19,.025),(.025,.22,.11))])


def shovel():
    name="KA_T01_Tol_Shovel"; wood,iron,rv=setup(("Mat_T01_Shovel_Wood","Mat_T01_Shovel_Iron","Mat_T01_Shovel_Rivets")); p=[]
    # Shift the source geometry so that the specified grip center is at world origin.
    p.append(cyl("Connected Hardwood Shaft",(0,0,-.25),.019,.94,wood,28))
    p.append(cyl("T Grip",(0,0,.235),.013,.16,wood,28,(0,math.pi/2,0)))
    p.append(cube("Grip Join",(0,0,.205),(.052,.040,.070),wood,.008,4))
    blade_profile = [
        (-.105, -.890), (-.075, -.955), (0.0, -.985), (.075, -.955), (.105, -.890),
        (.100, -.785), (.075, -.725), (.035, -.690), (-.035, -.690), (-.075, -.725), (-.100, -.785),
    ]
    p.append(poly_prism_xz("Pointed Shovel Blade", blade_profile, .018, iron))
    p.append(poly_prism_xz("Blade Center Ridge",[(0,-.972),(.018,-.90),(.015,-.735),(0,-.700),(-.015,-.735),(-.018,-.90)],.026,iron))
    p.append(cyl("Long Blade Socket",(0,0,-.68),.028,.16,iron,28))
    p.append(cube("Left Foot Step",(-.085,0,-.735),(.070,.032,.018),iron,.003,3))
    p.append(cube("Right Foot Step",(.085,0,-.735),(.070,.032,.018),iron,.003,3))
    rivets4(p,rv,[(.022,0,-.70),(-.022,0,-.70),(0,.022,-.70),(0,-.022,-.70)])
    a=join(p,name,(wood,iron,rv)); finish(a,[collision("UCX_"+name+"_00",(0,0,-.27),(.05,.05,.94)),collision("UCX_"+name+"_01",(0,0,-.82),(.19,.04,.28))])


def mace():
    name="KA_T01_Wep_Mace"; wood,iron,rv=setup(("Mat_T01_Mace_Wood","Mat_T01_Mace_Iron","Mat_T01_Mace_Rivets")); p=[]
    p.append(cyl("Holzschaft",(0,0,.12),.020,.66,wood,24))
    p.append(cyl("Kolbenkern",(0,0,.355),.033,.11,iron,24,scale=(1,1,1.12)))
    for i in range(6):
        a=i*math.tau/6; p.append(cube(f"Schlagrippe_{i+1}",(.041*math.cos(a),.041*math.sin(a),.355),(.012,.020,.105),iron,.002,2,rot=(0,0,a)))
    p.append(cyl("Eisenband",(0,0,.278),.023,.022,iron,24)); rivets4(p,rv,[(.023,0,.278),(-.023,0,.278),(0,.023,.278),(0,-.023,.278)])
    a=join(p,name,(wood,iron,rv)); finish(a,[collision("UCX_"+name+"_00",(0,0,.12),(.05,.05,.66)),collision("UCX_"+name+"_01",(0,0,.355),(.10,.10,.13))])


def warhammer():
    name="KA_T01_Wep_Warhammer"; wood,iron,rv=setup(("Mat_T01_Hammer_Wood","Mat_T01_Hammer_Iron","Mat_T01_Hammer_Rivets")); p=[]
    p.append(cyl("Long War Grip",(0,0,.10),.022,.76,wood,28))
    p.append(cube("Massive Warhammer Eye",(0,0,.45),(.072,.090,.105),iron,.006,5))
    p.append(cube("Large Armored Face",(0,.105,.45),(.100,.120,.125),iron,.008,5))
    spike_profile = [
        (-.045, .405), (-.120, .415), (-.220, .438),
        (-.285, .450), (-.220, .462), (-.120, .485), (-.045, .495),
    ]
    p.append(poly_prism("Long Armor Piercing Pick", spike_profile, .060, iron))
    p.append(poly_prism_xz("Top Spike",[(-.035,.50),(0,.61),(.035,.50)],.065,iron))
    p.append(cyl("Reinforced Collar",(0,0,.36),.030,.035,iron,28)); rivets4(p,rv,[(.030,0,.36),(-.030,0,.36),(0,.030,.36),(0,-.030,.36)])
    a=join(p,name,(wood,iron,rv)); finish(a,[collision("UCX_"+name+"_00",(0,0,.10),(.06,.06,.76)),collision("UCX_"+name+"_01",(0,0,.46),(.08,.42,.20))])


def spear():
    name="KA_T01_Wep_Spear"; wood,iron,rv=setup(("Mat_T01_Spear_Wood","Mat_T01_Spear_Iron","Mat_T01_Spear_Rivets")); p=[]
    p.append(cyl("Holzschaft",(0,0,.39),.016,1.68,wood,40))
    p.append(cyl("Tuellenfassung",(0,0,1.235),.024,.07,iron,24,scale=(1,1,1)))
    outline=[(0,1.27),(.055,1.35),(.075,1.43),(.045,1.51),(0,1.60),(-.045,1.51),(-.075,1.43),(-.055,1.35)]
    p.append(poly_prism_xz("Sharp Leaf Spearhead",outline,.014,iron))
    p.append(poly_prism_xz("Raised Spearhead Ridge",[(0,1.285),(.014,1.40),(0,1.59),(-.014,1.40)],.024,iron))
    rivets4(p,rv,[(.024,0,1.235),(-.024,0,1.235),(0,.024,1.235),(0,-.024,1.235)])
    a=join(p,name,(wood,iron,rv)); finish(a,[collision("UCX_"+name+"_00",(0,0,.39),(.045,.045,1.68)),collision("UCX_"+name+"_01",(0,0,1.40),(.12,.035,.25))])


def shield():
    name="KA_T01_Wep_Shield_Round"; wood,iron,rv=setup(("Mat_T01_Shield_Wood","Mat_T01_Shield_Iron","Mat_T01_Shield_Rivets")); p=[]
    # The rear grip center is the origin; the shield plane therefore sits at Y=.055.
    p.append(cyl("Holzschild",(0,.055,0),.39,.026,wood,64,(math.pi/2,0,0)))
    bpy.ops.mesh.primitive_torus_add(major_radius=.375,minor_radius=.014,major_segments=48,minor_segments=6,location=(0,.070,0),rotation=(math.pi/2,0,0)); rim=bpy.context.object; rim.name="Eisenrand"; rim.data.materials.append(iron); p.append(rim)
    bpy.ops.mesh.primitive_uv_sphere_add(segments=24,ring_count=12,radius=.105,location=(0,.092,0)); boss=bpy.context.object; boss.name="Schildbuckel"; boss.scale=(1,.45,1); bpy.ops.object.transform_apply(location=False,rotation=False,scale=True); boss.data.materials.append(iron); p.append(boss)
    p.append(cyl("Rueckgriff",(0,0,0),.017,.24,wood,24,(0,math.pi/2,0)))
    rivets4(p,rv,[(.13,.079,0),(-.13,.079,0),(0,.079,.13),(0,.079,-.13)],.005)
    a=join(p,name,(wood,iron,rv)); finish(a,[collision("UCX_"+name+"_00",(0,.055,0),(.79,.07,.79))])


def seax():
    name="KA_T01_Wep_Seax"; wood,iron,rv=setup(("Mat_T01_Seax_Wood","Mat_T01_Seax_Iron","Mat_T01_Seax_Rivets")); p=[]
    p.append(cyl("Holzgriff",(0,0,0),.017,.14,wood,30,(math.pi/2,0,0),(1,1.18,1)))
    p.append(cyl("Zwinge",(0,.077,0),.021,.014,iron,24,(math.pi/2,0,0),(1,1.18,1)))
    pts=[(.084,-.018),(.285,-.018),(.35,.010),(.35,.018),(.23,.022),(.084,.018)]
    p.append(poly_prism("Seaxklinge",pts,.006,iron)); rivets4(p,rv,[(.021,.077,0),(-.021,.077,0),(0,.077,.025),(0,.077,-.025)])
    a=join(p,name,(wood,iron,rv)); finish(a,[collision("UCX_"+name+"_00",(0,0,0),(.05,.15,.06)),collision("UCX_"+name+"_01",(0,.21,0),(.025,.28,.06))])


def axe():
    name="KA_T01_Wep_Axe"; wood,iron,rv=setup(("Mat_T01_Axe_Wood","Mat_T01_Axe_Iron","Mat_T01_Axe_Rivets")); p=[]
    p.append(cyl("Holzstiel",(0,0,.12),.022,.68,wood,24,scale=(.9,1.05,1)))
    # Form a visible axe eye from thick side cheeks around the shaft, leaving open gaps.
    p.append(cube("Axtauge_L",(-.024,.032,.38),(.014,.065,.115),iron,.004,4)); p.append(cube("Axtauge_R",(.024,.032,.38),(.014,.065,.115),iron,.004,4))
    p.append(cube("Axtauge_Ruecken",(0,.002,.38),(.062,.018,.115),iron,.004,4))
    blade_profile = [
        (.050, .325), (.125, .310), (.225, .290), (.265, .295),
        (.265, .465), (.220, .475), (.125, .450), (.050, .435),
    ]
    p.append(poly_prism("Axtschneide", blade_profile, .044, iron))
    p.append(cyl("Eisenband",(0,.022,.30),.026,.028,iron,24,scale=(.92,1.05,1))); rivets4(p,rv,[(.026,.022,.30),(-.026,.022,.30),(0,.049,.30),(0,-.005,.30)],.004)
    a=join(p,name,(wood,iron,rv)); finish(a,[collision("UCX_"+name+"_00",(0,0,.12),(.055,.065,.68)),collision("UCX_"+name+"_01",(0,.10,.38),(.075,.28,.15))])


BUILDERS = {
    "Sickle": sickle,
    "Shovel": shovel,
    "Mace": mace,
    "Warhammer": warhammer,
    "Spear": spear,
    "Shield": shield,
    "Seax": seax,
    "Axe": axe,
}

if __name__ == "__main__":
    arguments = sys.argv[sys.argv.index("--") + 1:] if "--" in sys.argv else []
    selected = arguments or list(BUILDERS)
    for key in selected:
        BUILDERS[key]()
    print("SELECTED_PLAN_ASSETS_CREATED " + ",".join(selected))

