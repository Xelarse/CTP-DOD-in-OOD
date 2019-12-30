#include "..\includes\App.h"
#include <memory>
#include "imgui/imgui.h"

App::App() : _wnd(800, 600, "Creative Tech: DoD in OOP")
{
	_wnd.Gfx().SetProjection(DirectX::XMMatrixPerspectiveLH(1.0f, 3.0f / 4.0f, 0.5f, 40.0f));
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

	PreUpdate(dt);
	Update(dt);
	PostUpdate(dt);

	//present 
	_wnd.Gfx().EndFrame();
}

void App::PreUpdate(float dt)
{
}

void App::Update(float dt)
{
	if (_showDemoWindow)
	{
		ImGui::ShowDemoWindow(&_showDemoWindow);
	}
}

void App::PostUpdate(float dt)
{
}
