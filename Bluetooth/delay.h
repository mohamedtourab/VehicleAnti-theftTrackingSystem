/**************************************************************************************
*	File name: delay.h
*	Author: Moamen Ali
*	Date: 14/2/2018
*	Description:
*			This file contains:
*			- prototypes of functions used to generate delay using SysTick counter
*			  system timer (SysTick) is 24 bit down counter that counts down from 
*			  a reload value to zero. 
*			it features:
*			- Autoreload capability
*			- Maskable system interrupt generation when the counter reaches 0 
*	Microcontroller: STM32F407VG
***************************************************************************************/

#ifndef DELAY_H_
#define DELAY_H_

#include "stm32f407xx.h"

/**************************************************************************************
**********				  Helper Function not used by User 					 **********
**************************************************************************************/

/* 
 * This Function is used to Initializes the System Timer and its interrupt,
 * and starts the System Tick Timer.
 * it using SysTick_Config function which is built in core_cm4.h which take
 * Number of ticks between two interrupts as argument.
 * it also use the value of SystemCoreClock which is set in system-stm32f4xx.c file
 * and equals the processor clock.
 * we use this function to set the time between two interrupts to '1' ms.
*/
void vSysTick_Init (void);

/**************************************************************************************
**********				  Driver Function used by User	 					 **********
**************************************************************************************/

/* 
 * This function is used to generate a delay in milliseconds
 * Inputs:
 	- Time: the required time delay in milliseconds
*/
void Delay_ms (uint32_t Time);

#endif
