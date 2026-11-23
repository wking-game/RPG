# Testkopf: prüft den Leitfaden design/leitfaden_blender_unreal_figuren.md (Abschnitt 5.3).
# Baut einen Spielkopf mit Ringen um Augen und Mund, getrennten Lippen, Augäpfeln.
# Ausführen in Blender: exec(open(r"...build_testkopf.py").read())
import bpy, bmesh, math
from mathutils import Vector

SC = bpy.data.scenes.get("Testkopf") or bpy.data.scenes.new("Testkopf")
bpy.context.window.scene = SC
for o in list(SC.objects):
    if o.name.startswith(("SM_Testkopf", "SK_Testkopf")):
        bpy.data.objects.remove(o, do_unlink=True)

G = lambda d2, s: math.exp(-d2 / (2 * s * s))
def sm(a, b, t):
    t = max(0.0, min(1.0, (t - a) / (b - a))); return t * t * (3 - 2 * t)
def center(vs): return sum((v.co for v in vs), Vector()) / len(vs)
def boundary_verts(bm): return {v for e in bm.edges if e.is_boundary for v in e.verts}
def rings_from(bm, start, n):
    rings = [set(start)]; seen = set(start)
    for _ in range(n):
        nxt = {e.other_vert(v) for v in rings[-1] for e in v.link_edges} - seen
        rings.append(nxt); seen |= nxt
    return rings

# 1) Grundkugel aus Quads (10x10 je Würfelseite), Gesicht zeigt nach -Y
bm = bmesh.new()
bmesh.ops.create_cube(bm, size=2.0)
bmesh.ops.subdivide_edges(bm, edges=bm.edges[:], cuts=9, use_grid_fill=True)
for v in bm.verts: v.co = v.co.normalized()

def front_patch(xr, zr):
    out = []
    for f in bm.faces:
        c = f.calc_center_median()
        if f.normal.y < -0.5:
            x = c.x / abs(c.y); z = c.z / abs(c.y)
            if xr[0] < x < xr[1] and zr[0] < z < zr[1]: out.append(f)
    return out

# 2) Ringe: Fläche dreimal einsetzen (inset) -> konzentrische Kantenschleifen, Mitte löschen
# Regel aus dem Test: Ringstärke x Anzahl muss deutlich unter der halben Lochhöhe bleiben,
# sonst kippt die Innenfläche und Ringe falten sich über die Öffnung.
for xr, zr in (((0.2, 0.6), (0.0, 0.2)), ((-0.6, -0.2), (0.0, 0.2)), ((-0.4, 0.4), (-0.6, -0.4))):
    p = front_patch(xr, zr)
    for _ in range(3):
        bmesh.ops.inset_region(bm, faces=p, thickness=0.02, depth=0.0, use_even_offset=True)
    bmesh.ops.delete(bm, geom=p, context='FACES')

# 3) Öffnungen formen (vor dem Glätten), Verschiebung auf 4 Ringe abklingen lassen
def shape_hole(verts, target_fn, falloff=0.6, n=4):
    c = center(verts)
    deltas = []
    for v in verts:
        t = target_fn(v.co, c); a = math.atan2(v.co.z - c.z, v.co.x - c.x)
        deltas.append((a, t - v.co)); v.co = t
    def dl(co):
        a = math.atan2(co.z - c.z, co.x - c.x)
        return min(deltas, key=lambda ad: abs(math.atan2(math.sin(ad[0] - a), math.cos(ad[0] - a))))[1]
    for k, ring in enumerate(rings_from(bm, verts, n)[1:], start=1):
        for v in ring: v.co += dl(v.co) * falloff ** k
    return c

bv = boundary_verts(bm)
holes = {"mouth": [v for v in bv if v.co.z < -0.15],
         "eyeL": [v for v in bv if v.co.z >= -0.15 and v.co.x > 0],
         "eyeR": [v for v in bv if v.co.z >= -0.15 and v.co.x < 0]}
def almond(side):
    W, H = 0.17, 0.065         # halbe Breite / Höhe in Kugel-Einheiten (Loch vorher 0.14 x 0.04)
    def f(co, c):
        t = math.atan2(co.z - c.z, (co.x - c.x) * side); s = math.sin(t)
        x = c.x + side * W * math.cos(t)
        z = c.z + (H * s * (1 + 0.15 * math.cos(t)) if s > 0 else 0.6 * H * s) + 0.012 * math.cos(t)
        return Vector((x, co.y, z))
    return f
def slit(co, c):
    return Vector((co.x, co.y, c.z + (co.z - c.z) * 0.15))
