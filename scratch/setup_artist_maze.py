import os
import shutil

map_dir = r"c:\Users\vlade\source\repos\raylib_imgui_3d_snake\assets\map"
bin_map_dir = r"c:\Users\vlade\source\repos\raylib_imgui_3d_snake\bin\Debug\assets\map"
assets_dir = r"c:\Users\vlade\source\repos\raylib_imgui_3d_snake\assets"
bin_assets_dir = r"c:\Users\vlade\source\repos\raylib_imgui_3d_snake\bin\Debug\assets"

os.makedirs(map_dir, exist_ok=True)
os.makedirs(bin_map_dir, exist_ok=True)
os.makedirs(assets_dir, exist_ok=True)
os.makedirs(bin_assets_dir, exist_ok=True)

# 1. Create MTL files
labirint_mtl_cyr = """newmtl Material.001
Kd 1 1 1
map_Kd лабиринт_Material.001_BaseColor.png

newmtl Material.002
Kd 1 1 1
map_Kd лабиринт_Material.002_BaseColor.png
"""

labirint_mtl_asc = """newmtl Material.001
Kd 1 1 1
map_Kd labirint_Material.001_BaseColor.png

newmtl Material.002
Kd 1 1 1
map_Kd labirint_Material.002_BaseColor.png
"""

with open(os.path.join(map_dir, "лабиринт.mtl"), "w", encoding="utf-8") as f:
    f.write(labirint_mtl_cyr)

with open(os.path.join(map_dir, "labirint.mtl"), "w", encoding="utf-8") as f:
    f.write(labirint_mtl_asc)

# 2. Create labirint.obj from лабиринт.obj
src_obj = os.path.join(map_dir, "лабиринт.obj")
dst_obj = os.path.join(map_dir, "labirint.obj")

if os.path.exists(src_obj):
    with open(src_obj, "r", encoding="utf-8", errors="ignore") as f_in:
        content = f_in.read()
    content_asc = content.replace("mtllib лабиринт.mtl", "mtllib labirint.mtl")
    with open(dst_obj, "w", encoding="utf-8") as f_out:
        f_out.write(content_asc)
    print("Created labirint.obj")

# 3. Create ASCII texture copies
tex_pairs = [
    ("лабиринт_Material.001_BaseColor.png", "labirint_Material.001_BaseColor.png"),
    ("лабиринт_Material.001_Normal.png", "labirint_Material.001_Normal.png"),
    ("лабиринт_Material.001_Height.png", "labirint_Material.001_Height.png"),
    ("лабиринт_Material.001_Roughness.png", "labirint_Material.001_Roughness.png"),
    ("лабиринт_Material.001_Metallic.png", "labirint_Material.001_Metallic.png"),
    ("лабиринт_Material.002_BaseColor.png", "labirint_Material.002_BaseColor.png"),
    ("лабиринт_Material.002_Normal.png", "labirint_Material.002_Normal.png"),
    ("лабиринт_Material.002_Height.png", "labirint_Material.002_Height.png"),
    ("лабиринт_Material.002_Roughness.png", "labirint_Material.002_Roughness.png"),
    ("лабиринт_Material.002_Metallic.png", "labirint_Material.002_Metallic.png"),
]

for cyr, asc in tex_pairs:
    cyr_path = os.path.join(map_dir, cyr)
    asc_path = os.path.join(map_dir, asc)
    if os.path.exists(cyr_path):
        shutil.copy2(cyr_path, asc_path)

# Copy everything in map_dir to bin_map_dir
for item in os.listdir(map_dir):
    s = os.path.join(map_dir, item)
    d = os.path.join(bin_map_dir, item)
    if os.path.isfile(s):
        shutil.copy2(s, d)

# 4. Generate maze_physics.txt for artist maze
obj_path = src_obj
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

physics_boxes = []

# Ground is Y = -1.35, Height is Y = 1.35
# Expand outer bounds slightly so entity can't escape
physics_boxes.append((-25.0, -2.0, -35.0, -20.0, 3.0, 35.0))
physics_boxes.append((20.0, -2.0, -35.0, 25.0, 3.0, 35.0))
physics_boxes.append((-25.0, -2.0, -35.0, 25.0, 3.0, -30.0))
physics_boxes.append((-25.0, -2.0, 30.0, 25.0, 3.0, 35.0))

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
            thick_x = max(0.4, max_x - min_x)
            thick_z = max(0.4, max_z - min_z)
            cx = (min_x + max_x) / 2.0
            cz = (min_z + max_z) / 2.0

            p_min_x = cx - thick_x / 2.0
            p_max_x = cx + thick_x / 2.0
            p_min_z = cz - thick_z / 2.0
            p_max_z = cz + thick_z / 2.0

            physics_boxes.append((p_min_x, -1.35, p_min_z, p_max_x, 1.35, p_max_z))

print(f"Generated {len(physics_boxes)} physics boxes for artist maze.")

for target_dir in [assets_dir, bin_assets_dir]:
    phys_file = os.path.join(target_dir, "maze_physics.txt")
    with open(phys_file, "w", encoding="utf-8") as f:
        f.write(f"{len(physics_boxes)}\n")
        for b in physics_boxes:
            f.write(f"{b[0]:.3f} {b[1]:.3f} {b[2]:.3f} {b[3]:.3f} {b[4]:.3f} {b[5]:.3f}\n")
    print(f"Wrote {phys_file}")

print("Artist maze setup completed!")
