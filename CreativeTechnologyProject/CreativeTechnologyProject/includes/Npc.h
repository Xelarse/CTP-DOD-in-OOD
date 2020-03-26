#pragma once
#include <random>
#include "AAVariable.h"

class Npc
{
public:
	Npc(float health, int armour, float shield);
	Npc() = delete;
	~Npc() = default;

	void PrintHPandArmour();
	static void SizeOfNpcPrint();

private:
	static std::string _healthTag;
	static std::string _shieldTag;
	static std::string _armourTag;

	AA::Variable<float, _healthTag> _health;
	AA::Variable<float, _shieldTag> _shield;
	AA::Variable<int, _armourTag> _armour;
};

std::string Npc::_healthTag = "NPCHEALTH";
std::string Npc::_shieldTag = "NPCSHIELD";
std::string Npc::_armourTag = "NPCARMOUR";
