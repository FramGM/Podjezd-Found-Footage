import math

walls = [
    ((0, 0), (100, 0)),
    ((100, 0), (100, 150)),
    ((100, 150), (0, 150)),
    ((0, 150), (0, 0)),

    ((0, 0), (30, 0)),
    ((30, 0), (0, 25)),

    ((40, 150), (100, 80)),

    ((20, 20), (40, 25)),
    ((40, 25), (45, 50)),
    ((45, 50), (25, 60)),
    
    ((50, 30), (70, 35)),
    ((70, 35), (75, 50)),
    ((75, 50), (90, 55)),
    ((90, 55), (60, 70)),
    
    ((15, 80), (35, 75)),
    ((35, 75), (45, 95)),
    ((45, 95), (20, 110)),
    ((20, 110), (25, 130)),
    
    ((50, 90), (70, 100)),
    ((70, 100), (65, 120)),
    ((65, 120), (90, 130)),

    ((40, 60), (60, 60)),
    ((60, 60), (60, 80)),
    ((60, 80), (45, 80)),
]

HEIGHT = 6.0
UV_SCALE_FLOOR = 0.2
UV_SCALE_WALL = 0.5

obj_file = open("c:/Users/vlade/source/repos/raylib_imgui_3d_snake/bin/Debug/assets/maze.obj", "w")
mtl_file = open("c:/Users/vlade/source/repos/raylib_imgui_3d_snake/bin/Debug/assets/maze.mtl", "w")

mtl_file.write("newmtl Material_Floor\n")
mtl_file.write("map_Kd tavrida_floor_BaseColor.png\n\n")

mtl_file.write("newmtl Material_Ceiling\n")
mtl_file.write("map_Kd tavrida_white_wall_BaseColor.png\n\n")

mtl_file.write("newmtl Material_Wall\n")
mtl_file.write("map_Kd tavrida_green_wall_BaseColor.png\n\n")

obj_file.write("mtllib maze.mtl\n")

vertices = []
uvs = []
normals = []
faces = []

def add_vertex(x, y, z):
    vertices.append((x, y, z))
    return len(vertices)

def add_uv(u, v):
    uvs.append((u, v))
    return len(uvs)

def add_normal(x, y, z):
    normals.append((x, y, z))
    return len(normals)

def add_quad(v1, v2, v3, v4, uv1, uv2, uv3, uv4, n, material):
    faces.append(([v1, v2, v3], [uv1, uv2, uv3], [n, n, n], material))
    faces.append(([v1, v3, v4], [uv1, uv3, uv4], [n, n, n], material))

# 1. Floor
n_floor = add_normal(0, 1, 0)
v1 = add_vertex(-100, 0, -100)
v2 = add_vertex(200, 0, -100)
v3 = add_vertex(200, 0, 250)
v4 = add_vertex(-100, 0, 250)

uv1 = add_uv(-100 * UV_SCALE_FLOOR, -100 * UV_SCALE_FLOOR)
uv2 = add_uv(200 * UV_SCALE_FLOOR, -100 * UV_SCALE_FLOOR)
uv3 = add_uv(200 * UV_SCALE_FLOOR, 250 * UV_SCALE_FLOOR)
uv4 = add_uv(-100 * UV_SCALE_FLOOR, 250 * UV_SCALE_FLOOR)

add_quad(v1, v2, v3, v4, uv1, uv2, uv3, uv4, n_floor, "Material_Floor")

# 2. Ceiling
n_ceil = add_normal(0, -1, 0)
v1 = add_vertex(-100, HEIGHT, -100)
v2 = add_vertex(-100, HEIGHT, 250)
v3 = add_vertex(200, HEIGHT, 250)
v4 = add_vertex(200, HEIGHT, -100)

uv1 = add_uv(-100 * UV_SCALE_FLOOR, -100 * UV_SCALE_FLOOR)
uv2 = add_uv(-100 * UV_SCALE_FLOOR, 250 * UV_SCALE_FLOOR)
uv3 = add_uv(200 * UV_SCALE_FLOOR, 250 * UV_SCALE_FLOOR)
uv4 = add_uv(200 * UV_SCALE_FLOOR, -100 * UV_SCALE_FLOOR)

add_quad(v1, v2, v3, v4, uv1, uv2, uv3, uv4, n_ceil, "Material_Ceiling")

# 3. Walls
THICKNESS = 1.0

for (start, end) in walls:
    dx = end[0] - start[0]
    dz = end[1] - start[1]
    length = math.hypot(dx, dz)
    nx = -dz / length
    nz = dx / length

    p1 = (start[0] + nx * THICKNESS/2, start[1] + nz * THICKNESS/2)
    p2 = (end[0] + nx * THICKNESS/2, end[1] + nz * THICKNESS/2)
    p3 = (end[0] - nx * THICKNESS/2, end[1] - nz * THICKNESS/2)
    p4 = (start[0] - nx * THICKNESS/2, start[1] - nz * THICKNESS/2)

    sides = [
        (p1, p2),
        (p2, p3),
        (p3, p4),
        (p4, p1)
    ]

    for (s_start, s_end) in sides:
        s_dx = s_end[0] - s_start[0]
        s_dz = s_end[1] - s_start[1]
        s_len = math.hypot(s_dx, s_dz)
        if s_len < 0.0001: continue
        
        snx = s_dz / s_len
        snz = -s_dx / s_len

        n_idx = add_normal(snx, 0, snz)
        
        v1 = add_vertex(s_start[0], 0, s_start[1])
        v2 = add_vertex(s_end[0], 0, s_end[1])
        v3 = add_vertex(s_end[0], HEIGHT, s_end[1])
        v4 = add_vertex(s_start[0], HEIGHT, s_start[1])

        uv1 = add_uv(0, 0)
        uv2 = add_uv(s_len * UV_SCALE_WALL, 0)
        uv3 = add_uv(s_len * UV_SCALE_WALL, HEIGHT * UV_SCALE_WALL)
        uv4 = add_uv(0, HEIGHT * UV_SCALE_WALL)

        add_quad(v1, v2, v3, v4, uv1, uv2, uv3, uv4, n_idx, "Material_Wall")

for v in vertices:
    obj_file.write(f"v {v[0]:.4f} {v[1]:.4f} {v[2]:.4f}\n")
for uv in uvs:
    obj_file.write(f"vt {uv[0]:.4f} {uv[1]:.4f}\n")
for n in normals:
    obj_file.write(f"vn {n[0]:.4f} {n[1]:.4f} {n[2]:.4f}\n")

current_mat = ""
for f in faces:
    if f[3] != current_mat:
        current_mat = f[3]
        obj_file.write(f"usemtl {current_mat}\n")
    
    obj_file.write(f"f {f[0][0]}/{f[1][0]}/{f[2][0]} {f[0][1]}/{f[1][1]}/{f[2][1]} {f[0][2]}/{f[1][2]}/{f[2][2]}\n")

obj_file.close()
mtl_file.close()
print("Generated maze.obj")
