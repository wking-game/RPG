"""Build the asset showcase map /Game/Gamer/Maps/Showcase (headless, repeatable).

All static meshes under /Game/Gamer are lined up in rows (one row per folder) with a name label that is
readable from above, so they can be looked at in-game from the bird view (user wish 2026-11-14).
The rows start 35 m from the PlayerStart, outside the start area where the server spawns resource nodes.

Run via tools/build_showcase.ps1. View: tools/mp_test.ps1 -Map /Game/Gamer/Maps/Showcase -Render with
test steps bird:on;birdfocus:at:<x>,<y>;birdzoom:<cm>;shot (the script prints a suggested step list).
"""
import unreal

MAP = '/Game/Gamer/Maps/Showcase'
TEMPLATE = '/Engine/Maps/Templates/Template_Default'
ROOT = '/Game/Gamer'
GROUND_MATERIAL = '/Game/Gamer/Materials/M_Gamer_Ground'
FLOOR_SCALE = unreal.Vector(25.0, 25.0, 8.0)
START_X = 3500.0        # first row 35 m in front of the PlayerStart
GAP = 250.0             # free space between objects (cm)
ROW_GAP = 600.0
LABEL_TAG = 'GamerShowcase'

errors = []


def log(msg):
    unreal.log('GAMER_SHOWCASE: ' + msg)


def error(msg):
    errors.append(msg)
    unreal.log_error('GAMER_SHOWCASE: ERROR ' + msg)


def actor_subsystem():
    return unreal.get_editor_subsystem(unreal.EditorActorSubsystem)


def load_or_create_map():
    if not unreal.EditorAssetLibrary.does_asset_exist(MAP):
        world = unreal.EditorLoadingAndSavingUtils.load_map(TEMPLATE)
        if not unreal.EditorLoadingAndSavingUtils.save_map(world, MAP):
            error('cannot create %s' % MAP)
            return None
        log('created %s' % MAP)
    return unreal.EditorLoadingAndSavingUtils.load_map(MAP)


def setup_floor():
    for actor in actor_subsystem().get_all_level_actors():
        if isinstance(actor, unreal.StaticMeshActor) and actor.get_actor_label().startswith('Floor'):
            actor.set_actor_scale3d(FLOOR_SCALE)
            if unreal.EditorAssetLibrary.does_asset_exist(GROUND_MATERIAL):
                actor.static_mesh_component.set_material(0, unreal.load_asset(GROUND_MATERIAL))


def clear_previous():
    removed = 0
    for actor in actor_subsystem().get_all_level_actors():
        if actor.actor_has_tag(LABEL_TAG):
            actor_subsystem().destroy_actor(actor)
            removed += 1
    if removed:
        log('removed %d actors of the previous showcase' % removed)


def collect_meshes():
    """{folder: [StaticMesh]} for all static meshes under ROOT, sorted."""
    registry = unreal.AssetRegistryHelpers.get_asset_registry()
    data = registry.get_assets_by_path(ROOT, recursive=True)
    groups = {}
    for item in data:
        if str(item.asset_class_path.asset_name) != 'StaticMesh':
            continue
        folder = str(item.package_path)[len(ROOT) + 1:] or '(root)'
        groups.setdefault(folder, []).append(str(item.package_name))
    result = {}
    for folder in sorted(groups):
        result[folder] = [unreal.load_asset(path) for path in sorted(groups[folder])]
    return result


def add_label(text, location, size=45.0, color=unreal.Color(255, 235, 170, 255)):
    label = actor_subsystem().spawn_actor_from_class(unreal.TextRenderActor, location)
    component = label.text_render
    component.set_text(text)
    component.set_world_size(size)
    component.set_horizontal_alignment(unreal.HorizTextAligment.EHTA_CENTER)
    component.set_text_render_color(color)
    # Lying flat, readable from above with the bird camera looking north (+X up on screen).
    label.set_actor_rotation(unreal.Rotator(roll=0.0, pitch=90.0, yaw=180.0), False)
    label.tags = [LABEL_TAG]
    label.set_actor_label('Label_' + text.replace(' ', '_')[:40])
    return label


def place(groups):
    x = START_X
    steps = []
    for folder, meshes in groups.items():
        sizes = []
        for mesh in meshes:
            box = mesh.get_bounding_box()
            sizes.append((box.max.x - box.min.x, box.max.y - box.min.y))
        row_depth = max(s[0] for s in sizes) if sizes else 0.0
        row_width = sum(s[1] for s in sizes) + GAP * max(0, len(sizes) - 1)
        y = -row_width / 2.0
        add_label(folder.replace('/', ' / '), unreal.Vector(x, y - 400.0, 2.0), size=80.0, color=unreal.Color(255, 255, 255, 255))
        for mesh, (depth, width) in zip(meshes, sizes):
            box = mesh.get_bounding_box()
            center_y = y + width / 2.0
            # Put the mesh so its bounds start at x and its bounds center sits on center_y.
            loc = unreal.Vector(x - box.min.x, center_y - (box.min.y + box.max.y) / 2.0, 0.0)
            # spawn_actor_from_object returns None in the commandlet, so spawn a StaticMeshActor and set the mesh.
            actor = actor_subsystem().spawn_actor_from_class(unreal.StaticMeshActor, loc)
            actor.static_mesh_component.set_static_mesh(mesh)
            actor.tags = [LABEL_TAG]
            actor.set_actor_label('Show_' + mesh.get_name())
            add_label(mesh.get_name().replace('SM_', ''), unreal.Vector(x - 80.0, center_y, 2.0))
            y += width + GAP
        log('row %-20s %2d meshes at x=%.0f width %.0f m' % (folder, len(meshes), x, row_width / 100.0))
        steps.append((folder, x + row_depth / 2.0, row_width))
        x += row_depth + ROW_GAP
    return steps


def main():
    world = load_or_create_map()
    if world:
        setup_floor()
        clear_previous()
        groups = collect_meshes()
        total = sum(len(m) for m in groups.values())
        log('%d static meshes in %d folders' % (total, len(groups)))
        steps = place(groups)
        for folder, cx, width in steps:
            zoom = max(1200.0, min(8000.0, width * 0.75))
            log('view %s: birdfocus:at:%.0f,0;birdzoom:%.0f;birdturn:0,-60' % (folder, cx, zoom))
        if not errors and unreal.EditorLoadingAndSavingUtils.save_map(world, MAP):
            log('saved %s' % MAP)
    log('done, %d error(s)' % len(errors))


main()
