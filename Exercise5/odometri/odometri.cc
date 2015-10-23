#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core.hpp>

#include <libplayerc++/playerc++.h>
#include <iostream>
#include <time.h>
//#include "scorpion.h"
#include <stdio.h>
#include <iostream>
#include <cmath>
#include <time.h>
#include <iomanip>

using namespace std;
using namespace PlayerCc;
timespec lolbandit = {3, 0};
timespec readsleep = {0, 100000};
double posx;
double posy;
    PlayerClient robert("172.16.187.128");
    Position2dProxy pp(&robert);
    IrProxy ir(&robert);
    BumperProxy bumper(&robert);

void read () {
    robert.Read();
    nanosleep(&readsleep, NULL);

}


int main() {

    robert.SetDataMode(PLAYER_DATAMODE_PULL);
    robert.SetReplaceRule(true, PLAYER_MSGTYPE_DATA, -1);

    // Reset/ start
    //pp.SetOdometry(0.0, 0.0, 0.0);

    // Read odometry, see how far we have moved, and update particles.
    read();
    printf("x: %f, y: %f\n",pp.GetXPos(),pp.GetYPos());
    pp.SetSpeed(0.0, 0.0);
    pp.SetSpeed(0.3, 0.0);
    nanosleep(&lolbandit, NULL);
    pp.SetSpeed(0.0, 0.0);
    nanosleep(&lolbandit, NULL);
    read();
    printf("x: %f, y: %f\n",pp.GetXPos(),pp.GetYPos());

    pp.SetSpeed(0.0, 0.3);
    nanosleep(&lolbandit, NULL);
    pp.SetSpeed(0.0, 0.0);
    nanosleep(&lolbandit, NULL);
    read();
    printf("x: %f, y: %f\n",pp.GetXPos(),pp.GetYPos());

    pp.SetSpeed(0.3, 0.0);
    nanosleep(&lolbandit, NULL);
    pp.SetSpeed(0.0, 0.0);
    nanosleep(&lolbandit, NULL);

    read();
    printf(";x: %f, y: %f\n",pp.GetXPos(),pp.GetYPos());
    posx = pp.GetXPos();
    posy = pp.GetYPos();
        printf(";x: %f, y: %f\n",posx,posy);

}
