"""
Imports the garments built by tools/blender/build_clothing.py (game/Saved/Clothing/cloth_<Body>_<Outfit>.fbx)
as skeletal meshes on the body's skeleton: /Game/Gamer/Characters/Clothing/<Body>/SKM_Cloth_<Body>_<Outfit>.
Material: M_Cloth (vector parameter "Tint", scalar "Roughness", fine weave noise); one instance per slot
(Wool, Trim, Linen, Leather, Apron, Cloak, Metal) with default colours from design/stil_norden_gezeitenhall.md. The game colours them
per figure (GamerFigures / dynamic material instances). Run: tools/clothing.ps1 Import
"""
import os

import unreal

SRC = os.path.join(os.path.abspath(unreal.Paths.project_saved_dir()), 'Clothing')
DEST = '/Game/Gamer/Characters/Clothing'
BODY = '/Game/Gamer/Characters/MetaHuman/{0}/Body/SKM_MHC_{0}_BodyMesh'
# Style palette: Moosgrün wool, Krapprot trim, Treibholz linen, dark leather
DEFAULTS = {'Wool': ((0x56, 0x65, 0x4F), 0.92), 'Trim': ((0x90, 0x4C, 0x3E), 0.85),
            'Linen': ((0xA9, 0x9B, 0x80), 0.9), 'Leather': ((0x4A, 0x36, 0x26), 0.6),
            'Apron': ((0x90, 0x4C, 0x3E), 0.9), 'Cloak': ((0x4E, 0x64, 0x75), 0.95), 'Metal': ((0xA8, 0x7E, 0x45), 0.35)}
errors = []
assets = unreal.get_editor_subsystem(unreal.EditorAssetSubsystem)
mel = unreal.MaterialEditingLibrary


def log(message):
    unreal.log('GAMER_CLOTH: ' + message)


def fail(message):
    errors.append(message)
    unreal.log_error('GAMER_CLOTH: ' + message)


def srgb(c):
    return unreal.LinearColor(*[((v / 255.0 + 0.055) / 1.055) ** 2.4 for v in c], 1.0)


def base_material():
    path = DEST + '/M_Cloth'
    if assets.does_asset_exist(path):
        return unreal.load_asset(path)
    tools = unreal.AssetToolsHelpers.get_asset_tools()
    mat = tools.create_asset('M_Cloth', DEST, unreal.Material, unreal.MaterialFactoryNew())
    tint = mel.create_material_expression(mat, unreal.MaterialExpressionVectorParameter, -600, 0)
    tint.set_editor_property('parameter_name', 'Tint')
    tint.set_editor_property('default_value', srgb((0x8A, 0x8C, 0x86)))
    # fine weave: world-independent noise on the UVs darkens the colour a little, so cloth is not plastic-flat
    noise = mel.create_material_expression(mat, unreal.MaterialExpressionNoise, -900, 200)
    noise.set_editor_property('scale', 40.0)
    noise.set_editor_property('levels', 2)
    lerp = mel.create_material_expression(mat, unreal.MaterialExpressionLinearInterpolate, -300, 100)
    dark = mel.create_material_expression(mat, unreal.MaterialExpressionMultiply, -450, 160)
    dark.set_editor_property('const_b', 0.78)
    mel.connect_material_expressions(tint, '', dark, 'A')
    mel.connect_material_expressions(tint, '', lerp, 'A')
    mel.connect_material_expressions(dark, '', lerp, 'B')
    mel.connect_material_expressions(noise, '', lerp, 'Alpha')
    mel.connect_material_property(lerp, '', unreal.MaterialProperty.MP_BASE_COLOR)
    rough = mel.create_material_expression(mat, unreal.MaterialExpressionScalarParameter, -600, 350)
    rough.set_editor_property('parameter_name', 'Roughness')
    rough.set_editor_property('default_value', 0.9)
    mel.connect_material_property(rough, '', unreal.MaterialProperty.MP_ROUGHNESS)
    mat.set_editor_property('two_sided', True)  # open hems and sleeves are seen from inside
    mat.set_editor_property('used_with_skeletal_mesh', True)
    mel.recompile_material(mat)
    assets.save_asset(path, only_if_is_dirty=False)
    log('created ' + path)
    return mat


def slot_instance(parent, slot):
    path = '%s/MI_Cloth_%s' % (DEST, slot)
    if assets.does_asset_exist(path):
        return unreal.load_asset(path)
    tools = unreal.AssetToolsHelpers.get_asset_tools()
    mi = tools.create_asset('MI_Cloth_' + slot, DEST, unreal.MaterialInstanceConstant,
                            unreal.MaterialInstanceConstantFactoryNew())
    mel.set_material_instance_parent(mi, parent)
    colour, rough = DEFAULTS[slot]
    mel.set_material_instance_vector_parameter_value(mi, 'Tint', srgb(colour))
    mel.set_material_instance_scalar_parameter_value(mi, 'Roughness', rough)
    assets.save_asset(path, only_if_is_dirty=False)
    return mi


parent = base_material()
instances = {slot: slot_instance(parent, slot) for slot in DEFAULTS}
for body_name in ('Kraeftig', 'Schmal'):
    body = unreal.load_asset(BODY.format(body_name))
    if body is None:
        fail('missing body ' + BODY.format(body_name))
        continue
    skeleton = body.get_editor_property('skeleton')
    for outfit in ('Tunika', 'Kleid', 'Schuerzenkleid', 'Umhang'):
        fbx = os.path.join(SRC, 'cloth_%s_%s.fbx' % (body_name, outfit))
        if not os.path.exists(fbx):
            fail('missing ' + fbx)
            continue
        folder = '%s/%s' % (DEST, body_name)
        name = 'SKM_Cloth_%s_%s' % (body_name, outfit)
        task = unreal.AssetImportTask()
        task.filename = fbx
        task.destination_path = folder
        task.destination_name = name
        task.automated = True
        task.replace_existing = True
        task.save = True
        ui = unreal.FbxImportUI()
        ui.import_mesh = True
        ui.import_as_skeletal = True
        ui.mesh_type_to_import = unreal.FBXImportType.FBXIT_SKELETAL_MESH
        ui.skeleton = skeleton
        ui.import_materials = False
        ui.import_textures = False
        ui.import_animations = False
        ui.create_physics_asset = False
        ui.skeletal_mesh_import_data.set_editor_property('import_morph_targets', False)
        task.options = ui
        unreal.AssetToolsHelpers.get_asset_tools().import_asset_tasks([task])
        mesh = unreal.load_asset('%s/%s' % (folder, name))
        if not isinstance(mesh, unreal.SkeletalMesh):
            fail('%s: import failed (%s)' % (name, task.imported_object_paths))
            continue
        # material slots by name (Wool, Trim, Linen, Leather from Blender)
        materials = list(mesh.get_editor_property('materials'))
        for m in materials:
            slot = str(m.get_editor_property('material_slot_name'))
            key = next((k for k in DEFAULTS if slot.startswith(k)), None)
            if key:
                m.set_editor_property('material_interface', instances[key])
        mesh.set_editor_property('materials', materials)
        assets.save_asset(mesh.get_path_name(), only_if_is_dirty=False)
        box = mesh.get_bounds()
        log('%s: skeleton %s, slots %s, bounds %s' % (name, mesh.get_editor_property('skeleton').get_name(),
            [str(m.get_editor_property('material_slot_name')) for m in materials], box.box_extent))

log('done, %d error(s)' % len(errors))
