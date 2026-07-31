import os
from PIL import Image

png1 = r"c:\Users\vlade\source\repos\raylib_imgui_3d_snake\assets\map\лабиринт_Material.001_BaseColor.png"
png2 = r"c:\Users\vlade\source\repos\raylib_imgui_3d_snake\assets\map\лабиринт_Material.002_BaseColor.png"

img1 = Image.open(png1)
img2 = Image.open(png2)

print(f"PNG1 (Material.001): size={img1.size}, mode={img1.mode}")
print(f"PNG2 (Material.002): size={img2.size}, mode={img2.mode}")

# Get sample colors
colors1 = img1.getcolors(maxcolors=1000)
colors2 = img2.getcolors(maxcolors=1000)

print(f"PNG1 sample pixel (0,0): {img1.getpixel((0,0))}")
print(f"PNG2 sample pixel (0,0): {img2.getpixel((0,0))}")
print(f"PNG2 sample pixel (512,512): {img2.getpixel((512,512))}")
