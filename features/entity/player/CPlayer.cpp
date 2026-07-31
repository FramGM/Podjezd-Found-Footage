#include "CPlayer.h"
#include <exception>
#include <algorithm>
#include <raymath.h>
#include "../../soundsystem/CSoundSystem.h"

CPlayer::CPlayer(Vector3 vecPos, Vector3 vecViewAngles, float flHeight, Vector3 vecVelocity) : CEntity(vecPos, vecViewAngles, flHeight, vecVelocity) 
{ 
	m_Camera = { 0 };
	m_Camera.fovy = 70;
	m_Camera.position = vecPos;
	m_Camera.up = { 0.0f, 1.0f, 0.0f };
	m_Camera.projection = CAMERA_PERSPECTIVE;
}

CPlayer::CPlayer(Vector3 vecPos, Vector3 vecViewAngles, Camera3D _Camera, float flHeight, Vector3 vecVelocity) : CEntity(vecPos, vecViewAngles, flHeight, vecVelocity)
{
	m_Camera = _Camera;
}

void CPlayer::SetFOV(float flFOV)
{
	m_Camera.fovy = std::clamp(flFOV, 1.f, 180.f);
}

void CPlayer::UpdateCamera()
{
	float flBobOffsetY = sinf(m_flBobbingTimer) * 0.05f * m_flBobbingAmount * m_flBobbingAmountMult;
	float flBobOffsetX = cosf(m_flBobbingTimer * 0.5f) * 0.035f * m_flBobbingAmount * m_flBobbingAmountMult;

	this->m_Camera.position = { this->GetEntityPos().x + flBobOffsetX, this->GetEntityPos().y + this->GetHeight() - 0.2f + flBobOffsetY, this->GetEntityPos().z };
	Vector3 lookDir = { cosf(this->GetViewAngles().x) * cosf(this->GetViewAngles().y), sinf(this->GetViewAngles().x), -cosf(this->GetViewAngles().x) * sinf(this->GetViewAngles().y) };
	this->m_Camera.target = Vector3Add(this->m_Camera.position, lookDir);
}

void CPlayer::DoSound()
{
	if (!this->IsGrounded())
	{
		g_pSoundSystem.get()->StopActionSound("footstep_run");
		g_pSoundSystem.get()->StopActionSound("footstep_walk");
		return;
	}

	float flCurrentTime = (float)GetTime();

	if (this->m_State.CheckState(STATE_WALK) || this->m_State.CheckState(STATE_RUN))
	{
		if (flCurrentTime - m_flLastStepTime >= 0.2f)
		{
			if (this->m_State.CheckState(STATE_RUN))
			{
				g_pSoundSystem.get()->SetActionSoundVolume("white_noise", 1.f);
				g_pSoundSystem.get()->SetActionSoundVolume("cosmic_noise", 1.f);

				g_pSoundSystem.get()->StopActionSound("footstep_walk");

				if (!g_pSoundSystem.get()->IsActionSoundPlaying("footstep_run"))
					g_pSoundSystem.get()->PlayActionSound("footstep_run");
			}
			else
			{
				g_pSoundSystem.get()->SetActionSoundVolume("white_noise", 0.5f);
				g_pSoundSystem.get()->SetActionSoundVolume("cosmic_noise", 0.5f);


				g_pSoundSystem.get()->StopActionSound("footstep_run");

				if (!g_pSoundSystem.get()->IsActionSoundPlaying("footstep_walk"))
					g_pSoundSystem.get()->PlayActionSound("footstep_walk");
			}
			m_flLastStepTime = flCurrentTime;
		}
	}
	else
	{
		g_pSoundSystem.get()->SetActionSoundVolume("white_noise", 0.15f);
		g_pSoundSystem.get()->SetActionSoundVolume("cosmic_noise", 0.15f);

		g_pSoundSystem.get()->StopActionSound("footstep_run");
		g_pSoundSystem.get()->StopActionSound("footstep_walk");
		m_flLastStepTime = 0.0f;
	}
}
