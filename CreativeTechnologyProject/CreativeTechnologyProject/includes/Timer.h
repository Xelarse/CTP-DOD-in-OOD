#pragma once
#include <chrono>
#include <functional>

//A timer class that on creation makes a timestamp of the current time using chrono 
//this timer will then either call the function passed in the constuction when the StopTimer fucntion is called with the duration
//Its also called if the timer hasnt been stopped and falls out of scope, allowing you to use nameless scopes for timers

class Timer
{
public:
	Timer() = delete;

	//On StopTimer or when instance falls out of scope, returns the time in milliseconds elapsed from creation through the callback function given
	Timer(std::function<void(long long)> durationCallback);
	~Timer();

	void StopTimer();

private:
	bool _stopped;
	std::chrono::time_point<std::chrono::steady_clock> _startTime;
	std::function<void(long long)> _callbackFunction;
};

