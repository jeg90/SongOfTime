#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int
main(int argc, char *argv[ ])
{
	//Used for reading retval from getopt
    	int c;

	//Create the different mode flags
    	int errFlag=0;
	int startFlag=0;
	int finishFlag=0;
	int reportFlag=0;

	//Start Title 
    	char *startTitle=NULL;

	//getopt flags and variables
    	extern char *optarg;
    	extern int optind, optopt;

	//Iterate through command line options to set mode flags
    	while ((c = getopt(argc, argv, "s:fr?")) != -1) 
	{
        	switch(c) 
		{
        		case 's':
            			startTitle = optarg;
				if(startTitle==NULL)
				{
					errFlag=1;
				}
				startFlag=1;
            			break;
        		case 'f':
            			finishFlag=1;
            			break;
			case 'r':
				reportFlag=1;
				break;
            		case ':':       /* -f or -o without operand */
                    		errFlag=1;
                    		break;
        		case '?':
            			errFlag=1;
				break;
        	}
    	}

	//Error case: bad args configuration
	if(errFlag==1 || (startFlag==1 && finishFlag==1) || (startFlag==0 && finishFlag==0))
	{
		printf("\nImproper arguments, Try: songoftime [-s TaskName] [-f] [-r]");
	}
	else if(startFlag==1)//Start command
	{
		//check if a task is already being tracked- (already open)
		int alreadyOpen;
		if(alreadyOpen==1)//Print error message and quit
		{
			printf("\nA task is already being tracked.  Please close task before continuing");
			printf("\n\tTry: songoftime -f");
			exit(0);
		}
		
		
	}
	else if(finishFlag==1)//Task finish command
	{
		//Check if a task is currently being tracked (already open)
		int alreadyOpen;
		if(!alreadyOpen==0)//If no task is open, print error message and quit
		{
			printf("A task is not currently being tracked.  Please set task to be tracked");
			printf("\n\tTry: songoftime -s \"TaskName\"");
			exit(0);
		}
		
	}
	else if(reportFlag==1)//Report time statistics via command line
	{

	}

    
}
