"""Ground material /Game/Gamer/Materials/M_Gamer_Ground for the flat floor of the world map (until a landscape exists).

Called from tools/unreal/build_world.py (build()); can also run alone as a Python commandlet.
Needs the ground textures T_Ground<Layer>_BC/_N/_ORM (ORM alpha = height), built by tools/blender/process_nature.py
("ground" in tools/cc0/nature.json) and imported by tools/import_assets.ps1 into /Game/Gamer/Nature/Textures.

How it looks (all world-aligned on X/Y, no UVs):
  - five layers: forest floor with pine needles, sand with needles (together the base), light sand patches, moss patches,
    earthy dirt under the grass meadows;
  - layer masks come from large value-noise fields (FIELDS below). The field code is the same as the PCG node
    "Spatial Noise" (Perlin2D) with RandomOffset 0, so build_world.py can place ferns on the moss and grass on the
    meadows with exactly the same fields;
  - transitions by height blending (height maps in ORM alpha);
  - against visible tiling: every main layer is sampled at two scales (second one rotated, mixed more with distance)
    and the colour gets a large brightness/hue variation;
  - seasons: MPC_GamerWorld.SeasonPhase (0 spring, 1 summer, 2 autumn, 3 winter, cyclic 0..4) if the collection exists:
    autumn browner/more leaves, winter paler. Without the collection the material is built with summer.
"""
import os

import unreal

MATERIAL_DIR = '/Game/Gamer/Materials'
MATERIAL_NAME = 'M_Gamer_Ground'
TEXTURE_DIR = '/Game/Gamer/Nature/Textures'
MPC_PATH = '/Game/Gamer/Materials/MPC_GamerWorld'

# Layers: (texture base name, tile size in cm, sampled a second time at a larger rotated scale)
LAYERS = [
    ('GroundForest', 220.0, True),      # A: forest floor, pine needle litter (base)
    ('GroundNeedleSand', 260.0, True),  # B: sand with some needles (base)
    ('GroundSand', 320.0, True),        # C: light sand patches
    ('GroundMoss', 240.0, True),        # D: moss and leaves
    ('GroundDirt', 260.0, False),       # E: earth under the grass meadows
]
FAR_SCALE = 3.7   # second sample: tile x 3.7, rotated by about 37 degrees

# Noise fields, identical in material and PCG: value = Perlin2D(q), q = (P * scale + offset) * 0.0001, P in cm.
# (scale, offset x, offset y, iterations); one noise cell = 10000 / scale cm.
FIELDS = {
    'moss': (5.0, 21000.0, 7000.0, 3),       # 20 m cells; ferns grow where moss > MOSS_THRESHOLD
    'grass': (4.0, -13000.0, 26000.0, 3),    # 25 m cells; grass meadows where grass > GRASS_THRESHOLD
    'sand': (3.2, 9000.0, -17000.0, 3),      # light sand patches
    'needles': (2.4, -5000.0, -11000.0, 3),  # forest floor <-> needle sand
    'macro': (1.1, 3000.0, 15000.0, 2),      # colour variation
    'detail': (14.0, 7000.0, 4000.0, 2),     # 7 m cells: brightness variation against tiling seen from above
}
MOSS_THRESHOLD = 0.56
GRASS_THRESHOLD = 0.5
SAND_THRESHOLD = 0.66

errors = []


def log(msg):
    unreal.log('GAMER_GROUND: ' + msg)


def error(msg):
    errors.append(msg)
    unreal.log_error('GAMER_GROUND: ERROR ' + msg)


# Value noise of the PCG Spatial Noise node (PCGSpatialNoise.cpp: ValueHash, Noise2D, CalcPerlin2D).
# frac(c * 0.3183099) is split into an exact part (163/512) and a small rest, so float precision matches double.
PERLIN_BODY = r'''
float2 q = (P * Prm.x + Prm.yz) * 0.0001;
float v = 0.0;
float str = 1.0;
int iters = (int)Prm.w;
for (int n = 0; n < 6; n++)
{
    if (n >= iters) break;
    str *= 0.5;
    float2 f = floor(q);
    float2 u = q - f;
    u = u * u * (3.0 - 2.0 * u);
    float h[4];
    [unroll] for (int k = 0; k < 4; k++)
    {
        float2 c = f + float2(k & 1, k >> 1);
        float2 a = frac(c * 0.318359375) + c * -0.000049475 + float2(0.71, 0.113);
        float2 p = 50.0 * frac(a);
        h[k] = -1.0 + 2.0 * frac(p.x * p.y * (p.x + p.y));
    }
    v += str * lerp(lerp(h[0], h[1], u.x), lerp(h[2], h[3], u.x), u.y);
    q = float2(q.x * 1.6 - q.y * 1.2, q.x * 1.2 + q.y * 1.6);
}
return 0.5 + 0.5 * v;
'''

