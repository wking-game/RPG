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
  /Game/Gamer/Nature/...            AI 3D plants and rocks from Assets/manifest_ki3d.json (tools/blender/process_ki3d.py),
                                    each with a material instance M_Nature_<Name> of M_Gamer_Base
  /Game/Gamer/Buildings/StyleTest/... style test buildings from Assets/manifest_buildings.json
                                    (tools/blender/restyle_buildings.py); restyled surfaces are instances of
                                    M_Gamer_Building (moss, damp, weathering), the originals of M_Gamer_Base
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

# AI 3D models (plants, rocks): own manifest, because build_assets.py rewrites manifest.json.
ki3d_path = os.path.join(ASSETS, 'manifest_ki3d.json')
ki3d = {'textures': [], 'materials': [], 'meshes': []}
if os.path.exists(ki3d_path):
    with open(ki3d_path, encoding='utf-8') as handle:
        ki3d.update(json.load(handle))
# CC0 photoscans and generated trees (tools/blender/process_nature.py, build_trees.py).
nature_path = os.path.join(ASSETS, 'manifest_nature.json')
nature = {'textures': [], 'materials': [], 'meshes': []}
if os.path.exists(nature_path):
    with open(nature_path, encoding='utf-8') as handle:
        nature.update(json.load(handle))

# Style test buildings (tools/blender/restyle_buildings.py): textures carry their own folder, materials a parent
# ('building' = M_Gamer_Building with moss/damp/weathering parameters, 'base' = M_Gamer_Base).
buildings_path = os.path.join(ASSETS, 'manifest_buildings.json')
buildings = {'textures': [], 'materials': [], 'meshes': []}
if os.path.exists(buildings_path):
    with open(buildings_path, encoding='utf-8') as handle:
        buildings.update(json.load(handle))

textures = {}
for entry, folder in ([(e, ROOT + '/Textures') for e in manifest['textures']]
                      + [(e, ROOT + '/Nature/Textures') for e in ki3d['textures'] + nature['textures']]
                      + [(e, e.get('folder', ROOT + '/Buildings/Textures')) for e in buildings['textures']]):
    name = entry['name']
    tex = import_file(os.path.join(ASSETS, entry['file']), folder, name)
    if tex is None:
        continue
    tex.set_editor_property('srgb', bool(entry['srgb']))
    if name.endswith('_N'):
        tex.set_editor_property('compression_settings', unreal.TextureCompressionSettings.TC_NORMALMAP)
    elif name.endswith('_ORM'):
        tex.set_editor_property('compression_settings', unreal.TextureCompressionSettings.TC_MASKS)
    textures[name] = tex
log('textures: %d' % len(textures))

icons_manifest = os.path.join(ASSETS, 'manifest_icons.json')   # rendered by tools/blender/render_icons.py
extra_icons = json.load(open(icons_manifest, encoding='utf-8'))['icons'] if os.path.exists(icons_manifest) else []
for entry in manifest.get('icons', []) + extra_icons:
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
# PCG scatters rocks and branches as instanced meshes: without this flag the game shows the default material instead.
base.set_editor_property('used_with_instanced_static_meshes', True)
bc = texture_parameter(base, 'BaseColor', unreal.MaterialSamplerType.SAMPLERTYPE_COLOR, textures.get('T_Trim_Wood_BC'), -600, -200)
nrm = texture_parameter(base, 'Normal', unreal.MaterialSamplerType.SAMPLERTYPE_NORMAL, textures.get('T_Trim_Wood_N'), -600, 100)
orm = texture_parameter(base, 'ORM', unreal.MaterialSamplerType.SAMPLERTYPE_MASKS, textures.get('T_Trim_Wood_ORM'), -600, 400)
mel.connect_material_property(bc, 'RGB', unreal.MaterialProperty.MP_BASE_COLOR)
mel.connect_material_property(nrm, 'RGB', unreal.MaterialProperty.MP_NORMAL)
mel.connect_material_property(orm, 'R', unreal.MaterialProperty.MP_AMBIENT_OCCLUSION)
mel.connect_material_property(orm, 'G', unreal.MaterialProperty.MP_ROUGHNESS)
mel.connect_material_property(orm, 'B', unreal.MaterialProperty.MP_METALLIC)
mel.recompile_material(base)

