#include "..\includes\Timer.h"

Timer::Timer(std::function<void(long long)> durationCallback) : _stopped(false), _callbackFunction(durationCallback)
{
	_startTime = std::chrono::high_resolution_clock::now();
}

Timer::~Timer()
{
	if (!_stopped)
	{
		StopTimer();
	}
}

void Timer::StopTimer()
{
	auto endTime = std::chrono::high_resolution_clock::now();

	long long start = std::chrono::time_point_cast<std::chrono::milliseconds>(_startTime).time_since_epoch().count();
	long long end = std::chrono::time_point_cast<std::chrono::milliseconds>(endTime).time_since_epoch().count();

	_stopped = true;

	_callbackFunction(end - start);
}
