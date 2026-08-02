#include "CMap.h"
#include "rlgl.h"
#include <raymath.h>
#include <cstdio>
#include <string>
#include <vector>
#include <iostream>

#ifdef _WIN32
extern "C" __declspec(dllimport) unsigned long __stdcall GetFileAttributesW(const wchar_t* lpFileName);
extern "C" __declspec(dllimport) int __stdcall MultiByteToWideChar(unsigned int CodePage, unsigned long dwFlags, const char* lpMultiByteStr, int cbMultiByte, wchar_t* lpWideCharStr, int cchWideChar);

static bool FileExistsUnicode(const std::string& path)
{
    int wlen = MultiByteToWideChar(65001U, 0, path.c_str(), -1, NULL, 0);
    if (wlen <= 0) return FileExists(path.c_str());
    std::wstring wpath(wlen, 0);
    MultiByteToWideChar(65001U, 0, path.c_str(), -1, &wpath[0], wlen);

    unsigned long dwAttrs = GetFileAttributesW(wpath.c_str());
    return (dwAttrs != 0xFFFFFFFF && !(dwAttrs & 0x10));
}
#else
static bool FileExistsUnicode(const std::string& path)
{
    return FileExists(path.c_str());
}
#endif

static std::string GetAssetPath(const std::string& filename)
{
    std::vector<std::string> prefixes = { "", "assets/", "assets/map/", "bin/Debug/assets/", "bin/Debug/assets/map/", "../assets/", "../assets/map/" };
    for (const auto& prefix : prefixes) {
        std::string testPath = prefix + filename;
        if (FileExistsUnicode(testPath)) return testPath;
    }
    return filename;
}

