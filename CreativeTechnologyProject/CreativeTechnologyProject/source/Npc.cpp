#include "..\includes\Npc.h"
#include <iostream>

Npc::Npc(MemoryManager* manager, float health, int armour, float shield) :
	_health(AllmanVariable<float>(manager, AllmanHash("NPCHEALTH"), health)),
	_shield(AllmanVariable<float>(manager, AllmanHash("NPCSHIELD"), shield)),
	_armour(AllmanVariable<int>(manager, AllmanHash("NPCARMOUR"), armour))
{
}