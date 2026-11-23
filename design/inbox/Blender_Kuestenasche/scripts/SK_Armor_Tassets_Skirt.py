"""Erzeugt Hüftgurt, gelenkige Tassets und mehrlagigen geschlitzten Waffenrock."""

from pathlib import Path
import math
import sys

import bpy

sys.dont_write_bytecode = True
SCRIPT_DIR = Path(__file__).resolve().parent
if str(SCRIPT_DIR) not in sys.path:
    sys.path.insert(0, str(SCRIPT_DIR))
import kustenasche_common as ka


ASSET = "SK_Armor_Tassets_Skirt"
COLLECTION = "05_SK_Armor_Tassets_Skirt"


def make_tasset(collection, material, brass, center_x, z_top, z_bottom, name,
                center_y=-0.132, width_top=0.050, width_bottom=0.025):
    rows, columns = 8, 6
    vertices = []
    faces = []
    for back in (False, True):
        for row in range(rows + 1):
            t = row / rows
            z = z_top + (z_bottom - z_top) * t
            width = width_top + (width_bottom - width_top) * t
            for col in range(columns + 1):
                u = -1.0 + 2.0 * col / columns
                hem = 0.006 * (1.0 - abs(u)) * math.sin(math.pi * t)
                y = center_y + 0.020 * (u * u) + 0.008 * math.sin(math.pi * t)
                if back:
                    y += 0.009
                vertices.append((center_x + width * u, y, z - hem))
    grid_size = (rows + 1) * (columns + 1)
    stride = columns + 1
    for layer in range(2):
        offset = layer * grid_size
        for row in range(rows):
            for col in range(columns):
                a = offset + row * stride + col
                b = a + 1
                d = offset + (row + 1) * stride + col
                c = d + 1
                face = (a, b, c, d)
                # Frontseite zeigt nach -Y; die Innenfläche erhält die Gegenrichtung.
                faces.append(tuple(reversed(face)) if layer == 0 else face)
    # Doppelseitige Quad-Randflächen schließen Platte ohne offenen Blechrand.
    for col in range(columns):
        a, b = col, col + 1
        faces.append((a, b, grid_size + b, grid_size + a))
        a, b = rows * stride + col, rows * stride + col + 1
        faces.append((a, grid_size + a, grid_size + b, b))
    for row in range(rows):
        a, b = row * stride, (row + 1) * stride
        faces.append((a, b, grid_size + b, grid_size + a))
        a, b = row * stride + columns, (row + 1) * stride + columns
        faces.append((a, grid_size + a, grid_size + b, b))
    ka.create_mesh(name, vertices, faces, material, collection)

    edge_points = []
    for row in range(rows + 1):
        t = row / rows
        z = z_top + (z_bottom - z_top) * t
        width = width_top + (width_bottom - width_top) * t
        edge_points.append((center_x + width, center_y + 0.020 + 0.008 * math.sin(math.pi * t), z))
    ka.swept_tube(name + "_Brass_Edge", edge_points, 0.0018,
                  brass, collection, sides=6)


def add_chain_hinge(collection, brass, x, z):
    # Zwei ineinanderliegende Ovale an jedem seitlichen Aufhängepunkt.
    for index, dz in enumerate((0.0, -0.010)):
        center = (x, -0.128, z + dz)
        points = [(center[0] + 0.008 * math.cos(2 * math.pi * i / 16),
                   center[1] - 0.001 * math.cos(2 * math.pi * i / 16),
                   center[2] + 0.012 * math.sin(2 * math.pi * i / 16))
                  for i in range(16)]
        ka.swept_tube("Tasset_Hinge_Link_%s_%d" % ("L" if x > 0 else "R", index),
                      points, 0.0018, brass, collection, sides=6, cyclic=True)


def make_cloth_panel(collection, material, panel_index, theta_center):
    rows, columns = 10, 5
    theta_width = math.radians(18.5)
    vertices = []
    faces = []
    for layer in (0, 1):
        for row in range(rows + 1):
            t = row / rows
            z_base = 0.925 + (0.405 - 0.925) * t
            for col in range(columns + 1):
                u = -1.0 + 2.0 * col / columns
                theta = theta_center + theta_width * u
                rx = 0.171 + 0.035 * t
                ry = 0.112 + 0.020 * t
                pleat = 0.0045 * math.sin((u + 1.0) * math.pi * 2.0 + panel_index * 0.75)
                pleat *= math.sin(math.pi * t)
                hem = 0.010 * (0.5 + 0.5 * math.cos((u + 1.0) * math.pi))
                hem += 0.004 * math.sin(theta * 7.0 + panel_index)
                z = z_base - hem * (t ** 5)
                radius_x = rx + pleat - (0.004 if layer else 0.0)
                radius_y = ry + pleat - (0.004 if layer else 0.0)
                vertices.append((radius_x * math.sin(theta),
                                 -radius_y * math.cos(theta), z))
    grid_size = (rows + 1) * (columns + 1)
    stride = columns + 1
    for layer in range(2):
        offset = layer * grid_size
        for row in range(rows):
            for col in range(columns):
                a = offset + row * stride + col
                b = a + 1
                d = offset + (row + 1) * stride + col
                c = d + 1
                face = (a, b, c, d)
                # Radial-Außenfläche zeigt vom Körper weg; die Rückseite liegt innen.
                faces.append(tuple(reversed(face)) if layer == 0 else face)
    # Seitenkanten sind ebenfalls Quads; das ergibt einen dünnen, geschlossenen Stoffstreifen.
    for col in range(columns):
        a, b = col, col + 1
        faces.append((a, b, grid_size + b, grid_size + a))
        a, b = rows * stride + col, rows * stride + col + 1
        faces.append((a, grid_size + a, grid_size + b, b))
    for row in range(rows):
        a, b = row * stride, (row + 1) * stride
        faces.append((a, b, grid_size + b, grid_size + a))
        a, b = row * stride + columns, (row + 1) * stride + columns
        faces.append((a, grid_size + a, grid_size + b, b))
    name = "WarSkirt_Cloth_Panel_%02d" % panel_index
    ka.create_mesh(name, vertices, faces, material, collection)


