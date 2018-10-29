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
unsigned char column_sel = 0x10; // grounds column to display pattern

unsigned short joystick = 0x0000;
unsigned short temp = 0x000;

// Pins on PORTA are used as input for A2D conversion
// The Default Channel is 0 (PA0)
// The value of pinNum determines the pin on PORTA used for A2D conversion
// Valid values range between 0 and 7, where the value represents the desired pin for A2D conversion
void Set_A2D_Pin(unsigned char pinNum)
{
	ADMUX = (pinNum <= 0x07) ? pinNum : ADMUX;
	// Allow channel to stabilize
	static unsigned char i = 0;
	for(i = 0; i < 15; ++i){asm("nop");}
}

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
		if((joystick < 450) || (joystick > 600)) { tasks[1].period = 500;}
		if((joystick < 350) || (joystick > 700)) { tasks[1].period = 250;}
		if((joystick < 250) || (joystick > 800)) { tasks[1].period = 100;}
		if((joystick < 150) || (joystick > 900)) { tasks[1].period = 25;}
		speed_state = speed_check;
		break;
	}
	return speed_state;
}

enum movement_states {start, left_right, up_down} movement_state;
int TickFct_movement(int movement_state)
{	
	switch(movement_state)
	{
	case start:
		movement_state = left_right;
		break;
	case left_right:
		Set_A2D_Pin(0x00);
		convert();
		joystick = ADC;
		if(joystick < 450)
		{
			if(column_val == 0x01) { column_val = 0x80;}
			else if (column_val != 0x01) { column_val = (column_val >> 1);} // Obviously a right shift must occur
		}
		if(joystick > 600)
		{
			if(column_val == 0x80) { column_val = 0x01;}
			else if (column_val != 0x80) { column_val = (column_val << 1);} // Obviously a left shift must occur
		}
		movement_state = up_down;
		break;
	case up_down:
		Set_A2D_Pin(0x01);
		convert();
		temp = ADC;
		if(temp > 600)
		{
			if(column_sel == 0x01) { column_sel = 0x80;}
			else if (column_sel != 0x01) { column_sel = (column_sel >> 1);} // Obviously a right shift must occur
		}
		if(temp < 450)
		{
			if(column_sel == 0x80) { column_sel = 0x01;}
			else if (column_sel != 0x80) { column_sel = (column_sel << 1);} // Obviously a left shift must occur
		}
		movement_state = left_right;
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
			PORTB = ~column_sel;
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
	tasks[i].period = 500;
	tasks[i].elapsedTime = 0;
	tasks[i].TickFct = &TickFct_movement;
	i++;
	tasks[i].state = -1;
	tasks[i].period = 50;
	tasks[i].elapsedTime = 0;
	tasks[i].TickFct = &TickFct_LEDState;
    while (1) 
    {
    }
}

