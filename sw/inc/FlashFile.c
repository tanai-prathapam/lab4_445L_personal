// FlashFile.c
// Runs on MSPM0
// High-level implementation of the file system implementation.
// Daniel and Jonathan Valvano
// June 6, 2026
#include <stdint.h>
#include "../inc/Flash.h"
#include "../inc/FlashFile.h"
// This solution is predicated on the assumption that erased flash will read as all 1's
// The data sheet says flash is indeterminate after erasing, but experimentation shows it as all 1's

// After erasing 
//uint8_t Buff[512];
// File system is in Flash locations 0x10000 to 0x1FFFF (64k)
// Since MSPM0 has a simple way to write 64-bit data, all pointers are 64 bits
// Sector size is 1k (erase boundary), 64 sectors
// Block size is 256 bytes
// Write data size is 64 bits, 8 bytes, one flash word

// Directory/FAT are in last two sectors of Flash Rom
//   Directory has 16 entries for file numbers 0 to 15 (16*8=128=0x80 bytes)
//   FAT has 240 entries for blocks 0 to 239 (240 blocks each 256 bytes is 61,440 bytes 
//   2 sectors for Directory/FAT leaving 62 sectors for data (63,488 bytes) (most of 64k Flash)
//   Directory+FAT is (16+240) 64-bit values = 2048 bytes (last two sectors 0x1F800-0x1FFFF)
//   Since directory/FAT are in ROM, they are always loaded, valid, and available
#define STARTFLASH 0x10000
#define ENDFLASH   0x20000
#define DIRECTORY 0x1F800
#define FILEALLOCATIONTABLE (0x1F800+0x80)
// although the directory and FAT entries are stored as 64 bits to simplify programming
//   each entry uses just the least significant 32 bits to speed up execution
// these macros read from directory and FAT
#define Directory(i) (*(uint32_t *)(DIRECTORY+8*i))
//#define Directory (uint64_t *) 0x1F800
//#define FAT (uint64_t *) (0x1F800+0x80)
#define FAT(i) (*(uint32_t *)(DIRECTORY+0x80+8*i))
#define MAXFILE 16
#define MAXBLOCK 240
//#define FREE64 ((uint64_t) 0xFFFFFFFFFFFFFFFF)
#define FREE ((uint32_t) 0xFFFFFFFF)

// Return the larger of two integers.
uint32_t max(uint32_t a, uint32_t b){
  if(a > b){
    return a;
  }
  return b;
}


// Return the index of the last block in the file
// associated with a given starting block.
// Note: This function will loop forever without returning
// if the file has no end (i.e., the FAT is corrupted).
uint32_t lastblock(uint32_t n){
  uint32_t m;
//  if(n == FREE){
//    return FREE;    // disk completely empty
//  }
  while(1){
    m = FAT(n);
    if(m == FREE) return n;
    n = m;
  }
}

// Return the index of a free block.
// Input: none
// Output: 0 to 239 of a free block
//         240 if there are no free blocks
uint32_t findfreeblock(void){
  uint32_t lu = 0; // last block used in entire file system
  uint32_t lt;     // last block of this file
  uint32_t num = 0;      // file number
  if(Directory(0) == FREE) return 0; // empty disk
  uint32_t first = Directory(num);
  while((first != FREE)&&(num<16)){
    lt = lastblock(first);    
    lu = max(lu, lt);
    num = num + 1;
    first = Directory(num);
  }
  return lu+1;
}

// Append a sector index 'n' at the end of file 'num'.
// This helper function is part of OS_File_Append(), which
// should have already verified that there is free space,
// so it returns 0 (successful).
// Error: return 255 if the FAT is corrupted or if hardware error
uint32_t appendfat(uint32_t num, uint32_t n){ uint32_t result;
  uint32_t m; // find previous last block of file 'num'
  uint32_t i = Directory(num);
  uint32_t data[2]; 
  if(i == FREE) return 255; // error, this file is empty?? bug
  m = FAT(i);
  uint32_t count=0;
  while(m != FREE){
    i = m;
    m = FAT(i);
    count++;
    if(count>240) return 255; // invalid FAT
  }
  // program  FAT[i] = n;
  data[0] = n; // ls word
  data[1] = 0; // pointer to first block
  result = Flash_Write(FILEALLOCATIONTABLE+8*i, data);
  return result;
}

