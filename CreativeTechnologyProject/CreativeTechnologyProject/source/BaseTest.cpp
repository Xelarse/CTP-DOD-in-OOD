#include "..\includes\BaseTest.h"
#include "imgui/imgui.h"
#include <cmath>
#include <string>

void BaseTest::AddTimeToQueue(long long duration)
{
	//Add the values to the queue, if the queue is already at its limit then pop off the front first

	if (_results.size() >= _queueLength)
	{
		while (_results.size() > _queueLength)
		{
			_results.pop_front();
		}
	}

	_results.push_back(duration);
}

void BaseTest::RenderImguiWindow()
{
	if (_results.size() > 0)
	{
		std::vector<float> results;
		results.reserve(_queueLength);

		for (size_t i = 0; i < _results.size(); i++)
		{
			results.push_back(_results[i]);
		}
		ImGui::InputInt("NPC's processed per update", &_runCount);
		SanityCheckRunCount();
		std::string desc = "Most recent update: " + std::to_string(results.back()) + "ms";
		ImGui::Text(desc.c_str());
		ImGui::PlotLines("Time taken in milliseconds.\nAxis run from 0 to 100 ms", results.data(), results.size(), 0, 0, 0.0f, 200.0f, ImVec2(0, 500));
	}
}

void BaseTest::ClearResults()
{
	_results.clear();
}

void BaseTest::ShieldAdjustment(float* shieldVal)
{
	float modifier = 1.12f;
	float value = *shieldVal;
	int flooredVal = floor(value);

	if (flooredVal % 2 == 0)
	{
		*shieldVal += value * modifier;
	}
	else
	{
		*shieldVal -= value * modifier;
	}
}

void BaseTest::HealthAdjustment(float* healthVal)
{
	*healthVal += 1.25f;

	if (*healthVal > 100)
	{
		*healthVal = 100;
	}
}

void BaseTest::ArmourAdjustment(int* armourVal)
{
	*armourVal -= 1;

	if (*armourVal < 0)
	{
		*armourVal = 20;
	}
}
