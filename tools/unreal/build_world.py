"""Build the game world map /Game/Gamer/Maps/World (headless, repeatable).

Steps:
  1. Map: copy of /Engine/Maps/Templates/Template_Default (light, sky, fog, PlayerStart, floor),
     floor enlarged so the PCG layers and the start area fit; floor material M_Gamer_Ground
     (tools/unreal/build_ground_material.py).
  2. PCG: graph asset /Game/Gamer/PCG/PCG_WorldVegetation and a PCG volume with the
     vegetation layers (see build_pcg()). Generation happens afterwards in the PCG builder
     commandlet (second step of tools/build_world.ps1).

Run via tools/build_world.ps1 (UnrealEditor-Cmd, -run=pythonscript). Editor must be closed.
Prints GAMER_WORLD lines; the last one is "GAMER_WORLD: done, <n> error(s)".
"""
import os
import sys

import unreal

# build_ground_material.py lies next to this script; it also holds the noise fields shared with the PCG layers.
sys.path.insert(0, os.path.join(os.path.abspath(unreal.Paths.project_dir()), '..', 'tools', 'unreal'))
import build_ground_material as ground  # noqa: E402

MAP = '/Game/Gamer/Maps/World'
TEMPLATE = '/Engine/Maps/Templates/Template_Default'

# Template floor: SM_Template_Map_Floor (10 m x 10 m) at scale 8 = 80 m x 80 m.
# 25 = 250 m x 250 m; Z stays 8 (thickness, top stays at Z=0).
FLOOR_LABEL = 'Floor'
FLOOR_SCALE = unreal.Vector(25.0, 25.0, 8.0)

errors = []


def log(msg):
    unreal.log('GAMER_WORLD: ' + msg)


def error(msg):
    errors.append(msg)
    unreal.log_error('GAMER_WORLD: ERROR ' + msg)


def actors():
    return unreal.get_editor_subsystem(unreal.EditorActorSubsystem).get_all_level_actors()


def ensure_map():
    eal = unreal.EditorAssetLibrary
    if eal.does_asset_exist(MAP):
        log('map exists, updating %s' % MAP)
    else:
        world = unreal.EditorLoadingAndSavingUtils.load_map(TEMPLATE)
        if not world:
            error('cannot load template %s' % TEMPLATE)
            return None
        if not unreal.EditorLoadingAndSavingUtils.save_map(world, MAP):
            error('cannot save template copy as %s' % MAP)
            return None
        log('created %s from %s' % (MAP, TEMPLATE))
    world = unreal.EditorLoadingAndSavingUtils.load_map(MAP)
    if not world:
        error('cannot load %s' % MAP)
    return world


def setup_floor():
    floors = [a for a in actors() if isinstance(a, unreal.StaticMeshActor) and a.get_actor_label().startswith(FLOOR_LABEL)]
    if len(floors) != 1:
        error('expected one floor actor, found %d' % len(floors))
        return
    floor = floors[0]
    floor.set_actor_scale3d(FLOOR_SCALE)
    material = ground.build()
    for message in ground.errors:
        error('ground material: ' + message)
    if material:
        floor.static_mesh_component.set_material(0, material)
        log('floor material %s' % material.get_path_name())
    origin, extent = floor.get_actor_bounds(False)
    log('floor %s scale=(%.0f,%.0f,%.0f) size=%.0f m x %.0f m top=%.0f' % (
        floor.get_actor_label(), FLOOR_SCALE.x, FLOOR_SCALE.y, FLOOR_SCALE.z,
        extent.x / 50.0, extent.y / 50.0, origin.z + extent.z))


