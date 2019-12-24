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

void JobManager::TestJobManager()
{
	std::lock_guard<std::mutex> lock(_jobQueueMutex);
	std::cout << "Starting test scenario for job manager." << std::endl;

	_jobQueue.emplace_back(Job(JobManager::threadTest10Secs));

	for (size_t i = 0; i < 10; i++)
	{
		_jobQueue.emplace_back(Job(JobManager::threadTestFunc));
	}

	std::cout << "Job queue count before processing = " << _jobQueue.size() << std::endl;

	ProcessJobs();
}

void JobManager::threadTestFunc()
{
	srand(static_cast<unsigned int>(time(0)));
	int randomTime = (rand()%10) + 1;
	std::cout << "Threads random time = " << randomTime << std::endl;
	std::this_thread::sleep_for(std::chrono::seconds(randomTime));
}

void JobManager::threadTest10Secs()
{
	std::cout << "Threads random time = 10" << std::endl;
	std::this_thread::sleep_for(std::chrono::seconds(10));
}
