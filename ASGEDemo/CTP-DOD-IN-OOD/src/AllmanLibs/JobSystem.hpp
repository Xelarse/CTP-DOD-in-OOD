#pragma once
#include <functional>
#include <list>
#include <mutex>
#include <random>
#include <thread>
#include <atomic>
#include <future>
#include <algorithm>

/*
	Class that is used to interface with a detached thread.
	Used by the JobSystem in its thread pool.
*/
class PoolableThread
{
public:

	////---------- Constructors and Destructor ----------////
	PoolableThread() = delete;
	PoolableThread(const PoolableThread& otherCopy) = delete;
	PoolableThread(const PoolableThread&& otherMove) = delete;
	PoolableThread& operator= (const PoolableThread& otherCopy) = delete;
	PoolableThread& operator= (const PoolableThread&& otherMove) = delete;

	PoolableThread(std::function<void()> orderedJobCallback, std::function<void()> jobCompleteCallback) :
	_threadIdle(true),
	_threadAlive(true),
	_processingOrderedJob(false),
	_orderedJobComplete(orderedJobCallback),
	_jobComplete(jobCompleteCallback)
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
		{
			std::lock_guard<std::mutex> lock(_taskMutex);
			_task = task;
		}
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
				{
					std::lock_guard<std::mutex> lock(_taskMutex);
					_task();
					_task = nullptr;
				}
				_threadIdle = true;
				if (_processingOrderedJob)
				{
					_orderedJobComplete();
				}
				else
				{
					_jobComplete();
				}
			}
			else { std::this_thread::sleep_for(std::chrono::milliseconds(1)); }
		}
		exitPromise.set_value();
	}

	/*
		Used as a Safety net to ensure the detached thread is terminated before,
		destruction of this class instance.
		Utilises the exit future passed into the thread on construction
	*/
	void WaitForThreadToExit()
	{
		_threadAlive = false;
		_exitFuture.wait();
	}

	std::thread _thread;
	std::atomic<bool> _threadIdle;
	std::atomic<bool> _threadAlive;
	std::atomic<bool> _processingOrderedJob;
	const std::function<void()> _orderedJobComplete;
	const std::function<void()> _jobComplete;

	std::mutex _taskMutex;
	std::function<void()> _task = nullptr;

	//Future used in conjunction with a promise to terminate the thread in a thread safe manner
	std::future<void> _exitFuture;
};

/*
	This class manages a thread pool which allows the user to run
	threaded tasks on it.
	Nested class job is used to define what tasks can be run on it.
	tasks to run on the pool must be JOBS.
*/
class JobSystem
{
public:
	enum class JobCpuIntensity
	{
		LOW,
		MEDIUM,
		HIGH
	};

	/*
		Small class used to determine how tasks are run in the JobSystem.
		Has a function ptr, whether the job is ordered or not and what priority if ordered.
	*/
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
			: _dataProcessingFunction(func), _priority(priority), _priorityOrder(priorityOrder)
		{
		};

