#pragma once
#include <random>

class NpcNoMem
{
public:
	NpcNoMem(std::mt19937 ranGen);
	NpcNoMem() = delete;
	~NpcNoMem();

	float* _health = nullptr;
	float* _shield = nullptr;
	int* _armour = nullptr;
};