# ---------------------------------------------------------------------------
# PCG vegetation (pure decoration, no collision). Harvestable trees, bushes and
# rocks stay resource nodes spawned by the server (gameplay, replication).
#
# Generation cannot run inside -run=pythonscript (PCG executes in the subsystem
# tick). This script builds graph + volume; tools/build_world.ps1 then runs the
# PCGWorldPartitionBuilder commandlet, which generates and saves the map. The
# instances are saved with the map: identical on every machine, no runtime cost.
# ---------------------------------------------------------------------------
PCG_DIR = '/Game/Gamer/PCG'
GRAPH_NAME = 'PCG_WorldVegetation'
VOLUME_LABEL = 'PCG_Vegetation'
VOLUME_SCALE = unreal.Vector(120.0, 120.0, 5.0)  # PCG volume brush is 200 cm: 240 m x 240 m, Z -500..+500

# Grass (method from the PCG grass tutorial): few seed points, a grid copied onto each
# seed, random density + filter, scatter, size shrinks with distance to the seed.
AREA_HALF = 11800.0  # vegetation area +-118 m around the origin (inside the volume)

GRASS = {
    # 2026-11-13 first try: cell 70 cm = about 1 clump/m2 in a patch, barely visible in game.
    # 2026-11-14: seeds only on the "grass" noise field (meadows, same field tints the ground there earthy/green in
    # M_Gamer_Ground), about half the area. Seed spacing 1500 -> 1100 cm (1.86x seeds) so the number of patches stays
    # roughly the same (estimate, the count before was not measured; 2026-11-14: 87239 clumps).
    'seed_spacing': 1100.0,      # seed grid (jittered by half the spacing), filtered by the meadow field
    'grid_half_size': 500.0,     # each patch up to 10 m x 10 m
    'cell_size': 38.0,           # about 3.5 clumps/m2 inside a patch after filtering
    'offset': 150.0,             # random XY scatter per clump
    'keep': 0.5,                 # density filter lower bound (share of clumps dropped)
    'max_distance': 700.0,       # distance to seed where the size reaches its minimum
    'scale_near': 1.25, 'scale_far': 0.7,
    'cull_start': 4000, 'cull_end': 6000,
    'meshes': [  # (asset path, weight); missing assets are skipped with a warning
        ('/Game/Gamer/Nature/Grass/SM_GrassClump_Short', 4),
        ('/Game/Gamer/Nature/Grass/SM_GrassClump_Medium', 3),
        ('/Game/Gamer/Nature/Grass/SM_GrassClump_Tall', 1),
        ('/Game/Gamer/Nature/Grass/SM_GrassClump_Sparse', 2),
    ],
}

# Ground litter (last layer of the forest tutorial): small pebbles and twigs, clearly smaller
# than the harvestable stones/branches so nobody mistakes them for resources.
LITTER = {
    'spacing': 550.0,            # jittered grid, about 1 point per 30 m2 before filtering
    'keep': 0.6,
    'scale_min': 0.16, 'scale_max': 0.3,
    'sink': -2.0,
    'cull_start': 2000, 'cull_end': 3500,
    # Switched off 2026-11-14: players took the decorative pebbles/twigs for the loose stones and branches
    # they can pick up ("Man kann nicht alle Gegenstände ... aufheben"). Rule since then: nothing that looks
    # like a resource is scattered as pure decoration. The layer stays here for meshes that clearly differ.
    'meshes': [],
}


def set_props(obj, what, **props):
    for key, value in props.items():
        # Python names drop a leading "In" of C++ names (InRangeMin -> range_min), try both.
        candidates = [key] + ([key[3:]] if key.startswith('in_') else [])
        for index, name in enumerate(candidates):
            try:
                obj.set_editor_property(name, value)
                break
            except Exception as exc:  # wrong name/type: report, keep building the rest
                if index == len(candidates) - 1:
                    error('%s.%s: %s' % (what, key, exc))


# Attribute selectors are set from text ("$Density" = point property, "ScaleFactor" = attribute).
# The ScriptMethods set_point_property/set_attribute_name only work on the base struct and hit
# an ensure on the derived input/output selectors.
def _selector(cls, text):
    # Text form of the selector struct (see export_text()): PCGBegin(<selector>)PCGEnd
    sel = cls()
    sel.import_text('PCGBegin(%s)PCGEnd' % text)
    if sel.export_text() != 'PCGBegin(%s)PCGEnd' % text:
        error('selector %s: wanted %s, got %s' % (cls.__name__, text, sel.export_text()))
    return sel


