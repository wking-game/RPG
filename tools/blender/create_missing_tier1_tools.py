import bpy
import math
import os
import sys


ROOT = r"D:\gamer\gamer"
SOURCE_DIRECTORY = os.path.join(ROOT, "Assets", "Source")
EXPORT_DIRECTORY = os.path.join(ROOT, "Assets", "Items")


def clear_scene():
    bpy.ops.object.select_all(action="SELECT")
    bpy.ops.object.delete(use_global=False)
    for collection in (bpy.data.meshes, bpy.data.curves, bpy.data.materials):
        for block in list(collection):
            if block.users == 0:
                collection.remove(block)


def create_material(name, color, metallic=0.0, roughness=0.55):
    material = bpy.data.materials.new(name)
    material.use_nodes = True
    shader = material.node_tree.nodes.get("Principled BSDF")
    shader.inputs["Base Color"].default_value = (*color, 1.0)
    shader.inputs["Metallic"].default_value = metallic
    shader.inputs["Roughness"].default_value = roughness
    noise = material.node_tree.nodes.new("ShaderNodeTexNoise")
    noise.inputs["Scale"].default_value = 5.0
    noise.inputs["Detail"].default_value = 2.5
    ramp = material.node_tree.nodes.new("ShaderNodeValToRGB")
    ramp.color_ramp.elements[0].color = (*(channel * 0.58 for channel in color), 1.0)
    ramp.color_ramp.elements[1].color = (*(min(channel * 1.35, 1.0) for channel in color), 1.0)
    material.node_tree.links.new(noise.outputs["Fac"], ramp.inputs["Fac"])
    material.node_tree.links.new(ramp.outputs["Color"], shader.inputs["Base Color"])
    return material


def add_cylinder(name, location, radius, depth, material, rotation=(0, 0, 0), vertices=32, scale=(1, 1, 1)):
    bpy.ops.mesh.primitive_cylinder_add(vertices=vertices, radius=radius, depth=depth, location=location, rotation=rotation)
    obj = bpy.context.object
    obj.name = name
    obj.scale = scale
    bpy.ops.object.transform_apply(location=False, rotation=False, scale=True)
    obj.data.materials.append(material)
    bevel = obj.modifiers.new("Worn Edge Bevel", "BEVEL")
    bevel.width = min(radius * 0.12, 0.0025)
    bevel.segments = 3
    bpy.context.view_layer.objects.active = obj
    bpy.ops.object.modifier_apply(modifier=bevel.name)
    return obj


def add_box(name, location, dimensions, material, rotation=(0, 0, 0), bevel_width=0.003, bevel_segments=5):
    bpy.ops.mesh.primitive_cube_add(location=location, rotation=rotation)
    obj = bpy.context.object
    obj.name = name
    obj.dimensions = dimensions
    bpy.ops.object.transform_apply(location=False, rotation=False, scale=True)
    obj.data.materials.append(material)
    bevel = obj.modifiers.new("Forged Edge Bevel", "BEVEL")
    bevel.width = bevel_width
    bevel.segments = bevel_segments
    bpy.context.view_layer.objects.active = obj
    bpy.ops.object.modifier_apply(modifier=bevel.name)
    return obj


def add_rivet(name, location, material, radius=0.004):
    bpy.ops.mesh.primitive_uv_sphere_add(segments=16, ring_count=10, radius=radius, location=location)
    obj = bpy.context.object
    obj.name = name
    obj.scale = (1.0, 1.0, 0.62)
    bpy.ops.object.transform_apply(location=False, rotation=False, scale=True)
    obj.data.materials.append(material)
    return obj


def add_prism(name, points_yz, thickness, material, bevel_width=0.002):
    vertex_count = len(points_yz)
    vertices = [(-thickness / 2, y, z) for y, z in points_yz]
    vertices += [(thickness / 2, y, z) for y, z in points_yz]
    faces = [tuple(range(vertex_count - 1, -1, -1)), tuple(range(vertex_count, vertex_count * 2))]
    for index in range(vertex_count):
        next_index = (index + 1) % vertex_count
        faces.append((index, next_index, vertex_count + next_index, vertex_count + index))
    mesh = bpy.data.meshes.new(name + "_Mesh")
    mesh.from_pydata(vertices, [], faces)
    mesh.update()
    obj = bpy.data.objects.new(name, mesh)
    bpy.context.collection.objects.link(obj)
    obj.data.materials.append(material)
    bevel = obj.modifiers.new("Forged Edge Bevel", "BEVEL")
    bevel.width = bevel_width
    bevel.segments = 4
    bpy.context.view_layer.objects.active = obj
    bpy.ops.object.modifier_apply(modifier=bevel.name)
    return obj


