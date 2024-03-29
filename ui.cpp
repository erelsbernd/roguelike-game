#include <ncurses.h>
#include <stdlib.h>
#include <string.h>

#include <algorithm>

#include "core.h"
#include "character.h"
#include "debug.h"
#include "dungeon.h"
#include "move.h"
#include "npc.h"
#include "objectfactory.h"
#include "pc.h"
#include "ui.h"

bool UI::inspect = false;

static void getPositionStr(char buf[50], Character *c)
{
	int i;
	for(i=0; i<50; i++) buf[i]=' ';

	int cx, cy, pcx, pcy;
	c->getLocation(&cx, &cy);
	pc->getLocation(&pcx, &pcy);

	int dx = cx - pcx;
	int dy = cy - pcy;

	if (dx>0)
		sprintf(buf + 2, "%02d E   ",  dx);
	else
		sprintf(buf + 2, "%02d W   ", -dx);
	if (dx==0)
		sprintf(buf + 2, "00   ");
	if (dy>0)
		sprintf(buf + 8, "%02d S   ",  dy);
	else
		sprintf(buf + 8, "%02d N   ", -dy);
	if (dy==0)
		sprintf(buf + 8, "00   ");
}

static int compareNPC(const void *a, const void *b)
{
	int ax, ay, bx, by, pcx, pcy;

	(*(Character **)a)->getLocation(&ax, &ay);
	(*(Character **)b)->getLocation(&bx, &by);
	pc->getLocation(&pcx, &pcy);

	return (ABS((ax-pcx))+ABS((ay-pcy)))
		- (ABS((bx-pcx))+ABS((by-pcy)));
}

int UI::help()
{
	FILE* fp = fopen("README", "r");

	if (!fp) {
		fprintf(stderr, "README not found\n");
		return -1;
	}

	// ncurses stuff
	initscr();
	start_color();
	raw();
	noecho();
	curs_set(0);
	keypad(stdscr, TRUE);

	int r = 0, c = 0;
	char buffer[255][80];
	while(fgets(buffer[r++], 80, fp));
	fclose(fp);

	int rmax = r;

	int from = 0;

	while(1) {
		clear();

		char tmp[80];
		char *p;

		for (r=0; r<22; r++) {
			p = tmp;

			for (c=0; c<80; c++) {
				if ('\t'==buffer[from+r][c]) {
					sprintf(p, "    ");
					p+=4;
				}
				else *p++ = buffer[from+r][c];
			}
			mvprintw(r, 0, "%s", tmp);
		}

		int color;
		init_pair(color = 9, COLOR_GREEN, COLOR_BLACK);

		attron(COLOR_PAIR(color));

		mvprintw(24-1, 0, 
				"Use arrow keys to scroll up and down. Press Q to quit.");

		attroff(COLOR_PAIR(color));

		refresh();

		int ch = getch();
		if (ch=='Q' || ch=='q') break;
		if (ch==KEY_DOWN) 
			if (from+22<rmax) 
				from++;
		if (ch==KEY_UP)   
			if (from>0)
				from--;
	}

	endwin();

	return 0;
}

int UI::initColors()
{
	init_pair(COLOR_PC, COLOR_RED, COLOR_YELLOW);
	// HP
	init_pair(COLOR_HP, COLOR_WHITE, COLOR_RED);
	// MP
	init_pair(COLOR_MP, COLOR_WHITE, COLOR_BLUE);
	// Frozen
	init_pair(COLOR_FROZEN, COLOR_BLACK, COLOR_CYAN);
	// Poizen
	init_pair(COLOR_POISON, COLOR_BLACK, COLOR_GREEN);

	for (int i = 0; i < 8; i++) {
		init_pair(i, i, COLOR_BLACK);
	}

	return 0;
}

