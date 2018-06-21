/**************************************************************************************
*	File name: ErrorHandler.c
*	Auther : 
*		-Abdelrhman Hosny
*		-Amr Mohamed
*	Date: 27/4/2018
*	Description:
*		This file contains:
*			- implementation of functions that handles the errors
*	Microcontroller: STM32F407VG
***************************************************************************************/ 
#include "ErrorHandler.h"

/*************************************************************************************
**********							Global Variables						**********
*************************************************************************************/
uint8_t ErrorFlags[ERROR_HANDLER_NUMBER_OF_ERROR_SOURCES] = {0};


/*************************************************************************************
**********					Error handler callback function					**********
*************************************************************************************/

/*
 * This callback function used to apply a dimming PWM signal on a GPIO pin in case of error
 * Inputs:
 *		-ErrorId :	the ID of the error
 * Output:NONE
*/
void ErrorHandler_Callbackfn (uint8_t ErrorId)
{

	ErrorFlags[ErrorId] = 1;
}


/***********************************************************************************
**********					Error handler functions' bodies					********
***********************************************************************************/

/*
 * This function used to initialize the error flags
 * Inputs:NONE
 * Output:NONE
*/

void ErrorHandler_Init(void)
{
	uint8_t LoopIndex = 0;
	for (LoopIndex = 0; LoopIndex < ERROR_HANDLER_NUMBER_OF_ERROR_SOURCES; LoopIndex++)
	{
		ErrorFlags[LoopIndex] = 0;
	}
}

/*
 * This function used to apply a dimming PWM signal on a GPIO pin to indecate an error
 * Inputs:
 *		-ErrorId :	the ID of the error
 * Output:NONE
*/
void ErrorHandler_SetError(uint8_t ErrorId)
{
	ErrorFlags[ErrorId] = 1;
}

/*
 * This function used to deactivate a dimming PWM signal from a GPIO pin to clear the error indecatopn
 * Inputs:
 *		-ErrorId :	the ID of the error
 * Output:NONE
*/

void ErrorHandler_ClearError(uint8_t ErrorId)
{
	ErrorFlags[ErrorId] = 0;
}

/*
 * This function is a FSM to manage the on going operations of the ErrorHandler module
 *Inputs:NONE
 * Output:NONE
*/

void ErrorHandler_ManagOnGoingOperation(void)
{
	uint8_t LoopIndex = 0;
	for (LoopIndex = 0; LoopIndex < ERROR_HANDLER_NUMBER_OF_ERROR_SOURCES; LoopIndex++)
	{
		if(ErrorFlags[LoopIndex] == 1)
		{
			PWM_Start (ErrorHandlerConfigParam [LoopIndex]);
		}
		else
		{
			PWM_Clear (ErrorHandlerConfigParam [LoopIndex]);
		}
	}
}