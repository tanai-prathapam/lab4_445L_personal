/*!
 * @defgroup SPI
 * @brief Synchronous serial communication
 <table>
<caption id="SPIpins5">SPI-LCD pins </caption>
<tr><th>Pin <th>Function  <th>Description
<tr><td>PB9 <td>SPI1 SCLK <td>LCD SPI clock (SPI)   
<tr><td>PB6 <td>GPIO CS   <td>LCD SPI CS     
<tr><td>PA12<td>GPIO CS   <td>SDC SPI CS     
<tr><td>PB8 <td>SPI1 PICO <td>LCD SPI data (SPI)    
<tr><td>PB7 <td>SPI1 POCI <td>SCD SPI data (SPI)    
<tr><td>PB15<td>GPIO      <td>LCD !RST =1 for run, =0 for reset  
<tr><td>PA13<td>GPIO      <td>LCD D/C RS =1 for data, =0 for command  
</table>
 * @{*/
/**
 * @file      SPI.h
 * @brief     Synchronous serial communication
 * @details   SPI uses a chip select, clock, data out and data in. This interface is used for TFT and SDC
 * This interface can be used for MKII LCD and the ST7735R LCD
 * \image html SPIinterface.png  width=500px
 * @version   RTOS v7.0
 * @author    Daniel Valvano and Jonathan Valvano
 * @copyright Copyright 2025 by Jonathan W. Valvano, valvano@mail.utexas.edu,
 * @warning   AS-IS
 * @note      For more information see  http://users.ece.utexas.edu/~valvano/
 * @date      Dec 26, 2025
 <table>
<caption id="SPIpins6">SPI-LCD pins </caption>
<tr><th>Pin <th>Function  <th>Description
<tr><td>PB9 <td>SPI1 SCLK <td>LCD SPI clock (SPI)   
<tr><td>PB6 <td>GPIO CS   <td>LCD SPI CS     
<tr><td>PA12<td>GPIO CS   <td>SDC SPI CS     
<tr><td>PB8 <td>SPI1 PICO <td>LCD SPI data (SPI)    
<tr><td>PB7 <td>SPI1 POCI <td>SCD SPI data (SPI)    
<tr><td>PB15<td>GPIO      <td>LCD !RST =1 for run, =0 for reset  
<tr><td>PA13<td>GPIO      <td>LCD D/C RS =1 for data, =0 for command  
</table>
  ******************************************************************************/
#ifndef __SPI_H__
#define __SPI_H__

#define ECE445L_Lab6 0
#if ECE445L_Lab6
// SPI1 SCLK
#define SPI1_SCLKINDEX (PA17INDEX)  // PA17 is SPI1 SCLK

// SPI1 PICO
#define SPI1_PICOINDEX (PA18INDEX)  // PA18 is SPI1 PICO MOSI

// SPI1 POCI MISO
#define SPI1_POCIINDEX (PA16INDEX)  // PA16 is  SPI1 POCI MISO

// SDC CS
#define SDC_CS           GPIOA
#define SDC_CS_PIN       (1<<25)         // CS controlled by software
#define SDC_CS_INDEX     (PA25INDEX)     // PA25 GPIO
#define SDC_CS_LOW()     (SDC_CS->DOUTCLR31_0 = SDC_CS_PIN)   // PA25 low
#define SDC_CS_HIGH()    (SDC_CS->DOUTSET31_0 = SDC_CS_PIN)   // PA25 high

// TFT CS
#define TFT_CS           GPIOA
#define TFT_CS_PIN       (1<<27)         // TFT CS controlled by software
#define TFT_CS_INDEX     (PA27INDEX)     // PA27 GPIO
#define TFT_CS_LOW()     (TFT_CS->DOUTCLR31_0 = TFT_CS_PIN)   // PA27 low
#define TFT_CS_HIGH()    (TFT_CS->DOUTSET31_0 = TFT_CS_PIN)   // PA27 high

// TFT DC
#define TFT_DC           GPIOA
#define TFT_DC_PIN       (1<<13)         // D/C controlled by software
#define TFT_DC_INDEX     (PA13INDEX)     // PA13 GPIO
#define TFT_DC_LOW()     (TFT_DC->DOUTCLR31_0 = TFT_DC_PIN)   // PB16 low
#define TFT_DC_HIGH()    (TFT_DC->DOUTSET31_0 = TFT_DC_PIN)   // PB16 high

