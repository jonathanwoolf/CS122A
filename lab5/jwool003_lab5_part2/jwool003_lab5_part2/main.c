/*
 * jwool003_lab5_part1.c
 *
 * Created: 10/23/2018 4:24:11 PM
 * Author : Jonathan
 */ 

#define timerPeriod 1
#define tasksNum 1

// ===Inputs===
#define PB0 (~PINB & 0x01)
#define PB1 (~PINB & 0x02)
#define PB2 (~PINB & 0x04)
#define PB3 (~PINB & 0x08)
#define PB4 (~PINB & 0x10)
#define PB5 (~PINB & 0x20)

#include <avr/io.h>
#include "scheduler.h"

unsigned char rotation[8] = {0x01, 0x03, 0x02, 0x06, 0x04, 0x0C, 0x08, 0x09};
	
enum stepper_states{stepper_start, clockwise_90, clockwise_180, clockwise_360, counterclockwise_90, counterclockwise_180, counterclockwise_360};

int stepper_tick(int state){
	
	static unsigned char i = 0;
	static unsigned numPhases = 0;
	switch(state){
		case stepper_start:
			numPhases = 0;
			PORTA = 0;
			if(PB0) { state = clockwise_90;}
			else if(PB1) { state = clockwise_180;}
			else if(PB2) { state = clockwise_360;}
			else if(PB3) { state = counterclockwise_90;}
			else if(PB4) { state = counterclockwise_180;}
			else if(PB5) { state = counterclockwise_360;}
			else { state = stepper_start;}
			break;
		case clockwise_90:
			PORTA = rotation[i];
			i++;
			numPhases++;
			if(i >= 7) { i = 0;}
			state = clockwise_90;
			if(numPhases >= 1024){ state = stepper_start;}
			break;
		case clockwise_180:
			PORTA = rotation[i];
			i++;
			numPhases++;
			if(i >= 7) { i = 0;}
			state = clockwise_180;
			if(numPhases >= 1950){ state = stepper_start;}
			break;
		case clockwise_360:
			PORTA = rotation[i];
			i++;
			numPhases++;
			if(i >= 7) { i = 0;}
			state = clockwise_360;
			if(numPhases >= 3650){ state = stepper_start;}
			break;
		case counterclockwise_90:
			PORTA = rotation[i];
			i--;
			numPhases++;
			if(i <= 0) { i = 7;}
			state = counterclockwise_90;
			if(numPhases >= 1024){ state = stepper_start;}
			break;
		case counterclockwise_180:
			PORTA = rotation[i];
			i--;
			numPhases++;
			if(i <= 0) { i = 7;}
			state = counterclockwise_180;
			if(numPhases >= 1950){ state = stepper_start;}
			break;
		case counterclockwise_360:
			PORTA = rotation[i];
			i--;
			numPhases++;
			if(i <= 0) { i = 7;}
			state = counterclockwise_360;
			if(numPhases >= 3650){ state = stepper_start;}
			break;
		default:
			state = stepper_start;
			break;
	}
	return state;
}	
int main(void)
{
    DDRA = 0xFF; PORTA = 0x00; // Output
    DDRB = 0x00; PORTB = 0xFF; // Input
    DDRD = 0xFF; PORTD = 0x00; // Testing Output 
	
	TimerSet(timerPeriod);
	TimerOn();
	
	unsigned char i = 0;
	tasks[i].state = stepper_start;
	tasks[i].period = 3;
	tasks[i].elapsedTime = 0;
	tasks[i].TickFct = &stepper_tick;
	
	while (1) 
    {
		//if(PB0) { PORTD = 0xFF;}
		//else if(PB1) { PORTD = 0x00;}
    }
}

