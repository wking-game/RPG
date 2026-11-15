"""
Character figure and animations (Phase 2, step 1): import, retarget to the UE5 skeleton (SK_Mannequin).

Run headless (editor closed) with tools/import_characters.ps1. Repeatable: replaces existing assets.

Sources (see design/asset_log.md):
  /Game/Characters/Mannequins/...   Epic UE5 Mannequin (Manny/Quinn, idle/walk/jog/jump/fall/land), copied
                                    from the engine templates (E:/UE58/Templates/TemplateResources/High/Characters)
  D:/gamer/cc0_quellen/extern/kevin_iglesias_*      Kevin Iglesias FBX (sprint, gathering, mining, hammering)
  D:/gamer/cc0_quellen/extern/quaternius_*          Quaternius UAL2 glTF, CC0 (tree chopping, farming)

Creates
  /Game/Gamer/Characters/Source/Kevin/...      Kevin skeleton, model and source animations
  /Game/Gamer/Characters/Source/Quaternius/... Quaternius skeleton, mannequin and source animations
  /Game/Gamer/Characters/Retarget/IK_*, RTG_*  IK rigs and retargeters (FK chains, no IK goals)
  /Game/Gamer/Characters/Anims/A_*             final animations on SK_Mannequin (names used by UGamerAnimInstance)
Writes "GAMER_CHAR: ..." lines; exits with "GAMER_CHAR: done, N error(s)".
"""
import os

import unreal

EXTERN = 'D:/gamer/cc0_quellen/extern'
KEVIN_BASIC = EXTERN + '/kevin_iglesias_human_basic_motions_free'
KEVIN_CRAFT = EXTERN + '/kevin_iglesias_human_crafting_free/Animations/Male/Work'
QUAT = EXTERN + '/quaternius_universal_animation_library_2/Universal Animation Library 2[Standard]/Unreal-Godot'

ROOT = '/Game/Gamer/Characters'
SRC_KEVIN = ROOT + '/Source/Kevin'
SRC_QUAT = ROOT + '/Source/Quaternius'
RETARGET = ROOT + '/Retarget'
ANIMS = ROOT + '/Anims'
MANNY = '/Game/Characters/Mannequins/Meshes/SKM_Manny_Simple'

asset_tools = unreal.AssetToolsHelpers.get_asset_tools()
editor_assets = unreal.get_editor_subsystem(unreal.EditorAssetSubsystem)
errors = []

# Final name -> (source kind, file or animation name)
KEVIN_ANIMS = {
    'A_Sprint': KEVIN_BASIC + '/Animations/Male/Movement/Sprint/HumanM@Sprint01_Forward.fbx',
    'A_Gather': KEVIN_CRAFT + '/Gathering/HumanM@Gathering01.fbx',
    'A_Gather2': KEVIN_CRAFT + '/Gathering/HumanM@Gathering02.fbx',
    'A_Gather3': KEVIN_CRAFT + '/Gathering/HumanM@Gathering03.fbx',
    'A_Mine': KEVIN_CRAFT + '/Mining/HumanM@MiningOneHand01_R - Wall.fbx',
    'A_MineGround': KEVIN_CRAFT + '/Mining/HumanM@MiningOneHand01_R - Ground.fbx',
    'A_Hammer': KEVIN_CRAFT + '/Hammering/HumanM@HammeringGround01_R - Loop.fbx',
}
QUAT_ANIMS = {
    'A_Chop': 'TreeChopping_Loop',
    'A_PlantSeed': 'Farm_PlantSeed',
    'A_Harvest': 'Farm_Harvest',
    'A_Water': 'Farm_Watering',
    'A_Consume': 'Consume',
}

FINGERS_UE = [('Thumb', 'thumb'), ('Index', 'index'), ('Middle', 'middle'), ('Ring', 'ring'), ('Pinky', 'pinky')]
FINGERS_KEVIN = {'thumb': 'thumb0{n}', 'index': 'indexFinger0{n}', 'middle': 'middleFinger0{n}',
                 'ring': 'ringFinger0{n}', 'pinky': 'pinky0{n}'}


def log(message):
    unreal.log('GAMER_CHAR: ' + message)


def fail(message):
    errors.append(message)
    unreal.log_error('GAMER_CHAR: ' + message)


