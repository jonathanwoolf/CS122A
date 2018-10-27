/*
 * jwool003_lab6_part1.c
 *
 * Created: 10/25/2018 3:55:42 PM
 * Author : Jonathan
 */ 
#define timerPeriod 1
#define tasksNum 1

#include <avr/io.h>
#include "scheduler.h"

void A2D_init() {
	ADCSRA |= (1 << ADEN) | (1 << ADSC) | (1 << ADATE);
	// ADEN: Enables analog-to-digital conversion
	// ADSC: Starts analog-to-digital conversion
	// ADATE: Enables auto-triggering, allowing for constant
	//	    analog to digital conversions.
}


int main(void)
{
	
    A2D_init();
	
    while (1) 
    {
    }
}

