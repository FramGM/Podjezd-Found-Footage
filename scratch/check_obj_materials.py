import os

obj_path = r"c:\Users\vlade\source\repos\raylib_imgui_3d_snake\assets\map\лабиринт.obj"

materials_usage = {}
current_mtl = None
current_obj = None

with open(obj_path, 'r', encoding='utf-8', errors='ignore') as f:
    for line in f:
        line = line.strip()
        if line.startswith('o '):
            current_obj = line.split()[1]
        elif line.startswith('usemtl '):
            current_mtl = line.split()[1]
            if current_mtl not in materials_usage:
                materials_usage[current_mtl] = {'objs': set(), 'faces': 0, 'y_min': 999, 'y_max': -999}
        elif line.startswith('f ') and current_mtl:
            materials_usage[current_mtl]['faces'] += 1
            if current_obj:
                materials_usage[current_mtl]['objs'].add(current_obj)

print("Material Usage:")
for mtl, info in materials_usage.items():
    print(f"Material {mtl}: {info['faces']} faces across {len(info['objs'])} objects: {sorted(list(info['objs']))[:5]}")
