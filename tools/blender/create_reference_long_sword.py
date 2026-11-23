import bpy
import math
import os
from mathutils import Vector

ROOT = r"D:\gamer\gamer"
NAME = "KA_T01_Wep_LongSword_BronzeGuard"
BLEND = os.path.join(ROOT, "Assets", "Source", NAME + ".blend")
FBX = os.path.join(ROOT, "Assets", "Items", NAME + ".fbx")
PREVIEW = os.path.join(ROOT, "Assets", "Previews", "Tier1", NAME + ".png")


def clear():
    bpy.ops.object.select_all(action="SELECT")
    bpy.ops.object.delete(use_global=False)
    for collection in list(bpy.data.collections):
        if collection.name != "Collection":
            bpy.data.collections.remove(collection)
    for blocks in (bpy.data.meshes, bpy.data.curves, bpy.data.materials, bpy.data.cameras, bpy.data.lights):
        for block in list(blocks):
            if block.users == 0:
                blocks.remove(block)


def material(name, dark, light, metallic, roughness, scale=8.0, bump_strength=0.22):
    mat = bpy.data.materials.new(name)
    mat.use_nodes = True
    nodes = mat.node_tree.nodes
    links = mat.node_tree.links
    shader = nodes.get("Principled BSDF")
    shader.inputs["Metallic"].default_value = metallic
    shader.inputs["Roughness"].default_value = roughness
    noise = nodes.new("ShaderNodeTexNoise")
    noise.inputs["Scale"].default_value = scale
    noise.inputs["Detail"].default_value = 7.0
    noise.inputs["Roughness"].default_value = 0.78
    ramp = nodes.new("ShaderNodeValToRGB")
    ramp.color_ramp.elements[0].color = (*dark, 1)
    ramp.color_ramp.elements[1].color = (*light, 1)
    bump = nodes.new("ShaderNodeBump")
    bump.inputs["Strength"].default_value = bump_strength
    bump.inputs["Distance"].default_value = 0.004
    links.new(noise.outputs["Fac"], ramp.inputs["Fac"])
    links.new(ramp.outputs["Color"], shader.inputs["Base Color"])
    links.new(noise.outputs["Fac"], bump.inputs["Height"])
    links.new(bump.outputs["Normal"], shader.inputs["Normal"])
    return mat


def bevel_object(obj, width=0.0025, segments=3):
    modifier = obj.modifiers.new("Edge Finishing", "BEVEL")
    modifier.width = width
    modifier.segments = segments
    bpy.context.view_layer.objects.active = obj
    bpy.ops.object.modifier_apply(modifier=modifier.name)
    return obj


def create_blade(steel, edge):
    stations = [(-0.68, 0.000), (-0.655, 0.060), (-0.57, 0.075), (0.38, 0.070), (0.49, 0.050), (0.56, 0.000)]
    vertices = []
    ring_size = 8
    for x, half_width in stations:
        thickness = 0.012 if half_width else 0.001
        w = max(half_width, 0.001)
        vertices.extend([
            (x, 0.000, w), (x, -thickness * 0.55, w * 0.58),
            (x, -thickness, 0), (x, -thickness * 0.55, -w * 0.58),
            (x, 0.000, -w), (x, thickness * 0.55, -w * 0.58),
            (x, thickness, 0), (x, thickness * 0.55, w * 0.58),
        ])
    faces = []
    for station in range(len(stations) - 1):
        for side in range(ring_size):
            nxt = (side + 1) % ring_size
            a = station * ring_size + side
            b = station * ring_size + nxt
            c = (station + 1) * ring_size + nxt
            d = (station + 1) * ring_size + side
            faces.append((a, b, c, d))
    faces += [tuple(range(ring_size - 1, -1, -1)), tuple((len(stations)-1)*ring_size+i for i in range(ring_size))]
    mesh = bpy.data.meshes.new("LongDiamondBlade_Mesh")
    mesh.from_pydata(vertices, [], faces)
    mesh.update()
    blade = bpy.data.objects.new("Dark Two Edged Blade", mesh)
    bpy.context.collection.objects.link(blade)
    blade.data.materials.append(steel)
    bevel_object(blade, 0.0012, 2)
    upper = prism("Bright Upper Edge", [(-0.655,0.060),(0.38,0.070),(0.49,0.050),(0.56,0),(0.49,0.036),(0.37,0.052),(-0.64,0.045)], 0.026, edge, 0.0008)
    lower = prism("Bright Lower Edge", [(-0.655,-0.060),(-0.64,-0.045),(0.37,-0.052),(0.49,-0.036),(0.56,0),(0.49,-0.050),(0.38,-0.070)], 0.026, edge, 0.0008)
    return [blade, upper, lower]


