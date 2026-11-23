"""Erzeugt Brust-/Rückenpanzer und gemeinsame A-Pose-Gewichte für Küstenasche."""

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


ASSET = "SK_Armor_Cuirass"
COLLECTION = "03_SK_Armor_Cuirass"
TORSO_PROFILE = [
    (0.900, 0.148, 0.105),
    (0.955, 0.158, 0.108),
    (1.035, 0.140, 0.094),
    (1.125, 0.153, 0.096),
    (1.245, 0.179, 0.108),
    (1.340, 0.197, 0.113),
    (1.405, 0.202, 0.111),
    (1.455, 0.181, 0.103),
    (1.490, 0.133, 0.086),
]


def torso_surface(x, z, offset=0.003):
    rx, ry = TORSO_PROFILE[0][1], TORSO_PROFILE[0][2]
    for i in range(len(TORSO_PROFILE) - 1):
        z0, ax, ay = TORSO_PROFILE[i]
        z1, bx, by = TORSO_PROFILE[i + 1]
        if z0 <= z <= z1:
            t = (z - z0) / max(1e-6, z1 - z0)
            rx, ry = ax + (bx - ax) * t, ay + (by - ay) * t
            break
    ratio = max(0.08, 1.0 - (x / max(rx, 1e-6)) ** 2)
    return (x, -ry * math.sqrt(ratio) - offset, z)


def add_sun_brooch(collection, brass, steel):
    center = torso_surface(0.0, 1.335, 0.008)
    ka.ellipsoid("Cuirass_Sun_Brooch_Rim", center,
                 (0.043, 0.008, 0.043), brass, collection, 24, 12)
    ka.ellipsoid("Cuirass_Sun_Brooch_Core",
                 (center[0], center[1] - 0.006, center[2]),
                 (0.030, 0.006, 0.030), steel, collection, 24, 12)
    ka.ellipsoid("Cuirass_Sun_Brooch_Center",
                 (center[0], center[1] - 0.011, center[2]),
                 (0.007, 0.003, 0.007), brass, collection, 16, 8)
    for ray in range(12):
        angle = 2.0 * math.pi * ray / 12.0
        x0, z0 = 0.046 * math.cos(angle), 1.335 + 0.046 * math.sin(angle)
        x1, z1 = 0.058 * math.cos(angle), 1.335 + 0.058 * math.sin(angle)
        p0 = torso_surface(x0, z0, 0.006)
        p1 = torso_surface(x1, z1, 0.006)
        ka.swept_tube("Cuirass_Sun_Ray_%02d" % ray, [p0, p1], 0.0018,
                      brass, collection, sides=6)


def add_collar_chain(collection, brass):
    # Kleine geschlossene ovale Glieder liegen tangential um den hohen Kragen.
    count = 28
    for i in range(count):
        theta = 2.0 * math.pi * i / count
        center = Vector((0.110 * math.sin(theta),
                         -0.090 * math.cos(theta),
                         1.505 + (0.0015 if i % 2 else -0.0015)))
        tangent = Vector((math.cos(theta), math.sin(theta), 0.0)).normalized()
        vertical = Vector((0.0, 0.0, 1.0))
        points = []
        for step in range(12):
            angle = 2.0 * math.pi * step / 12.0
            point = center + tangent * (0.008 * math.cos(angle)) + vertical * (0.006 * math.sin(angle))
            points.append(tuple(point))
        ka.swept_tube("Collar_Chain_Link_%02d" % i, points, 0.0016,
                      brass, collection, sides=6, cyclic=True)


def build():
    ka.prepare_scene()
    asset_collection = ka.new_asset_collection(COLLECTION)
    geo = ka.ensure_collection("GEO_Cuirass", asset_collection)
    rig_collection = ka.ensure_collection("RIG_Cuirass", asset_collection)

    steel = ka.weathered_material("KA_MAT_DarkHammeredSteel", (0.055, 0.071, 0.070),
                                  0.78, 0.66, 130.0, 0.16, 0.0012, 0.58, 0.82)
    brass = ka.weathered_material("KA_MAT_PatinatedOldBrass", (0.31, 0.225, 0.073),
                                  0.62, 0.72, 72.0, 0.12, 0.0010, 0.64, 0.88)
    dark = ka.weathered_material("KA_MAT_Cuirass_Seams", (0.024, 0.033, 0.032),
                                 0.72, 0.79, 90.0, 0.12, 0.0008, 0.72, 0.92)

    ka.lathe("Cuirass_Anatomical_Shell", TORSO_PROFILE, steel, geo,
             segments=64, cap=False)
    ka.lathe("Cuirass_High_Gorget", [
        (1.425, 0.156, 0.095), (1.445, 0.160, 0.100),
        (1.510, 0.121, 0.086), (1.535, 0.104, 0.078),
        (1.542, 0.100, 0.076),
    ], steel, geo, segments=48, cap=False)

    # Schmale Doppelnaht über Brust und Taille; asymmetrische Endpunkte wirken handgeschmiedet.
    for sign in (-1.0, 1.0):
        path = [torso_surface(sign * x, z, 0.004)
                for x, z in ((0.055, 1.455), (0.105, 1.405),
                             (0.130, 1.330), (0.112, 1.245),
                             (0.092, 1.155), (0.115, 1.045),
                             (0.135, 0.965))]
        ka.swept_tube("Cuirass_Raised_Seam_" + ("L" if sign > 0 else "R"),
                      path, 0.0025, dark, geo, sides=8)

    # Gerollte Messingkante am unteren Brustpanzer und an der Kragenbasis.
    hem = [torso_surface(0.151 * math.sin(theta), 0.947, 0.004)
           for theta in [2.0 * math.pi * i / 72.0 for i in range(72)]]
    ka.swept_tube("Cuirass_Lower_Bead", hem, 0.0032, brass, geo,
                  sides=8, cyclic=True)
    collar_edge = [(0.158 * math.sin(2.0 * math.pi * i / 72.0),
                    -0.098 * math.cos(2.0 * math.pi * i / 72.0), 1.451)
                   for i in range(72)]
    ka.swept_tube("Gorget_Lower_Bead", collar_edge, 0.0030, brass, geo,
                  sides=8, cyclic=True)

    add_sun_brooch(geo, brass, steel)
    add_collar_chain(geo, brass)

    armature = ka.create_standard_armature("ARM_Cuirass", rig_collection)
    cuirass = ka.finalize_asset(geo, ASSET, origin=(0.0, 0.0, 0.0), uv_margin=0.010)
    ka.bind_mesh_to_armature(cuirass, armature,
                             bone_names=("pelvis", "spine_01", "spine_02", "spine_03", "neck_01"))
    cuirass["fit_rig"] = "KA_1.75m_A_Pose_ZUp_front_-Y"
    cuirass["origin_note"] = "Character root / feet plane"
    return cuirass


if __name__ == "__main__":
    build()
