#pragma once
#include <memory>

class CMenu
{
	void Init();
	void MainTitle();
};

std::unique_ptr<CMenu> g_pMenu = std::make_unique<CMenu>();