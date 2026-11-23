"""First three (T1-T3) procedural variants for Küstenasche slots 3-8.

Geometry is made from custom quad lofts, quad panels and quad tubing. No Blender
primitive operators are used. The generated mesh is rigged to the shared
21-bone A-pose. Cloth/finger/collision groups are authoring masks that still
need their corresponding UE5 Chaos Cloth / Physics Asset setup.
"""
import math
import bpy
from mathutils import Vector

import variant_common as vc


def mat(key, color, metallic, roughness, bump=.18, distance=.0015, scale=150):
    return vc.pbr("KA_" + key, color, metallic, roughness, bump, distance, scale)


def quad_segment_shell(start, end, rows, material, build, sides=20, thickness=.0025,
                       inner_flatten=0.82, inward_sign=0):
    """Anatomical tapered shell around a bone segment; rows=(t, radius_a, radius_b)."""
    a, b = Vector(start), Vector(end)
    axis = (b - a).normalized()
    reference = Vector((0, 1, 0))
    if abs(axis.dot(reference)) > .92:
        reference = Vector((1, 0, 0))
    u = axis.cross(reference).normalized()
    v = axis.cross(u).normalized()
    inward = Vector((-1 if inward_sign >= 0 else 1, 0, 0))
    outer, inner = [], []
    for t, ra, rb in rows:
        center = a + (b - a) * t
        for is_inner, target in ((False, outer), (True, inner)):
            ia = max(.001, ra - (thickness if is_inner else 0))
            ib = max(.001, rb - (thickness if is_inner else 0))
            for j in range(sides):
                angle = 2 * math.pi * j / sides
                direction = math.cos(angle) * u + math.sin(angle) * v
                scale = inner_flatten if direction.dot(inward) > .35 else 1.0
                offset = (u * (ia * math.cos(angle)) + v * (ib * math.sin(angle))) * scale
                point = center + offset
                target.append(tuple(point))
    row_count = len(rows)
    faces = []
    for r in range(row_count - 1):
        for j in range(sides):
            a0, a1 = r * sides + j, r * sides + (j + 1) % sides
            faces.append((a0, a1, a1 + sides, a0 + sides))
            b0, b1 = row_count * sides + a0, row_count * sides + a1
            faces.append((b0 + sides, b1 + sides, b1, b0))
    # Quad end walls close the cuff edges and keep the radial thickness physical.
    for j in range(sides):
        j1 = (j + 1) % sides
        faces.append((j, j1, row_count * sides + j1, row_count * sides + j))
        q = (row_count - 1) * sides
        faces.append((q + j1, q + j, row_count * sides + q + j,
                      row_count * sides + q + j1))
    build.add(outer + inner, faces, material)
    return (a, axis, u, v)


def segment_surface_point(start, end, t, angle, ra, rb, inward_sign=0, flatten=.86):
    a, b = Vector(start), Vector(end)
    axis = (b - a).normalized()
    ref = Vector((0, 1, 0)) if abs(axis.y) < .92 else Vector((1, 0, 0))
    u = axis.cross(ref).normalized()
    v = axis.cross(u).normalized()
    direction = math.cos(angle) * u + math.sin(angle) * v
    inward = Vector((-1 if inward_sign >= 0 else 1, 0, 0))
    scale = flatten if direction.dot(inward) > .35 else 1.0
    center = a + (b - a) * t
    offset = (u * (ra * math.cos(angle)) + v * (rb * math.sin(angle))) * scale
    return tuple(center + offset)


def arm_path(side, lower=True):
    s = 1 if side == "L" else -1
    if lower:
        return ((s * .385, 0, 1.285), (s * .535, -.005, 1.135))
    return ((s * .185, 0, 1.445), (s * .385, 0, 1.285))


def add_arm_band(build, start, end, t0, t1, ra, rb, material, side,
                 sides=20, thickness=.002):
    rows = ((t0, ra, rb), ((t0 + t1) * .5, ra * 1.01, rb * 1.01), (t1, ra, rb))
    return quad_segment_shell(start, end, rows, material, build, sides, thickness,
                              inward_sign=1 if side == "L" else -1)


def add_cross_wrap(build, start, end, radii, material, side, turns=2, radius=.0018):
    pts = []
    count = 42
    for i in range(count):
        t = .08 + .84 * i / (count - 1)
        angle = 2 * math.pi * turns * (i / (count - 1))
        ra = radii[0] * (1 - t) + radii[-1] * t
        rb = radii[1] * (1 - t) + radii[-1] * t
        pts.append(segment_surface_point(start, end, t, angle, ra + .002, rb + .002,
                                         1 if side == "L" else -1))
    vc.add_tube(build, pts, radius, material, sides=5)
    # Counter-wound tendon line makes the crossed wrap legible at game distance.
    pts = []
    for i in range(count):
        t = .08 + .84 * i / (count - 1)
        angle = -2 * math.pi * turns * (i / (count - 1)) + .28
        ra = radii[0] * (1 - t) + radii[-1] * t
        rb = radii[1] * (1 - t) + radii[-1] * t
        pts.append(segment_surface_point(start, end, t, angle, ra + .0035, rb + .0035,
                                         1 if side == "L" else -1))
    vc.add_tube(build, pts, radius * .72, material, sides=5)


def add_oval_link(build, center, rx, rz, tube_radius, material, plane="xz", sides=4):
    cx, cy, cz = center
    points = []
    for i in range(8):
        a = 2 * math.pi * i / 8
        if plane == "xz":
            points.append((cx + rx * math.cos(a), cy, cz + rz * math.sin(a)))
        else:
            points.append((cx + rx * math.cos(a), cy + rz * math.sin(a), cz))
    vc.add_tube(build, points, tube_radius, material, sides=sides, closed=True)


def add_fur_tufts(build, centers, material, count=32, length=.045, seed=0):
    # Hand-cut tapered locks as small quad tubes; deterministic distribution.
    for i in range(count):
        t = i / max(1, count - 1)
        center = Vector(centers(t))
        angle = i * 2.399963 + seed
        direction = Vector((.22 * math.cos(angle), .35 * math.sin(angle), -1.0)).normalized()
        p0 = center
        p1 = center + direction * length * .34
        p2 = center + direction * length * .74 + Vector((.006 * math.sin(i), 0, 0))
        p3 = center + direction * length + Vector((.008 * math.cos(angle), .002 * math.sin(angle), 0))
        verts, faces = vc.tube_mesh([tuple(p0), tuple(p1), tuple(p2), tuple(p3)],
                                    [.008, .006, .0035, .0007], sides=5)
        build.add(verts, faces, material)


