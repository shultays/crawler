#include "Skill.h"

#include "Creature.h"
#include "CreatureGen.h"

int nextBuffIndex = 0;

Skill::Skill() {
	silent = false;
	timeToNextCast = globalTick;
}

void SlowBuff::start(Creature* creature) {

	sprintf_s(buff, "%s is slowed!", creature->name);
	if (!silent) pushMessage(buff);
	BuffGroup::start(creature);
}

void SlowBuff::end(Creature* creature) {

	sprintf_s(buff, "%s is no longer slowed", creature->name);
	if (!silent) pushMessage(buff);
	BuffGroup::end(creature);
}


void Berserk::start(Creature* creature) {
	if (!ended) return;
	ended = false;
	creature->fearless++;

	sprintf_s(buff, "%s ENRAGES!", creature->name);
	if (!silent) pushMessage(buff);
	tickToDie = globalTick + 100.0f;
}

void Berserk::end(Creature* creature) {
	if (ended) return;
	ended = true;
	creature->fearless--;

	sprintf_s(buff, "%s is now calm.", creature->name);
	if (!silent) pushMessage(buff);
}

void PoisonBladeBuff::start(Creature* creature) {
	if (!ended) return;
	ended = false;
	if (showText) {

		sprintf_s(buff, "%s applies poison to its %s.", creature->name, creature->weapon->name);
		if (!silent) pushMessage(buff);
	}

	tickToDie = globalTick + 180.0f;
	creature->attackListeners.push_back(this);
}

