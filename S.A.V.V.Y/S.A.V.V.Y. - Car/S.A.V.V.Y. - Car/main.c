/*
 * S.A.V.V.Y. - Car
 *
 * Created: 11/13/2018 7:07:48 PM
 * Author : Jonathan Woolf
 */ 

#define timerPeriod 1
#define tasksNum 2

#include <avr/io.h>
#include <avr/interrupt.h>
#include "scheduler.h"
#include "usart_ATmega1284.h"

unsigned char r_data1, r_data2, r_data3, counter = 0x00; // USART variables

unsigned char carSpeed = 0; // 00 - stopped, 01 - creep, 10 - medium, 11 - fast
unsigned char carXAxis = 0; // 10 - left, 00 - straight, 01 - right
unsigned char carYAxis = 0; // 0 - forward, 1 - reverse

unsigned char carValues = 0x00; // bits 0-1 are speed, 2-3 are left/right, 4 is forward/reverse

enum uart_state{receive, toggle};
int uart_tick(int state)
{
	switch(state)
	{
		case receive:
			if(USART_HasReceived(1) && counter == 0) // Receives first copy of signal
			{
				r_data1 = USART_Receive(1);  
				//USART_Flush(1);
				counter++; // Increments counter
				if((r_data1 >> 7) != 1) {counter == 0;}
				r_data1 = r_data1 & 0x1F; // Remove denoter
				state = receive;
			}
			else if(USART_HasReceived(1) && counter == 1) // Receives second copy of signal
			{
				r_data2 = USART_Receive(1);
				//USART_Flush(1);
				counter++; // Increments counter
				if((r_data2 >> 6) != 1) {counter == 0;}
				r_data2 = r_data2 & 0x1F;
				state = receive;
			}
			else if(USART_HasReceived(1) && counter == 2) // Receives third copy of signal
			{
				r_data3 = USART_Receive(1);
				//USART_Flush(1);
				counter = 0; // Resets the counter
				state = toggle;
				if((r_data3 >> 5) != 1) {state = receive;}
				r_data3 = r_data3 & 0x1F;
				
			}
			break;
		case toggle:
			if((r_data1 == r_data2) || (r_data1 == (r_data3))) // If r_data1 matches another r_data, set carValues to r_data1
			{
				carValues = r_data1;
			}
			else if(r_data2 == r_data3) // If r_data2 matches r_data3, set carValues to r_data2
			{
				carValues = r_data2;
			}
			carSpeed = (carValues & 0x03);  // carSpeed is represented by the first two values of carValues
			carXAxis = ((carValues >> 2) & 0x03); // carXAxis is represented by the second 2 values of carValues
			carYAxis = ((carValues >> 4) & 0x01); // carYAxis is represented by the 5th value of carValues

// 			if(carSpeed == 0x01) { tasks[1].period = 300;}
// 			else if(carSpeed == 0x02) { tasks[1].period = 150;}
// 			else if(carSpeed == 0x03) { tasks[1].period = 50;}
			state = receive;
			break;
		default:
			state = receive;
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
			if(carXAxis == 0x02) // Joystick is being tilted left
			{
				PORTB = 0x84;
				PORTD = 0x80;
			}
			else if(carXAxis == 0x01) // Joystick is being tilted right
			{
				PORTB = 0x14; // 0x10
				PORTD = 0x20; // 0x20
			}
			movement_state = up_down;
			break;
		case up_down: // Left joystick controls forward and reverse movements
			if(carYAxis == 0x00 && carSpeed >= 0x01) // Joystick is being tilted up
			{
				PORTB = 0x91; // Forward friends 10 signals to output 
				PORTD = 0xA0;
			}
			else if(carYAxis == 0x01 && carSpeed >= 0x01) // Joystick is being tilted down
			{
				PORTB = 0x4A; // Backwards buds 01 signals to output
				PORTD = 0x50;
			}
			else if(carXAxis == 0x00 && carSpeed == 0x00)
			{
					PORTB = 0x00; // Real nowhere man sitting in his nowhere land
					PORTD = 0x00;
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
// enum car_States {synch} car_state;
// int TickFct_carState(int state)
// {
// 	switch(car_state)
// 	{
// 		case synch:
// 			//PORTB = 0x0F; //Test for DC Motor 0x08 is clockwise so 0x10 is counter
// 			car_state = synch;
// 			break;
// 		default:
// 			car_state = synch;
// 			break;
// 	}
// 	return car_state;
// }

int main(void)
{
	DDRA = 0xFF; PORTA = 0x00; // Output to column val
	DDRB = 0xFF; PORTB = 0x00; // Output to column sel
	// Input from RF Receiver will be received from RX1 
	
	TimerSet(timerPeriod);
	TimerOn();
	
	initUSART(1);
	
	unsigned char i = 0;
	tasks[i].state = -1;
	tasks[i].period = 50;
	tasks[i].elapsedTime = 0;
	tasks[i].TickFct = &uart_tick;
	i++;
	tasks[i].state = -1;
	tasks[i].period = 1;
	tasks[i].elapsedTime = 0;
	tasks[i].TickFct = &TickFct_movement;
// 	i++;
// 	tasks[i].state = -1;
// 	tasks[i].period = 50;
// 	tasks[i].elapsedTime = 0;
// 	tasks[i].TickFct = &TickFct_carState;
	
	while (1)
	{
	}
}