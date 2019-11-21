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
