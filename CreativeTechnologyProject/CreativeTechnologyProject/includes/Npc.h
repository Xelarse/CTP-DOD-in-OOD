#pragma once
class Npc
{
public:
	Npc(float* healthPtr, int* armourPtr, float* shieldPtr);
	Npc() = delete;
	~Npc();

	void PrintHPandArmour();
	static void SizeOfNpcPrint();

private:
	float* _shield;
	float* _health;
	int* _armour;
};

