#include <iostream>
#include <vector>
#include <cstring>
#include <algorithm>
#include <cmath>

#include "Object.h"

using namespace std;

void Robot::setPos(int i, int j) {
	this->x = j * 0.5 + 0.25;
	this->y = 50 - i * 0.5 + 0.25;
}

void Workbench::setPos(int i, int j) {
	this->x = j * 0.5 + 0.25;
	this->y = 50 - i * 0.5 + 0.25;
}

void Map::init() {
	//io加速
	ios::sync_with_stdio(false);
	cin.tie(0);
	cout.tie(0);
	int rNum = 0;//机器人下标
	int wNum = 0;//工作台下标
	for (int i = 0; i < LENGTH; ++i) {
		for (int j = 0; j < LENGTH; ++j) {
			cin >> map[i][j];
			if (map[i][j] == 'A') {
				robots[rNum].setPos(i, j);
				++rNum;
			}
			else if (map[i][j] >= '1' && map[i][j] <= '9') {
				workbenches[wNum].setPos(i, j);
				workbenches[wNum].type = map[i][j] - '0';
				++wNum;
			}
		}
	}
	//计算各个工作台之间的距离
	for (int i = 0; i < wNum; ++i) {
		for (int j = i; j < wNum; ++j) {
			distance[i][j] = distance[j][i] = sqrt(powf(workbenches[i].x - workbenches[j].x, 2) + powf(workbenches[i].y - workbenches[j].y, 2));
		}
	}
	cout << "OK" << endl;
}

void Map::frameInput() {
	//io加速
	ios::sync_with_stdio(false);
	cin.tie(0);
	cout.tie(0);
	cin >> frameNumber >> money;
	cin >> workbenchNum;
	for (int i = 0; i < workbenchNum; ++i) {
		cin >> workbenches[i].type >> workbenches[i].x >> workbenches[i].y >> workbenches[i].restTime
			>> workbenches[i].materialState >> workbenches[i].productState;
	}
	for (int i = 0; i < MAXROBOTS; ++i) {
		cin >> robots[i].workbenchId >> robots[i].carryType >> robots[i].timeValue >> robots[i].collisionValue
			>> robots[i].w >> robots[i].vx >> robots[i].vy >> robots[i].toward >> robots[i].x >> robots[i].y;
	}
	string ok;
	cin >> ok;
}

void Map::output() {
	cout << frameNumber << endl;
	for (int i = 0; i < MAXROBOTS; ++i) {
		for (Instruction ins : robots[i].instructions) {
			if (ins.instruct == "forward" || ins.instruct == "rotate")
				cout << ins.instruct << " " << ins.robotId << " " << ins.param << endl;
			else cout << ins.instruct << " " << ins.robotId << endl;
		}
	}
	cout << "OK" << endl;
}