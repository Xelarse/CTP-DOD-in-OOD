#include "..\includes\App.h"
#include "imgui/imgui.h"

App::App() : _wnd(800, 600, "Creative Tech: DoD in OOP")
{
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
	const float c = sin(timer.Peek()) / 2.0f + 0.5f;

	//Check input to toggle imgui 
	if (_wnd._keyboard.KeyIsPressed(VK_SPACE))
	{
		_wnd.Gfx().DisableImgui();
	}
	else
	{
		_wnd.Gfx().EnableImgui();
	}

	//Start DirectX frame 
	_wnd.Gfx().BeginFrame(c, c, 1.0f);

	if (_showDemoWindow)
	{
		ImGui::ShowDemoWindow(&_showDemoWindow);
	}

	//present 
	_wnd.Gfx().EndFrame();
}