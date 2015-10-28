#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core.hpp>

#include <stdio.h>
#include <iostream>
#include <cmath>
#include <time.h>
#include <iomanip>

//#include "index.h"
#include "camera.h"
#include "particles.h"
#include "random_numbers.h"

PlayerClient robert("172.16.187.128");
Position2dProxy pp(&robert);
IrProxy ir(&robert);
//BumperProxy bumper(&robert);

// Reset/ start
pp.SetOdometry();
pp.ResetOdometry();

// Read odometry, see how far we have moved, and update particles.
cout << "x: " << pp.GetXPos() << "y: " << pp.GetYPos() << endl;

pp.SetSpeed(0.3, 0.0);
sleep(5);

cout << "x: " << pp.GetXPos() << "y: " << pp.GetYPos() << endl;

pp.SetSpeed(0.0, 0.3);
sleep(3);

cout << "x: " << pp.GetXPos() << "y: " << pp.GetYPos() << endl;

pp.SetSpeed(0.3, 0.0);
sleep(3);

cout << "x: " << pp.GetXPos() << "y: " << pp.GetYPos() << endl;
