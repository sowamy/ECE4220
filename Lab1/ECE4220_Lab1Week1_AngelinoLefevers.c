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
#define PUSHBUTTON1 17
#define PUSHBUTTON2 18
#define PUSHBUTTON3 19
#define PUSHBUTTON4 20
#define SPEAKER		6


void alternateLights( int light1, int light2 );
void speaker( int milli);
int buttonControl( void );

int main( void ) {
	short on = 1;
	int button = 0;
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

	pinMode(PUSHBUTTON1, INPUT);
	pullUpDnControl(PUSHBUTTON1, PUD_DOWN);

	pinMode(PUSHBUTTON2, INPUT);
	pullUpDnControl(PUSHBUTTON2, PUD_DOWN);

	pinMode(PUSHBUTTON3, INPUT);
	pullUpDnControl(PUSHBUTTON3, PUD_DOWN);

	pinMode(PUSHBUTTON4, INPUT);
	pullUpDnControl(PUSHBUTTON4, PUD_DOWN);

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
	
	buttonControl();

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
	int on = 1;

	buttonControl();

	while(1) {
		digitalWrite(SPEAKER, HIGH);
		usleep(milli);
		digitalWrite(SPEAKER, LOW);

		usleep(milli);
	} // END WHILE : Infinite Loop
} // END FUNCTION speaker

int buttonControl() {
	int button;

	printf("Pick button (1-5): ");
	scanf( "%d", &button );

	while(1) {
		switch( button ) {
			case 1:
				if( digitalRead( PUSHBUTTON ) ) { return 0; }
				break;
			case 2:
				if( digitalRead( PUSHBUTTON1 ) ) { return 0; }
				break;
			case 3:
				if( digitalRead( PUSHBUTTON2 ) ) { return 0; }
				break;
			case 4:
				if( digitalRead( PUSHBUTTON3 ) ) { return 0; }
				break;

			case 5:
				if( digitalRead( PUSHBUTTON4 )) {return 0; }
				break;
		};
	}
}
