#pragma once
#include <random>

class Npc
{
public:
	Npc(float* healthPtr, int* armourPtr, float* shieldPtr);
	Npc() = delete;
	Npc(std::mt19937 ranGen);
	~Npc();

	void PrintHPandArmour();
	static void SizeOfNpcPrint();

	float* GetShield() { return _shield; }
	float* GetHealth() { return _health; }
	int* GetArmour() { return _armour; }

private:
	float* _shield;
	float* _health;
	int* _armour;

	bool _selfCleanup;
};

