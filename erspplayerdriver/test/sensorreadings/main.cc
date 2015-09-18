#include <libplayerc++/playerc++.h>
#include <iostream>
#include <args-debugexperiments.h>
#include <scorpion.h>
#include <time.h>
#include <curses.h>
using namespace PlayerCc;

int main(int argc, char** argv)
{
	// Parse input arguments
	parse_args(argc,argv);

	// libplayerc++ throws a PlayerError exception when
	// it runs into trouble
	// The PlayerError is often more readable...
	try
	{
		PlayerClient    robot(gHostname, gPort);
		IrProxy ir(&robot, gIndex);
    PowerProxy pow(&robot, gIndex);

		time_t start, seconds;
	
		// We have to read and wait a little to get subscriptions..
		robot.Read();
		sleep(5);
		robot.Read();
		
		// Print some names and sensors value first
		printf("Number of IR range sensors %i:\n", ir.GetCount());
		printf("Timestamp (sec. since start), ");
		for (int i = 0; i < ir.GetCount(); i++) {
			printf(", %s %i", scorpion_ir_range_name(i),i);
		}
		printf(", Battery charge percentage");
		printf("\n");

		// Start loop printing sensor reading ...
		start = time (NULL);
		for(int i = gIterationsNumber; i > 0; i--)
		{ 
			for(int i = gReadNumber; i > 0; i--)
			{
				// Read from proxies
				robot.Read();
				
				seconds = time (NULL) - start;
				
				printf("%ld,", seconds);
				for (int i = 0; i < ir.GetCount(); i++) {
					printf("%f,", ir.GetRange(i));
				}
				printf("%f", pow.GetCharge());
	
				std::cout << std::endl; 			
				if (gReadWait > 0) sleep(gReadWait);
			}

			if (gIterationsWait > 0) sleep(gIterationsWait);

		} // end for-loop
	
	} // end try
	catch (PlayerCc::PlayerError e)
	{
		std::cerr << e << std::endl;
		return -1;
	}
}
