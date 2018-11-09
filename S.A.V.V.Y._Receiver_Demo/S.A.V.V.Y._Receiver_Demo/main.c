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

unsigned char tmpA, tmpB, r_data; // USART variables

unsigned char column_val = 0x08; // Sets the pattern displayed on columns
unsigned char column_sel = 0x10; // Grounds column to display pattern

enum uart_state{uart_start, receive, toggle};

int uart_tick(int state)
{
	switch(state)
	{
		case uart_start:
			tmpA = 0x10;
			tmpB = ~0x08;
			r_data = 0x00;
			state = receive;
			break;
		case receive:
			if(USART_HasReceived(1)){
				r_data = USART_Receive(1);
			}
			state = toggle;
			break;
		case toggle:
			tmpA = r_data; 
			state = receive;
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
			PORTA = r_data;
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
	tasks[i].period = 10;
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