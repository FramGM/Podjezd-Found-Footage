#include "CPhysics.h"
#include "../movement/CMovement.h"
#include "../entity/player/CPlayer.h"
#include "../map/CMap.h"
#include <cmath>
#include "../entitylist/CEntityList.h"

void CPhysics::OnRun()
{
    for (int i = 0; i < g_pEntityList.get()->GetEntityCount(); i++)
    {
        CollisionRun(g_pEntityList.get()->GetEntity(i));
    }
}

void CPhysics::CollisionRun(CEntity* pEntity)
{
    // --- ФИЗИКА И КОЛЛИЗИИ ---
    Vector3 pos = pEntity->GetEntityPos();
    Vector3 vel = pEntity->GetVelocity();
    float radius = pEntity->GetRadius();
    float height = pEntity->GetHeight();

    float frameTime = g_pMovement.get()->GetCurrentFrameTime();
    
    // Sub-stepping to prevent tunneling through thin walls at low framerates
    int steps = 1;
    float maxVel = std::max({std::abs(vel.x), std::abs(vel.y), std::abs(vel.z)});
    float maxDist = maxVel * frameTime;
    if (maxDist > 0.1f) {
        steps = (int)std::ceil(maxDist / 0.1f);
    }
    
    float dt = frameTime / steps;

    for (int s = 0; s < steps; ++s) {
        // Коллизия по оси X
        pos.x += vel.x * dt;
        pEntity->SetEntityPos(pos); // обновим для AABB
        BoundingBox pBox = g_pPhysics.get()->GetPlayerAABB(*dynamic_cast<CEntity*>(pEntity));
        
        for (const auto& b : g_pMap.get()->GetBoxes()) {
            BoundingBox bBox = g_pPhysics.get()->GetBoxAABB(b);
            if (g_pPhysics.get()->CheckCollisionBoxesStrict(pBox, bBox)) {
                if (pos.x < b.m_vecPosition.x) pos.x = bBox.min.x - radius;
                else pos.x = bBox.max.x + radius;
                pEntity->SetEntityPos(pos);
                pBox = g_pPhysics.get()->GetPlayerAABB(*dynamic_cast<CEntity*>(pEntity)); // Обновляем AABB после сдвига
            }
        }

        // Коллизия по оси Z
        pos.z += vel.z * dt;
        pEntity->SetEntityPos(pos);
        pBox = g_pPhysics.get()->GetPlayerAABB(*dynamic_cast<CEntity*>(pEntity));
        
        for (const auto& b : g_pMap.get()->GetBoxes()) {
            BoundingBox bBox = g_pPhysics.get()->GetBoxAABB(b);
            if (g_pPhysics.get()->CheckCollisionBoxesStrict(pBox, bBox)) {
                if (pos.z < b.m_vecPosition.z) pos.z = bBox.min.z - radius;
                else pos.z = bBox.max.z + radius;
                pEntity->SetEntityPos(pos);
                pBox = g_pPhysics.get()->GetPlayerAABB(*dynamic_cast<CEntity*>(pEntity));
            }
        }

        // Коллизия по оси Y
        float prevY = pos.y;
        pos.y += vel.y * dt;
        pEntity->SetEntityPos(pos);
        pEntity->SetGrounded(false);
        pBox = g_pPhysics.get()->GetPlayerAABB(*dynamic_cast<CEntity*>(pEntity));

        // Пол (земля на Y = -1.35f)
        if (pos.y <= -1.35f) {
            pos.y = -1.35f;
            vel.y = 0;
            pEntity->SetGrounded(true);
            pEntity->SetEntityPos(pos);
            pEntity->SetVelocity(vel);
            pBox = g_pPhysics.get()->GetPlayerAABB(*dynamic_cast<CEntity*>(pEntity));
        }

        // Потолок (на Y = 1.35f)
        if (pos.y + height >= 1.35f) {
            pos.y = 1.35f - height;
            if (vel.y > 0) vel.y = 0;
            pEntity->SetEntityPos(pos);
            pEntity->SetVelocity(vel);
            pBox = g_pPhysics.get()->GetPlayerAABB(*dynamic_cast<CEntity*>(pEntity));
        }

        // Кубы по оси Y (приземление на крышу или удар головой)
        for (const auto& b : g_pMap.get()->GetBoxes()) {
            BoundingBox bBox = g_pPhysics.get()->GetBoxAABB(b);
            if (g_pPhysics.get()->CheckCollisionBoxesStrict(pBox, bBox)) {
                if (vel.y <= 0 && prevY >= bBox.max.y - 0.15f) { // Падаем на куб сверху
                    pos.y = bBox.max.y;
                    vel.y = 0;
                    pEntity->SetGrounded(true);
                }
                else if (vel.y > 0 && prevY + height <= bBox.min.y + 0.15f) { // Бьемся головой снизу
                    pos.y = bBox.min.y - height;
                    vel.y = 0;
                }
                pEntity->SetEntityPos(pos);
                pEntity->SetVelocity(vel);
                pBox = g_pPhysics.get()->GetPlayerAABB(*dynamic_cast<CEntity*>(pEntity));
            }
        }
    }
}

BoundingBox CPhysics::GetPlayerAABB(CEntity& p) {
    return {
        { p.GetEntityPos().x - p.GetRadius(), p.GetEntityPos().y, p.GetEntityPos().z - p.GetRadius() },
        { p.GetEntityPos().x + p.GetRadius(), p.GetEntityPos().y + p.GetHeight(), p.GetEntityPos().z + p.GetRadius() }
    };
}

BoundingBox CPhysics::GetBoxAABB(const Box& b) {
    return {
        { b.m_vecPosition.x - b.m_vecSize.x / 2.0f, b.m_vecPosition.y - b.m_vecSize.y / 2.0f, b.m_vecPosition.z - b.m_vecSize.z / 2.0f },
        { b.m_vecPosition.x + b.m_vecSize.x / 2.0f, b.m_vecPosition.y + b.m_vecSize.y / 2.0f, b.m_vecPosition.z + b.m_vecSize.z / 2.0f }
    };
}

// Кастомная проверка коллизий (строго > и < вместо >= и <=)
// Это предотвращает баг, когда касание (==) считается за пересечение.
bool CPhysics::CheckCollisionBoxesStrict(BoundingBox box1, BoundingBox box2)
{
    return ((box1.max.x > box2.min.x) && (box1.min.x < box2.max.x) &&
        (box1.max.y > box2.min.y) && (box1.min.y < box2.max.y) &&
        (box1.max.z > box2.min.z) && (box1.min.z < box2.max.z));
}
