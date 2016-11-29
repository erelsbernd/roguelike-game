#ifndef INVENTORY_H
#define INVENTORY_H

#include "item.h"

static const int MAX_SIZE = 10;

using namespace std;

class Inventory
{
	public:
		Inventory();
		~Inventory();

		vector<Item *> slots;

		void expunge(int i);

		inline bool add(Item *i)
		{
			if (isFull())
				return false;
			slots.push_back(i);
			return true;
		}

		inline bool isFull()
		{
			return slots.size() == MAX_SIZE;
		}
		
		inline Item *remove(int i)
		{
			if (i < 0 || i >= (int)slots.size())
				return NULL;
			Item *old = slots[i];
			slots.erase(slots.begin() + i);
			return old;
		}

};

#endif