# Layer weights A..D (E = 1 - sum). F = fields (moss, grass, sand, needles), H = heights A..D, HE = height E,
# T = thresholds (moss, grass, sand).
WEIGHTS_BODY = r'''
float w[5];
float t = smoothstep(0.34, 0.66, F.w);
w[0] = t; w[1] = 1.0 - t; w[2] = 0.0; w[3] = 0.0; w[4] = 0.0;
float layer[3];
layer[0] = 0.85 * smoothstep(T.z - 0.08, T.z + 0.08, F.z);   // light sand
layer[1] = smoothstep(T.x - 0.04, T.x + 0.06, F.x);          // moss
layer[2] = 0.55 * smoothstep(T.y - 0.06, T.y + 0.10, F.y);   // earth under grass (partial)
int target[3] = {2, 3, 4};
[unroll] for (int i = 0; i < 3; i++)
{
    [unroll] for (int j0 = 0; j0 < 5; j0++) w[j0] *= 1.0 - layer[i];
    w[target[i]] += layer[i];
}
float h[5] = {H.x, H.y, H.z, H.w, HE};
float best = -10.0;
float v[5];
[unroll] for (int j1 = 0; j1 < 5; j1++)
{
    v[j1] = w[j1] > 0.001 ? w[j1] + 0.45 * h[j1] : -10.0;
    best = max(best, v[j1]);
}
float sum = 0.0;
[unroll] for (int j2 = 0; j2 < 5; j2++)
{
    v[j2] = max(v[j2] - best + 0.2, 0.0);
    sum += v[j2];
}
sum = max(sum, 1e-4);
return float4(v[0], v[1], v[2], v[3]) / sum;
'''

# Base colour: near/far mix per layer, then layer blend. Far share grows with camera distance D (cm) and varies with M.
BLEND_BC_BODY = r'''
// Slow ramp (a short one showed as a ring around the camera); far away the large scale hides the repetition.
float far = saturate(0.35 + 0.55 * saturate((D - 1500.0) / 7000.0) + 0.3 * (M - 0.5));
float wE = saturate(1.0 - dot(W, 1.0));
float3 a = lerp(A1, A2, far);
float3 b = lerp(B1, B2, far);
float3 c = lerp(C1, C2, far) * 0.78;   // Ground033 is very light
float3 d = lerp(D1, D2, far);
return a * W.x + b * W.y + c * W.z + d * W.w + E1 * wE;
'''

BLEND_N_BODY = r'''
float wE = saturate(1.0 - dot(W, 1.0));
float3 n = A * W.x + B * W.y + C * W.z + D * W.w + E * wE;
return normalize(n);
'''

BLEND_ORM_BODY = r'''
float wE = saturate(1.0 - dot(W, 1.0));
return A * W.x + B * W.y + C * W.z + D * W.w + E * wE;
'''

# Final colour: large-scale variation, meadow tint, season. F = fields, M = macro field, S = season phase 0..4.
FINISH_BODY = r'''
float3 col = Col;
float lum = dot(col, float3(0.3, 0.59, 0.11));
// the scans are bright and warm: a bit darker and less saturated for a northern pine forest
col = lerp(col, lum.xxx, 0.18) * 0.82;
// large-scale brightness and hue variation against tiling seen from above
col *= lerp(0.82, 1.14, M) * lerp(0.8, 1.14, Dt);
col = lerp(col, col * float3(1.06, 1.0, 0.9), saturate((F.w - 0.5) * 2.0) * 0.5);
// meadows: slightly greener and darker earth where PCG grows the grass
float meadow = smoothstep(T.y - 0.06, T.y + 0.12, F.y);
col = lerp(col, col * float3(0.80, 0.90, 0.62), 0.45 * meadow);
// seasons (cyclic distance to the season centre)
float s = fmod(S + 4.0, 4.0);
float dAut = min(abs(s - 2.0), 4.0 - abs(s - 2.0));
float dWin = min(abs(s - 3.0), 4.0 - abs(s - 3.0));
float dSpr = min(abs(s - 0.0), 4.0 - abs(s - 0.0));
float autumn = saturate(1.0 - dAut);
float winter = saturate(1.0 - dWin);
float spring = saturate(1.0 - dSpr);
float moss = smoothstep(T.x - 0.04, T.x + 0.06, F.x);
col = lerp(col, col * float3(0.9, 1.05, 0.8), spring * 0.35 * saturate(meadow + moss));
col = lerp(col, col * float3(1.12, 0.92, 0.70), autumn * 0.35);
lum = dot(col, float3(0.3, 0.59, 0.11));
col = lerp(col, lum * float3(0.97, 1.0, 1.06) * 1.1, winter * 0.45);
return max(col, 0.0);
'''

