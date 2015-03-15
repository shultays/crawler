#include<stdio.h>
#include<stdlib.h>
#include"Tools.h"
#include"Maze.h"
#include"GameWindow.h"
#include"Creature.h"
#include"CreatureGen.h"
#include"MazeActions.h"
#include<time.h>

unsigned seed;

float globalTick = 0;
float realTick = 0;
WINDOW * win;
float lastAPUpdateTick = 0.0f;
Maze maze;
bool tickGame;
bool gotoNextLevel = false;
float controlEndTime = 350.0f;
int currentLevel = 1;
int scrollVal = 0;
int maxScroll = 0;
int messageScrollVal = 0;
Pos scrolledPos = Pos(-1, -1);;

int nextCreatureIndex = 0;
int skillFocus = 0, maxFocus = 0;
int focusedItem = -1, focusedSkill = -1;
int messagePos;
char messages[MAX_MESSAGE][128];
bool cheat = false;

vector<Creature*> creatures;
vector<DroppedEquipment> droppedEquipments;

int actionPoint = 100;

Pos cursor;
GameWindow<MAZE_W, MAZE_H> mazeWindow;
int globalVisible[MAZE_W][MAZE_H];
int gIndex = 0;
float lastGChangeTick = -1;


int createSingeCreatureP = 30;
int createCreatureGroupP = 40;
int takeControlP = 10;


enum {
	MENU_MAIN,
	MENU_CONTROL_CREATURE,
	MENU_SINGLE_MOVE,
	MENU_USE_SKILL
};

Creature *creatureToControl, *creatureToShow;

int menu = MENU_MAIN;

void resetGame(bool keepAdventurer = false) {
	printArt(2, 22);


	if (!keepAdventurer) {
		globalTick = realTick = 0.0f;
		currentLevel = 1;
	}
	actionPoint += 50;
	lastAPUpdateTick = globalTick;
	seed = (unsigned)time(0);
	printw("%u ran\n", seed);
	srand(seed);
	memset(globalVisible, 0, sizeof(globalVisible));
	maze.window = &mazeWindow;
	maze.genRand(3);
	for (unsigned i = 0; i < droppedEquipments.size(); i++) {
		delete droppedEquipments[i].e;
	}
	droppedEquipments.clear();

	for (unsigned i = keepAdventurer ? 1 : 0; i < creatures.size(); i++) {
		delete creatures[i];
	}
	nextCreatureIndex = 0;
	if (keepAdventurer) {
		creatures.resize(1);
	} else {
		creatures.clear();
		Creature* adventurer = new Creature();
		adventurer->explores = true;
		adventurer->wandersAround = false;
		adventurer->type = ADVENTURER;
		adventurer->pixel.character = '@';
		adventurer->pixel.color = getColorIndex(7, 0, 0);
		adventurer->movePerTick = 5.0f;
		adventurer->sight = 5.0f;
		adventurer->hpMax = adventurer->hp = 300;
		adventurer->mpMax = adventurer->mp = 150;
		adventurer->level = 1;
		adventurer->DR = 2;
		adventurer->loots = true;
		adventurer->consumes = true;
		strcpy_s(adventurer->name, "Adventurer");
		adventurer->tickToRegen = 100.0f;
		adventurer->levelsUp = true;
		adventurer->skillToLearn.push_back(SkillTree(new BerserkSkill(), 1));
		adventurer->skillToLearn.push_back(SkillTree(new StoneSkinSkill(), 3));
		adventurer->skillToLearn.push_back(SkillTree(new HasteSkill(), 5));
		char *spellName;
		char *damageType;
		int color;
		int t = ran(3);
		if (t == 0) {
			spellName = "Ench. Weapon (Fire)";
			damageType = "Fire";
			color = getColorIndex(7, 0, 0);
			adventurer->skillToLearn.push_back(SkillTree(new FireBallSkill(), 2));
		} else if (t == 1) {
			spellName = "Ench. Weapon (Ice)";
			damageType = "Ice";
			color = getColorIndex(0, 7, 7);
			adventurer->skillToLearn.push_back(SkillTree(new IceBoltSkill(), 2));
		} else {
			spellName = "Ench. Weapon (Lighting)";
			damageType = "Lighting";
			color = getColorIndex(7, 7, 0);
			adventurer->skillToLearn.push_back(SkillTree(new LightingSkill(), 2));
		}

		adventurer->skillToLearn.push_back(SkillTree(new ElementalWeaponSkill(spellName, damageType, color), 4));
		adventurer->skillToLearn.push_back(SkillTree(new BlurSkill(), 7));
		adventurer->reset(Pos(-1, -1));

		adventurer->equip(getWeapon(5, true));
		adventurer->equip(getArmor(4, true));
		adventurer->equip(getHelm(2, true));
		adventurer->equip(getShield(2));
		adventurer->equip(getBoots(1));
		adventurer->equip(getGloves(1));
		adventurer->equip(getRing(5));
		adventurer->equip(getRing(5));
		adventurer->equip(getAmulet(1));

		adventurer->equip(getConsumable(1, HEAL_POT));
		adventurer->equip(getConsumable(1, HEAL_POT));
		adventurer->equip(getConsumable(1, HEAL_POT));
		adventurer->equip(getConsumable(1, REGEN_POT));
		adventurer->equip(getConsumable(1, MANA_POT));
		adventurer->equip(getConsumable(1, MANA_POT));
		adventurer->equip(getConsumable(1, CHARM_SCROLL));
		adventurer->equip(getConsumable(1, BUTTERFLY_SCROLL));
		adventurer->equip(getConsumable(1, LIGHTING_SCROLL + ran(2)));

		creatures.push_back(adventurer);
	}


	creatures[0]->resetPos(maze.upstairs);


	if (!keepAdventurer) {
		messagePos = 0;
		for (int i = 0; i < MAX_MESSAGE; i++) {
			messages[i][0] = '\0';
		}
		messageCount = 0;
		pushMessage("Wild Adventurer appeared!");
	} else {
		sprintf_s(buff, "Adventurer is now on floor %d\n", currentLevel);
		pushMessage(buff);
	}

	for (int i = 0; i < 8; i++) {
		int t = 10;
		while (true) {
			Pos p = maze.emptyPos[ran(maze.emptyPos.size())];
			vector<Creature*> group;
			if (generateCreatureGroup(p, group, currentLevel, GOBLIN)) {
				for (unsigned i = 0; i < group.size(); i++) {
					group[i]->reset(group[i]->pos);
					creatures.push_back(group[i]);
				}
				break;
			}
			t--;
			if (t <= 0) break;
		}
	}

	cursor = creatures[0]->pos;
	tickGame = false;
}

