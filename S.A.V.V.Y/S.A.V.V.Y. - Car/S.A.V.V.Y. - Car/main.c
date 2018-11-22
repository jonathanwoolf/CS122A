/*
 * S.A.V.V.Y. - Car
 *
 * Created: 11/13/2018 7:07:48 PM
 * Author : Jonathan Woolf
 */

//FIXME: Make sure new else statement around remove denoter function doesn't remove performance

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
				USART_Flush(1);
				counter++; // Increments counter
				if((r_data1 >> 5) != 0x04) {counter = 0;}
				else { r_data1 = r_data1 & 0x1F;} // Remove denoter
				state = receive;
			}
			else if(USART_HasReceived(1) && counter == 1) // Receives second copy of signal
			{
				r_data2 = USART_Receive(1);
				USART_Flush(1);
				counter++; // Increments counter
				if((r_data2 >> 5) != 0x02) {counter = 0;}
				else { r_data2 = r_data2 & 0x1F;} // Remove denoter
				state = receive;
				if (r_data1 == r_data2) {carValues = r_data2; state = toggle;} 
			}
			else if(USART_HasReceived(1) && counter == 2) // Receives third copy of signal
			{
				r_data3 = USART_Receive(1);
				USART_Flush(1);
				counter = 0; // Resets the counter
				state = toggle;
				if((r_data3 >> 5) != 0x01) {state = receive;} // Remover denoter
				else {r_data3 = r_data3 & 0x1F;}
				if (r_data1 == r_data3 || r_data2 == r_data3) {carValues = r_data3;} 
				
			}
			break;
		case toggle:
			carSpeed = (carValues & 0x03);  // carSpeed is represented by the first two values of carValues
			carXAxis = ((carValues >> 2) & 0x03); // carXAxis is represented by the second 2 values of carValues
			carYAxis = ((carValues >> 4) & 0x01); // carYAxis is represented by the 5th value of carValues
			state = receive;
			break;
		default:
			state = receive;
			break;
	}
	return state;
}

enum movement_states {movement} movement_state;
int TickFct_movement(int movement_state)
{
	switch(movement_state)
	{
		case movement: // Left joystick controls forward and back / Right joystick controls left and right movements
			if(carXAxis == 0x00 && carSpeed == 0x00)
			{
				PORTB = 0x00; // Real nowhere man sitting in his nowhere land
				PORTD = 0x00;
			}
			else 
			{
				if(carYAxis == 0x00 && carXAxis == 0x00 && carSpeed >= 0x01) // Forward
				{
					PORTB = 0x91; // Forward friends 10 signals to output
					PORTD = 0xA0;
				}
				if(carYAxis == 0x01 && carXAxis == 0x00 && carSpeed >= 0x01) // Reverse
				{
					PORTB = 0x4A; // Backwards buds 01 signals to output
					PORTD = 0x50;
				}
				if(carYAxis == 0x00 && carXAxis == 0x02 && carSpeed >= 0x01) // Forward and Left
				{
					PORTB = 0x94; // Forward friends but to the left
					PORTD = 0x80;
				}
				if(carYAxis == 0x00 && carXAxis == 0x01 && carSpeed >= 0x01) // Forward and Right
				{
					PORTB = 0x14; // 0x10
					PORTD = 0xA0; // 0x20
				}
				if(carXAxis == 0x02 && carSpeed == 0x00) // Stopped Left
				{
					PORTB = 0x84;
					PORTD = 0x80;
				}
				if(carXAxis == 0x01 && carSpeed == 0x00) // Stopped Right
				{
					PORTB = 0x14; // 0x10
					PORTD = 0x20; // 0x20
				}	
			}
			movement_state = movement; // Return to left right state
			break;
		default:
			movement_state = movement;
			break;
	}
	return movement_state;
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
	tasks[i].state = -1;
	tasks[i].period = 25;
	tasks[i].elapsedTime = 0;
	tasks[i].TickFct = &uart_tick;
	i++;
	tasks[i].state = -1;
	tasks[i].period = 50;
	tasks[i].elapsedTime = 0;
	tasks[i].TickFct = &TickFct_movement;
	
	while (1)
	{
	}
}