#pragma once
#include <memory>
class CPlayer;
class CMovement
{
public:
	void OnRun(CPlayer* pPlayer, bool bRun);
	const float GetCurrentFrameTime() { return m_flFrameTime; }
private:
	float m_flFrameTime;
	void HandleMouseLook(CPlayer* pPlayer);
	void HandleMovementInput(CPlayer* pPlayer);
	void HandleGravityAndJump(CPlayer* pPlayer);
};
inline std::unique_ptr<CMovement> g_pMovement = std::make_unique<CMovement>();