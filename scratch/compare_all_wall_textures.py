import os
from PIL import Image

textures = [
    r"assets/map/labirint_Material.001_BaseColor.png",
    r"assets/map/labirint_Material.002_BaseColor.png",
    r"assets/tavrida_green_wall_BaseColor.png",
    r"assets/tavrida_white_wall_BaseColor.png",
    r"assets/new_wall_green.png",
    r"assets/new_wall_white.png"
]

for t in textures:
    if os.path.exists(t):
        img = Image.open(t)
        print(f"File {t:50s}: size={img.size}, mode={img.mode}")
        # Sample pixels
        p_top = img.getpixel((img.width//2, 10))
        p_mid = img.getpixel((img.width//2, img.height//2))
        p_bot = img.getpixel((img.width//2, img.height - 10))
        print(f"   Top={p_top}, Mid={p_mid}, Bot={p_bot}")
