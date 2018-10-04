/*	Partner(s) Name & E-mail: Jonathan Woolf jwool003@ucr.edu
 *	Lab Section: tues thursday 2-5
 *	Assignment: Lab #1  Exercise #2 
 *	Exercise Description: [optional - include for your own benefit]
 *	
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */


#include "rims.h"

/*Define user variables and functions for this state machine here.*/
unsigned char SM1_Clk;
void TimerISR() {
   SM1_Clk = 1;
}

enum SM1_States { SM1_B0, SM1_B1, SM1_B2, SM1_B3, SM1_B4, SM1_B5, SM1_B6, SM1_B7, wait1 } SM1_State;

TickFct_State_machine_1() {
   switch(SM1_State) { // Transitions
      case -1:
         SM1_State = SM1_B0;
         break;
         case SM1_B0: 
         if (A0 == 0) {
            SM1_State = SM1_B1;
         }
         else if (A0 == 1) {
            SM1_State = SM1_B7;
         }
         if(A7 != 0 || A6 != 0) {SM1_State = wait1;}
         break;
      case SM1_B1: 
         if (A0 == 0) {
            SM1_State = SM1_B2;
         }
         else if (A0 == 1) {
            SM1_State = SM1_B0;
         }
         if(A7 != 0 || A6 != 0) {SM1_State = wait1;}
         break;
      case SM1_B2: 
         if (A0 == 0) {
            SM1_State = SM1_B3;
         }
         else if (A0 == 1) {
            SM1_State = SM1_B1;
         }
         if(A7 != 0 || A6 != 0) {SM1_State = wait1;}
         break;
      case SM1_B3: 
         if (A0 == 0) {
            SM1_State = SM1_B4;
         }
         else if (A0 == 1) {
            SM1_State = SM1_B2;
         }
         if(A7 != 0 || A6 != 0) {SM1_State = wait1;}
         break;
      case SM1_B4: 
         if (A0 == 0) {
            SM1_State = SM1_B5;
         }
         else if (A0 == 1) {
            SM1_State = SM1_B3;
         }
         if(A7 != 0 || A6 != 0) {SM1_State = wait1;}
         break;
      case SM1_B5: 
         if (A0 == 0) {
            SM1_State = SM1_B6;
         }
         else if (A0 == 1) {
            SM1_State = SM1_B4;
         }
         if(A7 != 0 || A6 != 0) {SM1_State = wait1;}
         break;
      case SM1_B6: 
         if (A0 == 0) {
            SM1_State = SM1_B7;
         }
         else if (A0 == 1) {
            SM1_State = SM1_B5;
         }
         if(A7 != 0 || A6 != 0) {SM1_State = wait1;}
         break;
      case SM1_B7: 
         if (A0 == 0) {
            SM1_State = SM1_B0;
         }
         else if (A0 == 1) {
            SM1_State = SM1_B6;
         }
         if(A7 != 0 || A6 != 0) {SM1_State = wait1;}
         break;
      case wait1:
         SM1_State = wait1;
         if(A7 == 0 && A6 == 0) {SM1_State = SM1_B0;}
         break;
      default:
         SM1_State = SM1_B0;
   } // Transitions

   switch(SM1_State) { // State actions
      case SM1_B0:
         B = 1;
         break;
      case SM1_B1:
         B = 2;
         break;
      case SM1_B2:
         B = 4;
         break;
      case SM1_B3:
         B = 8;
         break;
      case SM1_B4:
         B = 16;
         break;
      case SM1_B5:
         B = 32;
         break;
      case SM1_B6:
         B = 64;
         break;
      case SM1_B7:
         B = 128;
         break;
      default: // ADD default behaviour below
      break;
   } // State actions

}

const unsigned char array[10] = {0x81, 0x42, 0xE7, 0x91, 0x77, 0x18, 0xEA, 0x62, 0x35, 0xC3};

enum SM2_States { SM2_B0, wait2 } SM2_State;

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
         SM2_State = wait2;
         if(A7 == 0 && A6 == 1) {SM2_State = SM2_B0;}
         break;
      case wait2:
         SM2_State = wait2;
         if(A7 == 0 && A6 == 1) {SM2_State = SM2_B0;}
         break;
      default:
         SM2_State = SM2_B0;
   } // Transitions
   return SM2_State;
}

const unsigned char array2[10] = {0x81, 0x42, 0x24, 0x18, 0xFF};

enum SM3_States { music, wait3 } SM3_State;

TickFct_State_machine_3() {
   static unsigned char i = 0;
   switch(SM3_State) { // Transitions
      case -1:
         SM3_State = music;
         break;
      case SM2_B0: 
         B = array2[i];
         i++;
         if(i >= 5) {i = 0;}
         SM3_State = wait2;
         if(A7 == 1 && A6 == 0) {SM3_State = music;}
         break;
      case wait3:
         SM3_State = wait3;
         if(A7 == 1 && A6 == 0) {SM3_State = music;}
         break;
      default:
         SM3_State = music;
   } // Transitions
   return SM3_State;
}
enum SM4_States { main4, wait4 } SM4_State;

TickFct_State_machine_4() {
   static unsigned char i = 0;
   switch(SM4_State) { // Transitions
      case -1:
         SM4_State = main4;
         break;
      case main4: 
         B = 0xFF;
         SM4_State = wait4;
         if(A7 == 1 && A6 == 1) {SM4_State = main4;}
         break;
      case wait4:
         SM4_State = wait4;
         if(A7 == 1 && A6 == 1) {SM4_State = main4;}
         break;
      default:
         SM4_State = main4;
   } // Transitions
   return SM4_State;
}

int main() {
   const unsigned int periodState_machine_1 = 1000; // 1000 ms default
   const unsigned int periodState_machine_2 = 1000; // 1000 ms default
   const unsigned int periodState_machine_3 = 500;
   TimerSet(periodState_machine_2);
   TimerOn();
   
   SM1_State = -1;
   SM2_State = -1; // Initial state
   SM3_State = -1;
   SM4_State = -1;
   B = 0; // Init outputs

   while(1) {
      TickFct_State_machine_1();
      TickFct_State_machine_2();
      TickFct_State_machine_3();
      TickFct_State_machine_4();
      while(!SM1_Clk);
      SM1_Clk = 0;
   } // while (1)
} // Main

