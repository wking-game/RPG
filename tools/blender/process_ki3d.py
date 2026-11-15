"""
Post-process the AI 3D models (Meshy GLBs from tools/ki3d/meshy_generate.py) for the game.

Run headless (from the repository root):
  E:\\Blender\\blender.exe -b --factory-startup -P tools/blender/process_ki3d.py -- [--only id,id] [--raw <folder>]
or with tools/ki3d.ps1.

Per asset state (tools/ki3d/flora.json):
  - join all parts, apply transforms, scale to the target size (height, or longest side)
  - pivot at the foot: centre of the lowest 4 % of the vertices (trunk base for trees), bottom at Z = 0
  - reduce triangles if far above the budget
  - base colour texture resized to the asset's texture size, one material slot M_Nature_<Name>
  - export FBX (1 unit = 1 cm, Z up, same settings as tools/blender/build_assets.py) and PNG into Assets/Nature/
Writes Assets/manifest_ki3d.json (read by tools/unreal/import_assets.py in addition to Assets/manifest.json,
which tools/blender/build_assets.py rewrites; the two never overwrite each other).
"""
import json
import math
import os
import sys

import bpy
import numpy as np
from mathutils import Matrix

REPO = os.path.abspath(os.path.join(os.path.dirname(__file__), '..', '..'))
SPEC = os.path.join(REPO, 'tools', 'ki3d', 'flora.json')
ASSETS = os.path.join(REPO, 'Assets')
MANIFEST = os.path.join(ASSETS, 'manifest_ki3d.json')
RAW_DEFAULT = os.path.abspath(os.path.join(REPO, '..', 'ki3d_rohdaten', 'meshy'))
GAME_ROOT = '/Game/Gamer'
FLAT_NORMAL = 'T_Flat_N'
NATURE_ORM = 'T_Nature_ORM'   # AO 1, roughness 0.85, metallic 0: matt plants and stone


def camel(text):
    return ''.join(part[:1].upper() + part[1:] for part in text.split('_'))


def names(asset, state):
    suffix = '' if len(asset['states']) == 1 else '_' + camel(state)
    base = camel(asset['id']) + suffix
    return 'SM_' + base, 'T_' + base + '_BC', 'M_Nature_' + base


def reset_scene():
    bpy.ops.wm.read_factory_settings(use_empty=True)
    scene = bpy.context.scene
    scene.unit_settings.system = 'METRIC'
    scene.unit_settings.scale_length = 0.01   # 1 Blender unit = 1 cm, like build_assets.py


def save_image(name, width, height, pixels, path, colorspace):
    image = bpy.data.images.new(name, width, height, alpha=False)
    image.colorspace_settings.name = colorspace
    image.pixels.foreach_set(pixels)
    image.filepath_raw = path
    image.file_format = 'PNG'
    image.save()
    return image


def write_flat_textures(folder):
    entries = []
    for name, rgb, colorspace in ((FLAT_NORMAL, (0.5, 0.5, 1.0), 'Non-Color'), (NATURE_ORM, (1.0, 0.85, 0.0), 'Non-Color')):
        pixels = np.tile(np.array(rgb + (1.0,), dtype=np.float32), 4 * 4)
        path = os.path.join(folder, name + '.png')
        save_image(name, 4, 4, pixels, path, colorspace)
        entries.append({'name': name, 'file': os.path.relpath(path, ASSETS).replace('\\', '/'), 'size': [4, 4], 'srgb': False})
    return entries


def base_color_image(obj):
    for material in obj.data.materials:
        if not material or not material.use_nodes:
            continue
        for node in material.node_tree.nodes:
            if node.type == 'BSDF_PRINCIPLED':
                links = node.inputs['Base Color'].links
                if links and links[0].from_node.type == 'TEX_IMAGE' and links[0].from_node.image:
                    return links[0].from_node.image
        for node in material.node_tree.nodes:
            if node.type == 'TEX_IMAGE' and node.image:
                return node.image
    return None


def triangles(mesh):
    return sum(len(polygon.vertices) - 2 for polygon in mesh.polygons)