def input_selector(text):
    return _selector(unreal.PCGAttributePropertyInputSelector, text)


def output_selector(text):
    return _selector(unreal.PCGAttributePropertyOutputSelector, text)


def existing_meshes(entries, layer):
    found = []
    for path, weight in entries:
        if unreal.EditorAssetLibrary.does_asset_exist(path):
            found.append((unreal.load_asset(path), weight))
        else:
            log('WARNING %s: mesh %s missing, skipped' % (layer, path))
    return found


def set_meshes(spawner, meshes, cull_start, cull_end, shadow=False, collision=False):
    spawner.set_mesh_selector_type(unreal.PCGMeshSelectorWeighted)
    selector = spawner.get_editor_property('mesh_selector_parameters')
    entries = []
    for mesh, weight in meshes:
        entry = unreal.PCGMeshSelectorWeightedEntry()
        desc = entry.get_editor_property('descriptor')  # PCG default: NoCollision, no overlap events
        set_props(desc, 'descriptor', static_mesh=mesh, instance_start_cull_distance=cull_start,
                  instance_end_cull_distance=cull_end, cast_shadow=shadow)
        if collision:
            # Blocking like a normal static mesh (UCX hulls of the mesh). The profile is applied again on load.
            body = desc.get_editor_property('body_instance')
            set_props(body, 'body_instance', collision_profile_name='BlockAll',
                      collision_enabled=unreal.CollisionEnabled.QUERY_AND_PHYSICS)
            desc.set_editor_property('body_instance', body)
        entry.set_editor_property('descriptor', desc)
        entry.set_editor_property('weight', weight)
        entries.append(entry)
    selector.set_editor_property('mesh_entries', entries)


class GraphBuilder:
    def __init__(self, graph):
        self.graph = graph

    def node(self, settings_class, x, y, **props):
        node, settings = self.graph.add_node_of_type(settings_class)
        node.set_node_position(x, y)
        set_props(settings, settings_class.__name__, **props)
        return node, settings

    def edge(self, src, src_pin, dst, dst_pin):
        if not self.graph.add_edge(src, src_pin, dst, dst_pin):
            error('edge %s.%s -> %s.%s failed' % (src.get_name(), src_pin, dst.get_name(), dst_pin))


def ensure_graph():
    path = '%s/%s' % (PCG_DIR, GRAPH_NAME)
    if unreal.EditorAssetLibrary.does_asset_exist(path):
        graph = unreal.load_asset(path)
        old = list(graph.get_editor_property('nodes'))
        for node in old:  # remove_nodes(TArray&) is an out-param in Python, so one by one
            graph.remove_node(node)
        log('graph %s cleared (%d nodes)' % (path, len(old)))
    else:
        tools = unreal.AssetToolsHelpers.get_asset_tools()
        graph = tools.create_asset(GRAPH_NAME, PCG_DIR, unreal.PCGGraph, unreal.PCGGraphFactory())
        log('graph %s created' % path)
    return graph


def field_node(b, name, x, y):
    """PCG Spatial Noise (Perlin2D) writing the noise field <name> of M_Gamer_Ground into the density."""
    scale, ox, oy, iterations = ground.FIELDS[name]
    node, _ = b.node(unreal.PCGSpatialNoiseSettings, x, y, mode=unreal.PCGSpatialNoiseMode.PERLIN2D,
                     iterations=iterations, random_offset=unreal.Vector(0.0, 0.0, 0.0),
                     transform=unreal.Transform(location=unreal.Vector(ox, oy, 0.0),
                                                scale=unreal.Vector(scale, scale, scale)))
    return node


def start_point(b, x, y):
    """One point at the player start (0,0): distance source for keeping the settlement area free."""
    point = unreal.PCGPoint()
    point.set_editor_property('transform', unreal.Transform(location=unreal.Vector(0.0, 0.0, 0.0)))
    node, _ = b.node(unreal.PCGCreatePointsSettings, x, y, points_to_create=[point],
                     coordinate_space=unreal.PCGCoordinateSpace.WORLD)
    return node


