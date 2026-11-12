"""Erzeugt den Holzbaukasten (Phase 1, Prioritaet A) nach design/inbox/03_assets_baukasten.md.

Aufruf (Blender headless):
  E:\\Blender\\blender.exe --background --factory-startup --python tools/blender/build_wood_kit.py -- art/wood_kit

Ergebnis im Zielordner:
  SM_Building_*.fbx            je ein Bauteil, Ursprung laut Steckbrief, 1 Einheit = 1 cm
  T_Trim_Wood_Placeholder.png  Platzhalter-Trim-Sheet mit dem verbindlichen UV-Streifen-Layout
  T_Turf_Placeholder.png       Platzhalter fuer die Turf-Kante
  preview_house.png            Testhaus aus den Modulen
  preview_modules.png          alle Module einzeln
  README.md                    Tabelle mit Dreiecken, Masse, Pivots, UV-Layout
"""
import bpy, bmesh, sys, os, math
from mathutils import Vector, Matrix

argv = sys.argv[sys.argv.index('--') + 1:] if '--' in sys.argv else []
OUT = os.path.abspath(argv[0]) if argv else os.path.abspath('wood_kit_out')
os.makedirs(OUT, exist_ok=True)

# ---------------------------------------------------------------- Trim-Sheet-Layout
CM_PER_UV = 200.0  # 1 UV-Einheit = 200 cm  -> 2048 px Textur = ca. 10 px/cm
STRIPS = {          # (v0, v1) im Trim-Sheet; u kachelt waagerecht
    'plank':  (0.70, 1.00),  # 60 cm: Bretter (laufen entlang u), 4 Bretter a 15 cm
    'timber': (0.45, 0.70),  # 50 cm: Balkenflaechen
    'end':    (0.35, 0.45),  # 20 cm: Hirnholz / Schnittkanten
    'dark':   (0.10, 0.35),  # 50 cm: dunkle, gealterte Bretter (Sockel, Unterseiten)
    'metal':  (0.00, 0.10),  # 20 cm: dunkle Beschlaege
}
TARGETS = {  # Dreiecks-Ziel laut Steckbrief, harte Obergrenze 2000
    'SM_Building_Foundation_Wood': 500, 'SM_Building_Floor_Wood': 500,
    'SM_Building_Wall_Wood': 800, 'SM_Building_Wall_Door_Wood': 800,
    'SM_Building_Door_Wood': 400, 'SM_Building_Pillar_Wood': 200,
    'SM_Building_Beam_Wood': 200, 'SM_Building_Roof_Wood': 600,
    'SM_Building_Roof_Ridge_Wood': 200, 'SM_Building_Roof_Gable_Wood': 400,
}
PIVOTS = {
    'SM_Building_Foundation_Wood': 'Mitte der Oberseite (Oberseite 20 cm ueber Gelaende, Sockel bis 120 cm darunter)',
    'SM_Building_Floor_Wood': 'Mitte der Oberseite',
    'SM_Building_Wall_Wood': 'unten mittig auf der Rasterlinie',
    'SM_Building_Wall_Door_Wood': 'unten mittig auf der Rasterlinie',
    'SM_Building_Door_Wood': 'untere Scharnierkante (Tuer liegt bei +X)',
    'SM_Building_Pillar_Wood': 'unten mittig',
    'SM_Building_Beam_Wood': 'Mitte der Laengsachse',
    'SM_Building_Roof_Wood': 'Traufkante unten mittig (Dach steigt Richtung +Y)',
    'SM_Building_Roof_Ridge_Wood': 'Firstlinie der Dach-Unterseite, mittig',
    'SM_Building_Roof_Gable_Wood': 'unten mittig auf der Rasterlinie',
}

X = Vector((1, 0, 0)); Y = Vector((0, 1, 0)); Z = Vector((0, 0, 1))
S45 = math.sqrt(0.5)


def V(*a):
    return Vector(a)


def frame(o, cx, cy, cz):
    """4x4-Matrix mit den Spalten cx, cy, cz und Ursprung o."""
    m = Matrix.Identity(4)
    for i in range(3):
        m[i][0], m[i][1], m[i][2], m[i][3] = cx[i], cy[i], cz[i], o[i]
    return m


