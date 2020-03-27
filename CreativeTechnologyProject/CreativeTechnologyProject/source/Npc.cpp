#include "..\includes\Npc.h"
#include <iostream>

Npc::Npc(MemoryManager* manager, float health, int armour, float shield) :
	_health(AAVariable<float>(manager, AAHash("NPCHEALTH"), health)),
	_shield(AAVariable<float>(manager, AAHash("NPCSHIELD"), shield)),
	_armour(AAVariable<int>(manager, AAHash("NPCARMOUR"), armour))
{
}