char *cheatText = "cheat", *c = cheatText;

bool control(int i) {
	bool hasChange = false;
	bool moved = false;
	Pos newP;

	if (*c == i) {
		c++;
		if (*c == 0) {
			cheat = !cheat;
			if (cheat) {
				sprintf_s(buff, "Cheats are enabled.");
				pushMessage(buff);
				sprintf_s(buff, "Adventurer can't die.");
				pushMessage(buff);
				sprintf_s(buff, "Creatures loot items / use consumables.");
				pushMessage(buff);
				sprintf_s(buff, "F1-F8 to generate weapon.");
				pushMessage(buff);
				sprintf_s(buff, "Q to go next level.");
				pushMessage(buff);
			} else {
				sprintf_s(buff, "Cheats are disabled.");
				pushMessage(buff);
			}
			c = cheatText;
		}
	} else {
		c = cheatText;
	}

	if (cheat) {
		if (i == 'q') gotoNextLevel = true;

		Equipment* e = NULL;

		if (i == KEY_F(1)) e = getWeapon(currentLevel + 3, true);
		if (i == KEY_F(2)) e = getArmor(currentLevel + 3, true);
		if (i == KEY_F(3)) e = getHelm(currentLevel + 3, true);
		if (i == KEY_F(4)) e = getShield(currentLevel + 3, true);
		if (i == KEY_F(5)) e = getBoots(currentLevel + 3, true);
		if (i == KEY_F(6)) e = getGloves(currentLevel + 3, true);
		if (i == KEY_F(7)) e = getAmulet(currentLevel + 3, true);
		if (i == KEY_F(8)) e = getConsumable(currentLevel + 3, -1, true);

		if (e != NULL) {
			for (unsigned i = 0; i < creatures.size(); i++) {
				creatures[i]->mazeState[cursor.x][cursor.y] |= HAS_NEW_ITEM;
			}
			DroppedEquipment eq;
			eq.e = e;
			eq.p = cursor;
			droppedEquipments.push_back(eq);
		}
	}


	if (i == '-') {
		if (messageScrollVal < messageCount - 12) messageScrollVal++;
		return true;
	} else if (i == '+') {
		if (messageScrollVal > 0) messageScrollVal--;
		return true;
	}

	if (i == 459 || i == 10) {
		if (menu == MENU_USE_SKILL) {
			menu = MENU_SINGLE_MOVE;
		} else if (menu == MENU_SINGLE_MOVE) {
			//menu = MENU_CONTROL_CREATURE;
			menu = MENU_MAIN;
			creatureToControl->controlAction = 0;
			creatureToControl->beingControlled = FALSE;
			creatureToControl->hasTarget = false;
			creatureToControl = NULL;
			cursor = creatures[0]->pos;

		} else if (menu == MENU_CONTROL_CREATURE) {
			menu = MENU_MAIN;
			creatureToControl->controlAction = 0;
			creatureToControl->beingControlled = FALSE;
			creatureToControl->hasTarget = false;
			creatureToControl = NULL;
			cursor = creatures[0]->pos;
		}

	}

	if (menu == MENU_USE_SKILL) {
		if (i == '2' && skillFocus < maxFocus - 1) skillFocus++;
		if (i == '8' && skillFocus > 0) skillFocus--;
		if (i == '5') {
			if (dist(cursor, creatureToControl->pos) > creatureToControl->getSight()) {

				sprintf_s(buff, "That cell is outside the range.");
				pushMessage(buff);
				return true;
			}

			if (focusedSkill >= 0) {
				creatureToControl->controlAction = CONTROL_USE_SKILL;
				creatureToControl->controlUseSkillIndex = focusedSkill;
			} else if (focusedItem >= 0) {
				creatureToControl->controlAction = CONTROL_USE_CONSUMEABLE;
				creatureToControl->controlUseItemIndex = focusedItem;
			} else return true;
			creatureToControl->controlActionDone = false;
			creatureToControl->controlSkillPos = cursor;
			tickGame = true;
			menu = MENU_SINGLE_MOVE;
		}
	} else if (menu == MENU_SINGLE_MOVE && ((i >= '0' && i <= '9') || i == '+')) {
		Pos p = creatureToControl->pos;

		if (i == '0') {
			menu = MENU_USE_SKILL;
			skillFocus = 0;
			maxFocus = 1;
		} else if (i == '5') {
			creatureToControl->controlAction = CONTROL_WAIT;
			creatureToControl->controlActionDone = FALSE;
			tickGame = true;
		}

		switch (i) {
			case '1':
				p = getPosForDir(p, DOWN_LEFT);
				break;
			case '2':
				p = getPosForDir(p, DOWN);
				break;
			case '3':
				p = getPosForDir(p, DOWN_RIGHT);
				break;
			case '4':
				p = getPosForDir(p, LEFT);
				break;
			case '6':
				p = getPosForDir(p, RIGHT);
				break;
			case '7':
				p = getPosForDir(p, UP_LEFT);
				break;
			case '8':
				p = getPosForDir(p, UP);
				break;
			case '9':
				p = getPosForDir(p, UP_RIGHT);
				break;
		}

		if (maze.walls[p.x][p.y] == 0) {
			if (creatureToControl->tryToPath(p)) {
				creatureToControl->controlAction = CONTROL_MOVE_TO_TARGET;
				creatureToControl->controlActionDone = FALSE;
				tickGame = true;
			}
		} else {
			for (unsigned i = 0; i < creatures.size(); i++) {
				if (creatures[i]->pos == p && creatureToControl->isEnemy(creatures[i]->type)) {
					creatureToControl->lastAttackCreatureIndex = creatures[i]->index;
					creatureToControl->controlAction = CONTROL_ATTACK_CREATURE;
					creatureToControl->controlActionDone = FALSE;
					tickGame = true;
				}
			}
		}

	}

	switch (i) {
		case 339:
			if (scrollVal > 0)
				scrollVal--;

			break;
		case 338:
			if (scrollVal < maxScroll)
				scrollVal++;
			break;
		case 27:
			exit(0);
			break;
		case 'r':
			resetGame();
			break;

		case ' ':
			tickGame = !tickGame;

			if (creatures.size() == 0 || creatures[0]->type != ADVENTURER) {
				resetGame();
				return true;
			}

			if (creatureToControl) {
				cursor = creatureToControl->pos;

				creatureToControl->controlAction = 0;
				creatureToControl->controlActionDone = false;
			} else {
				cursor = creatures[0]->pos;
			}

			break;

		case '1':
			if (!tickGame) {
				if (menu == MENU_MAIN) {
					if (actionPoint < createSingeCreatureP && !cheat) break;
					Creature* c;
					if (c = generateCreature(cursor, currentLevel + 1, ran(4))) {
						if (!cheat)actionPoint -= createSingeCreatureP;
						c->reset(c->pos);
						creatures.push_back(c);
					} else {
						sprintf_s(buff, "Cannot create creatures in visible or blocked spots.");
						pushMessage(buff);
					}
				} else if (menu == MENU_CONTROL_CREATURE) {
					if (creatureToShow) {
						menu = MENU_SINGLE_MOVE;
					}
				}

			}
			break;
		case '2':
			if (!tickGame) {
				if (menu == MENU_MAIN) {
					if (actionPoint < createCreatureGroupP && !cheat) break;
					vector<Creature*> group;
					if (generateCreatureGroup(cursor, group, currentLevel, GOBLIN)) {
						if (!cheat)actionPoint -= createCreatureGroupP;
						for (unsigned i = 0; i < group.size(); i++) {
							group[i]->reset(group[i]->pos);
							creatures.push_back(group[i]);
						}
					} else {
						sprintf_s(buff, "Cannot create creatures in visible or blocked spots.");
						pushMessage(buff);
					}
				}
			}
			break;
		case '3':
			if (!tickGame) {
				if (menu == MENU_MAIN) {
					if (actionPoint < takeControlP && !cheat) break;

					if (creatureToShow) {
						if (creatureToShow->type == ADVENTURER || creatureToShow->type == ADVENTURER_ALLY) {
							if (!cheat) {
								sprintf_s(buff, "Can't take control of Adventurer of its allies.");
								pushMessage(buff);
								return true;
							}
						}

						if (!cheat)actionPoint -= takeControlP;
						controlEndTime = globalTick + 300.0f;
						creatureToControl = creatureToShow;
						creatureToControl->beingControlled = true;
						creatureToControl->controlAction = CONTROL_WAIT;
						creatureToControl->controlActionDone = true;
						menu = MENU_SINGLE_MOVE;
					} else {
						sprintf_s(buff, "Move cursor on a creature to control it.");
						pushMessage(buff);
					}
				}
			}
			break;
		case KEY_UP:
			newP = getPosForDir(cursor, UP);
			moved = true;
			break;
		case KEY_DOWN:
			newP = getPosForDir(cursor, DOWN);
			moved = true;
			break;
		case KEY_LEFT:
			newP = getPosForDir(cursor, LEFT);
			moved = true;
			break;
		case KEY_RIGHT:
			newP = getPosForDir(cursor, RIGHT);
			moved = true;
			break;

	}

	if (moved) {
		if (maze.isPosValid3(newP)) {
			cursor = newP;
			hasChange = true;
		}
	}
	return hasChange;
}

