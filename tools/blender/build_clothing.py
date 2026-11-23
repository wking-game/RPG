"""
Own Norse clothing for the MetaHuman figures, built from the body mesh (run via tools/clothing.ps1 Build).

  blender --background --factory-startup --python build_clothing.py -- <dir with body_<Body>.fbx>

For each body (Kraeftig, Schmal) two outfits, each one skeletal mesh with material slots:
  Tunika: knee-long tunic with woven trim (hem, cuffs, neck), trousers, leg wraps + shoes, belt
  Kleid:  ankle-long dress with trim, shoes, belt
How: the upper garment parts are cut from the body surface (by bone weights and height) and pushed outwards along
the normals; skirts are cones around the legs. All garment vertices get their skin weights from the nearest body
surface (Data Transfer), so the clothes follow every animation of the body (leader pose in the game).
Style: design/kleidung_kandidaten_kostenlos.md and design/referenz/figuren (layers of wool, linen, leather).
Units: centimetres, as exported from Unreal. Output: cloth_<Body>_<Outfit>.fbx and build_ok.txt.
"""
import math
import os
import sys

import bmesh
import bpy
from mathutils import Vector

OUT = sys.argv[sys.argv.index('--') + 1] if '--' in sys.argv else os.getcwd()
BODIES = ['Kraeftig', 'Schmal']
HAND = ('hand_', 'pinky', 'ring_', 'thumb', 'middle', 'index')
FOOT = ('foot_', 'ball_', 'bigtoe', 'indextoe', 'middletoe', 'ringtoe', 'littletoe', 'toe')
HEADNECK = ('neck', 'head', 'FACIAL')
LEG = ('thigh', 'calf', 'knee')

# Material slots (Unreal sets the colours per figure).
SLOTS = ['Wool', 'Trim', 'Linen', 'Leather', 'Apron', 'Cloak', 'Metal']


def ovoid(center, size, name, segments=12, rings=8):
    """Small flattened sphere (brooch, bead); size = (x, y, z) radii in cm."""
    bm = bmesh.new()
    bmesh.ops.create_uvsphere(bm, u_segments=segments, v_segments=rings, radius=1.0)
    for v in bm.verts:
        v.co = Vector((center[0] + v.co.x * size[0], center[1] + v.co.y * size[1], center[2] + v.co.z * size[2]))
    me = bpy.data.meshes.new(name)
    bm.to_mesh(me)
    bm.free()
    obj = bpy.data.objects.new(name, me)
    bpy.context.scene.collection.objects.link(obj)
    return obj


def log(message):
    print('GAMER_CLOTH: ' + message, flush=True)


def reset():
    bpy.ops.wm.read_factory_settings(use_empty=True)


def import_body(name):
    bpy.ops.import_scene.fbx(filepath=os.path.join(OUT, 'body_%s.fbx' % name))
    arm = next(o for o in bpy.context.scene.objects if o.type == 'ARMATURE')
    body = next(o for o in bpy.context.scene.objects if o.type == 'MESH')
    return arm, body


def dominant_groups(body):
    names = [g.name for g in body.vertex_groups]
    result = []
    for v in body.data.vertices:
        best, weight = '', -1.0
        for g in v.groups:
            if g.weight > weight:
                best, weight = names[g.group], g.weight
        result.append(best)
    return result


