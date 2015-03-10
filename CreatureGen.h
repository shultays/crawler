#pragma once
#include "Creature.h"

void addBuff(Equipment* e, vector<int>& weights, int level, int maxBuffCount, int noBuffChance) {

	int firstAddedI = -1;
	int secondAddedI = -1;
	for (int i = 0; i < maxBuffCount; i++) {
		if (ran(100) < noBuffChance) continue;
		Buff* buff = getBuff(weights, level - i);
		if (buff) {
			e->addBuff(buff);
			weights[buff->buffIndex] = 0;

			if (firstAddedI == -1) {
				firstAddedI = buff->buffIndex;
			} else if (secondAddedI == -1) {
				secondAddedI = buff->buffIndex;
			}
		}
	}

	int post = firstAddedI;
	int pre = -1;
	if (secondAddedI != -1) {
		pre = post;
		post = secondAddedI;
	}
	if (pre == -1 && post != -1 && ran(10 - level) == 0) {
		pre = post;
		post = -1;
	}
	if (post != -1) {
		char buff[128];
		sprintf_s(buff, buffNamers[post], e->name);
		strcpy_s(e->name, buff);
	}
	if (pre != -1) {
		char buff[128];
		sprintf_s(buff, firstBuffNamers[pre], e->name);
		strcpy_s(e->name, buff);
	}
}

Weapon* getWeapon(int level) {
	bool no_weapon = level < 4 ? ran(4 - level) != 0 : 0;
	if (level == -1) {
		no_weapon = true;
	}

	int rangeMin = 0;
	int rangeMax = 4;

	if (no_weapon) {
		return NULL;
	} else {
		rangeMin = 1;
		rangeMax = 4;
	}
	Weapon *w = new Weapon();
	w->slot = WEAPON;
	w->type = rangeMin + ran(rangeMax - rangeMin + 1);
	switch (w->type) {
		case 0:
			break;
		case 1:
			w->minDamage = 4 + ran(4) + level;
			w->maxDamage = w->minDamage + 4 + ran(3 + level * 2);
			w->swingTime = ranf(15.0f, 20.0f);
			strcpy_s(w->name, "Sword");
			w->range = 1;
			break;
		case 2:
			w->minDamage = 2 + level;
			w->maxDamage = w->minDamage + ran(3 + level) + 2;
			w->swingTime = ranf(14.0f, 18.0f);
			strcpy_s(w->name, "Dagger");
			w->range = 1;
			break;
		case 3:
			w->minDamage = 6 + ran(6) + level;
			w->maxDamage = w->minDamage + 8 + ran(3 + level * 2);
			w->swingTime = ranf(17.0f, 22.0f);
			w->range = 1;
			strcpy_s(w->name, "Club");
			break;
		case 4:
			w->minDamage = 2 + ran(2) + level;
			w->maxDamage = w->minDamage + 12 + ran(3 + level * 2);
			w->swingTime = ranf(17.0f, 22.0f);
			w->range = 2;
			strcpy_s(w->name, "Spear");
			break;
	}

	vector<int> weights(BUFF_CNT);
	int noBuffChance = 60 - level * 10;
	if (noBuffChance < 10) noBuffChance = 10;

	weights[NO_BUFF] = 6;


	weights[POISONED] = 10;
	weights[FIRE_DAMAGE] = 10;
	weights[ICE_DAMAGE] = 10;
	weights[LIGHTING_DAMAGE] = 10;

	weights[ATK_SPEED] = 3;
	weights[MV_SPEED] = 1;
	weights[DR_ADD] = 0;
	weights[ATK_DMG] = 1;
	weights[ATK_MUL] = 1;
	weights[SIGHT] = 1;

	weights[HP] = 1;
	weights[MP] = 1;
	weights[HP_REGEN] = 0;
	weights[MP_REGEN] = 0;

	weights[EVASION] = 0;
	weights[BLOCK] = 1;


	int maxBuffCount = ran(level / 3 + 1) + 1;
	if (maxBuffCount > 3) maxBuffCount = 3;

	addBuff(w, weights, level - 1, maxBuffCount, noBuffChance);

	return w;
}

