#include "..\includes\JobManager.h"
#include <vector>
#include <algorithm>
#include <iostream>
#include <random>

JobManager::JobManager(int jobQueueSize)
{
	//TODO use the size if we switch to a contiguious container later for better performance
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
	//TODO make this more efficient by using a thread pool instead of creating threads each and every update

	std::vector<std::thread> threads;
	threads.reserve(_jobQueue.size());

	std::list<Job>::iterator jobIter = _jobQueue.begin();

	//Iterate through all the jobs in the queue and add their functions to a thread and remove them from the queue
	while (jobIter != _jobQueue.end())
	{
		threads.emplace_back(std::thread(jobIter->dataProcessingFunction));
		jobIter = _jobQueue.erase(jobIter);
	}

	//Wait for all the threads to join before continued execution
	std::cout << "Waiting for all workers to finish" << std::endl;
	std::for_each(threads.begin(), threads.end(), std::mem_fn(&std::thread::join));
	std::cout << "All workers finished and joined" << std::endl;
	std::cout << "Job queue size after processing threads = " << _jobQueue.size() << std::endl;
}
