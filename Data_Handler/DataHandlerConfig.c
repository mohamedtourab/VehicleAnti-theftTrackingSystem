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
uint8_t W_MSG[8] = {'W','a','r','n','i','n','g',26};
void Test(void);
void Rec(void);
void Error_func1(uint8_t id);

void cl (void);
const DataHandlerConfigType DataHandlerConfigParam =
{
	/*Warning message to be sent*/
	W_MSG,

	/*length of warning message*/
	8,

	/*phone number*/
	"01008029273",

	"156.219.231.203",
	15,

	"350",

	3,

	/*UART Channel number*/
	"LOC",

	3,


	/*a pointer to a call back function*/
	Test,
	Test,

	cl,


	/*a pointer to the error callback function*/
	Error_func1,

};