#include <cmath>
#include <cstdio>
#include <vector>
#include <iostream>
#include <algorithm>
#include <string>
#include <time.h>
#include "robot.h"
#include <libplayerc++/playerc++.h>
#include "scorpion.h"
using namespace std;

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

void Robot::turnXradians(double angle) {
    cout << "turning" << endl;
}
