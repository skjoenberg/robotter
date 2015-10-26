#include <cmath>
#include <cstdio>
#include <vector>
#include <iostream>
#include <algorithm>
#include <string>
#include <libplayerc++/playerc++.h>
#include "scorpion.h"

class Robot {
 public:
    PlayerCc::PlayerClient* robert;
    PlayerCc::Position2dProxy* pp;
    PlayerCc::IrProxy* ir;
    PlayerCc::BumperProxy* bumper;
    Robot();

    void read();
    void moveXcm(int cm);
    void turnXradians(double angle);
