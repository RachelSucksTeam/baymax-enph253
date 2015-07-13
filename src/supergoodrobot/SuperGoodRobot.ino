/* ENPH 253 PRELIMINARY CODE
last updated: 071315
version: 1.0

*/

//============== LIBRARIES ==============
#include <phys253.h>
#include <LiquidCrystal.h>
//#include <Servo253.h>
#include <avr/interrupt.h>

#include <EEPROMex.h>
#include <EEPROMVar.h>

#include "TAPE_FOLLOWING.h"

//============== PINS ==============
//ANALOG IO
#define TAPE_SENSOR_LEFT 0
#define TAPE_SENSOR_RIGHT 1
#define IR_FRONT_1 2
#define IR_FRONT_2 3
#define IR_SIDE 4
#define TAPE_SENSOR_SIDE 5
#define TAPE_SENSOR_FRONT 7

//SERVOS
#define SERVO_FOREARM 0
#define SERVO_BASEARM 1
#define SERVO_BASE 2

//MOTORS
#define MOTOR_LEFT 0
#define MOTOR_RIGHT 1
#define MOTOR_CLAW 2

//DIGITAL IO
//INPUT
#define SWITCH_CLAW_OPEN 4
#define SWITCH_CLAW_CLOSE 5
#define SWITCH_PLUSH_DETECT 6
#define HOOK_ARM_TOUCH_ON 7
#define HOOK_ARM_TOUCH_OFF 8

//OUTPUT
#define HOOK_ARM_RETRACT 15
#define HOOK_ARM_SPRING_UP 14
#define INTAKE_ROLLER_MOTOR 12

//============== DRIVE SETTINGS ==============
//QRD DRIVE
#define QRD_THRESHOLD 200

//IR DRIVE

//ARM CONSTANTS

//SERVO POSITIONS
#define BASESERVO_POSITION_START 90//some number
#define FOREARMSERVO_POSITION_START 90//some number
#define BASE_POSITION_START 90//some number

//FLAGS
boolean upRamp=false;
boolean sideTapePresent=false;
boolean clawOpen=false;
boolean previousSwitchVal=true;

#define KNOB_MAX 1023
#define STANDARD_DELAY_1 500 //ms
#define STANDARD_DELAY_2 200 //ms for faster things

//============== STATES ==============
enum RobotState {
	INITIALISING, PAST_DOOR, FOLLOW_TAPE_1, COLLECT_ITEM_1, FOLLOW_TAPE_2, COLLECT_ITEM_2, COLLECT_ITEM_3, COLLECT_ITEM_4, COLLECT_ITEM_5, COLLECT_ITEM_6, UP_RAMP, PAST_RAMP, IR, ZIPLINE, FINISHED, TEST,DRIVE, CLAW_ARM_TEST, MAIN_MENU, NUM_STATES
};

enum subMenu {
	STATE_MENU, TAPE_MENU, PLAN_MENU, NUM_SUBMENU, NO_SUBMENU
};

enum Plan { 
	FULL, NOT4, NOT5, TEST_DRIVE, TEST_CLAW, NUM_PLAN //placeholder for lack of len(enum) function, don't delete
};

//SET STATES
RobotState currentState = INITIALISING;
RobotState lastState = INITIALISING;  

subMenu currentSubMenu=NO_SUBMENU;

Plan currentPlan=FULL;

//============== INTERRUPTS ==============
/*volatile unsigned int INT_0 = 0;
ISR(INT0_vect) {LCD.clear(); LCD.home(); LCD.print("INT0: "); LCD.print(INT_0++);};
ISR(INT0_vect) {
  switchState(MAIN_MENU);
}*/

//============== SETUP ==============
void setup() {
	#include <phys253setup.txt>  
	portMode(0, INPUT) ;
	portMode(1, OUTPUT) ;

	LCD.clear();
	LCD.home();

	RCServo0.attach(RCServo0Output) ;
	RCServo1.attach(RCServo1Output) ;
	RCServo2.attach(RCServo2Output) ;

	//initiate servos
	setForearmServo(FOREARMSERVO_POSITION_START);
	setBasearmServo(BASESERVO_POSITION_START);
	setBaseServo(BASE_POSITION_START);
	//other stuff goes here

	//enableExternalInterrupt(INT0, RISING);  
}

void loop() {
//	if (digitalRead(0)!=previousSwitchVal) {
    if (stopbutton()) {
		currentSubMenu=NO_SUBMENU;
		switchState(MAIN_MENU);
//		previousSwitchVal=digitalRead(0);
	}

	switch(currentState) {
	//==============
	case INITIALISING:
		start_initialising(); //set plan
		break;
	//==============
	case FOLLOW_TAPE_1:
		while (sideTapePresent!=true) {
			readFollowTape_1();
			checkSideTape();
		}
		motor_rest();
		if (sideTapePresent==true) {
			switchState(COLLECT_ITEM_1);
		}
		break;
	//==============
	case COLLECT_ITEM_1:
		collect_item_1();
		while(sideTapePresent==true) { //go until you can't see sidetape anymore
			LCD.clear();
			LCD.print("Following sidetape");
			delay(STANDARD_DELAY_1);
			readFollowTape_1();
			checkSideTape();
		}
		if (sideTapePresent==false) {
		switchState(FOLLOW_TAPE_2);
		}
		break;
	//==============
	case FOLLOW_TAPE_2:
		while (sideTapePresent!=true) {
			readFollowTape_2();
			checkSideTape();
		}
		motor_rest();
		if (sideTapePresent==true) {
			switchState(COLLECT_ITEM_2);
		}
		break;
	//==============
	case COLLECT_ITEM_2:
		/*collect_item_2();
		while(sideTapePresent==true) { //go until you can't see sidetape anymore
			readFollowTape_2();
			checkSideTape();
		}
		switchState(FOLLOW_TAPE_3);*/
		break;
	//==============
	case COLLECT_ITEM_3:
	break;
	case COLLECT_ITEM_4:
	break;
	case COLLECT_ITEM_5:
	break;
	case COLLECT_ITEM_6:
	break;
	case UP_RAMP:
	break;
	case PAST_RAMP:
	break;
	case IR:
	break;
	case ZIPLINE:
	break;
	case FINISHED:
	break;
	case CLAW_ARM_TEST:
		claw_arm_test();
		break;
	case DRIVE:
		readFollowTape();
		break;
	case MAIN_MENU:
		if (currentSubMenu==NO_SUBMENU){
			mainMenu();
		}
		else {
			switch (currentSubMenu){
				case NO_SUBMENU:
					break;
				case STATE_MENU:
					stateMenu();
					break;
				case TAPE_MENU:
					tapeMenu();
					break;
				case PLAN_MENU:
					planMenu();
					break;
			}
		}
		break;
  }
}