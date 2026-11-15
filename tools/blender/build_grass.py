"""
Grass clumps for PCG scattering (Static Mesh Spawner, no collision), built from the CC0 grass-blade atlas
ambientCG Foliage006 (D:/gamer/cc0_quellen/ambientcg/Foliage006; only the image files are read).
Run headless from the repository root:
  E:\\Blender\\blender.exe -b --factory-startup -P tools/blender/build_grass.py -- [--only id,id] [--assets <folder>]
      [--preview <out.png>]

Atlas: the photo holds seven single blades and two leafy stems, no ready-made tufts. The blades are found as connected
areas of the opacity map, cut out along a tight outline (a strip of a few bands that follows each blade) and packed
unscaled into a 512 x 2048 atlas, so little texture space stays empty (the two stems are left out: their outline is
only 6-9 % filled and they would need more than half of the atlas):
  T_Grass_BC.png   colour (sRGB), opacity in A
  T_Grass_N.png    normal map, DirectX (ambientCG NormalDX)
  T_Grass_ORM.png  R = AO (darker towards the blade base), G = roughness, B = metallic (0)
Clumps: 3-6 "cards" around Z; each card is a fan of cut-out blade strips with roots close together that lean outward
and bend over their flat side (so blades show their face from above, too). Custom normals bent upward.
Pivot: centre of the clump on the ground (Z = 0); blade roots start 1 cm below it, so no gap shows on slopes.
Output: Assets/Nature/Grass/SM_GrassClump_*.fbx (no collision), Assets/Nature/Textures/T_Grass_*.png, entries merged
into Assets/manifest_nature.json (all other entries stay). --preview renders the exported FBX files: the single clumps
close up (<out>_row.png) and a scattered field at eye level and from above (<out>_eye.png, <out>_top.png).
"""
import json
import math
import os
import random
import sys

import bpy
import numpy as np
from mathutils import Matrix, Vector

REPO = os.path.abspath(os.path.join(os.path.dirname(__file__), '..', '..'))
ASSETS = os.path.join(REPO, 'Assets')
SOURCE = 'D:/gamer/cc0_quellen/ambientcg/Foliage006/Foliage006_2K-JPG'
ATLAS_W, ATLAS_H = 512, 2048
MATERIAL = 'M_Nature_Grass'
TEX_KEY = 'Grass'
GROUP = 'Nature/Grass'
UP = Vector((0.0, 0.0, 1.0))
NORMAL_UP = 0.6        # custom normals: 0 = blade face, 1 = straight up
NORMAL_OUT = 0.15      # a little outward from the clump axis, so the clump reads round
ROOT_SINK = 1.0        # cm the blade roots start below the ground
CROP_PAD = 10          # source pixels kept around each blade (colour bleed for mip maps)
EDGE_MARGIN = 3        # source pixels between the blade edge and the cut-out outline

# Kinds of atlas elements: blade (short tapered blade), long (the long narrow blade), stem (stem with side leaves).
BANDS = {'blade': 4, 'long': 6, 'stem': 6}
KIND_WIDTH = {'blade': 1.0, 'long': 1.5, 'stem': 1.0}    # the long blade is much narrower in the photo
MIN_TIP_Z = 3.0        # cm: drooping blades stay above the ground (no tips poking through it)

# What is built: (asset id, unreal name, seed, spec). kinds: kind -> (weight, blade length range in cm).
# lean: angle from vertical at the root, bend: extra bending towards the tip (degrees). width: across-scale of the photo.
CLUMPS = [
    ('grass_clump_short', 'SM_GrassClump_Short', 11,
     {'cards': 6, 'per_card': (6, 8), 'kinds': {'blade': (1.0, (14, 26))}, 'spread': 4.0,
      'lean': (4, 40), 'bend': (20, 70), 'width': (0.38, 0.55), 'fan': 40, 'tris': 400}),
    ('grass_clump_medium', 'SM_GrassClump_Medium', 23,
     {'cards': 5, 'per_card': (6, 8), 'kinds': {'blade': (0.55, (24, 42)), 'long': (0.45, (38, 56))}, 'spread': 4.5,
      'lean': (3, 26), 'bend': (20, 75), 'width': (0.38, 0.55), 'fan': 35, 'tris': 400}),
    ('grass_clump_tall', 'SM_GrassClump_Tall', 37,
     {'cards': 6, 'per_card': (5, 6), 'kinds': {'long': (0.7, (62, 98)), 'blade': (0.3, (26, 44))}, 'spread': 6.0,
      'lean': (2, 20), 'bend': (20, 80), 'width': (0.4, 0.55), 'fan': 30, 'tris': 400}),
    ('grass_clump_sparse', 'SM_GrassClump_Sparse', 41,
     {'cards': 3, 'per_card': (3, 4), 'kinds': {'blade': (0.6, (16, 32)), 'long': (0.4, (26, 44))}, 'spread': 5.0,
      'lean': (6, 32), 'bend': (25, 75), 'width': (0.4, 0.6), 'fan': 45, 'tris': 400}),
]


# ----------------------------------------------------------------------------------------------------------------
# Images (numpy arrays in Blender pixel order: row 0 is the bottom of the image = v 0)
# ----------------------------------------------------------------------------------------------------------------

