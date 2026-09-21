// Matrix.c
// Runs on  MSPM0
// Provide functions that initialize GPIO ports and SysTick 
// Use periodic polling
// Jonathan Valvano
// July 18, 2026

/* This example accompanies the book
   "Embedded Systems: Real Time Interfacing to Arm Cortex M Microcontrollers",
   ISBN: 978-1463590154, Jonathan Valvano, copyright (c) 2026

   

Copyright 2026 by Jonathan W. Valvano, valvano@mail.utexas.edu
    You may use, edit, run or distribute this file
    as long as the above copyright notice remains
 THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 VALVANO SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL,
 OR CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 For more information about my classes, my research, and my books, see
 http://users.ece.utexas.edu/~valvano/
 */

// PA27 connected to column 3 (keypad pin 4) using 10K pull-up
// PA26 connected to column 2 (keypad pin 3) using 10K pull-up
// PA25 connected to column 1 (keypad pin 2) using 10K pull-up
// PA24 connected to column 0 (keypad pin 1) using 10K pull-up
// PB3 connected to row 3 (keypad pin 8)
// PB2 connected to row 2 (keypad pin 7)
// PB1 connected to row 1 (keypad pin 6) 
// PB0 connected to row 0 (keypad pin 5)

// [1] [2] [3] [A]
// [4] [5] [6] [B]
// [7] [8] [9] [C]
// [*] [0] [#] [D]
// Pin1 . . . . . . . . Pin8
// Pin 1 -> Column 0 (column starting with 1)
// Pin 2 -> Column 1 (column starting with 2)
// Pin 3 -> Column 2 (column starting with 3)
// Pin 4 -> Column 3 (column starting with A)
// Pin 5 -> Row 0 (row starting with 1)
// Pin 6 -> Row 1 (row starting with 4)
// Pin 7 -> Row 2 (row starting with 7)
// Pin 8 -> Row 3 (row starting with *)

#include <stdint.h>
#include "../inc/FIFO.h"
#include <ti/devices/msp/msp.h>
#include "../inc/LaunchPad.h"
#include "../inc/Clock.h"


uint32_t HeartBeat;  // incremented every 25 ms


void SysTick_IntArm(uint32_t period, uint32_t priority){
  SysTick->CTRL  = 0x00;      // disable during initialization
  SysTick->LOAD  = period-1;  // set reload register
  //The ARM Cortex-M0+ only implements the most significant 2 bits of each 8-bit priority field (giving the 4 priority levels).
  SCB->SHP[1]    = (SCB->SHP[1]&(~0xC0000000))|priority<<30;    // set priority (bits 31,30)
  SysTick->VAL   = 0;         // clear count, cause reload
  SysTick->CTRL  = 0x07;      // Enable SysTick IRQ and SysTick Timer
}


// Initialization of Matrix keypad
void MatrixKeypad_Init(void){ 
// assumes LaunchPad_Init has been called
// PINCM
//   bit 25 is HiZ
//   bit 20 is drive strength (only available for PA10 PA11 PA28 PA31)
//   bit 18 is input enable control
//   bit 17 is pull up control
//   bit 16 is pull down control
//   bit 7 is PC peripheral connected, enable transparent data flow
//   bit 0 selects GPIO function
  IOMUX->SECCFG.PINCM[PB0INDEX] = 0x02000081; // HiZ GPIO output
  IOMUX->SECCFG.PINCM[PB1INDEX] = 0x02000081; // HiZ GPIO output
  IOMUX->SECCFG.PINCM[PB2INDEX] = 0x02000081; // HiZ GPIO output
  IOMUX->SECCFG.PINCM[PB3INDEX] = 0x02000081; // HiZ GPIO output
  GPIOB->DOE31_0 |= 0x0F;     // enable output PB3,2,1,0
  GPIOB->DOUTSET31_0 = 0x0F;  // turn them all off
  IOMUX->SECCFG.PINCM[PA27INDEX] = 0x00060081; // input, pull up
  IOMUX->SECCFG.PINCM[PA26INDEX] = 0x00060081; // input, pull up
  IOMUX->SECCFG.PINCM[PA25INDEX] = 0x00060081; // input, pull up
  IOMUX->SECCFG.PINCM[PA24INDEX] = 0x00060081; // input, pull up
}



struct Row{
  uint32_t PinsToTurnOn;  // output to select row
  uint32_t PinsToTurnOff; // output to deselect row
  char keycode[4];};
typedef const struct Row Row_t;
Row_t ScanTab[5]={
{   0x01, 0x0E, "123A" }, // row 0
{   0x02, 0x0D, "456B" }, // row 1
{   0x04, 0x0B, "789C" }, // row 2
{   0x08, 0x07, "*0#D" }, // row 3
{   0x00, 0x00, "    " }};

/* Returns ASCII code for key pressed,
   Num is the number of keys pressed
   both equal zero if no key pressed */
char MatrixKeypad_Scan(int32_t *Num){
  Row_t *pt;
  char column, key;
  int32_t j;
  (*Num) = 0;
  key = 0;    // default values
  pt = &ScanTab[0];
  while(pt->PinsToTurnOn){
    GPIOB->DOUTSET31_0 = pt->PinsToTurnOff;     // 3 pins are off output
    GPIOB->DOUTCLR31_0 = pt->PinsToTurnOn;      // one pin is on
    Clock_Delay(24);  // adjust this depending on capacitive load
    column = ((GPIOA->DIN31_0&0x0F000000)>>24);// read columns
    for(j=0; j<=3; j++){
      if((column&0x01)==0){
        key = pt->keycode[j];
        (*Num)++;
      }
      column>>=1;  // shift into position
    }
    pt++;
  }
  return key;
}

char static LastKey; 
void Matrix_Init(void){
  LastKey = 0;             // no key typed
  HeartBeat = 0;
  RxFifo_Init();
  MatrixKeypad_Init();     
  SysTick_IntArm(Clock_Freq()/40,2); //40Hz, 25 ms polling
// __enable_irq(); // put enable in main

} 
void SysTick_Handler(void){  char thisKey; int32_t n;
  thisKey = MatrixKeypad_Scan(&n); // scan 
  if((thisKey != LastKey) && (n == 1)){
    RxFifo_Put(thisKey);
    LastKey = thisKey;
  } else if(n == 0){
    LastKey = 0; // invalid
  }
  HeartBeat++;
}
// input ASCII character from keypad
// spin if Fifo is empty
char Matrix_InChar(void){  char letter;
  do{
    letter = RxFifo_Get();
  }
  while(letter == 0);
  return(letter);
}


