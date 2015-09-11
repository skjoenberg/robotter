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
        printf("%f   %f    %f \n", SCORPION_IR_TE_NNW, SCORPION_IR_BN_N, SCORPION_IR_TW_NNE);
        sleep(1);
    }
}
