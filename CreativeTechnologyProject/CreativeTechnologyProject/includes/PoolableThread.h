#pragma once
#include <thread>
#include <mutex>
#include <atomic>
#include <functional>
#include <future>

class PoolableThread
{

public:
	PoolableThread();
	~PoolableThread();
    PoolableThread(PoolableThread const& other) = delete;
    PoolableThread& operator=(PoolableThread const& other) = delete;


	bool IsThreadIdle();
	void RunTaskOnThread(std::function<void()> task);
	void WaitForThreadToExit();

	std::atomic<int> _testCounter = 0;

private:
	void ThreadLoop(std::promise<void> exitPromise);

	std::thread _thread;
	std::atomic<bool> _threadIdle = true;
	std::atomic<bool> _threadAlive = true;
	std::function<void()> _task = nullptr;

	//Future used in conjunction with a promise to terminate the thread in a thread safe manner
	std::future<void> _exitFuture;
};

