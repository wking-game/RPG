"""
Style test (2026-11-14): restyle stylized CC0 buildings towards the chosen look (own style leaning on Aska, see
design/stil_gelebter_norden.md): photo/scan materials, muted colours, high roughness, clear shapes.
Run headless from the repository root:
  E:\\Blender\\blender.exe -b --factory-startup -P tools/blender/restyle_buildings.py -- [--only hut,watchtower,village_house]
      [--sources D:\\gamer\\cc0_quellen] [--preview <folder>] [--no-textures]

Sources (CC0, unpacked by hand into <sources>/extern/..., only FBX/PNG/JPG are read):
  highlands/Hut, highlands/WatchTower   "Highlands - Fantasy 3D Buildings Pack" (Visions Mind)
  medieval_village_megakit/...           "Medieval Village MegaKit [Standard]" (Quaternius), modular pieces
  polyhaven/<set>                        photo textures (diff, nor_dx, arm; 2k)

Per building two meshes in Assets/Buildings/StyleTest:
  SM_<Name>_Original.fbx  the source as it is (same real-world scale, original textures) for the comparison
  SM_<Name>.fbx           restyled:
    - surfaces classified (Highlands: by the colour of the atlas texture under each face, majority per UV island;
      MegaKit: by the material slot) into thatch, planks, wood, stone, plaster, cloth; one material slot each
    - new UVs with real texel density (texture world size per material, see SURFACES): stone/plaster box projection,
      wood/planks along the main axis of each part (grain), thatch along the roof slope
    - real size (door height about 195 cm), edges bevelled (2 cm), roof overhang (village house: own thatch roof)
    - collision: UCX hulls of the source scaled along (Highlands), village house: none (= exact mesh in Unreal)
  Moss, damp and weathering are not baked: the shared material M_Gamer_Building (tools/unreal/import_assets.py)
  adds them by height above the pivot, facing and noise, with parameters per surface (SURFACES[...]['params']).
Textures (Assets/Buildings/StyleTest/Textures): T_Bld<Surface>_BC/N/ORM (shared by all restyled buildings) and
T_BldOrig<Name>_BC/N/ORM for the originals. Normal maps DirectX, ORM R=AO G=Roughness B=Metallic, 1 unit = 1 cm,
FBX axis_forward -Y, Z up (same conventions as process_nature.py).
Writes Assets/manifest_buildings.json (read by tools/unreal/import_assets.py).
--preview renders each mesh with Eevee (false colours per surface, plus textured) into <folder>.
"""
import colorsys
import json
import math
import os
import re
import sys

import bmesh
import bpy
import numpy as np
from mathutils import Matrix, Vector

REPO = os.path.abspath(os.path.join(os.path.dirname(__file__), '..', '..'))
ASSETS = os.path.join(REPO, 'Assets')
GROUP = 'Buildings/StyleTest'
TEX_REL = GROUP + '/Textures'
SOURCES = r'D:\gamer\cc0_quellen'
KIT = 'extern/medieval_village_megakit/Medieval Village MegaKit[Standard]'

# Restyled surfaces: Poly Haven set, world size of one texture repeat (cm), grain direction in the image
# ('u', 'v' or None), texture size, colour/roughness grading, and the moss/damp/weathering parameters of
# M_Gamer_Building (height in cm above the pivot = ground).
SURFACES = {
    'Thatch': {'set': 'thatch_roof_angled', 'world': 200.0, 'grain': 'v', 'tex': 2048, 'sat': 0.85, 'gain': 1.05,
               'warm': (1.10, 1.0, 0.82),   # the source is very grey; a little straw colour back
               'params': {'MossAmount': 0.55, 'MossHeight': 60.0, 'UpMoss': 0.55, 'DampHeight': 40.0, 'Weathering': 0.15}},
    'Planks': {'set': 'weathered_planks', 'world': 200.0, 'grain': 'v', 'tex': 2048, 'sat': 0.85, 'gain': 1.10,
               'params': {'MossAmount': 0.45, 'MossHeight': 45.0, 'UpMoss': 0.25, 'DampHeight': 55.0, 'Weathering': 0.30}},
    'Wood': {'set': 'wood_planks', 'world': 200.0, 'grain': 'u', 'tex': 2048, 'sat': 0.70, 'gain': 0.85,
             'params': {'MossAmount': 0.35, 'MossHeight': 40.0, 'UpMoss': 0.30, 'DampHeight': 50.0, 'Weathering': 0.45}},
    'Stone': {'set': 'stone_wall', 'world': 250.0, 'grain': None, 'tex': 2048, 'sat': 0.70, 'gain': 0.85,
              'params': {'MossAmount': 0.80, 'MossHeight': 90.0, 'UpMoss': 0.70, 'DampHeight': 70.0, 'Weathering': 0.10}},
    'Plaster': {'set': 'clay_plaster', 'world': 200.0, 'grain': None, 'tex': 2048, 'sat': 0.75, 'gain': 1.10,
                'params': {'MossAmount': 0.25, 'MossHeight': 35.0, 'UpMoss': 0.10, 'DampHeight': 60.0, 'Weathering': 0.10}},
    # Wool cloth (banner, flag): plaster texture recoloured to a muted woad blue; no photo source for cloth yet.
    'Cloth': {'set': 'clay_plaster', 'world': 120.0, 'grain': None, 'tex': 1024, 'tint': (0.30, 0.34, 0.40),
              'params': {'MossAmount': 0.0, 'MossHeight': 1.0, 'UpMoss': 0.0, 'DampHeight': 1.0, 'Weathering': 0.25}},
}
ROUGH_MIN = 0.55          # "Rauheit eher hoch": roughness remapped to ROUGH_MIN..1

