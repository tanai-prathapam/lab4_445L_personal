/**
 * @file      DFT.h
 * @brief     16-point discrete fourier transform
 * @details   16-point discrete fourier transform<br>
 * Let the sampling rate be fs<br>
 * first frequency f1 is fs/16<br>
 * second frequency f2 is 2*fs/16
 * @author    Valvano
 * @warning   AS-IS
 * @date      July 29, 2026
 */
 /*!
 * @defgroup Math
 * @brief Software implementations of math functions
 * @{*/
#ifndef __DSP_H__
#define __DSP_H__
#include <stdint.h>

/**
 * Initialization of noise reject filter<br>
 * compare audio signal with itself two samples ago<br>
 * Rxx(2) = (1/N) sum{x(n)x(n-2)} in limit as N-> infinity<br>
 * Approximate infinite sum with IIR filter<br>
 * Rxx2 = (127*Rxx2 +x(n)x(n-2) )/128<br>
 - K is the attack ratio (how fast it responds)
 - M is the scale
 - t0 = x(n)/32
 - t1 = x(n-1)/32
 - t2 = x(n-2)/32
 * @param none
 * @return none
 * @brief  Initialization of noise reject filter
*/
void NoiseReject_Init(void);

/**
 * Run noise reject filter<br>
 * Rxx2 is 0 if uncorrelated (noise)<br>
 * Rxx2 is 128 if correlated (signal)<br>
 * Rxx2 is -128 if correlated (signal); this does not occur with sound
 * @param x input to filter
 * @return output of filter, (Rxx2*x)/128
 * @brief removes random noise from audio signal
*/
int32_t NoiseReject(int32_t x);

/**
 * Initialization of 16-point discrete fourier transform
 * @param none
 * @return none
 * @brief  Initialization of discrete fourier transform
*/
void DFT_Init(void);


/**
 * One point added to discrete fourier transform
 * @param i is 0 to 15 index
 * @param x is input data at index i
 * @return none
 * @brief  Run DFT
*/
void DFT(uint32_t i, int32_t x);

/**
 * Calculate magnitude of the first frequency
 * @param none
 * @return magnitude at frequency f1
 * @brief  magnitude of the first frequency
*/
int32_t Mag1(void);


/**
 * Calculate magnitude of the second frequency
 * @param none
 * @return magnitude at frequency f2
 * @brief  magnitude of the second frequency
*/
int32_t Mag2(void);

#endif
