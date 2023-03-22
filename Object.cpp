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
				robots[rNum].ready = false;
				++rNum;
			}
			else if (map[i][j] >= '1' && map[i][j] <= '9') {
				if (map[i][j] - '0' >= 8)need[7] = 1e9; 
				workbenches[wNum].setPos(i, j); 
				workbenches[wNum].type = map[i][j] - '0'; 
				workbenches[wNum].robot_id = -1;
				C[wNum] = false;
				for (int idx = 0; idx < 8; idx++)B[wNum][idx] =  false;
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
	B_carrier.clear();
	cin >> frameNumber >> money;
	while (!deal.empty() && deal.top().first <= frameNumber) {
		for (int j = 7; j >= 1; j--)if (product[deal.top().second] >> j & 1) {
			need[j]++; 
		}
		deal.pop();
	}
	cin >> workbenchNum;
	for (int i = 0; i < workbenchNum; ++i) {
		cin >> workbenches[i].type >> workbenches[i].x >> workbenches[i].y >> workbenches[i].restTime
			>> workbenches[i].materialState >> workbenches[i].productState; 
		//产物
		if (!C[i] && workbenches[i].productState == 1 || workbenches[i].restTime >= 0) {//完成或者正在生产，并且没被锁
			C_carrier[workbenches[i].type].emplace_back(SimpleWorkbench(i, workbenches[i].productState == 1 ? 0 : workbenches[i].restTime));
		}
		//原料
		int k = workbenches[i].materialState ^ product[workbenches[i].type]; 
		int num = 0;
		for (int j = 1; j <= 7; j++)if (k >> j & 1)num++;
		for (int j = 1; j <= 7; j++) {
			if (k >> j & 1) {
				if (frameNumber == 1)need[j]++;
				if (!B[i][j])B_carrier[j].emplace_back(Material(i, j, num)); 
			}
		}
	}     
	for (int j = 1; j <= 7; j++)sort(B_carrier[j].begin(), B_carrier[j].end(), [&](Material& a, Material& b) {return a.num < b.num; }); 
	for (int i = 0; i < MAXROBOTS; ++i) {  
		cin >> robots[i].workbenchId >> robots[i].carryType >> robots[i].timeValue >> robots[i].collisionValue
			>> robots[i].w >> robots[i].vx >> robots[i].vy >> robots[i].toward >> robots[i].x >> robots[i].y;
		//设定半径与质量
		robots[i].R = (robots[i].carryType == EMPTY) ? RR1 : RR2;
		robots[i].quantity = (robots[i].carryType == EMPTY) ? QUANTITY1 : QUANTITY2; 
	} 
	  {
			fout << "frame" << frameNumber << endl;
			for (int i = 7; i >= 1; i--) {
				fout << i << "type :";
				for(auto &j: C_carrier[i])
				fout << j.id << " ";
				fout << endl;
			}  
			for (int i = 7; i >= 1; i--)fout << need[i] << " ";
			fout << endl;
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
		//初始状态或者到达
		if (robots[i].target_id == robots[i].workbenchId) {
			if (robots[i].workbenchId != ALONE) {
				if (robots[i].carryType != 0) {	 
					robot_sell(i);
					if (robots[i].ready) { // 卖了能不能直接买？
						robots[i].ready = false; 
						robot_buy(i);
					}
				}
				else robot_buy(i); 
			} 
			set_target(i);
		} 
		if (robots[i].carryType != 0)buy_next(i);
		//运动 
		robots[i].setInstruct(Instruction::ROTATE, i, 
			get_angular_velocity(robots[i], workbenches[robots[i].target_id]));
		robots[i].setInstruct(Instruction::FORWARD, i,
			get_line_speed(robots[i], workbenches[robots[i].target_id]));
		for (int j = i + 1; j < MAXROBOTS; j++)
			if (collision_detection(robots[i],robots[j])) {
				robots[i].setInstruct(Instruction::ROTATE, i, MAXSPIN / 2);
				robots[j].setInstruct(Instruction::ROTATE, j, -MAXSPIN / 2);
				break;
			}
	}
}  

