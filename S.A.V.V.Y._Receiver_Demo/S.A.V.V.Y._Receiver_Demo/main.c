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
#define tasksNum 3

#include <avr/io.h>
#include <avr/interrupt.h>
#include "scheduler.h"
#include "usart_ATmega1284.h"

unsigned char tmpA, tmpB, r_data; // USART variables

unsigned char carYAxis = 0;

unsigned char column_val = 0x08; // Sets the pattern displayed on columns
unsigned char column_sel = 0x10; // Grounds column to display pattern

enum uart_state{uart_start, receive, toggle};
int uart_tick(int state)
{
	switch(state)
	{
		case uart_start:
 			tmpA = column_sel;
 			tmpB = ~column_val;
			state = receive;
			break;
		case receive:
			if(USART_HasReceived(1)){
				r_data = USART_Receive(1);
			}
			USART_Flush(1);
			state = toggle;
			break;
		case toggle:
			carYAxis = r_data; 
			state = receive;
			break;
		default:
			state = uart_start;
			break;
	}
	return state;
}

// Joysticks are actually wired sideways so left/right and up/down are switched but the states are labeled correctly for their observed actions
enum movement_states {left_right, up_down} movement_state;
int TickFct_movement(int movement_state)
{
	switch(movement_state)
	{
		case left_right: // Right joystick controls left and right movements
// 			if(joystick > 650) // Joystick is being tilted left
// 			{
// 				if(column_val == 0x01) { column_val = 0x80;} // Move left a row
// 				else if (column_val != 0x01) { column_val = (column_val >> 1);} // Obviously a right shift must occur
// 			}
// 			if(joystick < 450) // Joystick is being tilted right
// 			{
// 				if(column_val == 0x80) { column_val = 0x01;} // Move right a row
// 				else if (column_val != 0x80) { column_val = (column_val << 1);} // Obviously a left shift must occur
// 			}
// 			else { column_val = column_val;}
			movement_state = up_down;
			break;
		case up_down: // Left joystick controls forward and reverse movements
			if(carYAxis == 1) // Joystick is being tilted up
			{
				if(column_sel == 0x01) { column_sel = 0x80;} // Move up a column
				else if (column_sel != 0x01) { column_sel = (column_sel >> 1);} // Obviously a right shift must occur
			}
			if(carYAxis == 2) // Joystick is being tilted down
			{
				if(column_sel == 0x80) { column_sel = 0x01;} // Move down a column
				else if (column_sel != 0x80) { column_sel = (column_sel << 1);} // Obviously a left shift must occur
			}
			//else { column_sel = column_sel;}
			movement_state = left_right; // Return to left right state
			break;
		default:
			movement_state = left_right;
			break;
	}
	return movement_state;
}

// Test harness for LED matrix to make sure all user inputs are read in correctly
enum LED_states {synch} LED_state;
int TickFct_LEDState(int state)
{
	switch(LED_state)
	{
		case synch:
			//PORTB = 0x01; //Test for DC Motor
			PORTB = ~column_val;
			PORTA = column_sel;
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
	// Input from RF Receiver will be received from RX1 
	
	TimerSet(timerPeriod);
	TimerOn();
	
	initUSART(1);
	
	unsigned char i = 0;
	tasks[i].state = uart_start;
	tasks[i].period = 50;
	tasks[i].elapsedTime = 0;
	tasks[i].TickFct = &uart_tick;
	i++;
	tasks[i].state = -1;
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