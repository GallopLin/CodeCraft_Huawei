#include <iostream>
#include <vector>
#include <cstring>
#include <cstdio>
#include <algorithm>

#include "Object.h"

using namespace std;

#define PI acos(-1)
#define INIT_CAPITAL 200000  //初始资金
#define JUDGEDIS 0.4  //机器人-工作台判定距离
#define RR1 0.45  //机器人半径（常态）
#define RR2 0.53  //机器人半径（持有物品）
#define DENSITY 20 //密度
#define MAXFORWARD 6  //最大前进速度
#define MAXBACKWARD -2  //最大后退速度
#define MAXSPIN 3.14159  //最大旋转速度
#define MAXTRACTION 250  //最大牵引力
#define MAXTORQUE 50  //最大力矩
#define QUANTITY1 DENSITY * PI * RR1 * RR1	//机器人质量（常态）
#define QUANTITY2 DENSITY * PI * RR2 * RR2  //机器人质量（持物品）

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