def build_graph(graph):
    b = GraphBuilder(graph)

    # Point sources: jittered grids on the flat floor (Z=0). Tested 2026-11-13: a "World Ray Hit
    # Query" surface yields no points inside the PCG builder commandlet, grids work. With a real
    # landscape later, sample the landscape data instead.

    # --- Layer 1: grass patches --------------------------------------------------
    grass_meshes = existing_meshes(GRASS['meshes'], 'grass')
    if grass_meshes:
        g = GRASS
        half = g['grid_half_size']
        # Seeds: jittered grid on the flat floor (Z=0). No ray casts needed; with a real
        # landscape later, sample the landscape data instead.
        spacing = g['seed_spacing']
        seed_grid, _ = b.node(unreal.PCGCreatePointsGridSettings, 0, -250,
                              grid_extents=unreal.Vector(AREA_HALF, AREA_HALF, 50.0),
                              cell_size=unreal.Vector(spacing, spacing, 100.0))
        seeds_all, _ = b.node(unreal.PCGTransformPointsSettings, 150, -250, seed=11,
                              offset_min=unreal.Vector(-spacing / 2, -spacing / 2, 0.0),
                              offset_max=unreal.Vector(spacing / 2, spacing / 2, 0.0))
        meadow = field_node(b, 'grass', 300, -400)
        seeds, _ = b.node(unreal.PCGDensityFilterSettings, 450, -250, lower_bound=ground.GRASS_THRESHOLD, upper_bound=1.0)
        grid, _ = b.node(unreal.PCGCreatePointsGridSettings, 300, 250,
                         grid_extents=unreal.Vector(half, half, 50.0),
                         cell_size=unreal.Vector(g['cell_size'], g['cell_size'], 100.0))
        copy, _ = b.node(unreal.PCGCopyPointsSettings, 600, 0)
        noise, _ = b.node(unreal.PCGAttributeNoiseSettings, 900, 0, seed=12,
                          mode=unreal.PCGAttributeNoiseMode.SET, noise_min=0.0, noise_max=1.0)
        filt, _ = b.node(unreal.PCGDensityFilterSettings, 1200, 0, lower_bound=g['keep'], upper_bound=1.0)
        scatter, _ = b.node(unreal.PCGTransformPointsSettings, 1500, 0, seed=13,
                            offset_min=unreal.Vector(-g['offset'], -g['offset'], -1.0),
                            offset_max=unreal.Vector(g['offset'], g['offset'], 0.0),
                            rotation_min=unreal.Rotator(roll=0.0, pitch=0.0, yaw=0.0),
                            rotation_max=unreal.Rotator(roll=0.0, pitch=0.0, yaw=360.0),
                            scale_min=unreal.Vector(0.85, 0.85, 0.85), scale_max=unreal.Vector(1.15, 1.15, 1.15))
        dist, _ = b.node(unreal.PCGDistanceSettings, 1800, 0, set_density=True,
                         maximum_distance=g['max_distance'],
                         source_shape=unreal.PCGDistanceShape.CENTER, target_shape=unreal.PCGDistanceShape.CENTER)
        # Density 0 at the seed .. 1 at max distance -> size factor near .. far.
        remap, _ = b.node(unreal.PCGAttributeRemapSettings, 2100, 0,
                          input_source=input_selector('$Density'),
                          in_range_min=0.0, in_range_max=1.0,
                          out_range_min=g['scale_near'], out_range_max=g['scale_far'],
                          allow_inverse_range=True,
                          output_target=output_selector('ScaleFactor'))
        mul, _ = b.node(unreal.PCGMetadataMathsSettings, 2400, 0,
                        operation=unreal.PCGMetadataMathsOperation.MULTIPLY,
                        input_source1=input_selector('$Scale'),
                        input_source2=input_selector('ScaleFactor'),
                        output_target=output_selector('$Scale'))
        spawner, spawner_settings = b.node(unreal.PCGStaticMeshSpawnerSettings, 2700, 0)
        set_meshes(spawner_settings, grass_meshes, g['cull_start'], g['cull_end'])

        b.edge(seed_grid, 'Out', seeds_all, 'In')
        b.edge(seeds_all, 'Out', meadow, 'In')
        b.edge(meadow, 'Out', seeds, 'In')
        b.edge(grid, 'Out', copy, 'Source')
        b.edge(seeds, 'Out', copy, 'Target')
        b.edge(copy, 'Out', noise, 'In')
        b.edge(noise, 'Out', filt, 'In')
        b.edge(filt, 'Out', scatter, 'In')
        b.edge(scatter, 'Out', dist, 'Source')
        b.edge(seeds, 'Out', dist, 'Target')
        b.edge(dist, 'Out', remap, 'In')
        b.edge(remap, 'Out', mul, 'InA')
        b.edge(remap, 'Out', mul, 'InB')
        b.edge(mul, 'Out', spawner, 'In')
        log('layer grass: %d mesh(es)' % len(grass_meshes))

    # --- Layer 2: ground litter (pebbles, twigs) --------------------------------
    litter_meshes = existing_meshes(LITTER['meshes'], 'litter')
    if litter_meshes:
        l = LITTER
        spacing = l['spacing']
        litter_grid, _ = b.node(unreal.PCGCreatePointsGridSettings, 0, 700,
                                grid_extents=unreal.Vector(AREA_HALF, AREA_HALF, 50.0),
                                cell_size=unreal.Vector(spacing, spacing, 100.0))
        sampler, _ = b.node(unreal.PCGTransformPointsSettings, 300, 700, seed=21,
                            offset_min=unreal.Vector(-spacing / 2, -spacing / 2, 0.0),
                            offset_max=unreal.Vector(spacing / 2, spacing / 2, 0.0))
        noise, _ = b.node(unreal.PCGAttributeNoiseSettings, 600, 700, seed=22,
                          mode=unreal.PCGAttributeNoiseMode.SET, noise_min=0.0, noise_max=1.0)
        filt, _ = b.node(unreal.PCGDensityFilterSettings, 900, 700, lower_bound=l['keep'], upper_bound=1.0)
        xform, _ = b.node(unreal.PCGTransformPointsSettings, 1200, 700, seed=23,
                          offset_min=unreal.Vector(0.0, 0.0, l['sink']), offset_max=unreal.Vector(0.0, 0.0, 0.0),
                          rotation_min=unreal.Rotator(roll=0.0, pitch=0.0, yaw=0.0),
                          rotation_max=unreal.Rotator(roll=0.0, pitch=0.0, yaw=360.0),
                          absolute_scale=True,
                          scale_min=unreal.Vector(l['scale_min'], l['scale_min'], l['scale_min']),
                          scale_max=unreal.Vector(l['scale_max'], l['scale_max'], l['scale_max']))
        spawner, spawner_settings = b.node(unreal.PCGStaticMeshSpawnerSettings, 1500, 700)
        set_meshes(spawner_settings, litter_meshes, l['cull_start'], l['cull_end'])
        b.edge(litter_grid, 'Out', sampler, 'In')
        b.edge(sampler, 'Out', noise, 'In')
        b.edge(noise, 'Out', filt, 'In')
        b.edge(filt, 'Out', xform, 'In')
        b.edge(xform, 'Out', spawner, 'In')
        log('layer litter: %d mesh(es)' % len(litter_meshes))

    build_ground_cover(b)
    build_cliffs(b)
    unreal.EditorAssetLibrary.save_loaded_asset(graph, False)


