#include "..\includes\Npc.h"
#include <iostream>

Npc::Npc(float* healthPtr, int* armourPtr, float* shieldPtr) : _health(healthPtr), _armour(armourPtr), _shield(shieldPtr)
{
}

Npc::~Npc()
{
}

void Npc::PrintHPandArmour()
{
	std::cout << "Health: " << *_health << " --- Armour: " << *_armour << std::endl;
}

void Npc::SizeOfNpcPrint()
{
	std::cout << "Size of NPC: " << sizeof(Npc) << std::endl;
	std::cout << "Size of float*: " << sizeof(float*) << " Size of int*: " << sizeof(int*) << std::endl;
}