def body_metrics(body, groups):
    """Heights (cm) taken from the body: crotch, waist, knee, ankle, neck base; hip radius."""
    vs = body.data.vertices
    zs = [v.co.z for v in vs]
    top = max(zs)
    thigh_z = [v.co.z for v, g in zip(vs, groups) if g.startswith('thigh')]
    calf_z = [v.co.z for v, g in zip(vs, groups) if g.startswith('calf')]
    foot_z = [v.co.z for v, g in zip(vs, groups) if g.startswith(FOOT)]
    pelvis_z = [v.co.z for v, g in zip(vs, groups) if g == 'pelvis']
    m = {
        'top': top,
        # lowest pelvis-weighted vertex = where the legs part
        'crotch': min(pelvis_z) + 2.0 if pelvis_z else top * 0.47,
        'knee': (min(thigh_z) + max(calf_z)) / 2 if thigh_z and calf_z else top * 0.28,
        'ankle': max(foot_z) + 1.0 if foot_z else top * 0.06,
    }
    m['waist'] = m['crotch'] + 0.13 * top
    m['neck'] = top - 0.045 * top
    # Ellipses (centre, half-axes) of the trunk: only pelvis/thigh/spine points, in the A-pose the hands hang
    # at hip height. The skirt follows them: under the belt (waist), at the widest hip, then flares.
    def ellipse(z, extra=()):
        band = [v.co for v, g in zip(vs, groups) if abs(v.co.z - z) < 2.0
                and (g == 'pelvis' or g.startswith('thigh') or g.startswith('spine') or g.startswith(extra))]
        cx = sum(p.x for p in band) / len(band)
        cy = sum(p.y for p in band) / len(band)
        return cx, cy, max(abs(p.x - cx) for p in band), max(abs(p.y - cy) for p in band)

    m['hip_z'] = m['crotch'] + 8
    m['waist_e'] = ellipse(m['waist'] - 4)
    m['hip_e'] = ellipse(m['hip_z'])
    cx, cy, rx, ry = m['hip_e']
    m['center'], m['rx'], m['ry'] = (cx, cy), rx, ry
    m['armpit'] = top - 0.17 * top
    m['chest_e'] = ellipse(m['armpit'] - 4)
    m['shoulder_z'] = top - 0.07 * top
    m['shoulder_e'] = ellipse(m['shoulder_z'], ('clavicle',))
    return m


def loosen(obj, iterations=12):
    """Laplacian smoothing (volume preserving): wool hangs over the body instead of showing every muscle."""
    mod = obj.modifiers.new('loose', 'LAPLACIANSMOOTH')
    mod.iterations = iterations
    mod.lambda_factor = 1.0
    mod.lambda_border = 0.0
    mod.use_volume_preserve = True
    mod.use_normalized = True
    bpy.context.view_layer.objects.active = obj
    bpy.ops.object.modifier_apply(modifier=mod.name)


def cut(body, groups, keep, offset, name):
    """New mesh object from the body faces whose vertices all pass keep(v, group); pushed out by offset cm."""
    bm = bmesh.new()
    bm.from_mesh(body.data)
    bm.verts.ensure_lookup_table()
    ok = [keep(v.co, groups[v.index]) for v in bm.verts]
    bmesh.ops.delete(bm, geom=[f for f in bm.faces if not all(ok[v.index] for v in f.verts)], context='FACES')
    bmesh.ops.delete(bm, geom=[v for v in bm.verts if not v.link_faces], context='VERTS')
    bm.normal_update()
    for v in bm.verts:
        v.co += v.normal * offset
    me = bpy.data.meshes.new(name)
    bm.to_mesh(me)
    bm.free()
    obj = bpy.data.objects.new(name, me)
    bpy.context.scene.collection.objects.link(obj)
    for vg in body.vertex_groups:  # same groups as the body (weights come from the transfer below)
        obj.vertex_groups.new(name=vg.name)
    return obj


def push(obj, offset):
    """Moves every vertex outwards along its normal."""
    bm = bmesh.new()
    bm.from_mesh(obj.data)
    bm.normal_update()
    for v in bm.verts:
        v.co += v.normal * offset
    bm.to_mesh(obj.data)
    bm.free()


