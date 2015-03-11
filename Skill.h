#pragma once

#include "Tools.h"

class Creature;
extern int nextBuffIndex;

class Buff {
public:
	int buffIndex;
	char name[32];
	bool ended;
	bool showText;
	float tickToDie;
	Buff() {
		ended = true;
		showText = false;
		buffIndex = nextBuffIndex++;
	}
	virtual void start(Creature* creature) = 0;
	virtual void end(Creature* creature) = 0;
	virtual void tick(Creature* creature) = 0;
	virtual void printStats(Creature* creature, int &x, int &y) = 0;
	virtual int goodness(Creature* creature) = 0;
	virtual ~Buff() {}
	virtual void improve() = 0;
};

class DRBuff : public Buff {
public:
	int buff;

	DRBuff(int buff) : Buff() {
		name[0] = 0;
		this->buff = buff;
	}

	void improve() {
		buff++;
		if (buff == 0) buff = 1;
	}

	void printStats(Creature* creature, int &x, int &y) {
		int good = goodness(creature);
		attrset(COLOR_PAIR(getColorIndex(good <= 0 ? 7 : 0, good >= 0 ? 7 : 0, 0)));

		int ypush = 0;
		if (name[0]) {
			mvprintw(x++, y + 2, "%s", name);
			ypush = 1;
		}
		mvaddch(x, y + ypush, good ? ACS_UARROW : ACS_DARROW);
		mvprintw(x++, y + 2 + ypush, "DR : %c%d", good ? '+' : '-', abs(buff));
	}

	void start(Creature* creature);
	void end(Creature* creature);
	void tick(Creature* creature) {}

	int goodness(Creature* creature) {
		return buff * 4;
	}
};

class HpBuff : public Buff {
public:
	int buff;

	HpBuff(int buff) : Buff() {
		name[0] = 0;
		this->buff = buff;
	}

	void improve() {
		buff += rani(1, 3) * 5;
		if (buff == 0) buff = 5;
	}
	void printStats(Creature* creature, int &x, int &y) {
		int good = goodness(creature);
		attrset(COLOR_PAIR(getColorIndex(good <= 0 ? 7 : 0, good >= 0 ? 7 : 0, 0)));

		int ypush = 0;
		if (name[0]) {
			mvprintw(x++, y + 2, "%s", name);
			ypush = 1;
		}
		mvaddch(x, y + ypush, good ? ACS_UARROW : ACS_DARROW);
		mvprintw(x++, y + 2 + ypush, "HP : %c%d", good ? '+' : '-', abs(buff));
	}

	void start(Creature* creature);
	void end(Creature* creature);
	void tick(Creature* creature) {}

	int goodness(Creature* creature) {
		return (int)((buff > 0 ? 1 : 0) + (buff < 0 ? -1 : 0) + buff * 0.5f);
	}
};

class MpBuff : public Buff {
public:
	int buff;

	MpBuff(int buff) : Buff() {
		name[0] = 0;
		this->buff = buff;
	}

	void improve() {
		buff += rani(1, 3) * 5;
		if (buff == 0) buff = 5;
	}

	void printStats(Creature* creature, int &x, int &y) {
		int good = goodness(creature);
		attrset(COLOR_PAIR(getColorIndex(good <= 0 ? 7 : 0, good >= 0 ? 7 : 0, 0)));

		int ypush = 0;
		if (name[0]) {
			mvprintw(x++, y + 2, "%s", name);
			ypush = 1;
		}
		mvaddch(x, y + ypush, good ? ACS_UARROW : ACS_DARROW);
		mvprintw(x++, y + 2 + ypush, "MP : %c%d", good ? '+' : '-', abs(buff));
	}

	void start(Creature* creature);
	void end(Creature* creature);
	void tick(Creature* creature) {}

	int goodness(Creature* creature) {
		return (int)((buff > 0 ? 1 : 0) + (buff < 0 ? -1 : 0) + buff * 0.35f);
	}
};

class ConstantDMGBoost : public Buff {
public:
	int buff;

	ConstantDMGBoost(int buff) : Buff() {
		name[0] = 0;
		this->buff = buff;
	}