int UI::eListSelect(int select)
{
	int i;
	char buf[50];

	mvprintw(0, 1, "Listing Equipment "
			"(press ESC or Q to quit)");

	int color = COLOR_GREEN;	
	attron(COLOR_PAIR(color));

	mvprintw(1, 20, "  use A-L or UP/DOWN keys to select ");
	mvprintw(2, 20, "  use T to take off ");

	attroff(COLOR_PAIR(color));

	for (i=0; i<12; i++) {
		Item *item = pc->equipment->slots[i];

		sprintf(buf, "%c %s %s  %-11s", (char)'A'+i, 
				select == i ? "[" : " ",
				select == i ? "]" : " ",
				ObjectFactory::TYPE[pc->equipment->getSlotType(i)].c_str());

		mvprintw(i+1, 1, " %s", buf);

		if (item) {
			int color = item->color;		
			attron(COLOR_PAIR(color));
			mvprintw(i+1, 5, "%c", item->symb);
			attroff(COLOR_PAIR(color));
		}
		mvaddch(i+1, 19, ACS_VLINE);
	}
	for (int c=1; c<19; c++)
		mvaddch(13, c, ACS_HLINE);
	mvaddch(13, 19, ACS_LRCORNER);

	return select;
}

int UI::mListSelectForDescription(int select)
{
  dungeon->printDungeon();
  
    int color = COLOR_GREEN;
    attron(COLOR_PAIR(color));
    
    mvprintw(0, 20, "  Press any key to return to dungeon ");
    NPC* mon = dungeon->vnpcv[select];
    int s = 1;
    
    mvprintw(s+2, 0, "%s ", mon->name.c_str());
    mvprintw(s+3, 0, "%s ", mon->desc.c_str());
    attroff(COLOR_PAIR(color));
  
    attron(COLOR_PAIR(color));

      mvprintw(s+8, 0, "   dam = %d-%d ",
               mon->getMonsterMinDam(), mon->getMonsterMaxDam());
      mvprintw(s+9, 0, " speed = %d ", mon->getSpeed());
      mvprintw(s+10, 0, "  abil = %d ", mon->getAbil());
  
    attroff(COLOR_PAIR(color));
  
  getch();
  
  
  
  return select;
}

int UI::iListSelectForDescription(int select)
{
  dungeon->printDungeon();
  
  int color = COLOR_GREEN;
  attron(COLOR_PAIR(color));
  
  mvprintw(0, 20, "  Press any key to return to dungeon ");
  Item* i = dungeon->itemv[select];
  int s = 1;
  
  mvprintw(s+2, 0, "%s ", i->name.c_str());
  mvprintw(s+3, 0, "%s ", i->desc.c_str());
  attroff(COLOR_PAIR(color));
  
  mvprintw(s+9, 0, "   hit = %d ", i->hit);
  mvprintw(s+10, 0, "   dam = %d-%d ",
           i->dam->min(), i->dam->max());
  mvprintw(s+11, 0, " dodge = %d ", i->dodge);
  mvprintw(s+12, 0, "   def = %d ", i->def);
  mvprintw(s+13, 0, " speed = %d ", i->speed);
  mvprintw(s+14, 0, "  attr = %d ", i-> attr);
  mvprintw(s+15, 0, "   val = %d ", i->  val);
  
  attroff(COLOR_PAIR(color));
  
  getch();
  
  
  
  return select;
}


