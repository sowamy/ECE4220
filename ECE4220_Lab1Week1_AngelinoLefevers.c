/*		FILE: 			ECE4220_Lab1_AngelinoLefevers
 * 		DESCRIPTION: 
 *			- Lab 1 of ECE4220 (Real Time Operating Systems)
 *			- Contains 2 functions to be executed.
 *				-- The first alternatively turns on/off two LED's every second.
 *				-- The second produces a sound on the speaker
 *			- This program utilizes a Raspberry Pi 3 Unit and a custom auxillary board
 */
#include "stdio.h"
#include "wiringPi.h"

#define REDLED		2
#define YELLOWLED	3
#define GREENLED	4
#define BLUELED		5
#define PUSHBUTTON	16
// #define PUSHBUTTON 17
// #define PUSHBUTTON 18
// #define PUSHBUTTON 19
// #define PUSHBUTTON 20
#define SPEAKER		6


void alternateLights( int light1, int light2 );
void speaker( int milli );

int main( void ) {
	short on = 1;
	int state = 0;
	int frequency;

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

	pinMode(SPEAKER, OUTPUT);

	while(on) {
		switch(state) {
			case 0: // State 0: Main Menu - UX
				system("clear");
				printf(	"WELCOME TO 4220 LAB 1 DEMONSTRATION\n-ENGINEERED BY: ANGELINO LEFEVERS\n\nSELECT FUNCTIONALITY TO DEMONSTRATE:\n1. LED CONTROL\n2. SPEAKER SOUND\n9. EXIT\n--> ");
				scanf("%d", &state);
				break;
			case 1: // State 1: Demonstrate LED Control
				alternateLights(REDLED, YELLOWLED);
				break;
			case 2: // State 2: Speaker Demonstration
				speaker(50);
				break;
			// TEST CASES
			case 3: // TURN ON ALL LED's	
				digitalWrite(REDLED, HIGH);
				digitalWrite(YELLOWLED, HIGH);
				digitalWrite(GREENLED, HIGH);
				digitalWrite(BLUELED, HIGH);
				state = 0;
				break;
			case 4: // TURE OFF ALL LED's
				digitalWrite(REDLED, LOW);
				digitalWrite(YELLOWLED, LOW);
				digitalWrite(GREENLED, LOW);
				digitalWrite(BLUELED, LOW);
				state = 0;
				break;
			case 5: // Custom frequency to speaker
				printf( "FREQUENCY: " );
				scanf("%d", &frequency);
				speaker(frequency);
				break;
			default:
				system("clear");
				printf("ERROR TRY AGAIN");
				state = 0;
				break;
		} // END SWITCH : State Control
	} // END WHILE : Program On/Off

	return 0;
} // END FUNCTION main

/*	FUNCTION: alternateLights
 *	DESCRIPTION: Alternates between two lights (arguments) a second a piece
 */
void alternateLights( int light1, int light2 ) {
	short on = 1;
	
	while(on) {
		if(	digitalRead(PUSHBUTTON) ) { on = 0; }	
	} // END WHILE : Reads value from pushbutton

	while(1) {
		digitalWrite(light1, HIGH);
		digitalWrite(light2, LOW);
		sleep(1);
		digitalWrite(light1, LOW);
		digitalWrite(light2, HIGH);
		sleep(1);
	} // END WHILE : Infinite Loop
} // END FUNCTION alternateLights

void speaker( int milli ) {
	while(1) {
		digitalWrite(SPEAKER, HIGH);
		usleep(milli);
		digitalWrite(SPEAKER, LOW);
		usleep(milli);
	} // END WHILE : Infinite Loop
} // END FUNCTION speaker
