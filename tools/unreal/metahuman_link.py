"""
After assembling (tools/metahuman.ps1 Assemble), run headless:
  tools/metahuman.ps1 Link

Scans /Game/Gamer/Characters/MetaHuman/<Folder>/ for the assembled body and face skeletal meshes and the grooms
(hair, beard, eyebrows: GroomAsset + GroomBindingAsset), makes the body skeleton compatible with SK_Mannequin
(so the existing animations play on it) and writes game/Content/Data/figures.json.
Folder -> key: Kraeftig/Schmal -> strong/slim (players), Kraeftig_Low/Schmal_Low -> strong_low/slim_low,
Bewohner_K<n>_Low / Bewohner_S<n>_Low -> villager_strong_<n> / villager_slim_<n>, Seherin_Low -> npc_seeress.
Fallback for the players: the Creator's default output /Game/MetaHumans/MHC_<name>.
Missing entries = the game keeps Manny/Quinn (GamerFigures).
"""
import functools
import json
import os
import re

import sys

import unreal

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import metahuman_looks as looks  # noqa: E402

ROOT = '/Game/Gamer/Characters/MetaHuman'
MANNEQUIN_SKELETON = '/Game/Characters/Mannequins/Meshes/SK_Mannequin'
FIXED = {
    'strong': [ROOT + '/Kraeftig', '/Game/MetaHumans/MHC_Kraeftig'],
    'slim': [ROOT + '/Schmal', '/Game/MetaHumans/MHC_Schmal'],
    'strong_low': [ROOT + '/Kraeftig_Low'],
    'slim_low': [ROOT + '/Schmal_Low'],
    'strong_mid': [ROOT + '/Kraeftig_Mid'],
    'slim_mid': [ROOT + '/Schmal_Mid'],
    'npc_seeress': [ROOT + '/Seherin_Low', ROOT + '/Seherin'],
}
errors = []


def log(message):
    unreal.log('GAMER_MH: ' + message)


def fail(message):
    errors.append(message)
    unreal.log_error('GAMER_MH: ' + message)


assets = unreal.get_editor_subsystem(unreal.EditorAssetSubsystem)
mannequin = unreal.load_asset(MANNEQUIN_SKELETON)

folders = dict(FIXED)
for path in unreal.EditorAssetLibrary.list_assets(ROOT, True, True):
    m = re.match(r'^%s/Bewohner_([KS])(\d+)_Low$' % re.escape(ROOT), str(path).rstrip('/'))
    if m:
        folders['villager_%s_%s' % ('strong' if m.group(1) == 'K' else 'slim', m.group(2))] = [m.group(0)]


def make_compatible(key, body):
    skeleton = body.get_editor_property('skeleton')
    try:
        compatible = list(skeleton.get_editor_property('compatible_skeletons'))
        if mannequin and all(str(c) != mannequin.get_path_name() for c in compatible):
            compatible.append(mannequin)
            skeleton.set_editor_property('compatible_skeletons', compatible)
            assets.save_asset(skeleton.get_path_name(), only_if_is_dirty=False)
        mann_compat = list(mannequin.get_editor_property('compatible_skeletons'))
        if all(str(c) != skeleton.get_path_name() for c in mann_compat):
            mann_compat.append(skeleton)
            mannequin.set_editor_property('compatible_skeletons', mann_compat)
            assets.save_asset(mannequin.get_path_name(), only_if_is_dirty=False)
    except Exception as error:
        fail('%s: compatible skeleton not set: %s' % (key, error))



@functools.lru_cache(maxsize=None)
def groom_materials(folder):
    """Groom asset path -> material overrides of the assembled blueprint BP_<Name> (MI_WI_* with the groom's own
    card atlas and colour); the groom assets themselves only reference Epic's generic MI_Hair*."""
    result = {}
    sub = unreal.get_engine_subsystem(unreal.SubobjectDataSubsystem)
    for asset_path in assets.list_assets(folder, False, False):
        bp = unreal.load_asset(str(asset_path).split('.')[0])
        if not isinstance(bp, unreal.Blueprint):
            continue
        for handle in sub.k2_gather_subobject_data_for_blueprint(bp):
            obj = unreal.SubobjectDataBlueprintFunctionLibrary.get_object(
                unreal.SubobjectDataBlueprintFunctionLibrary.get_data(handle))
            if isinstance(obj, unreal.GroomComponent) and obj.get_editor_property('groom_asset'):
                result[obj.get_editor_property('groom_asset').get_path_name()] = [
                    m.get_path_name() if m else '' for m in obj.get_editor_property('override_materials')]
    return result


