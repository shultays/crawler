#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stack>
#include<vector>
#include<queue>
#include<curses.h>

#define UP 0
#define LEFT 1
#define DOWN 2
#define RIGHT 3


#define B_UP 1
#define B_LEFT 2
#define B_DOWN 4
#define B_RIGHT 8

using namespace std;

WINDOW * win;

int ran(int n) {
	return rand() % n;
}


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
};
Pos player;
class Node {
public:
	Pos pos;
	int dir;
	int path;


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
		this->path = 1;
	}

};

void fixDir(int& dir) {
	while (dir <0) dir += 4;
	while (dir >3) dir -= 4;
}


Pos getPosForDir(Pos i, int dir) {
	fixDir(dir);
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
	}
	return i;
}

int changeDirChance = 10; //1
int threeWayPathChance = 3; //2
int fourWayPathChance = 1; //3
int stopChance = 2;  //4

template <int N, int M>
class Maze {
public:
	char arr[N][M];
	char room[N][M];
	char walls[N * 3][N * 3];

	int roomCount;
	vector<Pos> roomPos;

	int getRandomAction(int noActionChance, float changeDirMult, float threeWayMult, float fourWayMult, float stopMult) {
		int modifiedChangeDirChance = (int)(changeDirChance*changeDirMult);
		int modifiedThreeWayPathChance = (int)(threeWayPathChance*threeWayMult);
		int modofiedFourWayPathChance = (int)(fourWayMult*fourWayPathChance);
		int modifiedStopChance = (int)(stopChance*stopMult);

		int totalChance = noActionChance + modifiedChangeDirChance + modifiedThreeWayPathChance + modofiedFourWayPathChance + modifiedStopChance;
		if (totalChance == 0) {
			return 4;
		}
		int c = ran(totalChance);

		if (c < noActionChance) {
			return 0;
		}
		c -= noActionChance;


		if (c < modifiedChangeDirChance) {
			return 1;
		}
		c -= modifiedChangeDirChance;
		if (c < modifiedThreeWayPathChance) {
			return 2;
		}
		c -= modifiedThreeWayPathChance;
		if (c < modofiedFourWayPathChance) {
			return 3;
		}
		c -= modofiedFourWayPathChance;
		if (c < modifiedStopChance) {
			return 4;
		}
		c -= modifiedStopChance;

		return 4;
	}


	Maze() {
		memset(arr, 0, sizeof(arr));
		memset(room, 0, sizeof(room));
		memset(walls, ' ', sizeof(walls));
	}
	int isPosValid3(Pos pos) {
		if (pos.x < 0) return 0;
		if (pos.y < 0) return 0;
		if (pos.x >= 3 * N) return 0;
		if (pos.y >= 3 * M) return 0;
		return 1;
	}

	int isPosValid(Pos pos) {
		if (pos.x < 0) return 0;
		if (pos.y < 0) return 0;
		if (pos.x >= N) return 0;
		if (pos.y >= M) return 0;
		return 1;
	}

	int isNodeEmpty(Pos pos) {
		if (!isPosValid(pos)) return 0;
		return arr[pos.x][pos.y] == 0;
	}
	void write() {

		FILE *f;

		fopen_s(&f, "out3.txt", "w");


		for (int i = 0; i < N; i++) {
			for (int k = 0; k < 3; k++) {
				for (int j = 0; j < M; j++) {
					fprintf(f, "%c%c%c", walls[i * 3 + k][j * 3], walls[i * 3 + k][j * 3 + 1], walls[i * 3 + k][j * 3 + 2]);
				}
				fprintf(f, "\n");
			}

		}


		fclose(f);

		FILE *f2;
		fopen_s(&f2, "out2.txt", "wb");
		int dat[] = {
			0x20,
			0x8094e2,
			0x8294e2,
			0x9894e2,
			0x8094e2,
			0x8094e2,
			0x9494e2,
			0xb494e2,
			0x8294e2,
			0x9094e2,
			0x8294e2,
			0xa494e2,
			0x8c94e2,
			0xac94e2,
			0x9c94e2,
			0xbc94e2,
		};

		int t = 0xbfbbef;

		fwrite(&t, 3, 1, f2);
		for (int i = 0; i < N; i++) {
			for (int j = 0; j < M; j++) {

				fwrite(&dat[arr[i][j]], 1, arr[i][j] == 0 ? 1 : 3, f2);



			}
			int t = 0xa0d;
			fwrite(&t, 1, 2, f2);
		}
		fclose(f2);

		int a = 5;
	}

