#include "..\includes\NoSystemsTest.h"
#include "imgui/imgui.h"
#include <random>

NoSystemsTest::NoSystemsTest(int npcMax)
{
	_npcVec.reserve(npcMax);
	std::random_device rd;
	std::mt19937 generator(rd());

	for (size_t i = 0; i < npcMax; ++i)
	{
		_npcVec.emplace_back(std::make_unique<NpcNoMem>(generator));
		_npcHealthVec.push_back(_npcVec.back()->_health);
		_npcShieldVec.push_back(_npcVec.back()->_shield);
		_npcArmourVec.push_back(_npcVec.back()->_armour);
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
	for (size_t i = 0; i < _runCount; i++)
	{
		ShieldAdjustment(_npcShieldVec[i]);
	}
}

void NoSystemsTest::NpcHealthTest()
{
	for (size_t i = 0; i < _runCount; i++)
	{
		HealthAdjustment(_npcHealthVec[i]);
	}
}

void NoSystemsTest::NpcArmourTest()
{
	for (size_t i = 0; i < _runCount; i++)
	{
		ArmourAdjustment(_npcArmourVec[i]);
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
