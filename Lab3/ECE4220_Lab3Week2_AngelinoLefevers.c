/*	FILE: ECE4220_Lab3Week2_AngelinoLefevers.c
 *	DESCRIPTION:
 *		- Controls lighting system on a two lane road which funnels to a single lane road in the center with a pedestrian walkway.
 *		- Uses a pushbutton on a breakoff board to represent pedestrian crosswalk button.
 *		- Utilizes Raspberry Pi 3 unit with a custom auxillary board.
 *		- Uses functions from the library ece4220lab3
 *	TO COMPILE:
 *		gcc ECE4220_Lab3Week2_AngelinoLefevers.c -o L3W2 -lwiringPi -lpthread libece4220lab3.a
 *	TO RUN:
 *		sudo ./L3W2 <priority1> <priority2> <priority3>
 *		
 *		NOTE: Must have both ece4220lab3.h and libece4220lab3.a in the same directory with the kernal module ece4220lab3.ko installed 
 *
 *	AUTHOR: Angelino Lefevers
 */
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#include <wiringPi.h>

#include "ece4220lab3.h"

// Priorities
#define PRIORITY	51
#define PRIORITY1 	51
#define PRIORITY2	51
#define PRIORITY3	51

// Timing
#define LIGHTTIME	1000000 
#define PEDTIME 	1000000

// Wiring
#define REDLED		2
#define YELLOWLED	3
#define GREENLED	4
#define BLUELED		5
#define PUSHBUTTON1	16	

sem_t lock;
int pedLight = 0;
int p1 = PRIORITY1;
int p2 = PRIORITY2;
int p3 = PRIORITY3;

void* one(void* x);
void* two(void* x);
void* three(void* x);

int main( int argc, char* argv[] )
{
	// Set priorities using command line arguments - optional
	if(argc == 4) {
		p1 = atoi(argv[1]);
		p2 = atoi(argv[2]);
		p3 = atoi(argv[3]);
	} // END IF

	pthread_t os, light1, light2, pedestrian, checklight;
	wiringPiSetupGpio();

	pinMode(REDLED, OUTPUT);
	pinMode(YELLOWLED, OUTPUT);
	pinMode(GREENLED, OUTPUT);
	pinMode(BLUELED, OUTPUT);

	digitalWrite(REDLED, LOW);
	digitalWrite(YELLOWLED, LOW);
	digitalWrite(GREENLED, LOW);
	digitalWrite(BLUELED, LOW);

	pinMode(PUSHBUTTON1, INPUT);
	pullUpDnControl(PUSHBUTTON1, PUD_DOWN);

	sem_init(&lock, 0, 1);

	pthread_create(&light1, NULL, (void*)one, NULL);
	pthread_create(&light2, NULL, (void*)two, NULL);
	pthread_create(&pedestrian, NULL, (void*)three, NULL);
	pthread_create(&checklight, NULL, (void*)check_button, NULL);

	while(1) {} // Infinite Loop
	return 0;
} // END FUNCTION main

void* one(void* x)
{
	struct sched_param param;
	param.sched_priority = p1;
	sched_setscheduler(0, SCHED_FIFO, &param);

	while(1) {
		sem_wait(&lock);
		digitalWrite(REDLED, HIGH);
		usleep(LIGHTTIME);
		digitalWrite(REDLED, LOW);
		sem_post(&lock);
		usleep(100);
	} // END WHILE: Infinite Loop
} // END FUNCTION one

void* two(void* x)
{
	struct sched_param param;
	param.sched_priority = p2;
	sched_setscheduler(0, SCHED_FIFO, &param);

	while(1) {
		sem_wait(&lock);
		digitalWrite(YELLOWLED, HIGH);
		usleep(LIGHTTIME);
		digitalWrite(YELLOWLED, LOW);
		sem_post(&lock);
		usleep(100);
	} // END WHILE: Infinite Loop
} // END FUNCTION two

void* three(void* x)
{
	struct sched_param param;
	param.sched_priority = p3;
	sched_setscheduler(0, SCHED_FIFO, &param);

	while(1) {
		sem_wait(&lock);
		if( check_button() ) {
			digitalWrite(GREENLED, HIGH);
			usleep(PEDTIME);
			digitalWrite(GREENLED, LOW);
			clear_button();
		} // END IF : Check pedestrian light
		sem_post(&lock);
		usleep(100);
	} // END WHILE: Infinitie Loop
} // END FUNCTION three

