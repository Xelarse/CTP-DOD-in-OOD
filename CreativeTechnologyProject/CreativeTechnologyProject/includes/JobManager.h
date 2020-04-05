#pragma once
#include <functional>
#include <list>
#include <mutex>
#include "PoolableThread.h"

class JobManager
{
public:
	//Used to configure a percentage of the max usable threads before the computer starts locking up
	enum class JobCpuIntensity
	{
		LOW,
		MEDIUM,
		HIGH
	};

	JobManager() = delete;
	JobManager(JobCpuIntensity intensity, bool quickSetup = true);
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
		Job(std::function<void()> func, JobPriority priority = JobPriority::UNORDERED, int priorityOrder = -1) 
			: _dataProcessingFunction(func),  _priority(priority), _prioritryOrder(priorityOrder){};

		const std::function<void()> _dataProcessingFunction;
		const JobPriority _priority;
		const int _prioritryOrder;
	};

	void AddJobToQueue(Job job);
	void ProcessJobs();

 	inline const int GetTotalThreads() { return static_cast<const int>(_threads.size()); }

private:
	void OrderedJobComplete();
	void ProgressBatch();

	int CalculateThreadCountWithBenchmarking(JobCpuIntensity intensity);
	int CalculateThreadCountWithoutBenchmarking(JobCpuIntensity intensity);

	std::mutex _jobQueueMutex;
	std::list<Job> _jobQueue;

	std::atomic<int> _currentBatch = 0;
	std::atomic<int> _currentBatchProgress = 0;

	std::vector<std::unique_ptr<PoolableThread>> _threads;

	//Once the time taken to create the threads gets worse than this it treats the previous value as threadMax
	const float _performanceThreshold = 0.075f;
	const int _threadsPerStep = 2;

};

