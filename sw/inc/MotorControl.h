/*!
 * @defgroup RSLK
 * @brief RSLK sensors and Actuators
 <table>
<caption id="RSLKtach pins4">RSLK RSLK motor pins</caption>
<tr><th> TM4C <th> MSPM0 <th> Description
<tr><td> PB7  <td> PB8   <td> ELA  TA0_C0
<tr><td> PB2  <td> PB12  <td> ERA  TA0_C1
<tr><td>      <td>       <td> ERB  GPIO input (not connected)
<tr><td>      <td>       <td> ELB  GPIO input (not connected)
</table>
 <table>
<caption id="RSLKmotor4">RSLK motor pins</caption>
<tr><th> TM4C <th> MSPM0  <th> Description
<tr><td> PF2  <td> PB4  <td> Motor_PWML, ML+, IN3, PB.4/TIMA1_C0;
<tr><td> PF3  <td> PB1  <td> Motor_PWMR, MR+, IN1, PB.1/TIMA1_C1;
<tr><td> PA3  <td> PB0  <td> Motor_DIR_L,ML-, IN4, 1 means forward, 0 means backward
<tr><td> PA2  <td> PB16 <td> Motor_DIR_R,MR-, IN2
</table>
 * @{*/
/**
 * @file      MotorControl.h
 * @brief     RSLK PI motor controller
 * @details   Using input capture input and PWM output to control motor speed<br>

 * @version   RSLK v2.03
 * @author    Daniel Valvano and Jonathan Valvano
 * @copyright Copyright 2024 by Jonathan W. Valvano, valvano@mail.utexas.edu,
 * @warning   AS-IS
 * @note      For more information see  http://users.ece.utexas.edu/~valvano/
 * @date      July 5, 2026
 <table>
<caption id="RSLKtach pins5">RSLK RSLK motor pins</caption>
<tr><th> TM4C <th> MSPM0 <th> Description
<tr><td> PB7  <td> PB8   <td> ELA  TA0_C0
<tr><td> PB2  <td> PB12  <td> ERA  TA0_C1
<tr><td>      <td>       <td> ERB  GPIO input (not connected)
<tr><td>      <td>       <td> ELB  GPIO input (not connected)
</table>
 <table>
<caption id="RSLKmotor5">RSLK motor pins</caption>
<tr><th> TM4C <th> MSPM0  <th> Description
<tr><td> PF2  <td> PB4  <td> Motor_PWML, ML+, IN3, PB.4/TIMA1_C0;
<tr><td> PF3  <td> PB1  <td> Motor_PWMR, MR+, IN1, PB.1/TIMA1_C1;
<tr><td> PA3  <td> PB0  <td> Motor_DIR_L,ML-, IN4, 1 means forward, 0 means backward
<tr><td> PA2  <td> PB16 <td> Motor_DIR_R,MR-, IN2
</table>
  ******************************************************************************/

#ifndef __MOTORCONTOL_H__
#define __MOTORCONTOL_H__
#include <stdio.h>
#include <stdint.h>
/*****Note to students, feel free to change any or all of this ******/
/**
 * Initialize RSLK Controller
 * @param none
 * @return none
 * @brief  Initialize tach input and motor outputs
 */
void MC_Init(void);

/**
 * Run RSLK PI Controller
 * @param none
 * @return none
 * @brief  PI controller
 * @note should run at 100Hz
 */
void MC_PIControlLoop(void);

/**
 * Set desired speed in 0.1 rpm.
 * Robot should move in a straight line
 * @param newSpeed
 * @return none
 * @brief  Set desired speed
 */
void MC_SetDesiredSpeed(uint32_t newSpeed);

/**
 * Get desired speed in 0.1 rpm.
 * @param newSpeed
 * @return desiredSpeed
 * @brief  Get desired speed
 */
uint32_t MC_GetDesiredSpeed(void);

/**
 * Set Kp1
 * @param kp1
 * @return none
 * @brief  Set numerator for proportial control
 */
void MC_SetKp1(int32_t kp1);

/**
 * Set Kp2
 * @param kp2
 * @return none
 * @brief  Set denominator for proportial control
 */
void MC_SetKp2(int32_t kp2);

/**
 * Set Ki1
 * @param ki1
 * @return none
 * @brief  Set numerator for integral control
 */
void MC_SetKi1(int32_t ki1);

/**
 * Set Ki2
 * @param ki2
 * @return none
 * @brief  Set denominator for integral control
 */
void MC_SetKi2(int32_t ki2);

/**
 * Get Kp1
 * @param none
 * @return Kp1
 * @brief  Get numerator for proportial control
 */
int32_t MC_GetKp1(void);

/**
 * Get Kp2
 * @param none
 * @return Kp2
 * @brief  Get denominator for proportial control
 */
int32_t MC_GetKp2(void);

/**
 * Get Ki1
 * @param none
 * @return Ki1
 * @brief  Get numerator for integral control
 */
int32_t MC_GetKi1(void);

/**
 * Get Ki2
 * @param none
 * @return Ki2
 * @brief  Get denominator for integral control
 */
int32_t MC_GetKi2(void);

/**
 * Get LeftE
 * @param none
 * @return LeftE
 * @brief  Get controller error on left
 */
int32_t MC_GetLeftE(void);

/**
 * Get RightE
 * @param none
 * @return RightE
 * @brief  Get controller error on right
 */
int32_t MC_GetRightE(void);

/**
 * Get LeftU
 * @param none
 * @return LeftU
 * @brief  Get controller actuator output on left
 */
int32_t MC_GetLeftU(void);

/**
 * Get RightU
 * @param none
 * @return RightU
 * @brief  Get controller actuator output on right
 */
int32_t MC_GetRightU(void);

/**
 * Get Time 
 * since last change in desired speed, 10 ms
 * @param none
 * @return Time
 * @brief  Get controller time
 * @note Controller runs at 100Hz
 */
 uint32_t MC_Time(void); 
 
/**
 * \brief DUMPSIZE is the size of the dump buffer
 */
 #define DUMPSIZE 200

/**
 * Get buffer to dumped Right speed 
 * @param none
 * @return pointer to right speed log
 * @brief  Get right speed log
 * @note left and right speeds are dumped after each change in desired speed
 */
 uint32_t *MC_DumpRight(void);

/**
 * Get buffer to dumped left speed 
 * @param none
 * @return pointer to left speed log
 * @brief  Get left speed log
 * @note left and right speeds are dumped after each change in desired speed
 */
uint32_t *MC_DumpLeft(void);

#endif
