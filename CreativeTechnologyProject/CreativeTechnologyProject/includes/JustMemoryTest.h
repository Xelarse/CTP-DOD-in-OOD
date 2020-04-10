#pragma once
#include "BaseTest.h"
#include "MemorySystems.hpp"
#include "Npc.h"

class JustMemoryTest : public BaseTest
{
public:

	JustMemoryTest() = delete;
	JustMemoryTest(int npcMax);
	~JustMemoryTest() override;

	virtual void PreUpdate(float dt) override;
	virtual void Update(float dt) override;
	virtual void PostUpdate(float dt) override;

private:
	virtual void SanityCheckRunCount() override;

	void NpcShieldTest();
	void NpcHealthTest();
	void NpcArmourTest();

	std::vector<Npc> _npcs;
	MemoryManager* _memoryManager = nullptr;
};