def make_panel_grid(z_width_rows, y_func, material, build, columns=18, thickness=.002,
                    x_offset=0.0, x_range=None, fold=.006, phase=0.0):
    """Curved, double-sided quad fabric/plate panel. Rows are (z, half_width)."""
    if x_range is None:
        x_range = (-1.0, 1.0)
    grid = []
    for row, (z, halfw) in enumerate(z_width_rows):
        points = []
        for c in range(columns + 1):
            u = x_range[0] + (x_range[1] - x_range[0]) * c / columns
            x = x_offset + halfw * u
            y = y_func(x, z) + fold * math.sin((u + 1) * math.pi * 4 + phase) * (row / max(1, len(z_width_rows)-1))
            points.append((x, y, z))
        grid.append(points)
    verts, faces = vc.panel_grid(grid, columns, material, thickness)
    build.add(verts, faces, material)


def add_open_ring_mark(build, center, radius, y, brass, relief, scale=1.0):
    # Three separate arcs guarantee real gaps; short diagonal cuts read as the shared sign.
    gaps = ((.28, 2.18), (2.43, 4.28), (4.55, 2 * math.pi + .04))
    for lo, hi in gaps:
        path = []
        for i in range(20):
            a = lo + (hi - lo) * i / 19
            path.append((center[0] + radius * scale * math.cos(a), y,
                         center[1] + radius * scale * math.sin(a)))
        vc.add_tube(build, path, .0018 * scale, brass, sides=6)
    for a in (.22, 2.34, 4.43):
        x = center[0] + radius * scale * math.cos(a)
        z = center[1] + radius * scale * math.sin(a)
        vc.add_tube(build, [(x - .004, y - .001, z - .003),
                            (x, y - .003, z),
                            (x + .004, y - .001, z + .003)], .001 * scale, relief, sides=5)


def add_shoulder_groups(obj):
    groups = {}
    for side, sign in (("L", 1), ("R", -1)):
        groups[side] = {
            "weight": obj.vertex_groups.new(name="VG_Weight_Shoulder_" + side),
            "collision": obj.vertex_groups.new(name="VG_Collision_Shoulder_" + side),
        }
        bone = obj.vertex_groups.get("clavicle_" + side.lower())
        upper = obj.vertex_groups.get("upperarm_" + side.lower())
        for vert in obj.data.vertices:
            p = obj.matrix_world @ vert.co
            if sign * p.x < .07 or not (1.34 <= p.z <= 1.53):
                continue
            try:
                w = max(bone.weight(vert.index), upper.weight(vert.index)) if bone and upper else 0.0
            except RuntimeError:
                w = 0.0
            if w > .001:
                groups[side]["weight"].add([vert.index], w, "REPLACE")
            # Collision group is a smooth region mask for a later UE Physics Asset.
            radial = math.sqrt((abs(p.x) - .205) ** 2 + (p.y * .65) ** 2 + ((p.z - 1.425) * .7) ** 2)
            collision_w = max(0.0, min(1.0, 1.0 - radial / .11))
            if collision_w > .01:
                groups[side]["collision"].add([vert.index], collision_w, "REPLACE")
    obj["collision_groups_note"] = "VG_Collision_Shoulder_L/R are masks; author UE Physics Asset bodies separately"


def add_cloth_groups(obj, slot):
    pin = obj.vertex_groups.new(name="VG_Cloth_Pin")
    free = obj.vertex_groups.new(name="VG_Cloth_Free")
    for v in obj.data.vertices:
        p = obj.matrix_world @ v.co
        if slot == "cloak":
            pin_w = max(0.0, min(1.0, (p.z - 1.27) / .17))
        else:
            pin_w = max(0.0, min(1.0, (p.z - .91) / .10))
        if pin_w > .001:
            pin.add([v.index], pin_w, "REPLACE")
        free_w = 1.0 - pin_w
        if free_w > .001:
            free.add([v.index], free_w, "REPLACE")
    obj["cloth_setup_note"] = "vertex groups are pin/weight maps; configure UE5 Chaos Cloth after import"


def add_joint_masks(obj, slot):
    if slot == "arms":
        for side, sign in (("L", 1), ("R", -1)):
            group = obj.vertex_groups.new(name="VG_Elbow_Flex_" + side)
            for v in obj.data.vertices:
                p = obj.matrix_world @ v.co
                along = abs(abs(p.x) - .385)
                w = max(0.0, min(1.0, 1.0 - along / .075))
                if sign * p.x > .25 and 1.23 <= p.z <= 1.34 and w > .01:
                    group.add([v.index], w, "REPLACE")
    elif slot == "legs":
        for side, sign in (("L", 1), ("R", -1)):
            group = obj.vertex_groups.new(name="VG_Knee_Compression_" + side)
            for v in obj.data.vertices:
                p = obj.matrix_world @ v.co
                w = max(0.0, min(1.0, 1.0 - abs(p.z - .535) / .085))
                if sign * p.x > .025 and w > .01:
                    group.add([v.index], w, "REPLACE")


def add_hand_segment_groups(obj):
    groups = {}
    for side, sign in (("L", 1), ("R", -1)):
        groups[side] = {
            "thumb": obj.vertex_groups.new(name="VG_Hand_Thumb_" + side),
            "index": obj.vertex_groups.new(name="VG_Hand_Index_" + side),
            "block": obj.vertex_groups.new(name="VG_Hand_FingersBlock_" + side),
        }
    for v in obj.data.vertices:
        p = obj.matrix_world @ v.co
        side = "L" if p.x >= 0 else "R"
        s = 1 if side == "L" else -1
        if not (p.z < 1.14 and p.z > 1.00):
            continue
        signed_x = s * p.x
        if signed_x < .535:
            groups[side]["thumb"].add([v.index], 1.0, "REPLACE")
        elif signed_x >= .574:
            groups[side]["index"].add([v.index], 1.0, "REPLACE")
        elif p.z < 1.075:
            groups[side]["block"].add([v.index], 1.0, "REPLACE")


def finish(build, name, slot, tier, pivot, note, extras=None):
    col = vc.replace_asset_collection(name, slot)
    obj = build.create(name, col, pivot, slot, "T%d" % tier)
    obj["design_note"] = note
    if extras:
        for key, value in extras.items():
            obj[key] = value
    if slot == "shoulders":
        add_shoulder_groups(obj)
    if slot == "hands":
        add_hand_segment_groups(obj)
        obj["weapon_grip_socket_bone"] = "hand_r"
        obj["hand_r_rest_position_m"] = "(-0.535, -0.005, 1.135)"
    if slot in ("surcoat", "cloak"):
        add_cloth_groups(obj, slot)
    if slot in ("arms", "legs"):
        add_joint_masks(obj, slot)
    return obj


# === SLOT 3: SHOULDERS =======================================================

