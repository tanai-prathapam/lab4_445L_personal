// Flash.c
// Runs on MSPM0
// Erase and program internal flash ROM.
// Valvano
// June 4, 2026

/* 
 Copyright 2026 by Jonathan W. Valvano, valvano@mail.utexas.edu
    You may use, edit, run or distribute this file
    as long as the above copyright notice remains
 THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 VALVANO SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL,
 OR CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 For more information about my classes, my research, and my books, see
 http://users.ece.utexas.edu/~valvano/
 */
#include <ti/devices/msp/msp.h>
#define RAMFUNC \
    __attribute__((section(".TI.ramfunc"))) __attribute__((noinline))

void static UnprotectSector(uint32_t addr){
// assumes one bank, MAIN memory, sector size is 1024 bytes
// memory size is 128k, so sector number is 0 to 127
  uint32_t sector = (addr>>10);
  if(sector < 32){ // first 32 sectors protected by CMDWEPROTA 
    uint32_t sectorMask = (1 << sector);
    FLASHCTL->GEN.CMDWEPROTA &= ~sectorMask;
  }else{ // sectors 32 to 127 protected by CMDWEPROTB
    uint32_t sectorMask =  1 << ((sector - 32) / 8);
    FLASHCTL->GEN.CMDWEPROTB &= ~sectorMask;
  }
}

// returns 0 on pass and 1 on fail
RAMFUNC static uint32_t ExecuteCommandFromRAM(void){
  volatile uint32_t status;
  FLASHCTL->GEN.CMDEXEC = 0x01; /* Set bit 0 to execute command */
    /*
     * After executing a flash operation, we will enter a do-while and read the
     * STATCMD register using the status variable. Within the loop it will
     * poll until 0x03 (passed) or 0x01 (failed)
     * is read from the STATCMD register. This is to ensure that it will properly poll
     * even when the CPU is running at maximum speeds.
     */
// STATCMD register
//   bit 2 in progress (wait for this bit to go low)
//   bit 1 pass(1) or fail (0), result of the command
//   bit 0 done (wait for this bit to go high)
  do{
    status = FLASHCTL->GEN.STATCMD & 0x07;
  } while ((status != 0x03) && (status != 0x01));
  return (status==1);
}

// addr sets the location to be programmed
//   must have been previously erased
//   must be on 8-byte (64-bit boundary)
//   after erased, this location can only be programmed once
// data is a 2-element array containing 64 bits
// returns 0 on pass and 0x01 on fail
uint32_t Flash_Write(uint32_t addr, const uint32_t *data){
  UnprotectSector(addr);  // Unprotect sector with ECC by hardware 
// Enable 64 bits per data register for programming, with ECC enabled 
// CMDTYPE
//  bits 6-4 SIZE=0 for one flash word (64 bits)
//  bits 2-0 COMMAND=1 for Program
  FLASHCTL->GEN.CMDTYPE = (0<<4)|1;     // program one flash word
// CMDBYTEN
//  bits 7-0 enable all 8 bytes
//  bit8 enables ECC
  FLASHCTL->GEN.CMDBYTEN = 0x1FF; 
  FLASHCTL->GEN.CMDADDR = addr;
  FLASHCTL->GEN.CMDDATA0 = *data;   // Set data registers
  FLASHCTL->GEN.CMDDATA1 = *(data + 1);
  return ExecuteCommandFromRAM(); // Jump to RAM to execute command and wait for completion
}
// addr sets the location to be programmed
//   must have been previously erases
//   must be on 8-byte (64-bit boundary)
//   after erased, this location can only be programmed once
// data is an array containing count 32-bit words
// count is the number of 32-bit words to program
//   must be even
// Output: number of successful writes; return value equals count if ok
uint32_t Flash_WriteArray(uint32_t addr, const uint32_t *data, uint32_t count){
  uint32_t index=0;
  uint32_t status;
  while(index < count){
    status = Flash_Write(addr+4*index, &data[index]);
    if(status){
      return index/2;
    }
    index += 2;
  }
  return count;
}

// addr sets the sector to be erased (1024 byte boundary)
// after an erase, the flash is indeterminate until programmed
// returns 0 on pass and nonzero on fail
uint32_t Flash_Erase(uint32_t addr){
  UnprotectSector(addr);  // Unprotect sector with ECC by hardware 
// CMDTYPE
//  bits 6-4 SIZE=4 for sector
//  bits 2-0 COMMAND=2 for erase
  FLASHCTL->GEN.CMDTYPE = (4<<4) | (2);
  FLASHCTL->GEN.CMDADDR = addr; // address of sector to erase
  return ExecuteCommandFromRAM(); // Jump to RAM to execute command and wait for completion
}

