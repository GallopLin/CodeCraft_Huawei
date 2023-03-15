#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <cstring>
#include <cstdio>

#define PI acos(-1)
#define INIT_CAPITAL 200000  //��ʼ�ʽ�
#define JUDGEDIS 0.4  //������-����̨�ж�����
#define RR1 0.45  //�����˰뾶����̬��
#define RR2 0.53  //�����˰뾶��������Ʒ��
#define DENSITY 20 //�ܶ�
#define MAXFORWARD 6  //���ǰ���ٶ�
#define MAXBACKWARD -2  //�������ٶ�
#define MAXSPIN PI  //�����ת�ٶ�
#define MAXTRACTION 250  //���ǣ����
#define MAXTORQUE 50  //�������
#define QUANTITY1 DENSITY * PI * RR1 * RR1	//��������������̬��
#define QUANTITY2 DENSITY * PI * RR2 * RR2  //����������������Ʒ��
#define EMPTY 0 //δЯ��
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
	float param;//��Щָ����Ҫ
}Instruction;

class Robot {
public:
	bool ready;
	int target_id;
	int next_target_id;
	int workbenchId;//��������̨ ID
	int carryType;//Я����Ʒ����
	float timeValue;//ʱ���ֵϵ��
	float collisionValue;//��ײ��ֵϵ��
	float w;//���ٶ�
	float vx, vy;//���ٶ�(����)
	float v;
	float toward;//����
	float x, y;//����
	float R; // �뾶
	float quantity; // ����
	vector<Instruction> instructions;

	void setPos(int i, int j);
	void setInstruct(string ins, int id, float par);

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
	void strategy();//���Ժ�����Ϲ��ָ��
};


/// <summary>
/// ���, >0 ˳ʱ�� ���� ��ʱ��
/// </summary>
float cross(Robot& a, Workbench& b);

/// <summary>
/// ���ص���ת���������ԽǼ��ٶȡ���M=J*a��J��ת��������a�ǽǼ��ٶȣ�M������
/// J=mr*r
/// </summary>
float get_angular_velocity(Robot& a, Workbench& b);
/*
	���
*/
float dot(Robot& a, Workbench& b);
/*
	�����˳����Ŀ��λ�õĻ���,����Ϊ��ʱ�룬����Ϊ˳ʱ��
*/
float radian(Robot& a, Workbench& b);
/*
	��ȡ���ٶ�
*/
float get_line_speed(Robot& a, Workbench& b, Workbench& c);
/*
	Q: ���ֱײ
*/
bool speed_up(Robot& a, Workbench& b, Workbench& c);