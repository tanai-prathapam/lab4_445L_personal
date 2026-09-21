/**
 * @file      Matrix.h
 * @brief     16-key matrix keyboard connected to PA27-PA24 and PB3-PB0
 * @details   Matrix scanned keyboard
 * For more information see<br> Section 3.4 in https://users.ece.utexas.edu/~valvano/EE445L/ebook/index.htm<br>
 * @author    Valvano
 * @warning   AS-IS
 * @date      July 29, 2026
  <table>
<caption id="Keyboardpins">Pins on the 16-key matrix keyboard </caption>
<tr><th>Pin <th>GPIO<th>Hardware
<tr><td>PA27 <td>input<td>column 3 (keypad pin 4) using 10K pull-up
<tr><td>PA26 <td>input<td>column 2 (keypad pin 3) using 10K pull-up
<tr><td>PB17 <td>input<td>column 1 (keypad pin 2) using 10K pull-up
<tr><td>PA25 <td>input<td>column 0 (keypad pin 1) using 10K pull-up
<tr><td>PB3 <td>output<td>row 3 (keypad pin 8)
<tr><td>PB2 <td>output<td>row 1 (keypad pin 7)
<tr><td>PB1 <td>output<td>row 1 (keypad pin 6)
<tr><td>PB0 <td>output<td>row 0 (keypad pin 5)
</table>

* [1] [2] [3] [A]<br>
* [4] [5] [6] [B]<br>
* [7] [8] [9] [C]<br>
* [*] [0] [#] [D]<br>
* Pin1 . . . . . . . . Pin8<br>
* Pin 1 -> Column 0 (column starting with 1)<br>
* Pin 2 -> Column 1 (column starting with 2)<br>
* Pin 3 -> Column 2 (column starting with 3)<br>
* Pin 4 -> Column 3 (column starting with A)<br>
* Pin 5 -> Row 0 (row starting with 1)<br>
* Pin 6 -> Row 1 (row starting with 4)<br>
* Pin 7 -> Row 2 (row starting with 7)<br>
* Pin 8 -> Row 3 (row starting with *)<br>
  ******************************************************************************/
#ifndef __MATRIX_H__
#define __MATRIX_H__

// Runs on  MSPM0
// Provide functions that initialize GPIO ports and SysTick 
// Use periodic polling
// Jonathan Valvano
// July 18, 2026

/* This example accompanies the book
   "Embedded Systems: Real Time Interfacing to Arm Cortex M Microcontrollers",
   ISBN: 978-1463590154, Jonathan Valvano, copyright (c) 2026


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




/**
 * Initialization of Matrix keypad, 40 Hz SysTick interrupt
 * @param none
 * @return none
 * @brief  Initialize matrix keyboard
 * @note LaunchPad_Init has been called; this program should not reset Port A or B
 */
void Matrix_Init(void);
       
// 
// 
/**
 * input ASCII character from keypad
 * @param none
 * @return ASCII character of key touched
 * @note spin if Fifo is empty
 * */
char Matrix_InChar(void);


#endif // __MATRIX_H__