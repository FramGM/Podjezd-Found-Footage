import os
import sys

obj_path = r"c:\Users\vlade\source\repos\raylib_imgui_3d_snake\assets\map\лабиринт.obj"

if not os.path.exists(obj_path):
    print("OBJ not found:", obj_path)
    sys.exit(1)

min_x = min_y = min_z = float('inf')
max_x = max_y = max_z = float('-inf')

num_vertices = 0
num_faces = 0
objects = []
materials = set()

current_obj = None

with open(obj_path, 'r', encoding='utf-8', errors='ignore') as f:
    for line in f:
        line = line.strip()
        if line.startswith('o '):
            current_obj = line.split()[1]
            objects.append(current_obj)
        elif line.startswith('usemtl '):
            materials.add(line.split()[1])
        elif line.startswith('v '):
            parts = line.split()
            x, y, z = float(parts[1]), float(parts[2]), float(parts[3])
            num_vertices += 1
            if x < min_x: min_x = x
            if x > max_x: max_x = x
            if y < min_y: min_y = y
            if y > max_y: max_y = y
            if z < min_z: min_z = z
            if z > max_z: max_z = z
        elif line.startswith('f '):
            num_faces += 1

print(f"Vertices: {num_vertices}")
print(f"Faces: {num_faces}")
print(f"Objects: {objects}")
print(f"Materials: {list(materials)}")
print(f"Bounding Box:")
print(f"  X: min={min_x:.3f}, max={max_x:.3f}, width={max_x - min_x:.3f}")
print(f"  Y: min={min_y:.3f}, max={max_y:.3f}, height={max_y - min_y:.3f}")
print(f"  Z: min={min_z:.3f}, max={max_z:.3f}, depth={max_z - min_z:.3f}")
