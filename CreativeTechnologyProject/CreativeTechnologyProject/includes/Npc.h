#pragma once
class Npc
{
public:
	Npc(float* healthPtr, int* armourPtr);
	Npc() = delete;
	~Npc();

	void printHPandArmour();
	static void sizeOfNpcPrint();

private:
	float* _health;
	int* _armour;
};