	void genRand(int n) {
		genRooms();
		genPaths(n);
		genWalls();
		connectRooms();
		checkUnreached();
		fixSides();
		write();
	}

	void fixSides() {
		printw("fixSides..."); refresh();
		for (int i = 0; i < 3 * N; i++) {
			walls[i][0] = '*';
			walls[i][3 * M - 1] = '*';
		}

		for (int j = 0; j < 3 * M; j++) {
			walls[0][j] = '*';
			walls[3 * N - 1][j] = '*';
		}
		player.x = roomPos[0].x * 3 + 1;
		player.y = roomPos[0].y * 3 + 1;
		printw("  done\n"); refresh();
	}
	void checkUnreached() {
		printw("checkUnreached..."); refresh();
		queue<Pos> queue;
		int reached[N * 3][M * 3];
		memset(reached, 0, sizeof(reached));
		queue.emplace(Pos(roomPos[0].x * 3 + 1, roomPos[0].y * 3 + 1));
		while (queue.size() > 0) {
			Pos pos = queue.front();
			queue.pop();
			reached[pos.x][pos.y] = 1;

			if (walls[pos.x][pos.y] == ' ') {
				for (int i = 0; i < 4; i++) {
					Pos nPos = getPosForDir(pos, i);
					if (isPosValid3(nPos) && reached[nPos.x][nPos.y] == 0) {
						reached[nPos.x][nPos.y] = 1;
						queue.emplace(nPos);
					}
				}
			}
		}

		for (int i = 0; i < 3 * N; i++) {
			for (int j = 0; j < 3 * M; j++) {
				if (reached[i][j] == 0 && walls[i][j] == ' ') {
					walls[i][j] = '*';
				}
			}
		}
		printw(" done\n"); refresh();
	}

	class CompareNode {
		bool reverse;
	public:
		CompareNode(const bool& revparam = true) {
			reverse = revparam;
		}
		bool operator() (const Node& lhs, const Node& rhs) const {
			if (reverse) return (lhs.path > rhs.path);
			else return (lhs.path < rhs.path);
		}
	};

	void connectTwoRooms(int i, int j) {
		printw("\tconnecting %d %d ...", i, j); refresh();
		std::priority_queue<Node, std::vector<Node>, CompareNode> queue;
		int explored[3 * N][3 * M] = { -1 };
		memset(explored, -1, sizeof(explored));
		Pos initPos = Pos(roomPos[i].x * 3 + 1, roomPos[i].y * 3 + 1);
		queue.emplace(Node(initPos.x, initPos.y));
		int target = j + 1;
		Pos foundPos;
		while (queue.size()) {
			Node n = queue.top();
			queue.pop();
			if (room[n.pos.x / 3][n.pos.y / 3] == target) {
				foundPos = n.pos;
				break;
			}

			for (int i = 0; i < 4; i++) {
				Pos nPos = getPosForDir(n.pos, i);
				if (isPosValid3(nPos) && explored[nPos.x][nPos.y] == -1) {
					explored[nPos.x][nPos.y] = i;

					Node newNode = Node(nPos.x, nPos.y);
					newNode.path = n.path + (walls[nPos.x][nPos.y] == ' ' ? 1 : 10);
					queue.emplace(newNode);
				}
			}

		}

		do {
			walls[foundPos.x][foundPos.y] = ' ';
			int reverseDir = explored[foundPos.x][foundPos.y] + 2;
			fixDir(reverseDir);
			foundPos = getPosForDir(foundPos, reverseDir);
		} while (foundPos.x != initPos.x || foundPos.y != initPos.y);

		printw(" done\n"); refresh();
	}


	void connectRooms() {
		printw("connectRooms...\n"); refresh();
		for (int b = roomCount - 1; b > 0; b--) {
			int a = ran(b);

			connectTwoRooms(a, b);
		}

		for (int i = 0; i < roomCount * 4; i++) {
			int a = ran(roomCount);
			int b;
			do {
				b = ran(roomCount);
			} while (b == a);
			connectTwoRooms(a, b);
		}
		printw(" done\n"); refresh();
	}