class Builder:
    def __init__(self, name):
        self.name = name
        self.bm = bmesh.new()
        self.uv = self.bm.loops.layers.uv.new('UVMap')
        self.M = Matrix.Identity(4)

    def quad(self, pts, normal, strip, u_axis=None, mat=0):
        R = self.M.to_3x3()
        pts = [self.M @ Vector(p) for p in pts]
        nrm = (R @ Vector(normal)).normalized()
        if (pts[1] - pts[0]).cross(pts[2] - pts[0]).dot(nrm) < 0:
            pts = pts[::-1]
        if u_axis is not None:
            u_axis = R @ Vector(u_axis)
        if u_axis is None or u_axis.length < 1e-6:
            u_axis = max((pts[(i + 1) % len(pts)] - pts[i] for i in range(len(pts))), key=lambda d: d.length)
        u_axis = (u_axis - nrm * u_axis.dot(nrm)).normalized()
        v_axis = nrm.cross(u_axis).normalized()
        us = [p.dot(u_axis) for p in pts]
        vs = [p.dot(v_axis) for p in pts]
        u0, v0 = min(us), min(vs)
        f = self.bm.faces.new([self.bm.verts.new(p) for p in pts])
        f.material_index = mat
        if strip == 'turf':
            for loop, u, v in zip(f.loops, us, vs):
                loop[self.uv].uv = ((u - u0) / CM_PER_UV, (v - v0) / CM_PER_UV)
        else:
            s0, s1 = STRIPS[strip]
            ext = max(vs) - v0
            k = min(1.0, (s1 - s0) * CM_PER_UV / ext) if ext > 1e-6 else 1.0
            for loop, u, v in zip(f.loops, us, vs):
                loop[self.uv].uv = ((u - u0) / CM_PER_UV, s0 + (v - v0) * k / CM_PER_UV)
        return f

    def box(self, mn, mx, strip='timber', end='end', skip=(), mat=0):
        x0, y0, z0 = mn
        x1, y1, z1 = mx
        size = (x1 - x0, y1 - y0, z1 - z0)
        L = max(range(3), key=lambda i: size[i])
        elongated = size[L] >= 2.0 * max(size[i] for i in range(3) if i != L)
        axes = [X, Y, Z]
        faces = {
            '+x': ([(x1, y0, z0), (x1, y1, z0), (x1, y1, z1), (x1, y0, z1)], X, 0),
            '-x': ([(x0, y0, z0), (x0, y1, z0), (x0, y1, z1), (x0, y0, z1)], -X, 0),
            '+y': ([(x0, y1, z0), (x1, y1, z0), (x1, y1, z1), (x0, y1, z1)], Y, 1),
            '-y': ([(x0, y0, z0), (x1, y0, z0), (x1, y0, z1), (x0, y0, z1)], -Y, 1),
            '+z': ([(x0, y0, z1), (x1, y0, z1), (x1, y1, z1), (x0, y1, z1)], Z, 2),
            '-z': ([(x0, y0, z0), (x1, y0, z0), (x1, y1, z0), (x0, y1, z0)], -Z, 2),
        }
        for key, (pts, n, ax) in faces.items():
            if key in skip:
                continue
            if ax == L and elongated:
                self.quad(pts, n, end, None, mat)
            else:
                self.quad(pts, n, strip, axes[L] if ax != L else None, mat)

    def boards(self, origin, u_dir, Lu, w_dir, Lw, normal, strip='plank', mat=0):
        """Brettflaeche: Bretter laufen entlang u_dir, in Baender <= Streifenhoehe geteilt."""
        s0, s1 = STRIPS[strip]
        n = max(1, math.ceil(Lw / ((s1 - s0) * CM_PER_UV) - 1e-6))
        bw = Lw / n
        o, u, w = Vector(origin), Vector(u_dir).normalized(), Vector(w_dir).normalized()
        for i in range(n):
            a = o + w * (i * bw)
            self.quad([a, a + u * Lu, a + u * Lu + w * bw, a + w * bw], normal, strip, u, mat)

    def oct_prism(self, w, h, c, l0, l1, mat=0):
        """Achteckiges Kantholz (gebrochene Kanten c), Profil in x/y, Laenge entlang z."""
        pw, ph = w / 2, h / 2
        prof = [(-pw + c, -ph), (pw - c, -ph), (pw, -ph + c), (pw, ph - c),
                (pw - c, ph), (-pw + c, ph), (-pw, ph - c), (-pw, -ph + c)]
        for i in range(8):
            a, b = prof[i], prof[(i + 1) % 8]
            mid = Vector(((a[0] + b[0]) / 2, (a[1] + b[1]) / 2, 0)).normalized()
            self.quad([(a[0], a[1], l0), (b[0], b[1], l0), (b[0], b[1], l1), (a[0], a[1], l1)], mid, 'timber', Z, mat)
        self.quad([(p[0], p[1], l1) for p in prof], Z, 'end', None, mat)
        self.quad([(p[0], p[1], l0) for p in prof], -Z, 'end', None, mat)


