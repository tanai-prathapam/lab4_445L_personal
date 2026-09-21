/* BNO085.c
 * Jonathan Valvano
 * Date: 5/9/2026
   BNO085 accelerometer
 */


#include <ti/devices/msp/msp.h>
#include "../inc/BNO085.h"
#include "../inc/Clock.h"
#include "../inc/LaunchPad.h"


// The BNO085 could be connected to any UART Rx pin. 
// The accelerometer could be interfaced to either the sensor or the motor board
// Tested here is the BNO085 connected to UART3 on the robot board

// BNO085 MSPM0
// Vcc    3.3V  Power
// GND    GND
// SCL,AD0,CS,INT,PS1 left not connected
// SDA    PB13 RxD: is UART3 Rx (BNO085 to MSPM0) baud=115200 bps
// RST    PB12 GPIO output (drive low to reset)
// PS0    3.3V to select UART-RVC mode (there is a solder jumper on back)

#define BNO085_RX PB13INDEX
#define BNO085_RST PB12INDEX
#define BNO085_RST_PORT GPIOB
#define BNO085_RST_PIN (1<<12)
int16_t BNO085data[6];
  // Yaw   0.01 deg
  // Pitch 0.01 deg
  // Roll  0.01 deg 
  // X-acceleration 0.001g
  // Y-acceleration 0.001g
  // Z-acceleration 0.001g
uint8_t BNOIndex; // should be sequential
// To determine the actual orientation of the module, the rotations should be applied in the order yaw, pitch then roll.
void (*BNO085Function)(int16_t data[6]); 
int BNO085Index;
uint8_t BNO085LastByte;
uint8_t BNO085DataMessage[20]; // 19 byte fixed size message
int BNO085BadCheckSum; // errors
void BNO085_Reset(uint32_t time){
  BNO085_RST_PORT->DOUTCLR31_0 = BNO085_RST_PIN; // RST=0
  Clock_Delay(time); // time for BNO085 to reset
  BNO085_RST_PORT->DOUTSET31_0 = BNO085_RST_PIN; // RST=1    
}
// power Domain PD1
// for 32MHz bus clock, UART clock is 32MHz
// for 40MHz bus clock, UART clock is MCLK 40MHz
// for 80MHz bus clock, UART clock is MCLK 80MHz
//------------BNO085_Init------------
// Initialize the UART3 for 115,200 baud rate (assuming 80 MHz clock),
// 8 bit word length, no parity bits, one stop bit, FIFOs enabled
// Input: user function for real time sampling, passes pointer to 6 data values
// Output: none
void BNO085_Init(void (*function)(int16_t data[6])){
    // do not reset or activate PortA, already done in LaunchPad_Init
    // RSTCLR to GPIOB and UART3 peripherals
    //   bits 31-24 unlock key 0xB1
    //   bit 1 is Clear reset sticky bit
    //   bit 0 is reset gpio port
 // GPIOB->GPRCM.RSTCTL = (uint32_t)0xB1000003; // called previously
  UART3->GPRCM.RSTCTL = 0xB1000003;
    // Enable power to GPIOA and UART3 peripherals
    // PWREN
    //   bits 31-24 unlock key 0x26
    //   bit 0 is Enable Power
 // GPIOB->GPRCM.PWREN = (uint32_t)0x26000001; // called previously
  UART3->GPRCM.PWREN = 0x26000001;
  Clock_Delay(24); // time for uart to power up

 // the following code selects which pins to use
  IOMUX->SECCFG.PINCM[BNO085_RX]  = 0x00040082;
  //bit 18 INENA input enable
  //bit 7  PC connected
  //bits 5-0=2 for UART3_Rx

  // configure GPIO output function
  IOMUX->SECCFG.PINCM[BNO085_RST]  = 0x00000081;
  //bit 7  PC connected
  //bits 5-0=1 for GPIO
  BNO085_RST_PORT->DOE31_0 |= BNO085_RST_PIN; // enable outputs
  BNO085_RST_PORT->DOUTSET31_0 = BNO085_RST_PIN; // RST=1
  UART3->CLKSEL = 0x08; // bus clock
  UART3->CLKDIV = 0x00; // no divide
  UART3->CTL0 &= ~0x01; // disable UART3
  UART3->CTL0 = 0x00020008;
   // bit  17    FEN=1    enable FIFO
   // bits 16-15 HSE=00   16x oversampling
   // bit  14    CTSEN=0  no CTS hardware
   // bit  13    RTSEN=0  no RTS hardware
   // bit  12    RTS=0    not RTS
   // bits 10-8  MODE=000 normal
   // bits 6-4   TXE=000  disable TxD
   // bit  3     RXE=1    enable TxD
   // bit  2     LBE=0    no loop back
   // bit  0     ENABLE   0 is disable, 1 to enable
  if(Clock_Freq() == 40000000){
      // 40000000/16 = 2,500,000 Hz
     // Baud = 115200
      //    2,500,000/115200 = 21.701388888
      //   divider = 21+45/64 = 21.703125
    UART3->IBRD = 21;
    UART3->FBRD = 45; // baud =2,500,000/10.84375 = 115,273.77
  }else if (Clock_Freq() == 32000000){
    // 32000000/16 = 2,000,000
     // Baud = 115200
      //    2,000,000/115200 = 17.3611111
      //   divider = 21+23/64 = 17.359375
    UART3->IBRD = 17;
    UART3->FBRD = 23; // 115,211.52
  }else if (Clock_Freq() == 80000000){
     // 80000000/16 = 5,000,000 Hz
     // Baud = 115200
      //    5,000,000/115200 = 43.4027777
      //   divider = 43+26/64 = 43.40625
    UART3->IBRD = 43;
    UART3->FBRD = 26; // baud =5,000,000/43.40625 = 115,190.78
  }else return;
  BNO085Function = function;
  BNO085Index = 0; // looking for two 59s
  BNO085LastByte = 0;
  BNO085BadCheckSum = 0;
  BNO085DataMessage[0] = BNO085DataMessage[1] = 0xAA;
  UART3->LCRH = 0x00000030;
   // bits 5-4 WLEN=11 8 bits
   // bit  3   STP2=0  1 stop
   // bit  2   EPS=0   parity select
   // bit  1   PEN=0   no parity
   // bit  0   BRK=0   no break
  UART3->CPU_INT.IMASK = 0x0401;
  // bit 11 TXINT no
  // bit 10 RXINT yes
  // bit 0  Receive timeout, yes
  UART3->IFLS = 0x0422;
  // bits 11-8 RXTOSEL receiver timeout select 4 (0xF highest)
  // bits 6-4  RXIFLSEL 2 is greater than or equal to half
  // bits 2-0  TXIFLSEL 2 is less than or equal to half (not used)
  NVIC->ICPR[0] = 1<<3; // UART3 is IRQ 3
  NVIC->ISER[0] = 1<<3;
  NVIC->IP[0] = (NVIC->IP[0]&(~0xFF000000))|(1<<30);    // set priority (bits 31-30) IRQ 3
  UART3->CTL0 |= 0x01; // enable UART3
  BNO085_Reset(8000); // 100us
}
//uint8_t BNOdump[64];
//uint32_t BNOdumpindex=0;

