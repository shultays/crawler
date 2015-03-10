#pragma once

#include "Tools.h"
#include "Maze.h"
#include "Skill.h"

extern int nextCreatureIndex;
extern Maze maze;
extern float globalTick;
extern bool tickGame;
class Creature;
extern vector<Creature*> creatures;

class Equipment {
public:
	Equipment() {
		buffGroup = NULL;
	}
	int slot;
	int type;
	char name[128];
	BuffGroup* buffGroup;

	void addBuff(Buff* buff) {
		if (buffGroup == NULL) buffGroup = new BuffGroup();
		buffGroup->buffs.push_back(buff);
	}
};

class Weapon : public Equipment {
public:
	float swingTime;
	int minDamage;
	int maxDamage;
	int range;

	int getDamage() {
		return minDamage + ran(maxDamage - minDamage + 1);
	}
};

class Creature {
public:
	int index;
	int lastAttackCreatureIndex;

	int type;
	int oldType;
	int masterIndex;
	float lastTick;
	bool explores;
	bool wandersAround;

	char name[32];

	Pos pos;
	Pixel pixel;

	bool wantsToRun;
	float timeToFeelSafe;
	float timeToRunAgain;

	int hp;
	int mp;
	int mpMax;
	int hpMax;

	Weapon* weapon;
	Weapon* punchs;
	vector<vector<Equipment*>> eqipmentSlots;

	int level;

	float minExp;
	float maxExp;

	vector<Pos> visible;
	char visibleCells[MAZE_W][MAZE_H];

	Pos moveTarget;
	vector<Pos> targetPath;
	unsigned pathVal;

	bool hasTarget;
	bool isDead;

	float sight;
	float movePerTick;
	float lastRegenTick;
	float lastManaRegenTick;
	float tickToNextWander;

	float tickToRegen;
	float tickToManaRegen;

	int fearless;

	float timeToNextSkill;
	int sightMult;
	vector<Buff*> buffs;
	vector<Skill*> skills;
	vector<AttackListener*> attackListeners;
	Creature* master;

	vector<Creature*> creaturesToAttack;
	vector<Creature*> creaturesToMove;
	vector<Creature*> allies;
	vector<Creature*> enemies;

	float getTickToRegen() {
		return tickToRegen * hpRegenMult / 1000;
	}
	float getTickToManaRegen() {
		return tickToManaRegen * mpRegenMult / 1000;
	}

	float getExp() {
		return ranf(minExp, maxExp);
	}
	float getSight() {
		return sight * sightMult / 1000.0f;
	}

	int damageBoost;
	int damageMult;

	int attackSpeedMult;
	int moveSpeedMult;
	int mpRegenMult;
	int hpRegenMult;
	int chanceToHit;

	int getWeaponDamage() {
		return (weapon->getDamage()*damageMult) / 1000 + damageBoost;
	}
	int DR;

	int getDR() {
		return DR;
	}

	float perMoveTick() {
		return (movePerTick * moveSpeedMult) / 1000;
	}

	float perAttackTick() {
		return (weapon->swingTime * attackSpeedMult) / 1000;
	}

	float getWanderTime() {
		return ranf(20.0f, 60.0f);
	}

	bool evaded() {
		return ran(1000) > chanceToHit;
	}