# ---------------------------------------------------------------- Module
def m_foundation():
    b = Builder('SM_Building_Foundation_Wood')
    b.boards(V(-150, -150, -120), X, 300, Z, 100, -Y, 'dark')   # Sockel
    b.boards(V(-150, 150, -120), X, 300, Z, 100, Y, 'dark')
    b.boards(V(-150, -150, -120), Y, 300, Z, 100, -X, 'dark')
    b.boards(V(150, -150, -120), Y, 300, Z, 100, X, 'dark')
    b.box((-150, -150, -20), (150, -125, 0), skip=('-z',))     # Rahmen
    b.box((-150, 125, -20), (150, 150, 0), skip=('-z',))
    b.box((-150, -125, -20), (-125, 125, 0), skip=('-z', '+y', '-y'))
    b.box((125, -125, -20), (150, 125, 0), skip=('-z', '+y', '-y'))
    b.boards(V(-125, -125, -4), Y, 250, X, 250, Z, 'plank')     # Deck
    return b


def m_floor():
    b = Builder('SM_Building_Floor_Wood')
    b.boards(V(-150, -150, 0), Y, 300, X, 300, Z, 'plank')
    b.boards(V(-150, -150, -15), Y, 300, X, 300, -Z, 'dark')
    b.quad([(-150, -150, -15), (150, -150, -15), (150, -150, 0), (-150, -150, 0)], -Y, 'timber', X)
    b.quad([(-150, 150, -15), (150, 150, -15), (150, 150, 0), (-150, 150, 0)], Y, 'timber', X)
    b.quad([(-150, -150, -15), (-150, 150, -15), (-150, 150, 0), (-150, -150, 0)], -X, 'timber', Y)
    b.quad([(150, -150, -15), (150, 150, -15), (150, 150, 0), (150, -150, 0)], X, 'timber', Y)
    for yc in (-100, 100):                                      # Traeger
        b.box((-150, yc - 10, -35), (150, yc + 10, -15), skip=('+z',))
    return b


def wall_frame(b, door):
    b.box((-150, -10, 280), (150, 10, 300))                       # Rähm
    b.box((-150, -10, 20), (-137.5, 10, 280), skip=('+z', '-z'))  # Randpfosten (12,5 cm)
    b.box((137.5, -10, 20), (150, 10, 280), skip=('+z', '-z'))
    if not door:
        b.box((-150, -10, 0), (150, 10, 20), skip=('-z',))         # Schwelle


def infill(b, x0, x1, z0, z1):
    """Senkrechte Bretter, beidseitig, 12 cm stark (y -6..6)."""
    b.boards(V(x0, -6, z0), Z, z1 - z0, X, x1 - x0, -Y, 'plank')
    b.boards(V(x0, 6, z0), Z, z1 - z0, X, x1 - x0, Y, 'plank')


def brace(b, x0, z0, x1, z1, width=15):
    d = Vector((x1 - x0, 0, z1 - z0))
    L = d.length - 12
    d.normalize()
    old = b.M
    b.M = old @ frame(Vector(((x0 + x1) / 2, 0, (z0 + z1) / 2)), d, Y, Vector((-d.z, 0, d.x)))
    b.box((-L / 2, -10, -width / 2), (L / 2, 10, width / 2))
    b.M = old


def m_wall():
    b = Builder('SM_Building_Wall_Wood')
    wall_frame(b, False)
    infill(b, -137.5, 137.5, 20, 280)
    brace(b, -137.5, 20, 137.5, 280)
    return b


