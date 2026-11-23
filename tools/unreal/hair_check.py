"""
Cross-check for the hair (grooms): puts the blueprints that the MetaHuman assembly made (BP_<Name>, unchanged) into
an empty, unsaved test level with light and a camera in front of the heads, and takes a screenshot.
If their hair shows, the problem is in our game-side setup (GamerFigures::ApplyGrooms); if not, it is project or
render settings. Run in a full editor session (needs ticking frames):
  UnrealEditor.exe Gamer.uproject -ExecutePythonScript=tools/unreal/hair_check.py
Output: game/Saved/HairCheck/hair_check.png
"""
import os

import unreal

OUT = os.path.join(os.path.abspath(unreal.Paths.project_saved_dir()), 'HairCheck')
FIGURES = ['/Game/Gamer/Characters/MetaHuman/Kraeftig/BP_Kraeftig',
           '/Game/Gamer/Characters/MetaHuman/Bewohner_K1_Low/BP_Bewohner_K1_Low',
           '/Game/Gamer/Characters/MetaHuman/Bewohner_S1_Low/BP_Bewohner_S1_Low']
os.makedirs(OUT, exist_ok=True)


def log(message):
    unreal.log('GAMER_HAIR: ' + message)


les = unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
les.new_level('/Temp/HairCheck')
actors = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
sun = actors.spawn_actor_from_class(unreal.DirectionalLight, unreal.Vector(0, 0, 500), unreal.Rotator(0, -40, 160))
sun.light_component.set_editor_property('intensity', 8.0)
actors.spawn_actor_from_class(unreal.SkyLight, unreal.Vector(0, 0, 300))
actors.spawn_actor_from_class(unreal.SkyAtmosphere, unreal.Vector(0, 0, 0))
floor = actors.spawn_actor_from_class(unreal.StaticMeshActor, unreal.Vector(0, 0, 0))
floor.static_mesh_component.set_static_mesh(unreal.load_asset('/Engine/BasicShapes/Plane'))
floor.set_actor_scale3d(unreal.Vector(20, 20, 1))
for i, path in enumerate(FIGURES):
    cls = unreal.EditorAssetLibrary.load_blueprint_class(path)
    if cls is None:
        log('missing ' + path)
        continue
    a = actors.spawn_actor_from_class(cls, unreal.Vector(0, (i - 1) * 70, 0), unreal.Rotator(0, 0, 0))
    log('spawned %s' % a.get_name())
    for c in a.get_components_by_class(unreal.GroomComponent):
        log('  groom %s asset %s binding %s visible %s' % (c.get_name(), c.get_editor_property('groom_asset'),
            c.get_editor_property('binding_asset'), c.is_visible()))
# camera in front of the heads (MetaHumans face +Y in their blueprint? look from both sides: two shots)
cam = actors.spawn_actor_from_class(unreal.CameraActor, unreal.Vector(0, 0, 0))
shots = [(unreal.Vector(220, 0, 165), unreal.Rotator(-4, 180, 0), 'front_x'),
         (unreal.Vector(0, 0, 165), unreal.Rotator(0, 0, 0), 'unused')]
state = {'frame': 0, 'shot': 0}
# Scene capture into a render target (the editor viewport of an unattended session does not write screenshots)
world = unreal.EditorLevelLibrary.get_editor_world()
target = unreal.RenderingLibrary.create_render_target2d(world, 1280, 720, unreal.TextureRenderTargetFormat.RTF_RGBA8_SRGB)
capture = actors.spawn_actor_from_class(unreal.SceneCapture2D, unreal.Vector(0, 0, 0))
capture.capture_component2d.set_editor_property('texture_target', target)
capture.capture_component2d.set_editor_property('capture_source', unreal.SceneCaptureSource.SCS_FINAL_COLOR_LDR)
capture.capture_component2d.set_editor_property('fov_angle', 40.0)
capture.capture_component2d.set_editor_property('capture_every_frame', True)
views = [(unreal.Vector(220, 0, 160), unreal.Rotator(-3, 180, 0), 'pos_x'),
         (unreal.Vector(-220, 0, 160), unreal.Rotator(-3, 0, 0), 'neg_x'),
         (unreal.Vector(0, -260, 160), unreal.Rotator(-3, 90, 0), 'neg_y')]


def tick(delta):
    # Real time, not tick count: during loading/shader compiling slate ticks many times per frame.
    try:
        state['time'] = state.get('time', 0.0) + min(delta, 0.5)
        t = state['time']
        step = int((t - 20.0) // 8.0)  # 20 s to settle (shaders, streaming), then 8 s per view
        if t < 20.0 or step == state.get('step'):
            return
        state['step'] = step
        if step < len(views):
            loc, rot, name = views[step]
            capture.set_actor_location_and_rotation(loc, rot, False, False)
            comp = capture.capture_component2d
            comp.capture_scene()
            unreal.RenderingLibrary.export_render_target(world, target, OUT, 'hair_check_%s.png' % name)
            log('capture %s at %.1f s' % (name, t))
        elif step >= len(views) + 1:
            unreal.unregister_slate_post_tick_callback(handle)
            log('done')
            unreal.SystemLibrary.quit_editor()
    except Exception as error:
        log('tick error: %s' % error)
        unreal.unregister_slate_post_tick_callback(handle)
        unreal.SystemLibrary.quit_editor()


handle = unreal.register_slate_post_tick_callback(tick)
