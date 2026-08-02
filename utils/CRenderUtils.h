#pragma once
#include <string>
#include <memory>

struct TextPrintData
{
	TextPrintData();
	TextPrintData(std::string strText, float flStartTime, float flSpeed, bool bIsActive = true)
		: m_strFullText(strText), m_flStartTime(flStartTime), m_flSpeed(flSpeed), m_bIsActive(bIsActive) {
	}

	bool m_bIsActive;
	std::string m_strFullText;
	float m_flStartTime;
	float m_flSpeed;
};

class CRenderUtils
{
public:
	void PrintText(TextPrintData _Data);
};

inline std::unique_ptr<CRenderUtils> g_pRenderUtils = std::make_unique<CRenderUtils>();