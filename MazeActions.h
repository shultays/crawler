#pragma once

#include "CreatureGen.h"

extern Maze maze;
Creature* generateCreature(Pos pos, int level, int modif, int type = GOBLIN) {
	if (!maze.isPosValid3(pos) || maze.walls[pos.x][pos.y] != 0) {
		return NULL;
	}

	if (creatures[0]->visibleCells[pos.x][pos.y] == 1) {
		return NULL;
	}
	Creature* c = NULL;
	if (type == GOBLIN) {
		c = generateGoblin(level + ran(4) / 3);
		if (modif == MAGE || modif == PRIEST) {
			c->hpMax += 10 + 5 * level;
			c->mpMax += 40 + 20 * level;
			if (modif == PRIEST) {
				c->skills.push_back(new HealSkill(c));
				strcat_s(c->name, " Priest");
				c->pixel.color = getColorIndex(0, 4, 6);
			} else if (modif == MAGE) {
				c->pixel.color = getColorIndex(6, 2, 4);
				switch (ran(3)) {
					case 0:
						c->skills.push_back(new LightingSkill(c));
						break;
					case 1:
						c->skills.push_back(new IceBoltSkill(c));
						break;
					case 2:
						c->skills.push_back(new FireBallSkill(c));
						break;
				}

				strcat_s(c->name, " Warlock");
			}
		} else if (modif == ROGUE) {
			c->pixel.color = getColorIndex(0, 3, 0);
			c->hpMax += 10 + 5 * level;
			c->mpMax += 30 + 10 * level;
			c->movePerTick *= 0.8f;
			c->skills.push_back(new PoisonBladeSkill(c));
			strcat_s(c->name, " Rogue");
		} else if (modif == FIGHTER) {
			c->pixel.color = getColorIndex(2, 6, 2);
			c->hpMax += 30 + 10 * level;
			c->DR += 1;
			c->weapon->minDamage = (int)(c->weapon->minDamage*1.2f);
			c->weapon->maxDamage = (int)(c->weapon->maxDamage*1.2f);
			strcat_s(c->name, " Berserker");
		}
	}
	if(c){
		c->pos = pos;
	}
	return c;
}


bool generateCreatureGroup(Pos pos, vector<Creature*> &group, int level, int type = GOBLIN) {

	if (!maze.isPosValid3(pos) || maze.walls[pos.x][pos.y] != 0) {
		return false;
	}

	if (globalVisible[pos.x][pos.y] > 0) {
		return false;
	}

	vector<Pos> npos;
	doExplore<MAZE_W, MAZE_H>(pos, maze.walls, addWeights, 3.0f, npos);

	bool ret = true;
	int j = npos.size();
	int t = ran(3) + 1;

	int masterIndex = -1;

	while (t > 0 && j > 0) {
		int i = ran(j);
		Pos p = npos[i];

		if (globalVisible[p.x][p.y] == 0 && maze.walls[p.x][p.y] == 0) {
			
			bool modified = ran(2 - level / 2) == 0;
			int modif = -1;
			if (modified) modif = ran(4);
			Creature *c = generateCreature(p, level + modified?1:0, modif, type);
			if (c) {
				if (modified && masterIndex == -1) masterIndex = c->index;
				ret = true;
				group.push_back(c);
				t--;
			}

		}
		npos[i] = npos[j - 1];
		j--;
	}
	for (unsigned i = 0; i < group.size(); i++) {
		group[i]->masterIndex = masterIndex;
		group[i]->reset(group[i]->pos);
	}
	return ret;
}