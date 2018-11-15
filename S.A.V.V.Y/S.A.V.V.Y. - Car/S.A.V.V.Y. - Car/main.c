/*
 * S.A.V.V.Y. - Car
 *
 * Created: 11/13/2018 7:07:48 PM
 * Author : Jonathan Woolf
 */ 

#define timerPeriod 1
#define tasksNum 3

#include <avr/io.h>
#include <avr/interrupt.h>
#include "scheduler.h"
#include "usart_ATmega1284.h"

unsigned char tmpA, tmpB, r_data; // USART variables

unsigned char carSpeed = 0; // 00 - stopped, 01 - creep, 10 - medium, 11 - fast
unsigned char carXAxis = 0; // 10 - left, 00 - straight, 01 - right
unsigned char carYAxis = 0; // 0 - forward, 1 - reverse

unsigned char carValues = 0x00; // bits 0-1 are speed, 2-3 are left/right, 4 is forward/reverse

enum uart_state{uart_start, receive, toggle};
int uart_tick(int state)
{
	switch(state)
	{
		case uart_start:
			state = receive;
			break;
		case receive:
			if(USART_HasReceived(1))
			{
				r_data = USART_Receive(1);
			}
			USART_Flush(1);
			state = toggle;
			break;
		case toggle:
			carValues = r_data;
			carSpeed = (carValues & 0x03);  
			carXAxis = ((carValues >> 2) & 0x03);
			carYAxis = ((carValues >> 4) & 0x01);
// 			if(carSpeed == 0x01) { tasks[1].period = 300;}
// 			else if(carSpeed == 0x02) { tasks[1].period = 150;}
// 			else if(carSpeed == 0x03) { tasks[1].period = 50;}
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
			if(carXAxis == 0x01) // Joystick is being tilted right (it is pointed the wrong way so if wired differently this code must be flipped
			{
// 				if(column_val == 0x01) { column_val = 0x80;} // Move left a row
// 				else if (column_val != 0x01) { column_val = (column_val >> 1);} // Obviously a right shift must occur
			}
			if(carXAxis == 0x02) // Joystick is being tilted left
			{
// 				if(column_val == 0x80) { column_val = 0x01;} // Move right a row
// 				else if (column_val != 0x80) { column_val = (column_val << 1);} // Obviously a left shift must occur
			}
			movement_state = up_down;
			break;
		case up_down: // Left joystick controls forward and reverse movements
			if(carYAxis == 0x00 && carSpeed >= 0x01) // Joystick is being tilted up
			{
				PORTB = 0x91; // Forward friends 10 signals to output 
			}
			else if(carYAxis == 0x01 && carSpeed >= 0x01) // Joystick is being tilted down
			{
				PORTB = 0x4A; // Backwards buds 01 signals to output
			}
			else 
			{
				PORTB = 0x00;
			}
			movement_state = left_right; // Return to left right state
			break;
		default:
			movement_state = left_right;
			break;
	}
	return movement_state;
}

// Test harness for LED matrix to make sure all user inputs are read in correctly
enum car_States {synch} car_state;
int TickFct_carState(int state)
{
	switch(car_state)
	{
		case synch:
			//PORTB = 0x0F; //Test for DC Motor 0x08 is clockwise so 0x10 is counter
			car_state = synch;
			break;
		default:
			car_state = synch;
			break;
	}
	return car_state;
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
	tasks[i].period = 5;
	tasks[i].elapsedTime = 0;
	tasks[i].TickFct = &TickFct_movement;
	i++;
	tasks[i].state = -1;
	tasks[i].period = 50;
	tasks[i].elapsedTime = 0;
	tasks[i].TickFct = &TickFct_carState;
	
	while (1)
	{
	}
}