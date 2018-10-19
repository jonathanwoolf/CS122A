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

#include <avr/io.h>
#include "spi.h"
#include "bit.h"
#include "lcd.h"
#include "keypad.h"
#include "scheduler.h"

enum state {spi_start, spi_transmit};

unsigned char key,data = 0;

int spi(int state){
	
	key = GetKeypadKey();
	
	switch(state){
	    case spi_start:
	    	data = 0;
	        state = spi_transmit;
	        break;
	    case spi_transmit:
	    	LCD_ClearScreen();
	    	
			if(key == '1'){
				data = 0x11;
				LCD_DisplayString(1, "Ptrn:1");
				LCD_DisplayString(10, "Spd:1");
			}
			else if(key == '4'){
				data = 0x12;
				LCD_DisplayString(1, "Ptrn:1");
				LCD_DisplayString(10, "Spd:2");
			}
			else if(key == '7'){
				data = 0x13;
				LCD_DisplayString(1, "Ptrn:1");
				LCD_DisplayString(10, "Spd:3");
			}
			else if(key == '*'){
				data = 0x14;
				LCD_DisplayString(1, "Ptrn:1");
				LCD_DisplayString(10, "Spd:4");
			}
			else if(key == '2'){
				data = 0x21;
				LCD_DisplayString(1, "Ptrn:2");
				LCD_DisplayString(10, "Spd:1");
			}
			else if(key == '5'){
				data = 0x22;
				LCD_DisplayString(1, "Ptrn:2");
				LCD_DisplayString(10, "Spd:2");
			}
			else if(key == '8'){
				data = 0x23;
				LCD_DisplayString(1, "Ptrn:2");
				LCD_DisplayString(10, "Spd:3");
			}
			else if(key == 0){
				data = 0x24;
				LCD_DisplayString(1, "Ptrn:2");
				LCD_DisplayString(10, "Spd:4");
			}
			else if(key == '3'){
				data = 0x31;
				LCD_DisplayString(1, "Ptrn:3");
				LCD_DisplayString(10, "Spd:1");
			}
			else if(key == '6'){
				data = 0x32;
				LCD_DisplayString(1, "Ptrn:3");
				LCD_DisplayString(10, "Spd:2");
			}
			else if(key == '9'){
				data = 0x33;
				LCD_DisplayString(1, "Ptrn:3");
				LCD_DisplayString(10, "Spd:3");
			}
			else if(key == '#'){
				data = 0x34;
				LCD_DisplayString(1, "Ptrn:3");
				LCD_DisplayString(10, "Spd:4");
			}
			else if(key == 'A'){
				data = 0x41;
				LCD_DisplayString(1, "Ptrn:4");
				LCD_DisplayString(10, "Spd:1");
			}
			else if(key == 'B'){
				data = 0x42;
				LCD_DisplayString(1, "Ptrn:4");
				LCD_DisplayString(10, "Spd:2");
			}
			else if(key == 'C'){
				data = 0x43;
				LCD_DisplayString(1, "Ptrn:4");
				LCD_DisplayString(10, "Spd:3");
			}
			else if(key == 'D'){
				data = 0x44;
				LCD_DisplayString(1, "Ptrn:4");
				LCD_DisplayString(10, "Spd:4");
			}
			else{
				data = data;
			}
			
			SPI_MasterTransmit(data);
	        
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
	DDRC = 0xF0; PORTC = 0x0F;
	
	unsigned char i = 0;
	tasks[i].state = spi_start;
	tasks[i].period = 100;
	tasks[i].elapsedTime = 0;
	tasks[i].TickFct = spi;
	
    /* Replace with your application code */
	TimerSet(timerPeriod);
	TimerOn();
	
	SPI_MasterInit();
    LCD_init();
    LCD_ClearScreen();
    
    while (1) 
    {
    }
}

