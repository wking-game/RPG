"""
Preview render of nature FBX files with their textures (for checking the look before importing into Unreal).
  E:\\Blender\\blender.exe -b --factory-startup -P tools/blender/preview_nature.py -- <assets folder> <out.png> <fbx> [<fbx> ...]
Materials are rebuilt from the slot names (M_Nature_X -> Nature/Textures/T_X_BC|N|ORM.png); leaf cards use the
alpha of the base colour. The objects stand in a row on a grey ground, lit by a low sun and a sky colour.
"""
import math
import os
import sys

import bpy
from mathutils import Vector

args = sys.argv[sys.argv.index('--') + 1:]
assets, out, files = args[0], args[1], args[2:]
tex = os.path.join(assets, 'Nature', 'Textures')
bpy.ops.wm.read_factory_settings(use_empty=True)
scene = bpy.context.scene
scene.unit_settings.scale_length = 0.01


built = {}


def material_for(name):
    if name in built:
        return built[name]
    key = name.replace('M_Nature_', '')
    mat = bpy.data.materials.new(name + '_preview')
    mat.use_nodes = True
    nodes, links = mat.node_tree.nodes, mat.node_tree.links
    bsdf = next(n for n in nodes if n.type == 'BSDF_PRINCIPLED')
    built[name] = mat
    path = os.path.join(tex, 'T_%s_BC.png' % key)
    if os.path.exists(path):
        img = nodes.new('ShaderNodeTexImage')
        img.image = bpy.data.images.load(path)
        links.new(img.outputs['Color'], bsdf.inputs['Base Color'])
        if key.startswith(('Leaves', 'Needles', 'Nettle', 'Grass')):
            links.new(img.outputs['Alpha'], bsdf.inputs['Alpha'])
            if hasattr(mat, 'surface_render_method'):
                mat.surface_render_method = 'DITHERED'
            mat.use_backface_culling = False
    npath = os.path.join(tex, 'T_%s_N.png' % key)
    if os.path.exists(npath):
        nimg = nodes.new('ShaderNodeTexImage')
        nimg.image = bpy.data.images.load(npath)
        nimg.image.colorspace_settings.name = 'Non-Color'
        sep = nodes.new('ShaderNodeSeparateColor'); comb = nodes.new('ShaderNodeCombineColor')
        inv = nodes.new('ShaderNodeMath'); inv.operation = 'SUBTRACT'; inv.inputs[0].default_value = 1.0
        links.new(nimg.outputs['Color'], sep.inputs['Color'])
        links.new(sep.outputs['Red'], comb.inputs['Red'])
        links.new(sep.outputs['Green'], inv.inputs[1])
        links.new(inv.outputs['Value'], comb.inputs['Green'])   # DirectX -> OpenGL for Blender
        links.new(sep.outputs['Blue'], comb.inputs['Blue'])
        nmap = nodes.new('ShaderNodeNormalMap')
        links.new(comb.outputs['Color'], nmap.inputs['Color'])
        links.new(nmap.outputs['Normal'], bsdf.inputs['Normal'])
    return mat


x = 0.0
objects = []
for path in files:
    before = set(bpy.data.objects)
    bpy.ops.import_scene.fbx(filepath=path)
    new = [o for o in bpy.data.objects if o not in before]
    for obj in new:
        if obj.name.startswith('UCX_') or obj.type != 'MESH':
            bpy.data.objects.remove(obj, do_unlink=True)
            continue
        for slot in obj.material_slots:
            if slot.material:
                slot.material = material_for(slot.material.name.split('.')[0])
        width = obj.dimensions.x
        obj.location.x = x + width / 2
        x += width + 80
        objects.append(obj)

bpy.ops.mesh.primitive_plane_add(size=1, location=(x / 2, 0, 0))
ground = bpy.context.object
ground.scale = (x * 2 + 2000, 4000, 1)
gmat = bpy.data.materials.new('ground'); gmat.use_nodes = True
gmat.node_tree.nodes['Principled BSDF'].inputs['Base Color'].default_value = (0.20, 0.22, 0.14, 1)
ground.data.materials.append(gmat)

height = max(o.dimensions.z for o in objects)
cam_data = bpy.data.cameras.new('cam'); cam_data.lens = 24; cam_data.clip_end = 100000
cam = bpy.data.objects.new('cam', cam_data); scene.collection.objects.link(cam)
distance = max(x * 0.70, height * 1.45) * 1.1
cam.location = (x / 2, -distance, height * 0.5)
direction = Vector((x / 2, 0, height * 0.45)) - cam.location
cam.rotation_euler = direction.to_track_quat('-Z', 'Y').to_euler()
scene.camera = cam
sun_data = bpy.data.lights.new('sun', 'SUN'); sun_data.energy = 4.0
sun = bpy.data.objects.new('sun', sun_data); scene.collection.objects.link(sun)
sun.rotation_euler = (math.radians(50), 0, math.radians(35))
world = bpy.data.worlds.new('w'); scene.world = world; world.use_nodes = True
world.node_tree.nodes['Background'].inputs['Color'].default_value = (0.55, 0.68, 0.85, 1)
world.node_tree.nodes['Background'].inputs['Strength'].default_value = 0.9
for engine in ('BLENDER_EEVEE_NEXT', 'BLENDER_EEVEE'):
    try:
        scene.render.engine = engine
        break
    except Exception:
        continue
scene.render.resolution_x, scene.render.resolution_y = 1600, 800
scene.render.filepath = out
bpy.ops.render.render(write_still=True)
print('PREVIEW written', out)
