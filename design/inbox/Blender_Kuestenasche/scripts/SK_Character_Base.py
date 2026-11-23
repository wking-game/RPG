"""Prozeduraler 1,75-m-Basiskörper mit getrenntem Haar-Mesh und A-Pose-Rig."""

from pathlib import Path
import math
import sys

import bpy

sys.dont_write_bytecode = True
SCRIPT_DIR = Path(__file__).resolve().parent
if str(SCRIPT_DIR) not in sys.path:
    sys.path.insert(0, str(SCRIPT_DIR))
import kustenasche_common as ka


ASSET = "SK_Character_Base"
COLLECTION = "06_SK_Character_Base"


def build_body_mesh(collection, materials):
    skin, eye_white, iris, pupil, lip, hair = materials

    # Füße, Beine und Gelenke; Profile besitzen gleichmäßige Quad-Ring-Loops.
    for sign in (-1.0, 1.0):
        side = "L" if sign > 0 else "R"
        x = sign * 0.098
        ka.ellipsoid("Body_Foot_" + side, (sign * 0.100, -0.055, 0.055),
                     (0.052, 0.125, 0.055), skin, collection, 20, 10)
        ka.lathe("Body_Calf_" + side, [
            (0.095, 0.038, 0.048), (0.120, 0.044, 0.054),
            (0.205, 0.050, 0.066), (0.315, 0.063, 0.075),
            (0.405, 0.061, 0.073), (0.475, 0.053, 0.063),
            (0.535, 0.049, 0.059),
        ], skin, collection, segments=24, center=(x, 0.0))
        ka.ellipsoid("Body_Knee_" + side, (sign * 0.098, -0.002, 0.535),
                     (0.052, 0.064, 0.057), skin, collection, 20, 10)
        ka.lathe("Body_Thigh_" + side, [
            (0.520, 0.052, 0.061), (0.560, 0.060, 0.070),
            (0.640, 0.074, 0.084), (0.760, 0.083, 0.097),
            (0.860, 0.081, 0.096), (0.930, 0.073, 0.087),
            (0.985, 0.058, 0.070),
        ], skin, collection, segments=24, center=(x, 0.0))

    # Weiblicher, athletischer Rumpf mit leicht eingezogener Taille und Schulterbogen.
    ka.lathe("Body_Torso_Anatomical", [
        (0.875, 0.135, 0.092), (0.920, 0.151, 0.101),
        (0.985, 0.157, 0.105), (1.060, 0.133, 0.085),
        (1.130, 0.151, 0.094), (1.225, 0.178, 0.106),
        (1.305, 0.190, 0.111), (1.375, 0.198, 0.110),
        (1.425, 0.184, 0.099), (1.465, 0.147, 0.083),
        (1.485, 0.061, 0.058),
    ], skin, collection, segments=48, cap=False)

    # Zurückhaltende Brustform bleibt unter dem späteren Brustpanzer verborgen.
    for sign in (-1.0, 1.0):
        ka.ellipsoid("Body_Chest_Form_" + ("L" if sign > 0 else "R"),
                     (sign * 0.066, -0.077, 1.285),
                     (0.067, 0.036, 0.058), skin, collection, 20, 10)

    ka.lathe("Body_Neck", [
        (1.455, 0.050, 0.052), (1.480, 0.052, 0.054),
        (1.535, 0.052, 0.055), (1.590, 0.057, 0.058),
        (1.605, 0.060, 0.060),
    ], skin, collection, segments=24, cap=False)

    # Arme in derselben A-Pose wie das gemeinsame Rüstungs-Rig.
    for sign in (-1.0, 1.0):
        side = "L" if sign > 0 else "R"
        shoulder = (sign * 0.185, 0.0, 1.445)
        elbow = (sign * 0.385, 0.0, 1.285)
        wrist = (sign * 0.535, -0.005, 1.135)
        ka.ellipsoid("Body_Shoulder_" + side,
                     (sign * 0.195, 0.0, 1.430), (0.080, 0.084, 0.083),
                     skin, collection, 20, 10)
        upper = [tuple((1.0 - t) * a + t * b for a, b in zip(shoulder, elbow))
                 for t in (0.0, 0.16, 0.36, 0.62, 0.84, 1.0)]
        ka.swept_tube("Body_Upperarm_" + side, upper,
                      (0.052, 0.058, 0.060, 0.055, 0.048, 0.043),
                      skin, collection, sides=16)
        ka.ellipsoid("Body_Elbow_" + side, elbow,
                     (0.046, 0.050, 0.047), skin, collection, 16, 8)
        fore = [tuple((1.0 - t) * a + t * b for a, b in zip(elbow, wrist))
                for t in (0.0, 0.18, 0.38, 0.62, 0.84, 1.0)]
        ka.swept_tube("Body_Forearm_" + side, fore,
                      (0.044, 0.048, 0.052, 0.047, 0.039, 0.033),
                      skin, collection, sides=16)
        ka.ellipsoid("Body_Palm_" + side,
                     (sign * 0.557, -0.010, 1.095),
                     (0.040, 0.025, 0.054), skin, collection, 16, 8)
        for finger in range(4):
            offset = (finger - 1.5) * 0.014
            x0 = sign * 0.565 + offset
            x1 = x0 + sign * (0.006 - 0.002 * abs(finger - 1.5))
            length = 0.063 - 0.009 * abs(finger - 1.5)
            ka.swept_tube("Body_Finger_%s_%d" % (side, finger), [
                (x0, -0.019, 1.075),
                (x1, -0.021, 1.075 - length * 0.55),
                (x1, -0.020, 1.075 - length),
            ], (0.0080, 0.0065, 0.0040), skin, collection, sides=8)
        ka.swept_tube("Body_Thumb_" + side, [
            (sign * 0.535, -0.012, 1.110),
            (sign * 0.522, -0.027, 1.088),
            (sign * 0.520, -0.028, 1.066),
        ], (0.010, 0.008, 0.0045), skin, collection, sides=8)

    # Kopf mit klar lesbaren, dezenten Gesichtsformen; Blickrichtung -Y.
    ka.ellipsoid("Body_Head_Skull", (0.0, 0.0, 1.642),
                 (0.078, 0.091, 0.104), skin, collection, 24, 14)
    ka.ellipsoid("Body_Chin", (0.0, -0.039, 1.574),
                 (0.045, 0.046, 0.027), skin, collection, 18, 10)
    for sign in (-1.0, 1.0):
        side = "L" if sign > 0 else "R"
        ka.ellipsoid("Body_Ear_" + side, (sign * 0.078, 0.0, 1.635),
                     (0.014, 0.023, 0.030), skin, collection, 14, 8)
        ka.ellipsoid("Body_Cheek_" + side,
                     (sign * 0.045, -0.063, 1.620),
                     (0.030, 0.026, 0.025), skin, collection, 16, 8)
        ka.ellipsoid("Body_Eye_White_" + side,
                     (sign * 0.030, -0.083, 1.665),
                     (0.014, 0.006, 0.008), eye_white, collection, 14, 8)
        ka.ellipsoid("Body_Eye_Iris_" + side,
                     (sign * 0.030, -0.089, 1.665),
                     (0.0055, 0.0025, 0.006), iris, collection, 12, 8)
        ka.ellipsoid("Body_Eye_Pupil_" + side,
                     (sign * 0.030, -0.091, 1.665),
                     (0.0025, 0.0015, 0.0035), pupil, collection, 10, 6)
        brow = [(sign * 0.014, -0.085, 1.681),
                (sign * 0.030, -0.090, 1.685),
                (sign * 0.048, -0.081, 1.680)]
        ka.swept_tube("Body_Brow_" + side, brow,
                      (0.0035, 0.0040, 0.0025), hair, collection, sides=6)

    ka.ellipsoid("Body_Nose_Bridge", (0.0, -0.091, 1.638),
                 (0.011, 0.018, 0.025), skin, collection, 14, 8)
    ka.ellipsoid("Body_Nose_Tip", (0.0, -0.105, 1.625),
                 (0.014, 0.013, 0.010), skin, collection, 14, 8)
    ka.ellipsoid("Body_Lips", (0.0, -0.088, 1.595),
                 (0.020, 0.005, 0.006), lip, collection, 16, 8)


