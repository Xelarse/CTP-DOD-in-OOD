#pragma once
#include "BaseTest.h"
#include "JobManager.h"
#include "MemoryManager.h"
#include "Npc.h"

class AllSystemsTest : public BaseTest
{
public:
	AllSystemsTest() = delete;
	AllSystemsTest(int npcMax);
	~AllSystemsTest() override;

	// Inherited via BaseTest
	virtual void PreUpdate(float dt) override;
	virtual void Update(float dt) override;
	virtual void PostUpdate(float dt) override;

private:
	void NpcShieldTest();
	void NpcHealthTest();
	void NpcArmourTest();

	void SanityCheckRunCount() override;

	JobManager* _jobManager = nullptr;
	MemoryManager* _memoryManager = nullptr;
	std::vector<Npc> _npcs;
};

