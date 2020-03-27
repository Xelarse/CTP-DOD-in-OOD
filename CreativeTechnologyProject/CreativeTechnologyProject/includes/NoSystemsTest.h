#pragma once
#include <memory>
#include "BaseTest.h"
#include "NpcNoMem.h"

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

	std::vector<std::unique_ptr<NpcNoMem>> _npcVec;
};

