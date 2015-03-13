#pragma once

#include "Tools.h"
#include "Maze.h"
#include "Skill.h"
#include "CreatureGen.h"

extern int nextCreatureIndex;
extern Maze maze;
extern float globalTick;
extern bool tickGame;
extern bool gotoNextLevel;

class Creature;
extern vector<Creature*> creatures;
extern vector<DroppedEquipment> droppedEquipments;

#define IS_VISIBLE 1
#define IS_VISIBLE_BEFORE 2
#define IS_WALKED_ON 4
#define HAS_NEW_ITEM 8

enum {
	CONTROL_MOVE_TO_TARGET = 1,
	CONTROL_ATTACK_CREATURE,
	CONTROL_USE_SKILL,
	CONTROL_USE_CONSUMEABLE,
	CONTROL_RUNAWAY,
	CONTROL_APPROACH_TARGET,
	CONTROL_LOOT,
	CONTROL_EXPLORE,
	CONTROL_WAIT,
	CONTROL_WANDER
};

class Equipment {
public:
	int enchantCount;
	Equipment() {
		enchantCount = 0;
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

	virtual int goodness(Creature* creature) {
		int g = 1;
		if (buffGroup) g += buffGroup->goodness(creature);
		return g;
	}
};

enum {
	POTION = 0,
	SCROLL,
	BOOK,
};

class Consumable : public Equipment {
public:
	int consumeType;
	int count;
	Skill* skill;

	int goodness(Creature* creature) override {
		return 1;
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


	int goodness(Creature* creature) override {
		int g = 1 + (int)(((minDamage + maxDamage)*10.0f) / swingTime) + range * 3;
		if (buffGroup) g += buffGroup->goodness(creature);
		return g;
	}
};

class Creature {
public:
	bool beingControlled;
	bool controlActionDone;
	int controlAction;
	Pos controlSkillPos;
	int controlUseItemIndex;
	int controlUseSkillIndex;

	int index;
	int lastAttackCreatureIndex;

	int type;
	int oldType;
	int masterIndex;
	float lastTick;
	bool explores;
	bool wandersAround;
	bool levelsUp;
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
	vector<vector<Equipment*>> equipmentSlots;

	int level;

	int minExp;
	int maxExp;
	int expToNextLevel;
	int exp;

	vector<Pos> lootsToCheck;

	vector<Pos> visible;
	char mazeState[MAZE_W][MAZE_H];

	Pos moveTarget;
	vector<Pos> targetPath;
	unsigned pathVal;

	bool hasTarget;
	bool goBelow;
	bool isDead;

	float sight;
	float movePerTick;
	float lastRegenTick;
	float lastManaRegenTick;
	float lastConsumeTick;

	float tickToNextWander;

	float tickToRegen;
	float tickToManaRegen;
	int consumes;
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
		return tickToRegen * 1000.0f / hpRegenMult;
	}
	float getTickToManaRegen() {
		return tickToManaRegen * 1000.0f / mpRegenMult;
	}