Equipment* getArmor(int level) {
	int rangeMin = 0;
	int rangeMax = 4;

	bool no_armor;
	if (level < 2) {
		no_armor = true;
	} else if (level <= 5) {
		no_armor = ran(5 - level + 1) != 0;
	} else {
		no_armor = false;
	}

	if (no_armor) {
		return NULL;
	} else {
		rangeMin = 1;
		rangeMax = (int)((level - 2)*0.5f) + 1;
		if (rangeMax<1) rangeMax = 1;
		if (rangeMax>5) rangeMax = 5;
	}
	Equipment *e = new Equipment();
	e->slot = ARMOR;
	e->type = rangeMin + ran(rangeMax - rangeMin + 1);
	int DR = 0;
	int slow = 100;
	switch (e->type) {
		case 0:
			break;
		case 1:
			DR = 0 + ran(2 + (level - 2) / 6);
			slow = 100;
			strcpy_s(e->name, "Cloth Armor");
			break;
		case 2:
			DR = 2 + ran(2 + (level - 2) / 4);
			slow = 110;
			strcpy_s(e->name, "Leather Armor");
			break;
		case 3:
			DR = 4 + ran(4 + (level - 2) / 3);
			slow = 130;
			strcpy_s(e->name, "Mail Armor");
			break;
		case 4:
			DR = 5 + ran(6 + (int)((level - 2) / 2.5f));
			slow = 130;
			strcpy_s(e->name, "Plate Armor");
			break;
		case 5:
			DR = 6 + ran(8 + (int)(level / 2.5f));
			slow = 150;
			strcpy_s(e->name, "Full Plate Armor");
			break;
	}

	if (DR != 0) {
		e->addBuff(new DRBuff(DR));
	}
	if (slow != 100) {
		e->addBuff(new MoveSpeedBuff(slow));
	}

	vector<int> weights(BUFF_CNT);
	int noBuffChance = 110 - level * 10;
	if (noBuffChance < 30) noBuffChance = 30;

	weights[NO_BUFF] = 6;


	weights[POISONED] = 0;
	weights[FIRE_DAMAGE] = 0;
	weights[ICE_DAMAGE] = 0;
	weights[LIGHTING_DAMAGE] = 0;

	weights[ATK_SPEED] = 1;
	weights[MV_SPEED] = 0;
	weights[DR_ADD] = 0;
	weights[ATK_DMG] = 1;
	weights[ATK_MUL] = 1;
	weights[SIGHT] = 1;

	weights[HP] = 10;
	weights[MP] = 6;
	weights[HP_REGEN] = 4;
	weights[MP_REGEN] = 6;

	weights[EVASION] = 2;
	weights[BLOCK] = 0;


	int maxBuffCount = ran(level / 3 + 1) + 1;
	if (maxBuffCount > 3) maxBuffCount = 3;

	addBuff(e, weights, level, maxBuffCount, noBuffChance);

	return e;
}

Equipment* getShield(int level) {
	int rangeMin = 0;
	int rangeMax = 4;

	bool no_armor;
	if (level < 2) {
		no_armor = true;
	} else if (level <= 5) {
		no_armor = ran(5 - level + 1) != 0;
	} else {
		no_armor = false;
	}

	if (no_armor) {
		return NULL;
	} else {
		rangeMin = 1;
		rangeMax = (int)((level - 2)*0.5f) + 1;
		if (rangeMax<1) rangeMax = 1;
		if (rangeMax>5) rangeMax = 5;
	}
	Equipment *e = new Equipment();
	e->slot = SHIELD;
	e->type = rangeMin + ran(rangeMax - rangeMin + 1);
	int DR = 0;
	int block = 100;
	switch (e->type) {
		case 0:
			break;
		case 1:
			DR = 0 + ran(1 + (level - 2) / 6);
			block = 100;
			strcpy_s(e->name, "Buckler");
			break;
		case 2:
			DR = 0 + ran(1 + (level - 2) / 4);
			block = 90;
			strcpy_s(e->name, "Targe Shield");
			break;
		case 3:
			DR = 1 + ran(2 + (level - 2) / 3);
			block = 90;
			strcpy_s(e->name, "Heater Shield");
			break;
		case 4:
			DR = 2 + ran(3 + (int)((level - 2) / 2.5f));
			block = 80;
			strcpy_s(e->name, "Kite Shield");
			break;
		case 5:
			DR = 3 + ran(3 + (int)(level / 2.5f));
			block = 70;
			strcpy_s(e->name, "Tower Shield");
			break;
	}

	if (DR != 0) {
		e->addBuff(new DRBuff(DR));
	}
	if (block != 100) {
		e->addBuff(new EvasionBuff(block, true));
	}

	vector<int> weights(BUFF_CNT);
	int noBuffChance = 110 - level * 10;
	if (noBuffChance < 30) noBuffChance = 30;

	weights[NO_BUFF] = 6;


	weights[POISONED] = 0;
	weights[FIRE_DAMAGE] = 0;
	weights[ICE_DAMAGE] = 0;
	weights[LIGHTING_DAMAGE] = 0;

	weights[ATK_SPEED] = 1;
	weights[MV_SPEED] = 0;
	weights[DR_ADD] = 0;
	weights[ATK_DMG] = 1;
	weights[ATK_MUL] = 1;
	weights[SIGHT] = 0;

	weights[HP] = 10;
	weights[MP] = 6;
	weights[HP_REGEN] = 4;
	weights[MP_REGEN] = 6;

	weights[EVASION] = 0;
	weights[BLOCK] = 0;


	int maxBuffCount = ran(level / 3 + 1) + 1;
	if (maxBuffCount > 3) maxBuffCount = 3;

	addBuff(e, weights, level, maxBuffCount, noBuffChance);

	return e;
}

