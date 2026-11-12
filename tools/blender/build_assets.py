"""Erzeugt alle Blender-Assets des Projekts in einen Ordner (Standard: <Projekt>/Assets).

Aufruf (Blender headless):
  E:\\Blender\\blender.exe --background --factory-startup --python tools/blender/build_assets.py -- Assets

Inhalt:
  Textures/   T_Trim_Wood_BC/_N/_ORM (2048², Layout laut design/prompts/trim_wood.md),
              T_Turf_BC/_N (512²), T_Item_Log_BC/_N/_ORM (1024²)
              Normal Maps im DirectX-Format (Unreal), ORM = R AO, G Roughness, B Metallic
  Buildings/WoodKit/  SM_Building_*.fbx (10 Module, siehe build_wood_kit.py)
  Items/      SM_Item_Log.fbx (200 cm, Ø 30/25,5 cm, Ursprung Mitte, Längsachse X)
  UI/Icons/   T_Icon_Res_Wood.png (256², freigestellt)
  Source/     Gamer_Assets.blend, T_Icon_Res_Wood_512.png
  Previews/   Vorschaubilder
  manifest.json, README.md
"""
import bpy, bmesh, sys, os, math, json
import numpy as np
from mathutils import Vector, Matrix

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import build_wood_kit as wk  # noqa: E402

argv = sys.argv[sys.argv.index('--') + 1:] if '--' in sys.argv else []
ROOT = os.path.abspath(argv[0]) if argv else os.path.abspath('Assets')
DIRS = {k: os.path.join(ROOT, *v) for k, v in {
    'tex': ['Textures'], 'kit': ['Buildings', 'WoodKit'], 'items': ['Items'],
    'icons': ['UI', 'Icons'], 'src': ['Source'], 'prev': ['Previews']}.items()}
RNG = np.random.default_rng(42)
MANIFEST = {'generator': 'tools/blender/build_assets.py', 'units': '1 unit = 1 cm, Z up',
            'normal_maps': 'DirectX (Unreal)', 'orm': 'R=AO, G=Roughness, B=Metallic',
            'meshes': [], 'textures': [], 'icons': []}


# ================================================================ Hilfsfunktionen Textur
def hexf(h):
    h = h.lstrip('#')
    return np.array([int(h[i:i + 2], 16) / 255 for i in (0, 2, 4)], dtype=np.float32)


def pnoise(h, w, sx, sy, seed):
    """Glattes Rauschen, in beide Richtungen nahtlos (FFT-Faltung)."""
    n = np.random.default_rng(seed).standard_normal((h, w)).astype(np.float32)
    ky = np.fft.fftfreq(h)[:, None]
    kx = np.fft.rfftfreq(w)[None, :]
    g = np.exp(-2 * np.pi ** 2 * ((sx * kx) ** 2 + (sy * ky) ** 2))
    o = np.fft.irfft2(np.fft.rfft2(n) * g, s=(h, w))
    o -= o.min()
    return (o / (o.max() + 1e-8)).astype(np.float32)


def blur(a, s):
    h, w = a.shape
    ky = np.fft.fftfreq(h)[:, None]
    kx = np.fft.rfftfreq(w)[None, :]
    g = np.exp(-2 * np.pi ** 2 * s * s * (kx ** 2 + ky ** 2))
    return np.fft.irfft2(np.fft.rfft2(a) * g, s=(h, w)).astype(np.float32)


def wrap_dx(xs, cx, w):
    return ((xs - cx + w / 2) % w) - w / 2


def normal_dx(hgt, strength):
    gx = (np.roll(hgt, -1, 1) - np.roll(hgt, 1, 1)) * 0.5
    gy = np.zeros_like(hgt)
    gy[1:-1] = (hgt[2:] - hgt[:-2]) * 0.5          # Zeilen laufen nach unten
    n = np.stack([-gx * strength, -gy * strength, np.ones_like(hgt)], -1)   # DirectX: Y nach unten
    n /= np.linalg.norm(n, axis=-1, keepdims=True)
    return n * 0.5 + 0.5


def orm(hgt, rough, metal):
    ao = np.clip(1 - (blur(hgt, 6) - hgt) * 1.6, 0.55, 1.0)
    return np.stack([ao, np.clip(rough, 0, 1), np.clip(metal, 0, 1)], -1)


