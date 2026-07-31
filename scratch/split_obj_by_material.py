"""
Split labirint.obj into 2 objects by material.
Raylib 5.0 creates 1 mesh per object and ignores multi-material within an object.
So we merge all Material.001 faces into one object and all Material.002 faces into another.
"""
import os

input_path = r"c:\Users\vlade\source\repos\raylib_imgui_3d_snake\assets\map\лабиринт.obj"
output_path = r"c:\Users\vlade\source\repos\raylib_imgui_3d_snake\assets\map\labirint.obj"

vertices = []
uvs = []
normals = []
mat1_faces = []  # Material.001 (green lower wall)
mat2_faces = []  # Material.002 (white upper wall + ceiling caps)

current_mtl = None

with open(input_path, 'r', encoding='utf-8', errors='ignore') as f:
    for line in f:
        line = line.strip()
        if line.startswith('v '):
            vertices.append(line)
        elif line.startswith('vt '):
            uvs.append(line)
        elif line.startswith('vn '):
            normals.append(line)
        elif line.startswith('usemtl '):
            current_mtl = line.split()[1]
        elif line.startswith('f '):
            if current_mtl == 'Material.001':
                mat1_faces.append(line)
            elif current_mtl == 'Material.002':
                mat2_faces.append(line)

print(f"Vertices: {len(vertices)}")
print(f"UVs: {len(uvs)}")
print(f"Normals: {len(normals)}")
print(f"Material.001 faces: {len(mat1_faces)}")
print(f"Material.002 faces: {len(mat2_faces)}")

# Write split OBJ
with open(output_path, 'w', encoding='utf-8') as f:
    f.write("# Split OBJ - 2 objects for Raylib material support\n")
    f.write(f"mtllib labirint.mtl\n\n")
    
    # Write all geometry data first
    for v in vertices:
        f.write(v + "\n")
    f.write("\n")
    for vt in uvs:
        f.write(vt + "\n")
    f.write("\n")
    for vn in normals:
        f.write(vn + "\n")
    f.write("\n")
    
    # Object 1: all Material.001 faces (green lower walls)
    f.write("o WallsGreen\n")
    f.write("usemtl Material.001\n")
    for face in mat1_faces:
        f.write(face + "\n")
    f.write("\n")
    
    # Object 2: all Material.002 faces (white upper walls + ceiling caps)
    f.write("o WallsWhite\n")
    f.write("usemtl Material.002\n")
    for face in mat2_faces:
        f.write(face + "\n")

print(f"\nWrote split OBJ to: {output_path}")
print(f"Objects: 2 (WallsGreen, WallsWhite)")

# Also copy to bin/Debug/assets/map/
bin_output = r"c:\Users\vlade\source\repos\raylib_imgui_3d_snake\bin\Debug\assets\map\labirint.obj"
import shutil
os.makedirs(os.path.dirname(bin_output), exist_ok=True)
shutil.copy2(output_path, bin_output)
print(f"Copied to: {bin_output}")