# --- Seasons: Material Parameter Collection -------------------------------------------------
# MPC_GamerWorld.SeasonPhase (0..4, cyclic: 0 spring, 1 summer, 2 autumn, 3 winter, 4 = 0) is set by the game at
# runtime (C++). Name and path are fixed. The parameter is only added if missing: its id must stay the same,
# materials reference it by id. The default value is reset to summer on every import.
SEASON_DEFAULT = 1.3
mpc = create_or_load('MPC_GamerWorld', MATERIALS, unreal.MaterialParameterCollection,
                     unreal.MaterialParameterCollectionFactoryNew())
mpc_scalars = list(mpc.get_editor_property('scalar_parameters'))
season_entry = next((p for p in mpc_scalars if str(p.get_editor_property('parameter_name')) == 'SeasonPhase'), None)
if season_entry is None:
    season_entry = unreal.CollectionScalarParameter()
    season_entry.set_editor_property('parameter_name', 'SeasonPhase')
    mpc_scalars.append(season_entry)
season_entry.set_editor_property('default_value', SEASON_DEFAULT)
mpc.set_editor_property('scalar_parameters', mpc_scalars)
log('MPC_GamerWorld: %d scalar parameter(s), SeasonPhase default %.2f' % (len(mpc_scalars), SEASON_DEFAULT))

# Season behaviour of the leaf-card materials (parameters Evergreen, LeafDrop of M_Gamer_Foliage), by material name:
# first rule whose text is part of the name wins; masked materials without a match behave like grass.
# Kept here and not in the manifest: the Blender scripts rewrite their manifest entries.
SEASON_RULES = (
    ('Fern', 'deciduous'),       # ferns (e.g. fern_02): brown in autumn, gone in winter
    ('Shrub', 'grass'),          # low shrubs (shrub_02, shrub_04): colour change only, no leaf drop
    ('Needles', 'evergreen'),    # M_Nature_Needles...: conifers
    ('Leaves', 'deciduous'),     # M_Nature_Leaves...: deciduous trees and bushes
)
SEASON_PARAMETERS = {'grass': (0.0, 0.0), 'evergreen': (1.0, 0.0), 'deciduous': (0.0, 1.0)}   # (Evergreen, LeafDrop)


def season_kind(material_name):
    return next((kind for text, kind in SEASON_RULES if text.lower() in material_name.lower()), 'grass')