def ue_chains(spine_end, neck, head):
    """Chains for UE-style bone names (Manny, Quaternius)."""
    chains = [('Spine', 'spine_01', spine_end), ('Neck', neck[0], neck[1]), ('Head', head, head)]
    for side, s in (('Left', 'l'), ('Right', 'r')):
        chains += [(side + 'Clavicle', 'clavicle_' + s, 'clavicle_' + s),
                   (side + 'Arm', 'upperarm_' + s, 'hand_' + s),
                   (side + 'Leg', 'thigh_' + s, 'ball_' + s)]
        for chain, bone in FINGERS_UE:
            chains.append((side + chain, '%s_01_%s' % (bone, s), '%s_03_%s' % (bone, s)))
    return chains


def kevin_chains():
    """Kevin Iglesias rig; the FBX importer names the bones "B-<name>" with "_" instead of "."."""
    chains = [('Spine', 'spine', 'chest'), ('Neck', 'neck', 'neck'), ('Head', 'head', 'head')]
    for side, s in (('Left', 'L'), ('Right', 'R')):
        chains += [(side + 'Clavicle', 'shoulder_' + s, 'shoulder_' + s),
                   (side + 'Arm', 'upperArm_' + s, 'hand_' + s),
                   (side + 'Leg', 'thigh_' + s, 'toe_' + s)]
        for chain, bone in FINGERS_UE:
            pattern = FINGERS_KEVIN[bone]
            chains.append((side + chain, pattern.format(n=1) + '_' + s, pattern.format(n=3) + '_' + s))
    return [(c, 'B-' + a, 'B-' + b) for c, a, b in chains]


def import_fbx(path, destination, name, skeleton=None, mesh=True):
    options = unreal.FbxImportUI()
    options.set_editor_property('import_mesh', mesh)
    options.set_editor_property('import_as_skeletal', True)
    options.set_editor_property('import_materials', False)
    options.set_editor_property('import_textures', False)
    options.set_editor_property('import_animations', not mesh)
    options.set_editor_property('mesh_type_to_import',
                                unreal.FBXImportType.FBXIT_SKELETAL_MESH if mesh else unreal.FBXImportType.FBXIT_ANIMATION)
    if skeleton is not None:
        options.set_editor_property('skeleton', skeleton)
    if not mesh:
        anim = options.get_editor_property('anim_sequence_import_data')
        anim.set_editor_property('import_bone_tracks', True)
        anim.set_editor_property('remove_redundant_keys', False)
    task = unreal.AssetImportTask()
    task.set_editor_property('filename', path)
    task.set_editor_property('destination_path', destination)
    task.set_editor_property('destination_name', name)
    task.set_editor_property('replace_existing', True)
    task.set_editor_property('automated', True)
    task.set_editor_property('save', False)
    task.set_editor_property('options', options)
    asset_tools.import_asset_tasks([task])
    imported = list(task.get_editor_property('imported_object_paths') or [])
    if not imported:
        fail('import failed: ' + path)
    return imported


def first_of(paths, cls):
    for p in paths:
        a = unreal.load_asset(p)
        if isinstance(a, cls):
            return a
    return None


def create_fresh(name, path, cls, factory):
    """New asset, or the existing one (deleting does not work reliably in the commandlet); callers reset it."""
    full = path + '/' + name
    if editor_assets.does_asset_exist(full):
        return unreal.load_asset(full)
    return asset_tools.create_asset(name, path, cls, factory)


def make_ik_rig(name, mesh, root, chains):
    rig = create_fresh(name, RETARGET, unreal.IKRigDefinition, unreal.IKRigDefinitionFactory())
    if rig is None:
        raise RuntimeError('cannot create ' + name)
    ctrl = unreal.IKRigController.get_controller(rig)
    for existing in list(ctrl.get_retarget_chains()):
        ctrl.remove_retarget_chain(existing.chain_name)
    if not ctrl.set_skeletal_mesh(mesh):
        fail('%s: set_skeletal_mesh(%s) failed' % (name, mesh.get_name()))
    try:
        modifier = unreal.SkeletonModifier()
        modifier.set_skeletal_mesh(mesh)
        log('%s bones: %s' % (name, ', '.join(str(b) for b in modifier.get_all_bone_names())[:600]))
    except Exception as error:  # diagnostics only
        log('%s: bone list unavailable (%s)' % (name, error))
    # The FBX importer may turn "shoulder.L" into "shoulder_L": try both spellings.
    if not ctrl.set_retarget_root(root) and not ctrl.set_retarget_root(root.replace('.', '_')):
        fail('%s: retarget root %s not set' % (name, root))
    for chain, start, end in chains:
        if str(ctrl.add_retarget_chain(chain, start, end, '')) != 'None':
            continue
        if str(ctrl.add_retarget_chain(chain, start.replace('.', '_'), end.replace('.', '_'), '')) == 'None':
            fail('%s: chain %s (%s..%s) not added' % (name, chain, start, end))
    return rig


