#include "..\includes\JustJobTest.h"
#include "imgui/imgui.h"
#include <random>

JustJobTest::JustJobTest(int npcMax)
{
	_jobManager = new JobManager(JobManager::JobCpuIntensity::HIGH);
	_npcVec.reserve(npcMax);
	std::random_device rd;
	std::mt19937 generator(rd());

	for (size_t i = 0; i < npcMax; i++)
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
	_jobManager->AddJobToQueue(JobManager::Job([&](){ NpcShieldTest(); }));
	_jobManager->AddJobToQueue(JobManager::Job([&](){ NpcHealthTest(); }));
	_jobManager->AddJobToQueue(JobManager::Job([&](){ NpcArmourTest(); }));
}

void JustJobTest::PostUpdate(float dt)
{
	_jobManager->ProcessJobs();
}

void JustJobTest::NpcShieldTest()
{
	for (const auto& npc : _npcVec)
	{
		ShieldAdjustment(npc->_shield);
	}
}

void JustJobTest::NpcHealthTest()
{
	for (const auto& npc : _npcVec)
	{
		HealthAdjustment(npc->_health);
	}
}

void JustJobTest::NpcArmourTest()
{
	for (const auto& npc : _npcVec)
	{
		ArmourAdjustment(npc->_armour);
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