def prism(name, points_xz, thickness, mat, bevel=0.002):
    n = len(points_xz)
    verts = [(x,-thickness/2,z) for x,z in points_xz] + [(x,thickness/2,z) for x,z in points_xz]
    faces = [tuple(range(n-1,-1,-1)), tuple(range(n,n*2))]
    for i in range(n):
        j=(i+1)%n; faces.append((i,j,n+j,n+i))
    mesh=bpy.data.meshes.new(name+"_Mesh"); mesh.from_pydata(verts,[],faces); mesh.update()
    obj=bpy.data.objects.new(name,mesh); bpy.context.collection.objects.link(obj); obj.data.materials.append(mat)
    return bevel_object(obj,bevel,3)


def cylinder(name, location, radius, depth, mat, vertices=32, rotation=(0,math.pi/2,0), scale=(1,1,1)):
    bpy.ops.mesh.primitive_cylinder_add(vertices=vertices, radius=radius, depth=depth, location=location, rotation=rotation)
    obj=bpy.context.object; obj.name=name; obj.scale=scale
    bpy.ops.object.transform_apply(location=False,rotation=False,scale=True); obj.data.materials.append(mat)
    return bevel_object(obj,min(radius*0.12,0.003),3)


def create_guard(bronze, steel, leather):
    parts=[]
    parts.append(cylinder("Guard Core",(-0.72,0,0),0.088,0.030,bronze,48,scale=(1,0.92,1)))
    bpy.ops.mesh.primitive_torus_add(major_radius=0.078,minor_radius=0.008,major_segments=48,minor_segments=10,location=(-0.72,0,0),rotation=(math.pi/2,0,0))
    ring=bpy.context.object; ring.name="Bronze Guard Ring"; ring.data.materials.append(bronze); parts.append(ring)
    upper=[(-.735,.055),(-.765,.105),(-.785,.265),(-.755,.275),(-.700,.105),(-.680,.060)]
    lower=[(x,-z) for x,z in upper]
    parts += [prism("Upper Swept Quillon",upper,.026,bronze,.003),prism("Lower Swept Quillon",lower,.026,bronze,.003)]
    parts += [prism("Blade Shoulder",[(-.69,.073),(-.61,.073),(-.57,.052),(-.61,-.052),(-.69,-.073)],.036,steel,.002)]
    plate=prism("Original Riveted Guard Plate",[(-.755,.060),(-.688,.060),(-.665,.020),(-.665,-.060),(-.755,-.060)],.039,bronze,.002)
    parts.append(plate)
    for row in range(3):
        for col in range(2):
            x=-.733+col*.032; z=.036-row*.036
            bpy.ops.mesh.primitive_uv_sphere_add(segments=16,ring_count=8,radius=.006,location=(x,-.024,z))
            riv=bpy.context.object; riv.name="Guard Rivet"; riv.scale=(1,.45,1); bpy.ops.object.transform_apply(location=False,rotation=False,scale=True); riv.data.materials.append(steel); parts.append(riv)
    return parts


def create_grip(leather, bronze, steel):
    parts=[]
    parts.append(cylinder("Tang Collar",(-.805,0,0),.043,.040,steel,32))
    parts.append(cylinder("Black Leather Grip",(-1.005,0,0),.048,.38,leather,40,scale=(1,0.78,1)))
    for index in range(3):
        x=-.86-index*.145
        bpy.ops.mesh.primitive_torus_add(major_radius=.047,minor_radius=.0025,major_segments=32,minor_segments=7,location=(x,0,0),rotation=(0,math.pi/2,0))
        wrap=bpy.context.object; wrap.name="Subtle Grip Seam"; wrap.scale.z=1.20; bpy.ops.object.transform_apply(location=False,rotation=False,scale=True); wrap.data.materials.append(leather); parts.append(wrap)
    parts.append(cylinder("Pommel Neck",(-1.225,0,0),.040,.045,steel,32))
    parts.append(cylinder("Flared Pommel",(-1.285,0,0),.073,.105,steel,40,scale=(1,0.82,1.15)))
    parts.append(cylinder("Bronze Pommel Band",(-1.235,0,0),.060,.015,bronze,40,scale=(1,.82,1.1)))
    parts.append(cylinder("Pommel End Cap",(-1.345,0,0),.076,.022,steel,40,scale=(1,.85,1.15)))
    return parts