void CMap::Init()
{
    m_vecBoxes.clear();
    m_vecPhysicsBoxes.clear();

    auto SetupTexture = [](Texture2D& tex) {
        GenTextureMipmaps(&tex);
        SetTextureFilter(tex, TEXTURE_FILTER_TRILINEAR);
        SetTextureWrap(tex, TEXTURE_WRAP_REPEAT);
        rlTextureParameters(tex.id, RL_TEXTURE_FILTER_ANISOTROPIC, 16);
    };

    // 1. Load Artist Maze & Environment textures
    m_texGreen = LoadTexture(GetAssetPath("new_wall_green.png").c_str());
    if (m_texGreen.id == 0) m_texGreen = LoadTexture(GetAssetPath("labirint_Material.001_BaseColor.png").c_str());
    SetupTexture(m_texGreen);

    m_texWhite = LoadTexture(GetAssetPath("new_wall_white.png").c_str());
    if (m_texWhite.id == 0) m_texWhite = LoadTexture(GetAssetPath("labirint_Material.002_BaseColor.png").c_str());
    SetupTexture(m_texWhite);

    m_texFloor = LoadTexture(GetAssetPath("new_floor.png").c_str());
    if (m_texFloor.id == 0) m_texFloor = LoadTexture(GetAssetPath("tavrida_floor_BaseColor.png").c_str());
    SetupTexture(m_texFloor);

    m_texCeiling = LoadTexture(GetAssetPath("new_ceiling.png").c_str());
    if (m_texCeiling.id == 0) m_texCeiling = LoadTexture(GetAssetPath("tavrida_white_wall_BaseColor.png").c_str());
    SetupTexture(m_texCeiling);

    // Generate Floor Model with proper tiling and tessellation to fix perspective warp
    Mesh meshFloor = GenMeshPlane(60.0f, 80.0f, 15, 20); // Tessellate to prevent affine warp
    for (int i = 0; i < meshFloor.vertexCount; i++) {
        meshFloor.texcoords[i * 2 + 0] *= 30.0f; // Tile X 30 times
        meshFloor.texcoords[i * 2 + 1] *= 40.0f; // Tile Y 40 times
    }
    // Raylib GenMeshPlane uploads to GPU. We must update the VRAM buffers!
    UpdateMeshBuffer(meshFloor, 1, meshFloor.texcoords, meshFloor.vertexCount * 2 * sizeof(float), 0);
    m_mdlFloor = LoadModelFromMesh(meshFloor);
    m_mdlFloor.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = m_texFloor;

    // Generate Ceiling Model (same as floor but faces down)
    Mesh meshCeiling = GenMeshPlane(60.0f, 80.0f, 15, 20);
    for (int i = 0; i < meshCeiling.vertexCount; i++) {
        meshCeiling.texcoords[i * 2 + 0] *= 30.0f;
        meshCeiling.texcoords[i * 2 + 1] *= 40.0f;
        meshCeiling.normals[i * 3 + 1] = -1.0f; // Flip normal to face down
    }
    // Flip vertex winding so it's visible from below
    for (int i = 0; i < meshCeiling.triangleCount; i++) {
        unsigned short temp = meshCeiling.indices[i * 3 + 1];
        meshCeiling.indices[i * 3 + 1] = meshCeiling.indices[i * 3 + 2];
        meshCeiling.indices[i * 3 + 2] = temp;
    }
    // Update GPU buffers for texcoords (1), normals (3), and indices (6)
    UpdateMeshBuffer(meshCeiling, 1, meshCeiling.texcoords, meshCeiling.vertexCount * 2 * sizeof(float), 0);
    UpdateMeshBuffer(meshCeiling, 3, meshCeiling.normals, meshCeiling.vertexCount * 3 * sizeof(float), 0);
    UpdateMeshBuffer(meshCeiling, 6, meshCeiling.indices, meshCeiling.triangleCount * 3 * sizeof(unsigned short), 0);
    
    m_mdlCeiling = LoadModelFromMesh(meshCeiling);
    m_mdlCeiling.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = m_texCeiling;

    // 2. Load 3D Artist Maze Model
    m_mdlMaze = LoadModel(GetAssetPath("labirint.obj").c_str());
    if (m_mdlMaze.meshCount == 0) {
        m_mdlMaze = LoadModel(GetAssetPath("лабиринт.obj").c_str());
    }

    if (m_mdlMaze.meshCount > 0) {
        TraceLog(LOG_INFO, "MAZE: Model loaded with %d meshes, %d materials", m_mdlMaze.meshCount, m_mdlMaze.materialCount);
        for (int i = 0; i < m_mdlMaze.materialCount; i++) {
            Texture2D tex = m_mdlMaze.materials[i].maps[MATERIAL_MAP_DIFFUSE].texture;
            TraceLog(LOG_INFO, "MAZE:   material[%d] texture id=%d size=%dx%d", i, tex.id, tex.width, tex.height);
            m_mdlMaze.materials[i].maps[MATERIAL_MAP_DIFFUSE].color = WHITE;
            // Generate mipmaps and set filtering on the artist's MTL-loaded textures
            if (tex.id > 0 && tex.width > 1) {
                SetupTexture(m_mdlMaze.materials[i].maps[MATERIAL_MAP_DIFFUSE].texture);
            }
        }
        for (int i = 0; i < m_mdlMaze.meshCount; i++) {
            TraceLog(LOG_INFO, "MAZE:   mesh[%d] -> meshMaterial=%d (before fix)", i, m_mdlMaze.meshMaterial[i]);
        }

        // Raylib 5.0 bug: meshMaterial is always 0 regardless of usemtl in OBJ.
        // Our split OBJ has: mesh[0]=WallsGreen(Material.001), mesh[1]=WallsWhite(Material.002)
        // Fix mesh-to-material mapping, but keep the artist's own UV-baked textures from MTL:
        if (m_mdlMaze.meshCount >= 2 && m_mdlMaze.materialCount >= 2) {
            m_mdlMaze.meshMaterial[0] = 0;  // WallsGreen -> material[0] (Material.001 texture)
            m_mdlMaze.meshMaterial[1] = 1;  // WallsWhite -> material[1] (Material.002 texture)
            TraceLog(LOG_INFO, "MAZE: Fixed mesh-material mapping: mesh[0]->mat[0], mesh[1]->mat[1]");
        }

        // Procedurally apply custom shader to the maze
        std::string vsPath = "assets/shaders/wall.vs";
        std::string fsPath = "assets/shaders/wall.fs";
        m_shdWall = LoadShader(vsPath.c_str(), fsPath.c_str());

        m_shdFlat = LoadShader("assets/shaders/wall.vs", "assets/shaders/flat.fs");
        
        m_mdlFloor.materials[0].shader = m_shdFlat;
        m_mdlCeiling.materials[0].shader = m_shdFlat;

        // Get shader locations
        int locTex0 = GetShaderLocation(m_shdWall, "texture0");
        int locTex1 = GetShaderLocation(m_shdWall, "texture1");

        for (int i = 0; i < m_mdlMaze.materialCount; i++) {
            m_mdlMaze.materials[i].shader = m_shdWall;
            // Bind our seamless textures to the material map slots
            m_mdlMaze.materials[i].maps[MATERIAL_MAP_DIFFUSE].texture = m_texGreen;
            m_mdlMaze.materials[i].maps[MATERIAL_MAP_METALNESS].texture = m_texWhite; // Slot 1
            
            int texUnit1 = 1;
            SetShaderValue(m_shdWall, locTex1, &texUnit1, SHADER_UNIFORM_INT);
        }
    }
    // 3. Load physics AABB boxes from maze_physics.txt
    std::string physPath = GetAssetPath("maze_physics.txt");
    FILE* fpPhys = fopen(physPath.c_str(), "r");
    if (fpPhys) {
        int count = 0;
        if (fscanf(fpPhys, "%d", &count) == 1) {
            for (int i = 0; i < count; i++) {
                float minX, minY, minZ, maxX, maxY, maxZ;
                if (fscanf(fpPhys, "%f %f %f %f %f %f", &minX, &minY, &minZ, &maxX, &maxY, &maxZ) == 6) {
                    Vector3 size = { maxX - minX, maxY - minY, maxZ - minZ };
                    Vector3 pos = { minX + size.x / 2.0f, minY + size.y / 2.0f, minZ + size.z / 2.0f };
                    m_vecPhysicsBoxes.push_back({ BOX_WALL_GREEN, pos, size, 0.0f });
                }
            }
        }
        fclose(fpPhys);
    }
}

