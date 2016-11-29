#ifndef PC_H
#define PC_H

#include "character.h"
#include "equipment.h"
#include "inventory.h"

class PC : public Character
{
	public:
		PC();
		~PC();

		NPC *attacking; // currently attacking

		Equipment *equipment;
		Inventory *inventory;

		char *seenDungeon;
			
		void clearSeenDungeon();
		
		void dropItem(int i);
		void dropItem(Item *);

		inline char *getSeenDungeon() { return seenDungeon; }

		void pickUpItem();

		void setLocation(int x, int y);

		void takeOffItem(int i); // i:= equipment index

		void wearItem(int i); // i:= inventory index

		int getSpeed();

		int getTotalDam();

		int getTotalDamRanged();

		int getTotalDef();

		int hpmax(); // _hpmax plus item hit bonus
};

extern PC *pc;

#endif

