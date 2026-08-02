#pragma once
#include <memory>
#include <string>

struct ImFont;
class CPlayer;
class CMenu
{
public:
	void Init(bool& isFPSMode, CPlayer* pPlayer);
	void MainTitle();
	void IntroTitle();
	void SetFont(ImFont* pFont) { m_pFont = pFont; }
	const bool CanStartGame() { return m_bStartGame; }
    void SetGameWon(bool won) { m_bGameWon = won; m_bGameOver = false; m_bStartGame = !won; m_bShowMenu = won; }
    void SetGameOver(bool over) { m_bGameOver = over; m_bGameWon = false; m_bStartGame = !over; m_bShowMenu = over; }
	bool ShouldExitGame() const { return m_bExitGame; }
private:
	bool m_bShowMenu = true;
	bool m_bStartGame = false;
    bool m_bGameWon = false;
	bool m_bGameOver = false;
	bool m_bExitGame = false;
	bool m_bShowIntro = false;
	float m_flIntroStartTime = -1.0f;
	ImFont* m_pFont = nullptr;
};
inline std::unique_ptr<CMenu> g_pMenu = std::make_unique<CMenu>();