centers = {"eyeL": shape_hole(holes["eyeL"], almond(1), falloff=0.7),
           "eyeR": shape_hole(holes["eyeR"], almond(-1), falloff=0.7),
           "mouth": shape_hole(holes["mouth"], slit, falloff=0.6)}

# 4) Lidkante und Lippen werden erst nach dem Glätten eingezogen (Schritt 8),
#    damit sie dem Augapfel folgen statt stur nach +Y zu laufen.

# 5) Hals
neck = [f for f in bm.faces if f.normal.z < -0.6 and abs(f.calc_center_median().x) < 0.45 and -0.35 < f.calc_center_median().y < 0.55]
# Achtung: extrude_face_region lässt die Ausgangsflächen stehen -> selbst löschen,
# sonst entstehen innere Flächen (Kanten mit >2 Flächen) und lose Kanten.
for i in range(2):
    r = bmesh.ops.extrude_face_region(bm, geom=neck)
    bmesh.ops.delete(bm, geom=neck, context='FACES')
    for g in r["geom"]:
        if isinstance(g, bmesh.types.BMVert): g.co.z -= 0.22
    neck = [g for g in r["geom"] if isinstance(g, bmesh.types.BMFace)]
bmesh.ops.delete(bm, geom=neck, context='FACES')

# 6) Auf Kopfmaße (Meter) skalieren: 15 x 19 x 22 cm
for v in bm.verts: v.co = Vector((v.co.x * 0.075, v.co.y * 0.095, v.co.z * 0.11))
me = bpy.data.meshes.new("SM_Testkopf"); bm.to_mesh(me); bm.free()
ob = bpy.data.objects.new("SM_Testkopf", me); SC.collection.objects.link(ob)
bpy.context.view_layer.objects.active = ob
for o in SC.objects: o.select_set(o == ob)
m = ob.modifiers.new("Subsurf", "SUBSURF"); m.levels = 1
bpy.ops.object.modifier_apply(modifier="Subsurf")
for p in me.polygons: p.use_smooth = True

# 7) Gesichtszüge als weiche Felder
MZ = centers["mouth"].z * 0.11          # gemessene Mundlinie statt fester Zahl
for v in me.vertices:
    x, y, z = v.co
    front = max(0.0, -y / 0.095)
    if y > 0: y *= 1.06
    t = sm(0.0, -0.10, z); x *= 1 - 0.28 * t * (0.4 + 0.6 * front)
    y += 0.014 * front ** 3
    if z < -0.10:
        k = sm(-0.10, -0.15, z); y += 0.018 * k; x *= 1 - 0.12 * k
    if y < -0.02:
        y -= 0.006 * G(x * x * 0.25 + (z - 0.030) ** 2, 0.010)            # Brauenbogen
        for s in (1, -1):
            y += 0.004 * G((x - s * 0.028) ** 2 + (z - 0.004) ** 2, 0.016)  # Augenhöhle flach
            y -= 0.004 * G((x - s * 0.044) ** 2 + (z + 0.014) ** 2, 0.012)  # Jochbein
        if -0.046 < z < 0.03:                                              # Nase
            tz = sm(0.02, -0.028, z)
            y -= (0.006 + 0.017 * tz) * G(x * x, 0.006 + 0.008 * tz) * (1 - sm(-0.028, -0.040, z))
            for s in (1, -1): y -= 0.006 * G((x - s * 0.012) ** 2 + (z + 0.032) ** 2, 0.006)
        y -= 0.006 * G((x / 1.6) ** 2 + (z - MZ) ** 2, 0.008)              # Lippen
        y -= 0.007 * G((x / 1.3) ** 2 + (z - MZ + 0.036) ** 2, 0.012)      # Kinn
    v.co = Vector((x, y, z))
me.update()

# 8) Augäpfel an den Lidrand anpassen, Lidkante zum Augapfel einziehen, Mundsack
R = 0.0125
bm = bmesh.new(); bm.from_mesh(me)
bv = boundary_verts(bm)
res = {}
for name, sel in (("L", lambda c: c.x > 0.01 and c.z > -0.03), ("R", lambda c: c.x < -0.01 and c.z > -0.03)):
    rim = [v for v in bv if sel(v.co)]
    c = center(rim)
    ec = Vector((c.x, min(v.co.y for v in rim) + R, c.z))
    while min((v.co - ec).length for v in rim) < R + 0.0007:   # Augapfel hinter jeden Randpunkt schieben
        ec.y += 0.0002
    res[name] = (ec, max(v.co.z for v in rim) - min(v.co.z for v in rim), max(v.co.x for v in rim) - min(v.co.x for v in rim))
    cur = [e for e in bm.edges if e.is_boundary and e.verts[0] in set(rim)]
    r = bmesh.ops.extrude_edge_only(bm, edges=cur)
    for g in r["geom"]:
        if isinstance(g, bmesh.types.BMVert):          # Lidstärke 2,5 mm, an der Kugel entlang
            d = (ec - g.co); g.co += d.normalized() * 0.0025