static Color ScaleColor(Color c, float factor)
{
    return Color{
        (unsigned char)Clamp(c.r * factor, 0.0f, 255.0f),
        (unsigned char)Clamp(c.g * factor, 0.0f, 255.0f),
        (unsigned char)Clamp(c.b * factor, 0.0f, 255.0f),
        c.a
    };
}

void CMap::DrawCubeTexturedCustom(Texture2D texture, Vector3 vecPosition, float flWidth, float flHeight, float flLength, Color color)
{
    if (texture.id > 0)
    {
        rlSetTexture(0);
        rlSetTexture(texture.id);
    }

    float x = vecPosition.x;
    float y = vecPosition.y;
    float z = vecPosition.z;
    float w = flWidth / 2.0f;
    float h = flHeight / 2.0f;
    float l = flLength / 2.0f;

    float uW = (flWidth  > 0.1f) ? (flWidth  / 2.0f) : 1.0f;
    float vH = (flHeight > 0.1f) ? (flHeight / 2.0f) : 1.0f;
    float uL = (flLength > 0.1f) ? (flLength / 2.0f) : 1.0f;

    Color colZ = ScaleColor(color, 1.00f);
    Color colX = ScaleColor(color, 0.82f);
    Color colTop = ScaleColor(color, 0.72f);
    Color colBot = ScaleColor(color, 0.90f);

    rlBegin(RL_QUADS);

    // Front Face (+Z)
    rlColor4ub(colZ.r, colZ.g, colZ.b, colZ.a);
    rlNormal3f(0.0f, 0.0f, 1.0f);
    rlTexCoord2f(0.0f, 0.0f); rlVertex3f(x - w, y - h, z + l);
    rlTexCoord2f(uW, 0.0f);   rlVertex3f(x + w, y - h, z + l);
    rlTexCoord2f(uW, vH);     rlVertex3f(x + w, y + h, z + l);
    rlTexCoord2f(0.0f, vH);   rlVertex3f(x - w, y + h, z + l);

    // Back Face (-Z)
    rlColor4ub(colZ.r, colZ.g, colZ.b, colZ.a);
    rlNormal3f(0.0f, 0.0f, -1.0f);
    rlTexCoord2f(uW, 0.0f);   rlVertex3f(x - w, y - h, z - l);
    rlTexCoord2f(uW, vH);     rlVertex3f(x - w, y + h, z - l);
    rlTexCoord2f(0.0f, vH);   rlVertex3f(x + w, y + h, z - l);
    rlTexCoord2f(0.0f, 0.0f); rlVertex3f(x + w, y - h, z - l);

    // Top Face (+Y)
    rlColor4ub(colTop.r, colTop.g, colTop.b, colTop.a);
    rlNormal3f(0.0f, 1.0f, 0.0f);
    rlTexCoord2f(0.0f, uL);   rlVertex3f(x - w, y + h, z - l);
    rlTexCoord2f(0.0f, 0.0f); rlVertex3f(x - w, y + h, z + l);
    rlTexCoord2f(uW, 0.0f);   rlVertex3f(x + w, y + h, z + l);
    rlTexCoord2f(uW, uL);     rlVertex3f(x + w, y + h, z - l);

    // Bottom Face (-Y)
    rlColor4ub(colBot.r, colBot.g, colBot.b, colBot.a);
    rlNormal3f(0.0f, -1.0f, 0.0f);
    rlTexCoord2f(uW, 0.0f);   rlVertex3f(x - w, y - h, z - l);
    rlTexCoord2f(0.0f, 0.0f); rlVertex3f(x + w, y - h, z - l);
    rlTexCoord2f(0.0f, uL);   rlVertex3f(x + w, y - h, z + l);
    rlTexCoord2f(uW, uL);     rlVertex3f(x - w, y - h, z + l);

    // Right Face (+X)
    rlColor4ub(colX.r, colX.g, colX.b, colX.a);
    rlNormal3f(1.0f, 0.0f, 0.0f);
    rlTexCoord2f(uL, 0.0f);   rlVertex3f(x + w, y - h, z - l);
    rlTexCoord2f(uL, vH);     rlVertex3f(x + w, y + h, z - l);
    rlTexCoord2f(0.0f, vH);   rlVertex3f(x + w, y + h, z + l);
    rlTexCoord2f(0.0f, 0.0f); rlVertex3f(x + w, y - h, z + l);

    // Left Face (-X)
    rlColor4ub(colX.r, colX.g, colX.b, colX.a);
    rlNormal3f(-1.0f, 0.0f, 0.0f);
    rlTexCoord2f(0.0f, 0.0f); rlVertex3f(x - w, y - h, z - l);
    rlTexCoord2f(uL, 0.0f);   rlVertex3f(x - w, y - h, z + l);
    rlTexCoord2f(uL, vH);     rlVertex3f(x - w, y + h, z + l);
    rlTexCoord2f(0.0f, vH);   rlVertex3f(x - w, y + h, z - l);

    rlEnd();
}

