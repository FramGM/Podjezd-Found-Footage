#include "CRayTrace.h"
#include "../../features/map/CMap.h"
#include "../../features/physics/CPhysics.h"

bool CRayTrace::CheckLineOfSight(Vector3 vecStartPos, Vector3 vecEndPos)
{
	//if ray on y = 0, entity saw on any distance
	vecStartPos.y += 1.0f;
	vecEndPos.y += 1.0f;

	float targetDist = Vector3Distance(vecStartPos, vecEndPos);
	Vector3 dir = Vector3Normalize(Vector3Subtract(vecEndPos, vecStartPos));
	Ray ray = { vecStartPos, dir };

	for (const auto& b : g_pMap.get()->GetBoxes()) {
		BoundingBox bBox = g_pPhysics.get()->GetBoxAABB(b);
		RayCollision hit = GetRayCollisionBox(ray, bBox);
		if (hit.hit && hit.distance < targetDist) {
			return false; // View is blocked by a wall
		}
	}

	return true;
}
