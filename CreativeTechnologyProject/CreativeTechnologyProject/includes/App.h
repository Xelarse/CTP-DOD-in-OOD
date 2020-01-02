#pragma once
#include "Window.h"
#include "DeltaTimer.h"
#include "ImguiManager.h"
#include "AllSystemsTest.h"
#include "NoSystemsTest.h"

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

	enum TestTypes
	{
		NO_SYSTEMS,
		ALL_SYSTEMS,
		JUST_JOB,
		JUST_MEMORY
	};

	ImguiManager _imgui;
	Window _wnd;
	DeltaTimer timer;

	std::unique_ptr<AllSystemsTest> _allSysTest = nullptr;
	std::unique_ptr<NoSystemsTest> _noSysTest = nullptr;

	bool _testActive = true;
	bool _showDemoWindow = true;

	TestTypes _activeTest = TestTypes::ALL_SYSTEMS;
};