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

unsigned char s_data, counter = 0x00; // USART variables

unsigned char carSpeed = 0; // 00 - stopped, 01 - creep, 10 - medium, 11 - fast
unsigned char carXAxis = 0; // 10 - left, 00 - straight, 01 - right
unsigned char carYAxis = 0; // 0 - forward, 1 - reverse

unsigned char carValues = 0x00; // bits 0-1 are speed, 2-3 are left/right, 4 is forward/reverse

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
			carValues = (carValues & 0xFC); // speed value set to 00 for stopped
			if((joystick2 < 500) || (joystick2 > 600)) { carValues = ((carValues & 0xFC) | 0x01);} // speed value set to 01 for creep
			if((joystick2 < 150) || (joystick2 > 850)) { carValues = ((carValues & 0xFC) | 0x02);} // speed value set to 10 for medium
			if((joystick2 < 100) || (joystick2 > 900)) { carValues = ((carValues & 0xFC) | 0x03);} // speed value set to 11 for fast
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
			carValues = (carValues & 0xF3); // L/R set to 00 for straight
			if(joystick > 750) // Joystick is being tilted left
			{
				carValues = ((carValues & 0xF3) | 0x08); // L/R set to 10 for left
			}
			else if(joystick < 450) // Joystick is being tilted right
			{
				carValues = ((carValues & 0xF3) | 0x04); // L/R set to 01 for right
			}
			movement_state = up_down;
			break;
		case up_down: // Left joystick controls forward and reverse movements
 			Set_A2D_Pin(0x03); // Sets analog signal to the up/down axis of the left joystick
 			convert();
			joystick2 = ADC; // Read ADC value into joystick2 variable 
			if(joystick2 > 600) // Joystick is being tilted up 
			{
				carValues = (carValues & 0xEF); // F/R set to 0 for forward
			}
			else if(joystick2 < 500) // Joystick is being tilted down
			{
				carValues = (carValues | 0x10); // F/R set to 1 for reverse
			}
			movement_state = left_right; // Return to left right state
			break;
		default:
			movement_state = left_right;
			break;
	}
	return movement_state;
}


enum uart_state{uart_start, send};
int uart_tick(int state)
{
	switch(state)
	{
		case uart_start:
			counter = 0; // Reset counter to 0 after all three signals have been sent
			if(USART_HasTransmitted(1))
			{
				s_data = carValues; // Updates s_data if previous value has been transmitted
			}
			state = send;
			break;
		case send:
			state = uart_start;
			if(USART_IsSendReady(1) && counter < 3)  // Send three copies of the same signal for redundancy
			{ 
				USART_Send(s_data, 1); // Send s_data
				counter++; // Updates counter
				state = send;
			}
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
	tasks[i].period = 250;
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