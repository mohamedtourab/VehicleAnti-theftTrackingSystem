/**************************************************************************************
*File name: DataHandlerConfig.c
*Auther :edited by the user
*Date: 13/6/2018
*Description:
*       This file contains:
*               - A structurs that containes the DataHandler configurations 
*Microcontroller: STM32F407VG
***************************************************************************************/ 
#include "DataHandler.h"

void Test(void);
void Rec(void);
void Error_func(uint8_t id);

const DataHandlerConfigType DataHandlerConfigParam =
{
	/*Warning message to be sent*/
	"Warning",

	/*length of warning message*/
	7,

	/*phone number*/
	"01008029273",

	"156.218.203.254",
	15,

	"999",

	3,

	/*UART Channel number*/
	"LOC",

	3,


	/*a pointer to a call back function*/
	&Test,
	&Test,

	&Test,


	/*a pointer to the error callback function*/
	&Error_func,

};