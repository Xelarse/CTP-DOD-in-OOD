#include "..\includes\PoolableThread.h"
#include <chrono>

PoolableThread::PoolableThread()
{
	std::promise<void> exitPromise;
	_exitFuture = exitPromise.get_future();

	_thread = std::thread(&PoolableThread::ThreadLoop, this, std::move(exitPromise));
	_thread.detach();
}

PoolableThread::~PoolableThread()
{
	WaitForThreadToExit();
}

bool PoolableThread::IsThreadIdle()
{
	return _threadIdle;
}

void PoolableThread::RunTaskOnThread(std::function<void()> task)
{
	_task = task;
	_threadIdle = false;
}

void PoolableThread::WaitForThreadToExit()
{
	_threadAlive = false;
	_exitFuture.wait();
}

void PoolableThread::ThreadLoop(std::promise<void> exitPromise)
{
	while (_threadAlive)
	{
		
		if (_task != nullptr)
		{
			_task();
			_testCounter = 0;
			_task = nullptr;
			_threadIdle = true;
		}
		else
		{
			_testCounter++;
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}
	}
	exitPromise.set_value();
}
