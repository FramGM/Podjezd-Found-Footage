import os

obj_path = r"c:\Users\vlade\source\repos\raylib_imgui_3d_snake\assets\map\лабиринт.obj"

vertices = []
faces_mtl = []
current_mtl = None
current_obj = None

with open(obj_path, 'r', encoding='utf-8', errors='ignore') as f:
    for line in f:
        line = line.strip()
        if line.startswith('v '):
            parts = line.split()
            vertices.append((float(parts[1]), float(parts[2]), float(parts[3])))
        elif line.startswith('o '):
            current_obj = line.split()[1]
        elif line.startswith('usemtl '):
            current_mtl = line.split()[1]
        elif line.startswith('f ') and current_obj == 'Cube.001':
            parts = line.split()[1:]
            v_idxs = [int(p.split('/')[0]) - 1 for p in parts]
            faces_mtl.append((current_mtl, v_idxs))

print(f"Cube.001 has {len(faces_mtl)} faces:")
for mtl, v_idxs in faces_mtl:
    f_verts = [vertices[i] for i in v_idxs]
    min_y = min(v[1] for v in f_verts)
    max_y = max(v[1] for v in f_verts)
    min_x = min(v[0] for v in f_verts)
    max_x = max(v[0] for v in f_verts)
    min_z = min(v[1] for v in f_verts)
    max_z = max(v[1] for v in f_verts)
    print(f"  Face {mtl:12s}: Y=[{min_y:5.2f}, {max_y:5.2f}], X=[{min_x:5.2f}, {max_x:5.2f}]")
