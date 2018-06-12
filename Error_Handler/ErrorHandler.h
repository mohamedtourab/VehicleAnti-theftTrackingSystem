/**********************************************************************************	
*	File name: ErrorHandler.h
*	Auther : 
*			-Abdelrhman Hosny
*			-Amr Mohamed
*	Date : 27/4/2018
*	Discription :
*			this file contains:
*				-prototype of the APIs of the Error handler 
*	Target : STM32F407VG
**********************************************************************************/

#ifndef ERROR_HANDLER_H
#define ERROR_HANDLER_H

#include "ErrorHandlerConfig.h"
#include "PWM.h"

extern const uint8_t ErrorHandlerConfigParam [ERROR_HANDLER_NUMBER_OF_ERROR_SOURCES];

/***********************************************************************************
**********						Functions prototypes						********
***********************************************************************************/

/*
 * This callback function used to apply a dimming PWM signal on a GPIO pin in case of error
 * Inputs:
 *		-ErrorId :	the ID of the error
 * Output:NONE
*/
void ErrorHandler_Callbackfn (uint8_t ErrorId);

/*
 * This function used to apply a dimming PWM signal on a GPIO pin to indecate an error
 * Inputs:
 *		-ErrorId :	the ID of the error
 * Output:NONE
*/
void ErrorHandler_SetError(ErrorId);

/*
 * This function used to deactivate a dimming PWM signal from a GPIO pin to clear the error indecatopn
 * Inputs:
 *		-ErrorId :	the ID of the error
 * Output:NONE
*/

void ErrorHandler_ClearError(ErrorId);

#endif