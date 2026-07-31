#include "raylib.h"
#include "raymath.h"
#include "rlImGui.h"
#include "imgui.h"
#include "rlgl.h"
#include <vector>
#include "features/physics/CPhysics.h"
#include "features/map/CMap.h"
#include "features/entity/player/CPlayer.h"
#include "features/movement/CMovement.h"
#include "features/vhs/CVHSEffect.h"
#include "features/soundsystem/CSoundSystem.h"
#include "features/entitylist/CEntityList.h"
#include "features/entity/bot/CBot.h"

int main(void) 
{
	const int screenWidth = 1024;
	const int screenHeight = 768;

	SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT);
	InitWindow(screenWidth, screenHeight, "BACKROOMS: Soviet main entrance");

	// Инициализация звуковой системы
	g_pSoundSystem->Init();

	rlImGuiSetup(true);

	g_pVHSEffect->Init(screenWidth, screenHeight);

	// ОТКЛЮЧАЕМ ВМЕШАТЕЛЬСТВО IMGUI В КУРСОР!
	ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;

	g_pMap->Init();
	CPlayer* pPlayer = new CPlayer(g_pMap->GetSpawnPosition(), { 0, PI / 2.0f, 0 });
	CBot* pBot = new CBot(g_pMap->GetMapCenter());

	g_pEntityList.get()->AddEntity(pPlayer);
	g_pEntityList.get()->AddEntity(pBot);

	bool isFPSMode = true;
	DisableCursor(); // Блокирует курсор в центре экрана и прячет его
	while (!WindowShouldClose())
	{
		// Обновление аудио потоков
		g_pSoundSystem->Update();

		if (!g_pSoundSystem->IsActionSoundPlaying("white_noise"))
			g_pSoundSystem->PlayActionSound("white_noise");
		if (!g_pSoundSystem->IsActionSoundPlaying("cosmic_noise"))
			g_pSoundSystem->PlayActionSound("cosmic_noise");


		// Переключение режима (Игра / Интерфейс)
		if (IsKeyPressed(KEY_TAB)) {
			isFPSMode = !isFPSMode;
			if (isFPSMode) {
				DisableCursor(); // Снова прячем и фиксируем
			}
			else {
				EnableCursor();  // Показываем для кликов по ImGui
			}
		}

		if (IsKeyPressed(KEY_T)) {
			g_pMap->m_bSwapTextures = !g_pMap->m_bSwapTextures;
		}

		if (isFPSMode)
		{
			g_pMovement.get()->OnRun(pPlayer, isFPSMode);
			g_pPhysics.get()->OnRun();
			pPlayer->UpdateCamera();
			pBot->Update(g_pMovement.get()->GetCurrentFrameTime(), pPlayer);
		}
		// Обновление камеры к позиции игрока


		if (IsWindowResized())
			g_pVHSEffect->Resize(GetScreenWidth(), GetScreenHeight());

		// --- ОТРИСОВКА ---
		g_pVHSEffect->BeginRender();

		BeginMode3D(pPlayer->GetCamera());

		pBot->Draw();
		pPlayer->DoSound();
		g_pMap->DrawMap();
		EndMode3D();

		g_pVHSEffect->EndRender();

		BeginDrawing();
		ClearBackground(BLACK);
		g_pVHSEffect->RenderToScreen(pPlayer->m_bFlashlightOn);
		g_pVHSEffect->RenderFoundFootageHUD(isFPSMode, pPlayer->m_bFlashlightOn);

		// --- IMGUI ИНТЕРФЕЙС ---
		rlImGuiBegin();
		{
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
		}
		rlImGuiEnd();

		EndDrawing();
	}

	g_pSoundSystem->Shutdown();
	g_pVHSEffect->Shutdown();
	rlImGuiShutdown();
	CloseWindow();

	return 0;
}
