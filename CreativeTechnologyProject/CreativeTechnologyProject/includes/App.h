#pragma once
#include "Window.h"
#include "DeltaTimer.h"

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
};

