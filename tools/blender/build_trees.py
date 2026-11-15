"""
Procedural trees, shrubs and saplings for the photorealistic vegetation (decision 2026-11-13: free tools only).
Run headless from the repository root:
  E:\\Blender\\blender.exe -b --factory-startup -P tools/blender/build_trees.py -- [--only id,id] [--assets <folder>]

Branching after the idea of Weber and Penn ("Creation and Rendering of Realistic Trees", 1995): a trunk with levels
of child branches, each species with its own angles, lengths, bending and crown shape, all from a seed.
Version 2 (2026-11-14): Scots pine with a tall clear trunk, crown in the top third, dead stubs below and a bark that
turns from thick grey-brown plates into thin orange "mirror bark" towards the top; finer branching (2-3 levels) and
many small leaf or needle cards instead of a few big ones.

Bark: one 2k atlas per species with four vertical columns (thick base bark | transition | thin upper bark | branch
bark). Each column tiles vertically; the trunk is cut into height zones and every zone maps around the trunk onto
its column, the transition zone spans its column once. That gives a height gradient with a normal material.
Pine bark from the Poly Haven `pine_bark` photo (CC0), birch bark generated. Needles: shoots cut from the Poly Haven
`pine_tree_01` twig atlas (CC0, only the images are used) and recombined into a 2k atlas of shoot clusters. Leaves:
drawn here in the botanical shape and size of each species, surface detail from ambientCG LeafSet010 (CC0).
Fruit (apples, raspberries, hazelnuts) as small models, only in the "full" state; "picked" is the same plant
without them. Collision: UCX_ convex hulls around trunk (trees) or the whole bush (shrubs) only.
Version 3 (2026-11-14, Obst und Feldbau): wild pear, apple and pear also blooming and with unripe fruit, the wild
bushes blackberry, blueberry, wild strawberry, dog rose (hips), sloe and elder; field crops (onion, carrot, potato, flax)
in three growth stages and fallen fruit as items. Crops and items use only the fruit atlas (solid colour fields).
Shared leaf atlases to keep the texture budget small: blackberry, dog rose, wild strawberry and elder use the
raspberry atlas (compound leaves), sloe and blueberry share one atlas of small elliptic leaves.
Output: Assets/Nature/Trees|Plants|Crops/*.fbx, Assets/Items/*.fbx, Assets/Nature/Textures/*.png, entries in
Assets/manifest_nature.json.
"""
import json
import math
import os
import random
import sys

import bpy
import numpy as np
from mathutils import Quaternion, Vector

REPO = os.path.abspath(os.path.join(os.path.dirname(__file__), '..', '..'))
ASSETS = os.path.join(REPO, 'Assets')
CC0 = 'D:/gamer/cc0_quellen'
PINE_TWIG = os.path.join(CC0, 'polyhaven', 'pine_tree_01', 'pine_tree_01_twig_')
UP = Vector((0.0, 0.0, 1.0))
DOWN = Vector((0.0, 0.0, -1.0))
TEX = 2048

# What is built: (asset id, species, age 0..1, seed, state, unreal name, group, triangle budget)
#   state: full (with fruit), picked (no fruit), stump (cut trunk base), plain (no fruit kind)
ASSETS_TO_BUILD = [
    ('tree_pine', 'pine', 1.0, 11, 'plain', 'SM_TreePine', 'Nature/Trees', 35000),
    ('tree_pine_young', 'pine', 0.45, 12, 'plain', 'SM_TreePine_Young', 'Nature/Trees', 18000),
    ('tree_pine_sapling', 'pine', 0.16, 13, 'plain', 'SM_TreePine_Sapling', 'Nature/Trees', 5000),
    ('tree_pine_stump', 'pine', 1.0, 11, 'stump', 'SM_TreePine_Stump', 'Nature/Trees', 1500),
    ('tree_birch', 'birch', 1.0, 21, 'plain', 'SM_TreeBirch', 'Nature/Trees', 35000),
    ('tree_birch_young', 'birch', 0.45, 22, 'plain', 'SM_TreeBirch_Young', 'Nature/Trees', 18000),
    ('tree_birch_sapling', 'birch', 0.16, 23, 'plain', 'SM_TreeBirch_Sapling', 'Nature/Trees', 5000),
    ('tree_birch_stump', 'birch', 1.0, 21, 'stump', 'SM_TreeBirch_Stump', 'Nature/Trees', 1500),
    ('tree_apple', 'apple', 1.0, 31, 'full', 'SM_TreeApple_Full', 'Nature/Trees', 30000),
    ('tree_apple_picked', 'apple', 1.0, 31, 'picked', 'SM_TreeApple_Picked', 'Nature/Trees', 30000),
    ('bush_hazel', 'hazel', 1.0, 41, 'full', 'SM_BushHazel_Full', 'Nature/Plants', 15000),
    ('bush_hazel_picked', 'hazel', 1.0, 41, 'picked', 'SM_BushHazel_Picked', 'Nature/Plants', 15000),
    ('bush_berry', 'raspberry', 1.0, 51, 'full', 'SM_BushBerry_Full', 'Nature/Plants', 12000),
    ('bush_berry_picked', 'raspberry', 1.0, 51, 'picked', 'SM_BushBerry_Picked', 'Nature/Plants', 12000),
    # Fruit cycle (2026-11-14): blossoms and unripe fruit; wild pear.
    ('tree_apple_bloom', 'apple', 1.0, 31, 'bloom', 'SM_TreeApple_Bloom', 'Nature/Trees', 32000),
    ('tree_apple_unripe', 'apple', 1.0, 31, 'unripe', 'SM_TreeApple_Unripe', 'Nature/Trees', 30000),
    ('tree_apple_young', 'apple', 0.45, 32, 'picked', 'SM_TreeApple_Young', 'Nature/Trees', 14000),
    ('tree_apple_sapling', 'apple', 0.16, 33, 'picked', 'SM_TreeApple_Sapling', 'Nature/Trees', 4000),
    ('tree_pear_young', 'pear', 0.45, 62, 'picked', 'SM_TreePear_Young', 'Nature/Trees', 14000),
    ('tree_pear_sapling', 'pear', 0.16, 63, 'picked', 'SM_TreePear_Sapling', 'Nature/Trees', 4000),
    ('tree_pear', 'pear', 1.0, 61, 'full', 'SM_TreePear_Full', 'Nature/Trees', 30000),
    ('tree_pear_picked', 'pear', 1.0, 61, 'picked', 'SM_TreePear_Picked', 'Nature/Trees', 30000),
    ('tree_pear_bloom', 'pear', 1.0, 61, 'bloom', 'SM_TreePear_Bloom', 'Nature/Trees', 32000),
    ('tree_pear_unripe', 'pear', 1.0, 61, 'unripe', 'SM_TreePear_Unripe', 'Nature/Trees', 30000),
    # Wild bushes from the reference picture (raspberry exists above, sugar melon not for now).
    ('bush_blackberry', 'blackberry', 1.0, 71, 'full', 'SM_BushBlackberry_Full', 'Nature/Plants', 14000),
    ('bush_blackberry_picked', 'blackberry', 1.0, 71, 'picked', 'SM_BushBlackberry_Picked', 'Nature/Plants', 14000),
    ('bush_blueberry', 'blueberry', 1.0, 72, 'full', 'SM_BushBlueberry_Full', 'Nature/Plants', 9000),
    ('bush_blueberry_picked', 'blueberry', 1.0, 72, 'picked', 'SM_BushBlueberry_Picked', 'Nature/Plants', 9000),
    ('plant_strawberry', 'strawberry', 1.0, 73, 'full', 'SM_PlantStrawberry_Full', 'Nature/Plants', 4000),
    ('plant_strawberry_picked', 'strawberry', 1.0, 73, 'picked', 'SM_PlantStrawberry_Picked', 'Nature/Plants', 4000),
    ('bush_rose', 'rose', 1.0, 74, 'full', 'SM_BushRose_Full', 'Nature/Plants', 14000),
    ('bush_rose_picked', 'rose', 1.0, 74, 'picked', 'SM_BushRose_Picked', 'Nature/Plants', 14000),
    ('bush_sloe', 'sloe', 1.0, 75, 'full', 'SM_BushSloe_Full', 'Nature/Plants', 16000),
    ('bush_sloe_picked', 'sloe', 1.0, 75, 'picked', 'SM_BushSloe_Picked', 'Nature/Plants', 16000),
    ('bush_elder', 'elder', 1.0, 76, 'full', 'SM_BushElder_Full', 'Nature/Plants', 16000),
    ('bush_elder_picked', 'elder', 1.0, 76, 'picked', 'SM_BushElder_Picked', 'Nature/Plants', 16000),
    # Field crops, three growth stages each (the last one = ripe; also used for the wild crop nodes).
    ('crop_onion_1', 'crop_onion', 0.3, 81, 'crop', 'SM_Crop_Onion_1', 'Nature/Crops', 1500),
    ('crop_onion_2', 'crop_onion', 0.65, 81, 'crop', 'SM_Crop_Onion_2', 'Nature/Crops', 2500),
    ('crop_onion_3', 'crop_onion', 1.0, 81, 'crop', 'SM_Crop_Onion_3', 'Nature/Crops', 3000),
    ('crop_carrot_1', 'crop_carrot', 0.3, 82, 'crop', 'SM_Crop_Carrot_1', 'Nature/Crops', 1500),
    ('crop_carrot_2', 'crop_carrot', 0.65, 82, 'crop', 'SM_Crop_Carrot_2', 'Nature/Crops', 3000),
    ('crop_carrot_3', 'crop_carrot', 1.0, 82, 'crop', 'SM_Crop_Carrot_3', 'Nature/Crops', 4000),
    ('crop_potato_1', 'crop_potato', 0.3, 83, 'crop', 'SM_Crop_Potato_1', 'Nature/Crops', 1500),
    ('crop_potato_2', 'crop_potato', 0.65, 83, 'crop', 'SM_Crop_Potato_2', 'Nature/Crops', 3500),
    ('crop_potato_3', 'crop_potato', 1.0, 83, 'crop', 'SM_Crop_Potato_3', 'Nature/Crops', 4500),
    ('crop_flax_1', 'crop_flax', 0.3, 84, 'crop', 'SM_Crop_Flax_1', 'Nature/Crops', 1500),
    ('crop_flax_2', 'crop_flax', 0.65, 84, 'crop', 'SM_Crop_Flax_2', 'Nature/Crops', 3500),
    ('crop_flax_3', 'crop_flax', 1.0, 84, 'crop', 'SM_Crop_Flax_3', 'Nature/Crops', 4000),
    # Fallen fruit lying on the ground (items.json world_mesh).
    ('item_apple', 'item_apple', 1.0, 91, 'item', 'SM_Item_Apple', 'Items', 400),
    ('item_apple_bruised', 'item_apple_bruised', 1.0, 92, 'item', 'SM_Item_AppleBruised', 'Items', 400),
    ('item_pear', 'item_pear', 1.0, 93, 'item', 'SM_Item_Pear', 'Items', 400),
    ('item_pear_bruised', 'item_pear_bruised', 1.0, 94, 'item', 'SM_Item_PearBruised', 'Items', 400),
]
SPECIES_MATERIALS = {   # bark atlas key, foliage atlas key
    'pine': ('BarkPine', 'NeedlesPine'), 'birch': ('BarkBirch', 'LeavesBirch'), 'apple': ('BarkBrown', 'LeavesApple'),
    'hazel': ('BarkBrown', 'LeavesHazel'), 'raspberry': ('BarkBrown', 'LeavesRaspberry'),
    'pear': ('BarkBrown', 'LeavesPear'), 'blackberry': ('BarkBrown', 'LeavesRaspberry'), 'rose': ('BarkBrown', 'LeavesRaspberry'),
    'strawberry': ('BarkBrown', 'LeavesRaspberry'), 'elder': ('BarkBrown', 'LeavesRaspberry'),
    'sloe': ('BarkBrown', 'LeavesSmall'), 'blueberry': ('BarkBrown', 'LeavesSmall'),
    'crop_onion': ('Fruit', 'Fruit'), 'crop_carrot': ('Fruit', 'Fruit'), 'crop_potato': ('Fruit', 'Fruit'), 'crop_flax': ('Fruit', 'Fruit'),
    'item_apple': ('Fruit', 'Fruit'), 'item_apple_bruised': ('Fruit', 'Fruit'), 'item_pear': ('Fruit', 'Fruit'),
    'item_pear_bruised': ('Fruit', 'Fruit'),
}
# Which leaf atlas a species uses (shared atlases, see the docstring).
LEAF_KIND = {'blackberry': 'raspberry', 'rose': 'raspberry', 'strawberry': 'raspberry', 'elder': 'raspberry',
             'sloe': 'small', 'blueberry': 'small'}


def leaf_kind(species):
    return LEAF_KIND.get(species, species)

# Bark atlas columns (u range): thick base bark | transition (spans once) | thin upper bark | branch bark.
BARK_COLUMNS = {'thick': (0.0, 0.375), 'trans': (0.375, 0.625), 'thin': (0.625, 0.875), 'branch': (0.875, 1.0)}
COLUMN_PAD = 1.5 / TEX


def column(key):
    u0, u1 = BARK_COLUMNS[key]
    return (u0 + COLUMN_PAD, u1 - COLUMN_PAD)


FULL_COLUMN = (0.0, 1.0)

# ----------------------------------------------------------------------------------------------------------------
# Image helpers. Arrays are in image space (row 0 = top, y down); save_png flips to Blender's bottom-up order.
# Normal maps are DirectX style: +x right, +y down (green), which is exactly image space.
# ----------------------------------------------------------------------------------------------------------------


def save_png(name, pixels, folder, colorspace):
    pixels = np.ascontiguousarray(np.clip(pixels[::-1], 0.0, 1.0), dtype=np.float32)
    height, width = pixels.shape[:2]
    image = bpy.data.images.new(name, width, height, alpha=True)
    image.colorspace_settings.name = colorspace
    image.pixels.foreach_set(pixels.ravel())
    path = os.path.join(folder, name + '.png')
    image.filepath_raw = path
    image.file_format = 'PNG'
    image.save()
    bpy.data.images.remove(image)
    return {'name': name, 'file': 'Nature/Textures/%s.png' % name, 'size': [width, height], 'srgb': colorspace == 'sRGB'}


def load_image(path):
    """Pixels of an image file in image space (rows top to bottom), float RGBA."""
    image = bpy.data.images.load(path, check_existing=False)
    width, height = image.size
    pixels = np.empty(width * height * 4, dtype=np.float32)
    image.pixels.foreach_get(pixels)
    bpy.data.images.remove(image)
    return pixels.reshape(height, width, 4)[::-1].copy()


def bilinear(img, y, x, wrap=True):
    h, w = img.shape[:2]
    y0 = np.floor(y).astype(np.int64)
    x0 = np.floor(x).astype(np.int64)
    fy, fx = (y - y0), (x - x0)
    if wrap:
        y0, x0 = y0 % h, x0 % w
        y1, x1 = (y0 + 1) % h, (x0 + 1) % w
    else:
        y1, x1 = np.clip(y0 + 1, 0, h - 1), np.clip(x0 + 1, 0, w - 1)
        y0, x0 = np.clip(y0, 0, h - 1), np.clip(x0, 0, w - 1)
    if img.ndim == 3:
        fy, fx = fy[..., None], fx[..., None]
    return (img[y0, x0] * (1 - fx) * (1 - fy) + img[y0, x1] * fx * (1 - fy)
            + img[y1, x0] * (1 - fx) * fy + img[y1, x1] * fx * fy)


def box_down(img, factor):
    if factor <= 1:
        return img
    h, w = img.shape[:2]
    h2, w2 = h // factor, w // factor
    img = img[:h2 * factor, :w2 * factor]
    shape = (h2, factor, w2, factor) + img.shape[2:]
    return img.reshape(shape).mean(axis=(1, 3))