def build_shoulders_t1():
    b = vc.MeshBuild()
    fur = mat("Fur_WolfAsh_T1", (.19, .18, .16), 0, .91, .26, .003, 170)
    leather = mat("Leather_TendonTie_T1", (.11, .07, .046), .01, .78, .22, .0015, 185)
    thread = mat("Thread_Rust_T1", (.28, .105, .064), 0, .88, .20, .001, 200)
    for side in ("L", "R"):
        sign = 1 if side == "L" else -1
        start, end = ((sign * .07, 0, 1.455), (sign * .27, .005, 1.390))
        width = .080 if side == "L" else .071
        rows = ((0, .047, .066), (.16, .068, .082), (.42, width, .089),
                (.72, width * .92, .082), (.94, .055, .066), (1, .020, .030))
        quad_segment_shell(start, end, rows, fur, b, 28, .003, .9, sign)
        # Two crossed hand-knotted straps lie over the pelt, leaving the arm joint free.
        add_cross_wrap(b, start, end, (.047, .066, .057), leather, side, turns=1, radius=.0023)
        for t in (.18, .42, .68, .88):
            p = segment_surface_point(start, end, t, -.20, width * .92, .083, sign)
            vc.add_dome(b, p, (sign, -1, .4), .0032, .0017, thread)
        add_fur_tufts(b, lambda t, st=start, en=end, r=width: (
            st[0] * (1-t) + en[0] * t, -.065 - .010 * math.sin(t * math.pi),
            1.455 * (1-t) + 1.390 * t), fur, count=22, length=.038, seed=sign)
    return finish(b, "SK_Armor_Shoulders_WolfPelt_T1", "shoulders", 1, .88,
                  "Asymmetric ash-wolf pelt shoulder wraps with crossed tendon ties and articulated clavicle fit",
                  {"collision_groups": "VG_Collision_Shoulder_L/R", "weight_groups": "VG_Weight_Shoulder_L/R"})


def build_shoulders_t2():
    b = vc.MeshBuild()
    iron = mat("BogIron_Pauldron_T2", (.115, .135, .13), .77, .70, .20, .0014, 135)
    edge = mat("Iron_WornRim_T2", (.235, .245, .22), .82, .58, .13, .0009, 160)
    leather = mat("Leather_Binding_T2", (.09, .057, .039), .02, .78, .20, .0015, 175)
    rivet = mat("Rivet_Bronze_T2", (.25, .19, .09), .66, .76, .13, .0008, 125)
    for side in ("L", "R"):
        sign = 1 if side == "L" else -1
        start, end = ((sign * .065, .006, 1.455), (sign * .274, .008, 1.387))
        s = 1.0 if side == "L" else .82  # deliberately asymmetric silhouette
        rows = ((0, .043, .060), (.12, .060*s, .075*s), (.32, .082*s, .094*s),
                (.62, .077*s, .089*s), (.84, .060*s, .074*s), (1, .025, .036))
        quad_segment_shell(start, end, rows, iron, b, 32, .0035, .84, sign)
        # Raised forged perimeter and central ridge follow the shoulder axis.
        for angle in (-1.1, 1.1):
            pts = [segment_surface_point(start, end, .10 + .78*i/16, angle,
                                         .082*s, .094*s, sign) for i in range(17)]
            vc.add_tube(b, pts, .0034, edge, sides=7)
        for t in (.18, .35, .52, .69, .83):
            for angle in (-1.55, -.75, .75, 1.55):
                p = segment_surface_point(start, end, t, angle, .080*s, .094*s, sign)
                vc.add_dome(b, p, (sign*.5, -1, .4), .004, .002, rivet)
        add_arm_band(b, start, end, .02, .09, .050, .067, leather, side, 24, .002)
    return finish(b, "SK_Armor_Shoulders_AsymIron_T2", "shoulders", 2, .88,
                  "Asymmetric dark iron pauldron shells with hand-raised rims, rivets and leather articulation straps",
                  {"collision_groups": "VG_Collision_Shoulder_L/R", "weight_groups": "VG_Weight_Shoulder_L/R"})


def build_shoulders_t3():
    b = vc.MeshBuild()
    iron = mat("ForgedIron_Drake_T3", (.13, .15, .145), .83, .65, .23, .0015, 145)
    brass = mat("PatinatedBrass_Drake_T3", (.30, .225, .105), .68, .73, .14, .001, 115)
    glass = mat("SeaGlass_Muted_T3", (.12, .30, .28), .30, .48, .08, .0005, 95)
    leather = mat("Leather_PauldronLiner_T3", (.065, .045, .032), .01, .81, .2, .0013, 170)
    for side in ("L", "R"):
        sign = 1 if side == "L" else -1
        start, end = ((sign * .065, .004, 1.455), (sign * .285, .006, 1.380))
        s = 1.0 if side == "L" else .84
        rows = ((0, .042, .058), (.14, .061*s, .080*s), (.36, .086*s, .101*s),
                (.64, .079*s, .097*s), (.86, .054*s, .071*s), (1, .018, .028))
        quad_segment_shell(start, end, rows, iron, b, 36, .004, .86, sign)
        # Raised, swept wing lips: one high crest and two shorter feather ridges.
        for idx, angle in enumerate((-1.30, -.72, .72, 1.30)):
            length = .96 if idx in (0, 3) else .74
            pts = [segment_surface_point(start, end, .14 + length*i/20, angle,
                                         .086*s, .101*s, sign) for i in range(21)]
            vc.add_tube(b, pts, .0044 if idx in (0,3) else .0028,
                        brass if idx in (0,3) else iron, sides=8)
        # Stylized raised drake curve on the outer shell, matched to the curved surface.
        dragon = []
        for i in range(20):
            t = .28 + .52 * i/19
            angle = -.38 + .76 * math.sin(i * math.pi/19)
            dragon.append(segment_surface_point(start, end, t, angle,
                                                 .087*s, .103*s, sign))
        vc.add_tube(b, dragon, .0028, brass, sides=7)
        for t in (.2, .40, .60, .80):
            p = segment_surface_point(start, end, t, -.05, .087*s, .103*s, sign)
            vc.add_dome(b, p, (sign*.3, -1, .5), .0038, .002, brass)
        # Single restrained sea-glass cabochon, set into a physical raised bezel.
        bezel = segment_surface_point(start, end, .52, -.08, .088*s, .105*s, sign)
        vc.add_dome(b, bezel, (sign*.3, -1, .5), .008, .003, brass)
        vc.add_dome(b, (bezel[0], bezel[1]-.002, bezel[2]), (sign*.3, -1, .5), .005, .002, glass)
        add_arm_band(b, start, end, .02, .09, .049, .064, leather, side, 24, .002)
    return finish(b, "SK_Armor_Shoulders_DrakeWing_T3", "shoulders", 3, .88,
                  "Hand-forged wing-lipped shoulder plates with restrained raised drake relief, patinated brass and sea-glass bezels",
                  {"collision_groups": "VG_Collision_Shoulder_L/R", "weight_groups": "VG_Weight_Shoulder_L/R"})