void printEquipment(Creature *c, Equipment* e, int &x, int &y, bool addLast) {
	attrset(COLOR_PAIR(getColorIndex(4, 4, 4)));
	int count = 0;
	if (e->slot == CONSUMABLE) {
		count = ((Consumable*)e)->count;
	}
	int ench = e->enchantCount;
	if (count <= 1) {
		if (ench == 0)
			mvprintw(x++, y, "%s", e->name);
		else
			mvprintw(x++, y, "+%d %s", ench, e->name);
	} else {
		mvprintw(x++, y, "%s x%d", e->name, count);
	}
	if (e->slot == WEAPON) {
		Weapon *w = (Weapon*)e;
		mvprintw(x++, y, "DMG : %d - %d", w->minDamage, w->maxDamage);
		mvprintw(x++, y, "ATK SPD : %d", (int)w->swingTime);
		mvprintw(x++, y, "RNG : %d", w->range);
	}
	attrset(COLOR_PAIR(getColorIndex(4, 4, 4)));

	if (e->buffGroup) {
		e->buffGroup->printStats(c, x, y);
	}
	if (e->slot != CONSUMABLE || addLast) x++;
}

void gameRefresh() {
	erase();
	if (actionPoint > 100) actionPoint = 100;
	int row, col;
	getmaxyx(stdscr, row, col);
	mazeWindow.windowSize.x = row - 15;
	mazeWindow.windowSize.y = col - 45;

	for (int i = 0; i < MAZE_W; i++) {
		for (int j = 0; j < MAZE_H; j++) {
			if (maze.walls[i][j] == 1) {
				mazeWindow.data[i][j].character = ACS_CKBOARD;
			} else {
				mazeWindow.data[i][j].character = ' ';
			}
			mazeWindow.data[i][j].color = getColorIndex(7, 7, 7);

			if (creatures[0]->mazeState[i][j] & IS_VISIBLE) {
				mazeWindow.data[i][j].color = getColorIndex(7, 7, 7);
			} else {

				if (creatures[0]->mazeState[i][j] & IS_VISIBLE_BEFORE) {
					mazeWindow.data[i][j].color = getColorIndex(4, 4, 4);
				} else {
					mazeWindow.data[i][j].color = getColorIndex(2, 2, 2);
					if (maze.walls[i][j] == 0) {
						mazeWindow.data[i][j].character = ACS_BULLET;
					}
				}
			}
		}
	}

	mazeWindow.data[creatures[0]->pos.x][creatures[0]->pos.y] = creatures[0]->pixel;


	for (unsigned i = 0; i < creatures.size(); i++) {
		mazeWindow.data[creatures[i]->pos.x][creatures[i]->pos.y] = creatures[i]->pixel;
	}
	bool isEnded = creatures.size() == 0 || creatures[0]->type != ADVENTURER;


	creatureToShow = NULL;
	if (tickGame) {
		if (creatureToControl) {
			creatureToShow = creatureToControl;
		} else if (!isEnded) {
			creatureToShow = creatures[0];
		} else {
			if (gIndex == -1 || gIndex >= (int)creatures.size() || lastGChangeTick < realTick - 2000) {
				gIndex = ran(creatures.size());
				lastGChangeTick = realTick;
			}
			creatureToShow = creatures[gIndex];
		}
	} else {
		for (unsigned i = 0; i < creatures.size(); i++) {
			if (creatures[i]->pos == cursor) {
				creatureToShow = creatures[i];
			}
		}
	}

	for (unsigned i = 0; i < droppedEquipments.size(); i++) {
		mazeWindow.data[droppedEquipments[i].p.x][droppedEquipments[i].p.y].character = eqipmentIcons[droppedEquipments[i].e->slot];
		int t = droppedEquipments[i].e->goodness(creatures[0]);
		if (t < 0) {
			mazeWindow.data[droppedEquipments[i].p.x][droppedEquipments[i].p.y].color = getColorIndex(7, 0, 0);
		} else {
			t = 2 + ((t - 10) / 10);
			if (t<2) t = 2;
			if (t>6) t = 6;
			mazeWindow.data[droppedEquipments[i].p.x][droppedEquipments[i].p.y].color = getColorIndex(t, t, t);
		}
	}

	if (currentLevel != 1) {
		mazeWindow.data[maze.upstairs.x][maze.upstairs.y].character = '<';
	}
	mazeWindow.data[maze.downstairs.x][maze.downstairs.y].character = '>';
	int initX;
	int x = mazeWindow.windowPos.x;
	int y = mazeWindow.windowPos.y + mazeWindow.windowSize.y + 1;

	if (scrolledPos != cursor && !tickGame) {
		scrolledPos = cursor;
		scrollVal = 0;
	}

	x -= scrollVal;
	initX = x;
	if (creatureToShow) {
		attrset(COLOR_PAIR(creatureToShow->pixel.color));
		mvprintw(x++, y, "LVL %d %s", creatureToShow->level, creatureToShow->name);
		x++;

		attrset(COLOR_PAIR(getColorIndex(7, 0, 0)));
		mvprintw(x++, y, "HP : %d / %d", creatureToShow->hp, creatureToShow->hpMax);
		if (creatureToShow->mpMax > 0) {
			attrset(COLOR_PAIR(getColorIndex(0, 0, 7)));
			mvprintw(x++, y, "MP : %d / %d", creatureToShow->mp, creatureToShow->mpMax);
		}

		if (creatureToShow->levelsUp) {
			attrset(COLOR_PAIR(getColorIndex(7, 7, 0)));
			mvprintw(x++, y, "EXP : %d / %d", creatureToShow->exp, creatureToShow->expToNextLevel);
		} else {
			attrset(COLOR_PAIR(getColorIndex(7, 7, 0)));
			mvprintw(x++, y, "EXP : %d - %d", (int)creatureToShow->minExp, (int)creatureToShow->maxExp);
		}
		x++;
		attrset(COLOR_PAIR(getColorIndex(4, 4, 4)));
		if (creatureToShow->chanceToHit < 1000) {
			mvprintw(x++, y, "EVSN/BLK : %%%d", 100 - (int)creatureToShow->chanceToHit / 10);
		}
		if (creatureToShow->hpRegenMult != 1000) {
			mvprintw(x++, y, "%HP RGN : %%%d", (int)creatureToShow->hpRegenMult / 10);
		}
		if (creatureToShow->mpRegenMult != 1000) {
			mvprintw(x++, y, "%MP RGN : %%%d", (int)creatureToShow->mpRegenMult / 10);
		}
		mvprintw(x++, y, "DR : %d", creatureToShow->DR);
		mvprintw(x++, y, "SGHT : %d", (int)creatureToShow->getSight());
		mvprintw(x++, y, "MV SPD : %d", (int)creatureToShow->perMoveTick());
		mvprintw(x++, y, "ATK SPD : %d", (int)creatureToShow->perAttackTick());

		x++;


		attrset(COLOR_PAIR(getColorIndex(4, 4, 4)));
		mvprintw(x++, y, "Skills");

		x++;

		for (unsigned i = 0; i < creatureToShow->skills.size(); i++) {
			Skill *s = creatureToShow->skills[i];
			mvprintw(x, y, "%s", s->name);

			int t = y + strlen(s->name) + 1;
			bool p = false;
			if (s->requiresTarget()) {
				attrset(COLOR_PAIR(getColorIndex(4, 4, 4)));
				if (!p++) mvaddch(x, t++, '(');
				else mvaddch(x, t++, ' ');

				attrset(COLOR_PAIR(getColorIndex(7, 7, 0)));
				mvaddch(x, t++, 'X');
			}
			if (s->manaCost) {
				attrset(COLOR_PAIR(getColorIndex(4, 4, 4)));
				if (!p++) mvaddch(x, t++, '(');
				else mvaddch(x, t++, ' ');
				attrset(COLOR_PAIR(getColorIndex(0, 0, 7)));
				sprintf_s(buff, "%dmp", s->manaCost);
				mvprintw(x, t, "%s", buff);
				t += strlen(buff);
			}
			int cd = (int)max(s->timeToNextCast - globalTick, creatureToShow->timeToNextSkill - globalTick);

			if (cd > 0) {
				attrset(COLOR_PAIR(getColorIndex(4, 4, 4)));
				if (!p++) mvaddch(x, t++, '(');
				else mvaddch(x, t++, ' ');
				attrset(COLOR_PAIR(getColorIndex(0, 0, 7)));
				sprintf_s(buff, "%dcd", cd);
				mvprintw(x, t, "%s", buff);
				t += strlen(buff);
			}

			attrset(COLOR_PAIR(getColorIndex(4, 4, 4)));
			if (p) mvaddch(x, t++, ')');
			x++;
		}




		x++;
		mvprintw(x++, y, "Inventory");
		x++;

		for (unsigned i = 0; i < creatureToShow->equipmentSlots.size(); i++) {
			for (unsigned j = 0; j < creatureToShow->equipmentSlots[i].size(); j++) {
				if (creatureToShow->equipmentSlots[i][j] != NULL) {
					printEquipment(creatureToShow, creatureToShow->equipmentSlots[i][j], x, y, false);
				}
			}
		}


		for (unsigned i = 0; i < creatureToShow->buffs.size(); i++) {
			creatureToShow->buffs[i]->printStats(creatureToShow, x, y);
		}

		x++;
	}

	bool printed = false;
	for (unsigned i = 0; i < droppedEquipments.size(); i++) {
		if (droppedEquipments[i].p == cursor) {
			if (!printed) {
				printed = true;
				attrset(COLOR_PAIR(getColorIndex(4, 4, 4)));
				mvprintw(x++, y, "Items on Ground");
				x++;
			}
			printEquipment(creatures[0], droppedEquipments[i].e, x, y, true);
		}
	}

	maxScroll = x - initX - mazeWindow.windowSize.x + 1;

	attrset(COLOR_PAIR(getColorIndex(4, 4, 4)));
	int m = mazeWindow.windowPos.x + mazeWindow.windowSize.x - 1;
	int t = y;

	for (int i = y; i < col; i++) {
		mvaddch(0, i, ' ');
	}

	for (int i = y; i < col; i++) {
		for (int j = m; j < row; j++) {
			mvaddch(j, i, ' ');
		}
	}

	attrset(COLOR_PAIR(getColorIndex(7, 7, 0)));

	if (scrollVal > 0) {
		mvaddch(m - 1, t, ACS_UARROW);
		mvprintw(m - 1, t + 2, "PageUp");
		t += 9;
	}

	if (scrollVal < maxScroll) {
		mvaddch(m - 1, t, ACS_DARROW);
		mvprintw(m - 1, t + 2, "PageDown");
	}

	x = m + 1;

	attrset(COLOR_PAIR(getColorIndex(0, 7, 7)));
	mvprintw(x++, y + 2, "Action Points (%d / %d)", actionPoint, 100);
	int w = (col - 2) - (y + 1);
	for (int i = y + 1; i < col - 2; i++) {
		float k = actionPoint / 100.0f;
		float l = ((float)i - y - 1) / w;
		if (k < l) break;
		mvaddch(x, i, '=');
	}
	mvaddch(x, y, '|');
	mvaddch(x, col - 2, '|');
	x += 1;

	if (tickGame) {
		x++;
		mvprintw(x++, y, "Hit space to pause game and take action.", actionPoint, 100);
	} else {
		if (menu == MENU_MAIN) {
			attrset(COLOR_PAIR(getColorIndex(7, 7, 7)));
			mvaddch(x, y + 1, ACS_UARROW);
			mvaddch(x + 1, y + 0, ACS_LARROW);
			mvaddch(x + 1, y + 1, ACS_DARROW);
			mvaddch(x + 1, y + 2, ACS_RARROW);

			mvprintw(x + 1, y + 4, "Move cursor", actionPoint, 100);
			x += 2;

			attrset(COLOR_PAIR(actionPoint >= createSingeCreatureP ? getColorIndex(0, 7, 7) : getColorIndex(2, 2, 2)));
			mvprintw(x++, y, "1 - Generate a stronger creature (AP %d)", createSingeCreatureP);
			attrset(COLOR_PAIR(actionPoint >= createCreatureGroupP ? getColorIndex(0, 7, 7) : getColorIndex(2, 2, 2)));
			mvprintw(x++, y, "2 - Generate a creature group (AP %d)", createCreatureGroupP);
			attrset(COLOR_PAIR(actionPoint >= takeControlP ? getColorIndex(0, 7, 7) : getColorIndex(2, 2, 2)));
			mvprintw(x++, y, "3 - Take control of a unit (AP %d)", takeControlP);
			x++;
			attrset(COLOR_PAIR(getColorIndex(0, 7, 7)));
			mvprintw(x++, y, "Space - Toggle pause/play");
		} else if (menu == MENU_CONTROL_CREATURE) {

			attrset(COLOR_PAIR(getColorIndex(7, 7, 7)));
			mvprintw(x++, y, "1 - Move a single cell");
			x++;
			mvprintw(x++, y, "Enter - Release Control");

		} else if (menu == MENU_SINGLE_MOVE) {
			attrset(COLOR_PAIR(getColorIndex(7, 7, 7)));
			x++;
			mvaddch(x, y + 1, ACS_UARROW);
			mvaddch(x + 1, y + 0, ACS_LARROW);
			mvaddch(x + 1, y + 1, ACS_DARROW);
			mvaddch(x + 1, y + 2, ACS_RARROW);

			mvprintw(x + 1, y + 4, "Move cursor", actionPoint, 100);
			x += 3;

			mvprintw(x + 0, y, "7 8 9 | 5 Wait a single turn");
			mvprintw(x + 1, y, " \\|/  | 1-9 Move/Attack a cell");
			mvprintw(x + 2, y, "4-5-6 | 0 Use a skill/item");
			mvprintw(x + 3, y, " /|\\  |");
			mvprintw(x + 4, y, "1 2 3 |");
			x += 7;

			mvprintw(x++, y, "Enter - Release Control");
		} else if (menu == MENU_USE_SKILL) {
			attrset(COLOR_PAIR(getColorIndex(7, 7, 7)));
			x++;
			mvaddch(x, y + 1, ACS_UARROW);
			mvaddch(x + 1, y + 0, ACS_LARROW);
			mvaddch(x + 1, y + 1, ACS_DARROW);
			mvaddch(x + 1, y + 2, ACS_RARROW);

			mvprintw(x - 1, y + 18, "2 - Prev. Skill");
			mvprintw(x + 0, y + 18, "5 - Use. Skill");
			mvprintw(x + 1, y + 18, "8 - Use. Skill");

			mvprintw(x + 1, y + 4, "Move cursor", actionPoint, 100);
			x += 3;

			int skillSkip = skillFocus - 5;
			if (skillSkip < 0) skillSkip = 0;
			int j = 0;
			int focus = skillFocus;
			focusedItem = focusedSkill = -1;
			maxFocus = 0;
			for (unsigned i = 0; i < creatureToControl->skills.size(); i++) {
				Skill *s = creatureToControl->skills[i];
				if (!focus) focusedSkill = i;

				if (skillSkip > 0) {
					skillSkip--;
					focus--;
					maxFocus++;
				} else {
					attrset(COLOR_PAIR(getColorIndex(7, 7, 7)));
					if (!focus) mvprintw(x, y + 2, "> %s", s->name);
					else mvprintw(x, y + 2, "%s", s->name);
					int t = y + 2 + strlen(s->name) + 1 + (!focus ? 2 : 0);
					focus--;
					maxFocus++;
					bool p = false;
					if (s->requiresTarget()) {
						attrset(COLOR_PAIR(getColorIndex(7, 7, 7)));
						if (!p++) mvaddch(x, t++, '(');
						else mvaddch(x, t++, ' ');

						attrset(COLOR_PAIR(getColorIndex(7, 7, 0)));
						mvaddch(x, t++, 'X');
					}
					if (s->manaCost) {
						attrset(COLOR_PAIR(getColorIndex(7, 7, 7)));
						if (!p++) mvaddch(x, t++, '(');
						else mvaddch(x, t++, ' ');
						attrset(COLOR_PAIR(getColorIndex(0, 0, 7)));
						sprintf_s(buff, "%dmp", s->manaCost);
						mvprintw(x, t, "%s", buff);
						t += strlen(buff);
					}
					int cd = (int)max(s->timeToNextCast - globalTick, creatureToControl->timeToNextSkill - globalTick);

					if (cd > 0) {
						attrset(COLOR_PAIR(getColorIndex(7, 7, 7)));
						if (!p++) mvaddch(x, t++, '(');
						else mvaddch(x, t++, ' ');
						attrset(COLOR_PAIR(getColorIndex(0, 0, 7)));
						sprintf_s(buff, "%dcd", cd);
						mvprintw(x, t, "%s", buff);
						t += strlen(buff);
					}

					attrset(COLOR_PAIR(getColorIndex(7, 7, 7)));
					if (p) mvaddch(x, t++, ')');
					x++;
				}
			}

			for (unsigned i = 0; i < creatureToControl->equipmentSlots[CONSUMABLE].size(); i++) {
				if (creatureToControl->equipmentSlots[CONSUMABLE][i]) {

					if (!focus) focusedItem = i;
					Skill *s = ((Consumable*)creatureToControl->equipmentSlots[CONSUMABLE][i])->skill;
					if (skillSkip > 0) {
						skillSkip--;
						focus--;
						maxFocus++;
					} else {
						if (!focus) mvprintw(x, y + 2, "> %s", creatureToControl->equipmentSlots[CONSUMABLE][i]->name);
						else mvprintw(x, y + 2, "%s", creatureToControl->equipmentSlots[CONSUMABLE][i]->name);
						int t = y + 2 + strlen(creatureToControl->equipmentSlots[CONSUMABLE][i]->name) + 1 + (!focus ? 2 : 0);
						focus--;
						maxFocus++;
						bool p = false;
						if (s->requiresTarget()) {
							attrset(COLOR_PAIR(getColorIndex(7, 7, 7)));
							if (!p++) mvaddch(x, t++, '(');
							else mvaddch(x, t++, ' ');

							attrset(COLOR_PAIR(getColorIndex(7, 7, 0)));
							mvaddch(x, t++, 'X');
						}


						int cd = (int)(10.0f - (globalTick - creatureToControl->lastConsumeTick));

						if (cd > 0) {
							attrset(COLOR_PAIR(getColorIndex(7, 7, 7)));
							if (!p++) mvaddch(x, t++, '(');
							else mvaddch(x, t++, ' ');
							attrset(COLOR_PAIR(getColorIndex(7, 0, 0)));
							sprintf_s(buff, "%dcd", cd);
							mvprintw(x, t, "%s", buff);
							t += strlen(buff);
						}

						attrset(COLOR_PAIR(getColorIndex(7, 7, 7)));
						if (p) mvaddch(x, t++, ')');
						x++;
					}

				}
			}
			mvprintw(row - 1, y, "Enter - Previous Menu");
		}

	}


	attrset(COLOR_PAIR(255));
	mvprintw(0, 2, "Tick : %d", (int)(globalTick));

	mvprintw(0, 25, "Floor : %d", currentLevel);

	for (unsigned i = 0; i < creatures.size(); i++) {
		mazeWindow.data[creatures[i]->pos.x][creatures[i]->pos.y] = creatures[i]->pixel;
	}

	if (tickGame) {
		if (creatureToControl)
			mazeWindow.mapCenter = creatureToControl->pos;
		else
			mazeWindow.mapCenter = creatures[0]->pos;
	} else {
		mazeWindow.mapCenter = cursor;
		int t = (int)(realTick / 200.0f);

		if (t % 2) {
			mazeWindow.data[cursor.x][cursor.y].character = 'X';
			mazeWindow.data[cursor.x][cursor.y].color = getColorIndex(7, 7, 4);
		}
	}

	mazeWindow.render();

	if (isEnded) {
		char *text = "The Adventurer is dead!";
		char *text2 = " Hit space to restart";
		int l = strlen(text);
		int x = mazeWindow.windowPos.x + mazeWindow.windowSize.x / 2;
		int y = mazeWindow.windowPos.y + mazeWindow.windowSize.y / 2 - l / 2;
		for (int i = x - 1; i <= x + 2; i++) {
			for (int j = y - 1; j < y + l + 1; j++) {
				mvaddch(i, j, ' ');
			}
		}

		attrset(COLOR_PAIR(getColorIndex(7, 7, 7)));
		mvprintw(x, y, text);
		mvprintw(x + 1, y, text2);
	}

	attrset(COLOR_PAIR(255));

	if (tickGame) {
		messageScrollVal = 0;
	} else {
		int z = 0;
		if (messageScrollVal > 0) {
			mvprintw(mazeWindow.windowPos.x + mazeWindow.windowSize.x - 1, 2, " + Scroll Down");
			z += 16;
		}

		if (messageScrollVal < messageCount - 14) {
			mvprintw(mazeWindow.windowPos.x + mazeWindow.windowSize.x - 1, z + 1, "%c - Scroll Up", z == 0 ? ' ' : '/');
		}
	}
	int j = messagePos - messageScrollVal;
	int i = 0;

	do {

		mvprintw(row - i - 2, 2, "%s", messages[j]);
		j--;
		i++;
		if (j < 0) {
			j = MAX_MESSAGE - 1;
		}
	} while (j != messagePos && i < 13);

	attrset(COLOR_PAIR(1));
	move(0, 50);

	refresh();
}