	void genRooms() {
		printw("genRooms..."); refresh();
		roomCount = 0;
		for (int a = 0; a < N*M / 10; a++) {
			int w = 3 + ran(5);
			int h = 3 + ran(5);
			int x = ran(N - w);
			int y = ran(M - h);

			int canAdd = 1;

			for (int i = x; i < x + w; i++) {
				for (int j = y; j < y + h; j++) {
					if (arr[i][j] != 0) canAdd = 0;
				}
			}

			if (canAdd) {
				roomCount++;
				roomPos.push_back(Pos(x + w / 2, y + h / 2));
				for (int i = x; i < x + w; i++) {
					for (int j = y; j < y + h; j++) {
						arr[i][j] = 0x10;
						if (j == y)
							arr[i][j] |= B_UP;
						if (i == x)
							arr[i][j] |= B_LEFT;
						if (j == y + h - 1)
							arr[i][j] |= B_DOWN;
						if (i == x + w - 1)
							arr[i][j] |= B_RIGHT;
						if (arr[i][j] == 0x10) {
							room[i][j] = roomCount;
						}
					}
				}
			}
		}
		printw(" done\n"); refresh();
	}

	void genWalls() {
		printw("genWalls..."); refresh();
		for (int i = 0; i < N; i++) {
			for (int j = 0; j < M; j++) {

				int t = arr[i][j];

				if (t == -1) {

					walls[i * 3][j * 3] = '*';
					walls[i * 3 + 2][j * 3] = '*';
					walls[i * 3 + 2][j * 3 + 2] = '*';
					walls[i * 3][j * 3 + 2] = '*';


					walls[i * 3 + 1][j * 3] = '*';
					walls[i * 3][j * 3 + 1] = '*';
					walls[i * 3 + 1][j * 3 + 2] = '*';
					walls[i * 3 + 2][j * 3 + 1] = '*';

					walls[i * 3 + 1][j * 3 + 1] = '*';
				} else if (t == 0) {


				} else if (t < 16) {
					walls[i * 3][j * 3] = '*';
					walls[i * 3 + 2][j * 3] = '*';
					walls[i * 3 + 2][j * 3 + 2] = '*';
					walls[i * 3][j * 3 + 2] = '*';


					walls[i * 3 + 1][j * 3] = t & B_UP ? ' ' : '*';
					walls[i * 3][j * 3 + 1] = t & B_LEFT ? ' ' : '*';
					walls[i * 3 + 1][j * 3 + 2] = t & B_DOWN ? ' ' : '*';
					walls[i * 3 + 2][j * 3 + 1] = t & B_RIGHT ? ' ' : '*';

				} else if (t & 16) {
					t = t & 15;

					if (t == 0) {

					} else {
						walls[i * 3][j * 3] = t & (B_UP | B_LEFT) ? '*' : ' ';
						walls[i * 3 + 2][j * 3] = t & (B_UP | B_RIGHT) ? '*' : ' ';
						walls[i * 3 + 2][j * 3 + 2] = t & (B_DOWN | B_RIGHT) ? '*' : ' ';
						walls[i * 3][j * 3 + 2] = t & (B_DOWN | B_LEFT) ? '*' : ' ';


						walls[i * 3 + 1][j * 3] = t & B_UP ? '*' : ' ';
						walls[i * 3][j * 3 + 1] = t & B_LEFT ? '*' : ' ';
						walls[i * 3 + 1][j * 3 + 2] = t & B_DOWN ? '*' : ' ';
						walls[i * 3 + 2][j * 3 + 1] = t & B_RIGHT ? '*' : ' ';

					}
				}
			}
		}


		for (int i = 0; i < N; i++) {
			for (int j = 0; j < M; j++) {

				int t = arr[i][j];

				if (t == -1) {
				} else if (t == 0) {
				} else if (t < 16) {
				} else if (t & 16) {
					t = t & 15;

					if (t == 0) {

					} else {
						Pos p;
						p.x = i * 3 + 1;
						p.y = j * 3 + 1;
						if (t == B_LEFT && ran(5) == 0) {
							Pos nPos = p;
							nPos.x -= 3;
							if (isPosValid3(nPos) && walls[nPos.x][nPos.y] == ' ') {
								walls[p.x - 1][p.y] = ' ';
								walls[p.x - 2][p.y] = ' ';
							}
						}

						if (t == B_UP && ran(5) == 0) {
							Pos nPos = p;
							nPos.y -= 3;
							if (isPosValid3(nPos) && walls[nPos.x][nPos.y] == ' ') {
								walls[p.x][p.y - 1] = ' ';
								walls[p.x][p.y - 2] = ' ';
							}
						}

						if (t == B_RIGHT && ran(5) == 0) {
							Pos nPos = p;
							nPos.x += 3;
							if (isPosValid3(nPos) && walls[nPos.x][nPos.y] == ' ') {
								walls[p.x + 1][p.y] = ' ';
								walls[p.x + 2][p.y] = ' ';
							}
						}


						if (t == B_DOWN && ran(5) == 0) {
							Pos nPos = p;
							nPos.y += 3;
							if (isPosValid3(nPos) && walls[nPos.x][nPos.y] == ' ') {
								walls[p.x][p.y + 1] = ' ';
								walls[p.x][p.y + 2] = ' ';
							}
						}
					}
				}
			}
		}
		printw(" done\n"); refresh();
	}

