"""
Build the nature models for the game from CC0 photoscans (tools/cc0/nature.json, sources downloaded with
tools/cc0/fetch_cc0.py). Run headless from the repository root:
  E:\\Blender\\blender.exe -b --factory-startup -P tools/blender/process_nature.py -- [--only id,id] [--assets <folder>]

Per source material (one per scan):
  T_<Name>_BC.png   base colour (sRGB), with the alpha map in A for leaf cards (masked material)
  T_<Name>_N.png    normal map converted from OpenGL (Poly Haven nor_gl) to DirectX (green flipped) for Unreal
  T_<Name>_ORM.png  R = ambient occlusion, G = roughness, B = metallic (Poly Haven "arm", or built from roughness)
Per mesh: parts of the scan assembled (size, pivot at the foot, turn, position), scaled to the target size, pivot at
the foot (centre of the lowest 4 %), triangles reduced to the budget, one material slot, FBX into Assets/Nature/<group>.
Collision (per mesh, "collision" in nature.json): "ucx" adds convex hulls UCX_<Name>_NN to the FBX (whole mesh, or
"ucx_split" pieces along the longest horizontal axis; "ucx_min_z" keeps only vertices above that share of the height and
projects them to the ground, e.g. the trunk of a stump without the flat roots); "none" = no collision (ground cover).
Ground ("ground" in nature.json, built unless --only names meshes): tiling texture sets for M_Gamer_Ground in
Assets/Nature/Ground: T_<Name>_BC, T_<Name>_N (already DirectX), T_<Name>_ORM with the height map (1st..99th percentile
stretched to 0..1) in alpha for height blending.
Writes Assets/manifest_nature.json (read by tools/unreal/import_assets.py). The manifest is read again right before
writing and only the entries built in this run are replaced (tools/blender/build_trees.py writes the same file).
"""
import json
import math
import os
import sys

import bpy
import numpy as np
from mathutils import Matrix

REPO = os.path.abspath(os.path.join(os.path.dirname(__file__), '..', '..'))
SPEC = os.path.join(REPO, 'tools', 'cc0', 'nature.json')
ASSETS = os.path.join(REPO, 'Assets')
GAME_ROOT = '/Game/Gamer'


def reset_scene():
    bpy.ops.wm.read_factory_settings(use_empty=True)
    scene = bpy.context.scene
    scene.unit_settings.system = 'METRIC'
    scene.unit_settings.scale_length = 0.01   # 1 Blender unit = 1 cm, like build_assets.py


def load_pixels(path):
    image = bpy.data.images.load(path, check_existing=False)
    width, height = image.size
    pixels = np.empty(width * height * 4, dtype=np.float32)
    image.pixels.foreach_get(pixels)
    bpy.data.images.remove(image)
    return pixels.reshape(height, width, 4)


def save_png(name, pixels, path, colorspace, size):
    height, width = pixels.shape[:2]
    image = bpy.data.images.new(name, width, height, alpha=True)
    image.colorspace_settings.name = colorspace
    image.pixels.foreach_set(np.ascontiguousarray(pixels, dtype=np.float32).ravel())
    if size and size < width:
        image.scale(size, size)
    image.filepath_raw = path
    image.file_format = 'PNG'
    image.save()
    result = list(image.size)
    bpy.data.images.remove(image)
    return result


def first_existing(*paths):
    return next((p for p in paths if os.path.exists(p)), None)


def build_textures(source, root, tex_folder):
    """The three textures of one scan; returns manifest texture entries and the material entry."""
    prefix = os.path.join(root, *source['prefix'].split('/'))
    base = source['material'].replace('M_Nature_', '')
    size = int(source.get('tex', 1024))
    entries = []

    color = load_pixels(first_existing(prefix + '_diff_1k.jpg', prefix + '_diff_2k.jpg'))
    alpha_path = first_existing(prefix + '_alpha_1k.png', prefix + '_alpha_2k.png')
    masked = bool(source.get('masked'))
    if masked:
        if not alpha_path:
            raise RuntimeError(base + ': masked, but no alpha map found')
        color[:, :, 3] = load_pixels(alpha_path)[:, :, 0]
    else:
        color[:, :, 3] = 1.0
    bc = 'T_%s_BC' % base
    entries.append({'name': bc, 'file': 'Nature/Textures/%s.png' % bc,
                    'size': save_png(bc, color, os.path.join(tex_folder, bc + '.png'), 'sRGB', size), 'srgb': True})

    normal = load_pixels(first_existing(prefix + '_nor_gl_1k.jpg', prefix + '_nor_gl_2k.jpg'))
    normal[:, :, 1] = 1.0 - normal[:, :, 1]   # OpenGL -> DirectX (Unreal)
    normal[:, :, 3] = 1.0
    n = 'T_%s_N' % base
    entries.append({'name': n, 'file': 'Nature/Textures/%s.png' % n,
                    'size': save_png(n, normal, os.path.join(tex_folder, n + '.png'), 'Non-Color', size), 'srgb': False})

    arm = first_existing(prefix + '_arm_1k.jpg', prefix + '_arm_2k.jpg')
    if arm:
        orm = load_pixels(arm)
    else:
        rough = load_pixels(first_existing(prefix + '_rough_1k.jpg', prefix + '_rough_2k.jpg'))
        orm = np.zeros_like(rough)
        ao = first_existing(prefix + '_ao_1k.jpg', prefix + '_ao_2k.jpg')
        orm[:, :, 0] = load_pixels(ao)[:, :, 0] if ao else 1.0
        orm[:, :, 1] = rough[:, :, 0]
    orm[:, :, 3] = 1.0
    o = 'T_%s_ORM' % base
    entries.append({'name': o, 'file': 'Nature/Textures/%s.png' % o,
                    'size': save_png(o, orm, os.path.join(tex_folder, o + '.png'), 'Non-Color', size), 'srgb': False})

    material = {'name': source['material'], 'BaseColor': bc, 'Normal': n, 'ORM': o, 'masked': masked}
    return entries, material