		const std::function<void()> _dataProcessingFunction;
		const JobPriority _priority;
		const int _priorityOrder;
	};

	////---------- Constructors and Destructors ----------////

	JobSystem() = delete;
	JobSystem(const JobSystem& otherCopy) = delete;
	JobSystem(const JobSystem&& otherMove) = delete;
	JobSystem& operator= (const JobSystem& otherCopy) = delete;
	JobSystem& operator= (const JobSystem&& otherMove) = delete;

	JobSystem(JobCpuIntensity intensity, bool quickSetup = true) :
	_currentBatch(0),
	_currentBatchProgress(0)
	{
		//Use fast init using hardware_concurrency or full init using benchmark method
		const int poolSize = !quickSetup ? CalculateThreadCountWithBenchmarking(intensity) : CalculateThreadCountWithoutBenchmarking(intensity);
		_threads.reserve(static_cast<unsigned long long int>(poolSize));

		for (int i = 0; i < poolSize; ++i)
		{
			_threads.emplace_back(
					std::make_unique<PoolableThread>(
							[&](){ OrderedJobComplete(); },
							[&](){ JobComplete();}
						)
					);
		}
	}

	~JobSystem()
	{
		_jobQueue.clear();
	}

	////---------- Job Assignment and Processing Functions ----------////

	/*
		Adds a Job to the queue.
		Uses a mutex to ensure thread safety when accessing queue.
	*/
	void AddJobToQueue(Job job)
	{
		std::lock_guard<std::mutex> lock(_jobQueueMutex);
		_jobQueue.push_back(job);
	}

	void AddCachedJob(Job job)
	{
		std::lock_guard<std::mutex> lock(_cachedJobQueueMutex);
		_cachedJobs.push_back(job);
	}

	/*
		Processes the jobs on the queue.
		Call this once all jobs in an update have been assigned to the Job System.
		Priorities processing Ordered jobs over Unordered jobs.
		For best performance have as many Unordered jobs as possible
	*/
	void ProcessJobs()
	{
		std::vector<Job> unorderedJobs;
		//Add the cached jobs to the job queue
		for(auto &cached : _cachedJobs)
		{
			_jobQueue.push_back(cached);
		}

		//Sort the jobs from unordered to the lowest priority
		_jobQueue.sort(
				[](const Job &lhs, const Job &rhs)
				{ return lhs._priorityOrder < rhs._priorityOrder; }
		);

		_totalJobsInProgress = static_cast<int>(_jobQueue.size());
		_maxBatch = _jobQueue.back()._priorityOrder;

		//Pop off the unordered jobs and chuck them in a vector, once they're in the vector put the rest in a fifo queue
		std::list<Job>::iterator jobIter = _jobQueue.begin();
		while(jobIter != _jobQueue.end())
		{
			if(jobIter->_priority == Job::JobPriority::UNORDERED)
			{
				unorderedJobs.emplace_back(jobIter->_dataProcessingFunction, jobIter->_priority);
				jobIter = _jobQueue.erase(jobIter);
			}
			else
			{ ++jobIter; }
		}

		//Set up the _currentBatch and its count
		ProgressBatch();

		//Whilst there are still jobs left to send on threads
		while(unorderedJobs.size() > 0 || _jobQueue.size() > 0)
		{
			for(auto &thread : _threads)
			{
				if(thread->IsThreadIdle())
				{
					//Firstly try and send an ordered job of the current batch
					//Nameless scope used for the lock guard
					{
						std::lock_guard<std::mutex> lock(_jobQueueMutex);
						if(_jobQueue.size() > 0 && _jobQueue.front()._priorityOrder == _currentBatch)
						{
							thread->RunTaskOnThread(_jobQueue.front()._dataProcessingFunction, true);
							_jobQueue.pop_front();
							continue;
						}
					}

					//If there's no jobs or the ones existing aren't completed yet but there's a thread free ping an unordered job on it
					if(unorderedJobs.size() > 0)
					{
						thread->RunTaskOnThread(unorderedJobs.back()._dataProcessingFunction);
						unorderedJobs.pop_back();
						continue;
					}
				}
			}
		}

		//When all threads are finished processing
		bool doneProcessing = false;
		while(!doneProcessing && _totalJobsInProgress >= 0)
		{
			doneProcessing = true;
			for(auto& thread : _threads)
			{
				if(!thread->IsThreadIdle()) { doneProcessing = false; }
			}
		}
	}


	////---------- Public Utility Functions ----------////
	/*
		Returns the total threads in the pool.
		Can be useful if you have more threads than tasks,
		and you can split a task over multiple threads.
	*/
	int GetTotalThreads() { return static_cast<int>(_threads.size()); }

