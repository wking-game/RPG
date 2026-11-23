"""Procedural, quad-only starting variants for Küstenasche armor.

No bpy primitive operators are used. Main shells are profile-controlled quad
lofts matched to the project's 1.75 m A-pose body; detail meshes are custom
quad strips, panels, or small domes. Run an individual HEAD_*.py / TORSO_*.py
file in Blender 5.x. Each run replaces only its own generated collection.
"""

import math
import sys

import bpy
import bmesh
from mathutils import Vector

# Same A-pose skeleton and distance-based skinning used by the existing set.
sys.dont_write_bytecode = True
from kustenasche_common import create_standard_armature, bind_mesh_to_armature


ROOT = "KA_Armor_Variants"
BODY = {
    "head_origin_z": 1.47,       # neck_01 base in the shared character rig
    "pelvis_origin_z": 0.88,     # pelvis bone head
    "torso_profile": [          # (world z, half-width X, depth radius Y)
        (0.875, .132, .093), (0.925, .151, .100), (1.000, .158, .106),
        (1.065, .145, .094), (1.145, .137, .087), (1.235, .148, .092),
        (1.325, .166, .096), (1.405, .181, .099), (1.445, .184, .096),
        (1.485, .147, .082), (1.515, .078, .066),
    ],
}


def ensure_collection(name, parent):
    col = parent.children.get(name)
    if col is None:
        col = bpy.data.collections.new(name)
        parent.children.link(col)
    return col


def replace_asset_collection(name, slot):
    """Replace exactly the named generated asset collection, preserving scene content."""
    old = bpy.data.collections.get(name)
    if old:
        def remove_tree(collection):
            for child in list(collection.children):
                remove_tree(child)
            for obj in list(collection.objects):
                data = obj.data
                bpy.data.objects.remove(obj, do_unlink=True)
                if data is not None and data.users == 0:
                    if isinstance(data, bpy.types.Mesh):
                        bpy.data.meshes.remove(data)
                    elif isinstance(data, bpy.types.Armature):
                        bpy.data.armatures.remove(data)
            bpy.data.collections.remove(collection, do_unlink=True)
        remove_tree(old)
    root = bpy.data.collections.get(ROOT)
    if root is None:
        root = bpy.data.collections.new(ROOT)
        bpy.context.scene.collection.children.link(root)
    slot_col = ensure_collection("slot_" + slot, root)
    asset_col = bpy.data.collections.new(name)
    slot_col.children.link(asset_col)
    scene = bpy.context.scene
    scene.unit_settings.system = "METRIC"
    scene.unit_settings.scale_length = 1.0
    scene.unit_settings.length_unit = "METERS"
    return asset_col


def setv(node, name, value):
    socket = node.inputs.get(name)
    if socket is not None:
        socket.default_value = value


