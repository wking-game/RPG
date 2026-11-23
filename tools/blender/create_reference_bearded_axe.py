import bpy
import math
import os
from mathutils import Vector

ROOT = r"D:\gamer\gamer"
NAME = "KA_T01_Wep_BeardedAxe_Reference"
BLEND_PATH = os.path.join(ROOT, "Assets", "Source", NAME + ".blend")
FBX_PATH = os.path.join(ROOT, "Assets", "Items", NAME + ".fbx")
PREVIEW_PATH = os.path.join(ROOT, "Assets", "Previews", "Tier1", NAME + ".png")


def clear_scene():
    bpy.ops.object.select_all(action="SELECT")
    bpy.ops.object.delete(use_global=False)
    for blocks in (bpy.data.meshes, bpy.data.curves, bpy.data.materials, bpy.data.cameras, bpy.data.lights):
        for block in list(blocks):
            if block.users == 0:
                blocks.remove(block)


def wood_material():
    material = bpy.data.materials.new("Mat_BeardedAxe_AshWood")
    material.use_nodes = True
    nodes = material.node_tree.nodes
    links = material.node_tree.links
    shader = nodes.get("Principled BSDF")
    shader.inputs["Roughness"].default_value = 0.58
    shader.inputs["Specular IOR Level"].default_value = 0.20
    texture = nodes.new("ShaderNodeTexNoise")
    texture.noise_dimensions = "4D"
    texture.inputs["Scale"].default_value = 5.2
    texture.inputs["Detail"].default_value = 7.0
    texture.inputs["Roughness"].default_value = 0.72
    texture.inputs["Distortion"].default_value = 1.8
    texture.inputs["W"].default_value = 0.36
    mapping = nodes.new("ShaderNodeMapping")
    mapping.inputs["Scale"].default_value = (0.22, 5.0, 5.0)
    texcoord = nodes.new("ShaderNodeTexCoord")
    ramp = nodes.new("ShaderNodeValToRGB")
    ramp.color_ramp.elements[0].position = 0.24
    ramp.color_ramp.elements[0].color = (0.008, 0.0025, 0.001, 1)
    mid = ramp.color_ramp.elements.new(0.50)
    mid.color = (0.070, 0.018, 0.004, 1)
    ramp.color_ramp.elements[1].position = 0.76
    ramp.color_ramp.elements[1].color = (0.22, 0.070, 0.014, 1)
    bump = nodes.new("ShaderNodeBump")
    bump.inputs["Strength"].default_value = 0.22
    bump.inputs["Distance"].default_value = 0.012
    links.new(texcoord.outputs["Generated"], mapping.inputs["Vector"])
    links.new(mapping.outputs["Vector"], texture.inputs["Vector"])
    links.new(texture.outputs["Fac"], ramp.inputs["Fac"])
    links.new(ramp.outputs["Color"], shader.inputs["Base Color"])
    links.new(texture.outputs["Fac"], bump.inputs["Height"])
    links.new(bump.outputs["Normal"], shader.inputs["Normal"])
    return material


def iron_material(name, edge=False):
    material = bpy.data.materials.new(name)
    material.use_nodes = True
    nodes = material.node_tree.nodes
    links = material.node_tree.links
    shader = nodes.get("Principled BSDF")
    shader.inputs["Metallic"].default_value = 0.92
    shader.inputs["Roughness"].default_value = 0.18 if edge else 0.43
    noise = nodes.new("ShaderNodeTexNoise")
    noise.inputs["Scale"].default_value = 38.0 if edge else 11.0
    noise.inputs["Detail"].default_value = 8.0
    noise.inputs["Roughness"].default_value = 0.82
    ramp = nodes.new("ShaderNodeValToRGB")
    if edge:
        ramp.color_ramp.elements[0].color = (0.10, 0.12, 0.13, 1)
        ramp.color_ramp.elements[1].color = (0.38, 0.42, 0.43, 1)
    else:
        ramp.color_ramp.elements[0].color = (0.018, 0.022, 0.024, 1)
        ramp.color_ramp.elements[1].color = (0.075, 0.085, 0.085, 1)
    bump = nodes.new("ShaderNodeBump")
    bump.inputs["Strength"].default_value = 0.38 if not edge else 0.10
    bump.inputs["Distance"].default_value = 0.006
    links.new(noise.outputs["Fac"], ramp.inputs["Fac"])
    links.new(ramp.outputs["Color"], shader.inputs["Base Color"])
    links.new(noise.outputs["Fac"], bump.inputs["Height"])
    links.new(bump.outputs["Normal"], shader.inputs["Normal"])
    return material