void Map::robot_sell(int id) {
	// 卖给工作台 
	int wid = robots[id].workbenchId;
	int type = workbenches[wid].type;
	workbenches[wid].materialState |= (1 << robots[id].carryType);
	//所有原料凑齐，但要分情况讨论
	if (workbenches[wid].materialState == product[type] && type <= 7) {
		int remain = workbenches[wid].restTime;
		if (remain == -1)deal.push({ frameNumber, type }); //无需等待，会立刻生产
		else if (remain >= 0 && workbenches[wid].productState == 0) //生产结束后会立马继续生产
			deal.push({ frameNumber + remain, type });
	}
	B[wid][robots[id].carryType] = false;
	robots[id].setInstruct(Instruction::SELL, id, -1);
	robots[id].carryType = 0; // 卖掉了，不设置值，下一个target仍然想去卖 
}

void Map::robot_buy(int id) {
	// 向工作台买东西 
	int wid = robots[id].workbenchId;
	int type = workbenches[wid].type; 
	C[wid] = false;
	workbenches[wid].robot_id = -1;
	if (workbenches[wid].productState == 1) {
		//买完之后看看是否会继续生产
		if (workbenches[wid].materialState == product[type] && workbenches[wid].restTime >= 0) {
			int remain = workbenches[wid].restTime;
			deal.push({ frameNumber + remain, type });
		}
		robots[id].setInstruct(Instruction::BUY, id, -1);
		robots[id].carryType = type; //同上，不设置，会去买ty
		return;
	}  
	need[type]++; //不买，得告诉其他robot有此产品的需求
}

void Map::set_target(int id) {
	float diss;
	if (robots[id].carryType == 0) { //手里没货 去买
		SimpleWorkbench t;
		int chosen = -1;
		for (int le = 7; le >= 1; le--) { 
			if (le == 7 || le == 6 || le == 3)diss = 1e9; 
			if (need[le] != 0)
				for (auto& i : C_carrier[le]) {
					if (!C[i.id] && time_consume(robots[id], workbenches[i.id]) >= i.remain);
					else continue;
					float disss = dis(robots[id], workbenches[i.id]);
					if (disss < diss) {
						diss = disss;
						t = i;
						chosen = le;
					}
				}
			if ((le - 1) % 3 == 0 && diss < 1e9) { //选择这个工作台
				C[t.id] = true;
				workbenches[t.id].robot_id = id;
				robots[id].target_id = t.id; 
				need[chosen]--;
				return;
			}
		}
	}	
	else {  // 卖东西
		if (robots[id].carryType == 7) {
			diss = 1e9;
			Material t;
			for (auto& i : B_carrier[robots[id].carryType]) { 
				float disss = dis(robots[id], workbenches[i.id]);
				if (disss < diss) {
					diss = disss; 
					t = i;
				}
			}
			//// 8 9 号工作台不上锁
			//B[t.id][t.type] = true;
			robots[id].target_id = t.id; 
			return;
		}
		else if (robots[id].carryType > 3) {
			for (auto& i : B_carrier[robots[id].carryType]) {
				if (B[i.id][i.type] || i.type != robots[id].carryType)continue;
				if (workbenches[i.id].type != 9) B[i.id][i.type] = true;
				robots[id].target_id = i.id;  
				return;
			}
		}
		else {
			diss = 1e9;
			Material t;
			for (int j = 3; j >= 1; j--) {
				for (auto& i : B_carrier[j]) {
					if (B[i.id][i.type] || i.type != robots[id].carryType)continue;
					float disss = dis(robots[id], workbenches[i.id]);
					if (disss < diss) {
						diss = disss;
						t = i;
					}
				}
			}
			if (workbenches[t.id].type != 9)B[t.id][t.type] = true;
			robots[id].target_id = t.id;  
			return;
		}
	}
	robots[id].target_id = -1;
}  

