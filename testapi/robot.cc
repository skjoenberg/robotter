#include <cmath>
#include <cstdio>
#include <vector>
#include <iostream>
#include <algorithm>
#include <string>
#include "robot.h"
using namespace std;

Robot::Robot() {
    robert = robert("172.16.187.128");
    pp = pp(&robert);
    ir = ir(&robert);
    Bumper = Bumper(&robert);
}

void Robot::moveXcm() {
    cout << "moving" << endl;
}

void Robot::turnXradians(double angle) {
    cout << "turning" << endl;
}
