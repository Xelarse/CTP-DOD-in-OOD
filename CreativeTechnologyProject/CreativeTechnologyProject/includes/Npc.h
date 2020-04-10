#pragma once
#include <random>
#include "MemorySystemsInterface.hpp"

class Npc
{
public:
	Npc(MemoryManager* manager, float health, int armour, float shield);
	Npc() = delete;
	~Npc() = default;

	AllmanVariable<float> _health;
	AllmanVariable<float> _shield;
	AllmanVariable<int> _armour;
};