int UI::iListSelect(int select)
{
	dungeon->printDungeon();

	char buf[50];

	mvprintw(0, 1, "Listing Inventory "
			"(press ESC or Q to quit)");

	if (inspect && select>=0 && select<(int)pc->inventory->slots.size()) {
		dungeon->printDungeon();

		int color = COLOR_GREEN;	
		attron(COLOR_PAIR(color));

		mvprintw(1, 20, "  use I to toggle off inspect ");
		int s = pc->inventory->slots.size();
		Item *item = pc->inventory->slots[select];

		mvprintw(s+2, 0, "%s ", item->name.c_str());
		mvprintw(s+3, 0, "%s ", item->desc.c_str());
		attroff(COLOR_PAIR(color));
	} else {	
		int color = COLOR_GREEN;	
		attron(COLOR_PAIR(color));

		mvprintw(1, 20, "  use 0-9 or UP/DOWN keys to select ");
		mvprintw(2, 20, "  use D to drop ");
		mvprintw(3, 20, "  use I to inspect ");
		mvprintw(4, 20, "  use W to wear ");
		mvprintw(5, 20, "  use X to expunge ");

		if (select>=0 && select<(int)pc->inventory->slots.size()) {		
			int s = 12;
			Item *item = pc->inventory->slots[select];

			mvprintw(s+2, 0, "%s ", item->name.c_str());
			mvprintw(s+3, 0, "   hit = %d ", item->hit);
			mvprintw(s+4, 0, "   dam = %d-%d ", 
					item->dam->min(), item->dam->max());
			mvprintw(s+5, 0, " dodge = %d ", item->dodge);
			mvprintw(s+6, 0, "   def = %d ", item->  def);
			mvprintw(s+7, 0, " speed = %d ", item->speed);
			mvprintw(s+8, 0, "  attr = %d ", item-> attr);
			mvprintw(s+9, 0, "   val = %d ", item->  val);
		}
		attroff(COLOR_PAIR(color));
	}

	for (int i=0; i<(int)pc->inventory->slots.size(); i++) {
		Item *item = pc->inventory->slots[i];

		sprintf(buf, "%d %s %s  %-11s", i, 
				select == i ? "[" : " ",
				select == i ? "]" : " ",
				ObjectFactory::TYPE[item->type].c_str());

		mvprintw(i+1, 2, "%s", buf);

		int color = item->color;		
		attron(COLOR_PAIR(color));
		mvprintw(i+1, 5, "%c", item->symb);
		attroff(COLOR_PAIR(color));

		mvaddch(i+1, 19, ACS_VLINE);
	}

	for (int c=1; c<19; c++)
		mvaddch(pc->inventory->slots.size()+1, c, ACS_HLINE);
	mvaddch(pc->inventory->slots.size()+1, 19, ACS_LRCORNER); 

	return select;
}

static void mListFrom(int start)
{
	int i;
	char buf[50];

	int nummon = dungeon->nummon();

	mvprintw(0, 1, "Listing Monster %02d to %02d out of %02d "
			"(press ESC or Q to quit)", 
			start+1, MIN((start+21), nummon), nummon);

	std::sort(dungeon->npcv.begin(), dungeon->npcv.end(), compareNPC);

	for (i=start; i<MIN((start+21), nummon); i++) {
		Character *c = dungeon->npcv[i];
		getPositionStr(buf, c);

		mvprintw(i-start+1, 2, "  %s", buf);

		mvaddch(i-start+1, 18, ACS_VLINE);

		int color = c->getColor();

		attron(COLOR_PAIR(color));
		mvprintw(i-start+1, 2, "%c", c->getSymb());
		attroff(COLOR_PAIR(color));

	}
	int r = MIN((start+21), nummon)-start+1;
	for (int c=1; c<18; c++) {
		mvaddch(r, c, ACS_HLINE);
	}
	mvaddch(r, 18, ACS_LRCORNER);
}

static void oListFrom(int start)
{
	int i;
	char buf[50];

	int numobj = dungeon->numobj();

	mvprintw(0, 1, "Listing Object %d to %d out of %d "
			"(press ESC or Q to quit)", 
			start+1, MIN((start+21), numobj), numobj);

	for (i=start; i<MIN((start+21), numobj); i++) {
		Item *item = dungeon->itemv[i];

		sprintf(buf, "     %-12s", 
				ObjectFactory::TYPE[item->type].c_str());

		mvprintw(i-start+1, 1, "   %s", buf);

		int color = item->color;		
		attron(COLOR_PAIR(color));
		mvprintw(i-start+1, 2, "%c", item->getSymb());
		attroff(COLOR_PAIR(color));

		mvaddch(i-start+1, 18, ACS_VLINE);
	}
	int r = MIN((start+21), numobj);
	for (int c=1; c<18; c++)
		mvaddch(r, c, ACS_HLINE);
	mvaddch(r, 18, ACS_LRCORNER);
}