def save_png(name, rgb_top_down, folder, non_color=False):
    """rgb_top_down: (H, W, 3), Zeile 0 = oben. Blender erwartet Zeile 0 = unten."""
    hgt, wid = rgb_top_down.shape[:2]
    rgba = np.ones((hgt, wid, 4), dtype=np.float32)
    rgba[..., :3] = np.clip(rgb_top_down, 0, 1)
    img = bpy.data.images.new(name, wid, hgt, alpha=False)
    if non_color:
        img.colorspace_settings.name = 'Non-Color'
    img.pixels.foreach_set(np.ascontiguousarray(np.flipud(rgba)).ravel())
    path = os.path.join(folder, name + '.png')
    img.filepath_raw = path
    img.file_format = 'PNG'
    img.save()
    MANIFEST['textures'].append({'name': name, 'file': os.path.relpath(path, ROOT).replace('\\', '/'),
                                 'size': [wid, hgt], 'srgb': not non_color})
    return img


def srgb_to_lab(c):
    c = np.where(c <= 0.04045, c / 12.92, ((c + 0.055) / 1.055) ** 2.4)
    m = np.array([[0.4124, 0.3576, 0.1805], [0.2126, 0.7152, 0.0722], [0.0193, 0.1192, 0.9505]])
    xyz = m @ c / np.array([0.95047, 1.0, 1.08883])
    f = np.where(xyz > 0.008856, np.cbrt(xyz), 7.787 * xyz + 16 / 116)
    return np.array([116 * f[1] - 16, 500 * (f[0] - f[1]), 200 * (f[1] - f[2])])


def delta_e(a, b):
    return float(np.linalg.norm(srgb_to_lab(a) - srgb_to_lab(b)))


# ================================================================ Trim-Sheet Holz (2048²)
def boards(h, w, n, base, gap, seed, wear=0, knots=2):
    col = np.zeros((h, w, 3), np.float32)
    hg = np.ones((h, w), np.float32)
    grain = pnoise(h, w, 90, 1.2, seed)
    broad = pnoise(h, w, 400, 30, seed + 1)
    rows = np.arange(h, dtype=np.float32)[:, None]
    bh = h / n
    for b in range(n):
        r0, r1 = int(round(b * bh)), int(round((b + 1) * bh))
        sl = slice(r0, r1)
        f = 1.0 + RNG.uniform(-0.07, 0.07)
        col[sl] = base * f * (0.86 + 0.28 * grain[sl, :, None]) * (0.94 + 0.12 * broad[sl, :, None])
        y = rows[sl] - r0
        d = np.minimum(y, (r1 - r0 - 1) - y)
        bev = np.clip(d / 6.0, 0, 1)
        hg[sl] = 0.55 + 0.45 * bev + 0.04 * (grain[sl] - 0.5)
        col[sl] *= (0.78 + 0.22 * bev)[..., None]
        g = slice(r0, min(r0 + 3, r1))
        col[g] = gap
        hg[g] = 0.0
    xs = np.arange(w, dtype=np.float32)[None, :]
    ys = rows
    for _ in range(knots):
        b = int(RNG.integers(0, n))
        cx, cy = RNG.uniform(0, w), (b + RNG.uniform(0.35, 0.65)) * bh
        rx, ry = RNG.uniform(12, 18), RNG.uniform(7, 10)
        r = np.sqrt((wrap_dx(xs, cx, w) / rx) ** 2 + ((ys - cy) / ry) ** 2)
        m = np.clip(1.3 - r, 0, 1) * 0.75
        ring = 0.8 + 0.2 * np.cos(r * 9)
        col = col * (1 - m[..., None]) + (gap * 1.25) * ring[..., None] * m[..., None]
        hg -= m * 0.08
    for _ in range(wear):
        x0, y0 = RNG.uniform(0, w), RNG.uniform(0, h)
        ln, ang = RNG.uniform(20, 70), RNG.uniform(-0.25, 0.25)
        for t in np.linspace(0, ln, int(ln)):
            xi, yi = int(x0 + t * math.cos(ang)) % w, int(y0 + t * math.sin(ang))
            if 0 <= yi < h:
                col[yi, xi] *= 1.18
                hg[yi, xi] -= 0.04
    return col, np.clip(hg, 0, 1)


