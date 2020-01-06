#pragma once
#include "BaseTest.h"
#include "Npc.h"
#include <memory>

class NoSystemsTest : public BaseTest
{
public:

	NoSystemsTest() = delete;
	NoSystemsTest(int npcMax);
	~NoSystemsTest() override;

	// Inherited via BaseTest
	virtual void PreUpdate(float dt) override;
	virtual void Update(float dt) override;
	virtual void PostUpdate(float dt) override;


private:

	void NpcShieldTest();
	void NpcHealthTest();
	void NpcArmourTest();

	void SanityCheckRunCount() override;

	std::vector<std::unique_ptr<Npc>> _npcVec;
};

