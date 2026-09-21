// Tachmeter.c
// Runs on MSPM0
// Jonathan Valvano
// July 13, 2026

// tachometer
//   PB8  ELA  TA0_C0
//   PB7  ERB  not connected
//   PB6  ELB  not connected
//   PB12 ERA  TA0_C1

#include <stdint.h>
#include <ti/devices/msp/msp.h>
#include "../inc/InputCapture.h"
#include "../inc/Median.h"
// ****ECE445L write this ****

uint32_t Tachometer_GetRightrpm(void){
    // ****ECE445L write this ****

   return 42;
}
uint32_t Tachometer_GetLeftrpm(void){
// ****ECE445L write this ****

  return 42;
}


void Tachometer_Init(void){
// ****ECE445L write this ****


}
// ****ECE445L write this ****


void TIMA0_IRQHandler(void){
  uint32_t iidx = TIMA0->CPU_INT.IIDX;// this will acknowledge
  if(iidx == 5){ // 5 means capture CCD0, PB8  ELA  TA0_C0
// ****ECE445L write this ****

   
  }
  if(iidx == 6){ // 6 means capture CCD1=PB12 ERA  TA0_C1
    // ****ECE445L write this ****


  }
}