BUILDINGS = {
    # scale: source metres -> cm; door of the Hut is about 65 cm in the source, of the tower about 60 cm.
    'hut': {'name': 'Hut', 'kind': 'highlands', 'folder': 'Hut', 'scale': 300.0},
    'watchtower': {'name': 'WatchTower', 'kind': 'highlands', 'folder': 'WatchTower', 'scale': 320.0},
    'village_house': {'name': 'VillageHouse', 'kind': 'megakit', 'scale': 93.0},   # kit door 210 cm -> 195 cm
}

# MegaKit material slots: original texture set and restyled surface.
KIT_MATERIALS = {
    'MI_Plaster': ('Plaster', 'T_Plaster_BaseColor', 'T_Plaster_Normal', 'T_Plaster_ORM', None),
    'MI_WoodTrim': ('Wood', 'T_WoodTrim_BaseColor', 'T_WoodTrim_Normal', 'T_WoodTrim_ORM', None),
    'MI_WoodTrim_Wear': ('Wood', 'T_WoodTrim_BaseColor', 'T_WoodTrim_Normal', 'T_WoodTrim_ORM', None),
    'MI_RockTrim': ('Stone', 'T_RockTrim_BaseColor', 'T_RockTrim_Normal', 'T_RockTrim_ORM', None),
    'MI_Brick': ('Stone', 'T_Brick_BaseColor', 'T_Brick_Normal', None, 'T_Brick_Roughness'),
    'MI_RoundTiles': ('Thatch', 'T_RoundTiles_BaseColor', 'T_RoundTiles_Normal', None, 'T_RoundTiles_Roughness'),
    'MI_WindowGlass': ('Planks', None, None, None, None),
}
KIT_ORIGINAL_SLOT = {'MI_WoodTrim_Wear': 'MI_WoodTrim'}   # same textures (the wear overlay is not rebuilt)

FALSE_COLOURS = {'Thatch': (0.85, 0.7, 0.2), 'Planks': (0.45, 0.3, 0.15), 'Wood': (0.7, 0.4, 0.15),
                 'Stone': (0.45, 0.45, 0.5), 'Plaster': (0.9, 0.85, 0.75), 'Cloth': (0.1, 0.3, 0.9)}


def log(message):
    print('RESTYLE: ' + message)


# --- Scene and image helpers ----------------------------------------------------------------------------------------

def reset_scene():
    bpy.ops.wm.read_factory_settings(use_empty=True)
    scene = bpy.context.scene
    scene.unit_settings.system = 'METRIC'
    scene.unit_settings.scale_length = 0.01   # 1 Blender unit = 1 cm


def load_pixels(path, size=None):
    image = bpy.data.images.load(path, check_existing=False)
    if size and image.size[0] != size:
        image.scale(size, size)
    width, height = image.size
    pixels = np.empty(width * height * 4, dtype=np.float32)
    image.pixels.foreach_get(pixels)
    bpy.data.images.remove(image)
    return pixels.reshape(height, width, 4)


def save_png(name, pixels, path, colorspace):
    height, width = pixels.shape[:2]
    image = bpy.data.images.new(name, width, height, alpha=True)
    image.colorspace_settings.name = colorspace
    image.pixels.foreach_set(np.ascontiguousarray(pixels, dtype=np.float32).ravel())
    image.filepath_raw = path
    image.file_format = 'PNG'
    image.save()
    bpy.data.images.remove(image)
    return [width, height]


def luminance(rgb):
    return rgb[..., 0] * 0.2126 + rgb[..., 1] * 0.7152 + rgb[..., 2] * 0.0722


class TextureWriter:
    def __init__(self, folder):
        self.folder = folder
        self.entries = []
        os.makedirs(folder, exist_ok=True)

    def write(self, name, pixels, srgb):
        path = os.path.join(self.folder, name + '.png')
        size = save_png(name, pixels, path, 'sRGB' if srgb else 'Non-Color')
        self.entries.append({'name': name, 'file': '%s/%s.png' % (TEX_REL, name), 'size': size, 'srgb': srgb,
                             'folder': '/Game/Gamer/' + TEX_REL})
        return name

    def material(self, base, color, normal, orm):
        """Write T_<base>_BC/N/ORM, return the three names."""
        color[:, :, 3] = 1.0
        normal[:, :, 3] = 1.0
        orm[:, :, 3] = 1.0
        return (self.write('T_%s_BC' % base, color, True), self.write('T_%s_N' % base, normal, False),
                self.write('T_%s_ORM' % base, orm, False))


