#include <cmath>
#include <cstdio>
#include <vector>
#include <iostream>
#include <algorithm>
#include <string>
#include <time.h>
#include <libplayerc++/playerc++.h>
#include "scorpion.h"
#include "robot.h"
using namespace PlayerCc;
using namespace std;

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

void Robot::read () {
    robert->Read();
    timespec readsleep = {0, 100000};
    nanosleep(&readsleep, NULL);
}

void Robot::moveXcm(int cm) {
    double meters = (double) cm / 100.;
    read();
    double startx = pp->GetXPos();
    double starty = pp->GetYPos();
    double dist = 0;
    double currentx, currenty;
    pp->SetSpeed(0.3, 0.0);
    while(dist < meters) {
        read();
        currentx = pp->GetXPos();
        currenty = pp->GetYPos();
        dist = sqrt(pow(currentx - startx, 2.)+pow(currenty - starty, 2.));
    }
    pp->SetSpeed(0.0, 0.0);
    return;
}

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

void Robot::turnXradians(double angle) {
    double start = pp->GetYaw();
    double target = start + angle;
    double current = start;
    if (target < -M_PI) {
        target += 2 * M_PI;
    }
    if (target > M_PI) {
        target -= 2 * M_PI
    }
    if(angle < 0) {
        pp->SetSpeed(0.0, -0.3);
    } else {
        pp->SetSpeed(0.0, -0.3);
    }
    while(abs(current - start) > 0.2) {
        read();
        current = pp->GetYaw();
    }
    pp->SetSpeed(0.0, 0.0);
    return;
}
