import bpy
import math
from mathutils import Vector


ASSET = "KA_T01_Tol_BuildHammer"
BLEND_PATH = r"D:\gamer\gamer\Assets\Source\KA_T01_Tol_BuildHammer.blend"
FBX_PATH = r"D:\gamer\gamer\Assets\Items\KA_T01_Tol_BuildHammer.fbx"


def clear_scene():
    bpy.ops.object.select_all(action="SELECT")
    bpy.ops.object.delete(use_global=False)
    for datablocks in (bpy.data.meshes, bpy.data.curves, bpy.data.materials):
        for block in list(datablocks):
            if block.users == 0:
                datablocks.remove(block)


def material(name, base, metallic, roughness):
    mat = bpy.data.materials.new(name)
    mat.use_nodes = True
    bsdf = mat.node_tree.nodes.get("Principled BSDF")
    bsdf.inputs["Base Color"].default_value = (*base, 1.0)
    bsdf.inputs["Metallic"].default_value = metallic
    bsdf.inputs["Roughness"].default_value = roughness

    noise = mat.node_tree.nodes.new("ShaderNodeTexNoise")
    noise.inputs["Scale"].default_value = 7.0 if metallic else 4.0
    noise.inputs["Detail"].default_value = 3.0
    noise.inputs["Roughness"].default_value = 0.7
    ramp = mat.node_tree.nodes.new("ShaderNodeValToRGB")
    ramp.color_ramp.elements[0].position = 0.28
    ramp.color_ramp.elements[1].position = 0.72
    ramp.color_ramp.elements[0].color = (*tuple(c * 0.63 for c in base), 1.0)
    ramp.color_ramp.elements[1].color = (*tuple(min(1.0, c * 1.22) for c in base), 1.0)
    mat.node_tree.links.new(noise.outputs["Fac"], ramp.inputs["Fac"])
    mat.node_tree.links.new(ramp.outputs["Color"], bsdf.inputs["Base Color"])
    return mat


def mesh_from_rings(name, rings, segments, mat):
    verts = []
    faces = []
    for z, cx, cy, rx, ry, twist in rings:
        for j in range(segments):
            a = 2.0 * math.pi * j / segments + twist
            verts.append((cx + rx * math.cos(a), cy + ry * math.sin(a), z))
    ring_count = len(rings)
    for i in range(ring_count - 1):
        for j in range(segments):
            a = i * segments + j
            b = i * segments + (j + 1) % segments
            c = (i + 1) * segments + (j + 1) % segments
            d = (i + 1) * segments + j
            faces.append((a, b, c, d))
    faces.append(tuple(reversed(range(segments))))
    top = (ring_count - 1) * segments
    faces.append(tuple(top + j for j in range(segments)))
    mesh = bpy.data.meshes.new(name + "_Mesh")
    mesh.from_pydata(verts, [], faces)
    mesh.update()
    obj = bpy.data.objects.new(name, mesh)
    bpy.context.collection.objects.link(obj)
    obj.data.materials.append(mat)
    return obj


def add_beveled_cube(name, location, dimensions, mat, bevel=0.0025, segments=3):
    bpy.ops.mesh.primitive_cube_add(location=location)
    obj = bpy.context.object
    obj.name = name
    obj.dimensions = dimensions
    bpy.ops.object.transform_apply(location=False, rotation=False, scale=True)
    obj.data.materials.append(mat)
    modifier = obj.modifiers.new("Handgeschmiedete Kanten", "BEVEL")
    modifier.width = bevel
    modifier.segments = segments
    modifier.limit_method = "ANGLE"
    bpy.context.view_layer.objects.active = obj
    bpy.ops.object.modifier_apply(modifier=modifier.name)
    return obj


