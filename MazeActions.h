#pragma once

#include "CreatureGen.h"

extern Maze maze;
extern bool cheat;

Creature* generateCreature(Pos pos, int level, int modif, int type = GOBLIN) {
	if (!maze.isPosValid3(pos) || maze.walls[pos.x][pos.y] != 0) {
		return NULL;
	}

	if (globalVisible[pos.x][pos.y] != 0 && cheat == false) {
		return NULL;
	}

	Creature* c = NULL;
	if (type == GOBLIN) {
		c = generateGoblin(level + ran(4) / 3, pos);
		if (modif == MAGE || modif == PRIEST) {
			c->hpMax += 10 + 5 * level;
			c->mpMax += 40 + 20 * level;
			if (modif == PRIEST) {
				strcat_s(c->name, " Priest");
				c->pixel.color = getColorIndex(0, 4, 6);
				c->skillToLearn.push_back(SkillTree(new HealSkill(), 1));
				c->skillToLearn.push_back(SkillTree(new BloodlustSkill(), 3));
			} else if (modif == MAGE) {
				c->pixel.color = getColorIndex(6, 2, 6);
				switch (ran(3)) {
					case 0:
						c->skillToLearn.push_back(SkillTree(new LightingSkill(), 1));
						break;
					case 1:
						c->skillToLearn.push_back(SkillTree(new IceBoltSkill(), 1));
						break;
					case 2:
						c->skillToLearn.push_back(SkillTree(new FireBallSkill(), 1));
						break;
				}

				c->skillToLearn.push_back(SkillTree(new AmplifyDamage(), 3));
				strcat_s(c->name, " Warlock");
			}
		} else if (modif == ROGUE) {
			c->pixel.color = getColorIndex(2, 6, 2);
			c->hpMax += 10 + 5 * level;
			c->mpMax += 30 + 10 * level;
			c->movePerTick *= 0.8f;
			c->skillToLearn.push_back(SkillTree(new PoisonBladeSkill(), 1));
			c->skillToLearn.push_back(SkillTree(new BlurSkill(), 3));
			c->skillToLearn.push_back(SkillTree(new HasteSkill(), 4));
			strcat_s(c->name, " Rogue");
		} else if (modif == FIGHTER) {
			c->pixel.color = getColorIndex(0, 2, 0);
			c->hpMax += 30 + 10 * level;
			c->DR += 1;
			c->weapon->minDamage = (int)(c->weapon->minDamage*1.2f);
			c->weapon->maxDamage = (int)(c->weapon->maxDamage*1.2f);
			char *spellName;
			char *damageType;
			int color;
			int t = ran(3);
			if (t == 0) {
				spellName = "Ench. Weapon (Fire)";
				damageType = "Fire";
				color = getColorIndex(7, 0, 0);
			} else if (t == 1) {
				spellName = "Ench. Weapon (Ice)";
				damageType = "Ice";
				color = getColorIndex(0, 7, 7);
			} else {
				spellName = "Ench. Weapon (Lighting)";
				damageType = "Lighting";
				color = getColorIndex(7, 7, 0);
			}

			c->skillToLearn.push_back(SkillTree(new ElementalWeaponSkill(spellName, damageType, color), 1));
			c->skillToLearn.push_back(SkillTree(new BerserkSkill(), 2));
			c->skillToLearn.push_back(SkillTree(new StoneSkinSkill(), 4));
			strcat_s(c->name, " Berserker");
		}
	}
	return c;
}

bool generateCreatureGroup(Pos pos, vector<Creature*> &group, int level, int type = GOBLIN) {

	if (!maze.isPosValid3(pos) || maze.walls[pos.x][pos.y] != 0) {
		return false;
	}

	if (globalVisible[pos.x][pos.y] > 0 && cheat == false) {
		return false;
	}

	vector<Pos> npos;
	doExplore<MAZE_W, MAZE_H>(pos, maze.walls, addWeights, 3.0f, npos);

	bool ret = true;
	int j = npos.size();
	int t = ran(3) + 2;

	int masterIndex = -1;

	while (t > 0 && j > 0) {
		int i = ran(j);
		Pos p = npos[i];

		if ((cheat || globalVisible[p.x][p.y] == 0) && maze.walls[p.x][p.y] == 0) {

			int w = 5 - level / 2;
			if (w < 2) w = 2;
			bool modified = ran(w) == 0;
			int modif = -1;
			if (modified) modif = ran(4);
			Creature *c = generateCreature(p, level + (modified ? 1 : 0), modif, type);
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
	}
	return ret;
}