import os

obj_path = r"c:\Users\vlade\source\repos\raylib_imgui_3d_snake\assets\map\лабиринт.obj"

objects_data = {}

current_obj = None
current_mtl = None

with open(obj_path, 'r', encoding='utf-8', errors='ignore') as f:
    for line in f:
        line = line.strip()
        if line.startswith('o '):
            current_obj = line.split()[1]
            objects_data[current_obj] = {'verts': [], 'mtls': set()}
        elif line.startswith('usemtl '):
            current_mtl = line.split()[1]
            if current_obj:
                objects_data[current_obj]['mtls'].add(current_mtl)
        elif line.startswith('v ') and current_obj:
            parts = line.split()
            x, y, z = float(parts[1]), float(parts[2]), float(parts[3])
            objects_data[current_obj]['verts'].append((x, y, z))

print(f"Total Objects: {len(objects_data)}")
for name, data in objects_data.items():
    verts = data['verts']
    if not verts: continue
    min_x = min(v[0] for v in verts)
    max_x = max(v[0] for v in verts)
    min_y = min(v[1] for v in verts)
    max_y = max(v[1] for v in verts)
    min_z = min(v[2] for v in verts)
    max_z = max(v[2] for v in verts)
    mtls = list(data['mtls'])
    print(f"Object {name:10s}: mtls={mtls} | X:[{min_x:7.2f}, {max_x:7.2f}] (w={max_x-min_x:5.2f}) | Y:[{min_y:5.2f}, {max_y:5.2f}] (h={max_y-min_y:4.2f}) | Z:[{min_z:7.2f}, {max_z:7.2f}] (d={max_z-min_z:5.2f})")
