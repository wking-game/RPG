"""
MetaHuman preparation (everything that works without the Epic login), run headless:
  tools/metahuman.ps1 Prepare

Creates /Game/Gamer/Characters/MetaHuman/MHC_Kraeftig and MHC_Schmal (body builds "strong" and "slim",
not tied to a sex - decision 2026-11-13) and presets body measurements and skin after
design/stil_norden_gezeitenhall.md (weathered, realistic proportions). Existing assets are kept and
only re-preset. Log lines start with "GAMER_MH:"; last line "GAMER_MH: done, N error(s)".
"""
import unreal

FOLDER = '/Game/Gamer/Characters/MetaHuman'
errors = []

# Target measurements in cm (only constraints that exist are used; names are logged).
FIGURES = {
    'MHC_Kraeftig': {'Height': 182.0, 'Chest': 112.0, 'Waist': 92.0, 'Hip': 104.0, 'Across Shoulder': 40.0,
                     'Bicep': 36.0, 'Thigh': 60.0, 'Neck': 41.0},
    'MHC_Schmal': {'Height': 170.0, 'Chest': 92.0, 'Waist': 74.0, 'Hip': 96.0, 'Across Shoulder': 34.0,
                   'Bicep': 29.0, 'Thigh': 54.0, 'Neck': 34.0},
}
# Skin: U = tone (light..dark), V = undertone; weathered = a bit rougher than default (1.06).
SKIN = {'MHC_Kraeftig': (0.42, 0.55, 1.18), 'MHC_Schmal': (0.36, 0.52, 1.14)}


def log(message):
    unreal.log('GAMER_MH: ' + message)


def fail(message):
    errors.append(message)
    unreal.log_error('GAMER_MH: ' + message)


assets = unreal.get_editor_subsystem(unreal.EditorAssetSubsystem)
tools = unreal.AssetToolsHelpers.get_asset_tools()
mh = unreal.get_editor_subsystem(unreal.MetaHumanCharacterEditorSubsystem)

for name, measures in FIGURES.items():
    path = FOLDER + '/' + name
    character = unreal.load_asset(path) if assets.does_asset_exist(path) else \
        tools.create_asset(name, FOLDER, unreal.MetaHumanCharacter, unreal.MetaHumanCharacterFactoryNew())
    if character is None:
        fail('cannot create ' + path)
        continue
    if not mh.try_add_object_to_edit(character):
        fail('cannot open %s for editing' % name)
        continue
    try:
        constraints = list(mh.get_body_constraints(character))
        log('%s: body constraints: %s' % (name, ', '.join('%s=%.0f' % (c.name, c.target_measurement) for c in constraints)))
        changed = []
        for c in constraints:
            key = next((k for k in measures if str(c.name).lower() == k.lower()), None)
            if key is None:
                c.is_active = False  # everything else follows the body model freely
                continue
            c.is_active = True
            c.target_measurement = measures[key]
            changed.append('%s=%.0f' % (c.name, measures[key]))
        if changed:
            mh.set_body_constraints(character, constraints)
            mh.commit_body_state(character)  # store the evaluated body in the asset
        log('%s: body preset %s' % (name, ', '.join(changed) or 'none (no matching constraints)'))
    except Exception as error:
        fail('%s: body preset failed: %s' % (name, error))
    try:
        skin = character.get_editor_property('skin_settings')
        props = skin.get_editor_property('skin')
        u, v, rough = SKIN[name]
        props.set_editor_property('u', u)
        props.set_editor_property('v', v)
        props.set_editor_property('roughness', rough)
        skin.set_editor_property('skin', props)
        mh.commit_skin_settings(character, skin)
        log('%s: skin U=%.2f V=%.2f roughness=%.2f' % (name, u, v, rough))
    except Exception as error:
        fail('%s: skin preset failed: %s' % (name, error))
    mh.remove_object_to_edit(character)
    assets.save_asset(path, only_if_is_dirty=False)

log('done, %d error(s)' % len(errors))
