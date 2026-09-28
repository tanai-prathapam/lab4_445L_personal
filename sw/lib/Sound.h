/* Sound.h
 * Tanai Prathapam & Aleena Khtum
 * September 17, 2026
 * ECE445L Lab 3 Alarm Clock
 *
 * Speaker/tone module. Drives the gate of the NTR4501NT1G MOSFET
 * (through the recommended 10k series resistor, Lab03.docx prep item 2)
 * with a background square wave generated from a dedicated periodic
 * timer interrupt (TIMG7), not the SysTick/Time module.
 */
#ifndef SOUND_H
#define SOUND_H
#include <stdint.h>

// ---------- Prototypes -------------------------

// Arm TimerG7 for a periodic interrupt at 2x the desired tone frequency
// (each interrupt toggles the output pin once, so 2 interrupts = 1 cycle).
// priority is the NVIC priority (0=highest, 3=lowest). Sound starts OFF.
void Sound_Init(uint32_t priority);

// Start the background tone (MOSFET gate begins toggling)
void Sound_Enable(void);

// Stop the background tone (MOSFET gate forced low, speaker silent)
void Sound_Disable(void);

//Set the PWM Signal High Part of the Duty Cycle
static inline void Duty_High(uint32_t high_time);

//Set the PWM Signal Low Part of the Duty Cycle
static inline void Duty_Low(uint32_t low_time);

// Returns 1 if the tone is currently sounding, else 0
uint32_t Sound_Status(void);

#endif
