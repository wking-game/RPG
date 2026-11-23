"""Erzeugt einen geschlossenen, geschlitzten Visierhelm mit Halsbasis-Pivot."""

from pathlib import Path
import math
import sys

import bpy

sys.dont_write_bytecode = True
SCRIPT_DIR = Path(__file__).resolve().parent
if str(SCRIPT_DIR) not in sys.path:
    sys.path.insert(0, str(SCRIPT_DIR))
import kustenasche_common as ka


ASSET = "SM_Armor_Helmet"
COLLECTION = "02_SM_Armor_Helmet"

# Die vordere Helmseite zeigt nach -Y. Z=0 liegt an der Halsbasis.
PROFILE = [
    (0.010, 0.062, 0.073),
    (0.022, 0.073, 0.082),
    (0.050, 0.082, 0.095),
    (0.075, 0.087, 0.102),
    (0.104, 0.090, 0.105),
    (0.120, 0.090, 0.105),
    (0.150, 0.089, 0.104),
    (0.166, 0.088, 0.103),
    (0.174, 0.088, 0.103),
    (0.210, 0.086, 0.101),
    (0.216, 0.084, 0.099),
    (0.230, 0.080, 0.095),
    (0.255, 0.070, 0.084),
    (0.280, 0.054, 0.068),
    (0.300, 0.031, 0.045),
    (0.305, 0.012, 0.020),
]

# Winkel (rad) um die Vorderachse und Höhenband in Metern.
SLOTS = [
    (-0.42, -0.31, 0.174, 0.210),
    (0.31, 0.42, 0.174, 0.210),
    (-0.66, -0.56, 0.120, 0.165),
    (-0.25, -0.16, 0.120, 0.165),
    (0.16, 0.25, 0.120, 0.165),
    (0.56, 0.66, 0.120, 0.165),
]


def shell_radii(z):
    if z <= PROFILE[0][0]:
        return PROFILE[0][1], PROFILE[0][2]
    if z >= PROFILE[-1][0]:
        return PROFILE[-1][1], PROFILE[-1][2]
    for i in range(len(PROFILE) - 1):
        z0, x0, y0 = PROFILE[i]
        z1, x1, y1 = PROFILE[i + 1]
        if z0 <= z <= z1:
            t = (z - z0) / (z1 - z0)
            return x0 + (x1 - x0) * t, y0 + (y1 - y0) * t
    return PROFILE[-1][1], PROFILE[-1][2]


def surface_point(theta, z, offset=0.0):
    rx, ry = shell_radii(z)
    return ((rx + offset) * math.sin(theta),
            -(ry + offset) * math.cos(theta), z)


def build_shell(collection, steel, cut_slots=True, name="Helmet_OuterShell",
                radius_scale=1.0, dark_material=None):
    segments = 96
    vertices = []
    faces = []
    for z, rx, ry in PROFILE:
        rx *= radius_scale
        ry *= radius_scale
        for segment in range(segments):
            theta = -math.pi + 2.0 * math.pi * segment / segments
            vertices.append((rx * math.sin(theta), -ry * math.cos(theta), z))

    for row in range(len(PROFILE) - 1):
        z0, z1 = PROFILE[row][0], PROFILE[row + 1][0]
        z_mid = 0.5 * (z0 + z1)
        for segment in range(segments):
            theta0 = -math.pi + 2.0 * math.pi * segment / segments
            theta1 = -math.pi + 2.0 * math.pi * (segment + 1) / segments
            theta_mid = 0.5 * (theta0 + theta1)
            omit = False
            if cut_slots:
                for angle_min, angle_max, height_min, height_max in SLOTS:
                    if (angle_min <= theta_mid <= angle_max and
                            height_min <= z_mid <= height_max):
                        omit = True
                        break
            if omit:
                continue
            a = row * segments + segment
            d = row * segments + (segment + 1) % segments
            c = (row + 1) * segments + (segment + 1) % segments
            b = (row + 1) * segments + segment
            # Ring- und Höhenkanten als geschlossene Quad-Flächen.
            faces.append((a, d, c, b))
    # Die Helmkrone ist geschlossen; der untere Halsabschluss bleibt als Öffnung erhalten.
    last_ring = (len(PROFILE) - 1) * segments
    faces.append(tuple(last_ring + i for i in range(segments)))
    return ka.create_mesh(name, vertices, faces,
                          steel if dark_material is None else dark_material,
                          collection)


def add_shell_trim(collection, brass):
    # Zierdraht auf der Stirnseite folgt der Helmkrümmung und bleibt sehr zurückhaltend.
    brow = [surface_point(theta, 0.219, 0.0028)
            for theta in [math.radians(-43 + i * 2.0) for i in range(44)]]
    ka.swept_tube("Relief_Brow_Arc", brow, 0.0023, brass, collection, sides=8)

    for sign in (-1.0, 1.0):
        side_path = []
        for i in range(28):
            t = i / 27.0
            theta = sign * (0.82 - 0.13 * t)
            z = 0.078 + 0.195 * t
            side_path.append(surface_point(theta, z, 0.0022))
        ka.swept_tube("Relief_Temple_Line_" + ("L" if sign < 0 else "R"),
                      side_path, [0.0028 * (1.0 - 0.35 * i / 27.0) for i in range(28)],
                      brass, collection, sides=8)

    # Schmale goldene Nasenrippe, zwischen Sehschlitzen und Stirnmedaillon.
    ridge = [surface_point(0.0, 0.084 + 0.137 * i / 24.0, 0.0035)
             for i in range(25)]
    ka.swept_tube("Relief_Center_Ridge", ridge,
                  [0.0038 * (1.0 - 0.45 * i / 24.0) for i in range(25)],
                  brass, collection, sides=8)

    # Ringprofil am Halsabschluss. Eine kleine Unterbrechung vorn hält die Form handgemacht.
    collar_path = []
    for i in range(64):
        theta = -math.pi + 2.0 * math.pi * i / 64.0
        if abs(theta) < 0.12:
            continue
        collar_path.append(surface_point(theta, 0.063, 0.003))
    ka.swept_tube("Neck_Base_Brass_Bead", collar_path, 0.0035,
                  brass, collection, sides=8)