def build_surface_textures(writer, sources):
    materials = []
    for surface, spec in SURFACES.items():
        folder = os.path.join(sources, 'polyhaven', spec['set'])
        prefix = os.path.join(folder, spec['set'])
        size = spec['tex']
        color = load_pixels(prefix + '_diff_2k.jpg', size)
        normal = load_pixels(prefix + '_nor_dx_2k.jpg', size)
        orm = load_pixels(prefix + '_arm_2k.jpg', size)
        lum = luminance(color[:, :, :3])[..., None]
        if 'tint' in spec:
            # Keep the luminance detail (normalised around 1), take the hue from the tint.
            color[:, :, :3] = np.clip(np.asarray(spec['tint']) * (0.55 + 0.9 * lum / max(float(lum.mean()), 1e-4)) * 0.6, 0, 1)
            normal[:, :, :2] = 0.5 + (normal[:, :, :2] - 0.5) * 0.4   # cloth is flatter than plaster
            normal[:, :, 2] = 1.0
        else:
            # Muted colours: less saturation, slight exposure correction (values in sRGB).
            color[:, :, :3] = np.clip((lum + (color[:, :, :3] - lum) * spec['sat']) * spec['gain'], 0, 1)
            if 'warm' in spec:
                color[:, :, :3] = np.clip(color[:, :, :3] * np.asarray(spec['warm']), 0, 1)
        orm[:, :, 1] = ROUGH_MIN + (1.0 - ROUGH_MIN) * orm[:, :, 1]
        orm[:, :, 2] = 0.0
        names = writer.material('Bld' + surface, color, normal, orm)
        materials.append({'name': 'M_Bld_' + surface, 'BaseColor': names[0], 'Normal': names[1], 'ORM': names[2],
                          'parent': 'building', 'params': spec['params']})
        log('textures %s (%s, %d px, %.0f cm per repeat = %.0f px/m)' % (surface, spec['set'], size, spec['world'],
                                                                         size / spec['world'] * 100.0))
    return materials


def build_original_textures(writer, building, sources):
    """Original textures at 2k (sources are 4k / 2k). Returns material entries {name: entry}."""
    materials = {}
    if building['kind'] == 'highlands':
        folder = os.path.join(sources, 'extern', 'highlands', building['folder'], 'TextureMaps')
        prefix = os.path.join(folder, 'T_%s_' % building['folder'])
        color = load_pixels(prefix + 'diffuse.png', 2048)
        normal = load_pixels(prefix + 'normal.png', 2048)   # made for UE4: DirectX already
        orm = np.ones_like(color)
        orm[:, :, 1] = load_pixels(prefix + 'roughness.png', 2048)[:, :, 0]
        orm[:, :, 2] = load_pixels(prefix + 'metalness.png', 2048)[:, :, 0] if os.path.exists(prefix + 'metalness.png') else 0.0
        base = 'BldOrig' + building['name']
        names = writer.material(base, color, normal, orm)
        materials['M_BldOrig_' + building['name']] = {'name': 'M_BldOrig_' + building['name'], 'BaseColor': names[0],
                                                      'Normal': names[1], 'ORM': names[2], 'parent': 'base'}
        return materials
    folder = os.path.join(sources, *KIT.split('/'), 'Textures')
    for slot, (_, bc, n, orm_name, rough) in KIT_MATERIALS.items():
        if slot in KIT_ORIGINAL_SLOT:
            continue
        key = slot.replace('MI_', 'Kit')
        if bc is None:   # window glass: plain dark blue-grey
            color = np.zeros((64, 64, 4), dtype=np.float32)
            color[:, :, :3] = (0.18, 0.24, 0.32)
            normal = np.zeros_like(color)
            normal[:, :, :3] = (0.5, 0.5, 1.0)
            orm = np.zeros_like(color)
            orm[:, :, :3] = (1.0, 0.2, 0.0)
        else:
            color = load_pixels(os.path.join(folder, bc + '.png'))
            normal = load_pixels(os.path.join(folder, n + '.png'))   # root folder = DirectX (OpenGL copies in a subfolder)
            if orm_name:
                orm = load_pixels(os.path.join(folder, orm_name + '.png'))
            else:
                orm = np.ones_like(color)
                orm[:, :, 1] = load_pixels(os.path.join(folder, rough + '.png'))[:, :, 0]
                orm[:, :, 2] = 0.0
        names = writer.material('BldOrig' + key, color, normal, orm)
        materials['M_BldOrig_' + key] = {'name': 'M_BldOrig_' + key, 'BaseColor': names[0], 'Normal': names[1],
                                         'ORM': names[2], 'parent': 'base'}
    return materials


# --- Geometry helpers -----------------------------------------------------------------------------------------------

def import_fbx(path):
    before = set(bpy.data.objects)
    bpy.ops.import_scene.fbx(filepath=path)
    new = [o for o in bpy.data.objects if o not in before]
    for obj in new:
        world = obj.matrix_world.copy()
        obj.parent = None
        obj.matrix_world = world
    for obj in [o for o in new if o.type != 'MESH']:
        bpy.data.objects.remove(obj, do_unlink=True)
    new = [o for o in new if o.name in bpy.data.objects]
    for obj in new:
        apply_transform(obj)
    return new


def apply_transform(obj):
    obj.data.transform(obj.matrix_world)
    obj.matrix_world = Matrix.Identity(4)


def join(objects, name):
    bpy.ops.object.select_all(action='DESELECT')
    for obj in objects:
        obj.select_set(True)
    bpy.context.view_layer.objects.active = objects[0]
    if len(objects) > 1:
        bpy.ops.object.join()
    obj = bpy.context.view_layer.objects.active
    obj.name = name
    obj.data.name = name
    return obj


def triangles(mesh):
    return sum(len(p.vertices) - 2 for p in mesh.polygons)


def clean_slot_name(name):
    return re.sub(r'\.\d+$', '', name)


def set_slots(obj, names):
    """Replace the material slots by new (empty) materials with the given names, in order."""
    obj.data.materials.clear()
    for name in names:
        mat = bpy.data.materials.get(name) or bpy.data.materials.new(name)
        obj.data.materials.append(mat)


# --- Classification of the Highlands atlas --------------------------------------------------------------------------

BARY = [(1 / 3, 1 / 3, 1 / 3), (0.6, 0.2, 0.2), (0.2, 0.6, 0.2), (0.2, 0.2, 0.6), (0.45, 0.45, 0.1),
        (0.1, 0.45, 0.45), (0.45, 0.1, 0.45)]


