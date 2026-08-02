#include "CBot.h"
#include "../../../utils/raytrace/CRayTrace.h"
#include "../../soundsystem/CSoundSystem.h"
void CBot::Update(float flDeltaTime, CPlayer* pPlayer)
{
	m_pTarget = pPlayer;
	UpdateTargeting(flDeltaTime);
	UpdateMovement(flDeltaTime);
}
void CBot::UpdateTargeting(float flDeltaTime)
{
	if (!m_pTarget) return;
	if (m_pTarget->m_State.CheckState(STATE_RUN))
		m_flVisionRadius = BOT_RANGE_EXTENDED;
	else
		m_flVisionRadius = BOT_RANGE_DEFAULT;
	float dist = Vector3Distance(GetEntityPos(), m_pTarget->GetEntityPos());
	UpdateSounds(flDeltaTime, dist);
	if (dist <= m_flVisionRadius)
	{
		bool canSee = CRayTrace::CheckLineOfSight(GetEntityPos(), m_pTarget->GetEntityPos());
		if (canSee)
		{
			if (!g_pSoundSystem.get()->IsActionSoundPlaying("scream"))
				g_pSoundSystem.get()->PlayActionSound("scream");
			m_State = BOT_HUNTING;
			m_flSpeed = 3.6f;
		}
		else
		{
			if (m_State == BOT_HUNTING) 
			{
				m_State = BOT_PATROLLING;
				m_pTarget = nullptr;
				SetRandomDirection();
			}
		}
	}
	else
	{
		if (m_State == BOT_HUNTING) {
			m_State = BOT_PATROLLING;
			SetRandomDirection();
		}
	}
}
void CBot::UpdateSounds(float flDeltaTime, float dist)
{
	float maxHearingDist = BOT_RANGE_EXTENDED;
	float volume = 1.0f - (dist / maxHearingDist);
	if (volume < 0.0f) volume = 0.0f;
	if (volume > 1.0f) volume = 1.0f;
	g_pSoundSystem.get()->SetActionSoundVolume("mumbling", volume);
	g_pSoundSystem.get()->SetActionSoundVolume("scream", volume);
	if (dist <= m_flVisionRadius)
	{
		if (!g_pSoundSystem.get()->IsActionSoundPlaying("mumbling"))
		{
			m_flMumbleTimer -= flDeltaTime;
			if (m_flMumbleTimer <= 0.0f)
			{
				g_pSoundSystem.get()->PlayActionSound("mumbling");
				m_flMumbleTimer = (float)GetRandomValue(300, 800) / 100.0f; 
			}
		}
	}
}
void CBot::UpdateMovement(float flDeltaTime)
{
	if (m_State == BOT_IDLE)
	{
		m_State = BOT_PATROLLING;
		SetRandomDirection();
	}
	if (m_State == BOT_HUNTING && m_pTarget)
	{
		Vector3 dir = Vector3Normalize(Vector3Subtract(m_pTarget->GetEntityPos(), GetEntityPos()));
		SetVelocity({ dir.x * m_flSpeed, GetVelocity().y, dir.z * m_flSpeed });
	}
	else if (m_State == BOT_PATROLLING)
	{
		Vector3 vel = GetVelocity();
		if (fabs(vel.x) < 0.1f && fabs(vel.z) < 0.1f) {
			SetRandomDirection();
		}
		float distMoved = Vector3Distance(GetEntityPos(), m_vecLastPosition);
		if (distMoved < m_flSpeed * flDeltaTime * 0.1f)
		{
			m_flStuckTimer += flDeltaTime;
			if (m_flStuckTimer > 0.2f) {
				SetRandomDirection();
				m_flStuckTimer = 0.0f;
			}
		}
		else
		{
			m_flStuckTimer = 0.0f;
		}
	}
	m_vecLastPosition = GetEntityPos();
}
void CBot::Init()
{
	m_TexBabka = LoadTexture("assets/bot/babka.png");
}

CBot::~CBot()
{
	if (m_TexBabka.id != 0) {
		UnloadTexture(m_TexBabka);
	}
}

void CBot::Draw(Camera3D& camera)
{
	if (m_TexBabka.id != 0) {
		// Calculate center position (shift up by half height)
		Vector3 centerPos = GetEntityPos();
		centerPos.y += GetHeight() / 2.0f;
		
		// Use DrawBillboardRec to maintain aspect ratio
		Rectangle source = { 0.0f, 0.0f, (float)m_TexBabka.width, (float)m_TexBabka.height };
		float aspect = (float)m_TexBabka.width / (float)m_TexBabka.height;
		Vector2 size = { GetHeight() * aspect, GetHeight() };
		
		DrawBillboardRec(camera, m_TexBabka, source, centerPos, size, WHITE);
	} else {
		// Fallback to red cube
		DrawCube(GetEntityPos(), 0.6f, GetHeight(), 0.6f, RED);
		DrawCubeWires(GetEntityPos(), 0.6f, GetHeight(), 0.6f, MAROON);
	}
}
void CBot::SetRandomDirection()
{
	float randomAngle = GetRandomValue(0, 360) * DEG2RAD;
	SetVelocity({ cosf(randomAngle) * (m_flSpeed * 0.5f), GetVelocity().y, sinf(randomAngle) * (m_flSpeed * 0.5f) });
}