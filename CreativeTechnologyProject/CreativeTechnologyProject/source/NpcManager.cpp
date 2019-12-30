#include "NpcManager.h"
#include "MemoryManager.h"

NpcManager::NpcManager(int npcAmount, MemoryManager* memManager) : _memManager(memManager)
{
	_npcHealthBase = memManager->InitliaseNewMemoryBlock<float>(_npcHealthId, npcAmount);
	_npcArmourBase = memManager->InitliaseNewMemoryBlock<int>(_npcArmourId, npcAmount);
	_npcShieldBase = memManager->InitliaseNewMemoryBlock<float>(_npcShieldId, npcAmount);

	_classIds.push_back(_npcHealthId);
	_classIds.push_back(_npcArmourId);
	_classIds.push_back(_npcShieldId);

	Npc::SizeOfNpcPrint();

	for (size_t i = 0; i < npcAmount; i++)
	{
		float* health = memManager->InitliaseVariable(_npcHealthId, static_cast<float>(i * 10));
		float* shield = memManager->InitliaseVariable(_npcShieldId, static_cast<float>(i * 100));
		int* armour = memManager->InitliaseVariable(_npcArmourId, static_cast<int>(i));
		_npcs.emplace_back(std::make_unique<Npc>(health, armour, shield));
	}

	for (size_t i = 0; i < npcAmount; i++)
	{
		std::cout << "NPC " << i << ": ";
		_npcs[i]->PrintHPandArmour();
		std::cout << std::endl;
	}
}

NpcManager::~NpcManager()
{
	for (const auto& id : _classIds)
	{
		_memManager->FreeExistingBlock(id);
	}

	_npcs.clear();
}

void NpcManager::LoopAndPrintTest()
{
	for (int i = _npcs.size() - 1; i >= 0; i--)
	{
		_npcs[i]->PrintHPandArmour();
	}
}