# === SLOT 4: ARMS / BRACERS ================================================

def build_arms_t1():
    b = vc.MeshBuild()
    leather = mat("Leather_Bracer_T1", (.12, .078, .051), .015, .75, .25, .002, 180)
    tendon = mat("Tendon_Crosswrap_T1", (.32, .24, .16), 0, .9, .21, .001, 220)
    brass = mat("RawBrass_Buckle_T1", (.24, .18, .09), .60, .80, .12, .0008, 120)
    for side in ("L", "R"):
        sign = 1 if side == "L" else -1
        start, end = arm_path(side, True)
        rows = ((.07,.035,.041),(.11,.045,.052),(.19,.050,.056),(.34,.052,.059),
                (.50,.050,.056),(.68,.045,.052),(.86,.038,.044),(.94,.032,.038))
        quad_segment_shell(start, end, rows, leather, b, 24, .003, .78, sign)
        add_cross_wrap(b, start, end, (.045,.052,.038), tendon, side, turns=2, radius=.0024)
        for t in (.12,.48,.88):
            add_arm_band(b,start,end,t,t+.035,.050*(1-t)+.038*t,.057*(1-t)+.044*t,
                         leather,side,20,.0018)
        # Hand-cut buckle tongue and two raised keepers on the outer forearm.
        for t in (.18,.78):
            p=segment_surface_point(start,end,t,-1.55,.051,.058,sign)
            vc.add_dome(b,p,(sign*.4,-1,.1),.004,.002,brass)
    return finish(b,"SK_Armor_Arms_TendonBracers_T1","arms",1,.88,
                  "Dark greased leather forearm bracers with crossed tendon wraps and rough buckles")


def build_arms_t2():
    b = vc.MeshBuild()
    iron=mat("BogIron_Bracers_T2",(.12,.14,.135),.79,.68,.2,.0014,140)
    edge=mat("WornIron_Edges_T2",(.24,.25,.23),.84,.57,.14,.0008,165)
    leather=mat("Leather_Hinge_T2",(.08,.052,.036),.02,.79,.2,.0015,170)
    brass=mat("Brass_Rivets_T2",(.27,.20,.095),.66,.75,.12,.0008,125)
    for side in ("L","R"):
        sign=1 if side=="L" else -1
        start,end=arm_path(side,True)
        rows=((.04,.032,.038),(.08,.044,.050),(.18,.050,.057),(.38,.052,.059),
              (.60,.048,.056),(.82,.039,.047),(.94,.031,.037))
        quad_segment_shell(start,end,rows,iron,b,28,.0035,.80,sign)
        # Separate articulated elbow cup extends a little over the joint.
        upper_start,upper_end=arm_path(side,False)
        cap_rows=((.80,.034,.039),(.88,.046,.052),(.96,.052,.058),(1.03,.044,.050),(1.10,.030,.036))
        quad_segment_shell(upper_start,upper_end,cap_rows,iron,b,28,.003,.82,sign)
        for angle in (-1.45,1.45):
            pts=[segment_surface_point(start,end,.12+.72*i/14,angle,.052,.060,sign) for i in range(15)]
            vc.add_tube(b,pts,.003,edge,sides=7)
        for t in (.12,.30,.52,.74,.88):
            for angle in (-1.35,-.45,.45,1.35):
                p=segment_surface_point(start,end,t,angle,.052,.060,sign)
                vc.add_dome(b,p,(sign*.4,-1,.1),.0035,.0018,brass)
        for t0,t1 in ((.02,.09),(.88,.95)):
            add_arm_band(b,start,end,t0,t1,.042,.050,leather,side,24,.002)
    return finish(b,"SK_Armor_Arms_RivetedIron_T2","arms",2,.88,
                  "Raseneisen forearm shells with independent elbow cups, visible rivet rows and leather hinge bands")


def build_arms_t3():
    b=vc.MeshBuild()
    iron=mat("ForgedSteel_Bracers_T3",(.14,.16,.155),.84,.63,.22,.0015,145)
    brass=mat("PatinatedBrass_Etch_T3",(.29,.22,.105),.68,.73,.13,.0009,120)
    glass=mat("SeaGlass_Inlay_T3",(.13,.32,.29),.32,.50,.08,.0004,100)
    leather=mat("Leather_BracersLiner_T3",(.07,.047,.032),.01,.82,.19,.0014,175)
    for side in ("L","R"):
        sign=1 if side=="L" else -1
        start,end=arm_path(side,True)
        rows=((.04,.032,.038),(.09,.046,.054),(.20,.053,.061),(.42,.054,.062),
              (.65,.048,.057),(.84,.040,.048),(.95,.030,.036))
        quad_segment_shell(start,end,rows,iron,b,32,.004,.80,sign)
        # Fine raised borders and tidal-relief curves; all ornament is geometric.
        for angle in (-1.5,1.5):
            pts=[segment_surface_point(start,end,.08+.82*i/20,angle,.055,.064,sign) for i in range(21)]
            vc.add_tube(b,pts,.0025,brass,sides=7)
        tide=[]
        for i in range(25):
            t=.18+.60*i/24
            angle=.40*math.sin(i*math.pi/12)
            tide.append(segment_surface_point(start,end,t,angle,.055,.064,sign))
        vc.add_tube(b,tide,.0021,brass,sides=6)
        inlay=segment_surface_point(start,end,.52,0,.056,.065,sign)
        vc.add_dome(b,inlay,(sign*.35,-1,.2),.006,.0025,brass)
        vc.add_dome(b,(inlay[0],inlay[1]-.0015,inlay[2]),(sign*.35,-1,.2),.0035,.0018,glass)
        upper_start,upper_end=arm_path(side,False)
        cap_rows=((.82,.035,.041),(.90,.047,.054),(.98,.052,.058),(1.04,.043,.050),(1.10,.029,.035))
        quad_segment_shell(upper_start,upper_end,cap_rows,iron,b,28,.0035,.82,sign)
        add_arm_band(b,start,end,.03,.10,.047,.056,leather,side,24,.002)
    return finish(b,"SK_Armor_Arms_TidehallPlates_T3","arms",3,.88,
                  "Articulated forged forearm plates with restrained raised Gezeitenhall engraving and inset sea-glass")


# === SLOT 5: HANDS / GLOVES ================================================

def hand_points(side):
    s=1 if side=="L" else -1
    wrist=(s*.535,-.005,1.135)
    palm_end=(s*.570,-.022,1.074)
    return s,wrist,palm_end