void PoisonBladeBuff::end(Creature* creature) {
	if (ended) return;
	ended = true;

	if (showText) {

		sprintf_s(buff, "%s's %s is no longer poisoned.", creature->name, creature->weapon->name);
		if (!silent) pushMessage(buff);
	}

	for (unsigned i = 0; i < creature->attackListeners.size(); i++) {
		if (creature->attackListeners[i] == this) {
			creature->attackListeners[i] = creature->attackListeners[creature->attackListeners.size() - 1];
			creature->attackListeners.resize(creature->attackListeners.size() - 1);
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
	DamageOverTime *dot = new DamageOverTime(attacker->index);

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

	sprintf_s(buff, "%s is now %s!", creature->name, name);
	timeToNextDamage = globalTick + dotTick;
	if (!silent) pushMessage(buff);
}

void DamageOverTime::end(Creature* creature) {
	if (ended) return;
	ended = true;

	sprintf_s(buff, "%s is no longer %s.", creature->name, name);
	if (!silent) pushMessage(buff);
}

void DamageOverTime::tick(Creature* creature) {
	if (timeToNextDamage <= globalTick) {
		timeToNextDamage = globalTick + dotTick;

		if (dotDamage > 0) {

			sprintf_s(buff, "%s is damaged for %d (%s)!", creature->name, dotDamage, name);
			if (!silent) pushMessage(buff);
			creature->doDamage(damager, dotDamage);
		} else {

			sprintf_s(buff, "%s is healed for %d (%s)!", creature->name, -dotDamage, name);
			if (!silent) pushMessage(buff);
			creature->heal(-dotDamage);
		}
	}
}


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

void BerserkSkill::doCast(Creature* creature, vector<Creature*>& allies, vector<Creature*>& enemies, vector<Creature*>& enemiesToAttack, vector<Creature*>& enemiesToMove, Pos p) {
	Buff* b = new Berserk();
	creature->buffs.push_back(b);


	sprintf_s(buff, "%s goes berserk!", creature->name, name);
	if (!silent) pushMessage(buff);

	creature->buffs[creature->buffs.size() - 1]->start(creature);
	timeToNextCast = globalTick + delayToNextCast;
}


int SingleTargetSkill::shouldCast(Creature* creature, vector<Creature*>& allies, vector<Creature*>& enemies, vector<Creature*>& enemiesToAttack, vector<Creature*>& enemiesToMove) {
	if (enemies.size() > 0) {
		if (ran(250) == 0) {
			creatureToCast = enemies[ran(enemies.size())];
			return 3;
		}
	}
	return 0;
}

int HealSkill::shouldCast(Creature* creature, vector<Creature*>& allies, vector<Creature*>& enemies, vector<Creature*>& enemiesToAttack, vector<Creature*>& enemiesToMove) {
	if (creature->hp < creature->hpMax / 6 || (creature->hp < 10 && creature->mpMax > 10)) {
		creatureToHeal = creature;
		return 20;
	}
	if (enemies.size() > 0) {
		if (creature->hp < creature->hpMax / 3) {
			creatureToHeal = creature;
			return 10;
		}

		for (unsigned i = 0; canHealOther && i < allies.size(); i++) {
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

void HealSkill::doCast(Creature* creature, vector<Creature*>& allies, vector<Creature*>& enemies, vector<Creature*>& enemiesToAttack, vector<Creature*>& enemiesToMove, Pos p) {
	if (p.x != -1) {
		creatureToHeal = NULL;
		for (unsigned i = 0; i < creatures.size(); i++) {
			if (creatures[i]->pos == p) {
				creatureToHeal = creatures[i];
				break;
			}
		}
	}
	if (!creatureToHeal) return;
	int healAmount = rani(healMin, healMax) + creatureToHeal->hpMax * percentHeal / 100;

	if (creatureToHeal == creature) {
		sprintf_s(buff, "%s heals itself for %d hp!", creature->name, healAmount);
	} else {
		sprintf_s(buff, "%s heals %s for %d hp!", creature->name, creatureToHeal->name, healAmount);
	}
	if (!silent) pushMessage(buff);
	creatureToHeal->heal(healAmount);
	timeToNextCast = globalTick + delayToNextCast;
}


void AmplifyDamage::doCast(Creature* creature, vector<Creature*>& allies, vector<Creature*>& enemies, vector<Creature*>& enemiesToAttack, vector<Creature*>& enemiesToMove, Pos p) {
	if (p.x != -1) {
		creatureToCast = NULL;
		for (unsigned i = 0; i < creatures.size(); i++) {
			if (creatures[i]->pos == p) {
				creatureToCast = creatures[i];
				break;
			}
		}
	}
	if (!creatureToCast) return;



	sprintf_s(buff, "%s armor is reduced.", creatureToCast);
	if (!silent) pushMessage(buff);
	Buff *b = new DRBuff(-5);

	creature->buffs.push_back(b);
	b->tickToDie = globalTick + 100.0f;
	strcpy_s(b->name, "Damage Amplify");
	b->start(creature);

	timeToNextCast = globalTick + delayToNextCast;
}


int HasteSkill::shouldCast(Creature* creature, vector<Creature*>& allies, vector<Creature*>& enemies, vector<Creature*>& enemiesToAttack, vector<Creature*>& enemiesToMove) {
	if (enemies.size() >= 1 && ran(100) == 0) {
		return 1;
	}
	return 0;
}

void HasteSkill::doCast(Creature* creature, vector<Creature*>& allies, vector<Creature*>& enemies, vector<Creature*>& enemiesToAttack, vector<Creature*>& enemiesToMove, Pos p) {
	sprintf_s(buff, "%s is hastened.", creature, creature);
	if (!silent) pushMessage(buff);
	Buff *b = new SlowBuff(140, 120);
	creature->buffs.push_back(b);
	b->tickToDie = globalTick + 200.0f;
	strcpy_s(b->name, "Haste");
	b->start(creature);

	timeToNextCast = globalTick + delayToNextCast;
}


int ManaSkill::shouldCast(Creature* creature, vector<Creature*>& allies, vector<Creature*>& enemies, vector<Creature*>& enemiesToAttack, vector<Creature*>& enemiesToMove) {
	if (creature->mp < creature->mpMax / 6 && enemies.size() >= 2) {
		return 1;
	}
	return 0;
}

void ManaSkill::doCast(Creature* creature, vector<Creature*>& allies, vector<Creature*>& enemies, vector<Creature*>& enemiesToAttack, vector<Creature*>& enemiesToMove, Pos p) {

	sprintf_s(buff, "%s's mana is fully restored.!", creature->name);
	if (!silent) pushMessage(buff);
	creature->mp = creature->mpMax;
}

int RegenSkill::shouldCast(Creature* creature, vector<Creature*>& allies, vector<Creature*>& enemies, vector<Creature*>& enemiesToAttack, vector<Creature*>& enemiesToMove) {
	if (creature->hp < creature->hpMax / 7 || (enemiesToAttack.size() >= 3 && creature->hp < creature->hpMax / 2)) {
		return 5;
	}

	return 0;
}

void RegenSkill::doCast(Creature* creature, vector<Creature*>& allies, vector<Creature*>& enemies, vector<Creature*>& enemiesToAttack, vector<Creature*>& enemiesToMove, Pos p) {
	DamageOverTime *dot = new DamageOverTime(-1);

	creature->buffs.push_back(dot);

	dot->dotTick = 20.0f;
	dot->dotDamage = -10;
	dot->tickToDie = globalTick + 70.0f;
	strcpy_s(dot->name, "Regenerating");
	dot->start(creature);

}
void LightingSkill::doCast(Creature* creature, vector<Creature*>& allies, vector<Creature*>& enemies, vector<Creature*>& enemiesToAttack, vector<Creature*>& enemiesToMove, Pos p) {

	int damage = rani(minDamage, maxDamage);

	if (p.x != -1) {
		creatureToCast = NULL;
		for (unsigned i = 0; i < creatures.size(); i++) {
			if (creatures[i]->pos == p) {
				creatureToCast = creatures[i];
				break;
			}
		}
	}
	if (!creatureToCast) return;

	sprintf_s(buff, "%s casts lighting!", creature->name);
	if (!silent) pushMessage(buff);

	sprintf_s(buff, "%s damages %s for %d hp!", creature->name, creatureToCast->name, damage);
	if (!silent) pushMessage(buff);
	creatureToCast->doDamage(creature->index, damage);

	timeToNextCast = globalTick + delayToNextCast;
}

void FireBallSkill::doCast(Creature* creature, vector<Creature*>& allies, vector<Creature*>& enemies, vector<Creature*>& enemiesToAttack, vector<Creature*>& enemiesToMove, Pos p) {

	int damage = rani(minDamage, maxDamage);

	if (p.x != -1) {
		creatureToCast = NULL;
		for (unsigned i = 0; i < creatures.size(); i++) {
			if (creatures[i]->pos == p) {
				creatureToCast = creatures[i];
				break;
			}
		}
	}
	if (!creatureToCast) return;

	sprintf_s(buff, "%s casts Fire Ball!", creature->name);
	if (!silent) pushMessage(buff);

	sprintf_s(buff, "%s damages %s for %d hp!", creature->name, creatureToCast->name, damage);
	if (!silent) pushMessage(buff);
	creatureToCast->doDamage(creature->index, damage);

	DamageOverTime *dot = new DamageOverTime(creature->index);

	creatureToCast->buffs.push_back(dot);

	dot->dotTick = 20.0f;
	dot->dotDamage = 3;
	dot->tickToDie = globalTick + 70.0f;
	strcpy_s(dot->name, "Burning");
	dot->start(creatureToCast);


	timeToNextCast = globalTick + delayToNextCast;
}

void IceBoltSkill::doCast(Creature* creature, vector<Creature*>& allies, vector<Creature*>& enemies, vector<Creature*>& enemiesToAttack, vector<Creature*>& enemiesToMove, Pos p) {

	int damage = rani(minDamage, maxDamage);

	if (p.x != -1) {
		creatureToCast = NULL;
		for (unsigned i = 0; i < creatures.size(); i++) {
			if (creatures[i]->pos == p) {
				creatureToCast = creatures[i];
				break;
			}
		}
	}
	if (!creatureToCast) return;

	sprintf_s(buff, "%s casts Ice Bolt!", creature->name);
	if (!silent) pushMessage(buff);

	sprintf_s(buff, "%s damages %s for %d hp!", creature->name, creatureToCast->name, damage);
	if (!silent) pushMessage(buff);
	creatureToCast->doDamage(creature->index, damage);

	SlowBuff *slow = new SlowBuff();

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

void PoisonBladeSkill::doCast(Creature* creature, vector<Creature*>& allies, vector<Creature*>& enemies, vector<Creature*>& enemiesToAttack, vector<Creature*>& enemiesToMove, Pos p) {
	PoisonBladeBuff* buff = new PoisonBladeBuff(poisonDamage);
	buff->showText = true;
	creature->buffs.push_back(buff);
	buff->start(creature);
	timeToNextCast = globalTick + delayToNextCast;
}

int EnchantSkill::shouldCast(Creature* creature, vector<Creature*>& allies, vector<Creature*>& enemies, vector<Creature*>& enemiesToAttack, vector<Creature*>& enemiesToMove) {
	if (enemies.size() != 0) {
		return 0;
	}
	if (ran(150)) return 0;

	if (isWeapon) {
		if (creature->weapon->type == 0) return 0;
		return 1;
	} else {
		int toCheck[3] = { ARMOR, HELM, SHIELD };
		total = 0;
		for (int k = 0; k < 3; k++) {
			for (unsigned i = 0; i < creature->equipmentSlots[toCheck[k]].size(); i++) {
				if (creature->equipmentSlots[toCheck[k]][i])
					total++;
			}
		}
		if (total>0) {
			return 1;
		}
	}

	return 0;
}

void EnchantSkill::doCast(Creature* creature, vector<Creature*>& allies, vector<Creature*>& enemies, vector<Creature*>& enemiesToAttack, vector<Creature*>& enemiesToMove, Pos p) {

	bool enchant = false;
	Equipment *toEnchant = NULL;

	if (isWeapon) {
		if (creature->weapon->enchantCount < 2) {
			enchant = true;
		} else {
			enchant = ran(creature->weapon->enchantCount - 1) == 0;
		}

		if (enchant) {
			creature->weapon->minDamage = ((int)(creature->weapon->minDamage*1.08f));
			creature->weapon->minDamage += 3;
			creature->weapon->maxDamage = ((int)(creature->weapon->maxDamage*1.08f));
			creature->weapon->maxDamage += 3;

			toEnchant = creature->weapon;
		}
	} else {
		int j = ran(total);

		int toCheck[3] = { ARMOR, HELM, SHIELD };
		for (int k = 0; k < 3 && !toEnchant; k++) {
			for (unsigned i = 0; i < creature->equipmentSlots[toCheck[k]].size(); i++) {
				if (creature->equipmentSlots[toCheck[k]][i]) {
					if (j == 0) {
						toEnchant = creature->equipmentSlots[toCheck[k]][i];
						break;
					} else {
						j--;
					}
				}
			}
		}

		if (toEnchant->enchantCount < 2) {
			enchant = true;
		} else {
			enchant = ran(toEnchant->enchantCount - 1) == 0;
		}
	}

	if (enchant) {
		if (toEnchant->buffGroup == NULL) {
			vector<int> weights;
			Buff* buff = getBuff(weights, creature->level);
			toEnchant->addBuff(buff);
		} else {
			bool addNew = true;
			if (toEnchant->buffGroup->buffs.size() > 5) {
				addNew = false;
			} else {
				addNew = ran(toEnchant->buffGroup->buffs.size() + 1) == 0;
			}
			if (addNew) {
				vector<int> weights(BUFF_CNT);
				for (unsigned i = 1; i < weights.size(); i++) weights[i] = 1;
				for (unsigned i = 0; i < toEnchant->buffGroup->buffs.size(); i++) {
					weights[toEnchant->buffGroup->buffs[i]->buffIndex] = 0;
				}

				Buff* buff = getBuff(weights, creature->level);
				toEnchant->addBuff(buff);
			} else {
				toEnchant->buffGroup->buffs[ran(toEnchant->buffGroup->buffs.size())]->improve();
			}
			toEnchant->enchantCount++;

		}

		sprintf_s(buff, "%s enchants %s to +%d!", creature->name, toEnchant->name, toEnchant->enchantCount);
		if (!silent) pushMessage(buff);
		toEnchant->buffGroup->end(creature);
		toEnchant->buffGroup->start(creature);
	} else {

		sprintf_s(buff, "%s tries to enchant %s but fails!", creature->name, toEnchant->name);
		if (!silent) pushMessage(buff);
	}
}

int SummonSkill::shouldCast(Creature* creature, vector<Creature*>& allies, vector<Creature*>& enemies, vector<Creature*>& enemiesToAttack, vector<Creature*>& enemiesToMove) {
	if ((enemies.size() >= 3 && creature->hp < creature->hpMax / 3 && allies.size() < 2) || enemies.size() >= 5) {
		return 1;
	}
	return 0;
}

void SummonSkill::doCast(Creature* creature, vector<Creature*>& allies, vector<Creature*>& enemies, vector<Creature*>& enemiesToAttack, vector<Creature*>& enemiesToMove, Pos p) {
	vector<Pos> npos;
	Pos pos = creature->pos;
	if (enemies.size() > 0)
		pos = creature->getAveragePos(enemies);

	doExplore<MAZE_W, MAZE_H>(pos, maze.walls, seeWeights, 5.0f, npos);
	for (unsigned i = 0; i < enemies.size(); i++) {
		enemies[i]->lastAttackCreatureIndex = -1;
	}
	int r = 1;
	if (type == 1) {
		r = rani(4, 7);
	}
	for (int i = 0; i < r && npos.size() > 0; i++) {
		int j = ran(npos.size());
		Pos p = npos[j];
		npos[j] = npos[npos.size() - 1];
		npos.resize(npos.size() - 1);
		if (maze.walls[p.x][p.y] == 0) {
			Creature* c = generateButterfly();
			c->reset(p);
			creatures.push_back(c);
		}
	}
}

int CharmSkill::shouldCast(Creature* creature, vector<Creature*>& allies, vector<Creature*>& enemies, vector<Creature*>& enemiesToAttack, vector<Creature*>& enemiesToMove) {
	if (enemies.size() >= 4 && ran(5) == 0 && allies.size() == 0) {
		return 5;
	}
	return 0;
}

void CharmSkill::doCast(Creature* creature, vector<Creature*>& allies, vector<Creature*>& enemies, vector<Creature*>& enemiesToAttack, vector<Creature*>& enemiesToMove, Pos p) {
	CharmedBuff *b = new CharmedBuff();
	b->tickToDie = globalTick + 1000.0f;
	Creature* enemy = enemies[ran(enemies.size())];


	sprintf_s(buff, "%s charms %s!", creature->name, enemy->name);
	if (!silent) pushMessage(buff);
	enemy->master = creature;
	enemy->masterIndex = creature->index;
	enemy->wanderMult = 0.05f;
	b->start(enemy);
	enemy->buffs.push_back(b);
}


int BloodlustSkill::shouldCast(Creature* creature, vector<Creature*>& allies, vector<Creature*>& enemies, vector<Creature*>& enemiesToAttack, vector<Creature*>& enemiesToMove) {
	if (enemies.size() > 0 && allies.size() >= 2 && ran(50) == 0) {
		return 10;
	}
	return 0;
}

void BloodlustSkill::doCast(Creature* creature, vector<Creature*>& allies, vector<Creature*>& enemies, vector<Creature*>& enemiesToAttack, vector<Creature*>& enemiesToMove, Pos p) {
	sprintf_s(buff, "%s buffs its alies into blood frenzy!", creature->name);
	if (!silent) pushMessage(buff);

	for (unsigned i = 0; i < allies.size(); i++) {
		Buff *b = new HpBuff(20);
		b->tickToDie = globalTick + 150.0f;
		allies[i]->buffs.push_back(b);
		b->start(allies[i]);
		b = new ConstantDMGBoost(5);
		b->tickToDie = globalTick + 150.0f;
		allies[i]->buffs.push_back(b);
		b->start(allies[i]);
	}
	timeToNextCast = globalTick + delayToNextCast;
}

int ElementalWeaponSkill::shouldCast(Creature* creature, vector<Creature*>& allies, vector<Creature*>& enemies, vector<Creature*>& enemiesToAttack, vector<Creature*>& enemiesToMove) {
	if (enemies.size() >= 1 && ran(50) == 0) {
		return 2;
	}
	return 0;
}

void ElementalWeaponSkill::doCast(Creature* creature, vector<Creature*>& allies, vector<Creature*>& enemies, vector<Creature*>& enemiesToAttack, vector<Creature*>& enemiesToMove, Pos p) {
	sprintf_s(buff, "%s enchants its weapon with %s.", creature->name, damageType);
	if (!silent) pushMessage(buff);

	PlusDamageBuff *b = new PlusDamageBuff(5, 10, damageType, color);

	creature->buffs.push_back(b);
	b->tickToDie = globalTick + 100.0f;
	b->start(creature);

	timeToNextCast = globalTick + delayToNextCast;
}

int StoneSkinSkill::shouldCast(Creature* creature, vector<Creature*>& allies, vector<Creature*>& enemies, vector<Creature*>& enemiesToAttack, vector<Creature*>& enemiesToMove) {
	if (enemies.size() > 0 && creature->hp < creature->hpMax / 2 && ran(20) == 0) {
		return 10;
	}
	return 0;
}

void StoneSkinSkill::doCast(Creature* creature, vector<Creature*>& allies, vector<Creature*>& enemies, vector<Creature*>& enemiesToAttack, vector<Creature*>& enemiesToMove, Pos p) {
	sprintf_s(buff, "%s skins morph to stone.", creature->name);
	if (!silent) pushMessage(buff);

	Buff *b = new DRBuff(6);
	b->tickToDie = globalTick + 100.0f;
	creature->buffs.push_back(b);
	b->start(creature);
	timeToNextCast = globalTick + delayToNextCast;
}

int DispelSkill::shouldCast(Creature* creature, vector<Creature*>& allies, vector<Creature*>& enemies, vector<Creature*>& enemiesToAttack, vector<Creature*>& enemiesToMove) {

	return 0;
}

void DispelSkill::doCast(Creature* creature, vector<Creature*>& allies, vector<Creature*>& enemies, vector<Creature*>& enemiesToAttack, vector<Creature*>& enemiesToMove, Pos p) {
	Creature* creatureToCast = creature;

	if (p.x != -1) {
		creature = NULL;
		for (unsigned i = 0; i < creatures.size(); i++) {
			if (creatures[i]->pos == p) {
				creatureToCast = creatures[i];
				break;
			}
		}
	}

	if (creatureToCast == creature)
		sprintf_s(buff, "%s dispels all all buffs from itself.", creature->name);
	else
		sprintf_s(buff, "%s dispels all all buffs from %s.", creature->name, creatureToCast->name);

	if (!silent) pushMessage(buff);

	for (unsigned i = 0; i < creature->buffs.size(); i++) {
		creature->buffs[i]->end(creature);
		delete creature->buffs[i];
	}
	creature->buffs.clear();
	timeToNextCast = globalTick + delayToNextCast;
}


int BlurSkill::shouldCast(Creature* creature, vector<Creature*>& allies, vector<Creature*>& enemies, vector<Creature*>& enemiesToAttack, vector<Creature*>& enemiesToMove) {
	if (enemies.size() > 0) {
		return 1;
	}
	return 0;
}

void BlurSkill::doCast(Creature* creature, vector<Creature*>& allies, vector<Creature*>& enemies, vector<Creature*>& enemiesToAttack, vector<Creature*>& enemiesToMove, Pos p) {
	sprintf_s(buff, "%s's image gets blurry.", creature->name);
	if (!silent) pushMessage(buff);

	Buff *b = new EvasionBuff(35, false);
	b->tickToDie = globalTick + 250.0f;
	creature->buffs.push_back(b);
	b->start(creature);
	timeToNextCast = globalTick + delayToNextCast;
}

///

void DRBuff::start(Creature* creature) {
	if (!ended) return;
	ended = false;
	creature->DR += buff;
}

void DRBuff::end(Creature* creature) {
	if (ended) return;
	ended = true;
	creature->DR -= buff;
}

void ConstantDMGBoost::start(Creature* creature) {
	if (!ended) return;
	ended = false;
	creature->damageBoost += buff;
}

void ConstantDMGBoost::end(Creature* creature) {
	if (ended) return;
	ended = true;
	creature->damageBoost -= buff;
}

void DMGBultBuff::start(Creature* creature) {
	if (!ended) return;
	ended = false;
	addedBuff = (int)((creature->damageMult / 100.0f)* (buff - 100));
	creature->damageMult += addedBuff;
}

void DMGBultBuff::end(Creature* creature) {
	if (ended) return;
	ended = true;
	creature->damageMult -= addedBuff;
}

void SightBuff::start(Creature* creature) {
	if (!ended) return;
	ended = false;
	addedBuff = (int)((creature->sightMult / 100.0f)* (buff - 100));
	creature->sightMult += addedBuff;
}

void SightBuff::end(Creature* creature) {
	if (ended) return;
	ended = true;
	creature->sightMult -= addedBuff;
}

void AtkSpeedBuff::start(Creature* creature) {
	if (!ended) return;
	ended = false;
	addedBuff = (int)((creature->attackSpeedMult / 100.0f)* (buff - 100));
	creature->attackSpeedMult += addedBuff;
}

void AtkSpeedBuff::end(Creature* creature) {
	if (ended) return;
	ended = true;
	creature->attackSpeedMult -= addedBuff;
}

void MoveSpeedBuff::start(Creature* creature) {
	if (!ended) return;
	ended = false;
	addedBuff = (int)((creature->moveSpeedMult / 100.0f)* (buff - 100));
	creature->moveSpeedMult += addedBuff;
}

void MoveSpeedBuff::end(Creature* creature) {
	if (ended) return;
	ended = true;
	creature->moveSpeedMult -= addedBuff;
}

void PlusDamageBuff::start(Creature* creature) {
	if (!ended) return;
	ended = false;
	creature->attackListeners.push_back(this);
}

void PlusDamageBuff::end(Creature* creature) {
	if (ended) return;
	ended = true;
	for (unsigned i = 0; i < creature->attackListeners.size(); i++) {
		if (creature->attackListeners[i] == this) {
			creature->attackListeners[i] = creature->attackListeners[creature->attackListeners.size() - 1];
			creature->attackListeners.resize(creature->attackListeners.size() - 1);
			break;
		}
	}
}


void VampiricWeaponBuff::start(Creature* creature) {
	if (!ended) return;
	ended = false;
	creature->attackListeners.push_back(this);
}

void VampiricWeaponBuff::end(Creature* creature) {
	if (ended) return;
	ended = true;


	for (unsigned i = 0; i < creature->attackListeners.size(); i++) {
		if (creature->attackListeners[i] == this) {
			creature->attackListeners[i] = creature->attackListeners[creature->attackListeners.size() - 1];
			creature->attackListeners.resize(creature->attackListeners.size() - 1);
			break;
		}
	}
}

void VampiricWeaponBuff::attacked(Creature* attacker, Creature* defender, int damage) {
	if (ended) return;
	attacker->heal((damage * stealPercent) / 100 + 1);
}



void PlusDamageBuff::attacked(Creature* attacker, Creature* defender, int damage) {
	if (ended) return;
	int extra = rani(minDamage, maxDamage);


	sprintf_s(buff, "%s does extra %d %s damage!", attacker->name, extra, damageType);
	if (!silent) pushMessage(buff);
	defender->doDamage(attacker->index, extra);
}

void EvasionBuff::start(Creature* creature) {
	if (!ended) return;
	ended = false;
	addedBuff = (int)((creature->chanceToHit / 100.0f)* (buff - 100));
	creature->chanceToHit += addedBuff;
}

void EvasionBuff::end(Creature* creature) {
	if (ended) return;
	ended = true;
	creature->chanceToHit -= addedBuff;
}

void HpRegenBuff::start(Creature* creature) {
	if (!ended) return;
	ended = false;
	addedBuff = (int)((creature->hpRegenMult / 100.0f)* (buff - 100));
	creature->hpRegenMult += addedBuff;
}

void HpRegenBuff::end(Creature* creature) {
	if (ended) return;
	ended = true;
	creature->hpRegenMult -= addedBuff;
}

void MpRegenBuff::start(Creature* creature) {
	if (!ended) return;
	ended = false;
	addedBuff = (int)((creature->mpRegenMult / 100.0f)* (buff - 100));
	creature->mpRegenMult += addedBuff;
}

void MpRegenBuff::end(Creature* creature) {
	if (ended) return;
	ended = true;
	creature->mpRegenMult -= addedBuff;
}

void HpBuff::start(Creature* creature) {
	if (!ended) return;
	ended = false;
	creature->hp = (int)(creature->hp*(float)((creature->hpMax + buff) / creature->hpMax)) + 1;
	creature->hpMax += buff;
}

void HpBuff::end(Creature* creature) {
	if (ended) return;
	ended = true;
	creature->hp = (int)(creature->hp*(float)((creature->hpMax - buff) / creature->hpMax)) + 1;
	creature->hpMax -= buff;
	if (creature->hp > creature->hpMax) creature->hp = creature->hpMax;
}

void MpBuff::start(Creature* creature) {
	if (!ended) return;
	ended = false;
	if (creature->mpMax) {
		creature->mp = (int)(creature->mp*(float)((creature->mpMax + buff) / creature->mpMax));
	}
	creature->mpMax += buff;
	if (creature->mp > creature->mpMax) creature->mp = creature->mpMax;
}

void MpBuff::end(Creature* creature) {
	if (ended) return;
	ended = true;
	creature->mp = (int)(creature->mp*(float)((creature->mpMax - buff) / creature->mpMax));
	creature->mpMax -= buff;
	if (creature->mp > creature->mpMax) creature->mp = creature->mpMax;
}

void CharmedBuff::start(Creature* creature) {
	if (!ended) return;
	ended = false;
	creature->oldType = creature->type;
	creature->type = ADVENTURER_ALLY;
	creature->sight += 10;
	creature->moveSpeedMult += 500;
}

void CharmedBuff::end(Creature* creature) {
	if (ended) return;
	ended = true;
	creature->type = creature->oldType;
	creature->sight -= 10;
	creature->moveSpeedMult -= 500;
	creature->wanderMult = 1.0f;


	sprintf_s(buff, "%s is no longer charmed!", creature->name);
	if (!silent) pushMessage(buff);
}