// copy from hardware RX FIFO to process UART-RVC protocol
void static copyHardwareToSoftware3(void){
  uint8_t letter;
  while((UART3->STAT&0x04)==0){
    letter = UART3->RXDATA;
//    if(BNOdumpindex<64){
//      BNOdump[BNOdumpindex] = letter; 
//      BNOdumpindex++;
//    }
    if(BNO085Index == 0){ // header is AAAA
      if((letter == 0xAA)&&(BNO085LastByte == 0xAA)){
        BNO085Index = 2; // found header in message
      }
      BNO085LastByte = letter;
    }else{
      BNO085DataMessage[BNO085Index] = letter;
      BNO085Index++;
      if(BNO085Index == 19){
        BNO085LastByte = 0; // get ready for next message
        BNO085Index = 0;
        uint8_t check=0;
        for(int i=2;i<18;i++){ // does not include AAAA header
          check += BNO085DataMessage[i];
        }
        if(check == BNO085DataMessage[18]){
          BNOIndex = BNO085DataMessage[2];
          BNO085data[0] = (int16_t)(BNO085DataMessage[4]*256+BNO085DataMessage[3]);
          BNO085data[1] = (int16_t)(BNO085DataMessage[6]*256+BNO085DataMessage[5]);
          BNO085data[2] = (int16_t)(BNO085DataMessage[8]*256+BNO085DataMessage[7]);
          BNO085data[3] = (int16_t)(BNO085DataMessage[10]*256+BNO085DataMessage[9]);
          BNO085data[4] = (int16_t)(BNO085DataMessage[12]*256+BNO085DataMessage[11]);
          BNO085data[5] = (int16_t)(BNO085DataMessage[14]*256+BNO085DataMessage[13]);
            // call back
          (*BNO085Function)(BNO085data);     // pass distance back to higher level
        }else{
          BNO085BadCheckSum++; // error
        }
      }        
    }
  }
}

void UART3_IRQHandler(void){ uint32_t status;
  status = UART3->CPU_INT.IIDX; // reading clears bit in RIS
  if(status == 0x01){   // 0x01 receive timeout
    copyHardwareToSoftware3();
  }else if(status == 0x0B){ // 0x0B receive
    copyHardwareToSoftware3();
  }
}
