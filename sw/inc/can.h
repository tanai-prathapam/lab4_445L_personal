/**
 * @file      CAN.h
 * @brief     CAN
 * @details   Controller Area Network between the motor board and the sensor board. 
 This interface uses FDCAN0 to communicate on CAN bus PA12 and PA13.
120 ohm across CANH, CANL on both ends of network. If there are three boards
(two sensor and one motor), remove the jumper on the middle board.
Use TCAN1057AVDRQ1, because it implements 3.3V digital logic (not TCAN1057A-Q1, the version with pin 5 nc)
 * @version   ECE445M RTOS V1.2
 * @author    Jonathan Valvano
 * @copyright Copyright 2026 by Jonathan W. Valvano, valvano@mail.utexas.edu,
 * @warning   AS-IS
 * @note      For more information see  http://users.ece.utexas.edu/~valvano/
 * @date      May 11, 2026
   <table>
<caption id="CANpins1">CAN pins</caption>
<tr><th>TCAN1057AVDRQ0<th>MSPM0G3507
<tr><td>Pin1 TXD  <td>CAN_Tx PA12 FD-CAN module 0 transmit
<tr><td>Pin2 Vss  <td>ground
<tr><td>Pin3 VCC  <td>+5V with 0.1uF cap to ground
<tr><td>Pin4 RXD  <td>CAN_Rx PA13 FD-CAN module 0 receive (0 to 3.3V)
<tr><td>Pin5 VIO  <td>+3.3V (digital interface supply)
<tr><td>Pin6 CANL <td>to other CANL on network
<tr><td>Pin7 CANH <td>to other CANH on network
<tr><td>Pin8 RS   <td>ground, Slope-Control Input (maximum slew rate)
</table>
 ******************************************************************************/
/*!
 * @defgroup CAN
 * @brief Controller Area Network
 
  <table>
<caption id="CANpins1">CAN pins</caption>
<tr><th>TCAN1057AVDRQ1<th>MSPM0G3507
<tr><td>Pin1 TXD  <td>CAN_Tx PA12 FD-CAN module 0 transmit
<tr><td>Pin2 Vss  <td>ground
<tr><td>Pin3 VCC  <td>+5V with 0.1uF cap to ground
<tr><td>Pin4 RXD  <td>CAN_Rx PA13 FD-CAN module 0 receive (0 to 3.3V)
<tr><td>Pin5 VIO  <td>+3.3V (digital interface supply)
<tr><td>Pin6 CANL <td>to other CANL on network
<tr><td>Pin7 CANH <td>to other CANH on network
<tr><td>Pin8 RS   <td>ground, Slope-Control Input (maximum slew rate)
</table>
* @{*/



#ifndef __CAN_H__
#define __CAN_H__
#include <stdint.h>

/**
 * initialize Controller Area Network<br>
 * bit rate 1Mbps
 * @param none
 * @return none
 * @brief  Initialize CAN
*/
void CAN_Init(void);

/**
 * enable interrupts on the Controller Area Network
 * @param priority 0(highest) to 3(lowest)
 * @return none
 * @brief  Enable CAN interrupts
*/
void CAN_EnableInterrupts(uint32_t priority);


/**
 * Send a message on the Controller Area Network<br>
 * @param id 11-bit identifier
 * @param dlc 0 to 8, data length
 * @param data array of data to send (0 to 8 bytes)
 * @return 0 if failure, 1 if ok
 * @brief  Send CAN message
*/
int CAN_Send(uint32_t id, uint32_t dlc, uint8_t *data);

/**
 * Check to see if a receive message has been received on the Controller Area Network<br>
 * @param none 
 * @return 0 if no receive data ready, 1 if receive data is available
 * @brief Check for received CAN message
*/
int CAN_CheckMail(void);

/**
 * Receive message on the Controller Area Network, nonblocking.
 * If receive data is ready, gets the data and returns true.
 * If no receive data is ready, returns false
 * @param id pointer to 11-bit id, valid if function returns true
 * @param dlc pointer to data length, valid if function returns true
 * @param data array of data received (0 to 8 bytes), valid if function returns true
 * @return 0 if no receive data ready, 1 if message received
 * @brief Received a CAN message
*/
int CAN_GetMailNonBlock(uint32_t *id, uint32_t *dlc, uint8_t *data);

/**
 * Receive message on the Controller Area Network, blocking.
 * If receive data is ready, gets the data and returns.
 * If no receive data is ready, it will spin waiting for message
 * @param id pointer to 11-bit id
 * @param dlc pointer to data length
 * @param data array of data received (0 to 8 bytes)
 * @return none
 * @brief Receive a CAN message
*/
void CAN_GetMail(uint32_t *id, uint32_t *dlc, uint8_t *data);

#endif //  __CAN_H__

/** @}*/
