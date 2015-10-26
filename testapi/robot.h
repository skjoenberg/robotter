#include <cmath>
#include <cstdio>
#include <vector>
#include <iostream>
#include <algorithm>
#include <string>
#include <libplayerc++/playerc++.h>
#include "scorpion.h"

struct robot {
    PlayerCc::PlayerClient bot;
    PlayerCc::Position2dProxy pp;
    PlayerCc::IrProxy ir;
    PlayerCc::BumperProxy bumper;
}


void read(robot* theBot);
void moveXcm(robot* theBot, int cm);
void turnXradians(robot* theBot, double angle);