def build_ground(ground, root):
    """Tiling ground texture sets; returns manifest texture entries."""
    folder_rel = ground.get('folder', 'Nature/Ground')
    folder = os.path.join(ASSETS, *folder_rel.split('/'))
    os.makedirs(folder, exist_ok=True)
    size = int(ground.get('tex', 2048))
    entries = []

    def src(rel):
        return os.path.join(root, *rel.split('/'))

    for layer in ground['layers']:
        name = layer['name']
        color = load_pixels(src(layer['color']))
        color[:, :, 3] = 1.0
        normal = load_pixels(src(layer['normal_dx']))   # DirectX already (Poly Haven nor_dx, ambientCG NormalDX)
        normal[:, :, 3] = 1.0
        if 'arm' in layer:
            orm = load_pixels(src(layer['arm']))
        else:
            orm = np.zeros_like(color)
            orm[:, :, 0] = load_pixels(src(layer['ao']))[:, :, 0] if 'ao' in layer else 1.0
            orm[:, :, 1] = load_pixels(src(layer['rough']))[:, :, 0]
        orm[:, :, 2] = 0.0   # ground has no metal
        height = load_pixels(src(layer['height']))[:, :, 0]
        lo, hi = np.percentile(height, 1.0), np.percentile(height, 99.0)
        orm[:, :, 3] = np.clip((height - lo) / max(hi - lo, 1e-4), 0.0, 1.0)
        for suffix, pixels, space, srgb in (('BC', color, 'sRGB', True), ('N', normal, 'Non-Color', False),
                                            ('ORM', orm, 'Non-Color', False)):
            tex = 'T_%s_%s' % (name, suffix)
            entries.append({'name': tex, 'file': '%s/%s.png' % (folder_rel, tex),
                            'size': save_png(tex, pixels, os.path.join(folder, tex + '.png'), space, size), 'srgb': srgb})
        print('NATURE: ground %s done' % name)
    return entries


def convex_hull_object(name, points, max_verts=40):
    import bmesh
    obj = None
    for _ in range(3):
        bm = bmesh.new()
        for p in points:
            bm.verts.new(p)
        result = bmesh.ops.convex_hull(bm, input=bm.verts)
        # convex_hull keeps the input points inside the hull: remove them, the hull alone is the collision.
        keep = {v for g in result['geom'] if isinstance(g, bmesh.types.BMFace) for v in g.verts}
        inner = [v for v in bm.verts if v not in keep]
        bmesh.ops.delete(bm, geom=inner, context='VERTS')
        mesh = bpy.data.meshes.new(name)
        bm.to_mesh(mesh)
        bm.free()
        obj = bpy.data.objects.new(name, mesh)
        bpy.context.scene.collection.objects.link(obj)
        if len(mesh.vertices) <= max_verts:
            return obj
        # Too many points for a cheap hull: collapse, then hull the remaining points again.
        modifier = obj.modifiers.new('Decimate', 'DECIMATE')
        modifier.ratio = max_verts / len(mesh.vertices) * 0.9
        bpy.ops.object.select_all(action='DESELECT')
        obj.select_set(True)
        bpy.context.view_layer.objects.active = obj
        bpy.ops.object.modifier_apply(modifier=modifier.name)
        points = [v.co.copy() for v in obj.data.vertices]
        bpy.data.objects.remove(obj, do_unlink=True)
        bpy.data.meshes.remove(mesh)
    return convex_hull_object(name, points, max_verts=10000)