# Season colour and leaf mask (Custom node). Colours are linear RGB; see docs/09_vegetation_material.md.
# Recolouring keeps the luminance of each texel (texture detail stays), only the hue goes to the season colour.
FOLIAGE_SEASON_HLSL = r'''
// In: Tex (base colour RGBA), Phase (MPC SeasonPhase 0..4), Evergreen, LeafDrop (0/1), InstRand (PerInstanceRandom),
//     ObjPos (object position), WorldPos (pixel world position), positions in cm. Out: RGB base colour, A opacity mask.
#define G_H12(P, OUT) { float3 q_ = frac(float3((P).x, (P).y, (P).x) * 0.1031); q_ += dot(q_, q_.yzx + 33.33); OUT = frac((q_.x + q_.y) * q_.z); }
#define G_H13(P, OUT) { float3 q_ = frac((P) * 0.1031); q_ += dot(q_, q_.zyx + 31.32); OUT = frac((q_.x + q_.y) * q_.z); }
#define G_VN2(P, OUT) { float2 i_ = floor(P); float2 f_ = frac(P); f_ = f_ * f_ * (3.0 - 2.0 * f_); float a_, b_, c_, d_; G_H12(i_, a_); G_H12(i_ + float2(1.0, 0.0), b_); G_H12(i_ + float2(0.0, 1.0), c_); G_H12(i_ + float2(1.0, 1.0), d_); OUT = lerp(lerp(a_, b_, f_.x), lerp(c_, d_, f_.x), f_.y); }
#define G_VN3(P, OUT) { float3 i_ = floor(P); float3 f_ = frac(P); f_ = f_ * f_ * (3.0 - 2.0 * f_); float a_, b_, c_, d_, e_, g_, h_, k_; G_H13(i_, a_); G_H13(i_ + float3(1.0, 0.0, 0.0), b_); G_H13(i_ + float3(0.0, 1.0, 0.0), c_); G_H13(i_ + float3(1.0, 1.0, 0.0), d_); G_H13(i_ + float3(0.0, 0.0, 1.0), e_); G_H13(i_ + float3(1.0, 0.0, 1.0), g_); G_H13(i_ + float3(0.0, 1.0, 1.0), h_); G_H13(i_ + float3(1.0, 1.0, 1.0), k_); OUT = lerp(lerp(lerp(a_, b_, f_.x), lerp(c_, d_, f_.x), f_.y), lerp(lerp(e_, g_, f_.x), lerp(h_, k_, f_.x), f_.y), f_.z); }
#define G_KEY(P0, P1, T1, A1, B1) { float w_ = smoothstep(P0, P1, ph); kT = lerp(kT, T1, w_); kA = lerp(kA, A1, w_); kB = lerp(kB, B1, w_); }
#define G_WRAP(X) ((X) - 4.0 * floor((X) * 0.25))

const float3 LUMA = float3(0.2126, 0.7152, 0.0722);
const float3 C_BUD = float3(0.305, 0.485, 0.045);           // sRGB 150 185 60  leaf-out, light yellow-green
const float3 C_FRESH = float3(0.262, 0.485, 0.026);         // sRGB 140 185 45  fresh spring green
const float3 C_SUMMER = float3(0.061, 0.223, 0.017);        // sRGB  70 130 35  deep summer green
const float3 C_OLIVE = float3(0.205, 0.242, 0.038);         // sRGB 125 135 55  late summer
const float3 C_DRY = float3(0.305, 0.262, 0.061);           // sRGB 150 140 70  dry patches
const float3 C_LUSH = float3(0.035, 0.162, 0.020);          // sRGB  52 112 40  moist patches
const float3 C_AUTGRASS = float3(0.352, 0.223, 0.045);      // sRGB 160 130 60  autumn grass
const float3 C_STRAW = float3(0.376, 0.283, 0.141);         // sRGB 165 145 105 winter grass (straw)
const float3 C_YELLOW = float3(0.680, 0.402, 0.021);        // sRGB 215 170 40  autumn leaves: golden
const float3 C_ORANGE = float3(0.610, 0.171, 0.013);        // sRGB 205 115 30  autumn leaves: orange
const float3 C_RUST = float3(0.305, 0.045, 0.013);          // sRGB 150  60 30  autumn leaves: red-brown
const float3 C_DEADLEAF = float3(0.262, 0.102, 0.026);      // sRGB 140  90 45  leaves about to fall
const float3 C_NEEDLEWINTER = float3(0.070, 0.084, 0.023);  // sRGB  75  82 42  conifers in winter (olive)

float3 tex = Tex.rgb;
// Random per object: PerInstanceRandom for instanced meshes (PCG), position hash for single actors (0 + hash).
float objHash; G_H13(ObjPos * 0.01, objHash);
float rnd = frac(InstRand + objHash);
float rnd2; G_H12(float2(rnd * 97.31, objHash * 41.17 + 3.7), rnd2);
// Large patches of about 15-40 m: 0 = lush, 1 = dry.
float pn1, pn2; G_VN2(WorldPos.xy / 2800.0, pn1); G_VN2(WorldPos.xy / 1100.0 + 17.3, pn2);
float area = saturate((pn1 * 0.7 + pn2 * 0.3 - 0.22) / 0.56);

float3 kT; float kA; float kB; float ph; float vis = 1.0;
if (LeafDrop > 0.5)
{
    // Deciduous: leaf-out in spring, autumn colours, leaves drop in late autumn (opacity mask), bare in winter.
    float n1, n2; G_VN3(WorldPos / 35.0, n1); G_VN3(WorldPos / 13.0 + 5.3, n2);
    float leafN = saturate((n1 * 0.7 + n2 * 0.3 - 0.2) / 0.6);
    float pObj = G_WRAP(Phase + (rnd - 0.5) * 0.18 + (area - 0.5) * 0.08);
    float keep = smoothstep(0.30, 0.62, pObj) * (1.0 - smoothstep(2.36, 2.86, pObj));
    vis = step(0.02 + 0.96 * leafN, keep);
    ph = G_WRAP(pObj + (leafN - 0.5) * 0.10);
    float3 aut = lerp(lerp(C_YELLOW, C_ORANGE, smoothstep(0.35, 0.60, rnd2)), C_RUST, smoothstep(0.72, 0.95, rnd2));
    float autB = lerp(1.35, 1.0, smoothstep(0.35, 0.95, rnd2));
    kT = C_BUD; kA = 0.70; kB = 1.20;
    G_KEY(0.00, 0.45, C_BUD, 0.65, 1.18);
    G_KEY(0.45, 0.85, C_FRESH, 0.35, 1.08);
    G_KEY(0.85, 1.15, C_SUMMER, 0.30, 0.92);
    G_KEY(1.15, 1.75, C_SUMMER, 0.28, 0.90);
    G_KEY(1.75, 2.00, C_OLIVE, 0.30, 0.92);
    G_KEY(2.00, 2.35, aut, 0.90, autB);
    G_KEY(2.35, 2.85, C_DEADLEAF, 0.92, 0.85);
    G_KEY(2.85, 4.00, C_BUD, 0.70, 1.20);
    kB *= 1.0 - (area - 0.5) * 0.08;
}
else if (Evergreen > 0.5)
{
    // Conifers: fresh shoots in late spring, a little darker and more olive in winter.
    ph = G_WRAP(Phase + (rnd - 0.5) * 0.12 + (area - 0.5) * 0.06);
    kT = C_NEEDLEWINTER; kA = 0.50; kB = 0.75;
    G_KEY(0.15, 0.50, C_SUMMER, 0.10, 0.95);
    G_KEY(0.50, 0.85, C_FRESH, 0.22, 1.05);
    G_KEY(0.85, 1.30, C_SUMMER, 0.12, 0.97);
    G_KEY(2.10, 2.70, C_OLIVE, 0.25, 0.88);
    G_KEY(2.70, 3.10, C_NEEDLEWINTER, 0.50, 0.75);
    kB *= 1.0 - (area - 0.5) * 0.08;
}
else
{
    // Grass and herbs: fresh light green, rich summer green, slightly drier late summer, yellow-brown, straw.
    ph = G_WRAP(Phase + (rnd - 0.5) * 0.10 + (area - 0.5) * 0.16);
    kT = C_STRAW; kA = 0.78; kB = 0.95;
    G_KEY(0.05, 0.40, C_FRESH, 0.60, 1.18);
    G_KEY(0.40, 0.85, C_FRESH, 0.35, 1.08);
    G_KEY(0.85, 1.15, C_SUMMER, 0.35, 0.92);
    G_KEY(1.50, 1.95, C_OLIVE, 0.45, 0.97);
    G_KEY(2.05, 2.50, C_AUTGRASS, 0.72, 1.03);
    G_KEY(2.60, 3.10, C_STRAW, 0.82, 1.00);
    G_KEY(3.10, 3.90, C_STRAW, 0.78, 0.95);
    float dry = smoothstep(0.55, 0.90, area);
    float lush = (1.0 - smoothstep(0.10, 0.45, area)) * (1.0 - smoothstep(0.30, 0.70, kA));
    kT = lerp(kT, C_DRY, dry * 0.5); kA = lerp(kA, max(kA, 0.55), dry);
    kT = lerp(kT, C_LUSH, lush * 0.5); kA = lerp(kA, max(kA, 0.40), lush); kB *= 1.0 - 0.10 * lush;
}
kB *= 1.0 + (rnd - 0.5) * 0.22;
float3 tint = lerp(float3(0.94, 1.0, 1.06), float3(1.08, 1.0, 0.90), rnd2);
float lum = dot(tex, LUMA);
float3 recol = lum * kT / max(dot(kT, LUMA), 0.0001);
float3 col = lerp(tex, recol, saturate(kA)) * kB * tint;
return float4(clamp(col, 0.0, 0.9), Tex.a * vis);
#undef G_H12
#undef G_H13
#undef G_VN2
#undef G_VN3
#undef G_KEY
#undef G_WRAP
'''


