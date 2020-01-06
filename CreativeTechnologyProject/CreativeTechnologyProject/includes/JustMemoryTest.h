#pragma once
#include "BaseTest.h"
#include "MemoryManager.h"
#include "NpcManager.h"

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

	MemoryManager* _memManager = nullptr;
	NpcManager* _npcManager = nullptr;
};

