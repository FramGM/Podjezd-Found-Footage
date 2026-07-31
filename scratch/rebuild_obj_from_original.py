"""
Rebuild labirint.obj from original artist OBJ.
Split into 2 objects by material, but verify every Cube has both materials.
Also create proper ASCII-named copies of all assets.
"""
import os
import shutil
from PIL import Image

src_dir = r"c:\Users\vlade\source\repos\raylib_imgui_3d_snake\assets\map_original"
dst_dir = r"c:\Users\vlade\source\repos\raylib_imgui_3d_snake\assets\map"
bin_dir = r"c:\Users\vlade\source\repos\raylib_imgui_3d_snake\bin\Debug\assets\map"

obj_path = os.path.join(src_dir, "лабиринт.obj")

# 1. Parse original OBJ
vertices = []
uvs = []
normals = []
objects = {}  # {obj_name: {mtl_name: [face_lines]}}

current_obj = None
current_mtl = None

with open(obj_path, 'r', encoding='utf-8', errors='ignore') as f:
    for line in f:
        line = line.rstrip('\r\n')
        if line.startswith('v '):
            vertices.append(line)
        elif line.startswith('vt '):
            uvs.append(line)
        elif line.startswith('vn '):
            normals.append(line)
        elif line.startswith('o '):
            current_obj = line.split(None, 1)[1]
            objects[current_obj] = {}
        elif line.startswith('usemtl '):
            current_mtl = line.split(None, 1)[1]
            if current_obj and current_mtl not in objects[current_obj]:
                objects[current_obj][current_mtl] = []
        elif line.startswith('f ') and current_obj and current_mtl:
            objects[current_obj][current_mtl].append(line)

print(f"Parsed: {len(vertices)} vertices, {len(uvs)} UVs, {len(normals)} normals, {len(objects)} objects")

# 2. Verify each Cube has both materials
mat1_total = 0
mat2_total = 0
problems = []
for obj_name, mtls in objects.items():
    m1 = len(mtls.get('Material.001', []))
    m2 = len(mtls.get('Material.002', []))
    mat1_total += m1
    mat2_total += m2
    if m1 == 0:
        problems.append(f"  {obj_name}: MISSING Material.001!")
    if m2 == 0:
        problems.append(f"  {obj_name}: MISSING Material.002!")
    print(f"  {obj_name}: Material.001={m1} faces, Material.002={m2} faces")

print(f"\nTotal: Material.001={mat1_total} faces, Material.002={mat2_total} faces")
if problems:
    print("PROBLEMS:")
    for p in problems:
        print(p)
else:
    print("All objects have both materials - OK!")

# 3. Write split OBJ (2 objects: WallsGreen + WallsWhite)
output_obj = os.path.join(dst_dir, "labirint.obj")
with open(output_obj, 'w', encoding='utf-8') as f:
    f.write("# Rebuilt from artist OBJ - 2 objects for Raylib material support\n")
    f.write("# Original: 29 Cube objects, each with Material.001 + Material.002\n")
    f.write("mtllib labirint.mtl\n\n")
    
    for v in vertices:
        f.write(v + "\n")
    f.write("\n")
    for vt in uvs:
        f.write(vt + "\n")
    f.write("\n")
    for vn in normals:
        f.write(vn + "\n")
    f.write("\n")
    
    # Object 1: ALL Material.001 faces from ALL cubes
    f.write("o WallsGreen\n")
    f.write("usemtl Material.001\n")
    for obj_name in objects:
        for face in objects[obj_name].get('Material.001', []):
            f.write(face + "\n")
    f.write("\n")
    
    # Object 2: ALL Material.002 faces from ALL cubes
    f.write("o WallsWhite\n")
    f.write("usemtl Material.002\n")
    for obj_name in objects:
        for face in objects[obj_name].get('Material.002', []):
            f.write(face + "\n")

print(f"\nWrote split OBJ: {output_obj}")
print(f"  WallsGreen: {mat1_total} faces")
print(f"  WallsWhite: {mat2_total} faces")

# 4. Copy textures with ASCII names
tex_copies = [
    ("лабиринт_Material.001_BaseColor.png", "labirint_Material.001_BaseColor.png"),
    ("лабиринт_Material.002_BaseColor.png", "labirint_Material.002_BaseColor.png"),
]
for src_name, dst_name in tex_copies:
    src_path = os.path.join(src_dir, src_name)
    dst_path = os.path.join(dst_dir, dst_name)
    if os.path.exists(src_path):
        shutil.copy2(src_path, dst_path)
        img = Image.open(dst_path)
        print(f"Copied {src_name} -> {dst_name} ({img.size}, {img.mode})")

# 5. Copy to bin/Debug
os.makedirs(bin_dir, exist_ok=True)
for fname in ["labirint.obj", "labirint.mtl", "labirint_Material.001_BaseColor.png", "labirint_Material.002_BaseColor.png"]:
    src = os.path.join(dst_dir, fname)
    dst = os.path.join(bin_dir, fname)
    if os.path.exists(src):
        shutil.copy2(src, dst)
        print(f"Synced to bin: {fname}")

# 6. Check texture content
print("\n=== Texture Analysis ===")
for name in ["labirint_Material.001_BaseColor.png", "labirint_Material.002_BaseColor.png"]:
    path = os.path.join(dst_dir, name)
    img = Image.open(path)
    # Sample a grid
    w, h = img.size
    unique_colors = set()
    for y in range(0, h, h//10):
        for x in range(0, w, w//10):
            unique_colors.add(img.getpixel((x, y)))
    print(f"{name}: {img.size} {img.mode}, {len(unique_colors)} unique sampled colors")
    # Show some samples
    samples = list(unique_colors)[:5]
    print(f"  Sample colors: {samples}")
