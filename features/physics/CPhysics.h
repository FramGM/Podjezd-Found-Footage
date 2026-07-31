#pragma once
#include <raylib.h>
#include <memory>
#include "../entity/CEntity.h"
#include "../map/CMap.h"

class CPlayer;

class CPhysics
{
public:
    void OnRun();

    void CollisionRun(CEntity* pEntity);

    BoundingBox GetPlayerAABB(CEntity& p);
    BoundingBox GetBoxAABB(const Box& b);
    bool CheckCollisionBoxesStrict(BoundingBox box1, BoundingBox box2);
};

inline std::unique_ptr<CPhysics> g_pPhysics = std::make_unique<CPhysics>();