int UI::setInspect(bool inspect)
{
	UI::inspect = inspect;

	return 0;
}

int UI::printEquipmentSlots()
{
	mvprintw(23, 44, "Equipment");

	int col=55;
	for (int i=0; i<12; i++) {
		Item *item = pc->equipment->slots[i];

		mvprintw(22, col+i*2, "%c", (char) 'A'+i);

		mvprintw(23, col+i*2-1, "'");

		if (item) {
			int color = item->color;
			attron(COLOR_PAIR(color));
			mvprintw(23, col+i*2, "%c", item->symb);
			attroff(COLOR_PAIR(color));
		} else {
			mvprintw(23, col+i*2, " ");
		}
	}
	mvprintw(23, 78, "'");

	return 0;
}

int UI::printHP()
{
	int colored = int(16 * pc->hp/(double)pc->hpmax()+0.5);

	mvprintw(23, 1, "HP ""                ");

	char buf[50];
	sprintf(buf, "%d/%d""                ", pc->hp, pc->hpmax());
	buf[16] = 0;

	char coloredsubstr[colored + 1];
	memset(coloredsubstr, 0, sizeof(coloredsubstr));
	strncpy(coloredsubstr, buf, colored);

	attron(COLOR_PAIR(COLOR_HP));
	mvprintw(23, 4, "%s", coloredsubstr);
	attroff(COLOR_PAIR(COLOR_HP));

	if (colored < 16)
		mvprintw(23, 4 + colored, "%s", buf + colored);

	return 0;
}

int UI::printMP()
{
	int colored = int(16 * pc->mp/(double)pc->mpmax()+0.5);

	mvprintw(23, 21, "MP ""                ");

	char buf[50];
	sprintf(buf, "%d/%d""                ", pc->mp, pc->mpmax());
	buf[16] = 0;

	char coloredsubstr[colored + 1];
	memset(coloredsubstr, 0, sizeof(coloredsubstr));
	strncpy(coloredsubstr, buf, colored);

	attron(COLOR_PAIR(COLOR_MP));
	mvprintw(23, 24, "%s", coloredsubstr);
	attroff(COLOR_PAIR(COLOR_MP));

	if (colored < 16)
		mvprintw(23, 24 + colored, "%s", buf + colored);

	return 0;
}

int UI::printMonsterHP()
{
	printMonsterHP(pc->attacking);

	return 0;
}

int UI::printMonsterHP(NPC *att)
{
	if (!att) {
		clearRow(0);
		return -1;
	}

	int colored = int(16 * att->hp/(double)(att->hpmax())+0.5);

	mvprintw(0, 1, "HP ""                ");

	char buf[50];
	sprintf(buf, "%d/%d""                ", att->hp, att->hpmax());
	buf[16] = 0;

	char coloredsubstr[colored + 1];
	memset(coloredsubstr, 0, sizeof(coloredsubstr));
	strncpy(coloredsubstr, buf, colored);

	attron(COLOR_PAIR(COLOR_HP));
	mvprintw(0, 4, "%s", coloredsubstr);
	attroff(COLOR_PAIR(COLOR_HP));

	if (colored < 16)
		mvprintw(0, 4 + colored, "%s", buf + colored);

	// monster info	
	mvprintw(0, 20, " <- ");

	attron(COLOR_PAIR(att->getColor()));
	mvprintw(0, 24, "%c", att->getSymb());
	attroff(COLOR_PAIR(att->getColor()));

	mvprintw(0, 26, "targeted");

	return 0;
}

int UI::clearRow(int row)
{
	int i;
	for(i=0; i<80; i++)
		mvaddch(row, i, ' ');

	return 0;
}

int UI::reprint()
{
	dungeon->printDungeon();

	printHP();
	printMP();

	printEquipmentSlots();

	return 0;
}