def m_wall_door():
    b = Builder('SM_Building_Wall_Door_Wood')
    wall_frame(b, True)
    b.box((-150, -10, 0), (-80, 10, 20), skip=('-z',))            # Schwelle links/rechts
    b.box((80, -10, 0), (150, 10, 20), skip=('-z',))
    b.box((-80, -10, 0), (-65, 10, 230), skip=('-z', '+z'))       # Tuerpfosten
    b.box((65, -10, 0), (80, 10, 230), skip=('-z', '+z'))
    b.box((-80, -10, 230), (80, 10, 250))                         # Sturz
    b.box((-65, -10, 0), (65, 10, 3), 'dark', 'dark', skip=('-z',))  # dunkle Schwelle, 3 cm
    infill(b, -137.5, -80, 20, 280)
    infill(b, 80, 137.5, 20, 280)
    infill(b, -80, 80, 250, 280)
    return b


def m_door():
    b = Builder('SM_Building_Door_Wood')
    b.boards(V(0, -3, 0), Z, 226, X, 126, -Y, 'plank')
    b.boards(V(0, 3, 0), Z, 226, X, 126, Y, 'plank')
    b.quad([(0, -3, 0), (0, 3, 0), (0, 3, 226), (0, -3, 226)], -X, 'timber', Z)
    b.quad([(126, -3, 0), (126, 3, 0), (126, 3, 226), (126, -3, 226)], X, 'timber', Z)
    b.quad([(0, -3, 226), (126, -3, 226), (126, 3, 226), (0, 3, 226)], Z, 'timber', X)
    b.quad([(0, -3, 0), (126, -3, 0), (126, 3, 0), (0, 3, 0)], -Z, 'timber', X)
    b.box((8, 3, 35), (118, 7, 50), skip=('-y',))                 # Querriegel innen
    b.box((8, 3, 176), (118, 7, 191), skip=('-y',))
    d = Vector((111 - 15, 0, 176 - 50))
    L = d.length - 6
    d.normalize()
    b.M = frame(Vector((63, 0, 113)), d, Y, Vector((-d.z, 0, d.x)))
    b.box((-L / 2, 3, -6), (L / 2, 7, 6), skip=('-y',))           # Strebe
    b.M = Matrix.Identity(4)
    b.box((2, -6, 40), (72, -3, 48), 'metal', 'metal', skip=('+y',))   # Baender aussen
    b.box((2, -6, 178), (72, -3, 186), 'metal', 'metal', skip=('+y',))
    b.box((108, -9, 100), (116, -3, 122), 'metal', 'metal', skip=('+y',))  # Griff
    return b


def m_pillar():
    b = Builder('SM_Building_Pillar_Wood')
    b.oct_prism(25, 25, 4, 0, 300)
    return b


def m_beam():
    b = Builder('SM_Building_Beam_Wood')
    b.M = frame(Vector((0, 0, 0)), Y, Z, X)
    b.oct_prism(20, 25, 4, -150, 150)
    return b


def m_roof():
    b = Builder('SM_Building_Roof_Wood')
    s = Vector((0, S45, S45))
    n = Vector((0, -S45, S45))
    t, Ls = 12.0, 150 * math.sqrt(2)
    b.boards(Vector((-150, 0, 0)) + n * t, X, 300, s, Ls, n, 'plank')   # Deckung oben
    b.boards(Vector((-150, 0, 0)), X, 300, s, Ls, -n, 'dark')           # Unterseite

    def p(x, a, d):
        return Vector((x, 0, 0)) + s * a + n * d

    b.quad([p(-150, 0, 0), p(150, 0, 0), p(150, 0, t), p(-150, 0, t)], -s, 'timber', X)
    b.quad([p(-150, Ls, 0), p(150, Ls, 0), p(150, Ls, t), p(-150, Ls, t)], s, 'timber', X)
    b.quad([p(-150, 0, 0), p(-150, Ls, 0), p(-150, Ls, t), p(-150, 0, t)], -X, 'timber', s)
    b.quad([p(150, 0, 0), p(150, Ls, 0), p(150, Ls, t), p(150, 0, t)], X, 'timber', s)
    b.M = frame(Vector((0, 0, 0)), X, s, -n)                            # Sparren unten
    for xc in (-120, 0, 120):
        b.box((xc - 6, 0, 0), (xc + 6, Ls, 15), skip=('-z',))
    b.M = Matrix.Identity(4)
    b.box((-150, -10, -10), (150, 10, 10))                              # Traufbalken
    b.M = frame(Vector((0, 0, 0)), X, s, n)                             # Turf-Kante, Material 1
    b.box((-150, 0, t), (150, 30, t + 8), 'turf', 'turf', skip=('-z',), mat=1)
    b.M = Matrix.Identity(4)
    return b


