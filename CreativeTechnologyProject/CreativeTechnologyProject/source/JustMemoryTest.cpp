#include "..\includes\JustMemoryTest.h"

JustMemoryTest::JustMemoryTest(int npcMax)
{
	_memManager = new MemoryManager(sizeof(Npc), npcMax);
	_npcManager = new NpcManager(npcMax, _memManager);

	SanityCheckRunCount();
}

JustMemoryTest::~JustMemoryTest()
{
	delete _npcManager;
	delete _memManager;
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
	if (_runCount > _npcManager->GetNpcCount())
	{
		_runCount = _npcManager->GetNpcCount();
	}

	if (_runCount < 0)
	{
		_runCount = 0;
	}
}

void JustMemoryTest::NpcShieldTest()
{
	for (size_t i = 0; i < _runCount; i++)
	{
		float* npcShield = _npcManager->_npcShieldBase + i;
		ShieldAdjustment(npcShield);
	}
}

void JustMemoryTest::NpcHealthTest()
{
	for (size_t i = 0; i < _runCount; i++)
	{
		float* npcHealth = _npcManager->_npcHealthBase + i;
		HealthAdjustment(npcHealth);
	}
}

void JustMemoryTest::NpcArmourTest()
{
	for (size_t i = 0; i < _runCount; i++)
	{
		int* npcArmour = _npcManager->_npcArmourBase + i;
		ArmourAdjustment(npcArmour);
	}
}
