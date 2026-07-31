#pragma once
#include <raylib.h>
#include <vector>
#include <memory>

enum BoxType {
    BOX_WALL_GREEN = 0,
    BOX_WALL_WHITE = 1,
    BOX_FLOOR = 2,
    BOX_CEILING = 3
};

struct Box {
    BoxType m_eType;
    Vector3 m_vecPosition;
    Vector3 m_vecSize;
    float m_flRotation; // Yaw angle in degrees
};

class CMap
{
public:
    CMap() = default;
    ~CMap() = default;
    bool m_bSwapTextures = false;

    void Init();
    void Unload();
    void DrawMap();

    const std::vector<Box>& GetBoxes() const { return m_vecPhysicsBoxes; }
    Vector3 GetSpawnPosition() const { return m_vecSpawnPos; }
    
    Vector3 GetMapCenter() const {
        return { 0.0f, -1.35f, 0.0f };
    }

private:
    void DrawCubeTexturedCustom(Texture2D texture, Vector3 vecPosition, float flWidth, float flHeight, float flLength, Color color);
    void DrawModelTiled(Model model, Box b, float targetHeight, float targetY);
    void DrawModelTiledFloor(Model model, Box b, float targetY);

    Texture2D m_texGreen;
    Texture2D m_texWhite;
    Texture2D m_texFloor;
    Texture2D m_texCeiling;

    Model m_mdlMaze;
    Model m_mdlWall;
    Model m_mdlFloor;
    Model m_mdlCeiling;
    Model m_mdlWallFloor;
    Shader m_shdWall;

    std::vector<Box> m_vecBoxes;          // Visual 3D boxes
    std::vector<Box> m_vecPhysicsBoxes;   // Physics AABB boxes
    Vector3 m_vecSpawnPos = { -18.0f, -1.35f, 27.0f };
};

inline std::unique_ptr<CMap> g_pMap = std::make_unique<CMap>();