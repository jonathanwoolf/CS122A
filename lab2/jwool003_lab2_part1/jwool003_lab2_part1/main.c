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

unsigned char tmpB, r_data, s_data;

enum uart_state{uart_start, send, toggle};

char slave = 1;

int uart_tick(int state)
{
	switch(state)
	{
		case uart_start:
			tmpB = 0x00;
			s_data = 0x00;
			state = send;
			break;
		case send:
			if(slave == 0){
				if(USART_IsSendReady(1))
				{
					USART_Send(s_data, 1);
					tmpB = s_data;
				}
			}
			else{
				if(USART_HasReceived(1)){
					r_data = USART_Receive(1);
				}
			}
			state = toggle;
			break;
		case toggle:
			if(slave == 0){
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
			else{
				tmpB = r_data;
			}
			state = send;
			break;
		default:
			tmpB = 0x00;
			state = uart_start;
			break;
	}
	PORTB = tmpB;
	return state;
}

int main(void)
{
	DDRA = 0x00; PORTA = 0xFF;
	DDRB = 0xFF; PORTB = 0x00; // PORTB set to output, outputs init 0s

	
	TimerSet(timerPeriod);
	TimerOn();
	
	unsigned char i = 0;
	tasks[i].state = uart_start;
	if(slave == 0){
		tasks[i].period = 500;
	}
	else{
		tasks[i].period = 10;
	}
	tasks[i].elapsedTime = 0;
	tasks[i].TickFct = &uart_tick;
	
	initUSART(1);
	
	while(1)
	{
	}
}