def classify_colour(rgb):
    h, s, v = colorsys.rgb_to_hsv(*[float(c) for c in rgb])
    if s > 0.40 and 0.50 <= h <= 0.75:
        return 'Cloth'             # banner and flag (blue)
    if s > 0.55 and h < 0.08:
        return 'Wood'              # orange finial
    if s < 0.22:
        return 'Planks' if v > 0.45 else 'Stone'   # light grey: window glass
    if s < 0.35:
        return 'Planks'            # greyish brown boards (doors)
    if v > 0.29:
        return 'Thatch'            # thatch is lighter than the dark olive timber of the atlas
    return 'Wood'


def classify_highlands(obj, diffuse_path):
    """Surface class per face by the atlas colour, then majority (by area) per UV island."""
    pixels = load_pixels(diffuse_path, 1024)
    height, width = pixels.shape[:2]
    mesh = obj.data
    uv = mesh.uv_layers.active.data
    raw = []
    for poly in mesh.polygons:
        loops = list(poly.loop_indices)
        samples = []
        for i in range(1, len(loops) - 1):
            a, b, c = (Vector(uv[loops[0]].uv), Vector(uv[loops[i]].uv), Vector(uv[loops[i + 1]].uv))
            for wa, wb, wc in BARY:
                p = a * wa + b * wb + c * wc
                x = min(width - 1, max(0, int((p.x % 1.0) * width)))
                y = min(height - 1, max(0, int((p.y % 1.0) * height)))
                samples.append(pixels[y, x, :3])
        # Majority of the single samples (a mean colour would mix e.g. a banner's blue and its gold motif).
        counts = {}
        for sample in samples:
            cls = classify_colour(sample)
            counts[cls] = counts.get(cls, 0) + 1
        raw.append(max(counts.items(), key=lambda kv: kv[1])[0])

    # UV islands: faces joined across edges whose UVs agree on both sides.
    parent = list(range(len(mesh.polygons)))

    def find(i):
        while parent[i] != i:
            parent[i] = parent[parent[i]]
            i = parent[i]
        return i

    # Per edge the UV of each of its two vertices in each adjacent face.
    edge_uvs = {}
    for poly in mesh.polygons:
        loop_uv = {mesh.loops[li].vertex_index: tuple(uv[li].uv) for li in poly.loop_indices}
        for key in poly.edge_keys:
            edge_uvs.setdefault(key, []).append((poly.index, loop_uv[key[0]], loop_uv[key[1]]))
    for entries in edge_uvs.values():
        if len(entries) != 2:
            continue
        (fa, a0, a1), (fb, b0, b1) = entries
        if (Vector(a0) - Vector(b0)).length < 1e-4 and (Vector(a1) - Vector(b1)).length < 1e-4:
            parent[find(fa)] = find(fb)
    votes = {}
    for poly, cls in zip(mesh.polygons, raw):
        votes.setdefault(find(poly.index), {}).setdefault(cls, 0.0)
        votes[find(poly.index)][cls] += poly.area
    island_class = {root: max(v.items(), key=lambda kv: kv[1])[0] for root, v in votes.items()}
    classes = [island_class[find(p.index)] for p in mesh.polygons]
    summary = {}
    for poly, cls in zip(mesh.polygons, classes):
        summary[cls] = summary.get(cls, 0.0) + poly.area
    total = sum(summary.values())
    log('%s: classes by area %s (%d UV islands)' % (obj.name, ', '.join('%s %.0f%%' % (k, v / total * 100)
                                                                         for k, v in sorted(summary.items())), len(votes)))
    return classes


def assign_classes(obj, classes):
    names = sorted(set(classes))
    set_slots(obj, ['M_Bld_' + n for n in names])
    for poly, cls in zip(obj.data.polygons, classes):
        poly.material_index = names.index(cls)


# --- Restyle: UVs with real texel density ---------------------------------------------------------------------------

def surface_of_slot(obj, index):
    return obj.data.materials[index].name.replace('M_Bld_', '').split('.')[0]


