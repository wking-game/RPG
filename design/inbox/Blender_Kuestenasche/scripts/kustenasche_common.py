"""Gemeinsame Blender-Helfer für prozedurale Küstenasche-Assets."""

import math

import bpy
from mathutils import Vector


ROOT_COLLECTION_NAME = "KA_CharacterSet"


def _object_mode():
    if bpy.context.object and bpy.context.object.mode != "OBJECT":
        bpy.ops.object.mode_set(mode="OBJECT")


def prepare_scene():
    """Setzt reproduzierbare Einheiten, ohne andere Szenenobjekte zu löschen."""
    _object_mode()
    scene = bpy.context.scene
    scene.unit_settings.system = "METRIC"
    scene.unit_settings.scale_length = 1.0
    scene.unit_settings.length_unit = "METERS"
    return ensure_collection(ROOT_COLLECTION_NAME, bpy.context.scene.collection)


def ensure_collection(name, parent):
    # Unterordner sind parent-lokal; mehrere Assets dürfen nicht dieselbe GEO/RIG-Collection teilen.
    collection = parent.children.get(name)
    if collection is None:
        collection = bpy.data.collections.new(name)
        parent.children.link(collection)
    return collection


def _remove_collection_tree(collection):
    for child in list(collection.children):
        _remove_collection_tree(child)
    for obj in list(collection.objects):
        data = obj.data
        bpy.data.objects.remove(obj, do_unlink=True)
        if isinstance(data, bpy.types.Mesh) and data.users == 0:
            bpy.data.meshes.remove(data)
        elif isinstance(data, bpy.types.Armature) and data.users == 0:
            bpy.data.armatures.remove(data)
    bpy.data.collections.remove(collection, do_unlink=True)


def new_asset_collection(name):
    """Ersetzt bei erneutem Skriptlauf nur diese Asset-Collection."""
    _object_mode()
    root = prepare_scene()
    old = bpy.data.collections.get(name)
    if old is not None:
        _remove_collection_tree(old)
    return ensure_collection(name, root)


def _set_input(node, name, value):
    socket = node.inputs.get(name)
    if socket is not None:
        socket.default_value = value


def weathered_material(name, color, metallic, roughness, noise_scale=95.0,
                       bump_strength=0.12, bump_distance=0.0015,
                       roughness_low=None, roughness_high=None):
    """Erzeugt ein nicht glänzendes PBR-Material mit Rauheits- und Bump-Noise."""
    mat = bpy.data.materials.get(name)
    if mat is None:
        mat = bpy.data.materials.new(name)
    mat.diffuse_color = (*color, 1.0)
    mat.use_nodes = True
    nodes = mat.node_tree.nodes
    links = mat.node_tree.links
    nodes.clear()

    output = nodes.new("ShaderNodeOutputMaterial")
    output.location = (620, 40)
    shader = nodes.new("ShaderNodeBsdfPrincipled")
    shader.location = (360, 40)
    _set_input(shader, "Base Color", (*color, 1.0))
    _set_input(shader, "Metallic", metallic)
    _set_input(shader, "Roughness", roughness)

    if metallic >= 0.45:
        # Cycles-Pointiness bringt einen dezenten Abriebton an scharfen Kanten hervor.
        geometry = nodes.new("ShaderNodeNewGeometry")
        geometry.location = (-820, 390)
        edge_mask = nodes.new("ShaderNodeValToRGB")
        edge_mask.location = (-590, 390)
        edge_mask.color_ramp.elements[0].position = 0.26
        edge_mask.color_ramp.elements[0].color = (0.0, 0.0, 0.0, 1.0)
        edge_mask.color_ramp.elements[1].position = 0.58
        edge_mask.color_ramp.elements[1].color = (1.0, 1.0, 1.0, 1.0)
        links.new(geometry.outputs["Pointiness"], edge_mask.inputs["Fac"])

        edge_amount = nodes.new("ShaderNodeMath")
        edge_amount.operation = "MULTIPLY"
        edge_amount.location = (-350, 390)
        edge_amount.inputs[1].default_value = 0.18
        links.new(edge_mask.outputs["Color"], edge_amount.inputs[0])

        edge_tint = tuple(min(1.0, channel * 1.62 + 0.025) for channel in color) + (1.0,)
        color_mix = nodes.new("ShaderNodeMixRGB")
        color_mix.blend_type = "MIX"
        color_mix.location = (80, 200)
        color_mix.inputs[1].default_value = (*color, 1.0)
        color_mix.inputs[2].default_value = edge_tint
        links.new(edge_amount.outputs[0], color_mix.inputs[0])
        links.new(color_mix.outputs["Color"], shader.inputs["Base Color"])

    tex = nodes.new("ShaderNodeTexCoord")
    tex.location = (-800, 80)
    noise = nodes.new("ShaderNodeTexNoise")
    noise.location = (-580, 80)
    _set_input(noise, "Scale", noise_scale)
    _set_input(noise, "Detail", 3.0)
    _set_input(noise, "Roughness", 0.72)
    links.new(tex.outputs["Generated"], noise.inputs["Vector"])

    ramp = nodes.new("ShaderNodeValToRGB")
    ramp.location = (-330, 180)
    low = roughness if roughness_low is None else roughness_low
    high = min(0.96, roughness + 0.16) if roughness_high is None else roughness_high
    ramp.color_ramp.elements[0].position = 0.15
    ramp.color_ramp.elements[0].color = (low, low, low, 1.0)
    ramp.color_ramp.elements[1].position = 0.85
    ramp.color_ramp.elements[1].color = (high, high, high, 1.0)
    links.new(noise.outputs["Fac"], ramp.inputs["Fac"])
    links.new(ramp.outputs["Color"], shader.inputs["Roughness"])

    bump = nodes.new("ShaderNodeBump")
    bump.location = (100, -150)
    _set_input(bump, "Strength", bump_strength)
    _set_input(bump, "Distance", bump_distance)
    links.new(noise.outputs["Fac"], bump.inputs["Height"])
    links.new(bump.outputs["Normal"], shader.inputs["Normal"])
    links.new(shader.outputs["BSDF"], output.inputs["Surface"])
    return mat