bool tick() {
	bool hasChange = false;

	while (globalTick - lastAPUpdateTick > 40.0f) {
		lastAPUpdateTick += 40.0f;
		actionPoint++;
		hasChange = true;
	}

	if (creatureToControl && globalTick > controlEndTime) {
		creatureToControl->controlAction = 0;
		creatureToControl->beingControlled = FALSE;
		creatureToControl->hasTarget = false;
		creatureToControl = NULL;
		menu = MENU_MAIN;
	}
	globalTick += 1.0f;
	for (unsigned i = 0; i < creatures.size(); i++) {
		if (creatures[i]->isDead) {
			if (creatureToControl = creatures[i]) {
				creatureToControl = NULL;
				menu = MENU_MAIN;
			}
			delete creatures[i];
			creatures[i] = creatures[creatures.size() - 1];
			creatures.pop_back();
			i--;
			continue;
		}
		hasChange |= creatures[i]->tick();
	}
	if (creatureToControl && creatureToControl->controlActionDone) {
		int a = 5;
	}
	if (tickGame && creatureToControl != NULL && creatureToControl->controlActionDone) {
		tickGame = false;
		cursor = creatureToControl->pos;
		creatureToControl->controlAction = 0;
	}

	return hasChange;
}

int WINAPI  WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPSTR lpszCmdLine, int nCmdShow) {

	ShowWindow(GetActiveWindow(), SW_SHOW);
	win = initscr();
	raw();
	keypad(stdscr, TRUE);
	start_color();
	initTools();

	scrollok(win, true);
	keypad(win, true);
	nodelay(win, true);
	curs_set(0);

	resize_term(45, 125);

	int row, col;
	getmaxyx(stdscr, row, col);
	mazeWindow.windowSize.x = row - 15;
	mazeWindow.windowSize.y = col - 45;
	for (int r = 0; r < 8; r++) {
		for (int g = 0; g < 8; g++) {
			for (int b = 0; b < 4; b++) {
				int index = getColorIndex(r, g, b * 2);
				int rc = (int)(r*(1000.0f / 7));
				int gc = (int)(g*(1000.0f / 7));
				int bc = (int)(b*(1000.0f / 3));

				init_color(index, rc, gc, bc);

				init_pair(index, index, COLOR_BLACK);
			}
		}
	}
	resetGame();
	msleep(3000);


	mazeWindow.windowPos.x = 1;
	mazeWindow.windowPos.y = 2;


	//scrollok(win, false);

	while (true) {
		int i;
		unsigned time = mtime();

		if (gotoNextLevel) {
			gotoNextLevel = false;
			currentLevel++;
			resetGame(true);
		}

		bool hasChange = false;
		while ((i = getch()) != ERR) {
			hasChange |= control(i);
		}
		if (tickGame) {
			hasChange |= tick();
		}
		if (hasChange || !tickGame) {
			gameRefresh();
			realTick += 20.0f;
			time -= mtime();
			int wait = 10 - time;
			if (wait > 0) {
				//msleep(wait);
			}
		}
	}
	endwin();

	return 0;
}
