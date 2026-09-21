/*!
 * @defgroup Flash
 * @brief Flash programming functions
 * @{*/

 /**
 * @file      FlashFile.h
 * @brief     High-level implementation of the file system implementation
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

 *  This solution is predicated on the assumption that erased flash will read as all 1's
 *  The data sheet says flash is indeterminate after erasing, but experimentation shows it as all 1's<br>

 *  After erasing <br>
 -  File system is in Flash locations 0x10000 to 0x1FFFF (64k)
 -  Since MSPM0 has a simple way to write 64-bit data, all pointers are 64 bits
 -  Sector size is 1k (erase boundary), 64 sectors
 -  Block size is 256 bytes
 -  Write data size is 64 bits, 8 bytes, one flash word

 
 *  Directory/FAT are in last two sectors of Flash Rom<br>
 -    Directory has 16 entries for file numbers 0 to 15 (16*8=128=0x80 bytes)
 -    FAT has 240 entries for blocks 0 to 239 (240 blocks each 256 bytes is 61,440 bytes 
 -    2 sectors for Directory/FAT leaving 62 sectors for data (63,488 bytes) (most of 64k Flash)
 -    Directory+FAT is (16+240) 64-bit values = 2048 bytes (last two sectors 0x1F800-0x1FFFF)
 -   Since directory/FAT are in ROM, they are always loaded, valid, and available
 */


#ifndef __FLASHFILE_H__
#define __FLASHFILE_H__

/**
 * Create a new file and save first block.
 * Save 256 bytes into the new file.
 * Can support up to 32 files
 * @param buf pointer to 256 bytes of data
 * @return number of the new file, 0 to 15 
 * @note  assumes OS_File_Format was called previously
 * @see OS_File_Format
 * @brief create new file
 * @warning  return 255 on failure or disk full
*/			
uint32_t OS_File_New(uint32_t buf[64]);


/**
 * Check the size of this file
 * @param num 32-bit file number, 0 to 15
 * @return the number of blocks allocated to the file
 * @brief file size new file
 * @warning  return 0 if file has not been created
*/	
uint32_t OS_File_Size(uint32_t num);

/**
 * Save 256 bytes into existing file
 * @param num 32-bit file number, 0 to 15
 * @param buf pointer to 256 bytes of data
 * @return 0 if successful 
 * @note  assumes OS_File_New was called previously
 * @see OS_File_New
 * @brief append to existing file
 * @warning  returns 255 on failure or disk full
*/	
uint32_t OS_File_Append(uint32_t num, uint32_t buf[64]);


/**
 * Read 256 bytes from the file
 * @param num 32-bit file number, 0 to 15
 * @param location logical address, 0 to 239
 * @return pointer to 256 bytes of the file if successful 
 * @brief read from existing file
 * @warning  returns 0 on failure because no data
*/
uint32_t *OS_File_Read(uint32_t num, uint32_t location);



/**
 * Erase all files and all data
 * @param none
 * @return 0 if success 
 * @brief create new file
 * @warning  return 1 on disk write failure
*/	
uint32_t OS_File_Format(void);
#endif // __FLASHFILE_H__
/** @}*/
