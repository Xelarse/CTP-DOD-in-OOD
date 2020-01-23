#pragma once
#include "imgui/imgui.h"
#include "Window.h"
#include "DeltaTimer.h"
#include "ImguiManager.h"
#include "AllSystemsTest.h"
#include "NoSystemsTest.h"
#include "JustJobTest.h"
#include "JustMemoryTest.h"
#include "PoolableThread.h"

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

	void RenderImguiWindow();
	void ClearExistingResults();

	enum class TestTypes
	{
		NO_SYSTEMS = 0,
		ALL_SYSTEMS = 1,
		JUST_JOB = 2,
		JUST_MEMORY = 3
	};

	ImguiManager _imgui;
	Window _wnd;
	DeltaTimer timer;

	std::unique_ptr<AllSystemsTest> _allSysTest = nullptr;
	std::unique_ptr<NoSystemsTest> _noSysTest = nullptr;
	std::unique_ptr<JustJobTest> _justJobTest = nullptr;
	std::unique_ptr<JustMemoryTest> _justMemoryTest = nullptr;

	//TODO PoolableThread test stuff, delete later
	//std::unique_ptr<PoolableThread> _testThread = nullptr;
	//void RenderThreadTestGui();

	//ImGui related stuff
	bool _testActive = true;
	bool _showDemoWindow = true;
	int _imguiActiveTest = 0;
	ImGuiWindowFlags _guiFlags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse;

	int _maxNpcInstances = 200000;

	TestTypes _activeTest = TestTypes::NO_SYSTEMS;
};