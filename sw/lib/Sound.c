/* Sound.c
 * Tanai Prathapam & Aleena Khatum
 * September 17, 2026
 * ECE445L Lab 3 Alarm Clock
 *
 * Drives the MOSFET gate pin with a background square wave using a
 * dedicated TIMG7 periodic interrupt (from Timer.c/TimerG7_IntArm),
 * independent of the Time module's TIMG0. Register-level GPIO access.
 *
 * Frequency check (Lab03.docx prep item 5): 32-ohm speaker, 1uF cap,
 * tau = R*C = 32*1e-6 = 32us, fc = 1/(2*pi*R*C) = ~4973 Hz.
 * fc/f = 4973/1000 = ~5.0, within the required 2 <= fc/f <= 10 range.
 *
 * *** Adjust SOUND_PINCM_INDEX / SOUND_PORT_MASK below to match the
 * actual GPIO pin you route to the MOSFET gate in your KiCad schematic. ***
 * The pin used here (PB6, PINCM index placeholder) is arbitrary; confirm
 * against your own hardware design before this pin is ever powered on.
 */
#include <stdint.h>
#include <ti/devices/msp/msp.h>
#include "../inc/Clock.h"
#include "../inc/Timer.h"
#include "../inc/Sound.h"
#include "../inc/LaunchPad.h" //PB4INDEX  

// TODO: verify this PINCM index against your KiCad schematic's chosen
// MOSFET-gate GPIO pin (see mspm0g3507.pdf Table 6-1/6-2 Pin Attributes,
// PINCMx column, then subtract 1 for the array index, matching the
// convention used in Clock.c and Switch.c).
#define SOUND_PINCM_INDEX PB4INDEX     // placeholder: confirm/replace for your gate pin
#define SOUND_PIN_MASK (1U << 4) // placeholder: bit position matching the pin above (example: bit 6)

#define TONE_HZ          1000    // desired audible alarm tone
#define TOGGLE_HZ        (2*TONE_HZ) // must toggle twice per cycle
#define SOUND_PERIOD     500     // TimerClock/prescale/period = 80,000,000/80/500 = 2000 Hz
#define SOUND_PRESCALE   80

static volatile uint32_t SoundActive;
extern volatile uint32_t ADC_Volume_raw; // Pulls the live slidepot data from main.c

void Sound_Init(uint32_t priority){
  IOMUX->SECCFG.PINCM[SOUND_PINCM_INDEX] = 0x00000081; // GPIO output
  GPIOB->DOE31_0 |= SOUND_PIN_MASK;                     // enable as output
  GPIOB->DOUT31_0 &= ~SOUND_PIN_MASK;                   // start low, MOSFET off
  SoundActive = 0;
  TimerG7_IntArm(SOUND_PERIOD, SOUND_PRESCALE, priority); // 2000 Hz toggle interrupt
}

void Sound_Enable(void) {
  SoundActive = 1;
}

void Sound_Disable(void) {
  SoundActive = 0;
}

static inline void Duty_High(uint32_t high_time){ 
    GPIOB->DOUTSET31_0 = SOUND_PIN_MASK; // toggle the MOSFET gate pin
    TIMG7->COUNTERREGS.LOAD = high_time - 1; // Load time for the HIGH phase
}

static inline void Duty_Low(uint32_t low_time){
    GPIOB->DOUTCLR31_0 = SOUND_PIN_MASK;
    TIMG7->COUNTERREGS.LOAD = low_time - 1; // Load time for the LOW phase
}

uint32_t Sound_Status(void){ return SoundActive; }

// TIMG7 interrupt service routine, fires at TOGGLE_HZ (2000 Hz)
void TIMG7_IRQHandler(void){
  TIMG7->CPU_INT.ICLR = 0x01; // acknowledge/clear the TIMG7 interrupt flag

  if (SoundActive) {
    // 12-bit ADC (0 to 4095) to a High_Time.
    uint32_t high_time = (ADC_Volume_raw * 500) / 4095; 
    if (high_time == 0) high_time = 1; //prevent high duty cycle of 0
    uint32_t low_time = 1000 - high_time;

    // Toggle Duty Cycle on every successive interrupt to generate a continuous wave
    if ((GPIOB->DOUT31_0 & SOUND_PIN_MASK) == 0) { 
      // Pin is currently LOW, make it HIGH (Atomic)
      Duty_High(high_time); 
    } 
    else { 
      // Pin is currently HIGH, make it LOW (Atomic) 
      Duty_Low(low_time);
    }
  }
  else {
    // Ensure speaker is off when SoundActive is 0 (Atomic)
    GPIOB->DOUTCLR31_0 = SOUND_PIN_MASK;
  }  
}

// #ifdef SOUND_TESTMAIN
// /* ------------------------------------------------------------------
//  * Standalone test main for the Sound module only.
//  * Build with SOUND_TESTMAIN defined and no other main() in the
//  * project. Per prep instructions this only needs to compile, not run.
//  * ------------------------------------------------------------------ */
// int main(void){
//   Clock_Init_HFXT_16_80MHz(0);
//   Sound_Init(3); // lowest priority, since a missed tick just shifts pitch slightly
//   __enable_irq();

//   Sound_Enable();
//   while(1){
//     // tone plays continuously in the background ISR
//   }
// }
// #endif