def load_pixels(path):
    image = bpy.data.images.load(path, check_existing=False)
    width, height = image.size
    pixels = np.empty(width * height * 4, dtype=np.float32)
    image.pixels.foreach_get(pixels)
    bpy.data.images.remove(image)
    return pixels.reshape(height, width, 4)


def save_png(name, pixels, folder, colorspace):
    height, width = pixels.shape[:2]
    image = bpy.data.images.new(name, width, height, alpha=True)
    image.colorspace_settings.name = colorspace
    image.pixels.foreach_set(np.ascontiguousarray(np.clip(pixels, 0.0, 1.0), dtype=np.float32).ravel())
    path = os.path.join(folder, name + '.png')
    image.filepath_raw = path
    image.file_format = 'PNG'
    image.save()
    bpy.data.images.remove(image)
    return {'name': name, 'file': 'Nature/Textures/%s.png' % name, 'size': [width, height],
            'srgb': colorspace == 'sRGB'}


def resize(img, nh, nw):
    """Bilinear resize (pixel centres), for the small scale step of the packing."""
    h, w = img.shape[:2]
    if (h, w) == (nh, nw):
        return img.copy()
    ys = np.clip((np.arange(nh) + 0.5) * h / nh - 0.5, 0, h - 1)
    xs = np.clip((np.arange(nw) + 0.5) * w / nw - 0.5, 0, w - 1)
    y0, x0 = np.floor(ys).astype(int), np.floor(xs).astype(int)
    y1, x1 = np.minimum(y0 + 1, h - 1), np.minimum(x0 + 1, w - 1)
    fy, fx = (ys - y0)[:, None], (xs - x0)[None, :]
    if img.ndim == 3:
        fy, fx = fy[..., None], fx[..., None]
    top = img[y0][:, x0] * (1 - fx) + img[y0][:, x1] * fx
    bottom = img[y1][:, x0] * (1 - fx) + img[y1][:, x1] * fx
    return top * (1 - fy) + bottom * fy


def push_pull(color, weight):
    """Fill pixels without weight from the surrounding colour (mip pyramid down and up), against edge halos."""
    levels = [(color * weight[..., None], weight)]
    c, w = levels[0]
    while min(c.shape[:2]) > 1:
        h2, w2 = c.shape[0] // 2, c.shape[1] // 2
        c = c[:h2 * 2, :w2 * 2].reshape(h2, 2, w2, 2, -1).sum(axis=(1, 3))
        w = w[:h2 * 2, :w2 * 2].reshape(h2, 2, w2, 2).sum(axis=(1, 3))
        levels.append((c, w))
    c, w = levels[-1]
    filled = c / np.maximum(w, 1e-8)[..., None]
    for c, w in reversed(levels[:-1]):
        up = np.repeat(np.repeat(filled, 2, axis=0), 2, axis=1)[:c.shape[0], :c.shape[1]]
        own = c / np.maximum(w, 1e-8)[..., None]
        a = np.clip(w, 0.0, 1.0)[..., None]
        filled = own * a + up * (1 - a)
    return filled


# ----------------------------------------------------------------------------------------------------------------
# Finding the blades in the atlas
# ----------------------------------------------------------------------------------------------------------------

def label_components(mask):
    """Connected areas (8-neighbourhood) of a bool mask, via runs per row and union-find. Returns an int label map."""
    parent = []

    def find(a):
        while parent[a] != a:
            parent[a] = parent[parent[a]]
            a = parent[a]
        return a

    runs, previous = [], []
    for row in range(mask.shape[0]):
        d = np.diff(np.concatenate(([0], mask[row].astype(np.int8), [0])))
        current = []
        for start, end in zip(np.flatnonzero(d == 1), np.flatnonzero(d == -1)):
            rid = len(parent)
            parent.append(rid)
            for p_start, p_end, pid in previous:
                if p_start <= end and p_end >= start:      # touching or diagonal
                    ra, rb = find(rid), find(pid)
                    if ra != rb:
                        parent[max(ra, rb)] = min(ra, rb)
            current.append((start, end, rid))
            runs.append((row, start, end, rid))
        previous = current
    labels = np.zeros(mask.shape, dtype=np.int32)
    compact = {}
    for row, start, end, rid in runs:
        root = find(rid)
        labels[row, start:end] = compact.setdefault(root, len(compact) + 1)
    return labels


def dilate(mask, radius):
    padded = np.pad(mask, radius)
    out = np.zeros_like(mask)
    h, w = mask.shape
    for dy in range(2 * radius + 1):
        for dx in range(2 * radius + 1):
            out |= padded[dy:dy + h, dx:dx + w]
    return out


