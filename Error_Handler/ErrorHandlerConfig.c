/**************************************************************************************
*   File name: ErrorHandlerConfig.c
*Auther : edited by the user
*   Date: 27/4/2018
*   Description:
*           This file contains:
*           - An array of the port ids for the gpio pins
*   Microcontroller: STM32F407VG
***************************************************************************************/ 
#include "ErrorHandler.h"
#include "ErrorHandlerConfig.h"

const uint8_t ErrorHandlerConfigParam [ERROR_HANDLER_NUMBER_OF_ERROR_SOURCES] =
{
    0,
    1,
    2
};