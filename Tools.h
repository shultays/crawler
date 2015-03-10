#pragma once

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
#define WINDOWS
#else
#define LINUX
#endif

#include<unordered_map>
#include<string.h>
#include<stack>
#include<vector>
#include<queue>
#include<assert.h>
#ifdef LINUX
#include <unistd.h>
#endif
#ifdef WINDOWS
#include <windows.h>
#endif
#undef MOUSE_MOVED
#include<curses.h>

using namespace std;

#define MAZE_W 75
#define MAZE_H 75

#define UP 0
#define LEFT 1
#define DOWN 2
#define RIGHT 3

#define UP_LEFT 4
#define UP_RIGHT 5
#define DOWN_RIGHT 6
#define DOWN_LEFT 7

#define MAX_MESSAGE 100

extern unordered_map<int, float> moveWeights;
extern unordered_map<int, float> addWeights;
extern unordered_map<int, float> seeWeights;

#define B_UP 1
#define B_LEFT 2
#define B_DOWN 4
#define B_RIGHT 8

#define DIAG_MUL 1.41421356237f

#define ADVENTURER 11
#define ADVENTURER_ALLY 12

#define GOBLIN 101

#define FIGHTER 0
#define MAGE 1
#define ROGUE 2
#define PRIEST 3

int ran(int n);
float ranf(float a, float b);
int rani(int a, int b);
void initTools();

class Pos {
public:
	int x, y;
	Pos() {
		x = y = -1;
	}
	Pos(int x, int y) {
		this->x = x;
		this->y = y;
	}


	bool operator == (const Pos& other) const {
		return (x == other.x && y == other.y);
	}
	bool operator != (const Pos& other) const {
		return (x != other.x || y != other.y);
	}
};

class Creature;

class AttackListener {
public:
	virtual void attacked(Creature* attacker, Creature* defender, int damage) = 0;
};


class Pixel {
public:
	int character;
	int color;
};


#define WEAPON 0
#define ARMOR 1
#define SHIELD 2
#define HELM 3
#define GLOVES 4
#define BOOTS 5
#define RING 6
#define AMULET 7
#define EQ_MAX 8

extern char eqipmentNames[EQ_MAX][32];
extern int eqipmentIcons[EQ_MAX];

extern int messagePos;
extern char messages[MAX_MESSAGE][128];
extern int globalVisible[MAZE_W][MAZE_H];

class Node {
public:
	Pos pos;
	int dir;
	float path;

	Node(int x, int y) {
		pos.x = x;
		pos.y = y;
		dir = 0;
		path = 0;
	}
	Node(int x, int y, int dir) {
		pos.x = x;
		pos.y = y;
		this->dir = dir;
		this->path = 1.0f;
	}

};

int dist(Pos& p1, Pos& p2);
int getReverseDir(int dir);

int getSideDir(int dir, int side);
class CompareNode {
public:
	CompareNode(const bool& revparam = true) {}
	bool operator() (const Node& lhs, const Node& rhs) const {
		return (lhs.path > rhs.path);
	}
};

