#include "CEntityList.h"
#include <exception>

void CEntityList::AddEntity(CEntity* pEntity)
{
	if (!pEntity)
		return;

	for (int i = 0; i < m_vecEntityList.size(); i++)
	{
		if (m_vecEntityList.at(i) == pEntity)
			throw std::exception("Entity already exists in the list");
	}

	m_vecEntityList.push_back(pEntity);
}

void CEntityList::RemoveEntity(CEntity* pEntity)
{
	if (!pEntity)
		return;

	for (int i = 0; i < m_vecEntityList.size(); i++)
	{
		if (m_vecEntityList.at(i) == pEntity)
		{
			m_vecEntityList.erase(m_vecEntityList.begin() + i);
			return;
		}
	}

	throw std::exception("Entity not found in the list");
}

void CEntityList::RemoveEntity(int iIndex)
{
	if (iIndex < 0 || iIndex >= m_vecEntityList.size())
		throw std::exception("Index out of bounds");

	m_vecEntityList.erase(m_vecEntityList.begin() + iIndex);
}

CEntity* CEntityList::GetEntity(int iIndex)
{
	if (iIndex < 0 || iIndex >= m_vecEntityList.size())
		throw std::exception("Index out of bounds");

	return m_vecEntityList.at(iIndex);
}