	void genPaths(int pathLen) {
		printw("genPaths..."); refresh();
		vector<Node> nodes;

		vector<Pos> allPos;
		for (int i = 0; i < N; i++) {
			for (int j = 0; j < M; j++) {
				Pos p;
				p.x = i;
				p.y = j;
				if (isNodeEmpty(p)) {
					allPos.push_back(p);

				}
			}
		}

		while (allPos.size() > 0) {
			Pos p = allPos[ran(allPos.size())];
			Node n = Node(p.x, p.y, ran(4));
			nodes.push_back(n);

			vector<Pos> added;
			while (nodes.size() > 0) {
				for (unsigned i = 0; i < nodes.size(); i++) {
					Node& n = nodes[i];

					for (unsigned j = 0; j < allPos.size(); j++) {
						if (allPos[j].x == n.pos.x && allPos[j].y == n.pos.y) {
							allPos[j] = allPos[allPos.size() - 1];
							allPos.pop_back();
							break;
						}
					}

					added.push_back(n.pos);
					int mult = pathLen - n.path;
					int noActionChance = (pathLen - n.path) * 10;
					if (noActionChance < 0) noActionChance = 0;
					Pos nextPos = getPosForDir(n.pos, n.dir);

					int left = isNodeEmpty(getPosForDir(n.pos, n.dir + 1));
					int right = isNodeEmpty(getPosForDir(n.pos, n.dir - 1));
					int front = isNodeEmpty(getPosForDir(n.pos, n.dir));

					if (!front)
						noActionChance = 0;

					int nCount = left + right + front;


					float changeDirMult = nCount >= 1 ? 1.0f : 0.0f;
					float threeWayMult = nCount >= 2 ? 1.0f : 0.0f;
					float fourWayMult = nCount >= 3 ? 1.0f : 0.0f;
					float stopMult = nodes.size() > 1 ? 1.0f : 0.0f;

					//printw("%d << %d %d << %d %f %f %f %f  << %d %d %d\n", n.dir, n.pos.x, n.pos.y, noActionChance, changeDirMult, threeWayMult, fourWayMult, stopMult, left, right, front);

					if (n.pos.x == 15 && n.pos.y == 8) {
						int a = 5;
					}
					int nextAction = getRandomAction(noActionChance, changeDirMult, threeWayMult, fourWayMult, stopMult);

					int oldDir = n.dir + 2;
					fixDir(oldDir);

					if (arr[n.pos.x][n.pos.y] != 0) {
						nextAction = 4;

					} else {
						arr[n.pos.x][n.pos.y] |= 1 << oldDir;

					}

					if (nextAction == 0) {
						arr[n.pos.x][n.pos.y] |= 1 << n.dir;
						n.pos = getPosForDir(n.pos, n.dir);
						n.path++;
					} else if (nextAction == 1) {
						int z;
						if (left && right) {
							z = ran(2) == 0 ? -1 : 1;
						} else if (left) {
							z = +1;
						} else {
							z = -1;
						}

						n.dir += z;
						fixDir(n.dir);
						arr[n.pos.x][n.pos.y] |= 1 << n.dir;
						n.pos = getPosForDir(n.pos, n.dir);
						n.path = 0;
					} else if (nextAction == 2) {

						int ns[4];
						int j = 0;
						if (left) ns[j++] = n.dir + 1;
						if (right) ns[j++] = n.dir - 1;
						if (front) ns[j++] = n.dir;
						int n1 = ran(j);
						int n2;
						do {
							n2 = ran(j);
						} while (n2 == n1);
						n1 = ns[n1];
						n2 = ns[n2];
						fixDir(n1);
						fixDir(n2);
						n.dir = n1;

						arr[n.pos.x][n.pos.y] |= 1 << n1;
						arr[n.pos.x][n.pos.y] |= 1 << n2;

						n.path = 0;

						Node newNode = Node(n.pos.x, n.pos.y, n2);
						n.pos = getPosForDir(n.pos, n.dir);
						newNode.pos = getPosForDir(newNode.pos, n2);
						nodes.push_back(newNode);
					} else if (nextAction == 3) {
						int n1 = n.dir - 1;
						int n2 = n.dir + 1;
						fixDir(n1);
						fixDir(n2);

						arr[n.pos.x][n.pos.y] |= 1 << n1;
						arr[n.pos.x][n.pos.y] |= 1 << n2;
						arr[n.pos.x][n.pos.y] |= 1 << n.dir;
						n.path = 0;

						n.pos = getPosForDir(n.pos, n.dir);

						Node newNode = Node(n.pos.x, n.pos.y, n1);
						Node newNode2 = Node(n.pos.x, n.pos.y, n2);
						newNode.pos = getPosForDir(newNode.pos, n1);
						newNode2.pos = getPosForDir(newNode2.pos, n2);
						nodes.push_back(newNode);
						nodes.push_back(newNode2);

					} else {
						if (nodes.size() > 0) {

							nodes[i] = nodes[nodes.size() - 1];
							nodes.pop_back();
							i--;
						}
					}
				}
			}

			if (added.size() < 2) {
				for (unsigned i = 0; i < added.size(); i++) {
					arr[added[i].x][added[i].y] = -1;
				}
			}
		}

		printw(" done\n"); refresh();
	}

};

