#include <iostream>
#include <vector>
#include <cstring>
#include <cstdio>
#include <algorithm>
#include <set>

#include "Object.h"

using namespace std;
#pragma warning (disable:4996)


int main() { 
	Map map;
	map.init();
	while (1) {
		map.frameInput();
		map.strategy();
		map.output();
	}
	return 0;
}

