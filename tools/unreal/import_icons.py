"""
Imports only the rendered UI icons (Assets/manifest_icons.json) into /Game/Gamer/UI/Icons.
The full import (tools/import_assets.ps1) re-imports every mesh and texture and can run out of memory on a PC with little free RAM;
after tools/blender/render_icons.py this small run is enough. Use tools/import_icons.ps1.
"""
import json
import os

import unreal

PROJECT_DIR = os.path.abspath(unreal.Paths.project_dir())
ASSETS = os.path.join(os.path.abspath(os.path.join(PROJECT_DIR, '..')), 'Assets')
DEST = '/Game/Gamer/UI/Icons'
asset_tools = unreal.AssetToolsHelpers.get_asset_tools()
editor_assets = unreal.get_editor_subsystem(unreal.EditorAssetSubsystem)
errors = []

entries = json.load(open(os.path.join(ASSETS, 'manifest_icons.json'), encoding='utf-8'))['icons']
for entry in entries:
    task = unreal.AssetImportTask()
    task.set_editor_property('filename', os.path.join(ASSETS, entry['file']))
    task.set_editor_property('destination_path', DEST)
    task.set_editor_property('destination_name', entry['name'])
    task.set_editor_property('replace_existing', True)
    task.set_editor_property('automated', True)
    task.set_editor_property('save', False)
    asset_tools.import_asset_tasks([task])
    if not task.get_editor_property('imported_object_paths'):
        errors.append('import failed: ' + entry['file'])
        continue
    tex = unreal.load_asset(DEST + '/' + entry['name'])
    tex.set_editor_property('compression_settings', unreal.TextureCompressionSettings.TC_EDITOR_ICON)
    tex.set_editor_property('lod_group', unreal.TextureGroup.TEXTUREGROUP_UI)
    tex.set_editor_property('srgb', True)

if not editor_assets.save_directory(DEST, only_if_is_dirty=False, recursive=True):
    errors.append('saving ' + DEST + ' failed')
unreal.log('GAMER_IMPORT: icons %d, done, %d error(s)' % (len(entries), len(errors)))
if errors:
    raise RuntimeError('GAMER_IMPORT failed: ' + '; '.join(errors))
