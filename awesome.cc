#include <libplayerc++/playerc++.h>
#include <iostream>
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
  double mindistance = 0.70;
 
  // Objekter
  PlayerClient robot(gHostname, gPort);
  Position2dProxy pp(&robot, gIndex);
  IrProxy ir(&robot, gIndex);
  robot.SetDataMode(PLAYER_DATAMODE_PULL);
  robot.SetReplaceRule(true, PLAYER_MSGTYPE_DATA, -1);
 
  bool obs = false;
 
  // Kør ligeud
  printf("Koerer! \n");
  pp.SetSpeed(0.3, 0.0);
  sleep(2);
 
  // LÆS LASER!
  while (1) {
    robot.Read();
 
    printf("%f\n", ir.GetRange(SCORPION_IR_BN_N));
    printf("%f\n", ir.GetRange(SCORPION_IR_BN_NE));
    printf("%f\n", ir.GetRange(SCORPION_IR_BN_NW));
 
    obs = ((ir.GetRange(SCORPION_IR_BN_N) < 0.60) ||
           (ir.GetRange(SCORPION_IR_BN_NE) < 0.60) ||
           (ir.GetRange(SCORPION_IR_BN_NW) < 0.60));
 
    if(obs) {
      printf("Succes!\r\n");
      pp.SetSpeed(0.0, 1.0);
      sleep(3);
      pp.SetSpeed(0.3, 0.0);
      sleep(1);
    }
  }
}