#include "..\includes\JobManager.h"
#include <vector>
#include <algorithm>
#include <iostream>
#include <random>

JobManager::JobManager(int jobQueueSize)
{
	//TODO temp override for testing, will probably do a benchmark here later to determine optimal thread count
	_threads.reserve(jobQueueSize);

	for (size_t i = 0; i < jobQueueSize; i++)
	{
		_threads.emplace_back(std::make_unique<PoolableThread>());
	}
}

JobManager::~JobManager()
{
	_jobQueue.clear();
}

void JobManager::AddJobToQueue(Job job)
{
	std::lock_guard<std::mutex> lock(_jobQueueMutex);
	_jobQueue.push_back(job);
}

void JobManager::ProcessJobs()
{
	//First we send jobs onto the pooled threads by checking if the thread is idle and if so giving it the job
	std::list<Job>::iterator jobIter = _jobQueue.begin();

	while (jobIter != _jobQueue.end())
	{
		for (auto& thread : _threads)
		{
			if (thread->IsThreadIdle())
			{
				thread->RunTaskOnThread(jobIter->dataProcessingFunction);
				jobIter = _jobQueue.erase(jobIter);
				break;
			}
		}
	}

	//When all threads return to idle then continue processing
	bool doneProcessing = false;
	while (!doneProcessing)
	{
		doneProcessing = true;

		for (auto& thread : _threads)
		{
			if (!thread->IsThreadIdle())
			{
				doneProcessing = false;
				break;
			}
		}
	}
}