def claw_prong(name, x0, x1, iron):
    # Four cross-sections form a slightly downward-curving, tapering nail claw.
    sections = [
        (-0.018, 0.235, 0.017, 0.022),
        (-0.040, 0.233, 0.014, 0.017),
        (-0.061, 0.226, 0.010, 0.012),
        (-0.0775, 0.215, 0.0065, 0.006),
    ]
    verts = []
    for y, z, half_x, half_z in sections:
        center_x = x0 if y > -0.05 else x1
        verts.extend([
            (center_x - half_x, y, z - half_z),
            (center_x + half_x, y, z - half_z),
            (center_x + half_x, y, z + half_z),
            (center_x - half_x, y, z + half_z),
        ])
    faces = []
    for i in range(len(sections) - 1):
        a = i * 4
        b = (i + 1) * 4
        for j in range(4):
            faces.append((a + j, a + (j + 1) % 4, b + (j + 1) % 4, b + j))
    faces.extend([(0, 3, 2, 1), (12, 13, 14, 15)])
    mesh = bpy.data.meshes.new(name + "_Mesh")
    mesh.from_pydata(verts, [], faces)
    mesh.update()
    obj = bpy.data.objects.new(name, mesh)
    bpy.context.collection.objects.link(obj)
    obj.data.materials.append(iron)
    bevel = obj.modifiers.new("Abgenutzte Kanten", "BEVEL")
    bevel.width = 0.0013
    bevel.segments = 2
    bpy.context.view_layer.objects.active = obj
    bpy.ops.object.modifier_apply(modifier=bevel.name)
    return obj


def add_rivet(name, location, rotation, mat):
    bpy.ops.mesh.primitive_uv_sphere_add(segments=12, ring_count=8, radius=0.0034, location=location, rotation=rotation)
    obj = bpy.context.object
    obj.name = name
    obj.scale = (1.0, 1.0, 0.58)
    bpy.ops.object.transform_apply(location=False, rotation=False, scale=True)
    obj.data.materials.append(mat)
    return obj


def add_collision(name, location, dimensions):
    bpy.ops.mesh.primitive_cube_add(location=location)
    obj = bpy.context.object
    obj.name = name
    obj.dimensions = dimensions
    bpy.ops.object.transform_apply(location=False, rotation=False, scale=True)
    obj.display_type = "WIRE"
    obj.hide_render = True
    return obj


