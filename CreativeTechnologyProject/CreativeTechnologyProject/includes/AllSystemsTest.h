#pragma once
#include "BaseTest.h"
#include "JobManager.h"
#include "NpcManager.h"
#include "MemoryManager.h"

class AllSystemsTest : public BaseTest
{
public:
	AllSystemsTest();
	~AllSystemsTest() override;

	// Inherited via BaseTest
	virtual void PreUpdate(float dt) override;
	virtual void Update(float dt) override;
	virtual void PostUpdate(float dt) override;

	virtual void RenderImguiWindow() override;

private:
	void NpcShieldTest();
	void NpcHealthTest();
	void NpcArmourTest();

	void SafetyCheckRunCount();

	MemoryManager* _memManager = nullptr;
	JobManager* _jobManager = nullptr;
	NpcManager* _npcManager = nullptr;
};

