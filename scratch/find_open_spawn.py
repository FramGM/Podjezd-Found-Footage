import os

obj_path = r"c:\Users\vlade\source\repos\raylib_imgui_3d_snake\assets\map\лабиринт.obj"

vertices = []
objects = {}
current_obj = None

with open(obj_path, 'r', encoding='utf-8', errors='ignore') as f:
    for line in f:
        line = line.strip()
        if not line or line.startswith('#'): continue
        parts = line.split()
        cmd = parts[0]
        if cmd == 'o':
            current_obj = parts[1]
            objects[current_obj] = []
        elif cmd == 'v':
            x, y, z = float(parts[1]), float(parts[2]), float(parts[3])
            vertices.append((x, y, z))
        elif cmd == 'f':
            idx_list = []
            for p in parts[1:]:
                v_idx = int(p.split('/')[0])
                if v_idx < 0: v_idx = len(vertices) + v_idx + 1
                idx_list.append(v_idx - 1)
            if current_obj:
                objects[current_obj].append(idx_list)

wall_boxes = []
for obj_name, obj_faces in objects.items():
    for face in obj_faces:
        face_verts = [vertices[i] for i in face]
        min_x = min(v[0] for v in face_verts)
        max_x = max(v[0] for v in face_verts)
        min_y = min(v[1] for v in face_verts)
        max_y = max(v[1] for v in face_verts)
        min_z = min(v[2] for v in face_verts)
        max_z = max(v[2] for v in face_verts)

        if (max_y - min_y) > 0.5:
            thick_x = max(0.5, max_x - min_x)
            thick_z = max(0.5, max_z - min_z)
            cx = (min_x + max_x) / 2.0
            cz = (min_z + max_z) / 2.0
            wall_boxes.append((cx - thick_x/2, cz - thick_z/2, cx + thick_x/2, cz + thick_z/2))

def is_free(x, z, radius=0.6):
    for min_x, min_z, max_x, max_z in wall_boxes:
        if (x + radius > min_x and x - radius < max_x and z + radius > min_z and z - radius < max_z):
            return False
    return True

print("Checking grid for open spawn points...")
open_points = []
for x in range(-18, 19, 2):
    for z in range(-28, 29, 2):
        if is_free(float(x), float(z)):
            open_points.append((x, z))

print(f"Found {len(open_points)} open positions.")
if open_points:
    print("Sample open positions:", open_points[:10])
    print("Center-most open position:", min(open_points, key=lambda p: p[0]**2 + p[1]**2))
