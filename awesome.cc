#include <libplayerc++/playerc++.h>
#include <iostream>
#include <time.h>
#include <args.h>
#include <scorpion.h>
using namespace PlayerCc;

#define MIN_DISTANCE 0.50
#define SPEED 0.2

void back_the_fuck_up(Position2dProxy* pp, PlayerClient* robert) {
    pp->SetSpeed(-SPEED, 0.0);
    sleep(2);
    pp->SetSpeed(0.0, 0.0);
}

void go_bot_go(Position2dProxy* pp) {
    pp->SetSpeed(SPEED, 0.0);
}

bool obsFront(IrProxy* ir) {
    return (ir->GetRange(SCORPION_IR_BN_N) < MIN_DISTANCE) ||
        (ir->GetRange(SCORPION_IR_TE_NNW) < MIN_DISTANCE) ||
        (ir->GetRange(SCORPION_IR_TW_NNE) < MIN_DISTANCE);
}

bool obsLeft(IrProxy* ir) {
    return (ir->GetRange(SCORPION_IR_BN_NW) < (MIN_DISTANCE-0.1) ||
            ir->GetRange(SCORPION_IR_TW_NNW) < (MIN_DISTANCE-0.1));
}

bool obsRight(IrProxy* ir) {
    return (ir->GetRange(SCORPION_IR_BN_NE) < (MIN_DISTANCE-0.1) ||
            ir->GetRange(SCORPION_IR_TE_NNE) < (MIN_DISTANCE-0.1));
}

void turnRight(Position2dProxy* pp, IrProxy* ir, PlayerClient* robert) {
    pp->SetSpeed(0.0, -0.5);
    while (obsFront(ir)) {
        robert->Read();
    }
}

void turnLeft(Position2dProxy* pp, IrProxy* ir, PlayerClient* robert) {
    pp->SetSpeed(0.0, 0.5);
    while (obsFront(ir)) {
        robert->Read();
    }
}

void frontAction(Position2dProxy* pp, IrProxy* ir, PlayerClient* robert) {
    pp->SetSpeed(0.0, 0.5);
}

int main(int argc, char** argv) {
    printf("Starter\n");

    // Objekter
    PlayerClient robert(gHostname, gPort);
    Position2dProxy pp(&robert, gIndex);
    IrProxy ir(&robert, gIndex);
    BumperProxy bumper(&robert);

    // Streaming data instead of buffering
    robert.SetDataMode(PLAYER_DATAMODE_PULL);
    robert.SetReplaceRule(true, PLAYER_MSGTYPE_DATA, -1);

    // LÃÂS LASER!
    while (1) {
        robert.Read();

        // handle object in front
        if (bumper.IsAnyBumped()) {
            back_the_fuck_up(&pp, &robert);
        }
        // Check front
        else if (obsFront(&ir)) {
            if (obsRight(&ir)) {
                turnLeft(&pp, &ir, &robert);
            }
            else {
                turnRight(&pp, &ir, &robert);
            }
        }
        else if (obsLeft(&ir)){
            turnRight(&pp, &ir, &robert);
        }
        else if (obsRight(&ir)) {
            turnLeft(&pp, &ir, &robert);
        }
        else {
            go_bot_go(&pp);
        }
    }
}
