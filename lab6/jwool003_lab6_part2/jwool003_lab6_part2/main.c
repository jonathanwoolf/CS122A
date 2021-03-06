/*
 * jwool003_lab6_part3.c
 *
 * Created: 10/25/2018 3:55:42 PM
 * Author : Jonathan
 */ 
#define timerPeriod 1
#define tasksNum 3

#include <avr/io.h>
#include <avr/interrupt.h>
#include "scheduler.h"

unsigned char column_val = 0x08; // sets the pattern displayed on columns
unsigned char column_sel = 0xFE; // grounds column to display pattern

unsigned short joystick = 0x0000;

void A2D_init() {
	ADCSRA |= (1 << ADEN) | (1 << ADSC) | (1 << ADATE);
	// ADEN: Enables analog-to-digital conversion
	// ADSC: Starts analog-to-digital conversion
	// ADATE: Enables auto-triggering, allowing for constant
	//	    analog to digital conversions.
}

void convert(){
	ADCSRA |=(1<<ADSC);//start ADC conversion
	while ( !(ADCSRA & (1<<ADIF)));//wait till ADC conversion
}

enum speed_states {speed_check} speed_state;
int TickFct_speed(int speed_state)
{
	convert();
	joystick = ADC; 
	
	switch(speed_state)
	{
	case speed_check:
		if((joystick < 450) || (joystick > 600)) { tasks[1].period = 1000;}
		if((joystick < 350) || (joystick > 700)) { tasks[1].period = 500;}
		if((joystick < 250) || (joystick > 800)) { tasks[1].period = 250;}
		if((joystick < 150) || (joystick > 900)) { tasks[1].period = 50;}
		speed_state = speed_check;
		break;
	}
	return speed_state;
}

enum movement_states {start, left, right} movement_state;
int TickFct_movement(int movement_state)
{
	convert();
	joystick = ADC;
	
	switch(movement_state)
	{
	case start:
		if(joystick < 400)
		{
			movement_state = left;
		}
		else if(joystick > 650)
		{
			movement_state = right;
		}
		else
		{
			movement_state = start;
		}
		break;
	case left:
		if(column_val == 0x01) { column_val = 0x80;}
		else if (column_val != 0x01) { column_val = (column_val >> 1);} // Obviously a right shift must occur
		movement_state = start;
		if(joystick < 400)
		{
			movement_state = left;
		}
		break;
	case right:
		if(column_val == 0x80) { column_val = 0x01;}
		else if (column_val != 0x80) { column_val = (column_val << 1);} // Obviously a left shift must occur
		movement_state = start;
		if(joystick > 650)
		{
			movement_state = right;
		}
		break;
	}
	return movement_state;
}

enum LED_states {synch} LED_state;
int TickFct_LEDState(int state)
{
	switch(LED_state)
	{ 
		case synch:
			PORTB = column_sel;
			PORTD = column_val;
			LED_state = synch;
			break;
		default:
			LED_state = synch;
			break;
	}
	return LED_state;
}

int main(void)
{
	DDRA = 0x00; PORTA = 0xFF; // Input 
	DDRB = 0xFF; PORTB = 0x00; // Output to column sel
	DDRD = 0xFF; PORTD = 0x00; // Output to column val
	
	TimerSet(timerPeriod);
	TimerOn();
    A2D_init();
	
	unsigned char i = 0;
	tasks[i].state = speed_check;
	tasks[i].period = 50;
	tasks[i].elapsedTime = 0;
	tasks[i].TickFct = &TickFct_speed;
	i++;
	tasks[i].state = start;
	tasks[i].period = 1000;
	tasks[i].elapsedTime = 0;
	tasks[i].TickFct = &TickFct_movement;
	i++;
	tasks[i].state = -1;
	tasks[i].period = 1;
	tasks[i].elapsedTime = 0;
	tasks[i].TickFct = &TickFct_LEDState;
    while (1) 
    {
    }
}