// TFT RST
#define TFT_RST          GPIOA
#define TFT_RST_PIN      (1<<24)         // !RST controlled by software
#define TFT_RST_INDEX    (PA24INDEX)     // PB15 GPIO
#define TFT_RST_LOW()    (TFT_RST->DOUTCLR31_0 = TFT_RST_PIN)   // PA24 low
#define TFT_RST_HIGH()   (TFT_RST->DOUTSET31_0 = TFT_RST_PIN)   // PA24 high

#else
// SPI1 SCLK
#define SPI1_SCLKINDEX (PB9INDEX)  // PB9 is SPI1 SCLK

// SPI1 PICO
#define SPI1_PICOINDEX (PB8INDEX)  // PB8 is SPI1 PICO MOSI

// SPI1 POCI
#define SPI1_POCIINDEX (PB7INDEX)  // PB7 is SPI1 POCI MISO

// SDC CS
#define SDC_CS           GPIOA
#define SDC_CS_PIN       (1<<12)         // CS controlled by software
#define SDC_CS_INDEX     (PA12INDEX)     // PA12 GPIO
#define SDC_CS_LOW()     (SDC_CS->DOUTCLR31_0 = SDC_CS_PIN)   // PA12 low
#define SDC_CS_HIGH()    (SDC_CS->DOUTSET31_0 = SDC_CS_PIN)   // PA12 high

// TFT CS
#define TFT_CS           GPIOB
#define TFT_CS_PIN       (1<<6)         // TFT CS controlled by software
#define TFT_CS_INDEX     (PB6INDEX)     // PB6 GPIO
#define TFT_CS_LOW()     (TFT_CS->DOUTCLR31_0 = TFT_CS_PIN)   // PB6 low
#define TFT_CS_HIGH()    (TFT_CS->DOUTSET31_0 = TFT_CS_PIN)   // PB6 high

// TFT DC
#define TFT_DC           GPIOA
#define TFT_DC_PIN       (1<<13)         // D/C controlled by software
#define TFT_DC_INDEX     (PA13INDEX)     // PA13 GPIO
#define TFT_DC_LOW()     (TFT_DC->DOUTCLR31_0 = TFT_DC_PIN)   // PB16 low
#define TFT_DC_HIGH()    (TFT_DC->DOUTSET31_0 = TFT_DC_PIN)   // PB16 high

// TFT RST
#define TFT_RST          GPIOB
#define TFT_RST_PIN      (1<<15)         // !RST controlled by software
#define TFT_RST_INDEX    (PB15INDEX)     // PB15 GPIO
#define TFT_RST_LOW()    (TFT_RST->DOUTCLR31_0 = TFT_RST_PIN)   // PB15 low
#define TFT_RST_HIGH()   (TFT_RST->DOUTSET31_0 = TFT_RST_PIN)   // PB15 high
#endif

/**
 * Output 8-bit data to SPI port.
 * RS=PA13=1 for data.
 * @param data is an 8-bit data to be transferred
 * @return none 
 * @brief Output data
 */
void SPI_OutData(char data);

/**
 * Output 8-bit command to SPI port.
 * RS=PA13=0 for command
 * @param  command is an 8-bit command to be transferred
 * @return none 
 * @brief Output command
 */
void SPI_OutCommand(char command);

/**
 *  Reset LCD 
 * -# drive RST high for 500ms
 * -# drive RST low for 500ms
 * -# drive RST high for 500ms
 * 
 * @param none
 * @return none 
 * @brief Reset LCD
 */
void SPI1_Reset(void);

// SDC CS initialization
void CS_Init(void);

/**
 * Initialize SPI1 for 8 MHz baud clock
 * for both SDC and TFT
 * using busy-wait synchronization.
 * Calls Clock_Freq to get bus clock
 * - PB9 SPI1 SCLK   
 * - PB6 SPI1 CS0       
 * - PB8 SPI1 PICO  
 * - PB15 GPIO !RST =1 for run, =0 for reset    
 * - PA13 GPIO RS =1 for data, =0 for command
 *
 * @note SPI0,SPI1 in power domain PD1 SysClk equals bus CPU clock
 * @param none
 * @return none 
 * @brief initialize SPI1
 */
void SPI1_Init(void);

 //---------TFT_OutCommand------------
 // Output 8-bit command to SPI port
 // Input: data is an 8-bit data to be transferred
 // Output: none
 void TFT_OutCommand(char command);

/**
 * Output 8-bit data to SPI port.
 * RS=PB16=1 for data.
 * @param data is an 8-bit data to be transferred
 * @return none 
 * @brief Output data
 */
void TFT_OutData(char data);


#endif // __SPI_H__
/** @}*/