def process(asset, state, raw, out_folder, tex_folder):
    glb = os.path.join(raw, '%s__%s.glb' % (asset['id'], state))
    if not os.path.exists(glb):
        print('KI3D: missing %s, skipped' % glb)
        return None
    mesh_name, tex_name, material_name = names(asset, state)
    reset_scene()
    bpy.ops.import_scene.gltf(filepath=glb)

    parts = [o for o in bpy.context.scene.objects if o.type == 'MESH']
    if not parts:
        raise RuntimeError('%s: no mesh in %s' % (mesh_name, glb))
    for part in parts:
        world = part.matrix_world.copy()
        part.parent = None
        part.matrix_world = world
    for other in [o for o in bpy.context.scene.objects if o.type != 'MESH']:
        bpy.data.objects.remove(other, do_unlink=True)
    bpy.ops.object.select_all(action='DESELECT')
    for part in parts:
        part.select_set(True)
    bpy.context.view_layer.objects.active = parts[0]
    if len(parts) > 1:
        bpy.ops.object.join()
    obj = bpy.context.view_layer.objects.active
    bpy.ops.object.transform_apply(location=True, rotation=True, scale=True)

    # Size: height (Z), or the longest side for lying objects.
    co = np.empty(len(obj.data.vertices) * 3, dtype=np.float64)
    obj.data.vertices.foreach_get('co', co)
    co = co.reshape(-1, 3)
    extent = co.max(axis=0) - co.min(axis=0)
    measured = extent.max() if asset.get('size_axis') == 'longest' else extent[2]
    factor = float(asset['size_cm']) / max(measured, 1e-6)
    obj.data.transform(Matrix.Scale(factor, 4))
    co *= factor

    # Pivot: centre of the lowest 4 % (trunk base), bottom on the ground.
    bottom = co[:, 2].min()
    height = co[:, 2].max() - bottom
    foot = co[co[:, 2] <= bottom + max(height * 0.04, 0.5)]
    obj.data.transform(Matrix.Translation((-float(foot[:, 0].mean()), -float(foot[:, 1].mean()), -float(bottom))))
    obj.data.update()

    # Triangle budget (Meshy already remeshes to target_polycount; this only catches outliers).
    tris = triangles(obj.data)
    budget = int(asset['tris'])
    if tris > budget * 1.3:
        modifier = obj.modifiers.new('Decimate', 'DECIMATE')
        modifier.ratio = budget / tris
        bpy.ops.object.modifier_apply(modifier=modifier.name)
        tris = triangles(obj.data)

    # Texture: base colour only (stylized look, few textures), resized to the asset's size.
    image = base_color_image(obj)
    if image is None:
        raise RuntimeError('%s: no base colour texture in %s' % (mesh_name, glb))
    width, height_px = image.size
    pixels = np.empty(width * height_px * 4, dtype=np.float32)
    image.pixels.foreach_get(pixels)
    tex_path = os.path.join(tex_folder, tex_name + '.png')
    out_image = save_image(tex_name, width, height_px, pixels, tex_path, 'sRGB')
    size = int(asset.get('tex', 1024))
    if size < width:
        out_image.scale(size, size)
        out_image.save()

    # One material slot; the importer maps it to the instance of the same name.
    obj.data.materials.clear()
    obj.data.materials.append(bpy.data.materials.new(material_name))
    for polygon in obj.data.polygons:
        polygon.material_index = 0
    obj.name = mesh_name
    obj.data.name = mesh_name

    os.makedirs(out_folder, exist_ok=True)
    fbx = os.path.join(out_folder, mesh_name + '.fbx')
    bpy.ops.object.select_all(action='DESELECT')
    obj.select_set(True)
    bpy.context.view_layer.objects.active = obj
    bpy.ops.export_scene.fbx(
        filepath=fbx, use_selection=True, object_types={'MESH'}, apply_unit_scale=True,
        apply_scale_options='FBX_SCALE_UNITS', axis_forward='-Y', axis_up='Z',
        mesh_smooth_type='OFF', add_leaf_bones=False, path_mode='STRIP')

    co = np.empty(len(obj.data.vertices) * 3, dtype=np.float64)
    obj.data.vertices.foreach_get('co', co)
    co = co.reshape(-1, 3)
    print('KI3D: %s %d tris, %.0f x %.0f x %.0f cm, texture %d px' % (
        mesh_name, tris, *(co.max(axis=0) - co.min(axis=0)), min(size, width)))
    group = asset['group']
    return {
        'texture': {'name': tex_name, 'file': os.path.relpath(tex_path, ASSETS).replace('\\', '/'),
                    'size': [min(size, width), min(size, height_px)], 'srgb': True},
        'material': {'name': material_name, 'BaseColor': tex_name, 'Normal': FLAT_NORMAL, 'ORM': NATURE_ORM},
        'mesh': {'id': '%s/%s' % (asset['id'], state), 'unreal_name': mesh_name,
                 'file': os.path.relpath(fbx, ASSETS).replace('\\', '/'), 'group': group,
                 'tris': tris, 'tris_target': budget, 'bbox_min_cm': [round(float(v), 1) for v in co.min(axis=0)],
                 'bbox_max_cm': [round(float(v), 1) for v in co.max(axis=0)], 'pivot': 'Fusspunkt (Mitte der untersten 4 %), Boden bei Z = 0',
                 'materials': [material_name], 'source': 'Meshy text to 3D, %s__%s.glb' % (asset['id'], state)},
        'path': '%s/%s/%s.%s' % (GAME_ROOT, group, mesh_name, mesh_name),
    }


