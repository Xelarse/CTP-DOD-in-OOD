#include "..\includes\AllSystemsTest.h"
#include "imgui/imgui.h"
#include <string>

AllSystemsTest::AllSystemsTest()
{
	//Init the managers for the test
	_memManager = std::make_unique<MemoryManager>(sizeof(Npc), 1001);
	_npcManager = std::make_unique<NpcManager>(1000, _memManager.get());
	_jobManager = std::make_unique<JobManager>(10);

	SafetyCheckRunCount();
}

AllSystemsTest::~AllSystemsTest()
{
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
		if (ImGui::Begin("Queue Vals"))
		{
			std::string val = "Back of Queue: " + std::to_string(_results.back());
			ImGui::Text(val.c_str());
		}
		ImGui::End();
	}
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
}