def trim_wood():
    W = 2048
    plank_c, plank_h = boards(614, W, 4, hexf('#8E6B46'), hexf('#4A3726'), 11)
    dark_c, dark_h = boards(512, W, 4, hexf('#5F4833') * 0.95 + 0.02, hexf('#33261A'), 21, wear=60)

    # Balkenfläche
    h = 512
    grain = pnoise(h, W, 120, 2.0, 31)
    broad = pnoise(h, W, 500, 60, 32)
    t = 0.55 * grain + 0.45 * broad
    xs = np.arange(W, dtype=np.float32)[None, :]
    adze = np.abs(np.sin(np.pi * xs / (W / 7)))           # 7 Dechselspuren, nahtlos
    tim_c = (hexf('#5F4833') + (hexf('#7A5A3A') - hexf('#5F4833')) * (0.35 + 0.65 * t)[..., None])
    tim_c = tim_c * (0.95 + 0.05 * adze)[..., None] * np.ones((h, 1, 1), np.float32)
    tim_h = 0.75 + 0.1 * adze * np.ones((h, 1)) + 0.05 * (grain - 0.5)

    # Hirnholz: 10 Balkenenden à 20 cm
    h = 205
    cells, cw = 10, W / 10
    ys = np.arange(h, dtype=np.float32)[:, None]
    idx = np.floor(xs / cw).astype(int)
    offs = RNG.uniform(-0.12, 0.12, (cells, 2)) * cw
    ccx = (idx + 0.5) * cw + offs[idx, 0]
    ccy = h / 2 + offs[idx, 1] * 0.5
    r = np.sqrt((xs - ccx) ** 2 + (ys - ccy) ** 2)
    nz = pnoise(h, W, 25, 25, 41)
    rings = 0.5 + 0.5 * np.sin(r * 0.6 + nz * 5)
    end_c = hexf('#B08A5C') + (hexf('#8E6B46') - hexf('#B08A5C')) * (rings ** 3)[..., None]
    fx = (xs % cw) / cw
    edge = np.clip(np.minimum(fx, 1 - fx) * cw / 3, 0, 1) * np.ones((h, 1))
    end_c *= (0.7 + 0.3 * edge)[..., None]
    end_h = 0.7 + 0.08 * rings + 0.2 * edge

    # Metall: geschmiedetes Eisen
    h = 205
    ys = np.arange(h, dtype=np.float32)[:, None]
    met_h = np.full((h, W), 0.6, np.float32)
    for _ in range(170):
        cx, cy, rad = RNG.uniform(0, W), RNG.uniform(0, h), RNG.uniform(6, 14)
        d = np.sqrt(wrap_dx(xs, cx, W) ** 2 + (ys - cy) ** 2)
        met_h -= 0.22 * np.clip(1 - (d / rad) ** 2, 0, 1)
    met_h = (met_h - met_h.min()) / (met_h.max() - met_h.min())
    met_c = hexf('#2E2B28') * 0.9 + (hexf('#4A4744') - hexf('#2E2B28')) * np.clip((met_h - 0.6) * 1.6, 0, 1)[..., None]
    rust = np.clip((pnoise(h, W, 12, 12, 51) - 0.86) * 6, 0, 1)[..., None] * 0.45
    met_c = met_c * (1 - rust) + hexf('#6B4A33') * rust

    strips = [  # (Farbe, Höhe, Rauheit, Metall)
        (plank_c, plank_h, 0.82, 0.0), (tim_c, tim_h, 0.8, 0.0), (end_c, end_h, 0.9, 0.0),
        (dark_c, dark_h, 0.88, 0.0), (met_c, met_h, 0.55, 1.0)]
    bc = np.concatenate([s[0] for s in strips], 0)
    nm = np.concatenate([normal_dx(s[1], 5.0) for s in strips], 0)
    om = np.concatenate([orm(s[1], s[2] + 0.06 * (s[1] - 0.5), np.full_like(s[1], s[3])) for s in strips], 0)
    assert bc.shape[0] == 2048
    checks = {
        'plank': delta_e(plank_c.reshape(-1, 3).mean(0), hexf('#8E6B46')),
        'timber': delta_e(tim_c.reshape(-1, 3).mean(0), hexf('#7A5A3A')),
        'end': delta_e(end_c.reshape(-1, 3).mean(0), hexf('#B08A5C')),
        'dark': delta_e(dark_c.reshape(-1, 3).mean(0), hexf('#5F4833')),
        'metal': delta_e(met_c.reshape(-1, 3).mean(0), hexf('#2E2B28')),
    }
    for k, v in checks.items():
        print('CHECK trim %-7s mittlere Farbe dE=%.1f %s' % (k, v, 'OK' if v <= 10 else 'ZU HOCH'))
    return (save_png('T_Trim_Wood_BC', bc, DIRS['tex']),
            save_png('T_Trim_Wood_N', nm, DIRS['tex'], True),
            save_png('T_Trim_Wood_ORM', om, DIRS['tex'], True))


