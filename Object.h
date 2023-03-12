#pragma once

#include <iostream>
#include <vector>
#include <string>
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
	static const string FORWARD;
	static const string ROTATE;
	static const string BUY;
	static const string SELL;
	static const string DESTORY;
	string instruct;
	int robotId;
	float param;//��Щָ����Ҫ
}Instruction;

class Robot {
public:
	int workbenchId;//��������̨ ID
	int carryType;//Я����Ʒ����
	float timeValue;//ʱ���ֵϵ��
	float collisionValue;//��ײ��ֵϵ��
	float w;//���ٶ�
	float vx, vy;//���ٶ�(����)
	float toward;//����
	float x, y;//����
	vector<Instruction> instructions;

	void setPos(int i, int j);

};

class Map {
public:
	int money,frameNumber;
	char map[LENGTH][LENGTH];//ʹ�ö�ά�����¼��ͼ��Ϣ���ݶ���
	Robot robots[MAXROBOTS];
	int workbenchNum;
	Workbench workbenches[MAXWORKBENCH];
	float distance[MAXWORKBENCH][MAXWORKBENCH];

	void init();
	void frameInput();
	void output();
};

