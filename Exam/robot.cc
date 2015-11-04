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
#include "defines.h"

using namespace PlayerCc;
using namespace std;
Robot::Robot() {
    robert = new PlayerClient("172.16.187.128");
    pp = new Position2dProxy(robert);
    ir = new IrProxy (robert);
    bumper = new BumperProxy(robert);
    robert->SetDataMode(PLAYER_DATAMODE_PULL);
    robert->SetReplaceRule(true, PLAYER_MSGTYPE_DATA, -1);
}

void Robot::read() {
    robert->Read();
    timespec readsleep = {0, 200000};
    nanosleep(&readsleep, NULL);
}

bool Robot::obsFront() {
    read();
    cout << "N:   " << ir->GetRange(SCORPION_IR_BN_N) << endl;
    cout << "NNE: " << ir->GetRange(SCORPION_IR_TE_NNE) << endl;
    cout << "NNW: " << ir->GetRange(SCORPION_IR_TW_NNW) << endl;

    return ((ir->GetRange(SCORPION_IR_BN_N) < MIN_DISTANCE) ||
            (ir->GetRange(SCORPION_IR_TE_NNW) < MIN_DISTANCE) ||
            (ir->GetRange(SCORPION_IR_TW_NNE) < MIN_DISTANCE));
}

bool Robot::obsLeft() {
    read();
    return (ir->GetRange(SCORPION_IR_BN_NW) < (MIN_DISTANCE-0.1) ||
            ir->GetRange(SCORPION_IR_TW_NNW) < (MIN_DISTANCE-0.1));
}

bool Robot::obsRight() {
    read();
    return (ir->GetRange(SCORPION_IR_BN_NE) < (MIN_DISTANCE-0.1) ||
            ir->GetRange(SCORPION_IR_TE_NNE) < (MIN_DISTANCE-0.1));
}


void Robot::turnObstacle(){
    double speed = 0.2;
    bool left = false;
    if (obsLeft()) {
        speed = -speed;
        left = true;
    }
    pp->SetSpeed(0.0 , speed);
    if (left) {
        while(obsFront() || obsLeft()){
        }
    } else {
        while(obsFront() || obsRight()){
        }
    }
    pp->SetSpeed(0.0, 0.0);
}

int Robot::moveXcm(int cm, double speed) {
    double meters = (double) cm / 100.;
    read();
    double startx = pp->GetXPos();
    double starty = pp->GetYPos();
    double dist = 0;
    double currentx, currenty;

    while(dist < meters) {
        pp->SetSpeed(speed, 0.0);
        read();
        if(obsFront() || obsRight() || obsLeft()) {
            pp->SetSpeed(0.0, 0.0);
            return -1;
        }
        currentx = pp->GetXPos();
        currenty = pp->GetYPos();
        dist = sqrt(pow(currentx - startx, 2.)+pow(currenty - starty, 2.));
    }
        pp->SetSpeed(0.0, 0.0);
    return 1;
}

void Robot::turnXradians(double angle) {
    read();
    double start = pp->GetYaw();
    double target = start + angle;
    double current = start;

    if (target < -M_PI) {
        target += 2 * M_PI;
    }
    if (target > M_PI) {
        target -= 2 * M_PI;
    }

    if(angle < 0) {
        pp->SetSpeed(0.0, -0.3);
    } else {
        pp->SetSpeed(0.0, 0.3);
    }

    while(abs(target - current) > 0.1) {
        //        cout << "lolle " << abs(current - start) << endl;
        read();
        current = pp->GetYaw();
    }

    pp->SetSpeed(0.0, 0.0);
    return;
}