def turf():
    n = 512
    a, b = pnoise(n, n, 6, 6, 61), pnoise(n, n, 40, 40, 62)
    t = 0.6 * a + 0.4 * b
    c = hexf('#3E6655') * 0.6 + hexf('#5C7F3A') * 0.4
    col = c + (hexf('#7A9A4A') - c) * t[..., None]
    return (save_png('T_Turf_BC', col, DIRS['tex']),
            save_png('T_Turf_N', normal_dx(t, 4.0), DIRS['tex'], True))


def log_textures():
    n, half = 1024, 512
    col = np.zeros((n, n, 3), np.float32)
    hg = np.full((n, n), 0.7, np.float32)
    # obere Hälfte: Rinde (u = Länge, v = Umfang, nahtlos)
    ridges = pnoise(half, n, 160, 3.0, 71)
    fine = pnoise(half, n, 20, 2.0, 72)
    t = np.clip(0.65 * ridges + 0.35 * fine, 0, 1)
    col[:half] = hexf('#4E3B2A') + (hexf('#79664C') - hexf('#4E3B2A')) * (t ** 1.4)[..., None]
    hg[:half] = 0.3 + 0.7 * t
    # unten links: Hirnholz-Scheibe
    ys, xs = np.mgrid[0:half, 0:half].astype(np.float32)
    r = np.sqrt((xs - 256) ** 2 + (ys - 256) ** 2) / 245.0
    nz = pnoise(half, half, 20, 20, 73)
    rings = 0.5 + 0.5 * np.sin(r * 9 * 2 * np.pi / 2 + nz * 3)
    disk = hexf('#B08A5C') + (hexf('#8E6B46') - hexf('#B08A5C')) * (rings ** 3)[..., None]
    disk = np.where((r < 0.12)[..., None], disk * 0.8, disk)
    disk = np.where((r > 0.9)[..., None], hexf('#5F4833'), disk)
    col[half:, :half] = disk
    hg[half:, :half] = 0.75 + 0.05 * rings
    # unten rechts: Astansatz
    rk = np.sqrt((xs - 256) ** 2 + (ys - 256) ** 2) / 245.0
    col[half:, half:] = hexf('#4A3726') * (0.85 + 0.15 * np.cos(rk * 30))[..., None]
    hg[half:, half:] = 0.5
    bark_de = delta_e(col[:half].reshape(-1, 3).mean(0), hexf('#5F4833'))
    end_de = delta_e(disk[r < 0.9].mean(0), hexf('#B08A5C'))
    print('CHECK log rinde dE=%.1f hirnholz dE=%.1f' % (bark_de, end_de))
    rough = np.where(np.arange(n)[:, None] < half, 0.9, 0.85) * np.ones((n, n))
    return (save_png('T_Item_Log_BC', col, DIRS['tex']),
            save_png('T_Item_Log_N', normal_dx(hg, 4.0), DIRS['tex'], True),
            save_png('T_Item_Log_ORM', orm(hg, rough, np.zeros((n, n))), DIRS['tex'], True))