	Creature() {
		index = nextCreatureIndex++;
		lastAttackCreatureIndex = -1;
		masterIndex = -1;
		hp = hpMax = 10;
		mp = mpMax = 0;
		level = 0;
		sight = 4.0f;
		strcpy_s(name, "Weakling");
		movePerTick = 10000.0f;
		pixel.color = ran(256);
		pixel.character = '?';
		minExp = maxExp = 0;
		tickToRegen = 150.0f;
		tickToManaRegen = 100.0f;
		explores = wandersAround = false;

		eqipmentSlots.resize(EQ_MAX);

		eqipmentSlots[WEAPON].resize(1);
		eqipmentSlots[ARMOR].resize(1);
		eqipmentSlots[SHIELD].resize(1);
		eqipmentSlots[HELM].resize(1);
		eqipmentSlots[GLOVES].resize(1);
		eqipmentSlots[BOOTS].resize(1);
		eqipmentSlots[RING].resize(2);
		eqipmentSlots[AMULET].resize(1);

		punchs = new Weapon();
		punchs->slot = WEAPON;
		punchs->minDamage = 2 + level;
		punchs->maxDamage = punchs->minDamage + 4 + level;
		punchs->swingTime = 15.0f;
		strcpy_s(punchs->name, "Punchs");
		punchs->range = 1;

		for (unsigned i = 0; i < eqipmentSlots.size(); i++) {
			for (unsigned j = 0; j < eqipmentSlots[i].size(); j++) {
				eqipmentSlots[i][j] = NULL;
			}
		}
		reset(Pos(-1, -1));
		equip(punchs);
	}
	~Creature() {
		for (unsigned i = 0; i < skills.size(); i++) {
			delete skills[i];
		}
		for (unsigned i = 0; i < buffs.size(); i++) {
			buffs[i]->end(this);
			delete buffs[i];
		}
		for (unsigned i = 0; i < eqipmentSlots.size(); i++) {
			for (unsigned j = 0; j < eqipmentSlots[i].size(); j++) {
				if (eqipmentSlots[i][j] != NULL) {
					if (eqipmentSlots[i][j]->buffGroup != NULL) {
						eqipmentSlots[i][j]->buffGroup->end(this);
						delete eqipmentSlots[i][j]->buffGroup;
					}
					delete eqipmentSlots[i][j];
				}
			}

		}
		if (weapon != punchs) {
			delete punchs;
		}
	}

	void reset(Pos pos) {
		if (pos.x >= 0) {
			this->pos = pos;
			maze.walls[pos.x][pos.y] = type;
			memset(visibleCells, false, sizeof(visibleCells));
			visible.clear();
			updateVisibility();
		}

		lastTick = globalTick;
		lastRegenTick = globalTick;
		lastManaRegenTick = globalTick;
		timeToNextSkill = globalTick;
		isDead = false;
		hasTarget = false;
		tickToNextWander = globalTick + getWanderTime();
		timeToRunAgain = globalTick;
		wantsToRun = false;
		fearless = 0;
		hp = hpMax;
		mp = mpMax;

		damageBoost = 0;
		damageMult = 1000;
		attackSpeedMult = 1000;
		moveSpeedMult = 1000;
		sightMult = 1000;
		chanceToHit = 1000;

		mpRegenMult = 1000;
		hpRegenMult = 1000;

		punchs->minDamage = 2 + level;
		punchs->maxDamage = punchs->minDamage + 4 + level;

		DR = 0;
	}

