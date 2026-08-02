#pragma once
#include <raylib.h>
class CEntity
{
public:
    CEntity(Vector3 vecPos, Vector3 vecViewAngles, float flHeight, Vector3 vecVelocity = { 0,0,0 });
    const Vector3& GetEntityPos() const { return m_vecPosition; }
    void SetEntityPos(const Vector3& vecPos) { m_vecPosition = vecPos; }
    const Vector3& GetVelocity() const { return m_vecVelocity; }
    void SetVelocity(const Vector3& vecVelocity) { m_vecVelocity = vecVelocity; }
    const Vector3& GetViewAngles() const { return m_vecViewAngles; }
    void SetViewAngles(const Vector3& vecViewAngles) { m_vecViewAngles = vecViewAngles; }
    float GetRadius() const { return m_flRadius; }
    void SetRadius(float flRadius) { m_flRadius = flRadius; }
    bool IsGrounded() const { return m_bOnGround; }
    void SetGrounded(bool bOnGround) { m_bOnGround = bOnGround; }
    float GetHeight() const { return m_flHeight; }
    void SetHeight(float flHeight) { m_flHeight = flHeight; }
private:
    Vector3 m_vecPosition;
    Vector3 m_vecVelocity;
    Vector3 m_vecViewAngles;
    float m_flRadius = 0.2f;
    float m_flHeight;
    bool m_bOnGround = false;
};