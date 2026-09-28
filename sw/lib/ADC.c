/* ADC.c
 * This is a C language project that implements ECE445L Lab3.
 * Aleena Khatum
 * September 21, 2026
 * Analog distance sensors, one sensor needed
 *   PB24 is ADC0 channel 5 (slidepot monitor)
 */

#include <ti/devices/msp/msp.h>
#include "../inc/LaunchPad.h"
#include "../inc/Clock.h"
#include "../inc/SSD1306.h"
#include "../inc/Timer.h"
#include "../inc/UART.h"
#include <math.h>
#define SSD1306 1
int32_t Averaging=0; // 0 to 6 to study CLT

void ADC_Init5(void){ // initialize ADCO channel 5
  ADC0->ULLMEM.GPRCM.RSTCTL = 0xB1000003; // 1) reset ADC
  ADC0->ULLMEM.GPRCM.PWREN = 0x26000001;  // 2) activate ADC
  Clock_Delay(24);                        // 3) wait for ADC to settle
  ADC0->ULLMEM.GPRCM.CLKCFG = 0xA9000000; // 4) ULPCLK connect to ADC
  ADC0->ULLMEM.CLKFREQ = 7;               // 5) 40-48 MHz (speed of processor)
  ADC0->ULLMEM.CTL0 = 0x03010000;         // 6) divide by 8 (speed at which ADC is clocked)
  ADC0->ULLMEM.CTL1 = 0x00000000;         // 7) mode (no averaging, software trigger, and one sample)
  ADC0->ULLMEM.CTL2 = 0x00000000;         // 8) MEMRES (put digital result here)
  ADC0->ULLMEM.MEMCTL[0] = 5;             // 9) channel 5 is PB20
  ADC0->ULLMEM.SCOMP0 = 0;                // 10) 8 sample clocks
  ADC0->ULLMEM.CPU_INT.IMASK = 0;         // 11) no interrupt
}

int32_t ADC_In5(void){
  ADC0->ULLMEM.CTL0 |= 0x00000001;             // 1) enable conversions
  ADC0->ULLMEM.CTL1 |= 0x00000100;             // 2) start ADC
  uint32_t volatile delay=ADC0->ULLMEM.STATUS; // 3) time to let ADC start
  while((ADC0->ULLMEM.STATUS&0x01)==0x01){}    // 4) wait for completion
  return ADC0->ULLMEM.MEMRES[0];               // 5) 12-bit result
}