//********OS_File_New*************
// Create a new file and save first block
// Save 256 bytes into the new file
// Can support up to 16 files
// Inputs:  buf, pointer to 256 bytes of data
// Outputs: number of the new file, 0 to 15
// Errors: return 255 on failure or disk full
uint32_t OS_File_New(uint32_t buf[64]){uint32_t result;
  uint32_t num = 0;
  uint32_t block = findfreeblock();
  uint32_t data[2]; 
  if(block >= 240) return 255; // disk is full
  while(Directory(num) != FREE){
    num++; 
    if(num >= MAXFILE) return 255;
  }
  data[0] = block;
  data[1] = 0; // pointer to first block
  // store block number of files first block into Directory(num)
  result = Flash_Write(DIRECTORY+8*num, data);
  if(result) return 255; // hardware error
  // the 0xFFFFFFFFFFFFFFFF is already at FAT(block)
  // write first 256-byte data to disk
  result = Flash_WriteArray(STARTFLASH+256*block, buf, 64);
  if(result != 64) return 255; // hardware error
  return num;
}

//********OS_File_Size*************
// Check the size of this file
// Inputs:  num, 32-bit file number, 0 to 15
// Outputs: 0 if file has not been created, 
//  otherwise return the number of blocks allocated to the file
// Errors:  return 255 if FAT is invalid
uint32_t OS_File_Size(uint32_t num){
  uint64_t block; 
  uint32_t size;
  block = Directory(num);
  size = 0;
  while(block != FREE){
    if(block > 240)return 255; // invalid FAT
    block = FAT(block);
    size = size + 1;
    if(size > MAXBLOCK) return 255;// invalid FAT
  }
  return size;
}

//********OS_File_Append*************
// Save 256 bytes into the file
// Inputs:  num, 32-bit file number, 0 to 15
//          buf, pointer to 256 bytes of data
// Outputs: 0 if successful
// Errors:  255 on failure or disk full
uint32_t OS_File_Append(uint32_t num, uint32_t buf[64]){
  uint32_t block; uint32_t result;
  if(Directory(num) == FREE) return 255; // no file 'num'
  block = findfreeblock();
  if(block >= 240){
    return 255;              // disk is full
  }
  result = Flash_WriteArray(STARTFLASH+256*block, buf, 64);
  if(result != 64) return 255; // hardware error
  result = appendfat(num, block);
  return result;
}

//********OS_File_Read*************
// Read 256 bytes from the file
// Inputs:  num, file number, 0 to 254
//          location, logical address, 0 to 239
// Outputs: pointer to 256 bytes of the file if successful
// Errors:  0 on failure because no data
uint32_t *OS_File_Read(uint32_t num, uint32_t location){
  uint32_t block;
  int i;
  block = Directory(num);
  if(block == FREE){
    return 0;                 // file not found
  }
  for(i = 0; i < location; i = i+1){
    block = FAT(block);
    if(block == FREE){
      return 0;               // reached the end of the file
    }
  }
  return (uint32_t *)(STARTFLASH+256*block);
}


//********OS_File_Format*************
// Erase all files and all data
// Inputs:  none
// Outputs: 0 if success
// Errors:  1 on disk write failure
uint32_t OS_File_Format(void){
  uint32_t result;
  for(uint32_t addr = 0x10000; addr < 0x20000; addr = addr+1024){
    result = Flash_Erase(addr);
	  if(result) return 1;  // disk erase failure
  }
  return 0;
}
