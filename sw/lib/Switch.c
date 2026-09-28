/* Switch.c
 * Tanai Prathapam
 * September 17, 2026
 * ECE445L Lab 3 Alarm Clock
 *
 * Debounced switch input module. S1=PA18 (positive logic), S2=PB21
 * (negative logic, needs internal pull-up). Register-level GPIO/IOMUX
 * access, matching the style of LaunchPad.c/Clock.c. Debounced by
 * periodic software polling; call Switch_Poll() every ~10ms.
 *
 * PINCM index numbers (array index into IOMUX->SECCFG.PINCM[]) are
 * PINCMx (datasheet Table 6-1/6-2) minus 1, matching the convention
 * already used in Clock.c (e.g. PA5INDEX=9 for PINCM10):
 *   PA18 -> PINCM40 -> index 39
 *   PB21 -> PINCM49 -> index 48
 */
#include <stdint.h>
#include <ti/devices/msp/msp.h>
#include "../inc/Switch.h"

#define PA18INDEX 39
#define PB21INDEX 48

#define S1_MASK 0x00040000  // bit 18 of GPIOA
#define S2_MASK 0x00200000  // bit 21 of GPIOB

#define DEBOUNCE_COUNT 3    // number of consecutive matching polls (Switch_Poll calls) required

static uint32_t S1StableCount, S2StableCount;
static uint32_t S1Debounced, S2Debounced;   // 1 = pressed, debounced state
static uint32_t S1LastRaw, S2LastRaw;       // raw sample from previous poll
static volatile uint32_t S1EventFlag, S2EventFlag;

void Switch_Init(void){
  IOMUX->SECCFG.PINCM[PA18INDEX] = 0x00040081; // GPIO input, no pull (external circuit provides logic)
  IOMUX->SECCFG.PINCM[PB21INDEX] = 0x00060081; // GPIO input, internal pull-up enabled (bit17 PIPU)
  S1StableCount = 0; S2StableCount = 0;
  S1Debounced = 0; S2Debounced = 0;
  S1LastRaw = 0; S2LastRaw = 0;
  S1EventFlag = 0; S2EventFlag = 0;
}

// Call every ~10ms. Simple debounce: a new raw sample must match the
// previous raw sample for DEBOUNCE_COUNT consecutive polls before the
// debounced state is updated.
void Switch_Poll(void){
  uint32_t s1Raw = (GPIOA->DIN31_0 & S1_MASK) ? 1 : 0;               // S1 positive logic
  uint32_t s2Raw = (GPIOB->DIN31_0 & S2_MASK) ? 0 : 1;               // S2 negative logic (pressed=low)

  if(s1Raw == S1LastRaw){
    if(S1StableCount < DEBOUNCE_COUNT) S1StableCount++;
  } else {
    S1StableCount = 0;
    S1LastRaw = s1Raw;
  }
  if(S1StableCount >= DEBOUNCE_COUNT && S1Debounced != s1Raw){
    S1Debounced = s1Raw;
    if(S1Debounced) S1EventFlag = 1; // new press
  }

  if(s2Raw == S2LastRaw){
    if(S2StableCount < DEBOUNCE_COUNT) S2StableCount++;
  } else {
    S2StableCount = 0;
    S2LastRaw = s2Raw;
  }
  if(S2StableCount >= DEBOUNCE_COUNT && S2Debounced != s2Raw){
    S2Debounced = s2Raw;
    if(S2Debounced) S2EventFlag = 1; // new press
  }
}

uint32_t Switch_S1Pressed(void){ return S1Debounced; }
uint32_t Switch_S2Pressed(void){ return S2Debounced; }

uint32_t Switch_S1Event(void){
  uint32_t result = S1EventFlag;
  S1EventFlag = 0;
  return result;
}

uint32_t Switch_S2Event(void){
  uint32_t result = S2EventFlag;
  S2EventFlag = 0;
  return result;
}

#ifdef SWITCH_TESTMAIN
/* ------------------------------------------------------------------
 * Standalone test main for the Switch module only.
 * Build with SWITCH_TESTMAIN defined and no other main() in the
 * project. Echoes S1/S2 debounced state onto the onboard LEDs.
 * Per prep instructions this only needs to compile, not run.
 * ------------------------------------------------------------------ */
#include "../inc/Clock.h"

#define PA0INDEX 0            // PA0 -> PINCM1 -> index 0, onboard red LED1 (negative logic)

static void LED_Init(void){
  IOMUX->SECCFG.PINCM[PA0INDEX] = 0x00000081; // GPIO output
  GPIOA->DOE31_0 |= 0x01;                     // enable PA0 as output
  GPIOA->DOUT31_0 |= 0x01;                    // LED off (negative logic)
}

int main(void){
  Clock_Init_HFXT_16_80MHz(0);
  Switch_Init();
  LED_Init();

  while(1){
    Switch_Poll();
    if(Switch_S1Event() || Switch_S2Event()){
      GPIOA->DOUT31_0 &= ~0x01; // turn LED on to show an event occurred
    }
    Clock_Delay1ms(10); // poll roughly every 10ms
  }
}
#endif
