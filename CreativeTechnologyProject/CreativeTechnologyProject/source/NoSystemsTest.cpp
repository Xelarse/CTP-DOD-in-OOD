#include "..\includes\NoSystemsTest.h"
#include "imgui/imgui.h"
#include <random>

NoSystemsTest::NoSystemsTest(int npcMax)
{
	_npcVec.reserve(npcMax);
	std::random_device rd;
	std::mt19937 generator(rd());

	for (size_t i = 0; i < npcMax; i++)
	{
		_npcVec.emplace_back(std::make_unique<NpcNoMem>(generator));
	}

	SanityCheckRunCount();
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

void NoSystemsTest::NpcShieldTest()
{
	for (const auto& npc : _npcVec)
	{
		ShieldAdjustment(npc->_shield);
	}
}

void NoSystemsTest::NpcHealthTest()
{
	for (const auto& npc : _npcVec)
	{
		HealthAdjustment(npc->_health);
	}
}

void NoSystemsTest::NpcArmourTest()
{
	for (const auto& npc : _npcVec)
	{
		ArmourAdjustment(npc->_armour);
	}
}

void NoSystemsTest::SanityCheckRunCount()
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
