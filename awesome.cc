#include <libplayerc++/playerc++.h>
#include <iostream>
#include <time.h>
#include <args.h>
#include <scorpion.h>
using namespace PlayerCc;

#define MIN_DISTANCE 1.0

bool obsFront(IrProxy* ir) {
    return (ir->GetRange(SCORPION_IR_BN_N) < MIN_DISTANCE) ||
        (ir->GetRange(SCORPION_IR_TE_NNW) < MIN_DISTANCE) ||
        (ir->GetRange(SCORPION_IR_TW_NNE) < MIN_DISTANCE);

}

bool obsLeft(IrProxy* ir) {
    return (ir->GetRange(SCORPION_IR_BN_NW) < MIN_DISTANCE);
}

bool obsRight(IrProxy* ir) {
    return (ir->GetRange(SCORPION_IR_BN_NE) < MIN_DISTANCE);
}

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

void frontAction(Position2dProxy* pp, IrProxy* ir, PlayerClient* robot) {
    pp->SetSpeed(0.0, 0.5);
    while(obsFront(ir)) {
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

    // LÃÂS LASER!
    while (1) {
        robot.Read();

        pp.SetSpeed(0.3, 0.0);

        // handle object in front
        if (obsLeft(&ir)){
            turnRight(&pp, &ir, &robot);
        }
        if (obsRight(&ir)) {
            turnLeft(&pp, &ir, &robot);
        }
        if (obsFront(&ir)) {
            frontAction(&pp, &ir, &robot);
        }

    }
}
