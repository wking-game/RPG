"""
Renders UI icons from the real 3D models (decision 2026-11-14: icons are rendered from the models, no AI images).
  E:\\Blender\\blender.exe -b --factory-startup -P tools/blender/render_icons.py -- <Assets folder> [<id> ...]
Reads tools/blender/icon_map.json ({ "<item or buildable id>": "<fbx path relative to Assets>" }) and writes
Assets/UI/Icons/T_Icon_<id>.png (256x256, transparent) plus Assets/manifest_icons.json.
All icons use the same light, the same view (30 degrees up, 35 degrees around) and the same fill (80 % of the square),
so they look like one set. Textures are found by material name (M_Nature_X, M_X -> T_X_BC/N/ORM under Assets/**/Textures).
Things without a model get no file; the game draws a named placeholder tile for them.
"""
import glob
import json
import math
import os
import sys

import bpy
import numpy as np
from mathutils import Vector

args = sys.argv[sys.argv.index('--') + 1:]
ASSETS = os.path.abspath(args[0])
ONLY = set(args[1:])
ROOT = os.path.dirname(os.path.abspath(__file__))
MAP = json.load(open(os.path.join(ROOT, 'icon_map.json'), encoding='utf-8'))
OUT = os.path.join(ASSETS, 'UI', 'Icons')
os.makedirs(OUT, exist_ok=True)
SIZE = 512          # rendered size, scaled down to 256
FINAL = 256
FILL = 0.80

TEXTURE_DIRS = glob.glob(os.path.join(ASSETS, '**', 'Textures'), recursive=True)
built = {}


def find_texture(key, suffix):
    for folder in TEXTURE_DIRS:
        path = os.path.join(folder, 'T_%s_%s.png' % (key, suffix))
        if os.path.exists(path):
            return path
    return None


def material_for(name):
    name = name.split('.')[0]
    if name in built:
        return built[name]
    key = name.replace('M_Nature_', '').replace('M_', '')
    mat = bpy.data.materials.new(name + '_icon')
    mat.use_nodes = True
    nodes, links = mat.node_tree.nodes, mat.node_tree.links
    bsdf = next(n for n in nodes if n.type == 'BSDF_PRINCIPLED')
    bsdf.inputs['Roughness'].default_value = 0.7
    built[name] = mat
    base = find_texture(key, 'BC')
    if base:
        img = nodes.new('ShaderNodeTexImage')
        img.image = bpy.data.images.load(base)
        links.new(img.outputs['Color'], bsdf.inputs['Base Color'])
        if key.startswith(('Leaves', 'Needles', 'Nettle', 'Grass')):
            links.new(img.outputs['Alpha'], bsdf.inputs['Alpha'])
            if hasattr(mat, 'surface_render_method'):
                mat.surface_render_method = 'DITHERED'
            mat.use_backface_culling = False
    else:
        bsdf.inputs['Base Color'].default_value = (0.55, 0.50, 0.44, 1)
    nrm = find_texture(key, 'N')
    if nrm:
        nimg = nodes.new('ShaderNodeTexImage')
        nimg.image = bpy.data.images.load(nrm)
        nimg.image.colorspace_settings.name = 'Non-Color'
        sep = nodes.new('ShaderNodeSeparateColor')
        comb = nodes.new('ShaderNodeCombineColor')
        inv = nodes.new('ShaderNodeMath')
        inv.operation = 'SUBTRACT'
        inv.inputs[0].default_value = 1.0
        links.new(nimg.outputs['Color'], sep.inputs['Color'])
        links.new(sep.outputs['Red'], comb.inputs['Red'])
        links.new(sep.outputs['Green'], inv.inputs[1])
        links.new(inv.outputs['Value'], comb.inputs['Green'])    # DirectX -> OpenGL for Blender
        links.new(sep.outputs['Blue'], comb.inputs['Blue'])
        nmap = nodes.new('ShaderNodeNormalMap')
        links.new(comb.outputs['Color'], nmap.inputs['Color'])
        links.new(nmap.outputs['Normal'], bsdf.inputs['Normal'])
    return mat