def expression(material, cls, x, y, **properties):
    node = mel.create_material_expression(material, cls, x, y)
    for key, value in properties.items():
        node.set_editor_property(key, value)
    return node


def link(source, output, target, target_input):
    if not mel.connect_material_expressions(source, output, target, target_input):
        fail('material link %s.%s -> %s.%s failed' % (source.get_name(), output, target.get_name(), target_input))


def link_property(source, output, prop):
    if not mel.connect_material_property(source, output, prop):
        fail('material link %s.%s -> %s failed' % (source.get_name(), output, prop))


# Leaf cards (leaves, needles, grass, nettle): masked by the alpha of the base colour texture, Two Sided Foliage.
# Low specular and roughness >= 0.6 (no white sky sheen on the up-bent grass normals); the normal is multiplied by
# TwoSidedSign, so back faces keep the front normal (the engine flips it, which made back faces dark).
foliage = create_or_load('M_Gamer_Foliage', MATERIALS, unreal.Material, unreal.MaterialFactoryNew())
mel.delete_all_material_expressions(foliage)
foliage.set_editor_property('blend_mode', unreal.BlendMode.BLEND_MASKED)
foliage.set_editor_property('two_sided', True)
foliage.set_editor_property('shading_model', unreal.MaterialShadingModel.MSM_TWO_SIDED_FOLIAGE)
foliage.set_editor_property('used_with_instanced_static_meshes', True)   # PCG grass is instanced
fbc = texture_parameter(foliage, 'BaseColor', unreal.MaterialSamplerType.SAMPLERTYPE_COLOR, textures.get('T_Trim_Wood_BC'), -600, -200)
fnrm = texture_parameter(foliage, 'Normal', unreal.MaterialSamplerType.SAMPLERTYPE_NORMAL, textures.get('T_Trim_Wood_N'), -600, 100)
form = texture_parameter(foliage, 'ORM', unreal.MaterialSamplerType.SAMPLERTYPE_MASKS, textures.get('T_Trim_Wood_ORM'), -600, 400)
season_node = expression(foliage, unreal.MaterialExpressionCollectionParameter, -600, -700, collection=mpc)
season_node.set_editor_property('parameter_name', 'SeasonPhase')   # after the collection: resolves the parameter id
evergreen_node = expression(foliage, unreal.MaterialExpressionScalarParameter, -600, -600, parameter_name='Evergreen', default_value=0.0)
leaf_drop_node = expression(foliage, unreal.MaterialExpressionScalarParameter, -600, -500, parameter_name='LeafDrop', default_value=0.0)
instance_random = expression(foliage, unreal.MaterialExpressionPerInstanceRandom, -600, -400)
object_position = expression(foliage, unreal.MaterialExpressionObjectPositionWS, -600, -340)
world_position = expression(foliage, unreal.MaterialExpressionWorldPosition, -600, -280)
season_inputs = []
for input_name in ('Tex', 'Phase', 'Evergreen', 'LeafDrop', 'InstRand', 'ObjPos', 'WorldPos'):
    custom_input = unreal.CustomInput()
    custom_input.set_editor_property('input_name', input_name)
    season_inputs.append(custom_input)
