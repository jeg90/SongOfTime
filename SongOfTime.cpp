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

//Stuct to hold Task objects
typedef struct
{
        char dateString[512];
        char taskString[512];
        long millisWorked;
} Task;

//Function declarations
char* createCurString(char*);
char* getFilePath(char *fName);
int openTask(char *taskName);
int closeTask();
int createBaseHTMLFile();
int checkExists(char *filePath);
int updateHTMLFile();
int updateAllTasksFile(char *taskName, char *curStartMillis, char *lastStartDate);
void listTasks();
int readTasksToBuffer(Task **, int *);
void reportTasks();



/////////////////////////////////////////////////
//Begin main execution
/////////////////////////////////////////////////
int main(int argc, char *argv[ ])
{
	//Used for reading retval from getopt
    	int c;

	//Create the different mode flags
    	int errFlag=0;
	int startFlag=0;
	int finishFlag=0;
	int reportFlag=0;
	int listFlag=0;

	//Start Title 
    	char *startTitle=NULL;

	//getopt flags and variables
    	extern char *optarg;
    	extern int optind, optopt;

	//Iterate through command line options to set mode flags
    	while ((c = getopt(argc, argv, "s:frl?")) != -1) 
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
			case 'l':
				listFlag=1;
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
	if(errFlag==1 || (startFlag==1 && finishFlag==1) || (startFlag==0 && finishFlag==0 && listFlag==0 && reportFlag==0))
	{
		printf("\nImproper arguments, Try: songoftime [-s TaskName] [-f] [-r]\n");
		exit(0);
	}
	else if(startFlag==1)//Start command
	{
		//check if a task is already being tracked- (already open)
		int alreadyOpen=checkExists(getFilePath((char *)".curTask.sot"));
		if(alreadyOpen==1)//Print error message and quit
		{
			printf("\nA task is already being tracked.  Please close task before continuing");
			printf("\n\tTry: songoftime -f\n");
			exit(0);
		}
		
		if(startTitle==NULL)
		{
			printf("\nA task must be entered for tracking.");
			printf("\n\tTry: songoftime -s \"TaskName\"\n");
			exit(0);
		}

		openTask(startTitle);
		
		
	}
	else if(finishFlag==1)//Task finish command
	{
		//Check if a task is currently being tracked (already open)
		int alreadyOpen=checkExists(getFilePath((char *)".curTask.sot"));
		if(alreadyOpen==0)//If no task is open, print error message and quit
		{
			printf("A task is not currently being tracked.  Please set task to be tracked");
			printf("\n\tTry: songoftime -s \"TaskName\"\n");
			exit(0);
		}

		closeTask();
		
	}
	else if(reportFlag==1)//Report time statistics via command line
	{
		reportTasks();
		exit(0);
	}
	else if(listFlag==1)//List all tasks in history via command line
	{
		listTasks();
		exit(0);
	}

}//End of main
////////////////////////////////////////////////////////////////
//End main execution
////////////////////////////////////////////////////////////////






/////////////////////////////////////////////////////////////////
//BEGIN FEATURE FUNCTIONS
//	Called from main to do work
//	of implementing single feature
/////////////////////////////////////////////////////////////////

//Called for -s option
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
	char *filePath=getFilePath((char *)".curTask.sot");

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


//Called for -f option
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
	char *filePath=getFilePath((char *)".curTask.sot");
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
	return 0;
}

