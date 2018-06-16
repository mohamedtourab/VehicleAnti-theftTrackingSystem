/****************************************************************************************************************************
*        File name: TIMER.h
*        Author: Moamen Ali & Moamen Ramadan
*        Date: 1/5/2018
*        Description:
*						 This file is the Header file for PWM driver that used to Enable the Timer peripheral in the PWM mode-
*						 to generate a PWM wave with a predetermined frequency and starting duty cycle that increased-
*						 by constant step till reach 100% and then decreased by the same step till reach zero and so on.
*						
*						 The Driver designed to use the internal crystal oscillator with-
*						 maximum frequency of 16 MHz and set Timer prescaller to zero so-
*						 timer operates on the maximum frequency 16 MHz.
*
*                        This file contains:
*                        - macros to access the TIMER registers.
*                        - prototypes of functions used to access TIMER registers.
*		 Usage:
*						 - User determines the number of channels to be used in TIMER_Cfg.h.
*						 - Set the configuration parameters in TIMER_Cfg.c:
*							.TimerId ==> The base address of the used timer (you can use the data type defined in TIMER.h)	
*							.TimerChannel ==> the used timer channel as each timer peripheral supports 4 channels
*							.GPIOChannelId ==> the index of the used gpio in GPIO_cfg.c
*							.PWMFrequency ==> Required wave Frequency in Hz.
*							.DutyCycle ==> The starting dutycycle of the required wave.
*							.Step ==> The value to be added or subtracted from the duty cycle to dim the led.
*						 - TIMER_Init function is used to initialize the peripheral registers.
*						 - PWM_Start function designed to be called periodic and each time this function called
*						   the dutycycle increased by the predetermined Step till reach 100% and then decreased by the same step
*        Microcontroller: STM32F407VG
*********************************************************************************************************************************/

#ifndef TIMER_H
#define TIMER_H
#include <stdint.h>
#include "TIMER_Cfg.h"



// enum for the return value of functions either Ok or NOK
typedef enum {TIMER_OK =0, TIMER_NOK}TIMER_ChkType;

//enum for TimerPerifID 
typedef enum {TIMER_2 = 0, TIMER_3 = 1, TIMER_4 = 2, TIMER_5 = 3}TIMER_PerifID;

/*----------------------------------------------------------------------------------------
**********		A structure type to contain all the required configuration		**********
----------------------------------------------------------------------------------------*/
typedef struct
{
	// base address of the used timer
	/* Values of TimerId can be:
			- TIMER_2
			- TIMER_3
			- TIMER_4
			- TIMER_5*/
	TIMER_PerifID	TimerId;

	// the used timer channel (Each timer has four independent channels)
	uint8_t	TimerChannel;

	// GPIO ChannelID which is the index of the used gpio in GPIO_cfg.c file
	uint8_t	GPIOChannelId;

	// Required wave Frequency in Hz with maximum frequency 16MHz and minimum 250 Hz
	uint32_t PWMFrequency;

	// Start DutyCycle
	uint8_t DutyCycle;

	// Step That will be added to the dutycycle each time user calls PWM_Start function
	uint8_t Step; 
}TIMER_ConfigType;


/*------------------------------------------------------------------------------------
**********					TIMER Registers Bit definetions					**********
------------------------------------------------------------------------------------*/

// TIMx_CR1
#define ARPE	7U
#define CMS		5U
#define DIR		4U
#define URS		2U
#define UDIS	1U
#define CEN		0U

// TIMx_SR
#define UIF		0U
#define CC1IF	1U
#define CC2IF	2U
#define CC3IF	3U
#define CC4IF	4U

//TIMx_EGR
#define UG		0U

//TIMx_CCMR1
#define CC1S	0U
#define OC1PE	3U
#define OC1M	4U
#define CC2S	8U
#define OC2PE	11U
#define OC2M	12U	

// TIMx_CCMR2
#define CC3S	0U
#define OC3PE	3U
#define OC3M	4U
#define CC4S	8U
#define OC4PE	11U
#define OC4M	12U

//TIMx_CCER
#define CC1P	1U
#define CC2P	5U
#define CC3P	9U
#define CC4P	13U
#define CC1E	0U
#define CC2E	4U
#define CC3E	8U
#define CC4E	12U

/* The Array of structure in which the user configure the required TIMER periferal in TIMER_Cfg.c*/
extern const TIMER_ConfigType TIMER_ConfigParam[TIMER_CHANNEL_NUM];


/*-------------------------------------------------------------------------------------------
**********								Driver APIs								   ********** 
-------------------------------------------------------------------------------------------*/
// Return type of all functions is TIMER_ChkType which can be either TIMER_OK or TIMER_NOK

/*A function to initialize all the TIMER channels with the configuration set in TIMER_Cfg*/
TIMER_ChkType TIMER_Init (void);

/* A function that is called to enable the output pin and each time called increases the DutyCycle 
   by the determined Step till reach 100% and then decreased by the sam step till reach 0 (Used to Dim the led) */
/*
 Inputs:
 	- ChannelId : index of the used Timer in TIMER_ConfigParam array
*/
TIMER_ChkType PWM_Start (uint32_t ChannelId);

/* A function that is called to fix the Dutycycle to zero
   Inputs:
   	  - ChannelId : index of the used Timer in Timer_ConfigParam
*/
TIMER_ChkType PWM_Clear (uint32_t ChannelId);
#endif
