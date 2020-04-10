#pragma once
#include <thread>
#include <mutex>
#include <atomic>
#include <functional>
#include <future>

class PoolableThread
{
public:

	////---------- Constructors and Destructor ----------////
	PoolableThread() = delete;
	PoolableThread(const PoolableThread& otherCopy) = delete;
	PoolableThread(const PoolableThread&& otherMove) = delete;
	PoolableThread& operator= (const PoolableThread& otherCopy) = delete;
	PoolableThread& operator= (const PoolableThread&& otherMove) = delete;

	PoolableThread(std::function<void()> orderedJobCallback) : _orderedJobComplete(orderedJobCallback)
	{
		std::promise<void> exitPromise;
		_exitFuture = exitPromise.get_future();
		_thread = std::thread(&PoolableThread::ThreadLoop, this, std::move(exitPromise));
		_thread.detach();
	}

	~PoolableThread()
	{
		WaitForThreadToExit();
	}

	////---------- Thread Interface ----------////
	/*
		Returns True if the thread is idle and ready for more work
	*/
	bool IsThreadIdle() { return _threadIdle; }

	/*
		Assigns the given function ptr to _task
		The function the thread runs on loop checks this ptr and if its not null then runs it
	*/
	void RunTaskOnThread(std::function<void()> task, bool orderedTask = false)
	{
		_threadIdle = false;
		_processingOrderedJob = orderedTask;
		_task = task;
	}
		
private:

	////---------- Thread Backend ----------////
	/*
		This is the function that is run by a thread.
		While loop to ensure thread stays active until this class says otherwise
	*/
	void ThreadLoop(std::promise<void> exitPromise)
	{
		while (_threadAlive)
		{
			if (_task != nullptr)
			{
				_task();
				_task = nullptr;
				_threadIdle = true;
				if (_processingOrderedJob) { _orderedJobComplete(); }
			}
			else { std::this_thread::sleep_for(std::chrono::milliseconds(1)); }
		}
		exitPromise.set_value();
	}

	/*
		Used as a Safety net to ensure the detatched thread is terminated before,
		destruction of this class instance.
		Utilises the exit future passed into the thread on construction
	*/
	void WaitForThreadToExit()
	{
		_threadAlive = false;
		_exitFuture.wait();
	}

	std::thread _thread;
	std::atomic<bool> _threadIdle = true;
	std::atomic<bool> _threadAlive = true;
	std::atomic<bool> _processingOrderedJob = false;
	const std::function<void()> _orderedJobComplete;
	std::function<void()> _task = nullptr;

	//Future used in conjunction with a promise to terminate the thread in a thread safe manner
	std::future<void> _exitFuture;
};

