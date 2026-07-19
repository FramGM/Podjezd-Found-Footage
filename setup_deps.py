import urllib.request
import zipfile
import os
import shutil

os.makedirs('vendor', exist_ok=True)

# raylib
print("Downloading raylib...")
urllib.request.urlretrieve("https://github.com/raysan5/raylib/releases/download/5.0/raylib-5.0_win64_msvc16.zip", "vendor/raylib.zip")
with zipfile.ZipFile("vendor/raylib.zip", 'r') as zip_ref:
    zip_ref.extractall("vendor")
os.rename("vendor/raylib-5.0_win64_msvc16", "vendor/raylib")
os.remove("vendor/raylib.zip")

# imgui
print("Downloading imgui...")
urllib.request.urlretrieve("https://github.com/ocornut/imgui/archive/refs/tags/v1.90.zip", "vendor/imgui.zip")
with zipfile.ZipFile("vendor/imgui.zip", 'r') as zip_ref:
    zip_ref.extractall("vendor")
os.rename("vendor/imgui-1.90", "vendor/imgui")
os.remove("vendor/imgui.zip")

# rlImGui
print("Downloading rlImGui...")
urllib.request.urlretrieve("https://github.com/raylib-extras/rlImGui/archive/refs/heads/main.zip", "vendor/rlImGui.zip")
with zipfile.ZipFile("vendor/rlImGui.zip", 'r') as zip_ref:
    zip_ref.extractall("vendor")
os.rename("vendor/rlImGui-main", "vendor/rlImGui")
os.remove("vendor/rlImGui.zip")

print("Done downloading dependencies.")
