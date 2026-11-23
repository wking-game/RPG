import math
import os
import sys

import bpy
from mathutils import Vector

sys.path.insert(0, os.path.dirname(__file__))
import create_missing_tier1_tools as base
import create_remaining_plan_assets as remaining


def cylinder_between(name, start, end, radius, material, vertices=20):
    start = Vector(start)
    end = Vector(end)
    direction = end - start
    obj = base.add_cylinder(name, (start + end) * 0.5, radius, direction.length, material, vertices=vertices)
    obj.rotation_mode = "QUATERNION"
    obj.rotation_quaternion = direction.to_track_quat("Z", "Y")
    bpy.context.view_layer.objects.active = obj
    bpy.ops.object.transform_apply(location=False, rotation=True, scale=False)
    return obj


def flame_material():
    material = bpy.data.materials.new("Mat_T01_Torch_Flame")
    material.use_nodes = True
    shader = material.node_tree.nodes.get("Principled BSDF")
    shader.inputs["Base Color"].default_value = (1.0, 0.08, 0.005, 1.0)
    shader.inputs["Emission Color"].default_value = (1.0, 0.035, 0.002, 1.0)
    shader.inputs["Emission Strength"].default_value = 7.0
    shader.inputs["Roughness"].default_value = 0.3
    return material


def build_wood_axe():
    name = "KA_T01_Tol_WoodAxe"
    wood, iron, rivets = base.begin_asset("WoodAxe")
    parts = [base.add_cylinder("Ash Handle", (0, 0, 0.10), 0.023, 0.70, wood, scale=(0.88, 1.05, 1))]
    parts += [base.add_box("Closed Axe Eye", (0, 0.018, 0.425), (0.068, 0.075, 0.115), iron, bevel_width=0.005)]
    parts += [base.add_box("Solid Rear Poll", (0, -0.045, 0.425), (0.070, 0.055, 0.105), iron, bevel_width=0.005)]
    profile = [(0.035, 0.375), (0.12, 0.355), (0.235, 0.335), (0.275, 0.345),
               (0.275, 0.515), (0.235, 0.525), (0.12, 0.495), (0.035, 0.475)]
    parts += [base.add_prism("Broad Felling Blade", profile, 0.045, iron, 0.003)]
    parts += [base.add_box("Wooden Eye Wedge", (0, 0.018, 0.486), (0.047, 0.050, 0.010), wood, bevel_width=0.002)]
    parts += [base.add_cylinder("Lower Collar", (0, 0, 0.345), 0.027, 0.030, iron)]
    base.add_four_rivets(parts, rivets, (0, 0, 0.345), 0.027, 0.027)
    asset = base.join_visible_parts(parts, name, (wood, iron, rivets))
    base.finish_asset(asset, [base.add_collision("UCX_" + name + "_00", (0, 0, 0.10), (0.06, 0.065, 0.70)), base.add_collision("UCX_" + name + "_01", (0, 0.14, 0.425), (0.08, 0.32, 0.20))])


def build_torch():
    name = "KA_T01_Tol_Torch"
    wood, iron, wrap = base.begin_asset("Torch", "Wrap")
    flame = flame_material()
    parts = [base.add_cylinder("Torch Shaft", (0, 0, 0.08), 0.022, 0.65, wood)]
    parts += [base.add_cylinder("Basket Lower Ring", (0, 0, 0.39), 0.052, 0.014, iron, vertices=32),
              base.add_cylinder("Basket Upper Ring", (0, 0, 0.52), 0.072, 0.014, iron, vertices=32)]
    for index in range(6):
        angle = index * math.tau / 6
        start = (0.047 * math.cos(angle), 0.047 * math.sin(angle), 0.39)
        end = (0.067 * math.cos(angle), 0.067 * math.sin(angle), 0.52)
        parts.append(cylinder_between(f"Basket Bar {index + 1}", start, end, 0.006, iron, 16))
    parts += [base.add_cylinder("Pitch Fuel", (0, 0, 0.46), 0.043, 0.14, wrap, vertices=28)]
    for z, radius in ((0.43, 0.050), (0.47, 0.054), (0.51, 0.057)):
        bpy.ops.mesh.primitive_torus_add(major_radius=radius, minor_radius=0.006, major_segments=32, minor_segments=8, location=(0, 0, z))
        ring = bpy.context.object; ring.name = "Basket Hoop"; ring.data.materials.append(iron); parts.append(ring)
    for index, (z, scale) in enumerate(((0.57, (0.040, 0.032, 0.085)), (0.625, (0.025, 0.020, 0.070))), 1):
        bpy.ops.mesh.primitive_ico_sphere_add(subdivisions=3, radius=1, location=(0, 0, z))
        obj = bpy.context.object; obj.name = f"Visible Flame {index}"; obj.scale = scale
        bpy.ops.object.transform_apply(location=False, rotation=False, scale=True); obj.data.materials.append(flame); parts.append(obj)
    asset = base.join_visible_parts(parts, name, (wood, iron, wrap, flame))
    base.finish_asset(asset, [base.add_collision("UCX_" + name + "_00", (0, 0, 0.08), (0.06, 0.06, 0.65)), base.add_collision("UCX_" + name + "_01", (0, 0, 0.48), (0.16, 0.16, 0.28))])


