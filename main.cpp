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

Maze maze;
bool tickGame;
bool gotoNextLevel = false;

int currentLevel = 1;

int nextCreatureIndex = 0;

int messagePos;
char messages[MAX_MESSAGE][128];

vector<Creature*> creatures;
vector<DroppedEquipment> droppedEquipments;

Pos cursor;
GameWindow<MAZE_W, MAZE_H> mazeWindow;
int globalVisible[MAZE_W][MAZE_H];

void resetGame(bool keepAdventurer = false) {
	if (!keepAdventurer) {
		globalTick = realTick = 0.0f;
		currentLevel = 1;
	}
	seed = (unsigned)time(0);
	printw("%u ran\n", seed);
	srand(seed);
	memset(globalVisible, 0, sizeof(globalVisible));

	maze.genRand(3);
	messagePos = 0;
	for (int i = 0; i < MAX_MESSAGE; i++) {
		messages[i][0] = '\0';
	}

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
		adventurer->mpMax = adventurer->mp = 100;
		adventurer->level = 1;
		adventurer->DR = 1;
		adventurer->loots = true;
		adventurer->consumes = true;
		strcpy_s(adventurer->name, "Adventurer");
		adventurer->tickToRegen = 100.0f;

		adventurer->reset(Pos(-1, -1));

		adventurer->equip(getWeapon(5));
		adventurer->equip(getArmor(4, true));
		adventurer->equip(getHelm(2));
		adventurer->equip(getShield(1));
		adventurer->equip(getBoots(1));
		adventurer->equip(getGloves(1));
		adventurer->equip(getRing(1));
		adventurer->equip(getRing(1));
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
		adventurer->skills.push_back(new BerserkSkill());
		creatures.push_back(adventurer);
	}


	creatures[0]->resetPos(maze.upstairs);


	pushMessage("Wild Adventurer appeared!");

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

