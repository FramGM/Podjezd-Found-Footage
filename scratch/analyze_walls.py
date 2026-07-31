import os

obj_path = r"c:\Users\vlade\source\repos\raylib_imgui_3d_snake\assets\map_original\лабиринт.obj"

vertices = []
objects = {}

current_obj = None
current_mtl = None

with open(obj_path, 'r', encoding='utf-8', errors='ignore') as f:
    for line in f:
        line = line.rstrip('\r\n')
        if line.startswith('v '):
            parts = line.split()
            vertices.append((float(parts[1]), float(parts[2]), float(parts[3])))
        elif line.startswith('o '):
            current_obj = line.split(None, 1)[1]
            objects[current_obj] = {'Material.001': [], 'Material.002': []}
        elif line.startswith('usemtl '):
            current_mtl = line.split(None, 1)[1]
        elif line.startswith('f ') and current_obj and current_mtl:
            # get vertex indices
            parts = line.split()[1:]
            v_indices = []
            for p in parts:
                v_idx = int(p.split('/')[0])
                # obj indices are 1-based, can be negative
                if v_idx < 0:
                    v_idx = len(vertices) + v_idx + 1
                v_indices.append(v_idx - 1)
            objects[current_obj][current_mtl].append(v_indices)

print("Analyzing vertical bounds per material per object:")
for obj_name, mtls in objects.items():
    m1_faces = mtls['Material.001']
    m2_faces = mtls['Material.002']
    
    m1_y = []
    for f in m1_faces:
        for vi in f:
            m1_y.append(vertices[vi][1])
            
    m2_y = []
    for f in m2_faces:
        for vi in f:
            m2_y.append(vertices[vi][1])
            
    m1_min = min(m1_y) if m1_y else 0
    m1_max = max(m1_y) if m1_y else 0
    m2_min = min(m2_y) if m2_y else 0
    m2_max = max(m2_y) if m2_y else 0
    
    # Is it fully white or does it have the proper split?
    # Expected: green is bottom (approx -1.35 to 0), white is top (approx 0 to 1.35)
    print(f"{obj_name}:")
    print(f"  Green: {len(m1_faces)} faces, Y:[{m1_min:.2f} to {m1_max:.2f}]")
    print(f"  White: {len(m2_faces)} faces, Y:[{m2_min:.2f} to {m2_max:.2f}]")
