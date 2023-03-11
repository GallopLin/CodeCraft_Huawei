#pragma once

#include <iostream>
#include <vector>
#include <cstring>
#include <cstdio>

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
	string instruct;
	int robotId;
	float param;//有些指令需要
}Instruction;

class Robot {
public:
	int workbenchId;//所处工作台 ID
	int carryType;//携带物品类型
	float timeValue;//时间价值系数
	float collisionValue;//碰撞价值系数
	float w;//角速度
	float vx, vy;//线速度(向量)
	float toward;//朝向
	float x, y;//坐标
	vector<Instruction> instructions;

	void setPos(int i, int j);

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
};

