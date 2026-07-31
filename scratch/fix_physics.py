import math
import os

vis_walls = []
with open('bin/Debug/assets/level_boxes.txt', 'r') as f:
    lines = f.read().splitlines()[1:]
    for l in lines:
        if not l.strip(): continue
        parts = l.split()
        if parts[0] == '0':
            vis_walls.append((float(parts[1]), float(parts[3]), float(parts[4]), float(parts[6]), float(parts[7])))

phys_walls = []
phys_lines_raw = []
with open('bin/Debug/assets/maze_physics.txt', 'r') as f:
    lines = f.read().splitlines()
    for l in lines[1:]:
        if not l.strip(): continue
        parts = l.split()
        minx, minz, maxx, maxz = float(parts[0]), float(parts[2]), float(parts[3]), float(parts[5])
        phys_walls.append((minx, minz, maxx, maxz))
        phys_lines_raw.append(l)

def point_in_phys(x, z):
    for pminx, pminz, pmaxx, pmaxz in phys_walls:
        if pminx - 0.1 <= x <= pmaxx + 0.1 and pminz - 0.1 <= z <= pmaxz + 0.1:
            return True
    return False

added_boxes = []

for cx, cz, sx, sz, rot in vis_walls:
    length = sx
    thickness = sz
    
    angle = math.radians(-rot)
    dx = math.cos(angle)
    dz = -math.sin(angle)
    
    step = 0.6
    steps = int(length / step)
    if steps < 1: steps = 1
    step = length / steps
    
    startX = cx - dx * (length / 2.0)
    startZ = cz - dz * (length / 2.0)
    
    for i in range(steps + 1):
        px = startX + dx * (i * step)
        pz = startZ + dz * (i * step)
        
        if not point_in_phys(px, pz):
            minx = px - 0.3
            maxx = px + 0.3
            minz = pz - 0.3
            maxz = pz + 0.3
            miny = 0.0
            maxy = 3.6
            added_boxes.append(f"{minx:.3f} {miny:.3f} {minz:.3f} {maxx:.3f} {maxy:.3f} {maxz:.3f}")
            phys_walls.append((minx, minz, maxx, maxz))

if len(added_boxes) > 0:
    print(f"Adding {len(added_boxes)} missing physics boxes.")
    with open('bin/Debug/assets/maze_physics.txt', 'w') as f:
        f.write(f"{len(phys_lines_raw) + len(added_boxes)}\n")
        for l in phys_lines_raw:
            f.write(l + "\n")
        for l in added_boxes:
            f.write(l + "\n")
    print("Done writing maze_physics.txt")
else:
    print("No missing boxes found.")
