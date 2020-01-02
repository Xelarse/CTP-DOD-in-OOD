#include "..\includes\AllSystemsTest.h"
#include "imgui/imgui.h"
#include <string>

AllSystemsTest::AllSystemsTest()
{
	//Init the managers for the test
	_memManager = new MemoryManager(sizeof(Npc), 200000);
	_npcManager = new NpcManager(200000, _memManager);
	_jobManager = new JobManager(10);

	SafetyCheckRunCount();
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

void AllSystemsTest::RenderImguiWindow()
{
	if (_results.size() > 0)
	{
		std::vector<float> results;
		results.reserve(_queueLength);

		for (size_t i = 0; i < _results.size(); i++)
		{
			results.push_back(_results[i]);
		}

		if (ImGui::Begin("Queue Vals"))
		{
			ImGui::InputInt("Job Iterations Count:", &_runCount);
			ImGui::PlotLines("Time taken, milliseconds:", results.data(), results.size(), 0, 0, 0.0f, 20.0f, ImVec2(0, 400));
		}
		ImGui::End();
	}

	SafetyCheckRunCount();
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

void AllSystemsTest::SafetyCheckRunCount()
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
