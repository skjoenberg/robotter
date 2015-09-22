#include <libplayerc++/playerc++.h>
#include <iostream>
#include <args.h>
#include <scorpion.h>
using namespace PlayerCc;

int main(int argc, char** argv)
{
	// Parse input arguments
	parse_args(argc,argv);
	// libplayerc++ throws a PlayerError exception when
	// it runs into trouble
	// The PlayerError is often more readable...
	printf("Lets see if it goes here\n");
	try
	{
		PlayerClient    robot(gHostname, gPort);
		PowerProxy pow(&robot, gIndex);
		/* Go into read-think-act loop */

		for(;;)
		{ 
			// Read from proxies
			robot.Read();

			// Powerinterface reading
			printf("=== Powerintface ===\n");
			printf("Battery charge reads: %f\n", pow.GetCharge());
		} // end for-loop
	} // end try
	catch (PlayerCc::PlayerError e)
	{
		std::cerr << e << std::endl;
		return -1;
	}
}