def project_uvs_impl(obj):
    mesh = obj.data
    bm = bmesh.new()
    bm.from_mesh(mesh)
    bm.faces.ensure_lookup_table()
    for layer in list(bm.loops.layers.uv):
        bm.loops.layers.uv.remove(layer)
    uv_layer = bm.loops.layers.uv.new('UVMap')
    force_v = bm.faces.layers.int.get('grain_z')

    visited = set()
    axes = {}
    for face in bm.faces:
        if face.index in visited:
            continue
        stack, members = [face], []
        visited.add(face.index)
        while stack:
            current = stack.pop()
            members.append(current)
            for edge in current.edges:
                for other in edge.link_faces:
                    if other.index not in visited and other.material_index == face.material_index:
                        visited.add(other.index)
                        stack.append(other)
        if SURFACES[surface_of_slot(obj, face.material_index)]['grain'] is None:
            continue
        points = np.array([v.co[:] for f in members for v in f.verts])
        points -= points.mean(axis=0)
        _, _, vt = np.linalg.svd(points, full_matrices=False)
        axis = Vector(vt[0]).normalized()
        if force_v is not None and face[force_v]:
            axis = Vector((0, 0, 1))
        # Offset per part, so neighbouring beams do not show the same piece of the texture.
        offset = Vector(((face.index * 0.6180339) % 1.0, (face.index * 0.4142135) % 1.0))
        for f in members:
            axes[f.index] = (axis, offset)

    for face in bm.faces:
        surface = surface_of_slot(obj, face.material_index)
        spec = SURFACES[surface]
        size = spec['world']
        n = face.normal
        if surface == 'Thatch':
            down = Vector((0, 0, -1))
            t = down - n * down.dot(n)
            if t.length < 0.2:   # (almost) horizontal face: any direction
                t = Vector((1, 0, 0)) - n * n.x
            t.normalize()
            b = n.cross(t).normalized()
            for loop in face.loops:
                p = loop.vert.co
                loop[uv_layer].uv = (p.dot(b) / size, -p.dot(t) / size)
        elif spec['grain']:
            axis, offset = axes[face.index]
            t = axis - n * axis.dot(n)
            if t.length < 0.2:   # face across the grain (end grain): use any in-plane direction
                ref = Vector((0, 0, 1)) if abs(n.z) < 0.9 else Vector((1, 0, 0))
                t = ref - n * ref.dot(n)
            t.normalize()
            b = n.cross(t).normalized()
            for loop in face.loops:
                p = loop.vert.co
                along, across = p.dot(t) / size + offset.y, p.dot(b) / size + offset.x
                loop[uv_layer].uv = (across, along) if spec['grain'] == 'v' else (along, across)
        else:
            ax = max(range(3), key=lambda i: abs(n[i]))
            for loop in face.loops:
                p = loop.vert.co
                if ax == 2:
                    uv = (p.x, p.y)
                elif ax == 0:
                    uv = (p.y * (1 if n.x > 0 else -1), p.z)
                else:
                    uv = (p.x * (-1 if n.y > 0 else 1), p.z)
                loop[uv_layer].uv = (uv[0] / size, uv[1] / size)
    bm.to_mesh(mesh)
    bm.free()
    while len(mesh.uv_layers) > 1:
        mesh.uv_layers.remove(mesh.uv_layers[0] if mesh.uv_layers[0].name != 'UVMap' else mesh.uv_layers[1])


def bevel_and_shade(obj, width=2.0, angle=35.0):
    bpy.ops.object.select_all(action='DESELECT')
    obj.select_set(True)
    bpy.context.view_layer.objects.active = obj
    bpy.ops.object.mode_set(mode='EDIT')
    bpy.ops.mesh.select_all(action='SELECT')
    bpy.ops.mesh.remove_doubles(threshold=0.05)
    bpy.ops.object.mode_set(mode='OBJECT')
    modifier = obj.modifiers.new('Bevel', 'BEVEL')
    modifier.width = width
    modifier.segments = 1
    modifier.limit_method = 'ANGLE'
    modifier.angle_limit = math.radians(angle)
    modifier.use_clamp_overlap = True
    modifier.harden_normals = True
    bpy.ops.object.shade_smooth_by_angle(angle=math.radians(angle), keep_sharp_edges=True)
    bpy.ops.object.modifier_apply(modifier=modifier.name)


# --- Highlands ------------------------------------------------------------------------------------------------------

def load_highlands(building, sources):
    folder = os.path.join(sources, 'extern', 'highlands', building['folder'])
    objects = import_fbx(os.path.join(folder, 'SM_%s.fbx' % building['folder']))
    mesh = next(o for o in objects if not o.name.startswith('UCX_'))
    hulls = [o for o in objects if o.name.startswith('UCX_')]
    # Imported in cm (scene unit 0.01) = source metres * 100; bring to the target scale.
    factor = building['scale'] / 100.0
    for obj in objects:
        obj.data.transform(Matrix.Scale(factor, 4))
        obj.data.update()
    return mesh, hulls, os.path.join(folder, 'TextureMaps', 'T_%s_diffuse.png' % building['folder'])


# --- MegaKit village house ------------------------------------------------------------------------------------------

# (piece, x, y, rotation in degrees) in kit metres; house 4 m x 6 m, front (door) towards -Y, outside of the wall
# pieces is their local -Y.
HOUSE_WALLS = [
    ('Wall_Plaster_Door_Flat', -1, -3, 0), ('Wall_Plaster_Window_Wide_Flat', 1, -3, 0),
    ('Wall_Plaster_Straight_Base', -1, 3, 180), ('Wall_Plaster_Straight_Base', 1, 3, 180),
    ('Wall_Plaster_Straight_Base', -2, -2, -90), ('Wall_Plaster_Window_Wide_Flat', -2, 0, -90),
    ('Wall_Plaster_Straight_Base', -2, 2, -90),
    ('Wall_Plaster_Straight_Base', 2, -2, 90), ('Wall_Plaster_Window_Wide_Flat', 2, 0, 90),
    ('Wall_Plaster_Straight', 2, 2, 90),
]
HOUSE_EXTRA = [
    ('WindowShutters_Wide_Flat_Closed', 1, -3, 0), ('WindowShutters_Wide_Flat_Closed', -2, 0, -90),
    ('WindowShutters_Wide_Flat_Closed', 2, 0, 90),
    ('Door_1_Flat', -1.53, -3, 0),
    ('Corner_Exterior_Wood', -2, -3, 0), ('Corner_Exterior_Wood', 2, -3, 0),
    ('Corner_Exterior_Wood', -2, 3, 0), ('Corner_Exterior_Wood', 2, 3, 0),
] + [('Floor_WoodDark', x, y, 0) for x in (-1, 1) for y in (-2, 0, 2)]
HOUSE_KIT_ROOF = [('Roof_RoundTiles_4x6', 0, 0, 0, 3.0), ('Roof_Front_Brick4', 0, -3, 0, 3.0),
                  ('Roof_Front_Brick4', 0, 3, 180, 3.0)]
