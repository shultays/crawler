#pragma once

#include "Tools.h"

class Creature;
extern int nextBuffIndex;

class Buff {
public:
	int buffIndex;
	char name[32];
	bool ended;
	float tickToDie;
	Buff(Creature* creature) {
		ended = true;
		buffIndex = nextBuffIndex++;
	}
	virtual void start(Creature* creature) = 0;
	virtual void end(Creature* creature) = 0;
	virtual void tick(Creature* creature) = 0;
	virtual void printStats(Creature* creature, int &x, int &y) = 0;

};

class Berserk : public Buff {
public:
	int damageMultBoost = 0;
	Berserk(Creature* creature) : Buff(creature) {
		strcpy_s(name, "Berserker");
	}

	void printStats(Creature* creature, int &x, int &y) {
		attrset(COLOR_PAIR(getColorIndex(0, 7, 0)));
		mvaddch(x + 1, y, ACS_UARROW);
		mvaddch(x + 2, y, ACS_UARROW);
		mvaddch(x + 3, y, ACS_UARROW);

		mvprintw(x++, y, "%s", name);
		mvprintw(x++, y + 2, "DR : %d", 2);
		mvprintw(x++, y + 2, "DMG BST : +%d", 5);
		mvprintw(x++, y + 2, "DMG MLT : +%%%d", 10);
		attrset(COLOR_PAIR(getColorIndex(7, 0, 0)));
		mvprintw(x++, y + 2, "FEARLESS");
	}

	void start(Creature* creature);
	void end(Creature* creature);

	void tick(Creature* creature);
};



class SlowBuff : public Buff {
public:
	int damageMultBoost = 0;
	int attackSlow;
	int moveSlow;

	SlowBuff(Creature* creature) : Buff(creature) {
		strcpy_s(name, "Slowed");
	}

	void printStats(Creature* creature, int &x, int &y) {
		attrset(COLOR_PAIR(getColorIndex(7, 0, 0)));
		mvaddch(x + 1, y, ACS_DARROW);
		mvaddch(x + 2, y, ACS_DARROW);

		mvprintw(x++, y, "%s", name);
		mvprintw(x++, y + 2, "ATK SPD : +%%%d", 10);
		mvprintw(x++, y + 2, "MV SPD : +%%%d", 10);
	}

	void start(Creature* creature);
	void end(Creature* creature);

	void tick(Creature* creature);
};


class DamageOverTime : public Buff {
public:
	float dotTick;
	float timeToNextDamage;
	int dotDamage;

	DamageOverTime(Creature* creature) : Buff(creature) {
		strcpy_s(name, "");
	}

	void printStats(Creature* creature, int &x, int &y) {
		attrset(COLOR_PAIR(getColorIndex(7, 0, 0)));
		mvprintw(x++, y, "%s", name);
	}
	void start(Creature* creature);
	void end(Creature* creature);

	void tick(Creature* creature);
};


class PoisonBladeBuff : public Buff, public AttackListener {
public:
	int poisonDamage = 0;
	float ticksPerDamage;
	PoisonBladeBuff(Creature* creature) : Buff(creature) {
		strcpy_s(name, "Poisoned Blade");
		poisonDamage = 4;
		ticksPerDamage = 30;
	}

	void printStats(Creature* creature, int &x, int &y) {
		attrset(COLOR_PAIR(getColorIndex(0, 7, 0)));

		mvprintw(x++, y, "%s", name);
	}

	void start(Creature* creature);
	void end(Creature* creature);

	void tick(Creature* creature);


	void PoisonBladeBuff::attacked(Creature* attacker, Creature* defender, int damage);
};

class Skill {
public:
	char name[32];
	int manaCost;
	float timeToNextCast;
	float globalSpellDelay;
	float delayToNextCast;

	Skill(Creature* creature);
	virtual int shouldCast(Creature* creature, vector<Creature*>& allies, vector<Creature*>& enemies, vector<Creature*>& enemiesToAttack, vector<Creature*>& enemiesToMove) = 0;
	virtual void doCast(Creature* creature, vector<Creature*>& allies, vector<Creature*>& enemies, vector<Creature*>& enemiesToAttack, vector<Creature*>& enemiesToMove) = 0;
};