ROTATE_BODY = r'''
return float2(P.x * 0.8 - P.y * 0.6, P.x * 0.6 + P.y * 0.8);
'''


class Builder:
    def __init__(self, material):
        self.material = material
        self.mel = unreal.MaterialEditingLibrary

    def node(self, cls, x, y, **props):
        node = self.mel.create_material_expression(self.material, cls, x, y)
        for key, value in props.items():
            try:
                node.set_editor_property(key, value)
            except Exception as exc:
                error('%s.%s: %s' % (cls.__name__, key, exc))
        return node

    def link(self, src, src_out, dst, dst_in):
        if not self.mel.connect_material_expressions(src, src_out, dst, dst_in):
            error('link %s.%s -> %s.%s failed' % (src.get_name(), src_out, dst.get_name(), dst_in))

    def out(self, src, src_out, prop):
        if not self.mel.connect_material_property(src, src_out, prop):
            error('output %s.%s -> %s failed' % (src.get_name(), src_out, prop))

    def custom(self, x, y, code, out_type, description, inputs):
        custom_inputs = []
        for name in inputs:
            ci = unreal.CustomInput()
            ci.set_editor_property('input_name', name)
            custom_inputs.append(ci)
        return self.node(unreal.MaterialExpressionCustom, x, y, code=code, output_type=out_type,
                         description=description, inputs=custom_inputs)

    def const4(self, x, y, values):
        return self.node(unreal.MaterialExpressionConstant4Vector, x, y,
                         constant=unreal.LinearColor(*[float(v) for v in values]))


def load_texture(name):
    path = '%s/%s' % (TEXTURE_DIR, name)
    if not unreal.EditorAssetLibrary.does_asset_exist(path):
        error('texture %s missing (run tools/import_assets.ps1)' % path)
        return None
    tex = unreal.load_asset(path)
    # World-aligned UVs: the streamer cannot measure the texel density, keep all mips resident (about 60 MB).
    if not tex.get_editor_property('never_stream'):
        tex.set_editor_property('never_stream', True)
        unreal.EditorAssetLibrary.save_loaded_asset(tex, False)
    return tex


def season_source(b):
    """MPC SeasonPhase if the collection (from tools/unreal/import_assets.py) exists, else summer as a constant."""
    if unreal.EditorAssetLibrary.does_asset_exist(MPC_PATH):
        mpc = unreal.load_asset(MPC_PATH)
        names = [str(p.get_editor_property('parameter_name')) for p in mpc.get_editor_property('scalar_parameters')]
        if 'SeasonPhase' in names:
            node = b.node(unreal.MaterialExpressionCollectionParameter, -400, -900, collection=mpc)
            node.set_editor_property('parameter_name', 'SeasonPhase')
            log('season from MPC_GamerWorld.SeasonPhase')
            return node
    log('WARNING MPC_GamerWorld.SeasonPhase not found, ground built with summer')
    return b.node(unreal.MaterialExpressionConstant, -400, -900, r=1.3)


