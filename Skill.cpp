#include "Skill.h"

#include "Creature.h"

int nextBuffIndex = 0;

Skill::Skill(Creature* creature) {
	timeToNextCast = globalTick;
}


void SlowBuff::start(Creature* creature) {
	if (!ended) return;
	ended = false;
	char buff[128];
	sprintf_s(buff, "%s#%d is slowed!", creature->name, creature->index);
	pushMessage(buff);

	attackSlow = creature->attackSpeedMult / 10;
	creature->attackSpeedMult += attackSlow;
	moveSlow = creature->moveSpeedMult / 10;
	creature->moveSpeedMult += moveSlow;

	tickToDie = globalTick + 120.0f;
}

void SlowBuff::end(Creature* creature) {
	if (ended) return;
	ended = true;

	creature->attackSpeedMult -= attackSlow;
	creature->moveSpeedMult -= moveSlow;

	char buff[128];
	sprintf_s(buff, "%s#%d is no longer slowed.", creature->name, creature->index);
	pushMessage(buff);
}

void SlowBuff::tick(Creature* creature) {}


void Berserk::start(Creature* creature) {
	if (!ended) return;
	ended = false;
	char buff[128];
	sprintf_s(buff, "%s#%d ENRAGES!", creature->name, creature->index);
	pushMessage(buff);
	ended = false;
	creature->DR += 2;
	creature->damageBoost += 5;

	damageMultBoost = creature->damageMult / 10;
	creature->damageMult += damageMultBoost;
	creature->fearless++;
	tickToDie = globalTick + 100.0f;
}

void Berserk::end(Creature* creature) {
	if (ended) return;
	ended = true;
	creature->DR -= 2;
	creature->damageBoost -= 5;
	creature->damageMult -= damageMultBoost;
	creature->fearless--;
	char buff[128];
	sprintf_s(buff, "%s#%d is now calm.", creature->name, creature->index);
	pushMessage(buff);
}

void Berserk::tick(Creature* creature) {}

void PoisonBladeBuff::start(Creature* creature) {
	if (!ended) return;
	ended = false;
	char buff[128];
	sprintf_s(buff, "%s#%d applies poison to its %s.", creature->name, creature->index, creature->weapon->name);
	pushMessage(buff);

	tickToDie = globalTick + 180.0f;
	creature->attackListeners.push_back(this);
}

void PoisonBladeBuff::end(Creature* creature) {
	if (ended) return;
	ended = true;

	char buff[128];
	sprintf_s(buff, "%s#%d's %s is no longer poisoned.", creature->name, creature->index, creature->weapon->name);
	pushMessage(buff);

	for (unsigned i = 0; i < creature->attackListeners.size(); i++) {
		if (creature->attackListeners[i] == this) {
			creature->attackListeners[i] = creature->attackListeners[creature->attackListeners.size() - 1];
			creature->attackListeners.resize(creature->attackListeners.size()-1);
			break;
		}
	}
}

void PoisonBladeBuff::attacked(Creature* attacker, Creature* defender, int damage) {
	if (ended) return;
	for (unsigned i = 0; i < defender->buffs.size(); i++) {
		if (defender->buffs[i]->buffIndex == buffIndex) {
			defender->buffs[i]->tickToDie = globalTick + 70.0f;
			return;
		}
	}
	DamageOverTime *dot = new DamageOverTime(defender);

	defender->buffs.push_back(dot);

	dot->dotTick = 20.0f;
	dot->dotDamage = 5;
	dot->tickToDie = globalTick + 70.0f;
	dot->buffIndex = buffIndex;
	strcpy_s(dot->name, "Poisoned");
	dot->start(defender);
}

void DamageOverTime::start(Creature* creature) {
	if (!ended) return;
	ended = false;
	char buff[128];
	sprintf_s(buff, "%s#%d is now %s!", creature->name, creature->index, name);
	timeToNextDamage = globalTick + dotTick;
	pushMessage(buff);
}

void DamageOverTime::end(Creature* creature) {
	if (ended) return;
	ended = true;
	char buff[128];
	sprintf_s(buff, "%s#%d is no longer %s.", creature->name, creature->index, name);
	pushMessage(buff);
}

void DamageOverTime::tick(Creature* creature) {
	if (timeToNextDamage <= globalTick) {
		timeToNextDamage = globalTick + dotTick;

		char buff[128];
		sprintf_s(buff, "%s is damaged for %d (%s)!", creature->name, dotDamage, name);
		pushMessage(buff);
		creature->doDamage(dotDamage);
	}
}

void PoisonBladeBuff::tick(Creature* creature) {}

int BerserkSkill::shouldCast(Creature* creature, vector<Creature*>& allies, vector<Creature*>& enemies, vector<Creature*>& enemiesToAttack, vector<Creature*>& enemiesToMove) {
	if (enemiesToAttack.size() >= 3 && enemies.size() >= 5) {
		return 3;
	} else if (creature->hp < creature->hpMax / 2 && enemiesToAttack.size() >= 1 && enemies.size() >= 3) {
		return 2;
	} else if (creature->hp < 20 && enemiesToAttack.size() >= 1) {
		return 4;
	}

	return 0;
}

void BerserkSkill::doCast(Creature* creature, vector<Creature*>& allies, vector<Creature*>& enemies, vector<Creature*>& enemiesToAttack, vector<Creature*>& enemiesToMove) {
	Buff* b = new Berserk(creature);
	creature->buffs.push_back(b);

	char buff[128];
	sprintf_s(buff, "%s#%d goes berserk!", creature->name, creature->index, name);
	pushMessage(buff);

	creature->buffs[creature->buffs.size() - 1]->start(creature);
	timeToNextCast = globalTick + delayToNextCast;
}

