"""Erzeugt mehrteilige Schulterplatten und separate Unterarmschienen."""

from pathlib import Path
import math
import sys

import bpy
from mathutils import Vector

sys.dont_write_bytecode = True
SCRIPT_DIR = Path(__file__).resolve().parent
if str(SCRIPT_DIR) not in sys.path:
    sys.path.insert(0, str(SCRIPT_DIR))
import kustenasche_common as ka


ASSET = "SK_Armor_Pauldrons_Arms"
COLLECTION = "04_SK_Armor_Pauldrons_Arms"


def point_on_arm(a, b, t, offset_y=0.0):
    p = Vector(a).lerp(Vector(b), t)
    p.y += offset_y
    return tuple(p)


def ring_points(center, axis, radius_a, radius_b, sides=16):
    center = Vector(center)
    axis = Vector(axis).normalized()
    reference = Vector((0.0, 0.0, 1.0))
    if abs(axis.dot(reference)) > 0.92:
        reference = Vector((0.0, 1.0, 0.0))
    u = axis.cross(reference).normalized()
    v = axis.cross(u).normalized()
    return [tuple(center + radius_a * math.cos(2 * math.pi * i / sides) * u +
                  radius_b * math.sin(2 * math.pi * i / sides) * v)
            for i in range(sides)]


def add_wing_plate(collection, steel, brass, sign):
    # Dünne Quad-Schale mit leicht angehobener Außenkante.
    nu, nv = 10, 6
    vertices = []
    faces = []

    def point(u, v, back=False):
        width = 0.014 + 0.046 * math.sin(math.pi * u) ** 0.7
        x = sign * (0.185 + 0.225 * u)
        z_center = 1.435 + 0.105 * u + 0.018 * math.sin(math.pi * u)
        z = z_center + v * width
        y = -0.046 - 0.026 * math.sin(math.pi * u) + 0.022 * (v * v)
        if back:
            y += 0.008
        return (x, y, z)

    row_size = nu + 1
    for back in (False, True):
        for j in range(nv + 1):
            v = -1.0 + 2.0 * j / nv
            for i in range(nu + 1):
                u = i / nu
                vertices.append(point(u, v, back))
    layer_size = row_size * (nv + 1)
    for layer in range(2):
        base = layer * layer_size
        for j in range(nv):
            for i in range(nu):
                a = base + j * row_size + i
                b = a + 1
                d = base + (j + 1) * row_size + i
                c = d + 1
                face = (a, b, c, d)
                faces.append(face if (sign > 0) != bool(layer) else tuple(reversed(face)))
    # Quad-Randflächen schließen Außenkante, Spitze und Ansatz.
    for i in range(nu):
        a, b = i, i + 1
        faces.append((a, b, layer_size + b, layer_size + a))
        a = nv * row_size + i
        b = a + 1
        faces.append((a, layer_size + a, layer_size + b, b))
    for j in range(nv):
        a = j * row_size
        b = (j + 1) * row_size
        faces.append((a, b, layer_size + b, layer_size + a))
        a = j * row_size + nu
        b = (j + 1) * row_size + nu
        faces.append((a, layer_size + a, layer_size + b, b))
    side = "L" if sign > 0 else "R"
    ka.create_mesh("Pauldron_Wing_Plate_" + side, vertices, faces, steel, collection)

    crest = [(sign * (0.195 + 0.215 * i / 24.0),
              -0.074 - 0.018 * math.sin(math.pi * i / 24.0),
              1.450 + 0.105 * i / 24.0 + 0.018 * math.sin(math.pi * i / 24.0))
             for i in range(25)]
    ka.swept_tube("Pauldron_Wing_Raised_Edge_" + side, crest,
                  [0.0055 * (1 - 0.8 * i / 24.0) for i in range(25)],
                  brass, collection, sides=8)


def add_rivet_row(collection, brass, a, b, radius_start, radius_end, count, side_name):
    for index in range(count):
        t = index / max(1, count - 1)
        center = Vector(a).lerp(Vector(b), t)
        radius = radius_start + (radius_end - radius_start) * t
        center.y -= radius * 0.92
        ka.ellipsoid("Vambrace_Rivet_%s_%02d" % (side_name, index), tuple(center),
                     (0.0045, 0.0025, 0.0045), brass, collection, 10, 6)


