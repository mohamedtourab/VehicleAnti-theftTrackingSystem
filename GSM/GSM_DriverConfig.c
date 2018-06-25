/**************************************************************************************
*File name: GSM_DriverConfig.c
*Author :edited by the user
*Date: 6/3/2018
*Description:
*	This file contains:
*		- A structure that contains the configurations to drive the M95 GSM
*Microcontroller: STM32F407VG
***************************************************************************************/ 

#include "GSM_Driver.h"
#include "GSM.h"

const GSM_DriverConfigType GSM_DriverConfigParam =
{
	/*UART Channel number*/
	1,
	/*a pointer to the client's callback function*/
	&GSM_DriverCallBackFn
};