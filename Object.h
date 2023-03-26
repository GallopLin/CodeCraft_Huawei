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
#define TERMINALVELOCITY 1.0 //�յ��ٶ�
#define STARTSPEED 2.0 //��ʼ�ٶ� 

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
	int robot_id; //�����Ʒ��robot��id

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

typedef struct Product {
	int id;//����̨id
	int type;//����
}Product;

class Robot {
public:
	int id;
	bool ready;
	int target_id;
	int workbenchId;//��������̨ ID
	int carryType;//Я����Ʒ����
	float timeValue;//ʱ���ֵϵ��
	float collisionValue;//��ײ��ֵϵ��
	float w;//���ٶ�
	float vx, vy;//���ٶ�(����) 
	float toward;//����
	float x, y;//����
	float R; // �뾶
	float quantity; // ����
	vector<Instruction> instructions; 
	double charge; // �����˵ĵ����
	int damntimes;
	 
	void setPos(int i, int j);
	void setInstruct(string ins, int id, float par);

};

class SimpleWorkbench {
public:
	int id;	//����̨id
	int remain; //ʣ������ʱ�� 
	int mati; //�ж���ԭ��
	SimpleWorkbench() {};
	SimpleWorkbench(int id, int remain, int num) :id(id), remain(remain), mati(num) {};
};

class Material {
public:
	int id; //����̨id
	int type; //��������  
	int num;
	Material() {};
	Material(int id, int type,int num) :id(id), type(type),num(num) {};
};

class Map {
public:
	//��������
	int K;
	int sn;
	float p;
	/*1��ʾȫͼֻ��9û��7��2������7*/
	int only_night;
	int money, frameNumber;
	char map[LENGTH][LENGTH];//ʹ�ö�ά�����¼��ͼ��Ϣ���ݶ���
	Robot robots[MAXROBOTS];
	int workbenchNum;
	Workbench workbenches[MAXWORKBENCH];
	float distance[MAXWORKBENCH][MAXWORKBENCH];  
	unordered_map<int, vector<SimpleWorkbench>>C_carrier;
	unordered_map<int, int>need; 
	unordered_map<int, vector<Material>>B_carrier;
	bool C[MAXWORKBENCH]; 
	bool B[MAXWORKBENCH][8];  
	//{֡��������̨����}
	priority_queue<pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>>>deal; 
	// 123����Ĵ���
	unordered_map<int, int>numofbuy; 
	// 456����̨����123�Ĵ���
	unordered_map<int, unordered_map<int,int>>numofsell;
	// ����̨�ĸ��� 
	unordered_map<int, int>numofwork;

	void init();
	void frameInput();
	void output();
	void strategy();//���Ժ�����Ϲ��ָ��  
	void set_target(int id);
	void buy_next(int id);
	void robot_buy(int id);
	void robot_sell(int id);
	void VirtualFieldAlgorithm(int id); //�����Ƴ��㷨
	/*
		��ֵ����
	*/
	float estimate_h(Robot& a, Workbench& b, Workbench& c); 
	/*
		����
	*/
	void rob(int id);

	void check_buy(int id);

	/*
		��ȡ������ȵȼ�
	*/
	vector<vector<int>> choose_buy(int id);

	set<int> choose_sell(int id);
	void ajustK();
}; 

/*
	����
*/
template <typename T, typename T1>
float dis(T& a, T1& b);
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
float get_line_speed(Robot& a, Workbench& b); 
/*
	����ǽ
*/
bool workbench_close_to_wall(Workbench& b);
/*
	����ǽ
*/
bool robot_close_to_wall(Robot& b);
/*
	������ʱ��
	����ʵʱ�������50֡����(��������ײ��ʱ��)
*/
int time_consume(Robot& a, Workbench& b);
/*
	����
*/ 
template <typename T>
void shuffle(vector<T>& v);
/*
	��ײ���
*/ 
bool collision_detection(Robot& a, Robot& b); 
