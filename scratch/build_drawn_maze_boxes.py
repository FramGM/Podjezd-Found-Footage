import os
import numpy as np
from PIL import Image

def make_seamless(img, blend_width=0.15):
    arr = np.array(img, dtype=float)
    h, w, c = arr.shape
    bw = int(w * blend_width)
    bh = int(h * blend_width)

    out = arr.copy()
    for x in range(bw):
        t = x / float(bw)
        alpha = 0.5 * (1.0 - np.cos(np.pi * t))
        out[:, x] = arr[:, x] * alpha + arr[:, w - bw + x] * (1.0 - alpha)
        out[:, w - bw + x] = arr[:, w - bw + x] * alpha + arr[:, x] * (1.0 - alpha)

    out_final = out.copy()
    for y in range(bh):
        t = y / float(bh)
        alpha = 0.5 * (1.0 - np.cos(np.pi * t))
        out_final[y, :] = out[y, :] * alpha + out[h - bh + y, :] * (1.0 - alpha)
        out_final[h - bh + y, :] = out[h - bh + y, :] * alpha + out[y, :] * (1.0 - alpha)

    return Image.fromarray(np.uint8(out_final))

def process_textures():
    raw_textures = [
        (r'C:\Users\vlade\.gemini\antigravity\brain\f98d681e-f8d6-4de2-ac12-476e98998e9d\seamless_green_wall_1785275957739.jpg', 'new_wall_green.png'),
        (r'C:\Users\vlade\.gemini\antigravity\brain\f98d681e-f8d6-4de2-ac12-476e98998e9d\seamless_white_wall_1785275968986.jpg', 'new_wall_white.png'),
        (r'C:\Users\vlade\.gemini\antigravity\brain\f98d681e-f8d6-4de2-ac12-476e98998e9d\seamless_floor_tile_1785275980989.jpg', 'new_floor.png'),
        (r'C:\Users\vlade\.gemini\antigravity\brain\f98d681e-f8d6-4de2-ac12-476e98998e9d\seamless_ceiling_white_1785275993709.jpg', 'new_ceiling.png')
    ]

    for src_path, out_name in raw_textures:
        img = Image.open(src_path).convert('RGB')
        seamless_img = make_seamless(img)

        for dirpath in ['assets', r'bin\Debug\assets']:
            os.makedirs(dirpath, exist_ok=True)
            out_file = os.path.join(dirpath, out_name)
            seamless_img.save(out_file, 'PNG')
            print(f'Saved 100% seamless PNG: {out_file}')

