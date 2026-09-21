/* MCP4921.c
 * Jonathan Valvano
 * June 29, 2026
 */
#include <ti/devices/msp/msp.h>
#include "../inc/Clock.h"
#include "../inc/LaunchPad.h"

// calls Clock_Freq to get bus clock
// initialize SPI for 8 MHz baud clock
// 16 bit data size
// busy-wait synchronization

// MCP4921
// pin signal MSPM0   mode
//  1   Vdd   +3.3V
//  2   /CS   PA8    SPI0_CS0
//  3   SCLK  PB18   SPI0_SCK
//  4   SDI   PB17   SPI0_PICO
//  5   /LDAC ground
//  6   REF          1.255 V refernce
//  7   AGND         ground
//  8   OUT          DAC output

// SPI0,SPI1 in power domain PD1 SysClk equals bus CPU clock



//---------MCP4921_Out------------
// Output 16-bit data to SPI0 port
// Input: data is  16-bit data to be transferred
// Output: none
// bit 14 is 0 to run unbuffered (LDAC grounded)
// bit 13 is 0 for gain=2 (1.255V reference gives 0 to 2.51V output)
// bit 12 is 1 to run in active mode
// bits 11-0 are data
void MCP4921_Out(uint32_t data){
    // ****ECE445L write this ****
 
}

// initialize MCP4921 for 8 MHz baud clock
// 16 bit data size
// busy-wait synchronization
// PA8 SPI0_CS0
// PB18 SPII_SCK
// PB17 SPI_PICO
void MCP4921_Init(uint32_t data){
  // ECE445L Lab 5, enter code here

}
