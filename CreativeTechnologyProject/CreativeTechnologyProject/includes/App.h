#pragma once
#include "Window.h"
#include "DeltaTimer.h"
#include "ImguiManager.h"

class App
{
public:
	App();
	int Go();
private:
	void DoFrame();
private:
	Window _wnd;
	DeltaTimer timer;
	ImguiManager _imgui;

	bool _showDemoWindow = true;
};