	void improve() {
		buff += rani(1, 5);
		if (buff == 0) buff = 2;
	}
	void printStats(Creature* creature, int &x, int &y) {
		int good = goodness(creature);
		attrset(COLOR_PAIR(getColorIndex(good <= 0 ? 7 : 0, good >= 0 ? 7 : 0, 0)));

		int ypush = 0;
		if (name[0]) {
			mvprintw(x++, y + 2, "%s", name);
			ypush = 1;
		}
		mvaddch(x, y + ypush, good ? ACS_UARROW : ACS_DARROW);
		mvprintw(x++, y + 2 + ypush, "DMG : %c%d", good ? '+' : '-', abs(buff));
	}

	void start(Creature* creature);
	void end(Creature* creature);
	void tick(Creature* creature) {}

	int goodness(Creature* creature) {
		return buff;
	}
};

class DMGBultBuff : public Buff {
public:
	int buff;
	int addedBuff;

	DMGBultBuff(int buff) : Buff() {
		name[0] = 0;
		if (buff < 10) buff = 10;
		this->buff = buff;
	}

	void improve() {
		buff += 5;
		if (buff == 0) buff = 5;
	}

	void printStats(Creature* creature, int &x, int &y) {
		int g = goodness(creature);

		attrset(COLOR_PAIR(getColorIndex(g <= 0 ? 7 : 0, g >= 0 ? 7 : 0, 0)));

		int ypush = 0;
		if (name[0]) {
			mvprintw(x++, y + 2, "%s", name);
			ypush = 1;
		}
		mvaddch(x, y + ypush, g > 0 ? ACS_UARROW : ACS_DARROW);
		mvprintw(x++, y + 2 + ypush, "DMG MLT : %c%%%d", (buff - 100) > 0 ? '+' : '-', abs(buff - 100));
	}

	void start(Creature* creature);
	void end(Creature* creature);
	void tick(Creature* creature) {}

	int goodness(Creature* creature) {
		return (int)((buff > 100 ? 1 : 0) + (buff < 100 ? -1 : 0) + (buff - 100) * 1.5f);
	}
};

class MoveSpeedBuff : public Buff {
public:
	int buff;

	int addedBuff;

	MoveSpeedBuff(int buff) : Buff() {
		name[0] = 0;
		if (buff < 10) buff = 10;
		this->buff = buff;
	}

	void improve() {
		buff += 5;
		if (buff == 0) buff = 5;
	}

	void printStats(Creature* creature, int &x, int &y) {
		int g = goodness(creature);

		attrset(COLOR_PAIR(getColorIndex(g <= 0 ? 7 : 0, g >= 0 ? 7 : 0, 0)));

		int ypush = 0;
		if (name[0]) {
			mvprintw(x++, y + 2, "%s", name);
			ypush = 1;
		}
		mvaddch(x, y + ypush, g > 0 ? ACS_UARROW : ACS_DARROW);
		mvprintw(x++, y + 2 + ypush, "MV SPD : %c%%%d", (buff - 100) > 0 ? '+' : '-', abs(buff - 100));
	}

	void start(Creature* creature);
	void end(Creature* creature);
	void tick(Creature* creature) {}

	int goodness(Creature* creature) {
		return (int)((buff > 100 ? 1 : 0) + (buff < 100 ? -1 : 0) + (buff - 100) * 1.1f);
	}
};

class AtkSpeedBuff : public Buff {
public:
	int buff;
	int addedBuff;

	AtkSpeedBuff(int buff) : Buff() {
		name[0] = 0;
		if (buff < 10) buff = 10;
		this->buff = buff;
	}

	void improve() {
		buff += 5;
		if (buff == 0) buff = 5;
	}

	void printStats(Creature* creature, int &x, int &y) {
		int g = goodness(creature);

		attrset(COLOR_PAIR(getColorIndex(g <= 0 ? 7 : 0, g >= 0 ? 7 : 0, 0)));

		int ypush = 0;
		if (name[0]) {
			mvprintw(x++, y + 2, "%s", name);
			ypush = 1;
		}
		mvaddch(x, y + ypush, g > 0 ? ACS_UARROW : ACS_DARROW);
		mvprintw(x++, y + 2 + ypush, "ATK SPD : %c%%%d", (buff - 100) > 0 ? '+' : '-', abs(buff - 100));
	}

	void start(Creature* creature);
	void end(Creature* creature);
	void tick(Creature* creature) {}

