/*
 *
 *	Authors: Wessam Adel and Mohamed Mamdouh
 *	Date: 18/12/2017
 *	Microcontroller: STM32F407VG
 *
*/


/*
 *
 *	Authors: Mohamed Mamdouh & Wessam Adel
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
		2,
		/*
		 	*****************************************************
		 	*  Which I2C Peripheral to be used, choose from:	*
		 	*  - I2C_1: 										*
			*			SCL: PB6/PB8							*
			*			SDA: PB7/PB9							*
		 	*  - I2C_2:											*
			*			SCL: PB10								*
			*			SDA: PB11								*
		 	*  - I2C_3:											*
			*			SCL: PA8								*
			*			SDA: PC9								*
		 	*****************************************************
		*/

		I2C_1,

		/*
		 	*****************************************************
			*													*
		 	*  			Choose the SCL Line Frequencey			*
			*			Review data sheet of the slave			*
			*			to find compatible speeds				*
			*													*
			*****************************************************
		*/
		50000,

		/*
		 	*****************************************************
			*													*
		 	*  		Choose the I2C Peripheral Frequencey		*
			*		-minimum value: PERIPHERAL_CLOCK_2MHz		*
			*		-maximum value: PERIPHERAL_CLOCK_50MHz		*
			*													*
			*****************************************************
		*/

		PERIPHERAL_CLOCK_5MHz,

		/*
		 	*****************************************************
			*													*
		 	*		Enable/Disable I2C Peripheral Frequencey	*
			*		- PERIPHERAL_ENABLE							*
			*		- PERIPHERAL_DISABLE						*
			*													*
			*****************************************************
		*/

		PERIPHERAL_ENABLE,

		/*
		 	*****************************************************
			*													*
		 	*		Choose the Speed Mode						*
			*		- STANDARD_MODE								*
			*		- FAST_MODE									*
			*													*
			*****************************************************
		*/

		STANDARD_MODE,

		/*
		 	*****************************************************
			*													*
		 	*		Choose the Bus Mode							*
			*		- I2C_MODE									*
			*		- SMBUS_MODE								*
			*													*
			*****************************************************
		*/

		I2C_MODE,

		/*
		 	*****************************************************
			*													*
		 	*		Enable/Disable General Call					*
			*		(Used in Slave Mode Only)					*
			*		- DISABLE_GENERAL_CALL						*
			*		- ENABLE_GENERAL_CALL						*
			*													*
			*****************************************************
		*/

		DISABLE_GENERAL_CALL,

		/*
		 	*****************************************************
			*													*
		 	*		Transmission Done Call Back Function		*
			*													*
			*****************************************************
		*/

		I2C_TxDone,

		/*
		 	*****************************************************
			*													*
		 	*		Reception Done Call Back Function			*
			*													*
			*****************************************************
		*/

		I2C_RxDone
	}
};


