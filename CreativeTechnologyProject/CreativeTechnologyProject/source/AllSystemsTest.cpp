#include "..\includes\AllSystemsTest.h"
#include "imgui/imgui.h"
#include <string>
#include <random>

AllSystemsTest::AllSystemsTest(int npcMax) :
	_memoryManager(new MemoryManager(sizeof(Npc), npcMax)), 
	_jobManager(new JobManager(JobManager::JobCpuIntensity::HIGH))
{
	std::random_device r;
	std::mt19937 gen(r());

	std::uniform_real_distribution<float> healthGen(0, 100);
	std::uniform_real_distribution<float> shieldGen(0, 200);
	std::uniform_int_distribution<> armourGen(0, 50);

	for (size_t i = 0; i < npcMax; ++i)
	{
		_npcs.push_back(Npc(_memoryManager, healthGen(gen), armourGen(gen), shieldGen(gen)));
	}

	SanityCheckRunCount();
}

AllSystemsTest::~AllSystemsTest()
{
	delete _jobManager;
	delete _memoryManager;
}

void AllSystemsTest::PreUpdate(float dt)
{
}

void AllSystemsTest::Update(float dt)
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

			_jobManager->AddJobToQueue(JobManager::Job(std::function<void()>(std::bind(&AllSystemsTest::NpcShieldTest, this, startVal, endVal))));
			_jobManager->AddJobToQueue(JobManager::Job(std::function<void()>(std::bind(&AllSystemsTest::NpcHealthTest, this, startVal, endVal))));
			_jobManager->AddJobToQueue(JobManager::Job(std::function<void()>(std::bind(&AllSystemsTest::NpcArmourTest, this, startVal, endVal))));
		}
	}
}

void AllSystemsTest::PostUpdate(float dt)
{
	_jobManager->ProcessJobs();
}

void AllSystemsTest::NpcShieldTest(int startInd, int endInd)
{
	float* base = _npcs.front()._shield.GetBasePtr();
	for (size_t i = startInd; i <= endInd; ++i)
	{
		float* npcShield = base + i;
		ShieldAdjustment(npcShield);
	}
}

void AllSystemsTest::NpcHealthTest(int startInd, int endInd)
{
	float* base = _npcs.front()._health.GetBasePtr();
	for (size_t i = startInd; i <= endInd; ++i)
	{
		float* npcHealth = base + i;
		HealthAdjustment(npcHealth);
	}
}

void AllSystemsTest::NpcArmourTest(int startInd, int endInd)
{
	int* base = _npcs.front()._armour.GetBasePtr();
	for (size_t i = startInd; i <= endInd; ++i)
	{
		int* npcArmour = base + i;
		ArmourAdjustment(npcArmour);
	}
}

void AllSystemsTest::SanityCheckRunCount()
{
	if (_runCount > _npcs.size())
	{
		_runCount = _npcs.size();
	}

	if (_runCount < 0)
	{
		_runCount = 0;
	}
}