def make_retargeter(name, src_rig, src_mesh, tgt_rig, tgt_mesh):
    rtg = create_fresh(name, RETARGET, unreal.IKRetargeter, unreal.IKRetargetFactory())
    ctrl = unreal.IKRetargeterController.get_controller(rtg)
    S, T = unreal.RetargetSourceOrTarget.SOURCE, unreal.RetargetSourceOrTarget.TARGET
    ctrl.set_ik_rig(S, src_rig)
    ctrl.set_ik_rig(T, tgt_rig)
    ctrl.set_preview_mesh(S, src_mesh)
    ctrl.set_preview_mesh(T, tgt_mesh)
    ctrl.remove_all_ops()
    ctrl.add_default_ops()
    ctrl.assign_ik_rig_to_all_ops(S, src_rig)
    ctrl.assign_ik_rig_to_all_ops(T, tgt_rig)
    ctrl.auto_map_chains(unreal.AutoMapChainType.EXACT, True)
    # Manny stands in an A-pose; align the target's retarget pose to the source's rest pose.
    ctrl.remove_retarget_pose('Aligned', T)
    pose = ctrl.create_retarget_pose('Aligned', T)
    ctrl.set_current_retarget_pose(pose, T)
    ctrl.auto_align_all_bones(T, unreal.RetargetAutoAlignMethod.CHAIN_TO_CHAIN)
    return rtg


def retarget(anims, src_mesh, tgt_mesh, rtg, names):
    """Batch retarget, then move the results to ANIMS/<final name>."""
    inputs = unreal.IKRetargetBatchOperationInputs()
    inputs.set_editor_property('assets_to_retarget', [unreal.EditorAssetLibrary.find_asset_data(a.get_path_name()) for a in anims])
    inputs.set_editor_property('source_mesh', src_mesh)
    inputs.set_editor_property('target_mesh', tgt_mesh)
    inputs.set_editor_property('ik_retarget_asset', rtg)
    inputs.set_editor_property('target_path', ANIMS)
    inputs.set_editor_property('include_referenced_assets', False)
    inputs.set_editor_property('overwrite_existing_files', False)
    out = unreal.IKRetargetBatchOperation.run_batch_retarget(inputs)
    by_source = {}
    for data in out:
        asset = data.get_asset()
        if isinstance(asset, unreal.AnimSequence):
            by_source[str(data.asset_name)] = asset
    done = 0
    for anim, final in zip(anims, names):
        result = by_source.get(anim.get_name())
        if result is None:
            fail('retarget produced nothing for ' + anim.get_name())
            continue
        if result.get_name() != final or not result.get_path_name().startswith(ANIMS + '/'):
            fail('unexpected retarget result %s for %s' % (result.get_path_name(), final))
            continue
        result.set_editor_property('enable_root_motion', False)
        log('%s: %.2f s' % (final, result.get_play_length()))
        done += 1
    log('retargeted %d of %d (%s)' % (done, len(anims), rtg.get_name()))


# --- Target: Manny ----------------------------------------------------------------------------
manny = unreal.load_asset(MANNY)
if manny is None:
    fail('Manny missing: ' + MANNY + ' (copy the template Mannequins folder first)')
