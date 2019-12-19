#pragma once
#include <string>
#include <vector>
#include <memory>
#include "Npc.h"

class NpcManager
{
public:
	NpcManager(int npcAmount, class MemoryManager* memManager);
	NpcManager() = delete;
	~NpcManager();

	void LoopAndPrintTest();

private:
	class MemoryManager* _memManager = nullptr;

	std::vector<std::unique_ptr<Npc>> _npcs;

	const char*	_npcHealthId = "NPCHEALTH";
	const char* _npcArmourId = "NPCARMOUR";

	std::vector<const char*> _classIds;

	float* _npcHealthBase = nullptr;
	int* _npcArmourBase = nullptr;
};

