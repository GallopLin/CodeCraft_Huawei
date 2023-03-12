#include <iostream>
#include <vector>
#include <cstring>
#include <cstdio>
#include <algorithm>

#include "Object.h"

using namespace std;

#define PI acos(-1)
#define INIT_CAPITAL 200000  //��ʼ�ʽ�
#define JUDGEDIS 0.4  //������-����̨�ж�����
#define RR1 0.45  //�����˰뾶����̬��
#define RR2 0.53  //�����˰뾶��������Ʒ��
#define DENSITY 20 //�ܶ�
#define MAXFORWARD 6  //���ǰ���ٶ�
#define MAXBACKWARD -2  //�������ٶ�
#define MAXSPIN 3.14159  //�����ת�ٶ�
#define MAXTRACTION 250  //���ǣ����
#define MAXTORQUE 50  //�������
#define QUANTITY1 DENSITY * PI * RR1 * RR1	//��������������̬��
#define QUANTITY2 DENSITY * PI * RR2 * RR2  //����������������Ʒ��

int main() {
	//cout << QUANTITY1;
	//system("pause");
	Map map;
	map.init();
	while (cin) {
		map.frameInput();
		map.strategy();
		map.output();
	}
	return 0;
}

