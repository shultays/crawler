#pragma once

#include "Tools.h"
#include "Maze.h"

int nextIndex;


extern Maze maze;
extern float globalTick;
extern bool tickGame;
class Creature;
extern vector<Creature> creatures;

class Weapon {
public:
	float swingTime;
	int minDamage;
	int maxDamage;
	int range;
	char name[32];
	int getDamage() {
		return minDamage + ran(maxDamage - minDamage + 1);
	}
};

class Creature {
public:
	int type;
	int oldType;

	float lastTick;
	bool explores;
	bool wandersAround;

	char name[32];

	Pos pos;
	Pixel pixel;

	int hp;
	int mp;

	Weapon weapon;

	int level;

	float minExp;
	float maxExp;

	vector<Pos> visible;
	char visibleCells[MAZE_W][MAZE_H];

	Pos moveTarget;
	vector<Pos> targetPath;
	int pathVal;
	bool hasTarget;
	bool isDead;

	float sight;
	float movePerTick;
	float getExp() {
		return ranf(minExp, maxExp);
	}
	float getSight() {
		return sight;
	}

	float perMoveTick() {
		return movePerTick;
	}

	float perAttackTick() {
		return weapon.swingTime;
	}

	void reset(Pos pos) {
		this->pos = pos;
		isDead = false;
		maze.walls[pos.x][pos.y] = type;
		memset(visibleCells, false, sizeof(visibleCells));
		visible.clear();
		updateVisibility();
		lastTick = 0.0f;
		findTarget();
	}


	void updateVisibility() {
		for (unsigned i = 0; i < visible.size(); i++) {
			if (visibleCells[visible[i].x][visible[i].y] == 1) {
				visibleCells[visible[i].x][visible[i].y] = 2;
			}
		}
		doExplore<MAZE_W, MAZE_H>(pos, maze.walls, seeWeights, getSight(), visible);
		for (unsigned i = 0; i < visible.size(); i++) {
			visibleCells[visible[i].x][visible[i].y] = 1;
		}
	}

	void findTarget() {
		hasTarget = true;
		if (explores) {
			vector<Pos> npos;
			doExplore<MAZE_W, MAZE_H>(pos, maze.walls, addWeights, 20.0f, npos);
			pathVal = 0;

			unsigned j = npos.size();

			while (j > 0) {
				unsigned i = ran(j);

				Pos p = npos[i];

				if (visibleCells[p.x][p.y] == 0 && maze.walls[p.x][p.y] == 0) {
					moveTarget = p;

					findPath<MAZE_W, MAZE_H>(pos, moveTarget, maze.walls, addWeights, 10000000.0f, targetPath);

					return;
				}
				npos[i] = npos[j - 1];
				j--;
			}

			for (int i = 0; i < MAZE_W; i++) {
				for (int j = 0; j < MAZE_H; j++) {
					if (visibleCells[i][j] == 0 && maze.walls[i][j] == 0) {
						moveTarget = Pos(i, j);

						findPath<MAZE_W, MAZE_H>(pos, moveTarget, maze.walls, addWeights, 10000000.0f, targetPath);

						return;
					}
				}
			}
		}

		hasTarget = false;
	}


	void moveToPos(Pos pos) {
		maze.walls[this->pos.x][this->pos.y] = 0;
		this->pos = pos;
		maze.walls[this->pos.x][this->pos.y] = type;
		updateVisibility();
	}

	bool tick() {
		if (isDead) {
			return false;
		}
		vector<Creature*> creaturesToAttack;
		vector<Creature*> creaturesToMove;

		for (unsigned i = 0; i < visible.size(); i++) {
			Pos p = visible[i];
			if (maze.walls[p.x][p.y] > 10) {
				if (isEnemy(maze.walls[p.x][p.y])) {
					Creature* creature;
					for (unsigned i = 0; i < creatures.size(); i++) {
						if (creatures[i].pos == p) {
							creature = &creatures[i];
							break;
						}
					}
					hasTarget = false;


					int dist_x = abs(p.x - pos.x);
					int dist_y = abs(p.y - pos.y);
					int dist = (int)sqrt(dist_x*dist_x + dist_y*dist_y);

					if (dist <= weapon.range) {
						creaturesToAttack.push_back(creature);
					} else {
						creaturesToMove.push_back(creature);
					}
				}
			}
		}


		if (creaturesToAttack.size() > 0) {
			if (globalTick - lastTick >= perAttackTick()) {
				int i = ran(creaturesToAttack.size());
				int damage = weapon.getDamage();

				char buff[128];
				sprintf_s(buff, "%s attacks %s with its %s for %d damage!", name, creaturesToAttack[i]->name, weapon.name, damage);
				pushMessage(buff);

				creaturesToAttack[i]->doDamage(damage);

				lastTick += perAttackTick();
				return true;
			}
		} else	if (creaturesToMove.size() > 0) {
			if (globalTick - lastTick >= perMoveTick()) {

				float x = 0.0f;
				float y = 0.0f;
				for (unsigned i = 0; i < creaturesToMove.size(); i++) {
					x += creaturesToMove[i]->pos.x;
					y += creaturesToMove[i]->pos.y;
				}
				x /= creaturesToMove.size();
				y /= creaturesToMove.size();

				Pos p;

				p.x = (int)round(x);
				p.y = (int)round(y);

				bool found = findPath<MAZE_W, MAZE_H>(pos, p, maze.walls, moveWeights, 10000000.0f, targetPath);
				if (found) {
					moveToPos(targetPath[0]);
					lastTick += perMoveTick();
					return true;
				}
			}
		} else if (!hasTarget && explores) {
			findTarget();
		}


		if (hasTarget && globalTick - lastTick >= perMoveTick()) {
			pathVal++;
			if (pathVal == targetPath.size()) {
				findTarget();
				return true;
			}
			lastTick += perMoveTick();
			moveToPos(targetPath[pathVal]);
			return true;
		}

		return false;
	}

	void doDamage(int damage) {
		hp -= damage;
		if (hp <= 0) {
			isDead = true;
			maze.walls[pos.x][pos.y] = 0;
			char buff[128];
			sprintf_s(buff, "%s dies!", name);
			pushMessage(buff);

		}
	}

	bool isEnemy(int type) {
		if (type > 100 && this->type <= 100) {
			return true;
		} else if (type <= 100 && this->type > 100) {
			return true;
		}
		return false;
	}
};