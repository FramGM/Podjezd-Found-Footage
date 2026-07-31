import os

obj_path = r"c:\Users\vlade\source\repos\raylib_imgui_3d_snake\assets\map\лабиринт.obj"

vertices = []
uvs = []
current_mtl = None

mtl_uvs = {'Material.001': [], 'Material.002': []}

with open(obj_path, 'r', encoding='utf-8', errors='ignore') as f:
    for line in f:
        line = line.strip()
        if line.startswith('vt '):
            parts = line.split()
            uvs.append((float(parts[1]), float(parts[2])))
        elif line.startswith('usemtl '):
            current_mtl = line.split()[1]
        elif line.startswith('f ') and current_mtl:
            parts = line.split()[1:]
            for p in parts:
                vals = p.split('/')
                if len(vals) >= 2 and vals[1]:
                    vt_idx = int(vals[1]) - 1
                    mtl_uvs[current_mtl].append(uvs[vt_idx])

for mtl, uv_list in mtl_uvs.items():
    if not uv_list: continue
    u_vals = [uv[0] for uv in uv_list]
    v_vals = [uv[1] for uv in uv_list]
    print(f"=== {mtl} ({len(uv_list)} UV vertices) ===")
    print(f"  U range: [{min(u_vals):.4f}, {max(u_vals):.4f}]")
    print(f"  V range: [{min(v_vals):.4f}, {max(v_vals):.4f}]")
    print(f"  Sample UVs:", uv_list[:5])
