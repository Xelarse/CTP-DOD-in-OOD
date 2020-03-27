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
	auto& frontNpcShield = _npcs.front()._shield;
	for (float* i = frontNpcShield.GetBasePtr(); i < frontNpcShield.GetBasePtr() + _runCount; ++i)
	{
		ShieldAdjustment(i);
	}
}

void AllSystemsTest::NpcHealthTest()
{
	auto& frontNpcHealth = _npcs.front()._health;
	for (float* i = frontNpcHealth.GetBasePtr(); i < frontNpcHealth.GetBasePtr() + _runCount; ++i)
	{
		HealthAdjustment(i);
	}
}

void AllSystemsTest::NpcArmourTest()
{
	auto& frontNpcArmour = _npcs.front()._armour;
	for (int* i = frontNpcArmour.GetBasePtr(); i < frontNpcArmour.GetBasePtr() + _runCount; ++i)
	{
		ArmourAdjustment(i);
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
