#include "..\includes\AllSystemsTest.h"
#include "imgui/imgui.h"
#include <string>

AllSystemsTest::AllSystemsTest(int npcMax)
{
	//Init the managers for the test
	_memManager = new MemoryManager(sizeof(Npc), npcMax);
	_npcManager = new NpcManager(npcMax, _memManager);
	_jobManager = new JobManager(10);

	SanityCheckRunCount();
}

AllSystemsTest::~AllSystemsTest()
{
	delete _jobManager;
	delete _npcManager;
	delete _memManager;
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
	for (size_t i = 0; i < _runCount; i++)
	{
		float* npcShield = _npcManager->_npcShieldBase + i;
		ShieldAdjustment(npcShield);
	}
}

void AllSystemsTest::NpcHealthTest()
{
	for (size_t i = 0; i < _runCount; i++)
	{
		float* npcHealth = _npcManager->_npcHealthBase + i;
		HealthAdjustment(npcHealth);
	}
}

void AllSystemsTest::NpcArmourTest()
{
	for (size_t i = 0; i < _runCount; i++)
	{
		int* npcArmour = _npcManager->_npcArmourBase + i;
		ArmourAdjustment(npcArmour);
	}
}

void AllSystemsTest::SanityCheckRunCount()
{
	if (_runCount > _npcManager->GetNpcCount())
	{
		_runCount = _npcManager->GetNpcCount();
	}

	if (_runCount < 0)
	{
		_runCount = 0;
	}
}