//Called for -l option
//Prints tasks to console
//Retval: void
void listTasks()
{
	//Check if .allTasks.sot exists
	//If it does()
	//	read from file
	//if it doesnt
		//print that no tasks are present

	//Check if .allTasks.sot exists
	int allTasksExists=checkExists( getFilePath((char *)".allTasks.sot") );

	//If it does
	if(allTasksExists)
	{
		int numTasks=0;
		Task *taskBuffer;
		readTasksToBuffer(&taskBuffer,&numTasks);
		int i=0;
		for(i=0;i<numTasks;i++)
		{
			printf("\t%s\n",taskBuffer[i].taskString);
		}
		free(taskBuffer);
	}
	else//If it doesn't
	{
		//Print that no tasks are present
		printf("No tasks found in current history");
	}
}
//Called for -r option
//Prints task statistics to console
//Retval: void
void reportTasks()
{
	//Check if .allTasks.sot exists
	//If it does()
	//	read from file
	//if it doesnt
	//	print that no tasks are present



	//Check if .allTasks.sot exists
	int allTasksExists=checkExists( getFilePath((char *)".allTasks.sot") );

	//If it does
	if(allTasksExists)
	{
		int numTasks=0;
		Task *taskBuffer;
		readTasksToBuffer(&taskBuffer,&numTasks);
		int i=0;
		for(i=0;i<numTasks;i++)
		{
	                int hWorked=taskBuffer[i].millisWorked/60/60;
        	        int mWorked=(taskBuffer[i].millisWorked-hWorked*60*60)/60;
			printf("\t%s-\t%d hours, %d minutes\n",taskBuffer[i].taskString,hWorked,mWorked);
		}
		free(taskBuffer);
	}
	else//If it doesn't
	{
		//Print that no tasks are present
		printf("No tasks found in current history");
	}
}

//////////////////////////////////////////////////////////////
//End FEATURE FUNCTIONS
//////////////////////////////////////////////////////////////





//////////////////////////////////////////////////////////////
//Begin Utility functions
//	Used for checking files,
//	basic bool routines etc.
//////////////////////////////////////////////////////////////

