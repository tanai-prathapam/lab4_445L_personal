/**
 * @file      mainPage.h
 * @brief     Doxygen text for main page.
 * @author    Daniel Valvano and Jonathan Valvano
 * @copyright Copyright 2026 by Jonathan W. Valvano, valvano@mail.utexas.edu,
 * @warning   AS-IS
 * @note      For more information see  http://users.ece.utexas.edu/~valvano/
 * @date      July 29, 2026
 * */

/*!
 * \mainpage ECE445L Introduction to Embedded Systems
 * July 29, 2026
 * \author Jonathan Valvano, PhD
 * \author Daniel Valvano, BS
 *
 * \section sec1 Overview
\par
An embedded system is a system that performs a specific task and has a computer embedded inside. A system is comprised of components and interfaces connected for a common purpose. This book is an introduction to embedded systems. Specific topics include the MSPM0+ microcontroller, finite-state machines, debugging, fixed-point numbers, the design of software in assembly language and C, elementary data structures, programming input/output, interrupts, measurements with analog to digital conversion, graphics, sound production with digital to analog conversion, introduction to networks using serial communication, and real-time systems.
\par
There is a web site accompanying this book https://users.ece.utexas.edu/~valvano/EE445L/ebook/index.htm. Posted here are projects for each of the example programs in the book

 * \section sec2 Hardware
\par
The hardware used in these projects include
 \li MSPM0G3507 LaunchPad (LP-MSPM0G3507)
 \li RSLK2 Robot Systems Learning Kit 
 \li LEDs, Switches, slide pot, speaker
 \li ST7735R or SSD1306 display
<br>

These project utilize the MSPM0G3507 LaunchPad.
For more information see<br> https://www.ti.com/product/LP-MSPM0G3507/part-details/LP-MSPM0G3507<br> 
 * \image html Fg01_07_02_LaunchPad.png width=500px
 * \image latex Fg01_07_02_LaunchPad.png "TI MSPM0 LaunchPad" width=10cm
 *
 *

The RSLK2 Robot Systems Learning Kit is platform with lots of I/O.
For more information see<br>
https://docs.google.com/document/d/11wp2RbnZV15mRgctMgae81lJd9SU2MToSahXqwRGQOs/edit?tab=t.0<br>
 * \image html RSLK.png width=500px
 * \image latex RSLK.png "TI Educational BoosterPack MKII" width=10cm
 *


 * \section sec3 Modules
 * \par
The documentation is divided into modules.
 \li ADC contains the analog to digital converter
 \li Display contains the ST7735R and SSD1306 displays
 \li RSLK contains the sensors and motor interfaces
 \li CAN contains the controller area network code
 \li Clock contains the MSPM0G3507 clock and timers
 \li DAC contains the digital to analog converter
 \li Debugging contains dump, TExaS scope, and TExaS logic analyzer
 \li EdgeTriggered contains edge-triggered interrupts 
 \li Wifi contains ESP8266 code 
 \li Math contains digital filters, DFT, and trig functions 
 \li FIFO contains software a first in first out queue
 \li Flash contains software to erase and program the internal flash
 \li LaunchPad contains software to input from LaunchPad switches and output LaunchPad LEDs
 \li SPI contains synchronous serial communication
 \li PWM contains pulse width modulation software
 \li Timer contains periodic interrupts and period and pulsewidth measurements
 \li UART contains universal asynchronous receiver/transmitter software


*/


