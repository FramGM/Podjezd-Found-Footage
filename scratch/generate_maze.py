import os

def generate_obj(layout_file, obj_file, physics_file):
    with open(layout_file, 'r') as f:
        lines = f.readlines()
        
    lines = [line.strip('\n') for line in lines]
    
    # Check grid dimensions
    rows = len(lines)
    cols = max(len(l) for l in lines) if rows > 0 else 0
    
    # We will generate walls as thin cubes.
    wall_thickness = 0.6
    cell_size = 2.0
    wall_height = 3.0
    
    boxes = []
    for r in range(rows):
        for c in range(len(lines[r])):
            ch = lines[r][c]
            x_center = (c / 2.0) * cell_size
            z_center = (r / 2.0) * cell_size
            
            if r % 2 == 0 and c % 2 == 0:
                if ch == '+':
                    boxes.append((
                        x_center - wall_thickness/2, 0.0, z_center - wall_thickness/2,
                        x_center + wall_thickness/2, wall_height, z_center + wall_thickness/2
                    ))
            elif r % 2 == 0 and c % 2 != 0:
                if ch == '-':
                    boxes.append((
                        x_center - cell_size/2, 0.0, z_center - wall_thickness/2,
                        x_center + cell_size/2, wall_height, z_center + wall_thickness/2
                    ))
            elif r % 2 != 0 and c % 2 == 0:
                if ch == '|':
                    boxes.append((
                        x_center - wall_thickness/2, 0.0, z_center - cell_size/2,
                        x_center + wall_thickness/2, wall_height, z_center + cell_size/2
                    ))
                    
    # Physics output
    with open(physics_file, 'w') as f:
        f.write(f"{len(boxes)}\n")
        for b in boxes:
            f.write(f"{b[0]} {b[1]} {b[2]} {b[3]} {b[4]} {b[5]}\n")
            
    # OBJ Output
    with open(obj_file, 'w') as f:
        f.write("mtllib tavrida.mtl\n")
        v_offset = 1
        
        # Helper to write a textured quad
        def write_quad(p1, p2, p3, p4, u1, v1, u2, v2, material):
            nonlocal v_offset
            # Normal calculation
            import numpy as np
            v1_arr = np.array(p1)
            v2_arr = np.array(p2)
            v3_arr = np.array(p3)
            normal = np.cross(v2_arr - v1_arr, v3_arr - v1_arr)
            n_len = np.linalg.norm(normal)
            if n_len > 0: normal /= n_len
            
            f.write(f"usemtl {material}\n")
            f.write(f"v {p1[0]} {p1[1]} {p1[2]}\n")
            f.write(f"v {p2[0]} {p2[1]} {p2[2]}\n")
            f.write(f"v {p3[0]} {p3[1]} {p3[2]}\n")
            f.write(f"v {p4[0]} {p4[1]} {p4[2]}\n")
            
            f.write(f"vt {u1} {v1}\n")
            f.write(f"vt {u2} {v1}\n")
            f.write(f"vt {u2} {v2}\n")
            f.write(f"vt {u1} {v2}\n")
            
            f.write(f"vn {normal[0]} {normal[1]} {normal[2]}\n")
            
            f.write(f"f {v_offset}/1/1 {v_offset+1}/2/1 {v_offset+2}/3/1 {v_offset+3}/4/1\n")
            v_offset += 4
            
        # Draw Floor and Ceiling as one giant quad covering the maze
        maze_width = (cols / 2.0) * cell_size
        maze_depth = (rows / 2.0) * cell_size
        
        # Floor (facing up)
        write_quad(
            (-5, 0, maze_depth+5), (maze_width+5, 0, maze_depth+5), (maze_width+5, 0, -5), (-5, 0, -5),
            0, maze_depth+10, maze_width+10, 0, "floor_concrete"
        )
        # Ceiling (facing down)
        write_quad(
            (-5, wall_height, -5), (maze_width+5, wall_height, -5), (maze_width+5, wall_height, maze_depth+5), (-5, wall_height, maze_depth+5),
            0, 0, maze_width+10, maze_depth+10, "ceiling_white"
        )
        
        # Walls
        for b in boxes:
            x1, y1, z1, x2, y2, z2 = b
            w_len = max(x2 - x1, z2 - z1)
            
            # We split walls vertically: 0 to 1.5 is green, 1.5 to 3.0 is white.
            mid_y = wall_height / 2.0
            
            # Helper to draw a box face
            def draw_box_faces(bx1, by1, bz1, bx2, by2, bz2, mat, uv_scale=1.0):
                # Front (Z+)
                write_quad((bx1, by1, bz2), (bx2, by1, bz2), (bx2, by2, bz2), (bx1, by2, bz2), 0, 1, (bx2-bx1)*uv_scale, 0, mat)
                # Back (Z-)
                write_quad((bx2, by1, bz1), (bx1, by1, bz1), (bx1, by2, bz1), (bx2, by2, bz1), 0, 1, (bx2-bx1)*uv_scale, 0, mat)
                # Left (X-)
                write_quad((bx1, by1, bz1), (bx1, by1, bz2), (bx1, by2, bz2), (bx1, by2, bz1), 0, 1, (bz2-bz1)*uv_scale, 0, mat)
                # Right (X+)
                write_quad((bx2, by1, bz2), (bx2, by1, bz1), (bx2, by2, bz1), (bx2, by2, bz2), 0, 1, (bz2-bz1)*uv_scale, 0, mat)
            
            # Bottom (Green)
            draw_box_faces(x1, 0, z1, x2, mid_y, z2, "wall_green", uv_scale=0.5)
            # Top (White)
            draw_box_faces(x1, mid_y, z1, x2, wall_height, z2, "wall_white", uv_scale=0.5)
            
    print(f"Generated {len(boxes)} boxes.")

if __name__ == '__main__':
    generate_obj('scratch/maze_layout.txt', 'bin/Debug/assets/maze.obj', 'bin/Debug/assets/maze_physics.txt')