def add_hand_fingers(build,side,material,index_material=None,small=.010):
    s,wrist,palm_end=hand_points(side)
    base=Vector(palm_end)
    # Index finger is an individual tapered segment; the other three are a broad block.
    index_start=base+Vector((s*.010,-.005,-.005))
    index_end=base+Vector((s*.012,-.018,-.060))
    quad_segment_shell(index_start,index_end,((0,.010,.010),(.25,.010,.009),(.72,.008,.007),(1,.005,.006)),
                       index_material or material,build,12,.0015,.86,s)
    block_start=base+Vector((-s*.008,-.004,-.004))
    block_end=base+Vector((-s*.006,-.016,-.053))
    quad_segment_shell(block_start,block_end,((0,.020,.012),(.22,.021,.012),(.70,.016,.009),(1,.009,.007)),
                       material,build,16,.0018,.86,s)
    thumb_start=Vector(wrist)+Vector((s*-.018,-.008,-.020))
    thumb_end=thumb_start+Vector((s*-.030,-.022,-.034))
    quad_segment_shell(thumb_start,thumb_end,((0,.012,.011),(.45,.010,.009),(1,.005,.006)),
                       material,build,12,.0015,.82,s)


def add_hand_knuckles(build,side,material):
    s,wrist,palm=hand_points(side)
    for i in range(4):
        x=s*.563+(i-1.5)*.013
        center=(x,-.049,1.071-.003*abs(i-1.5))
        vc.add_dome(build,center,(0,-1,0),.008,.003,material)


def build_hands_t1():
    b=vc.MeshBuild()
    wool=mat("Wool_Mittens_T1",(.22,.20,.17),0,.90,.27,.0025,180)
    leather=mat("Leather_MittenPalm_T1",(.11,.073,.049),.01,.79,.22,.0018,170)
    seam=mat("Thread_Mitten_T1",(.36,.32,.25),0,.91,.20,.001,220)
    for side in ("L","R"):
        s,wrist,palm=hand_points(side)
        quad_segment_shell(wrist,palm,((0,.028,.021),(.18,.037,.027),(.48,.041,.029),(.78,.035,.025),(1,.025,.021)),
                           wool,b,20,.002,.86,s)
        # Isolated thumb wedge and cuff; mitten remains closed for a robust grip silhouette.
        thumb_start=Vector(wrist)+Vector((s*-.018,-.01,-.020))
        thumb_end=thumb_start+Vector((s*-.031,-.022,-.037))
        quad_segment_shell(thumb_start,thumb_end,((0,.013,.011),(.5,.011,.009),(1,.005,.005)),
                           leather,b,12,.0015,.82,s)
        add_arm_band(b,wrist,palm,.01,.17,.034,.024,leather,side,20,.002)
        for angle in (-1.1,1.1):
            pts=[segment_surface_point(wrist,palm,.20+.55*i/12,angle,.043,.031,s) for i in range(13)]
            vc.add_tube(b,pts,.0016,seam,sides=5)
    obj=finish(b,"SK_Armor_Hands_WoolMittens_T1","hands",1,.88,
               "Padded wool mittens with a separate leather thumb wedge and sewn cuff")
    return obj


def build_hands_t2():
    b=vc.MeshBuild()
    leather=mat("Leather_CombatGlove_T2",(.12,.078,.052),.02,.74,.24,.0018,185)
    plate=mat("Iron_KnucklePlates_T2",(.14,.16,.15),.77,.68,.18,.001,145)
    stitch=mat("Thread_Glove_T2",(.38,.33,.25),0,.89,.19,.001,220)
    for side in ("L","R"):
        s,wrist,palm=hand_points(side)
        quad_segment_shell(wrist,palm,((0,.026,.019),(.18,.035,.026),(.48,.037,.027),(.80,.031,.023),(1,.022,.019)),
                           leather,b,20,.002,.85,s)
        add_hand_fingers(b,side,leather,leather)
        add_hand_knuckles(b,side,plate)
        add_arm_band(b,wrist,palm,.03,.16,.031,.023,plate,side,20,.0018)
        # Raised stitch channels along the palm edge.
        for angle in (-1.0,1.0):
            pts=[segment_surface_point(wrist,palm,.20+.55*i/12,angle,.039,.030,s) for i in range(13)]
            vc.add_tube(b,pts,.0014,stitch,sides=5)
    return finish(b,"SK_Armor_Hands_LeatherCombat_T2","hands",2,.88,
                  "Leather combat gloves with distinct index/block/ thumb segments and raised iron knuckle plates")


def build_hands_t3():
    b=vc.MeshBuild()
    leather=mat("Leather_MailMittenLiner_T3",(.075,.050,.034),.015,.80,.22,.0016,175)
    iron=mat("Mail_MittenRings_T3",(.20,.22,.205),.80,.62,.17,.0009,155)
    steel=mat("Gauntlet_FingerPlates_T3",(.16,.18,.17),.84,.58,.18,.0009,150)
    brass=mat("Brass_GauntletRivets_T3",(.28,.21,.10),.68,.74,.12,.0008,120)
    for side in ("L","R"):
        s,wrist,palm=hand_points(side)
        quad_segment_shell(wrist,palm,((0,.026,.019),(.18,.035,.026),(.48,.038,.028),(.82,.031,.023),(1,.022,.018)),
                           leather,b,20,.002,.85,s)
        add_hand_fingers(b,side,leather,steel)
        # Interlinked four-sided rings form a physical mail patch across the back of the hand.
        for row in range(5):
            for col in range(7):
                x=s*(.548+col*.0062)
                z=1.105-row*.009
                y=-.051-(.001 if (row+col)%2 else 0)
                add_oval_link(b,(x,y,z),.0041,.0034,.0011,iron,"xz",4)
        add_hand_knuckles(b,side,steel)
        for i in range(4):
            x=s*.563+(i-1.5)*.013
            vc.add_dome(b,(x,-.054,1.071-.003*abs(i-1.5)),(0,-1,0),.007,.0025,brass)
        add_arm_band(b,wrist,palm,.02,.14,.031,.023,steel,side,20,.002)
    return finish(b,"SK_Armor_Hands_MailGauntlet_T3","hands",3,.88,
                  "Leather-backed mail mittens with true interlinked rings, segmented index/block fingers and steel knuckle caps",
                  {"finger_articulation": "modular geometry attached to hand_l/r; no finger bones in current shared skeleton"})


# === SLOT 6: LEGS / GREAVES =================================================

def leg_path(side, upper=True):
    s=1 if side=="L" else -1
    if upper:
        return ((s*.092,0,.925),(s*.098,0,.535))
    return ((s*.098,0,.535),(s*.100,0,.105))


