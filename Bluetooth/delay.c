/**************************************************************************************
*	File name: delay.h
*	Author: Moamen Ali
*	Date: 14/2/2018
*	Description:
*			This file contains:
*			- implementations of functions used to generate delay using SysTick counter
*			  system timer (SysTick) is 24 bit down counter that counts down from 
*			  a reload value to zero. 
*			it features:
*			- Autoreload capability
*			- Maskable system interrupt generation when the counter reaches 0 
*	Microcontroller: STM32F407VG
***************************************************************************************/

#include "delay.h"

/*
 * This is the variable in which we load the required delay time
 * this value decremented each time interrupt is generated and when it reaches 
 * zero that means that we generate the required time delay in milliseconds
*/
static uint32_t SysTickCounter;

/* 
 * This Function is used to Initializes the System Timer and its interrupt,
 * and starts the System Tick Timer.
 * it using SysTick_Config function which is built in core_cm4.h which take
 * Number of ticks between two interrupts as argument.
 * it also use the value of SystemCoreClock which is set in system-stm32f4xx.c file
 * and equals the processor clock.
 * we use this function to set the time between two interrupts to '1' ms.
*/
void vSysTick_Init (void)
{
	// configure the time between two interrupts to 1 ms
	// SysTick_Config return
		// 0 to indicate that Function succeeded
		// 1 to indicate that Function faild
	// so we halt till SysTick_Config return 0 to indicate sucessful initialization
	while (SysTick_Config(SystemCoreClock / 1000) != 0);
}

/* 
 * This function is used to generate a delay in milliseconds
 * Inputs:
 	- Time: the required time delay in milliseconds
*/
void Delay_ms (uint32_t Time)
{
	// initialie the System timer
	vSysTick_Init();
	// load the required delay time to the global variable
	SysTickCounter = Time;
	// halt the program till the global variable reaches zero which indicates
	// that we generate the required time delay
	while (SysTickCounter != 0);
}

/*
 * This is the interrupt service routine which is executed each 1 millisecond
 * in this function we decrement the global variable SysTickCounter which is loaded
 * by the required delay to indicate that 1 millisecond has passed
*/
void SysTick_Handler (void)
{
	// check if the counter not zero
	if (SysTickCounter != 0)
	{ 
		// decrement the counter
		SysTickCounter--;
	}
}