def setup_scene():
    bpy.ops.wm.read_factory_settings(use_empty=True)
    scene = bpy.context.scene
    scene.unit_settings.scale_length = 0.01
    for engine in ('BLENDER_EEVEE_NEXT', 'BLENDER_EEVEE'):
        try:
            scene.render.engine = engine
            break
        except TypeError:
            continue
    scene.render.film_transparent = True
    scene.render.resolution_x = scene.render.resolution_y = SIZE
    scene.view_settings.view_transform = 'Standard'
    world = bpy.data.worlds.new('IconWorld')
    world.use_nodes = True
    world.node_tree.nodes['Background'].inputs['Color'].default_value = (0.55, 0.55, 0.55, 1)
    world.node_tree.nodes['Background'].inputs['Strength'].default_value = 0.9
    scene.world = world
    try:
        scene.eevee.taa_render_samples = 48
    except AttributeError:
        pass
    sun_d = bpy.data.lights.new('IconSun', 'SUN')
    sun_d.energy = 3.0
    sun_d.angle = math.radians(25)
    sun = bpy.data.objects.new('IconSun', sun_d)
    scene.collection.objects.link(sun)
    cd = bpy.data.cameras.new('IconCam')
    cd.type = 'ORTHO'
    cd.clip_start, cd.clip_end = 1, 100000
    cam = bpy.data.objects.new('IconCam', cd)
    scene.collection.objects.link(cam)
    scene.camera = cam
    return scene, sun, cam, cd


def clear_models():
    for obj in list(bpy.data.objects):
        if obj.type == 'MESH':
            bpy.data.objects.remove(obj, do_unlink=True)


def render_one(scene, sun, cam, cd, icon_id, fbx):
    clear_models()
    before = set(bpy.data.objects)
    bpy.ops.import_scene.fbx(filepath=os.path.join(ASSETS, fbx))
    objs = []
    for obj in [o for o in bpy.data.objects if o not in before]:
        if obj.name.startswith('UCX_') or obj.type != 'MESH':
            bpy.data.objects.remove(obj, do_unlink=True)
            continue
        for slot in obj.material_slots:
            if slot.material:
                slot.material = material_for(slot.material.name)
        objs.append(obj)
    if not objs:
        print('ICON skipped (no mesh): %s' % icon_id)
        return None
    bpy.context.view_layer.update()
    el, az = math.radians(30), math.radians(35)
    d = Vector((math.cos(el) * math.cos(az), math.cos(el) * math.sin(az), math.sin(el)))
    # Fit around the bounding box centre
    pts_world = [o.matrix_world @ Vector(c) for o in objs for c in o.bound_box]
    centre = sum(pts_world, Vector()) / len(pts_world)
    cam.location = centre + d * 3000
    cam.rotation_euler = (-d).to_track_quat('-Z', 'Y').to_euler()
    right = cam.rotation_euler.to_matrix() @ Vector((1, 0, 0))
    up = cam.rotation_euler.to_matrix() @ Vector((0, 1, 0))
    sun.rotation_euler = (-(d * 0.6 - right * 0.8 + up * 1.0)).to_track_quat('-Z', 'Y').to_euler()
    bpy.context.view_layer.update()
    inv = cam.matrix_world.inverted()
    pts = [inv @ (o.matrix_world @ v.co) for o in objs for v in o.data.vertices]
    xs, ys = [p.x for p in pts], [p.y for p in pts]
    ext = max(max(xs) - min(xs), max(ys) - min(ys))
    cd.ortho_scale = ext / FILL
    cd.shift_x = ((max(xs) + min(xs)) / 2) / cd.ortho_scale
    cd.shift_y = ((max(ys) + min(ys)) / 2) / cd.ortho_scale
    tmp = os.path.join(OUT, '_tmp_%s.png' % icon_id)
    scene.render.filepath = tmp
    bpy.ops.render.render(write_still=True)
    img = bpy.data.images.load(tmp)
    px = np.array(img.pixels[:], dtype=np.float32).reshape(SIZE, SIZE, 4)
    a = px[..., 3] > 0.5
    rows, cols = np.where(a.any(1))[0], np.where(a.any(0))[0]
    fill = max(rows[-1] - rows[0] + 1, cols[-1] - cols[0] + 1) / SIZE if len(rows) else 0.0
    img.scale(FINAL, FINAL)
    out = os.path.join(OUT, 'T_Icon_%s.png' % icon_id)
    img.filepath_raw = out
    img.file_format = 'PNG'
    img.save()
    bpy.data.images.remove(img)
    os.remove(tmp)
    print('ICON %s <- %s fill %.2f' % (icon_id, fbx, fill))
    return {'id': icon_id, 'name': 'T_Icon_%s' % icon_id, 'file': 'UI/Icons/T_Icon_%s.png' % icon_id, 'model': fbx, 'size': [FINAL, FINAL], 'fill_ratio': round(float(fill), 2)}


scene, sun, cam, cd = setup_scene()
manifest = []
for icon_id, fbx in MAP.items():
    if icon_id.startswith('_') or (ONLY and icon_id not in ONLY):
        continue
    entry = render_one(scene, sun, cam, cd, icon_id, fbx)
    if entry:
        manifest.append(entry)
if not ONLY:
    with open(os.path.join(ASSETS, 'manifest_icons.json'), 'w', encoding='utf-8') as f:
        json.dump({'generator': 'tools/blender/render_icons.py', 'icons': manifest}, f, indent=2, ensure_ascii=False)
print('ICONS done: %d' % len(manifest))
