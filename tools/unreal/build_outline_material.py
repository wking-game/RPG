"""
Builds /Game/Gamer/Materials/M_Outline: a post-process material that draws coloured contours around everything
that was rendered into the custom stencil. Stencil 1 = the player's aim target (warm Herdfeuer contour), stencil 2 = a magic
target revealed by the scan (soft turquoise silhouette with a glow, strength from the player's magic experience).
Both are set on the local machine only (AGamerCharacter::UpdateOutline, UGamerScanComponent), so nobody else sees them.
Decisions 2026-11-15: the highlight of the aimed thing is its silhouette; magic in the scan is about the silhouette, faint at first.
  UnrealEditor-Cmd.exe Gamer.uproject -run=pythonscript -script=tools/unreal/build_outline_material.py -nullrhi -unattended
Repeatable (rebuilds the graph). Parameters: OutlineColor, Thickness, Opacity; MagicColor, MagicThickness, MagicOpacity.
"""
import unreal

mel = unreal.MaterialEditingLibrary
asset_tools = unreal.AssetToolsHelpers.get_asset_tools()
editor_assets = unreal.get_editor_subsystem(unreal.EditorAssetSubsystem)
PATH = '/Game/Gamer/Materials'
NAME = 'M_Outline'
errors = []

HLSL_TEMPLATE = r'''
float2 UV = GetDefaultSceneTextureUV(Parameters, __SCENE__);
float3 Scene = SceneIn.rgb;
float2 Px = ResolvedView.ViewSizeAndInvSize.zw;
float Center = SceneTextureLookup(UV, __STENCIL__, false).x;      // custom stencil of this pixel
float NearNormal = 0.0;
float NearMagic = 0.0;
float NearGlow = 0.0;
for (int ring = 1; ring <= 2; ++ring)
{
    for (int i = 0; i < 8; ++i)
    {
        float a = 6.2831853 * i / 8.0;
        float2 dir = float2(cos(a), sin(a));
        float sn = SceneTextureLookup(UV + dir * (Thickness * ring * 0.5) * Px, __STENCIL__, false).x;
        float sm = SceneTextureLookup(UV + dir * (MagicThickness * ring * 0.5) * Px, __STENCIL__, false).x;
        if (sn > 0.0001 && sn < 1.5) { NearNormal = 1.0; }
        if (sm > 1.5) { NearMagic = 1.0; }
    }
}
// Soft glow around magic silhouettes: wider rings, weaker
for (int glow = 3; glow <= 5; ++glow)
{
    for (int j = 0; j < 8; ++j)
    {
        float b = 6.2831853 * j / 8.0;
        float sg = SceneTextureLookup(UV + float2(cos(b), sin(b)) * (MagicThickness * glow * 0.7) * Px, __STENCIL__, false).x;
        if (sg > 1.5) { NearGlow = max(NearGlow, 1.0 - (glow - 3) * 0.3); }
    }
}
bool Outside = Center < 0.0001;
float EdgeNormal = (Outside && NearNormal > 0.0) ? 1.0 : 0.0;
float EdgeMagic = (Outside && NearMagic > 0.0) ? 1.0 : 0.0;
float GlowMagic = (Outside && NearMagic < 0.5) ? NearGlow * 0.45 : 0.0;
float3 Result = lerp(Scene, OutlineColor.rgb, EdgeNormal * Opacity);
Result = lerp(Result, MagicColor.rgb, saturate(EdgeMagic + GlowMagic) * MagicOpacity);
return Result;
'''
SCENE_ID = int(unreal.SceneTextureId.PPI_POST_PROCESS_INPUT0.value)
STENCIL_ID = int(unreal.SceneTextureId.PPI_CUSTOM_STENCIL.value)
HLSL = HLSL_TEMPLATE.replace('__SCENE__', str(SCENE_ID)).replace('__STENCIL__', str(STENCIL_ID))


def expression(material, cls, x, y, **properties):
    node = mel.create_material_expression(material, cls, x, y)
    for key, value in properties.items():
        node.set_editor_property(key, value)
    return node


full = PATH + '/' + NAME
material = unreal.load_asset(full) if editor_assets.does_asset_exist(full) else \
    asset_tools.create_asset(NAME, PATH, unreal.Material, unreal.MaterialFactoryNew())
mel.delete_all_material_expressions(material)
material.set_editor_property('material_domain', unreal.MaterialDomain.MD_POST_PROCESS)
material.set_editor_property('blendable_location', unreal.BlendableLocation.BL_SCENE_COLOR_AFTER_TONEMAPPING)

input_names = ('OutlineColor', 'Thickness', 'Opacity', 'MagicColor', 'MagicThickness', 'MagicOpacity', 'SceneIn', 'StencilIn')
inputs = []
for input_name in input_names:
    custom_input = unreal.CustomInput()
    custom_input.set_editor_property('input_name', input_name)
    inputs.append(custom_input)
custom = expression(material, unreal.MaterialExpressionCustom, -300, 0, code=HLSL,
                    output_type=unreal.CustomMaterialOutputType.CMOT_FLOAT3,
                    description='Contours around the custom-stencil silhouettes', inputs=inputs)
y = -100
for name, kind, default in (
        ('OutlineColor', 'vec', unreal.LinearColor(0.80, 0.37, 0.10, 1.0)),      # Herdfeuer #DB9345, slightly brightened
        ('Thickness', 'scalar', 2.5),
        ('Opacity', 'scalar', 1.0),
        ('MagicColor', 'vec', unreal.LinearColor(0.405, 0.82, 0.723, 1.0)),        # Gezeitenlicht #A9E9DC
        ('MagicThickness', 'scalar', 2.0),
        ('MagicOpacity', 'scalar', 0.0)):                                          # 0 = invisible; the scan raises it
    if kind == 'vec':
        node = expression(material, unreal.MaterialExpressionVectorParameter, -700, y, parameter_name=name, default_value=default)
    else:
        node = expression(material, unreal.MaterialExpressionScalarParameter, -700, y, parameter_name=name, default_value=default)
    if not mel.connect_material_expressions(node, '', custom, name):
        errors.append('link ' + name)
    y += 110
# Scene texture nodes: they make the engine declare SceneTextureLookup and bind the scene colour and the custom stencil.
scene_node = expression(material, unreal.MaterialExpressionSceneTexture, -700, 600,
                        scene_texture_id=unreal.SceneTextureId.PPI_POST_PROCESS_INPUT0)
stencil_node = expression(material, unreal.MaterialExpressionSceneTexture, -700, 750,
                          scene_texture_id=unreal.SceneTextureId.PPI_CUSTOM_STENCIL)
if not mel.connect_material_expressions(scene_node, 'Color', custom, 'SceneIn'):
    errors.append('link SceneIn')
if not mel.connect_material_expressions(stencil_node, 'Color', custom, 'StencilIn'):
    errors.append('link StencilIn')
if not mel.connect_material_property(custom, '', unreal.MaterialProperty.MP_EMISSIVE_COLOR):
    errors.append('link emissive')
mel.recompile_material(material)
editor_assets.save_loaded_asset(material)
if errors:
    unreal.log_error('GAMER_OUTLINE: failed: ' + '; '.join(errors))
else:
    unreal.log('GAMER_OUTLINE: M_Outline built, 0 error(s)')
