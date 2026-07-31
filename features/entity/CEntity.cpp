#include "CEntity.h"

CEntity::CEntity(Vector3 vecPos, Vector3 vecViewAngles, float flHeight, Vector3 vecVelocity)
{
	m_flHeight = flHeight;
	m_vecPosition = vecPos;
	m_vecViewAngles = vecViewAngles;
	m_vecVelocity = vecVelocity;
}