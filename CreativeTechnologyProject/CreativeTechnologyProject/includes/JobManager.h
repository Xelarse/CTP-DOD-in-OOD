#pragma once
#include <functional>
#include <list>
#include <mutex>
#include "PoolableThread.h"

//When switching jobs to a pool of threads look at this https://www.bfilipek.com/2019/12/threading-loopers-cpp17.html
//For now this will help https://thispointer.com/c11-multithreading-part-2-joining-and-detaching-threads/

class JobManager
{
public:

	JobManager() = delete;
	JobManager(int jobQueueSize);
	~JobManager();


	class Job
	{
	public:
		Job() = delete;
		Job(std::function<void()> func) : dataProcessingFunction(func) {};

		std::function<void()> dataProcessingFunction;
	};

	void AddJobToQueue(Job job);
	void ProcessJobs();

private:
	std::mutex _jobQueueMutex;

	//TODO List for ease right now, might switch to queue later and handle deleting allocations in the middle of it with something fancy
	std::list<Job> _jobQueue;

	std::vector<std::unique_ptr<PoolableThread>> _threads;
};