int HealSkill::shouldCast(Creature* creature, vector<Creature*>& allies, vector<Creature*>& enemies, vector<Creature*>& enemiesToAttack, vector<Creature*>& enemiesToMove) {

	if (creature->hp < creature->hpMax / 10) {
		creatureToHeal = creature;
		return 20;
	}
	if (enemies.size() > 0) {

		if (creature->hp < creature->hpMax / 3) {
			creatureToHeal = creature;
			return 10;
		}

		for (unsigned i = 0; i < allies.size(); i++) {
			if (ran(20) == 0) {
				if (allies[i]->hp < allies[i]->hpMax / 3 || allies[i]->hp < healMin) {
					creatureToHeal = allies[i];
					return 2;
				}
			}
		}
	}
	return 0;
}

int SingleTargetSkill::shouldCast(Creature* creature, vector<Creature*>& allies, vector<Creature*>& enemies, vector<Creature*>& enemiesToAttack, vector<Creature*>& enemiesToMove) {
	if (enemies.size() > 0) {
		if (ran(40) == 0) {
			creatureToCast = enemies[ran(enemies.size())];
			return 3;
		}
	}
	return 0;
}

void HealSkill::doCast(Creature* creature, vector<Creature*>& allies, vector<Creature*>& enemies, vector<Creature*>& enemiesToAttack, vector<Creature*>& enemiesToMove) {
	int healAmount = rani(healMin, healMax);

	char buff[128];
	if (creatureToHeal == creature) {
		sprintf_s(buff, "%s#%d heals itself for %d hp!", creature->name, creature->index, healAmount);
	} else {
		sprintf_s(buff, "%s#%d heals %s#%d for %d hp!", creature->name, creature->index, creatureToHeal->name, creatureToHeal->index, healAmount);
	}
	pushMessage(buff);
	creatureToHeal->heal(healAmount);
	timeToNextCast = globalTick + delayToNextCast;
}

void LightingSkill::doCast(Creature* creature, vector<Creature*>& allies, vector<Creature*>& enemies, vector<Creature*>& enemiesToAttack, vector<Creature*>& enemiesToMove) {

	int damage = rani(minDamage, maxDamage);

	char buff[128];

	sprintf_s(buff, "%s#%d casts lighting!", creature->name);
	pushMessage(buff);

	sprintf_s(buff, "%s#%d damages %s#%d for %d hp!", creature->name, creature->index, creatureToCast->name, creatureToCast->index, damage);
	pushMessage(buff);
	creatureToCast->doDamage(damage);

	timeToNextCast = globalTick + delayToNextCast;
}

void FireBallSkill::doCast(Creature* creature, vector<Creature*>& allies, vector<Creature*>& enemies, vector<Creature*>& enemiesToAttack, vector<Creature*>& enemiesToMove) {

	int damage = rani(minDamage, maxDamage);

	char buff[128];

	sprintf_s(buff, "%s#%d casts Fire Ball!", creature->name);
	pushMessage(buff);

	sprintf_s(buff, "%s#%d damages %s#%d for %d hp!", creature->name, creature->index, creatureToCast->name, creatureToCast->index, damage);
	pushMessage(buff);
	creatureToCast->doDamage(damage);

	DamageOverTime *dot = new DamageOverTime(creatureToCast);

	creatureToCast->buffs.push_back(dot);

	dot->dotTick = 20.0f;
	dot->dotDamage = 3;
	dot->tickToDie = globalTick + 70.0f;
	strcpy_s(dot->name, "Burning");
	dot->start(creatureToCast);


	timeToNextCast = globalTick + delayToNextCast;
}

void IceBoltSkill::doCast(Creature* creature, vector<Creature*>& allies, vector<Creature*>& enemies, vector<Creature*>& enemiesToAttack, vector<Creature*>& enemiesToMove) {

	int damage = rani(minDamage, maxDamage);

	char buff[128];

	sprintf_s(buff, "%s#%d casts Ice Bolt!", creature->name);
	pushMessage(buff);

	sprintf_s(buff, "%s#%d damages %s#%d for %d hp!", creature->name, creature->index, creatureToCast->name, creatureToCast->index, damage);
	pushMessage(buff);
	creatureToCast->doDamage(damage);

	SlowBuff *slow = new SlowBuff(creatureToCast);

	creatureToCast->buffs.push_back(slow);
	slow->tickToDie = globalTick + 100.0f;
	slow->start(creatureToCast);

	timeToNextCast = globalTick + delayToNextCast;
}

int PoisonBladeSkill::shouldCast(Creature* creature, vector<Creature*>& allies, vector<Creature*>& enemies, vector<Creature*>& enemiesToAttack, vector<Creature*>& enemiesToMove) {
	if (enemies.size() > 0 && creature->weapon->type != 0) {
		return 5;
	}
	return 0;
}


void PoisonBladeSkill::doCast(Creature* creature, vector<Creature*>& allies, vector<Creature*>& enemies, vector<Creature*>& enemiesToAttack, vector<Creature*>& enemiesToMove) {
	PoisonBladeBuff* buff = new PoisonBladeBuff(creature);
	creature->buffs.push_back(buff);
	buff->start(creature);
	timeToNextCast = globalTick + delayToNextCast;
}

