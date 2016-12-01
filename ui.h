#ifndef UI_H
#define UI_H

class UI
{
	private:
		// true if display item details
		static bool inspect;

		static int eListSelect(int);
		static int iListSelect(int);
    static int mListSelectForDescription(int select);
    static int iListSelectForDescription(int select);

		static int printEquipmentSlots();

		static int selectNPC(NPC *);
    static int selectItem(Item* i);
		
	public:
		static int initColors();
		
		/* display a help page when -h is specified */
		static int help();

		static int clearRow(int row);
		static int reprint();
  
  
    //ranged combat
		static int rangedCombat();
    static int cellDescriptionMonsters();
    static int cellDescriptionItems();
  
  
		static int eList();
		static int iList();
		static int mList();
		static int oList();
		static int sList(); // spell list

		// turn on inspect
		static int setInspect(bool inspect);

		static int printHP();
		static int printMP();

		static int printMonsterHP();
		static int printMonsterHP(NPC *);

		static int selectTarget();
    static int selectTargetMonsters();
    static int selectTargetItems(vector<Item *>  &itemsInRoom);
};

#endif
