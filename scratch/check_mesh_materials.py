import os

obj_path = r"c:\Users\vlade\source\repos\raylib_imgui_3d_snake\assets\map\лабиринт.obj"

materials_order = []
mesh_materials = []

with open(obj_path, 'r', encoding='utf-8', errors='ignore') as f:
    for line in f:
        line = line.strip()
        if line.startswith('usemtl '):
            mtl = line.split()[1]
            if mtl not in materials_order:
                materials_order.append(mtl)
            mesh_materials.append(mtl)

print("Materials in order of appearance in OBJ:", materials_order)
print("First 15 mesh materials:", mesh_materials[:15])
