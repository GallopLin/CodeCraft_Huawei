#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <cstring>
#include <cstdio>
#include <set>
#include <queue>
#include <unordered_map>

#define MAXFRAME 9000
#define PI acos(-1)
#define INIT_CAPITAL 200000  //初始资金
#define JUDGEDIS 0.4  //机器人-工作台判定距离
#define RR1 0.45  //机器人半径（常态）
#define RR2 0.53  //机器人半径（持有物品）
#define DENSITY 20 //密度
#define MAXFORWARD 6  //最大前进速度
#define MAXBACKWARD -2  //最大后退速度
#define MAXSPIN PI  //最大旋转速度
#define MAXTRACTION 250  //最大牵引力
#define MAXTORQUE 50  //最大力矩
#define QUANTITY1 DENSITY * PI * RR1 * RR1	//机器人质量（常态）
#define QUANTITY2 DENSITY * PI * RR2 * RR2  //机器人质量（持物品）
#define EMPTY 0 //未携带
#define ALONE -1
#define TERMINALVELOCITY 1.0 //终点速度
#define STARTSPEED 2.0 //起始速度 

using namespace std;

#define LENGTH 100
#define MAXWORKBENCH 50
#define MAXROBOTS 4

typedef pair<int, int> PAIR;

class Workbench {
public:
	int type;
	float x, y;
	int restTime;
	int materialState;
	int productState;
	int robot_id; //来买产品的robot的id

	void setPos(int i, int j);

};

typedef struct Instruction
{
	static const string FORWARD;
	static const string ROTATE;
	static const string BUY;
	static const string SELL;
	static const string DESTORY;
	string instruct;
	int robotId;
	float param;//有些指令需要
}Instruction;

typedef struct Product {
	int id;//工作台id
	int type;//类型
}Product;

class Robot {
public:
	int id;
	bool ready;
	int target_id;
	int workbenchId;//所处工作台 ID
	int carryType;//携带物品类型
	float timeValue;//时间价值系数
	float collisionValue;//碰撞价值系数
	float w;//角速度
	float vx, vy;//线速度(向量) 
	float toward;//朝向
	float x, y;//坐标
	float R; // 半径
	float quantity; // 质量
	vector<Instruction> instructions; 
	double charge; // 机器人的电荷量
	int damntimes;
	 
	void setPos(int i, int j);
	void setInstruct(string ins, int id, float par);

};

class SimpleWorkbench {
public:
	int id;	//工作台id
	int remain; //剩余生产时间 
	int mati; //有多少原料
	SimpleWorkbench() {};
	SimpleWorkbench(int id, int remain, int num) :id(id), remain(remain), mati(num) {};
};

class Material {
public:
	int id; //工作台id
	int type; //产物类型  
	int num;
	Material() {};
	Material(int id, int type,int num) :id(id), type(type),num(num) {};
};

class Map {
public:
	//万有引力
	int K;
	int sn;
	float p;
	/*1表示全图只有9没有7，2代表有7*/
	int only_night;
	int money, frameNumber;
	char map[LENGTH][LENGTH];//使用二维数组记录地图信息（暂定）
	Robot robots[MAXROBOTS];
	int workbenchNum;
	Workbench workbenches[MAXWORKBENCH];
	float distance[MAXWORKBENCH][MAXWORKBENCH];  
	unordered_map<int, vector<SimpleWorkbench>>C_carrier;
	unordered_map<int, int>need; 
	unordered_map<int, vector<Material>>B_carrier;
	bool C[MAXWORKBENCH]; 
	bool B[MAXWORKBENCH][8];  
	//{帧数，工作台类型}
	priority_queue<pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>>>deal; 
	// 123被买的次数
	unordered_map<int, int>numofbuy; 
	// 456工作台接受123的次数
	unordered_map<int, unordered_map<int,int>>numofsell;
	// 工作台的个数 
	unordered_map<int, int>numofwork;

	void init();
	void frameInput();
	void output();
	void strategy();//测试函数，瞎放指令  
	void set_target(int id);
	void buy_next(int id);
	void robot_buy(int id);
	void robot_sell(int id);
	void VirtualFieldAlgorithm(int id); //虚拟势场算法
	/*
		价值估算
	*/
	float estimate_h(Robot& a, Workbench& b, Workbench& c); 
	/*
		抢夺
	*/
	void rob(int id);

	void check_buy(int id);

	/*
		获取买的优先等级
	*/
	vector<vector<int>> choose_buy(int id);

	set<int> choose_sell(int id);
	void ajustK();
}; 

/*
	距离
*/
template <typename T, typename T1>
float dis(T& a, T1& b);
/// <summary>
/// 叉积, >0 顺时针 否则 逆时针
/// </summary>
float cross(Robot& a, Workbench& b);

/// <summary>
/// 力矩等于转动惯量乘以角加速度。即M=J*a。J是转动惯量，a是角加速度，M是力矩
/// J=mr*r
/// </summary>
float get_angular_velocity(Robot& a, Workbench& b);
/*
	点积
*/
float dot(Robot& a, Workbench& b);
/*
	机器人朝向和目标位置的弧度,正数为逆时针，负数为顺时针
*/
float radian(Robot& a, Workbench& b);
/*
	获取线速度
*/
float get_line_speed(Robot& a, Workbench& b); 
/*
	靠近墙
*/
bool workbench_close_to_wall(Workbench& b);
/*
	靠近墙
*/
bool robot_close_to_wall(Robot& b);
/*
	估算大概时间
	与真实时间误差在50帧以内(不包括碰撞的时间)
*/
int time_consume(Robot& a, Workbench& b);
/*
	打乱
*/ 
template <typename T>
void shuffle(vector<T>& v);
/*
	碰撞检测
*/ 
bool collision_detection(Robot& a, Robot& b); 
