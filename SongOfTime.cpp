#include <unistd.h>
#include <stdio.h>

int
main(int argc, char *argv[ ])
{
    	int c;
    	int errFlag=0;
	int startFlag=0;
	int finishFlag=0;

    	char *startTitle=NULL;

    	extern char *optarg;
    	extern int optind, optopt;

    	while ((c = getopt(argc, argv, "s:f?")) != -1) 
	{
        	switch(c) 
		{
        		case 's':
            			startTitle = optarg;
				startFlag=1;
            			break;
        		case 'f':
            			finishFlag=1;
            			break;
            		case ':':       /* -f or -o without operand */
                    		errFlag=1;
                    		break;
        		case '?':
            			errFlag=1;
				break;
        	}
    	}

	if(errFlag==1 || (startFlag==1 && finishFlag==1) || (startFlag==0 && finishFlag==0))
	{
		printf("\nImproper arguments, Try: SongOfTime [-s TaskName] [-f]");
	}

    
}
