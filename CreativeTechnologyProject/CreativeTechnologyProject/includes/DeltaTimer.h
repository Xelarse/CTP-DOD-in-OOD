#pragma once
#include <chrono>

class DeltaTimer
{
public:
	DeltaTimer();
	float Mark();
	float Peek() const;

private:
	std::chrono::steady_clock::time_point _last;
};