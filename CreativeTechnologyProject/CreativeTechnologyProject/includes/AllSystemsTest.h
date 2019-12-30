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

	std::unique_ptr<MemoryManager> _memManager = nullptr;
	std::unique_ptr<JobManager> _jobManager = nullptr;
	std::unique_ptr<NpcManager> _npcManager = nullptr;
};

