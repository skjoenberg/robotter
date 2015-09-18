#include <libplayerc++/playerc++.h>
#include <iostream>
#include <unistd.h>

std::string  gHostname(PlayerCc::PLAYER_HOSTNAME);
uint         gPort(PlayerCc::PLAYER_PORTNUM);
uint         gIndex(0);
uint         gDebug(0);
uint         gReadNumber(1);
uint				 gReadWait(1);
uint         gIterationsNumber(1);
uint         gIterationsWait(1);

void print_usage(int argc, char** argv);

int parse_args(int argc, char** argv)
{
  // set the flags
  const char* optflags = "h:p:i:n:w:t:v:";
  int ch;

  // use getopt to parse the flags
  while(-1 != (ch = getopt(argc, argv, optflags)))
  {
    switch(ch)
    {
      // case values must match long_options
      case 'h': // hostname
          gHostname = optarg;
          break;
      case 'p': // port
          gPort = atoi(optarg);
          break;
      case 'i': // index
          gIndex = atoi(optarg);
          break;
			case 'd': // debug
					gDebug = atoi(optarg);
					break;														
			case 'n': // number of consecutive sensor reading to do with gReadWait sec. (sleep) between
					gReadNumber = atoi(optarg);
					break;
			case 'w': // seconds (sleep) to wait between each reading
					gReadWait = atoi(optarg);
					break;
			case 't': // number of time to do the sensor reading serie with gIterationWait sec. (sleep) between
					gIterationsNumber = atoi(optarg);
					break;
			case 'v':	// "sleep" - time that are waited between each reading serie
					gIterationsWait = atoi(optarg);
					break;
      case '?': 
          print_usage(argc, argv);
          exit (-1);
      default:  // unknown
	  if (isprint (optopt))
	     fprintf (stderr, "Unknown option `-%c'.\n", optopt);
	  else
          fprintf (stderr, "Unknown option character `\\x%x'.\n", optopt);
          print_usage(argc, argv);
          exit (-1);
    }
  }
  return (0);
} // end parse_args



void print_usage(int argc, char** argv)
{
  using namespace std;
  cerr << "USAGE:  " << *argv << " [options]" << endl << endl;
  cerr << "Where [options] can be:" << endl;
  cerr << "  -h <hostname>            : hostname to connect to (default: "
       << PlayerCc::PLAYER_HOSTNAME << ")" << endl;
  cerr << "  -p <port>                : port where Player will listen (default: "
       << PlayerCc::PLAYER_PORTNUM << ")" << endl;
  cerr << "  -i <index>               : device index (default: 0)" << endl;
  cerr << "  -d <level>               : debug message level (0 = none -- 9 = all)" << endl;
	cerr << "  -n <readnumber>          : number of consecutive reading to do with 'readwait' secs between" << endl;
	cerr << "  -w <readwait>            : secs 'sleep(readwait)' between each set of sensor reading" << endl;
	cerr << "  -t <iterationsnumber>    : number of read experiments to do" << endl;
	cerr << "  -v <iterationswait>      : secs 'sleep(iterationswait)' between each set of read experiments" 
       << endl;
} // end print_usage
