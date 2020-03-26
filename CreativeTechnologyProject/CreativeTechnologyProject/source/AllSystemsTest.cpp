#include "..\includes\AllSystemsTest.h"
#include "imgui/imgui.h"
#include <string>
#include <random>

AllSystemsTest::AllSystemsTest(int npcMax)
{
	//Init the managers for the test
	MemoryManager::InitialiseManager(sizeof(Npc), npcMax);
	_jobManager = new JobManager(JobManager::JobCpuIntensity::HIGH);

	std::random_device r;
	std::mt19937 gen(r());

	for (size_t i = 0; i < npcMax; ++i)
	{
		_npcs.push_back(Npc(gen));
	}

	SanityCheckRunCount();
}

AllSystemsTest::~AllSystemsTest()
{
	delete _jobManager;
	MemoryManager::ReleaseManager();
}

void AllSystemsTest::PreUpdate(float dt)
{
}

void AllSystemsTest::Update(float dt)
{
	//Add jobs to do to the queue
	//TODO ask james if theres a more elegant way to do this or if lambda functions are the way
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
	using NpcShield = AA::Variable<float, Npc::_shieldTag>;
	for (float* i = NpcShield::GetBasePtr(); i < NpcShield::GetBasePtr() + NpcShield::GetLength(); ++i)
	{
		ShieldAdjustment(i);
	}
}

void AllSystemsTest::NpcHealthTest()
{
	using NpcHealth = AA::Variable<float, Npc::_healthTag>;
	for (float* i = NpcHealth::GetBasePtr(); i < NpcHealth::GetBasePtr() + NpcHealth::GetLength(); ++i)
	{
		HealthAdjustment(i);
	}
}

void AllSystemsTest::NpcArmourTest()
{
	using NpcArmour = AA::Variable<int, Npc::_armourTag>;
	for (int* i = NpcArmour::GetBasePtr(); i < NpcArmour::GetBasePtr() + NpcArmour::GetLength(); ++i)
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