class BerserkSkill : public Skill {
public:
	BerserkSkill(Creature* creature) : Skill(creature) {
		strcpy_s(name, "Berserk");
		manaCost = 40;
		delayToNextCast = 500.0f;
		globalSpellDelay = 20.0f;
	}

	int shouldCast(Creature* creature, vector<Creature*>& allies, vector<Creature*>& enemies, vector<Creature*>& enemiesToAttack, vector<Creature*>& enemiesToMove);
	void doCast(Creature* creature, vector<Creature*>& allies, vector<Creature*>& enemies, vector<Creature*>& enemiesToAttack, vector<Creature*>& enemiesToMove);
};


class SingleTargetSkill : public Skill {
public:
	Creature* creatureToCast;
	SingleTargetSkill(Creature* creature) : Skill(creature) {

	}
	int shouldCast(Creature* creature, vector<Creature*>& allies, vector<Creature*>& enemies, vector<Creature*>& enemiesToAttack, vector<Creature*>& enemiesToMove);
	void doCast(Creature* creature, vector<Creature*>& allies, vector<Creature*>& enemies, vector<Creature*>& enemiesToAttack, vector<Creature*>& enemiesToMove) = 0;
};

class LightingSkill : public SingleTargetSkill {
public:
	int minDamage;
	int maxDamage;
	LightingSkill(Creature* creature) : SingleTargetSkill(creature) {
		strcpy_s(name, "Lighting");
		manaCost = 30;
		minDamage = 2;
		maxDamage = 20;
		delayToNextCast = 20.0f;
		globalSpellDelay = 20.0f;
	}
	void doCast(Creature* creature, vector<Creature*>& allies, vector<Creature*>& enemies, vector<Creature*>& enemiesToAttack, vector<Creature*>& enemiesToMove);
};


class FireBallSkill : public SingleTargetSkill {
public:
	int minDamage;
	int maxDamage;
	FireBallSkill(Creature* creature) : SingleTargetSkill(creature) {
		strcpy_s(name, "Fire Ball");
		manaCost = 30;
		minDamage = 4;
		maxDamage = 10;
		delayToNextCast = 20.0f;
		globalSpellDelay = 20.0f;
	}
	void doCast(Creature* creature, vector<Creature*>& allies, vector<Creature*>& enemies, vector<Creature*>& enemiesToAttack, vector<Creature*>& enemiesToMove);
};

class IceBoltSkill : public SingleTargetSkill {
public:
	int minDamage;
	int maxDamage;
	IceBoltSkill(Creature* creature) : SingleTargetSkill(creature) {
		strcpy_s(name, "Ice Bolt");
		manaCost = 30;
		minDamage = 4;
		maxDamage = 10;
		delayToNextCast = 20.0f;
		globalSpellDelay = 20.0f;
	}
	void doCast(Creature* creature, vector<Creature*>& allies, vector<Creature*>& enemies, vector<Creature*>& enemiesToAttack, vector<Creature*>& enemiesToMove);
};


class PoisonBladeSkill : public Skill {
public:
	PoisonBladeSkill(Creature* creature) : Skill(creature) {
		strcpy_s(name, "Lighting");
		manaCost = 0;
		delayToNextCast = 200.0f;
		globalSpellDelay = 2.0f;
	}
	int shouldCast(Creature* creature, vector<Creature*>& allies, vector<Creature*>& enemies, vector<Creature*>& enemiesToAttack, vector<Creature*>& enemiesToMove);
	void doCast(Creature* creature, vector<Creature*>& allies, vector<Creature*>& enemies, vector<Creature*>& enemiesToAttack, vector<Creature*>& enemiesToMove);
};


class HealSkill : public Skill {
public:
	Creature* creatureToHeal;
	int healMin;
	int healMax;

	HealSkill(Creature* creature) : Skill(creature) {
		strcpy_s(name, "Heal");
		manaCost = 20;
		delayToNextCast = 500.0f;
		globalSpellDelay = 10.0f;
		healMin = 20;
		healMax = 40;
	}

	int shouldCast(Creature* creature, vector<Creature*>& allies, vector<Creature*>& enemies, vector<Creature*>& enemiesToAttack, vector<Creature*>& enemiesToMove);
	void doCast(Creature* creature, vector<Creature*>& allies, vector<Creature*>& enemies, vector<Creature*>& enemiesToAttack, vector<Creature*>& enemiesToMove);
};