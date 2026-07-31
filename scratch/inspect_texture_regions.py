from PIL import Image

img1 = Image.open(r"c:\Users\vlade\source\repos\raylib_imgui_3d_snake\assets\map\лабиринт_Material.001_BaseColor.png")
img2 = Image.open(r"c:\Users\vlade\source\repos\raylib_imgui_3d_snake\assets\map\лабиринт_Material.002_BaseColor.png")

w, h = img1.size

print("=== PNG1 (Material.001) Vertical Profile ===")
for y_pct in [0, 10, 25, 40, 50, 60, 70, 80, 90, 99]:
    py = int(h * y_pct / 100.0)
    if py >= h: py = h - 1
    rgb = img1.getpixel((w // 2, py))
    print(f"  Y = {y_pct:2d}% (py={py:4d}): RGB = {rgb}")

print("\n=== PNG2 (Material.002) Vertical Profile ===")
for y_pct in [0, 10, 25, 40, 50, 60, 70, 80, 90, 99]:
    py = int(h * y_pct / 100.0)
    if py >= h: py = h - 1
    rgb = img2.getpixel((w // 2, py))
    print(f"  Y = {y_pct:2d}% (py={py:4d}): RGB = {rgb}")
