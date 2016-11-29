#include "equipment.h"

Equipment::Equipment()
{
	for (int i=0; i<12; i++)
		slots[i]=NULL;
}

Equipment::~Equipment()
{
	for (int i=0; i<12; i++)
		if (slots[i])
			delete slots[i];
}

int Equipment::getSlotIndex(int type)
{
	if (type == 8) {
		// RING
		if (slots[10] && !slots[11])
			return 11;
		else
			return 10;
	}
	if (type >= 9 && type <= 10) return type - 1;
	return type;
}

int Equipment::getSlotType(int index)
{
	if (index >= 10 && index <= 11)
		return 8;
	if (index >=  8 && index <=  9)
		return index + 1;
	return index;
}

bool Equipment::isEmpty()
{
	for (int i=0; i<12; i++)
		if (slots[i])
			return false;
	return true;
}

Item *Equipment::add(Item *item)
{
	int index = getSlotIndex(item->type);
	Item *old = slots[index];
	slots[index] = item;
	return old;
}

Item *Equipment::remove(int i)
{
	Item *old = slots[i];
	slots[i] = NULL;
	return old;
}

