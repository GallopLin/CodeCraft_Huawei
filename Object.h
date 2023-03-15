#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <cstring>
#include <cstdio>

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

using namespace std;

#define LENGTH 100
#define MAXWORKBENCH 50
#define MAXROBOTS 4

class Workbench {
public:
	int type;
	float x, y;
	int restTime;
	int materialState;
	int productState;

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

class Robot {
public:
	bool ready;
	int target_id;
	int next_target_id;
	int workbenchId;//所处工作台 ID
	int carryType;//携带物品类型
	float timeValue;//时间价值系数
	float collisionValue;//碰撞价值系数
	float w;//角速度
	float vx, vy;//线速度(向量)
	float v;
	float toward;//朝向
	float x, y;//坐标
	float R; // 半径
	float quantity; // 质量
	vector<Instruction> instructions;

	void setPos(int i, int j);
	void setInstruct(string ins, int id, float par);

};

class Map {
public:
	int money,frameNumber;
	char map[LENGTH][LENGTH];//使用二维数组记录地图信息（暂定）
	Robot robots[MAXROBOTS];
	int workbenchNum;
	Workbench workbenches[MAXWORKBENCH];
	float distance[MAXWORKBENCH][MAXWORKBENCH];

	void init();
	void frameInput();
	void output();
	void strategy();//测试函数，瞎放指令
};


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
float get_line_speed(Robot& a, Workbench& b, Workbench& c);
/*
	Q: 横冲直撞
*/
bool speed_up(Robot& a, Workbench& b, Workbench& c);