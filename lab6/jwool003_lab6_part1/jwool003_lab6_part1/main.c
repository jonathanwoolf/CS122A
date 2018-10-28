/*
 * jwool003_lab6_part1.c
 *
 * Created: 10/25/2018 3:55:42 PM
 * Author : Jonathan
 */ 
#define timerPeriod 1
#define tasksNum 1

#include <avr/io.h>
#include <avr/interrupt.h>
#include "scheduler.h"

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
enum LED_states {synch} LED_state;
int TickFct_LEDState(int state)
{
	unsigned short x;  // Value of ADC 10-bit register will be stored in variable x.
// 	unsigned char tmpB = (char)x; // stores bottom byte of ADC register i.e. tmpB = 0xFF
// 	unsigned char tmpD = (char)(x >> 8); // tmpD = 0x03;

	switch(LED_state)
	{ 
		case synch:
			convert();
			x = ADC;
			PORTB = x;
			PORTD = x >> 8;
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
	DDRB = 0xFF; PORTB = 0x00; // Output
	DDRD = 0x03; PORTD = 0xFC; // Mixed
	
	TimerSet(timerPeriod);
	TimerOn();
    A2D_init();
	
	unsigned char i = 0;
	tasks[i].state = -1;
	tasks[i].period = 100;
	tasks[i].elapsedTime = 0;
	tasks[i].TickFct = &TickFct_LEDState;
		
    while (1) 
    {
    }
}