def build_hair_mesh(collection, material):
    # Schlichte Kopfhaube plus getrennte, leicht wellige Strähnen bis zur Schulter.
    ka.lathe("Hair_Scalp_Cap", [
        (1.638, 0.072, 0.085), (1.652, 0.080, 0.094),
        (1.680, 0.080, 0.093), (1.710, 0.071, 0.083),
        (1.735, 0.052, 0.062), (1.748, 0.026, 0.032),
        (1.750, 0.001, 0.001),
    ], material, collection, segments=32, cap=True)

    strand_id = 0
    strand_count = 25
    for index in range(strand_count):
        theta = -2.90 + 5.80 * index / (strand_count - 1)
        if abs(theta) < 0.48:
            continue  # parting leaves forehead and eyes open
        start_x = 0.075 * math.sin(theta)
        start_y = -0.087 * math.cos(theta)
        phase = index * 1.71
        points = []
        radius_profile = []
        samples = 22
        for step in range(samples):
            t = step / (samples - 1)
            wave = math.sin(t * 3.4 * math.pi + phase)
            x = start_x + 0.010 * wave * (0.2 + t)
            y = start_y + (0.040 + 0.045 * abs(math.cos(theta))) * t
            z = 1.710 - (0.255 + 0.018 * math.sin(phase)) * t
            points.append((x, y, z))
            radius_profile.append(0.0105 * (1.0 - 0.83 * t) + 0.0012)
        ka.swept_tube("Hair_Wavy_Lock_%02d" % strand_id,
                      points, radius_profile, material, collection, sides=8)
        strand_id += 1

    # Zwei vordere Seitensträhnen rahmen das Gesicht, ohne die Augen zu verdecken.
    for sign in (-1.0, 1.0):
        for strand in range(3):
            points = []
            radii = []
            for step in range(20):
                t = step / 19.0
                x = sign * (0.060 + strand * 0.008 + 0.006 * math.sin(t * 2.7 * math.pi + strand))
                y = -0.060 + 0.045 * t + 0.006 * math.sin(t * 2.0 * math.pi + strand)
                z = 1.705 - (0.205 + strand * 0.018) * t
                points.append((x, y, z))
                radii.append(0.0090 * (1.0 - 0.75 * t) + 0.001)
            ka.swept_tube("Hair_Face_Lock_%s_%d" % ("L" if sign > 0 else "R", strand),
                          points, radii, material, collection, sides=8)


