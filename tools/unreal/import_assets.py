"""
Import the generated assets (Assets/manifest.json) into Unreal under /Game/Gamer/<group>/...

Run headless (editor closed), from the repository root:
  E:\\UE58\\Engine\\Binaries\\Win64\\UnrealEditor-Cmd.exe game\\Gamer.uproject -run=pythonscript
      -script="<repo>\\tools\\unreal\\import_assets.py" -unattended -nosplash
or with tools/import_assets.ps1.

Repeatable: existing assets are replaced. Creates
  /Game/Gamer/Textures/...          textures (sRGB and compression from the manifest)
  /Game/Gamer/Materials/M_Gamer_Base        opaque base material (BaseColor, Normal, ORM parameters)
  /Game/Gamer/Materials/M_Trim_Wood, M_Turf, M_Item_Log   instances of the base
  /Game/Gamer/Materials/M_BuildPreview      translucent unlit material for build previews (Color, Opacity)
  /Game/Gamer/Buildings/WoodKit/SM_...      building pieces (collision = mesh, so door openings are walkable)
  /Game/Gamer/Items/SM_Item_Log
  /Game/Gamer/UI/Icons/...
Writes a summary line "GAMER_IMPORT: ..." to the log; exits with an error if anything failed.
"""
import json
import os

import unreal

PROJECT_DIR = os.path.abspath(unreal.Paths.project_dir())
REPO = os.path.abspath(os.path.join(PROJECT_DIR, '..'))
ASSETS = os.path.join(REPO, 'Assets')
ROOT = '/Game/Gamer'
MATERIALS = ROOT + '/Materials'

asset_tools = unreal.AssetToolsHelpers.get_asset_tools()
editor_assets = unreal.get_editor_subsystem(unreal.EditorAssetSubsystem)
mel = unreal.MaterialEditingLibrary
errors = []


def log(message):
    unreal.log('GAMER_IMPORT: ' + message)


def fail(message):
    errors.append(message)
    unreal.log_error('GAMER_IMPORT: ' + message)


def import_file(path, destination, name, options=None):
    task = unreal.AssetImportTask()
    task.set_editor_property('filename', path)
    task.set_editor_property('destination_path', destination)
    task.set_editor_property('destination_name', name)
    task.set_editor_property('replace_existing', True)
    task.set_editor_property('automated', True)
    task.set_editor_property('save', False)
    if options is not None:
        task.set_editor_property('options', options)
    asset_tools.import_asset_tasks([task])
    imported = task.get_editor_property('imported_object_paths')
    if not imported:
        fail('import failed: ' + path)
        return None
    return unreal.load_asset(destination + '/' + name)


def create_or_load(name, path, asset_class, factory):
    full = path + '/' + name
    if editor_assets.does_asset_exist(full):
        return unreal.load_asset(full)
    return asset_tools.create_asset(name, path, asset_class, factory)


# --- Textures -------------------------------------------------------------------------------

manifest_path = os.path.join(ASSETS, 'manifest.json')
with open(manifest_path, encoding='utf-8') as handle:
    manifest = json.load(handle)

textures = {}
for entry in manifest['textures']:
    name = entry['name']
    tex = import_file(os.path.join(ASSETS, entry['file']), ROOT + '/Textures', name)
    if tex is None:
        continue
    tex.set_editor_property('srgb', bool(entry['srgb']))
    if name.endswith('_N'):
        tex.set_editor_property('compression_settings', unreal.TextureCompressionSettings.TC_NORMALMAP)
    elif name.endswith('_ORM'):
        tex.set_editor_property('compression_settings', unreal.TextureCompressionSettings.TC_MASKS)
    textures[name] = tex
log('textures: %d' % len(textures))

for entry in manifest.get('icons', []):
    tex = import_file(os.path.join(ASSETS, entry['file']), ROOT + '/UI/Icons', entry['name'])
    if tex is not None:
        tex.set_editor_property('compression_settings', unreal.TextureCompressionSettings.TC_EDITOR_ICON)
        tex.set_editor_property('lod_group', unreal.TextureGroup.TEXTUREGROUP_UI)
        tex.set_editor_property('srgb', True)

# --- Base material (opaque, lit) ------------------------------------------------------------


def texture_parameter(material, name, sampler, default, x, y):
    node = mel.create_material_expression(material, unreal.MaterialExpressionTextureSampleParameter2D, x, y)
    node.set_editor_property('parameter_name', name)
    node.set_editor_property('sampler_type', sampler)
    node.set_editor_property('texture', default)
    return node


base = create_or_load('M_Gamer_Base', MATERIALS, unreal.Material, unreal.MaterialFactoryNew())
mel.delete_all_material_expressions(base)
bc = texture_parameter(base, 'BaseColor', unreal.MaterialSamplerType.SAMPLERTYPE_COLOR, textures.get('T_Trim_Wood_BC'), -600, -200)
nrm = texture_parameter(base, 'Normal', unreal.MaterialSamplerType.SAMPLERTYPE_NORMAL, textures.get('T_Trim_Wood_N'), -600, 100)
orm = texture_parameter(base, 'ORM', unreal.MaterialSamplerType.SAMPLERTYPE_MASKS, textures.get('T_Trim_Wood_ORM'), -600, 400)
mel.connect_material_property(bc, 'RGB', unreal.MaterialProperty.MP_BASE_COLOR)
mel.connect_material_property(nrm, 'RGB', unreal.MaterialProperty.MP_NORMAL)
mel.connect_material_property(orm, 'R', unreal.MaterialProperty.MP_AMBIENT_OCCLUSION)
mel.connect_material_property(orm, 'G', unreal.MaterialProperty.MP_ROUGHNESS)
mel.connect_material_property(orm, 'B', unreal.MaterialProperty.MP_METALLIC)
mel.recompile_material(base)