def add_arm_armor(collection, steel, brass, sign):
    side = "L" if sign > 0 else "R"
    shoulder = (sign * 0.205, 0.0, 1.430)
    elbow = (sign * 0.385, 0.0, 1.285)
    wrist = (sign * 0.535, -0.005, 1.135)

    # Schulterkappe und gestaffelte Außenlappen.
    ka.ellipsoid("Pauldron_Dome_" + side, (sign * 0.235, 0.002, 1.420),
                 (0.102, 0.118, 0.077), steel, collection, 24, 12)
    add_wing_plate(collection, steel, brass, sign)
    for lame in range(2):
        z = 1.405 - lame * 0.047
        x = sign * (0.285 + lame * 0.012)
        ka.ellipsoid("Pauldron_Outer_Lame_%s_%d" % (side, lame),
                     (x, -0.008, z), (0.078 - lame * 0.008, 0.095, 0.033),
                     steel, collection, 20, 10)
        for rivet in range(4):
            theta = math.radians(-65 + 42 * rivet)
            p = (x + 0.067 * math.sin(theta), -0.088,
                 z + 0.018 * math.cos(theta))
            ka.ellipsoid("Pauldron_Rivet_%s_%d_%d" % (side, lame, rivet), p,
                         (0.0048, 0.003, 0.0048), brass, collection, 10, 6)

    # Oberarmhülse sitzt zwischen Schulterkappe und Ellenbogen.
    upper_points = [point_on_arm(shoulder, elbow, t)
                    for t in (0.0, 0.18, 0.38, 0.62, 0.82, 1.0)]
    ka.swept_tube("Upperarm_Plate_" + side, upper_points,
                  (0.067, 0.071, 0.068, 0.062, 0.057, 0.054),
                  steel, collection, sides=16)

    # Separater Unterarmschutz; die Profilringe bilden die Endränder.
    fore_points = [point_on_arm(elbow, wrist, t)
                   for t in (0.0, 0.16, 0.36, 0.60, 0.82, 1.0)]
    radii = (0.070, 0.073, 0.069, 0.062, 0.055, 0.050)
    ka.swept_tube("Vambrace_Separate_" + side, fore_points, radii,
                  steel, collection, sides=16)
    axis = Vector(wrist) - Vector(elbow)
    for t, radius in ((0.05, 0.070), (0.90, 0.052), (0.98, 0.050)):
        center = Vector(elbow).lerp(Vector(wrist), t)
        loop = ring_points(center, axis, radius * 1.02, radius * 0.93, 20)
        ka.swept_tube("Vambrace_Cuff_%s_%.2f" % (side, t), loop,
                      0.0032, brass, collection, sides=8, cyclic=True)
    add_rivet_row(collection, brass, elbow, wrist, radii[0], radii[-1], 8, side)

    # Ein schmales, dunkles Gelenkband lässt den Ellenbogen lesbar beweglich.
    ka.swept_tube("Elbow_Gap_" + side,
                  [point_on_arm(elbow, wrist, 0.08), point_on_arm(elbow, wrist, 0.12)],
                  [0.071, 0.071], steel, collection, sides=16)


def build():
    ka.prepare_scene()
    asset_collection = ka.new_asset_collection(COLLECTION)
    geo = ka.ensure_collection("GEO_Pauldrons_Arms", asset_collection)
    rig_collection = ka.ensure_collection("RIG_Pauldrons_Arms", asset_collection)

    steel = ka.weathered_material("KA_MAT_DarkHammeredSteel", (0.055, 0.071, 0.070),
                                  0.78, 0.66, 130.0, 0.16, 0.0012, 0.58, 0.82)
    brass = ka.weathered_material("KA_MAT_PatinatedOldBrass", (0.31, 0.225, 0.073),
                                  0.62, 0.72, 72.0, 0.12, 0.0010, 0.64, 0.88)

    add_arm_armor(geo, steel, brass, -1.0)
    add_arm_armor(geo, steel, brass, 1.0)
    armature = ka.create_standard_armature("ARM_Pauldrons_Arms", rig_collection)
    armor = ka.finalize_asset(geo, ASSET, origin=(0.0, 0.0, 0.0), uv_margin=0.010)
    ka.bind_mesh_to_armature(armor, armature)
    armor["fit_rig"] = "KA_1.75m_A_Pose_ZUp_front_-Y"
    armor["origin_note"] = "Character root / feet plane"
    return armor


if __name__ == "__main__":
    build()
