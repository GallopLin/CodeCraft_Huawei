#include <iostream>
#include <vector>
#include <cstring>
#include <algorithm>
#include <cmath>
#include <fstream>
#include <set>

#include "Object.h"

using namespace std;

//ofstream fout("C:\\Users\\ASUS\\Desktop\\HUAWEI\\2023\\WindowsRelease\\log.txt",ios::out);
//给定指令集合
const string Instruction::FORWARD = "forward";
const string Instruction::ROTATE = "rotate";
const string Instruction::BUY = "buy";
const string Instruction::SELL = "sell";
const string Instruction::DESTORY = "destory";
const int product[10] = { 0,0,0,0,6,10,12,112,128,254 };
const int buy[8] = { 0,3000,4400,5800,15400,17200,19200,76000 };
const int sell[8] = { 0,6000,7600,9200,22500,25000,27500,105000 };
const int ane[7] = { 0,1,3,5,8,10,15 };

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
		if (workbenches[i].productState == 1 || workbenches[i].restTime >= 0) {//完成或者正在生产
			C_carrier[workbenches[i].type].emplace_back(SimpleWorkbench(i, workbenches[i].productState == 1 ? 0 : workbenches[i].restTime));
		}
		//原料
		int k = workbenches[i].materialState ^ product[workbenches[i].type]; 
		int num = 0;
		for (int j = 1; j <= 7; j++)if (k >> j & 1) {
			num++;	//同等级 需要原料少的优先 
		}
		num -= need[workbenches[i].type]; // 不同级，缺的多的优先 
		for (int j = 1; j <= 7; j++) {
			if (k >> j & 1) {
				if (frameNumber == 1)need[j]++;
				if (!B[i][j])B_carrier[j].emplace_back(Material(i, j, num)); 
			}
		}
	}     
	for (int j = 1; j <= 7; j++)sort(B_carrier[j].begin(), B_carrier[j].end(), [&](Material& a, Material& b) {
		return a.num < b.num || (a.num == b.num && workbenches[a.id].type > workbenches[b.id].type);
		}); 
	for (int i = 0; i < MAXROBOTS; ++i) {  
		cin >> robots[i].workbenchId >> robots[i].carryType >> robots[i].timeValue >> robots[i].collisionValue
			>> robots[i].w >> robots[i].vx >> robots[i].vy >> robots[i].toward >> robots[i].x >> robots[i].y;
		//设定半径与质量
		robots[i].R = (robots[i].carryType == EMPTY) ? RR1 : RR2;
		robots[i].quantity = (robots[i].carryType == EMPTY) ? QUANTITY1 : QUANTITY2; 
		robots[i].id = i;
	}   
	/* 
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
					robot_sell(i);
					if (robots[i].ready) {
						robots[i].ready = false;
						robot_buy(i);
					}
				}
				else robot_buy(i); 
			} 
			set_target(i);
		}  
		//if (robots[i].carryType != 0)buy_next(i);
		//运动 
		robots[i].setInstruct(Instruction::ROTATE, i, 
			get_angular_velocity(robots[i], workbenches[robots[i].target_id]));
		robots[i].setInstruct(Instruction::FORWARD, i,
			get_line_speed(robots[i], workbenches[robots[i].target_id]));
		for (int j = i + 1; j < MAXROBOTS; j++) {
			if (collision_detection(robots[i], robots[j])) {
				robots[i].setInstruct(Instruction::ROTATE, i, MAXSPIN / 2);
				robots[j].setInstruct(Instruction::ROTATE, j, -MAXSPIN / 2);
				break;
			}
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
		if (remain == -1) {
			deal.push({ frameNumber, type }); //无需等待，会立刻生产 
		}
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
		//尝试购买
		{
			


		} 
		//买完之后看看是否会继续生产
		if (workbenches[wid].materialState == product[type] && workbenches[wid].restTime >= 0) {
			int remain = workbenches[wid].restTime;
			deal.push({ frameNumber + remain, type });
		}
		robots[id].setInstruct(Instruction::BUY, id, -1); 
		robots[id].carryType = type; //同上，不设置，会去买
		return;
	}  
	need[type]++; //不买，得告诉其他robot有此产品的需求
}

float Map::estimate_h(Robot& a, Workbench& b, Workbench& c) {
	Robot t = a;
	t.x = b.x; t.y = b.y;
	bool is_close = workbench_close_to_wall(b);
	t.vx = is_close ? 1.0 : 6.0;
	t.vy = 0;
	t.toward = a.toward + radian(a, b);
	float t1 = time_consume(a, b);
	float t2 = time_consume(t, c);
	if (t1 + t2 > MAXFRAME - frameNumber)return -1;
	float y = (1 - sqrtf(1 - (1 - t2 / MAXFRAME) * (1 - t2 / MAXFRAME))) * 0.2 + 0.8;
	float Gain = sell[b.type] * y - buy[b.type];
	return Gain / (t1 + t2);
}

void Map::set_target(int id) {
	float diss;
	if (robots[id].carryType == 0) { //手里没货 去买 
		SimpleWorkbench t; 
		diss = 0;
		int chosen;
		for (int j = 7; j >= 1; j--) {
			if (need[j] == 0)continue;
			for (auto& i : C_carrier[j]) {
				if (!C[i.id] && time_consume(robots[id], workbenches[i.id]) >= i.remain);
				else continue;
				//能买这个产品
				for (auto& k : B_carrier[workbenches[i.id].type]) {
					if (B[k.id][k.type])continue;
					//能卖
					float h = estimate_h(robots[id], workbenches[i.id], workbenches[k.id]);
					if (h > diss) {
						diss = h;
						t = i;
						chosen = j;
					}
				}
			}
		}
		if (diss > 0) { 
			C[t.id] = true;
			workbenches[t.id].robot_id = id;
			robots[id].target_id = t.id;
			need[chosen]--;
			return;
		}
	}
	else {  // 卖东西  
		Material t(-1, -1, 1e9);
		diss = 1e9;
		if (robots[id].carryType == 7) { 
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
		else {
			for (auto& i : B_carrier[robots[id].carryType]) {
				if (B[i.id][i.type])continue;
				if (i.num < t.num || (i.num == t.num && dis(robots[id], workbenches[i.id]) < diss)) { //优先级高一点
					t = i;
					diss = dis(robots[id], workbenches[i.id]);
				} 
			} 
			if (workbenches[t.id].type < 8) B[t.id][t.type] = true;
			else need[t.type]++;
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
			//那就比哪个近呗
			int rid = workbenches[wid].robot_id;
			float s1 = dis(robots[id], workbenches[wid]);
			float s2 = dis(robots[rid], workbenches[wid]);
			if (s2 <= s1)return; 
			//抢走
			workbenches[wid].robot_id = id;
			robots[id].ready = true;
			if (robots[rid].carryType != 0) {	//说明被抢走的robot是来卖东西顺便买东西
				robots[rid].ready = false;
			}
			else robots[rid].target_id = -1; //单纯来买东西
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
	float range = radian(a, b);
	float S = dis(a, b);
	float v = sqrtf(a.vx * a.vx + a.vy * a.vy);
	float a_ = MAXTRACTION / a.quantity; 
	if (fabs(range) <= PI / 42) {	//对齐
		float low = (v - TERMINALVELOCITY) * (v - TERMINALVELOCITY) / (2 * a_);
		//目的地是否靠墙，如果是减速，否则不减速
		if (S - low <= 0.4 - 1e-8)return is_w ? TERMINALVELOCITY : MAXFORWARD;
	}
	else {	//没对齐 
		//检查robot有没有绕target一直转
		if (S <= 2 * a.R)return 0;
		int sum = 0;
		for (int j = 1; j <= 6; j++) {
			sum += ane[j];
			if (fabs(range) <= PI / 42 * sum)return 6 + 1 - j;
		}
	}
	return MAXFORWARD;
}

bool workbench_close_to_wall(Workbench& b) {
	return b.x <= 2.5 || b.y <= 2.5 || b.x >= 47.5 || b.y >= 47.5;
}

bool robot_close_to_wall(Robot& b) {
	return b.x <= 2.5 || b.y <= 2.5 || b.x >= 47.5 || b.y >= 47.5;
}

int time_consume(Robot& a, Workbench& b) {
	bool is_w = workbench_close_to_wall(b);
	float range = radian(a, b);
	float S = dis(a, b);
	float v = sqrtf(a.vx * a.vx + a.vy * a.vy);
	float a_ = MAXTRACTION / a.quantity;
	float t = 0;
	float t1 = 0;
	int sum = 0;
	//没对齐
	for (int j = 1; j <= 6; j++) {
		sum += ane[j];
		if (fabs(range) <= PI / 42 * sum) {
			v = 7 - j;
			float a_ = MAXTORQUE / (a.quantity * a.R * a.R);
			float S = radian(a, b);
			//当前速度开始减速到0，会转多少
			float low = (a.w * a.w) / (2 * a_);
			if (low < range) {	//先加速后减速
				float S1 = (2 * MAXSPIN * MAXSPIN - a.w * a.w) / a_;
				if (S1 >= S) {
					float A = 2 * a_ * a_;
					float B = 4 * a_ * a.w;
					float C = a.w * a.w - 2 * a_ * S;
					t1 += (-B + sqrt(B * B - 4 * A * C)) / (2 * A);
					t1 += (a.w + a_ * t1) / a_;
				}
				else {
					t1 += (MAXSPIN - a.w + MAXSPIN) / a_;
					t1 += (S - (2 * MAXSPIN * MAXSPIN - a.w * a.w) / (2 * a_)) / MAXSPIN;
				}
			}
			else {
				float A = a_;
				float B = -2 * a.w;
				float C = 2 * S;
				t1 += (-B - sqrt(B * B - 4 * A * C)) / (2 * A);
			}
			break;
		}
	}
	//对齐 先加速后减速或者全程减速或者全程加速 
	if (is_w) {
		float S0 = (v * v - TERMINALVELOCITY * TERMINALVELOCITY) / (2 * a_); //全程减速的距离
		if (S0 < S) { // 全程减速距离不够，所以先加速
			float S1 = (2 * MAXFORWARD * MAXFORWARD - v * v - TERMINALVELOCITY * TERMINALVELOCITY) / (2 * a_); //加速到极点马上减速的距离
			if (S1 >= S) { //满足上述，没有匀速
				float A = 2 * a_ * a_;
				float B = 4 * a_ * v;
				float C = v * v - 2 * a_ * S - TERMINALVELOCITY;
				t += (-B + sqrtf(B * B - 4 * A * C)) / (2 * A); //加速阶段
				t += (v + t * a_ - TERMINALVELOCITY) / a_; //减速阶段
			}
			else {	//有匀速
				t += (MAXFORWARD - v + MAXFORWARD - TERMINALVELOCITY) / a_;
				t += (S - (2 * MAXFORWARD * MAXFORWARD - v * v - TERMINALVELOCITY * TERMINALVELOCITY) / (2 * a_)) / MAXFORWARD;
			}
		}
		else {	// 全程减速
			t += (2 * v - sqrtf(4 * v * v - 8 * a_ * S)) / (2 * a_);
		}
	}
	else {	//全程加速
		float S0 = (MAXFORWARD * MAXFORWARD - v * v) / (2 * a_);
		if (S0 < S) { // 有匀速
			t += (MAXFORWARD - v) / a_ + (S - S0) / MAXFORWARD;
		}
		else {
			t += (sqrtf(4 * v * v + 8 * a_ * S) - 2 * v) / (2 * a_);
		}
	}

	return (t1 + t) * 50 + 0.5;
}

template <typename T>
void shuffle(vector<T>& v) {
	int n = v.size();
	for (int i = n - 1; i >= 0; i--)swap(v[rand() % n], v[i]);
}

bool collision_detection(Robot& a, Robot& b) {
	//求robot间的角度，将a视作原点
	/*
	float vec;
	float x = (b.x - a.x);
	float y = (b.y - a.y);
	if (0 == x)vec = y > 0 ? PI / 2 : -PI / 2;
	else if (0 == y)vec = x > 0 ? 0 : PI;
	else vec = atan((y - 0) / (x - 0));
	if (x < 0 && y>0)vec += PI;
	else if (x < 0 && y < 0)vec -= PI;
	//比较这两个
	float rangea = fabs(a.toward - vec) > PI ? 2 * PI - fabs(a.toward - vec) : fabs(a.toward - vec);
	float rangeb = fabs(b.toward - vec) > PI ? 2 * PI - fabs(b.toward - vec) : fabs(b.toward - vec); 
	if (rangea <= PI / 8 && rangeb >= PI * 7 / 8) {
		return true;
	}
	*/
	float R = a.R + b.R + 0.5;
	if (dis(a, b) <= R) {
		float x = a.toward - b.toward;
		return PI * 6 / 8 <= fabs(x) && fabs(x) <= PI * 10 / 8 || PI + PI * 6 / 8 <= fabs(x) && fabs(x) <= PI + PI * 10 / 8;
	}
	return false;
} 