	int goodness(Creature* creature) {
		return (int)((buff > 100 ? 1 : 0) + (buff < 100 ? -1 : 0) + (buff - 100) * 1.2f);
	}
};

class SightBuff : public Buff {
public:
	int buff;

	int addedBuff;

	SightBuff(int buff) : Buff() {
		name[0] = 0;
		if (buff < 20) buff = 10;
		this->buff = buff;
	}

	void improve() {
		buff += 5;
		if (buff == 0) buff = 5;
	}

	void printStats(Creature* creature, int &x, int &y) {
		int g = goodness(creature);

		attrset(COLOR_PAIR(getColorIndex(g <= 0 ? 7 : 0, g >= 0 ? 7 : 0, 0)));

		int ypush = 0;
		if (name[0]) {
			mvprintw(x++, y + 2, "%s", name);
			ypush = 1;
		}
		mvaddch(x, y + ypush, g > 0 ? ACS_UARROW : ACS_DARROW);
		mvprintw(x++, y + 2 + ypush, "Sight : %c%%%d", (buff - 100) > 0 ? '+' : '-', abs(buff - 100));
	}

	void start(Creature* creature);
	void end(Creature* creature);
	void tick(Creature* creature) {}

	int goodness(Creature* creature) {
		return (int)((buff > 100 ? 1 : 0) + (buff < 100 ? -1 : 0) + (buff - 100) * 0.35f);
	}
};

class EvasionBuff : public Buff {
public:
	int buff;
	int addedBuff;
	bool isBlock;

	EvasionBuff(int buff, bool isBlock = false) : Buff() {
		name[0] = 0;
		if (buff < 10) buff = 10;
		this->buff = buff;
		this->isBlock = isBlock;
	}

	void improve() {
		buff -= 5;
		if (buff < 10) buff = 10;
		if (buff == 100) buff = 95;
	}

	void printStats(Creature* creature, int &x, int &y) {
		int g = goodness(creature);

		attrset(COLOR_PAIR(getColorIndex(g <= 0 ? 7 : 0, g >= 0 ? 7 : 0, 0)));

		int ypush = 0;
		if (name[0]) {
			mvprintw(x++, y + 2, "%s", name);
			ypush = 1;
		}
		mvaddch(x, y + ypush, g > 0 ? ACS_UARROW : ACS_DARROW);
		mvprintw(x++, y + 2 + ypush, "%s : %c%%%d", isBlock ? "%BLCK" : "%EVSN", (100 - buff) > 0 ? '+' : '-', abs(100 - buff));
	}

	void start(Creature* creature);
	void end(Creature* creature);
	void tick(Creature* creature) {}

	int goodness(Creature* creature) {
		return -(int)((buff > 100 ? 1 : 0) + (buff < 100 ? -1 : 0) + (buff - 100) * 1.5f);
	}
};

class HpRegenBuff : public Buff {
public:
	int buff;

	int addedBuff;

	HpRegenBuff(int buff) : Buff() {
		name[0] = 0;
		if (buff < 10) buff = 0;
		this->buff = buff;
	}

	void improve() {
		buff += 5;
		if (buff == 100) buff = 105;
	}

	void printStats(Creature* creature, int &x, int &y) {
		int g = goodness(creature);

		attrset(COLOR_PAIR(getColorIndex(g <= 0 ? 7 : 0, g >= 0 ? 7 : 0, 0)));

		int ypush = 0;
		if (name[0]) {
			mvprintw(x++, y + 2, "%s", name);
			ypush = 1;
		}
		mvaddch(x, y + ypush, g > 0 ? ACS_UARROW : ACS_DARROW);
		mvprintw(x++, y + 2 + ypush, "HP RGN : %c%%%d", (buff - 100) > 0 ? '+' : '-', abs(buff - 100));
	}

	void start(Creature* creature);
	void end(Creature* creature);
	void tick(Creature* creature) {}

	int goodness(Creature* creature) {
		return (int)((buff > 100 ? 1 : 0) + (buff < 100 ? -1 : 0) + (buff - 100) * 0.65f);
	}
};

class MpRegenBuff : public Buff {
public:
	int buff;

	int addedBuff;

	MpRegenBuff(int buff) : Buff() {
		name[0] = 0;
		if (buff < 10) buff = 0;
		this->buff = buff;
	}

	void improve() {
		buff += 5;
		if (buff == 100) buff = 105;
	}

