/**
 * @file      Median.h
 * @brief     Various implementations of the median filter
 * @details   Median filters are typically used to remove outliers (shot noise)<br>
 * @author    Valvano
 * @warning   AS-IS
 * @date      July 29, 2026
 */
 /*!
 * @defgroup Math
 * @brief Software implementations of math functions
 * @{*/
#ifndef __Median_H__
#define __Median_H__
/**
 * 5-wide median filter
 * @param u an array of 5 samples
 * @return median of the samples
 * @brief  5-wide median filter
*/ 
uint32_t median5(uint32_t u[5]);

/**
 * 3-wide median filter<br>
 * Includes a 3-deep MACQ of last three samples
 * @param x new data sample
 * @return median of the last three samples
 * @brief  3-wide median filter
 * @note 1 usec running at 80  MHz
*/ 
int8_t Median(int8_t x);

/**
 * 3-wide median filter
 * @param u1 new data sample
 * @param u2 new data sample
 * @param u3 new data sample
 * @return median of u1 u2 u3
 * @brief  3-wide median filter
*/ 
int8_t Median3(int8_t u1,int8_t u2,int8_t u3);


/**
 * 5-wide median filter<br>
 * Includes a 5-deep MACQ of last five samples
 * @param x new data sample
 * @return median of the last five samples
 * @brief  5-wide median filter
 * @note 11 usec running at 80  MHz
 * @warning Median5 and Median7 use the same MACQ
*/ 
int8_t Median5(int8_t x);


/**
 * 7-wide median filter<br>
 * Includes a 7-deep MACQ of last seven samples
 * @param x new data sample
 * @return median of the last seven samples
 * @brief  7-wide median filter
 * @note 16 usec running at 80  MHz
 * @warning Median5 and Median7 use the same MACQ
*/ 
int8_t Median7(int8_t x);

#endif // __Median_H__

/** @}*/
