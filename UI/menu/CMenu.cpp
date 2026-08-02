#include "CMenu.h"
#include <imgui.h>
#include <raylib.h>
#include <raymath.h>
#include "../../features/entity/player/CPlayer.h"
#include "../../features/vhs/CVHSEffect.h"
#include "../../features/entitylist/CEntityList.h"
#include "../../features/map/CMap.h"
#include "../../utils/CRenderUtils.h"
void CMenu::Init(bool& isFPSMode, CPlayer* pPlayer)
{
#ifdef DEBUG
	ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_Once);
	ImGui::Begin("First Person Controller HUD", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
	{
		ImGui::Text("Controls:");
		ImGui::Text("WASD - Move | SPACE - Jump | SHIFT - Sprint");
		ImGui::Text("TAB - %s", isFPSMode ? "Show Cursor (Pause)" : "Hide Cursor (Play)");
		ImGui::Separator();
		ImGui::Text("Player Angles:");
		ImGui::Text("Pitch: %.1f deg", pPlayer->GetViewAngles().x * RAD2DEG);
		ImGui::Text("Yaw:   %.1f deg", pPlayer->GetViewAngles().y * RAD2DEG);
		ImGui::Separator();
		ImGui::Text("Player State:");
		ImGui::Text("Speed: %.1f", Vector3Length({ pPlayer->GetVelocity().x, 0, pPlayer->GetVelocity().z }));
		ImGui::Text("Grounded: %s", pPlayer->IsGrounded() ? "YES" : "NO");
		ImGui::Separator();
		ImGui::Text("Settings:");
		ImGui::Checkbox("Enable Post-Processing", &g_pVHSEffect->m_bEnableVHS);
		if (g_pVHSEffect->m_bEnableVHS) {
			ImGui::Checkbox("Enable VHS Tape Effects", &g_pVHSEffect->m_bEnableVHSTapeEffects);
			if (g_pVHSEffect->m_bEnableVHSTapeEffects) {
				ImGui::SliderFloat("VHS Intensity", &g_pVHSEffect->m_flVHSIntensity, 0.0f, 3.0f);
			}
			ImGui::SliderFloat("Dither Intensity", &g_pVHSEffect->m_flDitherIntensity, 0.0f, 3.0f);
		}
	}
	ImGui::End();
#endif 
	if (!m_bShowMenu)
		return;
	ImVec2 vecScreenSize(GetScreenWidth(), GetScreenHeight());
	ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
	ImGui::SetNextWindowSize(vecScreenSize, ImGuiCond_Always);
	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.02f, 0.02f, 0.02f, 0.95f));
	ImGui::Begin("##Main", &m_bShowMenu, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus);
	{
		if (m_bShowIntro) {
			IntroTitle();
		} else {
			MainTitle();
		}
	}
	ImGui::End();
	ImGui::PopStyleColor();
}
void CMenu::MainTitle()
{
	ImVec2 vecScreenSize(GetScreenWidth(), GetScreenHeight());
	ImGui::SetWindowFontScale(4.0f);
	
	const char* title = m_bGameWon ? "YOU ESCAPED!" : (m_bGameOver ? "YOU DIED" : "Podjezd: Found Footage");
	ImVec2 titleSize = ImGui::CalcTextSize(title);
	ImGui::SetCursorPos(ImVec2((vecScreenSize.x - titleSize.x) / 2.0f, vecScreenSize.y * 0.25f));
	
	if (m_bGameWon) {
		ImGui::TextColored(ImVec4(1.0f, 0.8f, 0.2f, 1.0f), "%s", title);
	} else if (m_bGameOver) {
		ImGui::TextColored(ImVec4(0.8f, 0.1f, 0.1f, 1.0f), "%s", title);
	} else {
		ImGui::TextColored(ImVec4(0.2f, 0.8f, 0.3f, 1.0f), "%s", title);
	}
	
	ImGui::SetWindowFontScale(1.0f);
	ImGui::SetWindowFontScale(1.2f);
	const char* subtitle = m_bGameWon ? "You found the way out." : (m_bGameOver ? "The recording ends here." : "Can you find the exit?");
	ImVec2 subSize = ImGui::CalcTextSize(subtitle);
	ImGui::SetCursorPos(ImVec2((vecScreenSize.x - subSize.x) / 2.0f, vecScreenSize.y * 0.35f));
	ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "%s", subtitle);
	ImGui::SetWindowFontScale(1.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 4.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(30.0f, 15.0f));
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.15f, 0.15f, 0.15f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.25f, 0.25f, 0.25f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.1f, 0.1f, 0.1f, 1.0f));
	ImGui::SetWindowFontScale(1.8f);
	const char* btnText = (m_bGameWon || m_bGameOver) ? "EXIT GAME" : "START GAME";
	ImVec2 textBtnSize = ImGui::CalcTextSize(btnText);
	ImVec2 btnSize = ImVec2(textBtnSize.x + 60.0f, textBtnSize.y + 30.0f);
	ImGui::SetCursorPos(ImVec2((vecScreenSize.x - btnSize.x) / 2.0f, vecScreenSize.y * 0.55f));
	if (ImGui::Button(btnText, btnSize))
	{
		if (m_bGameWon || m_bGameOver) {
			m_bExitGame = true;
		} else {
			m_bShowIntro = true;
			m_flIntroStartTime = -1.0f;
		}
	}
	ImGui::SetWindowFontScale(1.0f);
	ImGui::PopStyleColor(3);
	ImGui::PopStyleVar(2);
	const char* footer = "Watch out for the bot...";
	ImVec2 footSize = ImGui::CalcTextSize(footer);
	ImGui::SetCursorPos(ImVec2((vecScreenSize.x - footSize.x) / 2.0f, vecScreenSize.y * 0.9f));
	ImGui::TextColored(ImVec4(0.3f, 0.3f, 0.3f, 1.0f), "%s", footer);
}

