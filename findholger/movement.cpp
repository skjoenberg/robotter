#include <libplayerc++/playerc++.h>
#include <iostream>
#include <time.h>
#include "scorpion.h"
using namespace PlayerCc;

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

bool obsWeakLeft(IrProxy* ir) {
    return (ir->GetRange(SCORPION_IR_BN_NW) < (MIN_DISTANCE-0.2) ||
            ir->GetRange(SCORPION_IR_TW_NNW) < (MIN_DISTANCE-0.2));
}

bool obsWeakRight(IrProxy* ir) {
    return (ir->GetRange(SCORPION_IR_BN_NE) < (MIN_DISTANCE-0.2) ||
            ir->GetRange(SCORPION_IR_TE_NNE) < (MIN_DISTANCE-0.2));
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
