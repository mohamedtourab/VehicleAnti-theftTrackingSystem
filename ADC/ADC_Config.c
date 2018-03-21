/*
 *
 *	Authors: Wessam Adel and Mohamed Mamdouh
 *	Date: 19/3/2018
 *	Microcontroller: STM32F407VG
 *
*/

#include "ADC_Config.h"

const ADC_ConfigType ADC_ConfigParam[ADC_PERIPHERAL_NUMBER]	=
{
	{
		1,	//Peripheral ID
		RESOLUTION_12,
		DISABLE_DISCONTINUOUS_MODE,
		DISABLE_SCAN_MODE,
		RIGHT_DATA_ALIGNMENT,
		SINGLE_MODE,
		PRESCALER_4
	}
};