# Ferns as ground cover (forest tutorial: grid -> density noise -> filter -> random turn/size -> spawner).
# Clumps: only where the "moss" field of M_Gamer_Ground is high, so the ferns stand on the moss patches.
# Around the player start (settlement) the density falls to zero: P(keep) = 1 - keep/(d/radius), so nothing within
# keep * radius (9 m), few up to the radius (30 m). No collision.
# Not scattered (rule 2026-11-14, nothing that looks like a resource): stumps and lying dead wood (they become
# resource nodes), the CC0 shrubs shrub_02/shrub_04 (sparse twigs, read as branches).
GROUND_COVER = {
    'spacing': 190.0,
    'moss_min': ground.MOSS_THRESHOLD - 0.02,
    'start_radius': 3000.0,
    'keep': 0.3,
    'scale_min': 0.75, 'scale_max': 1.25,
    'sink': -3.0,
    'cull_start': 4500, 'cull_end': 6500,
    'meshes': [
        ('/Game/Gamer/Nature/Plants/SM_Fern_Large', 3),
        ('/Game/Gamer/Nature/Plants/SM_Fern_Medium', 3),
        ('/Game/Gamer/Nature/Plants/SM_Fern_Small', 2),
    ],
}

# Sandstone cliffs at the edge of the play area (terrain, not a rock resource), with collision.
# The scan is open at the back (in Unreal local -Y, the body extends from the pivot to -Y; the FBX import flips
# Blender's Y): yaw = angle + 90 turns the rock face to the centre and the open back to the map edge.
CLIFFS = {
    'radius': 11500.0,
    'walls': [  # (angle in degrees around the centre, number of pieces, base scale)
        (38.0, 3, 1.25),
        (163.0, 2, 1.1),
        (262.0, 3, 1.35),
    ],
    'mesh': '/Game/Gamer/Nature/Deco/SM_SandstoneCliff',
}