# ================================================================ Materialien
def pbr_material(name, bc, nm=None, om=None):
    m = bpy.data.materials.new(name)
    m.use_nodes = True
    nt = m.node_tree
    bsdf = nt.nodes.get('Principled BSDF')
    t = nt.nodes.new('ShaderNodeTexImage')
    t.image = bc
    nt.links.new(t.outputs['Color'], bsdf.inputs['Base Color'])
    nt.nodes.active = t
    if nm is not None:   # DirectX-Normal für Blender (OpenGL) umdrehen
        tn = nt.nodes.new('ShaderNodeTexImage')
        tn.image = nm
        sep = nt.nodes.new('ShaderNodeSeparateColor')
        inv = nt.nodes.new('ShaderNodeMath')
        inv.operation = 'SUBTRACT'
        inv.inputs[0].default_value = 1.0
        comb = nt.nodes.new('ShaderNodeCombineColor')
        nmap = nt.nodes.new('ShaderNodeNormalMap')
        nt.links.new(tn.outputs['Color'], sep.inputs[0])
        nt.links.new(sep.outputs[0], comb.inputs[0])
        nt.links.new(sep.outputs[1], inv.inputs[1])
        nt.links.new(inv.outputs[0], comb.inputs[1])
        nt.links.new(sep.outputs[2], comb.inputs[2])
        nt.links.new(comb.outputs[0], nmap.inputs['Color'])
        nt.links.new(nmap.outputs['Normal'], bsdf.inputs['Normal'])
    if om is not None:
        to = nt.nodes.new('ShaderNodeTexImage')
        to.image = om
        so = nt.nodes.new('ShaderNodeSeparateColor')
        nt.links.new(to.outputs['Color'], so.inputs[0])
        nt.links.new(so.outputs[1], bsdf.inputs['Roughness'])
        nt.links.new(so.outputs[2], bsdf.inputs['Metallic'])
    return m


def outline_material():
    m = bpy.data.materials.new('M_Outline')
    m.use_nodes = True
    nt = m.node_tree
    for n in list(nt.nodes):
        if n.type != 'OUTPUT_MATERIAL':
            nt.nodes.remove(n)
    em = nt.nodes.new('ShaderNodeEmission')
    em.inputs['Color'].default_value = (*[c ** 2.2 for c in hexf('#2E2B28')], 1)
    nt.links.new(em.outputs[0], nt.nodes['Material Output'].inputs['Surface'])
    m.use_backface_culling = True
    return m


# ================================================================ Meshes
def build_log(name, length=200.0, r0=15.0, r1=12.75, seg=16, knots=True):
    bm = bmesh.new()
    uv = bm.loops.layers.uv.new('UVMap')
    xa, xb = -length / 2, length / 2
    ang = [2 * math.pi * i / seg for i in range(seg + 1)]
    A = [Vector((xa, r0 * math.cos(a), r0 * math.sin(a))) for a in ang]
    B = [Vector((xb, r1 * math.cos(a), r1 * math.sin(a))) for a in ang]

    def face(pts, uvs, outward):
        if (pts[1] - pts[0]).cross(pts[2] - pts[0]).dot(outward) < 0:
            pts, uvs = pts[::-1], uvs[::-1]
        f = bm.faces.new([bm.verts.new(p) for p in pts])
        for loop, u in zip(f.loops, uvs):
            loop[uv].uv = u
        return f

    for i in range(seg):
        mid = (A[i] + A[i + 1] + B[i] + B[i + 1]) / 4
        out = Vector((0, mid.y, mid.z))
        v0, v1 = 0.5 + 0.5 * i / seg, 0.5 + 0.5 * (i + 1) / seg
        face([A[i], B[i], B[i + 1], A[i + 1]], [(0, v0), (1, v0), (1, v1), (0, v1)], out)
    for ring, r, nx in ((A, r0, -1), (B, r1, 1)):
        pts = ring[:seg]
        uvs = [(0.25 + 0.24 * p.y / r, 0.25 + 0.24 * p.z / r) for p in pts]
        face(pts, uvs, Vector((nx, 0, 0)))
    if knots:
        for x, a in ((-40.0, math.radians(60)), (55.0, math.radians(200))):
            rr = r0 + (r1 - r0) * (x - xa) / length
            n = Vector((0, math.cos(a), math.sin(a)))
            tng = Vector((0, -math.sin(a), math.cos(a)))
            c = Vector((x, 0, 0)) + n * (rr - 0.3)
            apex = c + n * 2.8
            base = [c + (Vector((1, 0, 0)) * math.cos(k * 2 * math.pi / 5) + tng * math.sin(k * 2 * math.pi / 5)) * 3.0
                    for k in range(5)]
            for k in range(5):
                p0, p1 = base[k], base[(k + 1) % 5]
                face([p0, p1, apex], [(0.75 + 0.1 * math.cos(k), 0.25 + 0.1 * math.sin(k)),
                                      (0.75 + 0.1 * math.cos(k + 1), 0.25 + 0.1 * math.sin(k + 1)), (0.75, 0.25)],
                     ((p0 + p1 + apex) / 3) - c)
    me = bpy.data.meshes.new(name)
    bm.to_mesh(me)
    bm.free()
    ob = bpy.data.objects.new(name, me)
    bpy.context.scene.collection.objects.link(ob)
    return ob