def skirt(profile, name, segments=48, rings=16, arc=None):
    """Elliptic tube around the trunk/legs through the profile [(z, cx, cy, rx, ry), ...] (top to bottom,
    linear in between): skirt, tunic hem, belt. arc=(start, end) in degrees gives an open panel instead
    (0 deg = +X, 90 deg = +Y = the back of the figure)."""
    bm = bmesh.new()
    rows = []
    z_top, z_bottom = profile[0][0], profile[-1][0]
    for r in range(rings + 1):
        z = z_top + (z_bottom - z_top) * r / rings
        k = next(i for i in range(len(profile) - 1) if profile[i + 1][0] <= z + 1e-6 or i == len(profile) - 2)
        a0, a1 = profile[k], profile[k + 1]
        t = 0.0 if a0[0] == a1[0] else (a0[0] - z) / (a0[0] - a1[0])
        cx, cy, rx, ry = [a0[i] + (a1[i] - a0[i]) * t for i in range(1, 5)]
        row = []
        for s in range(segments + (1 if arc else 0)):
            if arc:
                a = math.radians(arc[0] + (arc[1] - arc[0]) * s / segments)
            else:
                a = 2 * math.pi * s / segments
            row.append(bm.verts.new((cx + math.cos(a) * rx, cy + math.sin(a) * ry, z)))
        rows.append(row)
    uv_layer = bm.loops.layers.uv.new('UV')
    for r in range(rings):
        for s in range(segments):
            n = len(rows[r])
            q = [rows[r][s], rows[r][(s + 1) % n], rows[r + 1][(s + 1) % n], rows[r + 1][s]]
            face = bm.faces.new(list(reversed(q)))  # normals outwards
            for loop, (u, v) in zip(face.loops, [(s, r), (s + 1, r), (s + 1, r + 1), (s, r + 1)][::-1]):
                loop[uv_layer].uv = (u / segments * 4, v / rings * 2)
    me = bpy.data.meshes.new(name)
    bm.to_mesh(me)
    bm.free()
    obj = bpy.data.objects.new(name, me)
    bpy.context.scene.collection.objects.link(obj)
    return obj


def set_material(obj, slot, faces=None):
    while len(obj.data.materials) < len(SLOTS):
        obj.data.materials.append(bpy.data.materials.get(SLOTS[len(obj.data.materials)]) or bpy.data.materials.new(SLOTS[len(obj.data.materials)]))
    index = SLOTS.index(slot)
    for p in obj.data.polygons:
        if faces is None or faces(p):
            p.material_index = index


def trim_band(obj, z_ranges):
    """Trim material on the faces whose centre lies in one of the height ranges."""
    def inside(p):
        z = p.center.z
        return any(lo <= z <= hi for lo, hi in z_ranges)
    set_material(obj, 'Trim', inside)


def join(objs, name):
    bpy.ops.object.select_all(action='DESELECT')
    for o in objs:
        o.select_set(True)
    bpy.context.view_layer.objects.active = objs[0]
    bpy.ops.object.join()
    obj = bpy.context.view_layer.objects.active
    obj.name = name
    obj.data.name = name
    return obj


def trunk_copy(body, groups):
    """Body copy without arms and hands: weight source for skirts and belts (in the A-pose the hands hang at hip
    height, and skirt vertices must not follow them)."""
    # cut() keeps the body's skin weights (bmesh carries the deform layer, groups are created in body order)
    keep = lambda co, g: g == 'pelvis' or g.startswith('spine') or g.startswith(LEG) or g.startswith(FOOT)
    return cut(body, groups, keep, 0.0, 'TrunkSrc')


def transfer_weights(obj, source):
    """Skin weights for generated parts (skirts, belts) from the nearest surface of source."""
    for vg in source.vertex_groups:
        if vg.name not in obj.vertex_groups:
            obj.vertex_groups.new(name=vg.name)
    mod = obj.modifiers.new('weights', 'DATA_TRANSFER')
    mod.object = source
    mod.use_vert_data = True
    mod.data_types_verts = {'VGROUP_WEIGHTS'}
    mod.vert_mapping = 'POLYINTERP_NEAREST'
    mod.layers_vgroup_select_src = 'ALL'
    mod.layers_vgroup_select_dst = 'NAME'
    bpy.context.view_layer.objects.active = obj
    bpy.ops.object.modifier_apply(modifier=mod.name)


