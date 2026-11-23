"""
Assemble one MetaHuman figure headless (needs the Epic login from an earlier editor session, which the
MetaHuman plugin keeps as persistent credentials):
  tools/metahuman.ps1 Assemble -Figure MHC_Kraeftig

Steps: open the character for editing, download texture sources if missing (blocking), check that it can be
assembled (rig present; auto-rig is done by the user in the Creator), assemble "UE Optimized High" into
/Game/Gamer/Characters/MetaHuman/<Name> and "UE Optimized Low" into .../<Name>_Low, save.
The figure comes from the environment variable GAMER_MH_FIGURE.
"""
import os
import sys

import unreal

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import metahuman_looks as looks  # noqa: E402
import metahuman_hairsim as hairsim  # noqa: E402

ROOT = '/Game/Gamer/Characters/MetaHuman'
errors = []


def log(message):
    unreal.log('GAMER_MH: ' + message)


def fail(message):
    errors.append(message)
    unreal.log_error('GAMER_MH: ' + message)


def has_textures(character):
    """bHasHighResolutionTextures is a read-only property (not a function) in Python."""
    return bool(character.get_editor_property('has_high_resolution_textures'))


figure = os.environ.get('GAMER_MH_FIGURE', '')
# Output folder name: MHC_Bewohner_K1 -> Bewohner_K1
NAMES = {figure: figure[4:] if figure.startswith('MHC_') else figure}
_q = os.environ.get('GAMER_MH_QUALITY', 'Both')
if _q == 'Mid':  # hair cards + simulation for the player figures (docs/01_entscheidungen.md, Haare)
    QUALITIES = [(unreal.MetaHumanQualityLevel.MEDIUM, '_Mid')]
else:
    QUALITIES = [(unreal.MetaHumanQualityLevel.LOW, '_Low')]
    if _q == 'Both':
        QUALITIES.insert(0, (unreal.MetaHumanQualityLevel.HIGH, ''))
mh = unreal.get_editor_subsystem(unreal.MetaHumanCharacterEditorSubsystem)
path = ROOT + '/' + figure
character = unreal.load_asset(path) if figure and unreal.EditorAssetLibrary.does_asset_exist(path) else None
if character is None:
    fail('unknown or missing figure "%s" in %s' % (figure, ROOT))
elif not mh.try_add_object_to_edit(character):
    fail('cannot open %s for editing' % figure)
else:
    try:
        present = has_textures(character)
        log('%s: texture sources present: %s' % (figure, present))
        if not present:
            params = unreal.MetaHumanCharacterTextureRequestParams()
            params.set_editor_property('report_progress', False)
            params.set_editor_property('blocking', True)
            log('%s: downloading texture sources (blocking) ...' % figure)
            mh.request_texture_sources(character, params)
            present = has_textures(character)
            log('%s: texture sources after download: %s' % (figure, present))
            if not present:
                fail('%s: texture download failed (login? see LogMetaHumanAuth lines)' % figure)
            else:
                unreal.EditorLoadingAndSavingUtils.save_dirty_packages(True, True)
        if not errors:
            buildable = mh.can_build_meta_human(character, True)
            log('%s: ready for assembly (rigged): %s' % (figure, buildable))
            if not buildable:
                fail('%s cannot be assembled (rig missing? see LogMetaHuman lines)' % figure)
        if not errors:
            coloured = False
            for quality, suffix in QUALITIES:
                params = unreal.MetaHumanCharacterEditorBuildParameters()
                params.set_editor_property('pipeline_type', unreal.MetaHumanDefaultPipelineType.OPTIMIZED)
                params.set_editor_property('pipeline_quality', quality)
                params.set_editor_property('absolute_build_path', ROOT)
                params.set_editor_property('name_override', NAMES[figure] + suffix)
                log('%s: assembling %s into %s/%s ...' % (figure, quality, ROOT, NAMES[figure] + suffix))
                mh.build_meta_human(character, params)
                if not coloured:
                    # Groom colour parameters exist only once the collection is built: set them, build again.
                    coloured = True
                    try:
                        set_count = looks.set_hair_colour(unreal, character, log)
                    except Exception as error:  # colour is optional: keep the default hair colour
                        unreal.log_warning('GAMER_MH: %s: hair colour not set: %s' % (figure, error))
                        set_count = 0
                    if set_count:
                        log('%s: hair colour set, assembling %s again' % (figure, quality))
                        mh.build_meta_human(character, params)
                unreal.EditorLoadingAndSavingUtils.save_dirty_packages(True, True)
                folder = ROOT + '/' + NAMES[figure] + suffix
                meshes = [p for p in unreal.EditorAssetLibrary.list_assets(folder, True, False)
                          if isinstance(unreal.load_asset(str(p).split('.')[0]), unreal.SkeletalMesh)]
                log('%s: %s skeletal meshes in %s: %d' % (figure, quality, folder, len(meshes)))
                if not meshes:
                    fail('%s: assembly %s produced no skeletal mesh (see MetaHuman: Error lines)' % (figure, quality))
                    break
                if suffix == '_Mid':  # visible card LODs 3-4 should follow the simulated guides
                    hairsim.enable_card_simulation(folder, log)
    except Exception as error:
        fail('%s: %s' % (figure, error))
    mh.remove_object_to_edit(character)

log('done, %d error(s)' % len(errors))
# Started as a full editor (-ExecutePythonScript): close it again.
unreal.SystemLibrary.quit_editor()