WALL_TOP = 3.12
PITCH = 50.0            # Nordic, steep
OVERHANG_EAVE = 0.6     # metres, horizontal
OVERHANG_GABLE = 0.45
ROOF_THICK = 0.32
HALF_SPAN = 2.12        # outer face of the long walls


def kit_piece(sources, name, x, y, rot, z=0.0):
    path = os.path.join(sources, *KIT.split('/'), 'FBX', name + '.fbx')
    objects = import_fbx(path)
    matrix = Matrix.Translation((x * 100.0, y * 100.0, z * 100.0)) @ Matrix.Rotation(math.radians(rot), 4, 'Z')
    for obj in objects:
        obj.data.transform(matrix)
        obj.data.update()
        for slot in obj.material_slots:
            if slot.material:
                slot.material.name = clean_slot_name(slot.material.name)
    return objects


def box_mesh(name, verts, faces, slot):
    mesh = bpy.data.meshes.new(name)
    mesh.from_pydata([Vector(v) * 100.0 for v in verts], [], faces)
    mesh.update()
    obj = bpy.data.objects.new(name, mesh)
    bpy.context.scene.collection.objects.link(obj)
    mat = bpy.data.materials.get(slot) or bpy.data.materials.new(slot)
    mesh.materials.append(mat)
    return obj


BOX_FACES = [(0, 1, 2, 3), (7, 6, 5, 4), (0, 4, 5, 1), (1, 5, 6, 2), (2, 6, 7, 3), (3, 7, 4, 0)]


def oriented_box(name, a, b, width, thick, normal, slot):
    """Box along the segment a-b, width across (perpendicular to the segment and normal), thickness along normal."""
    a, b, normal = Vector(a), Vector(b), Vector(normal).normalized()
    along = (b - a).normalized()
    side = along.cross(normal).normalized() * (width / 2)
    up = normal * thick
    verts = [a - side, b - side, b + side, a + side, a - side + up, b - side + up, b + side + up, a + side + up]
    return box_mesh(name, verts, BOX_FACES, slot)


def thatch_roof():
    """Own gable roof: two thick thatch slabs with overhang, ridge beam, crossed barge boards, plank gables."""
    tan = math.tan(math.radians(PITCH))
    ridge_z = WALL_TOP + HALF_SPAN * tan
    eave_x = HALF_SPAN + OVERHANG_EAVE
    eave_z = WALL_TOP - OVERHANG_EAVE * tan
    y0, y1 = -3 - OVERHANG_GABLE, 3 + OVERHANG_GABLE
    parts = []
    for sign in (-1, 1):
        normal = Vector((sign * math.sin(math.radians(PITCH)), 0, math.cos(math.radians(PITCH))))
        off = normal * ROOF_THICK
        low0, low1 = Vector((sign * eave_x, y0, eave_z)), Vector((sign * eave_x, y1, eave_z))
        top0, top1 = Vector((0, y0, ridge_z)), Vector((0, y1, ridge_z))
        # Upper surface meets at the ridge: move its ridge line up so both slabs close the gap.
        top_off = Vector((0, 0, ROOF_THICK / math.cos(math.radians(PITCH))))
        verts = [low0, low1, top1, top0, low0 + off, low1 + off, top1 + top_off, top0 + top_off]
        if sign > 0:
            faces = [(0, 3, 2, 1), (4, 5, 6, 7), (0, 1, 5, 4), (1, 2, 6, 5), (2, 3, 7, 6), (3, 0, 4, 7)]
        else:
            faces = [(0, 1, 2, 3), (7, 6, 5, 4), (0, 4, 5, 1), (1, 5, 6, 2), (2, 6, 7, 3), (3, 7, 4, 0)]
        parts.append(box_mesh('RoofSlab', verts, faces, 'M_Bld_Thatch'))
    top_z = ridge_z + ROOF_THICK / math.cos(math.radians(PITCH))
    # Ridge: a log-like beam along the ridge (turf/thatch ridges were held down with timber).
    parts.append(oriented_box('Ridge', (0, y0 - 0.05, top_z - 0.08), (0, y1 + 0.05, top_z - 0.08), 0.26, 0.22,
                              (0, 0, 1), 'M_Bld_Wood'))
    # Barge boards on both gables, crossing above the ridge (clear Nordic silhouette).
    for y in (y0 - 0.04, y1 + 0.04):
        for sign in (-1, 1):
            start = Vector((sign * (eave_x + 0.1), y, eave_z + ROOF_THICK * 0.6))
            direction = Vector((-sign * math.cos(math.radians(PITCH)), 0, math.sin(math.radians(PITCH))))
            length = (top_z - start.z) / direction.z + 0.75
            parts.append(oriented_box('Barge', start, start + direction * length, 0.24, 0.07,
                                      (0, 1 if y > 0 else -1, 0), 'M_Bld_Wood'))
    # Gables: vertical boards between the wall top and the roof.
    for y, sign in ((-3.0, -1), (3.0, 1)):
        t = 0.10
        verts = [(-HALF_SPAN, y - t, WALL_TOP), (HALF_SPAN, y - t, WALL_TOP), (0, y - t, ridge_z),
                 (-HALF_SPAN, y + t, WALL_TOP), (HALF_SPAN, y + t, WALL_TOP), (0, y + t, ridge_z)]
        faces = [(0, 2, 1), (3, 4, 5), (0, 1, 4, 3), (1, 2, 5, 4), (2, 0, 3, 5)]
        gable = box_mesh('Gable', verts, faces, 'M_Bld_Planks')
        parts.append(gable)
    return parts


