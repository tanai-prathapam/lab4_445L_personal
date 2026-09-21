/*!
 * @defgroup RSLK
 * @brief RSLK sensors and Actuators
 <table>
<caption id="BNO085pins0">BNO085 pins on the MSPM0G3507</caption>
<tr><th>BNO085<th>MSPM0
<tr><td>Vcc   <td>3.3V  Power
<tr><td>GND   <td>GND
<tr><td>SCL   <td>left not connected
<tr><td>SDA   <td>PB13 RxD: is UART3 Rx (BNO085 to MSPM0) baud=115200 bps
<tr><td>AD0   <td>left not connected
<tr><td>CS    <td>left not connected
<tr><td>INT   <td>left not connected
<tr><td>RST   <td>PB12 GPIO output (drive low to reset)
<tr><td>PS1   <td>left not connected (pulled low)
<tr><td>PS0   <td>3.3V to select UART-RVC mode (there is a solder jumper on back)
</table>
 * @{*/ 

/**
 * @file      BNO085.h
 * @brief     Interfacing the BNO085 IMU
 * \image html BNO085.jpg  width=300px
 * @details   The BNO085 is a 9-DOF IMU<br>
The BNO085 could be connected to any UART Rx pin. <br>
The accelerometer could be interfaced to either the sensor or the motor board<br>
Tested here is the BNO085 connected to UART3 PB13 on the robot board<br>
To determine the actual orientation of the module, the rotations should be applied in the order yaw, pitch then roll.<br>
In UART-RVC (remote vacuum cleaner) mode, a 19-byte message is sent every 10us (100Hz).
The message contains six 16-bit signed measurements<br>
- Yaw   0.01 deg (-180.00 to +180.00 deg)<br>
- Pitch 0.01 deg (-90.00 to +90.00 deg)<br>
- Roll  0.01 deg (-180.00 to +180.00 deg)<br>
- X-acceleration 0.001g<br>
- Y-acceleration 0.001g<br>
- Z-acceleration 0.001g<br>
 * @version   ECE445M RTOS V1.2
 * @author    Jonathan Valvano
 * @copyright Copyright 2026 by Jonathan W. Valvano, valvano@mail.utexas.edu,
 * @warning   AS-IS
 * @note      For more information see  http://users.ece.utexas.edu/~valvano/
 * @date      May 11, 2026
 * @warning   BNO085 was not tested in the presence of running motors
 <table>
<caption id="BNO085pins2">BNO085 pins on the MSPM0G3507</caption>
<tr><th>BNO085<th>MSPM0
<tr><td>Vcc   <td>3.3V  Power
<tr><td>GND   <td>GND
<tr><td>SCL   <td>left not connected
<tr><td>SDA   <td>PB13 RxD: is UART3 Rx (BNO085 to MSPM0) baud=115200 bps
<tr><td>AD0   <td>left not connected
<tr><td>CS    <td>left not connected
<tr><td>INT   <td>left not connected
<tr><td>RST   <td>PB12 GPIO output (drive low to reset)
<tr><td>PS1   <td>left not connected (pulled low)
<tr><td>PS0   <td>3.3V to select UART-RVC mode (there is a solder jumper on back)
</table>
*/

/**
 * Reset the 9-DOF BNO085 IMU<br>
 * RST =0<br>
 * wait time bus cycles<br>
 * RST =1<br>
 * @param time number of bus cycles to hold RST low
 * @return none 
 * @brief  Reset the BNO085 
 * @note only 100us (time=8000) was tested
 */
void BNO085_Reset(uint32_t time);

/**
 * Initialize the UART3 for 115,200 baud rate<br>
 * 8 bit word length, no parity bits, one stop bit, FIFO enabled<br>
 * Callback function will be called at 100 Hz<br>
 * data[0] Yaw   0.01 deg<br>
 * data[1] Pitch 0.01 deg<br>
 * data[2] Roll  0.01 deg <br>
 * data[3] X-acceleration 0.001g<br>
 * data[4] Y-acceleration 0.001g<br>
 * data[5] Z-acceleration 0.001g <br>
 * @param function pointer to callback function to which is passed an 6-element array
 * @return none 
 * @note Assumes BNO085 is configured for UART-RVC mode
 * @brief  Initialize 9-DOF BNO085 IMU
 */
void BNO085_Init(void (*function)(int16_t data[6]));
/** @}*/
