/*	Partner(s) Name & E-mail: Jonathan Woolf, Adriel Bustamante, Rohit Joshi
 *	Lab Section: 023
 *	Assignment: Lab #2 Exercise #1 
 *	Exercise Description: [optional - include for your own benefit]
 *	
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */

#define tasksNum 1
#define timerPeriod 1

#include <avr/io.h>
#include <avr/interrupt.h>

#include "usart_ATmega1284.h"
#include "scheduler.h"

unsigned char tmpA, tmpB, tmpC, s_data, r_data, leader;

enum uart_state{uart_start, check, send, toggle};

int uart_tick(int state){
	tmpA = ~PINA & 0x01;
	switch(state){
		case uart_start:
			tmpB = 0x00;
			tmpC = 0x00;
			leader = 0;
			s_data = 0x00;
			state = send;
			break;
		case check:
			if(tmpA == 0x01)
			{
				leader = 1;
				tmpC = 0x01;
				tasks[0].period = 1000;
			}
			else
			{
				leader = 0;
				tmpC = 0x00;
				tasks[0].period = 10;
			}
			state = send;
			break;
		case send:
			if(leader == 1)
			{
				if(USART_IsSendReady(1))
				{
					state = send;
					USART_Send(s_data, 1);
					tmpB = s_data;
				}
			}
			else
			{
				if(USART_HasReceived(0))
				{
					r_data = USART_Receive(0);
				}
			}
			state = toggle;
			break;
		case toggle:
			if(leader == 1)
			{
				if(USART_HasTransmitted(1))
				{
					if(s_data == 0x00)
					{
						s_data = 0x01;
					}
					else
					{
						s_data = 0x00;
					}
				}
			}
			else
			{
				tmpB = r_data;
			}
			state = check;
			break;
		default:
			tmpB = 0x00;
			tmpC = 0x00;
			state = uart_start;
			break;
	}
	PORTB = tmpB;
	PORTC = tmpC;
	return state;
}

int main(void)
{
	DDRA = 0x00; PORTA = 0xFF; // PINA set to input
	DDRB = 0xFF; PORTB = 0x00; // PORTB set to output, outputs init 0s
	DDRC = 0xFF; PORTC = 0x00;
		
	
	TimerSet(timerPeriod);
	TimerOn();
	
	initUSART(0);
	initUSART(1);
	
	unsigned char i = 0;
	tasks[i].state = uart_start;
	tasks[i].period = 1000;
	tasks[i].elapsedTime = 0;
	tasks[i].TickFct = &uart_tick;
	
	while(1)
	{
	}
}