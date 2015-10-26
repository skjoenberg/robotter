#include <cmath>
#include <cstdio>
#include <vector>
#include <iostream>
#include <algorithm>
#include <string>
#include <libplayerc++/playerc++.h>
#include "scorpion.h"
#include "robot.h"
using namespace std;
using namespace PlayerCc;

Robot::Robot() {
    PlayerClient roberto("192.168.240.129");
    Position2dProxy ppo(&roberto);
    IrProxy iro(&roberto);
    BumperProxy bumpero(&roberto);
    robert = &roberto;
    pp = &ppo;
    ir = &iro;
    bumper = &bumpero;
}

void Robot::moveXcm(int cm) {
    cout << "moving" << endl;
}

void Robot::turnXradians(double angle) {
    cout << "turning" << endl;
}
