"""
Deep analysis of labirint.obj + MTL to understand Raylib material indexing.
Raylib's tinyobj loader adds a DEFAULT material at index 0 when loading OBJ.
This script simulates what Raylib does.
"""
import os

obj_path = r"c:\Users\vlade\source\repos\raylib_imgui_3d_snake\assets\map\лабиринт.obj"
mtl_path = r"c:\Users\vlade\source\repos\raylib_imgui_3d_snake\assets\map\labirint.mtl"

# 1. Parse MTL to get material order
print("=" * 60)
print("MTL FILE ANALYSIS")
print("=" * 60)
mtl_materials = []
with open(mtl_path, 'r') as f:
    for line in f:
        line = line.strip()
        if line.startswith('newmtl '):
            mtl_materials.append(line.split()[1])
        elif line.startswith('map_Kd '):
            print(f"  {mtl_materials[-1]}: texture = {line.split()[1]}")

print(f"\nMTL defines {len(mtl_materials)} materials: {mtl_materials}")

# 2. Parse OBJ to understand mesh/material structure  
print("\n" + "=" * 60)
print("OBJ MESH/MATERIAL STRUCTURE")
print("=" * 60)

vertices = []
current_obj = None
current_mtl = None
mesh_groups = []  # (object, material, face_count)

face_count = 0

with open(obj_path, 'r', encoding='utf-8', errors='ignore') as f:
    for line in f:
        line = line.strip()
        if line.startswith('v '):
            parts = line.split()
            vertices.append((float(parts[1]), float(parts[2]), float(parts[3])))
        elif line.startswith('o '):
            if current_obj and current_mtl and face_count > 0:
                mesh_groups.append((current_obj, current_mtl, face_count))
            current_obj = line.split()[1]
            face_count = 0
        elif line.startswith('usemtl '):
            if current_obj and current_mtl and face_count > 0:
                mesh_groups.append((current_obj, current_mtl, face_count))
            current_mtl = line.split()[1]
            face_count = 0
        elif line.startswith('f '):
            face_count += 1

if current_obj and current_mtl and face_count > 0:
    mesh_groups.append((current_obj, current_mtl, face_count))

print(f"Total mesh groups (object+material combos): {len(mesh_groups)}")

# Count unique materials used
unique_mtls = set()
for obj, mtl, fc in mesh_groups:
    unique_mtls.add(mtl)
print(f"Unique materials referenced: {sorted(unique_mtls)}")

# Raylib's tinyobj loader behavior:
# - It reads the MTL file and creates material entries
# - Material index 0 is ALWAYS the default material (if no usemtl)  
# - User-defined materials start at index 1
# So with 2 MTL materials: index 0 = default, index 1 = Material.001, index 2 = Material.002
# Total materialCount = 3

print("\n" + "=" * 60)
print("PREDICTED RAYLIB MATERIAL INDEXING")
print("=" * 60)
print("Raylib's tinyobj adds a DEFAULT material at index 0")
print("  materials[0] = DEFAULT (unused catch-all)")
for i, mtl_name in enumerate(mtl_materials):
    print(f"  materials[{i+1}] = {mtl_name}")
print(f"\nPredicted materialCount = {len(mtl_materials) + 1}")

print("\n" + "=" * 60)
print("MESH MATERIAL MAPPING")
print("=" * 60)
# Raylib's tinyobj creates one mesh per (material, object) combo
# meshMaterial[mesh_idx] = material_index
for i, (obj, mtl, fc) in enumerate(mesh_groups):
    mtl_idx = mtl_materials.index(mtl) + 1 if mtl in mtl_materials else 0
    print(f"  mesh[{i:2d}] ({obj:10s} / {mtl:15s}, {fc:2d} faces) -> meshMaterial = {mtl_idx}")

# 3. Check which textures exist at various search paths
print("\n" + "=" * 60)
print("TEXTURE FILE EXISTENCE CHECK")
print("=" * 60)
search_dirs = ["", "assets/", "assets/map/", "bin/Debug/assets/", "bin/Debug/assets/map/"]
tex_files = [
    "tavrida_green_wall_BaseColor.png",
    "tavrida_white_wall_BaseColor.png",
    "labirint_Material.001_BaseColor.png",
    "labirint_Material.002_BaseColor.png",
    "floor_bc.png",
    "new_ceiling.png"
]

base = r"c:\Users\vlade\source\repos\raylib_imgui_3d_snake"
for tex in tex_files:
    found_in = []
    for sd in search_dirs:
        full = os.path.join(base, sd, tex)
        if os.path.exists(full):
            fsize = os.path.getsize(full)
            found_in.append(f"{sd or './'} ({fsize} bytes)")
    print(f"  {tex:50s}: {', '.join(found_in) if found_in else 'NOT FOUND'}")