def build_legs_t1():
    b=vc.MeshBuild()
    wool=mat("Wool_LegWraps_T1",(.25,.23,.19),0,.90,.25,.0023,185)
    linen=mat("Linen_WrapBands_T1",(.35,.32,.26),0,.92,.23,.0017,220)
    leather=mat("Leather_HeelTie_T1",(.11,.072,.048),.01,.79,.22,.0015,175)
    for side in ("L","R"):
        sign=1 if side=="L" else -1
        a0,a1=leg_path(side,True)
        quad_segment_shell(a0,a1,((0,.061,.070),(.16,.071,.079),(.38,.067,.075),(.62,.056,.066),(.84,.048,.056),(1,.044,.051)),
                           wool,b,24,.003,.86,sign)
        k0,k1=leg_path(side,False)
        quad_segment_shell(k0,k1,((0,.046,.054),(.12,.054,.062),(.30,.052,.060),(.52,.046,.054),(.76,.041,.049),(1,.034,.042)),
                           wool,b,24,.003,.86,sign)
        for t in (.12,.28,.46,.64,.82,.94):
            add_arm_band(b,k0,k1,t,t+.055,.052*(1-t)+.036*t,.061*(1-t)+.043*t,
                         linen,side,24,.0018)
        # Short ankle bindings sit above the foot joint.
        add_arm_band(b,k0,k1,.88,.98,.038,.047,leather,side,24,.0018)
    return finish(b,"SK_Armor_Legs_WoolWraps_T1","legs",1,.88,
                  "Wool leg wraps and fitted under-leg layers with ankle bindings and free knee compression zones")


def build_legs_t2():
    b=vc.MeshBuild()
    leather=mat("Leather_Tassets_T2",(.12,.079,.052),.025,.75,.24,.0018,180)
    iron=mat("BogIron_TassetRivets_T2",(.14,.16,.15),.75,.70,.16,.0009,145)
    chain=mat("Iron_HangingLinks_T2",(.20,.21,.19),.76,.69,.14,.0008,130)
    cloth=mat("Wool_LegLining_T2",(.18,.17,.145),0,.88,.23,.002,190)
    for side in ("L","R"):
        sign=1 if side=="L" else -1
        a0,a1=leg_path(side,True)
        quad_segment_shell(a0,a1,((0,.060,.069),(.24,.068,.077),(.50,.060,.068),(.78,.050,.058),(1,.044,.051)),
                           cloth,b,22,.003,.86,sign)
        k0,k1=leg_path(side,False)
        quad_segment_shell(k0,k1,((0,.047,.055),(.18,.053,.061),(.42,.050,.058),(.68,.043,.051),(1,.034,.041)),
                           cloth,b,22,.003,.86,sign)
        # Long front and side tasset panels: individually solid, rounded-edge quad grids.
        hipx=sign*.092
        for panel_id,angle in enumerate((-math.pi/2,-.15,-2.99)):
            cx=hipx + sign*(.018 if panel_id else 0)
            rows=[]
            for z,w in ((.925,.035),(.875,.043),(.810,.039),(.735,.030),(.665,.017)):
                yy=-.105 if panel_id==0 else -.03
                rows.append([(cx-w,yy,z),(cx-w*.35,yy-.008,z),(cx+w*.35,yy-.008,z),(cx+w,yy,z)])
            verts,faces=vc.panel_grid(rows,3,leather,.004)
            b.add(verts,faces,leather)
            for z in (.87,.81,.75,.69):
                vc.add_dome(b,(cx,-.116,z),(0,-1,0),.0032,.0016,iron)
        # Chain hangers at hip sides; each link is a small physical quad loop.
        for row in range(3):
            cx=sign*(.145+.012*(row%2))
            cy=-.018
            cz=.910-row*.026
            add_oval_link(b,(cx,cy,cz),.009,.013,.0015,chain,"xz" if row%2==0 else "yz",5)
        # Short fitted shin strips reinforce without closing the ankle bend.
        rows=((.15,.040,.048),(.20,.047,.054),(.68,.045,.052),(.85,.035,.043))
        quad_segment_shell(k0,k1,rows,leather,b,24,.003,.84,sign)
    return finish(b,"SK_Armor_Legs_LeatherTassets_T2","legs",2,.88,
                  "Layered dark leather thigh tassets suspended from physical chain loops over wool leg layers")


def build_legs_t3():
    b=vc.MeshBuild()
    iron=mat("ForgedSteel_Greaves_T3",(.14,.16,.155),.84,.62,.21,.0014,145)
    edge=mat("WornSteel_GreaveEdges_T3",(.25,.265,.25),.87,.53,.13,.0008,165)
    leather=mat("Leather_GreaveStraps_T3",(.075,.051,.036),.02,.78,.20,.0014,175)
    brass=mat("Brass_GreavePins_T3",(.28,.21,.105),.68,.74,.12,.0008,120)
    for side in ("L","R"):
        sign=1 if side=="L" else -1
        a0,a1=leg_path(side,True)
        quad_segment_shell(a0,a1,((.18,.060,.070),(.28,.070,.081),(.48,.065,.074),(.72,.054,.063),(.90,.046,.054)),
                           iron,b,28,.0035,.84,sign)
        k0,k1=leg_path(side,False)
        rows=((.05,.034,.042),(.12,.044,.053),(.24,.052,.061),(.45,.050,.059),(.68,.044,.053),(.87,.038,.047),(.96,.030,.038))
        quad_segment_shell(k0,k1,rows,iron,b,32,.004,.82,sign)
        # A separate domed knee cup spans the joint while leaving a narrow flexure channel.
        knee=Vector(k0)
        knee_start=tuple(knee+Vector((0,-.005,.034)))
        knee_end=tuple(knee+Vector((0,-.006,-.034)))
        quad_segment_shell(knee_start,knee_end,((0,.030,.026),(.22,.044,.032),(.5,.048,.034),(.78,.041,.031),(1,.026,.023)),
                           edge,b,24,.003,.84,sign)
        for angle in (-1.4,1.4):
            pts=[segment_surface_point(k0,k1,.16+.68*i/16,angle,.053,.062,sign) for i in range(17)]
            vc.add_tube(b,pts,.003,edge,sides=7)
        for t in (.20,.45,.70,.88):
            for angle in (-1.15,1.15):
                p=segment_surface_point(k0,k1,t,angle,.053,.062,sign)
                vc.add_dome(b,p,(sign*.3,-1,.1),.0038,.0018,brass)
        add_arm_band(b,k0,k1,.03,.10,.042,.050,leather,side,24,.002)
    return finish(b,"SK_Armor_Legs_ForgedGreaves_T3","legs",3,.88,
                  "Forged dark-steel greaves with separate knee cups, flexure support loops and worn edge beads")


# === SLOT 7: SURCOAT / WEAPON SKIRT ========================================

def surcoat_y(x,z,front=True):
    rx,ry=vc.interpolate_profile(max(.875,min(1.515,z)),vc.BODY["torso_profile"])
    shell_y=-ry*math.sqrt(max(.08,1-(x/max(rx,.001))**2)) if front else .9*ry*math.sqrt(max(.08,1-(x/max(rx,.001))**2))
    return shell_y + (-.020 if front else .025) - max(0,1.0-z)*.020


