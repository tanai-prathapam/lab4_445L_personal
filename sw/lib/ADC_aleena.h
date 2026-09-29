/* ADC.h
 * Aleena Khatum
 * September 21, 2026
 * ECE445L Lab 3 Alarm Clock
 *
 * Analog-to-Digital Converter module. Initializes and samples
 * ADC0 channel 5 (PB24) to monitor the slide potentiometer.
 */
#ifndef ADC_H
#define ADC_H
#include <stdint.h>

// ---------- Prototypes -------------------------

// Initialize ADC0 channel 5 (PB24) for software-triggered sampling
void ADC_Init5(void);

// Trigger a single conversion on ADC0 channel 5 and wait for completion
// Returns the 12-bit digital result (0 to 4095)
int32_t ADC_In5(void);

#endif