else:
    ik_manny = make_ik_rig('IK_Manny', manny, 'pelvis', ue_chains('spine_05', ('neck_01', 'neck_02'), 'head'))

    # --- Kevin Iglesias ---------------------------------------------------------------------------
    unreal.SystemLibrary.execute_console_command(None, 'Interchange.FeatureFlags.Import.FBX 0')
    paths = import_fbx(KEVIN_BASIC + '/Models/HumanM_Model.fbx', SRC_KEVIN, 'SKM_KevinHumanM')
    kevin_mesh = first_of(paths, unreal.SkeletalMesh)
    if kevin_mesh is None:
        fail('Kevin model not imported')
    else:
        kevin_skel = kevin_mesh.get_editor_property('skeleton')
        kevin_anims, kevin_names = [], []
        for final, path in KEVIN_ANIMS.items():
            got = import_fbx(path, SRC_KEVIN, final, kevin_skel, mesh=False)
            anim = first_of(got, unreal.AnimSequence)
            if anim is None:
                fail('no animation in ' + path)
                continue
            kevin_anims.append(anim)
            kevin_names.append(final)
        ik_kevin = make_ik_rig('IK_Kevin', kevin_mesh, 'B-hips', kevin_chains())
        rtg_kevin = make_retargeter('RTG_Kevin_To_Manny', ik_kevin, kevin_mesh, ik_manny, manny)
        retarget(kevin_anims, kevin_mesh, manny, rtg_kevin, kevin_names)
    unreal.SystemLibrary.execute_console_command(None, 'Interchange.FeatureFlags.Import.FBX 1')

    # --- Quaternius (glTF via Interchange) -------------------------------------------------------
    task = unreal.AssetImportTask()
    task.set_editor_property('filename', QUAT + '/UAL2_Standard.glb')
    task.set_editor_property('destination_path', SRC_QUAT)
    task.set_editor_property('replace_existing', True)
    task.set_editor_property('automated', True)
    task.set_editor_property('save', False)
    asset_tools.import_asset_tasks([task])
    qpaths = list(task.get_editor_property('imported_object_paths') or [])
    if not qpaths:
        # Interchange may not fill imported_object_paths: look in the folder instead.
        qpaths = [str(p).split('.')[0] for p in editor_assets.list_assets(SRC_QUAT, True, False)]
    quat_mesh = first_of(qpaths, unreal.SkeletalMesh)
    if quat_mesh is None:
        fail('Quaternius mesh not imported (%d assets)' % len(qpaths))
    else:
        all_q = [unreal.load_asset(p) for p in qpaths]
        q_by_name = {}
        for a in all_q:
            if isinstance(a, unreal.AnimSequence):
                q_by_name[a.get_name()] = a
        log('Quaternius: %d animations, e.g. %s' % (len(q_by_name), sorted(q_by_name)[:5]))
        q_anims, q_names = [], []
        for final, src in QUAT_ANIMS.items():
            match = next((a for n, a in q_by_name.items() if n == src or n.endswith('_' + src) or n.endswith(src)), None)
            if match is None:
                fail('Quaternius animation missing: ' + src)
                continue
            dest = SRC_QUAT + '/' + final
            copy = unreal.load_asset(dest) if editor_assets.does_asset_exist(dest) else editor_assets.duplicate_asset(match.get_path_name(), dest)
            if copy is None:
                fail('cannot copy %s to %s' % (match.get_name(), dest))
                continue
            q_anims.append(copy)
            q_names.append(final)
        ik_quat = make_ik_rig('IK_Quaternius', quat_mesh, 'pelvis', ue_chains('spine_03', ('neck_01', 'neck_01'), 'Head'))
        rtg_quat = make_retargeter('RTG_Quaternius_To_Manny', ik_quat, quat_mesh, ik_manny, manny)
        retarget(q_anims, quat_mesh, manny, rtg_quat, q_names)

# Pose check for choosing clips: lowest pelvis and right/left hand height (cm above the root) per clip.
CHAIN = ['pelvis', 'spine_01', 'spine_02', 'spine_03', 'spine_04', 'spine_05']
ARM = {'r': ['clavicle_r', 'upperarm_r', 'lowerarm_r', 'hand_r'], 'l': ['clavicle_l', 'upperarm_l', 'lowerarm_l', 'hand_l']}
for path in editor_assets.list_assets(ANIMS, False, False):
    anim = unreal.load_asset(str(path).split('.')[0])
    if not isinstance(anim, unreal.AnimSequence):
        continue
    length = anim.get_play_length()
    low_pelvis, low_hand, at = 1e9, 1e9, 0.0
    for i in range(25):
        t = length * i / 24.0
        def world(bones):
            tr = unreal.Transform()
            for b in bones:
                tr = unreal.AnimationLibrary.get_bone_pose_for_time(anim, b, t, False).multiply(tr)
            return tr
        pelvis = world(['pelvis'])
        low_pelvis = min(low_pelvis, pelvis.translation.z)
        for side in ARM:
            hand = world(CHAIN + ARM[side]).translation.z
            if hand < low_hand:
                low_hand, at = hand, t
    log('POSE %s: %.2f s, lowest pelvis %.0f cm, lowest hand %.0f cm at %.2f s' % (anim.get_name(), length, low_pelvis, low_hand, at))

editor_assets.save_directory(ROOT, only_if_is_dirty=False, recursive=True)
log('done, %d error(s)' % len(errors))
for e in errors:
    log('error: ' + e)
