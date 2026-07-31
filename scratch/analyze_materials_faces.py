import os

obj_path = r"c:\Users\vlade\source\repos\raylib_imgui_3d_snake\assets\map\лабиринт.obj"

vertices = []
normals = []
current_mtl = None

mtl_faces = {'Material.001': [], 'Material.002': []}

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
        elif line.startswith('f ') and current_mtl:
            parts = line.split()[1:]
            v_indices = []
            vn_indices = []
            for p in parts:
                vals = p.split('/')
                v_indices.append(int(vals[0]) - 1)
                if len(vals) >= 3 and vals[2]:
                    vn_indices.append(int(vals[2]) - 1)
            mtl_faces[current_mtl].append((v_indices, vn_indices))

for mtl, faces in mtl_faces.items():
    print(f"=== {mtl} ({len(faces)} faces) ===")
    y_coords = []
    normals_list = []
    for v_idxs, vn_idxs in faces:
        for vi in v_idxs:
            y_coords.append(vertices[vi][1])
        for vni in vn_idxs:
            normals_list.append(normals[vni])
            
    min_y, max_y = min(y_coords), max(y_coords)
    print(f"  Y range: [{min_y:.2f}, {max_y:.2f}]")
    
    # Check normals orientation (Up/Down vs Side)
    up_down = sum(1 for n in normals_list if abs(n[1]) > 0.8)
    sides = sum(1 for n in normals_list if abs(n[1]) <= 0.8)
    print(f"  Normals: Up/Down={up_down}, Horizontal Sides={sides}")
