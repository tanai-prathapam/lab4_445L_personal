/* Motor.c
 * Jonathan Valvano
 * v2.0.3
 * July 13, 2026
// Motor
//   MSPM0
//    PB4  Motor_PWML, ML+, IN3, PWM TIMA1_C0
//    PB1  Motor_PWMR, MR+, IN1, PWM TIMA1_C1
//    PB0  Motor_DIR_L,ML-, IN4, GPIO 0 means forward, 1 means backward
//    PB16 Motor_DIR_R,MR-, IN2, GPIO 0 means forward, 1 means backward

 */
#include <ti/devices/msp/msp.h>
#include "../inc/LaunchPad.h"
#include "../inc/Clock.h"
#include "../inc/PWM1.h"
#include "../inc/Motor.h"
#define DIR_L 1
#define DIR_R (1<<16)

void Motor_Init(void){
  // ****ECE445L write this ****


}

void Motor_Forward(uint32_t dutyLeft, uint32_t dutyRight){
  // ****ECE445L write this ****

  
}

void Motor_Backward(uint32_t dutyLeft, uint32_t dutyRight){
    // ****ECE445L write this ****

 
}
void Motor_Right(uint32_t dutyLeft, uint32_t dutyRight){
    // ****ECE445L write this ****

}
void Motor_Left(uint32_t dutyLeft, uint32_t dutyRight){
   // ****ECE445L write this ****


}
