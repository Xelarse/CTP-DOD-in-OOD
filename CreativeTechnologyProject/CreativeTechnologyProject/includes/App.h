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

	ImguiManager _imgui;
	Window _wnd;
	DeltaTimer timer;

	bool _showDemoWindow = true;
};