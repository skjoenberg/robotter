#include <libplayerc++/playerc++.h>
#include <iostream>
#include <time.h>
#include <args.h>
#include <scorpion.h>
using namespace PlayerCc;

#define MIN_DISTANCE 1.0

void back_the_fuck_up(Position2dProxy* pp, PlayerClient* robert) {
    pp->SetSpeed(-0.3, 0.0);
    sleep(2);
    pp->SetSpeed(0.0, 0.0);
}

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

void turnRight(Position2dProxy* pp, IrProxy* ir, PlayerClient* robert) {
    pp->SetSpeed(0.0, -0.5);
    while((ir->GetRange(SCORPION_IR_BN_N) < MIN_DISTANCE) &&
          (ir->GetRange(SCORPION_IR_BN_NE) < MIN_DISTANCE)) {
        robert->Read();
    }
    pp->SetSpeed(0.0, 0.0);
}

void turnLeft(Position2dProxy* pp, IrProxy* ir, PlayerClient* robert) {
    pp->SetSpeed(0.0, 0.5);
    while((ir->GetRange(SCORPION_IR_BN_N) < MIN_DISTANCE) &&
          (ir->GetRange(SCORPION_IR_BN_NW) < MIN_DISTANCE)) {
        robert->Read();
    }
    pp->SetSpeed(0.0, 0.0);
}

void frontAction(Position2dProxy* pp, IrProxy* ir, PlayerClient* robert) {
    pp->SetSpeed(0.0, 0.5);
    while(obsFront(ir)) {
        robert->Read();
    }
    pp->SetSpeed(0.0, 0.0);
}

int main(int argc, char** argv) {
    printf("Starter\n");

    // Objekter
    PlayerClient robert(gHostname, gPort);
    Position2dProxy pp(&robert, gIndex);
    IrProxy ir(&robert, gIndex);
    BumperProxy bumper(&robert);

    //streaming data instead of buffering
    robert.SetDataMode(PLAYER_DATAMODE_PULL);
    robert.SetReplaceRule(true, PLAYER_MSGTYPE_DATA, -1);

    // LÃÂS LASER!
    while (1) {
        robert.Read();

        pp.SetSpeed(0.3, 0.0);

        // handle object in front
        if (bumper.IsAnyBumped()) {
            back_the_fuck_up(&pp, &robert);
        }
        if (obsLeft(&ir)){
            turnRight(&pp, &ir, &robert);
        }
        if (obsRight(&ir)) {
            turnLeft(&pp, &ir, &robert);
        }
        if (obsFront(&ir)) {
            frontAction(&pp, &ir, &robert);
        }

    }
}
