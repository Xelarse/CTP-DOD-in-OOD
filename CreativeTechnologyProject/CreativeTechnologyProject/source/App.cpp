#include "..\includes\App.h"
#include <memory>
#include <functional>
#include "imgui/imgui.h"
#include "Timer.h"


App::App() : _wnd(800, 600, "Creative Tech: DoD in OOP")
{
	_wnd.Gfx().SetProjection(DirectX::XMMatrixPerspectiveLH(1.0f, 3.0f / 4.0f, 0.5f, 40.0f));

	_allSysTest = std::make_unique<AllSystemsTest>(_maxNpcInstances);
	_noSysTest = std::make_unique<NoSystemsTest>(_maxNpcInstances);
	_justJobTest = std::make_unique<JustJobTest>(_maxNpcInstances);
	_justMemoryTest = std::make_unique<JustMemoryTest>(_maxNpcInstances);
}

int App::Go()
{
	while (true)
	{
		//processes messages pending but doesnt block application flow. If ecode has a value after getting fed optional from process messages its time to quit
		if (const auto ecode = Window::ProcessMessages())
		{
			//if the returned optional actually has a value its time to leave
			return *ecode;
		}
		//Else just carry on with normal execution
		DoFrame();
	}
}

void App::DoFrame()
{
	const float dt = timer.Mark();

	//Start DirectX frame 
	_wnd.Gfx().BeginFrame(0.08f, 0.24f, 0.32f);

	if(_testActive)
	{
		std::function<void(long long)> callback;

		switch (_activeTest)
		{
			case App::TestTypes::NO_SYSTEMS:
				callback = [&](long long dura){_noSysTest->AddTimeToQueue(dura);};
				break;
			case App::TestTypes::ALL_SYSTEMS:
				callback = [&](long long dura){_allSysTest->AddTimeToQueue(dura);};
				break;
			case App::TestTypes::JUST_JOB:
				callback = [&](long long dura){_justJobTest->AddTimeToQueue(dura);};
				break;
			case App::TestTypes::JUST_MEMORY:
				callback = [&](long long dura){_justMemoryTest->AddTimeToQueue(dura);};
				break;
			default:
				break;
		}

		Timer timer = Timer(callback);
		PreUpdate(dt);
		Update(dt);
		PostUpdate(dt);
	}

	RenderImguiWindow();

	//if (_showDemoWindow)
	//{
	//	ImGui::ShowDemoWindow(&_showDemoWindow);
	//}

	//present 
	_wnd.Gfx().EndFrame();
}

void App::PreUpdate(float dt)
{
	switch (_activeTest)
	{
		case App::TestTypes::NO_SYSTEMS:
			_noSysTest->PreUpdate(dt);
			break;
		case App::TestTypes::ALL_SYSTEMS:
			_allSysTest->PreUpdate(dt);
			break;
		case App::TestTypes::JUST_JOB:
			_justJobTest->PreUpdate(dt);
			break;
		case App::TestTypes::JUST_MEMORY:
			_justMemoryTest->PreUpdate(dt);
			break;
		default:
			break;
	}
}

void App::Update(float dt)
{
	switch (_activeTest)
	{
		case App::TestTypes::NO_SYSTEMS:
			_noSysTest->Update(dt);
			break;
		case App::TestTypes::ALL_SYSTEMS:
			_allSysTest->Update(dt);
			break;
		case App::TestTypes::JUST_JOB:
			_justJobTest->Update(dt);
			break;
		case App::TestTypes::JUST_MEMORY:
			_justMemoryTest->Update(dt);
			break;
		default:
			break;
	}
}

void App::PostUpdate(float dt)
{
	switch (_activeTest)
	{
		case App::TestTypes::NO_SYSTEMS:
			_noSysTest->PostUpdate(dt);
			break;
		case App::TestTypes::ALL_SYSTEMS:
			_allSysTest->PostUpdate(dt);
			break;
		case App::TestTypes::JUST_JOB:
			_justJobTest->PostUpdate(dt);
			break;
		case App::TestTypes::JUST_MEMORY:
			_justMemoryTest->PostUpdate(dt);
			break;
		default:
			break;
	}
}

void App::RenderImguiWindow()
{
	ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
	ImGui::SetNextWindowSize(ImVec2(800, 600), ImGuiCond_Always);

	if (ImGui::Begin("CTP DOD in OOP", NULL, _guiFlags))
	{
		ImGui::Checkbox("Test Active", &_testActive);
		ImGui::Combo("Current Test", &_imguiActiveTest, "NO_SYSTEMS\0ALL_SYSTEMS\0JUST_JOB\0JUST_MEMORY");

		switch (_activeTest)
		{
			case App::TestTypes::NO_SYSTEMS:
				_noSysTest->RenderImguiWindow();
				break;
			case App::TestTypes::ALL_SYSTEMS:
				_allSysTest->RenderImguiWindow();
				break;
			case App::TestTypes::JUST_JOB:
				_justJobTest->RenderImguiWindow();
				break;
			case App::TestTypes::JUST_MEMORY:
				_justMemoryTest->RenderImguiWindow();
				break;
			default:
				break;
		}
		
	}
	ImGui::End();

	//Update the current active test based of combo from imgui
	_activeTest = static_cast<App::TestTypes>(_imguiActiveTest);
}
