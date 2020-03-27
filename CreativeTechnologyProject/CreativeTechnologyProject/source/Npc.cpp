#include "..\includes\Npc.h"
#include <iostream>

Npc::Npc(float health, int armour, float shield) :
	_health(AAVariable<float>(AAHash("NPCHEALTH"), health)),
	_shield(AAVariable<float>(AAHash("NPCSHIELD"), shield)),
	_armour(AAVariable<int>(AAHash("NPCARMOUR"), armour))
{
}