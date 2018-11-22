/*
 * S.A.V.V.Y. - Sensor Demo.c
 *
 * Created: 11/21/2018 3:33:06 PM
 * Author : Jonathan
 */ 


// CHECKNG WITHIN 20CM FOR FINAL BUILD


#define timerPeriod 1
#define tasksNum 1

#include <avr/io.h>
#include <avr/interrupt.h>
#include "scheduler.h"

// unsigned char r_data1, r_data2, r_data3, counter = 0x00; // USART variables
// 
// unsigned char carSpeed = 0; // 00 - stopped, 01 - creep, 10 - medium, 11 - fast
// unsigned char carXAxis = 0; // 10 - left, 00 - straight, 01 - right
// unsigned char carYAxis = 0; // 0 - forward, 1 - reverse
// 
// unsigned char carValues = 0x00; // bits 0-1 are speed, 2-3 are left/right, 4 is forward/reverse

// Code provided by UCR for ADC

// Test harness for Ultrasonic Sensors
enum car_States {synch} car_state;
int TickFct_carState(int state)
{
	switch(car_state)
	{
		case synch:
			PORTC = 0x01;
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
	DDRA = 0xFF; PORTA = 0x00; // ADC input currently set backwards please switch   
	DDRB = 0xFF; PORTB = 0x00; // Output to column sel
	DDRC = 0xFE; PORTD = 0x01; // C0 is input C1 (and others) is output
	// Input from RF Receiver will be received from RX1
	
	TimerSet(timerPeriod);
	TimerOn();
	//A2D_init();
	
	unsigned char i = 0;
	tasks[i].state = -1;
	tasks[i].period = 1; // Should be .01ms instead of 1 for optimal performance
	tasks[i].elapsedTime = 0;
	tasks[i].TickFct = &TickFct_carState;
	
	while (1)
	{
	}
}