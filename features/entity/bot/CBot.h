#pragma once
#include "../player/CPlayer.h"
#include <raymath.h>

enum EBotRange : int
{
	BOT_RANGE_DEFAULT = 25,
	BOT_RANGE_EXTENDED = 45
};

enum EBotState : int
{
	BOT_IDLE,
	BOT_PATROLLING,
	BOT_HUNTING
};

class CBot : public CEntity
{
public:
	CBot(Vector3 vecPos) : CEntity(vecPos, Vector3Zero(), 1.8f) {
		m_State = BOT_IDLE;
		m_flSpeed = 3.0f;
		m_flVisionRadius = BOT_RANGE_DEFAULT;
		SetRadius(0.2f);
	}

	void Update(float flDeltaTime, CPlayer* pPlayer);
	void Draw();

	void SetRandomDirection();

private:
	void UpdateTargeting(float flDeltaTime);
	void UpdateSounds(float flDeltaTime, float dist);
	void UpdateMovement(float flDeltaTime);

	EBotState m_State = BOT_IDLE;
	CPlayer* m_pTarget = nullptr;
	
	float m_flSpeed = 3.0f;
	float m_flVisionRadius = BOT_RANGE_DEFAULT;
	float m_flMumbleTimer = 0.0f;
	Vector3 m_vecLastPosition = {0.0f, 0.0f, 0.0f};
	float m_flStuckTimer = 0.0f;
};

