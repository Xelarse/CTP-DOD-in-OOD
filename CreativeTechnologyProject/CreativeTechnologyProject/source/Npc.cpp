#include "..\includes\Npc.h"
#include <iostream>

Npc::Npc(float health, int armour, float shield)
{
	_health = AA::Variable<float, _healthTag>(health);
	_shield = AA::Variable<float, _shieldTag>(shield);
	_armour = AA::Variable<int, _armourTag>(armour);
}

Npc::Npc(std::mt19937 ranGen)
{
	std::uniform_real_distribution<float> healthGen(0, 100);
	std::uniform_real_distribution<float> shieldGen(0, 200);
	std::uniform_int_distribution<> armourGen(0, 50);

	_health = AA::Variable<float, _healthTag>(healthGen(ranGen));
	_shield = AA::Variable<float, _shieldTag>(shieldGen(ranGen));
	_armour = AA::Variable<int, _armourTag>(armourGen(ranGen));
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