def stats(ob):
    me = ob.data
    vs = [v.co for v in me.vertices]
    mn = [round(min(v[i] for v in vs), 1) for i in range(3)]
    mx = [round(max(v[i] for v in vs), 1) for i in range(3)]
    return sum(len(p.vertices) - 2 for p in me.polygons), mn, mx


def export_fbx(ob, folder):
    bpy.ops.object.select_all(action='DESELECT')
    ob.select_set(True)
    bpy.context.view_layer.objects.active = ob
    path = os.path.join(folder, ob.name + '.fbx')
    bpy.ops.export_scene.fbx(
        filepath=path, use_selection=True, object_types={'MESH'}, apply_unit_scale=True,
        apply_scale_options='FBX_SCALE_UNITS', axis_forward='-Y', axis_up='Z',
        mesh_smooth_type='FACE', add_leaf_bones=False, path_mode='STRIP')
    return os.path.relpath(path, ROOT).replace('\\', '/')


# ================================================================ Icon
def set_engine_eevee(scene):
    for eng in ('BLENDER_EEVEE', 'BLENDER_EEVEE_NEXT'):
        try:
            scene.render.engine = eng
            return eng
        except TypeError:
            continue
    raise RuntimeError('EEVEE nicht gefunden')


def render_icon(m_log, m_outline):
    scene = bpy.context.scene
    logs = []
    pos = [(0, -15.0, 15.0), (0, 15.0, 15.0), (0, 0.0, 15.0 + 25.98)]
    for i, p in enumerate(pos):
        ob = build_log('IconLog_%d' % i, length=120.0, r0=15.0, r1=13.5, seg=16, knots=False)
        ob.data.materials.append(m_log)
        ob.data.materials.append(m_outline)
        ob.location = p
        logs.append(ob)
    eng = set_engine_eevee(scene)
    scene.render.film_transparent = True
    scene.render.resolution_x = scene.render.resolution_y = 512
    scene.view_settings.view_transform = 'Standard'
    world = bpy.data.worlds.new('IconWorld')
    world.use_nodes = True
    world.node_tree.nodes['Background'].inputs['Color'].default_value = (0.55, 0.55, 0.55, 1)
    world.node_tree.nodes['Background'].inputs['Strength'].default_value = 0.9
    scene.world = world
    sun_d = bpy.data.lights.new('IconSun', 'SUN')
    sun_d.energy = 3.0
    sun_d.angle = math.radians(25)
    sun = bpy.data.objects.new('IconSun', sun_d)
    scene.collection.objects.link(sun)

    el, az = math.radians(30), math.radians(35)
    d = Vector((math.cos(el) * math.cos(az), math.cos(el) * math.sin(az), math.sin(el)))
    target = Vector((0, 0, 28))
    cd = bpy.data.cameras.new('IconCam')
    cd.type = 'ORTHO'
    cam = bpy.data.objects.new('IconCam', cd)
    scene.collection.objects.link(cam)
    cam.location = target + d * 1000
    cam.rotation_euler = (-d).to_track_quat('-Z', 'Y').to_euler()
    scene.camera = cam
    # Licht von oben links aus Kamerasicht
    right = (cam.rotation_euler.to_matrix() @ Vector((1, 0, 0)))
    up = (cam.rotation_euler.to_matrix() @ Vector((0, 1, 0)))
    sun.rotation_euler = (-(d * 0.6 - right * 0.8 + up * 1.0)).to_track_quat('-Z', 'Y').to_euler()
    bpy.context.view_layer.update()
    inv = cam.matrix_world.inverted()
    pts = [inv @ (ob.matrix_world @ v.co) for ob in logs for v in ob.data.vertices]
    xs, ys = [p.x for p in pts], [p.y for p in pts]
    ext = max(max(xs) - min(xs), max(ys) - min(ys))
    cd.ortho_scale = ext / 0.8
    cd.shift_x = ((max(xs) + min(xs)) / 2) / cd.ortho_scale
    cd.shift_y = ((max(ys) + min(ys)) / 2) / cd.ortho_scale
    cd.clip_start, cd.clip_end = 1, 5000
    width_cm = 2 * cd.ortho_scale / 512                  # 2 px Kontur bei 512 px
    for ob in logs:
        mod = ob.modifiers.new('Outline', 'SOLIDIFY')
        mod.thickness = width_cm
        mod.offset = 1.0
        mod.use_flip_normals = True
        mod.material_offset = 1
    try:
        scene.eevee.taa_render_samples = 64
    except AttributeError:
        pass
    keep = set(logs) | {sun, cam}
    hidden = {ob: ob.hide_render for ob in scene.objects if ob not in keep}
    for ob in hidden:
        ob.hide_render = True
    src = os.path.join(DIRS['src'], 'T_Icon_Res_Wood_512.png')
    scene.render.filepath = src
    bpy.ops.render.render(write_still=True)
    for ob, state in hidden.items():
        ob.hide_render = state
    img = bpy.data.images.load(src)
    px = np.array(img.pixels[:], dtype=np.float32).reshape(512, 512, 4)
    a = px[..., 3] > 0.5
    rows, cols = np.where(a.any(1))[0], np.where(a.any(0))[0]
    fill = max(rows[-1] - rows[0] + 1, cols[-1] - cols[0] + 1) / 512
    print('CHECK icon engine=%s fuellgrad=%.2f (Soll 0.80)' % (eng, fill))
    img.scale(256, 256)
    out = os.path.join(DIRS['icons'], 'T_Icon_Res_Wood.png')
    img.filepath_raw = out
    img.file_format = 'PNG'
    img.save()
    MANIFEST['icons'].append({'name': 'T_Icon_Res_Wood', 'for': 'item_log', 'file': os.path.relpath(out, ROOT).replace('\\', '/'),
                              'size': [256, 256], 'fill_ratio': round(float(fill), 2), 'source': 'Source/T_Icon_Res_Wood_512.png'})
    for ob in logs + [sun, cam]:
        ob.hide_render = True
        ob.hide_viewport = True


