#include "CRenderUtils.h"
#include <imgui.h>
#include <string>
#include <algorithm>
void CRenderUtils::PrintText(TextPrintData _Data)
{
    if (!_Data.m_bIsActive)
        return;

    if (_Data.m_flStartTime < 0.0f) {
        _Data.m_flStartTime = ImGui::GetTime();
    }

    float elapsedTime = ImGui::GetTime() - _Data.m_flStartTime;
    int visibleCount = static_cast<int>(elapsedTime * _Data.m_flSpeed);

    visibleCount = std::clamp(visibleCount, 0, static_cast<int>(_Data.m_strFullText.size()));

    std::string currentText = _Data.m_strFullText.substr(0, visibleCount);
    ImGui::Text("%s", currentText.c_str());

    _Data.m_bIsActive = !(currentText == _Data.m_strFullText);
}