template<int W, int H>
bool doSearchExplore(Pos startPos, int arr[W][H], unordered_map<int, float> weights, float max_weight, int targetArr[W][H], int targetVal, Pos&endPos, vector<Pos> &path, vector<Pos>& exploredPos, bool ignoreDiag = false, bool addLastIfMinus = false) {
	int explored[W][H];
	exploredPos.clear();
	path.clear();
	std::priority_queue<Node, std::vector<Node>, CompareNode> queue;
	memset(explored, -1, sizeof(explored));

	queue.emplace(Node(startPos.x, startPos.y));
	exploredPos.push_back(startPos);
	bool found = false;
	while (queue.size()) {
		Node n = queue.top();
		queue.pop();
		if (targetVal >= 0 && targetArr[n.pos.x][n.pos.y] == targetVal) {
			endPos = n.pos;
			found = true;
			break;
		}

		for (int i = 0; i < (ignoreDiag ? 4 : 8); i++) {
			Pos nPos = getPosForDir(n.pos, i);
			if (nPos.x < 0 || nPos.y < 0 || nPos.x >= W || nPos.y >= H) continue;
			if (explored[nPos.x][nPos.y] == -1) {
				float weight = weights[arr[nPos.x][nPos.y]];
				if (i >= 4) {
					weight *= DIAG_MUL;
				}
				if (weight >= 0 && n.path + weight <= max_weight) {
					explored[nPos.x][nPos.y] = i;
					Node newNode = Node(nPos.x, nPos.y);
					newNode.path = n.path + weight;
					queue.emplace(newNode);
					exploredPos.push_back(nPos);
				} else if (weight < 0 && addLastIfMinus) {
					exploredPos.push_back(nPos);
				}
			}
		}
	}
	if (found) {
		vector<Pos> revPath;
		Pos pos = endPos;
		while (pos.x != startPos.x || pos.y != startPos.y) {
			revPath.push_back(pos);
			int reverseDir = getReverseDir(explored[pos.x][pos.y]);
			pos = getPosForDir(pos, reverseDir);
		}

		for (int i = revPath.size() - 1; i >= 0; i--) {
			path.push_back(revPath[i]);
			if (i == 0) {
				int a = 5;
			}
		}
	}
	return found;
}

template<int W, int H>
bool doSearch(Pos startPos, int arr[W][H], unordered_map<int, float> weights, float max_weight, int targetArr[W][H], int targetVal, Pos &endPos, vector<Pos>& path, bool ignoreDiag = false) {
	vector<Pos> exploredPos;
	return doSearchExplore<W, H>(startPos, arr, weights, max_weight, targetArr, targetVal, endPos, path, exploredPos, ignoreDiag);
}

template<int W, int H>
bool findPath(Pos startPos, Pos endPos, int arr[W][H], unordered_map<int, float> weights, float max_weight, vector<Pos>& path, bool ignoreDiag = false) {
	if (startPos == endPos) {
		path.clear();
		path.push_back(endPos);
		return true;
	}

	vector<Pos> exploredPos;
	int oldVal = arr[endPos.x][endPos.y];
	arr[endPos.x][endPos.y] = 0;
	int targetArr[W][H];
	memset(targetArr, 0, sizeof(targetArr));
	targetArr[endPos.x][endPos.y] = 1;
	bool ret = doSearchExplore<W, H>(startPos, arr, weights, max_weight, targetArr, 1, endPos, path, exploredPos, ignoreDiag);

	arr[endPos.x][endPos.y] = oldVal;
	return ret;
}

template<int W, int H>
void doExplore(Pos startPos, int arr[W][H], unordered_map<int, float> weights, float max_weight, vector<Pos>& exploredPos, bool ignoreDiag = false, bool addLastIfMinus = false) {
	Pos endPos;
	vector<Pos> path;
	int targetArr[W][H];
	doSearchExplore<W, H>(startPos, arr, weights, max_weight, targetArr, -1, endPos, path, exploredPos, ignoreDiag, addLastIfMinus);
}

Pos getPosForDir(Pos i, int dir);

int getColorIndex(int r, int g, int b);

void pushMessage(char *str);

void msleep(int sleepMS);

unsigned mtime();

enum {
	NO_BUFF = 0,
	ATK_SPEED,
	MV_SPEED,
	DR_ADD,
	ATK_DMG,
	ATK_MUL,
	SIGHT,
	POISONED,
	FIRE_DAMAGE,
	LIGHTING_DAMAGE,
	ICE_DAMAGE,
	EVASION,
	BLOCK,
	HP,
	MP,
	HP_REGEN,
	MP_REGEN,

	BUFF_CNT
};

extern char buffNamers[BUFF_CNT][32];
extern char firstBuffNamers[BUFF_CNT][32];

class Buff;

Buff* getBuff(vector<int>& weights, int level);