def add_surcoat_panels(build,material,kind="plain",trim=None):
    zwidth=((1.015,.135),(.970,.160),(.890,.225),(.780,.270),(.650,.300),(.520,.305))
    # Two front halves create a true center opening and independent hanging slits.
    for front in (True,False):
        for half in (-1,1):
            yrange=(-1,-.08) if half<0 else (.08,1)
            yfn=lambda x,z,f=front:surcoat_y(x,z,f)
            make_panel_grid(zwidth,yfn,material,build,columns=16,thickness=.0025,
                            x_range=yrange,fold=.008,phase=half*1.2+(0 if front else .7))
            if trim:
                for edge_x in (-1,1):
                    pts=[]
                    for i in range(15):
                        z=.54+.46*i/14
                        w=.135+(.305-.135)*(1-(z-.52)/.495)
                        x=half*(w*(.08 if edge_x<0 else 1.0))
                        pts.append((x,yfn(x,z)-.003,z))
                    vc.add_tube(build,pts,.0018,trim,sides=5)
    if kind=="tabard":
        add_open_ring_mark(build,(0,1.08),.042,surcoat_y(0,1.08,True)-.008,
                           trim,material,scale=1)
    elif kind=="brigandine":
        # Physical small overlaid leather/iron tiles on the upper skirt, not a flat print.
        for z in (.91,.84,.77):
            width=.20+(.91-z)*.35
            for x in (-width,-width*.55,-width*.18,width*.18,width*.55,width):
                y=surcoat_y(x,z,True)-.008
                vc.add_dome(build,(x,y,z),(0,-1,0),.007,.0024,trim)


def build_surcoat_t1():
    b=vc.MeshBuild()
    linen=mat("Linen_Overwrap_T1",(.38,.36,.31),0,.92,.25,.0018,220)
    seam=mat("Thread_Hem_T1",(.24,.13,.09),0,.9,.19,.001,210)
    leather=mat("Leather_BeltTabs_T1",(.12,.08,.055),.01,.79,.21,.0014,175)
    add_surcoat_panels(b,linen,"plain",seam)
    for x in (-.13,-.09,-.05,.05,.09,.13):
        pts=[]
        for i in range(16):
            z=1.00-.45*i/15
            pts.append((x*(.7+.3*(1.0-z)),surcoat_y(x,z,True)-.004,z))
        vc.add_tube(b,pts,.0012,leather,sides=5)
    return finish(b,"SK_Armor_Surcoat_Leinen_T1","surcoat",1,.88,
                  "Double-sided coarse linen over-wrap with vertical fold loops, split front panels and reinforced hem")


def build_surcoat_t2():
    b=vc.MeshBuild()
    cloth=mat("Cloth_ClanTabard_T2",(.20,.23,.22),0,.88,.24,.0018,205)
    linen=mat("Linen_TabardLining_T2",(.31,.29,.24),0,.91,.22,.0015,220)
    trim=mat("Leather_TabardTrim_T2",(.12,.078,.05),.01,.78,.20,.0014,175)
    add_surcoat_panels(b,cloth,"tabard",trim)
    # Lining is a second fitted panel held just inside each face, avoiding coplanar overlap.
    zwidth=((1.015,.132),(.970,.157),(.890,.222),(.780,.267),(.650,.297),(.520,.302))
    for front in (True,False):
        inset= .006 if front else -.006
        yfn=lambda x,z,f=front,d=inset:surcoat_y(x,z,f)+d
        for half in (-1,1):
            x_range=(-1,-.08) if half<0 else (.08,1)
            make_panel_grid(zwidth,yfn,linen,b,columns=16,thickness=.0015,
                            x_range=x_range,fold=.005,phase=half*.7)
    for z in (.94,.78,.62):
        for x in (-.24,-.16,-.08,.08,.16,.24):
            y=surcoat_y(x,z,True)-.007
            vc.add_dome(b,(x,y,z),(0,-1,0),.0035,.0015,trim)
    return finish(b,"SK_Armor_Surcoat_ClanTabard_T2","surcoat",2,.88,
                  "Sippenfarbener, double-sided linen tabard with stitched border, open-ring insignia and slitted panels")


def build_surcoat_t3():
    b=vc.MeshBuild()
    cloth=mat("Cloth_BrigandineSkirt_T3",(.13,.15,.14),0,.88,.23,.0018,190)
    leather=mat("Leather_BrigandineTiles_T3",(.095,.061,.041),.035,.74,.23,.0016,175)
    iron=mat("Iron_BrigandineRivets_T3",(.17,.19,.18),.81,.65,.17,.0008,145)
    brass=mat("Brass_ThreeKerbs_T3",(.28,.21,.10),.67,.74,.12,.0008,120)
    add_surcoat_panels(b,cloth,"brigandine",iron)
    # Front brigandine plates form separated rows over the split cloth skirt.
    for z in (.94,.86,.78):
        width=.19+(.94-z)*.30
        for i in range(7):
            x=-width+2*width*i/6
            y=surcoat_y(x,z,True)-.012
            vc.add_dome(b,(x,y,z),(0,-1,0),.010,.003,leather)
            vc.add_dome(b,(x,y-.002,z),(0,-1,0),.003,.0014,iron)
    add_open_ring_mark(b,(0,.69),.054,surcoat_y(0,.69,True)-.018,brass,iron,.9)
    return finish(b,"SK_Armor_Surcoat_BrigandineSlits_T3","surcoat",3,.88,
                  "Slitted brigandine weapon skirt with double fabric, physical leather plate studs and raised open-ring mark")


# === SLOT 8: CLOAK / FUR =====================================================

def make_cloak_grid(length=.78,width_top=.32,width_bottom=.42,rows=22,cols=28,
                    rear_y=.075,wave=.018,hem_irregular=.012):
    grid=[]
    for r in range(rows+1):
        # Nonlinear row spacing concentrates loops near the fixed neck/shoulder edge.
        v=(r/rows)**1.35
        z=1.455-length*v
        width=width_top*(1-v)+width_bottom*v
        row=[]
        for c in range(cols+1):
            u=-1+2*c/cols
            x=width*u
            # Draped back surface with shoulder pinning, broad gravity folds and broken hem.
            y=rear_y + .055*v + wave*math.sin(u*math.pi*5+v*2.5)*(v**.65)
            y+=.010*math.sin(u*math.pi*2)*(v**1.2)
            z2=z + hem_irregular*(v**5)*math.sin(u*math.pi*9)
            row.append((x,y,z2))
        grid.append(row)
    return grid


