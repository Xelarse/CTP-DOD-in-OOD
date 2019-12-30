#pragma once
#include "Window.h"
#include "DeltaTimer.h"
#include "ImguiManager.h"
#include "AllSystemsTest.h"

class App
{
public:
	App();
	int Go();
private:
	void DoFrame();

	void PreUpdate(float dt);
	void Update(float dt);
	void PostUpdate(float dt);

private:

	ImguiManager _imgui;
	Window _wnd;
	DeltaTimer timer;

	std::unique_ptr<AllSystemsTest> _allSysTest = nullptr;

	bool _showDemoWindow = false;
};