def create_mesh(name, vertices, faces, material, collection, smooth=True):
    mesh = bpy.data.meshes.new(name + "_Mesh")
    mesh.from_pydata(vertices, [], faces)
    mesh.update()
    obj = bpy.data.objects.new(name, mesh)
    collection.objects.link(obj)
    if material is not None:
        mesh.materials.append(material)
    for polygon in mesh.polygons:
        polygon.use_smooth = smooth
    return obj


def lathe(name, profile, material, collection, segments=24,
          center=(0.0, 0.0), cap=True, smooth=True):
    """Erzeugt ein zentriertes Rotationsprofil: (z, RadiusX, RadiusY)."""
    cx, cy = center
    vertices = []
    faces = []
    for z, rx, ry in profile:
        for i in range(segments):
            angle = 2.0 * math.pi * i / segments
            vertices.append((cx + rx * math.cos(angle),
                             cy + ry * math.sin(angle), z))
    for row in range(len(profile) - 1):
        for i in range(segments):
            a = row * segments + i
            b = row * segments + (i + 1) % segments
            c = (row + 1) * segments + (i + 1) % segments
            d = (row + 1) * segments + i
            faces.append((a, b, c, d))
    if cap:
        faces.append(tuple(reversed(range(segments))))
        last = (len(profile) - 1) * segments
        faces.append(tuple(last + i for i in range(segments)))
    return create_mesh(name, vertices, faces, material, collection, smooth)


def ellipsoid(name, location, scale, material, collection,
              segments=16, rings=10, smooth=True):
    x, y, z = location
    sx, sy, sz = scale
    profile = []
    for ring in range(rings + 1):
        phi = -0.5 * math.pi + math.pi * ring / rings
        radius = max(0.00015, math.cos(phi))
        profile.append((z + sz * math.sin(phi), sx * radius, sy * radius))
    return lathe(name, profile, material, collection, segments, (x, y), True, smooth)


