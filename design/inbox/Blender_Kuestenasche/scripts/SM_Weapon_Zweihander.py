"""Erzeugt einen 1,45-m-Küstenasche-Zweihänder für Blender 4.2+."""

from pathlib import Path
import math
import sys

import bpy

sys.dont_write_bytecode = True
SCRIPT_DIR = Path(__file__).resolve().parent
if str(SCRIPT_DIR) not in sys.path:
    sys.path.insert(0, str(SCRIPT_DIR))
import kustenasche_common as ka


ASSET = "SM_Weapon_Zweihander"
COLLECTION = "01_SM_Weapon_Zweihander"


def build_blade(collection, steel, edge_steel):
    # Lokale Achse +Z; Hohlkehle wird als flache Vertiefung in beiden Klingenflächen modelliert.
    stations = [
        (0.170, 0.027, 0.0048),
        (0.235, 0.041, 0.0080),
        (0.390, 0.046, 0.0085),
        (0.720, 0.044, 0.0080),
        (0.980, 0.038, 0.0072),
        (1.120, 0.027, 0.0060),
        (1.190, 0.012, 0.0040),
        (1.215, 0.0012, 0.0025),
    ]
    across = 10
    vertices = []
    faces = []

    def index(row, col, side):
        return row * (across + 1) * 2 + col * 2 + side

    for z, half_width, thickness in stations:
        for col in range(across + 1):
            u = -1.0 + 2.0 * col / across
            x = u * half_width
            # Eine sanfte, breite Rinne statt einer scharf eingeschnittenen Zierlinie.
            fuller = 0.0020 * max(0.0, 1.0 - abs(u) / 0.38)
            face_depth = max(0.0008, thickness * 0.5 - fuller)
            vertices.append((x, -face_depth, z))
            vertices.append((x, face_depth, z))

    for row in range(len(stations) - 1):
        for col in range(across):
            f0 = index(row, col, 0)
            f1 = index(row, col + 1, 0)
            f2 = index(row + 1, col + 1, 0)
            f3 = index(row + 1, col, 0)
            faces.append((f0, f1, f2, f3))
            b0 = index(row, col, 1)
            b1 = index(row, col + 1, 1)
            b2 = index(row + 1, col + 1, 1)
            b3 = index(row + 1, col, 1)
            faces.append((b3, b2, b1, b0))

    for row in range(len(stations) - 1):
        for col in (0, across):
            f0, f1 = index(row, col, 0), index(row + 1, col, 0)
            b0, b1 = index(row, col, 1), index(row + 1, col, 1)
            faces.append((f0, f1, b1, b0))

    for col in range(across):
        faces.append((index(0, col, 0), index(0, col, 1),
                      index(0, col + 1, 1), index(0, col + 1, 0)))
        last = len(stations) - 1
        faces.append((index(last, col, 0), index(last, col + 1, 0),
                      index(last, col + 1, 1), index(last, col, 1)))
    blade = ka.create_mesh("Blade_Fuller_QuadGrid", vertices, faces, steel, collection)
    for sign in (-1.0, 1.0):
        edge_path = [(sign * half_width * 0.985, 0.0, z)
                     for z, half_width, _thickness in stations]
        edge_radii = [0.0010 + 0.0005 * min(1.0, half_width / 0.04)
                      for _z, half_width, _thickness in stations]
        ka.swept_tube("Blade_Edge_Wear_" + ("L" if sign < 0 else "R"),
                      edge_path, edge_radii, edge_steel, collection, sides=6)
    return blade


def build_guard(collection, brass):
    # Je Seite ein geschwungener, zum Ende eingerollter Schmiedestab.
    for sign in (-1.0, 1.0):
        coords = [
            (0.000, 0.000, 0.178),
            (sign * 0.075, 0.000, 0.184),
            (sign * 0.160, 0.000, 0.205),
            (sign * 0.235, 0.000, 0.247),
            (sign * 0.292, 0.000, 0.303),
            (sign * 0.315, 0.000, 0.345),
            (sign * 0.299, 0.000, 0.365),
            (sign * 0.274, 0.000, 0.354),
            (sign * 0.270, 0.000, 0.333),
        ]
        radii = [0.018, 0.019, 0.018, 0.016, 0.014, 0.012, 0.010, 0.008, 0.006]
        ka.swept_tube("Guard_Curled_" + ("L" if sign < 0 else "R"),
                      coords, radii, brass, collection, sides=12)


