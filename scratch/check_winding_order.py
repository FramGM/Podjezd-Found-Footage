import os

obj_path = r"c:\Users\vlade\source\repos\raylib_imgui_3d_snake\assets\map\лабиринт.obj"

vertices = []
normals = []
current_mtl = None

floor_faces = []
ceil_faces = []

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
            v_idxs = [int(p.split('/')[0]) - 1 for p in parts]
            f_verts = [vertices[i] for i in v_idxs]
            avg_y = sum(v[1] for v in f_verts) / len(f_verts)
            if abs(avg_y - (-1.35)) < 0.05:
                floor_faces.append(v_idxs)
            elif abs(avg_y - (1.35)) < 0.05:
                ceil_faces.append(v_idxs)

print(f"Floor faces (Y ~ -1.35): {len(floor_faces)}")
print(f"Ceiling faces (Y ~ 1.35): {len(ceil_faces)}")

if floor_faces:
    print("Sample floor face verts:", [vertices[i] for i in floor_faces[0]])
if ceil_faces:
    print("Sample ceil face verts:", [vertices[i] for i in ceil_faces[0]])