def build_collision(entry, obj):
    """UCX_ convex hulls for the finished mesh (see module docstring)."""
    co = vertices(obj)
    height = co[:, 2].max() - co[:, 2].min()
    min_z = float(entry.get('ucx_min_z', 0.0)) * height
    if min_z > 0:
        co = co[co[:, 2] >= min_z]
        co = np.concatenate([co, co * np.array([1.0, 1.0, 0.0])])
    extent = co.max(axis=0) - co.min(axis=0)
    axis = 0 if extent[0] >= extent[1] else 1
    pieces = max(1, int(entry.get('ucx_split', 1)))
    lo, hi = co[:, axis].min(), co[:, axis].max()
    hulls = []
    for index in range(pieces):
        a = lo + (hi - lo) * index / pieces
        b = lo + (hi - lo) * (index + 1) / pieces
        margin = (hi - lo) * 0.02
        part = co[(co[:, axis] >= a - margin) & (co[:, axis] <= b + margin)]
        if len(part) < 8:
            continue
        hulls.append(convex_hull_object('UCX_%s_%02d' % (entry['name'], len(hulls)), [tuple(p) for p in part]))
    return hulls


def vertices(obj):
    co = np.empty(len(obj.data.vertices) * 3, dtype=np.float64)
    obj.data.vertices.foreach_get('co', co)
    return co.reshape(-1, 3)


def foot(co):
    bottom = co[:, 2].min()
    height = co[:, 2].max() - bottom
    low = co[co[:, 2] <= bottom + max(height * 0.04, 1e-4)]
    return float(low[:, 0].mean()), float(low[:, 1].mean()), float(bottom), float(height)


def triangles(mesh):
    return sum(len(polygon.vertices) - 2 for polygon in mesh.polygons)


def build_mesh(entry, source, root, out_root):
    reset_scene()
    bpy.ops.import_scene.gltf(filepath=os.path.join(root, *source['gltf'].split('/')))
    meshes = {o.name: o for o in bpy.context.scene.objects if o.type == 'MESH'}
    for obj in meshes.values():
        world = obj.matrix_world.copy()
        obj.parent = None
        obj.matrix_world = world
    wanted = [part['object'] for part in entry['parts']]
    for obj in list(bpy.context.scene.objects):
        if obj.type != 'MESH' or obj.name not in wanted:
            bpy.data.objects.remove(obj, do_unlink=True)
    missing = [name for name in wanted if name not in bpy.data.objects]
    if missing:
        raise RuntimeError('%s: objects not found: %s' % (entry['id'], ', '.join(missing)))

    parts = []
    for part in entry['parts']:
        obj = bpy.data.objects[part['object']]
        bpy.ops.object.select_all(action='DESELECT')
        obj.select_set(True)
        bpy.context.view_layer.objects.active = obj
        bpy.ops.object.transform_apply(location=True, rotation=True, scale=True)
        if not part.get('keep'):
            fx, fy, fz, height = foot(vertices(obj))
            scale = float(part.get('h', 1.0)) / max(height, 1e-9)
            at = part.get('at', [0, 0])
            matrix = (Matrix.Translation((at[0], at[1], 0.0)) @ Matrix.Rotation(math.radians(part.get('rot', 0)), 4, 'Z')
                      @ Matrix.Scale(scale, 4) @ Matrix.Translation((-fx, -fy, -fz)))
            obj.data.transform(matrix)
        parts.append(obj)

    bpy.ops.object.select_all(action='DESELECT')
    for obj in parts:
        obj.select_set(True)
    bpy.context.view_layer.objects.active = parts[0]
    if len(parts) > 1:
        bpy.ops.object.join()
    obj = bpy.context.view_layer.objects.active

    co = vertices(obj)
    extent = co.max(axis=0) - co.min(axis=0)
    measured = extent.max() if entry.get('size_axis') == 'longest' else extent[2]
    obj.data.transform(Matrix.Scale(float(entry['size_cm']) / max(measured, 1e-9), 4))
    fx, fy, fz, _ = foot(vertices(obj))
    obj.data.transform(Matrix.Translation((-fx, -fy, -fz)))
    obj.data.update()

    tris = triangles(obj.data)
    budget = int(entry['tris'])
    if tris > budget * 1.15:
        modifier = obj.modifiers.new('Decimate', 'DECIMATE')
        modifier.ratio = budget / tris
        bpy.ops.object.modifier_apply(modifier=modifier.name)
        tris = triangles(obj.data)

    obj.data.materials.clear()
    obj.data.materials.append(bpy.data.materials.new(source['material']))
    for polygon in obj.data.polygons:
        polygon.material_index = 0
    obj.name = entry['name']
    obj.data.name = entry['name']

    hulls = build_collision(entry, obj) if entry.get('collision') == 'ucx' else []

    folder = os.path.join(out_root, *entry['group'].split('/'))
    os.makedirs(folder, exist_ok=True)
    fbx = os.path.join(folder, entry['name'] + '.fbx')
    bpy.ops.object.select_all(action='DESELECT')
    for hull in hulls:
        hull.select_set(True)
    obj.select_set(True)
    bpy.context.view_layer.objects.active = obj
    bpy.ops.export_scene.fbx(
        filepath=fbx, use_selection=True, object_types={'MESH'}, apply_unit_scale=True,
        apply_scale_options='FBX_SCALE_UNITS', axis_forward='-Y', axis_up='Z',
        mesh_smooth_type='OFF', add_leaf_bones=False, path_mode='STRIP')
    co = vertices(obj)
    size = co.max(axis=0) - co.min(axis=0)
    print('NATURE: %s %d tris, %.0f x %.0f x %.0f cm, %d hull(s)' % (entry['name'], tris, *size, len(hulls)))
    result = {'id': entry['id'], 'unreal_name': entry['name'], 'file': os.path.relpath(fbx, out_root).replace('\\', '/'),
            'group': entry['group'], 'tris': tris, 'tris_target': budget,
            'bbox_min_cm': [round(float(v), 1) for v in co.min(axis=0)], 'bbox_max_cm': [round(float(v), 1) for v in co.max(axis=0)],
            'pivot': 'Fusspunkt (Mitte der untersten 4 %), Boden bei Z = 0', 'materials': [source['material']],
            'source': 'CC0 ' + source['gltf']}
    if entry.get('collision'):
        result['collision'] = entry['collision']
    return result


