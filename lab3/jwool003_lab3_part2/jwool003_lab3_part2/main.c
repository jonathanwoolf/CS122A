  /*	Partner(s) Name & E-mail: Jonathan Woolf, Adriel Bustamante, Rohit Joshi
 *	Lab Section: 023
 *	Assignment: Lab #3 Exercise #2 
 *	Exercise Description: [optional - include for your own benefit]
 *	
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */

#define timerPeriod 100
#define tasksNum 5

#include <avr/io.h>
#include "spi.h"
#include "lcd.h"
#include "scheduler.h"

void speedSet(char speed, int task){
	if(speed == 0x01){
		tasks[task].period = 400;	
	}
	else if(speed == 0x03){
		tasks[task].period =  200;	
	}
	else if(speed == 0x04){
		tasks[task].period = 100;
	}
	else {
		tasks[task].period = 300;	
	}
}

unsigned char i, flag, pattern, speed = 0;

enum spi_states{spi_start, spi_receive};

int spi_tick(int state){
	
	pattern = (receivedData & 0xF0);
	speed = (receivedData & 0x0F);
    
    switch(state){
        case spi_start:
            flag = 0;
            state = spi_receive;
        	break;
        case spi_receive:
        	if(pattern == 0x10){
        		flag = 1;
        		speedSet(speed, 1);
        	}
        	else if(pattern == 0x20){
        		flag = 2;
        		speedSet(speed, 2);
        	}
        	else if(pattern == 0x30){
        		flag = 3;
        		speedSet(speed, 3);
        	}
        	else if(pattern == 0x40){
        		flag = 4;
        		speedSet(speed, 4);
        	}
        	state = spi_receive;
        break;
        
        default:
        	flag = 0;
            state = spi_start;
        break;
    }
    return state;
}

enum F1_states {F1_start, F1_flash};

int F1_tick(int state){
	switch(state){
	    case F1_start:
	    	i = 0xF0;
	        state = F1_flash;
	        break;
	    case F1_flash:
	        i = 0x0F;
	        state = F1_start;
	        break;
	    default:
	        state = F1_start;
	        break;
	}
	//if(flag == 1){
		PORTC = i;
	//}
    return state;
}

enum F2_states {F2_start, F2_flash};

int F2_tick(int state){
	switch(state){

	    case F2_start:
	    	i = 0xAA;
	        state = F2_flash;
	        break;
	        
	    case F2_flash:
			i = 0x55;
	        state = F2_start;
	        break;
	        
	        break;
	    default:
	        state = F2_start;
	        break;
	}
	if(flag == 2){
		PORTC = i;
	}
    return state;
}

enum F3_states {F3_start, F3_flash};

int F3_tick(int state){
	switch(state){
	    case F3_start:
	    	i = 0x7E;
	        state = F3_flash;
	        break;
	    case F3_flash:
			i = 0x81;
	        state = F3_start;
	        break;
	    default:
	        state = F3_start;
	        break;
	}
	if(flag == 3){
		PORTC = i;
	}
    return state;
}

enum F4_states {F4_start, F4_flash};

int F4_tick(int state){
	switch(state){
	    case F4_start:
	    	i = 0x69; // UwU what's this?
	        state = F4_flash;
	        break;
	    case F4_flash:
			i = 0x96;
	        state = F4_start;
	        break;
	    default:
	        state = F4_start;
	        break;
	}
	if(flag == 4){
		PORTC = i;
	}
    return state;
}



int main(void)
{	
	DDRC = 0xFF; PORTC = 0x00;
	DDRD = 0xFF; PORTD = 0x00;
	
	unsigned char i = 0;
	tasks[i].state = spi_start;
	tasks[i].period = 100;
	tasks[i].elapsedTime = 0;
	tasks[i].TickFct = spi_tick;
	++i;
	tasks[i].state = F1_start;
	tasks[i].period = 300;
	tasks[i].elapsedTime = 0;
	tasks[i].TickFct = F1_tick;
	++i;
	tasks[i].state = F2_start;
	tasks[i].period = 300;
	tasks[i].elapsedTime = 0;
	tasks[i].TickFct = F2_tick;
	++i;
	tasks[i].state = F3_start;
	tasks[i].period = 300;
	tasks[i].elapsedTime = 0;
	tasks[i].TickFct = F3_tick;
	++i;
	tasks[i].state = F4_start;
	tasks[i].period = 300;
	tasks[i].elapsedTime = 0;
	tasks[i].TickFct = F4_tick;
	
	TimerSet(timerPeriod);
	TimerOn();
	SPI_ServantInit();

    while (1) 
    {
    }
}