def main():
    global ASSETS, MANIFEST
    argv = sys.argv[sys.argv.index('--') + 1:] if '--' in sys.argv else []
    only, raw = set(), RAW_DEFAULT
    for index, arg in enumerate(argv):
        if arg == '--only' and index + 1 < len(argv):
            only = {s.strip() for s in argv[index + 1].split(',') if s.strip()}
        if arg == '--raw' and index + 1 < len(argv):
            raw = argv[index + 1]
        if arg == '--assets' and index + 1 < len(argv):   # tests: write somewhere else than Assets/
            ASSETS = os.path.abspath(argv[index + 1])
            MANIFEST = os.path.join(ASSETS, 'manifest_ki3d.json')

    with open(SPEC, encoding='utf-8') as handle:
        spec = json.load(handle)
    manifest = {'generator': 'tools/blender/process_ki3d.py', 'units': '1 unit = 1 cm, Z up',
                'source': 'Meshy text to 3D; raw GLBs and meshy_log.json outside the repository in ' + raw.replace('\\', '/'),
                'textures': [], 'materials': [], 'meshes': [], 'nodes': {}}
    if os.path.exists(MANIFEST):
        with open(MANIFEST, encoding='utf-8') as handle:
            manifest.update({k: v for k, v in json.load(handle).items() if k in ('textures', 'materials', 'meshes', 'nodes')})

    tex_folder = os.path.join(ASSETS, 'Nature', 'Textures')
    os.makedirs(tex_folder, exist_ok=True)
    flat = write_flat_textures(tex_folder)
    by_name = {t['name']: t for t in manifest['textures']}
    by_name.update({t['name']: t for t in flat})
    materials = {m['name']: m for m in manifest['materials']}
    meshes = {m['id']: m for m in manifest['meshes']}
    paths = {}
    for mesh in meshes.values():
        paths[mesh['id']] = '%s/%s/%s.%s' % (GAME_ROOT, mesh['group'], mesh['unreal_name'], mesh['unreal_name'])

    errors = 0
    for asset in spec['assets']:
        if only and asset['id'] not in only:
            continue
        for state in asset['states']:
            try:
                result = process(asset, state, raw, os.path.join(ASSETS, *asset['group'].split('/')), tex_folder)
            except Exception as error:
                errors += 1
                print('KI3D ERROR: %s/%s: %s' % (asset['id'], state, error))
                continue
            if result:
                by_name[result['texture']['name']] = result['texture']
                materials[result['material']['name']] = result['material']
                meshes[result['mesh']['id']] = result['mesh']
                paths[result['mesh']['id']] = result['path']

    # Mesh paths per resource node (for game/Content/Data/resource_nodes.json "meshes").
    nodes = {}
    for node_id, states in spec['nodes'].items():
        entry = {key: paths[ref] for key, ref in states.items() if ref in paths}
        if entry:
            nodes[node_id] = entry

    manifest['textures'] = sorted(by_name.values(), key=lambda t: t['name'])
    manifest['materials'] = sorted(materials.values(), key=lambda m: m['name'])
    manifest['meshes'] = sorted(meshes.values(), key=lambda m: m['id'])
    manifest['nodes'] = nodes
    with open(MANIFEST, 'w', encoding='utf-8') as handle:
        json.dump(manifest, handle, indent=2, ensure_ascii=False)
    print('KI3D: done, %d meshes in the manifest, %d error(s)' % (len(manifest['meshes']), errors))
    if errors:
        sys.exit(1)


main()
