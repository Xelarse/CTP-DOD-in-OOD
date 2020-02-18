#include "..\includes\JobManager.h"
#include <vector>
#include <algorithm>
#include <iostream>
#include <random>
#include <Window.h>

JobManager::JobManager(int jobQueueSize)
{
	//TODO temp override for testing, will probably do a benchmark here later to determine optimal thread count
	_threads.reserve(jobQueueSize);

	for (size_t i = 0; i < jobQueueSize; ++i)
	{
		_threads.emplace_back(std::make_unique<PoolableThread>([&](){OrderedJobComplete();}));
	}

	//TODO REMOVE - Temp thing to test sort
	//AddJobToQueue(Job([](){OutputDebugString("\nJob 2\n"); return;}, Job::JobPriority::ORDERED, 2));
	//AddJobToQueue(Job([](){OutputDebugString("\nJob 1\n"); return;}, Job::JobPriority::ORDERED, 1));
	//AddJobToQueue(Job([](){OutputDebugString("\nUNORDERED\n"); return;}, Job::JobPriority::UNORDERED));
	//AddJobToQueue(Job([](){OutputDebugString("\nJob 2\n"); return;}, Job::JobPriority::ORDERED, 2));
	//AddJobToQueue(Job([](){OutputDebugString("\nJob 3\n"); return;}, Job::JobPriority::ORDERED, 3));
	//ProcessJobs();
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
	std::vector<Job> unorderedJobs;

	//Sort the jobs from unordered to the lowest priority
	_jobQueue.sort([](const Job& lhs, const Job& rhs) { return lhs._prioritryOrder < rhs._prioritryOrder; });

	//Pop off the unordered jobs and chuck them in a vector, once they're in the vector put the rest in a fifo queue
	std::list<Job>::iterator jobIter = _jobQueue.begin();
	while (jobIter != _jobQueue.end())
	{
		if (jobIter->_priority == Job::JobPriority::UNORDERED)
		{
			unorderedJobs.emplace_back(jobIter->_dataProcessingFunction, jobIter->_priority);
			jobIter = _jobQueue.erase(jobIter);
		}
		else
		{
			++jobIter;
		}
	}

	//Set up the _currentBatch and its count
	ProgressBatch();
	
	//Whilst there are still jobs left to send on threads
	while (unorderedJobs.size() > 0 || _jobQueue.size() > 0)
	{
		for (auto& thread : _threads)
		{
			if (thread->IsThreadIdle())
			{
				//Firstly try and send an ordered job of the current batch
				{
					std::lock_guard<std::mutex> lock(_jobQueueMutex);
					if (_jobQueue.size() > 0 && _jobQueue.front()._prioritryOrder == _currentBatch)
					{
						thread->RunTaskOnThread(_jobQueue.front()._dataProcessingFunction, true);
						_jobQueue.pop_front();
						continue;
					}
				}

				//If theres no jobs or the ones existing arent completed yet but theres a thread free ping an unordered job on it
				if (unorderedJobs.size() > 0)
				{
					thread->RunTaskOnThread(unorderedJobs.back()._dataProcessingFunction);
					unorderedJobs.pop_back();
					continue;
				}
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

void JobManager::OrderedJobComplete()
{
	--_currentBatchProgress;
	if (_currentBatchProgress == 0)
	{
		ProgressBatch();
	}
}

void JobManager::ProgressBatch()
{
	std::lock_guard<std::mutex> lock(_jobQueueMutex);
	if (_jobQueue.size() > 0)
	{
		_currentBatch = _jobQueue.front()._prioritryOrder;
		_currentBatchProgress = std::count_if(_jobQueue.begin(), _jobQueue.end(), [&](const Job& lhs){ return lhs._prioritryOrder == _currentBatch;});
	}
}
