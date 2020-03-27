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
	//Add jobs to do to the queue
	_jobManager->AddJobToQueue(JobManager::Job([&](){ NpcShieldTest(); }));
	_jobManager->AddJobToQueue(JobManager::Job([&](){ NpcHealthTest(); }));
	_jobManager->AddJobToQueue(JobManager::Job([&](){ NpcArmourTest(); }));
}

void AllSystemsTest::PostUpdate(float dt)
{
	_jobManager->ProcessJobs();
}

void AllSystemsTest::NpcShieldTest()
{
	float* base = _npcs.front()._shield.GetBasePtr();
	for (size_t i = 0; i < _runCount; ++i)
	{
		float* npcShield = base + i;
		ShieldAdjustment(npcShield);
	}
}

void AllSystemsTest::NpcHealthTest()
{
	float* base = _npcs.front()._health.GetBasePtr();
	for (size_t i = 0; i < _runCount; ++i)
	{
		float* npcHealth = base + i;
		HealthAdjustment(npcHealth);
	}
}

void AllSystemsTest::NpcArmourTest()
{
	int* base = _npcs.front()._armour.GetBasePtr();
	for (size_t i = 0; i < _runCount; ++i)
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