season_custom = expression(foliage, unreal.MaterialExpressionCustom, -250, -400, code=FOLIAGE_SEASON_HLSL,
                           output_type=unreal.CustomMaterialOutputType.CMOT_FLOAT4,
                           description='Season colour (RGB) and leaf mask (A)', inputs=season_inputs)
link(fbc, 'RGBA', season_custom, 'Tex')
link(season_node, '', season_custom, 'Phase')
link(evergreen_node, '', season_custom, 'Evergreen')
link(leaf_drop_node, '', season_custom, 'LeafDrop')
link(instance_random, '', season_custom, 'InstRand')
link(object_position, '', season_custom, 'ObjPos')
link(world_position, '', season_custom, 'WorldPos')
season_rgb = expression(foliage, unreal.MaterialExpressionComponentMask, 0, -450, r=True, g=True, b=True, a=False)
season_mask = expression(foliage, unreal.MaterialExpressionComponentMask, 0, -300, r=False, g=False, b=False, a=True)
link(season_custom, '', season_rgb, '')
link(season_custom, '', season_mask, '')
subsurface = expression(foliage, unreal.MaterialExpressionMultiply, 200, -350, const_b=0.4)   # subdued transmission
link(season_rgb, '', subsurface, 'A')
specular = expression(foliage, unreal.MaterialExpressionConstant, 200, -150, r=0.25)
roughness = expression(foliage, unreal.MaterialExpressionLinearInterpolate, 0, 450, const_a=0.6, const_b=0.95)
link(form, 'G', roughness, 'Alpha')
two_sided_sign = expression(foliage, unreal.MaterialExpressionTwoSidedSign, -300, 250)
normal = expression(foliage, unreal.MaterialExpressionMultiply, 0, 150)
link(fnrm, 'RGB', normal, 'A')
link(two_sided_sign, '', normal, 'B')
link_property(season_rgb, '', unreal.MaterialProperty.MP_BASE_COLOR)
link_property(season_mask, '', unreal.MaterialProperty.MP_OPACITY_MASK)
link_property(subsurface, '', unreal.MaterialProperty.MP_SUBSURFACE_COLOR)
link_property(specular, '', unreal.MaterialProperty.MP_SPECULAR)
link_property(roughness, '', unreal.MaterialProperty.MP_ROUGHNESS)
link_property(normal, '', unreal.MaterialProperty.MP_NORMAL)
link_property(form, 'R', unreal.MaterialProperty.MP_AMBIENT_OCCLUSION)
mel.recompile_material(foliage)