def join_visible_parts(parts, asset_name, materials):
    bpy.ops.object.select_all(action="DESELECT")
    for part in parts:
        part.select_set(True)
    bpy.context.view_layer.objects.active = parts[0]
    bpy.ops.object.join()
    asset = bpy.context.object
    asset.name = asset_name
    for material in materials:
        if material.name not in [slot.name for slot in asset.data.materials]:
            asset.data.materials.append(material)
    bpy.ops.object.transform_apply(location=True, rotation=True, scale=True)
    bpy.ops.object.mode_set(mode="EDIT")
    bpy.ops.mesh.select_all(action="SELECT")
    bpy.ops.uv.smart_project(angle_limit=math.radians(66.0), island_margin=0.02)
    bpy.ops.mesh.normals_make_consistent(inside=False)
    bpy.ops.object.mode_set(mode="OBJECT")
    for polygon in asset.data.polygons:
        polygon.use_smooth = True
    return asset


def add_collision(name, location, dimensions, rotation=(0, 0, 0)):
    bpy.ops.mesh.primitive_cube_add(location=location, rotation=rotation)
    obj = bpy.context.object
    obj.name = name
    obj.dimensions = dimensions
    bpy.ops.object.transform_apply(location=False, rotation=False, scale=True)
    obj.display_type = "WIRE"
    obj.hide_render = True
    return obj


def begin_asset(material_prefix, third_kind="Rivets"):
    clear_scene()
    scene = bpy.context.scene
    scene.unit_settings.system = "METRIC"
    scene.unit_settings.scale_length = 1.0
    scene.render.engine = "BLENDER_EEVEE"
    scene.world.color = (0.018, 0.022, 0.026)
    wood = create_material(f"Mat_T01_{material_prefix}_Wood", (0.29, 0.10, 0.03), 0.0, 0.6)
    iron = create_material(f"Mat_T01_{material_prefix}_Iron", (0.11, 0.14, 0.15), 0.82, 0.31)
    third_color = (0.20, 0.22, 0.22) if third_kind == "Rivets" else (0.18, 0.08, 0.025)
    third = create_material(f"Mat_T01_{material_prefix}_{third_kind}", third_color, 0.75 if third_kind == "Rivets" else 0.0, 0.28)
    return wood, iron, third