Maze<25, 25> maze;

void control(int i) {
	bool moved = false;
	Pos newP;
	switch (i) {
		case KEY_UP:
			newP = getPosForDir(player, UP);
			moved = true;
			break;
		case KEY_DOWN:
			newP = getPosForDir(player, DOWN);
			moved = true;
			break;
		case KEY_LEFT:
			newP = getPosForDir(player, LEFT);
			moved = true;
			break;
		case KEY_RIGHT:
			newP = getPosForDir(player, RIGHT);
			moved = true;
			break;

	}

	if (moved) {
		if (maze.isPosValid3(newP) && maze.walls[newP.x][newP.y] == ' ') {
			player = newP;
		}
	}
}
void handle_winch(int sig) {
	endwin();

}
void gameRefresh() {
	erase();
	int row, col;
	getmaxyx(stdscr, row, col);

	for (int i = 0; i < row; i++) {
		for (int j = 0; j < col; j++) {
			Pos p;
			p.x = player.x + i - row / 2;
			p.y = player.y + j - col / 2;

			if (p.x == player.x && p.y == player.y) {
				mvaddch(i, j, '@');
			} else if (maze.isPosValid3(p) && maze.walls[p.x][p.y] == ' ') {
				mvaddch(i, j, ' ');
			} else {
				mvaddch(i, j, 'X');
			}
		}
	}


	refresh();
}

int main() {
	win = initscr();
	scrollok(win, true);
	keypad(win, true);
	nodelay(win, true);
	curs_set(0);
	maze.genRand(3);


	resize_term(60, 80);

	scrollok(win, false);
	gameRefresh();
	while (true) {
		int i = getch();
		if (i != ERR) {
			control(i);
			gameRefresh();
		}
	}
	endwin();

	return 0;
}