def build_grip(collection, wood, leather, brass):
    # Zweiteiliger Griff; die schmale Messingmanschette sitzt genau um z=0.
    ka.lathe("Grip_Lower_Leather", [
        (-0.158, 0.018, 0.018), (-0.150, 0.023, 0.022),
        (-0.025, 0.022, 0.021), (-0.014, 0.018, 0.018),
    ], leather, collection, segments=20)
    ka.lathe("Grip_Upper_Wood", [
        (0.014, 0.018, 0.018), (0.025, 0.022, 0.021),
        (0.142, 0.023, 0.022), (0.158, 0.018, 0.018),
    ], wood, collection, segments=20)
    ka.lathe("Grip_Center_Ring", [
        (-0.012, 0.019, 0.019), (-0.008, 0.027, 0.025),
        (0.008, 0.027, 0.025), (0.012, 0.019, 0.019),
    ], brass, collection, segments=24)
    for z in (-0.148, -0.020, 0.020, 0.148):
        ka.lathe("Grip_End_Collar_" + str(z).replace("-", "m").replace(".", "_"), [
            (z - 0.004, 0.022, 0.021), (z, 0.026, 0.024),
            (z + 0.004, 0.022, 0.021),
        ], brass, collection, segments=20)

    # Dünne, leicht unregelmäßige Ledernaht als geschlossener Quad-Rohrzug.
    points = []
    turns = 7.0
    count = 210
    for i in range(count):
        t = i / (count - 1)
        angle = 2.0 * 3.141592653589793 * turns * t
        z = -0.140 + 0.105 * t
        radius = 0.0227 + 0.0005 * (0.5 + 0.5 * math.sin(13.0 * angle))
        points.append((radius * math.cos(angle),
                       radius * math.sin(angle), z))
    ka.swept_tube("Grip_Leather_Crosswrap", points, 0.0013, leather,
                  collection, sides=6)


def build_dragon_pommel(collection, steel, brass, dark):
    # Vereinfachter Drachenkopf: Schädel, Schnauze, Kiefer, Hörner und dunkle Augen.
    ka.lathe("Pommel_Neck", [
        (-0.191, 0.019, 0.018), (-0.180, 0.026, 0.023),
        (-0.163, 0.022, 0.020),
    ], brass, collection, segments=20)
    ka.ellipsoid("Pommel_Dragon_Skull", (0.0, 0.0, -0.207),
                 (0.050, 0.044, 0.027), steel, collection, 20, 10)
    ka.ellipsoid("Pommel_Dragon_Muzzle", (0.0, -0.038, -0.220),
                 (0.032, 0.025, 0.017), steel, collection, 16, 8)
    ka.ellipsoid("Pommel_Dragon_Jaw", (0.0, -0.030, -0.228),
                 (0.028, 0.021, 0.006), brass, collection, 16, 8)
    for sign in (-1.0, 1.0):
        ka.swept_tube("Pommel_Horn_" + ("L" if sign < 0 else "R"), [
            (sign * 0.025, 0.000, -0.194),
            (sign * 0.038, 0.002, -0.180),
            (sign * 0.044, -0.002, -0.169),
        ], [0.009, 0.006, 0.0015], steel, collection, sides=8)
        ka.ellipsoid("Pommel_Eye_" + ("L" if sign < 0 else "R"),
                     (sign * 0.022, -0.066, -0.204),
                     (0.006, 0.003, 0.005), brass, collection, 12, 6)
        ka.ellipsoid("Pommel_Eye_Inset_" + ("L" if sign < 0 else "R"),
                     (sign * 0.022, -0.069, -0.204),
                     (0.0025, 0.0015, 0.003), dark, collection, 10, 6)


def build():
    ka.prepare_scene()
    collection = ka.new_asset_collection(COLLECTION)
    steel = ka.weathered_material("KA_MAT_DarkHammeredSteel", (0.055, 0.071, 0.070),
                                  0.78, 0.66, 130.0, 0.16, 0.0012, 0.58, 0.82)
    edge_steel = ka.weathered_material("KA_MAT_EdgeWornSteel", (0.105, 0.125, 0.120),
                                       0.82, 0.60, 105.0, 0.10, 0.0008, 0.54, 0.76)
    brass = ka.weathered_material("KA_MAT_PatinatedOldBrass", (0.31, 0.225, 0.073),
                                  0.62, 0.72, 72.0, 0.12, 0.0010, 0.64, 0.88)
    wood = ka.weathered_material("KA_MAT_SmokedHardwood", (0.12, 0.075, 0.038),
                                 0.0, 0.78, 34.0, 0.20, 0.0010, 0.70, 0.90)
    leather = ka.weathered_material("KA_MAT_DarkGrainedLeather", (0.052, 0.032, 0.023),
                                    0.0, 0.83, 105.0, 0.24, 0.0010, 0.76, 0.94)
    dark = ka.weathered_material("KA_MAT_BlackenedRecess", (0.008, 0.012, 0.011),
                                 0.18, 0.88, 95.0, 0.08, 0.0005, 0.82, 0.96)

    build_blade(collection, steel, edge_steel)
    build_guard(collection, brass)
    build_grip(collection, wood, leather, brass)
    build_dragon_pommel(collection, steel, brass, dark)
    sword = ka.finalize_asset(collection, ASSET, origin=(0.0, 0.0, 0.0), uv_margin=0.008)
    sword["overall_length_m"] = 1.45
    sword["origin_note"] = "Grip center / hand socket at world (0, 0, 0)"
    sword["blade_axis"] = "+Z"
    return sword


if __name__ == "__main__":
    build()