def build_village_house(sources, restyled):
    pieces = []
    for name, x, y, rot in HOUSE_WALLS + HOUSE_EXTRA:
        pieces += kit_piece(sources, name, x, y, rot)
    if restyled:
        roof = thatch_roof()
        # A low stone plinth course under the walls (the kit walls start at the ground).
        plinth = []
        for a, b in (((-2.2, -3.1), (2.2, -3.1)), ((2.2, -3.1), (2.2, 3.1)), ((2.2, 3.1), (-2.2, 3.1)),
                     ((-2.2, 3.1), (-2.2, -3.1))):
            plinth.append(oriented_box('Plinth', (a[0], a[1], -0.15), (b[0], b[1], -0.15), 0.34, 0.27,
                                       (0, 0, 1), 'M_Bld_Stone'))
        pieces += roof + plinth
    else:
        for name, x, y, rot, z in HOUSE_KIT_ROOF:
            pieces += kit_piece(sources, name, x, y, rot, z)
    gable_names = {o.name for o in pieces if o.name.startswith('Gable')}
    # Slot names per piece: original kit slots, or restyled surfaces.
    for obj in pieces:
        names = []
        for slot in obj.material_slots:
            name = clean_slot_name(slot.material.name) if slot.material else 'MI_WoodTrim'
            if name.startswith('M_Bld_'):
                names.append(name)
            elif restyled:
                names.append('M_Bld_' + KIT_MATERIALS[name][0])
            else:
                names.append('M_BldOrig_' + KIT_ORIGINAL_SLOT.get(name, name).replace('MI_', 'Kit'))
        indices = [p.material_index for p in obj.data.polygons]
        set_slots(obj, names)
        for poly, index in zip(obj.data.polygons, indices):
            poly.material_index = index
        # Mark faces of the gables for vertical boards.
        if obj.name in gable_names:
            attr = obj.data.attributes.new('grain_z', 'INT', 'FACE')
            for i in range(len(obj.data.polygons)):
                attr.data[i].value = 1
    obj = join(pieces, 'tmp')
    merge_slots(obj)
    obj.data.transform(Matrix.Scale(BUILDINGS['village_house']['scale'] / 100.0, 4))
    obj.data.update()
    return obj


def merge_slots(obj):
    """Join leaves duplicate slots with the same name; collapse them."""
    names = [clean_slot_name(s.material.name) for s in obj.material_slots]
    unique = sorted(set(names))
    indices = [unique.index(names[p.material_index]) for p in obj.data.polygons]
    set_slots(obj, unique)
    for poly, index in zip(obj.data.polygons, indices):
        poly.material_index = index


# --- Export and preview ---------------------------------------------------------------------------------------------

def export(obj, hulls, name, out_folder):
    obj.name = name
    obj.data.name = name
    for index, hull in enumerate(hulls):
        hull.name = 'UCX_%s_%02d' % (name, index)
        hull.data.materials.clear()
    bpy.ops.object.select_all(action='DESELECT')
    for hull in hulls:
        hull.select_set(True)
    obj.select_set(True)
    bpy.context.view_layer.objects.active = obj
    path = os.path.join(out_folder, name + '.fbx')
    bpy.ops.export_scene.fbx(
        filepath=path, use_selection=True, object_types={'MESH'}, apply_unit_scale=True,
        apply_scale_options='FBX_SCALE_UNITS', axis_forward='-Y', axis_up='Z',
        mesh_smooth_type='OFF', add_leaf_bones=False, path_mode='STRIP')
    co = np.array([v.co[:] for v in obj.data.vertices])
    tris = triangles(obj.data)
    slots = [s.material.name for s in obj.material_slots]
    size = co.max(axis=0) - co.min(axis=0)
    log('%s: %d tris, %.0f x %.0f x %.0f cm, %d hull(s), slots %s' % (name, tris, *size, len(hulls), ', '.join(slots)))
    entry = {'id': name.lower(), 'unreal_name': name, 'file': os.path.relpath(path, ASSETS).replace('\\', '/'),
             'group': GROUP, 'tris': tris,
             'bbox_min_cm': [round(float(v), 1) for v in co.min(axis=0)],
             'bbox_max_cm': [round(float(v), 1) for v in co.max(axis=0)], 'materials': slots}
    if hulls:
        entry['collision'] = 'ucx'
    return entry