instances = {
    'M_Trim_Wood': ('T_Trim_Wood_BC', 'T_Trim_Wood_N', 'T_Trim_Wood_ORM'),
    # Turf has no ORM map of its own; the wood ORM is close enough for a thin roof edge.
    'M_Turf': ('T_Turf_BC', 'T_Turf_N', 'T_Trim_Wood_ORM'),
    'M_Item_Log': ('T_Item_Log_BC', 'T_Item_Log_N', 'T_Item_Log_ORM'),
}
materials = {}
for name, (bc_name, n_name, orm_name) in instances.items():
    mi = create_or_load(name, MATERIALS, unreal.MaterialInstanceConstant, unreal.MaterialInstanceConstantFactoryNew())
    mel.set_material_instance_parent(mi, base)
    for parameter, tex_name in (('BaseColor', bc_name), ('Normal', n_name), ('ORM', orm_name)):
        if tex_name in textures:
            mel.set_material_instance_texture_parameter_value(mi, parameter, textures[tex_name])
        else:
            fail('missing texture %s for %s' % (tex_name, name))
    materials[name] = mi
log('materials: %d instances' % len(materials))

# --- Build preview material (translucent, unlit) --------------------------------------------

preview = create_or_load('M_BuildPreview', MATERIALS, unreal.Material, unreal.MaterialFactoryNew())
mel.delete_all_material_expressions(preview)
preview.set_editor_property('blend_mode', unreal.BlendMode.BLEND_TRANSLUCENT)
preview.set_editor_property('shading_model', unreal.MaterialShadingModel.MSM_UNLIT)
preview.set_editor_property('two_sided', True)
color = mel.create_material_expression(preview, unreal.MaterialExpressionVectorParameter, -500, 0)
color.set_editor_property('parameter_name', 'Color')
color.set_editor_property('default_value', unreal.LinearColor(0.3, 0.8, 1.0, 1.0))
opacity = mel.create_material_expression(preview, unreal.MaterialExpressionScalarParameter, -500, 250)
opacity.set_editor_property('parameter_name', 'Opacity')
opacity.set_editor_property('default_value', 0.35)
mel.connect_material_property(color, 'RGB', unreal.MaterialProperty.MP_EMISSIVE_COLOR)
mel.connect_material_property(opacity, '', unreal.MaterialProperty.MP_OPACITY)
mel.recompile_material(preview)

# --- Meshes ---------------------------------------------------------------------------------

# Legacy FBX importer, so the options below are honoured (Interchange would create its own materials).
unreal.SystemLibrary.execute_console_command(None, 'Interchange.FeatureFlags.Import.FBX 0')

meshes = 0
for entry in manifest['meshes']:
    name = entry.get('unreal_name', entry['id'])
    destination = ROOT + '/' + entry['group']
    options = unreal.FbxImportUI()
    options.set_editor_property('import_mesh', True)
    options.set_editor_property('import_as_skeletal', False)
    options.set_editor_property('import_materials', False)
    options.set_editor_property('import_textures', False)
    options.set_editor_property('import_animations', False)
    options.set_editor_property('mesh_type_to_import', unreal.FBXImportType.FBXIT_STATIC_MESH)
    static = options.get_editor_property('static_mesh_import_data')
    static.set_editor_property('combine_meshes', True)
    static.set_editor_property('auto_generate_collision', False)
    static.set_editor_property('import_uniform_scale', 1.0)
    mesh = import_file(os.path.join(ASSETS, entry['file']), destination, name, options)
    if mesh is None:
        continue

    # Material slots by name from the manifest/FBX (M_Trim_Wood, M_Turf, M_Item_Log).
    static_materials = mesh.get_editor_property('static_materials')
    for index, slot in enumerate(static_materials):
        slot_name = str(slot.get_editor_property('material_slot_name'))
        target = next((mi for key, mi in materials.items() if slot_name.startswith(key)), None)
        if target is None and len(entry.get('materials', [])) > index:
            target = materials.get(entry['materials'][index])
        if target is None:
            fail('%s: no material for slot %s' % (name, slot_name))
            continue
        mesh.set_material(index, target)

    # Collision from the render mesh: exact for walls with door openings, cheap enough for these pieces.
    body = mesh.get_editor_property('body_setup')
    if body is not None:
        body.set_editor_property('collision_trace_flag', unreal.CollisionTraceFlag.CTF_USE_COMPLEX_AS_SIMPLE)
    meshes += 1

unreal.SystemLibrary.execute_console_command(None, 'Interchange.FeatureFlags.Import.FBX 1')
log('meshes: %d of %d' % (meshes, len(manifest['meshes'])))

# Remove anything a fallback importer may have created next to the meshes (materials, textures).
for folder in sorted({ROOT + '/' + e['group'] for e in manifest['meshes']}):
    for path in editor_assets.list_assets(folder, recursive=False, include_folder=False):
        data = editor_assets.find_asset_data(path)
        if str(data.asset_class_path.asset_name) != 'StaticMesh':
            editor_assets.delete_asset(path)
            log('removed stray asset ' + path)

if not editor_assets.save_directory(ROOT, only_if_is_dirty=False, recursive=True):
    fail('saving ' + ROOT + ' failed')

log('done, %d error(s)' % len(errors))
if errors:
    raise RuntimeError('GAMER_IMPORT failed: ' + '; '.join(errors))
