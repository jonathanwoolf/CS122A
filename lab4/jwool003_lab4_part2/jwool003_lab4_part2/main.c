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
enum BLINK0State {INIT,on} blink0_state;

void blink0_Init(){
	blink0_state = INIT;
}

void blink0_Tick(){
	//Actions
	switch(blink0_state){
		case INIT:
		PORTD = (PORTD & 0xFE);
		break;
		case on:
		PORTD |= 1;
		break;
		default:
		PORTD = 0;
		break;
	}
	//Transitions
	switch(blink0_state){
		case INIT:
		blink0_state = on;
		break;
		case on:
		blink0_state = INIT;
		break;
		default:
		blink0_state = INIT;
		break;
	}
}

enum BLINKS1tate {off,four} blink1_state;

void blink1_Init(){
	blink1_state = off;
}

void blink1_Tick(){
	//Actions
	switch(blink1_state){
		case off:
		PORTD = (PORTD & 0xFB);
		break;
		case four:
		PORTD |= 4;
		break;
		default:
		PORTD = 0;
		break;
	}
	//Transitions
	switch(blink1_state){
		case off:
		blink1_state = four;
		break;
		case four:
		blink1_state = off;
		break;
		default:
		blink1_state = off;
		break;
	}
}

enum BLINKS2tate {no,yes} blink2_state;

void blink2_Init(){
	blink2_state = no;
}

void blink2_Tick(){
	//Actions
	switch(blink2_state){
		case no:
		PORTD = (PORTD & 0xEF);
		break;
		case yes:
		PORTD |= 16;
		break;
		default:
		PORTD = 0;
		break;
	}
	//Transitions
	switch(blink2_state){
		case no:
		blink2_state = yes;
		break;
		case yes:
		blink2_state = no;
		break;
		default:
		blink2_state = no;
		break;
	}
}

void LedSecTask()
{
	blink0_Init();
	
	for(;;)
	{
		blink0_Tick();
		vTaskDelay(500);
	}
}

void LedSecTask1()
{
	blink1_Init();
	
	for(;;)
	{
		blink1_Tick();
		vTaskDelay(1000);
	}
}

void LedSecTask2()
{
	blink2_Init();
	
	for(;;)
	{
		blink2_Tick();
		vTaskDelay(2500);
	}
}


void StartSecPulse(unsigned portBASE_TYPE Priority)
{
	xTaskCreate(LedSecTask, (signed portCHAR *)"LedSecTask", configMINIMAL_STACK_SIZE, NULL, Priority, NULL );
	xTaskCreate(LedSecTask1, (signed portCHAR *)"LedSecTask1", configMINIMAL_STACK_SIZE, NULL, Priority, NULL );
	xTaskCreate(LedSecTask2, (signed portCHAR *)"LedSecTask2", configMINIMAL_STACK_SIZE, NULL, Priority, NULL );
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