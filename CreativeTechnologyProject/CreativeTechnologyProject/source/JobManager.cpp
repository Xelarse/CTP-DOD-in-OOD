#include "..\includes\JobManager.h"
#include <vector>
#include <algorithm>
#include <iostream>
#include <random>
#include <Window.h>

JobManager::JobManager(JobCpuIntensity intensity)
{
	
	const int poolSize = CalculateThreadCount(intensity);

	_threads.reserve(poolSize);

	for (size_t i = 0; i < poolSize; ++i)
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

int JobManager::CalculateThreadCount(JobCpuIntensity intensity)
{
	//const int totalConcurrentThreads = std::thread::hardware_concurrency();

	//switch (intensity)
	//{
	//	case JobManager::JobCpuIntensity::LOW:
	//		return floor(totalConcurrentThreads * 0.4f);
	//		break;
	//	case JobManager::JobCpuIntensity::MEDIUM:
	//		return floor(totalConcurrentThreads * 0.6f);
	//		break;
	//	case JobManager::JobCpuIntensity::HIGH:
	//		return floor(totalConcurrentThreads * 0.9f);
	//		break;
	//}

	//Tried this but never seems to give an actual result
	int currentTotal = 2;	//start with 2 as computers these days have at least 1 core 2 threads
	float currentTimeTaken = INFINITY;

		//Define the test function to be used on the threads
	std::function<void()> testFunc = []()
	{
		const int vectorSize = 1000000;

		std::random_device gen;
		std::mt19937 seed(gen());
		std::uniform_int_distribution<std::mt19937::result_type> dist(1, 10000);

		//make a vector and allot the space for many numbers
		std::vector<std::unique_ptr<int>> randomNumbers;
		randomNumbers.reserve(vectorSize);

		//Fill the vector with random numbers
		for (size_t i = 0; i < vectorSize; ++i)
		{
			randomNumbers.push_back(std::make_unique<int>(dist(seed)));
		}

		//Sort the vector
		std::sort(randomNumbers.begin(), randomNumbers.end());
	};

	while (true)
	{
		double testTime = 0;

		//Create a vector of threads
		std::vector<std::thread> threads;
		threads.reserve(currentTotal);

		for (size_t i = 0; i < currentTotal; ++i)
		{
			threads.emplace_back(std::thread(testFunc));
		}

		//Run a job on the threads and benchmark the time taken for the threads to join
		std::chrono::time_point<std::chrono::high_resolution_clock> timeStamp;
		std::for_each(threads.begin(), threads.end(), std::mem_fn(&std::thread::join));
		testTime = std::chrono::duration_cast<std::chrono::duration<double, std::ratio<1>>>(std::chrono::high_resolution_clock::now() - timeStamp).count();

		//Compare the time with the last current taken
		//If the time was worse than the last time taken multiplied by the threshold we found the max
		if (testTime > currentTimeTaken * _performanceThreshold)
		{
			switch (intensity)
			{
				case JobManager::JobCpuIntensity::LOW:
					return floor(currentTotal * 0.25f);
					break;
				case JobManager::JobCpuIntensity::MEDIUM:
					return floor(currentTotal * 0.5f);
					break;
				case JobManager::JobCpuIntensity::HIGH:
					return floor(currentTotal * 0.75f);
					break;
			}
		}
		//Otherwise iterate again increasing the current total and caching the time taken
		else
		{
			currentTotal += 2;
			currentTimeTaken = testTime;
		}
	}
}
