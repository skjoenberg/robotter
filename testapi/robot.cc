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
}

void Robot::read () {
    cout << "hej" << endl;
    robert->Read();
    timespec readsleep = {0, 100000};
    nanosleep(&readsleep, NULL);
}

void Robot::moveXcm(int cm) {
    double meters = (double) cm / 100.;
    cout << "hej" << endl;
    read();
    cout << "nej" << endl;
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

void Robot::turnXradians(double angle) {

}
