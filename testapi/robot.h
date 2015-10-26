#include <cmath>
#include <cstdio>
#include <vector>
#include <iostream>
#include <algorithm>
#include <string>
#include <libplayerc++/playerc++.h>
#include <scorpion.h>


class Robot {
 public:
    Robot();
    PlayerClient robert;
    Position2dProxy pp;
    IrProxy ir;
    BumperProxy Bumper;

    void moveXcm(int cm);
    void turnXradians(double angle);
};