Equipment* getHelm(int level) {
	int rangeMin = 0;
	int rangeMax = 4;

	bool no_armor;
	if (level < 3) {
		no_armor = true;
	} else if (level <= 6) {
		no_armor = ran(5 - level + 1) != 0;
	} else {
		no_armor = false;
	}

	if (no_armor) {
		return NULL;
	} else {
		rangeMin = 1;
		rangeMax = (int)((level - 4)*0.5f) + 1;
		if (rangeMax<1) rangeMax = 1;
		if (rangeMax>4) rangeMax = 4;
	}
	Equipment *e = new Equipment();
	e->slot = HELM;
	e->type = rangeMin + ran(rangeMax - rangeMin + 1);
	int DR = 0;
	switch (e->type) {
		case 0:
			break;
		case 1:
			DR = 0 + ran(1 + (level - 2) / 6);
			strcpy_s(e->name, "Hat");
			break;
		case 2:
			DR = 1 + ran(1 + (level - 2) / 5);
			strcpy_s(e->name, "Leather Hood");
			break;
		case 3:
			DR = 1 + ran(2 + (level - 2) / 4);
			strcpy_s(e->name, "Mail Helm");
			break;
		case 4:
			DR = 2 + ran(2 + (int)((level - 2) / 3));
			strcpy_s(e->name, "Plate Helm");
			break;
	}

	if (DR != 0) {
		e->addBuff(new DRBuff(DR));
	}


	vector<int> weights(BUFF_CNT);
	int noBuffChance = 110 - level * 10;
	if (noBuffChance < 30) noBuffChance = 30;

	weights[NO_BUFF] = 6;

	weights[POISONED] = 0;
	weights[FIRE_DAMAGE] = 0;
	weights[ICE_DAMAGE] = 0;
	weights[LIGHTING_DAMAGE] = 0;

	weights[ATK_SPEED] = 1;
	weights[MV_SPEED] = 0;
	weights[DR_ADD] = 0;
	weights[ATK_DMG] = 1;
	weights[ATK_MUL] = 1;
	weights[SIGHT] = 4;

	weights[HP] = 1;
	weights[MP] = 1;
	weights[HP_REGEN] = 2;
	weights[MP_REGEN] = 2;

	weights[EVASION] = 1;
	weights[BLOCK] = 0;


	int maxBuffCount = ran(level / 3 + 1) + 1;
	if (maxBuffCount > 3) maxBuffCount = 3;

	addBuff(e, weights, level, maxBuffCount, noBuffChance);

	return e;
}

Equipment* getBoots(int level) {
	int rangeMin = 0;
	int rangeMax = 4;

	bool no_armor;
	if (level < 3) {
		no_armor = true;
	} else if (level <= 6) {
		no_armor = ran(5 - level + 1) != 0;
	} else {
		no_armor = false;
	}

	if (no_armor) {
		return NULL;
	} else {
		rangeMin = 1;
		rangeMax = (int)((level - 4)*0.5f) + 1;
		if (rangeMax<1) rangeMax = 1;
		if (rangeMax>4) rangeMax = 4;
	}
	Equipment *e = new Equipment();
	e->slot = BOOTS;
	e->type = rangeMin + ran(rangeMax - rangeMin + 1);
	int DR = 0;
	switch (e->type) {
		case 0:
			break;
		case 1:
			DR = 0 + ran(1 + (level - 2) / 6);
			strcpy_s(e->name, "Shoes");
			break;
		case 2:
			DR = 1 + ran(2 + (level - 2) / 4);
			strcpy_s(e->name, "Leather Boots");
			break;
		case 3:
			DR = 1 + ran(4 + (level - 3) / 3);
			strcpy_s(e->name, "Mail Boots");
			break;
		case 4:
			DR = 2 + ran(6 + (int)((level - 2) / 3));
			strcpy_s(e->name, "Plate Boots");
			break;
	}

	if (DR != 0) {
		e->addBuff(new DRBuff(DR));
	}

	vector<int> weights(BUFF_CNT);
	int noBuffChance = 110 - level * 10;
	if (noBuffChance < 30) noBuffChance = 30;

	weights[NO_BUFF] = 6;

	weights[POISONED] = 0;
	weights[FIRE_DAMAGE] = 0;
	weights[ICE_DAMAGE] = 0;
	weights[LIGHTING_DAMAGE] = 0;

	weights[ATK_SPEED] = 0;
	weights[MV_SPEED] = 10;
	weights[DR_ADD] = 0;
	weights[ATK_DMG] = 1;
	weights[ATK_MUL] = 1;
	weights[SIGHT] = 0;

	weights[HP] = 1;
	weights[MP] = 1;
	weights[HP_REGEN] = 1;
	weights[MP_REGEN] = 1;

	weights[EVASION] = 4;
	weights[BLOCK] = 0;


	int maxBuffCount = ran(level / 3 + 1) + 1;
	if (maxBuffCount > 3) maxBuffCount = 3;

	addBuff(e, weights, level, maxBuffCount, noBuffChance);

	return e;
}

