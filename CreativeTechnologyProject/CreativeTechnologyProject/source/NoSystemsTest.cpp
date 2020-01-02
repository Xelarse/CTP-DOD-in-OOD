#include "..\includes\NoSystemsTest.h"
#include "imgui/imgui.h"
#include <random>

NoSystemsTest::NoSystemsTest()
{
	std::random_device rd;
	std::mt19937 generator(rd());

	for (size_t i = 0; i < 200000; i++)
	{
		_npcVec.emplace_back(std::make_unique<Npc>(generator));
	}
}

NoSystemsTest::~NoSystemsTest()
{
}

void NoSystemsTest::PreUpdate(float dt)
{
}

void NoSystemsTest::Update(float dt)
{
	NpcShieldTest();
	NpcHealthTest();
	NpcArmourTest();
}

void NoSystemsTest::PostUpdate(float dt)
{
}

void NoSystemsTest::RenderImguiWindow()
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

void NoSystemsTest::NpcShieldTest()
{
	for (size_t i = 0; i < _runCount; i++)
	{
		ShieldAdjustment(_npcVec[i]->GetShield());
	}
}

void NoSystemsTest::NpcHealthTest()
{
	for (size_t i = 0; i < _runCount; i++)
	{
		HealthAdjustment(_npcVec[i]->GetHealth());
	}
}

void NoSystemsTest::NpcArmourTest()
{
	for (size_t i = 0; i < _runCount; i++)
	{
		ArmourAdjustment(_npcVec[i]->GetArmour());
	}
}

void NoSystemsTest::SafetyCheckRunCount()
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
