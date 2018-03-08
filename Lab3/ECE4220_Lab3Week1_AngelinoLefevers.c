/*	FILE: ECE4220_Lab3Week1_AngelinoLefevers.c
 *	DESCRIPTION:
 *		- Uses a Raspberry Pi 3 unit with a custom auxillary board.
 *		- Controls a lighting systems for a two lane road which funnels into a single lane road with a crosswalk.
 *		- There are two lights which control traffic from right to left or vice versa.
 *		- The pushbutton utilized in this laboratory turns on the pedestrian walkway light before the light switches.
 *
 *	AUTHOR: Angelino Lefevers
 */
#include <stdio.h>
#include <time.h>
#include <pthread.h>
#include <sched.h>
#include <wiringPi.h>

#define REDLED		2
#define YELLOWLED	3
#define GREENLED	4
#define BLUELED		5

#define PUSHBUTTON	16

#define LIGHTTIME	100000
#define PEDTIME		200000

#define PRIORITY 51

int pedLight = 0;

void buttonPress(void* x);
void clear_button(void);
void streetLight(int light);
void pedestrianLight(void);

int main( void )
{
	pthread_t bt;

	wiringPiSetupGpio();

	pinMode(REDLED, OUTPUT);
	pinMode(YELLOWLED, OUTPUT);
	pinMode(GREENLED, OUTPUT);
	pinMode(BLUELED, OUTPUT);

	digitalWrite(REDLED, LOW);
	digitalWrite(YELLOWLED, LOW);
	digitalWrite(GREENLED, LOW);
	digitalWrite(BLUELED, LOW);

	pinMode(PUSHBUTTON, INPUT);
	pullUpDnControl(PUSHBUTTON, PUD_DOWN);

	pthread_create(&bt, NULL, (void*)buttonPress, NULL);

	while(1) {
		if( pedLight == 1 ) { pedestrianLight(); }
		streetLight(REDLED);
		if( pedLight == 1 ) { pedestrianLight(); }
		streetLight(YELLOWLED);
		// Turn on light 1
		
	} // END WHILE : Infinite Loop
} // END FUNCTION main

void buttonPress(void* x)
{
	// Set priority
	struct sched_param param;
	param.sched_priority = PRIORITY;
	sched_setscheduler(0, SCHED_FIFO, &param);

	while(1) {
		if( digitalRead(PUSHBUTTON) ) { pedLight = 1; }
		usleep(10);
	} // END WHILE: Infinite Loop
} // END FUNCTION butonPress

void clear_button(void) { pedLight = 0; }

void streetLight(int light)
{
	digitalWrite(light, HIGH);
	usleep(LIGHTTIME);
	digitalWrite(light, LOW);
} // END FUNCTION streetLight

void pedestrianLight(void)
{
	digitalWrite(GREENLED, HIGH);
	usleep(PEDTIME);
	digitalWrite(GREENLED, LOW);
	pedLight = 0;
} // END FUNCTION pedLight
