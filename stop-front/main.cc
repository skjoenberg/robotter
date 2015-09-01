#include <libplayerc++/playerc++.h>
#include <iostream>
#include <args.h>
#include <scorpion.h>
using namespace PlayerCc;

int main(int argc, char** argv) {
    double mindistance = 0.70;
    PlayerClient    robot(gHostname, gPort);
    Position2dProxy pp(&robot, gIndex);
    IrProxy ir(&robot, gIndex);

    pp.SetSpeed(0.1, 0.0);
    while (1) {
        robot.Read();
        if (ir.GetRange(SCORPION_IR_BN_N) < mindistance){
            pp.SetSpeed(0.0, 0.0);
            break;
        }
        sleep(0.2);
    }
    std::cout << "Hej" << std::endl;
}
