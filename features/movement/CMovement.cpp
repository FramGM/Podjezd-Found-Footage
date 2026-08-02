#include "CMovement.h"
#include "../entity/player/CPlayer.h"
#include "../soundsystem/CSoundSystem.h"
#include <raylib.h>
#include "raymath.h"
void CMovement::OnRun(CPlayer* pPlayer, bool bRun)
{
    m_flFrameTime = GetFrameTime();
    if (!bRun)
    {
        Vector3 vel = pPlayer->GetVelocity();
        vel.x = 0.0f;
        vel.z = 0.0f;
        vel.y -= 22.0f * m_flFrameTime;
        pPlayer->SetVelocity(vel);
        pPlayer->m_flBobbingAmount = Lerp(pPlayer->m_flBobbingAmount, 0.0f, 8.0f * m_flFrameTime);  
        return;
    }
    if (IsKeyPressed(KEY_F))
    {
        pPlayer->m_bFlashlightOn = !pPlayer->m_bFlashlightOn;
        g_pSoundSystem.get()->SetActionSoundVolume("click", 0.1f);
        g_pSoundSystem.get()->PlayActionSound("click");
    }
    HandleMouseLook(pPlayer);
    HandleMovementInput(pPlayer);
    HandleGravityAndJump(pPlayer);
}
void CMovement::HandleMouseLook(CPlayer* pPlayer)
{
    Vector2 mouseDelta = GetMouseDelta();
    Vector3 angles = pPlayer->GetViewAngles();
    angles.y -= mouseDelta.x * 0.003f;
    angles.x -= mouseDelta.y * 0.003f;
    angles.x = Clamp(angles.x, -88.0f * DEG2RAD, 88.0f * DEG2RAD);
    while (angles.y > PI) angles.y -= 2.0f * PI;
    while (angles.y < -PI) angles.y += 2.0f * PI;
    pPlayer->SetViewAngles(angles);
}
void CMovement::HandleMovementInput(CPlayer* pPlayer)
{
    bool bIsSprinting = IsKeyDown(KEY_LEFT_SHIFT);
    pPlayer->m_bIsSprinting = bIsSprinting;
    float speed = bIsSprinting ? 4.2f : 2.3f; 
    Vector3 angles = pPlayer->GetViewAngles();
    Vector3 forward = { cosf(angles.y), 0.0f, -sinf(angles.y) };
    Vector3 right = { sinf(angles.y), 0.0f, cosf(angles.y) };
    Vector3 input = { 0 };
    if (IsKeyDown(KEY_W)) input = Vector3Add(input, forward);
    if (IsKeyDown(KEY_S)) input = Vector3Subtract(input, forward);
    if (IsKeyDown(KEY_A)) input = Vector3Subtract(input, right);
    if (IsKeyDown(KEY_D)) input = Vector3Add(input, right);
    float flCurrentSpeed = Vector3Length(input);
    if (flCurrentSpeed > 0.0f) {
        input = Vector3Normalize(input);
        input = Vector3Scale(input, speed);
    }
    Vector3 vel = pPlayer->GetVelocity();
    vel.x = input.x;
    vel.z = input.z;
    pPlayer->SetVelocity(vel); 
    pPlayer->m_State.ClearStates();
    pPlayer->m_State.AddState(flCurrentSpeed <= 0.1f ? STATE_IDLE : bIsSprinting ? STATE_RUN : STATE_WALK);
    float flStepFrequency = bIsSprinting ? 11.5f : 8.0f;
    float flTargetBobbing = (flCurrentSpeed > 0.0f && pPlayer->IsGrounded()) ? (bIsSprinting ? 1.3f : 0.9f) : 0.0f;
    pPlayer->m_flBobbingAmount = Lerp(pPlayer->m_flBobbingAmount, flTargetBobbing, 8.0f * m_flFrameTime);
    float oldTimer = pPlayer->m_flBobbingTimer;
    pPlayer->m_flBobbingTimer += m_flFrameTime * flStepFrequency * (flCurrentSpeed > 0.0f ? 1.0f : 0.0f);
    if (pPlayer->m_flBobbingTimer > PI * 4.0f) {
        pPlayer->m_flBobbingTimer -= PI * 4.0f;
        oldTimer -= PI * 4.0f;
    }
}
void CMovement::HandleGravityAndJump(CPlayer* pPlayer)
{
    Vector3 vel = pPlayer->GetVelocity();
    vel.y -= 22.0f * m_flFrameTime;
    if (pPlayer->IsGrounded() && IsKeyPressed(KEY_SPACE)) 
    {
        vel.y = 7.5f; 
        pPlayer->SetGrounded(false);
    }
    pPlayer->SetVelocity(vel);
}