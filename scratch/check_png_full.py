from PIL import Image

img1 = Image.open(r"c:\Users\vlade\source\repos\raylib_imgui_3d_snake\assets\map\лабиринт_Material.001_BaseColor.png")
img2 = Image.open(r"c:\Users\vlade\source\repos\raylib_imgui_3d_snake\assets\map\лабиринт_Material.002_BaseColor.png")

print("=== PNG1 (Material.001) ===")
print("Min RGB:", min(img1.getdata()))
print("Max RGB:", max(img1.getdata()))

print("=== PNG2 (Material.002) ===")
print("Min RGB:", min(img2.getdata()))
print("Max RGB:", max(img2.getdata()))

# Save small thumbnails or stats
img1.resize((100, 100)).save(r"c:\Users\vlade\source\repos\raylib_imgui_3d_snake\scratch\thumb1.png")
img2.resize((100, 100)).save(r"c:\Users\vlade\source\repos\raylib_imgui_3d_snake\scratch\thumb2.png")