//Reads Task structs into a buffer
//Retval: int
//	0- successful execution
//	-1-.allTasks.sot file doesn't exists
//	-2-Buffer allocation error (failed malloc)
//	-3-File I/O Problems
//	-4-Invalid argument(s)
int readTasksToBuffer(Task** buffer,int *numTasks)
{
	if(numTasks==NULL || buffer==NULL)
	{
		return(-4);
	}

	//Return error if file doesn't exist
	if( checkExists(getFilePath((char *)".allTasks.sot"))==0 )
	{
		return(-1);
	}
        

	//Open file
	char *allTasksPath=getFilePath((char *)".allTasks.sot");
        std::ifstream taskFile(allTasksPath);

	if(!taskFile.good())
	{
		return (-3);
	}
	
        //Get number of structs
        taskFile.read((char *)numTasks,sizeof(int));

        ///////////////////////////////////////////
        //Read in structs from file
        *buffer=(Task *) malloc( (*numTasks)*sizeof(Task) );
        int i=0;
        for(i=0;i<(*numTasks);i++)
        {
        	taskFile.read((char *)( (*buffer)+i),sizeof(Task));
        }
        taskFile.close();

	//Return 0 for success.
	return 0;
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


//Updates the .timeSong.html file
//Retval: int
//	0- on success
//	1- on File I/O Error
int updateHTMLFile()
{
	//Create file handle and buffer
	std::ofstream htmlFileWriter(getFilePath((char *)".timeSong.html"));
	char writeBuffer[512];
	int numStructs;

	//Copy in basic header html information
	strcpy(writeBuffer,"<html><head></head><body><h2>Recent Tasks and Times</h2>");
	htmlFileWriter.write(writeBuffer,strlen(writeBuffer));

	/////////////////////////////////////////
	//Read in struct information
        std::ifstream taskFile(getFilePath((char *)".allTasks.sot"));

        //Get number of structs
        taskFile.read((char *)&numStructs,sizeof(int));

        int taskExistsFlag=-1;
        Task taskArr[numStructs];
        int i=0;
        for(i=0;i<numStructs;i++)
        {
             taskFile.read((char *)(taskArr+i),sizeof(Task));
        }
        taskFile.close();
	/////////////////////////////////////////

	/////////////////////////////////////////
	//Write struct info to HTML file
	char *indivPrintBuffer=(char *)malloc(sizeof(char)*512);
	for(i=0;i<numStructs;i++)
	{
		//Get hours and minutes worked by taking advantage of a little integer division
		int hWorked=taskArr[i].millisWorked/60/60;
		int mWorked=(taskArr[i].millisWorked-hWorked*60*60)/60;
		sprintf(indivPrintBuffer,"Task: %s\tTime:%d hours, %d minutes\tLast Worked:  %s",taskArr[i].taskString,hWorked,mWorked,taskArr[i].dateString);
		
		strcpy(writeBuffer,"<p>");
		strcat(writeBuffer,indivPrintBuffer);
		strcat(writeBuffer,"</p>");
		htmlFileWriter.write(writeBuffer,strlen(writeBuffer));
	}
	/////////////////////////////////////////

	//Copy in finishing tag information
	strcpy(writeBuffer,"</body></html>");
	htmlFileWriter.write(writeBuffer,strlen(writeBuffer));
	htmlFileWriter.close();	
	return 0;
}


//Updates the .allTasks.sot, which holds binary structs of each task
//Retval: int
//	0- on success
//	1- on File I/O Error
int updateAllTasksFile(char *taskName, char *curStartMillis, char *lastStartDate)
{
	int numStructs=0;

	//Create new Task object
	Task newTask;
	strncpy(newTask.dateString,lastStartDate,512);
	strncpy(newTask.taskString,taskName,512);
	long curTime=time(0);
	long oldTime=atol(curStartMillis);
	newTask.millisWorked=(curTime-oldTime);

	//Get filepath and check if it exists
	char *allTasksPath=getFilePath((char *)".allTasks.sot");
	int allTasksExists=checkExists(allTasksPath);
	
	//If the file doesnt exists yet (this is our first task)
	if(allTasksExists==0)
	{
		//Open new file and write the cur number of tasks
		std::ofstream myFile(allTasksPath);
		numStructs=1;
		myFile.write((char *)&numStructs,sizeof(int));

		//Write struct object to file
		myFile.write((char *)&newTask,sizeof(Task));
		myFile.close();
		return 0;
	}
	else//the file already exists.  Must be read in (not our first task)
	{
		//Open file
		std::ifstream taskFile(allTasksPath);

		//Get number of structs
		taskFile.read((char *)&numStructs,sizeof(int));

		///////////////////////////////////////////
		//Read in struct objects from file
		int taskExistsFlag=-1;
		Task taskArr[numStructs+1];
		int i=0;
		for(i=0;i<numStructs;i++)
		{
			taskFile.read((char *)(taskArr+i),sizeof(Task));
			if(strcmp(taskArr[i].taskString,taskName)==0)
			{
				taskExistsFlag=i;
			}
		}
		taskFile.close();
		////////////////////////////////////////////


		////////////////////////////////////////////
		//Update struct information as appropriate
		//A matching task was not found in the array
		if(taskExistsFlag==-1)
		{
			strncpy(taskArr[numStructs].taskString,newTask.taskString,512);
			strncpy(taskArr[numStructs].dateString,newTask.dateString,512);
			taskArr[numStructs].millisWorked=newTask.millisWorked;
			numStructs++;
		}
		else//A matching task was found in the array at index taskExistsFlag
		{
			strncpy(taskArr[taskExistsFlag].dateString,newTask.dateString,512);
			taskArr[taskExistsFlag].millisWorked=taskArr[taskExistsFlag].millisWorked+newTask.millisWorked;
		}
		/////////////////////////////////////////////


		/////////////////////////////////////////////
		//Rewrite struct information back into file
		std::ofstream taskFileWriter(allTasksPath);
		taskFileWriter.write((char *)&numStructs,sizeof(int));//Write number of structs
		for(i=0;i<numStructs;i++)
		{
			taskFileWriter.write((char *)(taskArr+i),sizeof(Task));
		}
		taskFileWriter.close();
		/////////////////////////////////////////////
		return 0;		
	}
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
//////////////////////////////////////////////////////////////
//End Utility functions
//////////////////////////////////////////////////////////////