	void printStats(Creature* creature, int &x, int &y) {
		int g = goodness(creature);

		attrset(COLOR_PAIR(getColorIndex(g <= 0 ? 7 : 0, g >= 0 ? 7 : 0, 0)));

		int ypush = 0;
		if (name[0]) {
			mvprintw(x++, y + 2, "%s", name);
			ypush = 1;
		}
		mvaddch(x, y + ypush, g > 0 ? ACS_UARROW : ACS_DARROW);
		mvprintw(x++, y + 2 + ypush, "MP RGN : %c%%%d", (buff - 100) > 0 ? '+' : '-', abs(buff - 100));
	}

	void start(Creature* creature);
	void end(Creature* creature);
	void tick(Creature* creature) {}

	int goodness(Creature* creature) {
		return (int)((buff > 100 ? 1 : 0) + (buff < 100 ? -1 : 0) + (buff - 100) * 0.35f);
	}
};

class BuffGroup : public Buff {
public:
	vector<Buff*> buffs;
	BuffGroup() : Buff() {
		name[0] = '\0';
	}
	virtual ~BuffGroup() {
		for (unsigned i = 0; i < buffs.size(); i++) {
			delete buffs[i];
		}
	}

	void improve() {
		for (unsigned i = 0; i < buffs.size(); i++) {
			buffs[i]->improve();
		}
	}

	void printStats(Creature* creature, int &x, int &y) {
		int shift = 0;
		if (name[0]) {
			int g = goodness(creature);
			attrset(COLOR_PAIR(getColorIndex(g <= 0 ? 7 : 0, g >= 0 ? 7 : 0, 0)));
			mvprintw(x++, y, "%s", name);
			shift = 1;
		}

		int t = y + shift;
		for (unsigned i = 0; i < buffs.size(); i++) {
			buffs[i]->printStats(creature, x, t);
		}
	}

	void start(Creature* creature) {
		if (!ended) return;
		ended = false;
		for (unsigned i = 0; i < buffs.size(); i++) {
			buffs[i]->start(creature);
		}
	}
	void end(Creature* creature) {
		if (ended) return;
		ended = true;
		for (unsigned i = 0; i < buffs.size(); i++) {
			buffs[i]->end(creature);
		}
	}
	void tick(Creature* creature) {
		for (unsigned i = 0; i < buffs.size(); i++) {
			buffs[i]->tick(creature);
		}
	}

	int goodness(Creature* creature) {
		int g = 0;
		for (unsigned i = 0; i < buffs.size(); i++) {
			int t = buffs[i]->goodness(creature);
			g += t;
		}
		return g;
	}
};

class Berserk : public BuffGroup {
public:
	Berserk() : BuffGroup() {
		strcpy_s(name, "Berserker");
		buffs.push_back(new AtkSpeedBuff(90));
		buffs.push_back(new DRBuff(4));
		buffs.push_back(new ConstantDMGBoost(5));
		buffs.push_back(new DMGBultBuff(110));
	}

	void printStats(Creature* creature, int &x, int &y) {
		BuffGroup::printStats(creature, x, y);
		attrset(COLOR_PAIR(getColorIndex(7, 0, 0)));
		mvprintw(x++, y + 3, "FEARLESS");
	}

	void start(Creature* creature);
	void end(Creature* creature);
	void tick(Creature* creature) {}

	int goodness(Creature* creature) {
		return BuffGroup::goodness(creature) - 5;
	}
};

class SlowBuff : public BuffGroup {
public:
	SlowBuff(int atkPer = 90, int mvPer = 90) : BuffGroup() {
		strcpy_s(name, "Slowed");
		buffs.push_back(new AtkSpeedBuff(atkPer));
		buffs.push_back(new MoveSpeedBuff(mvPer));
	}

	void SlowBuff::start(Creature* creature);

	void SlowBuff::end(Creature* creature);
};

class DamageOverTime : public Buff {
public:
	float dotTick;
	float timeToNextDamage;
	int dotDamage;

	DamageOverTime() : Buff() {
		strcpy_s(name, "");
	}

	void improve() {
		dotDamage += 2;
	}

	void printStats(Creature* creature, int &x, int &y) {
		attrset(COLOR_PAIR(getColorIndex(goodness(creature) <= 0 ? 7 : 0, goodness(creature) >= 0 ? 7 : 0, 0)));
		mvprintw(x++, y, "%s", name);
	}
	void start(Creature* creature);
	void end(Creature* creature);
	void tick(Creature* creature);

