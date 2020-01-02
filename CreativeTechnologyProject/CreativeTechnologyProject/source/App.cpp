#include "..\includes\App.h"
#include <memory>
#include <functional>
#include "imgui/imgui.h"
#include "Timer.h"


App::App() : _wnd(800, 600, "Creative Tech: DoD in OOP")
{
	_wnd.Gfx().SetProjection(DirectX::XMMatrixPerspectiveLH(1.0f, 3.0f / 4.0f, 0.5f, 40.0f));

	_allSysTest = std::make_unique<AllSystemsTest>();
	_noSysTest = std::make_unique<NoSystemsTest>();
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
			case App::NO_SYSTEMS:
				callback = [&](long long dura){_noSysTest->AddTimeToQueue(dura);};
				break;
			case App::ALL_SYSTEMS:
				callback = [&](long long dura){_allSysTest->AddTimeToQueue(dura);};
				break;
			case App::JUST_JOB:
				break;
			case App::JUST_MEMORY:
				break;
			default:
				break;
		}

		Timer timer = Timer(callback);
		PreUpdate(dt);
		Update(dt);
		PostUpdate(dt);
	}

	switch (_activeTest)
	{
		case App::NO_SYSTEMS:
			_noSysTest->RenderImguiWindow();
			break;
		case App::ALL_SYSTEMS:
			_allSysTest->RenderImguiWindow();
			break;
		case App::JUST_JOB:
			break;
		case App::JUST_MEMORY:
			break;
		default:
			break;
	}

	if (ImGui::Begin("Update Active"))
	{
		ImGui::Checkbox("Is Active", &_testActive);
	}
	ImGui::End();

	if (_showDemoWindow)
	{
		ImGui::ShowDemoWindow(&_showDemoWindow);
	}

	//present 
	_wnd.Gfx().EndFrame();
}

void App::PreUpdate(float dt)
{
	switch (_activeTest)
	{
		case App::NO_SYSTEMS:
			_noSysTest->PreUpdate(dt);
			break;
		case App::ALL_SYSTEMS:
			_allSysTest->PreUpdate(dt);
			break;
		case App::JUST_JOB:
			break;
		case App::JUST_MEMORY:
			break;
		default:
			break;
	}
}

void App::Update(float dt)
{
	switch (_activeTest)
	{
		case App::NO_SYSTEMS:
			_noSysTest->Update(dt);
			break;
		case App::ALL_SYSTEMS:
			_allSysTest->Update(dt);
			break;
		case App::JUST_JOB:
			break;
		case App::JUST_MEMORY:
			break;
		default:
			break;
	}
}

void App::PostUpdate(float dt)
{
	switch (_activeTest)
	{
		case App::NO_SYSTEMS:
			_noSysTest->PostUpdate(dt);
			break;
		case App::ALL_SYSTEMS:
			_allSysTest->PostUpdate(dt);
			break;
		case App::JUST_JOB:
			break;
		case App::JUST_MEMORY:
			break;
		default:
			break;
	}
}
