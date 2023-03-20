#include <iostream>
#include <vector>
#include <cstring>
#include <algorithm>
#include <cmath>
#include <fstream>
#include <set>

#include "Object.h"

using namespace std;

ofstream fout("C:\\Users\\ASUS\\Desktop\\HUAWEI\\2023\\WindowsRelease\\log.txt",ios::out);
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
				//设定初始目的地为空
				robots[rNum].target_id = -1;
				++rNum;
			}
			else if (map[i][j] >= '1' && map[i][j] <= '9') {
				workbenches[wNum].setPos(i, j); 
				workbenches[wNum].type = map[i][j] - '0'; 
				C[wNum] = false;
				for (int idx = 0; idx < 8; idx++)A[wNum][idx] = B[wNum][idx] =  false;
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
	C_carrier.clear();
	A_carrier.clear();
	B_carrier.clear();
	cin >> frameNumber >> money;
	cin >> workbenchNum;
	for (int i = 0; i < workbenchNum; ++i) {
		cin >> workbenches[i].type >> workbenches[i].x >> workbenches[i].y >> workbenches[i].restTime
			>> workbenches[i].materialState >> workbenches[i].productState; 
		//产物
		if (workbenches[i].restTime != -1 && !C[i]) {//完成或者正在生产，并且没被锁
			C_carrier[workbenches[i].type].emplace_back(SimpleWorkbench(i, workbenches[i].productState == 1 ? 0 : workbenches[i].restTime));
		}
		//原料
		int k = workbenches[i].materialState ^ product[workbenches[i].type]; 
		int num = 0;
		for (int j = 1; j <= 7; j++)if (k >> j & 1 && !B[i][j])num++;
		for (int j = 1; j <= 7; j++) {
			if (k >> j & 1) {
				if (!B[i][j])B_carrier[j].emplace_back(Material(i, j, num));
				if (!A[i][j])A_carrier[(j - 1) / 3 + 1].emplace_back(Material(i, j, num));
			}
		}
	}     
	for (int j = 1; j <= 7; j++)sort(B_carrier[j].begin(), B_carrier[j].end(), [&](Material& a, Material& b) {return a.num < b.num; });
	for (int j = 1; j <= 3; j++)sort(A_carrier[j].begin(), A_carrier[j].end(), [&](Material& a, Material& b) {return a.num < b.num; });
	for (int i = 0; i < MAXROBOTS; ++i) {
		cin >> robots[i].workbenchId >> robots[i].carryType >> robots[i].timeValue >> robots[i].collisionValue
			>> robots[i].w >> robots[i].vx >> robots[i].vy >> robots[i].toward >> robots[i].x >> robots[i].y;
		//设定半径与质量
		robots[i].R = (robots[i].carryType == EMPTY) ? RR1 : RR2;
		robots[i].quantity = (robots[i].carryType == EMPTY) ? QUANTITY1 : QUANTITY2;
	} 
	/*
	if (frameNumber == 6651) {
		fout << "A" << endl;
		for (int q = 2; q >= 0; q--) {
			for (auto& j : A_carrier[q]) {
				fout << j.id << " " << j.type << endl;
			}
		}
		fout << "B" << endl;
		for (int q = 7; q >= 1; q--) {
			fout << q << "type:";
			for (auto& j : B_carrier[q]) {
				fout << j.id << " ";
			}
			fout << endl;
		}
		fout << "C" << endl;
		for (int q = 7; q >= 1; q--) {
			for (auto& j : C_carrier[q]) {
				fout << j.id << " " << j.remain << endl;
			}
		}
	} 
	*/
	
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
		//初始状态或者到达
		if (robots[i].target_id == robots[i].workbenchId) {
			if (robots[i].workbenchId != ALONE) {
				if (robots[i].carryType != 0) {
					A[robots[i].workbenchId][robots[i].carryType] = false;
					B[robots[i].workbenchId][robots[i].carryType] = false;
					robots[i].setInstruct(Instruction::SELL, i, -1);
					robots[i].carryType = 0; // 卖掉了，不设置值，下一个target仍然想去卖 
				}
				else {
					C[robots[i].workbenchId] = false;
					robots[i].setInstruct(Instruction::BUY, i, -1);
					robots[i].carryType = workbenches[robots[i].workbenchId].type; //同上，不设置，会去买
				}
			} 
			set_target(i);
		} 
		//运动 
		robots[i].setInstruct(Instruction::ROTATE, i, 
			get_angular_velocity(robots[i], workbenches[robots[i].target_id]));
		robots[i].setInstruct(Instruction::FORWARD, i,
			get_line_speed(robots[i], workbenches[robots[i].target_id]));
	}
}  

void Map::set_target(int id) {
	if (robots[id].carryType == 0) { //手里没货
		for (int i = 3; i >= 1; i--) {
			for (auto& j : A_carrier[i]) {
				if (A[j.id][j.type])continue;
				float diss = 1e9;
				SimpleWorkbench t;
				for (auto& k : C_carrier[j.type]) {
					if (C[k.id] || time_consume(robots[id], workbenches[k.id]) < k.remain)continue;
					if (dis(robots[id], workbenches[k.id]) < diss) {
						diss = dis(robots[id], workbenches[k.id]);
						t = k;
					} 
				}
				if (diss < 1e9) {
					C[t.id] = true;
					A[j.id][j.type] = true;
					robots[id].target_id = t.id;
					return;
				}
			}
		}
	}	
	else { 
		for (auto& i : B_carrier[robots[id].carryType]) {
			if (B[i.id][robots[id].carryType] || !A[i.id][robots[id].carryType])continue;
			B[i.id][robots[id].carryType] = true; 
			robots[id].target_id = i.id;
			return;
		}
	}
	robots[id].target_id = -1;
} 

float dis(Robot& a, Workbench& b) {
	return sqrtf((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y));
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
	bool is_w = workbench_close_to_wall(b);
	bool is_r = robot_close_to_wall(a);
	float range = radian(a, b); 
	float S = dis(a, b);
	float v = sqrtf(a.vx * a.vx + a.vy * a.vy);
	float a_ = MAXTRACTION / a.quantity;
	float low = (v * v) / (2 * a_);
	//目的地是否靠墙，如果是减速，否则不减速
	if (S - low <= 0.4 - 1e-8)return is_w ? TERMINALVELOCITY : MAXFORWARD;
	else {
		/*
			是否直线，如果是则加速，否则机器人是否靠墙
		*/
		return fabs(range) <= (PI / 16) ? MAXFORWARD : (is_r ? STARTSPEED : MAXFORWARD);
	}
} 

bool workbench_close_to_wall(Workbench& b) {
	return b.x <= 2.5 || b.y <= 2.5 || b.x >= 47.5 || b.y >= 47.5;
}

bool robot_close_to_wall(Robot& b) {
	return b.x <= 2.5 || b.y <= 2.5 || b.x >= 47.5 || b.y >= 47.5;
}

int time_consume(Robot& a, Workbench& b) { 
	float S = dis(a, b);
	return S / MAXFORWARD * 50 ;
}

template <typename T>
void shuffle(vector<T>& v) {
	int n = v.size();
	for (int i = n - 1; i >= 0; i--)swap(v[rand() % n], v[i]);
}