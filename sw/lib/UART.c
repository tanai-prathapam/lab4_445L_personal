#include <stdint.h>
#include <stdio.h>
#include <ti/devices/msp/msp.h>
#include "../inc/LaunchPad.h"
#include "../inc/Clock.h"
#include "FIFO.h"    
#include "UART.h"    
#include "../inc/UART.h"
#include <ti/devices/msp/msp.h>

/*
-----------------------------------------------------UART 1-----------------------------------------------
*/

// // power Domain PD0
// // for 32MHz bus clock, bus clock is 32MHz
// // for 40MHz bus clock, bus clock is ULPCLK 20MHz
// // for 80MHz bus clock, bus clock is ULPCLK 40MHz
// // assume 40MHz bus clock, bus clock = 20MHz
// // initialize UART for 115200 baud rate
// // interrupt synchronization
// void UART1_Init(void){
//     // RSTCLR to GPIOA and UART1 peripherals
//     //   bits 31-24 unlock key 0xB1
//     //   bit 1 is Clear reset sticky bit
//     //   bit 0 is reset gpio port
//  // GPIOA->GPRCM.RSTCTL = (uint32_t)0xB1000003; // called previously
//   UART1->GPRCM.RSTCTL = 0xB1000003;
//     // Enable power to GPIOA and UART1 peripherals
//     // PWREN
//     //   bits 31-24 unlock key 0x26
//     //   bit 0 is Enable Power
//  // GPIOA->GPRCM.PWREN = (uint32_t)0x26000001; // called previously
//   UART1->GPRCM.PWREN = 0x26000001;
//   Clock_Delay(24); // time for uart to power up
//   // configure PA8 PA9 as alternate UART1 function
//   IOMUX->SECCFG.PINCM[PA9INDEX]  = 0x00040082; //rx msp, tx esp
//   //bit 7  PC connected
//   //bits 5-0=2 for UART0_Tx
//   IOMUX->SECCFG.PINCM[PA8INDEX]  = 0x00000082; //tx msp, rx esp
//   //bit 18 INENA input enable
//   //bit 7  PC connected
//   //bits 5-0=2 for UART0_Rx
//   TxFifo1_Init();
//   RxFifo1_Init();
//   UART1->CLKSEL = 0x08; // bus clock
//   UART1->CLKDIV = 0x00; // no divide
//   UART1->CTL0 &= ~0x01; // disable UART0
//   UART1->CTL0 = 0x00020018;
//    // bit  17    FEN=1    enable FIFO
//    // bits 16-15 HSE=00   16x oversampling
//    // bit  14    CTSEN=0  no CTS hardware
//    // bit  13    RTSEN=0  no RTS hardware
//    // bit  12    RTS=0    not RTS
//    // bits 10-8  MODE=000 normal
//    // bits 6-4   TXE=001  enable TxD
//    // bit  3     RXE=1    enable TxD
//    // bit  2     LBE=0    no loop back
//    // bit  0     ENABLE   0 is disable, 1 to enable
//   // 20000000/16 = 1,250,000 Hz
//  // Baud = 115200
//   /*
//   //   1,250,000/115200 = 10.850694
//   //   divider = 10+54/64 = 10.84375
//   UART0->IBRD = 10;
//   UART0->FBRD = 54; // baud =1,250,000/10.84375 = 115,274 bps
//   */
//   if(Clock_Freq() == 40000000){
//       // 20000000/16 = 1,250,000 Hz
//      // Baud = 115200
//       //   1,250,000/115200 = 10.850694
//       //   divider = 10+54/64 = 10.84375
//     UART1->IBRD = 10;
//     UART1->FBRD = 54; // baud =1,250,000/10.84375 = 115,274
//   }else if (Clock_Freq() == 32000000){
//     // 32000000/16 = 2,000,000
//      // Baud = 115200
//       //   2,000,000/115200 = 17.361
//       //   divider = 17+23/64 = 17.359
//     UART1->IBRD = 17;
//     UART1->FBRD = 23;
//   }else if (Clock_Freq() == 80000000){
//      // 40000000/16 = 2,500,000 Hz
//      // Baud = 115200
//       //    2,500,000/115200 = 21.701388
//       //   divider = 21+45/64 = 21.703125
//     UART1->IBRD = 21;
//     UART1->FBRD = 45; // baud =2,500,000/21.703125 = 115,191
//   }else return;
//   UART1->LCRH = 0x00000030;
//    // bits 5-4 WLEN=11 8 bits
//    // bit  3   STP2=0  1 stop
//    // bit  2   EPS=0   parity select
//    // bit  1   PEN=0   no parity
//    // bit  0   BRK=0   no break
//   UART1->CPU_INT.IMASK = 0x0C01;
//   // bit 11 TXINT
//   // bit 10 RXINT
//   // bit 0  Receive timeout
//   UART1->IFLS = 0x0422;
//   // bits 11-8 RXTOSEL receiver timeout select 4 (0xF highest)
//   // bits 6-4  RXIFLSEL 2 is greater than or equal to half
//   // bits 2-0  TXIFLSEL 2 is less than or equal to half
//   NVIC->ICPR[0] = 1<<13; // UART1 is IRQ 13
//   NVIC->ISER[0] = 1<<13;
//   NVIC->IP[3] = (NVIC->IP[3] & (~0x0000FF00)) | (2 << 14);    // set priority (bits 15, 14) IRQ 13
//   UART1->CTL0 |= 0x01; // enable UART0
// }
// copy from hardware RX FIFO to software RX FIFO
// stop when hardware RX FIFO is empty or software RX FIFO is full
void static copyHardwareToSoftware1(void){
  char letter;
  while(((UART1->STAT&0x04) == 0) && (RxFifo1_Size() < (RXFIFOSIZE - 1))){
    letter = UART1->RXDATA;
    RxFifo1_Put(letter);
  }
}

