#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctime>
#include <string.h>
#include <sys/types.h>
#include <pwd.h>
#include <fstream>
//This list of includes is getting a little out of
//hand for such a small project...should clean
//these up when I'm finished.

//Function declarations
int isOpenTask();
char* createCurString(char*);
char* getFilePath(char *fName);
int isOpenTask();
int openTask(char *taskName);
int closeTask();
int createBaseHTMLFile();
int checkExists(char *filePath);
int updateHTMLFile();
int updateAllTasksFile(char *taskName, char *curStartMillis, char *lastStartDate);

//Stuct to hold Task objects
typedef struct
{
        char dateString[512];
        char taskString[512];
        long millisWorked;
} Task;


int main(int argc, char *argv[ ])
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
		int alreadyOpen=isOpenTask();
		if(alreadyOpen==1)//Print error message and quit
		{
			printf("\nA task is already being tracked.  Please close task before continuing");
			printf("\n\tTry: songoftime -f");
			exit(0);
		}
		
		if(startTitle==NULL)
		{
			printf("\nA task must be entered for tracking.");
			printf("\n\tTry: songoftime -s \"TaskName\"");
		}

		openTask(startTitle);
		
		
	}
	else if(finishFlag==1)//Task finish command
	{
		//Check if a task is currently being tracked (already open)
		int alreadyOpen=isOpenTask();
		if(alreadyOpen==0)//If no task is open, print error message and quit
		{
			printf("A task is not currently being tracked.  Please set task to be tracked");
			printf("\n\tTry: songoftime -s \"TaskName\"");
			exit(0);
		}

		closeTask();
		
	}
	else if(reportFlag==1)//Report time statistics via command line
	{

	}

}

//Checks if a given filepath exists already or not
//Retval: int
//	0- if specified file doesnt exist.
//	1- if specified file does exist
int checkExists(char *fPath)
{
	if(fPath==NULL)
		return 0;

	std::ifstream inFile(fPath);
	if(inFile.good() )
	{
		return 1;
	}
	else
	{
		return 0;
	}
}


//Checks if a task is currently open
//Retval: int
//	1- if task is open
//	0- if no task is open
int isOpenTask()
{
	char *filePath=getFilePath(".curTask.sot");
	std::ifstream curTaskFile(filePath);
	if(curTaskFile.good())
	{
		return(1);
	}
	else
	{
		return(0);
	}

	curTaskFile.close();
}

//Does work of writing to task file
//char * taskName : String containing task to close in file
//Retval: int
//	0- on success
//	1- on file found error
//	2- on file write/read permissions error
//	3- on file formatting exception.
int openTask(char *taskName)
{
	//Get filepath
	char *filePath=getFilePath(".curTask.sot");

	//Create ofile object
	std::ofstream outFile(filePath);

	//Return error code if unable to open file
	if(!outFile)
		return(1);

	//Get string to write
	char *curString=createCurString(taskName);
	if(curString==NULL)
		return(2);

	outFile << curString;
	return 0;
}

//Does work of closing task in file, and updating statistics
//Retval: int
//	0- on success
//	1- on file found error
//	2- on file read/write permissions error
//	3- on file formatting exception
int closeTask()
{
	//Create read buffer
	char *readBuffer=(char *)malloc(512*sizeof(char));
	if(readBuffer==NULL)
	{
		return(2);
	}

	//Get filepath of curTask.sot file, open reader
	char *filePath=getFilePath(".curTask.sot");
	std::ifstream inFile(filePath);	
	if(inFile.good() )
	{
		inFile.getline(readBuffer,512);//Reads curTask.sot into buff
		inFile.close();
		remove(filePath);//Delete the file
	}
	else//File was not found, or was empty
	{
		free(readBuffer);
		inFile.close();
		return(1);
	}

	//split read buffer around tabs to get tokens
	char *name=strtok(readBuffer,"\t\n");
	char *millis=strtok(NULL,"\t\n");
	char *date=strtok(NULL,"\t\n");
	
	//Update .allTasks.sot file
	updateAllTasksFile(name,millis,date);
	//Update .timeSong.html file
	updateHTMLFile();
	
	

	free(readBuffer);
	return -1;
}

//Updates the .timeSong.html file
//Retval: int
//	0- on success
//	1- on File I/O Error
int updateHTMLFile()
{
	return -1;
}


//Updates the .allTasks.sot, which holds binary structs of each task
//Retval: int
//	0- on success
//	1- on File I/O Error
int updateAllTasksFile(char *taskName, char *curStartMillis, char *lastStartDate)
{
	return -1;
}

//Does work of writing base HTML code for timeSong.thml
//Retval: int
//	0- on success
//	1- on file write permissions error
int createBaseHTMLFile()
{
	return -1;
}

//Writes string to provided pointer position
//Provided string looks like: "TaskName       TimeOpenedInMillis      DateStamp"
//Retval: char *
//	valid ptr- on success
//	null- on null or bad argument
char* createCurString(char *taskStringPtr)
{
	//Check for bad error conditions
	if(taskStringPtr==NULL)
	{
		return NULL;
	}

	//String length needed is:
	//	 lengthof(taskName) + 1(tab delimit) + approx size of%ld + 1(tab delimit)+10(YYYY-MM-DD)+2 (\n, \0)
	int lengthNeeded=strlen(taskStringPtr)+1+11+1+10+2+5;
	char *curString=(char *)malloc(lengthNeeded*sizeof(char));

	//Check malloc return value
	if(curString==NULL)
		return NULL;
	
	strcpy(curString,taskStringPtr);//Copy in the taskName
	strcat(curString,"\t");//Add tab delimit

	//Get time() value into string
	time_t curTime=time(0);
	char *timeValue=(char *)malloc(sizeof(char)*13);
	if(timeValue==NULL)
	{
		free(curString);
		return NULL;
	}
	sprintf(timeValue,"%ld\0",curTime);
	strcat(curString,timeValue);

	strcat(curString,"\t");//Add second tab delimit

	//Add datestring
	char *dateString=(char *)malloc(11*sizeof(char));
	if(dateString==NULL)
	{
		free(timeValue);
		free(curString);
		return NULL;
	}
	struct tm * now = localtime( & curTime );
	sprintf(dateString,"%d-%d-%d",now->tm_year+1900,now->tm_mon+1,now->tm_mday);
	strcat(curString,dateString);

	strcat(curString,"\n\0");//Add newline and string ender
	
	//Free temp buffers
	free(timeValue);
	free(dateString);

	return curString;
}

//Gets homedirectory of user, and appends the provided name to give a full filepath
//Retval: ptr
//	NULL- on malloc failure
//	Valid Ptr- on success
char* getFilePath(char *fName)
{
	if(fName==NULL)
		return NULL;

	struct passwd *pw=getpwuid(getuid());
	const char *homedir=pw->pw_dir;

	//Create buffer for filePath
	char *filePath=(char *)malloc((strlen(homedir)+strlen(fName)+2)*sizeof(char));
	if(filePath==NULL)
	{
		return NULL;
	}

	strcpy(filePath,homedir);
	strcat(filePath,"/");
	strcat(filePath,fName);

	return filePath;
}