int UI::eList()
{
	refresh();

	int select = 0;

	// default select a non-empty slot
	for (int i=0; i<12; i++) {
		if (pc->equipment->slots[i]) {
			select = i;
			break;
		}
	}

	eListSelect(select);

	while (1) {
		int ch = getch();

		int quit = 0;

		int index = -1;
		if (ch>='A' && ch<='L') index = ch-'A';
		if (ch>='a' && ch<='l') index = ch-'a';
		if (ch>='0' && ch<='9') index = ch-'0';

		if (index >= 0) {
			eListSelect(select = index);
			continue;
		}

		switch (ch) {
			case 'Q':
			case 'q':
			case 27: // ESC
				quit = 1;
				break;
			case KEY_DOWN:
				select = (select + 1) % 12; 
				eListSelect(select);
				refresh();
				break;
			case KEY_UP:
				select = (select - 1 + 12) % 12;
				eListSelect(select);
				refresh();
				break;
			case 'T':
			case 't':
				pc->takeOffItem(select);
				eListSelect(select);
				printEquipmentSlots();
				refresh();
				break;
			default:
				if (pc->equipment->isEmpty())
					quit = 1;
				break;
		}
		if (quit)
			break;
	}

	clearRow(0);
	clearRow(22);

	return 0;
}

int UI::iList()
{
	refresh();

	int select = 0;

	iListSelect(select);

	while (1) {
		int ch = getch();

		int quit = 0;

		switch(ch) {
			case 'Q':
			case 'q':
			case 27: // ESC
				quit = 1;
				break;
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
			case '0':
				if (ch-'0' >= (int)pc->inventory->slots.size())
					break;
				else
					select = ch-'0';
				iListSelect(select);
				refresh();
				break;
			case KEY_DOWN:
				if (pc->inventory->slots.empty()) {
					break;
				}
				select = (select + 1) % 
					pc->inventory->slots.size();
				iListSelect(select);
				refresh();
				break;
			case KEY_UP:
				if (pc->inventory->slots.empty()) {
					break; 
				}
				select = (select - 1 + pc->inventory->slots.size()) % 
					pc->inventory->slots.size();
				iListSelect(select);
				refresh();
				break;
			case 'D':
			case 'd':
				if (pc->inventory->slots.empty()) {
					break;
				}
				pc->dropItem(select);
				reprint();
				if (select>=(int)pc->inventory->slots.size())
					iListSelect(--select);
				else
					iListSelect(select);
				refresh();
				break;
			case 'I':
			case 'i':
				if (pc->inventory->slots.empty()) {
					break;
				}
				reprint();
				setInspect(!inspect);
				iListSelect(select);
				refresh();
				break;
			case 'X':
			case 'x':
				if (pc->inventory->slots.empty()) {
					break;
				}
				pc->inventory->expunge(select);	
				reprint();
				if (select>=(int)pc->inventory->slots.size())
					iListSelect(--select);
				else
					iListSelect(select);
				refresh();
				break;
			case 'W':
			case 'w':
				if (pc->inventory->slots.empty()) {
					break;
				}
				pc->wearItem(select);
				reprint();
				if (select>=(int)pc->inventory->slots.size())
					iListSelect(--select);
				else
					iListSelect(select);
				refresh();
				break;
			case 'C':
			case 'c':
				sList();
				reprint();
				iListSelect(select);
				break;
			default:
				if (pc->inventory->slots.empty())
					//	quit = 1;
					break;
		}
		if (quit)
			break;
	}

	int i;
	for(i=0; i<80; i++)
	{
		mvprintw(0,  i, " ");
		mvprintw(22, i, " ");
	}

	return 0;
}

int UI::mList()
{
	int from = 0;

	mListFrom(from);
	refresh();

	while (1) {
		int ch = getch();

		int quit = 0;

		switch (ch) {
			case 'Q':
			case 'q':
			case 27: // ESC
				quit = 1;
				break;
			case KEY_DOWN:
				if (from>21-nummon && from<nummon-21) from++;
				mListFrom(from);
				refresh();
				break;
			case KEY_UP:
				from = MAX((from-1), 0);
				mListFrom(from);
				refresh();
				break;
		}
		if (quit)
			break;
	}

	for (int i=0; i<80; i++) {
		mvprintw(0,  i, " ");
		mvprintw(22, i, " ");
	}

	return 0;
}