def leather_material():
    material = bpy.data.materials.new("Mat_BeardedAxe_DarkLeather")
    material.use_nodes = True
    nodes = material.node_tree.nodes
    links = material.node_tree.links
    shader = nodes.get("Principled BSDF")
    shader.inputs["Roughness"].default_value = 0.69
    noise = nodes.new("ShaderNodeTexNoise")
    noise.inputs["Scale"].default_value = 28.0
    noise.inputs["Detail"].default_value = 5.0
    ramp = nodes.new("ShaderNodeValToRGB")
    ramp.color_ramp.elements[0].color = (0.018, 0.006, 0.003, 1)
    ramp.color_ramp.elements[1].color = (0.055, 0.012, 0.004, 1)
    bump = nodes.new("ShaderNodeBump")
    bump.inputs["Strength"].default_value = 0.30
    bump.inputs["Distance"].default_value = 0.004
    links.new(noise.outputs["Fac"], ramp.inputs["Fac"])
    links.new(ramp.outputs["Color"], shader.inputs["Base Color"])
    links.new(noise.outputs["Fac"], bump.inputs["Height"])
    links.new(bump.outputs["Normal"], shader.inputs["Normal"])
    return material


def handle_center(x):
    t = (x + 0.70) / 1.46
    z = 0.025 * math.sin(t * math.pi) - 0.018 * math.sin(t * math.pi * 2.0)
    y = 0.010 * math.sin(t * math.pi * 1.35)
    return Vector((x, y, z))


def create_handle(material):
    rings = 38
    sides = 14
    vertices = []
    for ring in range(rings):
        x = -0.72 + ring * (1.50 / (rings - 1))
        center = handle_center(x)
        t = ring / (rings - 1)
        grip = math.exp(-((t - 0.68) / 0.18) ** 2)
        end = math.exp(-((t - 0.98) / 0.10) ** 2)
        radius_y = 0.027 + 0.006 * grip + 0.004 * end
        radius_z = 0.034 + 0.006 * grip + 0.005 * end
        for side in range(sides):
            angle = side * math.tau / sides
            vertices.append((x, center.y + math.cos(angle) * radius_y, center.z + math.sin(angle) * radius_z))
    faces = []
    faces.append(tuple(range(sides - 1, -1, -1)))
    for ring in range(rings - 1):
        for side in range(sides):
            next_side = (side + 1) % sides
            a = ring * sides + side
            b = ring * sides + next_side
            c = (ring + 1) * sides + next_side
            d = (ring + 1) * sides + side
            faces.append((a, b, c, d))
    faces.append(tuple((rings - 1) * sides + side for side in range(sides)))
    mesh = bpy.data.meshes.new("CurvedAshHandle_Mesh")
    mesh.from_pydata(vertices, [], faces)
    mesh.update()
    obj = bpy.data.objects.new("Curved Ash Handle", mesh)
    bpy.context.collection.objects.link(obj)
    obj.data.materials.append(material)
    bevel = obj.modifiers.new("Hand Finished Edges", "BEVEL")
    bevel.width = 0.0025
    bevel.segments = 3
    bpy.context.view_layer.objects.active = obj
    bpy.ops.object.modifier_apply(modifier=bevel.name)
    for polygon in obj.data.polygons:
        polygon.use_smooth = True
    return obj


def prism_xz(name, points, thickness, material, bevel=0.003):
    count = len(points)
    vertices = [(x, -thickness / 2, z) for x, z in points] + [(x, thickness / 2, z) for x, z in points]
    faces = [tuple(range(count - 1, -1, -1)), tuple(range(count, count * 2))]
    for index in range(count):
        nxt = (index + 1) % count
        faces.append((index, nxt, count + nxt, count + index))
    mesh = bpy.data.meshes.new(name + "_Mesh")
    mesh.from_pydata(vertices, [], faces)
    mesh.update()
    obj = bpy.data.objects.new(name, mesh)
    bpy.context.collection.objects.link(obj)
    obj.data.materials.append(material)
    modifier = obj.modifiers.new("Forged Edge Softening", "BEVEL")
    modifier.width = bevel
    modifier.segments = 3
    bpy.context.view_layer.objects.active = obj
    bpy.ops.object.modifier_apply(modifier=modifier.name)
    return obj


