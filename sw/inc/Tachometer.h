/*!
 * @defgroup RSLK
 * @brief RSLK sensors and Actuators
 <table>
<caption id="RSLKtach pins">RSLK RSLK motor pins</caption>
<tr><th> TM4C <th> MSPM0 <th> Description
<tr><td> PB7  <td> PB8   <td> ELA  TA0_C0
<tr><td> PB2  <td> PB12  <td> ERA  TA0_C1
<tr><td>      <td>       <td> ERB  GPIO input (not connected)
<tr><td>      <td>       <td> ELB  GPIO input (not connected)
</table>
 * @{*/
/**
 * @file      Tachometer.h
 * @brief     RSLK tachometer
 * @details   Using input capture to measure motor speed<br>

 * @version   RSLK v2.03
 * @author    Daniel Valvano and Jonathan Valvano
 * @copyright Copyright 2026 by Jonathan W. Valvano, valvano@mail.utexas.edu,
 * @warning   AS-IS
 * @note      For more information see  http://users.ece.utexas.edu/~valvano/
 * @date      July 11, 2026
 <table>
<caption id="RSLKtach2">RSLK Bumper switches</caption>
<tr><th> TM4C <th> MSPM0 <th> Description
<tr><td> PB7  <td> PB8   <td> ELA  TA0_C0
<tr><td> PB2  <td> PB12  <td> ERA  TA0_C1
<tr><td>      <td>       <td> ERB  GPIO input (not connected) 
<tr><td>      <td>       <td> ELB  GPIO input (not connected) 
</table>
  ******************************************************************************/

#ifndef __TACHOMETER_H__
#define __TACHOMETER_H__
#include <stdint.h>

/**
 * Check for stopped motors
 * Higher level software must run this every 10ms
 * @param none
 * @return none
 * @brief  Check for stopped motors
 * @note If the wheel is not spinning there will be no input capture interrupts
 */
void Tachometer_CheckForStopped(void); // run this every 10ms

/**
 * Initialize RSLK tachometers, 16-bit, 1us resolution
 * @param none
 * @return none
 * @brief  Initialize motors
 * @note Uses input capture on ELA and ERA
 */
void Tachometer_Init(void);

/** 
 * Right motor speed in 0.1 RPM.
 * Does not wait for next measurement, returns with last measurement
 * @param none
 * @return Rightrpm
 * @brief  Right motor speed
 * @note Uses input capture on ERA
 * @note Since ERB is not connected, RSLK2 cannot measure direction
 */ 
uint32_t Tachometer_GetRightrpm(void);

/** 
 * Left motor speed in 0.1 RPM.
 * Does not wait for next measurement, returns with last measurement
 * @param none
 * @return Leftrpm
 * @brief  Left motor speed
 * @note Uses input capture on ELA 
 * @note Since ELB is not connected, RSLK2 cannot measure direction
 */ 
uint32_t Tachometer_GetLeftrpm(void);

#endif