def build():
    """Create or rebuild the material; returns it (None on error)."""
    errors.clear()
    tools = unreal.AssetToolsHelpers.get_asset_tools()
    path = '%s/%s' % (MATERIAL_DIR, MATERIAL_NAME)
    if unreal.EditorAssetLibrary.does_asset_exist(path):
        material = unreal.load_asset(path)
    else:
        material = tools.create_asset(MATERIAL_NAME, MATERIAL_DIR, unreal.Material, unreal.MaterialFactoryNew())
    mel = unreal.MaterialEditingLibrary
    mel.delete_all_material_expressions(material)
    b = Builder(material)
    F2 = unreal.CustomMaterialOutputType.CMOT_FLOAT2
    F1 = unreal.CustomMaterialOutputType.CMOT_FLOAT1
    F3 = unreal.CustomMaterialOutputType.CMOT_FLOAT3
    F4 = unreal.CustomMaterialOutputType.CMOT_FLOAT4

    # World position X/Y in cm, rotated copy for the far samples, camera distance.
    world = b.node(unreal.MaterialExpressionWorldPosition, -3000, 0)
    pos = b.node(unreal.MaterialExpressionComponentMask, -2800, 0, r=True, g=True, b=False, a=False)
    b.link(world, '', pos, '')
    rot = b.custom(-2600, 200, ROTATE_BODY, F2, 'P rotated 37 deg', ['P'])
    b.link(pos, '', rot, 'P')
    camera = b.node(unreal.MaterialExpressionCameraPositionWS, -3000, 400)
    dist = b.node(unreal.MaterialExpressionDistance, -2800, 400)
    b.link(camera, '', dist, 'A')
    b.link(world, '', dist, 'B')

    # Noise fields.
    def field(name, x, y):
        prm = b.const4(x - 250, y + 60, FIELDS[name])
        node = b.custom(x, y, PERLIN_BODY, F1, 'Field %s (PCG Perlin2D)' % name, ['P', 'Prm'])
        b.link(pos, '', node, 'P')
        b.link(prm, '', node, 'Prm')
        return node

    moss = field('moss', -2300, -1200)
    grass = field('grass', -2300, -1000)
    sand = field('sand', -2300, -800)
    needles = field('needles', -2300, -600)
    macro = field('macro', -2300, -400)
    detail = field('detail', -2300, -200)
    fields = b.node(unreal.MaterialExpressionAppendVector, -1900, -1100)
    fields2 = b.node(unreal.MaterialExpressionAppendVector, -1900, -900)
    fields3 = b.node(unreal.MaterialExpressionAppendVector, -1700, -1000)
    b.link(moss, '', fields, 'A')
    b.link(grass, '', fields, 'B')
    b.link(sand, '', fields2, 'A')
    b.link(needles, '', fields2, 'B')
    b.link(fields, '', fields3, 'A')
    b.link(fields2, '', fields3, 'B')
    thresholds = b.const4(-1700, -1250, (MOSS_THRESHOLD, GRASS_THRESHOLD, SAND_THRESHOLD, 0.0))

    # Texture samples per layer.
    samples = {}
    shared = unreal.SamplerSourceMode.SSM_WRAP_WORLD_GROUP_SETTINGS
    y = 0
    for index, (base, tile, far) in enumerate(LAYERS):
        tex = {s: load_texture('T_%s_%s' % (base, s)) for s in ('BC', 'N', 'ORM')}
        if None in tex.values():
            continue
        tile_param = b.node(unreal.MaterialExpressionScalarParameter, -2500, y, parameter_name='Tile' + base,
                            default_value=tile)
        uv = b.node(unreal.MaterialExpressionDivide, -2200, y)
        b.link(pos, '', uv, 'A')
        b.link(tile_param, '', uv, 'B')
        entry = {}
        for k, (suffix, sampler) in enumerate((('BC', unreal.MaterialSamplerType.SAMPLERTYPE_COLOR),
                                               ('N', unreal.MaterialSamplerType.SAMPLERTYPE_NORMAL),
                                               ('ORM', unreal.MaterialSamplerType.SAMPLERTYPE_MASKS))):
            s = b.node(unreal.MaterialExpressionTextureSampleParameter2D, -1900, y + k * 260,
                       parameter_name='%s_%s' % (base, suffix), texture=tex[suffix], sampler_type=sampler,
                       sampler_source=shared)
            b.link(uv, '', s, 'UVs')
            entry[suffix] = s
        if far:
            far_tile = b.node(unreal.MaterialExpressionMultiply, -2200, y + 300, const_b=FAR_SCALE)
            b.link(tile_param, '', far_tile, 'A')
            far_uv = b.node(unreal.MaterialExpressionDivide, -2050, y + 300)
            b.link(rot, '', far_uv, 'A')
            b.link(far_tile, '', far_uv, 'B')
            s = b.node(unreal.MaterialExpressionTextureSampleParameter2D, -1600, y + 780,
                       parameter_name='%s_BC' % base, texture=tex['BC'],
                       sampler_type=unreal.MaterialSamplerType.SAMPLERTYPE_COLOR, sampler_source=shared)
            b.link(far_uv, '', s, 'UVs')
            entry['BC_far'] = s
        samples[base] = entry
        y += 1100
    if len(samples) != len(LAYERS):
        return None
    A, B, C, D, E = (samples[l[0]] for l in LAYERS)

    # Heights (ORM alpha) -> weights.
    h_ab = b.node(unreal.MaterialExpressionAppendVector, -1300, -600)
    h_cd = b.node(unreal.MaterialExpressionAppendVector, -1300, -450)
    heights = b.node(unreal.MaterialExpressionAppendVector, -1150, -500)
    b.link(A['ORM'], 'A', h_ab, 'A')
    b.link(B['ORM'], 'A', h_ab, 'B')
    b.link(C['ORM'], 'A', h_cd, 'A')
    b.link(D['ORM'], 'A', h_cd, 'B')
    b.link(h_ab, '', heights, 'A')
    b.link(h_cd, '', heights, 'B')
    weights = b.custom(-900, -700, WEIGHTS_BODY, F4, 'Layer weights A..D (E = rest)', ['F', 'H', 'HE', 'T'])
    b.link(fields3, '', weights, 'F')
    b.link(heights, '', weights, 'H')
    b.link(E['ORM'], 'A', weights, 'HE')
    b.link(thresholds, '', weights, 'T')

    blend_bc = b.custom(-500, -300, BLEND_BC_BODY, F3, 'Base colour blend',
                        ['W', 'D', 'M', 'A1', 'A2', 'B1', 'B2', 'C1', 'C2', 'D1', 'D2', 'E1'])
    b.link(weights, '', blend_bc, 'W')
    b.link(dist, '', blend_bc, 'D')
    b.link(macro, '', blend_bc, 'M')
    for key, layer in (('A', A), ('B', B), ('C', C), ('D', D)):
        b.link(layer['BC'], 'RGB', blend_bc, key + '1')
        b.link(layer['BC_far'], 'RGB', blend_bc, key + '2')
    b.link(E['BC'], 'RGB', blend_bc, 'E1')

    blend_n = b.custom(-500, 200, BLEND_N_BODY, F3, 'Normal blend', ['W', 'A', 'B', 'C', 'D', 'E'])
    blend_orm = b.custom(-500, 600, BLEND_ORM_BODY, F4, 'ORM blend', ['W', 'A', 'B', 'C', 'D', 'E'])
    for node, suffix, out in ((blend_n, 'N', 'RGB'), (blend_orm, 'ORM', 'RGBA')):
        b.link(weights, '', node, 'W')
        for key, layer in (('A', A), ('B', B), ('C', C), ('D', D), ('E', E)):
            b.link(layer[suffix], out, node, key)

    season = season_source(b)
    finish = b.custom(-150, -300, FINISH_BODY, F3, 'Variation, meadow tint, season', ['Col', 'F', 'M', 'T', 'S', 'Dt'])
    b.link(blend_bc, '', finish, 'Col')
    b.link(fields3, '', finish, 'F')
    b.link(macro, '', finish, 'M')
    b.link(thresholds, '', finish, 'T')
    b.link(season, '', finish, 'S')
    b.link(detail, '', finish, 'Dt')

    rough = b.node(unreal.MaterialExpressionComponentMask, -150, 600, r=False, g=True, b=False, a=False)
    ao = b.node(unreal.MaterialExpressionComponentMask, -150, 750, r=True, g=False, b=False, a=False)
    b.link(blend_orm, '', rough, '')
    b.link(blend_orm, '', ao, '')
    specular = b.node(unreal.MaterialExpressionConstant, -150, 450, r=0.35)

    P = unreal.MaterialProperty
    b.out(finish, '', P.MP_BASE_COLOR)
    b.out(blend_n, '', P.MP_NORMAL)
    b.out(rough, '', P.MP_ROUGHNESS)
    b.out(ao, '', P.MP_AMBIENT_OCCLUSION)
    b.out(specular, '', P.MP_SPECULAR)

    mel.recompile_material(material)
    unreal.EditorAssetLibrary.save_loaded_asset(material, False)
    log('material %s built, %d error(s)' % (path, len(errors)))
    return None if errors else material


if __name__ == '__main__':
    build()
