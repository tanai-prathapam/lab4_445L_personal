// ----------------------------------------------------------------------
//
// MotorControl.c
// 
// ----------------------------------------------------------------------
// 
//  This code runs the Motor Controller 
// RSLK v2.0.3
// July 13, 2026
// Motor
//   MSPM0
//   PB4  Motor_PWML, ML+, IN3, PWM TIMA1_C0
//   PB1  Motor_PWMR, MR+, IN1, PWM TIMA1_C1
//   PB0  Motor_DIR_L,ML-, IN4, GPIO 0 means forward, 1 means backward
//   PB16 Motor_DIR_R,MR-, IN2, GPIO 0 means forward, 1 means backward
// tachometer
//   PB8  ELA  TA0_C0
//   PB7  ERB  not connected
//   PB6  ELB  not connected
//   PB12 ERA  TA0_C1
// Negative Logic Bumper switches
//   PA27 Left, Bump 0, 
//   PB15 Center Left, Bump 1, 
//   PA28 Center Right, Bump 2
//   PA31 Right, Bump 3

#include <stdint.h>

#include "../inc/MotorControl.h"

// ****ECE445L write this ****

   
// --------------------   MC_PIControlLoop    ----------------------------------
//
// This routine is the main PI control loop. It is run every 10ms. The Count10ms 
// flag indicates if the motor is running. If not, the rev/sec variable is cleared. 
//
void MC_PIControlLoop(void){
  
}


// desired speeds in 0.1 rpm
void MC_SetDesiredSpeed(uint32_t newSpeed){

}


// --------------------- Initialize Motor Control -------------------
//
void MC_Init(void) {

  
  
}

