#pragma once

#include "CreatureGen.h"

extern Maze maze;
bool generateCreature(Pos pos, Creature &c) {
	if (!maze.isPosValid3(pos) || maze.walls[pos.x][pos.y] != 0) {
		return false;
	}

	if (creatures[0].visibleCells[pos.x][pos.y] == 1) {
		return false;
	}

	c = generateGoblin(1 + ran(4) / 3);
	c.reset(pos);
	return true;
}


bool generateCreatureGroup(Pos pos, vector<Creature> &group) {

	if (!maze.isPosValid3(pos) || maze.walls[pos.x][pos.y] != 0) {
		return false;
	}

	if (creatures[0].visibleCells[pos.x][pos.y] == 1) {
		return false;
	}

	vector<Pos> npos;
	doExplore<MAZE_W, MAZE_H>(pos, maze.walls, addWeights, 3.0f, npos);

	bool ret = true;
	int j = npos.size();
	int t = ran(4) + 2;
	while (t > 0 && j > 0) {
		int i = ran(j);
		Pos p = npos[i];

		if (creatures[0].visibleCells[p.x][p.y] != 1 && maze.walls[p.x][p.y] == 0) {
			Creature c;
			bool added = generateCreature(p, c);
			if (added) {
				ret = true;
				group.push_back(c);
				t--;
			}
		}
		npos[i] = npos[j - 1];
		j--;
	}
	return ret;
}