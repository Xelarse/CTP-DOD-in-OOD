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
	float* base = _npcs.front()._shield.GetBasePtr();
	for (size_t i = 0; i < _runCount; ++i)
	{
		float* npcShield = base + i;
		ShieldAdjustment(npcShield);
	}
}

void JustMemoryTest::NpcHealthTest()
{
	float* base = _npcs.front()._health.GetBasePtr();
	for (size_t i = 0; i < _runCount; ++i)
	{
		float* npcHealth = base + i;
		HealthAdjustment(npcHealth);
	}
}

void JustMemoryTest::NpcArmourTest()
{
	int* base = _npcs.front()._armour.GetBasePtr();
	for (size_t i = 0; i < _runCount; ++i)
	{
		int* npcArmour = base + i;
		ArmourAdjustment(npcArmour);
	}
}