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
#include "../lib/UART.h"
#include "../inc/FIFO.h"
#include "../lib/FIFO.h"
#include "Lab3.h"

// Global Clock State
volatile uint8_t Mode = 0;   // 0 = 12-hour, 1 = 24-hour
volatile uint8_t Hour = 12;
volatile uint8_t Minute = 0;
volatile uint8_t Second = 0;

// Flag to trigger Wi-Fi transmission
volatile uint8_t Send_Flag = 0;

#define ESP8266_RST (1<<25)  // PA25

void RESET_8266(void){
  // Initialize reset port on PA25 as GPIO Output
  IOMUX->SECCFG.PINCM[PA25INDEX] = 0x81;     // GPIO mode
  GPIOA->DOE31_0 |= ESP8266_RST;             // Enable output on PA25
  
  // Generate a reset signal to ESP8266 on Port PA25
  GPIOA->DOUTCLR31_0 = ESP8266_RST;          // Drive PA25 LOW (Active reset)
  Clock_Delay1ms(10);                        // Hold reset low for 10ms
  GPIOA->DOUTSET31_0 = ESP8266_RST;          // Drive PA25 HIGH (Release reset)
  
  // Give the ESP8266 time to initialize before asking if it is ready
  Clock_Delay1ms(500); 

  UART1_OutString("\n\rResetting the ESP8266\n\r");
}

#define ESP8266_RDY (1<<19)  // PB19

void SETUP_WIFI(void) {
  // Setup RDY input on PB19
  // 0x00040081 sets INENA (Input Enable) and PF=1 (GPIO)
  IOMUX->SECCFG.PINCM[PB19INDEX] = 0x00040081; 

  UART1_OutString("\n\rWaiting for WiFi RDY signal to be asserted\n\r");

  // Loop until RDY input is valid 
  while((GPIOB->DIN31_0 & ESP8266_RDY) == 0){
      // Spin and wait. The MSPM0 halts here until the Wi-Fi connects.
  }
      
  UART1_OutString("\n\rRDY signal asserted\n\r");
}

void Input(void){
  // Student writes this
  char ws_cmd;
  
  if(RxFifo1_Size() > 0){ // if unread data in  FIFO
    ws_cmd = RxFifo1_Get(); //get next character (oldest)
    UART1_OutChar(ws_cmd); //debug uart
    uint8_t cmd_num = ws_cmd - '0';   // ASCII to int conversion
  
    // Command #1: Toggle MODE select 
    if(cmd_num == 0x1)  {  
        if (Mode == 0x1) Mode =0x0;
        else if (Mode == 0x0) Mode = 0x1;
    }

    // Command #2: Hour ++
    else if (cmd_num == 0x2) {
      Hour = (Hour + 1) % 24;
    }  

    // Command #3: Hour --
    else if (cmd_num == 0x3) {
      if (Hour == 0) Hour = 23;
      else Hour--;
    }

    // Command #4: Min ++
    else if (cmd_num == 0x4) {
      Minute = (Minute + 1) % 60;
    }

    // Command #5: Min --
    else if (cmd_num == 0x5) {
      if (Minute == 0) Minute = 59;
      else Minute--;
    }

    // Command #6: Sec ++
    else if (cmd_num == 0x6) {
      Second = (Second + 1) % 60;
    }

    // Command #7: Sec --
    else if (cmd_num == 0x7) {
      if (Second == 0) Second = 59;
      else Second--;
    }
  }
}
  

void TIMG0_IRQHandler(void){// runs every 1ms
  if((TIMG0->CPU_INT.IIDX) == 1){ // this will acknowledge
    GPIOB->DOUTTGL31_0 = BLUE;  // profile PB22
    GPIOB->DOUTTGL31_0 = BLUE;  // profile PB22
    
    // Student writes this
    // Static variable retains its value between interrupts
    static uint16_t ms_count = 0;
    ms_count++;
    
    // Update main ever second: Check if 1000 milliseconds (1 second) have passed
    if(ms_count >= 1000){
      ms_count = 0;
        
        //  Lab 3 Clock Logic goes here 
        // Second++; 
        // if(Second >= 60) { ... Minute++; ... }
        
      Send_Flag = 1; //broadcast new time to main (update)
    }
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

    // Student writes this
    if (Send_Flag == 1) {
      char csv_string[32];
      
      // Build CSV string: "Mode,Hour,Minute,Second\n"
      sprintf(csv_string, "%d,%d,%d,%d\n", Mode, Hour, Minute, Second);
      
      // Send to ESP8266 via UART1
      UART1_OutString(csv_string);
      
      Send_Flag = 0; 
    }

    Input(); // Poll the FIFO for incoming web clicks
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
