#include <libplayerc++/playerc++.h>
#include <iostream>
#include <args.h>
#include <scorpion.h>
using namespace PlayerCc;

int main(int argc, char** argv)
{

    PlayerClient    robot(gHostname, gPort);
    Position2dProxy pp(&robot, gIndex);
    IrProxy ir(&robot, gIndex);
    
    if (ir.GetRange(SCORPION_IR_BN_N) < mindistance) 


}