#pragma once
#include <memory>
#include <cstdio>
#include "raylib.h"

class CVHSEffect
{
public:
	CVHSEffect();
	~CVHSEffect() = default;

	void Init(int iWidth, int iHeight);
	void Resize(int iWidth, int iHeight);
	void BeginRender();
	void EndRender();
	void RenderToScreen(bool bFlashlightOn);
	void RenderFoundFootageHUD(bool bIsFPSMode, bool bFlashlightOn);
	void Shutdown();

	RenderTexture2D m_RenderTarget;
	Shader m_VHSShader;
	bool m_bEnableVHS;
	bool m_bEnableVHSTapeEffects;
	float m_flVHSIntensity;
	float m_flDitherIntensity;

private:
	int m_iTimeLoc;
	int m_iRenderSizeLoc;
	int m_iVHSIntensityLoc;
	int m_iFlashlightOnLoc;
	int m_iEnableVHSTapeEffectsLoc;
	int m_iDitherIntensityLoc;
};

inline std::unique_ptr<CVHSEffect> g_pVHSEffect = std::make_unique<CVHSEffect>();