# ================================================================ Ablauf
def main():
    for p in DIRS.values():
        os.makedirs(p, exist_ok=True)
    bpy.ops.wm.read_factory_settings(use_empty=True)
    scene = bpy.context.scene
    scene.unit_settings.system = 'METRIC'
    scene.unit_settings.scale_length = 0.01
    scene.view_settings.view_transform = 'Standard'

    t_bc, t_n, t_o = trim_wood()
    tu_bc, tu_n = turf()
    l_bc, l_n, l_o = log_textures()
    m_wood = pbr_material('M_Trim_Wood', t_bc, t_n, t_o)
    m_turf = pbr_material('M_Turf', tu_bc, tu_n)
    m_log = pbr_material('M_Item_Log', l_bc, l_n, l_o)
    m_out = outline_material()

    objs = {}
    for fn in wk.MODULES:
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
        tris, mn, mx = stats(ob)
        f = export_fbx(ob, DIRS['kit'])
        MANIFEST['meshes'].append({'id': b.name, 'file': f, 'group': 'Buildings/WoodKit', 'tris': tris,
                                   'tris_target': wk.TARGETS[b.name], 'tris_cap': 2000, 'bbox_min_cm': mn,
                                   'bbox_max_cm': mx, 'pivot': wk.PIVOTS[b.name],
                                   'materials': [m.name for m in me.materials]})
        print('MESH %-30s tris=%4d ziel=%4d bbox=%s..%s' % (b.name, tris, wk.TARGETS[b.name], mn, mx))

    log = build_log('SM_Item_Log')
    log.data.materials.append(m_log)
    tris, mn, mx = stats(log)
    f = export_fbx(log, DIRS['items'])
    length, diam = mx[0] - mn[0], mx[2] - mn[2]
    ok = abs(length - 200) <= 4 and abs(diam - 30) <= 0.6 and tris <= 300
    MANIFEST['meshes'].append({'id': 'item_log', 'unreal_name': 'SM_Item_Log', 'file': f, 'group': 'Items',
                               'tris': tris, 'tris_target': 300, 'tris_cap': 1000, 'bbox_min_cm': mn,
                               'bbox_max_cm': mx, 'pivot': 'Mitte des Stamms, Laengsachse X',
                               'materials': ['M_Item_Log']})
    print('MESH SM_Item_Log tris=%d laenge=%.1f durchmesser=%.1f %s' % (tris, length, diam, 'OK' if ok else 'PRUEFEN'))
    objs['SM_Item_Log'] = log

    render_icon(m_log, m_out)

    wk.OUT = DIRS['prev']
    wk.render_previews(scene, objs, m_turf)

    blend = os.path.join(DIRS['src'], 'Gamer_Assets.blend')
    bpy.ops.wm.save_as_mainfile(filepath=blend, relative_remap=True)
    MANIFEST['source_blend'] = os.path.relpath(blend, ROOT).replace('\\', '/')
    with open(os.path.join(ROOT, 'manifest.json'), 'w', encoding='utf-8') as fh:
        json.dump(MANIFEST, fh, indent=2, ensure_ascii=False)
    write_readme()
    print('DONE', ROOT)


