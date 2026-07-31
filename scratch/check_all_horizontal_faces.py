import os

obj_path = r"c:\Users\vlade\source\repos\raylib_imgui_3d_snake\assets\map\лабиринт.obj"

vertices = []
normals = []
current_mtl = None

h_faces = []

with open(obj_path, 'r', encoding='utf-8', errors='ignore') as f:
    for line in f:
        line = line.strip()
        if line.startswith('v '):
            parts = line.split()
            vertices.append((float(parts[1]), float(parts[2]), float(parts[3])))
        elif line.startswith('vn '):
            parts = line.split()
            normals.append((float(parts[1]), float(parts[2]), float(parts[3])))
        elif line.startswith('usemtl '):
            current_mtl = line.split()[1]
        elif line.startswith('f '):
            parts = line.split()[1:]
            v_idxs = []
            vn_idxs = []
            for p in parts:
                vals = p.split('/')
                v_idxs.append(int(vals[0]) - 1)
                if len(vals) >= 3 and vals[2]:
                    vn_idxs.append(int(vals[2]) - 1)
            
            # Check if any normal is vertical
            is_horiz = False
            for vni in vn_idxs:
                if abs(normals[vni][1]) > 0.8:
                    is_horiz = True
                    break
            if is_horiz:
                f_verts = [vertices[i] for i in v_idxs]
                avg_y = sum(v[1] for v in f_verts) / len(f_verts)
                h_faces.append((avg_y, current_mtl, v_idxs))

print(f"Total horizontal faces: {len(h_faces)}")
y_levels = set(round(hf[0], 2) for hf in h_faces)
print(f"Unique Y levels of horizontal faces: {sorted(list(y_levels))}")
