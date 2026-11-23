"""
Preview render of body + garment (workbench, front and side), for checking tools/blender/build_clothing.py.
  blender --background --factory-startup --python preview_clothing.py -- <dir> <Body> <Outfit> <out.png>
"""
import math
import os
import sys

import bpy
from mathutils import Vector

args = sys.argv[sys.argv.index('--') + 1:]
folder, body_name, outfit, out = args[0], args[1], args[2], args[3]
bpy.ops.wm.read_factory_settings(use_empty=True)
bpy.ops.import_scene.fbx(filepath=os.path.join(folder, 'body_%s.fbx' % body_name))
for piece in outfit.split('+'):
    bpy.ops.import_scene.fbx(filepath=os.path.join(folder, 'cloth_%s_%s.fbx' % (body_name, piece)))
colours = {'Wool': (0.34, 0.40, 0.31, 1), 'Trim': (0.56, 0.30, 0.24, 1), 'Linen': (0.66, 0.61, 0.50, 1),
           'Leather': (0.30, 0.20, 0.13, 1), 'Apron': (0.56, 0.30, 0.24, 1), 'Cloak': (0.31, 0.39, 0.46, 1),
           'Metal': (0.70, 0.52, 0.25, 1)}
for mat in bpy.data.materials:
    key = next((k for k in colours if mat.name.startswith(k)), None)
    mat.diffuse_color = colours[key] if key else (0.75, 0.6, 0.5, 1)
meshes = [o for o in bpy.context.scene.objects if o.type == 'MESH']
lo = Vector((min(min((o.matrix_world @ Vector(c)).x for c in o.bound_box) for o in meshes), 0, 0))
zmax = max(max((o.matrix_world @ Vector(c)).z for c in o.bound_box) for o in meshes)
scene = bpy.context.scene
scene.render.engine = 'BLENDER_WORKBENCH'
scene.display.shading.light = 'STUDIO'
scene.display.shading.color_type = 'MATERIAL'
scene.render.resolution_x, scene.render.resolution_y = 900, 900
scene.render.film_transparent = False
cam = bpy.data.objects.new('cam', bpy.data.cameras.new('cam'))
scene.collection.objects.link(cam)
cam.data.type = 'ORTHO'
cam.data.ortho_scale = zmax * 1.15
scene.camera = cam
base, ext = os.path.splitext(out)
for label, angle in (('front', 0), ('side', 90), ('back', 180)):
    a = math.radians(angle)
    dist = zmax * 4
    cam.location = (math.sin(a) * dist, -math.cos(a) * dist, zmax * 0.5)
    cam.rotation_euler = (math.radians(90), 0, a)
    scene.render.filepath = '%s_%s%s' % (base, label, ext)
    bpy.ops.render.render(write_still=True)
    print('GAMER_CLOTH: preview', scene.render.filepath, flush=True)
