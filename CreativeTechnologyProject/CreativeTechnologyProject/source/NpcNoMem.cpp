#include "..\includes\NpcNoMem.h"

NpcNoMem::NpcNoMem(std::mt19937 ranGen)
{
	std::uniform_real_distribution<float> healthGen(0, 100);
	std::uniform_real_distribution<float> shieldGen(0, 200);
	std::uniform_int_distribution<> armourGen(0, 50);

	_health = new float(healthGen(ranGen));
	_shield = new float(shieldGen(ranGen));
	_armour = new int(armourGen(ranGen));
}

NpcNoMem::~NpcNoMem()
{
	if (_health != nullptr)
	{
		delete _health;
	}

	if (_armour != nullptr)
	{
		delete _armour;
	}

	if (_shield != nullptr)
	{
		delete _shield;
	}
}