int UI::oList()
{
	int from = 0;

	oListFrom(from);
	refresh();

	while (1) {
		int ch = getch();

		int quit = 0;

		switch (ch) {
			case 'Q':
			case 'q':
			case 27: // ESC
				quit = 1;
				break;
			case KEY_DOWN:
				if (from>21-nummon && from<nummon-21) from++;
				oListFrom(from);
				refresh();
				break;
			case KEY_UP:
				from = MAX((from-1), 0);
				oListFrom(from);
				refresh();
				break;
		}
		if (quit)
			break;
	}

	clearRow(0);
	clearRow(22);

	return 0;
}

/** get description of selected item in room */
int UI::cellDescriptionItems()
{
  //mvprintw(21, 2, " In cellDescriptionItems");
  //getch();
  int x, y;
  //mvprintw(20, 2, "Calling selectTargetItems");
  //getch();
  vector<Item *> itemsInRoom;
  int target = selectTargetItems(itemsInRoom);
  //mvprintw(19, 2, "selectTargetItems called");
  //getch();
  
  Item *i;
  if (target < 0)
				return 0;
  
  //get npc from vector of npcs
  i = itemsInRoom[target];
  
  //get location of target npc
  i->getLocation(&x, &y);
  
  dungeon->printDungeon();
  printMP();
  UI::printHP();
  refresh();
  return 0;
}


/** get description of selected monster in room */
int UI::cellDescriptionMonsters()
{
  
  int npcx, npcy;
  int target = selectTargetMonsters();
  NPC *att;
  if (target < 0)
				return 0;
  
  //get npc from vector of npcs
  att = dungeon->vnpcv[target];
  
  //get location of target npc
  att->getLocation(&npcx, &npcy);
  
  
  
  dungeon->printDungeon();
  printMP();
  UI::printHP();
  refresh();
  return 0;
}

/** ranged combat list */
int UI::rangedCombat()
{
  if (pc->equipment->slots[2] == NULL) {
    return 0;
  }
  int npcx, npcy;
  int target = selectTarget();
  NPC *att;
  if (target < 0)
				return 0;
  
  //get npc from vector of npcs
  att = dungeon->vnpcv[target];
  
  //get location of target npc
  att->getLocation(&npcx, &npcy);
  
  // update what PC attacking
  pc->attacking = att;
  
  if (att) {
    // attack
      // net damage = total ranged damage - total defense
      int dam = pc->getTotalDamRanged() - att->getTotalDef();
      
      if (dam < 0) dam = 0;
      
      if (att->hp > dam) {
        // failed to kill
        att->hp -= dam;
        return 0;
      } else {
        // kill successfully
        att->hp = 0;
        att->setDead();
        dungeon->cmap[npcy][npcx] = NULL;
      }
      UI::printHP();
    }
  
  
  
  dungeon->printDungeon();
  printMP();
  refresh();
  return 0;
}