def pbr(name, color, metallic, roughness, bump_strength=.16, bump_distance=.0018,
        scale=110.0, detail=4.0):
    mat = bpy.data.materials.get(name) or bpy.data.materials.new(name)
    mat.diffuse_color = (*color, 1.0)
    mat.use_nodes = True
    nt = mat.node_tree
    nt.nodes.clear()
    out = nt.nodes.new("ShaderNodeOutputMaterial")
    out.location = (650, 60)
    bs = nt.nodes.new("ShaderNodeBsdfPrincipled")
    bs.location = (400, 60)
    setv(bs, "Base Color", (*color, 1))
    setv(bs, "Roughness", roughness)
    tex = nt.nodes.new("ShaderNodeTexCoord")
    tex.location = (-850, 60)
    noise = nt.nodes.new("ShaderNodeTexNoise")
    noise.location = (-620, 60)
    setv(noise, "Scale", scale)
    setv(noise, "Detail", detail)
    setv(noise, "Roughness", .72)
    nt.links.new(tex.outputs["Generated"], noise.inputs["Vector"])
    rough = nt.nodes.new("ShaderNodeValToRGB")
    rough.location = (-330, 210)
    rough.color_ramp.elements[0].position = .18
    rough.color_ramp.elements[0].color = (max(.05, roughness - .08),) * 3 + (1,)
    rough.color_ramp.elements[1].position = .84
    rough.color_ramp.elements[1].color = (min(.98, roughness + .13),) * 3 + (1,)
    nt.links.new(noise.outputs["Fac"], rough.inputs["Fac"])
    nt.links.new(rough.outputs["Color"], bs.inputs["Roughness"])
    metal = nt.nodes.new("ShaderNodeValToRGB")
    metal.location = (-330, 390)
    spread = .025 if metallic < .1 else .075
    lo = max(0.0, metallic - spread)
    hi = min(1.0, metallic + spread)
    metal.color_ramp.elements[0].position = .18
    metal.color_ramp.elements[0].color = (lo, lo, lo, 1)
    metal.color_ramp.elements[1].position = .82
    metal.color_ramp.elements[1].color = (hi, hi, hi, 1)
    nt.links.new(noise.outputs["Fac"], metal.inputs["Fac"])
    nt.links.new(metal.outputs["Color"], bs.inputs["Metallic"])
    # Fine grain and broad material irregularity both affect the normal.
    bump = nt.nodes.new("ShaderNodeBump")
    bump.location = (130, -130)
    setv(bump, "Strength", bump_strength)
    setv(bump, "Distance", bump_distance)
    height_source = noise.outputs["Fac"]
    mat_key = name.lower()
    if any(token in mat_key for token in ("wool", "linen", "gambeson", "thread")):
        weave = nt.nodes.new("ShaderNodeTexWave")
        weave.location = (-600, -300)
        weave.wave_type = "BANDS"
        weave.bands_direction = "X"
        setv(weave, "Scale", 155.0)
        setv(weave, "Distortion", 2.5)
        setv(weave, "Detail", 2.0)
        nt.links.new(tex.outputs["Generated"], weave.inputs["Vector"])
        relief = nt.nodes.new("ShaderNodeMixRGB")
        relief.location = (-330, -280)
        relief.blend_type = "MULTIPLY"
        relief.inputs[0].default_value = .32
        nt.links.new(noise.outputs["Fac"], relief.inputs[1])
        nt.links.new(weave.outputs["Fac"], relief.inputs[2])
        height_source = relief.outputs["Color"]
    elif metallic > .45:
        hammer = nt.nodes.new("ShaderNodeTexNoise")
        hammer.location = (-600, -300)
        setv(hammer, "Scale", 34.0)
        setv(hammer, "Detail", 2.0)
        setv(hammer, "Roughness", .68)
        nt.links.new(tex.outputs["Generated"], hammer.inputs["Vector"])
        relief = nt.nodes.new("ShaderNodeMixRGB")
        relief.location = (-330, -280)
        relief.blend_type = "MULTIPLY"
        relief.inputs[0].default_value = .30
        nt.links.new(noise.outputs["Fac"], relief.inputs[1])
        nt.links.new(hammer.outputs["Fac"], relief.inputs[2])
        height_source = relief.outputs["Color"]
    elif "leather" in mat_key or "cuir" in mat_key:
        grain = nt.nodes.new("ShaderNodeTexVoronoi")
        grain.location = (-600, -300)
        grain.feature = "DISTANCE_TO_EDGE"
        setv(grain, "Scale", 105.0)
        nt.links.new(tex.outputs["Generated"], grain.inputs["Vector"])
        relief = nt.nodes.new("ShaderNodeMixRGB")
        relief.location = (-330, -280)
        relief.blend_type = "MULTIPLY"
        relief.inputs[0].default_value = .22
        nt.links.new(noise.outputs["Fac"], relief.inputs[1])
        nt.links.new(grain.outputs["Distance"], relief.inputs[2])
        height_source = relief.outputs["Color"]
    nt.links.new(height_source, bump.inputs["Height"])
    nt.links.new(bump.outputs["Normal"], bs.inputs["Normal"])
    color_ramp = nt.nodes.new("ShaderNodeValToRGB")
    color_ramp.location = (-330, -90)
    color_ramp.color_ramp.elements[0].position = .22
    color_ramp.color_ramp.elements[0].color = tuple(c * .72 for c in color) + (1,)
    color_ramp.color_ramp.elements[1].position = .78
    color_ramp.color_ramp.elements[1].color = tuple(min(1, c * 1.12) for c in color) + (1,)
    nt.links.new(noise.outputs["Fac"], color_ramp.inputs["Fac"])
    nt.links.new(color_ramp.outputs["Color"], bs.inputs["Base Color"])
    nt.links.new(bs.outputs["BSDF"], out.inputs["Surface"])
    mat["PBR_Metallic"] = float(metallic)
    mat["PBR_Roughness_Base"] = float(roughness)
    mat["PBR_Procedural_Roughness"] = True
    mat["PBR_Procedural_Metallic"] = True
    mat["PBR_Bump"] = True
    return mat


class MeshBuild:
    def __init__(self):
        self.vertices, self.faces, self.material_ids = [], [], []
        self.materials = []

    def material_index(self, material):
        if material not in self.materials:
            self.materials.append(material)
        return self.materials.index(material)

    def add(self, vertices, faces, material):
        base = len(self.vertices)
        self.vertices.extend(tuple(v) for v in vertices)
        mat_id = self.material_index(material)
        for face in faces:
            if len(face) != 4:
                raise ValueError("Variant geometry must be quad-only")
            self.faces.append(tuple(base + i for i in face))
            self.material_ids.append(mat_id)

    def create(self, name, collection, origin_z, slot, tier):
        # Geometry is authored in character-space, then offset so Blender/UE pivot is anatomical.
        local = [(x, y, z - origin_z) for x, y, z in self.vertices]
        mesh = bpy.data.meshes.new(name + "_Mesh")
        mesh.from_pydata(local, [], self.faces)
        mesh.materials.clear()
        for mat in self.materials:
            mesh.materials.append(mat)
        mesh.update()
        for poly, mat_id in zip(mesh.polygons, self.material_ids):
            poly.material_index = mat_id
            poly.use_smooth = True
        bm = bmesh.new()
        bm.from_mesh(mesh)
        bmesh.ops.recalc_face_normals(bm, faces=bm.faces)
        bm.to_mesh(mesh)
        bm.free()
        obj = bpy.data.objects.new(name, mesh)
        collection.objects.link(obj)
        obj.location = (0, 0, origin_z)
        obj["slot_id"] = "slot_" + slot
        obj["tier"] = tier
        obj["pivot_world_z_m"] = origin_z
        obj["topology"] = "quad-only disconnected shell/detail islands"
        obj["forward_axis"] = "-Y"
        obj["scale_for_UE_meters"] = 1.0
        # Smart UV creates UV0 on all disconnected details; shell loops remain unchanged.
        bpy.ops.object.select_all(action="DESELECT")
        obj.select_set(True)
        bpy.context.view_layer.objects.active = obj
        bpy.ops.object.mode_set(mode="EDIT")
        bpy.ops.mesh.select_all(action="SELECT")
        bpy.ops.uv.smart_project(angle_limit=math.radians(65), island_margin=.008)
        bpy.ops.object.mode_set(mode="OBJECT")
        if mesh.uv_layers:
            mesh.uv_layers[0].name = "UV0"
        rig_collection = ensure_collection("RIG_" + name, collection)
        armature = create_standard_armature("ARM_" + name, rig_collection)
        if slot == "head":
            bind_mesh_to_armature(obj, armature, bone_names=("head",), max_influences=1)
        elif slot == "shoulders":
            bind_mesh_to_armature(
                obj, armature,
                bone_names=("spine_03", "neck_01", "clavicle_l", "clavicle_r",
                            "upperarm_l", "upperarm_r"), max_influences=3)
        elif slot == "arms":
            bind_mesh_to_armature(
                obj, armature,
                bone_names=("clavicle_l", "clavicle_r", "upperarm_l", "upperarm_r",
                            "lowerarm_l", "lowerarm_r"), max_influences=3)
        elif slot == "hands":
            bind_mesh_to_armature(
                obj, armature,
                bone_names=("lowerarm_l", "lowerarm_r", "hand_l", "hand_r"), max_influences=2)
        elif slot == "legs":
            bind_mesh_to_armature(
                obj, armature,
                bone_names=("pelvis", "thigh_l", "thigh_r", "calf_l", "calf_r",
                            "foot_l", "foot_r"), max_influences=3)
        elif slot == "surcoat":
            bind_mesh_to_armature(
                obj, armature,
                bone_names=("pelvis", "spine_01", "spine_02", "spine_03"), max_influences=3)
        elif slot == "cloak":
            bind_mesh_to_armature(
                obj, armature,
                bone_names=("pelvis", "spine_01", "spine_02", "spine_03", "neck_01",
                            "clavicle_l", "clavicle_r"), max_influences=3)
        else:
            bind_mesh_to_armature(
                obj, armature,
                bone_names=("pelvis", "spine_01", "spine_02", "spine_03",
                            "neck_01", "clavicle_l", "clavicle_r"),
                max_influences=3,
            )
        obj["fit_rig"] = "KA_1.75m_A_Pose_ZUp_front_-Y"
        return obj


