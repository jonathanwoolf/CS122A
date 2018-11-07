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

unsigned char tmpA, tmpB, r_data, s_data, flag; // USART variables

unsigned char column_val = 0x08; // Sets the pattern displayed on columns
unsigned char column_sel = 0x10; // Grounds column to display pattern

unsigned short joystick, joystick2; // Variables to store ADC values of joysticks

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
			if(joystick > 650) // Joystick is being tilted left
			{
				if(column_val == 0x01) { column_val = 0x80;} // Move left a row
				else if (column_val != 0x01) { column_val = (column_val >> 1);} // Obviously a right shift must occur
			}
			if(joystick < 500) // Joystick is being tilted right
			{
				if(column_val == 0x80) { column_val = 0x01;} // Move right a row
				else if (column_val != 0x80) { column_val = (column_val << 1);} // Obviously a left shift must occur
			}
			
			movement_state = up_down;
			break;
		case up_down: // Left joystick controls forward and reverse movements
 			Set_A2D_Pin(0x03); // Sets analog signal to the up/down axis of the left joystick
 			convert();
			joystick2 = ADC; // Read ADC value into joystick2 variable 
			if(joystick2 > 600) // Joystick is being tilted up 
			{
				if(column_sel == 0x01) { column_sel = 0x80;} // Move up a column
				else if (column_sel != 0x01) { column_sel = (column_sel >> 1);} // Obviously a right shift must occur
			}
			if(joystick2 < 500) // Joystick is being tilted down
			{
				if(column_sel == 0x80) { column_sel = 0x01;} // Move down a column
				else if (column_sel != 0x80) { column_sel = (column_sel << 1);} // Obviously a left shift must occur
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
			tmpA = 0x00;
			tmpB = 0x00;
			s_data = 0x00;
			flag = 0;
			state = send;
			break;
		case send:
			if(USART_IsSendReady(1))
			{
				USART_Send(s_data, 1);
// 				if(flag)
// 				{
// 					tmpA = s_data;
// 					flag = 0;
// 				}
// 				else
// 				{
// 					tmpB = s_data;
// 					flag = 1;
// 				}
			}
			state = toggle;
			break;
		case toggle:
			tmpA = column_val;
			tmpB = ~column_sel;
			if(USART_HasTransmitted(1))
			{
				if(s_data == tmpA)
				{
					s_data = tmpB;
				}
				else
				{
					s_data = tmpA;
				}
			}
			state = send;
			break;
		default:
			state = uart_start;
			break;
	}
	return state;
}

// Test harness for LED matrix to make sure all user inputs are read in correctly
// enum LED_states {synch} LED_state;
// int TickFct_LEDState(int state)
// {
// 	switch(LED_state)
// 	{
// 		case synch:
// 			if(test1 && test2) {PORTD = 0x03;}
// 			else if(test1) {PORTD = 0x01;}
// 			else if(test2) {PORTD = 0x02;}
// 			else {PORTD = 0x00;}
// 			//PORTB = 0x01; //Test for DC Motor
// 			//PORTB = ~column_sel;
// 			//PORTD = column_val;
// 			LED_state = synch;
// 			break;
// 		default:
// 			LED_state = synch;
// 			break;
// 	}
// 	return LED_state;
// }

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
	tasks[i].state = -1;
	tasks[i].period = 500;
	tasks[i].elapsedTime = 0;
	tasks[i].TickFct = &TickFct_movement;
	i++;
	tasks[i].state = uart_start;
	tasks[i].period = 10;
	tasks[i].elapsedTime = 0;
	tasks[i].TickFct = &uart_tick;
//	i++;
// 	tasks[i].state = -1;
// 	tasks[i].period = 50;
// 	tasks[i].elapsedTime = 0;
// 	tasks[i].TickFct = &TickFct_LEDState;
	while (1)
	{
	}
}