import os

obj_path = r"c:\Users\vlade\source\repos\raylib_imgui_3d_snake\assets\map\лабиринт.obj"

vertices = []
objects = {}

current_obj = None

with open(obj_path, 'r', encoding='utf-8', errors='ignore') as f:
    for line in f:
        line = line.strip()
        if line.startswith('v '):
            parts = line.split()
            vertices.append((float(parts[1]), float(parts[2]), float(parts[3])))
        elif line.startswith('o '):
            current_obj = line.split()[1]
            objects[current_obj] = []
        elif line.startswith('f ') and current_obj:
            parts = line.split()[1:]
            v_idxs = [int(p.split('/')[0]) - 1 for p in parts]
            objects[current_obj].extend(v_idxs)

print("Object Y ranges:")
for name, v_idxs in objects.items():
    if not v_idxs: continue
    y_vals = [vertices[i][1] for i in v_idxs]
    min_y, max_y = min(y_vals), max(y_vals)
    print(f"  {name:10s}: Y min = {min_y:6.3f}, Y max = {max_y:6.3f}, height = {max_y - min_y:6.3f}")
