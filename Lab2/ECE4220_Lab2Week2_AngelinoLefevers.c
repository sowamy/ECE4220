/*	FILE: ECE4220_Lab2Week2_AngelinoLefevers.c
 * 	DESCRIPTION: 
 *	- This program uses linux scheduling to coordinate three threads to recreate a message
 * 		whose components are intermixed between two text files.
 *	- The first and second threads:
 *		-- Each accesses files independently to retrieve their contents line by line
 *		-- Writes their contents to the buffer for the third thread to access and handle
 *	- The third thread:
 *		-- Accesses and gets data from the buffer
 *		-- Prints the data to the screen
 *	COMPILE: gcc ECE4220_Lab2Week2_AngelinoLefevers.c -lpthread
 *
 *	AUTHOR: Angelino Lefevers
 *	CLASS: ECE 4220 - Real-Time Operating Systems
 */
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>
#include <sched.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/timerfd.h>
#include <time.h>

#define PRIORITY 100
#define PERIOD 0	// Period in seconds
#define NPERIOD 400000	// Period in nanoseconds
#define STARTTIME 0
#define NSTARTTIME 500000

// Sets the periods in both micro and in seconds
long period12 = PERIOD;		// Period for reading files line by line
long period3 = PERIOD/2;	// Period for reading from the buffer and printing to the screen
long nperiod12 = NPERIOD;
long nperiod3 = NPERIOD/2;

// Sets the start times of each process
long start1 = STARTTIME;
long start2 = STARTTIME + PERIOD/2;
long start3 = STARTTIME + PERIOD/4;
long nstart1 = NSTARTTIME;
long nstart2 = NSTARTTIME + NPERIOD/2;
long nstart3 = NSTARTTIME + NPERIOD/4;

char buffer[256];
char* bufferPtr = &buffer[0];
size_t bufferSize = 256;

void thread1( void *ptr ) 
{
	// Opening file
	FILE* fp;
	fp = fopen("./Data/first.txt", "r");
	if(fp == NULL ) { puts("FILE ERROR, THREAD 1"); }

	// Sets up the scheduler and its scheme for the process
	struct sched_param param;					// Structure to hold settings that the scheduler is to be set at
	param.sched_priority = PRIORITY;			// Sets the priority of the process in the kernal operating system
	sched_setscheduler(0, SCHED_FIFO, &param);	// Sets the algorithm in which calculates the priority of each process, and the order in which it is ran

	// Creates a new timer object
	int timer_fd = timerfd_create(CLOCK_REALTIME, 0);	// How the clock which coordinates the processes will react
	
	// Sets up variables to interact with timer
	struct itimerspec itval;		// Structure to hold and send timing data
	struct itimerspec *a = &itval;	// Used to interact with the timing library and the timer of the kernal

	// Sets the initial value of the timer	
	itval.it_value.tv_sec = start1;
	itval.it_value.tv_nsec = nstart1;

	// Sets the period of the timer
	itval.it_interval.tv_sec = period12;
	itval.it_interval.tv_nsec = nperiod12;
	
	// Sets up the timer in th NULLe program, which will be used to coordinate processes
	timerfd_settime(timer_fd, 0, &itval, NULL);

	// Used to find if the execution of the task passes the period window of operation
	uint64_t num_periods = 0;

	// Primary Loop: Used to continuously run the primary purpose of the program
	while(1) {
		// Attempts to read how much is left of the process(file descriptor) to execute
		// Returns 0 if the execution time surpasses the given period
		read(timer_fd, &num_periods, sizeof(num_periods));

		// Throws and error and ends the process if the execution of the process surpasses a given period
		if( num_periods > 1 ) {
			puts("Missed Window");
			exit(1);
		} // END IF: Checks if process missed its deadline

		// ------------ CORE FUNCTIONALITY -------------
		getline(&bufferPtr, &bufferSize, fp);
	} // END WHILE : infinite loop
} // END FUNCTION thread1