def join(parts, materials):
    bpy.ops.object.select_all(action="DESELECT")
    for part in parts: part.select_set(True)
    bpy.context.view_layer.objects.active=parts[0]; bpy.ops.object.join(); asset=bpy.context.object; asset.name=NAME
    for mat in materials:
        if mat.name not in [slot.name for slot in asset.data.materials]: asset.data.materials.append(mat)
    bpy.ops.object.transform_apply(location=True,rotation=True,scale=True)
    bpy.ops.object.mode_set(mode="EDIT"); bpy.ops.mesh.select_all(action="SELECT")
    bpy.ops.uv.smart_project(angle_limit=math.radians(64),island_margin=.018); bpy.ops.mesh.normals_make_consistent(inside=False)
    bpy.ops.object.mode_set(mode="OBJECT")
    return asset


def collision(name,loc,dims):
    bpy.ops.mesh.primitive_cube_add(location=loc); obj=bpy.context.object; obj.name=name; obj.dimensions=dims
    bpy.ops.object.transform_apply(location=False,rotation=False,scale=True); obj.display_type="WIRE"; obj.hide_render=True; return obj


def look_at(obj,target): obj.rotation_euler=(Vector(target)-obj.location).to_track_quat("-Z","Y").to_euler()


def preview():
    collection=bpy.data.collections.new("PREVIEW_RIG"); bpy.context.scene.collection.children.link(collection)
    camera_data=bpy.data.cameras.new("Preview_Camera"); camera_data.lens=52
    camera=bpy.data.objects.new("Preview_Camera",camera_data); collection.objects.link(camera); camera.location=(-.38,-3.65,.82); look_at(camera,(-.40,0,0)); bpy.context.scene.camera=camera
    for name,loc,energy,size,color in (("Key",(-.2,-1.4,1.4),360,1.4,(1,.72,.5)),("Fill",(-1.7,-.5,.3),120,1.3,(.5,.65,1)),("Rim",(.2,1.0,.8),250,1.1,(.65,.8,1))):
        data=bpy.data.lights.new(name,"AREA"); data.energy=energy; data.shape="DISK"; data.size=size; data.color=color
        light=bpy.data.objects.new(name,data); collection.objects.link(light); light.location=loc; look_at(light,(-.4,0,0))
    scene=bpy.context.scene; scene.render.engine="BLENDER_EEVEE"; scene.render.resolution_x=1100; scene.render.resolution_y=650; scene.render.resolution_percentage=100
    scene.render.image_settings.file_format="PNG"; scene.render.filepath=PREVIEW; scene.world.use_nodes=True
    bg=scene.world.node_tree.nodes.get("Background"); bg.inputs["Color"].default_value=(.008,.010,.014,1); bg.inputs["Strength"].default_value=.14
    scene.view_settings.look="AgX - Medium High Contrast"; scene.view_settings.exposure=-.45


def main():
    clear(); scene=bpy.context.scene; scene.unit_settings.system="METRIC"; scene.unit_settings.scale_length=1
    steel=material("Mat_LongSword_BlackenedSteel",(.008,.010,.012),(.065,.075,.08),.95,.32,10,.35)
    edge=material("Mat_LongSword_PolishedEdges",(.12,.14,.15),(.42,.47,.49),.98,.16,35,.10)
    bronze=material("Mat_LongSword_AgedBronze",(.045,.018,.003),(.24,.095,.012),.88,.34,12,.30)
    leather=material("Mat_LongSword_BlackLeather",(.004,.003,.002),(.025,.018,.012),.0,.72,30,.30)
    parts=create_blade(steel,edge)+create_guard(bronze,steel,leather)+create_grip(leather,bronze,steel)
    asset=join(parts,(steel,edge,bronze,leather))
    cols=[collision("UCX_"+NAME+"_00",(-.05,0,0),(1.30,.06,.17)),collision("UCX_"+NAME+"_01",(-1.0,0,0),(.70,.17,.20))]
    preview(); bpy.ops.object.select_all(action="DESELECT")
    for obj in [asset]+cols: obj.select_set(True)
    bpy.context.view_layer.objects.active=asset; bpy.ops.wm.save_as_mainfile(filepath=BLEND)
    bpy.ops.export_scene.fbx(filepath=FBX,use_selection=True,object_types={"MESH"},apply_unit_scale=True,apply_scale_options="FBX_SCALE_ALL",axis_forward="-Y",axis_up="Z",mesh_smooth_type="FACE",add_leaf_bones=False,bake_anim=False)
    bpy.ops.render.render(write_still=True)
    tris=sum(len(p.vertices)-2 for p in asset.data.polygons)
    print(f"REFERENCE_LONG_SWORD_COMPLETE {NAME} triangles={tris} materials={len(asset.data.materials)}")


if __name__=="__main__": main()