private:

	////---------- Job Priority Functions ----------////
	/*
		Callback for Pooled thread if its processing an ordered job.
		Decrements a counter and if all the jobs in a current batch are done,
		it progresses the batch counter and moves onto the next batch.
	*/
	void OrderedJobComplete()
	{
		--_currentBatchProgress;
		if (_currentBatchProgress <= 0) { ProgressBatch(); }
		JobComplete();
	}

	void JobComplete()
	{
		--_totalJobsInProgress;
	}

	/*
		Called when a batch of prioritized jobs are completed.
		Increments to the next batch and sets the counter to how many jobs there are for that batch.
		Counter is decremented by OrderedJobComplete().
	*/
	void ProgressBatch()
	{
		std::lock_guard<std::mutex> lock(_jobQueueMutex);
		if (_jobQueue.size() > 0)
		{
			_currentBatch = _jobQueue.front()._priorityOrder;
			_currentBatchProgress = static_cast<int>(
					std::count_if(
							_jobQueue.begin(),
							_jobQueue.end(),
							[&](const Job& lhs) { return lhs._priorityOrder == _currentBatch; }
							)
						);
		}
	}

	////---------- Thread Benchmarking Functions ----------////
	/*
		Takes a given max thread count and returns a lower one based on the given intensity
	*/
	int CalculateThreadsForGivenMax(int threadMax, JobCpuIntensity desiredIntensity)
	{
		float modifier = 0.0f;
		switch (desiredIntensity)
		{
			case JobSystem::JobCpuIntensity::LOW:
				modifier = 0.4f;
				break;
			case JobCpuIntensity::MEDIUM:
				modifier = 0.6f;
				break;
			case JobSystem::JobCpuIntensity::HIGH:
				modifier = 0.9f;
				break;
		}
		//This is to ensure theres at least 2 threads. Safety check
		//Every computer this day in age should be able to handle 2 threads
		int count = static_cast<int>(floor(static_cast<float>(threadMax) * modifier));
		return count < 2 ? 2 : count;
	}

	/*
		The function run on threads whilst testing the Hosts PC max potential threads.
		This function is ran simultaneously on many threads in this test.
	*/
	static void ThreadBenchmarkTestFunction()
	{
		const int vectorSize = 800000;

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
	}

	/*
		Calculates the max amount of concurrent threads to be used on the Host machine.
		This is done through running a test with enough data to force context switching,
		and seeing when performance degrades.
		Once this point is hit that max count is then used to calculate a threadcount.
	*/
	int CalculateThreadCountWithBenchmarking(JobCpuIntensity intensity)
	{
		int currentTotal = static_cast<int>(std::thread::hardware_concurrency());	//Starts with the max concurrent threads of the CPU
		double previousTimeTaken = 0;
		double previousDiff = 0;
		double currentTimeTaken = 0;
		double currentDiff = 0;

		do
		{
			//Switch the vars
			previousTimeTaken = currentTimeTaken;
			previousDiff = currentDiff;

			//Create a vector of threads
			std::vector<std::thread> threads;
			threads.reserve(static_cast<unsigned long long int>(currentTotal));

			for (int i = 0; i < currentTotal; ++i)
			{
				threads.emplace_back(std::thread(JobSystem::ThreadBenchmarkTestFunction));
			}

			//Run a job on the threads and benchmark the time taken for the threads to join
			std::chrono::time_point<std::chrono::high_resolution_clock> timeStamp;
			std::for_each(threads.begin(), threads.end(), std::mem_fn(&std::thread::join));
			currentTimeTaken = static_cast<double>(std::chrono::duration_cast<std::chrono::milliseconds>(
					std::chrono::high_resolution_clock::now() - timeStamp
			).count());
			currentDiff = currentTimeTaken - previousTimeTaken;
			previousDiff = previousDiff == 0 ? currentDiff : previousDiff;
			currentTotal += _threadsPerStep;
		} while (previousDiff / currentDiff > (1.0 - _performanceThreshold));

		return CalculateThreadsForGivenMax(currentTotal - _threadsPerStep, intensity);
	}

	/*
		Quicker method of initilising Job pool count.
		Takes the Host systems logical thread count and uses that as the max.
		Much faster than CalculateThreadCountWithBenchmarking().
	*/
	int CalculateThreadCountWithoutBenchmarking(JobCpuIntensity intensity)
	{
		return CalculateThreadsForGivenMax(static_cast<int>(std::thread::hardware_concurrency()), intensity);
	}


	std::mutex _jobQueueMutex;
	std::list<Job> _jobQueue;

	std::mutex _cachedJobQueueMutex;
	std::vector<Job> _cachedJobs;

	std::atomic<int> _currentBatch;
	std::atomic<int> _currentBatchProgress;
	std::atomic<int> _maxBatch;
	std::atomic<int> _totalJobsInProgress;

	std::vector<std::unique_ptr<PoolableThread>> _threads;

	//Once the time taken to create the threads gets worse than this it treats the previous value as threadMax
	const float _performanceThreshold = 0.075f;
	const int _threadsPerStep = 2;
};