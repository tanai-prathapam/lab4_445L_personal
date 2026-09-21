// Lab4Fmain.c
// Runs on MSPM0
// Uses ST7735.c to display the clock
// patterns to the LCD.
//    16-bit color, 128 wide by 160 high LCD
// Mark McDermott, Daniel Valvano and Jonathan Valvano
// June 27, 2026

// Specify your hardware connections, feel free to change
// PB4 is squarewave output to speaker
// PA18 is mode select (on Launchpad)
// PA27 is left
// PA15 is right
// PA28 is up
// PA16 is down
// if alarm is sounding, any button will quiet the alarm

#include <ti/devices/msp/msp.h>
#include <stdio.h>
#include <stdlib.h>
#include "../inc/LaunchPad.h"
#include "../inc/Clock.h"
#include "../inc/SPI.h"
#include "../inc/ST7735_SDC.h"
#include "../inc/Timer.h"
#include "../inc/UART.h"
#include "Lab3.h"



void RESET_8266(void){
// Student writes this

}

void SETUP_WIFI(void) {
// Student writes this


}



//------------UART1_OutString------------
// Output String (NULL termination)
// Input: pointer to a NULL-terminated string to be transferred
// Output: none
//
void UART1_OutString(char *pt){
     // ****ECE445L write this ****
  
}
//-----------------------UART1_OutUDec-----------------------
// Output a 32-bit number in unsigned decimal format
// Input: 32-bit number to be transferred
// Output: none
// Variable format 1-10 digits with no space before or after
//
void UART1_OutUDec(uint32_t n){
// This function uses recursion to convert decimal number
//   of unspecified length as an ASCII string
   // ****ECE445L write this ****
  
}


void Input(void){
// Student writes this

}
  



void TIMG0_IRQHandler(void){// runs every 1ms
  if((TIMG0->CPU_INT.IIDX) == 1){ // this will acknowledge
    GPIOB->DOUTTGL31_0 = BLUE;  // profile PB22
    GPIOB->DOUTTGL31_0 = BLUE;  // profile PB22
// Student writes this


     GPIOB->DOUTTGL31_0 = BLUE;  // profile PB22
  }
}

// ----------------------------------------------------------------------------
// ----------------------       MAIN       ------------------------------------
// ----------------------------------------------------------------------------
int main(void){
  __disable_irq(); 

  LaunchPad_Init();
  Clock_Init_HFXT_40_80MHz(0);  // 0.005% accurate running off external crystal oscillator
  UART_Init();        // Setup Debug UART port
  RESET_8266();       // Reset the WiFi chip
  SETUP_WIFI();       // Setup the Wifi channel and Wait for RDY Signal
  UART1_Init();       // UART channel to ESP8266

  ST7735_InitR(INITR_REDTAB); //INITR_REDTAB for AdaFruit, INITR_BLACKTAB for SPI HiLetgo ST7735R
  ST7735_FillScreen(ST7735_BLACK);
  ST7735_SetCursor(0, 0); 

 // Switch_Init();          
  Speaker_Init();         // PB4 squarewave to speaker
  LED_Init();
  
  
  __enable_irq();

  while(1){               // interrupts every 1ms


// Build CSV string to send to Web Application
// Student writes this


     


    __WFI();
  }
}

void Speaker_Init(void){
      // ****ECE445L write this ****

 
}

void LED_Init(void){
    // write this
    
}
void LED_Out(uint32_t data){
    // write this
    
}
void LED_Toggle(void){
    // write this
    
}
