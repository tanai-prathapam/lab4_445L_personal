/*!
 * @defgroup RSLK
 * @brief RSLK sensors and Actuators
 <table>
<caption id="RSLKmotor pins">RSLK RSLK motor pins</caption>
<tr><th> TM4C <th> MSPM0  <th> Description
<tr><td> PF2  <td> PB4  <td> Motor_PWML, ML+, IN3, PB.4/TIMA1_C0;
<tr><td> PF3  <td> PB1  <td> Motor_PWMR, MR+, IN1, PB.1/TIMA1_C1;
<tr><td> PA3  <td> PB0  <td> Motor_DIR_L,ML-, IN4, 1 means forward, 0 means backward
<tr><td> PA2  <td> PB16 <td> Motor_DIR_R,MR-, IN2
</table>
 * @{*/
/**
 * @file      Motor.h
 * @brief     RSLK dual H-bridge driver
 * @details   RSLK motor interface<br>

 * @version   RSLK v2.03
 * @author    Daniel Valvano and Jonathan Valvano
 * @copyright Copyright 2026 by Jonathan W. Valvano, valvano@mail.utexas.edu,
 * @warning   AS-IS
 * @note      For more information see  http://users.ece.utexas.edu/~valvano/
 * @date      July 12, 2026
 <table>
<caption id="RSLKmotor2">RSLK Bumper switches</caption>
<tr><th> TM4C <th> MSPM0  <th> Description
<tr><td> PF2  <td> PB4  <td> Motor_PWML, ML+, IN3, PB.4/TIMA1_C0;
<tr><td> PF3  <td> PB1  <td> Motor_PWMR, MR+, IN1, PB.1/TIMA1_C1;
<tr><td> PA3  <td> PB0  <td> Motor_DIR_L,ML-, IN4, 1 means forward, 0 means backward
<tr><td> PA2  <td> PB16 <td> Motor_DIR_R,MR-, IN2
</table>
  ******************************************************************************/

#ifndef __MOTOR_H__
#define __MOTOR_H__

#define MOTOR_PERIOD 10000 // 10,000us = 10ms
#define MOTORMIN 2
#define MOTORMAX (MOTOR_PERIOD-2)
/**
 * Initialize RSLK motors
 * @param none
 * @return none
 * @brief  Initialize motors
 * @note PWM period is 10000 us
 */
void Motor_Init(void);

/**
 * Set RSLK motor power forward<br>
 * @param dutyLeft 2 to 99998 us
 * @param dutyRight 2 to 99998 us
 * @return none
 * @note period is 10000 us
 * @brief  Go forward
 */
void Motor_Forward(uint32_t dutyLeft, uint32_t dutyRight);

/**
 * Set RSLK motor power backward<br>
 * @param dutyLeft 2 to 99998 us
 * @param dutyRight 2 to 99998 us
 * @return none
 * @note period is 10000 us
 * @brief  Go backward
 */
void Motor_Backward(uint32_t dutyLeft, uint32_t dutyRight);

/**
 * Set RSLK motor power right turn<br>
 * Activate left motor forward and right motor backward (pivot)
 * @param dutyLeft 2 to 99998 us
 * @param dutyRight 2 to 99998 us
 * @return none
 * @note period is 10000 us
 * @brief  Turn right
 */
void Motor_Right(uint32_t dutyLeft, uint32_t dutyRight);

/**
 * Set RSLK motor power left turn<br>
 * Activate right motor forward and left motor backward (pivot)
 * @param dutyLeft 2 to 99998 us
 * @param dutyRight 2 to 99998 us
 * @return none
 * @note period is 10000 us
 * @brief Turn left
 */
 void Motor_Left(uint32_t dutyLeft, uint32_t dutyRight);

#endif
