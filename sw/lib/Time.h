/* Time.h
 * Tanai Prathapam
 * September 17, 2026
 * ECE445L Lab 3 Alarm Clock
 *
 * Low-level time-of-day module. Maintains hours:minutes:seconds using a
 * periodic TimerG0 interrupt configured directly through registers
 * (see Timer.c TimerG0_IntArm), with no TI DriverLib/TivaWare-style
 * driver calls, per Lab 3 preparation requirement.
 */
#ifndef TIME_H
#define TIME_H
#include <stdint.h>

// ---------- Prototypes -------------------------

// Initialize TimerG0 for a 10ms periodic interrupt and reset the clock
// to 00:00:00. priority is the NVIC priority (0 = highest, 3 = lowest).
// Call once, before __enable_irq().
void Time_Init(uint32_t priority);

// Set the current time of day, 24-hour format (0-23, 0-59, 0-59)
void Time_Set(uint32_t hour, uint32_t min, uint32_t sec);

// Get the current time of day, 24-hour format
void Time_Get(uint32_t *hour, uint32_t *min, uint32_t *sec);

// Set the alarm time, 24-hour format
void Time_SetAlarm(uint32_t hour, uint32_t min, uint32_t sec);

// Get the alarm time, 24-hour format
void Time_GetAlarm(uint32_t *hour, uint32_t *min, uint32_t *sec);

// Arm/disarm the alarm comparison
void Time_AlarmOn(void);
void Time_AlarmOff(void);

// Returns 1 if the alarm is armed and current time equals alarm time
uint32_t Time_CheckAlarm(void);

// Semaphore set by the ISR once per second. Foreground code should poll
// Time_Flag() and call Time_ClearFlag() after servicing it (e.g. updating
// the LCD), this avoids a critical section between ISR and main.
uint32_t Time_Flag(void);
void Time_ClearFlag(void);

#endif
