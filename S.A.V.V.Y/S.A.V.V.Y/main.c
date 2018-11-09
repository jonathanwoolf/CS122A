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

unsigned char tmpA, tmpB, s_data, flag; // USART variables

unsigned char column_val = 0x08; // Sets the pattern displayed on columns
unsigned char column_sel = 0x10; // Grounds column to display pattern

unsigned char carSpeed = 0; // 0 - stopped, 1 - slow, 2 - medium, 3 - fast
unsigned char carXAxis = 1; // 0 - left, 1 - straight, 2 - right
unsigned char carYAxis = 0; // 0 - reverse, 1 - forward

unsigned short joystick, joystick2; // Variables to store ADC values of joysticks

// Code provided by UCR for ADC

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

// End of code provided by UCR for ADC

// Sets speed for drive and reverse
enum speed_states {speed_check} speed_state;
int TickFct_speed(int speed_state)
{
	Set_A2D_Pin(0x03); // Sets analog signal to the up/down axis of the left joystick
 	convert();
 	joystick2 = ADC;
	
	switch(speed_state)
	{
		case speed_check: // Decides how fast the car is going
			if((joystick2 < 500) || (joystick2 > 600)) { tasks[1].period = 500;}
			if((joystick2 < 400) || (joystick2 > 700)) { tasks[1].period = 250;}
			if((joystick2 < 200) || (joystick2 > 800)) { tasks[1].period = 100;}
			if((joystick2 < 100) || (joystick2 > 900)) { tasks[1].period = 25;}
			speed_state = speed_check;
			break;
	}
	return speed_state;
}

// Joysticks are actually wired sideways so left/right and up/down are switched but the states are labeled correctly for their observed actions
enum movement_states {left_right, up_down} movement_state;
int TickFct_movement(int movement_state)
{
	switch(movement_state)
	{
		case left_right: // Right joystick controls left and right movements
			Set_A2D_Pin(0x00); // Sets analog signal to the left/right axis of the right joystick
			convert();
			joystick = ADC; // Read ADC value into joystick variable
			carXAxis = 0; // No motion detected on x-axis
			if(joystick > 650) // Joystick is being tilted left
			{
				carXAxis = 1;
// 				if(column_val == 0x01) { column_val = 0x80;} // Move left a row
// 				else if (column_val != 0x01) { column_val = (column_val >> 1);} // Obviously a right shift must occur
			}
			if(joystick < 450) // Joystick is being tilted right
			{
				carXAxis = 2; 
// 				if(column_val == 0x80) { column_val = 0x01;} // Move right a row
// 				else if (column_val != 0x80) { column_val = (column_val << 1);} // Obviously a left shift must occur
			}
			movement_state = up_down;
			break;
		case up_down: // Left joystick controls forward and reverse movements
 			Set_A2D_Pin(0x03); // Sets analog signal to the up/down axis of the left joystick
 			convert();
			joystick2 = ADC; // Read ADC value into joystick2 variable 
			carYAxis = 0; // No motion detected on y-axis
			if(joystick2 > 600) // Joystick is being tilted up 
			{
				carYAxis = 1; 
// 				if(column_sel == 0x01) { column_sel = 0x80;} // Move up a column
// 				else if (column_sel != 0x01) { column_sel = (column_sel >> 1);} // Obviously a right shift must occur
			}
			if(joystick2 < 500) // Joystick is being tilted down
			{
				carYAxis = 2;
// 				if(column_sel == 0x80) { column_sel = 0x01;} // Move down a column
// 				else if (column_sel != 0x80) { column_sel = (column_sel << 1);} // Obviously a left shift must occur
			}
			movement_state = left_right; // Return to left right state
			break;
		default:
			movement_state = left_right;
			break;
	}
	return movement_state;
}


enum uart_state{uart_start, send, toggle};
int uart_tick(int state)
{
	switch(state)
	{
		case uart_start:
			s_data = 0x00;
			state = send;
			break;
		case send:
			if(USART_IsSendReady(1)) { USART_Send(s_data, 1);}
			state = toggle;
			break;
		case toggle:
			if(USART_HasTransmitted(1))
			{
				s_data = carYAxis;
			}
			state = send;
			break;
		default:
			state = uart_start;
			break;
	}
	return state;
}


int main(void)
{
	DDRA = 0x00; PORTA = 0xFF; // Input
	DDRB = 0xFF; PORTB = 0x00; // Output to column sel
    // Output from RF transmitter will be sent from TX1 DO NOT INITIALIZE DDRD / PORTD as it will not send
	
	TimerSet(timerPeriod);
	TimerOn();
	A2D_init();
	
	initUSART(1);
	
	unsigned char i = 0;
	tasks[i].state = speed_check;
	tasks[i].period = 50;
	tasks[i].elapsedTime = 0;
	tasks[i].TickFct = &TickFct_speed;
	i++;
	tasks[i].state = -1;
	tasks[i].period = 500;
	tasks[i].elapsedTime = 0;
	tasks[i].TickFct = &TickFct_movement;
	i++;
	tasks[i].state = uart_start;
	tasks[i].period = 50;
	tasks[i].elapsedTime = 0;
	tasks[i].TickFct = &uart_tick;

	while (1)
	{
	}
}