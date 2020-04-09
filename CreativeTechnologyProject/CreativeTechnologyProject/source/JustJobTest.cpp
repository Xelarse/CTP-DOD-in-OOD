#include "..\includes\JustJobTest.h"
#include "imgui/imgui.h"
#include <random>

JustJobTest::JustJobTest(int npcMax)
{
	_jobManager = new JobManager(JobManager::JobCpuIntensity::HIGH);
	_npcVec.reserve(npcMax);
	std::random_device rd;
	std::mt19937 generator(rd());

	for (size_t i = 0; i < npcMax; ++i)
	{
		_npcVec.emplace_back(std::make_unique<NpcNoMem>(generator));
	}
	SanityCheckRunCount();
}

JustJobTest::~JustJobTest()
{
	delete _jobManager;
}

void JustJobTest::PreUpdate(float dt)
{
}

void JustJobTest::Update(float dt)
{
	//Get how many threads there are for jobs to be processed on
	int threadsPerTest = floor(_jobManager->GetTotalThreads() / 3);

	if (_runCount < threadsPerTest || threadsPerTest == 0)
	{
		_jobManager->AddJobToQueue(JobManager::Job([&](){ NpcShieldTest(0, _runCount - 1); }));
		_jobManager->AddJobToQueue(JobManager::Job([&](){ NpcHealthTest(0, _runCount - 1); }));
		_jobManager->AddJobToQueue(JobManager::Job([&](){ NpcArmourTest(0, _runCount - 1); }));
	}
	else
	{
		//Divide into 3 for each of the tests
		int stride = floor(_runCount / threadsPerTest);

		//Send off the jobs ready to process
		for (int i = 0; i < threadsPerTest; ++i)
		{
			int startVal = stride * i;
			int endVal = i == threadsPerTest - 1 ? _runCount - 1 : (stride * (i + 1)) - 1;

			_jobManager->AddJobToQueue(JobManager::Job(std::function<void()>(std::bind(&JustJobTest::NpcShieldTest, this, startVal, endVal))));
			_jobManager->AddJobToQueue(JobManager::Job(std::function<void()>(std::bind(&JustJobTest::NpcHealthTest, this, startVal, endVal))));
			_jobManager->AddJobToQueue(JobManager::Job(std::function<void()>(std::bind(&JustJobTest::NpcArmourTest, this, startVal, endVal))));
		}
	}
}

void JustJobTest::PostUpdate(float dt)
{
	_jobManager->ProcessJobs();
}

void JustJobTest::NpcShieldTest(int startInd, int endInd)
{
	for (size_t i = startInd; i <= endInd; ++i)
	{
		ShieldAdjustment(_npcVec[i]->_shield);
	}
}

void JustJobTest::NpcHealthTest(int startInd, int endInd)
{
	for (size_t i = startInd; i <= endInd; ++i)
	{
		HealthAdjustment(_npcVec[i]->_health);
	}
}

void JustJobTest::NpcArmourTest(int startInd, int endInd)
{
	for (size_t i = startInd; i <= endInd; ++i)
	{
		ArmourAdjustment(_npcVec[i]->_armour);
	}
}

void JustJobTest::SanityCheckRunCount()
{
	if (_runCount > _npcVec.size())
	{
		_runCount = _npcVec.size();
	}

	if (_runCount < 0)
	{
		_runCount = 0;
	}
}