void Map::buy_next(int id) {
	if (robots[id].ready)return;
	int wid = robots[id].target_id;
	if (wid == -1)return;
	bool has_product = workbenches[wid].productState == 1 || (workbenches[wid].restTime >= 0
		&& time_consume(robots[id], workbenches[wid]) >= workbenches[wid].restTime);
	if (has_product) {
		if (C[wid]) { // 说明有robot预定该产品
			//预定此产品的robot正在送产品到该工作台，那么需要判断
			if (robots[workbenches[wid].robot_id].ready) {
				//说明此时两个或者多个robot往这边卖东西，并争抢买该产品，那么以距离定胜负
				if (dis(robots[id], workbenches[wid]) < dis(robots[workbenches[wid].robot_id], workbenches[wid])) {
					//更近, 那么抢走,但是不要该对方的target_id，因为对方正在过来卖东西
					robots[workbenches[wid].robot_id].ready = false;
					robots[id].ready = true;
					workbenches[wid].robot_id = id;
				}
				else return; //更远，那么抢不走
			}
			robots[workbenches[wid].robot_id].target_id = -1; //抢走它
			robots[id].ready = true;
			workbenches[wid].robot_id = id;
		}
		else if (need[workbenches[wid].type] > 0) { //没robot预定，那么要判断此产品有没有需求 
			robots[id].ready = true;
			workbenches[wid].robot_id = id;
			need[workbenches[wid].type]--;
			C[wid] = true;
		}
	}
}

template <typename T, typename T1>
float dis(T& a, T1& b) {
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
		return fabs(range) <= (PI / 16) ? MAXFORWARD : ((is_r && (abs(range) >= PI / 6)) ? STARTSPEED
			: ((abs(range) >= PI / 2) ? STARTSPEED : MAXFORWARD));
	}
} 

bool workbench_close_to_wall(Workbench& b) {
	return b.x <= 2.5 || b.y <= 2.5 || b.x >= 47.5 || b.y >= 47.5;
}

bool robot_close_to_wall(Robot& b) {
	return b.x <= 2.5 || b.y <= 2.5 || b.x >= 47.5 || b.y >= 47.5;
}

int time_consume(Robot& a, Workbench& b) {  
	float t = 0;
	bool is_w = workbench_close_to_wall(b);  
	float S = dis(a, b) - a.R;
	float v = sqrtf(a.vx * a.vx + a.vy * a.vy);
	float a_ = MAXTRACTION / a.quantity;
	if (is_w) { // 先加速后减速，或者全程减速
		float S0 = v * v / (2 * a_);
		if (S0 < S) {// 先加速后减速，则最后速度应该是TERMINALVELOCITY
			float A = 2 * a_ * a_;
			float B = 4 * a_ * v;
			float C = v * v - 2 * a_ * S - TERMINALVELOCITY;
			t = (-B + sqrtf(B * B - 4 * A * C)) / (2 * A); //加速阶段
			t += (v + t * a_ - TERMINALVELOCITY) / a_; //减速阶段
		}
		else {
			t = (2 * v - sqrtf(4 * v * v - 8 * a_ * S)) / (2 * a_);
		}
	}
	else {	//全程加速
		float S0 = (MAXFORWARD * MAXFORWARD - v * v) / (2 * a_);
		if (S0 < S) {
			t = (MAXFORWARD - v) / a_ + (S - S0) / MAXFORWARD;
		}
		else {
			t = (sqrtf(4 * v * v + 8 * a_ * S) - 2 * v) / (2 * a_);
		}
	}
	return t * 50;
}

template <typename T>
void shuffle(vector<T>& v) {
	int n = v.size();
	for (int i = n - 1; i >= 0; i--)swap(v[rand() % n], v[i]);
}

bool collision_detection(Robot& a, Robot& b) {
	float R = a.R + b.R + 0.5; 
	if (dis(a, b) <= R) {
		float x = a.toward - b.toward;
		return PI * 6 / 8 <= fabs(x) && fabs(x) <= PI * 10 / 8 || PI + PI * 6 / 8 <= fabs(x) && fabs(x) <= PI + PI * 10 / 8;
	}
	return false;
}