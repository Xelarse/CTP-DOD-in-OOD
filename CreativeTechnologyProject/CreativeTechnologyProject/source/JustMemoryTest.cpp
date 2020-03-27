#include "..\includes\JustMemoryTest.h"

JustMemoryTest::JustMemoryTest(int npcMax) :
	_memoryManager(new MemoryManager(sizeof(Npc), npcMax))
{	
	std::random_device r;
	std::mt19937 gen(r());

	std::uniform_real_distribution<float> healthGen(0, 100);
	std::uniform_real_distribution<float> shieldGen(0, 200);
	std::uniform_int_distribution<> armourGen(0,50);

	for (size_t i = 0; i < npcMax; ++i)
	{
		_npcs.push_back(Npc(_memoryManager, healthGen(gen), armourGen(gen), shieldGen(gen)));
	}

	SanityCheckRunCount();
}

JustMemoryTest::~JustMemoryTest()
{
	delete _memoryManager;
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
	auto& frontNpcShield = _npcs.front()._shield;
	for (float* i = frontNpcShield.GetBasePtr(); i < frontNpcShield.GetBasePtr() + _runCount; ++i)
	{
		ShieldAdjustment(i);
	}
}

void JustMemoryTest::NpcHealthTest()
{
	auto& frontNpcHealth = _npcs.front()._health;
	for (float* i = frontNpcHealth.GetBasePtr(); i < frontNpcHealth.GetBasePtr() + _runCount; ++i)
	{
		HealthAdjustment(i);
	}
}

void JustMemoryTest::NpcArmourTest()
{
	auto& frontNpcArmour = _npcs.front()._armour;
	for (int* i = frontNpcArmour.GetBasePtr(); i < frontNpcArmour.GetBasePtr() + _runCount; ++i)
	{
		ArmourAdjustment(i);
	}
}