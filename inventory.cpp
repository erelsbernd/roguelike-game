#include "inventory.h"

Inventory::Inventory()
{
}

Inventory::~Inventory()
{
	for (int i=0; i<(int)slots.size(); i++)
		delete slots[i];
	
	slots.clear();
}

void Inventory::expunge(int i)
{
	delete slots[i];
	slots.erase(slots.begin() + i);
}