def resize(img, out_h, out_w, wrap=True):
    h, w = img.shape[:2]
    factor = max(1, min(h // max(out_h, 1), w // max(out_w, 1)))
    img = box_down(img, factor)
    h, w = img.shape[:2]
    ys = (np.arange(out_h) + 0.5) * h / out_h - 0.5
    xs = (np.arange(out_w) + 0.5) * w / out_w - 0.5
    yy, xx = np.meshgrid(ys, xs, indexing='ij')
    return bilinear(img, yy, xx, wrap)


def box_blur(a, radius, wrap=True):
    radius = int(max(radius, 1))
    out = a
    for axis in (0, 1):
        mode = 'wrap' if wrap else 'edge'
        pad = [(0, 0)] * out.ndim
        pad[axis] = (radius + 1, radius)
        p = np.pad(out, pad, mode=mode)
        c = np.cumsum(p, axis=axis)
        n = out.shape[axis]
        hi = np.take(c, np.arange(2 * radius + 1, 2 * radius + 1 + n), axis=axis)
        lo = np.take(c, np.arange(0, n), axis=axis)
        out = (hi - lo) / (2 * radius + 1)
    return out


def tileable_noise(h, w, rng, octaves=((4, 1.0), (8, 0.5), (16, 0.25), (32, 0.12)), stretch=(1.0, 1.0)):
    """Seamless noise from sines with whole-number frequencies. stretch=(sx, sy) favours x or y variation."""
    y, x = np.mgrid[0:h, 0:w].astype(np.float32)
    y *= 2.0 * np.pi / h
    x *= 2.0 * np.pi / w
    aspect = h / w
    result = np.zeros((h, w), dtype=np.float32)
    for freq, amp in octaves:
        for _ in range(3):
            fx = max(0, int(round(rng.randint(1, freq) * stretch[0])))
            fy = int(round(rng.randint(0, freq) * aspect * stretch[1]))
            if fx == 0 and fy == 0:
                fx = 1
            result += amp * np.sin(fx * x + fy * y + rng.uniform(0, 6.283))
    result -= result.min()
    return result / max(result.max(), 1e-6)


def normal_from_height(height, strength, wrap=True):
    """DirectX normal map (image space) from a height field."""
    if wrap:
        gx = (np.roll(height, -1, axis=1) - np.roll(height, 1, axis=1)) * 0.5
        gy = (np.roll(height, -1, axis=0) - np.roll(height, 1, axis=0)) * 0.5
    else:
        gy, gx = np.gradient(height)
    n = np.dstack((-gx * strength, -gy * strength, np.ones_like(height)))
    return n / np.linalg.norm(n, axis=2, keepdims=True)


def encode_normal(n):
    out = np.ones(n.shape[:2] + (4,), dtype=np.float32)
    out[:, :, :3] = n * 0.5 + 0.5
    return out


def decode_normal(px, flip_green=False):
    n = px[:, :, :3] * 2.0 - 1.0
    if flip_green:
        n[:, :, 1] = -n[:, :, 1]
    return n / np.maximum(np.linalg.norm(n, axis=2, keepdims=True), 1e-6)


def scale_normal_xy(n, sx, sy):
    n = n.copy()
    n[:, :, 0] *= sx
    n[:, :, 1] *= sy
    return n / np.maximum(np.linalg.norm(n, axis=2, keepdims=True), 1e-6)


def rgba(rgb, alpha=None):
    out = np.ones(rgb.shape[:2] + (4,), dtype=np.float32)
    out[:, :, :3] = rgb
    if alpha is not None:
        out[:, :, 3] = alpha
    return out


def orm_map(ao, rough):
    out = np.ones(ao.shape + (4,), dtype=np.float32)
    out[:, :, 0] = ao
    out[:, :, 1] = rough
    out[:, :, 2] = 0.0
    return out


def smoothstep(a, b, x):
    t = np.clip((x - a) / (b - a), 0.0, 1.0)
    return t * t * (3 - 2 * t)


def tile_rows(img, times):
    return np.concatenate([img] * times, axis=0)


# ----------------------------------------------------------------------------------------------------------------
# Bark atlases (2k, four columns, see BARK_COLUMNS)
# ----------------------------------------------------------------------------------------------------------------

def column_widths():
    return {key: int(round((u1 - u0) * TEX)) for key, (u0, u1) in BARK_COLUMNS.items()}


def assemble_columns(parts):
    """parts: key -> (rgb, normal, ao, rough) each TEX tall and its column wide."""
    rgb = np.zeros((TEX, TEX, 3), dtype=np.float32)
    nrm = np.zeros((TEX, TEX, 3), dtype=np.float32)
    ao = np.ones((TEX, TEX), dtype=np.float32)
    rough = np.ones((TEX, TEX), dtype=np.float32)
    for key, (u0, u1) in BARK_COLUMNS.items():
        x0, x1 = int(round(u0 * TEX)), int(round(u1 * TEX))
        c, n, a, r = parts[key]
        rgb[:, x0:x1], nrm[:, x0:x1], ao[:, x0:x1], rough[:, x0:x1] = c, n, a, r
    return rgba(rgb), encode_normal(nrm), orm_map(ao, rough)


def pine_bark_atlas(rng):
    """Scots pine: thick grey-brown plates below, orange flaky mirror bark above, grey-orange branch bark."""
    prefix = os.path.join(CC0, 'polyhaven', 'pine_bark', 'pine_bark')
    photo = load_image(prefix + '_diff_2k.jpg')[:, :, :3]
    normal = decode_normal(load_image(prefix + '_nor_gl_2k.jpg'), flip_green=True)   # OpenGL -> DirectX
    ao = load_image(prefix + '_ao_2k.jpg')[:, :, 0]
    rough = load_image(prefix + '_rough_2k.jpg')[:, :, 0]
    widths = column_widths()
    lum_photo = photo.mean(axis=2)

    def thick(width, tile_h):
        reps = TEX // tile_h
        c = tile_rows(resize(photo, tile_h, width), reps)
        n = tile_rows(scale_normal_xy(resize(normal, tile_h, width), 1.5, 1.3), reps)
        a = tile_rows(resize(ao, tile_h, width), reps)
        r = tile_rows(resize(rough, tile_h, width), reps)
        lum = c.mean(axis=2, keepdims=True)
        grey = lum * np.array([1.0, 0.93, 0.86], dtype=np.float32)
        c = (c * 0.68 + grey * 0.32) * (0.45 + 0.55 * a[:, :, None])     # greyer, deeper fissures
        return c, n, a * 0.9 + 0.1 * a ** 2, np.clip(r * 0.9 + 0.1, 0, 1)

    def thin(width, tile_h):
        """Thin papery flakes, peeling horizontally: the bark photo turned by 90 degrees, flattened and tinted."""
        reps = TEX // tile_h
        turned_l = np.rot90(lum_photo)
        turned_ao = np.rot90(ao)
        detail = tile_rows(resize(turned_l, tile_h, width), reps) * 0.4 + tile_rows(resize(lum_photo, tile_h, width), reps) * 0.6
        detail = detail / max(detail.mean(), 1e-6)
        aot = tile_rows(resize(turned_ao, tile_h, width), reps) * 0.4 + tile_rows(resize(ao, tile_h, width), reps) * 0.6
        bands = tileable_noise(TEX, width, rng, ((3, 1.0), (7, 0.6), (15, 0.35), (31, 0.2)), stretch=(0.25, 2.5))
        flakes = tileable_noise(TEX, width, rng, ((6, 1.0), (13, 0.5), (27, 0.3)), stretch=(0.5, 1.8))
        orange = np.array([0.78, 0.43, 0.20], dtype=np.float32)
        pale = np.array([0.86, 0.60, 0.36], dtype=np.float32)
        greyish = np.array([0.50, 0.40, 0.33], dtype=np.float32)
        mix_pale = smoothstep(0.55, 0.85, bands)[:, :, None] * 0.45
        mix_grey = smoothstep(0.62, 0.9, flakes)[:, :, None] * 0.75
        c = orange * (1 - mix_pale) + pale * mix_pale
        c = c * (1 - mix_grey) + greyish * mix_grey
        c = c * np.clip(0.55 + 0.45 * detail, 0.4, 1.3)[:, :, None] * (0.8 + 0.2 * aot[:, :, None])
        edge = np.abs(np.roll(flakes, 2, axis=0) - flakes)
        c = c * (1 - np.clip(edge * 4, 0, 0.2))[:, :, None]                   # thin dark flake edges
        height = 0.25 * flakes + 0.15 * bands + 0.5 * aot
        n = normal_from_height(height, 9.0 * width / 512.0)
        a = 0.75 + 0.25 * aot
        r = 0.55 + 0.25 * (1 - flakes)
        return c, n, a, r

    c_thick, n_thick, a_thick, r_thick = thick(widths['thick'], 1024)
    c_thin, n_thin, a_thin, r_thin = thin(widths['thin'], 512)
    # Transition: thick plates below, orange flakes first on the plates, then everywhere; not tiled vertically.
    w = widths['trans']
    tc, tn, ta, tr = thick(w, 1024)
    hc, hn, ha, hr = thin(w, 512)
    gradient = np.linspace(1.0, 0.0, TEX, dtype=np.float32)[:, None]       # row 0 = top of the zone (v = 1)
    noise = tileable_noise(TEX, w, rng, ((3, 1.0), (7, 0.5), (15, 0.25)), stretch=(1.0, 0.6))
    plates = box_blur(ta, 6)
    m = np.clip((gradient - 0.55) * 1.9 + (noise - 0.5) * 0.9 + (plates - 0.6) * 1.0 + 0.5, 0, 1)
    m = smoothstep(0.0, 1.0, m)
    c_tr = tc * (1 - m[:, :, None]) + hc * m[:, :, None]
    n_tr = tn * (1 - m[:, :, None]) + hn * m[:, :, None]
    n_tr /= np.linalg.norm(n_tr, axis=2, keepdims=True)
    a_tr = ta * (1 - m) + ha * m
    r_tr = tr * (1 - m) + hr * m
    # Branches: grey-brown with orange flakes, finer.
    wb = widths['branch']
    bc, bn, ba, br = thin(wb, 256)
    lum = bc.mean(axis=2, keepdims=True)
    bc = bc * 0.55 + lum * np.array([0.95, 0.80, 0.68], dtype=np.float32) * 0.45
    bc *= 0.8
    parts = {'thick': (c_thick, n_thick, a_thick, r_thick), 'trans': (c_tr, n_tr, a_tr, r_tr),
             'thin': (c_thin, n_thin, a_thin, r_thin), 'branch': (bc, bn, ba, br)}
    return assemble_columns(parts)


def lenticels(h, w, rng, count, length, thickness, wrap=True):
    """Short dark horizontal dashes (birch lenticels), tileable."""
    marks = np.zeros((h, w), dtype=np.float32)
    for _ in range(count):
        cx, cy = rng.uniform(0, w), rng.uniform(0, h)
        half_w = rng.uniform(*length)
        half_h = rng.uniform(*thickness)
        x0, x1 = int(cx - half_w - 2), int(cx + half_w + 3)
        y0, y1 = int(cy - half_h - 2), int(cy + half_h + 3)
        xs = np.arange(x0, x1)
        ys = np.arange(y0, y1)
        dx = (xs - cx)[None, :]
        dy = (ys - cy)[:, None]
        v = np.clip(1.0 - (dx / half_w) ** 2 - (dy / half_h) ** 2, 0, 1) * rng.uniform(0.6, 1.0)
        yi, xi = np.meshgrid(ys % h, xs % w, indexing='ij')
        np.maximum.at(marks, (yi, xi), v)
    return marks


def birch_bark_atlas(rng):
    """Silver birch: black fissured base, white bark with dark lenticels above, dark red-brown twig bark."""
    widths = column_widths()

    def fissured(width):
        ridges = tileable_noise(TEX, width, rng, ((4, 1.0), (9, 0.6), (19, 0.35), (41, 0.2)), stretch=(2.2, 0.25))
        cells = tileable_noise(TEX, width, rng, ((5, 1.0), (11, 0.5)), stretch=(1.0, 0.6))
        crack = smoothstep(0.35, 0.6, ridges)
        top = smoothstep(0.55, 0.9, ridges * 0.7 + cells * 0.3)
        dark = np.array([0.07, 0.065, 0.06], dtype=np.float32)
        body = np.array([0.22, 0.21, 0.20], dtype=np.float32)
        light = np.array([0.62, 0.60, 0.56], dtype=np.float32)
        c = dark * (1 - crack[:, :, None]) + body * crack[:, :, None]
        c = c * (1 - top[:, :, None] * 0.85) + light * top[:, :, None] * 0.85
        height = crack * 0.7 + top * 0.3
        return c, normal_from_height(height, 14.0 * width / 512.0), 0.45 + 0.55 * crack, 0.85 - 0.1 * top

    def white(width):
        base = tileable_noise(TEX, width, rng, ((3, 1.0), (9, 0.4), (27, 0.15)), stretch=(1.0, 0.7))
        c = np.empty((TEX, width, 3), dtype=np.float32)
        chalk = np.array([0.86, 0.85, 0.80], dtype=np.float32)
        grey = np.array([0.66, 0.65, 0.61], dtype=np.float32)
        peach = np.array([0.82, 0.70, 0.60], dtype=np.float32)
        mix = smoothstep(0.55, 0.85, base)[:, :, None]
        c[:] = chalk * (1 - mix) + grey * mix
        peel = smoothstep(0.75, 0.95, tileable_noise(TEX, width, rng, ((7, 1.0), (15, 0.5)), stretch=(0.4, 2.0)))
        c = c * (1 - peel[:, :, None] * 0.5) + peach * peel[:, :, None] * 0.5
        bands = tileable_noise(TEX, width, rng, ((5, 1.0), (11, 0.5)), stretch=(0.15, 3.0))
        band = smoothstep(0.8, 0.95, bands) * 0.45                       # faint horizontal grey rings
        c *= (1 - band)[:, :, None]
        marks = lenticels(TEX, width, rng, int(260 * width / 512), (width * 0.012, width * 0.06), (1.0, 3.0))
        scars = np.zeros_like(marks)
        for _ in range(int(6 * width / 512)):
            cx, cy, r = rng.uniform(0, width), rng.uniform(0, TEX), rng.uniform(width * 0.02, width * 0.05)
            yy, xx = np.mgrid[0:TEX, 0:width].astype(np.float32)
            dx = np.minimum(np.abs(xx - cx), width - np.abs(xx - cx))
            dy = np.minimum(np.abs(yy - cy), TEX - np.abs(yy - cy))
            scars = np.maximum(scars, np.clip(1.0 - (dx * 1.2 + dy) / (r * 1.8), 0, 1) * 1.6)
        marks = np.clip(np.maximum(marks, scars), 0, 1)
        dark = np.array([0.09, 0.08, 0.08], dtype=np.float32)
        c = c * (1 - marks[:, :, None]) + dark * marks[:, :, None]
        height = base * 0.25 - marks * 0.5 + peel * 0.2
        return c, normal_from_height(height, 6.0 * width / 512.0), 1.0 - marks * 0.3, 0.55 + marks * 0.3

    def twig(width):
        base = tileable_noise(TEX, width, rng, ((4, 1.0), (9, 0.5), (19, 0.25)), stretch=(1.0, 0.8))
        c = np.empty((TEX, width, 3), dtype=np.float32)
        c[:] = np.array([0.24, 0.15, 0.12], dtype=np.float32)
        c *= (0.8 + 0.4 * base)[:, :, None]
        marks = lenticels(TEX, width, rng, int(200 * width / 256), (1.5, 4.0), (0.6, 1.2))
        c = c * (1 - marks[:, :, None] * 0.6) + np.array([0.62, 0.58, 0.52]) * marks[:, :, None] * 0.6
        return c, normal_from_height(base * 0.3 + marks * 0.2, 4.0), np.ones((TEX, width), np.float32) * 0.95, 0.6 + 0.1 * base

    c_thick, n_thick, a_thick, r_thick = fissured(widths['thick'])
    c_thin, n_thin, a_thin, r_thin = white(widths['thin'])
    w = widths['trans']
    fc, fn, fa, fr = fissured(w)
    wc, wn, wa, wr = white(w)
    gradient = np.linspace(1.0, 0.0, TEX, dtype=np.float32)[:, None]
    streaks = tileable_noise(TEX, w, rng, ((3, 1.0), (7, 0.6), (15, 0.3)), stretch=(2.0, 0.3))
    m = np.clip((gradient - 0.55) * 1.6 + (streaks - 0.5) * 1.5 + 0.5, 0, 1)
    m = smoothstep(0.0, 1.0, m)[:, :, None]
    c_tr = fc * (1 - m) + wc * m
    n_tr = fn * (1 - m) + wn * m
    n_tr /= np.linalg.norm(n_tr, axis=2, keepdims=True)
    parts = {'thick': (c_thick, n_thick, a_thick, r_thick),
             'trans': (c_tr, n_tr, fa * (1 - m[:, :, 0]) + wa * m[:, :, 0], fr * (1 - m[:, :, 0]) + wr * m[:, :, 0]),
             'thin': (c_thin, n_thin, a_thin, r_thin), 'branch': twig(widths['branch'])}
    return assemble_columns(parts)


def brown_bark(rng):
    """Poly Haven bark_brown_02 (apple, hazel, raspberry): one tiling texture, normal OpenGL -> DirectX."""
    prefix = os.path.join(CC0, 'polyhaven', 'bark_brown_02', 'bark_brown_02')
    color = load_image(prefix + '_diff_2k.jpg')[:, :, :3]
    normal = decode_normal(load_image(prefix + '_nor_gl_2k.jpg'), flip_green=True)
    ao = load_image(prefix + '_ao_2k.jpg')[:, :, 0]
    rough = load_image(prefix + '_rough_2k.jpg')[:, :, 0]
    color = color * (0.7 + 0.3 * ao[:, :, None])
    return rgba(color), encode_normal(normal), orm_map(ao, rough)


def end_grain(size, rng):
    """Cut wood: pale with growth rings and a darker bark edge, centred (stump caps map the whole square)."""
    yy, xx = np.mgrid[0:size, 0:size].astype(np.float32)
    r = np.sqrt((xx - size / 2) ** 2 + (yy - size / 2) ** 2) / (size / 2)
    wobble = tileable_noise(size, size, rng, ((3, 1.0), (7, 0.3))) * 0.08
    rings = 0.5 + 0.5 * np.sin((r + wobble) * 2 * np.pi * 22)
    light = np.array([0.80, 0.66, 0.46]); dark = np.array([0.62, 0.45, 0.28])
    color = light * (1 - rings[:, :, None] * 0.6) + dark * rings[:, :, None] * 0.6
    edge = np.clip((r - 0.9) * 10, 0, 1)[:, :, None]
    color = color * (1 - edge) + np.array([0.25, 0.17, 0.11]) * edge
    n = normal_from_height(rings * 0.2 + edge[:, :, 0] * 0.3, 3.0, wrap=False)
    return rgba(color), encode_normal(n), orm_map(np.ones((size, size), np.float32), np.full((size, size), 0.8, np.float32))


# Fruit atlas: 5 x 5 colour fields (fruit, blossoms, crop parts). Each field: colour a -> b across, a little noise.
FRUIT_CELLS = 5
FRUIT_FIELDS = [
    ('apple', (0.74, 0.12, 0.05), (0.86, 0.42, 0.06)),          # red with a yellow-green side
    ('raspberry', (0.62, 0.03, 0.07), (0.72, 0.03, 0.12)),
    ('hazelnut', (0.66, 0.47, 0.25), (0.74, 0.52, 0.25)),
    ('husk', (0.34, 0.45, 0.17), (0.39, 0.45, 0.17)),
    ('pear', (0.62, 0.62, 0.20), (0.70, 0.52, 0.18)),           # yellow-green, russet side
    ('blackberry', (0.05, 0.03, 0.06), (0.12, 0.06, 0.14)),
    ('blueberry', (0.13, 0.17, 0.34), (0.22, 0.27, 0.45)),      # dusty blue bloom
    ('strawberry', (0.78, 0.06, 0.05), (0.85, 0.12, 0.08)),
    ('rosehip', (0.82, 0.22, 0.04), (0.70, 0.10, 0.04)),
    ('sloe', (0.16, 0.18, 0.32), (0.28, 0.30, 0.45)),           # blue-black with pale bloom
    ('elderberry', (0.07, 0.03, 0.08), (0.14, 0.05, 0.14)),
    ('unripe', (0.36, 0.52, 0.14), (0.45, 0.58, 0.18)),         # small green fruit
    ('blossom', (0.96, 0.94, 0.90), (0.95, 0.85, 0.86)),        # white with a pink touch
    ('bruised', (0.62, 0.18, 0.05), (0.32, 0.18, 0.07)),        # apple with brown bruises
    ('bruised_pear', (0.60, 0.56, 0.20), (0.36, 0.26, 0.10)),
    ('leaf', (0.18, 0.36, 0.10), (0.24, 0.44, 0.13)),           # crop leaves (solid, no alpha)
    ('leaf_old', (0.42, 0.44, 0.16), (0.50, 0.42, 0.18)),       # yellowing potato haulm
    ('carrot', (0.86, 0.40, 0.06), (0.92, 0.48, 0.10)),
    ('potato', (0.66, 0.52, 0.32), (0.58, 0.44, 0.26)),
    ('onion', (0.70, 0.46, 0.20), (0.80, 0.58, 0.30)),
    ('flax_flower', (0.36, 0.52, 0.86), (0.48, 0.62, 0.92)),
    ('straw', (0.72, 0.60, 0.30), (0.80, 0.68, 0.38)),
    ('stem', (0.30, 0.42, 0.14), (0.34, 0.46, 0.16)),
    ('soil', (0.22, 0.15, 0.09), (0.26, 0.18, 0.11)),
    ('wood', (0.36, 0.25, 0.15), (0.42, 0.30, 0.18)),
]
FRUIT_UV = {name: ((index % FRUIT_CELLS) / FRUIT_CELLS, 1.0 - (index // FRUIT_CELLS + 1) / FRUIT_CELLS)
            for index, (name, _, _) in enumerate(FRUIT_FIELDS)}


def fruit_cell_rect(name, pad=0.12):
    """UV rectangle (u0, v0, u1, v1) inside a colour field, for leaf blades and other flat parts."""
    u0, v0 = FRUIT_UV[name]
    cell = 1.0 / FRUIT_CELLS
    return (u0 + cell * pad, v0 + cell * pad, u0 + cell * (1 - pad), v0 + cell * (1 - pad))


def fruit_texture(size=640):
    """Colour fields for fruit, blossoms and crop parts (image space: row 0 = top = v 1)."""
    cell = size // FRUIT_CELLS
    canvas = np.ones((size, size, 3), dtype=np.float32) * 0.5
    rough = np.full((size, size), 0.55, dtype=np.float32)
    yy, xx = np.mgrid[0:cell, 0:cell].astype(np.float32) / cell
    rng = np.random.default_rng(7)
    for index, (name, a, b) in enumerate(FRUIT_FIELDS):
        r, c = divmod(index, FRUIT_CELLS)
        t = np.clip(0.6 * xx + 0.4 * yy, 0, 1)[..., None]
        color = np.array(a, np.float32) * (1 - t) + np.array(b, np.float32) * t
        noise = rng.normal(0.0, 0.025, (cell, cell, 1)).astype(np.float32)
        if name.startswith('bruised'):
            # Brown, soft patches (bruises) on the fruit colour.
            spots = np.zeros((cell, cell), np.float32)
            for _ in range(5):
                cy, cx, rad = rng.uniform(0.15, 0.85), rng.uniform(0.15, 0.85), rng.uniform(0.08, 0.18)
                spots = np.maximum(spots, np.clip(1 - ((yy - cy) ** 2 + (xx - cx) ** 2) / rad ** 2, 0, 1))
            color = color * (1 - spots[..., None] * 0.65) + np.array((0.24, 0.13, 0.05), np.float32) * spots[..., None] * 0.65
        canvas[r * cell:(r + 1) * cell, c * cell:(c + 1) * cell] = np.clip(color + noise, 0, 1)
        if name in ('blueberry', 'sloe'):
            rough[r * cell:(r + 1) * cell, c * cell:(c + 1) * cell] = 0.8     # waxy bloom
        elif name in ('apple', 'pear', 'blackberry', 'elderberry', 'rosehip', 'strawberry'):
            rough[r * cell:(r + 1) * cell, c * cell:(c + 1) * cell] = 0.35
    flat = np.zeros((size, size, 3), dtype=np.float32); flat[:, :, 2] = 1.0
    return rgba(canvas), encode_normal(flat), orm_map(np.ones((size, size), np.float32), rough)


# ----------------------------------------------------------------------------------------------------------------
# Foliage atlases. Each atlas is a grid of cells; a card shows one cell, its stalk at the bottom centre of the cell.
# ----------------------------------------------------------------------------------------------------------------

class Layers:
    """Colour, alpha, normal, AO and roughness canvases for painting foliage atlases (image space)."""

    def __init__(self, size, fill):
        self.rgb = np.empty((size, size, 3), dtype=np.float32)
        self.rgb[:] = np.array(fill, dtype=np.float32)
        self.a = np.zeros((size, size), dtype=np.float32)
        self.n = np.zeros((size, size, 3), dtype=np.float32)
        self.n[:, :, 2] = 1.0
        self.ao = np.ones((size, size), dtype=np.float32)
        self.rough = np.full((size, size), 0.55, dtype=np.float32)
        self.size = size

    def paint(self, y0, x0, cover, rgb, normal=None, ao=None, rough=None):
        """Alpha-over a patch (cover = 0..1 coverage) at (y0, x0); clipped to the canvas."""
        h, w = cover.shape
        ys0, xs0 = max(y0, 0), max(x0, 0)
        ys1, xs1 = min(y0 + h, self.size), min(x0 + w, self.size)
        if ys0 >= ys1 or xs0 >= xs1:
            return
        sl = (slice(ys0 - y0, ys1 - y0), slice(xs0 - x0, xs1 - x0))
        dst = (slice(ys0, ys1), slice(xs0, xs1))
        c = cover[sl][:, :, None]
        self.rgb[dst] = self.rgb[dst] * (1 - c) + rgb[sl] * c
        self.a[dst] = self.a[dst] + cover[sl] * (1 - self.a[dst])
        if normal is not None:
            self.n[dst] = self.n[dst] * (1 - c) + normal[sl] * c
        if ao is not None:
            self.ao[dst] = self.ao[dst] * (1 - c[:, :, 0]) + ao[sl] * c[:, :, 0]
        if rough is not None:
            self.rough[dst] = self.rough[dst] * (1 - c[:, :, 0]) + rough[sl] * c[:, :, 0]

    def finish(self, cluster_ao=0.35, blur=None, alpha_gamma=0.85):
        blur = blur or self.size // 48
        shade = box_blur(self.a, blur, wrap=False)
        ao = self.ao * (1.0 - cluster_ao * np.clip(shade * 1.4, 0, 1))
        n = self.n / np.maximum(np.linalg.norm(self.n, axis=2, keepdims=True), 1e-6)
        alpha = np.clip(self.a, 0, 1) ** alpha_gamma
        return rgba(self.rgb, alpha), encode_normal(n), orm_map(ao, self.rough)


def draw_stroke(layers, points, widths, color, normal_z=1.0):
    """A twig as a chain of capsules; widths per point (px)."""
    for (ax, ay), (bx, by), wa, wb in zip(points[:-1], points[1:], widths[:-1], widths[1:]):
        w = max(wa, wb)
        x0, x1 = int(min(ax, bx) - w - 2), int(max(ax, bx) + w + 3)
        y0, y1 = int(min(ay, by) - w - 2), int(max(ay, by) + w + 3)
        px = np.arange(x0, x1, dtype=np.float32)[None, :]
        py = np.arange(y0, y1, dtype=np.float32)[:, None]
        dx, dy = bx - ax, by - ay
        t = np.clip(((px - ax) * dx + (py - ay) * dy) / max(dx * dx + dy * dy, 1e-6), 0, 1)
        d = np.sqrt((px - ax - t * dx) ** 2 + (py - ay - t * dy) ** 2)
        width = wa + (wb - wa) * t
        cover = np.clip(width - d + 0.5, 0, 1)
        across = np.clip(d / np.maximum(width, 0.5), 0, 1)
        shade = 1.05 - 0.35 * across
        rgb = np.array(color, dtype=np.float32)[None, None, :] * shade[:, :, None]
        # Round twig: normal turns sideways towards the edges.
        length = math.hypot(dx, dy) or 1.0
        nx, ny = -dy / length, dx / length
        side = np.sign((px - ax) * nx + (py - ay) * ny) * across
        normal = np.dstack((side * nx, side * ny, np.sqrt(np.clip(1 - side ** 2, 0.05, 1)) * normal_z))
        layers.paint(y0, x0, cover, rgb, normal, ao=np.full(cover.shape, 0.8, np.float32))


LEAF_SHAPES = {
    # length/width ratio, widest point along the blade, tip sharpness, teeth (count, depth), base shape
    'birch': (1.45, 0.30, 1.9, (13, 0.06), 'truncate'),
    'apple': (1.85, 0.45, 1.4, (24, 0.025), 'round'),
    'hazel': (1.2, 0.55, 1.6, (18, 0.05), 'heart'),
    'raspberry': (1.55, 0.38, 1.7, (16, 0.07), 'round'),
    'pear': (1.45, 0.45, 1.5, (20, 0.015), 'round'),          # glossy, finely toothed, rounder than apple
    'small': (1.9, 0.5, 1.3, (10, 0.02), 'round'),            # sloe / blueberry: small elliptic leaves
}


def leaf_detail_patch():
    """Luminance detail from the inside of a CC0 leaf photo (veins, blotches), mean 1."""
    path = os.path.join(CC0, 'ambientcg', 'LeafSet010', 'LeafSet010_2K-JPG_Color.jpg')
    try:
        photo = load_image(path)[:, :, :3]
    except Exception:
        return None
    patch = photo[270:640, 330:700]
    lum = patch.mean(axis=2)
    return (lum / max(lum.mean(), 1e-6)).astype(np.float32)


def draw_leaf(layers, attach, angle, petiole, blade, shape, color, rng, detail, under=False, stalk_color=None):
    """One leaf: petiole from attach along angle, then the blade. Per-leaf tilt, midrib fold, vein relief."""
    ratio, widest, tip, (teeth, depth), base = shape
    ca, sa = math.cos(angle), math.sin(angle)
    stalk_end = (attach[0] + ca * petiole, attach[1] + sa * petiole)
    if petiole > 1:
        draw_stroke(layers, [attach, stalk_end], [max(blade * 0.012, 1.0), max(blade * 0.009, 0.8)],
                    stalk_color or tuple(np.array(color) * 0.8))
    length, width = blade, blade / ratio
    cx, cy = stalk_end[0] + ca * length * 0.5, stalk_end[1] + sa * length * 0.5
    reach = length * 0.56 + 3
    x0, x1 = int(cx - reach), int(cx + reach) + 1
    y0, y1 = int(cy - reach), int(cy + reach) + 1
    px = np.arange(x0, x1, dtype=np.float32)[None, :]
    py = np.arange(y0, y1, dtype=np.float32)[:, None]
    u = ((px - cx) * ca + (py - cy) * sa) / length + 0.5          # 0 at the blade base, 1 at the tip
    v = (-(px - cx) * sa + (py - cy) * ca) / (width * 0.5)        # -1..1 across
    t = np.clip(u, 0, 1)
    p_base = {'round': 0.5, 'wedge': 1.1, 'truncate': 0.3, 'heart': 0.4}[base]
    lower = np.clip(1.0 - ((widest - t) / widest) ** 2, 0, 1) ** p_base
    s_up = np.clip((t - widest) / (1 - widest), 0, 1)
    upper = np.clip(1.0 - s_up ** 2, 0, 1) ** (0.5 * tip) * (1.0 - 0.25 * s_up ** 3)
    profile = np.where(t < widest, lower, upper)
    if base == 'heart':
        profile = np.where(t < 0.1, np.maximum(profile, 0.7 * (t / 0.1) ** 0.4), profile)
    saw = (t * teeth) % 1.0
    saw2 = (t * teeth * 2.0) % 1.0
    profile = profile * (1.0 - depth * saw - depth * 0.45 * saw2 * (t > 0.08))
    inside = (u > 0) & (u < 1)
    edge_dist = (profile - np.abs(v)) * width * 0.5               # px inside the margin
    cover = np.clip(edge_dist + 0.5, 0, 1) * inside
    if not cover.any():
        return
    # Veins: midrib and pinnate side veins running towards the tip.
    mid = np.exp(-(v * width * 0.5 / max(width * 0.018, 0.8)) ** 2)
    phase = (u - 0.55 * np.abs(v) * width / length) * (teeth * 0.6)
    side = np.exp(-(((phase + 0.5) % 1.0 - 0.5) / 0.06) ** 2) * (np.abs(v) < profile * 0.9) * (u > 0.05)
    veins = np.clip(mid * 0.9 + side * 0.45, 0, 1)
    col = np.array(color, dtype=np.float32)[None, None, :]
    if detail is not None:
        dh, dw = detail.shape
        sx = rng.uniform(0.0, 0.4) * dw + (v * 0.5 + 0.5) * dw * 0.55
        sy = rng.uniform(0.0, 0.4) * dh + t * dh * 0.55
        d = bilinear(detail, sy, sx, wrap=True)
        col = col * (0.55 + 0.45 * d)[:, :, None]
    shade = 0.92 + 0.12 * (1 - np.abs(v)) - 0.07 * (v > 0) + 0.05 * (1 - t)
    col = col * shade[:, :, None]
    vein_col = np.array(color, dtype=np.float32) * (1.25 if not under else 1.1) + np.array([0.03, 0.03, 0.0])
    col = col * (1 - veins[:, :, None] * 0.45) + vein_col[None, None, :] * veins[:, :, None] * 0.45
    rim = np.clip(1.0 - edge_dist / 2.0, 0, 1) * cover
    col = col * (1 - 0.18 * rim[:, :, None])
    # Normal: random tilt of the whole leaf, the two halves folded along the midrib, slight curl, vein grooves.
    tilt = rng.uniform(0.0, 0.55)
    tilt_dir = rng.uniform(0, 2 * math.pi)
    fold = rng.uniform(0.15, 0.45)
    curl = rng.uniform(-0.35, 0.35)
    perp = (-sa, ca)
    sgn = np.sign(v) * np.sqrt(np.clip(np.abs(v), 0, 1))
    nx = tilt * math.cos(tilt_dir) + fold * sgn * perp[0] + curl * (t - 0.5) * ca - side * 0.08 * perp[0] * np.sign(v)
    ny = tilt * math.sin(tilt_dir) + fold * sgn * perp[1] + curl * (t - 0.5) * sa - side * 0.08 * perp[1] * np.sign(v)
    n = np.dstack((nx, ny, np.ones_like(nx)))
    n /= np.linalg.norm(n, axis=2, keepdims=True)
    rough = np.full(cover.shape, rng.uniform(0.55, 0.7) + (0.12 if under else 0.0), np.float32)
    ao = 0.85 + 0.15 * t
    layers.paint(y0, x0, cover, col, n, ao=ao.astype(np.float32), rough=rough)


def polyline(start, angle, length, steps, wander, rng, curve=0.0):
    points = [start]
    a = angle
    for _ in range(steps):
        a += rng.uniform(-wander, wander) + curve
        last = points[-1]
        points.append((last[0] + math.cos(a) * length / steps, last[1] + math.sin(a) * length / steps))
    return points


def point_along(points, t):
    seg_lengths = [math.hypot(b[0] - a[0], b[1] - a[1]) for a, b in zip(points[:-1], points[1:])]
    total = sum(seg_lengths) or 1.0
    target, walked = t * total, 0.0
    for (a, b), s in zip(zip(points[:-1], points[1:]), seg_lengths):
        if walked + s >= target or s == seg_lengths[-1]:
            f = min(max((target - walked) / max(s, 1e-6), 0.0), 1.0)
            return (a[0] + (b[0] - a[0]) * f, a[1] + (b[1] - a[1]) * f), math.atan2(b[1] - a[1], b[0] - a[0])
        walked += s
    a, b = points[-2], points[-1]
    return b, math.atan2(b[1] - a[1], b[0] - a[0])


LEAF_ATLASES = {
    # cells (columns, rows), physical card height (cm), blade length (cm), petiole (cm), colour, twig colour, layout
    'birch': {'cells': (4, 2), 'card_cm': 46.0, 'blade': (3.6, 5.6), 'petiole': (1.2, 2.0), 'tint': (0.20, 0.31, 0.095),
              'twig': (0.24, 0.15, 0.12), 'layout': 'hanging'},
    'apple': {'cells': (2, 2), 'card_cm': 40.0, 'blade': (5.5, 8.5), 'petiole': (1.5, 2.6), 'tint': (0.20, 0.31, 0.09),
              'twig': (0.30, 0.22, 0.16), 'layout': 'spur'},
    'hazel': {'cells': (2, 2), 'card_cm': 44.0, 'blade': (7.0, 10.5), 'petiole': (0.8, 1.4), 'tint': (0.23, 0.34, 0.10),
              'twig': (0.36, 0.26, 0.18), 'layout': 'zigzag'},
    'raspberry': {'cells': (2, 2), 'card_cm': 30.0, 'blade': (4.6, 6.6), 'petiole': (0.4, 0.8), 'tint': (0.17, 0.29, 0.09),
                  'twig': (0.30, 0.26, 0.16), 'layout': 'compound'},
    'pear': {'cells': (2, 2), 'card_cm': 40.0, 'blade': (4.5, 7.0), 'petiole': (2.2, 4.0), 'tint': (0.17, 0.30, 0.09),
             'twig': (0.28, 0.21, 0.16), 'layout': 'spur'},
    'small': {'cells': (2, 2), 'card_cm': 18.0, 'blade': (1.8, 3.2), 'petiole': (0.2, 0.4), 'tint': (0.18, 0.31, 0.10),
              'twig': (0.20, 0.16, 0.14), 'layout': 'zigzag'},
}


def leaf_cell_layout(kind, spec, anchor, cw, ch, ppc, rng):
    """Twigs (points, widths) and leaves (attach, angle, petiole px, blade px, underside) for one cell."""
    twigs, leaves = [], []
    up = -math.pi / 2
    blade = lambda: rng.uniform(*spec['blade']) * ppc
    petiole = lambda: rng.uniform(*spec['petiole']) * ppc
    if spec['layout'] == 'hanging':                 # birch: long thin twig with alternate leaves and short side shoots
        main = polyline(anchor, up + rng.uniform(-0.12, 0.12), ch * 0.9, 10, 0.06, rng)
        twigs.append((main, [ppc * 0.32, ppc * 0.12]))
        shoots = [main]
        for k, t in enumerate((0.22, 0.38, 0.55, 0.7)):
            p, a = point_along(main, t)
            sign = 1 if k % 2 else -1
            shoot = polyline(p, a + sign * rng.uniform(0.35, 0.65), ch * rng.uniform(0.16, 0.3), 4, 0.08, rng)
            twigs.append((shoot, [ppc * 0.16, ppc * 0.08]))
            shoots.append(shoot)
        for s_index, shoot in enumerate(shoots):
            n = 13 if s_index == 0 else 4
            start = 0.1 if s_index == 0 else 0.15
            for k in range(n):
                t = start + (1 - start) * (k + rng.uniform(0.2, 0.8)) / n
                p, a = point_along(shoot, t)
                sign = 1 if k % 2 else -1
                leaves.append((p, a + sign * rng.uniform(0.35, 0.9), petiole(), blade(), False))
            p, a = point_along(shoot, 1.0)
            leaves.append((p, a + rng.uniform(-0.15, 0.15), petiole() * 0.7, blade() * 0.85, False))
    elif spec['layout'] == 'spur':                  # apple: twig with spurs carrying rosettes of 3-5 leaves
        main = polyline(anchor, up + rng.uniform(-0.1, 0.1), ch * 0.7, 7, 0.04, rng)
        twigs.append((main, [ppc * 0.45, ppc * 0.2]))
        branches = [main]
        for k, t in enumerate((0.35, 0.6)):
            p, a = point_along(main, t)
            sign = 1 if (k + rng.randint(0, 1)) % 2 else -1
            br = polyline(p, a + sign * rng.uniform(0.4, 0.6), ch * rng.uniform(0.26, 0.34), 5, 0.05, rng)
            twigs.append((br, [ppc * 0.28, ppc * 0.14]))
            branches.append(br)
        for b_index, br in enumerate(branches):
            spurs = 4 if b_index == 0 else 3
            for k in range(spurs):
                t = 0.15 + 0.85 * (k + rng.uniform(0.3, 0.9)) / spurs
                p, a = point_along(br, min(t, 1.0))
                count = rng.randint(3, 5)
                spread = rng.uniform(1.6, 2.4)
                for j in range(count):
                    off = -spread / 2 + spread * (j + rng.uniform(0.3, 0.7)) / count
                    leaves.append((p, a + off, petiole(), blade() * rng.uniform(0.8, 1.05), False))
    elif spec['layout'] == 'zigzag':                # hazel: zig-zag twig, big alternate leaves
        main = [anchor]
        a = up + rng.uniform(-0.12, 0.12)
        nodes = 6
        for k in range(nodes):
            a_k = a + (0.16 if k % 2 else -0.16)
            last = main[-1]
            step = ch * 0.8 / nodes
            main.append((last[0] + math.cos(a_k) * step, last[1] + math.sin(a_k) * step))
        twigs.append((main, [ppc * 0.4, ppc * 0.16]))
        p, a_side = point_along(main, 0.35)
        side = polyline(p, a_side + rng.choice((-1, 1)) * rng.uniform(0.5, 0.8), ch * 0.35, 4, 0.08, rng)
        twigs.append((side, [ppc * 0.22, ppc * 0.12]))
        for k in range(1, len(main)):
            p = main[k]
            prev = main[k - 1]
            a_k = math.atan2(p[1] - prev[1], p[0] - prev[0])
            sign = 1 if k % 2 else -1
            leaves.append((p, a_k + sign * rng.uniform(0.55, 1.05), petiole(), blade() * (0.75 + 0.05 * k), False))
        for k in range(3):
            p, a_k = point_along(side, 0.3 + 0.35 * k)
            leaves.append((p, a_k + (1 if k % 2 else -1) * rng.uniform(0.5, 0.9), petiole(), blade() * 0.8, False))
    else:                                           # raspberry: compound leaves with three leaflets
        main = polyline(anchor, up + rng.uniform(-0.1, 0.1), ch * 0.55, 5, 0.06, rng)
        twigs.append((main, [ppc * 0.35, ppc * 0.25]))
        for k in range(4):
            p, a = point_along(main, 0.15 + 0.85 * k / 3)
            sign = 1 if k % 2 else -1
            rach_a = a + sign * rng.uniform(0.5, 0.9) if k < 3 else a
            rach_len = ppc * rng.uniform(3.0, 5.0)
            end = (p[0] + math.cos(rach_a) * rach_len, p[1] + math.sin(rach_a) * rach_len)
            twigs.append(([p, end], [ppc * 0.12, ppc * 0.08]))
            under = rng.random() < 0.2
            for off in (-1.15, 1.15):
                leaves.append((end, rach_a + off + rng.uniform(-0.15, 0.15), ppc * 0.3, blade() * 0.85, under))
            leaves.append((end, rach_a + rng.uniform(-0.1, 0.1), ppc * 0.8, blade() * 1.1, under))
    return twigs, leaves


def leaf_atlas(kind, rng):
    spec = LEAF_ATLASES[kind]
    cols, rows = spec['cells']
    cw, ch = TEX // cols, TEX // rows
    tint = np.array(spec['tint'], dtype=np.float32)
    layers = Layers(TEX, tint * 0.85)
    detail = leaf_detail_patch()
    shape = LEAF_SHAPES[kind]
    for cy in range(rows):
        for cx in range(cols):
            ox, oy = cx * cw, cy * ch
            ppc = ch / spec['card_cm']
            anchor = (ox + cw * 0.5, oy + ch - 6.0)
            cell_rng = random.Random(rng.random())
            twigs, leaves = leaf_cell_layout(kind, spec, anchor, cw, ch, ppc, cell_rng)
            cell_tone = cell_rng.uniform(0.88, 1.1)
            for points, (w0, w1) in twigs:
                widths = [w0 + (w1 - w0) * k / max(len(points) - 1, 1) for k in range(len(points))]
                draw_stroke(layers, points, widths, spec['twig'])
            cell_rng.shuffle(leaves)
            for attach, angle, pet, blade, under in leaves:
                tone = cell_tone * cell_rng.uniform(0.78, 1.18)
                hue = cell_rng.uniform(-0.03, 0.04)
                color = tint * tone + np.array([hue, hue * 0.5, -hue * 0.3], dtype=np.float32)
                if cell_rng.random() < 0.06:                          # an older, yellower leaf here and there
                    color = color * 0.7 + np.array([0.36, 0.38, 0.10], dtype=np.float32) * 0.3
                if under:
                    color = color * 0.45 + np.array([0.46, 0.50, 0.40], dtype=np.float32) * 0.55
                draw_leaf(layers, attach, angle, pet, blade, shape, tuple(np.clip(color, 0.02, 1.0)), cell_rng,
                          detail, under=under, stalk_color=tuple(tint * 0.75 + np.array([0.05, 0.02, 0.0])))
    return layers.finish(cluster_ao=0.35)


def leaf_cells(kind):
    """UV rectangles (u0, v0, u1, v1) and physical size (width, height cm) of the atlas cells."""
    spec = LEAF_ATLASES[kind]
    cols, rows = spec['cells']
    cells = []
    for cy in range(rows):
        for cx in range(cols):
            u0, u1 = cx / cols, (cx + 1) / cols
            v1, v0 = 1.0 - cy / rows, 1.0 - (cy + 1) / rows
            pad_u, pad_v = 0.004, 0.002
            cells.append(((u0 + pad_u, v0 + pad_v, u1 - pad_u, v1 - pad_v), spec['card_cm']))
    return cells


# --- Pine needles from the Poly Haven pine_tree_01 twig atlas -----------------------------------------------------

def flood_from_border(mask):
    """Pixels of mask connected to the border (4-neighbourhood), by repeated dilation."""
    reach = np.zeros_like(mask)
    reach[0, :], reach[-1, :], reach[:, 0], reach[:, -1] = mask[0, :], mask[-1, :], mask[:, 0], mask[:, -1]
    while True:
        grown = reach.copy()
        grown[1:] |= reach[:-1]; grown[:-1] |= reach[1:]
        grown[:, 1:] |= reach[:, :-1]; grown[:, :-1] |= reach[:, 1:]
        grown &= mask
        if (grown == reach).all():
            return reach
        reach = grown


def pine_sprites():
    """The two needle shoots of the Poly Haven twig atlas: colour, alpha, normal, AO, roughness, base and tip."""
    diff = load_image(PINE_TWIG + 'diff_2k.jpg')[:, :, :3]
    alpha = load_image(PINE_TWIG + 'alpha_2k.jpg')[:, :, 0]
    normal = decode_normal(load_image(PINE_TWIG + 'nor_dx_2k.jpg'))
    arm = load_image(PINE_TWIG + 'arm_2k.jpg')
    sprites = []
    for (x0, x1, y0, y1), base_side in (((40, 480, 60, 940), 'bottom'), ((1300, 1960, 640, 1095), 'left')):
        a = alpha[y0:y1, x0:x1].copy()
        foreign = flood_from_border(a > 0.25)
        a[foreign] = 0.0
        # Remove soft fringes that belonged to foreign regions.
        a[box_blur(foreign.astype(np.float32), 2, wrap=False) > 0.05] = 0.0
        ys, xs = np.nonzero(a > 0.5)
        if base_side == 'bottom':
            row = ys.max()
            base = (float(xs[ys == row].mean()), float(row))
        else:
            col = xs.min()
            base = (float(col), float(ys[xs == col].mean()))
        d2 = (xs - base[0]) ** 2 + (ys - base[1]) ** 2
        k = int(np.argmax(d2))
        tip = (float(xs[k]), float(ys[k]))
        sprites.append({'rgb': diff[y0:y1, x0:x1], 'a': a, 'n': normal[y0:y1, x0:x1], 'ao': arm[y0:y1, x0:x1, 0],
                        'rough': arm[y0:y1, x0:x1, 1], 'base': base, 'tip': tip})
    return sprites


def stamp_sprite(layers, sprite, base, angle, length, mirror=False, tone=1.0):
    """Paint a sprite so that its base lands on base and its base->tip axis points along angle with length px."""
    sb, st = sprite['base'], sprite['tip']
    s_angle = math.atan2(st[1] - sb[1], st[0] - sb[0])
    s_len = math.hypot(st[0] - sb[0], st[1] - sb[1])
    scale = length / s_len
    h, w = sprite['a'].shape
    reach = max(h, w) * scale * 1.05
    x0, x1 = int(base[0] - reach), int(base[0] + reach) + 1
    y0, y1 = int(base[1] - reach), int(base[1] + reach) + 1
    px = np.arange(x0, x1, dtype=np.float32)[None, :] - base[0]
    py = np.arange(y0, y1, dtype=np.float32)[:, None] - base[1]
    rot = angle - s_angle
    # Target -> sprite local axes (along, across), optional mirror across the axis.
    ca, sa = math.cos(angle), math.sin(angle)
    along = (px * ca + py * sa) / scale
    across = (-px * sa + py * ca) / scale
    if mirror:
        across = -across
    cs, ss = math.cos(s_angle), math.sin(s_angle)
    sx = sb[0] + along * cs - across * ss
    sy = sb[1] + along * ss + across * cs
    inside = (sx >= 0) & (sx <= w - 1) & (sy >= 0) & (sy <= h - 1)
    sxc, syc = np.clip(sx, 0, w - 1), np.clip(sy, 0, h - 1)
    cover = bilinear(sprite['a'], syc, sxc, wrap=False) * inside
    if not cover.any():
        return
    rgb = bilinear(sprite['rgb'], syc, sxc, wrap=False) * tone
    n = bilinear(sprite['n'], syc, sxc, wrap=False)
    # Rotate (and mirror) the normal's xy with the sprite.
    nx, ny = n[:, :, 0], n[:, :, 1]
    if mirror:   # reflect across the sprite axis in sprite space
        c2, s2 = math.cos(2 * s_angle), math.sin(2 * s_angle)
        nx, ny = nx * c2 + ny * s2, nx * s2 - ny * c2
    cr, sr = math.cos(rot), math.sin(rot)
    n = np.dstack((nx * cr - ny * sr, nx * sr + ny * cr, n[:, :, 2]))
    layers.paint(y0, x0, cover.astype(np.float32), rgb, n, ao=bilinear(sprite['ao'], syc, sxc, wrap=False),
                 rough=bilinear(sprite['rough'], syc, sxc, wrap=False))


NEEDLE_CELLS = [   # (angles in degrees from straight up, relative lengths) per 1024 cell, 2 x 2
    ((-52, -24, 2, 27, 50), (0.62, 0.78, 0.92, 0.75, 0.6)),     # dense branch-end tuft
    ((-40, -12, 14, 42), (0.72, 0.88, 0.85, 0.7)),               # tuft
    ((-17, 16), (0.86, 0.8)),                                    # pair of shoots
    ((-28, 4, 32), (0.7, 0.92, 0.66)),                            # three shoots
]
NEEDLE_CARD_CM = 46.0


def needle_atlas(rng):
    sprites = pine_sprites()
    fill = np.zeros(3, dtype=np.float32)
    total = 0.0
    for s in sprites:
        m = s['a'] > 0.6
        fill += s['rgb'][m].sum(axis=0)
        total += m.sum()
    fill /= max(total, 1)
    layers = Layers(TEX, fill)
    cell = TEX // 2
    for index, (angles, lengths) in enumerate(NEEDLE_CELLS):
        cx, cy = index % 2, index // 2
        base = (cx * cell + cell * 0.5, cy * cell + cell - 10.0)
        order = list(range(len(angles)))
        rng.shuffle(order)
        for k in order:
            sprite = sprites[rng.randrange(2)]
            angle = -math.pi / 2 + math.radians(angles[k] + rng.uniform(-5, 5))
            stamp_sprite(layers, sprite, base, angle, cell * lengths[k] * rng.uniform(0.84, 0.9),
                         mirror=rng.random() < 0.5, tone=rng.uniform(0.85, 1.1))
    rgb_alpha, normal, orm = layers.finish(cluster_ao=0.3, alpha_gamma=0.75)
    # Slightly deeper, bluer green than the source scan (closer to Scots pine in the reference light).
    c = rgb_alpha[:, :, :3]
    lum = c.mean(axis=2, keepdims=True)
    c = lum + (c - lum) * 1.25
    c = c * np.array([0.88, 0.95, 0.92], dtype=np.float32)
    # Fewer brown dead needles than the scan (a healthy summer crown): pull orange pixels halfway to green.
    warm = np.clip((c[:, :, 0] - c[:, :, 1]) * 6.0, 0, 1)[:, :, None] * 0.55
    green = lum * np.array([0.78, 1.12, 0.80], dtype=np.float32)
    c = c * (1 - warm) + green * warm
    rgb_alpha[:, :, :3] = np.clip(c, 0, 1)
    return rgb_alpha, normal, orm


def needle_cells():
    cells = []
    for index in range(4):
        cx, cy = index % 2, index // 2
        u0, u1 = cx * 0.5, cx * 0.5 + 0.5
        v1, v0 = 1.0 - cy * 0.5, 0.5 - cy * 0.5
        cells.append(((u0 + 0.003, v0 + 0.003, u1 - 0.003, v1 - 0.003), NEEDLE_CARD_CM))
    return cells


# ----------------------------------------------------------------------------------------------------------------
# Branch structure
# ----------------------------------------------------------------------------------------------------------------

class Branch:
    def __init__(self, points, radii, level, sides, kind='branch'):
        self.points, self.radii, self.level, self.sides, self.kind = points, radii, level, sides, kind
        self.children = []
        self._cum = None

    def cumulative(self):
        if self._cum is None:
            cum = [0.0]
            for a, b in zip(self.points[:-1], self.points[1:]):
                cum.append(cum[-1] + (b - a).length)
            self._cum = cum
        return self._cum

    def length(self):
        return self.cumulative()[-1]

    def at(self, t):
        """Point, direction and radius at t (0..1 along the branch)."""
        cum = self.cumulative()
        target = min(max(t, 0.0), 1.0) * cum[-1]
        for index in range(len(self.points) - 1):
            if cum[index + 1] >= target or index == len(self.points) - 2:
                seg = cum[index + 1] - cum[index]
                f = 0.0 if seg < 1e-6 else min(max((target - cum[index]) / seg, 0.0), 1.0)
                a, b = self.points[index], self.points[index + 1]
                radius = self.radii[index] + (self.radii[index + 1] - self.radii[index]) * f
                return a.lerp(b, f), (b - a).normalized(), radius
        return self.points[-1], (self.points[-1] - self.points[-2]).normalized(), self.radii[-1]

    def direction(self):
        return (self.points[-1] - self.points[0]).normalized()


def perpendicular(direction):
    other = Vector((1, 0, 0)) if abs(direction.x) < 0.9 else Vector((0, 1, 0))
    return direction.cross(other).normalized()


def horizontal_side(direction):
    side = direction.cross(UP)
    return side.normalized() if side.length > 1e-4 else perpendicular(direction)


def grow_line(start, direction, length, segments, radius0, radius1, wander, bend, rng, taper=1.0):
    """Polyline with random wander (degrees) and bend (callable s -> up amount per step, or a number)."""
    points, radii = [start.copy()], [radius0]
    d = direction.normalized()
    step = length / segments
    for index in range(segments):
        s = (index + 0.5) / segments
        jitter = Vector((rng.gauss(0, 1), rng.gauss(0, 1), rng.gauss(0, 1))) * math.radians(wander) * 0.5
        d = (d + jitter).normalized()
        b = bend(s) if callable(bend) else bend
        d = (d + UP * b).normalized()
        points.append(points[-1] + d * step)
        f = ((index + 1) / segments) ** taper
        radii.append(radius0 + (radius1 - radius0) * f)
    return points, radii


def child_direction(parent_dir, azimuth_side, angle_deg):
    a = math.radians(angle_deg)
    return (parent_dir * math.cos(a) + azimuth_side * math.sin(a)).normalized()


def side_at_azimuth(direction, azimuth):
    """Unit vector perpendicular to direction at an azimuth measured from the horizontal side."""
    side = horizontal_side(direction)
    return Quaternion(direction, azimuth) @ side


def lerp(a, b, t):
    return a + (b - a) * t


def clamp01(x):
    return min(max(x, 0.0), 1.0)


def make_trunk(height, r0, r1, segments, lean_deg, wander_deg, rng, sides, taper=0.75, sweep=0.0):
    """Trunk polyline with denser rings near the ground (root flare) and an optional gentle sweep."""
    azimuth = rng.uniform(0, 2 * math.pi)
    lean = math.radians(rng.uniform(0.3, 1.0) * lean_deg)
    direction = Vector((math.cos(azimuth) * math.sin(lean), math.sin(azimuth) * math.sin(lean), math.cos(lean)))
    ts = [0.0]
    for z in (12.0, 30.0, 60.0):
        if z < height * 0.2:
            ts.append(z / height)
    first = ts[-1]
    for k in range(1, segments + 1):
        ts.append(first + (1 - first) * k / segments)
    sweep_dir = Vector((math.cos(azimuth + 1.7), math.sin(azimuth + 1.7), 0.0))
    phase = rng.uniform(0, math.pi)
    points, radii = [], []
    start = Vector((0, 0, -15.0))
    offset = Vector((0, 0, 0))
    previous_t = 0.0
    d = direction.copy()
    for t in ts:
        if t > 0:
            jitter = Vector((rng.gauss(0, 1), rng.gauss(0, 1), 0)) * math.radians(wander_deg) * 0.5
            d = (d + jitter * (t - previous_t) * segments / max(segments, 1)).normalized()
            offset = offset + d * (t - previous_t) * (height + 15.0)
        previous_t = t
        bow = sweep_dir * sweep * math.sin(math.pi * t * 1.3 + phase) * height * 0.01
        points.append(start + offset + bow)
        radii.append(r1 + (r0 - r1) * max(0.0, 1.0 - t) ** taper)
    return Branch(points, radii, 0, sides, kind='trunk')


# --- Scots pine --------------------------------------------------------------------------------------------------

def pine_profile(age):
    height = 2000.0 * age ** 1.4
    return {
        'height': height,
        'radius': max(24.0 * age ** 1.5, 1.1),
        'crown0': lerp(0.1, 0.58, clamp01((age - 0.4) / 0.6) ** 1.2),
        'mature': clamp01((age - 0.45) / 0.55),
    }


def grow_pine(age, rng):
    pr = pine_profile(age)
    height, radius, crown0, mature = pr['height'], pr['radius'], pr['crown0'], pr['mature']
    trunk_sides = 12 if age > 0.8 else (8 if age > 0.3 else 5)
    trunk = make_trunk(height, radius, max(0.25, radius * 0.06), 14 if age > 0.8 else (10 if age > 0.3 else 6),
                       2.5, 2.5 + 3.0 * mature, rng, trunk_sides, taper=0.7, sweep=0.6 * mature)
    trunk_len = trunk.length()
    # Whorls of main branches in the live crown.
    whorl_gap = lerp(lerp(16.0, 46.0, clamp01(age / 0.45)), 44.0, mature)
    crown_len = trunk_len * (0.985 - crown0)
    whorls = max(3, int(crown_len / whorl_gap))
    lmax = lerp(height * 0.36, 330.0, mature)
    for w in range(whorls):
        tc = (w + rng.uniform(-0.25, 0.25)) / whorls        # 0 bottom .. 1 top of the crown
        tc = clamp01(tc)
        t = crown0 + (0.985 - crown0) * tc
        count = rng.randint(2, 4) if mature > 0.5 else rng.randint(3, 5)
        if mature > 0.5 and rng.random() < 0.08:
            continue                                         # lost whorl
        base_az = rng.uniform(0, 2 * math.pi)
        for k in range(count):
            az = base_az + k * 2 * math.pi / count + rng.uniform(-0.5, 0.5)
            # Crown shape: young cone, old irregular umbrella (wide high up, rounded flat top).
            cone = 0.12 + 0.88 * (1 - tc)
            umbrella = (0.7 + 0.3 * math.sin(math.pi * min(tc, 0.6) / 1.2)) * (1 - max(0.0, tc - 0.55) / 0.45 * 0.78)
            shape = lerp(cone, umbrella, mature)
            length = max(lmax * shape * rng.uniform(0.7, 1.15), 12.0)
            angle = lerp(lerp(78, 58, tc), lerp(98, 52, tc ** 1.3), mature) + rng.uniform(-10, 10)
            point, direction, r_parent = trunk.at(t)
            side = horizontal_side(direction)
            side = Quaternion(direction, az) @ side
            branch_dir = child_direction(direction, side, angle)
            r0 = max(min(r_parent * 0.6, 0.035 * length + 0.4), 0.35)
            start = point + side * r_parent * 0.3
            droop = lerp(0.0, -0.05, mature)
            upturn = lerp(0.06, 0.2, mature)
            bend = (lambda s, d=droop, u=upturn: d + u * max(0.0, s - 0.45) / 0.55)
            segs = 7 if mature > 0.5 else (5 if age > 0.3 else 3)
            pts, rad = grow_line(start, branch_dir, length, segs, r0, max(r0 * 0.2, 0.15), 7, bend, rng)
            sides = 6 if r0 > 4 else (5 if r0 > 1.5 else (4 if r0 > 0.6 else 3))
            branch = Branch(pts, rad, 1, sides)
            trunk.children.append(branch)
            if age > 0.3:
                pine_side_branches(branch, mature, age, rng)
    # Dead stubs on the clear trunk below the live crown (mature trees), a few dry twigs on young ones.
    if age > 0.3:
        stubs = int(lerp(4, 14, mature))
        for k in range(stubs):
            t = rng.uniform(0.12, crown0 - 0.02) if mature > 0.3 else rng.uniform(0.02, crown0)
            if t <= 0.03:
                continue
            point, direction, r_parent = trunk.at(t)
            side = Quaternion(direction, rng.uniform(0, 2 * math.pi)) @ horizontal_side(direction)
            length = rng.uniform(8, 45) * lerp(0.4, 1.0, mature)
            r0 = rng.uniform(2.0, 4.0) * lerp(0.4, 1.0, mature)
            pts, rad = grow_line(point + side * r_parent * 0.5, child_direction(direction, side, rng.uniform(75, 110)),
                                 length, 2, r0, r0 * rng.uniform(0.3, 0.7), 10, -0.03, rng)
            trunk.children.append(Branch(pts, rad, 1, 4, kind='stub'))
    return [trunk], pr


def pine_side_branches(branch, mature, age, rng):
    """Second order branches: flat sprays to the left and right of a main branch."""
    length = branch.length()
    spacing = lerp(24.0, 22.0, mature)
    n = int(length * 0.78 / spacing)
    sign = rng.choice((-1, 1))
    for k in range(n):
        t = 0.18 + 0.77 * (k + rng.uniform(0.2, 0.8)) / max(n, 1)
        point, direction, r_parent = branch.at(t)
        sign = -sign
        side = horizontal_side(direction) * sign
        child = (direction * math.cos(math.radians(rng.uniform(38, 62))) + side * math.sin(math.radians(rng.uniform(38, 62)))
                 + UP * rng.uniform(0.0, 0.3)).normalized()
        sub_len = min(lerp(55.0, 140.0, mature), (1 - t) * length * 0.62 + 18) * rng.uniform(0.7, 1.1)
        r0 = max(min(r_parent * 0.6, 0.03 * sub_len + 0.25), 0.2)
        pts, rad = grow_line(point, child, sub_len, 3, r0, max(r0 * 0.3, 0.12), 9, 0.07, rng)
        branch.children.append(Branch(pts, rad, 2, 3))


# --- Broadleaf trees and shrubs ----------------------------------------------------------------------------------

BROADLEAF = {
    'birch': {
        'height': 1500.0, 'h_exp': 1.25, 'radius': 19.0, 'crown0': (0.2, 0.36), 'lean': 4.0, 'wander': 3.0, 'sweep': 1.0,
        'trunk_top': 0.97,
        'l1': {'count': (6, 22), 'angle': (32, 52), 'lmax': 0.26, 'shape': 'ovate', 'bend': (0.05, -0.12), 'wander': 8,
               'segments': 7},
        'l2': {'spacing': (30, 46), 'from': 0.18, 'angle': (40, 68), 'frac': 0.42, 'add': 25, 'max': 170, 'bend': -0.12,
               'segments': 4, 'wander': 10},
        'l3': {'spacing': (20, 26), 'from': 0.3, 'angle': (50, 80), 'len': (45, 95), 'bend': -0.42, 'segments': 3,
               'wander': 8, 'radius': (0.32, 0.5)},
    },
    'apple': {
        'height': 520.0, 'h_exp': 1.0, 'radius': 15.0, 'crown0': (0.3, 0.3), 'lean': 9.0, 'wander': 10.0, 'sweep': 2.0,
        'trunk_top': 0.72,
        'l1': {'count': (4, 11), 'angle': (38, 62), 'lmax': 0.55, 'shape': 'spherical', 'bend': (0.03, -0.05), 'wander': 16,
               'segments': 6},
        'l2': {'spacing': (18, 24), 'from': 0.15, 'angle': (40, 72), 'frac': 0.55, 'add': 25, 'max': 150, 'bend': -0.03,
               'segments': 4, 'wander': 16},
        'l3': {'spacing': (12, 16), 'from': 0.15, 'angle': (40, 75), 'len': (22, 50), 'bend': 0.04, 'segments': 2,
               'wander': 14, 'radius': (0.35, 0.6)},
    },
}

BROADLEAF['pear'] = {
    # Wild pear: taller and narrower than the apple, upright branches, pointed crown.
    'height': 720.0, 'h_exp': 1.0, 'radius': 16.0, 'crown0': (0.32, 0.32), 'lean': 6.0, 'wander': 8.0, 'sweep': 1.5,
    'trunk_top': 0.8,
    'l1': {'count': (5, 12), 'angle': (24, 44), 'lmax': 0.42, 'shape': 'ovate', 'bend': (0.05, -0.02), 'wander': 12,
           'segments': 6},
    'l2': {'spacing': (18, 24), 'from': 0.15, 'angle': (35, 62), 'frac': 0.5, 'add': 22, 'max': 130, 'bend': -0.02,
           'segments': 4, 'wander': 14},
    'l3': {'spacing': (12, 16), 'from': 0.15, 'angle': (40, 75), 'len': (20, 45), 'bend': 0.03, 'segments': 2,
           'wander': 14, 'radius': (0.35, 0.6)},
}

SHRUBS = {
    'hazel': {'height': 360.0, 'stems': (9, 13), 'radius': (1.8, 3.0), 'spread': (6, 30), 'base': 22.0, 'bend': -0.035,
              'wander': 7, 'segments': 7,
              'l1': {'spacing': (20, 26), 'from': 0.25, 'angle': (35, 62), 'frac': 0.5, 'add': 30, 'max': 130,
                     'bend': -0.03, 'segments': 3, 'wander': 10}},
    'raspberry': {'height': 130.0, 'stems': (14, 19), 'radius': (0.55, 0.8), 'spread': (4, 26), 'base': 22.0,
                  'bend': lambda s: -0.02 - 0.12 * s, 'wander': 6, 'segments': 6,
                  'l1': {'spacing': (16, 22), 'from': 0.45, 'angle': (40, 70), 'frac': 0.25, 'add': 10, 'max': 32,
                         'bend': 0.03, 'segments': 2, 'wander': 10}},
    # Bramble: long canes arching over and down to the ground.
    'blackberry': {'height': 150.0, 'stems': (10, 14), 'radius': (0.7, 1.0), 'spread': (20, 55), 'base': 30.0,
                   'bend': lambda s: -0.05 - 0.22 * s, 'wander': 7, 'segments': 7,
                   'l1': {'spacing': (16, 22), 'from': 0.35, 'angle': (40, 70), 'frac': 0.25, 'add': 12, 'max': 36,
                          'bend': 0.02, 'segments': 2, 'wander': 10}},
    # Blueberry: knee-high, many thin green twigs.
    'blueberry': {'height': 40.0, 'stems': (22, 30), 'radius': (0.18, 0.3), 'spread': (5, 40), 'base': 28.0, 'bend': 0.0,
                  'wander': 10, 'segments': 4,
                  'l1': {'spacing': (5, 7), 'from': 0.3, 'angle': (35, 65), 'frac': 0.4, 'add': 6, 'max': 12,
                         'bend': 0.02, 'segments': 2, 'wander': 12}},
    # Wild strawberry: a low rosette of leaf stalks and runners.
    'strawberry': {'height': 16.0, 'stems': (10, 14), 'radius': (0.12, 0.18), 'spread': (40, 70), 'base': 10.0,
                   'bend': lambda s: 0.02 - 0.1 * s, 'wander': 8, 'segments': 3,
                   'l1': {'spacing': (30, 40), 'from': 0.9, 'angle': (20, 40), 'frac': 0.2, 'add': 3, 'max': 5,
                          'bend': 0.0, 'segments': 1, 'wander': 5}},
    # Dog rose: arching shoots, hips at the tips.
    'rose': {'height': 210.0, 'stems': (8, 12), 'radius': (0.8, 1.3), 'spread': (12, 42), 'base': 26.0,
             'bend': lambda s: -0.02 - 0.12 * s, 'wander': 8, 'segments': 7,
             'l1': {'spacing': (14, 20), 'from': 0.3, 'angle': (40, 70), 'frac': 0.3, 'add': 14, 'max': 40,
                    'bend': 0.02, 'segments': 2, 'wander': 12}},
    # Sloe (blackthorn): dense, dark, many short side twigs.
    'sloe': {'height': 240.0, 'stems': (12, 18), 'radius': (1.0, 1.7), 'spread': (5, 28), 'base': 30.0, 'bend': -0.02,
             'wander': 12, 'segments': 7,
             'l1': {'spacing': (9, 13), 'from': 0.2, 'angle': (50, 80), 'frac': 0.25, 'add': 12, 'max': 34,
                    'bend': 0.0, 'segments': 2, 'wander': 18}},
    # Elder: a few thick, arching stems, big pinnate leaves, flat umbels of black berries.
    'elder': {'height': 380.0, 'stems': (6, 9), 'radius': (2.0, 3.2), 'spread': (8, 32), 'base': 26.0,
              'bend': lambda s: -0.01 - 0.06 * s, 'wander': 6, 'segments': 7,
              'l1': {'spacing': (22, 30), 'from': 0.35, 'angle': (35, 60), 'frac': 0.4, 'add': 30, 'max': 110,
                     'bend': -0.02, 'segments': 3, 'wander': 10}},
}


def crown_shape(shape, t):
    if shape == 'spherical':
        return 0.3 + 0.7 * math.sin(math.pi * clamp01(t))
    if shape == 'ovate':     # widest a third up the crown, narrow pointed top
        return 0.35 + 0.65 * (math.sin(math.pi * min(t / 0.7, 1.0) * 0.5 + 0.35) if t < 0.35 else (1 - t) / 0.65 * 0.95 + 0.05)
    return 1.0


def spaced_positions(length, spacing, t_from, t_to, rng):
    n = max(1, int(length * (t_to - t_from) / spacing))
    return [t_from + (t_to - t_from) * (k + rng.uniform(0.15, 0.85)) / n for k in range(n)]


def grow_level(parent, spec, level, rng, age_scale=1.0, phyllo='spiral'):
    length = parent.length()
    spacing = rng.uniform(*spec['spacing']) * (0.75 + 0.25 * age_scale)
    az = rng.uniform(0, 2 * math.pi)
    for t in spaced_positions(length, spacing, spec['from'], 0.97, rng):
        point, direction, r_parent = parent.at(t)
        az += math.radians(137.5) + rng.uniform(-0.5, 0.5) if phyllo == 'spiral' else math.pi + rng.uniform(-0.6, 0.6)
        side = Quaternion(direction, az) @ horizontal_side(direction)
        angle = rng.uniform(*spec['angle'])
        if 'len' in spec:
            sub_len = rng.uniform(*spec['len']) * (0.55 + 0.45 * age_scale)
        else:
            sub_len = min(spec['max'] * (0.5 + 0.5 * age_scale), (1 - t) * length * spec['frac'] + spec['add'] * age_scale)
            sub_len *= rng.uniform(0.75, 1.15)
        if 'radius' in spec:
            r0 = rng.uniform(*spec['radius'])
        else:
            r0 = max(min(r_parent * 0.62, 0.035 * sub_len + 0.3), 0.25)
        bend = spec['bend']
        pts, rad = grow_line(point + side * r_parent * 0.3, child_direction(direction, side, angle), max(sub_len, 6.0),
                             spec['segments'], r0, max(r0 * 0.3, 0.12), spec['wander'], bend, rng)
        sides = 5 if r0 > 3 else (4 if r0 > 0.9 else 3)
        child = Branch(pts, rad, level, sides)
        parent.children.append(child)


def grow_broadleaf(key, age, rng):
    sp = BROADLEAF[key]
    height = sp['height'] * age ** sp['h_exp']
    radius = max(sp['radius'] * age ** 1.5, 0.9)
    crown0 = lerp(sp['crown0'][0], sp['crown0'][1], age)
    sides = 10 if age > 0.8 else (7 if age > 0.3 else 5)
    trunk = make_trunk(height * sp['trunk_top'], radius, max(0.3, radius * 0.05), 12 if age > 0.8 else (8 if age > 0.3 else 5),
                       sp['lean'], sp['wander'], rng, sides, taper=0.85, sweep=sp['sweep'])
    l1 = sp['l1']
    count = int(lerp(l1['count'][0], l1['count'][1], age))
    golden = math.radians(137.5)
    az = rng.uniform(0, 2 * math.pi)
    for k in range(count):
        tc = (k + rng.uniform(0.1, 0.9)) / count
        t = crown0 + (0.97 - crown0) * tc
        point, direction, r_parent = trunk.at(t)
        az += golden + rng.uniform(-0.4, 0.4)
        side = Quaternion(direction, az) @ horizontal_side(direction)
        length = max(height * l1['lmax'] * crown_shape(l1['shape'], tc) * rng.uniform(0.75, 1.15), 15.0)
        angle = rng.uniform(*l1['angle']) + (8 if key == 'apple' and tc < 0.3 else 0)
        b0, b1 = l1['bend']
        r0 = max(min(r_parent * 0.62, 0.03 * length + 0.5), 0.35)
        pts, rad = grow_line(point + side * r_parent * 0.3, child_direction(direction, side, angle), length,
                             l1['segments'] if age > 0.3 else 3, r0, max(r0 * 0.2, 0.15), l1['wander'],
                             (lambda s, a=b0, b=b1: a + (b - a) * s), rng)
        branch = Branch(pts, rad, 1, 6 if r0 > 4 else (5 if r0 > 2 else 4))
        trunk.children.append(branch)
        if age > 0.3 or key != 'birch':
            grow_level(branch, sp['l2'], 2, rng, age_scale=age)
            if age > 0.4:
                for twig in branch.children:
                    grow_level(twig, sp['l3'], 3, rng, age_scale=age, phyllo='alternate')
        elif age > 0.1:
            grow_level(branch, dict(sp['l2'], spacing=(14, 20), max=40, add=10), 2, rng, age_scale=age)
    if key == 'birch' and age > 0.3:
        # Hanging twigs straight from the upper trunk as well.
        for t in spaced_positions(trunk.length(), 60, 0.75, 0.97, rng):
            point, direction, r_parent = trunk.at(t)
            side = Quaternion(direction, rng.uniform(0, 6.28)) @ horizontal_side(direction)
            pts, rad = grow_line(point, child_direction(direction, side, 70), rng.uniform(40, 80) * age, 3,
                                 0.4, 0.15, 8, -0.4, rng)
            trunk.children.append(Branch(pts, rad, 3, 3))
    return [trunk], {'height': height, 'radius': radius, 'crown0': crown0, 'mature': clamp01((age - 0.45) / 0.55)}


def grow_shrub(key, age, rng):
    sp = SHRUBS[key]
    stems = []
    count = rng.randint(*sp['stems'])
    for index in range(count):
        azimuth = index / count * 2 * math.pi + rng.uniform(-0.35, 0.35)
        tilt = math.radians(rng.uniform(*sp['spread']))
        direction = Vector((math.cos(azimuth) * math.sin(tilt), math.sin(azimuth) * math.sin(tilt), math.cos(tilt)))
        start = Vector((math.cos(azimuth), math.sin(azimuth), 0)) * rng.uniform(0.2, 1.0) * sp['base'] + Vector((0, 0, -4))
        length = sp['height'] * rng.uniform(0.7, 1.08) / max(direction.z, 0.45)
        r0 = rng.uniform(*sp['radius'])
        pts, rad = grow_line(start, direction, length, sp['segments'], r0, max(r0 * 0.18, 0.12), sp['wander'], sp['bend'], rng)
        stem = Branch(pts, rad, 0, 5 if r0 > 1.5 else 4, kind='stem')
        grow_level(stem, sp['l1'], 1, rng, age_scale=age, phyllo='alternate')
        stems.append(stem)
    return stems, {'height': sp['height'], 'radius': sp['radius'][1], 'crown0': 0.0, 'mature': 1.0}


def all_branches(stems):
    out, todo = [], list(stems)
    while todo:
        branch = todo.pop()
        out.append(branch)
        todo += branch.children
    return out


# ----------------------------------------------------------------------------------------------------------------
# Mesh building
# ----------------------------------------------------------------------------------------------------------------

class MeshData:
    def __init__(self):
        self.verts, self.faces, self.uvs, self.mats, self.normals = [], [], [], [], []

    def add_face(self, indices, uvs, material):
        self.faces.append(indices)
        self.uvs.append(uvs)
        self.mats.append(material)

    def tris(self):
        return sum(len(f) - 2 for f in self.faces)


def add_tube(md, points, radii, material, cols=FULL_COLUMN, sides=6, wraps=1, v_scale=100.0, v_span=None,
             v_offset=0.0, flare=None, cap_material=None):
    """Tube along points. UV: u around (wraps times across the column cols), v tiles every v_scale cm or spans v_span."""
    n = len(points)
    if n < 2:
        return
    wraps = max(1, min(wraps, sides))
    while sides % wraps:
        wraps -= 1
    spw = sides // wraps
    dirs = []
    for i in range(n):
        d = points[min(i + 1, n - 1)] - points[max(i - 1, 0)]
        dirs.append(d.normalized() if d.length > 1e-6 else UP.copy())
    dist = [0.0]
    for i in range(1, n):
        dist.append(dist[-1] + (points[i] - points[i - 1]).length)
    total = max(dist[-1], 1e-6)
    side = perpendicular(dirs[0])
    u0, u1 = cols
    rings = []
    for i in range(n):
        if i > 0:
            side = dirs[i - 1].rotation_difference(dirs[i]) @ side
            side = (side - dirs[i] * side.dot(dirs[i])).normalized()
        v = (v_span[0] + (v_span[1] - v_span[0]) * dist[i] / total) if v_span else v_offset + dist[i] / v_scale
        ring = []
        for w in range(wraps):
            for j in range(spw + 1):
                angle = 2 * math.pi * (w * spw + j) / sides
                offset = Quaternion(dirs[i], angle) @ side
                r = radii[i] * (flare(points[i], angle) if flare else 1.0)
                md.verts.append(points[i] + offset * r)
                md.normals.append(offset)
                ring.append((len(md.verts) - 1, u0 + (u1 - u0) * j / spw))
        rings.append((ring, v))
    for (ra, va), (rb, vb) in zip(rings[:-1], rings[1:]):
        for w in range(wraps):
            base = w * (spw + 1)
            for j in range(spw):
                a0, ua0 = ra[base + j]
                a1, ua1 = ra[base + j + 1]
                b0, _ = rb[base + j]
                b1, _ = rb[base + j + 1]
                md.add_face([a0, a1, b1, b0], [(ua0, va), (ua1, va), (ua1, vb), (ua0, vb)], material)
    if cap_material is not None:
        ring, _ = rings[-1]
        center = points[-1]
        md.verts.append(center.copy())
        md.normals.append(dirs[-1])
        c = len(md.verts) - 1
        verts = [index for index, _ in ring]
        unique = []
        for index in verts:
            if not unique or (md.verts[index] - md.verts[unique[-1]]).length > 1e-4:
                unique.append(index)
        if (md.verts[unique[0]] - md.verts[unique[-1]]).length < 1e-4:
            unique.pop()
        for k in range(len(unique)):
            a, b = unique[k], unique[(k + 1) % len(unique)]
            pa = md.verts[a] - center
            pb = md.verts[b] - center
            scale = max(radii[-1], 1e-3) * 2.1
            md.add_face([a, b, c], [(0.5 + pa.x / scale, 0.5 + pa.y / scale), (0.5 + pb.x / scale, 0.5 + pb.y / scale),
                                    (0.5, 0.5)], cap_material)


def split_branch(branch, ts):
    """Pieces (points, radii) of a branch between the fractions in ts (ascending, inside 0..1)."""
    cum = branch.cumulative()
    total = cum[-1]
    pieces = []
    bounds = [0.0] + [t for t in ts if 0.0 < t < 1.0] + [1.0]
    for a, b in zip(bounds[:-1], bounds[1:]):
        points, radii = [], []
        pa, _, ra = branch.at(a)
        points.append(pa); radii.append(ra)
        for p, r, c in zip(branch.points, branch.radii, cum):
            if a * total + 1e-3 < c < b * total - 1e-3:
                points.append(p.copy()); radii.append(r)
        pb, _, rb = branch.at(b)
        points.append(pb); radii.append(rb)
        pieces.append((points, radii, a, b))
    return pieces


def root_flare(amount, lobes, phase):
    def f(point, angle):
        z = max(point.z, 0.0)
        return 1.0 + amount * math.exp(-z / 32.0) * (0.55 + 0.45 * math.cos(lobes * angle + phase))
    return f


class Crown:
    """Ellipsoid around the foliage; cards get normals bent towards its surface normal (soft crown shading)."""

    def __init__(self, points):
        arr = np.array([tuple(p) for p in points]) if points else np.zeros((1, 3))
        lo, hi = arr.min(axis=0), arr.max(axis=0)
        self.center = Vector(((lo + hi) * 0.5).tolist())
        half = np.maximum((hi - lo) * 0.5, 20.0)
        self.radii = Vector(half.tolist())

    def outward(self, p):
        q = p - self.center
        g = Vector((q.x / self.radii.x ** 2, q.y / self.radii.y ** 2, q.z / self.radii.z ** 2))
        return g.normalized() if g.length > 1e-9 else UP.copy()


def add_card(md, base, axis, facing, length, width, uv, material, crown, fold=0.16, droop=0.0, mix=0.6):
    """A leaf or needle card folded along its middle (4 triangles); front face turned out of the crown."""
    axis = axis.normalized()
    side = axis.cross(facing)
    if side.length < 1e-4:
        side = perpendicular(axis)
    side.normalize()
    normal = side.cross(axis).normalized()
    out = crown.outward(base + axis * length * 0.5)
    if normal.dot(out) < 0:
        side, normal = -side, -normal
    u0, v0, u1, v1 = uv
    um = (u0 + u1) * 0.5
    half = side * width * 0.5
    lift = normal * fold * width
    tip = base + axis * length + DOWN * (droop * length)
    mid = base + axis * length * 0.5 + DOWN * (droop * length * 0.3)
    corners = [(base - half, (u0, v0)), (base + lift * 0.4, (um, v0)), (base + half, (u1, v0)),
               (tip - half, (u0, v1)), (tip + lift, (um, v1)), (tip + half, (u1, v1))]
    first = len(md.verts)
    for position, _ in corners:
        md.verts.append(position)
        n = normal * (1 - mix) + crown.outward(position) * mix
        md.normals.append(n.normalized() if n.length > 1e-6 else normal)
    uvs = [uv_ for _, uv_ in corners]
    bl, bc, br, tl, tc, tr = range(first, first + 6)
    md.add_face([bl, bc, tc, tl], [uvs[0], uvs[1], uvs[4], uvs[3]], material)
    md.add_face([bc, br, tr, tc], [uvs[1], uvs[2], uvs[5], uvs[4]], material)
    del mid


def add_sphere(md, center, radius, uv_origin, material, squash=0.9, rings=4, segs=6):
    first = len(md.verts)
    for i in range(rings + 1):
        phi = math.pi * i / rings
        for j in range(segs):
            theta = 2 * math.pi * j / segs
            offset = Vector((math.sin(phi) * math.cos(theta), math.sin(phi) * math.sin(theta), math.cos(phi) * squash))
            md.verts.append(center + offset * radius)
            md.normals.append(offset.normalized())
    u0, v0 = uv_origin
    for i in range(rings):
        for j in range(segs):
            a = first + i * segs + j
            b = first + i * segs + (j + 1) % segs
            c = first + (i + 1) * segs + (j + 1) % segs
            d = first + (i + 1) * segs + j
            k = 1.0 / FRUIT_CELLS
            uv = [(u0 + k * (0.1 + 0.8 * j / segs), v0 + k * (0.1 + 0.8 * i / rings)),
                  (u0 + k * (0.1 + 0.8 * (j + 1) / segs), v0 + k * (0.1 + 0.8 * i / rings)),
                  (u0 + k * (0.1 + 0.8 * (j + 1) / segs), v0 + k * (0.1 + 0.8 * (i + 1) / rings)),
                  (u0 + k * (0.1 + 0.8 * j / segs), v0 + k * (0.1 + 0.8 * (i + 1) / rings))]
            md.add_face([a, d, c, b], uv, material)


# --- Wood ---------------------------------------------------------------------------------------------------------

def bark_zones(species, age):
    """Trunk height zones (fractions) for the bark columns: thick below, transition, thin above."""
    if species == 'pine':
        if age > 0.8:
            return 0.30, 0.52
        if age > 0.3:
            return 0.08, 0.3
        return 0.0, 0.0
    if species == 'birch':
        if age > 0.8:
            return 0.035, 0.17
        if age > 0.3:
            return 0.02, 0.1
        return 0.0, 0.0
    return None


def add_wood(md, stems, species, age, rng, BARK):
    zones = bark_zones(species, age)
    for stem in stems:
        if stem.kind == 'trunk' and zones is not None:
            flare = root_flare(0.42 if age > 0.8 else 0.2, rng.choice((4, 5, 6)), rng.uniform(0, 6.28)) if age > 0.3 else None
            z1, z2 = zones
            radius0 = stem.radii[0]
            for points, radii, a, b in split_branch(stem, [z1, z2]):
                if b - a < 1e-4:
                    continue
                mid_r = (radii[0] + radii[-1]) * 0.5
                if b <= z1 + 1e-6:
                    key, wraps, span = 'thick', 2 if radius0 > 12 else 1, None
                    v_scale = 150.0 if radius0 > 12 else 90.0
                elif b <= z2 + 1e-6 and z2 > z1:
                    key, wraps, span, v_scale = 'trans', 1, (0.004, 0.996), 1.0
                else:
                    key, wraps, span = 'thin', 1, None
                    v_scale = max(2 * math.pi * mid_r * 6.0, 90.0)
                add_tube(md, points, radii, BARK, column(key), stem.sides, wraps=wraps, v_scale=v_scale, v_span=span,
                         flare=flare if a == 0.0 else None)
        elif stem.kind == 'trunk' or stem.kind == 'stem':
            add_tube(md, stem.points, stem.radii, BARK, FULL_COLUMN, stem.sides,
                     wraps=max(1, round(2 * math.pi * stem.radii[0] / 60.0)), v_scale=60.0)
    for branch in all_branches(stems):
        if branch.level == 0:
            continue
        if zones is not None:
            key = 'thin' if (species == 'birch' and branch.level == 1 and branch.radii[0] > 2.5) else 'branch'
            v_scale = 60.0 if key == 'branch' else 120.0
            add_tube(md, branch.points, branch.radii, BARK, column(key), branch.sides, v_scale=v_scale,
                     v_offset=rng.random())
        else:
            add_tube(md, branch.points, branch.radii, BARK, FULL_COLUMN, branch.sides,
                     wraps=1, v_scale=40.0 if branch.radii[0] < 1 else 60.0, v_offset=rng.random())


# --- Foliage ------------------------------------------------------------------------------------------------------

def foliage_points_pine(stems):
    pts = []
    for b in all_branches(stems):
        if b.level >= 1 and b.kind != 'stub':
            pts += [b.points[-1], b.points[len(b.points) // 2]]
    return pts


def pine_foliage(md, stems, pr, rng, LEAF, cells):
    mature = pr['mature']
    age_scale = clamp01(pr['height'] / 2000.0) ** 0.35
    branches = [b for b in all_branches(stems) if b.kind != 'stub']
    crown = Crown(foliage_points_pine(stems) or [stems[0].points[-1]])
    shoot_cm = lerp(16.0, 40.0, age_scale)        # single shoot card length
    tuft_cm = lerp(22.0, 64.0, age_scale)
    spacing = lerp(5.0, 12.0, age_scale)
    deepest = max(b.level for b in branches)

    def shoot(point, axis, size, cell_choices, crossed=True, roll=None):
        uv, _ = cells[rng.choice(cell_choices)]
        roll = rng.uniform(0, math.pi) if roll is None else roll
        face = Quaternion(axis, roll) @ perpendicular(axis)
        add_card(md, point, axis, face, size, size, uv, LEAF, crown, fold=0.22, droop=0.05)
        if crossed:
            uv, _ = cells[rng.choice(cell_choices)]
            face = Quaternion(axis, roll + math.pi / 2 + rng.uniform(-0.3, 0.3)) @ perpendicular(axis)
            add_card(md, point, axis, face, size * rng.uniform(0.85, 1.0), size, uv, LEAF, crown, fold=0.22, droop=0.05)

    for b in branches:
        if b.level == 0:
            continue
        carries = b.level == deepest or b.level == 2 or not b.children
        length = b.length()
        if carries:
            t_from = 0.28 if b.level >= 2 else 0.55
            for t in spaced_positions(length, spacing, t_from, 0.92, rng):
                point, direction, _ = b.at(t)
                side = horizontal_side(direction) * rng.choice((-1, 1))
                axis = (direction * 0.8 + side * rng.uniform(0.2, 0.8) + UP * rng.uniform(0.25, 0.7)).normalized()
                shoot(point, axis, shoot_cm * rng.uniform(0.8, 1.15), (2, 3), crossed=rng.random() < 0.55)
        # Tuft at the tip of every shoot-bearing branch.
        if carries or b.level == 1:
            point, direction, _ = b.at(1.0)
            axis = (direction + UP * rng.uniform(0.3, 0.6)).normalized()
            base_roll = rng.uniform(0, math.pi)
            for k in range(2 if b.level >= 2 else 3):
                shoot(point - direction * tuft_cm * 0.25, axis, tuft_cm * rng.uniform(0.85, 1.1), (0, 1), crossed=False,
                      roll=base_roll + k * math.pi / (2 if b.level >= 2 else 3))
    # Leader at the top.
    top = stems[0]
    point, direction, _ = top.at(1.0)
    for k in range(3):
        shoot(point - direction * tuft_cm * 0.3, (direction + UP).normalized(), tuft_cm, (0, 1), crossed=False,
              roll=k * math.pi / 3)


def broadleaf_foliage(md, stems, species, pr, rng, LEAF, cells, density=1.0):
    branches = all_branches(stems)
    deepest = max(b.level for b in branches)
    crown_pts = [p for b in branches if b.level >= max(1, deepest - 1) for p in (b.points[-1], b.points[len(b.points) // 2])]
    crown = Crown(crown_pts or [stems[0].points[-1]])
    size_scale = {'strawberry': 0.45, 'rose': 0.8, 'elder': 1.35, 'blueberry': 0.9, 'sloe': 1.0}.get(species, 1.0)
    age_scale = clamp01(pr['height'] / {'birch': 1500.0, 'apple': 520.0}.get(species, pr['height'])) ** 0.3
    hosts = []
    for b in branches:
        if b.level == 0 and b.kind != 'stem':
            continue
        if species == 'birch':
            if b.level == deepest or (b.level >= 2 and not b.children) or (deepest <= 2 and b.level >= 1):
                hosts.append((b, 0.12, 8.0))
            elif b.level == 2:
                hosts.append((b, 0.35, 20.0))
        elif species in ('apple', 'pear'):
            if b.level == deepest:
                hosts.append((b, 0.1, 6.5))
            elif b.level == deepest - 1:
                hosts.append((b, 0.35, 11.0))
        elif species == 'hazel':
            if b.level == 1:
                hosts.append((b, 0.1, 8.0))
            elif b.kind == 'stem':
                hosts.append((b, 0.3, 9.0))
        else:   # raspberry
            if b.level == 1:
                hosts.append((b, 0.0, 6.0))
            elif b.kind == 'stem':
                hosts.append((b, 0.2, 6.5))
    for b, t_from, spacing in hosts:
        length = b.length()
        for t in spaced_positions(length, spacing / density, t_from, 1.0, rng):
            point, direction, _ = b.at(t)
            uv, card_cm = cells[rng.randrange(len(cells))]
            size = card_cm * size_scale * lerp(0.55, 1.0, age_scale) * rng.uniform(0.82, 1.15)
            spec = LEAF_ATLASES[leaf_kind(species)]
            aspect = (TEX / spec['cells'][0]) / (TEX / spec['cells'][1])
            width = size * aspect
            out = crown.outward(point)
            if species == 'birch':
                axis = (DOWN * 1.0 + out * rng.uniform(0.1, 0.35) + direction * 0.15
                        + Vector((rng.gauss(0, 0.15), rng.gauss(0, 0.15), 0))).normalized()
                facing = (out + Vector((rng.gauss(0, 0.6), rng.gauss(0, 0.6), 0))).normalized()
                add_card(md, point, axis, facing, size, width, uv, LEAF, crown, fold=0.12, droop=0.0, mix=0.55)
            else:
                side = horizontal_side(direction) * rng.choice((-1, 1))
                axis = (direction * 0.6 + side * rng.uniform(0.4, 1.0) + out * 0.4 + UP * rng.uniform(-0.1, 0.35)).normalized()
                facing = (UP * rng.uniform(0.6, 1.2) + out * 0.5 + side * rng.uniform(-0.4, 0.4)).normalized()
                add_card(md, point, axis, facing, size, width, uv, LEAF, crown, fold=0.14, droop=rng.uniform(0.0, 0.15),
                         mix=0.55)
        # A small tuft at the end of the host.
        point, direction, _ = b.at(1.0)
        uv, card_cm = cells[rng.randrange(len(cells))]
        size = card_cm * size_scale * lerp(0.55, 1.0, age_scale) * rng.uniform(0.85, 1.05)
        spec = LEAF_ATLASES[leaf_kind(species)]
        width = size * (TEX / spec['cells'][0]) / (TEX / spec['cells'][1])
        if species == 'birch':
            add_card(md, point, (DOWN + direction * 0.3).normalized(), crown.outward(point), size, width, uv, LEAF, crown,
                     fold=0.12)
        else:
            add_card(md, point - direction * size * 0.2, (direction + UP * 0.3).normalized(),
                     (UP + crown.outward(point)).normalized(), size, width, uv, LEAF, crown, fold=0.14)
    return crown


# Fruit per species: kind (colour field), count, radius cm, cluster (min, max), reach out of the crown (cm), squash.
FRUIT_SPECS = {
    'apple': ('apple', 95, 3.8, (1, 2), (14.0, 26.0), 0.92), 'hazel': ('hazelnut', 44, 2.6, (2, 3), (32.0, 46.0), 1.2),
    'raspberry': ('raspberry', 55, 1.0, (3, 3), (2.0, 5.0), 1.1), 'pear': ('pear', 80, 3.4, (1, 2), (14.0, 26.0), 1.35),
    'blackberry': ('blackberry', 60, 1.0, (3, 4), (2.0, 5.0), 1.15), 'blueberry': ('blueberry', 70, 0.55, (1, 3), (0.5, 2.0), 1.0),
    'strawberry': ('strawberry', 14, 0.8, (1, 2), (1.0, 3.0), 1.3), 'rose': ('rosehip', 45, 1.0, (1, 3), (3.0, 7.0), 1.45),
    'sloe': ('sloe', 90, 0.75, (1, 3), (2.0, 6.0), 1.0), 'elder': ('elderberry', 22, 0.35, (18, 26), (6.0, 12.0), 1.0),
}
SMALL_FRUIT = {'raspberry', 'blackberry', 'blueberry', 'strawberry', 'rosehip', 'sloe', 'elderberry'}


def add_fruit(md, stems, species, rng, FRUIT, crown, seed, variant='ripe'):
    """Fruit on the outside and lower half of the crown, hanging below/outside the leaf cards (readable from 10-20 m).
    variant: ripe, unripe (smaller and green) or bloom (many small white blossoms instead of fruit)."""
    spec = FRUIT_SPECS.get(species)
    if not spec:
        return
    kind, count, radius, cluster, reach, squash = spec
    if variant == 'unripe':
        kind, radius = 'unripe', radius * 0.6
    elif variant == 'bloom':
        kind, count, radius, cluster, squash = 'blossom', int(count * 1.1), max(radius * 0.45, 0.6), (3, 4), 0.35
    rng = random.Random(seed * 13 + 5 + (0 if variant == 'ripe' else 1000))
    branches = all_branches(stems)
    deepest = max(b.level for b in branches)
    hosts = [b for b in branches if b.level >= max(deepest - 1, 1)] or branches
    small = kind in SMALL_FRUIT or spec[0] in SMALL_FRUIT
    tree = species in ('apple', 'pear')
    placed = 0
    attempts = 0
    while placed < count and attempts < count * 40:
        attempts += 1
        host = rng.choice(hosts)
        point, direction, _ = host.at(rng.uniform(0.55, 1.0))
        rel = point - crown.center
        q = Vector((rel.x / crown.radii.x, rel.y / crown.radii.y, rel.z / crown.radii.z))
        if not small:
            # Outer shell of the crown, mostly its lower two thirds (seen from the side and from below).
            if q.length < (0.62 if tree else 0.5) or q.z > (0.55 if tree and variant != 'bloom' else 0.8):
                continue
        elif q.length < 0.5:
            continue
        if tree and point.z < 140:
            continue
        out = crown.outward(point)
        side = Vector((out.x, out.y, 0.0))
        side = side.normalized() if side.length > 1e-6 else out
        hang = point + side * rng.uniform(*reach) + DOWN * (radius * 2.2 + rng.uniform(0.0, 6.0) * (not small))
        if hang.z < radius * 1.2:
            continue
        for c in range(rng.randint(*cluster)):
            spread = 3.5 if kind == 'elderberry' else 1.5
            offset = Vector((rng.uniform(-1, 1), rng.uniform(-1, 1), rng.uniform(-0.8, 0.2) * (0.3 if kind == 'elderberry' else 1)))
            center = hang + offset * radius * spread * (c > 0)
            r = radius * rng.uniform(0.85, 1.15)
            if kind == 'hazelnut':
                add_sphere(md, center, r, FRUIT_UV['hazelnut'], FRUIT, squash=1.2, rings=3, segs=4)
                add_sphere(md, center + UP * r * 1.0, r * 0.9, FRUIT_UV['husk'], FRUIT, squash=0.6, rings=2, segs=4)
            elif kind == 'pear' or (kind == 'unripe' and species == 'pear'):
                # Pear: a round body with a narrower neck on top.
                add_sphere(md, center, r, FRUIT_UV[kind], FRUIT, squash=1.0, rings=4, segs=6)
                add_sphere(md, center + UP * r * 1.05, r * 0.62, FRUIT_UV[kind], FRUIT, squash=1.25, rings=3, segs=6)
            else:
                big = kind in ('apple', 'unripe', 'blossom') and tree
                tiny = kind in ('elderberry', 'blossom')
                add_sphere(md, center, r, FRUIT_UV[kind], FRUIT, squash=squash, rings=2 if tiny else (4 if big else 3),
                           segs=4 if (tiny or not big) else 6)
        placed += 1


# --- Field crops and fallen fruit (fruit atlas only) ---------------------------------------------------------------

def add_blade(md, base, direction, length, width, uv_rect, material, bend=0.0, segments=3, twist=0.0):
    """A flat leaf blade (double-sided ribbon) from base along direction, bending down by bend per segment."""
    side = perpendicular(direction)
    side = Quaternion(direction, twist) @ side
    points = [base]
    d = direction.normalized()
    for k in range(segments):
        d = (d + DOWN * bend).normalized()
        points.append(points[-1] + d * (length / segments))
    u0, v0, u1, v1 = uv_rect
    for k in range(segments):
        w0 = width * (1.0 - 0.85 * (k / segments) ** 1.5) * 0.5
        w1 = width * (1.0 - 0.85 * ((k + 1) / segments) ** 1.5) * 0.5
        a, b = points[k], points[k + 1]
        normal = side.cross(b - a).normalized()
        quad = [a - side * w0, a + side * w0, b + side * w1, b - side * w1]
        vs = (v0 + (v1 - v0) * k / segments, v0 + (v1 - v0) * (k + 1) / segments)
        uvs = [(u0, vs[0]), (u1, vs[0]), (u1, vs[1]), (u0, vs[1])]
        for sign, order in ((1, (0, 1, 2, 3)), (-1, (3, 2, 1, 0))):
            first = len(md.verts)
            for i in order:
                md.verts.append(quad[i])
                md.normals.append(normal * sign)
            md.add_face(list(range(first, first + 4)), [uvs[i] for i in order], material)
    return points


def add_rod(md, start, end, radius, uv_name, material, sides=4):
    """Thin round stem from start to end, coloured by a fruit atlas field."""
    direction = (end - start)
    if direction.length < 1e-4:
        return
    side = perpendicular(direction.normalized())
    first = len(md.verts)
    for point in (start, end):
        for k in range(sides):
            offset = Quaternion(direction.normalized(), 2 * math.pi * k / sides) @ side
            md.verts.append(point + offset * radius)
            md.normals.append(offset)
    u0, v0, u1, v1 = fruit_cell_rect(uv_name)
    for k in range(sides):
        a, b = first + k, first + (k + 1) % sides
        c, d = b + sides, a + sides
        md.add_face([a, b, c, d], [(u0, v0), (u1, v0), (u1, v1), (u0, v1)], material)


def build_crop(kind, age, rng, md, M):
    """Field crop at growth age 0..1 (sprout, young, ripe). Pivot at the ground, a little soil mound."""
    hulls = []
    ripe = age > 0.9
    leaf_uv = fruit_cell_rect('leaf_old' if (kind == 'crop_potato' and ripe) else 'leaf')
    if kind == 'crop_onion':
        height = 38.0 * age
        for k in range(rng.randint(5, 7) if age > 0.5 else 3):
            az = rng.uniform(0, 2 * math.pi)
            tilt = math.radians(rng.uniform(5, 22) + (25 if ripe else 0))
            direction = Vector((math.cos(az) * math.sin(tilt), math.sin(az) * math.sin(tilt), math.cos(tilt)))
            add_blade(md, Vector((0, 0, 1.0 + 2.0 * age)), direction, height * rng.uniform(0.75, 1.1), 1.4, leaf_uv, M,
                      bend=0.05 + (0.25 if ripe else 0.0), segments=3)
        if age > 0.5:
            r = 2.0 + 2.2 * age
            add_sphere(md, Vector((0, 0, r * 0.55)), r, FRUIT_UV['onion'], M, squash=0.85, rings=4, segs=7)
    elif kind == 'crop_carrot':
        height = 32.0 * age
        for k in range(rng.randint(6, 9) if age > 0.5 else 3):
            az = rng.uniform(0, 2 * math.pi)
            tilt = math.radians(rng.uniform(8, 35))
            direction = Vector((math.cos(az) * math.sin(tilt), math.sin(az) * math.sin(tilt), math.cos(tilt)))
            points = add_blade(md, Vector((0, 0, 1.0)), direction, height * rng.uniform(0.7, 1.0), 0.5, fruit_cell_rect('stem'), M,
                               bend=0.06, segments=3)
            # Feathery leaflets along the stalk.
            for t in (1, 2, 3):
                if t >= len(points):
                    continue
                for sign in (-1, 1):
                    leaflet = (perpendicular(direction) * sign + UP * 0.6).normalized()
                    add_blade(md, points[t], leaflet, 6.0 * age + 1.0, 2.2, leaf_uv, M, bend=0.1, segments=2)
        if age > 0.5:
            add_sphere(md, Vector((0, 0, 0.8)), 0.9 + 0.9 * age, FRUIT_UV['carrot'], M, squash=1.6, rings=3, segs=6)
    elif kind == 'crop_potato':
        height = 48.0 * age
        for k in range(rng.randint(4, 6) if age > 0.5 else 2):
            az = rng.uniform(0, 2 * math.pi)
            tilt = math.radians(rng.uniform(10, 35) + (20 if ripe else 0))
            direction = Vector((math.cos(az) * math.sin(tilt), math.sin(az) * math.sin(tilt), math.cos(tilt)))
            top = Vector((0, 0, 0)) + direction * height
            add_rod(md, Vector((0, 0, 0)), top, 0.5, 'stem', M)
            for t in (0.35, 0.55, 0.75, 1.0):
                base = direction * height * t
                for j in range(2):
                    leaflet = (Quaternion(UP, rng.uniform(0, 6.28)) @ Vector((1, 0, 0)) + UP * 0.3).normalized()
                    add_blade(md, base, leaflet, 9.0 * age + 2.0, 4.5 * age + 1.0, leaf_uv, M, bend=0.12, segments=2)
        if ripe:
            for k in range(3):
                az = rng.uniform(0, 2 * math.pi)
                add_sphere(md, Vector((math.cos(az) * 12, math.sin(az) * 12, 1.5)), 2.6, FRUIT_UV['potato'], M, squash=0.75,
                           rings=3, segs=6)
    else:  # flax
        height = 75.0 * age
        stems = rng.randint(14, 20) if age > 0.5 else 8
        stem_uv = 'straw' if ripe else 'stem'
        for k in range(stems):
            base = Vector((rng.uniform(-9, 9), rng.uniform(-9, 9), 0))
            top = base + Vector((rng.uniform(-6, 6), rng.uniform(-6, 6), height * rng.uniform(0.85, 1.05)))
            add_rod(md, base, top, 0.25, stem_uv, M, sides=3)
            if 0.5 < age <= 0.9:
                add_sphere(md, top, 1.1, FRUIT_UV['flax_flower'], M, squash=0.4, rings=2, segs=5)
            elif ripe:
                add_sphere(md, top, 0.6, FRUIT_UV['straw'], M, squash=1.1, rings=2, segs=4)
            if not ripe:
                for t in (0.35, 0.6):
                    point = base.lerp(top, t)
                    add_blade(md, point, (Vector((rng.uniform(-1, 1), rng.uniform(-1, 1), 0.8))).normalized(), 3.5, 0.6,
                              leaf_uv, M, bend=0.05, segments=1)
    # Soil mound around the base.
    add_sphere(md, Vector((0, 0, -3.0)), 9.0, FRUIT_UV['soil'], M, squash=0.4, rings=3, segs=8)
    pts = [Vector(v) for v in md.verts]
    hulls.append(pts)
    return hulls


def build_item(kind, rng, md, M):
    """Fallen fruit lying on the ground: fruit body plus a short stalk."""
    pear = kind.startswith('item_pear')
    field = {'item_apple': 'apple', 'item_apple_bruised': 'bruised', 'item_pear': 'pear', 'item_pear_bruised': 'bruised_pear'}[kind]
    r = 3.6 if pear else 3.8
    if pear:
        add_sphere(md, Vector((0, 0, r * 0.95)), r, FRUIT_UV[field], M, squash=0.95, rings=5, segs=8)
        add_sphere(md, Vector((r * 0.9, 0, r * 1.05)), r * 0.62, FRUIT_UV[field], M, squash=0.9, rings=4, segs=8)
        add_rod(md, Vector((r * 1.4, 0, r * 1.1)), Vector((r * 1.9, 0, r * 1.4)), 0.25, 'wood', M)
    else:
        add_sphere(md, Vector((0, 0, r * 0.9)), r, FRUIT_UV[field], M, squash=0.9, rings=6, segs=10)
        add_rod(md, Vector((0, 0, r * 1.7)), Vector((0.3, 0.2, r * 2.15)), 0.22, 'wood', M)
    return [[Vector(v) for v in md.verts]]


# --- Assembly -----------------------------------------------------------------------------------------------------

def build_object(name, md, material_names):
    mesh = bpy.data.meshes.new(name)
    mesh.from_pydata([tuple(v) for v in md.verts], [], md.faces)
    uv_layer = mesh.uv_layers.new(name='UVMap')
    for polygon, uvs in zip(mesh.polygons, md.uvs):
        for k in range(polygon.loop_total):
            uv_layer.data[polygon.loop_start + k].uv = uvs[k]
    for material_name in material_names:
        mesh.materials.append(bpy.data.materials.get(material_name) or bpy.data.materials.new(material_name))
    for polygon, material in zip(mesh.polygons, md.mats):
        polygon.material_index = material
    mesh.normals_split_custom_set_from_vertices([tuple(n) for n in md.normals])
    mesh.validate()
    obj = bpy.data.objects.new(name, mesh)
    bpy.context.scene.collection.objects.link(obj)
    return obj


def convex_hull_object(name, points):
    import bmesh
    bm = bmesh.new()
    for p in points:
        bm.verts.new(p)
    bmesh.ops.convex_hull(bm, input=bm.verts)
    mesh = bpy.data.meshes.new(name)
    bm.to_mesh(mesh)
    bm.free()
    obj = bpy.data.objects.new(name, mesh)
    bpy.context.scene.collection.objects.link(obj)
    return obj


def collision_points(branch, t0, t1, extra=0.0, base_flare=1.0):
    points = []
    for t in np.linspace(t0, t1, 6):
        point, direction, radius = branch.at(float(t))
        side = perpendicular(direction)
        r = radius * (base_flare if t < 0.02 else 1.0) + extra
        for k in range(8):
            points.append(point + (Quaternion(direction, 2 * math.pi * k / 8) @ side) * r)
    return points


def build_asset(asset_id, species, age, seed, state, name, mats):
    rng = random.Random(seed)
    if state in ('crop', 'item'):
        md = MeshData()
        material_names = [mats['fruit']]
        hulls = build_crop(species, age, rng, md, 0) if state == 'crop' else build_item(species, rng, md, 0)
        reset_scene_keep_materials()
        obj = build_object(name, md, material_names)
        tris = sum(len(poly.vertices) - 2 for poly in obj.data.polygons)
        collision = [convex_hull_object('UCX_%s_%02d' % (name, index), points) for index, points in enumerate(hulls[:1])]
        return obj, collision, tris, material_names
    if species == 'pine':
        stems, pr = grow_pine(age, rng)
    elif species in BROADLEAF:
        stems, pr = grow_broadleaf(species, age, rng)
    else:
        stems, pr = grow_shrub(species, age, rng)
    md = MeshData()
    BARK, LEAF, FRUIT, END = 0, 1, 2, 3
    material_names = [mats['bark'], mats['leaf'], mats['fruit'], mats['end']]
    hulls = []
    if state == 'stump':
        trunk = stems[0]
        cut = min(55.0 / max(trunk.length(), 1.0), 0.2)
        points, radii = [], []
        for t in np.linspace(0, cut, 5):
            point, _, radius = trunk.at(float(t))
            points.append(point)
            radii.append(radius)
        flare = root_flare(0.42, rng.choice((4, 5, 6)), rng.uniform(0, 6.28))
        add_tube(md, points, radii, BARK, column('thick'), trunk.sides, wraps=2 if radii[0] > 12 else 1, v_scale=150.0,
                 flare=flare, cap_material=END)
        hull = []
        for p, r in zip(points, radii):
            for k in range(10):
                a = 2 * math.pi * k / 10
                hull.append(p + Vector((math.cos(a), math.sin(a), 0)) * r * flare(p, a))
        hulls.append(hull)
    else:
        add_wood(md, stems, species, age, rng, BARK)
        wood_tris = md.tris()
        rng_leaf = random.Random(seed * 7 + 1)
        if species == 'pine':
            pine_foliage(md, stems, pr, rng_leaf, LEAF, needle_cells())
            crown = None
        else:
            crown = broadleaf_foliage(md, stems, species, pr, rng_leaf, LEAF, leaf_cells(leaf_kind(species)))
        if state in ('full', 'unripe', 'bloom') and crown is not None:
            add_fruit(md, stems, species, rng, FRUIT, crown, seed, variant={'full': 'ripe'}.get(state, state))
        print('TREES:   %s wood %d tris, foliage+fruit %d tris' % (name, wood_tris, md.tris() - wood_tris))
        if species in SHRUBS:
            hulls.append([p for b in all_branches(stems) for p in b.points])
        else:
            trunk = stems[0]
            top = clamp01(pr['crown0'] + 0.12) if species == 'pine' else clamp01(pr['crown0'] + 0.1)
            top = max(top, 0.35)
            hulls.append(collision_points(trunk, 0.0, top * 0.45, base_flare=1.25))
            hulls.append(collision_points(trunk, top * 0.45, top))

    reset_scene_keep_materials()
    obj = build_object(name, md, material_names)
    used = sorted({m for m in md.mats})
    remap = {old: new for new, old in enumerate(used)}
    names = [material_names[i] for i in used]
    obj.data.materials.clear()
    for material_name in names:
        obj.data.materials.append(bpy.data.materials.get(material_name))
    for polygon in obj.data.polygons:
        polygon.material_index = remap[md.mats[polygon.index]]
    tris = sum(len(p.vertices) - 2 for p in obj.data.polygons)
    collision = [convex_hull_object('UCX_%s_%02d' % (name, index), points) for index, points in enumerate(hulls[:6])]
    return obj, collision, tris, names


def reset_scene_keep_materials():
    for obj in list(bpy.data.objects):
        bpy.data.objects.remove(obj, do_unlink=True)


def export(obj, collision, folder):
    os.makedirs(folder, exist_ok=True)
    bpy.ops.object.select_all(action='DESELECT')
    for o in [obj] + collision:
        o.select_set(True)
    bpy.context.view_layer.objects.active = obj
    path = os.path.join(folder, obj.name + '.fbx')
    bpy.ops.export_scene.fbx(
        filepath=path, use_selection=True, object_types={'MESH'}, apply_unit_scale=True,
        apply_scale_options='FBX_SCALE_UNITS', axis_forward='-Y', axis_up='Z',
        mesh_smooth_type='OFF', add_leaf_bones=False, path_mode='STRIP')
    return path


def main():
    global ASSETS
    argv = sys.argv[sys.argv.index('--') + 1:] if '--' in sys.argv else []
    only = set()
    skip_textures = False
    for index, arg in enumerate(argv):
        if arg == '--only' and index + 1 < len(argv):
            only = {s.strip() for s in argv[index + 1].split(',') if s.strip()}
        if arg == '--assets' and index + 1 < len(argv):
            ASSETS = os.path.abspath(argv[index + 1])
        if arg == '--skip-textures':
            skip_textures = True
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
            manifest.update(json.load(handle))
    textures = {t['name']: t for t in manifest['textures']}
    materials = {m['name']: m for m in manifest['materials']}
    meshes = {m['id']: m for m in manifest['meshes']}

    wanted = [a for a in ASSETS_TO_BUILD if not only or a[0] in only]
    needed = set()
    for a in wanted:
        needed.update(SPECIES_MATERIALS[a[1]])
    needed.update({'Fruit', 'EndGrain'})
    rng = random.Random(4711)

    makers = {
        'BarkPine': (lambda: pine_bark_atlas(random.Random(101)), False),
        'BarkBirch': (lambda: birch_bark_atlas(random.Random(102)), False),
        'BarkBrown': (lambda: brown_bark(random.Random(103)), False),
        'NeedlesPine': (lambda: needle_atlas(random.Random(104)), True),
        'LeavesBirch': (lambda: leaf_atlas('birch', random.Random(105)), True),
        'LeavesApple': (lambda: leaf_atlas('apple', random.Random(106)), True),
        'LeavesHazel': (lambda: leaf_atlas('hazel', random.Random(107)), True),
        'LeavesRaspberry': (lambda: leaf_atlas('raspberry', random.Random(108)), True),
        'LeavesPear': (lambda: leaf_atlas('pear', random.Random(110)), True),
        'LeavesSmall': (lambda: leaf_atlas('small', random.Random(111)), True),
        'Fruit': (lambda: fruit_texture(), False),
        'EndGrain': (lambda: end_grain(512, random.Random(109)), False),
    }
    made = {}
    for key in sorted(needed):
        name = 'M_Nature_' + key
        maker, masked = makers[key]
        if not skip_textures or not os.path.exists(os.path.join(tex_folder, 'T_%s_BC.png' % key)):
            maps = maker()
            entries = [save_png('T_%s_BC' % key, maps[0], tex_folder, 'sRGB'),
                       save_png('T_%s_N' % key, maps[1], tex_folder, 'Non-Color'),
                       save_png('T_%s_ORM' % key, maps[2], tex_folder, 'Non-Color')]
            textures.update({e['name']: e for e in entries})
            print('TREES: material %s (%dx%d)' % (name, entries[0]['size'][0], entries[0]['size'][1]))
        materials[name] = {'name': name, 'BaseColor': 'T_%s_BC' % key, 'Normal': 'T_%s_N' % key, 'ORM': 'T_%s_ORM' % key,
                           'masked': masked}
        made[key] = name

    errors = 0
    for asset_id, species, age, seed, state, name, group, budget in wanted:
        bark_key, leaf_key = SPECIES_MATERIALS[species]
        mats = {'bark': made[bark_key], 'leaf': made[leaf_key], 'fruit': made['Fruit'], 'end': made['EndGrain']}
        try:
            obj, collision, tris, names = build_asset(asset_id, species, age, seed, state, name, mats)
            path = export(obj, collision, os.path.join(ASSETS, *group.split('/')))
            co = np.array([v.co[:] for v in obj.data.vertices])
            size = co.max(axis=0) - co.min(axis=0)
            print('TREES: %s %d tris (budget %d), %.0f x %.0f x %.0f cm, materials %s' % (name, tris, budget, *size, names))
            meshes[asset_id] = {'id': asset_id, 'unreal_name': name, 'file': os.path.relpath(path, ASSETS).replace('\\', '/'),
                                'group': group, 'tris': tris, 'tris_target': budget, 'collision': 'ucx',
                                'bbox_min_cm': [round(float(v), 1) for v in co.min(axis=0)],
                                'bbox_max_cm': [round(float(v), 1) for v in co.max(axis=0)],
                                'pivot': 'Stammfuss, Boden bei Z = 0' if state != 'item' else 'Unterseite, Boden bei Z = 0',
                                'materials': names,
                                'source': 'tools/blender/build_trees.py (%s, age %.2f, seed %d, %s)' % (species, age, seed, state)}
            if tris > budget:
                print('TREES WARNING: %s is above its triangle budget' % name)
        except Exception as error:
            import traceback
            traceback.print_exc()
            errors += 1
            print('TREES ERROR: %s: %s' % (asset_id, error))

    manifest['textures'] = sorted(textures.values(), key=lambda t: t['name'])
    manifest['materials'] = sorted(materials.values(), key=lambda m: m['name'])
    manifest['meshes'] = sorted(meshes.values(), key=lambda m: m['id'])
    with open(manifest_path, 'w', encoding='utf-8') as handle:
        json.dump(manifest, handle, indent=2, ensure_ascii=False)
    print('TREES: done, %d error(s)' % errors)
    if errors:
        sys.exit(1)


main()
