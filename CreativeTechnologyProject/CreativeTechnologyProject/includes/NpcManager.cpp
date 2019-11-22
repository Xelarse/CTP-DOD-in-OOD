#include "NpcManager.h"
#include "MemoryManager.h"

NpcManager::NpcManager(int npcAmount, MemoryManager* memManager) : _memManager(memManager)
{
	_npcHealthBase = memManager->InitliaseNewMemoryBlock<float>(_npcHealthId, npcAmount);
	_npcArmourBase = memManager->InitliaseNewMemoryBlock<int>(_npcArmourId, npcAmount);

	Npc::sizeOfNpcPrint();

	for (size_t i = 0; i < npcAmount; i++)
	{
		float* health = memManager->InitliaseVariable(_npcHealthId, static_cast<float>(i * 10));
		int* armour = memManager->InitliaseVariable(_npcArmourId, static_cast<int>(i));
		_npcs.emplace_back(std::make_unique<Npc>(health, armour));
	}

	for (size_t i = 0; i < npcAmount; i++)
	{
		std::cout << "NPC " << i << ": ";
		_npcs[i]->printHPandArmour();
		std::cout << std::endl;
	}
}

NpcManager::~NpcManager()
{
}
