/*	Partner(s) Name & E-mail: Jonathan Woolf, Adriel Bustamante, Rohit Joshi
 *	Lab Section: 023
 *	Assignment: Lab #2 Exercise #1 
 *	Exercise Description: [optional - include for your own benefit]
 *	
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */

#define timerPeriod 100
#define tasksNum 1
#define master 1 //decides if micro-controller is master slave

#include <avr/io.h>
#include "spi.h"
#include "bit.h"
#include "lcd.h"
#include "keypad.h"
#include "scheduler.h"

enum state {spi_start, spi_transmit};

unsigned char i = 0;
//unsigned char tmpC = 0x00;

int spi(int state){
	switch(state){
	    case spi_start:
	    	i = 0;
	        state = spi_transmit;
	        break;
	    case spi_transmit:
			if (master == 1){
				if(i >= 0xFF){
					i = 0;
				}
				else if(i < 0xFF)
				{
					i++;
				}
	    		SPI_MasterTransmit(i); 
		    }
	    	else if (master == 0) {
				PORTA = receivedData;
	    	}
	        state = spi_transmit;
	        break;
	    default:
	        state = spi_start;
	        break;
	}
    return state;
}


int main(void)
{	
	DDRA = 0xFF; PORTA = 0x00;
	
	unsigned char i = 0;
	tasks[i].state = spi_start;
	tasks[i].period = 200;
	tasks[i].elapsedTime = 0;
	tasks[i].TickFct = spi;
	
    /* Replace with your application code */
	TimerSet(timerPeriod);
	TimerOn();
	if (master == 1){
		SPI_MasterInit();
	}
	if (master == 0){
		SPI_ServantInit();
	}
    while (1) 
    {
    }
}

