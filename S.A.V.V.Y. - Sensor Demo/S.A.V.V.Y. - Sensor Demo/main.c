/*
 * S.A.V.V.Y. - Sensor Demo.c
 *
 * Created: 11/21/2018 3:33:06 PM
 * Author : Jonathan
 */ 


// CHECKNG WITHIN 20CM FOR FINAL BUILD


#define timerPeriod 1
#define tasksNum 1
#define F_CPU 8000000UL // 8mhz microcontroller

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "scheduler.h"

#define ECHO 3 // input to listen for echo uses -INT0- External Interrupt Request 0.
#define TRIGGER 3 // output triggers sensor
#define OBJECT_DETECTED 5 // output pin for when an object is detected

volatile unsigned short distance;
volatile unsigned short distance1;
volatile unsigned short distance2;
volatile unsigned short distance3;
volatile char stop_flag = 0; 
void EnableDistance() {
	SREG |= 0x80; // enable global interrupts

	EICRA |= (1 << ISC10); // set interrupt to trigger on logic change
	EICRA &= ~(1 << ISC11); // set interrupt to trigger on logic change
	
	EIMSK |= (1 << INT1); // enable external interrupt 1 (PD3)
	
	// set sensor trigger pin as output
	DDRA |= (1 << TRIGGER);
	PORTA &= ~(1 << TRIGGER);
	// set sensor echo pin as input, enable pull-up
	DDRD &= ~(1 << ECHO);
	PORTD |= (1 << ECHO);
	// set sensor output pin as output
	DDRA |= (1 << OBJECT_DETECTED);
	PORTA &= ~(1 << OBJECT_DETECTED);
}

// Returns the distance in centimeters
unsigned short distanceCM() {
	PORTA |= (1 << TRIGGER); // set trigger pin high
	_delay_us(10);
	PORTA &= ~(1 << TRIGGER); // set trigger pin low
	return sonar/58; // 2 (pulse travels there and back) * 10000cm (max range of pulse) / 343 (speed of sound) = 58
}


// Test harness for Ultrasonic Sensors
enum sonar_States {sonarPulse};
int TickFct_sonarState(int state)
{
	switch(state)
	{
		case sonarPulse://For ULTRASONAR SENSOR
			if(stop_flag == 1)
			{
				PORTB = 0x01;
			}
			else { PORTB = 0x02;}
			state = sonarPulse;
			break;
		default:
			state = sonarPulse;
			break;
	}
	return state;
}

int main(void)
{
	DDRA = 0xFF; PORTA = 0x00; // Output for trigger pin
	DDRB = 0xFF; PORTB = 0x00; // Output to motors and lights
	DDRD = 0xFF; PORTD = 0x00; // Output to motors
	
	EnableDistance();
	TimerSet(timerPeriod);
	TimerOn();
	
	unsigned char i = 0;
	tasks[i].state = -1;
	tasks[i].period = 1; // Should be .01ms instead of 1 for optimal performance
	tasks[i].elapsedTime = 0;
	tasks[i].TickFct = &TickFct_sonarState;
	
	while (1)
	{
		distance1 = distanceCM();
		distance2 = distanceCM();
		distance3 = distanceCM();
		
		distance = (distance1 + distance2 + distance3) / 3;
		if(distance <= 20) { stop_flag = 1;}
		else { stop_flag = 0;}		 		 
	}
}