	int getExp() {
		return rani(minExp, maxExp);
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
	bool loots;
	bool cantAttack;
	float wanderMult;
	int getWeaponDamage() {
		return (weapon->getDamage()*damageMult) / 1000 + damageBoost;
	}

	int DR;

	int getDR() {
		return DR;
	}

	float perMoveTick() {
		return movePerTick * 1000.0f / moveSpeedMult;
	}

	float perAttackTick() {
		return weapon->swingTime * 1000.0f / attackSpeedMult;
	}

	float getWanderTime() {
		return ranf(20.0f, 60.0f)*wanderMult;
	}

	bool evaded() {
		return ran(1000) > chanceToHit;
	}

	Creature() {
		beingControlled = false;
		controlActionDone = false;
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
		expToNextLevel = 200;
		exp = 0;
		tickToRegen = 150.0f;
		tickToManaRegen = 100.0f;
		wanderMult = 1.0f;
		explores = wandersAround = false;
		consumes = false;
		isDead = true;
		loots = false;
		cantAttack = false;
		equipmentSlots.resize(EQ_MAX);
		equipmentSlots[WEAPON].resize(1);
		equipmentSlots[ARMOR].resize(1);
		equipmentSlots[SHIELD].resize(1);
		equipmentSlots[HELM].resize(1);
		equipmentSlots[GLOVES].resize(1);
		equipmentSlots[BOOTS].resize(1);
		equipmentSlots[RING].resize(2);
		equipmentSlots[AMULET].resize(1);
		equipmentSlots[CONSUMABLE].resize(100);
		DR = 0;
		levelsUp = false;
		punchs = new Weapon();
		punchs->slot = WEAPON;
		punchs->minDamage = 2 + level;
		punchs->maxDamage = punchs->minDamage + 4 + level;
		punchs->swingTime = 15.0f;
		strcpy_s(punchs->name, "Punchs");
		punchs->range = 1;

		for (unsigned i = 0; i < equipmentSlots.size(); i++) {
			for (unsigned j = 0; j < equipmentSlots[i].size(); j++) {
				equipmentSlots[i][j] = NULL;
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
		for (unsigned i = 0; i < equipmentSlots.size(); i++) {
			for (unsigned j = 0; j < equipmentSlots[i].size(); j++) {
				if (equipmentSlots[i][j] != NULL) {
					if (equipmentSlots[i][j]->buffGroup != NULL) {
						equipmentSlots[i][j]->buffGroup->end(this);
						delete equipmentSlots[i][j]->buffGroup;
					}
					delete equipmentSlots[i][j];
				}
			}
		}
		for (unsigned i = 0; i < visible.size(); i++) {
			if (mazeState[visible[i].x][visible[i].y] | IS_VISIBLE) {
				globalVisible[visible[i].x][visible[i].y]--;
			}
		}
		if (weapon != punchs) {
			delete punchs;
		}
	}
	void resetPos(Pos pos) {
		this->pos = pos;
		maze.walls[pos.x][pos.y] = type;
		memset(mazeState, 0, sizeof(mazeState));
		visible.clear();
		updateVisibility();
		goBelow = false;
	}

	void reset(Pos pos) {
		if (pos.x >= 0) {
			resetPos(pos);
		}

		lastTick = globalTick;
		lastRegenTick = globalTick;
		lastManaRegenTick = globalTick;
		timeToNextSkill = globalTick - 100;
		lastConsumeTick = globalTick - 100;
		isDead = false;
		hasTarget = false;
		tickToNextWander = globalTick + getWanderTime();
		timeToRunAgain = globalTick;
		wantsToRun = false;
		fearless = 0;
		hp = hpMax;
		mp = mpMax;
		goBelow = false;

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

	}

	void updateVisibility() {
		for (unsigned i = 0; i < visible.size(); i++) {
			if (mazeState[visible[i].x][visible[i].y] | IS_VISIBLE) {
				globalVisible[visible[i].x][visible[i].y]--;
			}
			mazeState[visible[i].x][visible[i].y] = (~IS_VISIBLE) & mazeState[visible[i].x][visible[i].y];
		}
		doExplore<MAZE_W, MAZE_H>(pos, maze.walls, seeWeights, getSight(), visible, false, true);
		for (unsigned i = 0; i < visible.size(); i++) {
			mazeState[visible[i].x][visible[i].y] |= IS_VISIBLE | IS_VISIBLE_BEFORE;
			globalVisible[visible[i].x][visible[i].y]++;
		}
	}

	void findTarget() {
		hasTarget = true;
		goBelow = false;
		if (explores) {
			vector<Pos> npos;
			doExplore<MAZE_W, MAZE_H>(pos, maze.walls, addWeights, 20.0f, npos);
			pathVal = 0;

			unsigned j = ran(npos.size());
			for (unsigned k = 0; k < npos.size(); k++) {
				int i = (k + j) % (npos.size());

				Pos p = npos[i];

				if ((mazeState[p.x][p.y] & IS_VISIBLE_BEFORE) == 0 && maze.walls[p.x][p.y] == 0) {
					moveTarget = p;
					findPath<MAZE_W, MAZE_H>(pos, moveTarget, maze.walls, addWeights, 100000.0f, targetPath);
					return;
				}
			}
			npos.clear();
			doExplore<MAZE_W, MAZE_H>(pos, maze.walls, addWeights, 40.0f, npos);
			pathVal = 0;

			j = ran(npos.size());
			for (unsigned k = 0; k < npos.size(); k++) {
				int i = (k + j) % (npos.size());

				Pos p = npos[i];

				if ((mazeState[p.x][p.y] & IS_VISIBLE_BEFORE) == 0 && maze.walls[p.x][p.y] == 0) {
					moveTarget = p;
					findPath<MAZE_W, MAZE_H>(pos, moveTarget, maze.walls, addWeights, 100000.0f, targetPath);
					return;
				}
			}

			for (int i = 0; i < MAZE_W; i++) {
				for (int j = 0; j < MAZE_H; j++) {
					if ((mazeState[i][j] & IS_VISIBLE_BEFORE) == 0 && maze.walls[i][j] == 0) {
						moveTarget = Pos(i, j);

						findPath<MAZE_W, MAZE_H>(pos, moveTarget, maze.walls, addWeights, 100000.0f, targetPath);

						return;
					}
				}
			}
			moveTarget = maze.downstairs;
			findPath<MAZE_W, MAZE_H>(pos, moveTarget, maze.walls, addWeights, 100000.0f, targetPath);
			goBelow = true;
			return;

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

		if (controlAction == 0) {
			checkSkills();
			checkConsumables();
			checkShouldRunAway();
		}
		if (controlAction == CONTROL_USE_SKILL) {
			if (timeToNextSkill <= globalTick) {
				if (mp >= skills[controlUseSkillIndex]->manaCost && skills[controlUseSkillIndex]->timeToNextCast <= globalTick) {
					skills[controlUseSkillIndex]->doCast(this, allies, enemies, creaturesToAttack, creaturesToMove, controlSkillPos);
					controlActionDone = true;

					mp -= skills[controlUseSkillIndex]->manaCost;
					timeToNextSkill = globalTick + skills[controlUseSkillIndex]->globalSpellDelay;
				}
			}
		}

		if (controlAction == CONTROL_USE_CONSUMEABLE) {
			if (lastConsumeTick + 10.0f <= globalTick) {
				Consumable* consumable = ((Consumable*)equipmentSlots[CONSUMABLE][controlUseItemIndex]);
				consumable->skill->doCast(this, allies, enemies, creaturesToAttack, creaturesToMove, controlSkillPos);
				consumable->count--;
				controlActionDone = true;

				if (consumable->consumeType == POTION) {
					sprintf_s(buff, "%s#%d quaffs a %s.", name, index, consumable->name);
				} else 	if (consumable->consumeType == SCROLL) {
					sprintf_s(buff, "%s#%d reads %s.", name, index, consumable->name);
				} else 	if (consumable->consumeType == BOOK) {
					sprintf_s(buff, "%s#%d read a page from %s.", name, index, consumable->name);
				}

				if (consumable->count == 0) {
					delete consumable;
					equipmentSlots[CONSUMABLE][controlUseItemIndex] = NULL;
				}
				lastConsumeTick = globalTick;
			}
		}

		tickBuffs();


		if (wantsToRun && controlAction == 0 || controlAction == CONTROL_RUNAWAY) {
			if (tryToRun()) {
				return true;
			}
		} else if (creaturesToAttack.size() > 0 && controlAction == 0 || controlAction == CONTROL_ATTACK_CREATURE) {
			if (tryToAttack()) {
				return true;
			}
		} else	if (creaturesToMove.size() > 0 && controlAction == 0 || controlAction == CONTROL_APPROACH_TARGET) {
			if (tryToApproach()) {
				return true;
			}
		} else if (lootsToCheck.size() > 0 && controlAction == 0 || controlAction == CONTROL_LOOT) {
			if (pos == lootsToCheck.back()) {
				vector<Equipment*> droppedNow;
				for (unsigned i = 0; i < droppedEquipments.size(); i++) {
					if (droppedEquipments[i].p == pos) {
						Equipment* old = NULL;
						char p[128];
						strcpy_s(p, droppedEquipments[i].e->name);
						int slot = droppedEquipments[i].e->slot;
						if (checkEquip(droppedEquipments[i].e, &old)) {

							if (old) {
								droppedNow.push_back(old);
								sprintf_s(buff, "%s#%d drops %s and equips %s!", name, index, old->name, p);
							} else {
								if (slot != CONSUMABLE)
									sprintf_s(buff, "%s#%d equips %s!", name, index, p);
								else
									sprintf_s(buff, "%s#%d loots %s!", name, index, p);
							}
							pushMessage(buff);

							droppedEquipments[i] = droppedEquipments[droppedEquipments.size() - 1];
							i--;
							droppedEquipments.resize(droppedEquipments.size() - 1);
							continue;
						}
					}
				}
				for (unsigned i = 0; i < droppedNow.size(); i++) {
					dropEquipment(droppedNow[i]);
				}

				hasTarget = false;
				mazeState[pos.x][pos.y] &= ~HAS_NEW_ITEM;
				lootsToCheck.resize(lootsToCheck.size() - 1);
				return true;
			} else if (hasTarget == false || moveTarget != lootsToCheck.back()) {

				moveTarget = lootsToCheck.back();
				tryToPath(moveTarget);
				return true;
			}
		} else if (hp < hpMax / 3 && controlAction == 0 || controlAction == CONTROL_WAIT) {
			if (controlAction == CONTROL_WAIT) {
				controlActionDone = true;
			}
			return false;
		} else if (!hasTarget && explores && controlAction == 0 || controlAction == CONTROL_EXPLORE) {
			findTarget();
		} else if (wandersAround && tickToNextWander < globalTick &&  globalTick - lastTick >= perMoveTick() && (controlAction == 0 || controlAction == CONTROL_WANDER)) {
			if (tryToWander()) {
				return true;
			}
		}

		if (hasTarget) {
			if (tryToGoTarget()) {
				return true;
			}

		}
		return false;
	}

	void doDamage(int damager, int damage) {
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
			for (unsigned i = 0; i < creatures.size(); i++) {
				if (damager == creatures[i]->index) {
					creatures[i]->addExp(getExp());
				}
			}
			die();
		}
	}

	void addExp(int add) {
		if (!levelsUp) return;
		exp += add;
		while (exp >= expToNextLevel) {
			level++;
			exp -= expToNextLevel;
			expToNextLevel = (int)(1.2f*expToNextLevel);
			expToNextLevel += 200;
			hpMax += 5 + ran(10);
			mpMax += 5 + ran(5);
			if (level % 3 == 0) {
				DR++;
			}
			if (level % 2) {
				damageBoost += 2 + ran(3);
			}

			if (level % 3) {
				chanceToHit -= 10;
				attackSpeedMult += 5;
				moveSpeedMult += 5;
			}
			hp = hpMax;
			mp = mpMax;

			sprintf_s(buff, "%s#%d leveled to level %d!", name, index, level);
			pushMessage(buff);
		}
	}

	bool checkEquip(Equipment* toEquip, Equipment** old) {
		int good = toEquip->goodness(this);
		if (good <= 0) {
			return false;
		} else {
			int minIndex = -1;
			int minGood = 10000000;
			for (unsigned i = 0; i < equipmentSlots[toEquip->slot].size(); i++) {
				if (equipmentSlots[toEquip->slot][i] == NULL) {
					minIndex = i;
					minGood = 0;
				} else {
					int goodEquiped = equipmentSlots[toEquip->slot][i]->goodness(this);
					if (goodEquiped < minGood) {
						minIndex = i;
						minGood = goodEquiped;
					}
				}
			}

			if (minGood < good && minIndex != -1) {
				*old = equipmentSlots[toEquip->slot][minIndex];
				deequip(equipmentSlots[toEquip->slot][minIndex]);
				equip(toEquip);
				return true;
			}
			return false;
		}
	}

	void dropEquipment(Equipment* e) {
		if (e == NULL) return;
		for (unsigned i = 0; i < creatures.size(); i++) {
			creatures[i]->mazeState[pos.x][pos.y] |= HAS_NEW_ITEM;
		}
		DroppedEquipment eq;
		eq.e = e;
		eq.p = pos;
		droppedEquipments.push_back(eq);
	}

	void die() {
		for (unsigned i = 0; i < skills.size(); i++) {
			skills[i]->silent = true;
		}
		for (unsigned i = 0; i < buffs.size(); i++) {
			buffs[i]->setSilent();
		}
		for (unsigned i = 0; i < equipmentSlots.size(); i++) {
			for (unsigned j = 0; j < equipmentSlots[i].size(); j++) {
				if (equipmentSlots[i][j] != punchs && equipmentSlots[i][j] != NULL && ran(1) == 0) {
					Equipment*e = equipmentSlots[i][j];
					equipmentSlots[i][j] = NULL;
					dropEquipment(e);
				}
			}
		}
		if (ran(3) == 0) {
			dropEquipment(getConsumable(level));
		}
		isDead = true;
		maze.walls[pos.x][pos.y] = 0;

		sprintf_s(buff, "%s#%d dies!", name, index);
		pushMessage(buff);
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
		if (cantAttack) {
			return;
		}
		for (unsigned i = 0; i < visible.size(); i++) {
			Pos p = visible[i];
			if (p == pos) continue;

			if (loots && mazeState[visible[i].x][visible[i].y] & HAS_NEW_ITEM) {
				lootsToCheck.push_back(p);
				mazeState[visible[i].x][visible[i].y] &= ~HAS_NEW_ITEM;
			}

			if (maze.walls[p.x][p.y] > 10) {
				Creature* creature = NULL;
				for (unsigned i = 0; i < creatures.size(); i++) {
					if (creatures[i]->pos == p) {
						creature = creatures[i];
						break;
					}
				}
				if (creature == NULL) continue;
				if (isEnemy(creature->type)) {
					enemies.push_back(creature);
					if (controlAction == 0) hasTarget = false;

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

	void checkConsumables() {
		if (!consumes) {
			return;
		}
		if (lastConsumeTick + 10.0f <= globalTick) {
			int itemToConsume = -1;
			int maxPriority = -1;
			for (unsigned i = 0; i < equipmentSlots[CONSUMABLE].size(); i++) {
				Consumable* consumable = ((Consumable*)equipmentSlots[CONSUMABLE][i]);
				if (!consumable) continue;
				int skillVal = consumable->skill->shouldCast(this, allies, enemies, creaturesToAttack, creaturesToMove);
				if (skillVal && skillVal > maxPriority) {
					maxPriority = skillVal;
					itemToConsume = i;
				}
			}

			if (itemToConsume != -1) {
				Consumable* consumable = ((Consumable*)equipmentSlots[CONSUMABLE][itemToConsume]);


				if (consumable->consumeType == POTION) {
					sprintf_s(buff, "%s#%d quaffs a %s.", name, index, consumable->name);
				} else 	if (consumable->consumeType == SCROLL) {
					sprintf_s(buff, "%s#%d reads %s.", name, index, consumable->name);
				} else 	if (consumable->consumeType == BOOK) {
					sprintf_s(buff, "%s#%d read a page from %s.", name, index, consumable->name);
				}

				pushMessage(buff);


				consumable->skill->doCast(this, allies, enemies, creaturesToAttack, creaturesToMove, Pos(-1, -1));
				consumable->count--;
				if (consumable->count == 0) {
					delete consumable;
					equipmentSlots[CONSUMABLE][itemToConsume] = NULL;
				}
				lastConsumeTick = globalTick;
			}
		}
	}
	void checkSkills() {
		if (timeToNextSkill <= globalTick) {
			int skillToCast = -1;
			int maxPriority = -1;
			for (unsigned i = 0; i < skills.size(); i++) {
				if (mp >= skills[i]->manaCost && skills[i]->timeToNextCast <= globalTick) {
					int skillVal = skills[i]->shouldCast(this, allies, enemies, creaturesToAttack, creaturesToMove);
					if (skillVal && skillVal > maxPriority) {
						maxPriority = skillVal;
						skillToCast = i;
					}
				}
			}
			if (skillToCast != -1) {
				mp -= skills[skillToCast]->manaCost;
				skills[skillToCast]->doCast(this, allies, enemies, creaturesToAttack, creaturesToMove, Pos(-1, -1));
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
					foundPos = findPath<MAZE_W, MAZE_H>(pos, moveTarget = safePos[i], maze.walls, moveWeights, dist(pos, moveTarget)*2.0f, targetPath);
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
			if (controlAction == CONTROL_ATTACK_CREATURE) {
				controlActionDone = true;
				controlAction = 0;
			}
			if (!creaturesToAttack[attackIndex]->evaded()) {
				int damage = getWeaponDamage();

				sprintf_s(buff, "%s#%d attacks %s#%d with its %s for %d damage!", name, index, creaturesToAttack[attackIndex]->name, creaturesToAttack[attackIndex]->index, weapon->name, damage);
				pushMessage(buff);

				for (unsigned i = 0; i < attackListeners.size(); i++) {
					attackListeners[i]->attacked(this, creaturesToAttack[attackIndex], damage);
				}
				creaturesToAttack[attackIndex]->doDamage(index, damage);

			} else {

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

			bool found = p != pos && findPath<MAZE_W, MAZE_H>(pos, p, maze.walls, moveWeights, dist(p, pos)*2.0f, targetPath);
			if (found) {
				moveToPos(targetPath[0]);
				lastTick = globalTick;
				return true;
			} else {
				int min = 100000;
				int min_i = -1;
				for (unsigned i = 0; i < creaturesToMove.size(); i++) {
					int d = dist(pos, creaturesToMove[i]->pos);
					if (min > d) {
						min = d;
						min_i = i;
					}
				}
				if (min_i != -1) {
					found = findPath<MAZE_W, MAZE_H>(pos, creaturesToMove[min_i]->pos, maze.walls, moveWeights, min*2.0f, targetPath);
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

				hasPos = findPath<MAZE_W, MAZE_H>(pos, p, maze.walls, moveWeights, dist(pos, p)*2.0f, npos);
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
				hasPos = findPath<MAZE_W, MAZE_H>(pos, p, maze.walls, moveWeights, dist(pos, p)*2.0f, npos);
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
				findPath<MAZE_W, MAZE_H>(pos, p, maze.walls, moveWeights, dist(pos, p)*2.0f, npos);
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
	bool tryToPath(Pos p) {
		moveTarget = p;
		hasTarget = false;
		if (findPath<MAZE_W, MAZE_H>(pos, moveTarget, maze.walls, addWeights, 100000.0f, targetPath)) {
			hasTarget = true;
			pathVal = 0;
			return true;
		}
		return false;
	}


	bool tryToGoTarget() {
		if (hasTarget && globalTick - lastTick >= perMoveTick()) {
			if (pos == moveTarget || pathVal >= targetPath.size()) {
				if (pos == moveTarget && goBelow) {
					gotoNextLevel = true;
					hasTarget = false;
					return true;
				}
				if (controlAction == CONTROL_MOVE_TO_TARGET) {
					controlActionDone = true;
				}
				hasTarget = false;
				return true;
			}
			lastTick = globalTick;
			bool moved = moveToPos(targetPath[pathVal]);
			if (!moved && controlAction == CONTROL_MOVE_TO_TARGET) {
				controlActionDone = true;
			}
			pathVal++;
			return true;
		}
		return false;
	}

	bool deequip(Equipment *&equipment) {
		if (equipment == NULL) return false;
		if (equipment->buffGroup) {
			equipment->buffGroup->end(this);
		}
		equipment = NULL;
		return true;
	}


	bool equip(Equipment* equipment) {
		if (equipment == NULL) return false;
		int slot = equipment->slot;
		if (slot == CONSUMABLE) {
			for (unsigned i = 0; i < equipmentSlots[slot].size(); i++) {
				if (equipmentSlots[slot][i] != NULL && equipmentSlots[slot][i]->type == equipment->type) {
					((Consumable*)equipmentSlots[slot][i])->count++;
					delete equipment;
					return true;
				}
			}
		}

		int emptyIndex = -1;
		for (unsigned i = 0; i < equipmentSlots[slot].size(); i++) {
			if (equipmentSlots[slot][i] == NULL) {
				emptyIndex = i;
				break;
			}
		}

		if (emptyIndex == -1) {
			emptyIndex = ran(equipmentSlots[slot].size());
			Equipment* e = equipmentSlots[slot][emptyIndex];
			if (e != punchs) {
				deequip(equipmentSlots[slot][emptyIndex]);
				delete e;
			}
		}

		equipmentSlots[slot][emptyIndex] = equipment;
		if (equipment->buffGroup) {
			equipment->buffGroup->start(this);
		}

		if (slot == WEAPON) {
			weapon = (Weapon*)equipment;
		}
		return true;
	}
};