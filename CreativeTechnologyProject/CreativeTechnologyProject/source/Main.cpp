#include <iostream>
#include <cassert>
#include "MemoryAllocator.h"
#include "MemoryManager.h"
#include "NpcManager.h"

// Carry on from here
// http://dmitrysoshnikov.com/compilers/writing-a-memory-allocator/?fbclid=IwAR1riXugjt6f8_VB2V37c-nkgj-kDvBwJIHz2Gg_rDXBzpxor68fvKUhmvg#best-fit-search

int main()
{
	MemoryManager* memoryManager = new MemoryManager(sizeof(Npc), 50);
	
	NpcManager* npcManager = new NpcManager(50, memoryManager);

	delete npcManager;
}