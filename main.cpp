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
#include "UI/menu/CMenu.h"

int main(void) 
{
	const int screenWidth = 1024;
	const int screenHeight = 768;

	SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT);
	InitWindow(screenWidth, screenHeight, "Podjezd: Found Footage");

	// Инициализация звуковой системы
	g_pSoundSystem->Init();

	rlImGuiBeginInitImGui();
	ImGuiIO& io = ImGui::GetIO();
	ImFont* pIntroFont = io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\arial.ttf", 24.0f, NULL, io.Fonts->GetGlyphRangesCyrillic());
	g_pMenu->SetFont(pIntroFont);
	rlImGuiEndInitImGui();

	g_pVHSEffect->Init(screenWidth, screenHeight);

	// ОТКЛЮЧАЕМ ВМЕШАТЕЛЬСТВО IMGUI В КУРСОР!
	ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;

	g_pMap->Init();
	CPlayer* pPlayer = new CPlayer(g_pMap->GetSpawnPosition(), { 0, PI / 2.0f, 0 });
	CBot* pBot = new CBot(g_pMap->GetMapCenter());
	pBot->Init();

	g_pEntityList.get()->AddEntity(pPlayer);
	g_pEntityList.get()->AddEntity(pBot);

	bool isFPSMode = false;
	bool wasGameStarted = false;
	while (!WindowShouldClose() && !g_pMenu->ShouldExitGame())
	{
		if (IsKeyReleased(KEY_F11)) {
			int monitor = GetCurrentMonitor();
			if (IsWindowFullscreen()) {
				ToggleFullscreen();
				SetWindowSize(screenWidth, screenHeight);
			}
			else {
				SetWindowSize(GetMonitorWidth(monitor), GetMonitorHeight(monitor));
				ToggleFullscreen();
			}
		}

		bool gameStarted = g_pMenu->CanStartGame();
		if (gameStarted && !wasGameStarted) {
			wasGameStarted = true;
			isFPSMode = true;
			DisableCursor();
		}

		if (gameStarted)
		{
			// Обновление аудио потоков
			g_pSoundSystem->Update();

			if (!g_pSoundSystem->IsActionSoundPlaying("white_noise"))
				g_pSoundSystem->PlayActionSound("white_noise");
			if (!g_pSoundSystem->IsActionSoundPlaying("cosmic_noise"))
				g_pSoundSystem->PlayActionSound("cosmic_noise");

#ifdef DEBUG
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
#endif // DEBUG

			if (isFPSMode)
			{
				g_pMovement.get()->OnRun(pPlayer, isFPSMode);
				g_pPhysics.get()->OnRun();
				pPlayer->UpdateCamera();
				pBot->Update(g_pMovement.get()->GetCurrentFrameTime(), pPlayer);
				// Check win condition
				if (g_pMap->HasReachedExit(pPlayer->GetEntityPos())) {
					g_pMenu->SetGameWon(true);
					gameStarted = false;
					isFPSMode = false;
					EnableCursor();
					
					// Full reset for next game
					pPlayer->SetEntityPos(g_pMap->GetSpawnPosition());
					pPlayer->SetVelocity({0, 0, 0});
					pPlayer->SetViewAngles({0, PI / 2.0f, 0});
					pPlayer->UpdateCamera();
					
					pBot->SetEntityPos({-15.0f, -1.35f, 27.0f});
					pBot->SetVelocity({0, 0, 0});
				}

				// Check bot collision
				Vector3 pPos = pPlayer->GetEntityPos();
				if (Vector3Distance(pPos, pBot->GetEntityPos()) < 1.0f) {
					g_pMenu->SetGameOver(true); // Die
					isFPSMode = false;
					EnableCursor();

					pPlayer->SetEntityPos(g_pMap->GetSpawnPosition());
					pPlayer->SetVelocity({0, 0, 0});
					pPlayer->SetViewAngles({0, PI / 2.0f, 0});
					pPlayer->UpdateCamera();
					
					pBot->SetEntityPos({-15.0f, -1.35f, 27.0f});
					pBot->SetVelocity({0, 0, 0});
				}
			}

			// --- ОТРИСОВКА ---
			g_pVHSEffect->BeginRender();

			BeginMode3D(pPlayer->GetCamera());

			pBot->Draw(pPlayer->GetCamera());
			pPlayer->DoSound();
			g_pMap->DrawMap();
			EndMode3D();

			g_pVHSEffect->EndRender();
		}

		if (IsWindowResized())
			g_pVHSEffect->Resize(GetScreenWidth(), GetScreenHeight());

		BeginDrawing();
		ClearBackground(BLACK);
		
		if (g_pMenu->CanStartGame()) {
			g_pVHSEffect->RenderToScreen(pPlayer->m_bFlashlightOn);
			g_pVHSEffect->RenderFoundFootageHUD(isFPSMode, pPlayer->m_bFlashlightOn);
		}

		// --- IMGUI ИНТЕРФЕЙС ---
		rlImGuiBegin();
		{
			g_pMenu->Init(isFPSMode, pPlayer);
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