def find_elements(opacity):
    """Blades and stems of the atlas: label map (full size) and one dict per element (outline, crop, kind)."""
    size = opacity.shape[0]
    half = opacity.reshape(size // 2, 2, size // 2, 2).max(axis=(1, 3)) > 0.4
    labels_half = label_components(dilate(half, 2))       # joins thin tips that the photo mask broke off
    areas = np.bincount(labels_half.ravel())
    labels = np.repeat(np.repeat(labels_half, 2, axis=0), 2, axis=1)
    elements = []
    for lab in np.flatnonzero(areas > 1500):
        if lab == 0:
            continue
        comp = (labels == lab) & (opacity > 0.35)
        rows = np.flatnonzero(comp.any(axis=1))
        cols = np.flatnonzero(comp.any(axis=0))
        if len(rows) < 40:
            continue
        r0, r1, c0, c1 = int(rows[0]), int(rows[-1]), int(cols[0]), int(cols[-1])
        sub = comp[r0:r1 + 1, c0:c1 + 1]
        has = sub.any(axis=1)
        minx = np.where(has, sub.argmax(axis=1), -1).astype(np.float64)
        maxx = np.where(has, sub.shape[1] - 1 - sub[:, ::-1].argmax(axis=1), -1).astype(np.float64)
        for arr in (minx, maxx):                           # rows without pixels: take the neighbour row
            for i in range(1, len(arr)):
                if arr[i] < 0:
                    arr[i] = arr[i - 1]
        minx += c0
        maxx += c0 + 1                                     # outline in continuous pixel coordinates
        height, width = r1 + 1 - r0, c1 + 1 - c0
        kind = 'stem' if width > 250 else ('long' if height > 1400 else 'blade')
        bands = BANDS[kind]
        ys = [r0 + height * (k / bands) ** 0.85 for k in range(bands + 1)]   # denser bands towards the tip
        left, right = [], []
        for k, y in enumerate(ys):
            lo = int(ys[max(k - 1, 0)]) - r0
            hi = int(math.ceil(ys[min(k + 1, bands)])) - r0
            lo, hi = max(lo, 0), min(max(hi, lo + 1), height)
            left.append(max(minx[lo:hi].min() - EDGE_MARGIN, c0 - CROP_PAD))
            right.append(min(maxx[lo:hi].max() + EDGE_MARGIN, c1 + 1 + CROP_PAD))
        base_rows = max(int(height * 0.03), 3)
        x_ref = float(((minx[:base_rows] + maxx[:base_rows]) * 0.5).mean())
        crop = (max(c0 - CROP_PAD, 0), max(r0 - CROP_PAD, 0), min(c1 + 1 + CROP_PAD, size), min(r1 + 1 + CROP_PAD, size))
        outline_area = sum((ys[k + 1] - ys[k]) * ((right[k] - left[k]) + (right[k + 1] - left[k + 1])) * 0.5
                           for k in range(bands))
        elements.append({'label': int(lab), 'kind': kind, 'bands': bands, 'r0': r0, 'r1': r1 + 1, 'ys': ys,
                         'left': left, 'right': right, 'x_ref': x_ref, 'crop': crop,
                         's': [(y - r0) / height for y in ys], 'fill': float(sub.sum()) / max(outline_area, 1.0)})
    # The two stems with side leaves fill only 6-9 % of their outline (much masked overdraw) and would take more
    # than half of the atlas; without them the seven blades fit unscaled into 512 x 2048.
    skipped = [e for e in elements if e['kind'] == 'stem']
    elements = [e for e in elements if e['kind'] != 'stem']
    print('GRASS: %d stem element(s) with side leaves left out (overdraw, atlas space)' % len(skipped))
    elements.sort(key=lambda e: (e['crop'][0], -e['crop'][1]))
    for index, e in enumerate(elements):
        e['id'] = index
    return labels, elements


def pack(elements):
    """Column packing of the crops into ATLAS_W x ATLAS_H; one common scale (as large as fits, at most 1)."""
    for step in range(40):
        scale = 1.0 - 0.02 * step
        columns, x_next, placed = [], 0, {}
        for e in sorted(elements, key=lambda e: -(e['crop'][3] - e['crop'][1])):
            cw, ch = e['crop'][2] - e['crop'][0], e['crop'][3] - e['crop'][1]
            sw, sh = int(math.ceil(cw * scale)), int(math.ceil(ch * scale))
            for col in columns:
                if sw <= col[1] and col[2] + sh <= ATLAS_H:
                    placed[e['id']] = (col[0], col[2], sw, sh)
                    col[2] += sh
                    break
            else:
                if x_next + sw > ATLAS_W or sh > ATLAS_H:
                    break
                columns.append([x_next, sw, sh])
                placed[e['id']] = (x_next, 0, sw, sh)
                x_next += sw
        else:
            for e in elements:
                e['place'] = placed[e['id']]
            return scale
    raise RuntimeError('grass atlas: elements do not fit')


def element_uv(e, x, y):
    cx0, cy0, cx1, cy1 = e['crop']
    px, py, sw, sh = e['place']
    return ((px + (x - cx0) * sw / (cx1 - cx0)) / ATLAS_W, (py + (y - cy0) * sh / (cy1 - cy0)) / ATLAS_H)


def to_atlas(src, elements, labels=None, fill=None):
    """Crop every element out of a source map and put it at its place in the atlas; returns map and coverage."""
    channels = src.shape[2] if src.ndim == 3 else 1
    atlas = np.zeros((ATLAS_H, ATLAS_W, channels), dtype=np.float32)
    weight = np.zeros((ATLAS_H, ATLAS_W), dtype=np.float32)
    for e in elements:
        cx0, cy0, cx1, cy1 = e['crop']
        px, py, sw, sh = e['place']
        crop = src[cy0:cy1, cx0:cx1].astype(np.float32)
        if labels is not None:                 # only this blade's own pixels (no parts of neighbours, no specks)
            crop = crop * (labels[cy0:cy1, cx0:cx1] == e['label'])
        crop = crop.reshape(cy1 - cy0, cx1 - cx0, channels)
        atlas[py:py + sh, px:px + sw] = resize(crop, sh, sw)
        weight[py:py + sh, px:px + sw] = 1.0
    if fill is not None:
        atlas[weight == 0] = fill
    return atlas, weight


def build_textures(tex_folder):
    opacity = load_pixels(SOURCE + '_Opacity.jpg')[:, :, 0]
    labels, elements = find_elements(opacity)
    scale = pack(elements)
    print('GRASS: %d atlas elements, packed at scale %.2f into %d x %d' % (len(elements), scale, ATLAS_W, ATLAS_H))
    for e in elements:
        print('GRASS:   element %d %-5s %4d px long, crop %s -> atlas %s, outline filled %.0f %%'
              % (e['id'], e['kind'], e['r1'] - e['r0'], e['crop'], e['place'], 100 * e['fill']))

    alpha, _ = to_atlas(opacity, elements, labels)
    del opacity
    color, weight = to_atlas(load_pixels(SOURCE + '_Color.jpg')[:, :, :3], elements)
    bc = np.ones((ATLAS_H, ATLAS_W, 4), dtype=np.float32)
    bc[:, :, :3] = push_pull(color, weight)
    bc[:, :, 3] = alpha[:, :, 0]

    normal, _ = to_atlas(load_pixels(SOURCE + '_NormalDX.jpg')[:, :, :3], elements, fill=(0.5, 0.5, 1.0))
    n = np.ones((ATLAS_H, ATLAS_W, 4), dtype=np.float32)
    n[:, :, :3] = normal

    rough, _ = to_atlas(load_pixels(SOURCE + '_Roughness.jpg')[:, :, 0], elements, fill=0.6)
    orm = np.ones((ATLAS_H, ATLAS_W, 4), dtype=np.float32)
    orm[:, :, 1] = rough[:, :, 0]
    orm[:, :, 2] = 0.0
    for e in elements:                       # AO: darker towards the blade base (deep inside the clump)
        px, py, sw, sh = e['place']
        cy0, cy1 = e['crop'][1], e['crop'][3]
        y_src = cy0 + (np.arange(sh) + 0.5) * (cy1 - cy0) / sh
        s = np.clip((y_src - e['r0']) / (e['r1'] - e['r0']), 0.0, 1.0)
        t = np.clip(s / 0.45, 0.0, 1.0)
        orm[py:py + sh, px:px + sw, 0] = (0.55 + 0.45 * t * t * (3 - 2 * t))[:, None]

    entries = [save_png('T_%s_BC' % TEX_KEY, bc, tex_folder, 'sRGB'),
               save_png('T_%s_N' % TEX_KEY, n, tex_folder, 'Non-Color'),
               save_png('T_%s_ORM' % TEX_KEY, orm, tex_folder, 'Non-Color')]
    coverage = float((bc[:, :, 3] > 0.33).mean())
    print('GRASS: textures written, %.0f %% of the atlas is blade' % (100 * coverage))
    return entries, elements


# ----------------------------------------------------------------------------------------------------------------
# Clump meshes
# ----------------------------------------------------------------------------------------------------------------

def spine(length, theta0, dtheta, steps=32):
    """Blade centre line in its lean plane: (horizontal, vertical, angle from vertical) at s = 0..1."""
    points = [(0.0, 0.0)]
    for i in range(steps):
        th = theta0 + dtheta * ((i + 0.5) / steps) ** 1.6
        h, z = points[-1]
        points.append((h + math.sin(th) * length / steps, z + math.cos(th) * length / steps))

    def at(s):
        f = min(max(s, 0.0), 1.0) * steps
        i = min(int(f), steps - 1)
        t = f - i
        h = points[i][0] + (points[i + 1][0] - points[i][0]) * t
        z = points[i][1] + (points[i + 1][1] - points[i][1]) * t
        return h, z, theta0 + dtheta * min(max(s, 0.0), 1.0) ** 1.6
    return at


def add_blade(md, e, root, phi, length, theta0, dtheta, width, twist0, twist1, mirror):
    direction = Vector((math.cos(phi), math.sin(phi), 0.0))
    side = UP.cross(direction)
    at = spine(length, theta0, dtheta)
    for _ in range(20):                                   # less droop if the outer half would reach the ground
        if min(at(s / 10.0)[1] for s in range(5, 11)) + root.z >= MIN_TIP_Z:
            break
        dtheta *= 0.85
        at = spine(length, theta0, dtheta)
    cm_per_px = length / (e['r1'] - e['r0'])
    sign = -1.0 if mirror else 1.0
    first = len(md['verts'])
    face_normals = []
    for k, (y, s) in enumerate(zip(e['ys'], e['s'])):
        h, z, th = at(s)
        tangent = direction * math.sin(th) + UP * math.cos(th)
        upper = -direction * math.cos(th) + UP * math.sin(th)        # the face that looks up (and inward)
        tau = twist0 + twist1 * s
        side_t = side * math.cos(tau) + tangent.cross(side) * math.sin(tau)
        upper_t = upper * math.cos(tau) + tangent.cross(upper) * math.sin(tau)
        face_normals.append(upper_t)
        centre = root + direction * h + UP * z
        for x in (e['left'][k], e['right'][k]):
            p = centre + side_t * ((x - e['x_ref']) * cm_per_px * width * sign)
            radial = Vector((p.x, p.y, 0.0))
            radial = radial.normalized() if radial.length > 0.5 else direction
            if s >= 0.5:
                md['outer_min_z'] = min(md.get('outer_min_z', 1e9), p.z)
            md['verts'].append(p)
            md['normals'].append((upper_t * (1 - NORMAL_UP) + UP * NORMAL_UP + radial * NORMAL_OUT).normalized())
            md['uvs'].append(element_uv(e, x, y))
    bands = len(e['ys']) - 1
    for k in range(bands):
        a, b = first + 2 * k, first + 2 * k + 1                         # left, right at k
        c, d = first + 2 * k + 3, first + 2 * k + 2                     # right, left at k + 1
        quad = [a, b, c, d]
        geo = (md['verts'][b] - md['verts'][a]).cross(md['verts'][d] - md['verts'][a])
        if geo.dot(face_normals[k]) < 0:                                 # front face = upper face of the blade
            quad = [a, d, c, b]
        md['faces'].append(quad)


def build_clump(name, seed, spec, elements):
    rng = random.Random(seed)
    by_kind = {}
    for e in elements:
        by_kind.setdefault(e['kind'], []).append(e)
    kinds = [k for k in spec['kinds'] if k in by_kind]
    weights = [spec['kinds'][k][0] for k in kinds]
    plan = []
    for card in range(spec['cards']):
        for _ in range(rng.randint(*spec['per_card'])):
            kind = rng.choices(kinds, weights)[0]
            plan.append((card, kind, rng.choice(by_kind[kind])))
    tris = sum(2 * p[2]['bands'] for p in plan)
    while tris > spec['tris']:                          # budget: drop blades from the fullest card
        counts = {}
        for p in plan:
            counts[p[0]] = counts.get(p[0], 0) + 1
        fullest = max(counts, key=lambda c: (counts[c], c))
        index = max(i for i, p in enumerate(plan) if p[0] == fullest)
        tris -= 2 * plan[index][2]['bands']
        plan.pop(index)

    md = {'verts': [], 'normals': [], 'uvs': [], 'faces': []}
    cards = spec['cards']
    card_dir, card_root = {}, {}
    for card in range(cards):
        phi = 2 * math.pi * card / cards + rng.uniform(-0.35, 0.35)
        radius = spec['spread'] * rng.uniform(0.1, 0.55)
        card_dir[card] = phi
        card_root[card] = Vector((math.cos(phi) * radius, math.sin(phi) * radius, -ROOT_SINK))
    for card, kind, e in plan:
        lo, hi = spec['kinds'][kind][1]
        length = rng.uniform(lo, hi)
        jitter = Vector((rng.gauss(0, 1), rng.gauss(0, 1), 0.0)) * spec['spread'] * 0.22
        phi = card_dir[card] + math.radians(rng.uniform(-spec['fan'], spec['fan']))
        theta0 = math.radians(rng.uniform(*spec['lean']))
        dtheta = math.radians(rng.uniform(*spec['bend'])) * (0.6 + 0.4 * (length - lo) / max(hi - lo, 1e-6))
        width = rng.uniform(*spec['width']) * KIND_WIDTH[kind] if kind != 'stem' else rng.uniform(0.85, 1.0)
        add_blade(md, e, card_root[card] + jitter, phi, length, theta0, dtheta, width,
                  rng.uniform(-0.35, 0.35), rng.uniform(-0.6, 0.6), rng.random() < 0.5)

    mesh = bpy.data.meshes.new(name)
    mesh.from_pydata([tuple(v) for v in md['verts']], [], md['faces'])
    mesh.validate()
    uv_layer = mesh.uv_layers.new(name='UVMap')
    for polygon in mesh.polygons:
        polygon.use_smooth = True
        for li in range(polygon.loop_start, polygon.loop_start + polygon.loop_total):
            uv_layer.data[li].uv = md['uvs'][mesh.loops[li].vertex_index]
    mesh.materials.append(bpy.data.materials.get(MATERIAL) or bpy.data.materials.new(MATERIAL))
    mesh.normals_split_custom_set_from_vertices([tuple(n) for n in md['normals']])
    mesh.update()
    obj = bpy.data.objects.new(name, mesh)
    bpy.context.scene.collection.objects.link(obj)
    wanted = np.array([tuple(md['normals'][mesh.loops[i].vertex_index]) for i in range(len(mesh.loops))])
    obj['outer_min_z'] = md.get('outer_min_z', 0.0)
    return obj, len(plan), wanted


def corner_normals(mesh):
    out = np.empty(len(mesh.loops) * 3, dtype=np.float64)
    mesh.corner_normals.foreach_get('vector', out)
    return out.reshape(-1, 3)


def angle_stats(a, b):
    dots = np.clip((a * b).sum(axis=1), -1.0, 1.0)
    angles = np.degrees(np.arccos(dots))
    return float(angles.mean()), float(angles.max())


def export(obj, folder):
    os.makedirs(folder, exist_ok=True)
    bpy.ops.object.select_all(action='DESELECT')
    obj.select_set(True)
    bpy.context.view_layer.objects.active = obj
    path = os.path.join(folder, obj.name + '.fbx')
    bpy.ops.export_scene.fbx(
        filepath=path, use_selection=True, object_types={'MESH'}, apply_unit_scale=True,
        apply_scale_options='FBX_SCALE_UNITS', axis_forward='-Y', axis_up='Z',
        mesh_smooth_type='OFF', add_leaf_bones=False, path_mode='STRIP')
    return path


def check_fbx_normals(path, wanted):
    """Import the exported FBX again and compare its corner normals with the intended custom normals."""
    before = set(bpy.data.objects)
    bpy.ops.import_scene.fbx(filepath=path)
    new = [o for o in bpy.data.objects if o not in before]
    result = None
    for obj in new:
        if obj.type == 'MESH':
            normals = corner_normals(obj.data)
            world = np.array(obj.matrix_world.to_3x3().inverted().transposed())
            normals = normals @ world.T
            normals /= np.linalg.norm(normals, axis=1, keepdims=True)
            if len(normals) == len(wanted):
                result = angle_stats(normals, wanted) + (float(normals[:, 2].mean()),)
    for obj in new:
        data = obj.data
        bpy.data.objects.remove(obj, do_unlink=True)
        if data is not None and data.users == 0 and isinstance(data, bpy.types.Mesh):
            bpy.data.meshes.remove(data)
    return result


# ----------------------------------------------------------------------------------------------------------------
# Preview (Eevee): a field of clumps on grey ground, from the exported FBX files
# ----------------------------------------------------------------------------------------------------------------

def preview_material(tex_folder):
    mat = bpy.data.materials.new('grass_preview')
    mat.use_nodes = True
    nodes, links = mat.node_tree.nodes, mat.node_tree.links
    bsdf = next(n for n in nodes if n.type == 'BSDF_PRINCIPLED')
    bc = nodes.new('ShaderNodeTexImage')
    bc.image = bpy.data.images.load(os.path.join(tex_folder, 'T_%s_BC.png' % TEX_KEY))
    links.new(bc.outputs['Color'], bsdf.inputs['Base Color'])
    clip = nodes.new('ShaderNodeMath')                       # masked like Unreal (opacity mask clip 0.333)
    clip.operation = 'GREATER_THAN'
    clip.inputs[1].default_value = 0.333
    links.new(bc.outputs['Alpha'], clip.inputs[0])
    links.new(clip.outputs['Value'], bsdf.inputs['Alpha'])
    orm = nodes.new('ShaderNodeTexImage')
    orm.image = bpy.data.images.load(os.path.join(tex_folder, 'T_%s_ORM.png' % TEX_KEY))
    orm.image.colorspace_settings.name = 'Non-Color'
    sep_orm = nodes.new('ShaderNodeSeparateColor')
    links.new(orm.outputs['Color'], sep_orm.inputs['Color'])
    links.new(sep_orm.outputs['Green'], bsdf.inputs['Roughness'])
    nimg = nodes.new('ShaderNodeTexImage')
    nimg.image = bpy.data.images.load(os.path.join(tex_folder, 'T_%s_N.png' % TEX_KEY))
    nimg.image.colorspace_settings.name = 'Non-Color'
    sep, comb = nodes.new('ShaderNodeSeparateColor'), nodes.new('ShaderNodeCombineColor')
    inv = nodes.new('ShaderNodeMath')
    inv.operation = 'SUBTRACT'
    inv.inputs[0].default_value = 1.0
    links.new(nimg.outputs['Color'], sep.inputs['Color'])
    links.new(sep.outputs['Red'], comb.inputs['Red'])
    links.new(sep.outputs['Green'], inv.inputs[1])
    links.new(inv.outputs['Value'], comb.inputs['Green'])      # DirectX -> OpenGL for Blender
    links.new(sep.outputs['Blue'], comb.inputs['Blue'])
    nmap = nodes.new('ShaderNodeNormalMap')
    links.new(comb.outputs['Color'], nmap.inputs['Color'])
    links.new(nmap.outputs['Normal'], bsdf.inputs['Normal'])
    if hasattr(mat, 'surface_render_method'):
        mat.surface_render_method = 'DITHERED'
    mat.use_backface_culling = False
    return mat


def look_at(obj, target):
    obj.rotation_euler = (Vector(target) - obj.location).to_track_quat('-Z', 'Y').to_euler()


def render_preview(out, fbx_paths, tex_folder):
    bpy.ops.wm.read_factory_settings(use_empty=True)
    scene = bpy.context.scene
    scene.unit_settings.system = 'METRIC'
    scene.unit_settings.scale_length = 0.01
    mat = preview_material(tex_folder)
    protos = {}
    for path in fbx_paths:
        before = set(bpy.data.objects)
        bpy.ops.import_scene.fbx(filepath=path)
        for obj in [o for o in bpy.data.objects if o not in before]:
            if obj.type != 'MESH':
                bpy.data.objects.remove(obj, do_unlink=True)
                continue
            for slot in obj.material_slots:
                slot.material = mat
            protos[os.path.splitext(os.path.basename(path))[0]] = obj
    names = sorted(protos)
    rng = random.Random(7)

    def place(proto, location, yaw, scale):
        inst = proto.copy()
        scene.collection.objects.link(inst)
        inst.matrix_world = (Matrix.Translation(location) @ Matrix.Rotation(yaw, 4, 'Z') @ Matrix.Scale(scale, 4)
                             @ proto.matrix_world)
        return inst

    # Single clumps in front, on bare ground, for checking each one (sorted by height: sparse, short, medium, tall).
    order = [n for n in ('SM_GrassClump_Sparse', 'SM_GrassClump_Short', 'SM_GrassClump_Medium', 'SM_GrassClump_Tall')
             if n in protos] + [n for n in names if n not in ('SM_GrassClump_Sparse', 'SM_GrassClump_Short',
                                                               'SM_GrassClump_Medium', 'SM_GrassClump_Tall')]
    widths = [max(protos[n].dimensions.x, protos[n].dimensions.y) for n in order]
    x = -(sum(widths) + 25 * (len(order) - 1)) / 2
    singles = []
    for name, width in zip(order, widths):
        place(protos[name], Vector((x + width / 2, -250, 0)), 0.0, 1.0)
        singles.append((name, x + width / 2, width, protos[name].dimensions.z))
        x += width + 25
    # A field behind (like PCG: random position, yaw and scale, about 12 clumps per square metre), mostly short grass.
    mix = [(n, w) for n, w in (('SM_GrassClump_Short', 0.55), ('SM_GrassClump_Medium', 0.25),
                               ('SM_GrassClump_Tall', 0.1), ('SM_GrassClump_Sparse', 0.1)) if n in protos]
    count = 0
    for _ in range(int(12 * 12.0 * 12.0)):
        x, y = rng.uniform(-600, 600), rng.uniform(-120, 1080)
        name = rng.choices([m[0] for m in mix], [m[1] for m in mix])[0]
        place(protos[name], Vector((x, y, 0)), rng.uniform(0, 2 * math.pi), rng.uniform(0.8, 1.2))
        count += 1
    for proto in protos.values():
        proto.hide_render = True

    bpy.ops.mesh.primitive_plane_add(size=1, location=(0, 300, 0))
    ground = bpy.context.object
    ground.scale = (6000, 6000, 1)
    gmat = bpy.data.materials.new('ground')
    gmat.use_nodes = True
    gbsdf = next(n for n in gmat.node_tree.nodes if n.type == 'BSDF_PRINCIPLED')
    gbsdf.inputs['Base Color'].default_value = (0.16, 0.155, 0.145, 1)
    gbsdf.inputs['Roughness'].default_value = 0.95
    ground.data.materials.append(gmat)

    sun_data = bpy.data.lights.new('sun', 'SUN')
    sun_data.energy = 4.0
    sun_data.angle = math.radians(1.5)
    sun = bpy.data.objects.new('sun', sun_data)
    scene.collection.objects.link(sun)
    sun.rotation_euler = (math.radians(50), 0, math.radians(35))
    world = bpy.data.worlds.new('sky')
    scene.world = world
    world.use_nodes = True
    world.node_tree.nodes['Background'].inputs['Color'].default_value = (0.55, 0.68, 0.85, 1)
    world.node_tree.nodes['Background'].inputs['Strength'].default_value = 0.9
    for engine in ('BLENDER_EEVEE_NEXT', 'BLENDER_EEVEE'):
        try:
            scene.render.engine = engine
            break
        except Exception:
            continue
    scene.render.resolution_x, scene.render.resolution_y = 1600, 900
    cam_data = bpy.data.cameras.new('cam')
    cam_data.lens = 35
    cam_data.clip_start = 5
    cam_data.clip_end = 100000
    cam = bpy.data.objects.new('cam', cam_data)
    scene.collection.objects.link(cam)
    scene.camera = cam
    stem, ext = os.path.splitext(out)
    written = []
    shots = [('_row', (0, -450, 42), (0, -250, 30), 22, (1600, 900)), ('_eye', (0, -560, 165), (0, 250, 0), 35, (1600, 900)),
             ('_top', (0, 0, 380), (0, 250, 0), 35, (1600, 900))]
    for name, cx, width, height in singles:      # close-ups of the single clumps, slightly from above
        d = max(height, width) * 1.5
        shots.append(('_' + name, (cx, -250 - d, height * 0.75), (cx, -250, height * 0.4), 35, (900, 900)))
    for suffix, location, target, lens, resolution in shots:
        cam.location = location
        cam_data.lens = lens
        scene.render.resolution_x, scene.render.resolution_y = resolution
        look_at(cam, target)
        scene.render.filepath = stem + suffix + (ext or '.png')
        bpy.ops.render.render(write_still=True)
        written.append(scene.render.filepath)
    print('GRASS: preview with %d clumps written: %s' % (count + len(names), ', '.join(written)))


# ----------------------------------------------------------------------------------------------------------------

def main():
    global ASSETS
    argv = sys.argv[sys.argv.index('--') + 1:] if '--' in sys.argv else []
    only, preview = set(), None
    for index, arg in enumerate(argv):
        if arg == '--only' and index + 1 < len(argv):
            only = {s.strip() for s in argv[index + 1].split(',') if s.strip()}
        if arg == '--assets' and index + 1 < len(argv):
            ASSETS = os.path.abspath(argv[index + 1])
        if arg == '--preview' and index + 1 < len(argv):
            preview = os.path.abspath(argv[index + 1])
    bpy.ops.wm.read_factory_settings(use_empty=True)
    bpy.context.scene.unit_settings.system = 'METRIC'
    bpy.context.scene.unit_settings.scale_length = 0.01

    tex_folder = os.path.join(ASSETS, 'Nature', 'Textures')
    os.makedirs(tex_folder, exist_ok=True)
    manifest_path = os.path.join(ASSETS, 'manifest_nature.json')
    manifest = {'generator': 'tools/blender/process_nature.py (+ tools/blender/build_trees.py)', 'units': '1 unit = 1 cm, Z up',
                'normal_maps': 'DirectX (Unreal)', 'orm': 'R=AO, G=Roughness, B=Metallic', 'textures': [], 'materials': [], 'meshes': []}
    if os.path.exists(manifest_path):
        with open(manifest_path, encoding='utf-8') as handle:
            manifest.update(json.load(handle))         # merge: everything else in the manifest stays
    textures = {t['name']: t for t in manifest['textures']}
    materials = {m['name']: m for m in manifest['materials']}
    meshes = {m['id']: m for m in manifest['meshes']}

    entries, elements = build_textures(tex_folder)
    textures.update({e['name']: e for e in entries})
    materials[MATERIAL] = {'name': MATERIAL, 'BaseColor': entries[0]['name'], 'Normal': entries[1]['name'],
                           'ORM': entries[2]['name'], 'masked': True}

    errors, paths = 0, []
    out_folder = os.path.join(ASSETS, *GROUP.split('/'))
    for asset_id, name, seed, spec in CLUMPS:
        if only and asset_id not in only:
            continue
        try:
            for obj in list(bpy.data.objects):
                bpy.data.objects.remove(obj, do_unlink=True)
            obj, blades, wanted = build_clump(name, seed, spec, elements)
            tris = sum(len(p.vertices) - 2 for p in obj.data.polygons)
            set_err = angle_stats(corner_normals(obj.data), wanted)
            path = export(obj, out_folder)
            fbx_err = check_fbx_normals(path, wanted)
            paths.append(path)
            co = np.array([v.co[:] for v in obj.data.vertices])
            lo, hi = co.min(axis=0), co.max(axis=0)
            print('GRASS: %s %d tris (budget %d), %d blades, height %.1f cm, footprint %.0f x %.0f cm, '
                  'lowest point of the outer blade halves %.1f cm' % (name, tris, spec['tris'], blades, hi[2], hi[0] - lo[0],
                                                                      hi[1] - lo[1], obj['outer_min_z']))
            print('GRASS:   custom normals in Blender: mean %.2f / max %.2f deg off; after FBX round trip: %s'
                  % (set_err[0], set_err[1], 'mean %.2f / max %.2f deg off, mean normal Z %.2f' % fbx_err
                     if fbx_err else 'NOT COMPARABLE'))
            if fbx_err is None or fbx_err[1] > 2.0:
                print('GRASS WARNING: %s: custom normals did not survive the FBX export' % name)
            meshes[asset_id] = {'id': asset_id, 'unreal_name': name, 'file': os.path.relpath(path, ASSETS).replace('\\', '/'),
                                'group': GROUP, 'tris': tris, 'tris_target': spec['tris'], 'collision': 'none',
                                'bbox_min_cm': [round(float(v), 1) for v in lo],
                                'bbox_max_cm': [round(float(v), 1) for v in hi],
                                'pivot': 'Mitte des Bueschels am Boden, Z = 0 (Halmansatz %.0f cm darunter)' % ROOT_SINK,
                                'materials': [MATERIAL],
                                'source': 'tools/blender/build_grass.py (ambientCG Foliage006, CC0; seed %d)' % seed}
        except Exception as error:
            import traceback
            traceback.print_exc()
            errors += 1
            print('GRASS ERROR: %s: %s' % (asset_id, error))

    manifest['textures'] = sorted(textures.values(), key=lambda t: t['name'])
    manifest['materials'] = sorted(materials.values(), key=lambda m: m['name'])
    manifest['meshes'] = sorted(meshes.values(), key=lambda m: m['id'])
    with open(manifest_path, 'w', encoding='utf-8') as handle:
        json.dump(manifest, handle, indent=2, ensure_ascii=False)
    print('GRASS: manifest has %d meshes, %d materials, %d textures' % (len(manifest['meshes']), len(manifest['materials']),
                                                                        len(manifest['textures'])))
    if preview and paths:
        render_preview(preview, paths, tex_folder)
    print('GRASS: done, %d error(s)' % errors)
    if errors:
        sys.exit(1)


main()
