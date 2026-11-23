"""
Simple, matching models for work stations, farm buildings and tavern stations (2026-11-16, user: "So wie vorgeschlagen":
replace the placeholder shapes). Run headless from the repository root:
  E:\\Blender\\blender.exe -b --factory-startup -P tools/blender/build_stations.py -- [--only workbench,field] [--preview <folder>]
      [--no-textures]

Style: design/stil_norden_gezeitenhall.md - greyed wood, grass sod, traces of use (ash, chips, worn soil), clear silhouettes
from above, no pure colours. Built from simple parts (boxes, logs, stones, roofs) in real size; the pivot is the bottom
centre (ground at Z = 0), the outer size matches the building slot in GamerBuilding.cpp (PlaceholderFor).

Materials: the shared building surfaces of tools/blender/restyle_buildings.py (M_Bld_Planks, Wood, Stone, Thatch, Plaster,
Cloth; textures already in Assets/Buildings/StyleTest/Textures) plus four new ones made here from CC0 photo sets in
D:\\gamer\\cc0_quellen\\polyhaven (no new downloads): Soil (dirt_floor), Sod (forrest_ground_03, greener), Ash
(dirt_floor, dark grey; also used for soot-black iron) and Water (clay_plaster, dark blue-grey, flat). Parent material in
Unreal: M_Gamer_Building (moss, damp, weathering by height above the pivot - "traces of life" without baking).
UVs: box projection in real size (world cm per texture repeat per surface), so textures keep their density.
Collision: a few UCX boxes per model. Output: Assets/Buildings/Stations/SM_Station_<Name>.fbx and entries (meshes,
materials, textures) merged into Assets/manifest_buildings.json (read by tools/unreal/import_assets.py).
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
GROUP = 'Buildings/Stations'
TEX_REL = 'Buildings/StyleTest/Textures'     # shared with the restyled buildings
SOURCES = r'D:\gamer\cc0_quellen'

# World size of one texture repeat (cm) per surface; shared surfaces from restyle_buildings.py.
WORLD = {'Planks': 200.0, 'Wood': 200.0, 'Stone': 250.0, 'Thatch': 200.0, 'Plaster': 200.0, 'Cloth': 120.0,
         'Soil': 200.0, 'Sod': 200.0, 'Ash': 150.0, 'Water': 150.0}
# New surfaces made here: Poly Haven set, colour grading, moss/damp/weathering parameters of M_Gamer_Building.
NEW_SURFACES = {
    'Soil': {'set': 'dirt_floor', 'tex': 1024, 'sat': 0.8, 'gain': 0.9, 'tint': None,
             'params': {'MossAmount': 0.15, 'MossHeight': 10.0, 'UpMoss': 0.10, 'DampHeight': 20.0, 'Weathering': 0.0}},
    'Sod': {'set': 'forrest_ground_03', 'tex': 1024, 'sat': 0.9, 'gain': 0.95, 'tint': (0.82, 1.0, 0.72),
            'params': {'MossAmount': 0.6, 'MossHeight': 400.0, 'UpMoss': 0.6, 'DampHeight': 20.0, 'Weathering': 0.0}},
    'Ash': {'set': 'dirt_floor', 'tex': 1024, 'sat': 0.0, 'gain': 0.45, 'tint': None,
            'params': {'MossAmount': 0.0, 'MossHeight': 1.0, 'UpMoss': 0.0, 'DampHeight': 1.0, 'Weathering': 0.0}},
    'Water': {'set': 'clay_plaster', 'tex': 512, 'sat': 0.0, 'gain': 0.30, 'tint': (0.75, 0.88, 1.0), 'flat': True,
              'params': {'MossAmount': 0.0, 'MossHeight': 1.0, 'UpMoss': 0.0, 'DampHeight': 1.0, 'Weathering': 0.0}},
}
ROUGH_MIN = 0.55
SLOTS = ['Planks', 'Wood', 'Stone', 'Thatch', 'Plaster', 'Cloth', 'Soil', 'Sod', 'Ash', 'Water']


def log(message):
    print('STATIONS: ' + message)


# --- Textures --------------------------------------------------------------------------------------------------------

def load_pixels(path, size):
    image = bpy.data.images.load(path, check_existing=False)
    if image.size[0] != size:
        image.scale(size, size)
    w, h = image.size
    px = np.empty(w * h * 4, dtype=np.float32)
    image.pixels.foreach_get(px)
    bpy.data.images.remove(image)
    return px.reshape(h, w, 4)


def save_png(name, pixels, colorspace):
    folder = os.path.join(ASSETS, *TEX_REL.split('/'))
    os.makedirs(folder, exist_ok=True)
    h, w = pixels.shape[:2]
    image = bpy.data.images.new(name, w, h, alpha=True)
    image.colorspace_settings.name = colorspace
    image.pixels.foreach_set(np.ascontiguousarray(pixels, dtype=np.float32).ravel())
    image.filepath_raw = os.path.join(folder, name + '.png')
    image.file_format = 'PNG'
    image.save()
    bpy.data.images.remove(image)
    return {'name': name, 'file': '%s/%s.png' % (TEX_REL, name), 'size': [w, h], 'srgb': colorspace == 'sRGB',
            'folder': '/Game/Gamer/' + TEX_REL}


def build_new_textures():
    textures, materials = [], []
    for surface, spec in NEW_SURFACES.items():
        prefix = os.path.join(SOURCES, 'polyhaven', spec['set'], spec['set'])
        size = spec['tex']
        color = load_pixels(prefix + '_diff_2k.jpg', size)
        normal = load_pixels(prefix + '_nor_dx_2k.jpg', size)
        orm = load_pixels(prefix + '_arm_2k.jpg', size)
        rgb = color[:, :, :3]
        lum = (rgb[..., 0] * 0.2126 + rgb[..., 1] * 0.7152 + rgb[..., 2] * 0.0722)[..., None]
        rgb = np.clip((lum + (rgb - lum) * spec['sat']) * spec['gain'], 0, 1)
        if spec['tint']:
            rgb = np.clip(rgb * np.asarray(spec['tint']), 0, 1)
        color[:, :, :3] = rgb
        if spec.get('flat'):
            normal[:, :, :2] = 0.5 + (normal[:, :, :2] - 0.5) * 0.15
            normal[:, :, 2] = 1.0
        orm[:, :, 1] = ROUGH_MIN + (1.0 - ROUGH_MIN) * orm[:, :, 1]
        orm[:, :, 2] = 0.0
        for px in (color, normal, orm):
            px[:, :, 3] = 1.0
        base = 'Bld' + surface
        textures += [save_png('T_%s_BC' % base, color, 'sRGB'), save_png('T_%s_N' % base, normal, 'Non-Color'),
                     save_png('T_%s_ORM' % base, orm, 'Non-Color')]
        materials.append({'name': 'M_Bld_' + surface, 'BaseColor': 'T_%s_BC' % base, 'Normal': 'T_%s_N' % base,
                          'ORM': 'T_%s_ORM' % base, 'parent': 'building', 'params': spec['params']})
        log('textures %s (%s, %d px)' % (surface, spec['set'], size))
    return textures, materials


# --- Geometry --------------------------------------------------------------------------------------------------------

class Builder:
    """Faces with a surface name and a UV offset per part; box-projected UVs in real size at the end."""

    def __init__(self, seed):
        self.verts, self.faces, self.surfaces, self.offsets, self.smooth = [], [], [], [], []
        self.hulls = []
        self.rng = random.Random(seed)

    def add(self, verts, faces, surface, smooth=False):
        base = len(self.verts)
        self.verts += [Vector(v) for v in verts]
        offset = (self.rng.random(), self.rng.random())
        for f in faces:
            self.faces.append([base + i for i in f])
            self.surfaces.append(surface)
            self.offsets.append(offset)
            self.smooth.append(smooth)

    def box(self, center, size, surface, yaw=0.0, tilt=(0.0, 0.0), jitter=0.0, hull=False):
        """Box of size (x, y, z) around center, turned by yaw (deg) and tilted (pitch, roll deg)."""
        sx, sy, sz = (s * 0.5 for s in size)
        corners = [(-sx, -sy, -sz), (sx, -sy, -sz), (sx, sy, -sz), (-sx, sy, -sz),
                   (-sx, -sy, sz), (sx, -sy, sz), (sx, sy, sz), (-sx, sy, sz)]
        rot = (Matrix.Rotation(math.radians(yaw), 3, 'Z') @ Matrix.Rotation(math.radians(tilt[0]), 3, 'Y')
               @ Matrix.Rotation(math.radians(tilt[1]), 3, 'X'))
        verts = []
        for c in corners:
            v = Vector(c)
            if jitter:
                v += Vector((self.rng.uniform(-1, 1), self.rng.uniform(-1, 1), self.rng.uniform(-1, 1))) * jitter
            verts.append(Vector(center) + rot @ v)
        faces = [(0, 3, 2, 1), (4, 5, 6, 7), (0, 1, 5, 4), (1, 2, 6, 5), (2, 3, 7, 6), (3, 0, 4, 7)]
        self.add(verts, faces, surface)
        if hull:
            self.hulls.append(verts)
        return verts

    def stone(self, center, size, yaw=None):
        """Rough field stone: a jittered box, slightly sunk."""
        yaw = self.rng.uniform(0, 360) if yaw is None else yaw
        return self.box(center, size, 'Stone', yaw=yaw, tilt=(self.rng.uniform(-8, 8), self.rng.uniform(-8, 8)),
                        jitter=min(size) * 0.18)

    def log(self, p0, p1, radius, surface='Wood', sides=8, caps=True, taper=1.0):
        """Round log from p0 to p1 (smooth sides, flat end caps)."""
        p0, p1 = Vector(p0), Vector(p1)
        axis = (p1 - p0).normalized()
        side = axis.cross(Vector((0, 0, 1)))
        if side.length < 1e-3:
            side = axis.cross(Vector((1, 0, 0)))
        side.normalize()
        up = side.cross(axis)
        verts = []
        for point, r in ((p0, radius), (p1, radius * taper)):
            for k in range(sides):
                a = 2 * math.pi * k / sides
                verts.append(point + (side * math.cos(a) + up * math.sin(a)) * r)
        faces = [(k, (k + 1) % sides, sides + (k + 1) % sides, sides + k) for k in range(sides)]
        self.add(verts, faces, surface, smooth=True)
        if caps:
            self.add(verts[:sides], [tuple(reversed(range(sides)))], surface)
            self.add(verts[sides:], [tuple(range(sides))], surface)

    def disc(self, center, radius, height, surface, sides=12, squash=1.0):
        """Flat cylinder standing up (ash bed, water surface, tub)."""
        c = Vector(center)
        verts = []
        for z in (0.0, height):
            for k in range(sides):
                a = 2 * math.pi * k / sides
                verts.append(c + Vector((math.cos(a) * radius, math.sin(a) * radius * squash, z)))
        faces = [(k, (k + 1) % sides, sides + (k + 1) % sides, sides + k) for k in range(sides)]
        faces.append(tuple(range(sides, 2 * sides)))
        self.add(verts, faces, surface, smooth=True)

    def dome(self, center, radii, surface, rings=5, sides=12, open_front=False):
        """Half ellipsoid (heap, oven dome). open_front leaves a gap facing -Y (oven mouth)."""
        c = Vector(center)
        rings_v = []
        for i in range(rings + 1):
            phi = (math.pi / 2) * i / rings
            ring = []
            for k in range(sides):
                a = 2 * math.pi * k / sides
                ring.append(c + Vector((math.cos(a) * math.cos(phi) * radii[0], math.sin(a) * math.cos(phi) * radii[1],
                                        math.sin(phi) * radii[2])))
            rings_v.append(ring)
        verts = [v for ring in rings_v for v in ring]
        faces = []
        mouth = {int(sides * 0.75) % sides} if open_front else set()
        for i in range(rings):
            for k in range(sides):
                if i < 2 and k in mouth:
                    continue
                faces.append((i * sides + k, i * sides + (k + 1) % sides, (i + 1) * sides + (k + 1) % sides, (i + 1) * sides + k))
        self.add(verts, faces, surface, smooth=True)

    def gable_roof(self, center, length, width, eave_z, ridge_z, surface, thick=12.0):
        """Two roof slabs along X from the eaves (z eave_z, half width out) to the ridge, with thickness."""
        cx, cy, _ = center
        hl, hw = length * 0.5, width * 0.5
        for sign in (-1, 1):
            outer = [(cx - hl, cy + sign * hw, eave_z), (cx + hl, cy + sign * hw, eave_z),
                     (cx + hl, cy, ridge_z), (cx - hl, cy, ridge_z)]
            inner = [(x, y, z - thick) for x, y, z in outer]
            verts = outer + inner
            faces = [(0, 1, 2, 3), (7, 6, 5, 4), (0, 4, 5, 1), (1, 5, 6, 2), (3, 2, 6, 7), (0, 3, 7, 4)]
            if sign < 0:
                faces = [tuple(reversed(f)) for f in faces]
            self.add(verts, faces, surface)
        self.hulls.append([Vector((cx - hl, cy - hw, eave_z - thick)), Vector((cx + hl, cy - hw, eave_z - thick)),
                           Vector((cx - hl, cy + hw, eave_z - thick)), Vector((cx + hl, cy + hw, eave_z - thick)),
                           Vector((cx - hl, cy, ridge_z)), Vector((cx + hl, cy, ridge_z))])

    def gable_ends(self, center, length, width, wall_z, ridge_z, surface, thick=8.0):
        """Triangular gable walls closing both ends of a house between wall top and ridge."""
        cx, cy, _ = center
        hw = width * 0.5
        for x in (cx - length * 0.5, cx + length * 0.5 - thick):
            tri = [(x, cy - hw, wall_z), (x, cy + hw, wall_z), (x, cy, ridge_z)]
            back = [(px + thick, py, pz) for px, py, pz in tri]
            self.add(tri + back, [(0, 2, 1), (3, 4, 5), (0, 1, 4, 3), (1, 2, 5, 4), (2, 0, 3, 5)], surface)

    def house(self, length, width, wall_h, ridge_h, roof, wall='Planks', door_w=110.0, door_h=185.0, overhang=40.0):
        """Small house: stone socket, plank walls (door gap in the front, -Y), corner posts, gable roof along X."""
        hl, hw = length * 0.5, width * 0.5
        sock = 35.0
        self.box((0, 0, sock * 0.5), (length, width, sock), 'Stone', jitter=3.0, hull=True)
        t = 12.0
        z0, z1 = sock, sock + wall_h
        zc = (z0 + z1) * 0.5
        # Back and side walls.
        self.box((0, hw - t * 0.5, zc), (length, t, wall_h), wall, hull=True)
        self.box((-hl + t * 0.5, 0, zc), (t, width, wall_h), wall, hull=True)
        self.box((hl - t * 0.5, 0, zc), (t, width, wall_h), wall, hull=True)
        # Front wall in two parts beside the door, lintel above it, a dark door leaf set back.
        side_len = (length - door_w) * 0.5
        for sign in (-1, 1):
            self.box((sign * (door_w * 0.5 + side_len * 0.5), -hw + t * 0.5, zc), (side_len, t, wall_h), wall, hull=True)
        self.box((0, -hw + t * 0.5, z0 + door_h + (wall_h - door_h) * 0.5), (door_w, t, wall_h - door_h), wall)
        self.box((0, -hw + t * 1.2, z0 + door_h * 0.5), (door_w - 10, 4, door_h), 'Wood')
        for x in (-hl, hl):
            for y in (-hw, hw):
                self.box((x, y, zc), (22, 22, wall_h + 10), 'Wood')
        # Roof with overhang; gable ends in planks.
        self.gable_ends((0, 0, 0), length - 4, width - 4, z1, z1 + (ridge_h - z1) * 0.92, wall)
        self.gable_roof((0, 0, 0), length + overhang * 2, width + overhang * 2, z1 - 15, ridge_h, roof, thick=14)
        self.log((-hl - overhang, 0, ridge_h - 4), (hl + overhang, 0, ridge_h - 4), 9, 'Wood')

    def finish(self, name):
        mesh = bpy.data.meshes.new(name)
        mesh.from_pydata([tuple(v) for v in self.verts], [], self.faces)
        mesh.validate()
        used = [s for s in SLOTS if s in self.surfaces]
        for s in used:
            mesh.materials.append(bpy.data.materials.get('M_Bld_' + s) or bpy.data.materials.new('M_Bld_' + s))
        uv = mesh.uv_layers.new(name='UVMap')
        for poly in mesh.polygons:
            surface = self.surfaces[poly.index]
            poly.material_index = used.index(surface)
            poly.use_smooth = self.smooth[poly.index]
            n = poly.normal
            axis = max(range(3), key=lambda i: abs(n[i]))
            a, b = [(1, 2), (0, 2), (0, 1)][axis]
            world = WORLD[surface]
            ou, ov = self.offsets[poly.index]
            for li in poly.loop_indices:
                co = mesh.vertices[mesh.loops[li].vertex_index].co
                uv.data[li].uv = (co[a] / world + ou, co[b] / world + ov)
        obj = bpy.data.objects.new(name, mesh)
        bpy.context.scene.collection.objects.link(obj)
        hulls = []
        for index, points in enumerate(self.hulls[:8]):
            hulls.append(convex_hull('UCX_%s_%02d' % (name, index), points))
        return obj, hulls


def convex_hull(name, points):
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


def chips(b, center, spread, count, surface='Planks'):
    """Wood chips / straw on the ground: small thin flakes (traces of use)."""
    for _ in range(count):
        x = center[0] + b.rng.uniform(-spread, spread)
        y = center[1] + b.rng.uniform(-spread, spread)
        b.box((x, y, 0.6), (b.rng.uniform(5, 11), b.rng.uniform(2, 4), 1.2), surface, yaw=b.rng.uniform(0, 180))


def worn_ground(b, size, surface='Soil', height=1.5):
    """Trodden patch of soil under a station (reads from above)."""
    b.disc((0, 0, -0.5), size[0] * 0.5, height, surface, sides=10, squash=size[1] / size[0])


# --- Models (outer size = building slot, see GamerBuilding.cpp PlaceholderFor) ----------------------------------------

def m_workbench(b):     # 160 x 80 x 90
    worn_ground(b, (190, 120))
    for x in (-68, 68):
        for y in (-30, 30):
            b.log((x, y, 0), (x * 1.03, y * 1.05, 80), 5.5)
    b.log((-68, 0, 25), (68, 0, 25), 4)
    b.box((0, -18, 85), (160, 38, 9), 'Planks', hull=False)
    b.box((0, 21, 85), (156, 38, 9), 'Planks')
    b.hulls.append([Vector(p) for p in [(-80, -40, 0), (80, -40, 0), (-80, 40, 0), (80, 40, 0),
                                        (-80, -40, 90), (80, -40, 90), (-80, 40, 90), (80, 40, 90)]])
    b.box((58, -30, 96), (18, 14, 12), 'Wood')                     # bench hook / vise block
    b.log((-40, 5, 92), (10, 25, 92), 3, 'Wood')                     # a stick on the bench
    b.box((-20, 10, 91), (26, 10, 3), 'Planks', yaw=20)
    chips(b, (0, -55), 45, 14)


def m_campfire(b):      # 120 x 120 x 30
    b.disc((0, 0, -0.5), 42, 2.5, 'Ash', sides=12)
    for k in range(10):
        a = 2 * math.pi * k / 10 + b.rng.uniform(-0.1, 0.1)
        b.stone((math.cos(a) * 50, math.sin(a) * 50, 8), (b.rng.uniform(20, 28), b.rng.uniform(16, 22), 16))
    for k in range(4):
        a = 2 * math.pi * k / 4 + 0.4
        b.log((math.cos(a) * 32, math.sin(a) * 32, 2), (math.cos(a) * 4, math.sin(a) * 4, 28), 5, 'Wood')
    b.log((-20, -8, 4), (18, 12, 4), 5.5, 'Ash')                    # charred log in the ash
    b.hulls.append([Vector(p) for p in [(-60, -60, 0), (60, -60, 0), (-60, 60, 0), (60, 60, 0),
                                        (-60, -60, 20), (60, -60, 20), (-60, 60, 20), (60, 60, 20)]])


def m_bed(b):           # 200 x 100 x 35
    for y in (-46, 46):
        b.log((-100, y, 9), (100, y, 9), 9)
    for x in (-95, 95):
        b.log((x, -50, 9), (x, 50, 9), 8)
    b.box((0, 0, 14), (184, 86, 14), 'Thatch', jitter=2.0, hull=True)   # straw
    b.box((20, 4, 23), (120, 78, 4), 'Cloth', yaw=3, jitter=1.5)       # wool blanket
    b.box((-78, 0, 25), (30, 60, 9), 'Cloth', jitter=2.0)              # rolled pillow
    chips(b, (0, 0), 95, 12, 'Thatch')


def m_storage(b):       # 600 x 300 x 300
    b.house(560, 260, 175, 300, 'Sod', wall='Planks', door_w=140, door_h=170, overhang=20)
    for k, x in enumerate((-190, -150)):
        b.disc((x, -175, 0), 26, 70, 'Planks', sides=10)              # barrels by the door
        b.disc((x, -175, 69.5), 24, 1, 'Wood', sides=10)
    b.box((200, -175, 30), (110, 60, 60), 'Planks', yaw=4)           # crate
    worn_ground(b, (300, 120))


def m_woodcutter(b):    # 250 x 120 x 80
    worn_ground(b, (260, 140))
    b.disc((-80, -10, 0), 30, 52, 'Wood', sides=10)                  # chopping block
    b.log((-80, -10, 54), (-58, 10, 92), 2.0, 'Wood')                # axe handle stuck in it
    b.box((-82, -12, 56), (16, 4, 12), 'Ash', yaw=45)                # axe head (dark iron)
    for i, (y, z) in enumerate(((-25, 12), (0, 12), (25, 12), (-12, 34), (12, 34), (0, 56))):
        b.log((10, y, z), (120, y + b.rng.uniform(-3, 3), z), 12, 'Wood', sides=8)
    b.hulls.append([Vector(p) for p in [(-115, -60, 0), (125, -60, 0), (-115, 60, 0), (125, 60, 0),
                                        (-115, -60, 68), (125, -60, 68), (-115, 60, 68), (125, 60, 68)]])
    chips(b, (-80, -10), 60, 26)


def m_stonecutter(b):   # 200 x 120 x 80
    worn_ground(b, (220, 140))
    for x in (-40, 40):                                              # two trestles
        b.log((x, -35, 0), (x, 0, 55), 4)
        b.log((x, 35, 0), (x, 0, 55), 4)
        b.log((x - 10, 0, 55), (x + 10, 0, 55), 5)
    b.box((0, 0, 62 + 12), (100, 55, 36), 'Stone', jitter=2.5, hull=True)   # block being worked
    for k in range(6):                                               # finished blocks
        b.box((72 + (k % 2) * 30, -30 + (k // 2) * 26, 8 + (k // 4) * 15), (28, 22, 15), 'Stone', yaw=b.rng.uniform(-6, 6),
              jitter=1.5)
    b.log((-75, 30, 8), (-50, 45, 8), 2.5, 'Wood')                   # mallet
    b.box((-48, 46, 8), (16, 12, 12), 'Wood', yaw=30)
    chips(b, (0, -45), 70, 22, 'Stone')
    b.hulls.append([Vector(p) for p in [(-100, -60, 0), (100, -60, 0), (-100, 60, 0), (100, 60, 0),
                                        (-100, -60, 55), (100, -60, 55), (-100, 60, 55), (100, 60, 55)]])


def m_farmstead(b):     # 600 x 400 x 280
    b.house(500, 320, 150, 280, 'Thatch', wall='Planks', door_w=120, door_h=170, overhang=45)
    b.dome((215, -170, 0), (55, 45, 70), 'Thatch', rings=4, sides=10)     # haystack by the door
    for x in (-230, -170, -110):                                          # a short wattle fence
        b.log((x, -195, 0), (x, -195, 90), 4)
    b.log((-235, -195, 70), (-105, -195, 70), 3)
    b.log((-235, -195, 40), (-105, -195, 40), 3)
    worn_ground(b, (300, 160))


def m_field(b):         # 100 x 100 x 18: one tile; the game lays tiles over the drawn field
    rows = 4
    for r in range(rows):
        y0 = -50 + r * 25
        verts = [(-50, y0, 0), (50, y0, 0), (50, y0 + 12.5, 14), (-50, y0 + 12.5, 14), (-50, y0 + 25, 0), (50, y0 + 25, 0)]
        b.add(verts, [(0, 1, 2, 3), (3, 2, 5, 4)], 'Soil')
    b.hulls.append([Vector(p) for p in [(-50, -50, 0), (50, -50, 0), (-50, 50, 0), (50, 50, 0),
                                        (-50, -50, 14), (50, -50, 14), (-50, 50, 14), (50, 50, 14)]])


def m_composter(b):     # 120 x 120 x 90
    for x in (-55, 55):
        for y in (-55, 55):
            b.box((x, y, 45), (10, 10, 90), 'Wood')
    for k, z in enumerate((12, 32, 52, 72)):
        b.box((0, 55, z), (120, 4, 14), 'Planks')                    # back
        b.box((-55, 0, z), (4, 120, 14), 'Planks')
        b.box((55, 0, z), (4, 120, 14), 'Planks')
        if z < 40:
            b.box((0, -55, z), (120, 4, 14), 'Planks')               # low front: easy to shovel
    b.dome((0, 0, 0), (52, 52, 68), 'Soil', rings=4, sides=12)
    chips(b, (0, -75), 30, 8, 'Thatch')
    b.hulls.append([Vector(p) for p in [(-60, -60, 0), (60, -60, 0), (-60, 60, 0), (60, 60, 0),
                                        (-60, -60, 90), (60, -60, 90), (-60, 60, 90), (60, 60, 90)]])


def m_rain_catcher(b):  # 90 x 90 x 70: 1 log, 1 large stone, 3 small stones, 5 sticks (the recipe)
    b.stone((0, 0, 10), (85, 70, 22), yaw=10)                        # large stone
    b.log((-40, 0, 36), (40, 0, 36), 20, 'Wood', sides=10)           # hollowed log as trough
    b.disc((0, 0, 50), 34, 7.5, 'Water', sides=10, squash=0.45)      # water surface in it
    for x, y in ((-30, -24), (5, 25), (32, -22)):                    # wedge stones
        b.stone((x, y, 24), (16, 12, 12))
    for x, y in ((-38, -30), (-38, 30), (38, -30), (38, 30)):        # stick frame
        b.log((x, y, 18), (x * 0.6, y * 0.6, 70), 1.6, 'Wood', sides=5)
    b.log((-24, 0, 69), (24, 0, 69), 1.6, 'Wood', sides=5)
    b.box((0, 0, 66), (70, 60, 2), 'Planks', tilt=(0, 8))           # bark sheet funnelling rain
    b.hulls.append([Vector(p) for p in [(-45, -40, 0), (45, -40, 0), (-45, 40, 0), (45, 40, 0),
                                        (-45, -40, 58), (45, -40, 58), (-45, 40, 58), (45, 40, 58)]])


def m_tavern(b):        # 800 x 500 x 320
    b.house(740, 440, 175, 320, 'Thatch', wall='Planks', door_w=150, door_h=185, overhang=40)
    b.box((0, 0, 330), (60, 60, 12), 'Wood')                         # smoke hole cover
    b.box((-200, -270, 22), (180, 30, 6), 'Planks')                  # bench outside
    for x in (-270, -130):
        b.box((x, -270, 10), (8, 26, 20), 'Wood')
    b.disc((230, -265, 0), 28, 75, 'Planks', sides=10)               # ale barrel
    b.log((310, -255, 0), (310, -255, 230), 6)                       # sign post with a drinking horn shape
    b.box((340, -255, 215), (60, 4, 34), 'Planks')
    worn_ground(b, (420, 200))


def m_hearth(b):        # 140 x 100 x 60
    b.disc((0, 0, -0.5), 46, 2.5, 'Ash', sides=10, squash=0.7)
    for k in range(12):
        a = 2 * math.pi * k / 12
        b.stone((math.cos(a) * 60, math.sin(a) * 42, 9), (22, 18, 18))
    for x in (-45, 45):                                              # kettle stand
        b.log((x, 0, 0), (x * 0.95, 0, 60), 3)
    b.log((-48, 0, 58), (48, 0, 58), 2.5)
    b.disc((0, 0, 14), 24, 28, 'Ash', sides=10)                      # sooty iron kettle
    b.box((-40, 25, 18), (40, 30, 5), 'Stone')                       # baking stone
    b.hulls.append([Vector(p) for p in [(-70, -50, 0), (70, -50, 0), (-70, 50, 0), (70, 50, 0),
                                        (-70, -50, 30), (70, -50, 30), (-70, 50, 30), (70, 50, 30)]])


def m_clay_oven(b):     # 160 x 140 x 120
    b.box((0, 0, 15), (160, 140, 30), 'Stone', jitter=3.0, hull=True)
    b.dome((0, 5, 30), (70, 62, 88), 'Plaster', rings=6, sides=14, open_front=True)
    b.disc((0, -48, 30), 20, 1, 'Ash', sides=8, squash=0.4)          # soot at the mouth
    for k in range(5):
        b.log((55 + (k % 3) * 2, -70 + k * 6, 6 + (k // 3) * 9), (95, -70 + k * 6, 6 + (k // 3) * 9), 4.5)
    b.hulls.append([Vector((math.cos(a) * 70, 5 + math.sin(a) * 62, 30)) for a in np.linspace(0, 6.2, 10)]
                   + [Vector((0, 5, 118))])


def m_brew_kettle(b):   # 110 x 110 x 100
    for k in range(9):
        a = 2 * math.pi * k / 9
        b.stone((math.cos(a) * 46, math.sin(a) * 46, 12), (24, 20, 24))
    b.disc((0, 0, -0.5), 36, 2, 'Ash', sides=10)
    b.log((-30, -10, 4), (25, 12, 4), 5, 'Ash')
    b.dome((0, 0, 20), (46, 46, 30), 'Ash', rings=3, sides=14)        # big sooty kettle: round belly ...
    b.disc((0, 0, 20), 46, 34, 'Ash', sides=14)                      # ... and straight wall up to the rim
    b.disc((0, 0, 50), 42, 4, 'Water', sides=14)                     # brew
    b.log((10, 5, 40), (-25, 30, 100), 2.0, 'Wood')                  # stirring paddle
    b.hulls.append([Vector(p) for p in [(-55, -55, 0), (55, -55, 0), (-55, 55, 0), (55, 55, 0),
                                        (-55, -55, 92), (55, -55, 92), (-55, 55, 92), (55, 55, 92)]])


def m_feast_table(b):   # 400 x 120 x 80
    b.box((0, 0, 76), (400, 70, 8), 'Planks', hull=True)
    for x in (-170, 0, 170):                                         # trestles
        b.box((x, 0, 36), (8, 60, 72), 'Wood')
    for y in (-52, 52):                                              # benches
        b.box((0, y, 42), (380, 22, 6), 'Planks', hull=True)
        for x in (-160, 0, 160):
            b.box((x, y, 20), (8, 18, 40), 'Wood')
    for x in (-130, -40, 60, 150):                                   # bowls and a jug
        b.disc((x, -12 + b.rng.uniform(-6, 6), 80), 9, 5, 'Wood', sides=8)
    b.disc((10, 15, 80), 8, 22, 'Plaster', sides=8)
    worn_ground(b, (440, 180))


MODELS = {
    'workbench': ('Workbench', m_workbench, 'building_workbench'),
    'campfire': ('Campfire', m_campfire, 'building_campfire'),
    'bed': ('Bed', m_bed, 'building_bed'),
    'storage': ('Storage', m_storage, 'building_storage'),
    'woodcutter': ('Woodcutter', m_woodcutter, 'building_woodcutter'),
    'stonecutter': ('Stonecutter', m_stonecutter, 'building_stonecutter'),
    'farmstead': ('Farmstead', m_farmstead, 'building_farmstead'),
    'field': ('FieldTile', m_field, 'building_field'),
    'composter': ('Composter', m_composter, 'building_composter'),
    'rain_catcher': ('RainCatcher', m_rain_catcher, 'building_rain_catcher'),
    'tavern': ('Tavern', m_tavern, 'building_tavern'),
    'hearth': ('Hearth', m_hearth, 'building_hearth'),
    'clay_oven': ('ClayOven', m_clay_oven, 'building_clay_oven'),
    'brew_kettle': ('BrewKettle', m_brew_kettle, 'building_brew_kettle'),
    'feast_table': ('FeastTable', m_feast_table, 'building_feast_table'),
}


# --- Export, preview, main -------------------------------------------------------------------------------------------

def export(obj, hulls, name):
    folder = os.path.join(ASSETS, *GROUP.split('/'))
    os.makedirs(folder, exist_ok=True)
    bpy.ops.object.select_all(action='DESELECT')
    for o in [obj] + hulls:
        o.select_set(True)
    bpy.context.view_layer.objects.active = obj
    path = os.path.join(folder, name + '.fbx')
    bpy.ops.export_scene.fbx(filepath=path, use_selection=True, object_types={'MESH'}, apply_unit_scale=True,
                             apply_scale_options='FBX_SCALE_UNITS', axis_forward='-Y', axis_up='Z',
                             mesh_smooth_type='FACE', add_leaf_bones=False, path_mode='STRIP')
    co = np.array([v.co[:] for v in obj.data.vertices])
    tris = sum(len(p.vertices) - 2 for p in obj.data.polygons)
    slots = [s.material.name for s in obj.material_slots]
    size = co.max(axis=0) - co.min(axis=0)
    log('%s: %d tris, %.0f x %.0f x %.0f cm, %d hull(s), %s' % (name, tris, *size, len(hulls), ', '.join(slots)))
    return {'id': name.lower(), 'unreal_name': name, 'file': os.path.relpath(path, ASSETS).replace('\\', '/'),
            'group': GROUP, 'tris': tris, 'bbox_min_cm': [round(float(v), 1) for v in co.min(axis=0)],
            'bbox_max_cm': [round(float(v), 1) for v in co.max(axis=0)], 'materials': slots, 'collision': 'ucx',
            'pivot': 'Mitte unten, Boden bei Z = 0', 'source': 'tools/blender/build_stations.py (eigene Geometrie, CC0-Texturen)'}


def preview(obj, out_png):
    """Quick Eevee look in flat surface colours (checks shape and silhouette, not the final textures)."""
    colours = {'Planks': (0.42, 0.38, 0.32), 'Wood': (0.36, 0.30, 0.24), 'Stone': (0.40, 0.41, 0.40),
               'Thatch': (0.55, 0.47, 0.30), 'Plaster': (0.55, 0.45, 0.35), 'Cloth': (0.30, 0.33, 0.38),
               'Soil': (0.28, 0.21, 0.15), 'Sod': (0.30, 0.36, 0.22), 'Ash': (0.12, 0.12, 0.12), 'Water': (0.12, 0.16, 0.20)}
    for slot in obj.material_slots:
        mat = slot.material
        mat.use_nodes = True
        bsdf = next(n for n in mat.node_tree.nodes if n.type == 'BSDF_PRINCIPLED')
        bsdf.inputs['Base Color'].default_value = (*colours[mat.name.replace('M_Bld_', '').split('.')[0]], 1.0)
        bsdf.inputs['Roughness'].default_value = 0.85
    scene = bpy.context.scene
    scene.render.engine = 'BLENDER_EEVEE_NEXT' if 'BLENDER_EEVEE_NEXT' in [e.identifier for e in
                                                                           bpy.types.RenderSettings.bl_rna.properties['engine'].enum_items] else 'BLENDER_EEVEE'
    scene.render.resolution_x, scene.render.resolution_y = 640, 480
    world = bpy.data.worlds.new('W')
    world.use_nodes = True
    world.node_tree.nodes['Background'].inputs['Color'].default_value = (0.60, 0.66, 0.65, 1)
    scene.world = world
    sun = bpy.data.objects.new('Sun', bpy.data.lights.new('Sun', 'SUN'))
    sun.data.energy = 3.0
    sun.rotation_euler = (math.radians(50), 0, math.radians(35))
    scene.collection.objects.link(sun)
    co = np.array([v.co[:] for v in obj.data.vertices])
    center = Vector(((co.min(axis=0) + co.max(axis=0)) * 0.5).tolist())
    radius = float(np.linalg.norm(co.max(axis=0) - co.min(axis=0))) * 0.5
    cam = bpy.data.objects.new('Cam', bpy.data.cameras.new('Cam'))
    direction = Vector((math.cos(math.radians(-55)) * math.cos(math.radians(30)), math.sin(math.radians(-55)) * math.cos(math.radians(30)),
                        math.sin(math.radians(30))))
    cam.location = center + direction * radius * 3.0
    cam.rotation_euler = (center - cam.location).to_track_quat('-Z', 'Y').to_euler()
    cam.data.lens = 50
    cam.data.clip_end = 100000
    scene.collection.objects.link(cam)
    scene.camera = cam
    scene.render.filepath = out_png
    bpy.ops.render.render(write_still=True)


def main():
    argv = sys.argv[sys.argv.index('--') + 1:] if '--' in sys.argv else []
    only, preview_dir, textures = set(), None, True
    for i, arg in enumerate(argv):
        if arg == '--only' and i + 1 < len(argv):
            only = {s.strip() for s in argv[i + 1].split(',') if s.strip()}
        if arg == '--preview' and i + 1 < len(argv):
            preview_dir = os.path.abspath(argv[i + 1])
        if arg == '--no-textures':
            textures = False
    bpy.ops.wm.read_factory_settings(use_empty=True)
    bpy.context.scene.unit_settings.system = 'METRIC'
    bpy.context.scene.unit_settings.scale_length = 0.01

    manifest_path = os.path.join(ASSETS, 'manifest_buildings.json')
    manifest = json.load(open(manifest_path, encoding='utf-8')) if os.path.exists(manifest_path) else {}
    for key in ('textures', 'materials', 'meshes'):
        manifest.setdefault(key, [])
    if textures:
        new_tex, new_mat = build_new_textures()
        manifest['textures'] = sorted({t['name']: t for t in manifest['textures'] + new_tex}.values(), key=lambda t: t['name'])
        manifest['materials'] = sorted({m['name']: m for m in manifest['materials'] + new_mat}.values(), key=lambda m: m['name'])

    errors = 0
    entries = []
    for key, (name, maker, _) in MODELS.items():
        if only and key not in only:
            continue
        try:
            for obj in list(bpy.data.objects):
                bpy.data.objects.remove(obj, do_unlink=True)
            b = Builder(hash(key) & 0xFFFF)
            maker(b)
            obj, hulls = b.finish('SM_Station_' + name)
            entries.append(export(obj, hulls, 'SM_Station_' + name))
            if preview_dir:
                os.makedirs(preview_dir, exist_ok=True)
                for h in hulls:
                    bpy.data.objects.remove(h, do_unlink=True)
                preview(obj, os.path.join(preview_dir, 'SM_Station_%s.png' % name))
        except Exception as error:
            import traceback
            traceback.print_exc()
            errors += 1
            log('ERROR %s: %s' % (key, error))
    manifest['meshes'] = sorted({m['id']: m for m in manifest['meshes'] + entries}.values(), key=lambda m: m['id'])
    with open(manifest_path, 'w', encoding='utf-8') as handle:
        json.dump(manifest, handle, indent=2, ensure_ascii=False)
    log('done, %d model(s), %d error(s)' % (len(entries), errors))
    if errors:
        sys.exit(1)


main()