def build():
    ka.prepare_scene()
    asset_collection = ka.new_asset_collection(COLLECTION)
    geo = ka.ensure_collection("GEO_Tassets_Skirt", asset_collection)
    rig_collection = ka.ensure_collection("RIG_Tassets_Skirt", asset_collection)

    steel = ka.weathered_material("KA_MAT_DarkHammeredSteel", (0.055, 0.071, 0.070),
                                  0.78, 0.66, 130.0, 0.16, 0.0012, 0.58, 0.82)
    brass = ka.weathered_material("KA_MAT_PatinatedOldBrass", (0.31, 0.225, 0.073),
                                  0.62, 0.72, 72.0, 0.12, 0.0010, 0.64, 0.88)
    leather = ka.weathered_material("KA_MAT_DarkGrainedLeather", (0.052, 0.032, 0.023),
                                    0.0, 0.83, 105.0, 0.24, 0.0010, 0.76, 0.94)
    linen = ka.weathered_material("KA_MAT_RoughAshLinen", (0.255, 0.225, 0.170),
                                  0.0, 0.91, 58.0, 0.28, 0.0015, 0.84, 0.98)

    # Untergurt aus genarbtem Leder plus genieteter Metallsteg.
    ka.lathe("Waist_Belt_Leather", [
        (0.914, 0.164, 0.108), (0.920, 0.178, 0.116),
        (0.958, 0.178, 0.116), (0.966, 0.164, 0.108),
    ], leather, geo, segments=64, cap=False)
    ka.lathe("Waist_Belt_Rim", [
        (0.919, 0.180, 0.117), (0.925, 0.182, 0.118),
        (0.930, 0.180, 0.117), (0.956, 0.180, 0.117),
        (0.961, 0.182, 0.118), (0.966, 0.180, 0.117),
    ], steel, geo, segments=64, cap=False)

    # Schlichte, breite Schnalle mit eingelassenem offenem Ringzeichen.
    ka.ellipsoid("Belt_Buckle_Base", (0.0, -0.121, 0.940),
                 (0.032, 0.008, 0.024), brass, geo, 20, 10)
    ka.ellipsoid("Belt_Buckle_Inset", (0.0, -0.129, 0.940),
                 (0.021, 0.004, 0.014), leather, geo, 16, 8)
    ring = [(0.014 * math.cos(2 * math.pi * i / 32), -0.134,
             0.940 + 0.010 * math.sin(2 * math.pi * i / 32))
            for i in range(32)]
    ka.swept_tube("Belt_Buckle_Open_Ring", ring, 0.0018, brass,
                  geo, sides=6, cyclic=True)

    # Längliche Schuppenplatten: zwei Reihen vorn sowie seitliche Tassets.
    for sign in (-1.0, 1.0):
        side = "L" if sign > 0 else "R"
        add_chain_hinge(geo, brass, sign * 0.145, 0.910)
        make_tasset(geo, steel, brass, sign * 0.070, 0.922, 0.570,
                    "Tasset_Front_Outer_" + side, center_y=-0.135,
                    width_top=0.052, width_bottom=0.030)
        make_tasset(geo, steel, brass, sign * 0.070, 0.855, 0.650,
                    "Tasset_Front_Inner_" + side, center_y=-0.151,
                    width_top=0.044, width_bottom=0.022)
        make_tasset(geo, steel, brass, sign * 0.157, 0.913, 0.625,
                    "Tasset_Side_" + side, center_y=-0.005,
                    width_top=0.042, width_bottom=0.022)
        for rivet in range(5):
            z = 0.900 - 0.065 * rivet
            ka.ellipsoid("Tasset_Rivet_%s_%d" % (side, rivet),
                         (sign * 0.070, -0.166, z),
                         (0.0045, 0.003, 0.0045), brass, geo, 10, 6)

    # Acht grobe Stoffbahnen mit bewusst freiem Vorder- und Rückenschlitz.
    # Der seitliche Überstand verdeckt die Nähte, ohne die Beinsilhouette zu verschließen.
    for panel in range(8):
        theta = 2.0 * math.pi * (panel + 0.5) / 8.0
        make_cloth_panel(geo, linen, panel, theta)

    armature = ka.create_standard_armature("ARM_Tassets_Skirt", rig_collection)
    skirt = ka.finalize_asset(geo, ASSET, origin=(0.0, 0.0, 0.0), uv_margin=0.012)
    ka.bind_mesh_to_armature(skirt, armature,
                             bone_names=("pelvis", "spine_01", "thigh_l", "thigh_r",
                                         "calf_l", "calf_r"))
    skirt["fit_rig"] = "KA_1.75m_A_Pose_ZUp_front_-Y"
    skirt["origin_note"] = "Character root / feet plane"
    skirt["cloth_panels"] = 8
    return skirt


if __name__ == "__main__":
    build()