bool control(int i) {
	bool hasChange = false;
	bool moved = false;
	Pos newP;
	switch (i) {
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
			}
			cursor = creatures[0]->pos;
			hasChange = true;
			break;

		case '1':
			if (!tickGame) {
				Creature* c;
				if (c = generateCreature(cursor, 1, ran(4))) {
					c->reset(c->pos);
					creatures.push_back(c);
				}
			}
			break;
		case '2':
			if (!tickGame) {
				vector<Creature*> group;
				if (generateCreatureGroup(cursor, group, currentLevel, GOBLIN)) {
					for (unsigned i = 0; i < group.size(); i++) {
						group[i]->reset(group[i]->pos);
						creatures.push_back(group[i]);
					}
				}
			}
			break;
		case '3':
			gotoNextLevel = true;
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

void gameRefresh() {
	erase();
	attrset(COLOR_PAIR(255));
	mvprintw(0, 2, "Tick : %d", (int)(globalTick));

	mvprintw(0, 20, "Level : %d", currentLevel);
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
	//mazeWindow.data[creatures[0]->moveTarget.x][creatures[0]->moveTarget.y].character = 'X';
	//mazeWindow.data[creatures[0]->moveTarget.x][creatures[0]->moveTarget.y].color = getColorIndex(7, 0, 0);

	for (unsigned i = 0; i < creatures.size(); i++) {
		mazeWindow.data[creatures[i]->pos.x][creatures[i]->pos.y] = creatures[i]->pixel;
	}

	Creature *creatureToShow = NULL;
	if (tickGame) {
		creatureToShow = creatures[0];
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

	if (creatureToShow) {
		int x = mazeWindow.windowPos.x;
		int y = mazeWindow.windowPos.y + mazeWindow.windowSize.y + 2;

		attrset(COLOR_PAIR(creatureToShow->pixel.color));
		mvprintw(x++, y, "LVL %d %s #%d", creatureToShow->level, creatureToShow->name, creatureToShow->index);
		x++;

		attrset(COLOR_PAIR(getColorIndex(7, 0, 0)));
		mvprintw(x++, y, "HP : %d / %d", creatureToShow->hp, creatureToShow->hpMax);
		if (creatureToShow->mpMax > 0) {
			attrset(COLOR_PAIR(getColorIndex(0, 0, 7)));
			mvprintw(x++, y, "MP : %d / %d", creatureToShow->mp, creatureToShow->mpMax);
		}
		if (creatureToShow->type > 100) {
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

		mvprintw(x++, y, "Inventory", (int)creatureToShow->perMoveTick());
		x++;

		if (creatureToShow->weapon->type != 0) {
			mvprintw(x++, y, "(%d) %s", creatureToShow->weapon->goodness(creatureToShow), creatureToShow->weapon->name);
			mvprintw(x++, y, "DMG : %d - %d", creatureToShow->weapon->minDamage, creatureToShow->weapon->maxDamage);
			mvprintw(x++, y, "ATK SPD : %d", (int)creatureToShow->weapon->swingTime);
			mvprintw(x++, y, "RNG : %d", creatureToShow->weapon->range);
			if (creatureToShow->weapon->buffGroup) {
				creatureToShow->weapon->buffGroup->printStats(creatureToShow, x, y);
			}
			x++;
		}

		for (unsigned i = 1; i < creatureToShow->equipmentSlots.size(); i++) {
			for (unsigned j = 0; j < creatureToShow->equipmentSlots[i].size(); j++) {
				if (creatureToShow->equipmentSlots[i][j] != NULL) {
					attrset(COLOR_PAIR(getColorIndex(4, 4, 4)));
					int count = 0;
					if (i == CONSUMABLE) {
						count = ((Consumable*)creatureToShow->equipmentSlots[i][j])->count;
					}
					if (count <= 1) {
						mvprintw(x++, y, "(%d) %s", creatureToShow->equipmentSlots[i][j]->goodness(creatureToShow), creatureToShow->equipmentSlots[i][j]->name);
					} else {
						mvprintw(x++, y, "(%d) %s x%d", creatureToShow->equipmentSlots[i][j]->goodness(creatureToShow), creatureToShow->equipmentSlots[i][j]->name, count);
					}
					if (creatureToShow->equipmentSlots[i][j]->buffGroup) {
						creatureToShow->equipmentSlots[i][j]->buffGroup->printStats(creatureToShow, x, y);
					}
					if (i != CONSUMABLE) x++;
				}
			}
		}

		x++;

		for (unsigned i = 0; i < creatureToShow->buffs.size(); i++) {
			creatureToShow->buffs[i]->printStats(creatureToShow, x, y);
		}

	}

	for (unsigned i = 0; i < creatures.size(); i++) {
		mazeWindow.data[creatures[i]->pos.x][creatures[i]->pos.y] = creatures[i]->pixel;
	}

	if (tickGame) {
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


	if (creatures.size() == 0 || creatures[0]->type != ADVENTURER) {
		char *text = "The Adventurer is dead!";
		int l = strlen(text);
		int x = mazeWindow.windowPos.x + mazeWindow.windowSize.x / 2;
		int y = mazeWindow.windowPos.y + mazeWindow.windowSize.y / 2 - l / 2;
		for (int i = x - 1; i <= x + 1; i++) {
			for (int j = y - 1; j < y + l + 1; j++) {
				mvaddch(i, j, ' ');
			}
		}

		attrset(COLOR_PAIR(getColorIndex(7, 7, 7)));
		mvprintw(x, y, text);
	}

	attrset(COLOR_PAIR(255));

	int j = messagePos;
	int i = 0;
	do {
		mvprintw(mazeWindow.windowPos.x + mazeWindow.windowSize.x + i, 2, "%s", messages[j]);
		j--;
		i++;
		if (j < 0) {
			j = MAX_MESSAGE - 1;
		}
	} while (j != messagePos && i < 13);


	move(0, 0);
	refresh();
}

bool tick() {
	bool hasChange = false;
	globalTick += 1.0f;
	for (unsigned i = 0; i < creatures.size(); i++) {
		if (creatures[i]->isDead) {
			delete creatures[i];
			creatures[i] = creatures[creatures.size() - 1];
			creatures.pop_back();
			i--;
			continue;
		}
		hasChange |= creatures[i]->tick();
	}

	return hasChange;
}

int WINAPI  WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPSTR lpszCmdLine, int nCmdShow) {

	ShowWindow(GetActiveWindow(), SW_SHOW);
	win = initscr();
	start_color();
	initTools();

	scrollok(win, true);
	keypad(win, true);
	nodelay(win, true);
	curs_set(0);

	resize_term(45, 120);

	resetGame();

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
