import os

obj_path = r"c:\Users\vlade\source\repos\raylib_imgui_3d_snake\assets\map\лабиринт.obj"

vertices = []
uvs = []
objects_mtls = {}

current_obj = None
current_mtl = None

with open(obj_path, 'r', encoding='utf-8', errors='ignore') as f:
    for line in f:
        line = line.strip()
        if line.startswith('v '):
            parts = line.split()
            vertices.append((float(parts[1]), float(parts[2]), float(parts[3])))
        elif line.startswith('vt '):
            parts = line.split()
            uvs.append((float(parts[1]), float(parts[2])))
        elif line.startswith('o '):
            current_obj = line.split()[1]
            objects_mtls[current_obj] = {}
        elif line.startswith('usemtl '):
            current_mtl = line.split()[1]
            if current_mtl not in objects_mtls[current_obj]:
                objects_mtls[current_obj][current_mtl] = []
        elif line.startswith('f ') and current_obj and current_mtl:
            parts = line.split()[1:]
            v_idxs = [int(p.split('/')[0]) - 1 for p in parts]
            vt_idxs = [int(p.split('/')[1]) - 1 for p in parts if len(p.split('/')) >= 2 and p.split('/')[1]]
            objects_mtls[current_obj][current_mtl].append((v_idxs, vt_idxs))

print(f"Total objects in OBJ: {len(objects_mtls)}")
for obj_name, mtls in objects_mtls.items():
    mtl_names = list(mtls.keys())
    print(f"Object {obj_name:10s}: Materials={mtl_names}")
    for mtl_name, faces in mtls.items():
        if not faces: continue
        all_v = [v for f in faces for v in f[0]]
        y_coords = [vertices[v][1] for v in all_v]
        all_vt = [vt for f in faces for vt in f[1]]
        u_coords = [uvs[vt][0] for vt in all_vt] if all_vt else [0]
        v_coords = [uvs[vt][1] for vt in all_vt] if all_vt else [0]
        print(f"  -> {mtl_name:12s} ({len(faces):2d} faces): Y=[{min(y_coords):5.2f}, {max(y_coords):5.2f}], U=[{min(u_coords):.2f}, {max(u_coords):.2f}], V=[{min(v_coords):.2f}, {max(v_coords):.2f}]")
