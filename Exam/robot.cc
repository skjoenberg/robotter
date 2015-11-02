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
    robert = new PlayerClient("172.16.187.128");;
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


bool obsFront() {
    return (ir->GetRange(SCORPION_IR_BN_N) < MIN_DISTANCE) ||
        (ir->GetRange(SCORPION_IR_TE_NNW) < MIN_DISTANCE) ||
        (ir->GetRange(SCORPION_IR_TW_NNE) < MIN_DISTANCE);
}


void turnObstacle(){
    pp->SetSpeed(0.0 , 0,3);
    while(obsFront()){
        read();
    }
    pp->SetSpeed(0.0, 0.0);
}
int Robot::moveXcm(int cm) {
    double meters = (double) cm / 100.;
    read();
    double startx = pp->GetXPos();
    double starty = pp->GetYPos();
    double dist = 0;
    double currentx, currenty;
    while(dist < meters) {
        pp->SetSpeed(0.3, 0.0);
        read();
        if(obsFront){
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
