import os

obj_path = r"c:\Users\vlade\source\repos\raylib_imgui_3d_snake\assets\map\лабиринт.obj"

vertices = []
faces = []
objects = {}

current_obj = None

with open(obj_path, 'r', encoding='utf-8', errors='ignore') as f:
    for line in f:
        line = line.strip()
        if not line or line.startswith('#'):
            continue
        parts = line.split()
        cmd = parts[0]
        
        if cmd == 'o':
            current_obj = parts[1]
            objects[current_obj] = []
        elif cmd == 'v':
            x, y, z = float(parts[1]), float(parts[2]), float(parts[3])
            vertices.append((x, y, z))
        elif cmd == 'f':
            # f v1/vt1/vn1 v2/vt2/vn2 ...
            idx_list = []
            for p in parts[1:]:
                v_idx = int(p.split('/')[0])
                if v_idx < 0:
                    v_idx = len(vertices) + v_idx + 1
                idx_list.append(v_idx - 1)
            if current_obj:
                objects[current_obj].append(idx_list)

print(f"Loaded {len(vertices)} vertices, {len(objects)} objects.")

physics_boxes = []

for obj_name, obj_faces in objects.items():
    # Extract bounding box of each quad/face or wall group in the object
    for face in obj_faces:
        face_verts = [vertices[i] for i in face]
        min_x = min(v[0] for v in face_verts)
        max_x = max(v[0] for v in face_verts)
        min_y = min(v[1] for v in face_verts)
        max_y = max(v[1] for v in face_verts)
        min_z = min(v[2] for v in face_verts)
        max_z = max(v[2] for v in face_verts)

        # Ignore horizontal floor/ceiling faces (where height max_y - min_y is almost 0)
        # Wall faces have vertical height!
        if (max_y - min_y) > 0.5:
            # Expand thin wall faces slightly to form solid AABB colliders
            thick_x = max(0.4, max_x - min_x)
            thick_z = max(0.4, max_z - min_z)
            cx = (min_x + max_x) / 2.0
            cz = (min_z + max_z) / 2.0
            
            p_min_x = cx - thick_x / 2.0
            p_max_x = cx + thick_x / 2.0
            p_min_z = cz - thick_z / 2.0
            p_max_z = cz + thick_z / 2.0
            
            # Ground to ceiling Y height:
            # Map Y ranges from -1.35 to +1.35 (total 2.7m height).
            physics_boxes.append((p_min_x, min_y, p_min_z, p_max_x, max_y, p_max_z))

print(f"Generated {len(physics_boxes)} raw wall colliders.")
