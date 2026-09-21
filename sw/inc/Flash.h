/*!
 * @defgroup Flash
 * @brief Flash programming functions
 * @{*/
/**
 * @file      Flash.h
 * @brief     Flash programming functions
 * @details   Runs on MSPM0G3507.
 * One memory bank as MAIN memory. 
 * Each flash word is 8 bytes (64 bits).
 * Each word line is 16 word lines, 128 bytes.
 * Each sector 8 word lines, 1024 bytes (minimum erase size).
 * There is one Bank on MSPM0G3507, with 128k bytes, 128 sectors.
 * Once a byte has been programmed, it cannot be reprogrammed unless the sector is erased

 * @version   ECE445M RTOS V1.2
 * @author    Jonathan Valvano
 * @copyright Copyright 2026 by Jonathan W. Valvano, valvano@mail.utexas.edu,
 * @warning   AS-IS
 * @note      For more information see  http://users.ece.utexas.edu/~valvano/
 * @date      June 4, 2026

 */

#ifndef __FLASH_H__
#define __FLASH_H__


/**
 * Write 64-bits, 8 bytes, to Flash <br>
 * addr must have been previously erased<br>
 * addr must be on 8-byte (64-bit boundary)<br>  
 * Takes 4055 cycles = 50.7us running at 80 MHz
 * @param addr sets the location to be programmed
 * @param data is a 2-element array containing 64 bits
 * @return 0 on pass and nonzero on fail
 * @see Flash_Erase() Flash_WriteArray()
 * @brief  Write to flash
 * @note after erased, this location can only be programmed once
 */
uint32_t Flash_Write(uint32_t addr, const uint32_t *data);


/**
 * Write an array of data to Flash <br>
 * addr must have been previously erased<br>
 * addr must be on 8-byte (64-bit boundary) <br>
 * count must be even 
 * @param addr sets the start location to be programmed
 * @param data is an array containing count 32-bit words
 * @param count is the number of 32-bit words to program
 * @return number of successful writes; return value == count if ok
 * @see Flash_Erase() Flash_Write()
 * @brief  Write array to flash
 * @note after erased, this location can only be programmed once
 */
uint32_t Flash_WriteArray(uint32_t addr, const uint32_t *data, uint32_t count);


/**
 * Erase 1024-byte sector <br>
 * addr must be on 1024-byte (64-bit boundary)  <br>
 * Takes 2488 cycles = 31.1us running at 80 MHz
 * @param addr sets the location to be programmed
 * @return 0 on pass and nonzero on fail
 * @see Flash_Write() Flash_WriteArray()
 * @brief  Write to flash
 * @note Datasheet says flash is indeterminate after erase until it is programmed.
 */
uint32_t Flash_Erase(uint32_t addr);

#endif //  __FLASH_H__
/** @}*/