def build_ground_cover(b):
    meshes = existing_meshes(GROUND_COVER['meshes'], 'ground cover')
    if not meshes:
        return
    c = GROUND_COVER
    spacing = c['spacing']
    y = 1500
    grid, _ = b.node(unreal.PCGCreatePointsGridSettings, 0, y,
                     grid_extents=unreal.Vector(AREA_HALF, AREA_HALF, 50.0),
                     cell_size=unreal.Vector(spacing, spacing, 100.0))
    jitter, _ = b.node(unreal.PCGTransformPointsSettings, 300, y, seed=31,
                       offset_min=unreal.Vector(-spacing / 2, -spacing / 2, 0.0),
                       offset_max=unreal.Vector(spacing / 2, spacing / 2, 0.0))
    moss = field_node(b, 'moss', 600, y)
    moss_filter, _ = b.node(unreal.PCGDensityFilterSettings, 900, y, lower_bound=c['moss_min'], upper_bound=1.0)
    centre = start_point(b, 900, y + 250)
    dist, _ = b.node(unreal.PCGDistanceSettings, 1200, y, set_density=True, maximum_distance=c['start_radius'],
                     source_shape=unreal.PCGDistanceShape.CENTER, target_shape=unreal.PCGDistanceShape.CENTER)
    noise, _ = b.node(unreal.PCGAttributeNoiseSettings, 1500, y, seed=32,
                      mode=unreal.PCGAttributeNoiseMode.MULTIPLY, noise_min=0.0, noise_max=1.0)
    keep, _ = b.node(unreal.PCGDensityFilterSettings, 1800, y, lower_bound=c['keep'], upper_bound=1.0)
    xform, _ = b.node(unreal.PCGTransformPointsSettings, 2100, y, seed=33,
                      offset_min=unreal.Vector(-20.0, -20.0, c['sink']), offset_max=unreal.Vector(20.0, 20.0, 0.0),
                      rotation_min=unreal.Rotator(roll=-4.0, pitch=-4.0, yaw=0.0),
                      rotation_max=unreal.Rotator(roll=4.0, pitch=4.0, yaw=360.0),
                      absolute_scale=True,
                      scale_min=unreal.Vector(c['scale_min'], c['scale_min'], c['scale_min']),
                      scale_max=unreal.Vector(c['scale_max'], c['scale_max'], c['scale_max']))
    spawner, spawner_settings = b.node(unreal.PCGStaticMeshSpawnerSettings, 2400, y)
    set_meshes(spawner_settings, meshes, c['cull_start'], c['cull_end'], shadow=True)
    b.edge(grid, 'Out', jitter, 'In')
    b.edge(jitter, 'Out', moss, 'In')
    b.edge(moss, 'Out', moss_filter, 'In')
    b.edge(moss_filter, 'Out', dist, 'Source')
    b.edge(centre, 'Out', dist, 'Target')
    b.edge(dist, 'Out', noise, 'In')
    b.edge(noise, 'Out', keep, 'In')
    b.edge(keep, 'Out', xform, 'In')
    b.edge(xform, 'Out', spawner, 'In')
    log('layer ground cover (ferns): %d mesh(es)' % len(meshes))