def m_ridge():
    b = Builder('SM_Building_Roof_Ridge_Wood')
    apex = Vector((0, 0, 12.0 * math.sqrt(2)))     # Oberkante Dachdeckung an der Firstlinie
    for sgn in (1, -1):
        sf = Vector((0, sgn * S45, S45))
        nf = Vector((0, -sgn * S45, S45))
        b.M = frame(apex, X, -sf, nf)
        b.box((-150, -5, 0), (150, 25, 5), skip=('-z',))
    b.M = Matrix.Identity(4)
    return b


def m_gable():
    b = Builder('SM_Building_Roof_Gable_Wood')
    w = 20.0
    top = 150 - w * math.sqrt(2)
    zs = [0, 50, 100, top]
    for i in range(3):
        z0, z1 = zs[i], zs[i + 1]
        h0, h1 = top - z0, top - z1
        for y, nrm in ((-6, -Y), (6, Y)):
            if h1 < 0.5:
                pts = [(-h0, y, z0), (h0, y, z0), (0, y, z1)]
            else:
                pts = [(-h0, y, z0), (h0, y, z0), (h1, y, z1), (-h1, y, z1)]
            b.quad(pts, nrm, 'plank', X)
    Le = 150 * math.sqrt(2)
    for sgn in (1, -1):
        e = Vector((sgn * S45, 0, S45))
        m = Vector((sgn * S45, 0, -S45))
        b.M = frame(Vector((-sgn * 150, 0, 0)), e, Y, m)
        b.box((0, -10, 0), (Le, 10, w))
    b.M = Matrix.Identity(4)
    return b


MODULES = [m_foundation, m_floor, m_wall, m_wall_door, m_door,
           m_pillar, m_beam, m_roof, m_ridge, m_gable]


# ---------------------------------------------------------------- Texturen, Materialien
def hexc(h):
    h = h.lstrip('#')
    return [int(h[i:i + 2], 16) / 255 for i in (0, 2, 4)]


def save_image(name, arr, path):
    import numpy as np
    hgt, wid = arr.shape[:2]
    img = bpy.data.images.new(name, wid, hgt, alpha=False)
    img.pixels.foreach_set(np.ascontiguousarray(arr, dtype=np.float32).ravel())
    img.filepath_raw = path
    img.file_format = 'PNG'
    img.save()
    return img


