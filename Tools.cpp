#include "Tools.h"

unordered_map<int, float> moveWeights;
unordered_map<int, float> addWeights;
unordered_map<int, float> seeWeights;

int ran(int n) {
	return rand() % n;
}
float ranf(float a, float b) {
	return a + (b - a)*(ran(10000) / 10000.0f);
}

int rani(int a, int b) {
	return a + ran(b - a + 1);
}
void initTools() {
	seeWeights.clear();
	seeWeights[0] = 1.0f;
	seeWeights[1] = -1.0f;
	seeWeights[2] = -1.0f;

	seeWeights[ADVENTURER] = 0.0f;
	seeWeights[ADVENTURER_ALLY] = 0.0f;
	seeWeights[GOBLIN] = 0.0f;

	addWeights.clear();
	addWeights[0] = 1.0f;
	addWeights[1] = -1.0f;
	addWeights[2] = -1.0f;

	addWeights[ADVENTURER] = 1.0f;
	addWeights[ADVENTURER_ALLY] = 1.0f;
	addWeights[GOBLIN] = 1.0f;

	moveWeights.clear();
	moveWeights[0] = 1.0f;
	moveWeights[1] = -1.0f;
	moveWeights[2] = -1.0f;


	moveWeights[ADVENTURER] = -1.0f;
	moveWeights[ADVENTURER_ALLY] = -1.0f;
	moveWeights[GOBLIN] = -1.0f;
}



int dist(Pos& p1, Pos& p2) {
	return abs(p1.x - p2.x) + abs(p1.y - p2.y);
}


int getReverseDir(int dir) {
	if (dir < 4) {
		return (dir + 2) % 4;
	} else {
		int t = dir - 4;
		t = (dir + 2) % 4;
		return t + 4;
	}
}

int getSideDir(int dir, int side) {
	if (dir < 4) {
		int t = dir + side;
		if (dir < 0) dir += 4;
		return dir % 4;
	} else {
		int t = dir + side - 4;
		if (dir < 0) dir += 4;
		return (dir % 4) + 4;
	}
}


Pos getPosForDir(Pos i, int dir) {
	switch (dir) {
		case UP:
			i.x--;
			break;
		case LEFT:
			i.y--;
			break;
		case DOWN:
			i.x++;
			break;
		case RIGHT:
			i.y++;
			break;


		case UP_LEFT:
			i.x--;
			i.y--;
			break;
		case UP_RIGHT:
			i.x--;
			i.y++;
			break;
		case DOWN_LEFT:
			i.x++;
			i.y--;
			break;
		case DOWN_RIGHT:
			i.x++;
			i.y++;
			break;
	}
	return i;
}


int getColorIndex(int r, int g, int b) {
	return (b >> 1) | (g << 2) | (r << 5);
}


void pushMessage(char *str) {
	messagePos++;
	if (messagePos >= MAX_MESSAGE) {
		messagePos -= MAX_MESSAGE;
	}
	strcpy_s(messages[messagePos], str);
}



void msleep(int sleepMS) {
#ifdef LINUX
	usleep(sleepMs * 1000);
#endif
#ifdef WINDOWS
	Sleep(sleepMS);
#endif
}



unsigned mtime() {
#ifdef LINUX
	return GetTickCount(); //linux
#endif
#ifdef WINDOWS
	return (unsigned)GetTickCount();
#endif
}