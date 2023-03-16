#include <iostream>
#include <vector>
#include <cstring>
#include <algorithm>
#include <cmath>
#include <fstream>
#include <set>

#include "Object.h"

using namespace std;

//ofstream fout("C:\Users\ASUS\Desktop\新建文件夹\2023\WindowsRelease\log.txt");
//给定指令集合
const string Instruction::FORWARD = "forward";
const string Instruction::ROTATE = "rotate";
const string Instruction::BUY = "buy";
const string Instruction::SELL = "sell";
const string Instruction::DESTORY = "destory";
const int product[10] = { 0,0,0,0,6,10,12,112,128,254 };

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
	srand((unsigned int)time(NULL));
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
				robots[rNum].target_id = -1;
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
	cin >> workbenchNum;
	require.clear();
	resource.clear();
	for (int i = 0; i < workbenchNum; ++i) {
		cin >> workbenches[i].type >> workbenches[i].x >> workbenches[i].y >> workbenches[i].restTime
			>> workbenches[i].materialState >> workbenches[i].productState;
		if (workbenches[i].productState) {
			PAIR temp(i, workbenches[i].type);
			resource.insert(temp);
		}
		if (workbenches[i].type == 1 || workbenches[i].type == 2 || workbenches[i].type == 3)continue;
		int k = product[workbenches[i].type] ^ workbenches[i].materialState;
		for (int j = 1; j <= 7; ++j) {
			if (k >> j & 1) {
				PAIR temp(i, j);
				require.insert(temp);
			}
		}
	}
	for (int i = 0; i < MAXROBOTS; ++i) {
		cin >> robots[i].workbenchId >> robots[i].carryType >> robots[i].timeValue >> robots[i].collisionValue
			>> robots[i].w >> robots[i].vx >> robots[i].vy >> robots[i].toward >> robots[i].x >> robots[i].y;
		//设定半径与质量
		robots[i].R = (robots[i].carryType == EMPTY) ? RR1 : RR2;
		robots[i].quantity = (robots[i].carryType == EMPTY) ? QUANTITY1 : QUANTITY2;
		robots[i].v = sqrtf(robots[i].vx * robots[i].vx + robots[i].vy * robots[i].vy); 
		if (robots[i].target_id == -1) {//初始化目标，多加了一个判断语句，可能会变慢
			robotChooseTarget(i);
			robotChooseNextTarget(i);
		}
		if (robots[i].workbenchId != ALONE && robots[i].workbenchId == robots[i].target_id) {
			PAIR temp(robots[i].workbenchId, workbenches[robots[i].workbenchId].type);
			block.erase(temp);
			robots[i].target_id = -1;
			if (robots[i].carryType == 0)robots[i].setInstruct(Instruction::BUY, i, 0);
			else robots[i].setInstruct(Instruction::SELL, i, 0);
		}
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
		//仅用于测试
		if (robots[i].target_id == -1) {
			if (robots[i].next_target_id == -1)robots[i].next_target_id = rand() % workbenchNum;
			robots[i].target_id = robots[i].next_target_id;
			robotChooseNextTarget(i);
		}
		float next = get_angular_velocity(robots[i], workbenches[robots[i].target_id]);
		robots[i].setInstruct(Instruction::ROTATE, i, next); 
		robots[i].setInstruct(Instruction::FORWARD, i,
			get_line_speed(robots[i], workbenches[robots[i].target_id], workbenches[robots[i].next_target_id]));
		
	}
}

void Map::robotChooseTarget(int id) {//可使用其他策略
	if (robots[id].carryType == 0) {
		set<PAIR>::iterator it;
		for (it = resource.begin(); it != resource.end(); ++it)  //使用迭代器进行遍历   
		{
			if (!block.count(*it)) {
				robots[id].target_id = (*it).first;
				block.insert(*it);
				break;
			}
		}
	}
	else {
		set<PAIR>::iterator it;
		for (it = require.begin(); it != require.end(); ++it)  //使用迭代器进行遍历   
		{
			if (!block.count(*it) && (*it).second == robots[id].carryType) {
				robots[id].target_id = (*it).first;
				block.insert(*it);
				break;
			}
		}
	}
}

void Map::robotChooseNextTarget(int id) {
	if (robots[id].carryType != 0) {//机器人有东西，目标是卖，下一个目标是买
		set<PAIR>::iterator it;
		for (it = resource.begin(); it != resource.end(); ++it)  //使用迭代器进行遍历   
		{
			if (!block.count(*it)) {
				robots[id].next_target_id = (*it).first;
				block.insert(*it);
				break;
			}
		}
	}
	else {//机器人手里没东西，目标是买，下一个目标是卖 
		set<PAIR>::iterator it;
		for (it = require.begin(); it != require.end(); ++it)  //使用迭代器进行遍历   
		{
			if (!block.count(*it) && (*it).second == robots[id].target_id) {
				robots[id].next_target_id = (*it).first;
				block.insert(*it);
				break;
			}
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

float get_line_speed(Robot& a, Workbench& b, Workbench& c) {
	bool flag = speed_up(a, b, c); 
	float range = radian(a, b);
	if (flag)return MAXFORWARD; 
	float S = sqrtf((b.x - a.x) * (b.x - a.x) + (b.y - a.y) * (b.y - a.y));
	float v = sqrtf(a.vx * a.vx + a.vy * a.vy);
	float a_ = MAXTRACTION / a.quantity;
	float low = (v * v) / (2 * a_);
	if (S - low <= 0.4 - 1e-8)return 0;
	else {
		return fabs(range) <= (PI / 16) ? MAXFORWARD : 2.0;
	}
}
 
bool speed_up(Robot& a, Workbench& b, Workbench& c) {
	float r = radian(a, b);
	if (fabs(r) > PI / 16)return false;
	Robot t = a;
	t.x = b.x;
	t.y = b.y;
	float range = radian(t, c); 
	return fabs(range) <= (PI * 3 / 8);
}