def make_trim(path, size=1024):
    import numpy as np
    rng = np.random.default_rng(7)
    grain = np.repeat(rng.random((size, size // 32)), 32, axis=1).astype(np.float32)
    img = np.ones((size, size, 4), dtype=np.float32)

    def fill(strip, col, line=None, board_cm=None, g=0.10):
        v0, v1 = STRIPS[strip]
        r0, r1 = int(v0 * size), int(v1 * size)
        c = np.array(hexc(col), dtype=np.float32)
        img[r0:r1, :, :3] = c * (1 - g + 2 * g * grain[r0:r1, :, None])
        if board_cm:
            step = board_cm / CM_PER_UV * size
            k = float(r0)
            while k < r1 - 1:
                img[int(k):int(k) + 3, :, :3] = np.array(hexc(line), dtype=np.float32)
                k += step
        img[r0:r0 + 2, :, :3] = 0.08          # Trennlinie zwischen Streifen

    fill('plank', '#8E6B46', '#4A3726', 15)
    fill('timber', '#7A5A3A', None, None, 0.14)
    fill('end', '#B08A5C', '#6B5034', 5, 0.05)
    fill('dark', '#5F4833', '#33261A', 12.5)
    fill('metal', '#2E2B28', None, None, 0.05)
    return save_image('T_Trim_Wood_Placeholder', img, path)


def make_turf(path, size=256):
    import numpy as np
    rng = np.random.default_rng(3)
    n = rng.random((size, size)).astype(np.float32)
    img = np.ones((size, size, 4), dtype=np.float32)
    img[..., :3] = np.array(hexc('#5C7F3A'), dtype=np.float32) * (0.8 + 0.4 * n[..., None])
    return save_image('T_Turf_Placeholder', img, path)


def material(name, image):
    m = bpy.data.materials.new(name)
    m.use_nodes = True
    nt = m.node_tree
    tex = nt.nodes.new('ShaderNodeTexImage')
    tex.image = image
    bsdf = nt.nodes.get('Principled BSDF')
    nt.links.new(tex.outputs['Color'], bsdf.inputs['Base Color'])
    nt.nodes.active = tex
    return m


# ---------------------------------------------------------------- Ablauf
def main():
    bpy.ops.wm.read_factory_settings(use_empty=True)
    scene = bpy.context.scene
    scene.unit_settings.system = 'METRIC'
    scene.unit_settings.scale_length = 0.01      # 1 Blender-Einheit = 1 cm

    trim = make_trim(os.path.join(OUT, 'T_Trim_Wood_Placeholder.png'))
    turf = make_turf(os.path.join(OUT, 'T_Turf_Placeholder.png'))
    m_wood = material('M_Trim_Wood', trim)
    m_turf = material('M_Turf', turf)

    objs, rows = {}, []
    for fn in MODULES:
        b = fn()
        me = bpy.data.meshes.new(b.name)
        b.bm.to_mesh(me)
        b.bm.free()
        me.materials.append(m_wood)
        if b.name == 'SM_Building_Roof_Wood':
            me.materials.append(m_turf)
        ob = bpy.data.objects.new(b.name, me)
        scene.collection.objects.link(ob)
        objs[b.name] = ob
        tris = sum(len(p.vertices) - 2 for p in me.polygons)
        xs = [v.co.x for v in me.vertices]; ys = [v.co.y for v in me.vertices]; zs = [v.co.z for v in me.vertices]
        bb = ((min(xs), min(ys), min(zs)), (max(xs), max(ys), max(zs)))
        rows.append((b.name, tris, TARGETS[b.name], bb, len(me.materials)))

        bpy.ops.object.select_all(action='DESELECT')
        ob.select_set(True)
        bpy.context.view_layer.objects.active = ob
        bpy.ops.export_scene.fbx(
            filepath=os.path.join(OUT, b.name + '.fbx'), use_selection=True, object_types={'MESH'},
            apply_unit_scale=True, apply_scale_options='FBX_SCALE_UNITS',
            axis_forward='-Y', axis_up='Z', mesh_smooth_type='FACE', add_leaf_bones=False)

    for name, tris, target, bb, nmat in rows:
        flag = 'OK' if tris <= target else ('UEBER ZIEL' if tris <= 2000 else 'UEBER OBERGRENZE')
        print('MODULE %-30s tris=%4d ziel=%4d %s bbox=%s mats=%d' % (
            name, tris, target, flag, tuple(tuple(round(c, 1) for c in p) for p in bb), nmat))

    write_readme(rows)
    render_previews(scene, objs, m_turf)


def write_readme(rows):
    lines = [
        '# Holzbaukasten – erzeugte Module (Phase 1, Priorität A)', '',
        'Erzeugt mit `tools/blender/build_wood_kit.py` (Blender, headless) nach `design/inbox/03_assets_baukasten.md`.',
        'Geometrie ist **Platzhalter in endgültigen Maßen**: richtige Größen, Pivots und UVs, aber noch ohne endgültige Textur.',
        'Einheit: 1 = 1 cm. Export: FBX, Z oben. **Import in Unreal noch nicht geprüft** (Maßstab, Ausrichtung).', '',
        '| Modul | Dreiecke | Ziel | Bounding Box min → max (cm) | Ursprung |', '|---|---:|---:|---|---|',
    ]
    for name, tris, target, bb, nmat in rows:
        mn = ', '.join('%g' % round(c, 1) for c in bb[0])
        mx = ', '.join('%g' % round(c, 1) for c in bb[1])
        lines.append('| `%s` | %d | %d | (%s) → (%s) | %s |' % (name, tris, target, mn, mx, PIVOTS[name]))
    lines += [
        '', '## Trim-Sheet-Layout (verbindlich für die echte Textur)', '',
        'Eine Textur `T_Trim_Wood` (2048², BC/N/ORM). u kachelt waagerecht, v ist in Streifen geteilt. 1 UV = 200 cm.', '',
        '| Streifen | v von–bis | Höhe real | Inhalt |', '|---|---|---|---|',
        '| plank | 0,70–1,00 | 60 cm | helle Bretter, laufen waagerecht (entlang u), 4 Bretter à 15 cm |',
        '| timber | 0,45–0,70 | 50 cm | Balkenflächen, Maserung entlang u |',
        '| end | 0,35–0,45 | 20 cm | Hirnholz, Schnittkanten |',
        '| dark | 0,10–0,35 | 50 cm | dunkle, gealterte Bretter (Sockel, Unterseiten), à 12,5 cm |',
        '| metal | 0,00–0,10 | 20 cm | dunkle Beschläge (Türbänder, Griff) |', '',
        'Turf-Kante (nur Dach, Material-Slot 2): eigene kachelbare Grastextur, 1 UV = 200 cm.',
        'Deutung: Die „schmale Turf-Kante“ liegt als 30 cm breiter, 8 cm hoher Streifen an der **Traufe** auf der Dachfläche.',
    ]
    with open(os.path.join(OUT, 'README.md'), 'w', encoding='utf-8') as f:
        f.write('\n'.join(lines) + '\n')


def place(objs, name, loc, rot_z=0.0):
    ob = objs[name].copy()
    bpy.context.scene.collection.objects.link(ob)
    ob.matrix_world = Matrix.Translation(Vector(loc)) @ Matrix.Rotation(math.radians(rot_z), 4, 'Z')
    return ob


def camera(scene, target, direction, ortho):
    cd = bpy.data.cameras.new('Cam')
    cd.type = 'ORTHO'
    cd.ortho_scale = ortho
    cd.clip_start, cd.clip_end = 1, 50000
    cam = bpy.data.objects.new('Cam', cd)
    scene.collection.objects.link(cam)
    d = Vector(direction).normalized()
    cam.location = Vector(target) + d * 8000
    cam.rotation_euler = (-d).to_track_quat('-Z', 'Y').to_euler()
    scene.camera = cam
    return cam


def render_previews(scene, objs, m_turf):
    scene.render.engine = 'BLENDER_WORKBENCH'
    sh = scene.display.shading
    sh.light = 'STUDIO'
    sh.color_type = 'TEXTURE'
    sh.show_cavity = True
    sh.show_object_outline = True
    scene.render.resolution_x, scene.render.resolution_y = 1600, 1000
    world = bpy.data.worlds.new('W')
    world.color = (0.62, 0.70, 0.74)
    scene.world = world

    # Testhaus 300 x 300 cm, First entlang X
    house = [
        place(objs, 'SM_Building_Foundation_Wood', (0, 0, 0)),
        place(objs, 'SM_Building_Wall_Door_Wood', (0, -150, 0)),
        place(objs, 'SM_Building_Door_Wood', (-63, -150, 3)),
        place(objs, 'SM_Building_Wall_Wood', (0, 150, 0)),
        place(objs, 'SM_Building_Wall_Wood', (-150, 0, 0), 90),
        place(objs, 'SM_Building_Wall_Wood', (150, 0, 0), 90),
        place(objs, 'SM_Building_Roof_Gable_Wood', (-150, 0, 300), 90),
        place(objs, 'SM_Building_Roof_Gable_Wood', (150, 0, 300), 90),
        place(objs, 'SM_Building_Roof_Wood', (0, -150, 300)),
        place(objs, 'SM_Building_Roof_Wood', (0, 150, 300), 180),
        place(objs, 'SM_Building_Roof_Ridge_Wood', (0, 0, 450)),
    ]
    for x in (-150, 150):
        for y in (-150, 150):
            house.append(place(objs, 'SM_Building_Pillar_Wood', (x, y, 0)))
    bpy.ops.mesh.primitive_plane_add(size=2400, location=(0, 0, -20))
    ground = bpy.context.active_object
    ground.data.materials.append(m_turf)

    for ob in objs.values():
        ob.hide_render = True
    camera(scene, (0, 0, 190), (-1.0, -1.7, 1.15), 980)
    scene.render.filepath = os.path.join(OUT, 'preview_house.png')
    bpy.ops.render.render(write_still=True)

    for ob in house + [ground]:
        ob.hide_render = True
    for i, ob in enumerate(objs.values()):
        ob.hide_render = False
        ob.location = (600 + (i % 5) * 450, 0 if i < 5 else 650, 0)
    camera(scene, (1500, 320, 100), (-0.55, -1.5, 1.05), 2700)
    scene.render.filepath = os.path.join(OUT, 'preview_modules.png')
    bpy.ops.render.render(write_still=True)
    print('PREVIEWS', os.path.join(OUT, 'preview_house.png'), os.path.join(OUT, 'preview_modules.png'))


main()