def main():
    clear_scene()
    scene = bpy.context.scene
    scene.unit_settings.system = "METRIC"
    scene.unit_settings.scale_length = 1.0
    scene.unit_settings.length_unit = "METERS"
    scene.render.engine = "BLENDER_EEVEE"
    scene.world.color = (0.018, 0.023, 0.028)

    wood = material("Mat_T01_BuildHammer_Wood", (0.105, 0.070, 0.045), 0.0, 0.72)
    iron = material("Mat_T01_BuildHammer_Iron", (0.12, 0.16, 0.17), 0.78, 0.34)
    rivet = material("Mat_T01_BuildHammer_Rivets", (0.19, 0.22, 0.22), 0.85, 0.27)

    # Final grip origin is world (0,0,0): source dimensions 0..0.460 m shifted by -0.180 m.
    rings = []
    ring_count = 15
    for i in range(ring_count):
        t = i / (ring_count - 1)
        z = -0.180 + 0.460 * t
        cx = 0.0015 * math.sin(t * math.pi * 1.4)
        cy = 0.010 * max(0.0, (t - 0.48) / 0.52) ** 1.35
        swell = 0.0032 * math.exp(-((t - 0.08) / 0.12) ** 2)
        grip = 0.0016 * math.exp(-((t - 0.34) / 0.18) ** 2)
        rx = 0.0153 + swell + grip - 0.0017 * t
        ry = 0.0180 + swell + grip - 0.0013 * t
        rings.append((z, cx, cy, rx, ry, 0.035 * math.sin(t * 4.0)))
    handle = mesh_from_rings("Holzschaft", rings, 24, wood)
    bevel = handle.modifiers.new("Weiche Griffkanten", "BEVEL")
    bevel.width = 0.0007
    bevel.segments = 2
    bpy.context.view_layer.objects.active = handle
    bpy.ops.object.modifier_apply(modifier=bevel.name)

    parts = [handle]
    # Main iron eye/neck and broad striking face (+Y).
    parts.append(add_beveled_cube("Hammerauge", (0.0, 0.002, 0.240), (0.036, 0.045, 0.044), iron, 0.0032, 3))
    parts.append(add_beveled_cube("Schlaghals", (0.0, 0.045, 0.240), (0.031, 0.050, 0.038), iron, 0.0028, 3))
    parts.append(add_beveled_cube("Schlagflaeche", (0.0, 0.0725, 0.240), (0.043, 0.012, 0.051), iron, 0.0023, 3))

    # Split claw points toward -Y with a visible V-shaped separation.
    parts.append(claw_prong("Nagelklaue_L", -0.0095, -0.0070, iron))
    parts.append(claw_prong("Nagelklaue_R", 0.0095, 0.0070, iron))

    bpy.ops.mesh.primitive_cylinder_add(vertices=24, radius=0.020, depth=0.020, location=(0.0, 0.0065, 0.202))
    band = bpy.context.object
    band.name = "Eisenband"
    band.data.materials.append(iron)
    bevel = band.modifiers.new("Bandkanten", "BEVEL")
    bevel.width = 0.0014
    bevel.segments = 2
    bpy.context.view_layer.objects.active = band
    bpy.ops.object.modifier_apply(modifier=bevel.name)
    parts.append(band)

    rivets = [
        add_rivet("Niete_R", (0.0205, 0.0065, 0.202), (0.0, math.pi / 2.0, 0.0), rivet),
        add_rivet("Niete_L", (-0.0205, 0.0065, 0.202), (0.0, -math.pi / 2.0, 0.0), rivet),
        add_rivet("Niete_V", (0.0, 0.0270, 0.202), (math.pi / 2.0, 0.0, 0.0), rivet),
        add_rivet("Niete_H", (0.0, -0.0140, 0.202), (-math.pi / 2.0, 0.0, 0.0), rivet),
    ]
    parts.extend(rivets)

    # Join visible pieces into one game mesh while preserving the three material slots.
    bpy.ops.object.select_all(action="DESELECT")
    for obj in parts:
        obj.select_set(True)
    bpy.context.view_layer.objects.active = handle
    bpy.ops.object.join()
    asset = bpy.context.object
    asset.name = ASSET
    # Ensure the exact slot order required by the plan.
    for mat in (wood, iron, rivet):
        if mat.name not in [slot.name for slot in asset.data.materials]:
            asset.data.materials.append(mat)
    bpy.ops.object.transform_apply(location=True, rotation=True, scale=True)

    # Clean UV set with a compact island margin.
    bpy.ops.object.mode_set(mode="EDIT")
    bpy.ops.mesh.select_all(action="SELECT")
    bpy.ops.uv.smart_project(angle_limit=math.radians(66.0), island_margin=0.02)
    bpy.ops.mesh.normals_make_consistent(inside=False)
    bpy.ops.object.mode_set(mode="OBJECT")
    for poly in asset.data.polygons:
        poly.use_smooth = True

    # Two simple, separate UCX collision bodies as required by the asset plan.
    col_handle = add_collision("UCX_KA_T01_Tol_BuildHammer_00", (0.0, 0.004, 0.050), (0.040, 0.050, 0.460))
    col_head = add_collision("UCX_KA_T01_Tol_BuildHammer_01", (0.0, 0.000, 0.240), (0.050, 0.155, 0.060))

    # Export exactly the game mesh and its collision hulls.
    bpy.ops.object.select_all(action="DESELECT")
    for obj in (asset, col_handle, col_head):
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
    bpy.ops.wm.save_as_mainfile(filepath=BLEND_PATH)

    triangles = sum(len(poly.vertices) - 2 for poly in asset.data.polygons)
    print(f"BUILD_HAMMER_OK name={asset.name} triangles={triangles} materials={len(asset.data.materials)}")


main()
