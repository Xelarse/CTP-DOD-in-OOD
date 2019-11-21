#pragma once
#include <functional>
#include <queue>

class JobManager
{
public:
	class Job
	{
		void* _dataPtr;
		size_t _dataSetLength;
		std::function<void(void*, size_t)> dataProcessingFunction;
	};

	void AddJobToQueue(Job job);
	void ProcessJobsOnThreads();

private:

	std::queue<Job> _jobQueue;

	//thread pool here
};