mouth = [v for v in bv if abs(v.co.x) < 0.04 and -0.08 < v.co.z < -0.03]
cur = [e for e in bm.edges if e.is_boundary and e.verts[0] in set(mouth)]
for depth, s in ((0.004, 0.95), (0.010, 0.8)):
    r = bmesh.ops.extrude_edge_only(bm, edges=cur)
    nv = [g for g in r["geom"] if isinstance(g, bmesh.types.BMVert)]
    cur = [g for g in r["geom"] if isinstance(g, bmesh.types.BMEdge) and g.is_boundary]
    cc = center(nv)
    for v in nv: v.co = cc + (v.co - cc) * s + Vector((0, depth, 0))
# Mundsack hinten schließen, sonst sieht man ins leere Kopfinnere. Das Ende ist ein flacher
# Schlitz (grid_fill liefert dort still nichts) -> obere und untere Punkte verschweißen (bleibt Quads).
bag_verts = set(mouth)
for ring in rings_from(bm, [v for e in cur for v in e.verts], 2): bag_verts |= ring
bmesh.ops.remove_doubles(bm, verts=list({v for e in cur for v in e.verts}), dist=0.0015)
bm.verts.index_update(); bm.faces.index_update(); bm.faces.ensure_lookup_table()
ob["lip_rim"] = [v.index for v in mouth if v.is_valid]          # Lippenkante für die Shape-Keys
MOUTH_FACES = [f.index for f in bm.faces if f.is_valid and all(v.is_valid and v in bag_verts for v in f.verts) and f.calc_center_median().y > -0.072]
bmesh.ops.recalc_face_normals(bm, faces=bm.faces[:])
bm.to_mesh(me); bm.free()
for p in me.polygons: p.use_smooth = True

def mat(n, col, rough=0.5, sss=0.0):
    mm = bpy.data.materials.get(n) or bpy.data.materials.new(n); mm.use_nodes = True
    b = next(x for x in mm.node_tree.nodes if x.type == "BSDF_PRINCIPLED")
    b.inputs["Base Color"].default_value = (*col, 1); b.inputs["Roughness"].default_value = rough
    if sss and "Subsurface Weight" in b.inputs: b.inputs["Subsurface Weight"].default_value = sss
    mm.diffuse_color = (*col, 1); return mm
skin = mat("M_Testkopf_Haut", (0.62, 0.42, 0.33), 0.55, 0.15)
mats = [mat("M_Testkopf_Sklera", (0.85, 0.83, 0.78), 0.25), mat("M_Testkopf_Iris", (0.18, 0.30, 0.38), 0.3),
        mat("M_Testkopf_Pupille", (0.01, 0.01, 0.01), 0.2)]
me.materials.clear(); me.materials.append(skin); me.materials.append(mat("M_Testkopf_Mundraum", (0.12, 0.03, 0.03), 0.6))
for i in MOUTH_FACES: me.polygons[i].material_index = 1   # erst nach clear(), das setzt Indizes zurück
for name, (c, hh, ww) in res.items():
    b2 = bmesh.new(); bmesh.ops.create_uvsphere(b2, u_segments=20, v_segments=14, radius=R)
    em = bpy.data.meshes.new("SM_Testkopf_Auge_" + name); b2.to_mesh(em); b2.free()
    for x in mats: em.materials.append(x)
    for p in em.polygons:
        p.use_smooth = True
        p.material_index = 2 if p.center.z > 0.96 * R else (1 if p.center.z > 0.86 * R else 0)
    eo = bpy.data.objects.new("SM_Testkopf_Auge_" + name, em); SC.collection.objects.link(eo)
    eo.parent = ob
    eo.location = c
    eo.rotation_euler = (math.radians(90), 0, 0)
    ob["eye_" + name] = list(c)
    print("Auge", name, "Öffnung H x B (mm):", round(hh * 1000, 1), "x", round(ww * 1000, 1))
ob["mouth"] = [0.0, 0.0, MZ]
print("Kopf:", len(me.polygons), "Quads,", sum(len(p.vertices) - 2 for p in me.polygons), "Dreiecke")
