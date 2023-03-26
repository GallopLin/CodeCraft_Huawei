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
	only_night = 0;//判断是不是只有9
	sn = -1;
	p = PI / 32;
	K = 200;
	for (int i = 0; i < LENGTH; ++i) {
		for (int j = 0; j < LENGTH; ++j) {
			cin >> map[i][j];
			if (sn == -1) {
				if (map[i][j] == '1')sn = 1;
				else if (map[i][j] == '6')sn = 2;
				else if (map[i][j] == '3')sn = 3;
				else if (map[i][j] == '7')sn = 4;
			}
			if (map[i][j] == 'A') {
				robots[rNum].setPos(i, j);
				//设定初始目的地为空
				robots[rNum].target_id = -1;
				robots[rNum].ready = false;
				robots[rNum].charge = 1;//设置机器人的电荷量 
				robots[rNum].damntimes = 0;
				++rNum;
			}
			else if (map[i][j] >= '1' && map[i][j] <= '9') {
				if (map[i][j] - '0' >= 8) need[7] = 1e9; 
				if (map[i][j] - '0' == 9 && only_night == 0)only_night = 1;
				if (map[i][j] - '0' == 7)only_night = 2; 
				numofwork[map[i][j] - '0']++;
				workbenches[wNum].setPos(i, j); 
				workbenches[wNum].type = map[i][j] - '0'; 
				workbenches[wNum].robot_id = -1;
				C[wNum] = false;
				for (int idx = 0; idx < 8; idx++)B[wNum][idx] =  false;
				++wNum;
			}
		}
	}
	ajustK();
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
	while (!deal.empty() && deal.top().first <= frameNumber) {	//还原原料格
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
			int mati = 0;
			for (int j = 1; j <= 7; j++)if (workbenches[i].materialState >> j & 1)mati++;
			C_carrier[workbenches[i].type].emplace_back(SimpleWorkbench(i, workbenches[i].productState == 1 ? 0 : workbenches[i].restTime, mati));
		}
		//原料
		int k = workbenches[i].materialState ^ product[workbenches[i].type];
		int num = 0;
		if (only_night == 2) {  //存在7 才算优先级    
			for (int j = 1; j <= 7; j++)if (k >> j & 1 && !B[i][j]) {
				num++;	//同等级 需要原料少的优先   
			}
			//如果该工作台没有在生产，优先级上升
			if (workbenches[i].restTime == -1)num -= 3;
		} 
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
		//rob(i);
		if (robots[i].carryType != 0 && sn != 3)buy_next(i);
		//else check_buy(i);
		//运动 
		VirtualFieldAlgorithm(i);
		robots[i].setInstruct(Instruction::ROTATE, i, 
			get_angular_velocity(robots[i], workbenches[robots[i].target_id]));
		robots[i].setInstruct(Instruction::FORWARD, i,
			get_line_speed(robots[i], workbenches[robots[i].target_id])); 
	}
}  

