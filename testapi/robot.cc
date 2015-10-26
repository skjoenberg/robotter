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

void read () {
    this.robert.read();
    timespec readsleep = {0, 100000};
    nanosleep(&readsleep, NULL);
}

void Robot::moveXcm(int cm) {
    double meters = (double) cm / 100.;
    this.read();
    double startx = this.robert.getXpos();
    double starty = this.robert.getYpos();
    dist = 0;
    double currentx, currenty;
    this.pp.SetSpeed(0.3, 0.0);
    while(dist < meters) {
        this.read();
        double currentx = this.robert.getXpos();
        double currenty = this.robert.getYpos();
        dist = sqrt(pow(currentx - startx, 2.)+pow(currenty - starty, 2.));
    }
    this.pp.SetSpeed(0.0, 0.0);
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
    if (target < 0) {
        target += 2 * M_PI;
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