def add_brow_medallion_and_hinges(collection, brass, steel, dark):
    # Kleines, mattes Sonnenzeichen als Anschluss an die Brustbrosche der Rüstung.
    boss_center = surface_point(0.0, 0.244, 0.004)
    ka.ellipsoid("Forehead_Sun_Boss", boss_center,
                 (0.017, 0.0045, 0.017), brass, collection, 20, 10)
    ka.ellipsoid("Forehead_Sun_Core", (boss_center[0], boss_center[1] - 0.004, boss_center[2]),
                 (0.008, 0.0025, 0.008), steel, collection, 16, 8)
    for ray in range(8):
        angle = 2.0 * math.pi * ray / 8.0
        x0, z0 = 0.020 * math.cos(angle), 0.244 + 0.020 * math.sin(angle)
        x1, z1 = 0.026 * math.cos(angle), 0.244 + 0.026 * math.sin(angle)
        rx0, _ry0 = shell_radii(z0)
        rx1, _ry1 = shell_radii(z1)
        p0 = surface_point(math.asin(max(-1.0, min(1.0, x0 / rx0))), z0, 0.0035)
        p1 = surface_point(math.asin(max(-1.0, min(1.0, x1 / rx1))), z1, 0.0035)
        ka.swept_tube("Sun_Ray_%02d" % ray,
                      [p0, p1], 0.0015,
                      brass, collection, sides=6)

    for sign in (-1.0, 1.0):
        theta = sign * 1.02
        p = surface_point(theta, 0.169, 0.004)
        ka.ellipsoid("Visor_Hinge_" + ("L" if sign < 0 else "R"), p,
                     (0.010, 0.004, 0.012), brass, collection, 12, 8)
        # Je zwei kleine Nieten am Stirnband.
        for z in (0.078, 0.093, 0.108):
            rivet = surface_point(sign * 0.94, z, 0.004)
            ka.ellipsoid("Collar_Rivet_%s_%03d" % ("L" if sign < 0 else "R", int(z * 1000)),
                         rivet, (0.0042, 0.0035, 0.0042), brass,
                         collection, 10, 6)


def build():
    ka.prepare_scene()
    collection = ka.new_asset_collection(COLLECTION)
    steel = ka.weathered_material("KA_MAT_DarkHammeredSteel", (0.055, 0.071, 0.070),
                                  0.78, 0.66, 130.0, 0.16, 0.0012, 0.58, 0.82)
    brass = ka.weathered_material("KA_MAT_PatinatedOldBrass", (0.31, 0.225, 0.073),
                                  0.62, 0.72, 72.0, 0.12, 0.0010, 0.64, 0.88)
    inner = ka.weathered_material("KA_MAT_HelmetInterior", (0.010, 0.016, 0.015),
                                  0.28, 0.88, 92.0, 0.07, 0.0004, 0.82, 0.96)

    # Außenhaut mit echten offenen Seh- und Atemschlitzen; dunkle Innenhaut liegt dahinter.
    outer_shell = build_shell(collection, steel, cut_slots=True)
    shell_thickness = outer_shell.modifiers.new("ShellThickness_4mm", "SOLIDIFY")
    shell_thickness.thickness = 0.004
    shell_thickness.offset = -1.0
    shell_thickness.use_even_offset = True
    bpy.ops.object.select_all(action="DESELECT")
    outer_shell.select_set(True)
    bpy.context.view_layer.objects.active = outer_shell
    bpy.ops.object.modifier_apply(modifier=shell_thickness.name)
    build_shell(collection, inner, cut_slots=False, name="Helmet_InnerBacking",
                radius_scale=0.91)

    # Zusätzlicher Kragenflansch aus überlappenden, quad-basierten Profilringen.
    ka.lathe("Neck_Gorget_Flange", [
        (0.018, 0.066, 0.077),
        (0.024, 0.073, 0.084),
        (0.040, 0.083, 0.097),
        (0.051, 0.086, 0.102),
        (0.057, 0.083, 0.098),
    ], steel, collection, segments=48, cap=False)

    add_shell_trim(collection, brass)
    add_brow_medallion_and_hinges(collection, brass, steel, inner)
    helmet = ka.finalize_asset(collection, ASSET, origin=(0.0, 0.0, 0.0), uv_margin=0.010)
    helmet["origin_note"] = "Neck base at world (0, 0, 0)"
    helmet["overall_height_m"] = 0.305
    helmet["front_axis"] = "-Y"
    helmet["slot_count"] = len(SLOTS)
    return helmet


if __name__ == "__main__":
    build()