def build_cliffs(b):
    import math
    meshes = existing_meshes([(CLIFFS['mesh'], 1)], 'cliffs')
    if not meshes:
        return
    points = []
    for angle, pieces, scale in CLIFFS['walls']:
        for index in range(pieces):
            # pieces side by side along the tangent, about 4 m apart (mesh is 4.9 m wide), slightly turned
            offset = (index - (pieces - 1) / 2.0) * 420.0 * scale
            a = math.radians(angle)
            r = CLIFFS['radius'] + (index % 2) * 120.0
            x = r * math.cos(a) - offset * math.sin(a)
            yy = r * math.sin(a) + offset * math.cos(a)
            yaw = angle + 90.0 + (index * 37 % 25 - 12)
            s = scale * (1.0 + (index * 53 % 30) / 100.0)
            point = unreal.PCGPoint()
            point.set_editor_property('transform', unreal.Transform(
                location=unreal.Vector(x, yy, -5.0), rotation=unreal.Rotator(roll=0.0, pitch=0.0, yaw=yaw),
                scale=unreal.Vector(s, s, s)))
            points.append(point)
    create, _ = b.node(unreal.PCGCreatePointsSettings, 0, 2300, points_to_create=points,
                       coordinate_space=unreal.PCGCoordinateSpace.WORLD)
    spawner, spawner_settings = b.node(unreal.PCGStaticMeshSpawnerSettings, 300, 2300)
    set_meshes(spawner_settings, meshes, 0, 0, shadow=True, collision=True)
    b.edge(create, 'Out', spawner, 'In')
    log('layer cliffs: %d piece(s)' % len(points))


def ensure_volume(graph):
    eas = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
    volumes = [a for a in eas.get_all_level_actors() if isinstance(a, unreal.PCGVolume) and a.get_actor_label() == VOLUME_LABEL]
    if volumes:
        volume = volumes[0]
    else:
        volume = eas.spawn_actor_from_class(unreal.PCGVolume, unreal.Vector(0.0, 0.0, 0.0))
        volume.set_actor_label(VOLUME_LABEL)
    volume.set_actor_location(unreal.Vector(0.0, 0.0, 0.0), False, False)
    volume.set_actor_scale3d(VOLUME_SCALE)
    comp = volume.get_editor_property('pcg_component')
    comp.set_graph(graph)
    set_props(comp, 'pcg_component', seed=42, is_component_partitioned=False,
              generation_trigger=unreal.PCGComponentGenerationTrigger.GENERATE_ON_LOAD)
    origin, extent = volume.get_actor_bounds(False)
    log('volume %s bounds x=+-%.0f y=+-%.0f z=%.0f..%.0f' % (
        VOLUME_LABEL, extent.x, extent.y, origin.z - extent.z, origin.z + extent.z))


def build_pcg(world):
    graph = ensure_graph()
    if graph:
        build_graph(graph)
        ensure_volume(graph)


def main():
    world = ensure_map()
    if world:
        setup_floor()
        build_pcg(world)
        if not errors:
            if unreal.EditorLoadingAndSavingUtils.save_map(world, MAP):
                log('saved %s' % MAP)
            else:
                error('save failed')
    log('done, %d error(s)' % len(errors))


main()
