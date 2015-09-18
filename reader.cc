#include <libplayerc++/playerc++.h>
#include <iostream>
#include <time.h>
#include <args.h>
#include <scorpion.h>
using namespace PlayerCc;

int main(int argc, char** argv) {
     // Objekter
    PlayerClient robert(gHostname, gPort);
    Position2dProxy pp(&robert, gIndex);
    IrProxy ir(&robert, gIndex);
    BumperProxy bumper(&robert);

    // Streaming data instead of buffering
    robert.SetDataMode(PLAYER_DATAMODE_PULL);
    robert.SetReplaceRule(true, PLAYER_MSGTYPE_DATA, -1);

    while(1) {
        robert.Read();
        printf("NNW: %f, N: %f, NNE: %f.\n", ir.GetRange(SCORPION_IR_TE_NNW), ir.GetRange(SCORPION_IR_BN_N), ir.GetRange(SCORPION_IR_TW_NNE));
        sleep(1);
    }
}