Equipment* getGloves(int level) {
	int rangeMin = 0;
	int rangeMax = 4;

	bool no_armor;
	if (level < 3) {
		no_armor = true;
	} else if (level <= 6) {
		no_armor = ran(5 - level + 1) != 0;
	} else {
		no_armor = false;
	}

	if (no_armor) {
		return NULL;
	} else {
		rangeMin = 1;
		rangeMax = (int)((level - 4)*0.5f) + 1;
		if (rangeMax<1) rangeMax = 1;
		if (rangeMax>4) rangeMax = 4;
	}
	Equipment *e = new Equipment();
	e->slot = GLOVES;
	e->type = rangeMin + ran(rangeMax - rangeMin + 1);
	int DR = 0;
	switch (e->type) {
		case 0:
			break;
		case 1:
			DR = 0 + ran(1 + (level - 2) / 6);
			strcpy_s(e->name, "Cloth Gloves");
			break;
		case 2:
			DR = 1 + ran(1 + (level - 2) / 4);
			strcpy_s(e->name, "Leather Gloves");
			break;
		case 3:
			DR = 1 + ran(2 + (level - 2) / 3);
			strcpy_s(e->name, "Chain Gloves");
			break;
		case 4:
			DR = 2 + ran(2 + (level - 2) / 3);
			strcpy_s(e->name, "Gauntlets");
			break;
	}

	if (DR != 0) {
		e->addBuff(new DRBuff(DR));
	}

	vector<int> weights(BUFF_CNT);
	int noBuffChance = 110 - level * 10;
	if (noBuffChance < 30) noBuffChance = 30;

	weights[NO_BUFF] = 6;

	weights[POISONED] = 0;
	weights[FIRE_DAMAGE] = 2;
	weights[ICE_DAMAGE] = 2;
	weights[LIGHTING_DAMAGE] = 2;

	weights[ATK_SPEED] = 10;
	weights[MV_SPEED] = 0;
	weights[DR_ADD] = 0;
	weights[ATK_DMG] = 4;
	weights[ATK_MUL] = 4;
	weights[SIGHT] = 0;

	weights[HP] = 1;
	weights[MP] = 1;
	weights[HP_REGEN] = 1;
	weights[MP_REGEN] = 1;

	weights[EVASION] = 0;
	weights[BLOCK] = 1;


	int maxBuffCount = ran(level / 3 + 1) + 1;
	if (maxBuffCount > 3) maxBuffCount = 3;

	addBuff(e, weights, level, maxBuffCount, noBuffChance);

	return e;
}

