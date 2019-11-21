#include "..\includes\JobManager.h"

void JobManager::AddJobToQueue(Job job)
{
	_jobQueue.push(std::move(job));
}

void JobManager::ProcessJobsOnThreads()
{
	//for each job in the queue
		//cache a local ref of job and pop if off the queue
		//Run a thread using the function pointer feeding in the dataptr and datasetlength


	//wait for the join? cant really remember
}