# Buildings (style test 2026-11-14): one shared material adds moss, damp and weathering, so the models stay clean
# (design/stil_gelebter_norden.md: "Abnutzung und Moos über ein gemeinsames Material"). Masks from the height above
# the object pivot (= ground), the vertex normal and world-space noise; parameters per material instance.
BUILDING_HLSL = r'''
// In: Tex (base colour), Orm (R AO, G roughness), WorldPos, ObjPos (actor pivot, cm), Nrm (vertex normal, world),
//     MossAmount, MossHeight, UpMoss, DampHeight, Weathering. Out: RGB base colour, A roughness.
#define B_H13(P, OUT) { float3 q_ = frac((P) * 0.1031); q_ += dot(q_, q_.zyx + 31.32); OUT = frac((q_.x + q_.y) * q_.z); }
#define B_VN3(P, OUT) { float3 i_ = floor(P); float3 f_ = frac(P); f_ = f_ * f_ * (3.0 - 2.0 * f_); float a_, b_, c_, d_, e_, g_, h_, k_; B_H13(i_, a_); B_H13(i_ + float3(1.0, 0.0, 0.0), b_); B_H13(i_ + float3(0.0, 1.0, 0.0), c_); B_H13(i_ + float3(1.0, 1.0, 0.0), d_); B_H13(i_ + float3(0.0, 0.0, 1.0), e_); B_H13(i_ + float3(1.0, 0.0, 1.0), g_); B_H13(i_ + float3(0.0, 1.0, 1.0), h_); B_H13(i_ + float3(1.0, 1.0, 1.0), k_); OUT = lerp(lerp(lerp(a_, b_, f_.x), lerp(c_, d_, f_.x), f_.y), lerp(lerp(e_, g_, f_.x), lerp(h_, k_, f_.x), f_.y), f_.z); }
const float3 LUMA = float3(0.2126, 0.7152, 0.0722);
const float3 MOSS_DARK = float3(0.030, 0.045, 0.010);    // sRGB  48  60 25
const float3 MOSS_LIGHT = float3(0.105, 0.135, 0.024);   // sRGB  92 104 42
float3 c = Tex.rgb;
float rough = Orm.g;
float h = WorldPos.z - ObjPos.z;
float n1, n2, n3; B_VN3(WorldPos / 90.0, n1); B_VN3(WorldPos / 23.0 + 7.1, n2); B_VN3(WorldPos / 5.5 + 3.3, n3);
float patch = n1 * 0.48 + n2 * 0.30 + n3 * 0.22;
float fiber = saturate(abs(Tex.r - Tex.g) * 5.0 + (1.0 - Tex.b) * 0.22);
float micro = smoothstep(0.48, 0.68, frac(n3 * 3.71 + n2 * 1.93));
// Damp band at the bottom: darker, a little less rough.
float damp = 1.0 - smoothstep(0.0, max(DampHeight, 1.0), h - (patch - 0.5) * DampHeight * 0.8);
c *= lerp(1.0, 0.6, damp * 0.85);
rough = lerp(rough, rough * 0.85, damp);
// Weathering: wood greys out, most on surfaces facing up.
float lum = dot(c, LUMA);
float w = saturate(Weathering * (0.6 + 0.4 * Nrm.z + (n2 - 0.5) * 0.8));
c = lerp(c, lum * float3(0.93, 0.96, 1.0) * 1.08, w);
// Moss: near the ground and on upward faces, in patches, more in cavities (low AO).
float bottom = 1.0 - smoothstep(0.0, max(MossHeight, 1.0), h - (patch - 0.5) * MossHeight * 0.9);
float up = saturate((Nrm.z - 0.25) / 0.6) * UpMoss;
float mask = saturate(max(bottom, up) * MossAmount) * (1.25 - 0.35 * Orm.r);
float mossField = mask * (0.20 + patch * 0.72 + fiber * 0.22);
float moss = smoothstep(0.57, 0.76, mossField) * lerp(0.72, 1.0, micro);
float3 mossCol = lerp(MOSS_DARK, MOSS_LIGHT, saturate(patch * 0.62 + fiber * 0.30)) * (0.75 + 1.5 * lum);
c = lerp(c, mossCol, moss);
rough = lerp(rough, 0.95, moss);
return float4(c, saturate(rough));
#undef B_H13
#undef B_VN3
'''
building = create_or_load('M_Gamer_Building', MATERIALS, unreal.Material, unreal.MaterialFactoryNew())
mel.delete_all_material_expressions(building)
# Drawn fields lay their soil tile as instances (AGamerFarmBuilding::SoilTiles): without this flag they show the default material.
building.set_editor_property('used_with_instanced_static_meshes', True)
bbc =texture_parameter(building, 'BaseColor', unreal.MaterialSamplerType.SAMPLERTYPE_COLOR, textures.get('T_Trim_Wood_BC'), -900, -200)
bnrm = texture_parameter(building, 'Normal', unreal.MaterialSamplerType.SAMPLERTYPE_NORMAL, textures.get('T_Trim_Wood_N'), -900, 100)
borm = texture_parameter(building, 'ORM', unreal.MaterialSamplerType.SAMPLERTYPE_MASKS, textures.get('T_Trim_Wood_ORM'), -900, 400)
building_inputs = []
building_custom_names = ('Tex', 'Orm', 'WorldPos', 'ObjPos', 'Nrm', 'MossAmount', 'MossHeight', 'UpMoss', 'DampHeight', 'Weathering')
for input_name in building_custom_names:
    custom_input = unreal.CustomInput()
    custom_input.set_editor_property('input_name', input_name)
    building_inputs.append(custom_input)