def soften_leg_weights(obj, body, groups, top_z, hem_z, max_leg=0.35):
    """Skirts, apron and cloak panel: below top_z (hip) the nearest-surface weights are thighs/calves, so lifting a
    leg (jump, sitting) dragged the cloth with it and it ballooned. Here they follow the pelvis and only partly the
    thigh on their side (0 at top_z rising to max_leg at the hem); calf/foot influence is removed."""
    xs = [v.co.x for v, g in zip(body.data.vertices, groups) if g.startswith('thigh') and g.endswith('_l')]
    left_sign = 1.0 if xs and sum(xs) / len(xs) > 0 else -1.0
    names = [vg.name for vg in obj.vertex_groups]
    for name in ('pelvis', 'thigh_l', 'thigh_r'):
        if name not in names:
            obj.vertex_groups.new(name=name)
    pelvis, thigh_l, thigh_r = (obj.vertex_groups[n] for n in ('pelvis', 'thigh_l', 'thigh_r'))
    xs_obj = [v.co.x for v in obj.data.vertices]
    half_w = max(1.0, (max(xs_obj) - min(xs_obj)) / 2) if xs_obj else 1.0
    cx = (max(xs_obj) + min(xs_obj)) / 2 if xs_obj else 0.0
    for v in obj.data.vertices:
        if v.co.z >= top_z:
            continue
        t = min(1.0, (top_z - v.co.z) / max(1.0, top_z - hem_z))
        leg = max_leg * t
        # side: 1 = fully left thigh, 0 = fully right; the middle (front/back) shares both
        side = min(1.0, max(0.0, 0.5 + 0.5 * left_sign * (v.co.x - cx) / (half_w * 0.6)))
        for vg in obj.vertex_groups:
            if vg.name not in ('pelvis', 'thigh_l', 'thigh_r'):
                try:
                    vg.remove([v.index])
                except RuntimeError:
                    pass
        pelvis.add([v.index], 1.0 - leg, 'REPLACE')
        thigh_l.add([v.index], leg * side, 'REPLACE')
        thigh_r.add([v.index], leg * (1.0 - side), 'REPLACE')


def finish(obj, arm):
    bpy.context.view_layer.objects.active = obj
    bpy.ops.object.vertex_group_limit_total(group_select_mode='ALL', limit=8)
    bpy.ops.object.vertex_group_normalize_all(group_select_mode='ALL', lock_active=False)
    unweighted = sum(1 for v in obj.data.vertices if not any(g.weight > 0 for g in v.groups))
    log('%s: %d vertices without skin weights' % (obj.name, unweighted))
    obj.parent = arm
    am = obj.modifiers.new('Armature', 'ARMATURE')
    am.object = arm


