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

	PWM_Start(ErrorHandlerConfigParam[ErrorId]);
}


/***********************************************************************************
**********					Error handler functions' bodies					********
***********************************************************************************/
/*
 * This function used to apply a dimming PWM signal on a GPIO pin to indecate an error
 * Inputs:
 *		-ErrorId :	the ID of the error
 * Output:NONE
*/
void ErrorHandler_SetError(ErrorId)
{
	PWM_Start(ErrorHandlerConfigParam[ErrorId]);
}

/*
 * This function used to deactivate a dimming PWM signal from a GPIO pin to clear the error indecatopn
 * Inputs:
 *		-ErrorId :	the ID of the error
 * Output:NONE
*/

void ErrorHandler_ClearError(ErrorId)
{
	PWM_Clear(ErrorHandlerConfigParam[ErrorId]);
}