def add_cloak_surface(build,grid,material,thickness=.003):
    verts,faces=vc.panel_grid(grid,len(grid[0])-1,material,thickness)
    build.add(verts,faces,material)


def add_clasp(build,material,center=(0,.045,1.43)):
    cx,cy,cz=center
    path=[]
    for i in range(25):
        a=.20+2*math.pi*.84*i/24
        path.append((cx+.035*math.cos(a),cy,cz+.027*math.sin(a)))
    vc.add_tube(build,path,.0024,material,sides=7)
    for a in (.2,2.3,4.4):
        x=cx+.035*math.cos(a);z=cz+.027*math.sin(a)
        vc.add_tube(build,[(x-.004,cy-.002,z-.003),(x,cy-.004,z),(x+.004,cy-.002,z+.003)],.0014,material,sides=5)


def build_cloak_t1():
    b=vc.MeshBuild()
    pelt=mat("Sheepskin_Ash_T1",(.34,.31,.26),0,.94,.29,.003,185)
    leather=mat("Leather_CloakClasp_T1",(.12,.078,.05),.02,.79,.20,.0014,175)
    grid=make_cloak_grid(.62,.29,.36,rows=16,cols=22,rear_y=.07,wave=.012,hem_irregular=.016)
    add_cloak_surface(b,grid,pelt,.004)
    # Dense shoulder locks transition to sparse ragged lower fleece fringe.
    add_fur_tufts(b,lambda t:(-.28+.56*t,.04,1.425+.012*math.sin(t*math.pi)),
                  pelt,count=30,length=.055,seed=.3)
    add_fur_tufts(b,lambda t:(-.34+.68*t,.13,1.455-.62+.012*math.sin(t*math.pi*10)),
                  pelt,count=28,length=.052,seed=1.4)
    add_clasp(b,leather)
    return finish(b,"SK_Clothing_Cloak_Sheepskin_T1","cloak",1,.88,
                  "Short unshorn sheepskin mantle with dense nape/shoulder mesh, broken lower fleece fringe and neck clasp",
                  {"cloth_groups":"VG_Cloth_Pin / VG_Cloth_Free"})


def build_cloak_t2():
    b=vc.MeshBuild()
    fur=mat("WolfPelt_Charcoal_T2",(.16,.17,.16),0,.93,.30,.003,180)
    lining=mat("Linen_CloakLining_T2",(.28,.25,.21),0,.90,.23,.0018,215)
    brass=mat("Brass_PeltClasp_T2",(.27,.20,.10),.66,.75,.12,.0008,120)
    grid=make_cloak_grid(.86,.31,.43,rows=22,cols=28,rear_y=.08,wave=.022,hem_irregular=.020)
    add_cloak_surface(b,grid,fur,.004)
    # Lining is a second draped panel slightly inside the pelt shell.
    lining_grid=[[(x,y-.004,z) for x,y,z in row] for row in grid]
    add_cloak_surface(b,lining_grid,lining,.002)
    add_fur_tufts(b,lambda t:(-.30+.60*t,.035,1.438+.016*math.sin(t*math.pi)),
                  fur,count=36,length=.062,seed=.7)
    add_fur_tufts(b,lambda t:(-.41+.82*t,.16,1.455-.86+.014*math.sin(t*math.pi*12)),
                  fur,count=42,length=.070,seed=2.1)
    add_clasp(b,brass)
    return finish(b,"SK_Clothing_Cloak_WolfPelt_T2","cloak",2,.88,
                  "Mid-length charcoal wolf-pelt cloak with a separate inner lining, dense shoulders and rough hide fringe",
                  {"cloth_groups":"VG_Cloth_Pin / VG_Cloth_Free"})


def build_cloak_t3():
    b=vc.MeshBuild()
    cloth=mat("Cloak_RelicBasalt_T3",(.12,.15,.145),0,.88,.24,.0018,190)
    lining=mat("Cloak_RelicLining_T3",(.20,.19,.16),0,.90,.23,.0016,210)
    brass=mat("Brass_RuneBordure_T3",(.28,.215,.11),.66,.74,.14,.001,120)
    glass=mat("SeaGlass_Thread_T3",(.12,.31,.29),.30,.52,.08,.0005,100)
    grid=make_cloak_grid(1.03,.33,.47,rows=28,cols=32,rear_y=.085,wave=.024,hem_irregular=.008)
    add_cloak_surface(b,grid,cloth,.004)
    inside=[[(x,y-.004,z) for x,y,z in row] for row in grid]
    add_cloak_surface(b,inside,lining,.002)
    # Raised narrow border follows both sides and hem; no luminous texture dependence.
    for sign in (-1,1):
        pts=[]
        for i in range(28):
            v=i/27
            z=1.455-1.03*v
            x=sign*(.33*(1-v)+.47*v)
            y=.11+.055*v
            pts.append((x,y,z))
        vc.add_tube(b,pts,.0022,brass,sides=6)
    for i in range(19):
        u=-.94+1.88*i/18
        x=.47*u
        y=.145+.024*math.sin(u*math.pi*5)
        z=.425+.008*math.sin(i*math.pi/2)
        vc.add_tube(b,[(x-.008,y,z),(x,y-.002,z+.008),(x+.008,y,z)],.0016,
                    glass if i%3==0 else brass,sides=5)
    add_open_ring_mark(b,(0,1.18),.035,.102,brass,glass,.8)
    add_clasp(b,brass)
    return finish(b,"SK_Clothing_Cloak_Gezeitenhall_T3","cloak",3,.88,
                  "Mid-length dark Gezeitenhall cloak with sculpted gravity folds, high shoulder density, restrained raised rune hem and muted sea-glass accents",
                  {"cloth_groups":"VG_Cloth_Pin / VG_Cloth_Free"})


BUILDERS={
    "SHOULDERS_T1":build_shoulders_t1,"SHOULDERS_T2":build_shoulders_t2,"SHOULDERS_T3":build_shoulders_t3,
    "ARMS_T1":build_arms_t1,"ARMS_T2":build_arms_t2,"ARMS_T3":build_arms_t3,
    "HANDS_T1":build_hands_t1,"HANDS_T2":build_hands_t2,"HANDS_T3":build_hands_t3,
    "LEGS_T1":build_legs_t1,"LEGS_T2":build_legs_t2,"LEGS_T3":build_legs_t3,
    "SURCOAT_T1":build_surcoat_t1,"SURCOAT_T2":build_surcoat_t2,"SURCOAT_T3":build_surcoat_t3,
    "CLOAK_T1":build_cloak_t1,"CLOAK_T2":build_cloak_t2,"CLOAK_T3":build_cloak_t3,
}


def generate_variant(variant_id):
    if variant_id not in BUILDERS:
        raise ValueError("Unknown slot variant: " + variant_id)
    return BUILDERS[variant_id]()
