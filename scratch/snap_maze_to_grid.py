import numpy as np

lines = [
    # --- OUTER BORDER ---
    ((0, 0), (98, 0)),
    ((98, 0), (98, 140)),
    ((98, 140), (0, 140)),
    ((0, 140), (0, 0)),
    
    # --- TOP LEFT ---
    ((0, 12), (24, 12)),
    ((24, 12), (24, 20)),
    ((24, 20), (32, 20)),
    ((10, 12), (10, 32)),
    ((10, 32), (32, 32)),
    ((32, 32), (32, 44)),
    ((32, 44), (44, 44)),
    ((10, 42), (20, 42)),
    ((20, 42), (20, 52)),
    ((10, 52), (10, 68)),

    # --- TOP RIGHT ---
    ((44, 0), (44, 20)),
    ((44, 20), (52, 20)),
    ((52, 20), (52, 28)),
    ((52, 28), (62, 28)),
    ((62, 28), (62, 0)),
    ((84, 12), (98, 12)),
    ((84, 12), (84, 30)),
    ((84, 30), (74, 30)),
    ((80, 24), (94, 24)),
    ((74, 40), (98, 40)),
    ((84, 40), (84, 52)),
    ((84, 52), (94, 52)),
    
    # --- MIDDLE LEFT ---
    ((0, 62), (8, 62)),
    ((8, 62), (8, 70)),
    ((8, 70), (14, 70)),
    ((14, 70), (14, 86)),
    ((20, 62), (48, 62)),
    ((48, 62), (48, 72)),
    ((48, 72), (20, 72)),
    ((20, 72), (20, 82)),
    ((20, 82), (30, 82)),
    ((0, 84), (10, 84)),
    ((10, 84), (10, 110)),
    ((10, 110), (16, 110)),
    ((16, 110), (16, 118)),

    # --- MIDDLE SPIRAL ---
    ((30, 50), (62, 50)),
    ((62, 50), (62, 40)),
    ((62, 40), (50, 40)),
    ((50, 40), (50, 46)),
    ((50, 46), (56, 46)),
    
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

GRID_SIZE = 4.0
FACTOR = 0.4 # Scale map coordinates to meters

snapped_lines = []
for p1, p2 in lines:
    x1, z1 = p1
    x2, z2 = p2
    
    sx1 = round((x1 * FACTOR) / GRID_SIZE) * GRID_SIZE
    sz1 = round((z1 * FACTOR) / GRID_SIZE) * GRID_SIZE
    sx2 = round((x2 * FACTOR) / GRID_SIZE) * GRID_SIZE
    sz2 = round((z2 * FACTOR) / GRID_SIZE) * GRID_SIZE
    
    if sx1 == sx2 and sz1 == sz2:
        continue
        
    snapped_lines.append(((sx1, sz1), (sx2, sz2)))

visual_boxes = []
physics_boxes = []

# Floor
visual_boxes.append((2, 0.0, 0.0, 0.0, 128.0, 0.1, 128.0, 0.0))
physics_boxes.append((0.0, -0.05, 0.0, 128.0, 0.1, 128.0))

wall_thickness = 4.0

for i, (p1, p2) in enumerate(snapped_lines):
    x1, z1 = p1
    x2, z2 = p2

    dx = x2 - x1
    dz = z2 - z1
    length = np.hypot(dx, dz)
    
    cx = (x1 + x2) / 2.0
    cz = (z1 + z2) / 2.0
    rot_deg = np.degrees(np.arctan2(dz, dx))
    
    # Visual Box (Type 0 = Green Wall)
    # Since we are snapping to 4m grid, length is a multiple of 4m.
    visual_boxes.append((0, cx, 1.8, cz, length, 3.6, wall_thickness, rot_deg))
    
    # Physics Box
    physics_boxes.append((cx, 1.8, cz, length, 3.6, wall_thickness))

with open("../bin/Debug/assets/level_boxes.txt", "w") as f:
    f.write(f"{len(visual_boxes)}\n")
    for box in visual_boxes:
        f.write(f"{box[0]} {box[1]:.5f} {box[2]:.5f} {box[3]:.5f} {box[4]:.5f} {box[5]:.5f} {box[6]:.5f} {box[7]:.5f}\n")

with open("../assets/level_boxes.txt", "w") as f:
    f.write(f"{len(visual_boxes)}\n")
    for box in visual_boxes:
        f.write(f"{box[0]} {box[1]:.5f} {box[2]:.5f} {box[3]:.5f} {box[4]:.5f} {box[5]:.5f} {box[6]:.5f} {box[7]:.5f}\n")

with open("../bin/Debug/assets/maze_physics.txt", "w") as f:
    f.write(f"{len(physics_boxes)}\n")
    for box in physics_boxes:
        f.write(f"{box[0]:.5f} {box[1]:.5f} {box[2]:.5f} {box[3]:.5f} {box[4]:.5f} {box[5]:.5f}\n")

with open("../assets/maze_physics.txt", "w") as f:
    f.write(f"{len(physics_boxes)}\n")
    for box in physics_boxes:
        f.write(f"{box[0]:.5f} {box[1]:.5f} {box[2]:.5f} {box[3]:.5f} {box[4]:.5f} {box[5]:.5f}\n")

print("Generated snapped 4m grid maze!")