//------------UART_InChar------------
// Wait for new serial port input
// Input: none
// Output: ASCII code for key typed
// char UART1_InChar(void){
//   char letter;
//   do{
//     letter = RxFifo1_Get();
//   }while(letter==0);
//   return(letter);
// }

//------------UART_InCharNonBlock------------
// Input for new serial port input if available
// Input: none
// Output: ASCII code for key typed, 0 if RxFifo empty
char UART1_InCharNonBlock(void){
  char letter = RxFifo1_Get();
  return(letter);
}

// copy from software TX FIFO to hardware TX FIFO
// stop when software TX FIFO is empty or hardware TX FIFO is full
void static copySoftwareToHardware1(void){
  char letter;
  while(((UART1->STAT&0x80) == 0) && (TxFifo1_Size() > 0)){
    letter = TxFifo1_Get();
    UART1->TXDATA = letter;
  }
}
//------------UART_OutChar------------
// Output 8-bit to serial port
// Input: letter is an 8-bit ASCII character to be transferred
// Output: none
// void UART1_OutChar(char data){
//     while(TxFifo1_Put(data) == 0){};
//     UART1->CPU_INT.IMASK &= ~0x0800;   // disarm TX FIFO interrupt
//     copySoftwareToHardware1();
//     UART1->CPU_INT.IMASK |= 0x0800;    // rearm TX FIFO interrupt
// }

void UART1_IRQHandler(void){ uint32_t status;
//   char ws_cmd;
//   if(!((UART1->STAT&0x04) == 0x04)){
//     ws_cmd = (UART1->RXDATA); 
//     RxFifo1_Put(ws_cmd);
//     //UART_OutChar(ws_cmd);
//   }
    
  status = UART1->CPU_INT.IIDX; // reading clears bit in RIS
  if(status == 0x01){   // 0x01 receive timeout
    copyHardwareToSoftware1();
  }else if(status == 0x0B){ // 0x0B receive
    copyHardwareToSoftware1();
  }else if(status == 0x0C){ // 0x0C transmit
    copySoftwareToHardware1();
    if(TxFifo1_Size() == 0){             // software TX FIFO is empty
      UART1->CPU_INT.IMASK &= ~0x0800;    // disable TX FIFO interrupt
    }
  }
}

//------------UART1_OutString------------
// Output String (NULL termination)
// Input: pointer to a NULL-terminated string to be transferred
// Output: none
//
void UART1_OutString(char *pt){
  // ****ECE445L write this ****
  while(*pt){
    UART1_OutChar(*pt); // send one char at a time
    pt++;              
  }
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
  if(n >= 10){
    UART1_OutUDec(n / 10); // Recursively call for the upper digits
    n = n % 10;            // Isolate the lowest digit
  }
  UART1_OutChar(n + '0');    // Convert the isolated digit to ASCII and send
}