	int goodness(Creature* creature) { return -dotDamage * 3; }
};

class PoisonBladeBuff : public Buff, public AttackListener {
public:
	int poisonDamage;
	float ticksPerDamage;
	PoisonBladeBuff(int poisonDamage = 4) : Buff() {
		strcpy_s(name, "Poisoned Blade");
		this->poisonDamage = poisonDamage;
		ticksPerDamage = 30;
	}
	void improve() {
		this->poisonDamage++;
	}
	void printStats(Creature* creature, int &x, int &y) {
		attrset(COLOR_PAIR(getColorIndex(0, 7, 0)));
		mvprintw(x++, y, "%s", name);
	}

	void start(Creature* creature);
	void end(Creature* creature);
	void tick(Creature* creature) {}

	int goodness(Creature* creature) { return poisonDamage * 3; }

	void PoisonBladeBuff::attacked(Creature* attacker, Creature* defender, int damage);
};

class PlusDamageBuff : public Buff, public AttackListener {
public:
	int minDamage;
	int maxDamage;
	int color;
	char *damageType;

	void improve() {
		int t;
		this->minDamage += t = ran(3) + 3;
		this->minDamage += t + ran(3);
	}

	PlusDamageBuff(int minDamage, int maxDamage, char* damageType, int color) : Buff() {
		sprintf_s(name, "%s damage (%d - %d)", damageType, minDamage, maxDamage);
		this->minDamage = minDamage;
		this->maxDamage = maxDamage;
		this->damageType = damageType;
		this->color = color;
	}

	void printStats(Creature* creature, int &x, int &y) {
		attrset(COLOR_PAIR(color));
		mvprintw(x++, y, "%s", name);
	}

	void start(Creature* creature);
	void end(Creature* creature);
	void tick(Creature* creature) {}

	int goodness(Creature* creature) { return (minDamage + maxDamage) + 1; }

	void attacked(Creature* attacker, Creature* defender, int damage);
};

class CharmedBuff : public Buff {
public:

	void improve() {}

	CharmedBuff() : Buff() {
		strcpy_s(name, "Charmed");
	}

	void printStats(Creature* creature, int &x, int &y) {
		attrset(COLOR_PAIR(getColorIndex(7, 0, 7)));
		mvprintw(x++, y, "%s", name);
	}

	void start(Creature* creature);
	void end(Creature* creature);
	void tick(Creature* creature) {}

	int goodness(Creature* creature) { return 0;; }

};
class Skill {
public:
	char name[32];
	int manaCost;
	float timeToNextCast;
	float globalSpellDelay;
	float delayToNextCast;
	bool print;

	Skill();
	virtual int shouldCast(Creature* creature, vector<Creature*>& allies, vector<Creature*>& enemies, vector<Creature*>& enemiesToAttack, vector<Creature*>& enemiesToMove) = 0;
	virtual void doCast(Creature* creature, vector<Creature*>& allies, vector<Creature*>& enemies, vector<Creature*>& enemiesToAttack, vector<Creature*>& enemiesToMove) = 0;
};