void CMap::DrawModelTiled(Model model, Box b, float targetHeight, float targetY) {
    if (model.meshCount == 0) return;

    BoundingBox bbox = GetModelBoundingBox(model);
    Vector3 modelSize = {
        bbox.max.x - bbox.min.x,
        bbox.max.y - bbox.min.y,
        bbox.max.z - bbox.min.z
    };
    Vector3 modelCenter = {
        (bbox.min.x + bbox.max.x) / 2.0f,
        (bbox.min.y + bbox.max.y) / 2.0f,
        (bbox.min.z + bbox.max.z) / 2.0f
    };

    float scaleX = b.m_vecSize.x / (modelSize.x > 0 ? modelSize.x : 4.0f);
    float scaleY = targetHeight / (modelSize.y > 0 ? modelSize.y : 1.0f);
    float scaleZ = b.m_vecSize.z / (modelSize.z > 0 ? modelSize.z : 1.0f);

    Matrix matCenter  = MatrixTranslate(-modelCenter.x, -modelCenter.y, -modelCenter.z);
    Matrix matScale   = MatrixScale(scaleX, scaleY, scaleZ);
    Matrix matRotate  = MatrixRotateY(-b.m_flRotation * DEG2RAD);
    Matrix matWallPos = MatrixTranslate(b.m_vecPosition.x, targetY, b.m_vecPosition.z);

    Matrix transform = MatrixMultiply(
        MatrixMultiply(
            MatrixMultiply(matCenter, matScale),
            matRotate),
        matWallPos);
    transform = MatrixMultiply(model.transform, transform);

    for (int m = 0; m < model.meshCount; m++) {
        int matIdx = (model.meshMaterial != nullptr) ? model.meshMaterial[m] : m;
        if (matIdx >= 0 && matIdx < model.materialCount) {
            DrawMesh(model.meshes[m], model.materials[matIdx], transform);
        }
    }
}


