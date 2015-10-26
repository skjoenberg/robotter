#include <cmath>
#include <cstdio>
#include <vector>
#include <iostream>
#include <algorithm>
#include <string>
#include <time.h>
#include "robot.h"
using namespace std;

timespec sovert = {3, 0};

int main() {

    Robot robert;
    robert.moveXcm(50);
    nanosleep(&sovert, NULL);
    robert.moveXcm(100);
    nanosleep(&sovert, NULL);
    robert.moveXcm(150);
    nanosleep(&sovert, NULL);
    return 0;
}
