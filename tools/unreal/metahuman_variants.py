"""
Looks for the MetaHuman figures: hair, beard, eyebrows and skin tone (table: metahuman_looks.py).
Run: tools/metahuman.ps1 Variants   (full editor session, quits itself; afterwards Assemble each figure, then Link)

- Player figures MHC_Kraeftig / MHC_Schmal get hair and beard.
- Villager variants and the seeress are copies of the rigged base figures (same face rig, no new auto-rig).
- Hair colour is set in the Assemble step (the groom parameters exist only after a build).
Clothing is NOT set here: own Norse garments come later (design/kleidung_kandidaten_kostenlos.md).
"""
import os
import random
import statistics
import sys

import unreal

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import metahuman_looks as looks  # noqa: E402

errors = []


def log(message):
    unreal.log('GAMER_MH: ' + message)


def fail(message):
    errors.append(message)
    unreal.log_error('GAMER_MH: ' + message)


def ensure_copy(name, base):
    path = looks.ROOT + '/' + name
    if unreal.EditorAssetLibrary.does_asset_exist(path):
        return unreal.load_asset(path)
    copy = unreal.EditorAssetLibrary.duplicate_asset(looks.ROOT + '/' + base, path)
    log('%s: copied from %s' % (name, base))
    return copy


def set_grooms(character, grooms):
    collection = character.get_editor_property('internal_collection')
    instance = collection.get_editor_property('default_instance')
    for slot, item in grooms.items():
        wardrobe = unreal.load_asset('%s/%s/%s.%s' % (looks.GROOMS, looks.FOLDER[slot], item, item))
        if wardrobe is None:
            fail('%s: wardrobe item %s not found' % (character.get_name(), item))
            continue
        key = collection.try_add_item_from_wardrobe_item(slot, wardrobe)
        # Replaces an earlier choice for this slot (the script may run again after the looks change).
        instance.set_single_slot_selection(slot, key)


def set_skin(mh, character, skin_uv):
    if not skin_uv:
        return
    skin = character.get_editor_property('skin_settings')
    props = skin.get_editor_property('skin')
    props.set_editor_property('u', skin_uv[0])
    props.set_editor_property('v', skin_uv[1])
    skin.set_editor_property('skin', props)
    mh.commit_skin_settings(character, skin)  # drops downloaded textures; Assemble fetches them again


def base_coefficients(mh, base):
    character = unreal.load_asset(looks.ROOT + '/' + base)
    if not mh.try_add_object_to_edit(character):
        raise RuntimeError('cannot open %s for editing' % base)
    try:
        return list(mh.get_face_model_coefficients(character))
    finally:
        mh.remove_object_to_edit(character)


def individual_face(mh, character, base, seed, strength):
    """Own face: the base face's model coefficients plus seeded noise (deterministic, so running again gives the
    same face), then a new auto-rig through Epic's service (the old rig no longer fits the changed face)."""
    coeffs = base_coefficients(mh, base)
    rng = random.Random(seed)
    spread = max(statistics.pstdev(coeffs) if len(coeffs) > 1 else 0.0, 0.25)
    new = [c + rng.gauss(0.0, spread * strength) for c in coeffs]
    log('%s: face model %d coefficients, base spread %.3f, noise %.3f (seed %d)'
        % (character.get_name(), len(coeffs), spread, spread * strength, seed))
    mh.set_face_model_coefficients(character, new)
    mh.commit_face_state(character)
    request = unreal.MetaHumanCharacterAutoRiggingRequestParams()
    request.blocking = True
    request.report_progress = False
    request.rig_type = unreal.MetaHumanRigType.JOINTS_ONLY
    mh.request_auto_rigging(character, request)
    log('%s: auto-rig requested (blocking), ready for assembly: %s'
        % (character.get_name(), mh.can_build_meta_human(character, False)))


mh = unreal.get_editor_subsystem(unreal.MetaHumanCharacterEditorSubsystem)
# One figure per process keeps memory low (several in one editor ran out of memory, 16.11.).
only = os.environ.get('GAMER_MH_FIGURE', '')
faces = float(os.environ.get('GAMER_MH_FACES', '0') or 0)  # >0: own faces for the copies, strength factor
for index, (name, (base, grooms, _colour, skin_uv, _quality)) in enumerate(looks.FIGURES.items()):
    if only and only != name:
        continue
    try:
        character = ensure_copy(name, base) if base else unreal.load_asset(looks.ROOT + '/' + name)
        if character is None:
            fail('%s: not found' % name)
            continue
        if not mh.try_add_object_to_edit(character):
            fail('%s: cannot open for editing' % name)
            continue
        try:
            if base and faces > 0:
                individual_face(mh, character, base, 1000 + index, faces)
            set_skin(mh, character, skin_uv)
            set_grooms(character, grooms)
            log('%s: grooms %s, skin %s' % (name, ', '.join(grooms.values()), skin_uv))
        finally:
            mh.remove_object_to_edit(character)
        unreal.EditorAssetLibrary.save_loaded_asset(character, False)
    except Exception as error:
        fail('%s: %s' % (name, error))

log('done, %d error(s)' % len(errors))
unreal.SystemLibrary.quit_editor()
