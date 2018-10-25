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

#include <avr/io.h>
#include "scheduler.h"

unsigned char rotation[8] = {0x01, 0x03, 0x02, 0x06, 0x04, 0x0C, 0x08, 0x09};
	
enum stepper_states{stepper_start, stepper_clockwise, stepper_counterclockwise};

int stepper_tick(int state){
	
	static unsigned char i = 0;
	
	switch(state){
		case stepper_start:
			PORTA = rotation[i];
			if(PB0) { state = stepper_clockwise;}
			else if(PB1) { state = stepper_counterclockwise;}
			else if(PB0 && PB1) { state = stepper_start;}
			else { state = stepper_start;}
			break;
		case stepper_clockwise:
			i++;
			if(i >= 7) { i = 0;}
			PORTA = rotation[i];
			if(PB0) { state = stepper_clockwise;}
			else if(PB1) { state = stepper_counterclockwise;}
			else if(PB0 && PB1) { state = stepper_start;}
			else { state = stepper_start;}
			break;
		case stepper_counterclockwise:
			i--;
			if(i <= 0) { i = 7;}
			PORTA = rotation[i];
			if(PB0) { state = stepper_clockwise;}
			else if(PB1) { state = stepper_counterclockwise;}
			else if(PB0 && PB1) { state = stepper_start;}
			else { state = stepper_start;}
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

