#include <libplayerc++/playerc++.h>
#include <iostream>
#include <rand>
#include <time.h>
#include <args.h>
#include <scorpion.h>
using namespace PlayerCc;
 
/*
int front(Position2dProxy* pp, PlayerClient* robot, IrProxy* ir, double mindistance){
  printf("Har lige laest\n");
 
  sleep(1);
  if (dist < mindistance){
    printf("Hej\n");
    return 1;
  }
  return 0;
}
*/
 
int main(int argc, char** argv) {
  printf("Starter\n");
  // Distance
  double mindistance = 0.60;
 
  //random seed
  srand (time(NULL));
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
 
  // Kør ligeud
  printf("Koerer! \n");
  pp.SetSpeed(0.3, 0.0);
  sleep(2);
 
  // LÆS LASER!
  while (1) {
    robot.Read();
    int random_num = (rand() % 100) / 100; 
    /* Printing
    printf("%f\n", ir.GetRange(SCORPION_IR_BN_N));
    printf("%f\n", ir.GetRange(SCORPION_IR_BN_NE));
    printf("%f\n", ir.GetRange(SCORPION_IR_BN_NW));
  */
    obs_front = ((ir.GetRange(SCORPION_IR_BN_N) < mindistance) ||
           (ir.GetRange(SCORPION_IR_BN_NE) < mindistance) ||
           (ir.GetRange(SCORPION_IR_BN_NW) < mindistance));
 
    obs_right = (ir.GetRange(SCORPION_IR_BS_E) < mindistance);
    obs_left = (ir.GetRange(SCORPION_IR_BS_W) < mindistance);

    // handle object in front
    if(obs_front) {
      if(obs_left){
        //turn right
        pp.SetSpeed(0.0, -1.0);
        sleep(2);
      }
      else if(obs_right){
        // left
        pp.SetSpeed(0.0, 1.0);
        sleep(2);
      }
    }
  }
}