void Map::check_buy(int id) {
	int tid = robots[id].target_id;
	if (tid == -1)return;
	float remain = workbenches[tid].productState == 1 ? 0 : workbenches[tid].restTime;
	if (time_consume(robots[id], workbenches[tid]) < remain)robots[id].damntimes++;
	if (robots[id].damntimes >= 15) {
		robots[id].damntimes = 0; 
		C[tid] = false;
		workbenches[tid].robot_id = -1; 
		if (workbenches[tid].type <= 3)numofbuy[workbenches[tid].type]--;
		need[workbenches[tid].type]++;
		set_target(id);
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
	robots[id].damntimes = 0;
	robots[id].carryType = 0; // 卖掉了，不设置值，下一个target仍然想去卖 
}

void Map::robot_buy(int id) {
	// 向工作台买东西 
	int wid = robots[id].workbenchId;
	int type = workbenches[wid].type;
	C[wid] = false;
	workbenches[wid].robot_id = -1;
	//判断是否解锁，有没有robot来买
	for (int i = 0; i < MAXROBOTS; i++)if (i != id && robots[i].target_id == wid && robots[i].carryType == 0) {
		C[wid] = true;
		workbenches[wid].robot_id = i;
	}
	if (workbenches[wid].productState == 1) {  
		//尝试购买
		Material t(-1, -1, 1e9);
		float diss = 1e9;
		for (auto& i : B_carrier[type]) {
			if (B[i.id][i.type])continue;
			if (i.num < t.num || (i.num == t.num && dis(robots[id], workbenches[i.id]) < diss)) { //优先级高一点
				t = i;
				diss = dis(robots[id], workbenches[i.id]);
			}
		}
		if (time_consume(robots[id], workbenches[t.id]) > MAXFRAME - frameNumber)return;
		//买完之后看看是否会继续生产
		if (workbenches[wid].materialState == product[type] && workbenches[wid].restTime >= 0) {
			int remain = workbenches[wid].restTime;
			deal.push({ frameNumber + remain, type });
		} 
		robots[id].setInstruct(Instruction::BUY, id, -1); 
		robots[id].carryType = type; //同上，不设置，会去买
		return;
	}   
	//没买成功
	need[type]++;
	if (type <= 3)numofbuy[type]--;
}

float Map::estimate_h(Robot& a, Workbench& b, Workbench& c) {
	Robot t = a;
	t.x = b.x; t.y = b.y;
	bool is_close = workbench_close_to_wall(b);
	t.vx = is_close ? 1.0 : 6.0;
	t.vy = 0;
	float vec;
	float x = (b.x - a.x);
	float y = (b.y - a.y);
	if (0 == x)vec = y > 0 ? PI / 2 : -PI / 2;
	else if (0 == y)vec = x > 0 ? 0 : PI;
	else vec = atan((y - 0) / (x - 0));
	if (x < 0 && y>0)vec += PI;
	else if (x < 0 && y < 0)vec -= PI;
	t.toward = vec;
	float t1 = time_consume(a, b);
	float t2 = time_consume(t, c);
	if (t1 + t2 > MAXFRAME - frameNumber)return -1;
	float w = (1 - sqrtf(1 - (1 - t2 / MAXFRAME) * (1 - t2 / MAXFRAME))) * 0.2 + 0.8;
	float Gain = sell[b.type] * w - buy[b.type];
	return Gain / (t1 + t2);
}

void Map::rob(int id) {
	int ctype = robots[id].carryType;
	int tid = robots[id].target_id;
	for (int i = 0; i < MAXROBOTS; i++) {
		if (i == id || robots[i].target_id == -1)continue;
		int itype = robots[id].carryType;
		int iid = robots[id].target_id;
		if (ctype != 0 && itype == ctype) { //两个都去卖
			if (dis(robots[id], workbenches[tid]) + dis(robots[i], workbenches[iid]) >
				dis(robots[id], workbenches[iid]) + dis(robots[i], workbenches[tid])) {
				robots[id].target_id = iid;
				robots[i].target_id = tid;
				return;
			}
		}
		else if (ctype == 0 && itype == 0 && iid != -1 && tid == -1) {
			int t = workbenches[iid].productState == 1 ? 0 : workbenches[iid].restTime;
			if (time_consume(robots[id], workbenches[iid]) > t && dis(robots[id], workbenches[iid]) < dis(robots[i], workbenches[iid])) {
				robots[id].target_id = iid;
				robots[i].target_id = -1;
				return;
			}
		}
		else if (ctype == 0 && itype == 0 && iid != -1 && tid != -1) {
			int t1 = workbenches[tid].productState == 1 ? 0 : workbenches[tid].restTime;
			int t2 = workbenches[iid].productState == 1 ? 0 : workbenches[iid].restTime;
			if (time_consume(robots[id], workbenches[iid]) > t1 && time_consume(robots[i], workbenches[tid]) > t2) {
				if (dis(robots[id], workbenches[tid]) + dis(robots[i], workbenches[iid]) >
					dis(robots[id], workbenches[iid]) + dis(robots[i], workbenches[tid])) {
					robots[id].target_id = iid;
					robots[i].target_id = tid;
					return;
				}
			}
		}
	}
}

vector<vector<int>> Map::choose_buy(int id) {
	vector<vector<int>>res; 
	res.push_back({ 7 }); // 7的优先级总是最高的 
	// 4 5 6 的优先级按照缺的多的来
	vector<pair<int, int>>temp;
	for (int i = 4; i <= 6; i++) {
		temp.push_back({ need[i],i });
	}
	sort(temp.begin(), temp.end(), [&](pair<int, int>& a, pair<int, int>& b) {
		return a.first > b.first;
		}); 
	vector<int>v;
	v.push_back(temp[0].first);//缺多少
	res.push_back({ temp[0].second });
	for (int i = 1; i < 3; i++) {
		if (temp[i].first == v.back()) {	//相同
			res.back().push_back(temp[i].second); //加入同等级
		}
		else {	//比较少
			v.push_back(temp[i].first);
			res.push_back({ temp[i].second }); //新建（降低）一个等级
		}
	}
	vector<pair<int, int>>s;
	for (int i = 1; i <= 3; i++) {
		s.push_back({ numofbuy[i] ,i });
	}
	sort(s.begin(), s.end(), [&](pair<int, int>& a, pair<int, int>& b) { //被买的次数要少
		return a.first < b.first;
		});
	res.push_back({ s[0].second });
	v.push_back(s[0].first);
	for (int i = 1; i < 3; i++) {
		if (s[i].first == v.back()) {	//相同
			res.back().push_back(s[i].second); //加入同等级
		}
		else {	//比较少
			v.push_back(s[i].first);
			res.push_back({ s[i].second }); //新建（降低）一个等级
		}
	}
	return res;
}

set<int> Map::choose_sell(int id) { 
	int type = robots[id].carryType;
	if (type == 7) {  
		return { -1 };
	}
	else if (type >= 4) {
		return { 7 };
	}
	else {
		if (type == 3) {
			if (numofsell[5][3] < numofsell[6][3])return { 5 };
			else if (numofsell[5][3] > numofsell[6][3])return { 6 };
			else return { 5,6 };
		}
		else if (type == 2) {
			if (numofsell[4][2] < numofsell[6][2])return { 4 };
			else if (numofsell[4][2] > numofsell[6][2])return { 6 };
			else return { 4,6 };
		}
		else {
			if (numofsell[5][1] < numofsell[4][1])return { 5 };
			else if (numofsell[5][1] > numofsell[4][1])return { 4 };
			else return { 4,5 };
		}
	}
	return { -1 };
}

void Map::set_target(int id) { 
	float diss; 
	if (only_night == 1) {
		if (robots[id].carryType == 0) { //手里没货 去买 
			SimpleWorkbench t;
			Material s(-1, -1, 1e9);
			diss = 0;
			int chosen;
			for (int j = 7; j >= 1; j--) {
				if (need[j] == 0)continue;
				for (auto& i : C_carrier[j]) {
					if (time_consume(robots[id], workbenches[i.id]) >= i.remain) { //能买，不知道有没有被预定
						int num_buy = 0;
						for (int k = 0; k < MAXROBOTS; k++)if (k != id && robots[k].target_id == i.id && (robots[k].carryType == 0 || robots[k].ready))num_buy++;
						if (workbenches[i.id].productState == 1) { //只能有一个来买
							if (workbenches[i.id].restTime == -1 && num_buy >= 1)continue;
							if (workbenches[i.id].restTime >= 0 && num_buy >= 2)continue;
						}
						else if (num_buy >= 1)continue;
					}
					else continue;
					//能买这个产品 
					for (auto& k : B_carrier[workbenches[i.id].type]) {
						if (B[k.id][k.type])continue;
						if (k.num < s.num) {	//先按优先级买
							s = k;
							t = i;
							chosen = j;
							diss = estimate_h(robots[id], workbenches[i.id], workbenches[k.id]);
						}
						else if (k.num == s.num) {	//否则看单位帧所得
							float h = estimate_h(robots[id], workbenches[i.id], workbenches[k.id]);
							if (h > diss) {
								s = k;
								t = i;
								chosen = j;
								diss = h;
							}
						}
					}
				}
				if (j == 7 && diss > 0)break;
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
				else need[robots[id].carryType]++;
				robots[id].target_id = t.id;
				return;
			}
		}
		robots[id].target_id = -1;
	}
	else {
		int type = robots[id].carryType;
		if (type == 0) {
			SimpleWorkbench t;
			vector<vector<int>>v = choose_buy(id);
			for (auto& arr : v) {
				diss = 1e9;
				for (auto& j : arr) {
					if (need[j] == 0)continue; 
					for (auto& i : C_carrier[j]) {
						if (time_consume(robots[id], workbenches[i.id]) >= i.remain) { //能买，不知道有没有被预定
							int num_buy = 0;
							for (int k = 0; k < MAXROBOTS; k++)if (k != id && robots[k].target_id == i.id && (robots[k].carryType == 0 || robots[k].ready))num_buy++;
							if (workbenches[i.id].productState == 1) { //只能有一个来买
								if (workbenches[i.id].restTime == -1 && num_buy >= 1)continue;
								if (workbenches[i.id].restTime >= 0 && num_buy >= 2)continue;
							}
							else if (num_buy >= 1)continue;
						}
						else continue;
						if (diss > dis(robots[id], workbenches[i.id])) {
							t = i;
							diss = dis(robots[id], workbenches[i.id]);
						}
					}
				}  
				//每个优先级有东西可以买就可以返回了
				if (diss < 1e9) {
					C[t.id] = true;
					workbenches[t.id].robot_id = id;
					robots[id].target_id = t.id;
					need[workbenches[t.id].type]--;
					if (workbenches[t.id].type <= 3)numofbuy[workbenches[t.id].type]++;
					return;
				}
			}
		}
		else {
			Material t(-1, -1, 1e9);
			diss = 1e9;
			if (type == 7) {	//处理7
				for (auto& i : B_carrier[type]) {
					if (diss > dis(robots[id], workbenches[t.id])) {
						t = i;
						diss = dis(robots[id], workbenches[t.id]);
					}
				}
				robots[id].target_id = t.id;
				return;
			}
			else {
				auto toselltype = choose_sell(id);
				for (auto& i : B_carrier[type]) {
					if (toselltype.count(workbenches[i.id].type) && !B[i.id][i.type]) {
						if (i.num < t.num) {
							t = i;
							diss = dis(robots[id], workbenches[t.id]);
						}
						else if (i.num == t.num) {
							if (dis(robots[id], workbenches[t.id]) > dis(robots[id], workbenches[i.id])) {
								t = i;
								diss = dis(robots[id], workbenches[t.id]);
							}
						}
					}
				} 
			} 
			if (diss < 1e9) {
				B[t.id][t.type] = true; 
				robots[id].target_id = t.id;
				numofsell[workbenches[t.id].type][t.type]++;
				return;
			}
		}
		robots[id].target_id = -1;
	}
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
			if (workbenches[wid].type <= 3)numofbuy[workbenches[wid].type]++;
			need[workbenches[wid].type]--;
			C[wid] = true;
		}
	}
}

void Map::VirtualFieldAlgorithm(int id) {
	float fx = 0, fy = 0; // 计算机器人受到的力
	for (int j = 0; j < MAXROBOTS; j++) {
		if (j == id) continue;
		float dx = robots[j].x - robots[id].x, dy = robots[j].y - robots[id].y;
		float d = sqrt(dx * dx + dy * dy);
		float angle = atan2(dy, dx);
		float force = K * robots[id].charge * robots[j].charge / (d * d);
		fx += force * cos(angle);
		fy += force * sin(angle);
	}
	float fx2 = 0, fy2 = 0;
	fx2 += 250 * cos(robots[id].toward) + fx;
	fy2 += 250 * sin(robots[id].toward) + fy;
	float angle3 = atan2(fy2, fx2);
	robots[id].toward = angle3 + p;
}

void Map::ajustK() {
	switch (sn)
	{
	case 1:
		K = 180;
		p = PI / 16;
		break;
	case 2:
		K = 180;
		p = PI / 16;
		break;
	case 3:
		K = 230;
		p = 0;
		break;
	case 4:
		K = 200;
		p = PI / 48;
		break;
	default:
		break;
	}
	return;
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