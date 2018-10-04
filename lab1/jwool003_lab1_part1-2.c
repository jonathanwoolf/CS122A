/*
This code was automatically generated using the Riverside-Irvine State machine Builder tool
Version 2.7 --- 10/2/2018 22:23:25 PST
*/

#include "rims.h"

/*Define user variables and functions for this state machine here.*/
unsigned char SM1_Clk;
void TimerISR() {
   SM1_Clk = 1;
}

const unsigned char array[10] = {0x81, 0x42, 0xE7, 0x91, 0x77, 0x18, 0xEA, 0x62, 0x35, 0xC3};

enum SM2_States { SM2_B0 } SM2_State;

TickFct_State_machine_2() {
   static unsigned char i = 0;
   switch(SM2_State) { // Transitions
      case -1:
         SM2_State = SM2_B0;
         break;
         case SM2_B0: 
         B = array[i];
         if (A0 == 0) {
            i++;
            if(i == 10) {i = 0;}
         }
         else if (A0 == 1) {
            i--;
            if(i < 0) {i = 9;}
         }
         SM2_State = SM2_B0;
         break;
     
      default:
         SM2_State = SM2_B0;
   } // Transitions
   return SM2_State;
}

int main() {

   const unsigned int periodState_machine_2 = 1000; // 1000 ms default
   TimerSet(periodState_machine_2);
   TimerOn();
   
   SM2_State = -1; // Initial state
   B = 0; // Init outputs

   while(1) {
      TickFct_State_machine_2();
      while(!SM1_Clk);
      SM1_Clk = 0;
   } // while (1)
} // Main