void CMenu::IntroTitle()
{
	ImVec2 vecScreenSize(GetScreenWidth(), GetScreenHeight());
	
	ImGui::SetWindowFontScale(1.5f);
	ImGui::PushTextWrapPos(vecScreenSize.x * 0.8f);
	const char* loreText = 
		u8"СЕКРЕТНЫЙ АРХИВ КГБ. Дело №48-B. Статус: СОВЕРШЕННО СЕКРЕТНО.\n\n"
		u8"В 1989 году во время расследования странной сейсмической активности под зданием "
		u8"Института Радиофизики, оперативник КГБ под кодовым именем «Сокол» обнаружил "
		u8"пространственную аномалию — бесконечный бетонный лабиринт, не подчиняющийся законам физики.\n\n"
		u8"Найденная на месте видеопленка обрывается вскоре после того, как агент сообщает о том, "
		u8"что в этих стенах есть 'нечто'. Оперативник объявлен пропавшим без вести. "
		u8"Вам предстоит просмотреть эту восстановленную запись. "
		u8"Найдите выход, но помните... вы здесь не одни.";
		
	if (m_flIntroStartTime < 0.0f) {
		m_flIntroStartTime = GetTime();
	}

	ImGui::SetCursorPos(ImVec2(vecScreenSize.x * 0.1f, vecScreenSize.y * 0.3f));
	
	// Create struct and print
	TextPrintData printData(loreText, m_flIntroStartTime, 50.0f, true);
	if (m_pFont) ImGui::PushFont(m_pFont);
	g_pRenderUtils.get()->PrintText(printData);
	if (m_pFont) ImGui::PopFont();
	
	ImGui::PopTextWrapPos();

	bool bFinishedTyping = printData.m_bIsActive;

	ImGui::SetWindowFontScale(1.0f);
	
	if (bFinishedTyping)
	{
		ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 4.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(30.0f, 15.0f));
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.4f, 0.1f, 0.1f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.6f, 0.15f, 0.15f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.3f, 0.05f, 0.05f, 1.0f));
		
		ImGui::SetWindowFontScale(1.5f);
		const char* btnText = "START RECORDING";
		ImVec2 textBtnSize = ImGui::CalcTextSize(btnText);
		ImVec2 btnSize = ImVec2(textBtnSize.x + 60.0f, textBtnSize.y + 30.0f);
		ImGui::SetCursorPos(ImVec2((vecScreenSize.x - btnSize.x) / 2.0f, vecScreenSize.y * 0.7f));
		
		if (ImGui::Button(btnText, btnSize))
		{
			m_bShowIntro = false;
			m_bShowMenu = false;
			m_bStartGame = true;
			m_bGameWon = false;
		}
		
		ImGui::SetWindowFontScale(1.0f);
		ImGui::PopStyleColor(3);
		ImGui::PopStyleVar(2);
	}
}