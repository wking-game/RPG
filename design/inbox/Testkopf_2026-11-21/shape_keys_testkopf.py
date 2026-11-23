# ARKit-Shape-Keys für den Testkopf (Teilmenge der 52): eyeBlinkLeft/Right, jawOpen, mouthSmileLeft/Right.
# Links = linke Seite der Figur = +X (Figur blickt nach -Y). Nach build_testkopf.py ausführen.
import bpy, bmesh, math
from mathutils import Vector, Matrix

bpy.context.view_layer.update()   # sonst ist matrix_world der frisch gesetzten Augäpfel noch 0
ob = bpy.data.objects["SM_Testkopf"]; me = ob.data
if me.shape_keys:
    ob.shape_key_clear()
basis = ob.shape_key_add(name="Basis")
co0 = [v.co.copy() for v in me.vertices]
R = 0.0125
MZ = ob["mouth"][2]
def sm(a, b, t):
    t = max(0.0, min(1.0, (t - a) / (b - a))); return t * t * (3 - 2 * t)

bm = bmesh.new(); bm.from_mesh(me); bm.verts.ensure_lookup_table()
bound = {v.index for e in bm.edges if e.is_boundary for v in e.verts}

def rings(start, n):
    out = [set(start)]; seen = set(start)
    for _ in range(n):
        nxt = {e.other_vert(bm.verts[i]).index for i in out[-1] for e in bm.verts[i].link_edges} - seen
        out.append(nxt); seen |= nxt
    return out

def add_key(name, pos):
    k = ob.shape_key_add(name=name, from_mix=False)
    for i, p in enumerate(pos): k.data[i].co = p
    return k

# ---- eyeBlink ----
for name, side in (("eyeBlinkLeft", 1), ("eyeBlinkRight", -1)):
    eye = bpy.data.objects["SM_Testkopf_Auge_" + ("L" if side == 1 else "R")]
    ec = eye.matrix_world.translation.copy()
    tunnel = [i for i in bound if co0[i].z > -0.03 and co0[i].x * side > 0.01]
    rr = rings(tunnel, 5)
    rim = rr[1]
    loop = tunnel + list(rim)
    # untere Lidlinie: z der unteren Randpunkte als Funktion von x
    lower = sorted([(co0[i].x, co0[i].z) for i in loop if co0[i].z < ec.z], key=lambda a: a[0])
    def lower_z(x):
        best = min(lower, key=lambda a: abs(a[0] - x)); return best[1]
    pos = [c.copy() for c in co0]
    deltas = {}
    for i in loop:
        if co0[i].z > ec.z - 0.0005:                       # oberes Lid
            dz = (lower_z(co0[i].x) + 0.0004) - co0[i].z
            deltas[i] = dz
    def nearest_delta(i):
        a = math.atan2(co0[i].z - ec.z, co0[i].x - ec.x)
        cand = [(abs(math.atan2(math.sin(math.atan2(co0[j].z - ec.z, co0[j].x - ec.x) - a),
                                math.cos(math.atan2(co0[j].z - ec.z, co0[j].x - ec.x) - a))), d) for j, d in deltas.items()]
        return min(cand)[1]
    weights = [1.0, 1.0, 0.55, 0.28, 0.12, 0.05]
    for k, ring in enumerate(rr):
        for i in ring:
            if co0[i].z <= ec.z - 0.0005: continue
            d = deltas[i] if i in deltas else nearest_delta(i) * weights[k]
            pos[i] = co0[i] + Vector((0, 0, d))
            v = pos[i] - ec                                   # Lid gleitet über den Augapfel
            if v.length < R + 0.0012 and k <= 2: pos[i] = ec + v.normalized() * (R + 0.0012)
    add_key(name, pos)

# ---- jawOpen ----
mouth_b = list(ob["lip_rim"])                             # Lippenkante (vom Bauskript gespeichert)
mline = {}
xs = sorted({round(co0[i].x, 4) for i in mouth_b})
def line_z(x):
    near = sorted(mouth_b, key=lambda i: abs(co0[i].x - x))[:4]
    return sum(co0[i].z for i in near) / len(near)
bag = set().union(*rings(mouth_b, 3))                    # Mundsack + Lippenkante
piv = Vector((0, 0.008, -0.004)); ang = math.radians(13)
rot = Matrix.Rotation(ang, 4, 'X')
pos = []
for i, c in enumerate(co0):
    if abs(c.x) < 0.045 and i in bag:
        w = sm(0.0004, -0.0004, c.z - line_z(c.x))       # unter der Mundlinie = Unterkiefer
    else:
        w = sm(MZ + 0.002, MZ - 0.006, c.z)
    w *= 1 - sm(-0.01, 0.03, c.y)                          # Hinterkopf bleibt
    if c.z < -0.095: w *= 1 - sm(-0.04, -0.015, c.y)       # Hals bleibt
    p = piv + rot @ (c - piv)
    pos.append(c.lerp(p, w))
add_key("jawOpen", pos)

# ---- mouthSmile ----
xc = max(co0[i].x for i in mouth_b)
for name, side in (("mouthSmileLeft", 1), ("mouthSmileRight", -1)):
    pos = []
    for c in co0:
        g = math.exp(-((c.x - side * xc) ** 2 + (c.z - MZ) ** 2) / (2 * 0.011 ** 2))
        ch = math.exp(-((c.x - side * 0.032) ** 2 + (c.z - MZ - 0.022) ** 2) / (2 * 0.010 ** 2))
        front = 1 - sm(-0.05, -0.02, c.y)
        d = Vector((side * 0.0035 * g, 0.0025 * g - 0.0012 * ch, 0.0042 * g + 0.0022 * ch)) * front
        pos.append(c + d)
    add_key(name, pos)
bm.free()
print([k.name for k in me.shape_keys.key_blocks])