def build(name):
    reset()
    arm, body = import_body(name)
    groups = dominant_groups(body)
    m = body_metrics(body, groups)
    log('%s: top %.1f crotch %.1f waist %.1f knee %.1f ankle %.1f hip %.1fx%.1f'
        % (name, m['top'], m['crotch'], m['waist'], m['knee'], m['ankle'], m['rx'], m['ry']))

    def upper(co, g):  # torso and arms, no hands/neck/head, down to just below the waist (skirt and belt follow)
        return not g.startswith(HAND) and not g.startswith(HEADNECK) and not g.startswith(FOOT) \
            and not g.startswith(LEG) and co.z >= m['waist'] - 5

    def legs(co, g):
        return (g.startswith(LEG) or g == 'pelvis') and co.z <= m['waist'] and co.z >= m['ankle'] + 8

    def feet(co, g):
        return co.z <= m['ankle'] + 22 and not g.startswith(HAND)

    wx, wy, wrx, wry = m['waist_e']
    hx, hy, hrx, hry = m['hip_e']
    wz = m['waist'] - 4  # belt line

    def skirt_profile(z_hem, flare):
        # waist (under the belt) -> widest hip -> hem; a little ease everywhere so the body stays inside
        return [(wz + 2, wx, wy, wrx * 1.10 + 1.5, wry * 1.10 + 1.5),
                (m['hip_z'], hx, hy, hrx * 1.10 + 1.5, hry * 1.12 + 1.5),
                (z_hem, hx, hy, hrx * flare, hry * flare)]

    def belt_ring(name):
        return skirt([(wz + 4.0, wx, wy, wrx * 1.10 + 2.6, wry * 1.10 + 2.6),
                      (wz - 1.5, wx, wy, wrx * 1.10 + 2.6, wry * 1.10 + 2.6)], name, rings=2)

    trunk = trunk_copy(body, groups)
    outfits = {}
    # Tunika: upper part + knee-long skirt, trousers, leg wraps/shoes, belt
    up = cut(body, groups, upper, 0.0, 'Tunic')
    loosen(up)
    push(up, 1.8)
    sk = skirt(skirt_profile(m['knee'] + 4, 1.42), 'TunicSkirt')
    transfer_weights(sk, trunk)
    soften_leg_weights(sk, body, groups, m['hip_z'], m['knee'] + 4)
    tunic = join([up, sk], 'Tunic')
    set_material(tunic, 'Wool')
    trim_band(tunic, [(m['knee'] + 4, m['knee'] + 9)])
    trousers = cut(body, groups, legs, 0.6, 'Trousers')
    set_material(trousers, 'Linen')
    wraps = cut(body, groups, feet, 1.0, 'Wraps')
    set_material(wraps, 'Leather')
    belt1 = belt_ring('Belt')
    transfer_weights(belt1, trunk)
    set_material(belt1, 'Leather')
    outfits['Tunika'] = [tunic, trousers, wraps, belt1]

    # Kleid: upper part + ankle-long skirt, shoes, belt
    def dress_parts(tag):
        up2 = cut(body, groups, upper, 0.0, 'Dress' + tag)
        loosen(up2)
        push(up2, 1.8)
        sk2 = skirt(skirt_profile(m['ankle'] + 3, 1.62), 'DressSkirt' + tag)
        transfer_weights(sk2, trunk)
        soften_leg_weights(sk2, body, groups, m['hip_z'], m['ankle'] + 3)
        dress = join([up2, sk2], 'Dress' + tag)
        set_material(dress, 'Wool')
        trim_band(dress, [(m['ankle'] + 3, m['ankle'] + 9)])
        shoes = cut(body, groups, lambda co, g: co.z <= m['ankle'] + 6 and not g.startswith(HAND), 0.9, 'Shoes' + tag)
        set_material(shoes, 'Leather')
        return [dress, shoes]

    belt2 = belt_ring('Belt2')
    transfer_weights(belt2, trunk)
    set_material(belt2, 'Leather')
    outfits['Kleid'] = dress_parts('A') + [belt2]

    # Schuerzenkleid: dress + apron (chest to shin, front and back panels) held by two oval brooches with a
    # string of beads between them (reference sheets, design/referenz/figuren); belt over the apron.
    cx, cy, crx, cry = m['chest_e']
    ap_top = m['armpit'] - 1
    ap_hem = m['ankle'] + 22
    apron_profile = [(ap_top, cx, cy, crx * 1.08 + 3.2, cry * 1.10 + 3.2),
                     (wz, wx, wy, wrx * 1.10 + 3.4, wry * 1.10 + 3.4),
                     (m['hip_z'], hx, hy, hrx * 1.10 + 3.6, hry * 1.12 + 3.6),
                     (ap_hem, hx, hy, hrx * 1.52, hry * 1.52)]
    front = skirt(apron_profile, 'ApronFront', segments=16, arc=(-150, -30))   # -90 deg = -Y = front
    back = skirt(apron_profile, 'ApronBack', segments=16, arc=(35, 145))
    apron = join([front, back], 'Apron')
    transfer_weights(apron, trunk)
    soften_leg_weights(apron, body, groups, m['hip_z'], m['ankle'])
    set_material(apron, 'Apron')
    trim_band(apron, [(ap_hem, ap_hem + 4)])
    jewels = []
    by = cy - (cry * 1.10 + 3.2) - 0.8  # just in front of the apron top
    for side in (-1, 1):
        jewels.append(ovoid((cx + side * crx * 0.42, by, ap_top - 3), (2.6, 1.1, 3.4), 'Brooch'))
    for i in range(9):  # beads hang in a curve between the brooches
        t = i / 8.0
        x = cx + (-0.42 + 0.84 * t) * crx
        z = ap_top - 6 - 5.5 * math.sin(math.pi * t)
        jewels.append(ovoid((x, by - 0.6, z), (0.8, 0.8, 0.8), 'Bead', segments=8, rings=6))
    jewelry = join(jewels, 'Jewelry')
    transfer_weights(jewelry, trunk)
    set_material(jewelry, 'Metal')
    belt3 = skirt([(wz + 4.0, wx, wy, wrx * 1.10 + 4.4, wry * 1.10 + 4.4),
                   (wz - 1.5, wx, wy, wrx * 1.10 + 4.4, wry * 1.10 + 4.4)], 'Belt3', rings=2)
    transfer_weights(belt3, trunk)
    set_material(belt3, 'Leather')
    outfits['Schuerzenkleid'] = dress_parts('B') + [apron, jewelry, belt3]

    # Umhang (separate layer over any outfit): shoulder cape cut from the shoulders, a back panel down to the
    # knees (wide enough for the skirts), and a ring brooch on the right shoulder.
    sx, sy, srx, sry = m['shoulder_e']

    def shoulders(co, g):
        return co.z >= m['armpit'] + 2 and not g.startswith(HAND) and not g.startswith(HEADNECK) \
            and (g.startswith('spine') or g.startswith('clavicle') or g.startswith('upperarm'))

    cape = cut(body, groups, shoulders, 0.0, 'Cape')
    loosen(cape, 20)
    push(cape, 3.4)
    set_material(cape, 'Cloak')
    panel = skirt([(m['shoulder_z'] - 2, sx, sy, srx * 1.05 + 3.0, sry * 1.15 + 4.0),
                   (m['armpit'] - 6, cx, cy, crx * 1.12 + 5.0, cry * 1.20 + 5.0),
                   (m['hip_z'], hx, hy, hrx * 1.45 + 4.0, hry * 1.45 + 5.0),
                   (m['knee'] - 2, hx, hy, hrx * 1.75 + 4.0, hry * 1.65 + 6.0)],
                  'CloakPanel', segments=20, arc=(18, 162))
    transfer_weights(panel, trunk)
    soften_leg_weights(panel, body, groups, m['hip_z'], m['knee'] - 2, max_leg=0.2)
    set_material(panel, 'Cloak')
    trim_band(panel, [(m['knee'] - 2, m['knee'] + 2)])
    pin = ovoid((sx - srx * 0.55, sy - sry - 3.0, m['shoulder_z'] - 4), (2.4, 1.0, 2.4), 'CloakPin')
    transfer_weights(pin, trunk)
    set_material(pin, 'Metal')
    outfits['Umhang'] = [cape, panel, pin]

    for outfit, parts in outfits.items():
        obj = join(parts, 'SKM_Cloth_%s_%s' % (name, outfit))
        obj.data.shade_smooth()
        finish(obj, arm)
        bpy.ops.object.select_all(action='DESELECT')
        arm.select_set(True)
        obj.select_set(True)
        path = os.path.join(OUT, 'cloth_%s_%s.fbx' % (name, outfit))
        bpy.ops.export_scene.fbx(filepath=path, use_selection=True, object_types={'ARMATURE', 'MESH'},
                                 add_leaf_bones=False, bake_anim=False, use_armature_deform_only=False,
                                 mesh_smooth_type='FACE', apply_unit_scale=True)
        log('%s %s: %d verts, %d faces -> %s' % (name, outfit, len(obj.data.vertices), len(obj.data.polygons), path))
        obj.hide_set(True)


ok_file = os.path.join(OUT, 'build_ok.txt')
if os.path.exists(ok_file):
    os.remove(ok_file)
for body_name in BODIES:
    build(body_name)
with open(ok_file, 'w') as f:
    f.write('ok\n')
log('done')
