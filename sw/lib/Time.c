/* Time.c
 * Tanai Prathapam
 * September 17, 2026
 * ECE445L Lab 3 Alarm Clock
 *
 * Low-level time-of-day module. Derived from Timer.c (Valvano)
 * TimerG0_IntArm. Timekeeping is maintained entirely with register-level
 * TIMG0 configuration and a hand-written ISR, no TI DriverLib/TivaWare
 * style driver calls are used for the periodic interrupt or the clock
 * logic, per Lab 3 preparation requirement.
 *
 * Assumes an 80 MHz bus clock (Clock_Init_HFXT_16_80MHz or
 * Clock_Init80MHz already called in main before Time_Init).
 * TIMG0 clock = 80MHz bus clock (see Timer.c comment, CLKSEL = bus clock).
 * frequency = TimerClock/prescale/period
 *   80,000,000 / 256 / 3125 = 100 Hz  ->  10ms tick
 */
#include <stdint.h>
#include <ti/devices/msp/msp.h>
#include "../inc/Clock.h"
#include "../inc/Timer.h"
#include "../inc/Time.h"

#define TICKS_PER_SECOND 100     // 100 interrupts per second (10ms each)
#define TIMERPERIOD      3125    // LOAD value passed to TimerG0_IntArm
#define TIMERPRESCALE    256     // CPS+1 value passed to TimerG0_IntArm

static volatile uint32_t Hours, Minutes, Seconds;
static volatile uint32_t AlarmHours, AlarmMinutes, AlarmSeconds;
static volatile uint32_t AlarmArmed;
static volatile uint32_t SubSecondCount; // 0 to TICKS_PER_SECOND-1 within one second
static volatile uint32_t TimeFlag;       // set by ISR once per second

// Initialize TimerG0 for a 10ms periodic interrupt and reset clock to 00:00:00
void Time_Init(uint32_t priority){
  Hours = 0; Minutes = 0; Seconds = 0;
  AlarmHours = 0; AlarmMinutes = 0; AlarmSeconds = 0;
  AlarmArmed = 0;
  SubSecondCount = 0;
  TimeFlag = 0;
  TimerG0_IntArm(TIMERPERIOD, TIMERPRESCALE, priority); // arms TIMG0, NVIC IRQ16
}

// Critical section: briefly mask the TIMG0 NVIC bit (IRQ16) while the
// shared Hours/Minutes/Seconds globals are written, since the ISR also
// writes these same globals.
void Time_Set(uint32_t hour, uint32_t min, uint32_t sec){
  NVIC->ICER[0] = 1 << 16; // disable TIMG0 interrupt
  Hours = hour; Minutes = min; Seconds = sec;
  SubSecondCount = 0;
  NVIC->ISER[0] = 1 << 16; // re-enable TIMG0 interrupt
}

void Time_Get(uint32_t *hour, uint32_t *min, uint32_t *sec){
  NVIC->ICER[0] = 1 << 16;
  *hour = Hours; *min = Minutes; *sec = Seconds;
  NVIC->ISER[0] = 1 << 16;
}

void Time_SetAlarm(uint32_t hour, uint32_t min, uint32_t sec){
  AlarmHours = hour; AlarmMinutes = min; AlarmSeconds = sec;
}

void Time_GetAlarm(uint32_t *hour, uint32_t *min, uint32_t *sec){
  *hour = AlarmHours; *min = AlarmMinutes; *sec = AlarmSeconds;
}

void Time_AlarmOn(void){  AlarmArmed = 1; }
void Time_AlarmOff(void){ AlarmArmed = 0; }

uint32_t Time_CheckAlarm(void){
  if(AlarmArmed &&
     (Hours   == AlarmHours) &&
     (Minutes == AlarmMinutes) &&
     (Seconds == AlarmSeconds)){
    return 1;
  }
  return 0;
}

uint32_t Time_Flag(void){ return TimeFlag; }
void Time_ClearFlag(void){ TimeFlag = 0; }

// TIMG0 interrupt service routine, fires every 10ms.
// This ISR, plus TimerG0_IntArm, are the low-level periodic-interrupt
// mechanism used to maintain time for Lab 3, no driver library calls.
void TIMG0_IRQHandler(void){
  TIMG0->CPU_INT.ICLR = 0x01; // acknowledge/clear the TIMG0 interrupt flag
  GPIOB->DOUTTGL31_0 = BLUE;  // toggle blue LED every tick, scope-visible heartbeat for ISR rate/jitter
  SubSecondCount++;
  if(SubSecondCount >= TICKS_PER_SECOND){
    SubSecondCount = 0;
    Seconds++;
    if(Seconds >= 60){
      Seconds = 0;
      Minutes++;
      if(Minutes >= 60){
        Minutes = 0;
        Hours++;
        if(Hours >= 24){
          Hours = 0;
        }
      }
    }
    TimeFlag = 1; // tell foreground thread one second has elapsed
  }
}

#ifdef TIME_TESTMAIN
/* ------------------------------------------------------------------
 * Standalone test main for the Time module only.
 * Build this file with TIME_TESTMAIN defined (and no other main()
 * in the project) to compile-check the module per Lab 3 prep item 3.
 * Per prep instructions, this does not need to be run or debugged,
 * only designed, written, and compiled.
 * ------------------------------------------------------------------ */
int main(void){
  uint32_t h, m, s;

  Clock_Init_HFXT_16_80MHz(0);  // 80 MHz bus clock from LaunchPad crystal
  Time_Init(2);                  // arm TIMG0, NVIC priority 2
  Time_Set(11, 59, 55);          // start 5 seconds before a rollover
  Time_SetAlarm(12, 0, 0);
  Time_AlarmOn();

  __enable_irq();

  while(1){
    if(Time_Flag()){
      Time_ClearFlag();
      Time_Get(&h, &m, &s);      // in real use, hand h/m/s to the LCD module
      if(Time_CheckAlarm()){
        // in real use, call Sound_On() here to start the buzzer
      }
    }
  }
}
#endif
