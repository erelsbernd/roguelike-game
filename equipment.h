#ifndef EQUIPMENT_H
#define EQUIPMENT_H

#define SIZE 12

#include "item.h"

class Equipment
{
	public:
		Equipment();
		~Equipment();
		
		Item *slots[SIZE];

		Item *add(Item *);
		Item *remove(int);

		int getSlotIndex(int type);
		int getSlotType(int index);
				
		inline bool isEmpty(int type) {
			return slots[getSlotIndex(type)]==NULL;
		}

		bool isEmpty();
};

#endif