def main():
    global ASSETS
    argv = sys.argv[sys.argv.index('--') + 1:] if '--' in sys.argv else []
    only = set()
    for index, arg in enumerate(argv):
        if arg == '--only' and index + 1 < len(argv):
            only = {s.strip() for s in argv[index + 1].split(',') if s.strip()}
        if arg == '--assets' and index + 1 < len(argv):
            ASSETS = os.path.abspath(argv[index + 1])
    manifest_path = os.path.join(ASSETS, 'manifest_nature.json')

    with open(SPEC, encoding='utf-8') as handle:
        spec = json.load(handle)
    root = spec['root']
    manifest = {'generator': 'tools/blender/process_nature.py (+ tools/blender/build_trees.py)', 'units': '1 unit = 1 cm, Z up',
                'normal_maps': 'DirectX (Unreal)', 'orm': 'R=AO, G=Roughness, B=Metallic',
                'textures': [], 'materials': [], 'meshes': []}
    if os.path.exists(manifest_path):
        with open(manifest_path, encoding='utf-8') as handle:
            manifest.update({k: v for k, v in json.load(handle).items() if k in ('textures', 'materials', 'meshes')})
    textures, materials, meshes = {}, {}, {}   # built in this run

    tex_folder = os.path.join(ASSETS, 'Nature', 'Textures')
    os.makedirs(tex_folder, exist_ok=True)
    errors = 0
    if 'ground' in spec and (not only or 'ground' in only):
        try:
            textures.update({t['name']: t for t in build_ground(spec['ground'], root)})
        except Exception as error:
            errors += 1
            print('NATURE ERROR: ground: %s' % error)
    used_sources = {e['source'] for e in spec['meshes'] if not only or e['id'] in only}
    for key in sorted(used_sources):
        try:
            entries, material = build_textures(spec['sources'][key], root, tex_folder)
            textures.update({t['name']: t for t in entries})
            materials[material['name']] = material
            print('NATURE: textures for %s done' % material['name'])
        except Exception as error:
            errors += 1
            print('NATURE ERROR: textures %s: %s' % (key, error))
    for entry in spec['meshes']:
        if only and entry['id'] not in only:
            continue
        try:
            meshes[entry['id']] = build_mesh(entry, spec['sources'][entry['source']], root, ASSETS)
        except Exception as error:
            errors += 1
            print('NATURE ERROR: %s: %s' % (entry['id'], error))

    # Re-read right before writing: build_trees.py may have written its entries meanwhile.
    if os.path.exists(manifest_path):
        with open(manifest_path, encoding='utf-8') as handle:
            manifest.update({k: v for k, v in json.load(handle).items() if k in ('textures', 'materials', 'meshes')})
    for key, built, field in (('textures', textures, 'name'), ('materials', materials, 'name'), ('meshes', meshes, 'id')):
        merged = {item[field]: item for item in manifest[key]}
        merged.update(built)
        manifest[key] = sorted(merged.values(), key=lambda item: item[field])
    with open(manifest_path, 'w', encoding='utf-8') as handle:
        json.dump(manifest, handle, indent=2, ensure_ascii=False)
    print('NATURE: done, %d meshes in the manifest, %d error(s)' % (len(manifest['meshes']), errors))
    if errors:
        sys.exit(1)


main()
