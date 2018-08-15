#include "osm.h"
#include "sys/time.h"
#include "stdlib.h"
#include "unistd.h"
#include "math.h"
#include "stdio.h"


/*
    @var char DEFAULT_IT_AMOUNT
    @brief set the default amount of iterations in case of bad argument
    */
const unsigned int DEFAULT_IT_AMOUNT = 1000;

/*
    @var char DEAFULT_NAME_LENGTH
    @brief set the default length of the array used for storing the machine name 
    */
const unsigned int DEAFULT_NAME_LENGTH = 256;

/*
    @var char CAL_ERR
    @brief set the error value to be -1, in case of unsuccessful time measure
    */
const int CAL_ERR = -1;

/*
    @var int LOOP_UNR_OP
    @brief amount of operation in each loop
    */
const int LOOP_UNR_OP = 5 ; 



// global values for the library 
char* host_name = (char*) malloc(DEAFULT_NAME_LENGTH);

/* Help func that receives time measurment as a double, and in case its smaller
   than 0 it returns -1 otherwise it'll return the time measurment unchanged
   */ 
double calCheck(double cal)
{
	if(cal < 0)
	{
		return CAL_ERR;
	}
	return cal;
}


/* Initialization function that the user must call
   before running any other library function.
   The function may, for example, allocate memory or
   create/open files.
   Returns 0 upon success and -1 on failure
   */
int osm_init()
{
	int i;
	i = gethostname(host_name,DEAFULT_NAME_LENGTH);
	if(i)
	{
		host_name[0] = '\0';
	}
	return 0;
}

/* finalizer function that the user must call
 * after running any other library function.
 * The function may, for example, free memory or
 * close/delete files.
 * Returns 0 upon success and -1 on failure
 */
int osm_finalizer()
{
	free(host_name);
	return 0;
}

/* Empty function call, used to measure the time for calling a function
   */
void operation()
{
		
}

/* Time measurement function for an empty trap into the operating system.
   returns time in nano-seconds upon success, 
   and -1 upon failure.
   */
double osm_syscall_time(unsigned int iterations)
{
	struct timeval start, end, diff;
	int fail;
	fail = gettimeofday(&start, NULL);	
	if(fail)
	{
		return CAL_ERR;
	}
	for(unsigned int i = 0; i < iterations; i+=LOOP_UNR_OP)
	{
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;
		OSM_NULLSYSCALL;

	}
	fail = gettimeofday(&end, NULL);	
	if(fail)
	{
		return CAL_ERR;
	}
	timersub(&end, &start, &diff); 
	return (double)((diff.tv_usec * 1000 + diff.tv_sec * pow(10,9)) / (iterations));
}

/* Time measurement function for an empty function call.
   returns time in nano-seconds upon success, 
   and -1 upon failure.
   */
double osm_function_time(unsigned int iterations)
{
	struct timeval start, end, diff;
	int fail;
	fail = gettimeofday(&start, NULL);	
	if(fail)
	{
		return CAL_ERR;
	}
	for(unsigned int i = 0; i < iterations; i+=LOOP_UNR_OP)
	{
		operation();
		operation();
		operation();	
		operation();
		operation();

	}
	fail = gettimeofday(&end, NULL);	
	if(fail)
	{
		return CAL_ERR;
	}
	timersub(&end, &start, &diff); 
	return (double)((diff.tv_usec * 1000 + diff.tv_sec * pow(10,9)) / (iterations));
}

/* Time measurement function for a simple arithmetic operation.
   returns time in nano-seconds upon success,
   and -1 upon failure.
   */
double osm_operation_time(unsigned int iterations)
{
	struct timeval start, end, diff;
	int y = 0;	
	int fail;
	fail = gettimeofday(&start, NULL);	
	if(fail)
	{
		return CAL_ERR;
	}
	
	for(unsigned int i = 0; i < iterations; i+=LOOP_UNR_OP)
	{
		y = 1 & 2;
		y = 1 & 2;
		y = 1 & 2;
		y = 1 & 2;
		y = 1 & 2;
	}
	fail = gettimeofday(&end, NULL);	
	if(fail)
	{
		return CAL_ERR;
	}
	y++; 
	// this line was added to avoid the compilers warning about unused value
	timersub(&end, &start, &diff); 
	return (double)((diff.tv_usec * 1000 + diff.tv_sec * pow(10,9)) / (iterations));
}

/* Time measurement function for accessing the disk.
   returns time in nano-seconds upon success, 
   and -1 upon failure.
   */
double osm_disk_time(unsigned int iterations)
{
	return -1;
}

/* Helper function that receives an unsigned int, and returns the argument 
   in case its not 0, otherwise it'll return the default iteration amount
   */
int checkArg(unsigned int num)
{
	// the only invalid argument is 0
	if(num)
	{
		return num;
	}
	else
	{
		return DEFAULT_IT_AMOUNT;
	}
}

/* Function that receives 4 arguments which are the amount of iterations 
   that each time measurement function will do, the function return a struct
   of type timeMeasurmentStructure which contains all the result for all the
   measurements, the name of the machine that the measurements took place, 
   and ratios between a single instruction to: func call, trap call
   and hd accesses.
   The user has to release the memory for machine name on his own at
   the end of the use.
*/
timeMeasurmentStructure measureTimes (unsigned int operation_iterations,
				      unsigned int function_iterations,
				      unsigned int syscall_iterations,
				      unsigned int disk_iterations)
{
	timeMeasurmentStructure timesStruct;
	timesStruct.machineName = host_name;
	
	// checking that all the arguments aren't 0 in case they are, 
	// the argument changed to 1000
	operation_iterations = checkArg(operation_iterations);
	function_iterations = checkArg(function_iterations);
	syscall_iterations = checkArg(syscall_iterations);
	disk_iterations = checkArg(disk_iterations);
	//measuring the times
	timesStruct.instructionTimeNanoSecond = 
 					osm_operation_time(operation_iterations);
	timesStruct.functionTimeNanoSecond = 
					osm_function_time(function_iterations);
	timesStruct.trapTimeNanoSecond = osm_syscall_time(syscall_iterations);
	timesStruct.diskTimeNanoSecond = osm_disk_time(disk_iterations);
	//calculating the ratios 
	timesStruct.functionInstructionRatio = calCheck(timesStruct.functionTimeNanoSecond / 
					       timesStruct.instructionTimeNanoSecond);
	timesStruct.trapInstructionRatio = calCheck(timesStruct.trapTimeNanoSecond / 
					   timesStruct.instructionTimeNanoSecond);
	timesStruct.diskInstructionRatio = calCheck(timesStruct.diskTimeNanoSecond / 
					   timesStruct.instructionTimeNanoSecond);				   
	return timesStruct;
}