Equipment* getRing(int level) {
	int rangeMin = 0;
	int rangeMax = 4;

	bool no_armor;
	if (level < 3) {
		no_armor = true;
	} else if (level <= 6) {
		no_armor = ran(5 - level + 1) != 0;
	} else {
		no_armor = false;
	}

	if (no_armor) {
		return NULL;
	} else {
		rangeMin = 1;
		rangeMax = 8;
	}
	Equipment *e = new Equipment();
	e->slot = RING;
	e->type = rangeMin + ran(rangeMax - rangeMin + 1);
	switch (e->type) {
		case 0:
			break;
		case 1:
			strcpy_s(e->name, "Azure Ring");
			break;
		case 2:
			strcpy_s(e->name, "Saphire Ring");
			break;
		case 3:
			strcpy_s(e->name, "Diamond Ring");
			break;
		case 4:
			strcpy_s(e->name, "Ruby Ring");
			break;
		case 5:
			strcpy_s(e->name, "Opal Ring");
			break;
		case 6:
			strcpy_s(e->name, "Emerald Ring");
			break;
		case 7:
			strcpy_s(e->name, "Amber Ring");
			break;
		case 8:
			strcpy_s(e->name, "Bone Ring");
			break;
	}

	vector<int> weights(BUFF_CNT);
	int noBuffChance = 110 - level * 10;
	if (noBuffChance < 30) noBuffChance = 30;

	weights[NO_BUFF] = 1;

	weights[POISONED] = 0;
	weights[FIRE_DAMAGE] = 0;
	weights[ICE_DAMAGE] = 0;
	weights[LIGHTING_DAMAGE] = 0;

	weights[ATK_SPEED] = 1;
	weights[MV_SPEED] = 1;
	weights[DR_ADD] = 1;
	weights[ATK_DMG] = 1;
	weights[ATK_MUL] = 1;
	weights[SIGHT] = 1;

	weights[HP] = 1;
	weights[MP] = 1;
	weights[HP_REGEN] = 1;
	weights[MP_REGEN] = 1;

	weights[EVASION] = 1;
	weights[BLOCK] = 0;


	int maxBuffCount = ran(level / 3 + 1) + 1;
	if (maxBuffCount > 3) maxBuffCount = 3;

	addBuff(e, weights, level, maxBuffCount, noBuffChance);

	return e;
}

Equipment* getAmulet(int level) {
	int rangeMin = 0;
	int rangeMax = 4;

	bool no_armor;
	if (level < 3) {
		no_armor = true;
	} else if (level <= 6) {
		no_armor = ran(5 - level + 1) != 0;
	} else {
		no_armor = false;
	}

	if (no_armor) {
		return NULL;
	} else {
		rangeMin = 1;
		rangeMax = 8;
	}
	Equipment *e = new Equipment();
	e->slot = AMULET;
	e->type = rangeMin + ran(rangeMax - rangeMin + 1);
	switch (e->type) {
		case 0:
			break;
		case 1:
			strcpy_s(e->name, "Azure Amulet");
			break;
		case 2:
			strcpy_s(e->name, "Saphire Amulet");
			break;
		case 3:
			strcpy_s(e->name, "Diamond Amulet");
			break;
		case 4:
			strcpy_s(e->name, "Ruby Amulet");
			break;
		case 5:
			strcpy_s(e->name, "Opal Amulet");
			break;
		case 6:
			strcpy_s(e->name, "Emerald Amulet");
			break;
		case 7:
			strcpy_s(e->name, "Amber Amulet");
			break;
		case 8:
			strcpy_s(e->name, "Bone Amulet");
			break;
	}


	vector<int> weights(BUFF_CNT);
	int noBuffChance = 110 - level * 10;
	if (noBuffChance < 30) noBuffChance = 30;

	weights[NO_BUFF] = 1;

	weights[POISONED] = 0;
	weights[FIRE_DAMAGE] = 0;
	weights[ICE_DAMAGE] = 0;
	weights[LIGHTING_DAMAGE] = 0;

	weights[ATK_SPEED] = 1;
	weights[MV_SPEED] = 1;
	weights[DR_ADD] = 1;
	weights[ATK_DMG] = 1;
	weights[ATK_MUL] = 1;
	weights[SIGHT] = 1;

	weights[HP] = 1;
	weights[MP] = 1;
	weights[HP_REGEN] = 1;
	weights[MP_REGEN] = 1;

	weights[EVASION] = 1;
	weights[BLOCK] = 0;


	int maxBuffCount = ran(level / 3 + 1) + 1;
	if (maxBuffCount > 3) maxBuffCount = 3;

	addBuff(e, weights, level, maxBuffCount, noBuffChance);

	return e;
}

Creature* generateGoblin(int level, Pos pos) {
	Creature* c = new Creature();
	strcpy_s(c->name, "Goblin");
	c->level = level;
	c->hpMax = 20 + level * 10 + ran(4) * 5;
	c->hp = c->hpMax;
	c->mp = 0;
	c->minExp = 30.0f + level*10.0f;
	c->maxExp = c->minExp*1.2f;
	c->sight = 6.0f;
	c->movePerTick = 6.0f;
	c->explores = false;
	c->wandersAround = true;
	c->type = GOBLIN;
	c->level = level;
	c->pixel.character = 'g';
	c->pixel.color = getColorIndex(0, 4, 2);
	c->tickToRegen = 100.0f;
	c->reset(pos);

	c->equip(getWeapon(level));
	//c->armor = getArmor(level);
	return c;
}