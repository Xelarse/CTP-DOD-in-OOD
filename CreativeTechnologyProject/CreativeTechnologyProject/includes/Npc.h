#pragma once
class Npc
{
public:
	Npc(float* healthPtr, int* armourPtr);
	Npc() = delete;
	~Npc();

	void printHPandArmour();

private:
	float* _health;
	int* _armour;
};

