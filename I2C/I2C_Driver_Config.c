/*
 *
 *	Authors: Wessam Adel and Mohamed Mamdouh
 *	Date: 18/12/2017
 *	Microcontroller: STM32F407VG
 *
*/

#include "I2C_Driver.h"

const I2C_ConfigType I2C_ConfigParam[I2C_PERIPHERAL_NUMBER]=
{
	{
		1,															/*Peripheral ID*/
		100000,													/*Frequency SCL=100KHz*/
		PERIPHERAL_CLOCK_8MHz,					/*Peripheral Frequency*/
		PERIPHERAL_ENABLE,							/*enable the peripherla*/
		STANDARD_MODE,                  /*choose the standard mode*/
		I2C_MODE,                       /*choose I2C mode*/
		DISABLE_GENERAL_CALL,           /*choose to disable the general call*/
		ENABLE_ACK                      /*choose to enable the acknowledgment*/
	}
};

