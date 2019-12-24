#pragma once
class Npc
{
public:
	Npc(float* healthPtr, int* armourPtr);
	Npc() = delete;
	~Npc();

	void PrintHPandArmour();
	static void SizeOfNpcPrint();

private:
	float* _health;
	int* _armour;
};