def ring_loft(profile, segments=48, center=(0.0, 0.0)):
    """Profile-driven elliptical quad loft. Rows: (z, rx, ry[, cx, cy])."""
    verts = []
    for row in profile:
        z, rx, ry = row[:3]
        cx = row[3] if len(row) > 3 else center[0]
        cy = row[4] if len(row) > 4 else center[1]
        for j in range(segments):
            a = 2 * math.pi * j / segments
            verts.append((cx + rx * math.cos(a), cy + ry * math.sin(a), z))
    faces = []
    for r in range(len(profile) - 1):
        for j in range(segments):
            a = r * segments + j
            b = r * segments + (j + 1) % segments
            faces.append((a, b, b + segments, a + segments))
    return verts, faces


def shell_loft(profile, segments=64, material=None, armhole=None, front_slit=None,
               thickness=.0025):
    """Closed thickness shell. Optional side armholes/front slit are genuine openings."""
    outer, faces = ring_loft(profile, segments)
    outer_faces = []
    for face in faces:
        a, b, c, d = face
        zmid = sum(outer[i][2] for i in face) * .25
        ang = math.atan2((outer[a][1] + outer[b][1]) * .5,
                         (outer[a][0] + outer[b][0]) * .5)
        omit = False
        if armhole and armhole[0] <= zmid <= armhole[1]:
            da = min(abs(ang), abs(abs(ang) - math.pi))
            omit = da < armhole[2]
        if front_slit and front_slit[0] <= zmid <= front_slit[1]:
            # -Y is the front. Remove the corresponding shell cells for a true sight opening.
            front_angle = -math.pi / 2
            da = abs((ang - front_angle + math.pi) % (2 * math.pi) - math.pi)
            omit = omit or da < front_slit[2]
        if not omit:
            outer_faces.append(face)
    # An inset copy forms the inner shell; boundary quads join the two surfaces at every opening.
    n = len(outer)
    inner = []
    profile_rows = len(profile)
    for idx, (x, y, z) in enumerate(outer):
        row = profile[idx // segments]
        cx = row[3] if len(row) > 3 else 0.0
        cy = row[4] if len(row) > 4 else 0.0
        dx, dy = x - cx, y - cy
        length = math.hypot(dx, dy) or 1.0
        inner.append((x - thickness * dx / length, y - thickness * dy / length, z))
    edge_use = {}
    for f in outer_faces:
        for i in range(4):
            a, b = f[i], f[(i + 1) % 4]
            key = tuple(sorted((a, b)))
            edge_use.setdefault(key, []).append((a, b))
    all_verts = outer + inner
    all_faces = list(outer_faces) + [tuple(n + i for i in reversed(f)) for f in outer_faces]
    for uses in edge_use.values():
        if len(uses) == 1:
            a, b = uses[0]
            all_faces.append((b, a, n + a, n + b))
    return all_verts, all_faces


def tube_mesh(points, radii, sides=8, closed=False):
    """Quad tube around a designed path; scalar or per-point radii."""
    pts = [Vector(p) for p in points]
    if isinstance(radii, (int, float)):
        radii = [float(radii)] * len(pts)
    verts = []
    for i, point in enumerate(pts):
        if closed:
            tangent = (pts[(i + 1) % len(pts)] - pts[(i - 1) % len(pts)]).normalized()
        elif i == 0:
            tangent = (pts[1] - pts[0]).normalized()
        elif i == len(pts) - 1:
            tangent = (pts[-1] - pts[-2]).normalized()
        else:
            tangent = (pts[i + 1] - pts[i - 1]).normalized()
        ref = Vector((0, 0, 1)) if abs(tangent.z) < .9 else Vector((0, 1, 0))
        u = tangent.cross(ref).normalized()
        v = tangent.cross(u).normalized()
        for j in range(sides):
            a = 2 * math.pi * j / sides
            q = point + radii[i] * (math.cos(a) * u + math.sin(a) * v)
            verts.append(tuple(q))
    faces = []
    spans = len(pts) if closed else len(pts) - 1
    for i in range(spans):
        ni = (i + 1) % len(pts)
        for j in range(sides):
            nj = (j + 1) % sides
            faces.append((i * sides + j, i * sides + nj, ni * sides + nj, ni * sides + j))
    return verts, faces


def dome_mesh(center, normal, radius=.006, height=.003, sides=10):
    """Small raised rivet/fastener head, built as nested quad rings, not a primitive."""
    c = Vector(center)
    n = Vector(normal).normalized()
    u = n.cross(Vector((0, 0, 1)))
    if u.length < .1:
        u = n.cross(Vector((0, 1, 0)))
    u.normalize()
    v = n.cross(u).normalized()
    rings = [(0.0, .90), (height * .48, 1.0), (height * .84, .62), (height, .08)]
    verts = []
    for h, scale in rings:
        for j in range(sides):
            a = 2 * math.pi * j / sides
            p = c + n * h + radius * scale * (math.cos(a) * u + math.sin(a) * v)
            verts.append(tuple(p))
    faces = []
    for r in range(len(rings) - 1):
        for j in range(sides):
            a = r * sides + j
            b = r * sides + (j + 1) % sides
            faces.append((a, b, b + sides, a + sides))
    return verts, faces


def panel_grid(rows, columns, material_shape, thickness=.002):
    """Solid quad panel from rows of points, with quad back and side walls."""
    nr, nc = len(rows), len(rows[0])
    front = [p for row in rows for p in row]
    back = [(x, y + thickness, z) for x, y, z in front]
    verts = front + back
    faces = []
    for r in range(nr - 1):
        for c in range(nc - 1):
            a = r * nc + c
            faces.append((a, a + 1, a + nc + 1, a + nc))
            b = len(front) + a
            faces.append((b + nc, b + nc + 1, b + 1, b))
    boundary = []
    for c in range(nc - 1):
        boundary.append((c, c + 1))
        boundary.append(((nr - 1) * nc + c + 1, (nr - 1) * nc + c))
    for r in range(nr - 1):
        boundary.append(((r + 1) * nc, r * nc))
        boundary.append((r * nc + nc - 1, (r + 1) * nc + nc - 1))
    for a, b in boundary:
        faces.append((a, b, len(front) + b, len(front) + a))
    return verts, faces


def add_tube(build, points, radius, mat, sides=8, closed=False, radii=None):
    verts, faces = tube_mesh(points, radius if radii is None else radii, sides, closed)
    build.add(verts, faces, mat)


def add_dome(build, center, normal, radius, height, mat):
    verts, faces = dome_mesh(center, normal, radius, height)
    build.add(verts, faces, mat)


def interpolate_profile(z, profile):
    if z <= profile[0][0]:
        return profile[0][1:3]
    if z >= profile[-1][0]:
        return profile[-1][1:3]
    for a, b in zip(profile, profile[1:]):
        if a[0] <= z <= b[0]:
            t = (z - a[0]) / (b[0] - a[0])
            return (a[1] * (1 - t) + b[1] * t, a[2] * (1 - t) + b[2] * t)


def torso_point(theta, z, offset=.0, profile=None):
    profile = profile or BODY["torso_profile"]
    rx, ry = interpolate_profile(z, profile)
    # Smooth ellipse with small forward sternum bias.
    x = (rx + offset) * math.cos(theta)
    y = (ry + offset) * math.sin(theta) - .003 * math.cos(theta) ** 2
    return (x, y, z)


def torso_seam(build, theta, z0, z1, mat, radius=.0018, offset=.003, steps=14):
    points = [torso_point(theta, z0 + (z1 - z0) * i / (steps - 1), offset)
              for i in range(steps)]
    add_tube(build, points, radius, mat, 6)


def torso_ring(build, z, mat, radius=.003, offset=.004, segments=64):
    pts = [torso_point(2 * math.pi * i / segments, z, offset) for i in range(segments)]
    add_tube(build, pts, radius, mat, 6, closed=True)


def torso_shell(build, mat, profile=None, armholes=True, thickness=.003):
    profile = profile or BODY["torso_profile"]
    verts, faces = shell_loft(profile, segments=64, armhole=(1.235, 1.445, .31) if armholes else None,
                              thickness=thickness)
    build.add(verts, faces, mat)


def panel_on_front(build, rowspec, material, profile=None, columns=12, offset=.006,
                   thickness=.003):
    """Rows are (z, halfwidth); creates a fitted, filled panel over front -Y torso."""
    profile = profile or BODY["torso_profile"]
    grid = []
    for z, halfwidth in rowspec:
        rx, ry = interpolate_profile(z, profile)
        row = []
        for c in range(columns + 1):
            t = -1 + 2 * c / columns
            x = halfwidth * t
            y = -ry * math.sqrt(max(.04, 1 - (x / max(rx, .001)) ** 2)) - offset
            row.append((x, y, z))
        grid.append(row)
    verts, faces = panel_grid(grid, columns, material, thickness)
    build.add(verts, faces, material)


def mark_rivet_rows(build, z_values, theta_values, mat, radius=.006, offset=.009):
    for z in z_values:
        for theta in theta_values:
            p = torso_point(theta, z, offset)
            normal = Vector((math.cos(theta), math.sin(theta), .05)).normalized()
            add_dome(build, p, normal, radius, radius * .45, mat)


# -- Head slot ---------------------------------------------------------------

def make_head_t1():
    name = "SK_Armor_Head_LeatherCap_T1"
    col = replace_asset_collection(name, "head")
    wool = pbr("KA_Wool_SaltAsh_T1", (.19, .17, .14), 0, .83, .24, .0022, 150)
    leather = pbr("KA_Leather_GreasedDark_T1", (.105, .068, .045), .015, .72, .22, .0018, 185)
    thread = pbr("KA_Thread_RustRed_T1", (.22, .085, .055), 0, .88, .18, .001, 210)
    b = MeshBuild()
    profile = [(.125, .062, .074), (.142, .077, .090), (.175, .087, .100),
               (.225, .086, .097), (.265, .068, .079), (.295, .040, .048),
               (.307, .002, .002)]
    verts, faces = shell_loft(profile, 48, thickness=.002)
    b.add(verts, faces, wool)
    # Sewn leather binding follows the lower opening as a fitted profile band.
    verts, faces = ring_loft([(.130, .063, .075), (.137, .077, .090),
                               (.147, .079, .092)], 48)
    b.add(verts, faces, leather)
    # Front/back crown seams and paired tendon stitches are physical narrow tubes.
    for theta in (-math.pi / 2, math.pi / 2):
        pts = []
        for i in range(13):
            z = .145 + i * (.151 / 12)
            rx, ry = interpolate_profile(z, profile)
            a = theta + .045 * math.sin(i * .48)
            pts.append((rx * math.cos(a) * 1.012, ry * math.sin(a) * 1.012, z))
        add_tube(b, pts, .0018, leather, 6)
        for side in (-1, 1):
            stitch = []
            for i in range(12):
                z = .151 + i * .011
                rx, ry = interpolate_profile(z, profile)
                a = theta + side * .035
                stitch.append((rx * math.cos(a) * 1.018, ry * math.sin(a) * 1.018, z))
            add_tube(b, stitch, .0008, thread, 5)
    # Paired ear/cheek flaps taper with the jaw instead of ending as raw rectangles.
    for sign in (-1, 1):
        grid = []
        for z, width, y in ((.158, .016, -.040), (.127, .024, -.050),
                            (.085, .021, -.047), (.058, .009, -.041)):
            x = sign * (.067 + (.158 - z) * .035)
            grid.append([(x - width / 2, y, z), (x, y - .003, z), (x + width / 2, y, z)])
        verts, faces = panel_grid(grid, 2, leather, .003)
        b.add(verts, faces, leather)
        for t in (.115, .140):
            add_dome(b, (sign * .076, -.054, t), (sign * .35, -1, 0), .0032, .0015, thread)
    obj = b.create(name, col, BODY["head_origin_z"], "head", "T1")
    obj["design_note"] = "close wool cap, greased leather binding, tendon seam and cheek flaps"
    obj["fit_reference"] = "head shell envelope of KA 1.75m A-pose base"
    return obj


def make_head_t2():
    name = "SK_Armor_Head_Spangenhelm_T2"
    col = replace_asset_collection(name, "head")
    iron = pbr("KA_Iron_BogDark_T2", (.115, .135, .13), .78, .69, .20, .0014, 135)
    edge = pbr("KA_Iron_WornEdge_T2", (.23, .245, .22), .82, .58, .12, .0008, 170)
    brass = pbr("KA_Brass_Patinated_T2", (.29, .22, .105), .68, .73, .14, .001, 115)
    leather = pbr("KA_Leather_Liner_T2", (.09, .057, .038), .01, .78, .18, .0015, 160)
    b = MeshBuild()
    profile = [(.075, .064, .076), (.096, .078, .092), (.145, .087, .101),
               (.207, .088, .100), (.259, .075, .086), (.299, .049, .058),
               (.318, .002, .002)]
    verts, faces = shell_loft(profile, 64, thickness=.003)
    b.add(verts, faces, iron)
    # Four forged crown panels are physically separated by raised cross-straps.
    for theta in (0, math.pi / 2, math.pi, -math.pi / 2):
        pts = []
        for i in range(17):
            z = .095 + i * (.217 / 16)
            rx, ry = interpolate_profile(z, profile)
            a = theta + .012 * math.sin(i * .6)
            pts.append((rx * 1.026 * math.cos(a), ry * 1.026 * math.sin(a), z))
        add_tube(b, pts, .0048, edge, 8)
        # Small raised forged studs along the strap, set into the shell.
        for z in (.108, .162, .216, .270):
            rx, ry = interpolate_profile(z, profile)
            a = theta
            center = (rx * 1.04 * math.cos(a), ry * 1.04 * math.sin(a), z)
            normal = (math.cos(a), math.sin(a), .12)
            add_dome(b, center, normal, .0042, .0022, brass)
    # Deep fitted brow hoop, with alternating iron rim and subdued brass rivets.
    verts, faces = ring_loft([(.092, .071, .084), (.101, .080, .094),
                               (.114, .081, .095)], 64)
    b.add(verts, faces, edge)
    # Nasal guard: tapered, convex metal patch between real eye/face opening.
    rows = []
    for z, width in ((.181, .010), (.156, .009), (.121, .007), (.080, .011)):
        rows.append([(-width, -.103, z), (-width * .52, -.110, z),
                     (width * .52, -.110, z), (width, -.103, z)])
    verts, faces = panel_grid(rows, 3, iron, .004)
    b.add(verts, faces, iron)
    # Cheek lames and articulated leather hinge tabs, mirrored, with oval ear clearances.
    for sign in (-1, 1):
        rows = []
        for z, width, x in ((.152, .018, .069), (.128, .024, .076),
                            (.090, .022, .074), (.061, .012, .067)):
            rows.append([(sign * (x - width / 2), -.060, z),
                         (sign * x, -.085, z), (sign * (x + width / 2), -.060, z)])
        verts, faces = panel_grid(rows, 2, iron, .004)
        b.add(verts, faces, iron)
        add_tube(b, [(sign * .055, -.040, .112), (sign * .068, -.055, .119),
                     (sign * .079, -.050, .123)], .003, leather, 6)
        for z in (.103, .139):
            add_dome(b, (sign * .076, -.087, z), (sign * .35, -1, 0), .0037, .0018, brass)
    # Liner roll is dark leather; opening stays clear below the jaw.
    verts, faces = ring_loft([(.071, .064, .076), (.078, .073, .085),
                               (.084, .074, .086)], 64)
    b.add(verts, faces, leather)
    obj = b.create(name, col, BODY["head_origin_z"], "head", "T2")
    obj["design_note"] = "raseneisen spangenhelm, four ribs, nasal, cheek lames and liner"
    return obj


def make_head_t3():
    name = "SK_Armor_Head_PotHelm_T3"
    col = replace_asset_collection(name, "head")
    iron = pbr("KA_Iron_Hammered_T3", (.135, .152, .15), .82, .64, .23, .0017, 145)
    edge = pbr("KA_Iron_BurnishedEdges_T3", (.27, .29, .27), .86, .54, .13, .001, 180)
    brass = pbr("KA_Brass_Muted_T3", (.31, .235, .12), .70, .71, .12, .001, 120)
    dark = pbr("KA_Visor_Shadow_T3", (.018, .024, .023), .25, .88, .08, .0004, 90)
    leather = pbr("KA_Leather_Liner_T3", (.075, .046, .03), .01, .82, .20, .0016, 180)
    b = MeshBuild()
    profile = [(.052, .067, .079), (.073, .080, .096), (.118, .091, .108),
               (.185, .094, .108), (.251, .089, .103), (.297, .071, .084),
               (.325, .042, .051), (.336, .002, .002)]
    verts, faces = shell_loft(profile, 72, front_slit=(.183, .201, .34), thickness=.0035)
    b.add(verts, faces, iron)
    # Inset dark slot backing sits behind the aperture and follows the face curvature.
    rows = []
    for z, halfw in ((.183, .045), (.187, .050), (.197, .050), (.201, .045)):
        row = []
        for i in range(9):
            x = -halfw + (2 * halfw * i / 8)
            row.append((x, -.101 - .006 * (1 - (x / .052) ** 2), z))
        rows.append(row)
    verts, faces = panel_grid(rows, 8, dark, .001)
    b.add(verts, faces, dark)
    # Raised visor brow and lower face rim physically frame the eye slit.
    for z in (.178, .206):
        pts = []
        for i in range(21):
            x = -.067 + i * .0067
            y = -.108 - .006 * (1 - (x / .07) ** 2)
            pts.append((x, y, z + .002 * math.cos(i * math.pi / 20)))
        add_tube(b, pts, .0035, edge, 8)
    # Nasal spine descends from the brow to a shaped chin plate.
    rows = []
    for z, width in ((.178, .007), (.153, .009), (.110, .013), (.066, .021)):
        rows.append([(-width, -.108, z), (-width * .5, -.114, z),
                     (width * .5, -.114, z), (width, -.108, z)])
    verts, faces = panel_grid(rows, 3, edge, .004)
    b.add(verts, faces, edge)
    # Vertical reinforcing ribs and side cheek plates; all follow the shell contour.
    for theta in (-.20, .20, math.pi - .20, math.pi + .20):
        pts = []
        for i in range(12):
            z = .104 + i * (.207 / 11)
            rx, ry = interpolate_profile(z, profile)
            a = theta + .012 * math.sin(i * .55)
            pts.append((rx * 1.022 * math.cos(a), ry * 1.022 * math.sin(a), z))
        add_tube(b, pts, .004, edge, 8)
    for sign in (-1, 1):
        rows = []
        for z, width, x in ((.169, .018, .073), (.137, .027, .078),
                            (.093, .025, .074), (.061, .013, .067)):
            rows.append([(sign * (x - width / 2), -.069, z),
                         (sign * x, -.091, z), (sign * (x + width / 2), -.069, z)])
        verts, faces = panel_grid(rows, 2, iron, .004)
        b.add(verts, faces, iron)
        for z in (.110, .145):
            add_dome(b, (sign * .077, -.092, z), (sign * .25, -1, 0), .004, .002, brass)
    # Articulated lower rim and rear neck lames make the closed silhouette readable.
    verts, faces = ring_loft([(.055, .071, .082), (.063, .081, .096),
                               (.075, .083, .098)], 72)
    b.add(verts, faces, edge)
    verts, faces = ring_loft([(.045, .068, .079), (.052, .073, .086)], 72)
    b.add(verts, faces, leather)
    for theta in (math.pi * .60, math.pi * .76, math.pi, math.pi * 1.24, math.pi * 1.40):
        # Small hinge rivets are placed on the helmet's lower rear arc.
        x = .076 * math.cos(theta)
        y = .091 * math.sin(theta)
        add_dome(b, (x, y, .070), (math.cos(theta), math.sin(theta), 0), .0035, .0018, brass)
    obj = b.create(name, col, BODY["head_origin_z"], "head", "T3")
    obj["design_note"] = "closed pot helm with a true eye-slit aperture, inset shadow, cheek plates and ribs"
    obj["visor_aperture"] = "open mesh cut from shell; dark inset behind slot"
    return obj


# -- Torso slot --------------------------------------------------------------

def make_torso_t1():
    name = "SK_Armor_Torso_WoolTunic_T1"
    col = replace_asset_collection(name, "torso")
    wool = pbr("KA_Wool_SaltAsh_T1", (.24, .22, .18), 0, .88, .28, .0026, 175, 5)
    linen = pbr("KA_Linen_Natural_T1", (.34, .31, .25), 0, .91, .25, .0017, 230, 3)
    leather = pbr("KA_Leather_Ashebrown_T1", (.13, .085, .055), .01, .76, .22, .0018, 190)
    thread = pbr("KA_Thread_RustRed_T1", (.28, .10, .061), 0, .90, .2, .001, 210)
    b = MeshBuild()
    profile = [(.852, .145, .096), (.875, .157, .102), (.930, .162, .108),
               (1.000, .169, .112), (1.060, .155, .101), (1.145, .146, .093),
               (1.235, .155, .098), (1.325, .174, .102), (1.405, .187, .104),
               (1.448, .188, .101), (1.476, .148, .084), (1.495, .077, .068)]
    torso_shell(b, wool, profile, True, .0035)
    # Woven-looking linen gusset panels are physical overlay patches at the shoulder/front.
    panel_on_front(b, [(1.435, .090), (1.405, .116), (1.365, .127)], linen,
                   profile=profile, columns=10, offset=.004, thickness=.002)
    for theta in (-math.pi * .5, math.pi * .5):
        torso_seam(b, theta, .873, 1.435, thread, .0016, .004, 20)
    # Neck opening roll follows the actual narrow top ring; lower hem/belt are shaped to torso.
    torso_ring(b, .875, leather, .0025, .006)
    torso_ring(b, 1.475, linen, .0027, .004)
    # A fitted cross-belt uses the shell's elliptical profile, not a straight cylinder.
    verts, faces = ring_loft([(.956, .166, .110), (.972, .168, .111),
                               (.992, .169, .112)], 64)
    b.add(verts, faces, leather)
    for theta in (-2.3, -1.8, -1.35, -.85, .4, 2.3):
        z = 1.03 + .016 * math.sin(theta * 2)
        torso_seam(b, theta, z, 1.42, thread, .0011, .006, 10)
    obj = b.create(name, col, BODY["pelvis_origin_z"], "torso", "T1")
    obj["design_note"] = "coarse wool tunic with linen shoulder gusset, stitched seams and fitted belt"
    obj["armholes"] = "open lateral shell windows; neck and hem are open boundaries with shell thickness"
    return obj


def make_torso_t2():
    name = "SK_Armor_Torso_PaddedGambeson_T2"
    col = replace_asset_collection(name, "torso")
    cloth = pbr("KA_Gambeson_WoolCharcoal_T2", (.17, .18, .16), 0, .86, .27, .0024, 165, 5)
    linen = pbr("KA_Gambeson_LinenLining_T2", (.28, .25, .20), 0, .91, .24, .0018, 220)
    leather = pbr("KA_Leather_GreasedDark_T2", (.095, .062, .043), .01, .77, .23, .0018, 180)
    thread = pbr("KA_Thread_SaltGrey_T2", (.40, .39, .34), 0, .89, .2, .001, 205)
    b = MeshBuild()
    # The underbody is slightly padded and retains the same armhole/neck openings.
    profile = [(z, rx + .007, ry + .006) for z, rx, ry in BODY["torso_profile"]]
    torso_shell(b, cloth, profile, True, .005)
    # Curved vertical quilt channels on front and back create real raised stitched seams.
    for theta in (-math.pi / 2, math.pi / 2):
        for xoff in (-.105, -.065, -.025, .025, .065, .105):
            angle = theta + xoff * 2.3
            torso_seam(b, angle, .918, 1.425, thread, .0020, .009, 22)
        for z in (1.015, 1.105, 1.205, 1.310, 1.400):
            pts = [torso_point(theta + (-.48 + .96 * i / 18), z + .006 * math.sin(i * math.pi / 18), .009, profile)
                   for i in range(19)]
            add_tube(b, pts, .0016, thread, 6)
    # Collar/hem are padded rolls, shaped to their local torso loops.
    torso_ring(b, 1.466, leather, .0045, .009)
    torso_ring(b, .895, linen, .0035, .008)
    # Short, shoulder-covering padded cap panels taper away from arm articulation.
    for sign in (-1, 1):
        rows = []
        for z, width, cx in ((1.442, .068, .139), (1.416, .078, .154), (1.382, .067, .164)):
            row = []
            for j in range(7):
                x = cx - width / 2 + width * j / 6
                rx, ry = interpolate_profile(z, profile)
                y = -ry * math.sqrt(max(.1, 1 - (x / rx) ** 2)) - .010
                row.append((sign * x, y, z))
            rows.append(row)
        verts, faces = panel_grid(rows, 6, cloth, .005)
        b.add(verts, faces, cloth)
    # Leather closure straps and hand-sewn seam ends.
    for z in (1.02, 1.39):
        torso_ring(b, z, leather, .003, .011)
    obj = b.create(name, col, BODY["pelvis_origin_z"], "torso", "T2")
    obj["design_note"] = "thick wool gambeson, raised quilting, stitched channels, padded shoulder caps"
    obj["armholes"] = "cut shell openings remain clear through the shoulder articulation zone"
    return obj


def make_torso_t3():
    name = "SK_Armor_Torso_CuirBouilli_T3"
    col = replace_asset_collection(name, "torso")
    leather = pbr("KA_CuirBouilli_DarkUmber_T3", (.12, .079, .052), .035, .72, .25, .0020, 175, 4)
    edge = pbr("KA_Leather_BurnishedEdges_T3", (.23, .155, .085), .055, .62, .16, .001, 160)
    iron = pbr("KA_Iron_Rivets_T3", (.15, .17, .16), .78, .68, .15, .0008, 125)
    brass = pbr("KA_Brass_Patinated_T3", (.30, .22, .10), .66, .75, .14, .001, 110)
    under = pbr("KA_Linen_Undercoat_T3", (.18, .16, .13), 0, .89, .23, .0015, 220)
    b = MeshBuild()
    profile = [(z, rx + .004, ry + .004) for z, rx, ry in BODY["torso_profile"]]
    torso_shell(b, under, profile, True, .004)
    # Anatomical one-piece leather shell with neck and underarm apertures.
    cuir_profile = [(.900, .143, .100), (.928, .156, .107), (.990, .160, .111),
                    (1.055, .150, .101), (1.145, .145, .095), (1.235, .156, .101),
                    (1.320, .176, .106), (1.390, .187, .107), (1.432, .188, .103),
                    (1.464, .151, .086), (1.490, .079, .070)]
    torso_shell(b, leather, cuir_profile, True, .004)
    # Curved front cuir panels create plate-like overlaps while remaining fitted to the body.
    panel_on_front(b, [(1.39, .105), (1.34, .137), (1.25, .139),
                       (1.16, .125), (1.055, .122), (.985, .135)],
                   leather, cuir_profile, 14, .010, .004)
    panel_on_front(b, [(1.37, .052), (1.31, .084), (1.20, .075), (1.10, .050)],
                   edge, cuir_profile, 10, .015, .003)
    # High standing collar, built as fitted rings with raised stamped welt.
    verts, faces = ring_loft([(1.444, .076, .070), (1.466, .082, .076),
                               (1.503, .079, .073), (1.516, .073, .068)], 64)
    b.add(verts, faces, leather)
    torso_ring(b, 1.502, edge, .0028, .006)
    torso_ring(b, .944, leather, .005, .012)
    # Chest-shaped strap and paired side lames with embossed details.
    for theta in (-2.35, -2.0, -1.55, -1.10, -.78):
        torso_seam(b, theta, 1.02, 1.39, edge, .0022, .012, 16)
    # Raised open-ring device with three diagonal notches: intentionally small and unlit.
    emblem_y = torso_point(-math.pi / 2, 1.285, .021, cuir_profile)[1]
    gap_angles = (.2, 2.3, 4.4)
    arcs = ((.31, 2.19), (2.41, 4.29), (4.51, 2 * math.pi + .09))
    for start, end in arcs:
        pts = [( .030 * math.cos(start + (end - start) * i / 28),
                 emblem_y - .002,
                 1.285 + .030 * math.sin(start + (end - start) * i / 28))
                for i in range(29)]
        add_tube(b, pts, .0025, brass, 7, closed=False)
    for g in gap_angles:
        x0, z0 = .030 * math.cos(g), 1.285 + .030 * math.sin(g)
        add_tube(b, [(x0 - .004, emblem_y - .003, z0 - .003), (x0, emblem_y - .006, z0),
                     (x0 + .005, emblem_y - .003, z0 + .004)], .0012, edge, 5)
    # Rivet rows along seams and lower edge, spaced in world scale.
    for z in (1.03, 1.10, 1.18, 1.27, 1.35, 1.41):
        for theta in (-2.45, -2.08, -1.70, -1.32, -.95, -.60):
            p = torso_point(theta, z, .016, cuir_profile)
            n = Vector((math.cos(theta), math.sin(theta), 0.04)).normalized()
            add_dome(b, p, n, .0038, .0018, iron)
    # Small convex sun-brooch echo on sternum, composed of ring lofts and spokes.
    center_z = 1.355
    for radius, tube_r in ((.020, .0028), (.011, .0019)):
        ring = [(radius * math.cos(2 * math.pi * i / 32),
                 emblem_y - .005, center_z + radius * math.sin(2 * math.pi * i / 32)) for i in range(32)]
        add_tube(b, ring, tube_r, brass, 7, closed=True)
    for i in range(8):
        a = 2 * math.pi * i / 8
        add_tube(b, [(.011 * math.cos(a), emblem_y - .007, center_z + .011 * math.sin(a)),
                     (.019 * math.cos(a), emblem_y - .006, center_z + .019 * math.sin(a))], .0013, edge, 5)
    add_dome(b, (0, emblem_y - .008, center_z), (0, -1, 0), .006, .0025, brass)
    obj = b.create(name, col, BODY["pelvis_origin_z"], "torso", "T3")
    obj["design_note"] = "hardened leather cuirass with overlapping breast plates, high collar, rivets and open-ring mark"
    obj["armholes"] = "cut lateral openings with closed thickness walls; high collar has neck opening"
    return obj


BUILDERS = {
    "HEAD_T1": make_head_t1,
    "HEAD_T2": make_head_t2,
    "HEAD_T3": make_head_t3,
    "TORSO_T1": make_torso_t1,
    "TORSO_T2": make_torso_t2,
    "TORSO_T3": make_torso_t3,
}


def generate_variant(variant_id):
    if variant_id not in BUILDERS:
        raise ValueError("Unknown variant ID: " + variant_id)
    return BUILDERS[variant_id]()
