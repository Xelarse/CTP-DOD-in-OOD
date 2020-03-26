#include "..\includes\JustMemoryTest.h"

JustMemoryTest::JustMemoryTest(int npcMax)
{
	MemoryManager::InitialiseManager(sizeof(Npc), npcMax);
	
	std::random_device r;
	std::mt19937 gen(r());

	for (size_t i = 0; i < npcMax; ++i)
	{
		_npcs.push_back(Npc(gen));
	}

	SanityCheckRunCount();
}

JustMemoryTest::~JustMemoryTest()
{
	MemoryManager::ReleaseManager();
}

void JustMemoryTest::PreUpdate(float dt)
{
}

void JustMemoryTest::Update(float dt)
{
	NpcShieldTest();
	NpcHealthTest();
	NpcArmourTest();
}

void JustMemoryTest::PostUpdate(float dt)
{
}

void JustMemoryTest::SanityCheckRunCount()
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

void JustMemoryTest::NpcShieldTest()
{
	using NpcShield = AA::Variable<float, Npc::_shieldTag>;
	for (float* i = NpcShield::GetBasePtr(); i < NpcShield::GetBasePtr() + NpcShield::GetLength(); ++i)
	{
		ShieldAdjustment(i);
	}
}

void JustMemoryTest::NpcHealthTest()
{
	using NpcHealth = AA::Variable<float, Npc::_healthTag>;
	for (float* i = NpcHealth::GetBasePtr(); i < NpcHealth::GetBasePtr() + NpcHealth::GetLength(); ++i)
	{
		HealthAdjustment(i);
	}
}

void JustMemoryTest::NpcArmourTest()
{
	using NpcArmour = AA::Variable<int, Npc::_armourTag>;
	for (int* i = NpcArmour::GetBasePtr(); i < NpcArmour::GetBasePtr() + NpcArmour::GetLength(); ++i)
	{
		ArmourAdjustment(i);
	}
}