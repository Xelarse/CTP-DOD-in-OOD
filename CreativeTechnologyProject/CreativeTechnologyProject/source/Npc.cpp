#include "..\includes\Npc.h"
#include <iostream>

Npc::Npc(float health, int armour, float shield)
{
	_health = AA::Variable<float, _healthTag>(health);
	_shield = AA::Variable<float, _shieldTag>(shield);
	_armour = AA::Variable<int, _armourTag>(armour);
}


void Npc::PrintHPandArmour()
{
	std::cout << "Health: " << _health.Get() << " --- Armour: " << _armour.Get() << std::endl;
}

void Npc::SizeOfNpcPrint()
{
	std::cout << "Size of NPC: " << sizeof(Npc) << std::endl;
	std::cout << "Size of float*: " << sizeof(float*) << " Size of int*: " << sizeof(int*) << std::endl;
}
