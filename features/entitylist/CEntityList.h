#pragma once
#include <memory>
#include <vector>
#include "../entity/CEntity.h"

class CEntityList
{
public:
	void AddEntity(CEntity* pEntity);
	void RemoveEntity(CEntity* pEntity);
	void RemoveEntity(int iIndex);
	void ClearEntities() { m_vecEntityList.clear(); }

	CEntity* GetEntity(int iIndex);
	int GetEntityCount() { return m_vecEntityList.size(); }

private:
	std::vector<CEntity*> m_vecEntityList;
};

inline std::unique_ptr<CEntityList> g_pEntityList = std::make_unique<CEntityList>();