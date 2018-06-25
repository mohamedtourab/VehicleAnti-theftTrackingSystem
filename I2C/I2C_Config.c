/*
 *
 *	Authors: Wessam Adel and Mohamed Mamdouh
 *	Date: 18/12/2017
 *	Microcontroller: STM32F407VG
 *
*/

#include "I2C_Driver.h"

void I2C_TxDone(void);
void I2C_RxDone(void);

const I2C_ConfigType I2C_ConfigParam[I2C_PERIPHERAL_NUMBER]=
{
	{
		1,								/*Peripheral ID*/
		50000,							/*Frequency SCL=10KHz*/
		PERIPHERAL_CLOCK_5MHz,			/*Peripheral Frequency*/
		PERIPHERAL_ENABLE,				/*enable the peripherla*/
		STANDARD_MODE,                  /*choose the standard mode*/
		I2C_MODE,                       /*choose I2C mode*/
		DISABLE_GENERAL_CALL,           /*choose to disable the general call*/
		ENABLE_ACK,                     /*choose to enable the acknowledgment*/
		I2C_TxDone,
		I2C_RxDone,
	}
};