def swept_tube(name, points, radii, material, collection, sides=10,
               cyclic=False, smooth=True):
    """Quad-basierter Rohrzug entlang einer Punktfolge; Radius skalar oder je Punkt."""
    points = [Vector(p) for p in points]
    if len(points) < 2:
        raise ValueError("swept_tube benötigt mindestens zwei Punkte")
    if isinstance(radii, (int, float)):
        radii = [float(radii)] * len(points)
    if len(radii) != len(points):
        raise ValueError("Für jeden Rohrpunkt wird ein Radius benötigt")

    vertices = []
    for i, point in enumerate(points):
        if cyclic:
            tangent = (points[(i + 1) % len(points)] - points[(i - 1) % len(points)]).normalized()
        elif i == 0:
            tangent = (points[1] - points[0]).normalized()
        elif i == len(points) - 1:
            tangent = (points[-1] - points[-2]).normalized()
        else:
            tangent = (points[i + 1] - points[i - 1]).normalized()
        reference = Vector((0.0, 0.0, 1.0))
        if abs(tangent.dot(reference)) > 0.92:
            reference = Vector((0.0, 1.0, 0.0))
        axis_a = tangent.cross(reference).normalized()
        axis_b = tangent.cross(axis_a).normalized()
        for side in range(sides):
            angle = 2.0 * math.pi * side / sides
            offset = radii[i] * (math.cos(angle) * axis_a + math.sin(angle) * axis_b)
            vertices.append(tuple(point + offset))

    faces = []
    span_count = len(points) if cyclic else len(points) - 1
    for row in range(span_count):
        next_row = (row + 1) % len(points)
        for side in range(sides):
            next_side = (side + 1) % sides
            a = row * sides + side
            b = row * sides + next_side
            c = next_row * sides + next_side
            d = next_row * sides + side
            faces.append((a, b, c, d))
    return create_mesh(name, vertices, faces, material, collection, smooth)


def smart_uv(obj, island_margin=0.012):
    _object_mode()
    bpy.ops.object.select_all(action="DESELECT")
    obj.select_set(True)
    bpy.context.view_layer.objects.active = obj
    if not obj.data.uv_layers:
        obj.data.uv_layers.new(name="UVMap")
    bpy.ops.object.mode_set(mode="EDIT")
    bpy.ops.mesh.select_all(action="SELECT")
    bpy.ops.uv.smart_project(island_margin=island_margin,
                             angle_limit=math.radians(66.0))
    bpy.ops.object.mode_set(mode="OBJECT")


def finalize_asset(collection, asset_name, origin=(0.0, 0.0, 0.0),
                   uv_margin=0.012):
    """Fügt Mesh-Inseln zu einem exportierbaren Objekt zusammen und setzt den Pivot."""
    _object_mode()
    meshes = [obj for obj in collection.objects if obj.type == "MESH"]
    if not meshes:
        raise RuntimeError("Keine Mesh-Objekte in Collection: " + collection.name)
    bpy.ops.object.select_all(action="DESELECT")
    for obj in meshes:
        obj.select_set(True)
    active = meshes[0]
    bpy.context.view_layer.objects.active = active
    source_meshes = [obj.data for obj in meshes]
    if len(meshes) > 1:
        bpy.ops.object.join()
    for mesh_data in source_meshes:
        if mesh_data is not active.data and mesh_data.users == 0:
            bpy.data.meshes.remove(mesh_data)
    active.name = asset_name
    active.data.name = asset_name + "_Mesh"
    active.location = (0.0, 0.0, 0.0)
    active.rotation_euler = (0.0, 0.0, 0.0)
    active.scale = (1.0, 1.0, 1.0)
    active["asset_id"] = asset_name
    active["units"] = "meters; 1 Blender Unit = 1 meter"
    active["front_axis"] = "-Y"
    active["topology_note"] = "Quad-dominant mesh; inspect cap/ngon and slit boundaries before production export."

    scene = bpy.context.scene
    scene.cursor.location = origin
    bpy.ops.object.origin_set(type="ORIGIN_CURSOR")
    smart_uv(active, uv_margin)
    return active


def standard_bone_specs():
    """Restpose für 1,75-m-Figur; A-Pose, Vorderseite -Y, Z-Up."""
    specs = [
        ("root", (0.0, 0.0, 0.00), (0.0, 0.0, 0.10), None, False),
        ("pelvis", (0.0, 0.0, 0.88), (0.0, 0.0, 1.04), "root", True),
        ("spine_01", (0.0, 0.0, 1.00), (0.0, 0.0, 1.18), "pelvis", True),
        ("spine_02", (0.0, 0.0, 1.18), (0.0, 0.0, 1.35), "spine_01", True),
        ("spine_03", (0.0, 0.0, 1.35), (0.0, 0.0, 1.47), "spine_02", True),
        ("neck_01", (0.0, 0.0, 1.47), (0.0, 0.0, 1.59), "spine_03", True),
        ("head", (0.0, 0.0, 1.59), (0.0, 0.0, 1.73), "neck_01", True),
    ]
    for side, sign in (("l", 1.0), ("r", -1.0)):
        shoulder = (sign * 0.185, 0.0, 1.445)
        elbow = (sign * 0.385, 0.0, 1.285)
        wrist = (sign * 0.535, -0.005, 1.135)
        knuckle = (sign * 0.585, -0.010, 1.105)
        hip = (sign * 0.092, 0.0, 0.925)
        knee = (sign * 0.098, 0.0, 0.535)
        ankle = (sign * 0.100, 0.0, 0.105)
        toe = (sign * 0.100, -0.155, 0.055)
        specs.extend([
            ("clavicle_" + side, (0.0, 0.0, 1.455), shoulder, "spine_03", True),
            ("upperarm_" + side, shoulder, elbow, "clavicle_" + side, True),
            ("lowerarm_" + side, elbow, wrist, "upperarm_" + side, True),
            ("hand_" + side, wrist, knuckle, "lowerarm_" + side, True),
            ("thigh_" + side, hip, knee, "pelvis", True),
            ("calf_" + side, knee, ankle, "thigh_" + side, True),
            ("foot_" + side, ankle, toe, "calf_" + side, True),
        ])
    return specs