int UI::sList()
{
	int color = COLOR_CYAN;	
	attron(COLOR_PAIR(color));

	mvprintw(17, 21, "B: Blast nearby area   cost: 30  ");
	mvprintw(18, 21, "F: Frozen ball         cost: 20  ");
	mvprintw(19, 21, "H: Heal PC             cost: 10  ");
	mvprintw(20, 21, "P: Poison ball         cost: 20  ");
	mvprintw(21, 21, "T: Teleport            cost: 30  ");

	attroff(COLOR_PAIR(color));

	int ch = getch(), cost, target, npcx, npcy;

	NPC *att;

	switch (ch) {
		case 'Q':
		case 'q':
		case 27: // ESC
			break;
		case 'B':
		case 'b':
			if (pc->mp < (cost = 30))
				break;
			for (int r=pcy-1; r<=pcy+1; r++) {
				for (int c=pcx-1; c<=pcx+1; c++) {
					if (r>=0 && r<21 &&
							c>=0 && c<80 && 
							dungeon->hmap[r][c]!=IMMUTABLE) {
						if (dungeon->hmap[r][c]>0) {
							dungeon->hmap[r][c]=0;
							dungeon->tmap[r][c]=CORR;
						}
						if (dungeon->cmap[r][c]&&!
								dungeon->cmap[r][c]->isPC())
							dungeon->cmap[r][c]->hp/=10;
					}
				}
			}
			pc->mp -= cost;
			pc->setLocation(pcx, pcy); // force update seen dungeon
			dungeon->printDungeon();
			refresh();
			break;
		case 'F':
		case 'f':
			if (pc->mp < (cost = 20))
				break;
			target = selectTarget();
			if (target < 0)
				break;
			att = dungeon->vnpcv[target];

			att->getLocation(&npcx, &npcy);

			// frozen ball spell
			for (int r=npcy-1; r<=npcy+1; r++) {
				for (int c=npcx-1; c<=npcx+1; c++) {
					if (r>=0 && r<21 &&
							c>=0 && c<80 && 
							dungeon->hmap[r][c]!=IMMUTABLE) {
						if (dungeon->cmap[r][c]&&!
								dungeon->cmap[r][c]->isPC()) {
							dungeon->cmap[r][c]->frozen = 25;
						}
					}
				}
			}
			pc->mp -= cost;
			pc->setLocation(pcx, pcy); // force update seen dungeon
			dungeon->printDungeon();	
			UI::printMP();
			refresh();
		case 'H':
		case 'h':
			if (pc->mp < (cost = 10))
				break;
			pc->hp += pc->hpmax() * 0.25;
			if (pc->hp > pc->hpmax())
				pc->hp = pc->hpmax();
			pc->mp -= cost;
			UI::printHP();
			UI::printMP();
			break;
		case 'P':
		case 'p':
			if (pc->mp < (cost = 20))
				break;
			target = selectTarget();
			if (target < 0)
				break;
			att = dungeon->vnpcv[target];

			att->getLocation(&npcx, &npcy);

			// poison ball spell
			for (int r=npcy-1; r<=npcy+1; r++) {
				for (int c=npcx-1; c<=npcx+1; c++) {
					if (r>=0 && r<21 &&
							c>=0 && c<80 && 
							dungeon->hmap[r][c]!=IMMUTABLE) {
						if (dungeon->cmap[r][c]&&!
								dungeon->cmap[r][c]->isPC()) {
							dungeon->cmap[r][c]->hp*=0.9;
							dungeon->cmap[r][c]->poison = 25;
						}
					}
				}
			}
			pc->mp -= cost;
			pc->setLocation(pcx, pcy); // force update seen dungeon
			dungeon->printDungeon();	
			UI::printMP();
			refresh();
			break;
		case 'T':
		case 't':
			if (pc->mp < (cost = 30))
				break;
			while (1) {
				int r = rand() % 21;
				int c = rand() % 80;
				if (!dungeon->hmap[r][c] && !dungeon->cmap[r][c]) {
					Move::move(pc, c, r);
					break;
				}
			}
			pc->mp -= cost;
			UI::printMP();
			break;
	}

	return 0;
}


