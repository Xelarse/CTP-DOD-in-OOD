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
		_npcVec.emplace_back(std::make_unique<Npc>(generator));

		auto npc = _npcVec.back().get();
		_npcArmourVec.push_back(npc->GetArmour());
		_npcHealthVec.push_back(npc->GetHealth());
		_npcShieldVec.push_back(npc->GetShield());
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
	for (size_t i = 0; i < _runCount; i++)
	{
		ShieldAdjustment(_npcShieldVec[i]);
	}
}

void JustJobTest::NpcHealthTest()
{
	for (size_t i = 0; i < _runCount; i++)
	{
		HealthAdjustment(_npcHealthVec[i]);
	}
}

void JustJobTest::NpcArmourTest()
{
	for (size_t i = 0; i < _runCount; i++)
	{
		ArmourAdjustment(_npcArmourVec[i]);
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
