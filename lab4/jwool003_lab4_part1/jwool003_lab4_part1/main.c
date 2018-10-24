#include <stdint.h> 
#include <stdlib.h> 
#include <stdio.h> 
#include <stdbool.h> 
#include <string.h> 
#include <math.h> 
#include <avr/io.h> 
#include <avr/interrupt.h> 
#include <avr/eeprom.h> 
#include <avr/portpins.h> 
#include <avr/pgmspace.h> 
 
//FreeRTOS include files 
#include "FreeRTOS.h" 
#include "task.h" 
#include "croutine.h" 
enum BLINKState {INIT,on} blink_state;

void blink_Init(){
	blink_state = INIT;
}

void blink_Tick(){
	//Actions
	switch(blink_state){
		case INIT:
		PORTD = 0;
		break;
		case on:
		PORTD = 0x15;
		break;
		default:
		PORTD = 0;
		break;
	}
	//Transitions
	switch(blink_state){
		case INIT:
			blink_state = on;
		break;
		case on:
			blink_state = INIT;
		break;
		default:
			blink_state = INIT;
		break;
	}
}

void LedSecTask()
{
	blink_Init();
   for(;;) 
   { 	
	blink_Tick();
	vTaskDelay(1000); 
   } 
}

void StartSecPulse(unsigned portBASE_TYPE Priority)
{
	xTaskCreate(LedSecTask, (signed portCHAR *)"LedSecTask", configMINIMAL_STACK_SIZE, NULL, Priority, NULL );
}	
 
int main(void) 
{ 
   DDRA = 0x00; PORTA=0xFF;
   DDRD = 0xFF;
   //Start Tasks  
   StartSecPulse(1);
    //RunSchedular 
   vTaskStartScheduler(); 
 
   return 0; 
}