def preview(obj, out_png, writer_folder, false_colour):
    """Eevee render of obj with a 180 cm figure for scale."""
    scene = bpy.context.scene
    others = [o for o in scene.objects if o is not obj]
    for o in others:
        o.hide_render = True
    mats = []
    for slot in obj.material_slots:
        mat = slot.material
        mat.use_nodes = True
        nodes, links = mat.node_tree.nodes, mat.node_tree.links
        bsdf = next(n for n in nodes if n.type == 'BSDF_PRINCIPLED')
        key = mat.name.replace('M_Bld_', 'Bld').replace('M_BldOrig_', 'BldOrig')
        bc = os.path.join(writer_folder, 'T_%s_BC.png' % key)
        surface = mat.name.replace('M_Bld_', '')
        if false_colour and surface in FALSE_COLOURS:
            for link in list(bsdf.inputs['Base Color'].links):
                links.remove(link)
            bsdf.inputs['Base Color'].default_value = (*FALSE_COLOURS[surface], 1)
        elif os.path.exists(bc):
            tex = nodes.new('ShaderNodeTexImage')
            tex.image = bpy.data.images.load(bc, check_existing=True)
            links.new(tex.outputs['Color'], bsdf.inputs['Base Color'])
        bsdf.inputs['Roughness'].default_value = 0.85
    co = np.array([v.co[:] for v in obj.data.vertices])
    lo, hi = Vector(co.min(axis=0)), Vector(co.max(axis=0))
    center = (lo + hi) / 2
    size = (hi - lo).length
    bpy.ops.mesh.primitive_cylinder_add(radius=22, depth=180, location=(lo.x - 80, lo.y - 60, 90))
    person = bpy.context.object
    cam_data = bpy.data.cameras.new('cam')
    cam_data.lens = 35
    cam_data.clip_end = 1e6
    cam = bpy.data.objects.new('cam', cam_data)
    scene.collection.objects.link(cam)
    scene.camera = cam
    sun_data = bpy.data.lights.new('sun', 'SUN')
    sun_data.energy = 4.0
    sun = bpy.data.objects.new('sun', sun_data)
    scene.collection.objects.link(sun)
    sun.rotation_euler = (math.radians(50), 0, math.radians(-35))
    world = bpy.data.worlds.new('w')
    scene.world = world
    world.use_nodes = True
    world.node_tree.nodes['Background'].inputs['Color'].default_value = (0.55, 0.62, 0.7, 1)
    scene.render.engine = 'BLENDER_EEVEE'
    scene.render.resolution_x, scene.render.resolution_y = 1000, 800
    direction = Vector((-0.55, -1.0, 0.45)).normalized()
    cam.location = center + direction * size * 1.15
    cam.rotation_euler = (center - cam.location).to_track_quat('-Z', 'Y').to_euler()
    scene.render.filepath = out_png
    bpy.ops.render.render(write_still=True)
    for o in (person, cam, sun):
        bpy.data.objects.remove(o, do_unlink=True)
    for o in others:
        o.hide_render = False


# --- Main -----------------------------------------------------------------------------------------------------------

def main():
    argv = sys.argv[sys.argv.index('--') + 1:] if '--' in sys.argv else []
    only, sources, preview_folder, textures = set(), SOURCES, None, True
    for index, arg in enumerate(argv):
        if arg == '--only' and index + 1 < len(argv):
            only = {s.strip() for s in argv[index + 1].split(',') if s.strip()}
        if arg == '--sources' and index + 1 < len(argv):
            sources = argv[index + 1]
        if arg == '--preview' and index + 1 < len(argv):
            preview_folder = argv[index + 1]
        if arg == '--no-textures':
            textures = False
    out_folder = os.path.join(ASSETS, *GROUP.split('/'))
    tex_folder = os.path.join(ASSETS, *TEX_REL.split('/'))
    os.makedirs(out_folder, exist_ok=True)
    writer = TextureWriter(tex_folder)
    manifest_path = os.path.join(ASSETS, 'manifest_buildings.json')
    manifest = {'generator': 'tools/blender/restyle_buildings.py', 'units': '1 unit = 1 cm, Z up',
                'normal_maps': 'DirectX (Unreal)', 'orm': 'R=AO, G=Roughness, B=Metallic',
                'textures': [], 'materials': [], 'meshes': []}
    if os.path.exists(manifest_path):
        with open(manifest_path, encoding='utf-8') as handle:
            manifest.update({k: v for k, v in json.load(handle).items() if k in ('textures', 'materials', 'meshes')})
    materials, meshes = {}, {}
    errors = 0
    reset_scene()
    if textures:
        for entry in build_surface_textures(writer, sources):
            materials[entry['name']] = entry
    for key, building in BUILDINGS.items():
        if only and key not in only:
            continue
        try:
            if textures:
                materials.update(build_original_textures(writer, building, sources))
            name = 'SM_' + building['name']
            for restyled in (False, True):
                reset_scene()
                if building['kind'] == 'highlands':
                    obj, hulls, diffuse = load_highlands(building, sources)
                    if restyled:
                        assign_classes(obj, classify_highlands(obj, diffuse))
                    else:
                        set_slots(obj, ['M_BldOrig_' + building['name']])
                        for poly in obj.data.polygons:
                            poly.material_index = 0
                else:
                    obj, hulls = build_village_house(sources, restyled), []
                if restyled:
                    bevel_and_shade(obj)
                    project_uvs_impl(obj)
                mesh_name = name if restyled else name + '_Original'
                entry = export(obj, hulls, mesh_name, out_folder)
                entry['source'] = ('CC0 Highlands (Visions Mind) ' if building['kind'] == 'highlands'
                                   else 'CC0 Medieval Village MegaKit (Quaternius) ') + ('restyled' if restyled else 'original')
                meshes[entry['id']] = entry
                if preview_folder:
                    os.makedirs(preview_folder, exist_ok=True)
                    preview(obj, os.path.join(preview_folder, mesh_name + '.png'), tex_folder, False)
                    if restyled:
                        preview(obj, os.path.join(preview_folder, mesh_name + '_classes.png'), tex_folder, True)
        except Exception as error:
            import traceback
            traceback.print_exc()
            errors += 1
            log('ERROR %s: %s' % (key, error))

    texture_entries = {t['name']: t for t in writer.entries}
    for key, built, field in (('textures', texture_entries, 'name'), ('materials', materials, 'name'),
                              ('meshes', meshes, 'id')):
        merged = {item[field]: item for item in manifest[key]}
        merged.update(built)
        manifest[key] = sorted(merged.values(), key=lambda item: item[field])
    with open(manifest_path, 'w', encoding='utf-8') as handle:
        json.dump(manifest, handle, indent=2, ensure_ascii=False)
    log('done, %d meshes in the manifest, %d error(s)' % (len(manifest['meshes']), errors))
    if errors:
        sys.exit(1)


main()
