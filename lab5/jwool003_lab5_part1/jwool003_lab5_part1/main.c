/*
 * jwool003_lab5_part1.c
 *
 * Created: 10/23/2018 4:24:11 PM
 * Author : Jonathan
 */ 

#define timerPeriod 1
#define tasksNum 1
#define PB0 !(PINB & 0x01)
#define PB1 !(PINB & 0x02)

#include <avr/io.h>
#include "scheduler.h"

unsigned char rotation[8] = {0x01, 0x03, 0x02, 0x06, 0x04, 0x12, 0x08, 0x09};

	
enum stepper_states{stepper_start, stepper_clockwise, stepper_counterclockwise};

int stepper_tick(int state){
	
	static unsigned char i = 0;
	
	switch(state){
		case stepper_start:
			PORTB = rotation[i];
			if(PB0) { state = stepper_clockwise;}
			else if(PB1) { state = stepper_counterclockwise;}
			else { state = stepper_start;}
			break;
		case stepper_clockwise:
			i++;
			if(i >= 7) { i = 0;}
			PORTB = rotation[i];
			if(PB0) { state = stepper_clockwise;}
			else if(PB1) { state = stepper_counterclockwise;}
			else { state = stepper_start;}
			break;
		case stepper_counterclockwise:
			i--;
			if(i <= 0) { i = 7;}
			PORTB = rotation[i];
			if(PB0) { state = stepper_clockwise;}
			else if(PB1) { state = stepper_counterclockwise;}
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
    DDRA = 0x00; PORTA = 0xFF; // Output
    DDRB = 0xFF; PORTB = 0x00; // Input
    
	TimerSet(timerPeriod);
	TimerOn();
	
	unsigned char i = 0;
	tasks[i].state = stepper_start;
	tasks[i].period = 100;
	tasks[i].elapsedTime = 0;
	tasks[i].TickFct = &stepper_tick;
	
	while (1) 
    {
    }
}

