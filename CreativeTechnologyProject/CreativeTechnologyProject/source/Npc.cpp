#include "..\includes\Npc.h"
#include <iostream>

Npc::Npc(float* healthPtr, int* armourPtr) : _health(healthPtr), _armour(armourPtr)
{
}

Npc::~Npc()
{
}

void Npc::printHPandArmour()
{
	std::cout << "Health: " << *_health << " --- Armour: " << *_armour << std::endl;
}

void Npc::sizeOfNpcPrint()
{
	std::cout << "Size of NPC: " << sizeof(Npc) << std::endl;
	std::cout << "Size of float*: " << sizeof(float*) << " Size of int*: " << sizeof(int*) << std::endl;
}
