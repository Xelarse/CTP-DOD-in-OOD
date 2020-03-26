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