void CMap::DrawModelTiledFloor(Model model, Box b, float targetY) {

    if (model.meshCount == 0) return;


    BoundingBox bbox = GetModelBoundingBox(model);
    Vector3 modelSize = {
        bbox.max.x - bbox.min.x,
        bbox.max.y - bbox.min.y,
        bbox.max.z - bbox.min.z
    };
    Vector3 modelCenter = {
        (bbox.min.x + bbox.max.x) / 2.0f,
        (bbox.min.y + bbox.max.y) / 2.0f,
        (bbox.min.z + bbox.max.z) / 2.0f
    };

    float scaleY = b.m_vecSize.y / (modelSize.y > 0 ? modelSize.y : 1.0f);
    
    float tileWidthX = modelSize.x > 0.1f ? modelSize.x : 4.0f;
    float tileWidthZ = modelSize.z > 0.1f ? modelSize.z : 4.0f;
    
    int numTilesX = (int)ceil(b.m_vecSize.x / tileWidthX);
    int numTilesZ = (int)ceil(b.m_vecSize.z / tileWidthZ);
    
    float startX = -(b.m_vecSize.x / 2.0f);
    float startZ = -(b.m_vecSize.z / 2.0f);
    
    rlPushMatrix();
    rlTranslatef(b.m_vecPosition.x, targetY, b.m_vecPosition.z);
    rlRotatef(-b.m_flRotation, 0.0f, 1.0f, 0.0f);
    
    for (int ix = 0; ix < numTilesX; ix++) {
        float cx = startX + (ix * tileWidthX) + (tileWidthX / 2.0f);
        float currentW = tileWidthX;
        if (ix == numTilesX - 1) {
            currentW = b.m_vecSize.x - (ix * tileWidthX);
            cx = startX + (ix * tileWidthX) + (currentW / 2.0f);
        }
        float scaleX = currentW / tileWidthX;

        for (int iz = 0; iz < numTilesZ; iz++) {
            float cz = startZ + (iz * tileWidthZ) + (tileWidthZ / 2.0f);
            float currentL = tileWidthZ;
            if (iz == numTilesZ - 1) {
                currentL = b.m_vecSize.z - (iz * tileWidthZ);
                cz = startZ + (iz * tileWidthZ) + (currentL / 2.0f);
            }
            float scaleZ = currentL / tileWidthZ;
            
            rlPushMatrix();
            rlTranslatef(cx, 0.0f, cz);
            rlScalef(scaleX, scaleY, scaleZ);
            rlTranslatef(-modelCenter.x, -modelCenter.y, -modelCenter.z); 
            
            DrawModel(model, {0,0,0}, 1.0f, WHITE);
            
            rlPopMatrix();
        }
    }
    
    rlPopMatrix();
}

void CMap::DrawMap() {
    if (m_mdlMaze.meshCount > 0) {
        // Textures are assigned in Init() with correct material indices.
        // The shader uses fragPos.y to sample texture0 (green) or texture1 (white)     // No per-frame texture override needed.

        // 1. Draw 3D Artist Maze Walls
        DrawModel(m_mdlMaze, { 0.0f, 0.0f, 0.0f }, 1.0f, WHITE);

        // 2. Draw Floor Plane (at Y = -1.35f)
        DrawModel(m_mdlFloor, { 0.0f, -1.45f, 0.0f }, 1.0f, WHITE);

        // 3. Draw Ceiling Plane (at Y = +1.45f), rotated 180 on X so normals face DOWN
        DrawModelEx(m_mdlCeiling, { 0.0f, 1.45f, 0.0f }, { 1.0f, 0.0f, 0.0f }, 180.0f, { 1.0f, 1.0f, 1.0f }, WHITE);
        
        // 4. Draw Exit Portal
        DrawCube(m_vecExitPos, 1.5f, 2.0f, 1.5f, Color{100, 255, 100, 255}); // Glowing green door
        DrawCubeWires(m_vecExitPos, 1.55f, 2.05f, 1.55f, WHITE);
    } else {
        if (m_mdlWall.materialCount >= 2) {
            if (m_bSwapTextures) {
                m_mdlWall.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = m_texGreen;
                m_mdlWall.materials[1].maps[MATERIAL_MAP_DIFFUSE].texture = m_texWhite;
            } else {
                m_mdlWall.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = m_texWhite;
                m_mdlWall.materials[1].maps[MATERIAL_MAP_DIFFUSE].texture = m_texGreen;
            }
        }

        for (const auto& b : m_vecBoxes) {
            if (b.m_eType == BOX_WALL_WHITE) continue;
            if (b.m_eType == BOX_WALL_GREEN) {
                DrawModelTiled(m_mdlWall, b, 3.6f, 1.8f);
            }
            else if (b.m_eType == BOX_FLOOR) {
                DrawModelTiledFloor(m_mdlFloor, b, b.m_vecPosition.y);
            }
            else if (b.m_eType == BOX_CEILING) {
                DrawCubeTexturedCustom(m_texCeiling, b.m_vecPosition, b.m_vecSize.x, b.m_vecSize.y, b.m_vecSize.z, WHITE);
            }
        }
    }

    rlSetTexture(0);
}

void CMap::Unload()
{
    UnloadTexture(m_texGreen);
    UnloadTexture(m_texWhite);
    UnloadTexture(m_texFloor);
    UnloadTexture(m_texCeiling);

    if (m_mdlMaze.meshCount > 0) UnloadModel(m_mdlMaze);
    if (m_mdlWall.meshCount > 0) UnloadModel(m_mdlWall);
    if (m_mdlFloor.meshCount > 0) UnloadModel(m_mdlFloor);
    if (m_mdlWallFloor.meshCount > 0) UnloadModel(m_mdlWallFloor);
}