def write_readme():
    lines = [
        '# Assets', '',
        'Von Blender erzeugt mit `tools/blender/build_assets.py`. **Nicht von Hand bearbeiten** – Änderungen im Skript machen und neu erzeugen:',
        '', '```', 'E:\\Blender\\blender.exe --background --factory-startup --python tools/blender/build_assets.py -- Assets', '```', '',
        '## Konventionen', '',
        '- Einheit: 1 = 1 cm, Z oben. FBX mit Unit-Scale cm.',
        '- Normal Maps im **DirectX-Format** (Unreal-Standard, kein Grün-Umdrehen nötig).',
        '- `_ORM`: R = Ambient Occlusion, G = Roughness, B = Metallic. Normal und ORM sind **Non-Color** (in Unreal sRGB aus).',
        '- Ordner spiegeln die spätere Ablage in Unreal: `Assets/<Gruppe>/…` → `/Game/Gamer/<Gruppe>/…`.',
        '- `manifest.json` listet jedes Asset mit Datei, Dreiecken, Maßen, Ursprung und Materialien – für automatische Weiterverarbeitung (z. B. Import-Skript in Unreal).', '',
        '## Inhalt', '',
        '| Ordner | Inhalt |', '|---|---|',
        '| `Buildings/WoodKit/` | 10 Holz-Bauteile (FBX) |',
        '| `Items/` | `SM_Item_Log.fbx` |',
        '| `Textures/` | Trim-Sheet Holz (BC/N/ORM, 2048²), Turf (BC/N, 512²), Baumstamm (BC/N/ORM, 1024²) |',
        '| `UI/Icons/` | `T_Icon_Res_Wood.png` (256², freigestellt) |',
        '| `Source/` | `Gamer_Assets.blend` (alle Objekte), Icon in 512² |',
        '| `Previews/` | Vorschaubilder |', '',
        '## Meshes', '', '| Asset | Datei | Dreiecke | Ziel |', '|---|---|---:|---:|',
    ]
    for m in MANIFEST['meshes']:
        lines.append('| `%s` | `%s` | %d | %d |' % (m.get('unreal_name', m['id']), m['file'], m['tris'], m['tris_target']))
    lines += ['', 'Alle Texturen und Meshes sind prozedural von einem Skript erzeugt, das Claude Code geschrieben hat (Eintrag in `design/asset_log.md`).']
    with open(os.path.join(ROOT, 'README.md'), 'w', encoding='utf-8') as fh:
        fh.write('\n'.join(lines) + '\n')


if __name__ == '__main__':
    main()