def generate_maze_boxes():
    lines = [
        # Outer Boundary (with entrance gap at (0, 120..130) and exit gap at (55..65, 0))
        ((0, 0), (55, 0)),
        ((65, 0), (100, 0)),
        ((100, 0), (100, 140)),
        ((100, 140), (0, 140)),
        ((0, 140), (0, 130)),
        ((0, 120), (0, 0)),

        # --- TOP LEFT SECTION ---
        ((5, 20), (20, 5)),
        ((20, 5), (20, 20)),
        ((20, 20), (5, 20)),
        ((5, 5), (30, 25)),
        ((15, 25), (35, 10)),
        ((35, 10), (40, 25)),

        # --- TOP CENTER & SPIRAL ---
        ((55, 0), (55, 25)),
        ((55, 25), (50, 25)),
        ((50, 25), (50, 15)),
        ((50, 15), (45, 15)),
        ((35, 30), (55, 30)),
        ((55, 30), (55, 18)),
        ((55, 18), (40, 18)),
        ((40, 18), (40, 26)),
        ((40, 26), (50, 26)),
        ((50, 26), (50, 22)),

        # --- TOP RIGHT SECTION ---
        ((65, 0), (65, 15)),
        ((65, 15), (75, 15)),
        ((75, 5), (88, 18)),
        ((88, 18), (72, 28)),
        ((80, 18), (96, 18)),
        ((96, 18), (80, 32)),
        ((75, 32), (92, 32)),

        # --- MIDDLE SECTION ---
        ((10, 45), (10, 38)),
        ((10, 38), (16, 38)),
        ((16, 38), (16, 32)),
        ((16, 32), (22, 32)),
        ((22, 32), (22, 45)),
        ((20, 45), (65, 45)),
        ((25, 50), (48, 50)),
        ((48, 50), (48, 62)),
        ((48, 62), (42, 62)),
        ((42, 62), (42, 56)),
        ((42, 56), (32, 56)),
        ((32, 56), (32, 65)),
        ((32, 65), (25, 65)),
        ((25, 65), (25, 50)),

        # --- MIDDLE RIGHT ---
        ((60, 22), (68, 22)),
        ((68, 22), (68, 30)),
        ((68, 30), (60, 30)),
        ((60, 30), (60, 22)),
        ((70, 20), (70, 72)),
        ((75, 50), (82, 50)),
        ((82, 50), (82, 55)),
        ((82, 55), (88, 55)),
        ((88, 55), (88, 60)),
        ((88, 60), (94, 60)),
        ((94, 60), (94, 66)),
        ((94, 66), (88, 66)),
        ((88, 66), (88, 72)),
        ((88, 72), (75, 72)),

        # --- BOTTOM RIGHT & DIAGONAL ---
        ((38, 138), (68, 76)),
        ((65, 78), (96, 90)),
        ((96, 90), (65, 102)),
        ((70, 84), (90, 90)),
        ((90, 90), (70, 96)),
        ((70, 102), (70, 112)),
        ((70, 112), (82, 112)),
        ((82, 112), (82, 122)),
        ((82, 122), (76, 122)),
        ((76, 122), (76, 136)),
        ((76, 136), (86, 136)),
        ((86, 136), (86, 112)),
        ((86, 112), (92, 112)),
        ((92, 112), (92, 132)),
        ((92, 132), (97, 132)),
        ((97, 132), (97, 140)),

        # --- BOTTOM LEFT & ENTRANCE ---
        ((0, 130), (12, 130)),
        ((12, 130), (12, 140)),
        ((16, 118), (26, 118)),
        ((26, 118), (26, 134)),
        ((26, 134), (20, 134)),
        ((20, 134), (20, 128)),
        ((30, 82), (30, 132)),
        ((34, 88), (46, 88)),
        ((46, 88), (46, 94)),
        ((46, 94), (40, 94)),
        ((40, 94), (40, 104)),
        ((40, 104), (52, 104)),
        ((52, 104), (52, 114)),
        ((52, 114), (42, 114)),
        ((42, 114), (42, 124)),
        ((42, 124), (56, 124)),
        ((56, 124), (56, 136)),
        ((14, 86), (14, 96)),
        ((14, 96), (20, 96)),
        ((10, 68), (10, 78)),
        ((10, 78), (5, 78)),
        ((5, 78), (5, 72)),
        ((5, 72), (0, 72)),
    ]

    wall_thickness = 0.6
    wall_height = 3.6
    green_height = 3.6 * 0.45  # 1.62m
    white_height = wall_height - green_height  # 1.98m
    
    green_center_y = green_height / 2.0  # 0.81m
    white_center_y = green_height + white_height / 2.0  # 2.61m

    visual_boxes = []

    # 1. Collect unique vertices for pillars
    vertices = set()
    for p1, p2 in lines:
        vertices.add(p1)
        vertices.add(p2)

    # 2. Add physics pillars at vertices (for collision only)
    for vx, vz in vertices:
        cx, cz = float(vx), float(vz)
        # We don't add them to visual_boxes because they cause ugly squished .obj models!
        # Instead, we extend the wall segments to overlap at the corners.

    # 3. Draw wall segments that overlap at corners
    for i, (p1, p2) in enumerate(lines):
        x1, z1 = float(p1[0]), float(p1[1])
        x2, z2 = float(p2[0]), float(p2[1])

        dx = x2 - x1
        dz = z2 - z1
        length = np.hypot(dx, dz)
        
        # Extend length by wall_thickness so it covers the corner joints fully!
        actual_length = length + wall_thickness
        if actual_length <= 0.001:
            continue

        cx = (x1 + x2) / 2.0
        cz = (z1 + z2) / 2.0
        rot_deg = np.degrees(np.arctan2(dz, dx))
        
        sx = actual_length
        sz = wall_thickness

        # Output a single box for the entire wall (the .obj contains both top and bottom parts)
        visual_boxes.append((0, cx, wall_height / 2.0, cz, sx, wall_height, sz, rot_deg))

    # Outer bounds
    outer_bounds = [
        (-5.0, 70.0, 10.0, 170.0),
        (105.0, 70.0, 10.0, 170.0),
        (50.0, -5.0, 120.0, 10.0),
        (50.0, 145.0, 120.0, 10.0)
    ]
    for cx, cz, sx, sz in outer_bounds:
        visual_boxes.append((0, cx, wall_height / 2.0, cz, sx, wall_height, sz, 0.0))

    # Floor & Ceiling
    visual_boxes.append((2, 50.0, -0.1, 70.0, 130.0, 0.2, 170.0, 0.0))
    visual_boxes.append((3, 50.0, 3.7, 70.0, 130.0, 0.2, 170.0, 0.0))

    # Save level_boxes.txt
    for dirpath in ['assets', r'bin\Debug\assets']:
        os.makedirs(dirpath, exist_ok=True)
        out_file = os.path.join(dirpath, 'level_boxes.txt')
        with open(out_file, 'w') as f:
            f.write(f"{len(visual_boxes)}\n")
            for b in visual_boxes:
                f.write(f"{b[0]} {b[1]:.3f} {b[2]:.3f} {b[3]:.3f} {b[4]:.3f} {b[5]:.3f} {b[6]:.3f} {b[7]:.3f}\n")
        print(f"Wrote {len(visual_boxes)} visual boxes to {out_file}")

    # 2. PHYSICS BOXES (maze_physics.txt) - Sampled tight AABBs (Zero Invisible Walls!)
    physics_boxes = []
    step_size = 0.3

    # Outer Boundary Physics
    physics_boxes.append((-10.0, 0.0, -10.0, 110.0, wall_height, -0.6))
    physics_boxes.append((-10.0, 0.0, 140.6, 110.0, wall_height, 150.0))
    physics_boxes.append((-10.0, 0.0, -10.0, -0.6, wall_height, 150.0))
    physics_boxes.append((100.6, 0.0, -10.0, 110.0, wall_height, 150.0))

    for p1, p2 in lines:
        x1, z1 = float(p1[0]), float(p1[1])
        x2, z2 = float(p2[0]), float(p2[1])

        length = np.hypot(x2 - x1, z2 - z1)
        if length == 0:
            continue

        num_steps = max(1, int(np.ceil(length / step_size)))
        for i in range(num_steps + 1):
            t = i / float(num_steps)
            cx = x1 + t * (x2 - x1)
            cz = z1 + t * (z2 - z1)
            physics_boxes.append((
                cx - wall_thickness / 2.0, 0.0, cz - wall_thickness / 2.0,
                cx + wall_thickness / 2.0, wall_height, cz + wall_thickness / 2.0
            ))

    # Save maze_physics.txt
    for dirpath in ['assets', r'bin\Debug\assets']:
        os.makedirs(dirpath, exist_ok=True)
        out_file = os.path.join(dirpath, 'maze_physics.txt')
        with open(out_file, 'w') as f:
            f.write(f"{len(physics_boxes)}\n")
            for b in physics_boxes:
                f.write(f"{b[0]:.3f} {b[1]:.3f} {b[2]:.3f} {b[3]:.3f} {b[4]:.3f} {b[5]:.3f}\n")
        print(f"Wrote {len(physics_boxes)} physics boxes to {out_file}")

if __name__ == '__main__':
    process_textures()
    generate_maze_boxes()