building_custom = expression(building, unreal.MaterialExpressionCustom, -350, -200, code=BUILDING_HLSL,
                             output_type=unreal.CustomMaterialOutputType.CMOT_FLOAT4,
                             description='Moss, damp, weathering (RGB) and roughness (A)', inputs=building_inputs)
link(bbc, 'RGB', building_custom, 'Tex')
link(borm, 'RGB', building_custom, 'Orm')
link(expression(building, unreal.MaterialExpressionWorldPosition, -900, -500), '', building_custom, 'WorldPos')
# Actor position = pivot at the ground (ObjectPositionWS would be the centre of the bounds).
link(expression(building, unreal.MaterialExpressionActorPositionWS, -900, -560), '', building_custom, 'ObjPos')
link(expression(building, unreal.MaterialExpressionVertexNormalWS, -900, -620), '', building_custom, 'Nrm')
for index, (parameter, default) in enumerate((('MossAmount', 0.5), ('MossHeight', 60.0), ('UpMoss', 0.3),
                                              ('DampHeight', 50.0), ('Weathering', 0.2))):
    node = expression(building, unreal.MaterialExpressionScalarParameter, -650, -700 + index * 70,
                      parameter_name=parameter, default_value=default)
    link(node, '', building_custom, parameter)
building_rgb = expression(building, unreal.MaterialExpressionComponentMask, 0, -250, r=True, g=True, b=True, a=False)
building_rough = expression(building, unreal.MaterialExpressionComponentMask, 0, -100, r=False, g=False, b=False, a=True)
link(building_custom, '', building_rgb, '')
link(building_custom, '', building_rough, '')
link_property(building_rgb, '', unreal.MaterialProperty.MP_BASE_COLOR)
link_property(building_rough, '', unreal.MaterialProperty.MP_ROUGHNESS)
link_property(bnrm, 'RGB', unreal.MaterialProperty.MP_NORMAL)
link_property(borm, 'R', unreal.MaterialProperty.MP_AMBIENT_OCCLUSION)
link_property(borm, 'B', unreal.MaterialProperty.MP_METALLIC)
mel.recompile_material(building)
building_materials = {}   # name -> scalar parameters

