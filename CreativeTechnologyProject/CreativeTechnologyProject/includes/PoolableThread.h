#pragma once
#include <thread>
#include <mutex>
#include <atomic>
#include <functional>

class PoolableThread
{

public:
	PoolableThread();
	~PoolableThread();
    PoolableThread(PoolableThread const& other) = delete;
    PoolableThread& operator=(PoolableThread const& other) = delete;


	bool IsThreadIdle();
	void RunTaskOnThread(std::function<void()> task);
	void KillThread();

	std::atomic<int> _testCounter = 0;

private:
	void ThreadLoop();

	std::thread _thread;
	std::atomic<bool> _threadIdle = true;
	std::atomic<bool> _threadAlive = true;
	std::function<void()> _task = nullptr;
};

