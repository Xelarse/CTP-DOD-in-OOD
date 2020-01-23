#include "..\includes\PoolableThread.h"
#include <chrono>

PoolableThread::PoolableThread()
{
	//TODO theres gotta be a cleaner way of doing this.
	_thread = std::thread([&]()
		{
			ThreadLoop();
		});
}

PoolableThread::~PoolableThread()
{
	//TODO need to confirm if the thread will still pick this up or if the detached thread gets left hanging
	_threadAlive = false;
}

bool PoolableThread::IsThreadIdle()
{
	return _threadIdle;
}

void PoolableThread::RunTaskOnThread(std::function<void()> task)
{
	_task = task;
}

void PoolableThread::ThreadLoop()
{
	while (_threadAlive)
	{
		if (_task != nullptr)
		{
			_threadIdle = false;
			_task();
			_testCounter = 0;
			_task = nullptr;
			_threadIdle = true;
		}
		else
		{
			//TODO see whats the sweetspot for sleeping a thread
			_testCounter++;
			std::this_thread::sleep_for(std::chrono::milliseconds(200));
		}
	}
}
