/* Switch.h
 * Tanai Prathapam
 * September 17, 2026
 * ECE445L Lab 3 Alarm Clock
 *
 * Debounced switch input module for the onboard LaunchPad switches.
 * S1 = PA18 (positive logic, pressed = high, no internal pull needed)
 * S2 = PB21 (negative logic, pressed = low, requires internal pull-up)
 * Debouncing is done by periodic software polling (call Switch_Poll()
 * every ~10ms from the main loop or a periodic ISR), per Lab 3 Hint 7's
 * suggestion that either polled debouncing or edge interrupts are OK.
 */
#ifndef SWITCH_H
#define SWITCH_H
#include <stdint.h>

// ---------- Prototypes -------------------------

// Initialize PA18 as a plain digital input (S1) and PB21 as a digital
// input with internal pull-up enabled (S2).
void Switch_Init(void);

// Call this once every ~10ms (from a periodic timer or main loop with a
// delay) to sample and debounce both switches. This updates the
// module-private debounced state and one-shot press events.
void Switch_Poll(void);

// Returns 1 if S1 is currently debounced as pressed, else 0
uint32_t Switch_S1Pressed(void);

// Returns 1 if S2 is currently debounced as pressed, else 0
uint32_t Switch_S2Pressed(void);

// Returns 1 exactly once when S1 transitions released->pressed
// (new press event), then automatically clears itself
uint32_t Switch_S1Event(void);

// Returns 1 exactly once when S2 transitions released->pressed
// (new press event), then automatically clears itself
uint32_t Switch_S2Event(void);

#endif