instances = {
    'M_Trim_Wood': ('T_Trim_Wood_BC', 'T_Trim_Wood_N', 'T_Trim_Wood_ORM'),
    # Turf has no ORM map of its own; the wood ORM is close enough for a thin roof edge.
    'M_Turf': ('T_Turf_BC', 'T_Turf_N', 'T_Trim_Wood_ORM'),
    'M_Item_Log': ('T_Item_Log_BC', 'T_Item_Log_N', 'T_Item_Log_ORM'),
}
masked_materials = set()
for entry in ki3d['materials'] + nature['materials']:
    instances[entry['name']] = (entry['BaseColor'], entry['Normal'], entry['ORM'])
    if entry.get('masked'):
        masked_materials.add(entry['name'])
for entry in buildings['materials']:
    instances[entry['name']] = (entry['BaseColor'], entry['Normal'], entry['ORM'])
    if entry.get('parent') == 'building':
        building_materials[entry['name']] = entry.get('params', {})
materials = {}
for name, (bc_name, n_name, orm_name) in instances.items():
    mi = create_or_load(name, MATERIALS, unreal.MaterialInstanceConstant, unreal.MaterialInstanceConstantFactoryNew())
    mel.set_material_instance_parent(mi, foliage if name in masked_materials
                                     else building if name in building_materials else base)
    for parameter, value in building_materials.get(name, {}).items():
        mel.set_material_instance_scalar_parameter_value(mi, parameter, float(value))
    for parameter, tex_name in (('BaseColor', bc_name), ('Normal', n_name), ('ORM', orm_name)):
        if tex_name in textures:
            mel.set_material_instance_texture_parameter_value(mi, parameter, textures[tex_name])
        else:
            fail('missing texture %s for %s' % (tex_name, name))
    if name in masked_materials:
        kind = season_kind(name)
        for parameter, value in zip(('Evergreen', 'LeafDrop'), SEASON_PARAMETERS[kind]):
            mel.set_material_instance_scalar_parameter_value(mi, parameter, value)
        log('season: %s = %s' % (name, kind))
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
all_meshes = manifest['meshes'] + ki3d['meshes'] + nature['meshes'] + buildings['meshes']
for entry in all_meshes:
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
        # Longest matching name first (M_Nature_TreeApple_Picked must not take M_Nature_TreeApple).
        target = next((materials[key] for key in sorted(materials, key=len, reverse=True) if slot_name.startswith(key)), None)
        if target is None and len(entry.get('materials', [])) > index:
            target = materials.get(entry['materials'][index])
        if target is None:
            fail('%s: no material for slot %s' % (name, slot_name))
            continue
        mesh.set_material(index, target)

    # Collision from the render mesh: exact for walls with door openings, cheap enough for these pieces.
    # Generated trees and shrubs bring their own convex hulls (UCX_ objects in the FBX): trunk or bush only.
    # 'none' (grass, ground cover): no simple shapes (none in the FBX, none generated) and simple-as-complex,
    # so the render mesh is not used for traces either: nothing collides.
    body = mesh.get_editor_property('body_setup')
    if body is not None:
        collision = entry.get('collision')
        body.set_editor_property('collision_trace_flag',
                                 unreal.CollisionTraceFlag.CTF_USE_DEFAULT if collision == 'ucx'
                                 else unreal.CollisionTraceFlag.CTF_USE_SIMPLE_AS_COMPLEX if collision == 'none'
                                 else unreal.CollisionTraceFlag.CTF_USE_COMPLEX_AS_SIMPLE)
    meshes += 1

unreal.SystemLibrary.execute_console_command(None, 'Interchange.FeatureFlags.Import.FBX 1')
log('meshes: %d of %d' % (meshes, len(all_meshes)))

# Remove anything a fallback importer may have created next to the meshes (materials, textures).
for folder in sorted({ROOT + '/' + e['group'] for e in all_meshes}):
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