figures = {}
for key, candidates in sorted(folders.items()):
    path = next((c for c in candidates if assets.does_directory_exist(c)), None)
    if path is None:
        log('%s: none of %s (not assembled yet)' % (key, ', '.join(candidates)))
        continue
    body = face = None
    bindings = []
    for asset_path in assets.list_assets(path, True, False):
        asset = unreal.load_asset(str(asset_path).split('.')[0])
        if isinstance(asset, unreal.SkeletalMesh):
            name = asset.get_name().lower()
            if 'face' in name and face is None:
                face = asset
            elif 'body' in name and body is None:
                body = asset
        elif isinstance(asset, unreal.GroomBindingAsset):
            bindings.append(asset)
    if body is None:
        fail('%s: no body skeletal mesh in %s' % (key, path))
        continue
    entry = {'body': body.get_path_name()}
    if face is not None:
        entry['face'] = face.get_path_name()
    grooms = []
    folder_materials = groom_materials(path)
    for binding in bindings:
        try:
            groom = binding.get_editor_property('groom')
        except Exception:
            groom = None
        if groom is None:  # assembled names: Hair_X_Binding -> Hair_X
            groom = unreal.load_asset(binding.get_path_name().split('.')[0].replace('_Binding', ''))
        if isinstance(groom, unreal.GroomAsset):
            entry_groom = {'groom': groom.get_path_name(), 'binding': binding.get_path_name()}
            materials = folder_materials.get(groom.get_path_name())
            if materials:
                entry_groom['materials'] = materials
            grooms.append(entry_groom)
    if grooms:
        entry['grooms'] = grooms
    make_compatible(key, body)
    figures[key] = entry
    log('%s: body %s, face %s, grooms %d' % (key, body.get_name(), face.get_name() if face else '-', len(grooms)))

# Hair kits (character menu look_hair / look_beard): grooms of the Medium player figure and its kit copies
# <Body>_HK<n>_Mid, found by the wardrobe name of their binding (Hair_L_MessyClumps_Binding or Hair_M_TwistedBraids).
hair_kits = {}
for body_key, body_folder in (('strong', 'Kraeftig'), ('slim', 'Schmal')):
    found = {}
    folders = [ROOT + '/' + body_folder + '_Mid'] + sorted(
        str(p).rstrip('/') for p in unreal.EditorAssetLibrary.list_assets(ROOT, False, True)
        if re.match(r'^%s/%s_HK\d+_Mid$' % (re.escape(ROOT), body_folder), str(p).rstrip('/')))
    for folder in folders:
        if not assets.does_directory_exist(folder):
            continue
        for asset_path in assets.list_assets(folder, True, False):
            binding = unreal.load_asset(str(asset_path).split('.')[0])
            if not isinstance(binding, unreal.GroomBindingAsset):
                continue
            name = binding.get_name().replace('_Binding', '')
            try:
                groom = binding.get_editor_property('groom')
            except Exception:
                groom = None
            if isinstance(groom, unreal.GroomAsset) and name not in found:
                found[name] = {'groom': groom.get_path_name(), 'binding': binding.get_path_name()}
                materials = groom_materials(folder).get(groom.get_path_name())
                if materials:
                    found[name]['materials'] = materials
    kits = {option: found[name] for option, name in looks.HAIR_KITS.items() if name in found}
    # Short stubble has no groom in the Medium build: it is baked into that kit's face textures, so the option
    # brings the kit's face mesh (same topology; the other grooms' bindings fit it).
    for option, name in looks.HAIR_KITS.items():
        if option in kits:
            continue
        for figure, (base, grooms, _c, _s, _q) in looks.FIGURES.items():
            if base == 'MHC_' + body_folder and grooms.get('Beard') == 'WI_' + name:
                folder = ROOT + '/' + figure[4:] + '_Mid'
                faces = [unreal.load_asset(str(a).split('.')[0]) for a in assets.list_assets(folder, True, False)
                         if 'face' in str(a).split('.')[-1].lower()]
                faces = [f for f in faces if isinstance(f, unreal.SkeletalMesh)]
                if faces:
                    kits[option] = {'face': faces[0].get_path_name()}
    missing = sorted(option for option in looks.HAIR_KITS if option not in kits)
    log('hair kits %s: %d (missing: %s)' % (body_key, len(kits), ', '.join(missing) or '-'))
    if kits:
        hair_kits[body_key] = kits

out = os.path.join(os.path.abspath(unreal.Paths.project_content_dir()), 'Data', 'figures.json')
with open(out, 'w', encoding='utf-8') as f:
    json.dump({'schema': 1, 'figures': figures, 'hair_kits': hair_kits}, f, indent=2)
log('wrote %s with %d figure(s)' % (out, len(figures)))
log('done, %d error(s)' % len(errors))
