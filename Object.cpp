#include <iostream>
#include <vector>
#include <cstring>
#include <algorithm>
#include <cmath>
#include <fstream>

#include "Object.h"

using namespace std;

ofstream fout("C:\Users\ASUS\Desktop\新建文件夹\2023\WindowsRelease\log.txt");
//给定指令集合
const string Instruction::FORWARD = "forward";
const string Instruction::ROTATE = "rotate";
const string Instruction::BUY = "buy";
const string Instruction::SELL = "sell";
const string Instruction::DESTORY = "destory";

void Robot::setPos(int i, int j) {
	this->x = j * 0.5 + 0.25;
	this->y = 50 - i * 0.5 - 0.25;
}

void Robot::setInstruct(string ins, int id, float par) {
	Instruction temp;
	temp.instruct = ins;
	temp.robotId = id;
	temp.param = par;
	instructions.push_back(temp);
}

void Workbench::setPos(int i, int j) {
	this->x = j * 0.5 + 0.25;
	this->y = 50 - i * 0.5 - 0.25;
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
	string ok;
	cin >> ok;
	cout << "OK" << endl;
}

void Map::frameInput() {
	if (!cin)exit(0);
	//io加速
	ios::sync_with_stdio(false);
	cin.tie(0);
	cout.tie(0);
	cin >> frameNumber >> money;
	//cin >> money;
	cin >> workbenchNum;
	for (int i = 0; i < workbenchNum; ++i) {
		cin >> workbenches[i].type >> workbenches[i].x >> workbenches[i].y >> workbenches[i].restTime
			>> workbenches[i].materialState >> workbenches[i].productState;
	}
	for (int i = 0; i < MAXROBOTS; ++i) {
		cin >> robots[i].workbenchId >> robots[i].carryType >> robots[i].timeValue >> robots[i].collisionValue
			>> robots[i].w >> robots[i].vx >> robots[i].vy >> robots[i].toward >> robots[i].x >> robots[i].y;
		//设定半径与质量
		robots[i].R = (robots[i].carryType == EMPTY) ? RR1 : RR2;
		robots[i].quantity = (robots[i].carryType == EMPTY) ? QUANTITY1 : QUANTITY2;
		robots[i].v = sqrtf(robots[i].vx * robots[i].vx + robots[i].vy * robots[i].vy); 
	}
	string ok;
	cin >> ok;
}

void Map::output() {
	cout << frameNumber << endl;
	for (int i = 0; i < MAXROBOTS; ++i) {
		for (Instruction ins : robots[i].instructions) {
			cout << ins.instruct << " " << ins.robotId;
			if (ins.instruct == Instruction::FORWARD || ins.instruct == Instruction::ROTATE)cout << " " << ins.param;
			cout << endl;
		}
		robots[i].instructions.clear();
	}
	cout << "OK" << endl;
}

void Map::strategy() {
	for (int i = 0; i < MAXROBOTS; ++i) {
		if (robots[i].v <= 1e-8) {
			robots[i].target_id = rand() % workbenchNum; 
		}
		float next = get_angular_velocity(robots[i], workbenches[robots[i].target_id]);
		robots->setInstruct(Instruction::ROTATE, i, next);
		if (next == 0) { 
			robots->setInstruct(Instruction::FORWARD, i, get_line_speed(robots[i], workbenches[robots[i].target_id]));
		}
	}
}

float dot(Robot& a, Workbench& b) {
	return cos(a.toward) * (b.x - a.x) + sin(a.toward) * (b.y - a.y);
}

float cross(Robot& a, Workbench& b) {
	return cos(a.toward) * (b.y - a.y) - sin(a.toward) * (b.x - a.x);
}

float radian(Robot& a, Workbench& b) {
	float res = acos(dot(a, b) / sqrtf((b.x - a.x) * (b.x - a.x) + (b.y - a.y) * (b.y - a.y)));
	return cross(a, b) > 0 ? res : -res;
}

float get_angular_velocity(Robot& a, Workbench& b) {
	float a_ = MAXTORQUE / (a.quantity * a.R * a.R);
	float S = radian(a, b);
	//当前速度开始减速到0，会转多少
	float low = (a.w * a.w) / (2 * a_); 
	//现在减速恰好
	if (fabs(S) - low <= 1e-8) {
		return 0;
	}
	//否则继续加速或者匀速，直接设最大旋转速度
	else return S > 0 ? MAXSPIN : -MAXSPIN;
}

float get_line_speed(Robot& a, Workbench& b) {
	float S = sqrtf((b.x - a.x) * (b.x - a.x) + (b.y - a.y) * (b.y - a.y));
	float v = sqrtf(a.vx * a.vx + a.vy * a.vy);
	float a_ = MAXTRACTION / a.quantity;
	float low = (v * v) / (2 * a_);
	if (S - low <= 0.4 - 1e-8)return 0;
	else return MAXFORWARD;
}
 