class BerserkSkill : public Skill {
public:
	BerserkSkill() : Skill() {
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
	SingleTargetSkill() : Skill() {

	}
	int shouldCast(Creature* creature, vector<Creature*>& allies, vector<Creature*>& enemies, vector<Creature*>& enemiesToAttack, vector<Creature*>& enemiesToMove);
	void doCast(Creature* creature, vector<Creature*>& allies, vector<Creature*>& enemies, vector<Creature*>& enemiesToAttack, vector<Creature*>& enemiesToMove) = 0;
};

class LightingSkill : public SingleTargetSkill {
public:
	int minDamage;
	int maxDamage;
	LightingSkill() : SingleTargetSkill() {
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
	FireBallSkill() : SingleTargetSkill() {
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
	IceBoltSkill() : SingleTargetSkill() {
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
	int poisonDamage;
	PoisonBladeSkill(int poisonDamage = 4) : Skill() {
		strcpy_s(name, "Lighting");
		manaCost = 0;
		delayToNextCast = 200.0f;
		globalSpellDelay = 2.0f;
		this->poisonDamage;
	}
	int shouldCast(Creature* creature, vector<Creature*>& allies, vector<Creature*>& enemies, vector<Creature*>& enemiesToAttack, vector<Creature*>& enemiesToMove);
	void doCast(Creature* creature, vector<Creature*>& allies, vector<Creature*>& enemies, vector<Creature*>& enemiesToAttack, vector<Creature*>& enemiesToMove);
};

class HealSkill : public Skill {
public:
	Creature* creatureToHeal;
	int healMin;
	int healMax;
	int healPercent;
	int percentHeal;
	bool canHealOther;

	HealSkill(int healMin = 20, int healMax = 40, int percentHeal = 10, bool canHealOther = true) : Skill() {
		strcpy_s(name, "Heal");
		manaCost = 20;
		delayToNextCast = 500.0f;
		globalSpellDelay = 10.0f;
		this->healMin = healMin;
		this->healMax = healMax;
		this->percentHeal = percentHeal;
		this->canHealOther = canHealOther;
	}

	int shouldCast(Creature* creature, vector<Creature*>& allies, vector<Creature*>& enemies, vector<Creature*>& enemiesToAttack, vector<Creature*>& enemiesToMove);
	void doCast(Creature* creature, vector<Creature*>& allies, vector<Creature*>& enemies, vector<Creature*>& enemiesToAttack, vector<Creature*>& enemiesToMove);
};

class ManaSkill : public Skill {
public:
	ManaSkill() : Skill() {
		strcpy_s(name, "Mana");
		manaCost = 20;
		delayToNextCast = 500.0f;
		globalSpellDelay = 10.0f;
	}

	int shouldCast(Creature* creature, vector<Creature*>& allies, vector<Creature*>& enemies, vector<Creature*>& enemiesToAttack, vector<Creature*>& enemiesToMove);
	void doCast(Creature* creature, vector<Creature*>& allies, vector<Creature*>& enemies, vector<Creature*>& enemiesToAttack, vector<Creature*>& enemiesToMove);
};

class RegenSkill : public Skill {
public:
	RegenSkill() : Skill() {
		strcpy_s(name, "Regen");
	}

	int shouldCast(Creature* creature, vector<Creature*>& allies, vector<Creature*>& enemies, vector<Creature*>& enemiesToAttack, vector<Creature*>& enemiesToMove);
	void doCast(Creature* creature, vector<Creature*>& allies, vector<Creature*>& enemies, vector<Creature*>& enemiesToAttack, vector<Creature*>& enemiesToMove);
};


class CharmSkill : public Skill {
public:
	CharmSkill() : Skill() {
		strcpy_s(name, "Heal");
		manaCost = 20;
		delayToNextCast = 500.0f;
		globalSpellDelay = 10.0f;
	}

	int shouldCast(Creature* creature, vector<Creature*>& allies, vector<Creature*>& enemies, vector<Creature*>& enemiesToAttack, vector<Creature*>& enemiesToMove);
	void doCast(Creature* creature, vector<Creature*>& allies, vector<Creature*>& enemies, vector<Creature*>& enemiesToAttack, vector<Creature*>& enemiesToMove);
};

class EnchantSkill : public Skill {
public:
	bool isWeapon;
	int total;
	EnchantSkill(bool isWeapon) : Skill() {
		this->isWeapon = isWeapon;
		if (isWeapon)
			strcpy_s(name, "Enchant Weapon");
		else
			strcpy_s(name, "Enchant Armor");
	}

	int shouldCast(Creature* creature, vector<Creature*>& allies, vector<Creature*>& enemies, vector<Creature*>& enemiesToAttack, vector<Creature*>& enemiesToMove);
	void doCast(Creature* creature, vector<Creature*>& allies, vector<Creature*>& enemies, vector<Creature*>& enemiesToAttack, vector<Creature*>& enemiesToMove);
};

class SummonSkill : public Skill {
public:
	int type;
	SummonSkill(int type) : Skill() {
		this->type = type;
	}

	int shouldCast(Creature* creature, vector<Creature*>& allies, vector<Creature*>& enemies, vector<Creature*>& enemiesToAttack, vector<Creature*>& enemiesToMove);
	void doCast(Creature* creature, vector<Creature*>& allies, vector<Creature*>& enemies, vector<Creature*>& enemiesToAttack, vector<Creature*>& enemiesToMove);
};