	void updateVisibility() {
		for (unsigned i = 0; i < visible.size(); i++) {
			if (visibleCells[visible[i].x][visible[i].y] == 1) {
				globalVisible[visible[i].x][visible[i].y]--;
				visibleCells[visible[i].x][visible[i].y] = 2;
			}
		}
		doExplore<MAZE_W, MAZE_H>(pos, maze.walls, seeWeights, getSight(), visible, false, true);
		for (unsigned i = 0; i < visible.size(); i++) {
			visibleCells[visible[i].x][visible[i].y] = 1;
			globalVisible[visible[i].x][visible[i].y]++;
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

	Pos getAveragePos(vector<Creature*>& vec) {
		float x = 0.0f;
		float y = 0.0f;
		for (unsigned i = 0; i < vec.size(); i++) {
			x += vec[i]->pos.x;
			y += vec[i]->pos.y;
		}
		x /= vec.size();
		y /= vec.size();
		return Pos((int)x, (int)y);
	}

	bool moveToPos(Pos pos) {
		if (maze.walls[pos.x][pos.y] != 0)return true;
		maze.walls[this->pos.x][this->pos.y] = 0;
		this->pos = pos;
		assert(maze.walls[this->pos.x][this->pos.y] == 0);
		maze.walls[this->pos.x][this->pos.y] = type;
		updateVisibility();
		return false;
	}
	void heal(int hp) {
		this->hp += hp;
		if (this->hp > hpMax) {
			this->hp = hpMax;
		}
	}
	bool tick() {
		if (isDead) {
			return false;
		}
		while (globalTick - lastRegenTick > getTickToRegen()) {
			lastRegenTick += getTickToRegen();
			heal(hpMax / 20);
		}
		while (globalTick - lastManaRegenTick > getTickToManaRegen()) {
			lastManaRegenTick += getTickToRegen();
			mp += mpMax / 20;
			if (mp > mpMax) mp = mpMax;
		}

		master = NULL;

		checkCreaturesAround();
		checkSkills();
		tickBuffs();
		checkShouldRunAway();

		if (wantsToRun) {
			if (tryToRun()) {
				return true;
			}
		} else if (creaturesToAttack.size() > 0) {
			if (tryToAttack()) {
				return true;
			}
		} else	if (creaturesToMove.size() > 0) {
			if (tryToApproach()) {
				return true;
			}
		} else if (hp < hpMax / 3) {
			return false;
		} else if (!hasTarget && explores) {
			if (wantsToRun) {
				return false;
			}
			findTarget();
		} else if (wandersAround && tickToNextWander < globalTick &&  globalTick - lastTick >= perMoveTick()) {
			if (tryToWander()) {
				return true;
			}
		}

		if (tryToGoTarget()) {
			return true;
		}

		return false;
	}

	void doDamage(int damage) {
		if (hp <= 0) {
			return;
		}
		damage -= DR;
		if (damage < 1) damage = 1;
		hp -= damage;
		if (wantsToRun) {
			if (pathVal == targetPath.size()) {
				wantsToRun = false;
				timeToRunAgain = globalTick + 250;
			}
		}
		if (hp <= 0) {
			hp = 0;
			isDead = true;
			maze.walls[pos.x][pos.y] = 0;
			char buff[128];
			sprintf_s(buff, "%s#%d dies!", name, index);
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

	void checkCreaturesAround() {
		allies.clear();
		enemies.clear();
		creaturesToAttack.clear();
		creaturesToMove.clear();

		for (unsigned i = 0; i < visible.size(); i++) {
			Pos p = visible[i];
			if (p == pos) continue;
			if (maze.walls[p.x][p.y] > 10) {
				Creature* creature = NULL;
				for (unsigned i = 0; i < creatures.size(); i++) {
					if (creatures[i]->pos == p) {
						creature = creatures[i];
						break;
					}
				}
				if (creature == NULL) continue;
				if (isEnemy(maze.walls[p.x][p.y])) {
					enemies.push_back(creature);
					hasTarget = false;

					int dist_x = abs(p.x - pos.x);
					int dist_y = abs(p.y - pos.y);
					int dist = (int)sqrt(dist_x*dist_x + dist_y*dist_y);

					if (dist <= weapon->range) {
						creaturesToAttack.push_back(creature);
					} else {
						creaturesToMove.push_back(creature);
					}
				} else {
					if (creature->index == masterIndex) {
						master = creature;
					}
					allies.push_back(creature);
				}
			}
		}

	}

	void checkSkills() {
		if (timeToNextSkill <= globalTick) {
			int skillToCast = -1;
			int minSkillVal = 100000;
			for (unsigned i = 0; i < skills.size(); i++) {
				if (mp >= skills[i]->manaCost && skills[i]->timeToNextCast <= globalTick) {
					int skillVal = skills[i]->shouldCast(this, allies, enemies, creaturesToAttack, creaturesToMove);
					if (skillVal && skillVal < minSkillVal) {
						minSkillVal = skillVal;
						skillToCast = i;
					}
				}
			}
			if (skillToCast != -1) {
				mp -= skills[skillToCast]->manaCost;
				skills[skillToCast]->doCast(this, allies, enemies, creaturesToAttack, creaturesToMove);
				timeToNextSkill = globalTick + skills[skillToCast]->globalSpellDelay;
			}
		}

	}

	void tickBuffs() {

		for (unsigned i = 0; i < buffs.size(); i++) {
			buffs[i]->tick(this);
			if (buffs[i]->ended == false && buffs[i]->tickToDie < globalTick) {
				buffs[i]->end(this);
			}

			if (buffs[i]->ended) {
				delete buffs[i];
				buffs[i] = buffs[buffs.size() - 1];
				buffs.resize(buffs.size() - 1);
				i--;
				continue;
			}
		}
	}

	void checkShouldRunAway() {

		if (!fearless && timeToRunAgain < globalTick && enemies.size() >= 4 && !wantsToRun && hp < hpMax / 2) {

			int currentNCount = -1;
			for (int a = -1; a <= 1; a++) {
				for (int b = -1; b <= 1; b++) {
					if (maze.walls[pos.x + a][pos.y + b] != 1) {
						currentNCount++;
					}
				}
			}
			if (currentNCount > 2) {
				vector<Pos> safePos;
				Pos center = getAveragePos(enemies);
				int currentDist = dist(center, pos);

				for (unsigned i = 0; i < visible.size(); i++) {
					Pos p = visible[i];
					if (maze.walls[p.x][p.y] == 0) {
						int ncount = -1;

						int newDist = dist(center, p);
						if (newDist > currentDist) {
							if (maze.walls[p.x][p.y] == 0) {

								for (int a = -1; a <= 1; a++) {
									for (int b = -1; b <= 1; b++) {
										if (maze.walls[p.x + a][p.y + b] != 1 && maze.walls[p.x + a][p.y + b] != 2) {
											ncount++;
										}

									}
								}
								if (ncount < currentNCount) {
									safePos.push_back(p);
								}
							} else {
								ncount = 10;
							}
						}
					}
				}

				bool foundPos = false;
				for (unsigned i = 0; i < safePos.size() && !foundPos; i++) {
					foundPos = findPath<MAZE_W, MAZE_H>(pos, moveTarget = safePos[i], maze.walls, moveWeights, 10000000.0f, targetPath);
				}
				if (foundPos) {
					pathVal = 0;
					wantsToRun = true;
					timeToFeelSafe = globalTick + 50.0f;
				}
			}
		}

	}

	bool tryToRun() {
		if (pathVal < targetPath.size()) {
			if (globalTick - lastTick >= perMoveTick()) {
				if (moveToPos(targetPath[pathVal++])) {
					timeToFeelSafe = globalTick + 50.0f;
					return true;
				}
			}
		}
		if (timeToFeelSafe < globalTick) {
			wantsToRun = false;
			timeToRunAgain = globalTick + 250;
			return true;
		}
		return false;
	}

	bool tryToAttack() {
		if (globalTick - lastTick >= perAttackTick()) {
			bool reattack = false;

			if (type == ADVENTURER) {
				int a = 5;
			}
			int attackIndex;
			for (unsigned i = 0; i < creaturesToAttack.size(); i++) {
				if (creaturesToAttack[i]->index == lastAttackCreatureIndex) {
					reattack = true;
					attackIndex = i;
					break;
				}
			}
			if (!reattack) {
				attackIndex = ran(creaturesToAttack.size());
			}
			lastAttackCreatureIndex = creaturesToAttack[attackIndex]->index;

			if (!creaturesToAttack[attackIndex]->evaded()) {
				int damage = getWeaponDamage();
				char buff[128];
				sprintf_s(buff, "%s#%d attacks %s#%d with its %s for %d damage!", name, index, creaturesToAttack[attackIndex]->name, creaturesToAttack[attackIndex]->index, weapon->name, damage);
				pushMessage(buff);

				for (unsigned i = 0; i < attackListeners.size(); i++) {
					attackListeners[i]->attacked(this, creaturesToAttack[attackIndex], damage);
				}
				creaturesToAttack[attackIndex]->doDamage(damage);

			} else {
				char buff[128];
				sprintf_s(buff, "%s#%d attacks %s#%d  but misses.", name, index, creaturesToAttack[attackIndex]->name, creaturesToAttack[attackIndex]->index);
				pushMessage(buff);
			}

			lastTick = globalTick;
			return true;
		}
		return false;
	}

	bool tryToApproach() {
		if (wantsToRun) {
			return false;
		}
		if (globalTick - lastTick >= perMoveTick()) {
			Pos p = getAveragePos(creaturesToMove);

			bool found = p != pos && findPath<MAZE_W, MAZE_H>(pos, p, maze.walls, moveWeights, 10000000.0f, targetPath);
			if (found) {
				moveToPos(targetPath[0]);
				lastTick = globalTick;
				return true;
			} else {
				for (unsigned i = 0; i < creaturesToMove.size(); i++) {

					found = findPath<MAZE_W, MAZE_H>(pos, creaturesToMove[i]->pos, maze.walls, moveWeights, 10000000.0f, targetPath);
					if (found) {
						moveToPos(targetPath[0]);
						lastTick = globalTick;
						return true;
					}
				}
			}
		}
		return false;
	}

	bool tryToWander() {
		Pos p;
		bool hasPos = false;
		if (master) {
			vector<Pos> npos;
			doExplore<MAZE_W, MAZE_H>(master->pos, maze.walls, moveWeights, 3.0f, npos);
			if (npos.size() > 0) {
				p = npos[ran(npos.size())];

				hasPos = findPath<MAZE_W, MAZE_H>(pos, p, maze.walls, moveWeights, 10000000.0f, npos);
				if (hasPos && npos.size() == 1) hasPos = false;

				if (hasPos) {
					p = npos[0];
				}
			}
		}

		if (!hasPos && allies.size() > 0) {
			p = getAveragePos(allies);
			vector<Pos> npos;
			doExplore<MAZE_W, MAZE_H>(p, maze.walls, moveWeights, 3.0f, npos);
			if (npos.size() > 0) {
				p = npos[ran(npos.size())];
				hasPos = findPath<MAZE_W, MAZE_H>(pos, p, maze.walls, moveWeights, 10000000.0f, npos);
				if (hasPos && npos.size() == 1) hasPos = false;
				if (hasPos) {
					p = npos[0];
				}
			}
		}

		if (!hasPos) {
			vector<Pos> npos;
			doExplore<MAZE_W, MAZE_H>(pos, maze.walls, moveWeights, 3.0f, npos);
			if (npos.size() > 0) {
				hasPos = true;
				p = npos[ran(npos.size())];
				findPath<MAZE_W, MAZE_H>(pos, p, maze.walls, moveWeights, 10000000.0f, npos);
				if (hasPos && npos.size() == 1) hasPos = false;
				if (hasPos) p = npos[0];
			}
		}

		tickToNextWander = globalTick + getWanderTime();
		if (hasPos) {
			moveToPos(p);
			lastTick = globalTick;
		}
		return true;

	}

	bool tryToGoTarget() {
		if (hasTarget && globalTick - lastTick >= perMoveTick()) {
			pathVal++;
			if (pathVal == targetPath.size()) {
				findTarget();
				return true;
			}
			lastTick = globalTick;
			moveToPos(targetPath[pathVal]);
			return true;
		}
		return false;
	}

	bool equip(Equipment* equipment) {
		if (equipment == NULL) return false;
		int slot = equipment->slot;
		int emptyIndex = -1;
		for (unsigned i = 0; i < eqipmentSlots[slot].size(); i++) {
			if (eqipmentSlots[slot][i] == NULL) {
				emptyIndex = i;
				break;
			}
		}

		if (emptyIndex == -1) {
			emptyIndex = ran(eqipmentSlots[slot].size());

			if (eqipmentSlots[slot][emptyIndex] != punchs) {
				if (eqipmentSlots[slot][emptyIndex]->buffGroup) {
					eqipmentSlots[slot][emptyIndex]->buffGroup->end(this);
				}
				delete eqipmentSlots[slot][emptyIndex];
				eqipmentSlots[slot][emptyIndex] = NULL;
			}
		}

		eqipmentSlots[slot][emptyIndex] = equipment;
		if (equipment->buffGroup) {
			equipment->buffGroup->start(this);
		}

		if (slot == WEAPON) {
			weapon = (Weapon*)equipment;
		}

		return true;
	}





};