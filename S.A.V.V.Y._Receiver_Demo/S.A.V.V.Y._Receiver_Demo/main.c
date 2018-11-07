/*
 * S.A.V.V.Y._Receiver_Demo.c
 *
 * Created: 11/6/2018 7:07:48 PM
 * Author : Jonathan
 */ 

/*
 * S.A.V.V.Y. Control Unit
 *
 * Created: 10/30/2018 5:56:21 PM
 * Author : Jonathan Woolf
 */ 

#define timerPeriod 1
#define tasksNum 2

#include <avr/io.h>
#include <avr/interrupt.h>
#include "scheduler.h"
#include "usart_ATmega1284.h"

unsigned char tmpA, tmpB, r_data, s_data, flag; // USART variables

unsigned char column_val = 0x08; // Sets the pattern displayed on columns
unsigned char column_sel = 0x10; // Grounds column to display pattern

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

// ADEN: Enables analog-to-digital conversion
// ADSC: Starts analog-to-digital conversion
// ADATE: Enables auto-triggering, allowing for constant
//		  analog to digital conversions.
void A2D_init() { ADCSRA |= (1 << ADEN) | (1 << ADSC) | (1 << ADATE);}

void convert(){
	ADCSRA |= (1 << ADSC); // start ADC conversion
	while ( !(ADCSRA & (1<<ADIF))); // wait till ADC conversion
}

enum uart_state{uart_start, send, toggle};

int uart_tick(int state)
{
	switch(state)
	{
		case uart_start:
			tmpA = 0x10;
			tmpB = 0x08;
			s_data = 0x00;
			flag = 0;
			state = send;
			break;
		case send:
			if(USART_HasReceived(1)){
				r_data = USART_Receive(1);
			}
			state = toggle;
			break;
		case toggle:
			if(flag) { tmpA = r_data; flag = 0;}
			else { tmpB = r_data; flag = 1;}
			state = send;
			break;
		default:
			state = uart_start;
			break;
	}
	return state;
}

// Test harness for LED matrix to make sure all user inputs are read in correctly
enum LED_states {synch} LED_state;
int TickFct_LEDState(int state)
{
	switch(LED_state)
	{
		case synch:
			//PORTB = 0x01; //Test for DC Motor
			PORTB = tmpB;
			PORTA = tmpA;
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
	DDRA = 0xFF; PORTA = 0x00; // Output to column val
	DDRB = 0xFF; PORTB = 0x00; // Output to column sel
	DDRD = 0x00; PORTD = 0xFF; // Input from RF Receiver 
	
	TimerSet(timerPeriod);
	TimerOn();
	
	unsigned char i = 0;
	tasks[i].state = uart_start;
	tasks[i].period = 20;
	tasks[i].elapsedTime = 0;
	tasks[i].TickFct = &uart_tick;
	i++;
	tasks[i].state = -1;
	tasks[i].period = 50;
	tasks[i].elapsedTime = 0;
	tasks[i].TickFct = &TickFct_LEDState;
	while (1)
	{
	}
}