def create_head(iron, polished):
    body_points = [
        (-0.735, 0.065), (-0.625, 0.060), (-0.590, 0.005), (-0.600, -0.055),
        (-0.625, -0.125), (-0.600, -0.205), (-0.500, -0.245), (-0.385, -0.225),
        (-0.300, -0.185), (-0.370, -0.105), (-0.455, -0.050), (-0.500, 0.015),
        (-0.585, 0.075),
    ]
    body = prism_xz("Forged Bearded Axe Head", body_points, 0.060, iron, 0.004)
    edge_points = [(-0.600, -0.205), (-0.500, -0.245), (-0.385, -0.225), (-0.300, -0.185),
                   (-0.323, -0.158), (-0.405, -0.190), (-0.500, -0.205), (-0.585, -0.178)]
    edge = prism_xz("Polished Cutting Edge", edge_points, 0.064, polished, 0.0015)
    return [body, edge]


def create_eye_and_wedge(iron, wood):
    parts = []
    bpy.ops.mesh.primitive_torus_add(major_radius=0.039, minor_radius=0.010, major_segments=32, minor_segments=10,
                                    location=(-0.655, 0, 0.026), rotation=(0, math.pi / 2, 0))
    collar = bpy.context.object
    collar.name = "Forged Eye Collar"
    collar.scale.z = 1.15
    bpy.ops.object.transform_apply(location=False, rotation=False, scale=True)
    collar.data.materials.append(iron)
    parts.append(collar)
    bpy.ops.mesh.primitive_cylinder_add(vertices=16, radius=0.031, depth=0.030,
                                       location=(-0.742, 0, 0.022), rotation=(0, math.pi / 2, 0))
    wedge = bpy.context.object
    wedge.name = "Visible Hardwood Wedge"
    wedge.scale = (1.0, 0.92, 1.12)
    bpy.ops.object.transform_apply(location=False, rotation=False, scale=True)
    wedge.data.materials.append(wood)
    bevel = wedge.modifiers.new("Wedge Bevel", "BEVEL")
    bevel.width = 0.003
    bevel.segments = 3
    bpy.context.view_layer.objects.active = wedge
    bpy.ops.object.modifier_apply(modifier=bevel.name)
    parts.append(wedge)
    return parts


def create_leather_wrap(material):
    parts = []
    center_start = 0.12
    center_end = 0.43
    turns = 28
    points = turns * 10
    curve_data = bpy.data.curves.new("LeatherWrap_Curve", "CURVE")
    curve_data.dimensions = "3D"
    curve_data.bevel_depth = 0.0032
    curve_data.bevel_resolution = 3
    curve_data.resolution_u = 3
    spline = curve_data.splines.new("POLY")
    spline.points.add(points - 1)
    for index in range(points):
        t = index / (points - 1)
        x = center_start + (center_end - center_start) * t
        center = handle_center(x)
        angle = t * turns * math.tau
        y = center.y + math.cos(angle) * 0.035
        z = center.z + math.sin(angle) * 0.042
        spline.points[index].co = (x, y, z, 1)
    wrap = bpy.data.objects.new("Tight Leather Grip Wrap", curve_data)
    bpy.context.collection.objects.link(wrap)
    wrap.data.materials.append(material)
    parts.append(wrap)
    for x in (center_start, center_end):
        center = handle_center(x)
        bpy.ops.mesh.primitive_torus_add(major_radius=0.038, minor_radius=0.0045, major_segments=32, minor_segments=8,
                                        location=center, rotation=(0, math.pi / 2, 0))
        band = bpy.context.object
        band.name = "Leather Binding Collar"
        band.scale.z = 1.14
        bpy.ops.object.transform_apply(location=False, rotation=False, scale=True)
        band.data.materials.append(material)
        parts.append(band)
    return parts


def join_asset(parts, materials):
    bpy.ops.object.select_all(action="DESELECT")
    for part in parts:
        part.select_set(True)
    bpy.context.view_layer.objects.active = parts[0]
    bpy.ops.object.convert(target="MESH")
    bpy.ops.object.join()
    asset = bpy.context.object
    asset.name = NAME
    for material in materials:
        if material.name not in [slot.name for slot in asset.data.materials]:
            asset.data.materials.append(material)
    bpy.ops.object.transform_apply(location=True, rotation=True, scale=True)
    bpy.ops.object.mode_set(mode="EDIT")
    bpy.ops.mesh.select_all(action="SELECT")
    bpy.ops.uv.smart_project(angle_limit=math.radians(62), island_margin=0.018)
    bpy.ops.mesh.normals_make_consistent(inside=False)
    bpy.ops.object.mode_set(mode="OBJECT")
    return asset