int UI::selectTargetItems(vector<Item *>  &itemsInRoom)
{
  //if there are no items in the dungeon do nothing
  if (dungeon->itemv.empty())
    return -1;
  
  int index = 0;
  int roomIndex = 0;
  
  unsigned int i = 0;
  
  //mvprintw(18, 2, "entering for loop to get the room pc is in");
  //getch();
  //get the room the pc is in
  for (i = 0; i < dungeon->roomv.size(); i++) {
    if (dungeon->roomv[i]->contains(pc->getX(), pc->getY())) {
      roomIndex = i;
      break;
    }
  }
  
  Room* r = dungeon->roomv[roomIndex];
  int x, y = 0;
  
  //mvprintw(17, 2, "entering for loop to get items in same room");
  //getch();
  //get the items in the same room as pc
  for (i = 0; i < dungeon->itemv.size(); i++) {
    dungeon->itemv[i]->getLocation(&x, &y);
    if (r->contains(x, y)) {
      itemsInRoom.push_back(dungeon->itemv[i]);
    }
  }
  
  
  //first go through items
  selectItem(itemsInRoom[index]);
  
  while (1) {
    bool quit = false;
    
    int ch = getch();
    
    switch (ch) {
      case 'Q':
      case 'q':
        quit = true;
        break;
      case ' ':
        iListSelectForDescription(index);
        return index;
      case KEY_DOWN:
      case KEY_RIGHT:
      case 'R':
      case 'r':
      case 'F':
      case 'f':
      case 'P':
      case 'p':
        index = (index + 1)
        % (int)itemsInRoom.size();
        selectItem(itemsInRoom[index]);
        break;
      case KEY_UP:
      case KEY_LEFT:
        index = (index - 1 + (int)itemsInRoom.size())
        % (int)itemsInRoom.size();
        selectItem(itemsInRoom[index]);
        break;
      default:
        break;
    }
    
    if (quit)
      break;
  }
  reprint();
  
  return -1;
}

int UI::selectTargetMonsters()
{
  //if there are no items or monsters in the dungeon do nothing
  if (dungeon->vnpcv.empty())
    return -1;
  
  int index = 0;
  
  //first go through monsters
  selectNPC(dungeon->vnpcv[index]);
  
  while (1) {
    bool quit = false;
    
    int ch = getch();
    
    switch (ch) {
      case 'Q':
      case 'q':
        quit = true;
        break;
      case ' ':
        mListSelectForDescription(index);
        return index;
      case KEY_DOWN:
      case KEY_RIGHT:
      case 'R':
      case 'r':
      case 'F':
      case 'f':
      case 'P':
      case 'p':
        index = (index + 1)
        % (int)dungeon->vnpcv.size();
        selectNPC(dungeon->vnpcv[index]);
        break;
      case KEY_UP:
      case KEY_LEFT:
        index = (index - 1 + (int)dungeon->vnpcv.size())
        % (int)dungeon->vnpcv.size();
        selectNPC(dungeon->vnpcv[index]);
        break;
      default:
        break;
    }
    
    if (quit)
      break;
  }
  reprint();
  
  return -1;
}


int UI::selectItem(Item* i)
{
  reprint();
  
   mvprintw(1, 20, "  select item ");
  
  int x, y;
  
  i->getLocation(&x, &y);
  if ((x < 0) | (y < 0))
    return -1;
  
  mvprintw(y+1, x-1, "[");
  mvprintw(y+1, x+1, "]");
  refresh();
  
  return 0;
}

int UI::selectNPC(NPC *npc)
{
	reprint();

	// mvprintw(1, 20, "  select monster ");

	int x, y;
	npc->getLocation(&x, &y);

	mvprintw(y+1, x-1, "[");
	mvprintw(y+1, x+1, "]");

	printMonsterHP(npc);

	refresh();

	return 0;
}

int UI::selectTarget()
{	
	if (dungeon->vnpcv.empty())
		return -1;

	int index = 0;

	selectNPC(dungeon->vnpcv[index]);

	while (1) {
		bool quit = false;

		int ch = getch();

		switch (ch) {
			case 'Q':
			case 'q':
				quit = true;
				break;
			case ' ':
				pc->attacking = dungeon->vnpcv[index];
				return index;
			case KEY_DOWN:
			case KEY_RIGHT:
			case 'R':
			case 'r':
			case 'F':
			case 'f':
			case 'P':
			case 'p':
				index = (index + 1) 
					% (int)dungeon->vnpcv.size();
				selectNPC(dungeon->vnpcv[index]);
				break;
			case KEY_UP:
			case KEY_LEFT:
				index = (index - 1 + (int)dungeon->vnpcv.size()) 
					% (int)dungeon->vnpcv.size();
				selectNPC(dungeon->vnpcv[index]);
				break;
			default:
				break;
		}

		if (quit)
			break;
	}
	reprint();

	return -1;
}