def build():
    ka.prepare_scene()
    asset_collection = ka.new_asset_collection(COLLECTION)
    geo = ka.ensure_collection("GEO_Character_Base", asset_collection)
    hair_collection = ka.ensure_collection("HAIR_Character_Base", asset_collection)
    rig_collection = ka.ensure_collection("RIG_Character_Base", asset_collection)

    skin = ka.weathered_material("KA_MAT_BaseSkin", (0.42, 0.285, 0.205),
                                 0.0, 0.73, 62.0, 0.11, 0.0007, 0.68, 0.85)
    eye_white = ka.weathered_material("KA_MAT_EyeIvory", (0.72, 0.68, 0.58),
                                      0.0, 0.42, 28.0, 0.025, 0.0002, 0.38, 0.52)
    iris = ka.weathered_material("KA_MAT_SeaGlassGrey", (0.12, 0.24, 0.23),
                                 0.0, 0.48, 45.0, 0.04, 0.0002, 0.42, 0.57)
    pupil = ka.weathered_material("KA_MAT_Pupil", (0.012, 0.015, 0.014),
                                  0.0, 0.35, 35.0, 0.02, 0.0001, 0.30, 0.42)
    lip = ka.weathered_material("KA_MAT_MutedLip", (0.25, 0.105, 0.085),
                                0.0, 0.69, 60.0, 0.08, 0.0002, 0.62, 0.78)
    hair = ka.weathered_material("KA_MAT_AshBrownHair", (0.095, 0.066, 0.043),
                                 0.0, 0.78, 75.0, 0.12, 0.0005, 0.72, 0.90)

    build_body_mesh(geo, (skin, eye_white, iris, pupil, lip, hair))
    build_hair_mesh(hair_collection, hair)
    armature = ka.create_standard_armature("ARM_Character_Base", rig_collection)

    body = ka.finalize_asset(geo, ASSET, origin=(0.0, 0.0, 0.0), uv_margin=0.012)
    ka.bind_mesh_to_armature(body, armature)
    hairstyle = ka.finalize_asset(hair_collection, "SK_Character_Hair",
                                  origin=(0.0, 0.0, 0.0), uv_margin=0.012)
    ka.bind_mesh_to_armature(hairstyle, armature,
                             bone_names=("head", "neck_01", "spine_03"),
                             max_influences=2)
    body["height_m"] = 1.75
    body["origin_note"] = "Ground plane between feet"
    body["rig_name"] = armature.name
    body["pose"] = "A-pose; arms lowered approximately 39 degrees from horizontal"
    hairstyle["separate_hair_mesh"] = True
    hairstyle["rig_name"] = armature.name
    armature["body_mesh"] = body.name
    armature["hair_mesh"] = hairstyle.name
    return body, hairstyle, armature


if __name__ == "__main__":
    build()
