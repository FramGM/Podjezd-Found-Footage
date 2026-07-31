#pragma once
#include "../CEntity.h"
#include <stdint.h>

struct Timestamp
{
	float m_flStartTime;
	float m_flEndTime;
};

enum EStates : uint16_t
{
	STATE_IDLE = (1 << 0),
	STATE_WALK = (1 << 1),
	STATE_RUN = (1 << 2)
};

struct PlayerState
{
	void AddState(EStates iState) { m_fFlags |= iState; }
	void RemoveState(EStates iState) { m_fFlags &= ~iState; }

	void ClearStates() { m_fFlags = 0; }

	bool HasNoStates() const { return m_fFlags == 0; }
	bool CheckState(EStates iState) const { return (m_fFlags & iState) != 0; }
private:
	uint16_t m_fFlags = 0;
};

class CPlayer : public CEntity
{
public:
	CPlayer(Vector3 vecPos, Vector3 vecViewAngles, float flHeight = 1.8f, Vector3 vecVelocity = { 0,0,0 });
	CPlayer(Vector3 vecPos, Vector3 vecViewAngles, Camera3D _Camera, float flHeight, Vector3 vecVelocity = { 0,0,0 });

	Camera3D GetCamera() const { return m_Camera; }

	//automatically clamping 1 - 180
	void SetFOV(float flFOV);
	void UpdateCamera();
	void DoSound();

	bool m_bFlashlightOn = true;
	bool m_bIsSprinting = false;

	// View Bobbing dynamics
	float m_flBobbingTimer = 0.0f;
	float m_flBobbingAmount = 0.0f;
	float m_flBobbingAmountMult = 1.0f;

	float m_flLastStepTime = 0.0f;

	PlayerState m_State;
private:
	Camera3D m_Camera;
};