def collision_box(name, location, dimensions, rotation=(0, 0, 0)):
    bpy.ops.mesh.primitive_cube_add(location=location, rotation=rotation)
    obj = bpy.context.object
    obj.name = name
    obj.dimensions = dimensions
    bpy.ops.object.transform_apply(location=False, rotation=False, scale=True)
    obj.display_type = "WIRE"
    obj.hide_render = True
    return obj


def look_at(obj, target):
    obj.rotation_euler = (Vector(target) - obj.location).to_track_quat("-Z", "Y").to_euler()


def create_preview(asset):
    collection = bpy.data.collections.new("PREVIEW_RIG")
    bpy.context.scene.collection.children.link(collection)
    camera_data = bpy.data.cameras.new("Preview_Camera")
    camera_data.lens = 62
    camera = bpy.data.objects.new("Preview_Camera", camera_data)
    collection.objects.link(camera)
    camera.location = (1.40, -2.15, 0.82)
    look_at(camera, (0.02, 0, -0.05))
    bpy.context.scene.camera = camera
    for name, location, energy, size, color in (
        ("Preview_Key", (-0.8, -1.0, 1.3), 280, 1.2, (1.0, 0.72, 0.50)),
        ("Preview_Fill", (1.2, -0.4, 0.4), 95, 1.4, (0.48, 0.65, 1.0)),
        ("Preview_Rim", (-0.5, 1.0, 0.9), 190, 1.0, (0.65, 0.78, 1.0)),
    ):
        data = bpy.data.lights.new(name, "AREA")
        data.energy = energy
        data.shape = "DISK"
        data.size = size
        data.color = color
        light = bpy.data.objects.new(name, data)
        collection.objects.link(light)
        light.location = location
        look_at(light, (0, 0, -0.05))
    scene = bpy.context.scene
    scene.render.engine = "BLENDER_EEVEE"
    scene.render.resolution_x = 1000
    scene.render.resolution_y = 600
    scene.render.resolution_percentage = 100
    scene.render.image_settings.file_format = "PNG"
    scene.render.filepath = PREVIEW_PATH
    scene.world.use_nodes = True
    background = scene.world.node_tree.nodes.get("Background")
    background.inputs["Color"].default_value = (0.008, 0.010, 0.014, 1)
    background.inputs["Strength"].default_value = 0.12
    scene.view_settings.look = "AgX - Medium High Contrast"
    scene.view_settings.exposure = -0.65


def main():
    clear_scene()
    scene = bpy.context.scene
    scene.unit_settings.system = "METRIC"
    scene.unit_settings.scale_length = 1.0
    wood = wood_material()
    iron = iron_material("Mat_BeardedAxe_ForgedIron")
    polished = iron_material("Mat_BeardedAxe_PolishedEdge", edge=True)
    leather = leather_material()
    parts = [create_handle(wood)]
    parts.extend(create_head(iron, polished))
    parts.extend(create_eye_and_wedge(iron, wood))
    parts.extend(create_leather_wrap(leather))
    asset = join_asset(parts, (wood, iron, polished, leather))
    collisions = [
        collision_box("UCX_" + NAME + "_00", (0.02, 0, 0.01), (1.55, 0.09, 0.11)),
        collision_box("UCX_" + NAME + "_01", (-0.50, 0, -0.09), (0.46, 0.10, 0.34), rotation=(0, math.radians(-8), 0)),
    ]
    create_preview(asset)
    bpy.ops.object.select_all(action="DESELECT")
    for obj in [asset] + collisions:
        obj.select_set(True)
    bpy.context.view_layer.objects.active = asset
    bpy.ops.wm.save_as_mainfile(filepath=BLEND_PATH)
    bpy.ops.export_scene.fbx(filepath=FBX_PATH, use_selection=True, object_types={"MESH"}, apply_unit_scale=True,
                             apply_scale_options="FBX_SCALE_ALL", axis_forward="-Y", axis_up="Z",
                             mesh_smooth_type="FACE", add_leaf_bones=False, bake_anim=False)
    bpy.ops.render.render(write_still=True)
    triangles = sum(len(face.vertices) - 2 for face in asset.data.polygons)
    print(f"REFERENCE_AXE_COMPLETE {NAME} triangles={triangles} materials={len(asset.data.materials)}")


if __name__ == "__main__":
    main()
