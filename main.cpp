#include<stdio.h>
#include<stdlib.h>
#include"Tools.h"
#include"Maze.h"
#include"GameWindow.h"
#include"Creature.h"
#include"CreatureGen.h"
#include"MazeActions.h"
#include<time.h>


float globalTick = 0;
float realTick = 0;
WINDOW * win;

Maze maze;
bool tickGame;


int nextCreatureIndex = 0;

int messagePos;
char messages[MAX_MESSAGE][128];

vector<Creature*> creatures;
Pos cursor;
GameWindow<MAZE_W, MAZE_H> mazeWindow;
int globalVisible[MAZE_W][MAZE_H];

void resetGame() {
	memset(globalVisible, 0, sizeof(globalVisible));
	globalTick = 0;
	realTick = 0;

	maze.genRand(3);
	messagePos = 0;
	for (int i = 0; i < MAX_MESSAGE; i++) {
		messages[i][0] = '\0';
	}
	creatures.clear();
	creatures.push_back(new Creature());
	creatures[0]->explores = true;
	creatures[0]->wandersAround = false;
	creatures[0]->type = ADVENTURER;
	creatures[0]->pixel.character = '@';
	creatures[0]->pixel.color = getColorIndex(7, 0, 0);
	creatures[0]->movePerTick = 5.0f;
	creatures[0]->sight = 5.0f;
	creatures[0]->hpMax = creatures[0]->hp = 300;
	creatures[0]->mpMax = creatures[0]->mp = 100;
	creatures[0]->level = 1;
	strcpy_s(creatures[0]->name, "Adventurer");
	creatures[0]->tickToRegen = 120.0f;

	creatures[0]->reset(Pos(maze.roomPos[0].x, maze.roomPos[0].y));

	creatures[0]->equip(getWeapon(5));
	creatures[0]->equip(getArmor(4));
	creatures[0]->equip(getHelm(3));
	creatures[0]->equip(getShield(3));
	creatures[0]->equip(getBoots(3));
	creatures[0]->equip(getGloves(3));
	creatures[0]->equip(getRing(3));
	creatures[0]->equip(getRing(3));
	creatures[0]->equip(getAmulet(3));

	creatures[0]->skills.push_back(new BerserkSkill(creatures[0]));

	pushMessage("Wild Adventurer appeared!");

	for (int i = 0; i < 6; i++) {
		while (true) {
			Pos p = maze.emptyPos[ran(maze.emptyPos.size())];
			vector<Creature*> group;
			if (generateCreatureGroup(p, group, 1, GOBLIN)) {
				for (unsigned i = 0; i < group.size(); i++) {
					creatures.push_back(group[i]);
				}
				break;
			}
		}
	}

	cursor = creatures[0]->pos;
	tickGame = false;

	srand((unsigned)time(0));
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
			cursor = creatures[0]->pos;
			hasChange = true;
			break;

		case '1':
			if (!tickGame) {
				Creature* c;
				if (c = generateCreature(cursor, 1, ran(4))) {
					creatures.push_back(c);
				}
			}
			break;
		case '2':
			if (!tickGame) {
				vector<Creature*> group;
				if (generateCreatureGroup(cursor, group, 1, GOBLIN)) {
					for (unsigned i = 0; i < group.size(); i++) {
						creatures.push_back(group[i]);
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


void gameRefresh() {
	erase();
	attrset(COLOR_PAIR(255));
	mvprintw(0, 2, "Tick : %d", (int)(globalTick));

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

			if (creatures[0]->visibleCells[i][j] == 1) {
				mazeWindow.data[i][j].color = getColorIndex(7, 7, 7);
			} else {

				if (creatures[0]->visibleCells[i][j] == 2) {
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
	mazeWindow.data[creatures[0]->moveTarget.x][creatures[0]->moveTarget.y].character = 'X';
	mazeWindow.data[creatures[0]->moveTarget.x][creatures[0]->moveTarget.y].color = getColorIndex(7, 0, 0);


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
		mvprintw(x++, y, "SGHT : %d", (int)creatureToShow->getSight());
		mvprintw(x++, y, "MV SPD : %d", (int)creatureToShow->perMoveTick());
		mvprintw(x++, y, "ATK SPD : %d", (int)creatureToShow->perAttackTick());

		x++;

		mvprintw(x++, y, "Inventory", (int)creatureToShow->perMoveTick());
		x++;

		if (creatureToShow->weapon->type != 0) {
			mvprintw(x++, y, "WPN : %s", creatureToShow->weapon->name);
			mvprintw(x++, y, "DMG : %d - %d", creatureToShow->weapon->minDamage, creatureToShow->weapon->maxDamage);
			mvprintw(x++, y, "ATK SPD : %d", (int)creatureToShow->weapon->swingTime);
			mvprintw(x++, y, "RNG : %d", creatureToShow->weapon->range);
			if (creatureToShow->weapon->buffGroup) {
				creatureToShow->weapon->buffGroup->printStats(creatureToShow, x, y);
			}
			x++;
		}

		for (unsigned i = 1; i < creatureToShow->eqipmentSlots.size(); i++) {
			for (unsigned j = 0; j < creatureToShow->eqipmentSlots[i].size(); j++) {
				if (creatureToShow->eqipmentSlots[i][j] != NULL) {
					attrset(COLOR_PAIR(getColorIndex(4, 4, 4)));
					mvprintw(x++, y, "%s : %s", eqipmentNames[i], creatureToShow->eqipmentSlots[i][j]->name);
					if (creatureToShow->eqipmentSlots[i][j]->buffGroup) {
						creatureToShow->eqipmentSlots[i][j]->buffGroup->printStats(creatureToShow, x, y);
					}
					x++;
				}
			}
		}
		/*
		if (creatureToShow->armor->type != 0) {

		mvprintw(x++, y, "ARM : %s", creatureToShow->armor->name);
		mvprintw(x++, y, "DR : %d", creatureToShow->armor->DR);
		attrset(COLOR_PAIR(getColorIndex(7, 0, 0)));
		int slow = (int)(100 * creatureToShow->armor->slowness) - 100;
		if (slow != 0) {
		mvprintw(x++, y, "SLW : +%%%d", (int)(100 * creatureToShow->armor->slowness) - 100);
		}
		attrset(COLOR_PAIR(getColorIndex(4, 4, 4)));
		x++;
		}*/

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

	srand(31);
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
		int t = maze.walls[44][11];
		bool hasChange = false;
		while ((i = getch()) != ERR) {
			hasChange |= control(i);
		}
		if (tickGame) {
			hasChange |= tick();
		}
		gameRefresh();
		if (hasChange || !tickGame) {
			realTick += 20.0f;
			time -= mtime();
			int wait = 10 - time;
			if (wait > 0) {
				msleep(wait);
			}
		}
	}
	endwin();

	return 0;
}
