#include <libplayerc++/playerc++.h>
#include <iostream>
#include <time.h>
#include <args.h>
#include <scorpion.h>
using namespace PlayerCc;

#define MIN_DISTANCE 1.0

void turnRight(Position2dProxy* pp, IrProxy* ir, PlayerClient* robot) {
    pp->SetSpeed(0.0, -0.5);
    while((ir->GetRange(SCORPION_IR_BN_N) < MIN_DISTANCE) &&
          (ir->GetRange(SCORPION_IR_BN_NE) < MIN_DISTANCE)) {
        robot->Read();
        sleep(1);
    }
    pp->SetSpeed(0.0, 0.0);
}

void turnLeft(Position2dProxy* pp, IrProxy* ir, PlayerClient* robot) {
    pp->SetSpeed(0.0, 0.5);
    while((ir->GetRange(SCORPION_IR_BN_N) < MIN_DISTANCE) &&
          (ir->GetRange(SCORPION_IR_BN_NW) < MIN_DISTANCE)) {
        robot->Read();
        sleep(1);
    }
    pp->SetSpeed(0.0, 0.0);
}

int main(int argc, char** argv) {
    printf("Starter\n");



    // Objekter
    PlayerClient robot(gHostname, gPort);
    Position2dProxy pp(&robot, gIndex);
    IrProxy ir(&robot, gIndex);

    //streaming data instead of buffering
    robot.SetDataMode(PLAYER_DATAMODE_PULL);
    robot.SetReplaceRule(true, PLAYER_MSGTYPE_DATA, -1);

    bool obs_front = false;
    bool obs_right = false;
    bool obs_left = false;

    // LÃÂS LASER!
    while (1) {
        robot.Read();

        pp.SetSpeed(0.3, 0.0);
        obs_front = (ir.GetRange(SCORPION_IR_BN_N) < MIN_DISTANCE);
        obs_right = (ir.GetRange(SCORPION_IR_BN_NE) < MIN_DISTANCE);
        obs_left = (ir.GetRange(SCORPION_IR_BN_NW) < MIN_DISTANCE);

        // handle object in front
        if (obs_left){
                turnRight(&pp, &ir, &robot);
        }
        if (obs_right) {
                turnLeft(&pp, &ir, &robot);
        }

    }
}
