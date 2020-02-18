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
		enum class JobPriority
		{
			UNORDERED,
			ORDERED
		};

		Job() = delete;
		Job(std::function<void()> func, JobPriority priority = JobPriority::UNORDERED, int priorityOrder = -1) : _dataProcessingFunction(func),  _priority(priority), _prioritryOrder(priorityOrder){};

		const std::function<void()> _dataProcessingFunction;
		const JobPriority _priority;
		const int _prioritryOrder;
	};

	void AddJobToQueue(Job job);
	void ProcessJobs();

private:
	void OrderedJobComplete();
	void ProgressBatch();

	std::mutex _jobQueueMutex;
	std::list<Job> _jobQueue;

	std::atomic<int> _currentBatch = 0;
	std::atomic<int> _currentBatchProgress = 0;

	std::vector<std::unique_ptr<PoolableThread>> _threads;
};