void thread2( void *ptr ) {
	// Open File
	FILE* fp;
	fp = fopen("./Data/second.txt", "r");
	if(fp == NULL) { puts("FILE ERROR, THREAD 2"); }
	
	// Sets up the scheduler and its scheme for the process
	struct sched_param param;					// Structure to hold settings that the scheduler is to be set at
	param.sched_priority = PRIORITY;			// Sets the priority of the process in the kernal operating system
	sched_setscheduler(0, SCHED_FIFO, &param);	// Sets the algorithm in which calculates the priority of each process, and the order in which it is ran

	// Creates a new timer object
	int timer_fd = timerfd_create(CLOCK_REALTIME, 0);	// How the clock which coordinates the processes will react
	
	// Sets up variables to interact with timer
	struct itimerspec itval;		// Structure to hold and send timing data
	struct itimerspec *a = &itval;	// Used to interact with the timing library and the timer of the kernal

	// Sets the initial value (start time) of the timer	
	itval.it_value.tv_sec = start2;
	itval.it_value.tv_nsec = nstart2;

	// Sets the period of the timer
	itval.it_interval.tv_sec = period12;
	itval.it_interval.tv_nsec = nperiod12;
	
	// Sets up the timer in the program, which will be used to coordinate processes
	timerfd_settime(timer_fd, 0, &itval, NULL);

	// Used to find if the execution of the task passes the period window of operation
	uint64_t num_periods = 0;

	// Primary Loop: Used to continuously run the primary purpose of the program
	while(1) {
		// Attempts to read how much is left of the process(file descriptor) to execute
		// Returns 0 if the execution time surpasses the given period
		read(timer_fd, &num_periods, sizeof(num_periods));


		// Throws and error and ends the process if the execution of the process surpasses a given period
		if( num_periods > 1 ) {
			puts("Missed Window");
			exit(1);
		} // END IF: Checks if process missed its deadline

		// ------------ CORE FUNCTIONALITY -------------
		getline(&bufferPtr, &bufferSize, fp);
	} // END WHILE : infinite loop
} // END FUNCTION thread2

void thread3( void *ptr )
{
	// Sets up the scheduler and its scheme for the process
	struct sched_param param;					// Structure to hold settings that the schedu
	param.sched_priority = PRIORITY;
	sched_setscheduler(0, SCHED_FIFO, &param);

	int timer_fd = timerfd_create(CLOCK_REALTIME, 0);

	struct itimerspec itval;
	struct itimerspec *a = &itval;

	itval.it_value.tv_sec = start3;
	itval.it_value.tv_nsec = nstart3;

	// Sets the period of the timer
	itval.it_interval.tv_sec = period3;
	itval.it_interval.tv_nsec = nperiod3;
	
	// Sets up the timer in the program, which will be used to coordinate processes
	timerfd_settime(timer_fd, 0, &itval, NULL);

	// Used to find if the execution of the task passes the period window of operation
	uint64_t num_periods = 0;

	// Primary Loop: Used to continuously run the primary purpose of the program
	while(1) {
		// Attempts to read how much is left of the process(file descriptor) to execute
		// Returns 0 if the execution time surpasses the given period
		read(timer_fd, &num_periods, sizeof(num_periods));

		// Throws and error and ends the process if the execution of the process surpasses a given period
		if( num_periods > 1 ) {
			puts("Missed Window");
			exit(1);
		} // END IF: Checks if process missed its deadline

		// ------------ CORE FUNCTIONALITY -------------
		printf("%s", buffer);
	} // END WHILE : infinite loop
} // END FUNCTION thread3

int main( void ) 
{
	pthread_t t1, t2, t3;

	pthread_create(&t1, NULL, (void*) &thread1, NULL);
	pthread_create(&t2, NULL, (void*) &thread2, NULL);
	pthread_create(&t3, NULL, (void*) &thread3, NULL);

	while(1) {

	} // END WHILE : infinite loop
	return (0);
} // END FUNCTION main
