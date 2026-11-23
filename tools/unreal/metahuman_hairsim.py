"""
Hair movement for the Medium assemblies (players): the assembled grooms simulate only up to LOD 2 (strands/cards
with Simulation=Auto), but in the Medium build LOD 2 renders nothing; the visible card LODs 3-4 have
Simulation=Disable. This switches LODs 3-4 to Simulation=Enable for every group whose physics is on (hair, beard;
eyebrows stay static), so the cards follow the simulated guides.
Called by metahuman_assemble.py after a Mid build; can also run alone (headless):
  UnrealEditor-Cmd Gamer.uproject -run=pythonscript -script=tools/unreal/metahuman_hairsim.py
(env GAMER_MH_FOLDERS: comma-separated folders, default Kraeftig_Mid,Schmal_Mid)
"""
import os

import unreal

ROOT = '/Game/Gamer/Characters/MetaHuman'
SIM_LODS = (3, 4)


def enable_card_simulation(folder, log):
    changed = 0
    for path in unreal.EditorAssetLibrary.list_assets(folder, True, False):
        groom = unreal.load_asset(str(path).split('.')[0])
        if not isinstance(groom, unreal.GroomAsset):
            continue
        lods = groom.get_editor_property('hair_groups_lod')
        physics = groom.get_editor_property('hair_groups_physics')
        dirty = False
        for g, group in enumerate(lods):
            if not physics[g].get_editor_property('solver_settings').get_editor_property('enable_simulation'):
                continue
            entries = group.get_editor_property('lods')
            for i in SIM_LODS:
                if i < len(entries) and entries[i].get_editor_property('simulation') != unreal.GroomOverrideType.ENABLE:
                    entry = entries[i]  # indexing returns a copy: change it, then write it back
                    entry.set_editor_property('simulation', unreal.GroomOverrideType.ENABLE)
                    entries[i] = entry
                    dirty = True
            group.set_editor_property('lods', entries)
            lods[g] = group
        if dirty:
            groom.modify()
            groom.set_editor_property('hair_groups_lod', lods)
            unreal.EditorAssetLibrary.save_loaded_asset(groom, False)
            changed += 1
            log('%s: card LODs %s simulate now' % (groom.get_name(), SIM_LODS))
    return changed


if __name__ == '__main__':
    def _log(message):
        unreal.log('GAMER_MH: ' + message)
    total = 0
    for name in os.environ.get('GAMER_MH_FOLDERS', 'Kraeftig_Mid,Schmal_Mid').split(','):
        total += enable_card_simulation(ROOT + '/' + name.strip(), _log)
    _log('hair simulation enabled on %d groom(s)' % total)
    _log('done, 0 error(s)')
