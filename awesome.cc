#include <libplayerc++/playerc++.h>
#include <iostream>
#include <time.h>
#include <args.h>
#include <scorpion.h>
using namespace PlayerCc;

#define MIN_DISTANCE 0.50
#define SPEED 0.2

void forward(Position2dProxy* pp) {
    pp->SetSpeed(SPEED, 0.0);
}

bool obsFront(IrProxy* ir) {
    return (ir->GetRange(SCORPION_IR_BN_N) < MIN_DISTANCE) ||
        (ir->GetRange(SCORPION_IR_TE_NNW) < MIN_DISTANCE) ||
        (ir->GetRange(SCORPION_IR_TW_NNE) < MIN_DISTANCE);
}

bool obsBack(IrProxy* ir) {
    return (ir->GetRange(SCORPION_IR_BW_S) < MIN_DISTANCE) ||
        (ir->GetRange(SCORPION_IR_BE_S) < MIN_DISTANCE);
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


void reverse(Position2dProxy* pp, IrProxy* ir, PlayerClient* robert) {
    // Reverse
    pp->SetSpeed(-SPEED, 0.0);

    // Check front
    while (obsFront(ir)) {
        robert->Read();

        // Check back
        if (obsBack(ir)) {
            break;
        }
    }
    pp->SetSpeed(0.0, 0.0);
}

int main(int argc, char** argv) {
    printf("Initializing thrusters!\n");

    // Variables
    int turns = 0;


    // Player objects
    PlayerClient robert(gHostname, gPort);
    Position2dProxy pp(&robert, gIndex);
    IrProxy ir(&robert, gIndex);
    BumperProxy bumper(&robert);

    // Streaming data instead of buffering
    robert.SetDataMode(PLAYER_DATAMODE_PULL);
    robert.SetReplaceRule(true, PLAYER_MSGTYPE_DATA, -1);

    printf("Ready for take-off!\n");
    // Drive!
    while (1) {
        // Get sensor information
        robert.Read();
        printf("Turns: %d.\n", turns);

        // Check bumper
        if (bumper.IsAnyBumped()) {
            reverse(&pp, &ir, &robert);
        }
        else if (turns > 60) {
            turns = 0;
            pp.SetSpeed(-SPEED, 0.0);
            sleep(1);
        }
        // Check front
        else if (obsFront(&ir)) {
            turns++;
            // Turn left or right
            if (obsRight(&ir)) {
                turnLeft(&pp, &ir, &robert);
            }
            else {
                turnRight(&pp, &ir, &robert);
            }
        }

        // Check front-left and adjust
        else if (obsLeft(&ir)){
            turns++;
            turnRight(&pp, &ir, &robert);
        }

        // Check front-right and adjust
        else if (obsRight(&ir)) {
            turns++;
            turnLeft(&pp, &ir, &robert);
        }

        // Continue driving forward
        else {
            turns = 0;
            forward(&pp);
        }
    }
}
