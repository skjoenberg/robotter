#include <libplayerc++/playerc++.h>
#include <iostream>
#include <args.h>
#include <scorpion.h>
using namespace PlayerCc;

int main(int argc, char** argv) {
    // Distance
    double mindistance = 0.70;

    // Objekter
    PlayerClient    robot(gHostname, gPort);
    Position2dProxy pp(&robot, gIndex);
    IrProxy ir(&robot, gIndex);

    // Kør ligeud
    pp.SetSpeed(0.1, 0.0);

    // LÆS LASER!
    while (1) {
        robot.Read();
        if (ir.GetRange(SCORPION_IR_BN_N) < mindistance){
            pp.SetSpeed(0.0, 0.0);
            break;
        }
        sleep(0.2);
    }
    std::cout << "Jeg er færdig mester!" << std::endl;
}