def build_rake():
    name = "KA_T01_Tol_Rake"
    wood, iron, rivets = base.begin_asset("Rake")
    parts = [base.add_cylinder("Long Ash Shaft", (0, 0, 0.14), 0.019, 1.44, wood)]
    parts += [base.add_box("Forged Rake Bar", (0, 0.015, 0.84), (0.50, 0.035, 0.045), iron, bevel_width=0.004),
              base.add_box("Central Socket", (0, 0, 0.80), (0.075, 0.065, 0.11), iron, bevel_width=0.004)]
    for index in range(11):
        x = -0.225 + index * 0.045
        parts.append(cylinder_between(f"Curved Tine {index + 1}A", (x, 0.018, 0.83), (x, 0.055, 0.75), 0.0045, iron, 14))
        parts.append(cylinder_between(f"Curved Tine {index + 1}B", (x, 0.055, 0.75), (x, 0.095, 0.715), 0.0040, iron, 14))
    base.add_four_rivets(parts, rivets, (0, -0.018, 0.80), 0.028, 0.02, 0.0035)
    asset = base.join_visible_parts(parts, name, (wood, iron, rivets))
    base.finish_asset(asset, [base.add_collision("UCX_" + name + "_00", (0, 0, 0.14), (0.055, 0.055, 1.44)), base.add_collision("UCX_" + name + "_01", (0, 0.055, 0.78), (0.52, 0.13, 0.18))])


def build_hoe():
    name = "KA_T01_Tol_Hoe"
    wood, iron, rivets = base.begin_asset("Hoe")
    parts = [base.add_cylinder("Long Hoe Shaft", (0, 0, 0.16), 0.019, 1.32, wood)]
    parts += [cylinder_between("Tapered Socket", (0, 0, 0.74), (0, 0.09, 0.78), 0.027, iron, 24)]
    plate = base.add_box("Thin Angled Soil Blade", (0, 0.145, 0.755), (0.24, 0.13, 0.018), iron,
                         rotation=(math.radians(14), 0, 0), bevel_width=0.004)
    parts.append(plate)
    parts += [base.add_cylinder("Socket Collar", (0, 0, 0.72), 0.024, 0.028, iron)]
    base.add_four_rivets(parts, rivets, (0, 0, 0.72), 0.024, 0.024, 0.0035)
    asset = base.join_visible_parts(parts, name, (wood, iron, rivets))
    base.finish_asset(asset, [base.add_collision("UCX_" + name + "_00", (0, 0, 0.16), (0.055, 0.055, 1.32)), base.add_collision("UCX_" + name + "_01", (0, 0.16, 0.75), (0.19, 0.28, 0.16))])


