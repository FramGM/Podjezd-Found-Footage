import os
import numpy as np

def build_maze():
    # Exact vector map lines based on hand-drawn maze
    lines = [
        # Outer Enclosing Walls (Complete Boundary)
        ((0, 0), (55, 0)),
        ((65, 0), (100, 0)),
        ((100, 0), (100, 140)),
        ((100, 140), (0, 140)),
        ((0, 140), (0, 130)),
        ((0, 120), (0, 0)),

        # --- TOP LEFT SECTION ---
        # Triangle in top-left
        ((5, 20), (20, 5)),
        ((20, 5), (20, 20)),
        ((20, 20), (5, 20)),
        # Diagonal slash
        ((5, 5), (30, 25)),
        # Zig-zag below triangle
        ((15, 25), (35, 10)),
        ((35, 10), (40, 25)),

        # --- TOP CENTER & SPIRAL ---
        # Vertical wall from top border
        ((55, 0), (55, 25)),
        ((55, 25), (50, 25)),
        ((50, 25), (50, 15)),
        ((50, 15), (45, 15)),
        # Spiral
        ((35, 30), (55, 30)),
        ((55, 30), (55, 18)),
        ((55, 18), (40, 18)),
        ((40, 18), (40, 26)),
        ((40, 26), (50, 26)),
        ((50, 26), (50, 22)),

        # --- TOP RIGHT SECTION ---
        # Exit notch
        ((65, 0), (65, 15)),
        ((65, 15), (75, 15)),
        # V-shape Chevron 1
        ((75, 5), (88, 18)),
        ((88, 18), (72, 28)),
        # V-shape Chevron 2
        ((80, 18), (96, 18)),
        ((96, 18), (80, 32)),
        # Horizontal bar
        ((75, 32), (92, 32)),

        # --- MIDDLE SECTION ---
        # Stepped wall top-left of middle
        ((10, 45), (10, 38)),
        ((10, 38), (16, 38)),
        ((16, 38), (16, 32)),
        ((16, 32), (22, 32)),
        ((22, 32), (22, 45)),
        # Main middle horizontal hallway wall
        ((20, 45), (65, 45)),
        # Center Room ("Вожатый" star area)
        ((25, 50), (48, 50)),
        ((48, 50), (48, 62)),
        ((48, 62), (42, 62)),
        ((42, 62), (42, 56)),
        ((42, 56), (32, 56)),
        ((32, 56), (32, 65)),
        ((32, 65), (25, 65)),
        ((25, 65), (25, 50)),

        # --- MIDDLE RIGHT ---
        # Island Square box
        ((60, 22), (68, 22)),
        ((68, 22), (68, 30)),
        ((68, 30), (60, 30)),
        ((60, 30), (60, 22)),
        # Long vertical corridor wall
        ((70, 20), (70, 72)),
        # Comb pattern on middle-right
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
        # Huge Diagonal line across the bottom right
        ((38, 138), (68, 76)),
        # Large Arrow Shape (pointing right)
        ((65, 78), (96, 90)),
        ((96, 90), (65, 102)),
        # Inner parallel arrow lines
        ((70, 84), (90, 90)),
        ((90, 90), (70, 96)),
        # Lower-right maze paths
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
        # U-notch near entrance
        ((0, 130), (12, 130)),
        ((12, 130), (12, 140)),
        # Hook wall near entrance
        ((16, 118), (26, 118)),
        ((26, 118), (26, 134)),
        ((26, 134), (20, 134)),
        ((20, 134), (20, 128)),
        # Vertical divider
        ((30, 82), (30, 132)),
        # Stepped comb (lower center)
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
        # Small hooks
        ((14, 86), (14, 96)),
        ((14, 96), (20, 96)),
        # Small comb
        ((10, 68), (10, 78)),
        ((10, 78), (5, 78)),
        ((5, 78), (5, 72)),
        ((5, 72), (0, 72)),
    ]

    wall_thickness = 0.6
    wall_height = 3.6
    green_height = 3.6 * 0.45
    step_size = 0.25

    # 1. Physics Boxes
    boxes = []
    
    # Outer Map Boundary Enclosure Boxes
    boxes.append((-10.0, 0.0, -10.0, 110.0, wall_height, -0.6))
    boxes.append((-10.0, 0.0, 140.6, 110.0, wall_height, 150.0))
    boxes.append((-10.0, 0.0, -10.0, -0.6, wall_height, 150.0))
    boxes.append((100.6, 0.0, -10.0, 110.0, wall_height, 150.0))

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
            boxes.append((
                cx - wall_thickness / 2.0, 0.0, cz - wall_thickness / 2.0,
                cx + wall_thickness / 2.0, wall_height, cz + wall_thickness / 2.0
            ))

    physics_file = 'bin/Debug/assets/maze_physics.txt'
    os.makedirs('bin/Debug/assets', exist_ok=True)
    with open(physics_file, 'w') as f:
        f.write(f"{len(boxes)}\n")
        for b in boxes:
            f.write(f"{b[0]:.3f} {b[1]:.3f} {b[2]:.3f} {b[3]:.3f} {b[4]:.3f} {b[5]:.3f}\n")

    print(f"Generated {len(boxes)} physics boxes.")

    # 2. OBJ Model Export
    obj_file = 'bin/Debug/assets/maze.obj'
    mtl_file = 'bin/Debug/assets/tavrida.mtl'

    with open(mtl_file, 'w') as f:
        f.write("newmtl wall_green\nmap_Kd tavrida_green_wall_BaseColor.png\n\n")
        f.write("newmtl wall_white\nmap_Kd tavrida_white_wall_BaseColor.png\n\n")
        f.write("newmtl floor_concrete\nmap_Kd tavrida_floor_BaseColor.png\n\n")
        f.write("newmtl ceiling_white\nmap_Kd tavrida_white_wall_BaseColor.png\n\n")

    material_quads = {
        "wall_green": [],
        "wall_white": [],
        "floor_concrete": [],
        "ceiling_white": []
    }

    def add_quad(p1, p2, p3, p4, u1, v1, u2, v2, material):
        material_quads[material].append((p1, p2, p3, p4, u1, v1, u2, v2))

    # Floor
    add_quad(
        (-10, 0, 150), (110, 0, 150), (110, 0, -10), (-10, 0, -10),
        0, 80, 60, 0, "floor_concrete"
    )
    # Ceiling
    add_quad(
        (-10, wall_height, -10), (110, wall_height, -10), (110, wall_height, 150), (-10, wall_height, 150),
        0, 0, 60, 80, "ceiling_white"
    )

    # Walls
    half_t = wall_thickness / 2.0
    for p1, p2 in lines:
        x1, z1 = float(p1[0]), float(p1[1])
        x2, z2 = float(p2[0]), float(p2[1])

        dx, dz = x2 - x1, z2 - z1
        length = np.hypot(dx, dz)
        if length == 0:
            continue

        nx, nz = -dz / length * half_t, dx / length * half_t

        c1 = (x1 - nx, z1 - nz)
        c2 = (x2 - nx, z2 - nz)
        c3 = (x2 + nx, z2 + nz)
        c4 = (x1 + nx, z1 + nz)

        u_len = length / 2.0  # Repeat texture every 2 meters

        # Side 1 (c1 -> c2)
        add_quad((c1[0], 0, c1[1]), (c2[0], 0, c2[1]), (c2[0], green_height, c2[1]), (c1[0], green_height, c1[1]), 0, 1, u_len, 0, "wall_green")
        add_quad((c1[0], green_height, c1[1]), (c2[0], green_height, c2[1]), (c2[0], wall_height, c2[1]), (c1[0], wall_height, c1[1]), 0, 1, u_len, 0, "wall_white")

        # Side 2 (c3 -> c4)
        add_quad((c3[0], 0, c3[1]), (c4[0], 0, c4[1]), (c4[0], green_height, c4[1]), (c3[0], green_height, c3[1]), 0, 1, u_len, 0, "wall_green")
        add_quad((c3[0], green_height, c3[1]), (c4[0], green_height, c4[1]), (c4[0], wall_height, c4[1]), (c3[0], wall_height, c3[1]), 0, 1, u_len, 0, "wall_white")

        # Cap 1 (c4 -> c1)
        add_quad((c4[0], 0, c4[1]), (c1[0], 0, c1[1]), (c1[0], green_height, c1[1]), (c4[0], green_height, c4[1]), 0, 1, 0.3, 0, "wall_green")
        add_quad((c4[0], green_height, c4[1]), (c1[0], green_height, c1[1]), (c1[0], wall_height, c1[1]), (c4[0], wall_height, c4[1]), 0, 1, 0.3, 0, "wall_white")

        # Cap 2 (c2 -> c3)
        add_quad((c2[0], 0, c2[1]), (c3[0], 0, c3[1]), (c3[0], green_height, c3[1]), (c2[0], green_height, c2[1]), 0, 1, 0.3, 0, "wall_green")
        add_quad((c2[0], green_height, c2[1]), (c3[0], green_height, c3[1]), (c3[0], wall_height, c3[1]), (c2[0], wall_height, c2[1]), 0, 1, 0.3, 0, "wall_white")

    # Write OBJ with STRICT VT AND V INDEXING
    with open(obj_file, 'w') as f:
        f.write("mtllib tavrida.mtl\n")
        v_idx = 1
        vt_idx = 1
        vn_idx = 1

        mat_order = ["wall_green", "wall_white", "floor_concrete", "ceiling_white"]
        
        for mat in mat_order:
            f.write(f"usemtl {mat}\n")
            for p1, p2, p3, p4, u1, v1, u2, v2 in material_quads[mat]:
                v1_arr = np.array(p1, dtype=float)
                v2_arr = np.array(p2, dtype=float)
                v3_arr = np.array(p3, dtype=float)
                normal = np.cross(v2_arr - v1_arr, v3_arr - v1_arr)
                n_len = np.linalg.norm(normal)
                if n_len > 0:
                    normal /= n_len
                else:
                    normal = np.array([0.0, 1.0, 0.0])

                # Write 4 vertices
                f.write(f"v {p1[0]:.3f} {p1[1]:.3f} {p1[2]:.3f}\n")
                f.write(f"v {p2[0]:.3f} {p2[1]:.3f} {p2[2]:.3f}\n")
                f.write(f"v {p3[0]:.3f} {p3[1]:.3f} {p3[2]:.3f}\n")
                f.write(f"v {p4[0]:.3f} {p4[1]:.3f} {p4[2]:.3f}\n")

                # Write 4 texture coordinates
                f.write(f"vt {u1:.3f} {v1:.3f}\n")
                f.write(f"vt {u2:.3f} {v1:.3f}\n")
                f.write(f"vt {u2:.3f} {v2:.3f}\n")
                f.write(f"vt {u1:.3f} {v2:.3f}\n")

                # Write normal
                f.write(f"vn {normal[0]:.3f} {normal[1]:.3f} {normal[2]:.3f}\n")

                # Write face referencing the CORRECT VT indices!
                f.write(f"f {v_idx}/{vt_idx}/{vn_idx} {v_idx+1}/{vt_idx+1}/{vn_idx} {v_idx+2}/{vt_idx+2}/{vn_idx} {v_idx+3}/{vt_idx+3}/{vn_idx}\n")

                v_idx += 4
                vt_idx += 4
                vn_idx += 1

    print(f"Generated OBJ model successfully with correct VT indices: {obj_file}")

if __name__ == '__main__':
    build_maze()
