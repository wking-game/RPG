"""
Exports the assembled MetaHuman bodies (strong/slim) as FBX for the clothing builder in Blender
(tools/blender/build_clothing.py). Run: tools/clothing.ps1 Export
Output: game/Saved/Clothing/body_<Kraeftig|Schmal>.fbx (not versioned).
"""
import os

import unreal

ROOT = '/Game/Gamer/Characters/MetaHuman'
OUT = os.path.join(os.path.abspath(unreal.Paths.project_saved_dir()), 'Clothing')
errors = []


def log(message):
    unreal.log('GAMER_CLOTH: ' + message)


os.makedirs(OUT, exist_ok=True)
for name in ('Kraeftig', 'Schmal'):
    path = '%s/%s/Body/SKM_MHC_%s_BodyMesh' % (ROOT, name, name)
    mesh = unreal.load_asset(path)
    if mesh is None:
        errors.append(path)
        unreal.log_error('GAMER_CLOTH: missing ' + path)
        continue
    task = unreal.AssetExportTask()
    task.object = mesh
    task.filename = os.path.join(OUT, 'body_%s.fbx' % name)
    task.automated = True
    task.replace_identical = True
    task.prompt = False
    options = unreal.FbxExportOption()
    options.set_editor_property('collision', False)
    options.set_editor_property('level_of_detail', False)
    options.set_editor_property('export_morph_targets', False)
    task.options = options
    ok = unreal.Exporter.run_asset_export_task(task)
    log('%s -> %s: %s' % (path, task.filename, ok))
    if not ok:
        errors.append(path)

log('done, %d error(s)' % len(errors))
# Runs in a full editor session (-ExecutePythonScript): the commandlet asserts in SkinnedMeshComponent.
unreal.SystemLibrary.quit_editor()