def finish_asset(asset, collisions):
    bpy.ops.object.select_all(action="DESELECT")
    for obj in [asset] + collisions:
        obj.select_set(True)
    bpy.context.view_layer.objects.active = asset
    blend_path = os.path.join(SOURCE_DIRECTORY, asset.name + ".blend")
    fbx_path = os.path.join(EXPORT_DIRECTORY, asset.name + ".fbx")
    bpy.ops.wm.save_as_mainfile(filepath=blend_path)
    bpy.ops.export_scene.fbx(
        filepath=fbx_path,
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
    triangles = sum(len(polygon.vertices) - 2 for polygon in asset.data.polygons)
    print(f"MISSING_TOOL_OK {asset.name} triangles={triangles} materials={len(asset.data.materials)}")


def add_four_rivets(parts, material, center, x_offset=0.025, y_offset=0.025, radius=0.004):
    x, y, z = center
    locations = [(x + x_offset, y, z), (x - x_offset, y, z), (x, y + y_offset, z), (x, y - y_offset, z)]
    for index, location in enumerate(locations, 1):
        parts.append(add_rivet(f"Rivet_{index}", location, material, radius))


def build_wood_axe():
    asset_name = "KA_T01_Tol_WoodAxe"
    wood, iron, rivets = begin_asset("WoodAxe")
    parts = [add_cylinder("Long Ash Handle", (0, 0, 0.13), 0.023, 0.78, wood, scale=(0.9, 1.05, 1))]
    parts.append(add_box("Axe Eye", (0, 0.018, 0.45), (0.064, 0.07, 0.12), iron, bevel_width=0.004))
    blade_profile = [
        (0.035, 0.390), (0.115, 0.372), (0.215, 0.352),
        (0.260, 0.355), (0.260, 0.545), (0.210, 0.548),
        (0.110, 0.525), (0.035, 0.505),
    ]
    parts.append(add_prism("Broad Felling Blade", blade_profile, 0.047, iron, bevel_width=0.003))
    parts.append(add_cylinder("Lower Collar", (0, 0, 0.36), 0.027, 0.028, iron))
    add_four_rivets(parts, rivets, (0, 0, 0.36), 0.027, 0.027)
    asset = join_visible_parts(parts, asset_name, (wood, iron, rivets))
    finish_asset(asset, [add_collision("UCX_" + asset_name + "_00", (0, 0, 0.13), (0.06, 0.065, 0.78)), add_collision("UCX_" + asset_name + "_01", (0, 0.13, 0.45), (0.08, 0.30, 0.21))])


def build_hatchet():
    asset_name = "KA_T01_Tol_Hatchet"
    wood, iron, rivets = begin_asset("Hatchet")
    parts = [add_cylinder("Short Handle", (0, 0, 0.08), 0.021, 0.44, wood, scale=(0.92, 1.06, 1))]
    parts.append(add_box("Compact Eye", (0, 0.012, 0.28), (0.055, 0.06, 0.09), iron, bevel_width=0.0035))
    parts.append(add_prism("Compact Blade", [(0.035, -0.055), (0.16, -0.065), (0.17, 0.065), (0.035, 0.048)], 0.041, iron))
    parts.append(add_cylinder("Hatchet Collar", (0, 0, 0.22), 0.025, 0.024, iron))
    add_four_rivets(parts, rivets, (0, 0, 0.22), 0.025, 0.025, 0.0035)
    asset = join_visible_parts(parts, asset_name, (wood, iron, rivets))
    finish_asset(asset, [add_collision("UCX_" + asset_name + "_00", (0, 0, 0.08), (0.055, 0.06, 0.44)), add_collision("UCX_" + asset_name + "_01", (0, 0.08, 0.28), (0.07, 0.19, 0.14))])


def build_torch():
    asset_name = "KA_T01_Tol_Torch"
    wood, iron, wrap = begin_asset("Torch", "Wrap")
    parts = [add_cylinder("Torch Shaft", (0, 0, 0.08), 0.021, 0.64, wood)]
    for index in range(5):
        z = 0.36 + index * 0.025
        parts.append(add_cylinder(f"Pitch Cloth Wrap {index + 1}", (0, 0, z), 0.043 - index * 0.002, 0.026, wrap, vertices=32))
    parts.append(add_cylinder("Pitch Ember Cap", (0, 0, 0.49), 0.035, 0.035, wrap, vertices=32))
    parts.append(add_cylinder("Iron Retaining Ring", (0, 0, 0.325), 0.029, 0.018, iron, vertices=32))
    asset = join_visible_parts(parts, asset_name, (wood, iron, wrap))
    finish_asset(asset, [add_collision("UCX_" + asset_name + "_00", (0, 0, 0.08), (0.055, 0.055, 0.64)), add_collision("UCX_" + asset_name + "_01", (0, 0, 0.42), (0.10, 0.10, 0.20))])


def build_whetstone():
    asset_name = "KA_T01_Tol_Whetstone"
    wood, iron, leather = begin_asset("Whetstone", "Leather")
    stone = create_material("Mat_T01_Whetstone_Stone", (0.24, 0.27, 0.28), 0.0, 0.82)
    parts = [add_box("Grinding Stone", (0, 0, 0), (0.055, 0.19, 0.025), stone, bevel_width=0.009, bevel_segments=7)]
    parts.append(add_cylinder("Leather Suspension Loop", (0, -0.105, 0), 0.018, 0.014, leather, rotation=(math.pi / 2, 0, 0), vertices=32))
    parts.append(add_cylinder("Iron Eyelet", (0, -0.09, 0), 0.014, 0.012, iron, rotation=(math.pi / 2, 0, 0), vertices=32))
    for index, y in enumerate((-0.055, 0.0, 0.055), 1):
        parts.append(add_box(f"Grinding Groove {index}", (0, y, 0.013), (0.038, 0.006, 0.004), iron, bevel_width=0.001, bevel_segments=2))
    asset = join_visible_parts(parts, asset_name, (stone, leather, iron))
    finish_asset(asset, [add_collision("UCX_" + asset_name + "_00", (0, 0, 0), (0.06, 0.20, 0.032))])


def build_build_tool():
    asset_name = "KA_T01_Tol_BuildTool"
    wood, iron, rivets = begin_asset("BuildTool")
    parts = [add_cylinder("Shaped Ash Handle", (0, 0, 0.065), 0.020, 0.50, wood, vertices=40, scale=(0.88, 1.06, 1.0))]
    parts.append(add_cylinder("Grip Pommel", (0, 0, -0.19), 0.024, 0.034, wood, vertices=40, scale=(0.92, 1.08, 1.0)))

    # The central eye locks the complete head to the handle.
    parts.append(add_box("Forged Central Eye", (0, 0.0, 0.295), (0.052, 0.070, 0.060), iron, bevel_width=0.005, bevel_segments=6))
    # A broad front face is used to seat beams and building pegs.
    parts.append(add_box("Broad Setting Face", (0, 0.066, 0.295), (0.062, 0.072, 0.070), iron, bevel_width=0.006, bevel_segments=6))
    parts.append(add_box("Setting Face Cap", (0, 0.106, 0.295), (0.072, 0.014, 0.080), iron, bevel_width=0.004, bevel_segments=5))
    # The rear hook is connected directly to the eye and curves down for lifting and alignment.
    hook_profile = [
        (-0.034, 0.325), (-0.085, 0.318), (-0.132, 0.295),
        (-0.168, 0.258), (-0.178, 0.220), (-0.158, 0.215),
        (-0.140, 0.246), (-0.105, 0.270), (-0.060, 0.282), (-0.034, 0.278),
    ]
    parts.append(add_prism("Connected Alignment Hook", hook_profile, 0.040, iron, bevel_width=0.003))

    parts.append(add_cylinder("Reinforced Head Collar", (0, 0, 0.235), 0.026, 0.032, iron, vertices=40, scale=(0.94, 1.07, 1.0)))
    add_four_rivets(parts, rivets, (0, 0, 0.235), 0.026, 0.027, 0.004)
    # Two grip pins add a readable handcrafted fastening detail.
    parts.append(add_rivet("Grip Pin Left", (-0.020, 0, 0.03), rivets, 0.0035))
    parts.append(add_rivet("Grip Pin Right", (0.020, 0, 0.03), rivets, 0.0035))
    asset = join_visible_parts(parts, asset_name, (wood, iron, rivets))
    finish_asset(asset, [
        add_collision("UCX_" + asset_name + "_00", (0, 0, 0.065), (0.058, 0.065, 0.53)),
        add_collision("UCX_" + asset_name + "_01", (0, -0.025, 0.285), (0.085, 0.30, 0.16)),
    ])


def build_hammer():
    asset_name = "KA_T01_Tol_Hammer"
    wood, iron, rivets = begin_asset("Hammer")
    parts = [add_cylinder("Workshop Handle", (0, 0, 0.06), 0.019, 0.46, wood)]
    parts.append(add_box("Hammer Eye", (0, 0, 0.27), (0.045, 0.065, 0.052), iron, bevel_width=0.004))
    parts.append(add_box("Flat Face", (0, 0.06, 0.27), (0.052, 0.06, 0.058), iron, bevel_width=0.004))
    peen_profile = [
        (-0.032, 0.245), (-0.085, 0.252), (-0.145, 0.262),
        (-0.145, 0.278), (-0.085, 0.288), (-0.032, 0.295),
    ]
    parts.append(add_prism("Connected Cross Peen", peen_profile, 0.045, iron, bevel_width=0.0025))
    parts.append(add_cylinder("Hammer Collar", (0, 0, 0.215), 0.023, 0.022, iron))
    add_four_rivets(parts, rivets, (0, 0, 0.215), 0.023, 0.023, 0.0034)
    asset = join_visible_parts(parts, asset_name, (wood, iron, rivets))
    finish_asset(asset, [add_collision("UCX_" + asset_name + "_00", (0, 0, 0.06), (0.052, 0.058, 0.46)), add_collision("UCX_" + asset_name + "_01", (0, 0, 0.27), (0.07, 0.24, 0.09))])


def build_rake():
    asset_name = "KA_T01_Tol_Rake"
    wood, iron, rivets = begin_asset("Rake")
    parts = [add_cylinder("Long Rake Shaft", (0, 0, 0.18), 0.018, 1.48, wood)]
    parts.append(add_box("Rake Crossbar", (0, 0, 0.91), (0.46, 0.035, 0.04), wood, bevel_width=0.004))
    for index in range(9):
        x = -0.20 + index * 0.05
        parts.append(add_cylinder(f"Iron Tine {index + 1}", (x, 0.035, 0.84), 0.005, 0.16, iron, vertices=20))
    parts.append(add_box("Crossbar Plate", (0, -0.005, 0.91), (0.09, 0.045, 0.055), iron, bevel_width=0.003))
    add_four_rivets(parts, rivets, (0, -0.03, 0.91), 0.034, 0.012, 0.0035)
    asset = join_visible_parts(parts, asset_name, (wood, iron, rivets))
    finish_asset(asset, [add_collision("UCX_" + asset_name + "_00", (0, 0, 0.18), (0.05, 0.05, 1.48)), add_collision("UCX_" + asset_name + "_01", (0, 0.025, 0.86), (0.48, 0.08, 0.20))])


def build_hoe():
    asset_name = "KA_T01_Tol_Hoe"
    wood, iron, rivets = begin_asset("Hoe")
    parts = [add_cylinder("Hoe Shaft", (0, 0, 0.18), 0.019, 1.34, wood)]
    parts.append(add_box("Hoe Socket", (0, 0.01, 0.83), (0.052, 0.07, 0.08), iron, bevel_width=0.004))
    blade_profile = [
        (0.035, 0.790), (0.095, 0.770), (0.225, 0.740),
        (0.250, 0.755), (0.250, 0.885), (0.220, 0.900),
        (0.090, 0.875), (0.035, 0.855),
    ]
    parts.append(add_prism("Connected Soil Blade", blade_profile, 0.15, iron, bevel_width=0.003))
    parts.append(add_cylinder("Hoe Collar", (0, 0, 0.76), 0.024, 0.025, iron))
    add_four_rivets(parts, rivets, (0, 0, 0.76), 0.024, 0.024, 0.0035)
    asset = join_visible_parts(parts, asset_name, (wood, iron, rivets))
    finish_asset(asset, [add_collision("UCX_" + asset_name + "_00", (0, 0, 0.18), (0.055, 0.06, 1.34)), add_collision("UCX_" + asset_name + "_01", (0, 0.13, 0.83), (0.18, 0.28, 0.17))])


def build_chisel():
    asset_name = "KA_T01_Tol_Chisel"
    wood, iron, rivets = begin_asset("Chisel")
    parts = [add_cylinder("Wood Chisel Handle", (0, -0.055, 0), 0.024, 0.15, wood, rotation=(math.pi / 2, 0, 0), scale=(1, 1.12, 1))]
    parts.append(add_cylinder("Iron Ferrule", (0, 0.025, 0), 0.026, 0.024, iron, rotation=(math.pi / 2, 0, 0)))
    parts.append(add_prism("Chisel Blade", [(0.037, -0.014), (0.19, -0.022), (0.21, 0), (0.19, 0.022), (0.037, 0.014)], 0.012, iron, bevel_width=0.0015))
    parts.append(add_rivet("Handle Rivet 1", (0.025, -0.06, 0), rivets, 0.004))
    parts.append(add_rivet("Handle Rivet 2", (-0.025, -0.06, 0), rivets, 0.004))
    parts.append(add_rivet("Ferrule Rivet 1", (0, 0.025, 0.026), rivets, 0.0035))
    parts.append(add_rivet("Ferrule Rivet 2", (0, 0.025, -0.026), rivets, 0.0035))
    asset = join_visible_parts(parts, asset_name, (wood, iron, rivets))
    finish_asset(asset, [add_collision("UCX_" + asset_name + "_00", (0, -0.055, 0), (0.06, 0.16, 0.06)), add_collision("UCX_" + asset_name + "_01", (0, 0.12, 0), (0.025, 0.19, 0.055))])


def build_smith_hammer():
    asset_name = "KA_T01_Tol_SmithHammer"
    wood, iron, rivets = begin_asset("SmithHammer")
    parts = [add_cylinder("Smithing Handle", (0, 0, 0.07), 0.021, 0.50, wood)]
    parts.append(add_box("Heavy Hammer Eye", (0, 0, 0.30), (0.055, 0.075, 0.068), iron, bevel_width=0.005))
    parts.append(add_box("Heavy Flat Face", (0, 0.075, 0.30), (0.065, 0.075, 0.078), iron, bevel_width=0.005))
    peen_profile = [
        (-0.040, 0.265), (-0.095, 0.272), (-0.165, 0.289),
        (-0.165, 0.311), (-0.095, 0.328), (-0.040, 0.335),
    ]
    parts.append(add_prism("Connected Forging Peen", peen_profile, 0.052, iron, bevel_width=0.003))
    parts.append(add_cylinder("Smith Hammer Collar", (0, 0, 0.235), 0.026, 0.026, iron))
    add_four_rivets(parts, rivets, (0, 0, 0.235), 0.026, 0.026, 0.004)
    asset = join_visible_parts(parts, asset_name, (wood, iron, rivets))
    finish_asset(asset, [add_collision("UCX_" + asset_name + "_00", (0, 0, 0.07), (0.06, 0.065, 0.50)), add_collision("UCX_" + asset_name + "_01", (0, 0, 0.30), (0.08, 0.30, 0.11))])


def build_tongs():
    asset_name = "KA_T01_Tol_Tongs"
    wood, iron, rivets = begin_asset("Tongs")
    leather = create_material("Mat_T01_Tongs_Leather", (0.16, 0.055, 0.018), 0.0, 0.68)
    parts = []
    for side, sign in (("Left", -1), ("Right", 1)):
        parts.append(add_box(f"{side} Iron Arm", (sign * 0.018, 0.02, 0), (0.012, 0.46, 0.018), iron, rotation=(0, 0, sign * math.radians(2)), bevel_width=0.003))
        parts.append(add_box(f"{side} Leather Grip", (sign * 0.027, -0.16, 0), (0.022, 0.17, 0.028), leather, bevel_width=0.004))
        parts.append(add_prism(f"{side} Gripping Jaw", [(0.23, -0.018), (0.31, -0.035), (0.34, -0.015), (0.34, 0.015), (0.31, 0.035), (0.23, 0.018)], 0.026, iron, bevel_width=0.002))
        parts[-1].location.x = sign * 0.018
    parts.append(add_rivet("Central Pivot", (0, 0.08, 0), rivets, 0.012))
    parts.append(add_rivet("Left Grip Rivet", (-0.027, -0.16, 0.015), rivets, 0.004))
    parts.append(add_rivet("Right Grip Rivet", (0.027, -0.16, 0.015), rivets, 0.004))
    parts.append(add_rivet("Pivot Back Rivet", (0, 0.08, -0.012), rivets, 0.008))
    asset = join_visible_parts(parts, asset_name, (iron, leather, rivets))
    finish_asset(asset, [add_collision("UCX_" + asset_name + "_00", (0, -0.06, 0), (0.09, 0.40, 0.07)), add_collision("UCX_" + asset_name + "_01", (0, 0.26, 0), (0.10, 0.22, 0.09))])


BUILDERS = {
    "WoodAxe": build_wood_axe,
    "Torch": build_torch,
    "BuildTool": build_build_tool,
    "Hammer": build_hammer,
    "Rake": build_rake,
    "Hoe": build_hoe,
    "SmithHammer": build_smith_hammer,
    "Tongs": build_tongs,
}

if __name__ == "__main__":
    arguments = sys.argv[sys.argv.index("--") + 1:] if "--" in sys.argv else []
    selected = arguments or list(BUILDERS)
    for key in selected:
        BUILDERS[key]()
    print("SELECTED_TIER1_TOOLS_CREATED " + ",".join(selected))
