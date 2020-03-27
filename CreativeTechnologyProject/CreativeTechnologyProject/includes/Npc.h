#pragma once
#include <random>
#include "AAVariable.h"

class Npc
{
public:
	Npc(MemoryManager* manager, float health, int armour, float shield);
	Npc() = delete;
	~Npc() = default;

	AAVariable<float> _health;
	AAVariable<float> _shield;
	AAVariable<int> _armour;
};