def create_standard_armature(name, collection):
    """Erzeugt oder verwendet dasselbe benannte A-Pose-Skelett."""
    _object_mode()
    armature = bpy.data.objects.get(name)
    if armature is not None:
        if armature.type != "ARMATURE":
            raise TypeError(name + " existiert bereits und ist kein Armature-Objekt")
        if collection.objects.get(armature.name) is None:
            collection.objects.link(armature)
        return armature

    data = bpy.data.armatures.new(name + "_Data")
    armature = bpy.data.objects.new(name, data)
    collection.objects.link(armature)
    bpy.ops.object.select_all(action="DESELECT")
    armature.select_set(True)
    bpy.context.view_layer.objects.active = armature
    bpy.ops.object.mode_set(mode="EDIT")
    edit_bones = data.edit_bones
    created = {}
    for bone_name, head, tail, parent_name, use_deform in standard_bone_specs():
        bone = edit_bones.new(bone_name)
        bone.head = head
        bone.tail = tail
        created[bone_name] = (bone, parent_name, use_deform)
    for bone, parent_name, _use_deform in created.values():
        if parent_name:
            bone.parent = created[parent_name][0]
            bone.use_connect = (bone.parent.tail - bone.head).length < 0.0001
    bpy.ops.object.mode_set(mode="OBJECT")
    for bone_name, _head, _tail, _parent, use_deform in standard_bone_specs():
        data.bones[bone_name].use_deform = use_deform
    armature.show_in_front = True
    armature.display_type = "WIRE"
    armature["rig_convention"] = "KA_1.75m_A_Pose_ZUp_front_-Y"
    return armature


def bind_mesh_to_armature(obj, armature, bone_names=None, max_influences=3):
    """Gewichtet Quadflächen nach Abstand zu den Restpose-Knochen-Segmenten."""
    if obj.type != "MESH" or armature.type != "ARMATURE":
        raise TypeError("bind_mesh_to_armature erwartet Mesh und Armature")
    specs = standard_bone_specs()
    allowed = set(bone_names) if bone_names is not None else None
    deform_specs = [spec for spec in specs
                    if spec[4] and (allowed is None or spec[0] in allowed)]
    groups = {spec[0]: obj.vertex_groups.get(spec[0]) or
              obj.vertex_groups.new(name=spec[0]) for spec in deform_specs}

    def point_segment_distance(point, start, end):
        direction = end - start
        length_sq = direction.length_squared
        if length_sq <= 1e-12:
            return (point - start).length
        t = max(0.0, min(1.0, (point - start).dot(direction) / length_sq))
        return (point - (start + t * direction)).length

    for vertex in obj.data.vertices:
        point = obj.matrix_world @ vertex.co
        distances = []
        for spec in deform_specs:
            bone_name, head, tail = spec[0], Vector(spec[1]), Vector(spec[2])
            distances.append((point_segment_distance(point, head, tail), bone_name))
        closest = sorted(distances, key=lambda item: item[0])[:max_influences]
        raw = [(name, 1.0 / ((distance + 0.025) ** 4))
               for distance, name in closest]
        total = sum(weight for _name, weight in raw) or 1.0
        for bone_name, weight in raw:
            groups[bone_name].add([vertex.index], weight / total, "REPLACE")

    modifier = obj.modifiers.new("Armature_Deform", "ARMATURE")
    modifier.object = armature
    modifier.use_vertex_groups = True
    obj.parent = armature
    obj.matrix_parent_inverse = armature.matrix_world.inverted()
    obj["skeleton"] = armature.name
    return modifier

