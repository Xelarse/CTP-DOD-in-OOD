#include "..\includes\Npc.h"
#include <iostream>

Npc::Npc(float* healthPtr, int* armourPtr, float* shieldPtr) : _health(healthPtr), _armour(armourPtr), _shield(shieldPtr), _selfCleanup(false)
{
}

Npc::Npc(std::mt19937 ranGen) : _selfCleanup(true)
{
	std::uniform_int_distribution<> armourGen(0, 50);
	std::uniform_real_distribution<float> healthGen(0, 100);
	std::uniform_real_distribution<float> shieldGen(0, 200);

	_health = new float(healthGen(ranGen));
	_shield = new float(shieldGen(ranGen));
	_armour = new int(armourGen(ranGen));
}

Npc::~Npc()
{
	if (_selfCleanup)
	{
		delete _shield;
		delete _health;
		delete _armour;
	}
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