def build_tongs():
    name = "KA_T01_Tol_Tongs"
    wood, iron, rivets = base.begin_asset("Tongs")
    leather = base.create_material("Mat_T01_Tongs_Leather", (0.09, 0.035, 0.014), 0.0, 0.72)
    parts = []
    for label, sign in (("Left", -1), ("Right", 1)):
        parts += [cylinder_between(f"{label} Handle", (sign * 0.075, -0.26, 0), (sign * 0.012, 0.07, 0), 0.010, iron, 18),
                  cylinder_between(f"{label} Open Jaw", (sign * 0.012, 0.07, 0), (sign * 0.055, 0.30, 0), 0.010, iron, 18),
                  cylinder_between(f"{label} Leather Grip", (sign * 0.075, -0.26, 0), (sign * 0.052, -0.12, 0), 0.015, leather, 18),
                  base.add_box(f"{label} Jaw Pad", (sign * 0.065, 0.315, 0), (0.030, 0.060, 0.035), iron, rotation=(0, 0, -sign * math.radians(10)), bevel_width=0.004)]
    parts += [base.add_rivet("Central Pivot", (0, 0.07, 0), rivets, 0.014), base.add_rivet("Pivot Back", (0, 0.07, -0.014), rivets, 0.010)]
    asset = base.join_visible_parts(parts, name, (iron, leather, rivets))
    base.finish_asset(asset, [base.add_collision("UCX_" + name + "_00", (0, 0.02, 0), (0.18, 0.64, 0.08))])


def saw_blade(material):
    points = [(-0.27, 0.045), (0.29, 0.025), (0.31, -0.035)]
    tooth_count = 18
    for index in range(tooth_count + 1):
        x = 0.29 - index * (0.55 / tooth_count)
        z = -0.040 if index % 2 == 0 else -0.020
        points.append((x, z))
    return remaining.poly_prism_xz("Toothed Saw Blade", points, 0.004, material)


def build_saw():
    name = "KA_T01_Tol_Saw"
    wood, iron, rivets = base.begin_asset("Saw")
    parts = [saw_blade(iron)]
    parts += [base.add_box("Wooden Handle", (-0.32, 0, 0.005), (0.13, 0.035, 0.13), wood, rotation=(0, math.radians(-8), 0), bevel_width=0.012),
              base.add_box("Handle Ferrule", (-0.255, 0, 0.005), (0.035, 0.043, 0.09), iron, bevel_width=0.004)]
    for z in (-0.03, 0.035): parts.append(base.add_rivet("Handle Rivet", (-0.325, -0.020, z), rivets, 0.006))
    asset = base.join_visible_parts(parts, name, (wood, iron, rivets))
    base.finish_asset(asset, [base.add_collision("UCX_" + name + "_00", (0.01, 0, 0), (0.72, 0.055, 0.15))])


def build_pickaxe():
    name = "KA_T01_Tol_Pickaxe"
    wood, iron, rivets = base.begin_asset("Pickaxe")
    parts = [base.add_cylinder("Long Pick Shaft", (0, 0, 0.08), 0.022, 0.88, wood, scale=(0.90, 1.05, 1)),
             base.add_box("Forged Central Eye", (0, 0, 0.49), (0.070, 0.085, 0.105), iron, bevel_width=0.005)]
    head = [(-0.30, 0.47), (-0.20, 0.455), (-0.07, 0.445), (0.0, 0.455),
            (0.12, 0.445), (0.24, 0.42), (0.31, 0.395), (0.27, 0.455),
            (0.14, 0.50), (0.0, 0.525), (-0.10, 0.52), (-0.22, 0.505)]
    parts += [remaining.poly_prism_xz("Long Pick and Adze Head", head, 0.050, iron),
              base.add_box("Broad Adze Edge", (-0.30, 0, 0.475), (0.035, 0.105, 0.085), iron, bevel_width=0.003),
              base.add_cylinder("Head Collar", (0, 0, 0.415), 0.028, 0.030, iron)]
    base.add_four_rivets(parts, rivets, (0, 0, 0.415), 0.027, 0.027)
    asset = base.join_visible_parts(parts, name, (wood, iron, rivets))
    base.finish_asset(asset, [base.add_collision("UCX_" + name + "_00", (0, 0, 0.08), (0.06, 0.065, 0.88)), base.add_collision("UCX_" + name + "_01", (0, 0, 0.47), (0.67, 0.13, 0.16))])


BUILDERS = {"WoodAxe": build_wood_axe, "Torch": build_torch, "Rake": build_rake, "Hoe": build_hoe,
            "Tongs": build_tongs, "Saw": build_saw, "Pickaxe": build_pickaxe}

if __name__ == "__main__":
    requested = sys.argv[sys.argv.index("--") + 1:] if "--" in sys.argv else list(BUILDERS)
    for key in requested: BUILDERS[key]()
    print("CRITICIZED_ASSETS_REBUILT " + ",".join(requested))
